#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <ctype.h>

#include "sys_type.h"
#include "sys_adpt.h"
#include "sysfun.h"
#include "sys_pmgr.h"
#include "l_inet.h"
#include "l_ipcio.h"
#include "l_inet.h"

#include "cli_api.h"
#include "cli_def.h"
#include "cli_io.h"
#include "cli_task.h"
#include "cli_lib.h"
#include "cli_mgr.h"
#include "nudefs.h"
#include "cli_api_telnet.h"
#include "telnetd.h"
#include "netcfg_pom_ip.h"
#include "netcfg_type.h"
#include "buffer_mgr.h"
#include "dhcp_type.h"
#include "dhcp_pmgr.h"

static UI8_T CLI_IO_ReadByte();
static UI32_T total_counter_ports = SYS_ADPT_TOTAL_NBR_OF_LPORT;
static UI32_T uart_handler = SYSFUN_RESULT_ERROR;

static UI8_T l_ipcio_msg_buf[SYSFUN_SIZE_OF_MSG(sizeof(L_IPCIO_Msg_T))];
static UI16_T l_ipcio_wait_size = 0;

static UI16_T CLI_IO_GetKeyByVtTerminal(char *ch);

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
static UI16_T CLI_IO_GetKeyByAnsiTerminal(char *ch);
#endif  /* #if (SYS_CPNT_CLI_TERMINAL == TRUE) */

#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE)
static UI32_T count_time = 0;              /* count time to get/call DHCP */
static BOOL_T is_first_copy = TRUE;        /* check if first time to copy tftp file */
static BOOL_T CLI_IO_ProcessDynamicProvision(CLI_TASK_WorkingArea_T *ctrl_P);
#endif

BOOL_T CLI_IPCIO_HandleIPCReqMsg(SYSFUN_Msg_T *msg_p, UI32_T *need_wait_key_p)
{
    L_IPCIO_Msg_T *l_ipcio_msg_p = (L_IPCIO_Msg_T *)msg_p->msg_buf;
    BOOL_T need_resp = FALSE;

    if (msg_p == NULL)
        return FALSE;

    switch (l_ipcio_msg_p->ipcio_cmd)
    {
        case L_IPCIO_CMD_GETCHAR:
            if (*need_wait_key_p==1)
            {
                msg_p->msg_size = L_IPCIO_SIZE_OF_MSG(sizeof(l_ipcio_msg_p->ipcio_arg_ch));
                l_ipcio_msg_p->ipcio_arg_ch = '\0';
                need_resp = TRUE;
            }
            else
            {
                memcpy(l_ipcio_msg_buf, msg_p, SYSFUN_SIZE_OF_MSG(L_IPCIO_SIZE_OF_MSG(0)));
                *need_wait_key_p = 1;
            }
            break;

        case L_IPCIO_CMD_GETLINE:
            if (*need_wait_key_p ==2)
            {
                msg_p->msg_size = L_IPCIO_SIZE_OF_MSG(1);
                l_ipcio_msg_p->ipcio_arg_buffer[0] = '\0';
                need_resp = TRUE;
            }
            else
            {
                memcpy(l_ipcio_msg_buf, msg_p, SYSFUN_SIZE_OF_MSG(L_IPCIO_SIZE_OF_MSG(0)));
                l_ipcio_wait_size = (l_ipcio_msg_p->ipcio_arg_size < sizeof(l_ipcio_msg_p->ipcio_arg_buffer) ?
                                        l_ipcio_msg_p->ipcio_arg_size : sizeof(l_ipcio_msg_p->ipcio_arg_buffer)) - 1;
                *need_wait_key_p = 2;
            }
            break;

        case L_IPCIO_CMD_PRINT:
            printf(l_ipcio_msg_p->ipcio_arg_buffer);
            fflush(stdout);
            break;

        default:
            SYSFUN_Debug_Printf("%s: invalid command.\n", __FUNCTION__);
    }

    return need_resp;
}

void CLI_IPCIO_ParseKey(UI32_T msgq_handle, char ch, UI32_T *need_wait_key_p, char *buff)
{

    SYSFUN_Msg_T *msg_p = (SYSFUN_Msg_T *)l_ipcio_msg_buf;
    L_IPCIO_Msg_T *l_ipcio_msg_p = (L_IPCIO_Msg_T *)msg_p->msg_buf;

    if (need_wait_key_p == 0 || *need_wait_key_p == FALSE)
        return;

    switch (l_ipcio_msg_p->ipcio_cmd)
    {
        case L_IPCIO_CMD_GETCHAR:
            l_ipcio_msg_p->ipcio_arg_ch = (char)ch;
            msg_p->msg_size = L_IPCIO_SIZE_OF_MSG(sizeof(l_ipcio_msg_p->ipcio_arg_ch));
            SYSFUN_SendResponseMsg(msgq_handle, msg_p);
            *need_wait_key_p = 0;
            break;

        case L_IPCIO_CMD_GETLINE:
            if(buff == NULL)
             {
                 SYSFUN_Debug_Printf("\n %s %d", __FUNCTION__, __LINE__);
                 return;
             }
           //  printf("buff is %s, len is %lu \n", buff, strlen(buff));
             strcpy( l_ipcio_msg_p->ipcio_arg_buffer, buff);

             msg_p->msg_size = L_IPCIO_SIZE_OF_MSG(strlen(buff) + 1);
            // printf("\n msg_p->msg_size is %lu\n", msg_p->msg_size);
             SYSFUN_SendResponseMsg(msgq_handle, msg_p);
             *need_wait_key_p = 0;
            break;
    }
}

/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_OpenUart                           |
 *-----------------------------------------------------------|
 * PURPOSE : Open uart .             |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  :None.                                             |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
BOOL_T CLI_IO_OpenUart(void)
{

    if (uart_handler == SYSFUN_RESULT_ERROR)
    {
        uart_handler = SYSFUN_OpenUART(SYSFUN_UART_CHANNEL1);
    }

    return TRUE;
}


/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_GetUartHandler                           |
 *-----------------------------------------------------------|
 * PURPOSE : Get uart Handler.             |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  :None.                                             |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
BOOL_T CLI_IO_GetUartHandler(UI32_T *uart_handler_p)
{
    *uart_handler_p = uart_handler;

    return TRUE;
}

/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_GetKey                              |
 *-----------------------------------------------------------|
 * PURPOSE : Get byte or escape sequence                     |
 * INPUT   : None.                                           |
 * OUTPUT  : if the return value is PRNTABLE, the *ch is     |
 *           meaning.                                        |
 * RETURN  : Key code definition                             |
 * NOTE    : Escape sequence define                          |
 *              up    : 0x1B 0x5B 0x41                       |
 *              down  : 0x1B 0x5B 0x42                       |
 *              right : 0x1B 0x5B 0x43                       |
 *              left  : 0x1B 0x5B 0x44                       |
 *           ASCII control set                               |
 *              BEL: 0x07                                    |
 *              BS : 0x08                                    |
 *              TAB: 0x09                                    |
 *              ..........                                   |
 *-----------------------------------------------------------*/
UI16_T CLI_IO_GetKey(char *ch)
{
#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();
#endif  /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    char  c;

    /* Read content in */
    if((c = CLI_IO_ReadByte()) == 0)
       return KEYIN_ERROR;

    *ch = c;

#if (SYS_CPNT_CLI_TERMINAL == TRUE)

    switch(ctrl_P->CMenu.terminal_type)
    {
        case ANSI_BBS:
            return CLI_IO_GetKeyByAnsiTerminal(ch);

        case VT100:
        case VT102:
        default:
            return CLI_IO_GetKeyByVtTerminal(ch);
    }  /*end of switch*/

#else

    return CLI_IO_GetKeyByVtTerminal(ch);

#endif  /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

}

/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_ReadACharNoWait                     |
 *-----------------------------------------------------------|
 * PURPOSE : Read a char from console or telnet without      |
 *           blocking.                                       |
 *           Wrap function for CLI_IO_ReadACharFromConsole   |
 *           CLI_IO_ReadACharFromTelnet.                     |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  : A byte.   - Read a byte success, or 0 for fail  |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
UI8_T CLI_IO_ReadACharNoWait()
{
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();

    if( ctrl_P == NULL )
    {
        SYSFUN_Debug_Printf("\r\n%sFailed to get working area\r\n", __FUNCTION__);
        return 0;
    }

    if (ctrl_P->sess_type == CLI_TYPE_UART)
		return (CLI_IO_ReadACharFromConsole(ctrl_P));
	else
		return (CLI_IO_ReadACharFromTelnet(ctrl_P));

}

static UI8_T CLI_IO_ReadFromConsole(CLI_TASK_WorkingArea_T *ctrl_P);
static UI8_T CLI_IO_ReadFromTelnet(CLI_TASK_WorkingArea_T *ctrl_P);

/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_ReadByte                           |
 *-----------------------------------------------------------|
 * PURPOSE : Read a byte from console or telnet.             |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  : A byte.   - Read a byte success, or 0 for fail  |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
static UI8_T CLI_IO_ReadByte()
{
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();

    if( ctrl_P == NULL )
    {
        SYSFUN_Debug_Printf("\r\n%sFailed to get working area\r\n", __FUNCTION__);
        return 0;
    }

    if (ctrl_P->sess_type == CLI_TYPE_UART)
		return (CLI_IO_ReadFromConsole(ctrl_P));
	else
		return (CLI_IO_ReadFromTelnet(ctrl_P));
}


/*-----------------------------------------------------------|
 * ROUTINE NAME - cli_ReadFromConsole                        |
 *-----------------------------------------------------------|
 * PURPOSE : Read a byte from console.                       |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  : A byte.   - Read a byte success, or 0 for fail  |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
static UI8_T CLI_IO_ReadFromConsole(CLI_TASK_WorkingArea_T *ctrl_P)
{
    char c;
    int    ch;
    UI32_T idx;
    UI32_T start_ticks = SYSFUN_GetSysTick();

#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE)
    BOOL_T  is_provision_factory_default = FALSE;
    UI32_T  address_mode;

    CLI_MGR_Get_Startup_FactoryDefault_Flag(&is_provision_factory_default);

#endif

    assert(ctrl_P != NULL);
    ctrl_P->CMenu.IdleStartTick = start_ticks;

    while(1)
    {
#if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE)
        CLI_LIB_SwWatchDogRoutine(ctrl_P);
#endif

#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE)
        if ( (is_first_copy == TRUE) && (is_provision_factory_default == TRUE) )
        {
            count_time++ ;

            /* CLI task every 5 tick excute this function (SYSFUN_Sleep(5)),
             * Do DHCP provision checking every 100 loops. It is about 5 seconds here.
             */
            if( (count_time%100) ==0)
            {

                NETCFG_POM_IP_GetIpAddressMode(SYS_ADPT_VLAN_1_IF_INDEX_NUMBER, &address_mode);

                if (address_mode == NETCFG_TYPE_IP_ADDRESS_MODE_DHCP)
                {
                    /* not perform dynamic provision yet
                     */
                    if(FALSE == CLI_IO_ProcessDynamicProvision(ctrl_P))
                    {
                        /* CLI task every 5 tick excute this function (SYSFUN_Sleep(5)),
                         * Do DHCP provision checking every 1200 loops. It is about 1 minute here.
                         */
                        if(count_time == 1200)
                        {
                            /* retry to get DHCP IP address
                             */
                            DHCP_PMGR_Restart3(DHCP_TYPE_RESTART_CLIENT);
                            count_time = 0;
                        }
                    }
                }  /* NETCFG_TYPE_IP_ADDRESS_MODE_DHCP */
            }   /* end of counter */
        }   /* end of is_first_copy */
#endif  /* #if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE) */


#if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE)
      CLI_LIB_ShowReloadNotify(ctrl_P);
#endif  /* SYS_CPNT_SYSMGMT_DEFERRED_RELOAD */

      /* Check event arriving when in telnet client session
      	- JinneeJin, January 08, 2008 5:07:51 */
	  if (telnet_client_event_check(ctrl_P->telnet_client.socket))
	  	return 0; /* Telent connection close, return normal cli state */

      ch =  SYSFUN_UARTPollRxBuff(uart_handler, 1, &c);

        /* here we must wait for mgr thread */
        CLI_MAIN_HandleMGR_Thread(SYSFUN_TIMEOUT_NOWAIT, ch, NULL);

/*92.03.27 pttch add for disconnect session*/
#if ( CLI_SUPPORT_DISCONNECT_ALL_SESSION == 1 )/*isiah*/
      if ( ctrl_P->kill_itself == TRUE )
      {

        CLI_LIB_PrintStr("\r\nBroken by Administrator.\r\n");
        for(idx = 0; idx< total_counter_ports; idx ++)
        {
            if(ctrl_P->CMenu.counters_table[idx] != NULL)
            {
                free(ctrl_P->CMenu.counters_table[idx]);
            }
        }

        /* not to reset kill_itself here because this information is useful later.
         */
        return 0;
      }
#endif
      if(ch == 0)
      {
         UI32_T current_ticks = SYSFUN_GetSysTick();
         SYS_MGR_Console_T consoleCfg;

         memset(&consoleCfg, 0, sizeof(SYS_MGR_Console_T));

         SYS_PMGR_GetConsoleCfg(&consoleCfg);


         /*
           06/16/2004 10:28AM
           pttch port from ES3526V-17-00027(EPR00009859) for support split runtime and login timeout
         */

         switch(ctrl_P->CMenu.AuthenState)
         {
            case CLI_DEF_AUTHEN_STATE_READY:
            if(consoleCfg.login_timeout != 0 &&     /*0: disable*/
            current_ticks - start_ticks >= consoleCfg.login_timeout*100)
            {
#if (SYS_CPNT_EH == TRUE)
               CLI_LIB_PrintNullStr(2);
               CLI_API_SetandShow_Exception_Handeler_Msg(EH_TYPE_MSG_TIMEOUT, CLI_IO_READFROMCONSOLE, "Authentication");
               CLI_LIB_PrintNullStr(1);
#else
               CLI_LIB_PrintStr("\r\n\r\nTimeout.\r\n\r\n");
#endif
               for(idx = 0; idx< total_counter_ports; idx ++)
                  if(ctrl_P->CMenu.counters_table[idx] != NULL)
                     free(ctrl_P->CMenu.counters_table[idx]);
               return 0;
            }
            break;

            case CLI_DEF_AUTHEN_STATE_PASS:
            if(consoleCfg.inactive_timeout != 0 &&     /*0: disable*/
            current_ticks - start_ticks >= consoleCfg.inactive_timeout*100)
            {
#if (SYS_CPNT_EH == TRUE)
               CLI_LIB_PrintNullStr(2);
               CLI_API_SetandShow_Exception_Handeler_Msg(EH_TYPE_MSG_TIMEOUT, CLI_IO_READFROMCONSOLE, "Connect session");
               CLI_LIB_PrintNullStr(1);
#else
               /*jingyan zheng mark this line to fix defect ES4827G-FLF-ZZ-00231*/
//               CLI_LIB_PrintStr("\r\n\r\nSession closed by automatic logout.\r\n");
#endif

#if (SYS_CPNT_CLI_TERMINAL == TRUE) || (SYS_CPNT_CLI_FILTERING == TRUE)
               CLI_LIB_SetFilteringTerminalDefault(ctrl_P);
#endif/*#if (SYS_CPNT_CLI_TERMINAL == TRUE) || (SYS_CPNT_CLI_FILTERING == TRUE)*/

               for(idx = 0; idx< total_counter_ports; idx ++)
                  if(ctrl_P->CMenu.counters_table[idx] != NULL)
                     free(ctrl_P->CMenu.counters_table[idx]);
               {
                   char  temp[30] = {0};
                   UI8_T ip[16] = {0};
                  SYSLOG_OM_RecordOwnerInfo_T       owner_info;

                   owner_info.level = SYSLOG_LEVEL_INFO;
                   owner_info.module_no = SYSLOG_MODULE_APP_CLI;

                   owner_info.function_no = 0;
                   owner_info.error_no = 1;

                   /*if (ctrl_P->CMenu.RemoteTelnetClientIpAddr == 0)*/
                   if (ctrl_P->CMenu.RemoteSessionType == CLI_TYPE_UART)
                   {
                      sprintf(temp, "Console user");
                   }
                   else
                   {
                      if (ctrl_P->CMenu.RemoteSessionType == CLI_TYPE_SSH)
                      {
                         L_INET_InaddrToString((L_INET_Addr_T *)&ctrl_P->CMenu.TelnetIPAddress, L_INET_MAX_IPADDR_STR_LEN, ip);
                         sprintf(temp, "%s SSH user",ip);
                      }
                      else
                      {
                         L_INET_InaddrToString((L_INET_Addr_T *)&ctrl_P->CMenu.TelnetIPAddress, L_INET_MAX_IPADDR_STR_LEN, ip);
                         sprintf(temp, "%s VTY user",ip);
                      }
                   }
                   SYSLOG_PMGR_AddFormatMsgEntry(&owner_info, LOGOUT_PRIVILEG_MODE_MESSAGE_INDEX, temp, ctrl_P->CMenu.UserName, 0);
               }
               return 0;
            }
            break;

            case CLI_DEF_AUTHEN_STATE_NOT_READY:
            default:
               break;

         }
      }
      else
      {
      	 if(c != 0x00)
      	 {
            return (UI8_T)c;
         }
      }

      if (CLI_MGR_Opmode_Get_Operation_Mode() == CLI_MGR_OPMODE_TRANSITION_MODE)
      {
         CLI_LIB_PrintStr("\nCLI entering transition mode\r\n");
         for(idx = 0; idx< total_counter_ports; idx ++)
            if(ctrl_P->CMenu.counters_table[idx] != NULL)
               free(ctrl_P->CMenu.counters_table[idx]);
         CLI_MGR_Set_Realize_EnterTransitionMode(TRUE);
         return 0;
      }
      SYSFUN_Sleep(5);
   }

   for(idx = 0; idx< total_counter_ports; idx ++)
      if(ctrl_P->CMenu.counters_table[idx] != NULL)
         free(ctrl_P->CMenu.counters_table[idx]);
   return 0;
}

/*-----------------------------------------------------------|
 * ROUTINE NAME - cli_ReadFromTelnet                         |
 *-----------------------------------------------------------|
 * PURPOSE : Read a byte from telnet.                        |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  : A byte.   - Read a byte success, or 0 for fail  |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
static UI8_T CLI_IO_ReadFromTelnet(CLI_TASK_WorkingArea_T *ctrl_P)
{
   UI8_T  ch;
   UI32_T c = 0;
   int    rc;
   fd_set ibits;
   UI32_T start_ticks = SYSFUN_GetSysTick();
   struct timeval timeout;
   UI32_T idx;

   timeout.tv_sec  = 1;       /*  no. of seconds to block */
   timeout.tv_usec = 0;       /*  no. of micro seconds to block */

   assert(ctrl_P != NULL);
   ctrl_P->CMenu.IdleStartTick = start_ticks;

   while (1)
   {
#if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE)
      CLI_LIB_ShowReloadNotify(ctrl_P);
#endif  /* SYS_CPNT_SYSMGMT_DEFERRED_RELOAD */

      /* Check event arriving when in telnet client session
      	- JinneeJin, January 08, 2008 5:07:51 */
      if (telnet_client_event_check(ctrl_P->telnet_client.socket))
         return 0; /* Telent connection close, return normal cli state */

#if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE)
      CLI_LIB_SwWatchDogRoutine(ctrl_P);
#endif
      FD_ZERO(&ibits);
      FD_SET(ctrl_P->socket, &ibits);


      timeout.tv_sec  = 1;       /*  no. of seconds to block */
      timeout.tv_usec = 0;       /*  no. of micro seconds to block */

      rc = select(ctrl_P->socket+1, (fd_set *)&ibits, NULL, NULL, &timeout);
#if ( SYS_CPNT_SSHD == TRUE || CLI_SUPPORT_DISCONNECT_ALL_SESSION == 1 || SYS_CPNT_SSH2 == TRUE)/*isiah*/
      if ( ctrl_P->kill_itself == TRUE )
      {

        CLI_LIB_PrintStr("\r\nBroken by Administrator.\r\n");
        for(idx = 0; idx< total_counter_ports; idx ++)
        {
            if(ctrl_P->CMenu.counters_table[idx] != NULL)
            {
                free(ctrl_P->CMenu.counters_table[idx]);
            }
        }
        /*reset ssh kill self flag*/
        /*move to CLI_TASK_FreeWorkingSpace*/
        #if 0
        ctrl_P->kill_itself = FALSE;
        #endif
        return 0;
      }
#endif

      if (rc < 0)        /*error; maybe user terminate the program*/
      {
         SYSFUN_Sleep(200);

          for(idx = 0; idx< total_counter_ports; idx ++)
             if(ctrl_P->CMenu.counters_table[idx] != NULL)
                free(ctrl_P->CMenu.counters_table[idx]);
         return 0;
      }
      else if (rc == 0)  /*no key in*/
      {
         UI32_T current_ticks;
         SYS_MGR_Telnet_T telnetCfg;

         memset(&telnetCfg, 0, sizeof(SYS_MGR_Telnet_T));
         SYS_PMGR_GetTelnetCfg(&telnetCfg);

#if (SYS_CPNT_CLUSTER == TRUE)
         /* To keep command alive,we need to stop counting while relaying
          */
         if(ctrl_P->CMenu.IsTelnetRelaying == TRUE)
         {
              start_ticks = SYSFUN_GetSysTick();
			  ctrl_P->CMenu.IdleStartTick = start_ticks;
         }
#endif
         current_ticks = SYSFUN_GetSysTick();
         /*
           06/16/2004 10:28AM
           pttch port from ES3526V-17-00027(EPR00009859) for support split runtime and login timeout
         */

         switch(ctrl_P->CMenu.AuthenState)
         {
            case CLI_DEF_AUTHEN_STATE_READY:
            if(telnetCfg.login_timeout != 0   &&   /*0: disable*/
            current_ticks - start_ticks >= telnetCfg.login_timeout*100)
            {
#if (SYS_CPNT_EH == TRUE)
               CLI_LIB_PrintNullStr(2);
               CLI_API_SetandShow_Exception_Handeler_Msg(EH_TYPE_MSG_TIMEOUT, CLI_IO_READFROMCONSOLE, "Authentication");
               CLI_LIB_PrintNullStr(1);
#else
               CLI_LIB_PrintStr("\r\n\r\nTimeout.\r\n\r\n");
#endif
               for(idx = 0; idx< total_counter_ports; idx ++)
                  if(ctrl_P->CMenu.counters_table[idx] != NULL)
                     free(ctrl_P->CMenu.counters_table[idx]);
               return 0;
            }
            break;

            case CLI_DEF_AUTHEN_STATE_PASS:
            if(telnetCfg.inactive_timeout != 0   &&   /*0: disable*/
            current_ticks - start_ticks >= telnetCfg.inactive_timeout*100)
            {

#if (SYS_CPNT_EH == TRUE)
               CLI_LIB_PrintNullStr(2);
               CLI_API_SetandShow_Exception_Handeler_Msg(EH_TYPE_MSG_TIMEOUT, CLI_IO_READFROMCONSOLE, "Connect session");
               CLI_LIB_PrintNullStr(1);
#else
/*jingyan zheng mark this line to fix defect ES4827G-FLF-ZZ-00231*/
//               CLI_LIB_PrintStr("\r\n\r\nSession closed by automatic logout.\r\n");
#endif
               for(idx = 0; idx< total_counter_ports; idx ++)
                  if(ctrl_P->CMenu.counters_table[idx] != NULL)
                     free(ctrl_P->CMenu.counters_table[idx]);
               {
                   char  temp[L_INET_MAX_IPADDR_STR_LEN + 20] = {0};
                   UI8_T ip[L_INET_MAX_IPADDR_STR_LEN + 1] = {0};

                   SYSLOG_OM_RecordOwnerInfo_T       owner_info;
                   owner_info.level = SYSLOG_LEVEL_INFO;
                   owner_info.module_no = SYSLOG_MODULE_APP_CLI;

                   owner_info.function_no = 0;
                   owner_info.error_no = 1;

                   /*if (ctrl_P->CMenu.RemoteTelnetClientIpAddr == 0)*/
                   if (ctrl_P->CMenu.RemoteSessionType == CLI_TYPE_UART)
                   {
                      sprintf(temp, "Console user");
                   }
                   else
                   {
                      if (ctrl_P->CMenu.RemoteSessionType == CLI_TYPE_SSH)
                      {
                         /*L_INET_Ntoa(ctrl_P->CMenu.RemoteTelnetClientIpAddr, ip);*/
                         L_INET_InaddrToString((L_INET_Addr_T *)&ctrl_P->CMenu.TelnetIPAddress, L_INET_MAX_IPADDR_STR_LEN, ip);
                         sprintf(temp, "%s SSH user",ip);
                      }
                      else
                      {
                         /*L_INET_Ntoa(ctrl_P->CMenu.RemoteTelnetClientIpAddr, ip);*/
                         L_INET_InaddrToString((L_INET_Addr_T *)&ctrl_P->CMenu.TelnetIPAddress, L_INET_MAX_IPADDR_STR_LEN, ip);
                         sprintf(temp, "%s VTY user",ip);
                      }
                   }
                   SYSLOG_PMGR_AddFormatMsgEntry(&owner_info, LOGOUT_PRIVILEG_MODE_MESSAGE_INDEX, temp, ctrl_P->CMenu.UserName, 0);
               }
               return 0;
            }
            break;

            case CLI_DEF_AUTHEN_STATE_NOT_READY:
            default:
               break;



         }
      }
      else                /*key in*/
      {
         if (FD_ISSET(ctrl_P->socket, &ibits))   /* any input? */
         {
            c = recv(ctrl_P->socket, (UI8_T *)&ch, 1, 0); /*get input*/
         }

         if (c <= 0) /*error; maybe user terminate the program*/ /*end of file*/
         {
            SYSFUN_Sleep(200);

            for(idx = 0; idx< total_counter_ports; idx ++)
               if(ctrl_P->CMenu.counters_table[idx] != NULL)
                  free(ctrl_P->CMenu.counters_table[idx]);
            return 0;
         }
         else if ((ch != 0xff) && (ch != 0))  /*  read message success  */
         {
            return ch;
         }
      }
   }  /*while*/
}

/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_PrintOut                            |
 *-----------------------------------------------------------|
 * PURPOSE : Print out the string in the send_buf in working |
 *           space.                                          |
 * INPUT   : ctrl_P - A pointer of the CLI task working area |
 * OUTPUT  : None.                                           |
 * RETURN  : None.                                           |
 * NOTE    : This funuction just handle the way to print out,|
 *           the other service rutines are in CLI_LIB, and   |
 *           this function only service the function in the  |
 *           CLI_LIB.                                        |
 *-----------------------------------------------------------*/
void CLI_IO_PrintOut(CLI_TASK_WorkingArea_T *ctrl_P)
{
   BOOL_T is_print;
   int    len;

    assert(ctrl_P != NULL);
   if((len = strlen((char *)ctrl_P->send_buf) ) == 0)
      return;

   switch(ctrl_P->sess_type)
   {
   case CLI_TYPE_UART:
      /* to show '%' in console */
      printf("%s",(char *)ctrl_P->send_buf);
      fflush(stdout);
      //write(1, (char *)ctrl_P->send_buf, len);
      break;

   case CLI_TYPE_TELNET:
   {
        UI32_T retries;

        /* Retry 5 times if send fails */
        for (retries = 0; retries < 5; retries++)
        {
            if (len == send(ctrl_P->socket, (char *)ctrl_P->send_buf, (I32_T)len, 0))
            {
      break;
             }
            SYSFUN_Sleep(10);
        }

      break;
    }

   case CLI_TYPE_PROVISION:
   case CLI_TYPE_MODULE_PROVISION:
#if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE)
   case CLI_TYPE_PARTIAL_PROV:
#endif  /* #if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE) */
      CLI_MGR_Get_PrintProvisionMsg(&is_print);
      if(is_print)
      {
         if(ctrl_P->socket == 0)                /* provision from console */
         {
            printf((char *)ctrl_P->send_buf);
            fflush(stdout);
         }
         else                                      /* provision from telnet */
            send(ctrl_P->socket, (char *)ctrl_P->send_buf, (I32_T)len, 0);
      }
      else
      {
         if(ctrl_P->socket == 0)                /* provision from console */
         {
            printf(".");
            fflush(stdout);
         }
         else                                      /* provision from telnet */
            send(ctrl_P->socket, ".", 1, 0);
      }
      break;

   default:
      ;
   }
   ctrl_P->send_buf[0] = '\0';
}

/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_ReadACharFromConsole                |
 *-----------------------------------------------------------|
 * PURPOSE : Replace by CLI_IO_ReadACharNoWait               |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  : A byte.   - Read a byte success, or 0 for fail  |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
UI8_T CLI_IO_ReadACharFromTelnet(CLI_TASK_WorkingArea_T *ctrl_P)
{
   fd_set ibits;

   struct timeval timeout;
   UI8_T  ch;

    assert(ctrl_P != NULL);
   timeout.tv_sec  = 1;       //  no. of seconds to block
   timeout.tv_usec = 0;       //  no. of micro seconds to block

   FD_ZERO(&ibits);
   FD_SET(ctrl_P->socket, &ibits);

   if( select(ctrl_P->socket+1, (fd_set *)&ibits, NULL, NULL, &timeout) > 0 &&
       FD_ISSET(ctrl_P->socket, &ibits)                                     &&
       recv(ctrl_P->socket, (UI8_T *)&ch, 1, 0) > 0)
   {
      return ch;
   }
   else
   {
      return 0;
   }
}

/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_ReadACharFromConsole                |
 *-----------------------------------------------------------|
 * PURPOSE : Replace by CLI_IO_ReadACharNoWait               |
 * INPUT   : None.                                           |
 * OUTPUT  : None.                                           |
 * RETURN  : A byte.   - Read a byte success, or 0 for fail  |
 * NOTE    : None.                                           |
 *-----------------------------------------------------------*/
UI8_T CLI_IO_ReadACharFromConsole(CLI_TASK_WorkingArea_T *ctrl_P)
{
    char c;
    if (SYSFUN_UARTPollRxBuff(uart_handler, 1, &c) < 1)
        return 0;
    return (UI8_T)c;
}

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
/*-----------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_GetKeyByAnsiTerminal                |
 *-----------------------------------------------------------|
 * PURPOSE : Get byte or escape sequence by ANSI BBS terminal|
 * INPUT   : char c                                          |
 * OUTPUT  : none                                            |
 * RETURN  : Key code definition                             |
 * NOTE    : none                                            |
 *-----------------------------------------------------------*/
static UI16_T CLI_IO_GetKeyByAnsiTerminal(char *ch)
{
    char    c;
    UI16_T  ret_val;

    /* Parsing contents
     */
    if (*ch == 0x1B)
    {
        if((c = CLI_IO_ReadByte()) == 0)
        {
            return KEYIN_ERROR;
        }

        *ch = c;

        if ((c == 0x42) || (c == 0x62))
            ret_val = ESC_B;
        else if ((c == 0x43) || (c == 0x63))
            ret_val = ESC_C;
        else if ((c == 0x44) || (c == 0x64))
            ret_val = ESC_D;
        else if ((c == 0x46) || (c == 0x66))
            ret_val = ESC_F;
        else if ((c == 0x4C) || (c == 0x6C))
            ret_val = ESC_L;
        else if ((c == 0x51) || (c == 0x71))
            ret_val = ESC_Q;
        else if ((c == 0x55) || (c == 0x75))
            ret_val = ESC_U;
        else if ((c == 0x59) || (c == 0x79))
            ret_val = ESC_Y;
        else if (c == 0x5B)
        {
           if((c = CLI_IO_ReadByte()) == 0)
              return KEYIN_ERROR;

            switch (c)
            {
                case 0x41:
                   ret_val = UP;
                   break;

                case 0x42:
                   ret_val = DOWN;
                   break;

                case 0x43:
                   ret_val = RIGHT;
                   break;

                case 0x44:
                   ret_val = LEFT;
                   break;

                default:
                   ret_val = UNKNOWN;
                   break;
            }
        }
        else
        {
            ret_val = UNKNOWN;
        }
    }
    else if (*ch == 0xD)
    {
        ret_val = ENTER;
    }
    else if (*ch == 0xA)
    {
        ret_val = ENTER;
    }
    else if (*ch == 0x1)
    {
        ret_val = CTRL_A;
    }
    else if (*ch == 0x2)
    {
        ret_val = CTRL_B;
    }
    else if (*ch == 0x3)
    {
        ret_val = CTRL_C;
    }
    else if (*ch == 0x4)
    {
        ret_val = CTRL_D;
    }
    else if (*ch == 0x5)
    {
        ret_val = CTRL_E;
    }
    else if (*ch == 0x6)
    {
        ret_val = CTRL_F;
    }
    else if (*ch == 0x7)
    {
        ret_val = CTRL_G; /*Debug mode*/
    }
    else if (*ch == 0x8)
    {
        ret_val = BS;
    }
    else if (*ch == 0x09)
    {
        ret_val = TAB;
    }
    else if (*ch == 0xB)
    {
        ret_val = CTRL_K;
    }
    else if (*ch == 0xC)
    {
        ret_val = CTRL_L;
    }
    else if (*ch == 0xE)
    {
        ret_val = CTRL_N;
    }
    else if (*ch == 0x10)
    {
        ret_val = CTRL_P;
    }
    else if (*ch == 0x12)
    {
        ret_val = CTRL_R;
    }
    else if (*ch == 0x14)
    {
        ret_val = CTRL_T;
    }
    else if (*ch == 0x15)
    {
        ret_val = CTRL_U;
    }
    else if (*ch == 0x16)
    {
        ret_val = CTRL_V;
    }
    else if (*ch == 0x17)
    {
        ret_val = CTRL_W;
    }
    else if (*ch == 0x18)
    {
        ret_val = CTRL_X;
    }
    else if (*ch == 0x19)
    {
        ret_val = CTRL_Y;
    }
    else if (*ch == 0x1A)
    {
        ret_val = CTRL_Z;
    }
    else if ((*ch >= 0x20) && (*ch <= 0x7E))    /* printables */
    {
        if (*ch == 0x3f)
            ret_val = QUMARK;
        else
            ret_val = PRNTABLE;
    }
    else if (*ch == 0x7F)
    {
        /* modified by chuanny, 1999-01-21
         * Rev 0.01.19(2)
         * It is for Cisco product.
         * Original            BS: 0x08     DEL: 0x7F
         * Reflection(VT102)   BS: 0x7F     DEL: 0x1B 0x5B 0x33 0x7E
         * Reflection(ANSI)    BS: 0x08     DEL: 0x7F
         *
         * ret_val = DEL;
         */
        ret_val = DEL;
    }
    else
    {
        ret_val = UNKNOWN;
    }

    return (ret_val);
}
#endif  /* #if (SYS_CPNT_CLI_TERMINAL == TRUE) */

/*------------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_GetKeyByVtTerminal                   |
 *------------------------------------------------------------|
 * PURPOSE : Get byte or escape sequence by VT100/102 terminal|
 * INPUT   : char c                                           |
 * OUTPUT  : none                                             |
 * RETURN  : Key code definition                              |
 * NOTE    : none                                             |
 *------------------------------------------------------------*/
static UI16_T CLI_IO_GetKeyByVtTerminal(char *ch)
{
    char   c;
    UI16_T ret_val;

    /* Parsing contents */
    if (*ch == 0x1B)
    {
        if((c = CLI_IO_ReadByte()) == 0)
            return KEYIN_ERROR;

        *ch = c;

        if ((c == 0x42) || (c == 0x62))
            ret_val = ESC_B;
        else if ((c == 0x43) || (c == 0x63))
            ret_val = ESC_C;
        else if ((c == 0x44) || (c == 0x64))
            ret_val = ESC_D;
        else if ((c == 0x46) || (c == 0x66))
            ret_val = ESC_F;
        else if ((c == 0x4C) || (c == 0x6C))
            ret_val = ESC_L;
        else if ((c == 0x51) || (c == 0x71))
            ret_val = ESC_Q;
        else if ((c == 0x55) || (c == 0x75))
            ret_val = ESC_U;
        else if ((c == 0x59) || (c == 0x79))
            ret_val = ESC_Y;
        else if (c == 0x5B || c == 0x4F )  /* Simon */
        {
           if((c = CLI_IO_ReadByte()) == 0)
              return KEYIN_ERROR;

            switch (c)
            {
            case 0x41:
               ret_val = UP;
               break;

            case 0x42:
               ret_val = DOWN;
               break;

            case 0x43:
               ret_val = RIGHT;
               break;

            case 0x44:
               ret_val = LEFT;
               break;

            case 0x33:
               if((c = CLI_IO_ReadByte()) == 0)
                  return KEYIN_ERROR;

               if (c == 0x7E)
                  ret_val = DEL;
               else
                  ret_val = UNKNOWN;
               break;

            default:
               ret_val = UNKNOWN;
               break;
            }
        }
        else
        {
            ret_val = UNKNOWN;
        }
    }
    else if (*ch == 0xD)
    {
        ret_val = ENTER;
    }
    else if (*ch == 0xA)
    {
        ret_val = ENTER;
    }
    else if (*ch == 0x1)
    {
        ret_val = CTRL_A;
    }
    else if (*ch == 0x2)
    {
        ret_val = CTRL_B;
    }
    else if (*ch == 0x3)
    {
        ret_val = CTRL_C;
    }
    else if (*ch == 0x4)
    {
        ret_val = CTRL_D;
    }
    else if (*ch == 0x5)
    {
        ret_val = CTRL_E;
    }
    else if (*ch == 0x6)
    {
        ret_val = CTRL_F;
    }
    else if (*ch == 0x7)
    {
        ret_val = CTRL_G; /*Debug mode*/
    }
    else if (*ch == 0x8)
    {
        ret_val = BS;
    }
    else if (*ch == 0x09)
    {
        ret_val = TAB;
    }
    else if (*ch == 0xB)
    {
        ret_val = CTRL_K;
    }
    else if (*ch == 0xC)
    {
        ret_val = CTRL_L;
    }
    else if (*ch == 0xE)
    {
        ret_val = CTRL_N;
    }
    else if (*ch == 0x10)
    {
        ret_val = CTRL_P;
    }
    else if (*ch == 0x12)
    {
        ret_val = CTRL_R;
    }
    else if (*ch == 0x14)
    {
        ret_val = CTRL_T;
    }
    else if (*ch == 0x15)
    {
        ret_val = CTRL_U;
    }
    else if (*ch == 0x16)
    {
        ret_val = CTRL_V;
    }
    else if (*ch == 0x17)
    {
        ret_val = CTRL_W;
    }
    else if (*ch == 0x18)
    {
        ret_val = CTRL_X;
    }
    else if (*ch == 0x19)
    {
        ret_val = CTRL_Y;
    }
    else if (*ch == 0x1A)
    {
        ret_val = CTRL_Z;
    }
    else if ((*ch >= 0x20) && (*ch <= 0x7E))    /* printables */
    {
        if (*ch == 0x3f)
            ret_val = QUMARK;
        else
            ret_val = PRNTABLE;
    }
    else if (*ch == 0x7F)
    {
        /* modified by chuanny, 1999-01-21
         * Rev 0.01.19(2)
         * It is for Cisco product.
         * Original            BS: 0x08     DEL: 0x7F
         * Reflection(VT102)   BS: 0x7F     DEL: 0x1B 0x5B 0x33 0x7E
         * Reflection(ANSI)    VS: 0x08     DEL: 0x7F
         *
         * ret_val = DEL;
         */
        ret_val = BS;
    }
    else
    {
        ret_val = UNKNOWN;
    }
    return ret_val;
}

#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP == TRUE)
/*------------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_ProcessDynamicProvision              |
 *------------------------------------------------------------|
 * PURPOSE : Process Dynamic Provision                        |
 * INPUT   : ctrl_P                                           |
 * OUTPUT  : none                                             |
 * RETURN  : TRUE - has performed procedure                   |
 *                  (whatever success or fail)                |
 *           FALSE - not perform yet                          |
 *                  (not get dhcp option yet)                 |
 *------------------------------------------------------------*/
static BOOL_T CLI_IO_ProcessDynamicProvision(CLI_TASK_WorkingArea_T *ctrl_P)
{
    L_INET_AddrIp_T server_ip;
    char server_ip_buf[SYS_ADPT_DHCP_MAX_DOMAIN_NAME_LEN] = {0};
    char src_filename[MAXSIZE_tftpSrcFile+1] = {0};
    char username[MAXSIZE_fileCopyServerUserName + 1] = {0};
    char password[MAXSIZE_fileCopyServerPassword + 1] = {0};
    BOOL_T dhcp_config_staus, xfer_result;

    memset(&server_ip, 0, sizeof(server_ip));

    CLI_MGR_Get_DhcpRxOptionConfig(&dhcp_config_staus, server_ip_buf, src_filename);

    if(FALSE == dhcp_config_staus)
    {
        return FALSE;
    }

    is_first_copy = FALSE;  /* clear flag */

    CLI_LIB_PrintStr("\nPerforming the second startup provision ...\r\n");
    CLI_LIB_PrintStr("Waiting for getting TFTP configuration file ...\r\n");

    if (strcmp(src_filename, SYS_DFLT_restartConfigFile) == 0)
    {
        CLI_LIB_PrintStr_1("\nFailed to download file %s.\r\n", SYS_DFLT_restartConfigFile);
        return TRUE;
    }

    if(FALSE == L_INET_StringToInaddr(
        (UI8_T *)server_ip_buf, sizeof(L_INET_AddrIp_T), (L_INET_Addr_T*)&server_ip))
    {
        if(FALSE == CLI_LIB_HostStringToInaddr((UI8_T *)server_ip_buf, AF_INET, &server_ip))
        {
            CLI_LIB_PrintStr_1("\nFailed to convert %s to IP address.\r\n", server_ip_buf);
            return TRUE;
        }
    }

    CLI_MGR_Set_XferProgressStatus(TRUE);
    CLI_MGR_Set_XferProgressResult(FALSE); /*init state*/

    if (FALSE == XFER_PMGR_CopyFile(&server_ip,
                                    (UI8_T*)src_filename, /*keep the same as source*/
                                    (UI8_T*)src_filename,
                                    FS_FILE_TYPE_CONFIG,
                                    XFER_MGR_REMOTE_TO_LOCAL,
                                    XFER_MGR_REMOTE_SERVER_TFTP,
                                    (UI8_T *)username,
                                    (UI8_T *)password,
                                    (UI32_T)ctrl_P,
                                    SYS_BLD_CLI_GROUP_IPCMSGQ_KEY,
                                    0
                                    ))
    {
        CLI_LIB_PrintStr("Failed to copy from remote to stream.\r\n");
        return TRUE;
    }

    CLI_LIB_Print_CopyInProgressSign();
    CLI_MGR_Get_XferProgressResult(&xfer_result);

    if (TRUE == xfer_result)
    {
        UI8_T *xfer_buf;
        UI32_T xfer_buf_len;
        I32_T offset;

        CLI_LIB_PrintStr("Warning: System config will be changed and system will restart.\r\n");

        if( FS_SetStartupFilename(DUMMY_DRIVE,FS_FILE_TYPE_CONFIG, (UI8_T*)src_filename)!=FS_RETURN_OK )
        {
            CLI_LIB_PrintStr("Failed to set Startup configuration file name\r\n");
            return TRUE;
        }

        if((xfer_buf = (UI8_T *)BUFFER_MGR_Allocate()) == NULL)
        {
            CLI_LIB_PrintStr("Failed to allocate resource.\r\n");
            return TRUE;
        }

        memset(xfer_buf, 0, SYS_ADPT_MAX_FILE_SIZE);

        strcpy((char *)xfer_buf, CLI_MGR_RUNTIME_PROVISION_MAGIC_WORD);

         if (FS_RETURN_OK != XFER_PMGR_LocalToStream((UI8_T *)src_filename,
                                                     xfer_buf + strlen(CLI_DEF_RUNTIME_PROVISION_MAGIC_WORD),
                                                     &xfer_buf_len,
                                                     SYS_ADPT_MAX_FILE_SIZE - strlen(CLI_DEF_RUNTIME_PROVISION_MAGIC_WORD)))
        {
            CLI_LIB_PrintStr("Failed to load file to Stream\r\n");
            BUFFER_MGR_Free(xfer_buf);
            return TRUE;
        }

        /* notify to enter transition mode
         */
        CLI_LIB_PrintStr("Warning: System config will be changed and system will restart.\r\n");
        offset = BUFFER_MGR_GetOffset(xfer_buf);
        CLI_MGR_Set_BuffermgmtOffset(offset);
        CLI_MGR_Set_RuntimeProvisionFlag(TRUE);
        CLI_MGR_Notify_EnterTransitionMode();
    }
    else
    {
        CLI_LIB_PrintStr("Failed to copy from remote to stream.\r\n");
    }

    return TRUE;
}
#endif

/*------------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_DebugMode_GetChar                    |
 *------------------------------------------------------------|
 * PURPOSE : for backdoor mgr to get char                     |
 * INPUT   : cookie (sess_id)                                 |
 * OUTPUT  : none                                             |
 * RETURN  : a char or EOF if failed                          |
 *------------------------------------------------------------*/
int CLI_IO_DebugMode_GetChar(void *cookie)
{
    CLI_TASK_WorkingArea_T *ctrl_P;
    UI32_T sess_id = (UI32_T)cookie;
    int ch;

    if (NULL == (ctrl_P = CLI_TASK_GetWorkingAreaByIndex(sess_id)))
    {
        return EOF;
    }

    if (!ctrl_P->enter_debug_mode)
    {
        return EOF;
    }

    if (ctrl_P->sess_type == CLI_TYPE_UART)
        ch = CLI_IO_ReadFromConsole(ctrl_P);
    else
        ch = CLI_IO_ReadFromTelnet(ctrl_P);

    if (ctrl_P->sess_type != CLI_TYPE_UART)
        ch = ch == 0 ? EOF : ch;

    return ch;
}

/*------------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_DebugMode_Print_Local                |
 *------------------------------------------------------------|
 * PURPOSE : for backdoor mgr to print string                 |
 * INPUT   : ctrl_P                                           |
 *           buf - string to pring                            |
 *           len - num of char to print, 0 to print all       |
 * OUTPUT  : none                                             |
 * RETURN  : >= 0 - successful                                |
 *           < 0  - failed                                    |
 *------------------------------------------------------------*/
static int CLI_IO_DebugMode_Print_Local(CLI_TASK_WorkingArea_T *ctrl_P, const char *buf, size_t len)
{
    int ret;

    if (ctrl_P->sess_type == CLI_TYPE_UART)
    {
        if (len == 0)
            ret = printf("%s", buf);
        else
            ret = printf("%-*s", len, buf);
        fflush(stdout);
    }
    else
    {
        size_t remain_len;
        int ret_send;

        ret = remain_len = (len == 0) ? strlen(buf) : len;

        while (remain_len > 0)
        {
            if ((ret_send = send(ctrl_P->socket, buf, remain_len, 0)) < 0)
            {
                ret = -1;
                break;
            }
            else if (ret_send > 0)
            {
                remain_len -= ret_send;
                buf += ret_send;
            }
            else
            {
                SYSFUN_Sleep(10);
            }
        }
    }

    return ret;
}

/*------------------------------------------------------------|
 * ROUTINE NAME - CLI_IO_DebugMode_Print                      |
 *------------------------------------------------------------|
 * PURPOSE : for backdoor mgr to print string                 |
 * INPUT   : cookie (sess_id)                                 |
 *           s - string                                       |
 * OUTPUT  : none                                             |
 * RETURN  : >= 0 - successful                                |
 *           < 0  - failed                                    |
 *------------------------------------------------------------*/
int CLI_IO_DebugMode_Print(void *cookie, const char *s)
{
    CLI_TASK_WorkingArea_T *ctrl_P;
    UI32_T sess_id = (UI32_T)cookie;
    int ret = 0;

    if (NULL == (ctrl_P = CLI_TASK_GetWorkingAreaByIndex(sess_id)))
    {
        return -1;
    }

    if (s == 0)
    {
        return -1;
    }

    if (*s == 0)
    {
        return 0;
    }

    if (ctrl_P->sess_type == CLI_TYPE_UART)
    {
        ret = CLI_IO_DebugMode_Print_Local(ctrl_P, s, 0);
    }
    else
    {
        /* replace '\n' to '\r\n'
         */
        const char *p = s;
        size_t l, len = 0;

        while (ret >= 0)
        {
            len += (l = strcspn(p, "\r\n"));

            switch (*(p += l))
            {
                case 0:
                    if (len > 0)
                    {
                        ret = CLI_IO_DebugMode_Print_Local(ctrl_P, s, 0);
                    }
                    return 0;

                case '\r':
                    if (*(p + 1) == '\n')
                    {
                        p++;
                        len++;
                    }
                    p++;
                    len++;
                    break;

                case '\n':
                    if (len > 0)
                    {
                        ret = CLI_IO_DebugMode_Print_Local(ctrl_P, s, len);
                    }
                    ret = CLI_IO_DebugMode_Print_Local(ctrl_P, "\r\n", 0);
                    s = ++p;
                    len = 0;
                    break;
            }
        }
    }

    return ret;
}

