/* Module Name: CLI_OM.C
 * Purpose: Initialize the database resources and provide some Get/Set function
 *          for accessing the system log database.
 *
 * Notes:
 *
 * History:
 *
 * Copyright(C)      Accton Corporation, 1999 - 2001
 *
 */


/* INCLUDE FILE DECLARATIONS
 */
#include <stdlib.h>
#include "sysfun.h"
#include "stktplg_om.h"
#include "sys_adpt.h"
#include "string.h"
#include "sys_type.h"
#include "sys_cpnt.h"
#include "fs_type.h"
#include "cli_def.h"
#include "fs.h"
#include "cli_mgr.h"
#include "cli_om.h"
#include "cli_pom.h"
#include "cli_lib.h"
#include "cli_io.h"
#include "sys_bld.h"
#include "sys_module.h"
/* NAMING CONSTANT DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */


/* LOCAL SUBPROGRAM DECLARATIONS
 */


/* STATIC VARIABLE DECLARATIONS
 */
static UI32_T   cli_om_semaphore_id;
static char     stacking_buf [CLI_DEF_MAX_HEADER*CLI_DEF_DISP_WIDTH];
/* move from mgr -- rich */
static BOOL_T CLI_MGR_is_reproduce_done;
static UI32_T ipcmsgq_handle = 0;
/* MACRO FUNCTIONS DECLARACTION
 */
/* EXPORTED SUBPROGRAM BODIES
 */

void	CLI_OM_InitOmSem(void)
{
    /* Semaphore for critical section, mgr and om use same sem id*/
    if(SYSFUN_GetSem(SYS_BLD_SYS_SEMAPHORE_KEY_CLI_OM, &cli_om_semaphore_id)!=SYSFUN_OK)
        SYSFUN_Debug_Printf("%s:get om sem id fail.\n", __FUNCTION__);

    if (SYSFUN_GetMsgQ(SYS_BLD_CLI_PROC_OM_IPCMSGQ_KEY,
            SYSFUN_MSGQ_BIDIRECTIONAL, &ipcmsgq_handle) != SYSFUN_OK)
    {
        SYSFUN_Debug_Printf("%s(): SYSFUN_GetMsgQ fail.\n", __FUNCTION__);
    }

}

/*-----------------------------------------------------------------------------
 * FUNCTION NAME - CLI_OM_HandleIPCReqMsg
 *-----------------------------------------------------------------------------
 * PURPOSE : Handle the ipc request message for CLI OM.
 *
 * INPUT   : msgbuf_p -- input request ipc message buffer
 *
 * OUTPUT  : msgbuf_p -- output response ipc message buffer
 *
 * RETURN  : TRUE  - there is a response required to be sent
 *           FALSE - there is no response required to be sent
 *
 * NOTES   : None.
 *-----------------------------------------------------------------------------
 */
BOOL_T CLI_OM_HandleIPCReqMsg(SYSFUN_Msg_T* msgbuf_p)
{
    CLI_OM_IPCMsg_T *msg_p;
    BOOL_T           need_resp=TRUE;

    if (msgbuf_p == NULL)
    {
        return FALSE;
    }

    msg_p = (CLI_OM_IPCMsg_T*)msgbuf_p->msg_buf;

    /* dispatch IPC message and call the corresponding VLAN_OM function
     */
    switch (msg_p->type.cmd)
    {
        case CLI_OM_IPC_GETSTACKINGDB:
            msg_p->type.ret_ui8 =
                CLI_OM_GetStackingDB(msg_p->data.CLI_stacking_db_data.stacking_db);
            msgbuf_p->msg_size = CLI_OM_GET_MSGBUFSIZE(CLI_stacking_db_data);
            break;

        case CLI_OM_IPC_GETISREPRODUCEDONE:
            msg_p->type.ret_bool =
                CLI_OM_GetIsReproduceDone();
            msgbuf_p->msg_size = CLI_OM_GET_MSGBUFSIZE(CLI_om_arg1);
            break;

        case CLI_OM_IPC_GETCONFIGUARTIONMODULETYPES:
            msg_p->type.ret_bool =
                CLI_OM_GetConfiguartionModuleTypes(msg_p->data.CLI_module_types.module_types);
            msgbuf_p->msg_size = CLI_OM_GET_MSGBUFSIZE(CLI_module_types);
            break;

        case CLI_IO_IPC_GETUARTHANDLER:
            msg_p->type.ret_bool =
                CLI_OM_GetUartHandler(&msg_p->data.CLI_uart_handler.uart_handler);
            msgbuf_p->msg_size = CLI_OM_GET_MSGBUFSIZE(CLI_uart_handler);
            break;

        default:
            SYSFUN_Debug_Printf("%s(): Invalid cmd.\n", __FUNCTION__);
            /* Unknow command. There is no way to idntify whether this
             * ipc message need or not need a response. If we response to
             * a asynchronous msg, then all following synchronous msg will
             * get wrong responses and that might not easy to debug.
             * If we do not response to a synchronous msg, the requester
             * will be blocked forever. It should be easy to debug that
             * error.
             */
            need_resp=FALSE;
    }

    return need_resp;
} /* End of VLAN_OM_HandleIPCReqMsg */



 /* FUNCTION NAME: CLI_MGR_GetStackingDB
 * PURPOSE: This API is used to get the mac and unit id and device type
 * in CLI configuration file header
 * INPUT:   None.
 * OUTPUT:  stacking_db : array of structure.
 * RETUEN:  0  -- failure
 *          otherwise -- success. the returned value is the number of entries
 * NOTES:   None.
 */

UI8_T CLI_OM_GetStackingDB(STKTPLG_OM_StackingDB_T stacking_db[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK])
{
    char   type_str[30] = {0};
    char   content_str[30] = {0};
    UI32_T len = 0;
    UI32_T i = 0, j = 0, k = 0;
    char     *tmp_xbuf;
    char     *tmp_xbuf_origin;
    char     *cmd_buf;
    UI32_T  ret_val = 0;
    UI32_T  pars_count = 0;
    UI32_T  read_count = 0;
    UI32_T  unit_id = 0;
    char    mac_addr_str[20];
    UI32_T  device_type = 0;
    char    mac_addr[SYS_ADPT_MAC_ADDR_LEN] = {0};
    char    startupfile[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1] = {0};
    FS_File_Attr_T file_attr;
    UI32_T xfer_buf_length=0;
    char   temp_num[4] = {0};
    UI32_T fs_ret  = 0;

    /*2004/5/11
     *byrant
     *use static buffer instead of cli_mgr_allocate buffer
     */

    memset(stacking_buf,0,sizeof(stacking_buf));
    tmp_xbuf_origin = stacking_buf;

    if((cmd_buf = malloc(CLI_DEF_MAX_BUFSIZE)) == NULL)
    {
        return 0;
    }
    memset(cmd_buf,0,sizeof(cmd_buf));
    tmp_xbuf = tmp_xbuf_origin;
    if(FS_RETURN_OK!=FS_GetStartupFilename(DUMMY_DRIVE, FS_FILE_TYPE_CONFIG, (UI8_T *)startupfile))
    {
        CLI_LIB_PrintStr("Failed to get start file name\r\n");
        free(cmd_buf);
        return 0;
    }
    file_attr.file_type_mask = FS_FILE_TYPE_MASK(FS_FILE_TYPE_CONFIG);
    strcpy((char *)file_attr.file_name, (char *)startupfile);
    if(FS_GetFileInfo(DUMMY_DRIVE, &file_attr) != FS_RETURN_OK)
    {
        CLI_LIB_PrintStr("Failed to get start file info\r\n");
        free(cmd_buf);
        return 0;
    }
    fs_ret = FS_ReadFile( DUMMY_DRIVE, file_attr.file_name ,  (UI8_T *)tmp_xbuf, CLI_DEF_MAX_HEADER*CLI_DEF_DISP_WIDTH, &xfer_buf_length);
    if ( fs_ret != FS_RETURN_OK && fs_ret != FS_RETURN_FILE_TRUNCATED)
    {
        CLI_LIB_PrintStr("Failed to read file info\r\n");
        free(cmd_buf);
        return 0;
    }
    #if 1//rich
    while ( read_count < CLI_DEF_MAX_HEADER )
    {
        if (CLI_MGR_ReadLine_From_XferBuf(cmd_buf, &tmp_xbuf))
        {
            read_count ++;
            len = strlen((char *)cmd_buf);
            i = 0;
            j = 0;
            k = 0;
            for (i = 1; i < len; i++ )
            {
                if (cmd_buf[i] == '>')
                {
                    break;
                }
                if (cmd_buf[i] == '<')
                {
                    continue;
                }
                else
                {
                    /*prevent buffer excreed max*/
                    if (j>=30)
                    {
                        break;
                    }
                    type_str[j] = cmd_buf[i];
                    j++;
                }
            }

            k = 0;
            j = 0;
            if (strcmp((char *)type_str, "stackingMac") == 0)
            {
                for (k = i+1; k < len; k++ )
                {
                    if (cmd_buf[k] == '<')
                    {
                        break;
                    }
                    else
                    {
                        /*prevent buffer excreed max*/
                        if (j>=30)
                        {
                            break;
                        }
                        content_str[j] = cmd_buf[k];
                        j++;
                    }
                }
                pars_count ++;


                if(content_str[0] != '0'||content_str[1] != '0')
                {
                    unit_id = 0;
                    device_type = 0;
                    memset(mac_addr,0,SYS_ADPT_MAC_ADDR_LEN);
                    memset(mac_addr_str,0,sizeof(mac_addr_str));

                    memcpy(temp_num,content_str,2);
                    temp_num[2] = '\0';
                    unit_id = atoi((char *)temp_num);

                    memcpy(temp_num,content_str+21,2);
                    temp_num[2] = '\0';
                    device_type = atoi((char *)temp_num);

                    memcpy(mac_addr_str,content_str+3,17);
                    mac_addr_str[17] = '\0';
                    CLI_LIB_AtoEa(mac_addr_str, mac_addr);

                    stacking_db[pars_count - 1].unit_id = unit_id;
                    memcpy(stacking_db[pars_count - 1].mac_addr,mac_addr,SYS_ADPT_MAC_ADDR_LEN);
                    stacking_db[pars_count -1 ].device_type = device_type;
                    ret_val ++;
                }

                if(pars_count == SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK)
                {
                    SYSFUN_Debug_Printf("\r\n[get stackingMac done ret_val] = %lu\r\n",ret_val);
                    free(cmd_buf);
                    return  ret_val;
                }
            }
        }
        else
        {
            free(cmd_buf);
            return ret_val;
        }
    }
#endif //if 0
    free(cmd_buf);
    return ret_val;
}

/* FUNCTION NAME: CLI_OM_GetIsReproduceDone
 * PURPOSE: This API is used to get the cli reproduce config file done
 * INPUT:   None.
 * OUTPUT:  None
 * RETUEN:  TRUE  -- success
 *          FALSE -- failure
 * NOTES:   None.
 */
BOOL_T CLI_OM_GetIsReproduceDone(void)
{
    return CLI_MGR_is_reproduce_done;
}

/* FUNCTION NAME: CLI_OM_SetIsReproduceDone
 * PURPOSE: This API is used to get the cli reproduce config file done
 * INPUT:   None.
 * OUTPUT:  None
 * RETUEN:  TRUE  -- success
 *          FALSE -- failure
 * NOTES:   None.
 */
BOOL_T CLI_OM_SetIsReproduceDone(BOOL_T state)
{
    UI32_T orig_priority;

    SYSFUN_OM_ENTER_CRITICAL_SECTION(cli_om_semaphore_id);
    CLI_MGR_is_reproduce_done = state;
    SYSFUN_OM_LEAVE_CRITICAL_SECTION(cli_om_semaphore_id, orig_priority);

    return TRUE;
}


/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_OM_GetConfiguartionModuleTypes
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to get module type in config file
 *
 * INPUT    : module_types
 * OUTPUT   : module_types
 * RETURN   : TRUE/FALSE
 * NOTE     :
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_OM_GetConfiguartionModuleTypes(UI32_T  module_types[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK])
{
#if 0 /* rich */
    UI8_T  type_str[30] = {0};
    UI8_T  content_str[30] = {0};
    UI32_T len = 0;
    UI32_T i = 0, j = 0, k = 0;
    UI8_T     *tmp_xbuf;
    UI8_T     *tmp_xbuf_origin;
    UI8_T     *cmd_buf;
    UI32_T  read_count = 0;
    UI8_T   startupfile[SYS_ADPT_FILE_SYSTEM_NAME_LEN+1] = {0};
    FS_File_Attr_T file_attr;
    UI32_T xfer_buf_length=0;

    /*init module_types*/

    memset(module_types, 0, sizeof(UI32_T)*SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK);

    if((tmp_xbuf_origin = (UI8_T *)CLI_MGR_AllocateBuffer()) == NULL)
    {
        return FALSE;
    }

    if((cmd_buf = (UI8_T *)L_MM_Malloc(CLI_DEF_MAX_BUFSIZE, L_MM_USER_ID2(SYS_MODULE_CLI, CLI_TYPE_TRACE_ID_CLI_MGR_GETCONFIGUARTIONMODULETYPES))) == NULL)
    {
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        return FALSE;
    }
    memset(cmd_buf,0,sizeof(cmd_buf));
    tmp_xbuf = tmp_xbuf_origin;

    if(FS_RETURN_OK!=FS_GetStartupFilename(DUMMY_DRIVE, FS_FILE_TYPE_CONFIG, startupfile))
    {
        CLI_LIB_PrintStr("Failed to get start file name\r\n");
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        L_MM_Free(cmd_buf);
        return FALSE;
    }

    file_attr.file_type_mask = FS_FILE_TYPE_MASK(FS_FILE_TYPE_CONFIG);
    strcpy(file_attr.file_name, startupfile);
    if(FS_GetFileInfo(DUMMY_DRIVE, &file_attr) != FS_RETURN_OK)
    {
        CLI_LIB_PrintStr("Failed to get start file info\r\n");
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        L_MM_Free(cmd_buf);
        return FALSE;
    }

    if (FS_ReadFile( DUMMY_DRIVE, file_attr.file_name ,  tmp_xbuf, CLI_DEF_MAX_CONFIG_SIZE, &xfer_buf_length) != FS_RETURN_OK)
    {
        CLI_LIB_PrintStr("Failed to read file info\r\n");
        CLI_MGR_FreeBuffer(tmp_xbuf_origin);
        L_MM_Free(cmd_buf);
        return FALSE;
    }

    while ( read_count < CLI_DEF_MAX_HEADER )
    {
        if (CLI_LIB_ReadLine_From_XferBuf(cmd_buf, &tmp_xbuf))
        {
            read_count ++;
            len = strlen(cmd_buf);
            i = 0;
            j = 0;
            k = 0;

            for (i = 1; i < len; i++ )
            {
                if (cmd_buf[i] == '>')
                {
                    break;
                }
                if (cmd_buf[i] == '<')
                {
                    continue;
                }
                else
                {
                    /*prevent buffer excreed max*/
                    if (j>=30)
                    {
                        break;
                    }
                    type_str[j] = cmd_buf[i];
                    j++;
                }
            }

            k = 0;
            j = 0;

            if (strcmp(type_str, "stackingDB") == 0)
            {
                for (k = i+1; k < len; k++ )
                {
                    if (cmd_buf[k] == '<')
                    {
                        break;
                    }
                    else
                    {
                        /*prevent buffer excreed max*/
                        if (j>=30)
                        {
                            break;
                        }
                        content_str[j] = cmd_buf[k];
                        j++;
                    }
                }
            }

            if (strcmp(type_str, "stackingDB") == 0)
            {
                UI32_T  unit_id = 0;
                UI8_T   module_types_str[4] = {0};

                for(unit_id = 0 ;unit_id <SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ; unit_id++)
                {
                    memcpy(module_types_str,content_str + (unit_id*2 ),2);
                    module_types[unit_id] = CLI_LIB_AtoUl(module_types_str ,10 );
                    SYSFUN_Debug_Printf("stacking DB: unit = %lu, module type = %lu\n", unit_id, module_types[unit_id]);
                }
            }
        }
        else
        {
            CLI_MGR_FreeBuffer(tmp_xbuf_origin);
            L_MM_Free(cmd_buf);
            return FALSE;
        }
    }

    CLI_MGR_FreeBuffer(tmp_xbuf_origin);
    L_MM_Free(cmd_buf);
#endif /* rich */
    return TRUE;

}

/* FUNCTION NAME: CLI_OM_GetUartHandler
 * PURPOSE: This API is used to get uart Handler
 * INPUT:   None.
 * OUTPUT:  uart_handler_p
 * RETUEN:  TRUE  -- success
 *          FALSE -- failure
 * NOTES:   None.
 */
BOOL_T  CLI_OM_GetUartHandler(UI32_T *uart_handler_p)
{
    return CLI_IO_GetUartHandler(uart_handler_p);
}

void
CLI_OM_Handle_IPC ()
{
    UI32_T sysfun_ret;
    BOOL_T need_resp = TRUE;
    const UI32_T msg_size = CLI_OM_GET_MSGBUFSIZE(CLI_stacking_db_data);
    SYSFUN_Msg_T msgbuf[SYSFUN_SIZE_OF_MSG(msg_size)];

    sysfun_ret = SYSFUN_ReceiveMsg(ipcmsgq_handle, 0, SYSFUN_TIMEOUT_NOWAIT,
                    msg_size, msgbuf);

    if(sysfun_ret == SYSFUN_OK)
    {
        switch(msgbuf->cmd)
        {
#if (SYS_CPNT_CLI == TRUE)
            case SYS_MODULE_CLI:
                need_resp=CLI_OM_HandleIPCReqMsg(msgbuf);
                break;
#endif
        }

        if((need_resp==TRUE) && (SYSFUN_SendResponseMsg(ipcmsgq_handle, msgbuf)!=SYSFUN_OK))
            SYSFUN_Debug_Printf("%s: SYSFUN_SendResponseMsg fail.\n", __FUNCTION__);

    }

}
