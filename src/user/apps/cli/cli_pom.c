/*-----------------------------------------------------------------------------
 * FILE NAME: CLI_POM.C
 *-----------------------------------------------------------------------------
 * PURPOSE:
 *    This file implements the APIs for CLI OM IPC.
 *
 * NOTES:
 *    None.
 *
 * HISTORY:
 *    2007/07/30     --- Rich, Create
 *
 * Copyright(C)      Accton Corporation, 2007
 *-----------------------------------------------------------------------------
 */

/* INCLUDE FILE DECLARATIONS
 */

#include <stdio.h>
#include <string.h>
#include "sys_bld.h"
#include "sys_module.h"
#include "sys_type.h"
#include "l_mm.h"
#include "sysfun.h"
#include "stktplg_om.h"
#include "cli_om.h"
#include "cli_pom.h"
#include "sys_adpt.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */


/* DATA TYPE DECLARATIONS
 */


/* LOCAL SUBPROGRAM DECLARATIONS
 */


/* STATIC VARIABLE DEFINITIONS
 */

static UI32_T ipcmsgq_handle;


/* EXPORTED SUBPROGRAM BODIES
 */
/*-----------------------------------------------------------------------------
 * ROUTINE NAME : CLI_POM_InitiateProcessResource
 *-----------------------------------------------------------------------------
 * PURPOSE : Initiate resource for CLI_POM in the calling process.
 *
 * INPUT   : None.
 *
 * OUTPUT  : None.
 *
 * RETURN  : TRUE  --  Sucess
 *           FALSE --  Error
 *
 * NOTES   : None.
 *-----------------------------------------------------------------------------
 */
BOOL_T CLI_POM_InitiateProcessResource(void)
{
    /* Given that CSCA is run in L2_L4_PROC
     */
    if (SYSFUN_GetMsgQ(SYS_BLD_CLI_PROC_OM_IPCMSGQ_KEY,
            SYSFUN_MSGQ_BIDIRECTIONAL, &ipcmsgq_handle) != SYSFUN_OK)
    {
        SYSFUN_Debug_Printf("%s(): SYSFUN_GetMsgQ fail.\n", __FUNCTION__);
        return FALSE;
    }

    return TRUE;
} /* End of VLAN_POM_InitiateProcessResource */

/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_POM_GetFacilityType
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This api will call CLI_OM_GetFacilityType through the IPC msgq.
 * INPUT:
 *    None
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    port
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
UI8_T CLI_POM_GetStackingDB(STKTPLG_OM_StackingDB_T stacking_db[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK])
{
    const UI32_T msg_size = CLI_OM_GET_MSGBUFSIZE(CLI_stacking_db_data);
    /* message buffer is used for both request and response
     * its size must be max(buffer for request, buffer for response)
     */
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_OM_IPCMsg_T *msg_p;
    UI8_T result;


    msgbuf_p->cmd = SYS_MODULE_CLI;
    msgbuf_p->msg_size = msg_size;

    msg_p = (CLI_OM_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = CLI_OM_IPC_GETSTACKINGDB;


    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
            SYSFUN_TIMEOUT_WAIT_FOREVER, 0/*need not to send event*/,
            msg_size, msgbuf_p) != SYSFUN_OK)
    {
        L_MM_Free(msgbuf_p);
        return FALSE;
    }

    memcpy(stacking_db, msg_p->data.CLI_stacking_db_data.stacking_db, sizeof(STKTPLG_OM_StackingDB_T)*SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK);
    result = msg_p->type.ret_ui8;
    L_MM_Free(msgbuf_p);

    return result;
} /* End of CLI_POM_GetStackingDB */

/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_POM_GetIsReproduceDone
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This api will call CLI_POM_GetIsReproduceDone through the IPC msgq.
 * INPUT:
 *    None
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    port
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T CLI_POM_GetIsReproduceDone(void)
{
    const UI32_T msg_size = CLI_OM_GET_MSGBUFSIZE(CLI_om_arg1);
    /* message buffer is used for both request and response
     * its size must be max(buffer for request, buffer for response)
     */
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_OM_IPCMsg_T *msg_p;
    UI8_T result;


    msgbuf_p->cmd = SYS_MODULE_CLI;
    msgbuf_p->msg_size = msg_size;

    msg_p = (CLI_OM_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = CLI_OM_IPC_GETISREPRODUCEDONE;


    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
            SYSFUN_TIMEOUT_WAIT_FOREVER, 0/*need not to send event*/,
            msg_size, msgbuf_p) != SYSFUN_OK)
    {
        L_MM_Free(msgbuf_p);
        return FALSE;
    }

    result = msg_p->data.CLI_om_arg1.arg1;
    L_MM_Free(msgbuf_p);

    return result;
} /* End of CLI_POM_GetStackingDB */


/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_POM_GetConfiguartionModuleTypes
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This api will call CLI_OM_GetConfiguartionModuleTypes through the IPC msgq.
 * INPUT:
 *    None
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    port
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T CLI_POM_GetConfiguartionModuleTypes(UI32_T  module_types[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK])
{
    const UI32_T msg_size = CLI_OM_GET_MSGBUFSIZE(CLI_module_types);
    /* message buffer is used for both request and response
     * its size must be max(buffer for request, buffer for response)
     */
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_OM_IPCMsg_T *msg_p;
    UI8_T result;


    msgbuf_p->cmd = SYS_MODULE_CLI;
    msgbuf_p->msg_size = msg_size;

    msg_p = (CLI_OM_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = CLI_OM_IPC_GETCONFIGUARTIONMODULETYPES;


    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
            SYSFUN_TIMEOUT_WAIT_FOREVER, 0/*need not to send event*/,
            msg_size, msgbuf_p) != SYSFUN_OK)
    {
        L_MM_Free(msgbuf_p);
        return FALSE;
    }

    memcpy(module_types, msg_p->data.CLI_module_types.module_types, sizeof(UI32_T)*SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK);
    result = msg_p->type.ret_bool;
    L_MM_Free(msgbuf_p);

    return result;
} /* End of CLI_POM_GetConfiguartionModuleTypes */

/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_POM_GetUartHandler
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This api will call CLI_OM_GetUartHandler through the IPC msgq.
 * INPUT:
 *    None
 *
 * OUTPUT:
 *    uart_handler_p.
 *
 * RETURN:
 *    TRUE/FALSE
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T CLI_POM_GetUartHandler(UI32_T *uart_handler_p)
{
    const UI32_T msg_size = CLI_OM_GET_MSGBUFSIZE(CLI_uart_handler);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_OM_IPCMsg_T *msg_p;

    msgbuf_p->cmd = SYS_MODULE_CLI;
    msgbuf_p->msg_size = msg_size;

    msg_p = (CLI_OM_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = CLI_IO_IPC_GETUARTHANDLER;


    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
            SYSFUN_TIMEOUT_WAIT_FOREVER, 0,
            msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return FALSE;
    }

    *uart_handler_p = msg_p->data.CLI_uart_handler.uart_handler;

    return msg_p->type.ret_bool;
} /* End of CLI_POM_GetUartHandler */

