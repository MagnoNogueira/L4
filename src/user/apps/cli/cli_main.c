/****************************************************************************
 * INCLUDE FILE - cli_main.c
 * --------------------------------------------------------------------------
 * ABSTRACT: UIS menu screen related functions
 *---------------------------------------------------------------------------
 * Copyright(C)                                    Accton Corporation, 1997
 *****************************************************************************/
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "sys_type.h"
#include "sys_bld.h"
#include "sysfun.h"
#include "sys_cpnt.h"
#include "sys_module.h"
#include "l_mm.h"
#include "l_ipcio.h"
#include "leaf_sys.h"
#include "leaf_es3626a.h"
#include "sys_pmgr.h"
#include "sys_mgr.h"
#include "swctrl.h"
#include "swctrl_pmgr.h"
#include "swctrl_pom.h"
#include "userauth.h"
#include "userauth_pmgr.h"
#include "backdoor_mgr.h"
#include "syslog_type.h"

#include "cli_def.h"
#include "cli_main.h"
#include "cli_pars.h"
#include "clis.h"
#include "cli_task.h"
#include "cli_lib.h"
#include "cli_mgr.h"
#include "cli_type.h"
#include "cli_cmd.h"
#include "cli_msg.h"
#include "cli_io.h"
#include "cli_auth.h"
#include "cli_init.h"
#include "cli_arg.h"
#include "cli_api_show.h"

#if (SYS_CPNT_MOTD == TRUE)
#include "sys_bnr_pmgr.h"
#endif

#include "aaa_def.h"
#include "aaa_mgr.h"
#if (SYS_CPNT_ACCOUNTING == TRUE)
#include "aaa_pmgr.h"
#endif

#if(SYS_CPNT_CLUSTER==TRUE)
#include "cluster_pmgr.h"
#include "cluster_pom.h"
#include "cluster_type.h"
#include "sys_dflt.h"
#endif

//#if (SYS_CPNT_IPV6 == TRUE) || ( SYS_CPNT_MLDSNP== TRUE)
#include "l_ipv4v6.h" /* common lib for IPv4/v6 address syntax checking */
//#endif

#include "temp.h"
#include "trap_event.h"
#include "snmp_pmgr.h"

#if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE)
#include "buffer_mgr.h"
#endif

#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP== TRUE)
#include "sys_callback_mgr.h"
#endif

#if (SYS_CPNT_CLI_BANNER == TRUE)
#include "cli_banner.h"
#endif

#define NO_PRE_MATCH       -1
#define NO_INVALID_CHAR_TOKEN 65535

#define CLI_MAIN_PMGR_MSGBUF_MAX_REQUIRED_SIZE sizeof(CLI_MAIN_MGR_MSG_T)

typedef union CLI_MAIN_MGR_MSG_U
{

#if (SYS_CPNT_CLI == TRUE)
    CLI_MGR_IPCMsg_T cli_mgr_ipcmsg;
#endif
    L_IPCIO_Msg_T        l_ipcio_ipcmsg;

#if (SYS_CPNT_CLI_DYNAMIC_PROVISION_VIA_DHCP== TRUE)
    struct
    {
        SYS_CALLBACK_MGR_Msg_T sys_callback_msg_header;
        union
        {
            SYS_CALLBACK_MGR_DHCP_RxOptionConfig_CBData_T dhcp_rx_option;
        } sys_callback_msg_data;
    } sys_callback_msg;
#endif

} CLI_MAIN_MGR_MSG_T;

typedef struct
{
    UI16_T  ret_aidx;
    char    *err_ptr;
    UI16_T  err_kind;
    char   *args [CLI_DEF_MAX_ARGS_NUM+1];
    char   *args_full[CLI_DEF_MAX_ARGS_NUM+1];
} Arg_Ref_T;

typedef struct
{
    char     *p;
    UI16_T   len;
} Token_T;


typedef enum CLI_MAIN_MONITOR_EXIT_CODE_E
{
   CLI_MAIN_MONITOR_ERROR = 1,
   CLI_MAIN_MONITOR_TIMEOUT,
   CLI_MAIN_MONITOR_LOGOUT,
   CLI_MAIN_MONITOR_UNKNOWN,
} CLI_MAIN_MONITOR_EXIT_CODE_T;

/*
   pttch 92.04.24 add for check config file is valid or not
*/
typedef enum CLI_MAIN_CONFIG_E
{
   CLI_MAIN_CONFIG_FILE_OK = 1,
   CLI_MAIN_CONFIG_FILE_WRONG,
   CLI_MAIN_CONFIG_FILE_FAIL
} CLI_MAIN_CONFIG_T;

static BOOL_T  is_continue_provision = FALSE;
/*pttch 2004/5/17 02:32pm add for accton cli module provision*/

#define CLI_PROVISION_CMD_ERROR_COUNT 1
#define ARG_MATCH       CLI_PARS_ARG_MATCH
#define ARG_SUBMATCH    CLI_PARS_ARG_SUBMATCH
#define ARG_NOMATCH     CLI_PARS_ARG_NOMATCH


extern BOOL_T CLI_LIB_Str_Remove_Quote_NotQuoteString(char *str);
extern BOOL_T L_IPV4V6_Is_Valid_IPv6_PrefixString(char *str, UI32_T str_len);
extern BOOL_T L_IPV4V6_Is_Valid_IPv6_AddressString(char *str, UI32_T str_len);

static UI16_T  CLI_MAIN_ProcessCmdList(char *buff, UI16_T *cmd_idx, UI16_T *str_idx, UI16_T *cmd_preidx, UI16_T *str_testidx, char *arg_buf_temp);
static UI16_T  CLI_MAIN_ProcessArgList(char *buff, UI16_T cmd_idx, Arg_Ref_T *out);
static UI16_T  CLI_MAIN_ProcessEnterkey(char *buff, UI16_T cmd_idx, UI16_T arg_idx, char *args[], UI16_T err, CLI_TASK_WorkingArea_T *ctrl_P);
static UI16_T  CLI_MAIN_ProcessTABkey(char *buff, UI16_T cmd_idx, UI16_T arg_idx, UI16_T str_idx, UI16_T err);
static UI16_T  CLI_MAIN_ProcessQuType3(char *buff, UI16_T cmd_eidx, UI16_T arg_eidx, UI16_T str_idx, UI16_T err);
static UI16_T  CLI_MAIN_ProcessQuMark(char *buff, UI16_T cmd_eidx, UI16_T arg_eidx, UI16_T str_idx, UI16_T err, CLI_TASK_WorkingArea_T *ctrl_P);

static void    CLI_MAIN_Display_Exit_Msg(UI16_T reason);
 CLI_MAIN_MONITOR_EXIT_CODE_T    CLI_MAIN_Monitor(CLI_TASK_WorkingArea_T *ctrl_P);
static UI32_T  CLI_MAIN_ReadLine_From_XferBuf(char *cmd_buf, char **tmp_xbuf, CLI_TASK_WorkingArea_T *ctrl_P);

static int     Get_Tokens (char *s, Token_T *tokens, int max);
static char    *Get_Token1 (char *s, Token_T *token, const char *delimeter);

static int     Get_Tokens_Without_Delimit_By_Colon(char *s, Token_T *tokens, int max);
static char    *Get_Token1_Without_Delimit (char *s, Token_T *token, const char *delimeter);

static int     My_atoi(char *s, int *digit_len, int radidx);

static BOOL_T Parse_Arguments( Token_T *tokens, UI16_T aidx, UI16_T token_num, I16_T pre_match_len, Arg_Ref_T *out);
static int Check_Arg_By_Data_Type (Token_T *token, UI16_T aidx, UI16_T *match_len, Arg_Ref_T *out);
//static UI8_T check_listdata_content(UI8_T *content, UI32_T max_value, I8_T **invalid_char_ptr);
static void cli_main_parse_file_header(char *token);


/*pttch 2004/5/17 02:32pm add for accton cli module provision*/
static BOOL_T cli_main_store_module_command(char *cmd_buff);
/*static UI16_T cli_main_SubStrLen(char *s,char *substr);*/
static void CLI_MAIN_ChangeToGlobalMode(CLI_TASK_WorkingArea_T *ctrl_P);
static void CLI_MAIN_CheckWhetherPrintProvisionMsg(
    CLI_TASK_WorkingArea_T *ctrl_P
    );

#if (SYS_CPNT_MOTD == TRUE)
BOOL_T operate_sys_banner_msg_status = FALSE;
UI8_T  operate_sys_banner_msg_by_session_id;
UI8_T  operate_sys_banner_msg_delimiting_character ;
UI8_T  cli_sys_bnr_msg[SYS_ADPT_MAX_MOTD_LENGTH + 1];
UI32_T cli_sys_bnr_msg_position = 0;

#endif /*#if (SYS_CPNT_MOTD == TRUE)*/

/*--------------------- extern ---------------------*/
/* cli_msg.c */
extern char *uis_promptMsg[];

/* cli_cmd.c */
extern ARGLIST ui_arg_lst[];

/* cli_msg.c */
extern char    *ui_HelpMsg[];

static UI32_T arg_position;

static char CLI_MAIN_mgrtd_ipc_buf[SYSFUN_SIZE_OF_MSG(CLI_MAIN_PMGR_MSGBUF_MAX_REQUIRED_SIZE)];


/*****************************************************************************/
/* ROUTINE NAME - CLI_MAIN_INIT()                                                     */
/* ------------------------------------------------------------------------- */
/* FUNCTION: Main INIT function.                                                  */
/*                                                                           */
/* RETURN  : none.                                                           */
/* INPUT   : none                                                     */
/* OUTPUT  :                                                                 */
/* NOTE    : Init msgq                                                       */
/*****************************************************************************/

#if 0//rich
void CLI_MAIN_INIT(void)
{
    /* create mgr ipc message queue handle
     */
    if(SYSFUN_CreateMsgQ(SYS_BLD_CLI_GROUP_IPCMSGQ_KEY, SYSFUN_MSGQ_BIDIRECTIONAL, &ipc_msgq_handle)!=SYSFUN_OK)
    {
        return;
    }

    CLI_enter_debug_mode = FALSE;
}
#endif
/*****************************************************************************/
/* ROUTINE NAME - CLI_MAIN_HandleMGR_Thread()                                                     */
/* ------------------------------------------------------------------------- */
/* FUNCTION: Main MGR function.                                                  */
/*                                                                           */
/* RETURN  : none.                                                           */
/* INPUT   : sleep ticks                                                     */
/* OUTPUT  :                                                                 */
/* NOTE    : This is the main mgr thread for this module.                    */
/*****************************************************************************/

/* this function will work like mgr thread */


    static BOOL_T CLI_MAIN_WaitForEnterAtUART(void);

void CLI_MAIN_Enter_Main_Routine(CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACCOUNTING == TRUE)
    AAA_AccRequest_T            aaa_request;
#endif /* SYS_CPNT_ACCOUNTING == TRUE */

#if (SYS_CPNT_AUTHORIZATION == TRUE)
    AAA_AuthorRequest_T         author_request;
    AAA_AuthorReply_T           author_reply;
    BOOL_T                      author_result;
#endif /*#if (SYS_CPNT_AUTHORIZATION == TRUE)*/

    CLI_MAIN_MONITOR_EXIT_CODE_T    exit_code;

    USERAUTH_AuthResult_T auth_result;
    char product_name[MAXSIZE_swProdName+1];

#if (SYS_CPNT_MOTD == TRUE)
    char  sys_bnr_msg[CLI_DEF_MAX_BUFSIZE + 1] = {0};
    UI32_T section_id = 0;
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/

    assert(ctrl_P != NULL);
    assert(!(ctrl_P->is_empty));
    CLI_MAIN_Init_Menu_Table(ctrl_P);

    /*must enter master mode can do CLI engine*/
     /*check if the STKCTRL notify to enter transition or slave mode mode*/
    if( CLI_MGR_Opmode_Get_Operation_Mode()!= CLI_MGR_OPMODE_MASTER_MODE)
    {
        if ( ctrl_P->sess_type == CLI_TYPE_PROVISION || ctrl_P->sess_type == CLI_TYPE_MODULE_PROVISION)
        {
            CLI_MGR_Set_Provision_Flag(FALSE);
            CLI_MGR_Set_PrintProvisionMsg(FALSE);
        }
        return;
    }

    switch(ctrl_P->sess_type)
    {
    case CLI_TYPE_PROVISION:
    case CLI_TYPE_MODULE_PROVISION:
       ctrl_P->CMenu.AccMode = NORMAL_EXEC_MODE;    /* On provision really not use this mode.
                                                     * The mode have been overwrite on CLI_MAIN_Monitor.
                                                     */
       ctrl_P->CMenu.AccessPrivilege = SYS_DFLT_USERAUTH_ADMIN_PRIVILEGE+1;     /* <--- just write a patch */

       CLI_LIB_PrintStr("\r\nPerforming startup provision ...\r\n");
       if(ctrl_P->sess_type==CLI_TYPE_PROVISION)
       {
            SYSFUN_Debug_Printf("CLI enter main (PROVISION)\r\n");
       }

       if(ctrl_P->sess_type==CLI_TYPE_MODULE_PROVISION)
       {
            SYSFUN_Debug_Printf("CLI enter main (MODULE_PROVISION)\r\n");
       }
       break;

    case CLI_TYPE_UART:
       ctrl_P->CMenu.AuthenState = CLI_DEF_AUTHEN_STATE_NOT_READY;
       if( CLI_MAIN_WaitForEnterAtUART() == FALSE )
           return;

    case CLI_TYPE_TELNET:
#if (SYS_CPNT_CLI_BANNER == TRUE)
        /* show banner info
         */
        if(ctrl_P->sess_type==CLI_TYPE_TELNET)
        {
            CLI_MGR_DisplayBanner();
        }
#endif

#if(SYS_CPNT_CLUSTER==TRUE)

        {
            CLUSTER_TYPE_EntityInfo_T self;

            memset(&self,0,sizeof(CLUSTER_TYPE_EntityInfo_T));
            CLUSTER_POM_GetClusterInfo(&self);

            if (self.role==CLUSTER_TYPE_ACTIVE_MEMBER && ctrl_P->sess_type == CLI_TYPE_TELNET)
            {
                strcpy((char *)ctrl_P->CMenu.UserName, SYS_DFLT_USERAUTH_ADMIN_USERNAME);
                strcpy((char *)ctrl_P->CMenu.Password,SYS_DFLT_USERAUTH_ADMIN_PASSWORD);
                ctrl_P->CMenu.AccessPrivilege = SYS_DFLT_USERAUTH_ADMIN_PRIVILEGE;
                auth_result.privilege = SYS_DFLT_USERAUTH_ADMIN_PRIVILEGE;
                ctrl_P->CMenu.AuthenState = CLI_DEF_AUTHEN_STATE_NOT_CHECK_USERNAME_PASSWORD;
            }
        }

       if (ctrl_P->CMenu.AuthenState != CLI_DEF_AUTHEN_STATE_NOT_CHECK_USERNAME_PASSWORD)
       {
           ctrl_P->CMenu.AuthenState = CLI_DEF_AUTHEN_STATE_READY;
       }

#else
       ctrl_P->CMenu.AuthenState = CLI_DEF_AUTHEN_STATE_READY;
#endif
#if (SYS_CPNT_MOTD == TRUE)
/*
The customer Foundry wants Banner MOTD messages ("Welcome to EdgeIron 4802CF managment")
to appear before User Login messages ("User Access Verification").

 > Welcome to EdgeIron 4802CF management
 >
 > User Access Verification
 >
 > Username: admin
 > Password:
 >
 >       CLI session with the Edgelron 24G-POE is opened.
 >       To end the CLI session, enter [Exit].
*/
        while(SYS_BNR_PMGR_GetNextSysBnrMsgByID(SYS_BNR_MGR_MOTD_TYPE, sys_bnr_msg, &section_id, CLI_DEF_MAX_BUFSIZE) == TRUE)
        {
            if(section_id == 1)/*first enter*/
            {
                CLI_LIB_PrintStr("\r\n");
            }
            CLI_LIB_PrintStr(sys_bnr_msg);
        }
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/

        if (   (0)
#if (SYS_CPNT_CLUSTER == TRUE)
            || (ctrl_P->CMenu.AuthenState == CLI_DEF_AUTHEN_STATE_NOT_CHECK_USERNAME_PASSWORD)
#endif /* #if (SYS_CPNT_CLUSTER == TRUE) */
            || (CLI_AUTH_CheckLoginPassword(ctrl_P, &auth_result)))
        {
            ctrl_P->CMenu.AuthenState = CLI_DEF_AUTHEN_STATE_PASS;

#if (SYS_CPNT_AUTHORIZATION == TRUE)
            memset(&author_request, 0, sizeof(author_request));

            if (USERAUTH_PASSWORD == auth_result.method)
            {
                /*if username is empty,then do nothing*/
                if(ctrl_P->CMenu.UserName[0] != '\0')
                {
                    if (CLI_TYPE_UART == ctrl_P->sess_type) /*console*/
                    {
                        author_request.ifindex = 0;
                    }
                    else /*telnet*/
                    {
                        author_request.ifindex = CLI_TASK_GetMySessId() - CLI_TASK_MIN_TELNET_SESSION_ID + 1;
                        author_request.rem_ip_addr = ctrl_P->CMenu.TelnetIPAddress;
                    }

                    strncpy(author_request.user_name, ctrl_P->CMenu.UserName, sizeof(author_request.user_name)-1);
                    author_request.user_name[sizeof(author_request.user_name)-1] = '\0';
                    strncpy(author_request.password, ctrl_P->CMenu.Password, sizeof(author_request.password)-1);
                    author_request.password[sizeof(author_request.password)-1] = '\0';

                    author_request.client_type = AAA_CLIENT_TYPE_EXEC;
                    author_request.current_privilege = auth_result.privilege;

                    switch (auth_result.authen_by_whom)
                    {
                        case USERAUTH_AUTH_RADIUS:
                            author_request.authen_by_whom = AAA_AUTHEN_BY_RADIUS;
                            break;

                        case USERAUTH_AUTH_TACACS:
                            author_request.authen_by_whom = AAA_AUTHEN_BY_TACACS_PLUS;
                            break;

                        case USERAUTH_AUTH_LOCAL:
                            author_request.authen_by_whom = AAA_AUTHEN_BY_LOCAL;
                            break;

                        case USERAUTH_AUTH_NONE:
                        default:
                            author_request.authen_by_whom = AAA_AUTHEN_BY_UNKNOWN;
                                break;
                    }

                    if (TRUE == AAA_PMGR_AuthorRequest(&author_request, &author_reply))
                    {
                        if (author_reply.return_type == AAA_AuthorRequest_SUCCEEDED
                            || author_reply.return_type == AAA_AuthorRequest_SUCCEEDED_WITH_NO_PRIV)
                        {
                            author_result = TRUE;
                        }
                        else
                        {
                            author_result = FALSE;
                        }
                    }
                    else
                    {
                        if (author_reply.return_type == AAA_AuthorRequest_CONFIG_IMCOMPLETE)
                        {
                            author_result = TRUE;
                        }
                        else
                        {
                            author_result = FALSE;
                        }
                    }

                    if (TRUE == author_result)
                    {
                        if (author_reply.return_type == AAA_AuthorRequest_SUCCEEDED)
                        {
                            auth_result.privilege = author_reply.new_privilege;
                            ctrl_P->CMenu.AccessPrivilege = author_reply.new_privilege;
                        }
                    }
                    else
                    {
                        /*
                         * Backwards compatible. Reset the privilege to '0'.
                         * Because in authentication step, maybe we get '15'
                         * privilege.
                         */
                        auth_result.privilege = 0;
                        ctrl_P->CMenu.AccessPrivilege = 0;
                    }

                }
            }

#endif /*#if (SYS_CPNT_AUTHORIZATION == TRUE)*/

          switch (auth_result.privilege)
          {
          case 15:
          case 16: /*superuser*/
             ctrl_P->CMenu.AccMode = PRIVILEGE_EXEC_MODE;
             break;

          case 0:
          default:
             ctrl_P->CMenu.AccMode = NORMAL_EXEC_MODE;
             break;
          }

          if (!SYS_PMGR_GetProductName(SYS_VAL_LOCAL_UNIT_ID, (UI8_T *)product_name))
          {
            product_name[0] = '\0';
          }
          CLI_LIB_PrintStr("\r\n      CLI session with the ");
          CLI_LIB_PrintStr(product_name);
          CLI_LIB_PrintStr(" is opened.\r\n");
          CLI_LIB_PrintStr("      To end the CLI session, enter [Exit].\r\n\r\n");

#if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE)
          CLI_LIB_ShowReloadInfo(TRUE);
#endif  /* #if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE) */
       }
       else
       {
          return;
       }
       break;

    default:
       return;
    }

    ctrl_P->CMenu.CliState = UIS_MENU_INIT_STATE;
    //CLI_MAIN_Monitor(ctrl_P);

#if (SYS_CPNT_ACCOUNTING == TRUE)

    switch(ctrl_P->sess_type)
    {
        case CLI_TYPE_UART:
        case CLI_TYPE_TELNET:

            memset(&aaa_request, 0, sizeof(AAA_AccRequest_T));

            if (CLI_TYPE_UART == ctrl_P->sess_type)
            {
                aaa_request.ifindex = 0;
            }
            else
            {
                aaa_request.ifindex = CLI_TASK_GetMySessId() - CLI_TASK_MIN_TELNET_SESSION_ID + 1;
                memcpy(&aaa_request.rem_ip_addr, &ctrl_P->CMenu.TelnetIPAddress,
                       sizeof(aaa_request.rem_ip_addr));
            }

            strncpy(aaa_request.user_name, (char *)ctrl_P->CMenu.UserName, SYS_ADPT_MAX_USER_NAME_LEN);
            aaa_request.user_name[SYS_ADPT_MAX_USER_NAME_LEN] = '\0';
            aaa_request.client_type = AAA_CLIENT_TYPE_EXEC;
            aaa_request.request_type = AAA_ACC_START;

            switch (auth_result.authen_by_whom)
            {
                case USERAUTH_AUTH_RADIUS:
                    aaa_request.auth_by_whom = AAA_AUTHEN_BY_RADIUS;
                    break;

                case USERAUTH_AUTH_TACACS:
                    aaa_request.auth_by_whom = AAA_AUTHEN_BY_TACACS_PLUS;
                    break;

                case USERAUTH_AUTH_LOCAL:
                    aaa_request.auth_by_whom = AAA_AUTHEN_BY_LOCAL;
                    break;

                case USERAUTH_AUTH_NONE:
                default:
                    aaa_request.auth_by_whom = AAA_AUTHEN_BY_UNKNOWN;
                        break;
            }
            AAA_PMGR_AsyncAccountingRequest(&aaa_request);

            break;

        case CLI_TYPE_PROVISION:
        default:
            break;
    }
#endif /* SYS_CPNT_ACCOUNTING == TRUE */
    if((ctrl_P->sess_type == CLI_TYPE_UART)
        ||(ctrl_P->sess_type == CLI_TYPE_TELNET))
    {
        TRAP_EVENT_TrapData_T data;
        memset(&data,0,sizeof(TRAP_EVENT_TrapData_T));
        /* send trap when login succeed */
        data.trap_type = TRAP_EVENT_LOGIN_SUCCEED_TRAP;
        data.community_specified = FALSE;
        if(ctrl_P->sess_type == CLI_TYPE_UART)
        {
            data.u.login_succeed_trap.loginSucceedMode = VAL_loginSucceedMode_console;
           // data.u.login_succeed_trap.loginSucceedIp = 0;
         memset(&data.u.login_succeed_trap.loginSucceedIp ,0,sizeof(L_INET_AddrIp_T));

        }
        else if(ctrl_P->sess_type == CLI_TYPE_TELNET)
        {
            data.u.login_succeed_trap.loginSucceedMode = VAL_loginSucceedMode_telnet;
           // data.u.login_succeed_trap.loginSucceedIp =ctrl_P->CMenu.RemoteTelnetClientIpAddr;
         memcpy(&data.u.login_succeed_trap.loginSucceedIp,&ctrl_P->CMenu.TelnetIPAddress,sizeof(L_INET_AddrIp_T));

        }
        strncpy((char *)data.u.login_succeed_trap.loginSucceedUsername,(char *)ctrl_P->CMenu.UserName,strlen(ctrl_P->CMenu.UserName));
        SNMP_PMGR_ReqSendTrapOptional(&data,TRAP_EVENT_SEND_TRAP_OPTION_LOG_AND_TRAP);
    }
    exit_code = CLI_MAIN_Monitor(ctrl_P);

#if (SYS_CPNT_MOTD == TRUE)
    /*initial*/
    if( (operate_sys_banner_msg_status == TRUE) &&
    (operate_sys_banner_msg_by_session_id == CLI_TASK_GetMySessId()))
    {
        operate_sys_banner_msg_status = FALSE;
    }
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/

#if (SYS_CPNT_ACCOUNTING == TRUE)
    switch(ctrl_P->sess_type)
    {
        case CLI_TYPE_UART:
        case CLI_TYPE_TELNET:
            aaa_request.request_type = AAA_ACC_STOP;
            switch (exit_code)
            {
                case CLI_MAIN_MONITOR_ERROR:
                    aaa_request.terminate_cause = AAA_ACC_TERM_BY_NAS_ERROR;
                    break;
                case CLI_MAIN_MONITOR_TIMEOUT:
                    aaa_request.terminate_cause = AAA_ACC_TERM_BY_ACCT_IDLE_TIMEOUT;
                    break;
                case CLI_MAIN_MONITOR_LOGOUT:
                    aaa_request.terminate_cause = AAA_ACC_TERM_BY_USER_REQUEST;
                    break;
                case CLI_MAIN_MONITOR_UNKNOWN:
                default:
                    aaa_request.terminate_cause = AAA_ACC_TERM_BY_NAS_ERROR;
                    break;
            }

            AAA_PMGR_AsyncAccountingRequest(&aaa_request);
            break;

        case CLI_TYPE_PROVISION:
        default:
            break;
    }
#endif /* SYS_CPNT_ACCOUNTING == TRUE */
    return;
}

#if (SYS_CPNT_AUTOBAUDRATE == TRUE)
static BOOL_T CLI_MAIN_AutoDetectBautrate(UI32_T UART_Handle, SYS_MGR_Uart_BaudRate_T *baudrate_p)
{
    char c;
    int start, free, nu, count;
    int baud[] = {
        (int)SYS_MGR_UART_BAUDRATE_115200,
        (int)SYS_MGR_UART_BAUDRATE_57600,
        (int)SYS_MGR_UART_BAUDRATE_38400,
        (int)SYS_MGR_UART_BAUDRATE_19200,
        (int)SYS_MGR_UART_BAUDRATE_9600
    };

    start = 0;
    count = 0;
    nu = sizeof(baud) / sizeof(int);
    SYSFUN_EnterAutoBaud(UART_Handle);

    for (;;) {
        c = 0;
        free = 0;
        SYSFUN_SetUartBaudRate(UART_Handle, baud[start]);

        for (;;) {
            if (SYSFUN_UARTPollRxBuff(UART_Handle, 1, &c) > 0)
                break;
            else {
                /* every baudrate has max 50 ticks to wait for user input */
                if (free++ > 10)
                    break;
                SYSFUN_Sleep(5);
            }
        }

        if (c == 0x0d) {//Enter = 0x0d
            if (baudrate_p)
                *baudrate_p = (SYS_MGR_Uart_BaudRate_T)baud[start];
            SYSFUN_LeaveAutoBaud(UART_Handle);
            return TRUE;
        }

        start = (start + 1) % nu;
        if (start == 0) {
            if (++count == 10) //try 10 times
                break;
            SYSFUN_Sleep(5);
        }
    }
    SYSFUN_LeaveAutoBaud(UART_Handle);

    return FALSE;
}
#endif

static BOOL_T CLI_MAIN_WaitForEnterAtUART(void)
{
    #define WAIT_FOR_ENTER_STR "Press ENTER to start session "

    UI32_T length=0;

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();
    length = ctrl_P->CMenu.histsize;
#else
    length = CLI_DEF_MAX_HIST;
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

#if (SYS_CPNT_CLI_BANNER == FALSE)

    CLI_LIB_PrintNullStr(length +1);
    /* mechanism of wait for ENTER */
    CLI_LIB_PrintStr(WAIT_FOR_ENTER_STR);

#else/*#if (SYS_CPNT_CLI_BANNER == FALSE)*/

    /* show banner info
     */
    CLI_MGR_DisplayBanner();

#endif

    while (CLI_MGR_Opmode_Get_Operation_Mode() == CLI_MGR_OPMODE_MASTER_MODE)
    {
        char ch;
        int   i;
        UI16_T key = KEYIN_ERROR;

#if (SYS_CPNT_AUTOBAUDRATE == TRUE)
        BOOL_T is_detected = FALSE;
        UI32_T UART_Handle;
#endif

        CLI_IO_OpenUart();
#if (SYS_CPNT_AUTOBAUDRATE == TRUE)
        CLI_IO_GetUartHandler(&UART_Handle);
        if ((is_detected = CLI_MGR_CheckAutobaud()) == CLI_MGR_AUTOBAUDRATE_NOT_ENABLE)
            key = CLI_IO_GetKey(&ch);
        else
        {
            SYSFUN_Sleep(25);
            if (is_detected == CLI_MGR_AUTOBAUDRATE_DETECT_ENTERKEY)
            {
                while (CLI_MAIN_AutoDetectBautrate(UART_Handle, (SYS_MGR_Uart_BaudRate_T *)0) == FALSE) {
                    if (CLI_MGR_Opmode_Get_Operation_Mode() != CLI_MGR_OPMODE_MASTER_MODE)
                        return FALSE;
                    SYSFUN_Sleep(500);//key = KEYIN_ERROR;
                }
                key = ENTER;
            }
        }
#else
        key = CLI_IO_GetKey(&ch);
#endif

        if (key == ENTER)
        {
            for (i=0; i<strlen(WAIT_FOR_ENTER_STR); i++)
                CLI_LIB_PrintStr(BACK);

            for (i=0; i<strlen(WAIT_FOR_ENTER_STR); i++)
                CLI_LIB_PrintStr(SPACE);

            for (i=0; i<strlen(WAIT_FOR_ENTER_STR); i++)
                CLI_LIB_PrintStr(BACK);

            CLI_LIB_PrintNullStr(2);

            break;
        }
        else if(key == KEYIN_ERROR)
        {
            return FALSE;
        }
        else
        {
           SYSFUN_Sleep(5);
        }
    }
    if (CLI_MGR_Opmode_Get_Operation_Mode() == CLI_MGR_OPMODE_MASTER_MODE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

/* Initial uis_ctrl_panel[] of sess_id
 */
void CLI_MAIN_Init_Menu_Table(CLI_TASK_WorkingArea_T * ctrl_P)
{
   int i;

    assert(ctrl_P != NULL);

   ctrl_P->CMenu.AccMode  = NORMAL_EXEC_MODE;
   ctrl_P->CMenu.AccessPrivilege = 0;
   ctrl_P->CMenu.CliState = UIS_MENU_NOTCREATE_STATE;
   memset(ctrl_P->CMenu.OneCmdLine, ' ', CLI_DEF_MAX_BUFSIZE);
   ctrl_P->CMenu.OneCmdLine[0]='\0';
   memset(ctrl_P->CMenu.OldEditBuf, ' ', CLI_DEF_MAX_BUFSIZE);
   ctrl_P->CMenu.OldEditBuf[0]='\0';
   ctrl_P->CMenu.KeyState = 0;  /* set as normal state */
#if(SYS_CPNT_CLUSTER==TRUE)
   ctrl_P->CMenu.IsTelnetRelaying = FALSE;
#endif
   CLI_LIB_InitialHistBuf(ctrl_P);
   CLI_LIB_InitialEditMode(ctrl_P); /* default as enable */
   CLI_LIB_InitialDelBuf(ctrl_P);


   /* init the pointer of the counter table when "clear counter"
    *//*eth only*/
   for(i=0; i<SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
      ctrl_P->CMenu.counters_table[i] = NULL;

    /* get system information for CLI to use in the future
     */
    STKTPLG_POM_GetMyUnitID(&(ctrl_P->sys_info.my_unit_id));
    ctrl_P->sys_info.max_port_number = SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT;
}


/* Display message when UIS exit
 */
static void CLI_MAIN_Display_Exit_Msg(UI16_T reason)
{
   CLI_LIB_PrintNullStr(1);

   switch (reason)
   {
   case EXIT_READ_TIMEOUT:
        CLI_LIB_PrintStr(uis_promptMsg[UI_MSG_EXIT_TIMEOUT]);
        break;

   case EXIT_LOGIN_ERR_TOO_MANY:
        CLI_LIB_PrintStr(uis_promptMsg[UI_MSG_EXIT_LOGIN]);
        break;

   case EXIT_FAIL_CONNECT:
        CLI_LIB_PrintStr(uis_promptMsg[UI_MSG_EXIT_FAIL_CON]);
        break;

   case EXIT_IP_CHANGED:
        CLI_LIB_PrintStr(uis_promptMsg[UI_MSG_EXIT_IP_CHANGED]);
        break;

   case EXIT_USER_REQUEST:
   default:
        break;
   }

   CLI_LIB_PrintStr(NEWLINE);
   CLI_LIB_PrintStr(uis_promptMsg[UI_MSG_EXIT_CONNECT]);
   CLI_LIB_PrintStr(NEWLINE);
}



/* -------------------------------------------------------------------------- */
/* Called in uis menu task for user active exit, or .....                     */
/* -------------------------------------------------------------------------- */
void CLI_MAIN_Active_Exit(UI16_T reason, CLI_TASK_WorkingArea_T *ctrl_P)
{

    assert(ctrl_P != NULL);
   if (ctrl_P->sess_type != CLI_TYPE_PROVISION)
      CLI_MAIN_Display_Exit_Msg(reason);

   switch(ctrl_P->sess_type)
   {
   case CLI_TYPE_UART:
      /*back to the jump addr*/ /*the future of this task will be decided by cli_task*/
      /*CLI_JMP_Jump_To_Console_Pointer();*/ /*will be remove*/
      break;

   case CLI_TYPE_PROVISION:
   case CLI_TYPE_MODULE_PROVISION:

      CLI_LIB_PrintStr("\r\nCLI_TASK: CLI provision complete\r\n");

      /*set the provision status as finish*/
      CLI_MGR_Set_Provision_Flag(FALSE);

      /*delete task*/
      CLI_TASK_Del(ctrl_P);
      break;

   case CLI_TYPE_TELNET:
      /*free resource*/
      ctrl_P->is_empty = TRUE;

      /*delete task*/
      CLI_TASK_TnSessExit(ctrl_P);
      break;
   }
}



/*****************************************************************************/
/* CLI_MAIN_Monitor                                                          */
/*                                                                           */
/* Description:                                                              */
/*     Monitor the command line shell, which read the command from user      */
/*     and process the corresponding action routine.                         */
/*                                                                           */
/* Note:                                                                     */
/*                                                                           */
/*****************************************************************************/
CLI_MAIN_MONITOR_EXIT_CODE_T CLI_MAIN_Monitor(CLI_TASK_WorkingArea_T *ctrl_P)
{
    char     *cmd_buf;
    char     *old_buf;
    char     arg_buf[CLI_DEF_MAX_BUFSIZE+1];
    UI16_T    ret_type = 0;
    UI16_T    isNullStr;
    UI16_T    str_sidx, str_eidx, idx, str_eidx2;
    UI16_T    cmd_eidx, cmd_preidx, str_test;
    UI16_T    arg_idx;
    UI16_T    err;
    UI16_T    line_len;
    UI16_T    disp_width;
    UI16_T    i;
    BOOL_T    is_first_prov_line = TRUE;
    BOOL_T    is_first_prov_command_line = TRUE;

#if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE)
    BOOL_T    is_first_partial_prov = TRUE;
    char      *partial_tmp_xbuf;
#endif  /* #if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE) */

    Arg_Ref_T out;                 /* added by charles */

    assert(ctrl_P != NULL);
    char     *tmp_xbuf = ctrl_P->prov_buffer;
    cmd_buf = ctrl_P->CMenu.OneCmdLine;
    old_buf = ctrl_P->CMenu.OldEditBuf;

    /*Blanken buffers
    */
    memset(cmd_buf, ' ', CLI_DEF_MAX_BUFSIZE);
    *cmd_buf = '\0';
    memset(arg_buf, ' ', CLI_DEF_MAX_BUFSIZE);
    arg_buf[0] = '\0';

#if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE)
    ctrl_P->partial_provision = FALSE;
#endif  /* #if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE) */

    if ( ctrl_P->sess_type == CLI_TYPE_PROVISION || ctrl_P->sess_type == CLI_TYPE_MODULE_PROVISION)      /* From configuration file */
    {
        CLI_IO_OpenUart();
        CLI_MAIN_CheckWhetherPrintProvisionMsg(ctrl_P);
    }

    /* Enter processing loop
    */
    while (1)
    {
        /* Print the prompt string and read the command line */
        isNullStr = 1;

        do
        {
            UI32_T ret_config_file = 0;

            if ( ctrl_P->sess_type == CLI_TYPE_PROVISION || ctrl_P->sess_type == CLI_TYPE_MODULE_PROVISION)      /* From configuration file */
            {

                /*pttch 93.03.29 check by is_continue_provision */
                UI32_T error_count = 0;

                CLI_MGR_Get_ProvisionCMDCounter(&error_count);

                if (error_count >= CLI_PROVISION_CMD_ERROR_COUNT && is_continue_provision == FALSE)
                {
                    SYSFUN_Debug_Printf("\r\nLoaded the file with errors\r\n");
                    CLI_MGR_Set_Config_File_Flag(TRUE);
                    CLI_MGR_Set_Provision_Flag(FALSE);
                    return CLI_MAIN_MONITOR_ERROR;
                }

                ret_config_file = CLI_MAIN_ReadLine_From_XferBuf(cmd_buf, &tmp_xbuf, ctrl_P);

                if (ret_config_file == CLI_MAIN_CONFIG_FILE_OK)
                {
                    //cmd_buf[strlen(cmd_buf)-1] = 0; /* To replace the '\n' as '\0' */

                    CLI_LIB_PrintStr(cmd_buf);

                    ret_type = 0;

                    if(is_first_prov_line)
                    {
                        is_first_prov_line = FALSE;
                        ctrl_P->CMenu.AccMode  = PRIVILEGE_CFG_GLOBAL_MODE;
                    }
                    CLI_MGR_Set_Config_File_Flag(TRUE);
                }
                else
                {
                    if (ret_config_file == CLI_MAIN_CONFIG_FILE_WRONG)
                    {
                        CLI_MGR_Set_Config_File_Flag(FALSE);
                    }
                    CLI_MGR_Set_Provision_Flag(FALSE);
                    CLI_MGR_Set_PrintProvisionMsg(TRUE);
                    return CLI_MAIN_MONITOR_ERROR; /*to CLI_MAIN_Enter_Main_Routine()*/
                }
            } /* end of if ( ctrl_P->sess_type == CLI_TYPE_PROVISION || ctrl_P->sess_type == CLI_TYPE_MODULE_PROVISION) */
#if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE)
            else if (ctrl_P->partial_provision == TRUE)
            {
                if (is_first_partial_prov == TRUE)
                {
                    partial_tmp_xbuf = ctrl_P->partial_prov_buffer;
                    is_first_partial_prov = FALSE;
                    CLI_MAIN_CheckWhetherPrintProvisionMsg(ctrl_P);    /* press [p] to print detail */
                    ctrl_P->previous_sess_type = ctrl_P->sess_type;
                    ctrl_P->sess_type = CLI_TYPE_PARTIAL_PROV;
                    ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_GLOBAL_MODE;
                }

                ret_config_file = CLI_MAIN_ReadLine_From_XferBuf(cmd_buf, &partial_tmp_xbuf, ctrl_P);

                if (ret_config_file == CLI_MAIN_CONFIG_FILE_OK)
                {
                    CLI_LIB_PrintStr(cmd_buf);
                }
                else
                {
                    ctrl_P->partial_provision = FALSE;
                    is_first_partial_prov = TRUE;
                    ctrl_P->CMenu.AccMode  = PRIVILEGE_EXEC_MODE;   /* back to original mode */
                    ctrl_P->sess_type = ctrl_P->previous_sess_type;
                    BUFFER_MGR_Free(ctrl_P->partial_prov_buffer);
                }
            } /* end of else if(ctrl_P->partial_provision == TRUE) */
#endif  /* #if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE) */
            else                                      /* From stdio */
            {
                /* Type of End-line, 0->ENTER, 1->TAB, 2->'?', 3->error
                */
#if (SYS_CPNT_MOTD == TRUE)
                if(!(
                    (operate_sys_banner_msg_status == TRUE) &&
                    (operate_sys_banner_msg_by_session_id == CLI_TASK_GetMySessId())
                    )
                    )
                {
                    CLI_LIB_ShowPromptMsg(ctrl_P);
                }
#else

                CLI_LIB_ShowPromptMsg(ctrl_P);
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/

                ret_type = CLI_PARS_ReadLine(cmd_buf, CLI_DEF_MAX_BUFSIZE, FALSE, FALSE);
                /*add for enter transition mode*/
                /*
                switch(CLI_MGR_Opmode_Get_Operation_Mode())
                {
                case CLI_MGR_OPMODE_MASTER_MODE:
                break;

                case CLI_MGR_OPMODE_TRANSITION_MODE:
                continue;
                break;

                case CLI_MGR_OPMODE_SLAVE_MODE:
                continue;
                break;

                default:
                break;
                }
                */
                if (ret_type == 3)
                {
                    /* It may be disconnected by administrator or due to the idle-time is reached.
                     * For the first case, the kill_itself flag will be set to TRUE.
                     * Else, it is be disconnected due to idle-time is reached.
                     */
                    if (TRUE == ctrl_P->kill_itself)
                    {
                        /* For SSH/telnet, kill_itself flag will be reset in
                         * CLI_TASK_FreeWorkingSpace().
                         */
                        if (CLI_TYPE_UART == ctrl_P->sess_type)
                        {
                            ctrl_P->kill_itself = FALSE;
                        }
                        return CLI_MAIN_MONITOR_LOGOUT;
                    }
                    else
                    {
                        return CLI_MAIN_MONITOR_TIMEOUT;
                    }
                }
#if (SYS_CPNT_MOTD == TRUE)
                else if(ret_type == 4)
                {
                    operate_sys_banner_msg_status = FALSE;
                    continue; /* finish to handle system banner message */
                }
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/

            }

            isNullStr = CLI_LIB_CheckNullStr(cmd_buf);

            if (isNullStr)
            {
                *cmd_buf = '\0';

                if (ret_type == 0)
                    CLI_LIB_PrintNullStr(1);
            }
        } while ( (isNullStr) && (ret_type != 2) );

        /*add append module code here*/
        if ( ctrl_P->sess_type == CLI_TYPE_PROVISION)
        {
            if (TRUE == cli_main_store_module_command(cmd_buf))
            {
                continue;
            }
        }
        /* handle the mark of '!' */
        {
            UI32_T position;
            UI32_T char_position;
            BOOL_T is_mark = FALSE;
            BOOL_T is_fail_history = FALSE;


            for(position=0; position<strlen((char *)cmd_buf); position++)
            {
                if( !isspace(cmd_buf[position]) )
                {
                    if(cmd_buf[position] == '!')     /*leading character is '!'*/
                    {
                        if (CLI_LIB_IsProvision(ctrl_P) == TRUE)
                        {
                            CLI_MAIN_ChangeToGlobalMode(ctrl_P);

                            /*93.03.26 pttch add for unit mac header*/
                            cli_main_parse_file_header(cmd_buf);

                            CLI_LIB_PrintNullStr(1);
                        }
                        else /*user key in*/
                        {
                            UI32_T after_position;

                            for(after_position = position + 1; ; after_position++)
                            {
                                if(isspace(cmd_buf[after_position]))
                                {
                                    continue;
                                }
                                else if(cmd_buf[after_position] == 0)
                                {
                                    is_fail_history = TRUE;

                                    /* ! command, change to global configuration mode
                                    */
                                    CLI_MAIN_ChangeToGlobalMode(ctrl_P);
                                    CLI_LIB_PrintNullStr(1);
                                }
                                else
                                {
                                    char_position = after_position;
                                }
                                break;
                            }

                            if(!is_fail_history) /*there must be some characters*/
                            {
                                for( ; after_position<strlen((char *)cmd_buf); after_position++)
                                {
                                    if( cmd_buf[after_position]> '9' || cmd_buf[after_position] < '0')
                                    {
                                        is_fail_history = TRUE;
                                        CLI_LIB_PrintStr("\r\nAll characters after '!' must be digit to perform command history searching.\r\n");
                                        break;
                                    }

                                }
                            }

                            if(!is_fail_history)
                            {
                                UI32_T index_after_ex = atoi((char *)(cmd_buf+char_position));
                                UI32_T tail_idx = CLI_LIB_GetHistTailIdx(ctrl_P);

                                if(index_after_ex == 0)
                                {
                                    is_fail_history = TRUE;
                                    CLI_LIB_PrintStr("\r\nThe index should not be 0.\r\n");
                                }
                                else if(index_after_ex > tail_idx)
                                {
                                    is_fail_history = TRUE;
                                    CLI_LIB_PrintStr("\r\nNo such entry in command history.\r\n");
                                }
                                else
                                {
                                    strcpy((char *)cmd_buf, (char *)CLI_LIB_GetHistBuf(tail_idx - index_after_ex, ctrl_P));
                                    CLI_LIB_PrintNullStr(1);
                                    CLI_LIB_ShowPromptMsg(ctrl_P);
                                    CLI_LIB_PrintStr(cmd_buf);
                                }
                            }
                        }

                        is_mark = TRUE;
                        break; /*for*/
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if (is_mark && (CLI_LIB_IsProvision(ctrl_P) == TRUE || is_fail_history))
            {
                *cmd_buf = '\0';
                continue; /*do*/
            }
        }

        /*add to history buffer*/
        if (CLI_LIB_IsProvision(ctrl_P) == FALSE && ret_type == 0)
            CLI_LIB_AddHistBuf(cmd_buf, ctrl_P);


        /* Additional process for CTRL_V key
        */
        strcpy((char *)old_buf, (char *)cmd_buf);
        line_len = strlen((char *)cmd_buf);

        for (i=0; i<strlen((char *)cmd_buf); i++)
        {
            if ((*(cmd_buf+i) >= 0x01) && (*(cmd_buf+i) <= 0x1B))
            {
                if (*(cmd_buf+i) == 0x09 || *(cmd_buf+i) == 0x0d)/*pttch , to prevent some editor tftp will have 0x0d char*/
                    *(cmd_buf+i) = 0x20;  /* TAB serve as SPACE */
                else
                    *(cmd_buf+i) = 0x5E;  /* ASCII of '^' */
            }
        }

        /* Process command line
         */
        str_sidx = 0;
        arg_idx = NULL_ARG;

        err = CLI_MAIN_ProcessCmdList(cmd_buf, &cmd_eidx, &str_eidx, &cmd_preidx, &str_test, arg_buf);

        /* str_eidx: the last char of matched last cmd in cmd_buff
         * str_test: the last char of matched previous cmd in cmd_buff
         */
        str_eidx2 = str_eidx;

        if ((err == CLI_ERR_CMD_INVALID) || (err == CLI_ERR_CMD_AMBIGUOUS) ||
            (err == CLI_ERR_CMD_INVALID_RANGE) || (err == CLI_ERR_CMD_ARG_AMBIGUOUS) )
        {
            str_sidx = str_eidx;
        }
        else
        {
            /*  CLI_MAIN_ProcessCmdList does not have the return value "CLI_ERR_CMD_UNRECOGNIZED"
                CLI_ERR_CMD_INCOMPLETE: should have original str_sidx
                CLI_NO_ERROR: find previous, to get correct arg_buf
             */
            if(err!=CLI_ERR_CMD_INCOMPLETE)
            {
                /*
                 *  a[tab] will not has a space after the matched command without this.
                 */
                if( ((ret_type==2) && (*(cmd_buf+strlen((char *)cmd_buf)-1) != ' ')) || (ret_type==1) )
                {
                    str_eidx = str_test;
                }
            }
            /* get str_sidx: the first char of the last token
             */
            if (str_eidx > 0)
            {
                i = str_eidx-1;
                while ((*(cmd_buf+i) != ' ') && (i > 0))
                    i--;

                if (*(cmd_buf+i) == ' ')
                    str_sidx = i+1;
                else
                    str_sidx = i;
            }
        }

        /* Process argument list
         * 1. find arguments of previous command
         * 2. find arguments of current command
         */
        if ( (err == CLI_NO_ERROR) || (err == CLI_ERR_CMD_ARG_AMBIGUOUS) )
        {
            /* int slash_count = 0;
             */
            for (i=0; i<line_len-str_eidx; i++)
                arg_buf[i] = *(cmd_buf+i+str_eidx);

            arg_buf[i] = '\0';
            out.ret_aidx = arg_idx;
            out.err_ptr = 0;

            /* ASF3528B-F2-FLF-TER-00041
             * the following ARG_PRIVILEGE_XXX are incorrect in NEW codegen tool
             * e.g. ARG_PRIVILEGE_CFG_GLOBAL_RADIUSSERVER_A4_B7_C0
             *      ARG_PRIVILEGE_CFG_GLOBAL_RADIUSSERVER_A4_B5_C1
             *      ARG_PRIVILEGE_CFG_GLOBAL_RADIUSSERVER_A4_B3_C2
             *      ARG_PRIVILEGE_CFG_GLOBAL_RADIUSSERVER_A4_B1_C3
             *      ARG_PRIVILEGE_CFG_GLOBAL_TACACSSERVER_A2B0C0D0E0
             *      ARG_PRIVILEGE_CFG_GLOBAL_TACACSSERVER_A2B1C0
             *
             * CLI do not decode key here and decode in CLI_API_RADIUS
             * CLI_API_TACACS APIs
             */

            /* if err = CLI_ERR_CMD_ARG_AMBIGUOUS,
             * cmd_preidx should not be NULL_CMD
             */
            if(cmd_preidx != NULL_CMD)
            {
                /* a[?] || a[TAB]
                 * to find arg of previous cmd
                 * if has found the arg, ambiguous
                 */
                if( ((ret_type==2) && (*(cmd_buf+strlen((char *)cmd_buf)-1) != ' ')) || (ret_type==1) )
                {
                    UI16_T  err_cmd = err;
                    UI32_T  i, j, k;

                    /* only get one word
                     */

                    k = strlen((char *)arg_buf);

                    for( i=0; i<k; i++)
                    {
                        if(arg_buf[i]==' ')
                            continue;
                        else
                            break;
                    }

                    for( j=i ; j<k; j++ )
                    {
                        if( (arg_buf[j]==' ') || (arg_buf[j]=='\0') )
                        {
                            arg_buf[j]='\0';
                            break;
                        }
                    }

                    /* find argument index of previous command
                     */
                    err = CLI_MAIN_ProcessArgList(&arg_buf[i], cmd_preidx, &out);

                    if( cmd_preidx != cmd_eidx )  /*be found in cmdlist*/
                    {
                        switch(err)
                        {
                        case CLI_NO_ERROR:
                            /* both find in cmd and arg
                             */
                            if( (ui_arg_lst[out.ret_aidx].type != LINE_DATA) && (ui_arg_lst[out.ret_aidx].type != CHAR_DATA) )
                                err = CLI_ERR_CMD_AMBIGUOUS;
                            else if (err_cmd != CLI_ERR_CMD_ARG_AMBIGUOUS)
                                err = CLI_NO_ERROR;
                            else
                                err = err_cmd;
                            break;

                        case CLI_ERR_CMD_INCOMPLETE:  /* both find in cmd and arg */
                            err = CLI_ERR_CMD_AMBIGUOUS;
                            break;

                        default:
                            if (err_cmd != CLI_ERR_CMD_ARG_AMBIGUOUS)
                                err = CLI_NO_ERROR;
                            else
                                err = err_cmd;
                            break;
                        }
                    }
                }
            }

            /* NO_ERROR case:
               to find arg of current cmd */
            if( (err != CLI_ERR_CMD_ARG_AMBIGUOUS) && (err != CLI_ERR_CMD_AMBIGUOUS) )
            {
                str_sidx = 0;
                arg_idx = NULL_ARG;

                if (str_eidx2 > 0)
                {
                    i = str_eidx2-1;
                    while ((*(cmd_buf+i) != ' ') && (i > 0))
                        i--;

                    if (*(cmd_buf+i) == ' ')
                        str_sidx = i+1;
                    else
                        str_sidx = i;
                }

                for (i=0; i<line_len-str_eidx2; i++)
                    arg_buf[i] = *(cmd_buf+i+str_eidx2);

                arg_buf[i] = '\0';
                out.ret_aidx = arg_idx;
                out.err_ptr = 0;
                out.err_kind = CLI_NO_ERROR;

                err = CLI_MAIN_ProcessArgList(arg_buf, cmd_eidx, &out);
                str_eidx = str_eidx2;
            }

            /* 'a ?' */
            if(err == CLI_ERR_CMD_ARG_AMBIGUOUS)
            {
                err = CLI_ERR_CMD_AMBIGUOUS;
            }

            arg_idx = out.ret_aidx;
            idx = (UI32_T)out.err_ptr - (UI32_T)arg_buf  /*- (2 * slash_count)*/ ;  /* cjchu 2001/8/29 */

            if ((arg_idx != NULL_ARG) || (err == CLI_ERR_CMD_INVALID) || (err == CLI_ERR_CMD_INVALID_RANGE))
                str_sidx = str_eidx + idx;
        }  /* if */


        /* According to return type, switch to different branches
        */
        disp_width = CLI_LIB_GetTermDispWidth(ctrl_P);

        switch (ret_type)
        {
        case 0:             /* Enter key */
            if ( (err == CLI_ERR_CMD_INVALID) && (line_len >= disp_width) )
            {
                CLI_LIB_PrintStr("\r");  /* go back to header */
                CLI_LIB_ShowPromptMsg(ctrl_P); /* show prompt msg */
                str_sidx = CLI_LIB_RedisplayCmd(cmd_buf, str_sidx,disp_width, 1, ctrl_P);
            }

            CLI_LIB_PrintNullStr(1);
            /* to support '"' in CHAR_DATA */
            {
                UI16_T error_index;
                char  *temp_buf;

                if((arg_idx != NULL_ARG) && (ui_arg_lst[arg_idx].type == CHAR_DATA))
                {
                    if(*(cmd_buf+str_sidx)=='"')
                    {
                        temp_buf = (cmd_buf+str_sidx);

                        if(CLI_LIB_Check_Is_Legal_CharString(temp_buf, &error_index)!= TRUE)
                        {
                            str_sidx += (error_index-1);
                            err = CLI_ERR_CMD_INVALID;
                            break;
                        }
                    }
                }
            }
            err = CLI_MAIN_ProcessEnterkey(cmd_buf, cmd_eidx, arg_idx, out.args_full, err, ctrl_P);
            break;

        case 1:             /* TAB key */
            err = CLI_MAIN_ProcessTABkey(cmd_buf, cmd_eidx, arg_idx, str_sidx, err);
            break;

        case 2:             /* '?' key */
            err = CLI_MAIN_ProcessQuMark(cmd_buf, cmd_eidx, arg_idx, str_sidx, err, ctrl_P);
            break;
        }

        if (err > 0)
        {
            /*pttch add for check first command line for break/continue provision*/
            if (CLI_LIB_IsProvision(ctrl_P) == TRUE)      /* From configuration file */
            {
                /*
                   07/12/2004 10:24 pttch this error case should not treat as command line error
                                    do not need add any error command on it.
                 */
                if (err != CLI_EXIT_SESSION)
                {
                    CLI_MGR_Add_ProvisionCMDCounter();
                }
                if (is_first_prov_command_line)
                {
                    is_continue_provision = FALSE;
                }
            }
            if(err != CLI_EXIT_SESSION || CLI_LIB_IsProvision(ctrl_P) == FALSE)
                CLI_LIB_ShowErrMsg(err, cmd_buf, str_sidx, ctrl_P);

            if(err == CLI_ERR_TIMEOUT)
                return CLI_MAIN_MONITOR_TIMEOUT; /*to CLI_MAIN_Enter_Main_Routine()*//*charles*/
            else if (err == CLI_EXIT_SESSION)
                return CLI_MAIN_MONITOR_LOGOUT;
        }
        /*pttch add for check first command line for break/continue provision*/
        if ( ctrl_P->sess_type == CLI_TYPE_PROVISION || ctrl_P->sess_type == CLI_TYPE_MODULE_PROVISION)      /* From configuration file */
        {
            is_first_prov_command_line = FALSE;
        }

        switch (ret_type)
        {
        case 0:     /* Enter key */
            *cmd_buf = '\0';
            break;

        case 1:     /* TAB key */
            for (i=line_len; i<strlen((char *)cmd_buf); i++)
                *(old_buf+i) = *(cmd_buf+i);
            *(old_buf+i) = '\0';
            strcpy((char *)cmd_buf, (char *)old_buf);
            break;

        case 2:     /* '?' KEY */
            strcpy((char *)cmd_buf, (char *)old_buf);
            break;
        }  /* switch */


        /*check if the STKCTRL notify to enter transition or slave mode mode*/
        /*check if the STKCTRL notify to enter transition or slave mode mode*/
        if( CLI_MGR_Opmode_Get_Operation_Mode()!= CLI_MGR_OPMODE_MASTER_MODE)
        {
            if ( ctrl_P->sess_type == CLI_TYPE_PROVISION || ctrl_P->sess_type == CLI_TYPE_MODULE_PROVISION)
            {
                CLI_MGR_Set_Provision_Flag(FALSE);
                CLI_MGR_Set_PrintProvisionMsg(FALSE);
            }
            return CLI_MAIN_MONITOR_UNKNOWN;
        }

    } /* while */

    return CLI_MAIN_MONITOR_UNKNOWN;
}


/******************************************************************************/
/* CLI_MAIN_ProcessCmdList                                                    */
/*                                                                            */
/* Description:                                                               */
/*     Process command list in command line buffer                            */
/*                                                                            */
/* Arguments:                                                                 */
/*     I8_T    *buff    : Command line buffer                                 */
/*     UI16_T  *cmd_idx : Final searched command word                         */
/*     UI16_T  *str_idx : It's mean is as following description               */
/*                        (has found the last char of cmd)                    */
/* Return:                                                                    */
/*     CLI_NO_ERROR           => *str_idx means final matched cmd str end idx */
/*     CLI_ERR_CMD_INCOMPLETE => *str_idx means final matched cmd str end idx */
/*     CLI_ERR_CMD_AMBIGUOUS  => *str_idx means the ambig. cmd str start idx  */
/*     CLI_ERR_CMD_ARG_AMBIGUOUS  => *str_idx means the ambig in cmd and arg  */
/*     CLI_ERR_CMD_INVALID    => *str_idx means the started invalid str idx   */
/******************************************************************************/

static UI16_T CLI_MAIN_ProcessCmdList(char *buff, UI16_T *cmd_idx, UI16_T *str_idx,
                     UI16_T *cmd_preidx, UI16_T *str_testidx, char *arg_buf_temp)
{
    char   one_cmd_word[CLI_DEF_MAX_BUFSIZE+1];
    UI16_T  str_sidx, str_eidx, str_arg_idx;
    UI16_T  cmd_sidx, cmd_eidx, cmd_finalIdx;
    UI16_T  match_num;
    UI16_T  err, err_arg;
    Arg_Ref_T out;
    const CMDLIST *ui_cmd_lst;

    ui_cmd_lst = CLI_LIB_Get_Cmd_List();

    str_sidx = str_eidx = 0;
    cmd_sidx = 0;
    cmd_finalIdx = cmd_eidx = NULL_CMD;
    str_arg_idx = 0;
    err = CLI_NO_ERROR;

    do
    {
        one_cmd_word[0] = '\0';
        *cmd_preidx = cmd_eidx;
        *str_testidx = str_eidx;

        str_eidx=CLI_PARS_GetAWord(buff, &str_sidx, one_cmd_word);

        if (one_cmd_word[0] == '\0')        /* null string */
            break;

        err = CLI_PARS_LookupCmdWord(one_cmd_word, cmd_sidx, &cmd_eidx);

        if (err > 0)
        {
            /*
             * Ambiguous or Invalid word
             */
            if (cmd_finalIdx != NULL_CMD   &&
                ui_cmd_lst[cmd_finalIdx].child_idx == NULL_CMD) /*charles, Nov. 3, 2001*/
            {
                *cmd_idx = cmd_finalIdx;
                *str_idx = str_arg_idx;
                return CLI_NO_ERROR;
            }

            if( (cmd_finalIdx!= NULL_CMD) && (ui_cmd_lst[cmd_finalIdx].arg_idx != NULL_ARG)  /*charles, Dec. 6, 2001*/
                                    && (err != CLI_ERR_CMD_AMBIGUOUS) )  /* spanning-tree p? */
            {
                *cmd_idx = cmd_finalIdx;
                *str_idx = str_arg_idx;
                return CLI_NO_ERROR;
            }

            *cmd_idx = cmd_eidx;
            *str_idx = str_sidx;

            if (err == CLI_ERR_CMD_INVALID)
            {
                match_num = 0;
                if (cmd_eidx != NULL_CMD)
                CLI_LIB_StrCmp((char *)one_cmd_word, ui_cmd_lst[cmd_eidx].name, &match_num);
                *str_idx = str_sidx + match_num;
            }
            return err;
        }

        /* To find argument of previous command
           if find, return CLI_ERR_CMD_ARG_AMBIGUOUS.
         */
        if(*cmd_preidx != NULL_CMD)
        {
            memcpy(arg_buf_temp, &buff[str_sidx], (strlen((char *)buff)-(str_sidx)+1));

            {/* only get a token */
                UI32_T i;
                for( i=0 ; i<strlen((char *)arg_buf_temp) ; i++ )
                {
                    if( (arg_buf_temp[i]==' ') || (arg_buf_temp[i]=='\0') )
                    {
                        arg_buf_temp[i]='\0';
                        break;
                    }
                }
            }

            err_arg = CLI_MAIN_ProcessArgList(arg_buf_temp, (*cmd_preidx), &out);

            if( (ui_arg_lst[out.ret_aidx].type != LINE_DATA) && (ui_arg_lst[out.ret_aidx].type != CHAR_DATA) )
            {
                switch(err_arg)
                {
                    case CLI_NO_ERROR:
                    case CLI_ERR_CMD_INCOMPLETE:
                        *cmd_idx = cmd_eidx;
                        *str_idx = str_sidx;
                        return CLI_ERR_CMD_ARG_AMBIGUOUS;
                }
            }
        }

        if (ui_cmd_lst[cmd_eidx].cmd_func != NULL)
        {
            cmd_finalIdx = cmd_eidx;
            str_arg_idx = str_eidx;
        }

        cmd_sidx = ui_cmd_lst[cmd_eidx].child_idx;
        if (*(buff+str_eidx) != '\0')
            str_sidx = str_eidx;

        if (cmd_sidx == NULL_CMD)
        {
            /* Full command found
             */
            *cmd_idx = cmd_eidx;
            *str_idx = str_eidx;
            return CLI_NO_ERROR;
        }

    } while(*(buff+str_eidx) != '\0');

    /*
     * Process error message
     */
    if ((err == CLI_NO_ERROR)     &&
        (cmd_finalIdx == NULL_CMD || ui_cmd_lst[cmd_eidx].cmd_func == NULL ))  /*charles, Nov. 4, 2001*/
    {
        err = CLI_ERR_CMD_INCOMPLETE;
    }

    *cmd_idx = cmd_eidx;

    if (err == CLI_NO_ERROR)
        *str_idx = str_arg_idx;
    else
        *str_idx = str_eidx;

    return err;
}

/************************************************************************/
/* CLI_MAIN_ProcessEnterkey                                             */
/*                                                                      */
/* Description:                                                         */
/*     Process Enter key                                                */
/*                                                                      */
/* Arguments:                                                           */
/*     UI8_T  *buff    : Argument buffer break from command line (end)  */
/*     UI8_T  cmd_idx  : Final command word index leading this arg      */
/*     UI8_T  arg_idx  : Final matched argument index                   */
/*     UI8_T  *args[]  : Argument array of this command                 */
/*     UI16_T err      : The error status of this command line          */
/*                                                                      */
/* Return:                                                              */
/*     CLI_NO_ERROR          => Successfully process this command line  */
/*     CLI_ERR_CMD_INVALID, CLI_ERR_CMD_AMBIGUOUS,                      */
/*     CLI_ERR_CMD_INCOMPLETE, and CLI_ERR_CMD_SHOW_QUMARK              */
/*                          => Command error message                    */
/*     Others               => Error occurred due to the command action */
/************************************************************************/

static UI16_T CLI_MAIN_ProcessEnterkey(char *buff, UI16_T cmd_idx, UI16_T arg_idx, char *args[], UI16_T err, CLI_TASK_WorkingArea_T *ctrl_P)
{
   UI16_T buf_len;
   UI16_T err_code;
   const CMDLIST *ui_cmd_lst;

    assert(ctrl_P != NULL);

// dralee 99.1.26
   if (err == CLI_ERR_CMD_INCOMPLETE &&
       ((cmd_idx == NORMAL_EXEC_CMD_W1_SHOW && ctrl_P->CMenu.AccMode == NORMAL_EXEC_MODE) ||
        (cmd_idx == PRIVILEGE_EXEC_CMD_W1_SHOW && ctrl_P->CMenu.AccMode == PRIVILEGE_EXEC_MODE)))
      return CLI_ERR_CMD_SHOW_QUMARK;

   if (err > 0)
      return err;

   buf_len = strlen((char *)buff);
   err_code = CLI_ERR_CMD_NOT_IMPLEMENT;
   ui_cmd_lst = CLI_LIB_Get_Cmd_List();

    if (ui_cmd_lst[cmd_idx].max_args > 0)
    {
       /*
        * More than one arguments with this command
        */
       if (ui_cmd_lst[cmd_idx].cmd_func != NULL)
       {
           err_code = (UI16_T)(ui_cmd_lst[cmd_idx].cmd_func)(cmd_idx, args, ctrl_P);
       }
    }
    else
    {
        /* Command with no arguments
         */
        if (ui_cmd_lst[cmd_idx].cmd_func != NULL)
        {
            args[0] = 0;
            err_code = (UI16_T)(ui_cmd_lst[cmd_idx].cmd_func)(cmd_idx, args, ctrl_P);
        }
    } /* else */

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    if(ctrl_P->CMenu.length!=0)
    {
        CLI_API_Set_Print_Interactive_Mode(TRUE);
    }
#else
    CLI_API_Set_Print_Interactive_Mode(TRUE);
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    return err_code;
}


/************************************************************************/
/* CLI_MAIN_ProcessTABkey                                               */
/*                                                                      */
/* Description:                                                         */
/*     Process TAB key                                                  */
/*                                                                      */
/* Arguments:                                                           */
/*     UI8_T  *buff    : Command line buffer                            */
/*     UI16_T  cmd_idx  : Final matched command word index              */
/*     UI16_T  arg_idx  : Final matched argument index                  */
/*     UI16_T str_idx  : The start string index of final matched word   */
/*     UI16_T err      : The error status of this command line          */
/*                                                                      */
/* Return:                                                              */
/*     CLI_NO_ERROR : for cancel showing error message                  */
/************************************************************************/
static UI16_T CLI_MAIN_ProcessTABkey(char *buff, UI16_T cmd_idx, UI16_T arg_idx, UI16_T str_idx, UI16_T err)
{
    char  word_buf[CLI_DEF_MAX_WORDSIZE+1];
    char  min_word[CLI_DEF_MAX_WORDSIZE+1];
    char  min_cmd_buf[CLI_DEF_MAX_WORDSIZE+1];     /* at most 80 chars in a line in console */
    UI16_T buf_len, word_len, min_len, name_len, min_len_arg;
    UI16_T sidx;
    UI16_T cmd_use_idx;
    UI16_T  arg_use_idx;
    UI16_T match_num, cnt;
    UI16_T add_num, add_flag;
    UI16_T i, j;
    BOOL_T arg_return;
    BOOL_T is_ambiguous = FALSE;
    BOOL_T is_colon_data = FALSE;/*for check after unit_colon data*/
    const CMDLIST *ui_cmd_lst;

    ui_cmd_lst = CLI_LIB_Get_Cmd_List();

    /* No whitespace following end command word
     */
    buf_len = strlen(buff);
    if ( (*(buff+buf_len-1) == ' ') || (err == CLI_ERR_CMD_INVALID) || (err == CLI_ERR_CMD_INVALID_RANGE) )
    {
        return CLI_NO_ERROR;
    }

    /*-----------------------------------------------*/
    /* Check if the specified word is the final word */
    /*-----------------------------------------------*/
    sidx = str_idx;
    word_buf[0] = '\0';

    /*
    Special case for unit_colon and colon data , eg boot system 1:o<tab> must add right command.
    pttch 91.11.02

    pttch 2004/7/14 02:11 do not need check cmd_buff. because already check in previous code
                         only check this specify case.
    */

    if ((arg_idx != NULL_ARG)  &&(ui_arg_lst[arg_idx].type == COLON_DATA))
    {
        UI32_T i = sidx-1;
        while((*(buff+i) != ' ') && (*(buff+i) != ':'))
        {
            if (str_idx - i > CLI_DEF_MAX_WORDSIZE)/*protect buffer is not enough*/
                break;
            word_buf[str_idx-i] = *(buff+i);
            i--;
        }
        if (*(buff+i) == ':')
        {
            str_idx = i + 1;
            is_colon_data = TRUE;
        }
    }
    /*normal case pttch 91.11.02*/
    if (is_colon_data == FALSE)
    {
        while ((*(buff+sidx) != ' ') && (*(buff+sidx) != '\0'))
        {
            if (sidx-str_idx > CLI_DEF_MAX_WORDSIZE)/*protect buffer is not enough*/
                break;
            word_buf[sidx-str_idx] = *(buff+sidx);
            sidx++;
        }
    }

    word_len = sidx - str_idx;
    word_buf[word_len] = '\0';

    if (*(buff+sidx) == ' ')
    return CLI_NO_ERROR;

    /*----------------------------------------------------*/
    /* Add the additional characters to complete the word */
    /*----------------------------------------------------*/
    if ((err == CLI_NO_ERROR) || (err == CLI_ERR_CMD_INCOMPLETE))
    {
        if (arg_idx == NULL_ARG)
        {
            name_len = strlen((char *)ui_cmd_lst[cmd_idx].name);
            for (i=word_len; i<name_len; i++)
                *(buff+str_idx+i) = ui_cmd_lst[cmd_idx].name[i];
            *(buff+str_idx+i) = '\0';

            cmd_use_idx = CLI_PARS_GetNeiborCommandIndex(ui_cmd_lst, cmd_idx);
            while (cmd_use_idx != NULL_CMD)                /*check the word is ambiguous to neighbor,like "aa" "aab"*/
            {                                              /*"aa" exactly match but "aab" is ambiguous*/
                CLI_LIB_StrCmp(word_buf, ui_cmd_lst[cmd_use_idx].name, &match_num);
                if(strlen(word_buf) == match_num)
                {
                    is_ambiguous = TRUE;
                    break;
                }

                cmd_use_idx = CLI_PARS_GetNeiborCommandIndex(ui_cmd_lst, cmd_use_idx);
            }

            if (!is_ambiguous)
            {
                strcat((char *)buff, " ");
            }
        }
        else  /* argument */
        {
            if (ui_arg_lst[arg_idx].type != NO_DATA &&
                ui_arg_lst[arg_idx].type != COLON_DATA &&
                ui_arg_lst[arg_idx].type != UNIT_COLON_DATA &&
                ui_arg_lst[arg_idx].type != MODE_DATA)
            {
                return CLI_NO_ERROR;
            }

            if (MODE_DATA == ui_arg_lst[arg_idx].type)
            {
                enum CLI_ACCESS_MODE_E _mode = CLI_ACCESS_MODE_MAX_NUM;
                char * _mode_str = NULL;
                char * _help_msg = NULL;

                CLI_LIB_GetCommandMode(word_buf, &_mode);
                if (TRUE == CLI_LIB_GetCommandModeStringPtr(_mode, &_mode_str, &_help_msg))
                {
                    name_len = strlen(_mode_str);
                    memcpy(&buff[str_idx+word_len], &_mode_str[word_len], name_len-word_len);
                    buff[str_idx+name_len] = '\0';
                }
                else
                {
                    is_ambiguous = TRUE;
                }

            }
            else
            {
                name_len = strlen((char *)ui_arg_lst[arg_idx].name);

                if (ui_arg_lst[arg_idx].type != UNIT_COLON_DATA)/*pttch add,unit_colon_data do not need add char*/
                {
                    for (i=word_len; i<name_len; i++)
                        *(buff+str_idx+i) = ui_arg_lst[arg_idx].name[i];
                    *(buff+str_idx+i) = '\0';
                }

                arg_use_idx = ui_arg_lst[arg_idx].neibor_idx;
                while (arg_use_idx != NULL_ARG)               /*check the word is ambiguous to neighbor,like "aa" "aab"*/
                {                                             /*"aa" exactly match but "aab" is ambiguous*/
                    CLI_LIB_StrCmp(word_buf, ui_arg_lst[arg_use_idx].name, &match_num);
                    if(strlen(word_buf) == match_num && (ui_arg_lst[arg_use_idx].type == NO_DATA ||
                                                 ui_arg_lst[arg_use_idx].type == COLON_DATA))
                    {
                        is_ambiguous = TRUE;
                        break;
                    }

                    arg_use_idx = ui_arg_lst[arg_use_idx].neibor_idx;
                }
            }

            if (!is_ambiguous && ui_arg_lst[arg_idx].type != COLON_DATA  && ui_arg_lst[arg_idx].type != UNIT_COLON_DATA)
            {
                strcat((char *)buff, " ");
            }
        }
    }
    else if (err == CLI_ERR_CMD_AMBIGUOUS)
    {
        min_word[0] = '\0';
        min_len = 0;

        memcpy(min_cmd_buf, word_buf, word_len);
        min_cmd_buf[word_len]='\0';

        /* find cmds */
        {
            /*-------------------------------*/
            /* Find the minimum matched word */
            /*-------------------------------*/
            CLI_LIB_StrCmp(word_buf, ui_cmd_lst[cmd_idx].name, &match_num);

            if (match_num < word_len)
            {
                /* not find matched command, go to find argument */
                if (arg_idx != NULL_ARG)
                {
                    CLI_LIB_Find_Tab_Arg(buff, word_buf, str_idx, arg_idx, word_len, &min_len_arg);
                }
                return CLI_NO_ERROR;
            }

            min_len = match_num;
            while ((ui_cmd_lst[cmd_idx].name[min_len] != '-')
                && (ui_cmd_lst[cmd_idx].name[min_len] != '\0'))
            {
                min_len++;
            }

            for (i=0; i<min_len; i++)
                min_word[i] = ui_cmd_lst[cmd_idx].name[i];
            min_word[i] = '\0';

            /*--------------------------------------*/
            /* Find out those ambiguous words       */
            /* cnt: record the number of match cmds */
            /*--------------------------------------*/
            cnt = 1;
            cmd_use_idx = CLI_PARS_GetNeiborCommandIndex(ui_cmd_lst, cmd_idx);
            while (cmd_use_idx != NULL_CMD)
            {
                CLI_LIB_StrCmp(word_buf, ui_cmd_lst[cmd_use_idx].name, &match_num);
                if (match_num != word_len)
                    break;

                CLI_LIB_StrCmp((char *)min_word, ui_cmd_lst[cmd_use_idx].name, &match_num);
                if (match_num < min_len)
                {
                    min_len = match_num;
                    //return CLI_NO_ERROR; /* pttch, Jun18, 01 */
                }

                cmd_use_idx = CLI_PARS_GetNeiborCommandIndex(ui_cmd_lst, cmd_use_idx);
                cnt++;
            }

            /*-------------------------------------------*/
            /* Find the maximum matched character string */
            /*-------------------------------------------*/
            add_flag = 1;
            while ((ui_cmd_lst[cmd_idx].name[min_len]!='\0') && (add_flag==1))
            {
                /* Capture next want to matched word/characters
                 */
                if (ui_cmd_lst[cmd_idx].name[min_len] == '-')
                    add_num = 1;
                else
                {
                    add_num = 0;
                    while ((ui_cmd_lst[cmd_idx].name[min_len+add_num] != '-')
                        && (ui_cmd_lst[cmd_idx].name[min_len+add_num] != '\0'))
                    {
                        add_num++;
                    }
                }

                /* Check if it is matched in all ambiguous words
                */
                i = 1;
                cmd_use_idx = CLI_PARS_GetNeiborCommandIndex(ui_cmd_lst, cmd_idx);
                while ((i < cnt) && (cmd_use_idx != NULL_CMD))
                {
                    for (j=0; j<add_num; j++)
                    {
                        if (ui_cmd_lst[cmd_use_idx].name[min_len+j]
                            != ui_cmd_lst[cmd_idx].name[min_len+j])
                        {
                            add_flag = 0;
                            break;
                        }
                    }

                    if (add_flag == 0)
                        break;

                    cmd_use_idx = CLI_PARS_GetNeiborCommandIndex(ui_cmd_lst, cmd_use_idx);
                    i++;
                }  /* while */

                if (add_flag == 1)
                    min_len += add_num;
            }  /* while */

            for (i=word_len; i<min_len; i++)
            {
                /* min_cmd_buf: save matched command */
                *(buff+str_idx+i) = min_cmd_buf[i] = ui_cmd_lst[cmd_idx].name[i];
            }
            *(buff+str_idx+i) = '\0';
            min_cmd_buf[i]='\0';
        }

        /* find args */
        if (arg_idx != NULL_ARG)
        {
            arg_return = CLI_LIB_Find_Tab_Arg(buff, word_buf, str_idx, arg_idx, word_len, &min_len_arg);

            if( arg_return == FALSE )  /* not found matched arg */
            {
                return CLI_NO_ERROR;
            }

            if( min_len_arg < min_len )
            {
                min_len = min_len_arg ;
            }

            /* compare matched argument with matched command */
            match_num = CLI_LIB_StrNCmp((buff+str_idx), min_cmd_buf, 0, min_len, 0, min_len);

            for (i=word_len; i<match_num; i++)
                *(buff+str_idx+i) = ui_arg_lst[arg_idx].name[i];

            *(buff+str_idx+i) = '\0';
        } /* if (arg_idx != NULL_ARG) */
    }
    return CLI_NO_ERROR;
}


/************************************************************************/
/* CLI_MAIN_ProcessQuType3                                              */
/*                                                                      */
/* Description:                                                         */
/*     Process 'abbreviated-command-entry?' help feature                */
/*                                                                      */
/* Arguments:                                                           */
/*     I8_T *buff    : Input command line buffer                        */
/*     UI16_T  cmd_eidx : Final matched command word index              */
/*     UI16_T  arg_eidx : Final matched argument index                  */
/*     UI16_T str_idx  : The start string index of final matched word   */
/*     UI16_T err      : The error status of this command line          */
/*                                                                      */
/* Return:                                                              */
/*     CLI_NO_ERROR            => can be processed by this procedure    */
/*     CLI_ERR_CMD_AMBIGUOUS   => the ambiguous status that can not be  */
/*                               processed                              */
/************************************************************************/
static UI16_T CLI_MAIN_ProcessQuType3(char *buff, UI16_T cmd_eidx, UI16_T arg_eidx, UI16_T str_idx, UI16_T err)
{
    /*jingyan zheng  fix defect ES3628BT-FLF-ZZ-00066*/
    char  *words[CLI_DEF_MAX_CMD_NUM];   /* pointer array for showing the matched word */
    char  word_buf[CLI_DEF_MAX_WORDSIZE+1];
    UI16_T word_len;
    UI16_T widx;         /* count for words[] */
    UI16_T sidx;         /* for string index  */
    UI16_T match_num;
    UI16_T i;
    UI16_T idx;          /* for command/argument index */
    char  *name_string;
    char  *found_ptr;
    char  NULL_CHAR = 0;             /* used by a null string pointer to point to a memory with null data */
    const  CMDLIST *ui_cmd_lst;

    ui_cmd_lst = CLI_LIB_Get_Cmd_List();

    if((name_string = calloc(100, sizeof(UI8_T))) == NULL)
    {
        return CLI_NO_ERROR;
    }

    /*jingyan zheng  to fix defect ES3628BT-FLF-ZZ-00066*/
    for (i=0; i<CLI_DEF_MAX_CMD_NUM; i++)
    {
        words[i] = &NULL_CHAR;
    }
    widx = 0;

    if (err == CLI_ERR_CMD_AMBIGUOUS)
    {
        /* Check if ambiguous happened in final word
         */
        sidx = str_idx;
        while ((*(buff+sidx) != ' ') && (*(buff+sidx) != '\0'))
        {
            if (sidx-str_idx > CLI_DEF_MAX_WORDSIZE) /*protect buffer is not enough*/
                break;
            word_buf[sidx-str_idx] = *(buff+sidx);
            sidx++;
        }
        word_len = sidx - str_idx;
        word_buf[word_len] = '\0';

        if (*(buff+sidx) == ' ')
        {
            free(name_string);
            return CLI_ERR_CMD_AMBIGUOUS;
        }

        /*
         * Find the ambiguous command/argument index
         */

        /* get command */
        idx = CLI_PARS_GetCommandIndex(ui_cmd_lst, cmd_eidx);
        while (idx != NULL_CMD)
        {
            CLI_LIB_StrCmp(word_buf, ui_cmd_lst[idx].name, &match_num);
            if (match_num == word_len)
            {
                /*jingyan zheng defect ES3628BT-FLF-ZZ-00066*/
                if(widx >= CLI_DEF_MAX_CMD_NUM)
                    break;
                words[widx++] = ui_cmd_lst[idx].name;
            }
            else
                break;

            idx = CLI_PARS_GetNeiborCommandIndex(ui_cmd_lst, idx);
        }

        /* get argument */
        idx = arg_eidx;
        while (idx != NULL_ARG)
        {
            CLI_LIB_StrCmp(word_buf, ui_arg_lst[idx].name, &match_num);
            if (match_num == word_len)
                words[widx++] = ui_arg_lst[idx].name;
            else
                break;

            idx = ui_arg_lst[idx].neibor_idx;
        }
    }
    else
    {
        /* CLI_NO_ERROR or CLI_ERR_CMD_INCOMPLETE
        */
        if (arg_eidx == NULL_ARG)
        {
            /* although it's exactly matched, maybe it is subset of neighbors
             * ex: <bridge> v.s. <bridge-group>
             */
            UI8_T  name_len = strlen(ui_cmd_lst[cmd_eidx].name);
            UI16_T cmp_idx = CLI_PARS_GetNeiborCommandIndex(ui_cmd_lst, cmd_eidx);
            I16_T  cmp_flag;
            UI16_T match_num;

            words[widx++] = ui_cmd_lst[cmd_eidx].name;

            while(cmp_idx != NULL_CMD)
            {
                cmp_flag = CLI_LIB_StrCmp(ui_cmd_lst[cmd_eidx].name, ui_cmd_lst[cmp_idx].name, &match_num );

                if(match_num == name_len)
                    words[widx++] = ui_cmd_lst[cmp_idx].name;
                else
                    break;

                cmp_idx = CLI_PARS_GetNeiborCommandIndex(ui_cmd_lst, cmp_idx);
            }
        }
        else
        {
            if( err == CLI_ERR_CMD_INCOMPLETE )
            {
                if (ui_arg_lst[arg_eidx].type == PORT_NAME_DATA && ui_arg_lst[arg_eidx].neibor_idx != NULL_ARG)
                {  /*just for port name special case pttch add*/
                    arg_eidx = ui_arg_lst[arg_eidx].neibor_idx;
                }

                switch(ui_arg_lst[arg_eidx].type)
                {
                case WORD_DATA:
                    SYSFUN_Sprintf(name_string, "<%lu-%lu>", ui_arg_lst[arg_eidx].min_range, ui_arg_lst[arg_eidx].max_range);
                    words[widx++] = name_string;
                    break;

                case HEX_DATA:
                    SYSFUN_Sprintf(name_string, "<%lx-%lx>", ui_arg_lst[arg_eidx].min_range, ui_arg_lst[arg_eidx].max_range);
                    words[widx++] = name_string;
                    break;

                case UNIT_COLON_DATA:/*pttch stacking*/
                    if(strchr(buff, (int)':') == NULL)
                    {
                        UI32_T max_unit_num;

                        //STKTPLG_MGR_GetNumberOfUnit(&max_unit_num);
                        max_unit_num = SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;
                        SYSFUN_Sprintf(name_string, " :        unit delimeter");
                    }
                    //else
                    //{
                    //   SYSFUN_Sprintf(name_string, "%lu:", atoi(buff));
                    //}
                    words[widx++] = name_string;
                    break;

                case RANGE_DATA:
                    SYSFUN_Sprintf(name_string, "<%lu-%lu>        The range of value, by using hyphen",ui_arg_lst[arg_eidx].min_range, ui_arg_lst[arg_eidx].max_range);
                    words[widx++] = name_string;
                    break;

                case LIST_HAVE_UNIT_DATA:
                    if(strchr(buff, (int)'/') == NULL)
                        strcpy(name_string, "/        The delimiter used to distinguish unit and port");
                    else
                    {
                        char *ptr = strchr(buff, (int)'/');
                        while(*ptr != ' ')
                        {
                            ptr--;
                        }
                        ptr++;
                        SYSFUN_Sprintf(name_string, "<1-%ld>   The list of the port, by using hyphen and comma",SWCTRL_POM_UIGetUnitPortNumber(atoi((char *)ptr)));
                    }
                    words[widx++] = name_string;
                    break;

                case LIST_NO_UNIT_DATA:
                    SYSFUN_Sprintf(name_string, "<%lu-%lu> The list of the value, by using hyphen and comma",ui_arg_lst[arg_eidx].min_range,ui_arg_lst[arg_eidx].max_range);
                    words[widx++] = name_string;
                    break;

                case COLON_DATA:
                    words[widx++] = ui_arg_lst[arg_eidx].name; //charles, oct 3, 2001
                    break;

                case PORT_DATA:
                    {
                        char    delimiter = 0;
                        I8_T    middle = 0;
                        char   *keep_ptr = 0;
                        char   *s = buff;
                        Token_T tokens;
                        int     i;

                        if (ui_arg_lst[arg_eidx].type == PORT_DATA)
                            middle = '.';
                        else if (ui_arg_lst[arg_eidx].type == PVC_DATA)
                            middle = '/';

                        for (i=0; i<CLI_DEF_MAX_ARGS_NUM; i++)
                        {

                            s = Get_Token1(s, &tokens, &delimiter);

                            if ( tokens.len == 0 || s == 0 )
                                break;
                            else
                                keep_ptr = s;

                        }

                        /* for example: <interface atm 0?> */
                        if(keep_ptr == 0)
                            return CLI_NO_ERROR;
                        if( (found_ptr = strchr(keep_ptr, (int)middle))  == NULL )
                        {
                            name_string[0] = middle;/*for x.x and x , there is unused*/
                            name_string[1] = 0;
                            if(ui_arg_lst[arg_eidx].type == PORT_DATA)
                                strcat(name_string, "   The delimiter between port number and sub-interface");
                            else
                                strcat(name_string, "   The delimiter used to distinguish unit and port");

                            words[widx++] = name_string;
                        }
                        else
                        {
                            /* for example: <interface atm 0.?> */
                            char *firstCln[2]  = {0};
                            char *secondCln[2] = {0};

                            found_ptr = strchr(ui_arg_lst[arg_eidx].name, (int)middle);
                            firstCln[0] = (++found_ptr);

                            found_ptr = strchr(ui_HelpMsg[ui_arg_lst[arg_eidx].help_idx], (int)'#');
                            secondCln[0] = (++found_ptr);

                            CLI_LIB_DisplayMsg(firstCln, secondCln, CLI_DEF_MAX_MSGNUM);

                            free(name_string);
                            return CLI_NO_ERROR;
                        }
                    }
                    break;

                case PVC_DATA:
                    {
                        char    delimiter = 0;
                        I8_T    middle = 0;
                        char   *keep_ptr = 0;
                        char   *s = buff;
                        Token_T tokens;
                        int     i;
                        char    tmp_buf1[16] = {0};
                        char    show_buf[32] = {0};


                        middle = '/';

                        for (i=0; i<CLI_DEF_MAX_ARGS_NUM; i++)
                        {

                            s = Get_Token1(s, &tokens, &delimiter);

                            if ( tokens.len == 0 || s == 0 )
                                break;
                            else
                                keep_ptr = s;

                        }

                        if(keep_ptr == 0)
                            return CLI_NO_ERROR;
                        /* for example: <interface atm 0?> */
                        if( (found_ptr = strchr(keep_ptr, (int)middle))  == NULL )
                        {
                            name_string[0] = (char)middle;/*for x.x and x , there is unused*/
                            name_string[1] = 0;
                            if(ui_arg_lst[arg_eidx].type == PORT_DATA)
                                strcat(name_string, "   The delimiter between port number and sub-interface");
                            else
                                strcat(name_string, "   The delimiter used to distinguish unit and port");

                            words[widx++] = name_string;
                        }
                        else
                        {
                            /* for example: <interface atm 0.?> */
                            char *firstCln[2]  = {0};
                            char *secondCln[2] = {0};
                            char *x_ptr;
                            char *unit_ptr;
                            unit_ptr = found_ptr;/*search unit point*/
                            while(*unit_ptr != ' ')
                            {
                                unit_ptr--;
                            }
                            unit_ptr++;
                            /*name*/
                            found_ptr = strchr(ui_arg_lst[arg_eidx].name, (int)middle); /*pointer of the '/'*/
                            strcpy(show_buf, (found_ptr+1));
                            x_ptr = strchr(show_buf, (int)'x');
                            SYSFUN_Sprintf(tmp_buf1, "%lu>", SWCTRL_POM_UIGetUnitPortNumber(atoi(unit_ptr)));
                            strcpy(x_ptr, tmp_buf1);
                            firstCln[0] = show_buf;

                            /*help message*/
                            found_ptr = strchr(ui_HelpMsg[ui_arg_lst[arg_eidx].help_idx], (int)'#');
                            secondCln[0] = (++found_ptr);

                            CLI_LIB_DisplayMsg(firstCln, secondCln, CLI_DEF_MAX_MSGNUM);

                            free(name_string);
                            return CLI_NO_ERROR;
                        }
                    }
                    break;

                case NO_DATA:
                    {
                        /* although it's exactly matched, maybe it is subset of neighbors
                         * ex: <cc> v.s. <ccm-learning-database>
                         *     <name> v.s. <name-format>
                         */
                        UI8_T  name_len = strlen(ui_arg_lst[arg_eidx].name);
                        UI16_T cmp_idx  = ui_arg_lst[arg_eidx].neibor_idx;
                        I16_T  cmp_flag;
                        UI16_T match_num;

                        words[widx++] = ui_arg_lst[arg_eidx].name;

                        while(cmp_idx != NULL_ARG)
                        {
                            cmp_flag = CLI_LIB_StrCmp(ui_arg_lst[arg_eidx].name, ui_arg_lst[cmp_idx].name, &match_num );

                            if(match_num == name_len)
                                words[widx++] = ui_arg_lst[cmp_idx].name;
                            else
                                break;

                            cmp_idx = ui_arg_lst[cmp_idx].neibor_idx;
                        }
                    }
                    break;

                case MODE_DATA:
                    {
                        enum CLI_ACCESS_MODE_E _mode;
                        const char *_arg = &buff[str_idx];
                        const size_t _arglen = strlen(_arg);

                        for (_mode=0; _mode < CLI_ACCESS_MODE_MAX_NUM; ++_mode)
                        {
                            char * _mode_str = NULL;
                            char * _help_msg = NULL;
                            I16_T  _cmp_flag;
                            UI16_T _match_num;

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
                            if (NORMAL_EXEC_MODE == _mode)
                            {
                                continue;
                            }
#endif
                            if (FALSE == CLI_LIB_GetCommandModeStringPtr(_mode, &_mode_str, &_help_msg))
                            {
                                continue;
                            }

                            _cmp_flag = CLI_LIB_StrCmp(_arg, _mode_str, &_match_num );
                            if (0 == _cmp_flag || (_cmp_flag < 0 && _match_num == _arglen))
                            {
                                words[widx++] = _mode_str;
                            }
                        }

                        break;
                    }

                default:
                    {
                        const char *_arg = &buff[str_idx];
                        const size_t _arglen = strlen(_arg);
                        int ret;

                        ret = CLI_PARS_GetArgumentHelpMessage(_arg, _arglen,
                                                              ui_arg_lst, arg_eidx,
                                                              name_string, 100,
                                                              NULL, 0);
                        if (0 <= ret)
                        {
                            words[widx++] = name_string;
                            break;
                        }
                    }

                    words[widx++] = ui_arg_lst[arg_eidx].name;
                    break;
                }
            }
            else
            {
                /*exactly match*/
                idx = arg_eidx;

                switch(ui_arg_lst[idx].type)
                {
                case WORD_DATA:
                    SYSFUN_Sprintf(name_string, "<%lu-%lu>", ui_arg_lst[arg_eidx].min_range, ui_arg_lst[arg_eidx].max_range);
                    words[widx++] = name_string;
                    break;

                case HEX_DATA:
                    SYSFUN_Sprintf(name_string, "<%lx-%lx>", ui_arg_lst[arg_eidx].min_range, ui_arg_lst[arg_eidx].max_range);
                    words[widx++] = name_string;
                    break;

                case UNIT_COLON_DATA:/*pttch stacking*/
                    SYSFUN_Sprintf(name_string, "%u:", atoi(buff));
                    words[widx++] = name_string;
                    break;

                case RANGE_DATA:
                    SYSFUN_Sprintf(name_string, "<%lu-%lu>        The range of value, by using hyphen",ui_arg_lst[arg_eidx].min_range, ui_arg_lst[arg_eidx].max_range);
                    words[widx++] = name_string;
                    break;

                case LIST_HAVE_UNIT_DATA:
                    {
                        char *ptr = strchr(buff, (int)'/');/*search unit*/
                        while(*ptr != ' ')
                        {
                            ptr--;
                        }
                        ptr++;
                        SYSFUN_Sprintf(name_string, "<1-%lu>     The list of the port, by using hyphen and comma", SWCTRL_POM_UIGetUnitPortNumber(atoi((char *)ptr)));
                        words[widx++] = name_string;
                    }
                    break;

                case LIST_NO_UNIT_DATA:
                    SYSFUN_Sprintf(name_string, "<%lu-%lu> The list of the value, by using hyphen and comma",ui_arg_lst[arg_eidx].min_range,ui_arg_lst[arg_eidx].max_range);
                    words[widx++] = name_string;
                    break;

                case PORT_DATA:
                    {
                        I8_T middle = 0;

                        middle = '.';

                        if ( (strchr(buff, (int)middle)) == NULL)
                        {
                            name_string[0] = (char)middle;  /*can use x.x or only x*/
                            strcat(name_string, "   <cr>");
                            words[widx++] = name_string;
                        }
                        else
                        {
                            found_ptr = strchr(ui_arg_lst[arg_eidx].name, (int)middle);
                            words[widx++] = ++found_ptr;
                        }
                    }
                    break;

                case PVC_DATA:
                    {
                        I8_T middle = 0;

                        middle = '/';

                        if ( (strchr(buff, (int)middle)) == NULL)
                        {
                            name_string[0] = (char)middle;  /*can use x.x or only x*/
                            strcat(name_string, "   <cr>");
                            words[widx++] = name_string;
                        }
                        else
                        {
                            char  *x_ptr;
                            char tmp_buf[16] = {0};
                            char *unit_ptr;
                            unit_ptr = strchr(buff, (int)middle);/*search unit point*/
                            while(*unit_ptr != ' ')
                            {
                                unit_ptr--;
                            }
                            unit_ptr++;
                            found_ptr = strchr(ui_arg_lst[arg_eidx].name, (int)middle);
                            strcpy(name_string, (++found_ptr));

                            x_ptr = strchr(name_string, (int)'x');
                            SYSFUN_Sprintf(tmp_buf, "%lu>", SWCTRL_POM_UIGetUnitPortNumber(atoi(unit_ptr)));
                            strcpy(x_ptr, tmp_buf);

                            words[widx++] = name_string;
                        }
                    }
                    break;

                case LINE_DATA:
                case LIST_DATA:
                    strcpy(name_string, ui_arg_lst[idx].name);
                    strcat(name_string, "    <cr>");
                    words[widx++] = name_string;
                    break;

                case MODE_DATA:
                {
                    enum CLI_ACCESS_MODE_E _mode;
                    const char *_arg = &buff[str_idx];
                    const size_t _arglen = strlen(_arg);

                    for (_mode=0; _mode < CLI_ACCESS_MODE_MAX_NUM; ++_mode)
                    {
                        char * _mode_str = NULL;
                        char * _help_msg = NULL;
                        I16_T  _cmp_flag;
                        UI16_T _match_num;

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
                        if (NORMAL_EXEC_MODE == _mode)
                        {
                            continue;
                        }
#endif
                        if (FALSE == CLI_LIB_GetCommandModeStringPtr(_mode, &_mode_str, &_help_msg))
                        {
                            continue;
                        }

                        _cmp_flag = CLI_LIB_StrCmp(_arg, _mode_str, &_match_num );
                        if (0 == _cmp_flag || (_cmp_flag < 0 && _match_num == _arglen))
                        {
                            words[widx++] = _mode_str;
                        }
                    }

                    break;
                }


                default:
                    {
                        const char *_arg = &buff[str_idx];
                        const size_t _arglen = strlen(_arg);
                        int ret;

                        ret = CLI_PARS_GetArgumentHelpMessage(_arg, _arglen,
                                                              ui_arg_lst, arg_eidx,
                                                              name_string, 100,
                                                              NULL, 0);
                        if (0 <= ret)
                        {
                            words[widx++] = name_string;
                            break;
                        }
                    }

                    words[widx++] = ui_arg_lst[idx].name;
                    break;
                }

                /* pttch
                 */
                idx = (UI16_T)ui_arg_lst[idx].neibor_idx;
                sidx = str_idx;

                while ((*(buff+sidx) != ' ') && (*(buff+sidx) != '\0'))
                {           /*ome max command string is 25, but arg may excreed this number*/
                    if (sidx-str_idx > CLI_DEF_MAX_WORDSIZE) /*protect buffer is not enough*/
                        break;
                    word_buf[sidx-str_idx] = *(buff+sidx);
                    sidx++;
                }

                while (idx != NULL_ARG)   /* check the word is ambiguous to neighbor,like "aa" "aab" */
                {                         /* "aa" exactly match but "aab" is ambiguous               */
                    CLI_LIB_StrCmp(word_buf, ui_arg_lst[idx].name, &match_num);
                    if(strlen(word_buf)==match_num)
                    {
                        words[widx++] = ui_arg_lst[idx].name;
                    }
                    idx = ui_arg_lst[idx].neibor_idx;
                }
            }
        }
    }

    CLI_LIB_ShowQuType3Msg(words, widx);
    free(name_string);
    return CLI_NO_ERROR;
}


/************************************************************************/
/* CLI_MAIN_ProcessQuMark                                               */
/*                                                                      */
/* Description:                                                         */
/*     Process '?' help feature                                         */
/*                                                                      */
/* Arguments:                                                           */
/*     I8_T *buff    : Input command line buffer                        */
/*     UI16_T  cmd_eidx : Final matched command word index              */
/*     UI16_T  arg_eidx : Final matched argument index                  */
/*     UI16_T str_idx  : The start string index of final matched word   */
/*     UI8_T  err      : The error status of this command line          */
/*                                                                      */
/*    CLI_TASK_WorkingArea_T *ctrl_P: working space                     */
/* Return:                                                              */
/*     CLI_NO_ERROR            => can be processed by this procedure    */
/*     CLI_ERR_CMD_AMBIGUOUS   => the ambiguous status that can not be  */
/*                               processed                              */
/*     CLI_ERR_CMD_UNRECOGNIZE => if invalid command line               */
/************************************************************************/
static UI16_T CLI_MAIN_ProcessQuMark(char *buff, UI16_T cmd_eidx, UI16_T arg_eidx,
                                     UI16_T str_idx, UI16_T err, CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI16_T buf_len;
    UI16_T cmd_idx, arg_idx;
    UI16_T final;
    UI32_T arg_check, arg_legal = 0;
    UI16_T error_index;
    char *temp_buf,*temp_buf_p;
    const  CMDLIST *ui_cmd_lst;

    assert(ctrl_P != NULL);

    ui_cmd_lst = CLI_LIB_Get_Cmd_List();

    if ((err == CLI_ERR_CMD_INVALID) ||
        (err == CLI_ERR_CMD_INVALID_RANGE) ||
        (err == CLI_ERR_CMD_INVALID_RANGE_PLUS))
    {
        return CLI_ERR_CMD_UNRECOGNIZED;
    }

    final = 0;

    if (cmd_eidx == NULL_CMD)
    {
        /* (1) '?' command
         */
        cmd_idx = 0;
        arg_idx = NULL_ARG;

        switch(ctrl_P->CMenu.AccMode)
        {
            case NORMAL_EXEC_MODE:
            case PRIVILEGE_EXEC_MODE:
            default:
                CLI_LIB_PrintStr("EXEC commands:\r\n");
                break;

            case PRIVILEGE_CFG_GLOBAL_MODE:
            case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
            case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
            case PRIVILEGE_CFG_INTERFACE_ETH0_MODE:   /*wenfang*/
#endif
#if (SYS_CPNT_CRAFT_PORT == TRUE)
            case PRIVILEGE_CFG_INTERFACE_CRAFT_MODE:
#endif
#if (SYS_CPNT_FILTER_THROOTTLE == TRUE)
            case PRIVILEGE_CFG_IGMP_PROFILE_MODE:
#endif
            case PRIVILEGE_CFG_INTERFACE_VLAN_MODE:
#if (SYS_CPNT_IP_TUNNEL == TRUE)
            case PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE:
#endif /*(SYS_CPNT_IP_TUNNEL == TRUE)*/
            case PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE:
            case PRIVILEGE_CFG_VLAN_DATABASE_MODE:
            case PRIVILEGE_CFG_LINE_MODE:
            case PRIVILEGE_CFG_DOMAIN_MODE:
            case PRIVILEGE_CFG_ROUTER_MODE:
            case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
            case PRIVILEGE_CFG_ROUTEROSPF_MODE:
            case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
            case PRIVILEGE_CFG_DHCPPOOL_MODE:
#if (SYS_CPNT_BGP == TRUE)
      case PRIVILEGE_CFG_ROUTER_BGP_MODE:
#endif
#if (SYS_CPNT_NSM_POLICY == TRUE)
      case PRIVILEGE_CFG_ROUTE_MAP_MODE:
#endif
#if (SYS_CPNT_ACL == TRUE)
            case PRIVILEGE_CFG_ACL_MAC_MODE:
            case PRIVILEGE_CFG_ACL_STANDARD_MODE:
            case PRIVILEGE_CFG_ACL_EXTENDED_MODE:
            case PRIVILEGE_CFG_ACL_IP_MASK_MODE:
            case PRIVILEGE_CFG_ACL_MAC_MASK_MODE:
#if (SYS_CPNT_DAI == TRUE)
            case PRIVILEGE_CFG_ACL_ARP_MODE:
#endif
#if (SYS_CPNT_ACL_IPV6 == TRUE)
            case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
            case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
#endif
#endif /* SYS_CPNT_ACL */
#if (SYS_CPNT_AAA == TRUE)
            case PRIVILEGE_CFG_AAA_SG_MODE:
            case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
            case PRIVILEGE_CFG_TIME_RANGE_MODE:
#endif

#if (SYS_CPNT_ERPS == TRUE)
            case PRIVILEGE_CFG_ERPS_MODE:
#endif

                CLI_LIB_PrintStr("Configure commands:\r\n");
                break;
#if (SYS_CPNT_VDSL == TRUE)
            case DEBUG_MODE:
                CLI_LIB_PrintStr("Debug commands:\r\n");
                break;
#endif
        }
        CLI_LIB_ShowQuHelp(cmd_idx, arg_idx, final);
    }
    else
    {
        buf_len = strlen((char *)buff);

        if((arg_eidx != NULL_ARG) && (ui_arg_lst[arg_eidx].type == CHAR_DATA))
        {
            /* arg starts with '"' */
            if(*(buff+str_idx)=='"')
            {
                temp_buf_p = (buff+buf_len);

                /* arg ends with ' ' */
                if(*(temp_buf_p-1) == ' ')
                {
                    /* temp_buf does not include ' ' of the end */
                    for(arg_check=2; arg_check<(buf_len-str_idx); arg_check++)
                    {
                        if(*(temp_buf_p-arg_check) != ' ')
                        {
                            break;
                        }
                    }

                    temp_buf = (buff+str_idx);
                    temp_buf[buf_len-arg_check-str_idx+1]= '\0';

                    if(CLI_LIB_Check_Is_Legal_CharString(temp_buf, &error_index)!= TRUE)
                    {
                        /* (3) As 'show pr?' command
                         */
                        err = CLI_MAIN_ProcessQuType3(buff, cmd_eidx, arg_eidx, str_idx, err);

                        if (err > 0)
                        {
                            return err;
                        }
                        return CLI_NO_ERROR;
                    }
                    else
                    {
                        arg_legal = 1 ;
                    }
                }
                else
                {
                    /* (3) As 'show pr?' command
                     */
                    err = CLI_MAIN_ProcessQuType3(buff, cmd_eidx, arg_eidx, str_idx, err);

                    if (err > 0)
                    {
                        return err;
                    }
                    return CLI_NO_ERROR;
                }

            } /* end if(*(buff+str_idx)=='"') */
        }  /* end if(ui_arg_lst[arg_eidx].type == CHAR_DATA) */

        if ( (*(buff+buf_len-1) == ' ') || (arg_eidx != NULL_ARG && ui_arg_lst[arg_eidx].type == COLON_DATA && *(buff+buf_len-1) == ':')
            || (arg_eidx != NULL_ARG && ui_arg_lst[arg_eidx].type == UNIT_COLON_DATA && *(buff+buf_len-1) == ':') || (arg_legal == 1) )
        {
            /* (2) As 'show ?' command
             */
            if (err == CLI_ERR_CMD_AMBIGUOUS || CLI_ERR_CMD_ARG_AMBIGUOUS == err)
            {
                return err;
            }

            if (err == CLI_NO_ERROR)
            {
                final = 1;
            }

            if (arg_eidx != NULL_ARG)
            {
                cmd_idx = NULL_CMD;
                arg_idx = ui_arg_lst[arg_eidx].next_idx;
            }
            else if (arg_eidx == NULL_ARG)
            {
                cmd_idx = ui_cmd_lst[cmd_eidx].child_idx;
                arg_idx = ui_cmd_lst[cmd_eidx].arg_idx;
            }

            if (final && (arg_eidx != NULL_ARG && (ui_arg_lst[arg_eidx].type == LINE_DATA ||
                                                   ui_arg_lst[arg_eidx].type == LIST_DATA )) )
            {
                CLI_LIB_PrintStr(ui_arg_lst[arg_eidx].name);
                CLI_LIB_PrintStr("    <cr>\r\n");
            }
            else
            {
                CLI_LIB_ShowQuHelp(cmd_idx, arg_idx, final);
            }
        }
        else
        {
            /* (3) As 'show pr?' command
             */
            /* if type!= CLI_ERR_CMD_ARG_AMBIGUOUS, it means the cmd isn't ambiguous between cmd ans arg.
             * So, if arg_eidx!= NULL_ARG, we need to end the cmd_eidx to protect it from getting cmds.
             */
            if( (CLI_ERR_CMD_ARG_AMBIGUOUS != err) && (arg_eidx != NULL_ARG) )
            {
                cmd_eidx=NULL_CMD;
            }
            err = CLI_MAIN_ProcessQuType3(buff, cmd_eidx, arg_eidx, str_idx, err);

            if (err > 0)
                return err;
        }
    }

    return CLI_NO_ERROR;
}


/********************************************************************************************************************/
/******************************************************************************/
/* CLI_MAIN_ProcessArgList                                                    */
/*                                                                            */
/* Description:                                                               */
/*     Process argument list in command line buffer                           */
/*                                                                            */
/* INPUT:                                                                     */
/*     I8_T    *buff        : Argument buffer break from command line (end)   */
/*     UI16_T   cmd_idx      : Final command word index leading this arg      */
/* OUTPUT:                                                                    */
/*     UI8_T   *arg_ret_idx : Final touched argument index                    */
/*             (out->ret_aidx)                                                */
/*                                                                            */
/*     I8_T    *tokens[]    : Pointer to each separate argument field         */
/*                                                                            */
/*     UI16_T  *str_idx     : The starting string index of final touch word   */
/*             (out->err_idx) With invalid, it means the invalid str index    */
/*                                                                            */
/* Return:                                                                    */
/*     CLI_NO_ERROR           => complete command line                        */
/*     CLI_ERR_CMD_INCOMPLETE => incomplete argument tokens                   */
/*     !!! CLI_ERR_CMD_AMBIGUOUS  => ambiguous argument !!!                   */
/*     CLI_ERR_CMD_INVALID    => invalid argument word                        */
/*     CLI_ERR_CMD_INVALID_RANGE                                              */
/******************************************************************************/
/* ry note: ret_arg_idx, UI8_T -> UI16_T
 */

static UI16_T CLI_MAIN_ProcessArgList(char *buff, UI16_T cmd_idx, Arg_Ref_T *out)   // modified by charles
{
    Token_T tokens[CLI_DEF_MAX_ARGS_NUM+1];
    UI16_T  token_num;         /* got token number */
    UI16_T  aidx;
    UI8_T   RetVal;
    const   CMDLIST *ui_cmd_lst;
    char    *dup_buff;

    ui_cmd_lst = CLI_LIB_Get_Cmd_List();

    /* Check No-Argument case */
    if (ui_cmd_lst[cmd_idx].max_args <= 0 )
    {
        unsigned int i;
        unsigned int buf_len = strlen((char *)buff);

        for (i=0; i< buf_len; i++, buff++)
        {
            if ( *buff != ' ')
            {
                out->err_ptr = buff;
                return CLI_ERR_CMD_INVALID;
            }
        }
        return CLI_NO_ERROR;
    }

    memset ( tokens, 0, sizeof(tokens) );
    memset ( out->args, 0, sizeof(out->args) );
    memset ( out->args_full, 0, sizeof(out->args_full) );

    token_num = 0;

    out->ret_aidx = NULL_ARG;      // modified by charles
    out->err_ptr = 0;

    aidx = ui_cmd_lst[cmd_idx].arg_idx;


    dup_buff = strdup(buff);

    /* Fill in tokens */
    token_num = Get_Tokens_Without_Delimit_By_Colon(buff , tokens, CLI_DEF_MAX_ARGS_NUM);

    arg_position = 0;
    RetVal = Parse_Arguments (tokens, aidx, token_num, NO_PRE_MATCH, out);

    /* Re-try to parse arguments with delimiting token by ':'.
     *
     * It should not include err_kind == CLI_ERR_CMD_INCOMPLETE.
     * Because CLI_ERR_CMD_INCOMPLETE means all input string are OK, just miss
     * some tokens at tail.
     *
     * To do this for UNIT_COLON_DATA and COLON_DATA.
     * These two data type need delimit a token by ':'.
     */

    if ((FALSE == RetVal && out->err_kind != CLI_ERR_CMD_INCOMPLETE) &&
        (NULL != dup_buff && NULL != strchr(dup_buff, ':')) )
    {
        memset ( tokens, 0, sizeof(tokens) );
        memset ( out->args, 0, sizeof(out->args) );
        memset ( out->args_full, 0, sizeof(out->args_full) );

        token_num = 0;

        out->ret_aidx = NULL_ARG;
        out->err_ptr = 0;
        out->err_kind = CLI_NO_ERROR;

        strcpy(buff, dup_buff);
        token_num = Get_Tokens(buff , tokens, CLI_DEF_MAX_ARGS_NUM);

        arg_position = 0;
        RetVal = Parse_Arguments (tokens, aidx, token_num, NO_PRE_MATCH, out);
    }

    free(dup_buff);
    dup_buff = NULL;

    if (out->err_ptr == 0)
        out->err_ptr = tokens[token_num-1].p;  //charles, april 13

    switch (RetVal)
    {
        case TRUE:
            return CLI_NO_ERROR;

        default:
            return out->err_kind;
    }
}



/* return: TRUE: MATCH, FALSE:NO_MATCH */
/******************************************************************************/
/* Return:                                                                    */
/*     CLI_NO_ERROR           => complete command line                        */
/*     CLI_ERR_CMD_INCOMPLETE => incomplete argument tokens                   */
/*     CLI_ERR_CMD_AMBIGUOUS  => ambiguous argument                           */
/*     CLI_ERR_CMD_INVALID    => invalid argument word                        */
/******************************************************************************/

static BOOL_T Parse_Arguments( Token_T *tokens, UI16_T aidx, UI16_T token_num, I16_T pre_match_len, Arg_Ref_T *out)
{
#define MATCH_VALUE(x,y,z) ( x*4 + y*2 + z )

#define Match_NoNessary_NoPreMatch     MATCH_VALUE(0,0,0)
#define Match_NoNessary_PreMatch       MATCH_VALUE(0,0,1)
#define Match_Nessary_NoPreMatch       MATCH_VALUE(0,1,0)
#define Match_Nessary_PreMatch         MATCH_VALUE(0,1,1)

#define SubMatch_NoNessary_NoPreMatch  MATCH_VALUE(1,0,0)
#define SubMatch_NoNessary_PreMatch    MATCH_VALUE(1,0,1)
#define SubMatch_Nessary_NoPreMatch    MATCH_VALUE(1,1,0)
#define SubMatch_Nessary_PreMatch      MATCH_VALUE(1,1,1)

#define NoMatch_NoNessary_NoPreMatch   MATCH_VALUE(2,0,0)
#define NoMatch_NoNessary_PreMatch     MATCH_VALUE(2,0,1)
#define NoMatch_Nessary_NoPreMatch     MATCH_VALUE(2,1,0)
#define NoMatch_Nessary_PreMatch       MATCH_VALUE(2,1,1)

    I16_T    match_len;
    I16_T    max_match_len = NO_PRE_MATCH;
    int      ret_value;
    BOOL_T   match_neighbor;
#if (CLI_SUPPORT_QUOTE_STRING_FEATURE == TRUE)
    BOOL_T   quot_flag = FALSE;
    char     buf[CLI_DEF_MAX_BUFSIZE+1];
#endif
    while(1)
    {
        int case_value;

        if (aidx == NULL_ARG)
        {
            if(token_num==0)
                return TRUE;
            else
                return FALSE;
        }

        /*      if (ui_arg_lst[aidx].type == BYPASS_DATA)
         return TRUE;*/


        /* get LINE_DATA */
        if (ui_arg_lst[aidx].type == LINE_DATA ||
            ui_arg_lst[aidx].type == LIST_DATA )
        {
            int    position       = 0;
            BOOL_T have_non_space = FALSE;

            if (tokens->p == NULL)
            {  /*pttch add for checking line data can exist with bypass data 2002/6/6 */
                if (ui_arg_lst[aidx].neibor_idx != NULL_ARG && ui_arg_lst[ui_arg_lst[aidx].neibor_idx].type == BYPASS_DATA)
                {
                    out->err_kind = CLI_NO_ERROR;
                    return TRUE;
                }
                else
                {
                    out->err_kind = CLI_ERR_CMD_INCOMPLETE;
                    return FALSE;
                }
            }

            for (position = 0; position<strlen((char *)tokens->p); position++)
            {
                if(!isspace(tokens->p[position]))
                {
                    have_non_space = TRUE;
                    break;
                }
            }

            if(have_non_space)
            {
                token_num = 0;

#define LISTDATA_NO_ERROR      0
#define LISTDATA_INVALID_CHAR  1
#define LISTDATA_INVALID_RANGE 2

                out->args [ /*ui_arg_lst[aidx].pos*/arg_position ] = tokens->p;
                arg_position++;
#if (CLI_SUPPORT_QUOTE_STRING_FEATURE == TRUE)

                if (ui_arg_lst[aidx].type == LINE_DATA)
                {
                    UI32_T pos = --arg_position;
                    int token_length = 0;

                    /* string starts with quote, whatever ends with quote or not */
                    if(CLI_LIB_Is_Str_Enclosed_withQuote(out->args [ pos/*ui_arg_lst[aidx].pos*/ ],buf))
                    {
                        /*jingyan  zheng, defect ES4827G-FLF-ZZ-00497*/
                        CLI_LIB_Str_Remove_Quote(out->args [ pos ]);
                    }
                    else
                    {
                        CLI_LIB_Str_Remove_Quote_NotQuoteString(out->args [ pos/*ui_arg_lst[aidx].pos*/ ]);
                    }

                    token_length = strlen(out->args [ pos ]);

                    if(token_length > ui_arg_lst[aidx].max_range)
                    {
                        out->err_kind = CLI_ERR_CMD_INVALID_RANGE;
                        aidx = NULL_ARG;
                        return FALSE;
                    }
                }

#endif
                out->ret_aidx = aidx;
                aidx = NULL_ARG;
                out->args_full[arg_position] = out->args[arg_position];
                return TRUE;
            }
            else
            {
                out->err_kind = CLI_ERR_CMD_INCOMPLETE;
                return FALSE;
            }
        }


        if ( token_num == 0 )
        {
            out->err_kind = CLI_ERR_CMD_INCOMPLETE;

            /* check itself */
            if (  ui_arg_lst[aidx].necessary >= 1)
            {
                if ((aidx = ui_arg_lst[aidx].neibor_idx) == NULL_ARG )
                {
                    return FALSE;
                }
                else
                {
                    return Parse_Arguments(tokens, aidx, token_num, max_match_len, out);
                }
            }


            //if (  (ui_arg_lst[aidx].necessary == 0) && (ui_arg_lst[aidx].type == PORT_DATA) )
            //{
            //   return TRUE;
            //}

            if (  (ui_arg_lst[aidx].necessary == 0) && (ui_arg_lst[aidx].type == BYPASS_DATA) )
            {
                return TRUE;
            }


            /* check child */
            if ( Parse_Arguments(tokens, ui_arg_lst[aidx].next_idx, token_num, max_match_len, out) )
            {
                return TRUE;
            }

            /* check neighbor */
            if ((aidx = ui_arg_lst[aidx].neibor_idx) == NULL_ARG)
            {
                return FALSE;
            }
            else
            {
                return Parse_Arguments(tokens, aidx, token_num, max_match_len, out);
            }
        }

#if (CLI_SUPPORT_QUOTE_STRING_FEATURE == TRUE)

        if (ui_arg_lst[aidx].type == CHAR_DATA)
        {
            char temp[CLI_DEF_MAX_BUFSIZE+1] = {0};
            memcpy(temp,tokens->p,tokens->len);

            if(CLI_LIB_Check_Is_QuoteStr(temp))
            {
                CLI_LIB_Str_Remove_Quote(temp);
                tokens->len = strlen((char *)temp);
                memcpy(tokens->p,temp,tokens->len);
                quot_flag = TRUE;
            }
        }
#endif

        /* token_num >0 && aidx != NULL_ARG */
        if  (pre_match_len == NO_PRE_MATCH)
            out->err_ptr = 0;

        /* ASF3528B-F2-FLF-TER-00041
         * the following ARG_PRIVILEGE_XXX are incorrect in NEW codegen tool
         * e.g. ARG_PRIVILEGE_CFG_GLOBAL_RADIUSSERVER_A4_B7_C0
         *      ARG_PRIVILEGE_CFG_GLOBAL_RADIUSSERVER_A4_B5_C1
         *      ARG_PRIVILEGE_CFG_GLOBAL_RADIUSSERVER_A4_B3_C2
         *      ARG_PRIVILEGE_CFG_GLOBAL_RADIUSSERVER_A4_B1_C3
         *      ARG_PRIVILEGE_CFG_GLOBAL_TACACSSERVER_A2B0C0D0E0
         *      ARG_PRIVILEGE_CFG_GLOBAL_TACACSSERVER_A2B1C0
         *
         * CLI do not decode key here and decode in CLI_API_RADIUS
         * CLI_API_TACACS APIs
         */
        ret_value = Check_Arg_By_Data_Type (tokens, aidx, (UI16_T *)&match_len, out);
        if(ret_value != ARG_NOMATCH)
        {
            switch(ui_arg_lst[aidx].type)
            {
                case CHAR_DATA:
                case IP_ADDR_DATA:
                case IPV6_ADDR_DATA:
                case IPV6_PREFIX_DATA:
                case IPV6_ADDR_DATA_WITH_SCOPE_ID:
                case IPV6_LINK_LOCAL_ADDR_DATA_NO_SCOPE_ID:
                case IPV6_ADDR_RAW_DATA:
                case IPV4_ADDR_DATA:
                case IPV4_PREFIX_DATA:
                case MAC_ADDR_DATA:
                case WORD_DATA:
                case PVC_DATA:
                case LIST_NO_UNIT_DATA:
                case HEX_DATA:
                case HEX_STR_DATA:
                case UNIT_COLON_DATA:
                case LIST_HAVE_UNIT_DATA:
                case LINE_DATA:
                case MINUS_DATA:
                case BGP_COMMUNITY_DATA:
                    out->args_full[arg_position] = tokens->p;
                    break;
                case NO_DATA:
                case COLON_DATA:
                    out->args_full[arg_position] = ui_arg_lst[aidx].name;
                     break;

                case MODE_DATA:
                {
                    char * _arg = calloc(tokens->len+1, 1);

                    if (_arg)
                    {
                        enum CLI_ACCESS_MODE_E _mode = CLI_ACCESS_MODE_MAX_NUM;
                        char * _mode_str = NULL;
                        char * _help_msg = NULL;

                        memcpy(_arg, tokens->p, tokens->len);
                        _arg[tokens->len] = '\0';

                        CLI_LIB_GetCommandMode(_arg, &_mode);
                        if (TRUE == CLI_LIB_GetCommandModeStringPtr(_mode, &_mode_str, &_help_msg))
                        {
                            out->args_full[arg_position] = _mode_str;
                        }

                        free(_arg);
                    }

                    break;
                }

                default:
                    SYSFUN_Debug_Printf("ui_arg_lst[aidx].type = %d\n", ui_arg_lst[aidx].type);
                    assert(0);
                    break;
            }
        }
        case_value = ret_value * 4 + (ui_arg_lst[aidx].necessary >= 1 ? 2:0) + (pre_match_len!=NO_PRE_MATCH);

#if (CLI_SUPPORT_QUOTE_STRING_FEATURE == TRUE)
        if((quot_flag == TRUE) && (out->err_ptr != 0))
        {
            int i;
            out->err_ptr++;/* add one position for first quotation mark */
            for ( i=0; i< tokens->len; i++)
            {
                if(tokens->p[i] == '"')
                    out->err_ptr++;
            }
        }
#endif

        //max_match_len = ( pre_match_len > match_len ) ? pre_match_len : match_len;
        if (pre_match_len > match_len)
            max_match_len = pre_match_len;
        else
            max_match_len = match_len;

        switch ( case_value )
        {
            case Match_NoNessary_NoPreMatch:  /* 0 */
            case Match_Nessary_NoPreMatch:    /* 2 */
                break;   /* match next */

            case Match_NoNessary_PreMatch:    /* 1 */
            case Match_Nessary_PreMatch:      /* 3 */
                /* this argument is the best match node */
                return TRUE;

            case NoMatch_NoNessary_NoPreMatch:  /* 8 */
            case SubMatch_NoNessary_NoPreMatch: /* 4 */
                /* check child */
                if ( Parse_Arguments(tokens, ui_arg_lst[aidx].next_idx, token_num, max_match_len, out) )
                {
                    /* match next node */
                    aidx = ui_arg_lst[aidx].next_idx;
                    continue;
                }

                /* check neighbor */
                if ( Parse_Arguments(tokens, ui_arg_lst[aidx].neibor_idx, token_num, max_match_len, out) )
                {
                    /* match neighbor node */
                    aidx = ui_arg_lst[aidx].neibor_idx;
                    continue;
                }

                if( out->err_kind == CLI_ERR_CMD_AMBIGUOUS) /*neighbor return FALSE && ambiguous */
                {
                    out->ret_aidx = aidx;
                    return FALSE;
                }

                if ( match_len == 0 )
                {
                    /* error index */
                    out->ret_aidx = aidx; //charles
                    return FALSE;
                }

                break;   /* match current arg-node, continue to match next arg-node */





            case SubMatch_NoNessary_PreMatch:  /* 5 */
            case NoMatch_NoNessary_PreMatch:   /* 9 */
                if ( match_len > pre_match_len )
                {
                    return TRUE;
                }

                /* check child */
                if ( Parse_Arguments(tokens, ui_arg_lst[aidx].next_idx, token_num, max_match_len, out) )
                {
                    return TRUE;
                }

                /* check neighbor */
                if ( Parse_Arguments(tokens, ui_arg_lst[aidx].neibor_idx, token_num, max_match_len, out) )
                {
                    return TRUE;
                }
                else
                {
                    if( match_len == pre_match_len && match_len!=0 )
                    {
                        out->err_kind = CLI_ERR_CMD_AMBIGUOUS;
                    }
                    return FALSE;
                }



            case SubMatch_Nessary_NoPreMatch: /* 6 */
            case NoMatch_Nessary_NoPreMatch:  /* 10 */
                /* check neighbor */
                if ( Parse_Arguments(tokens, ui_arg_lst[aidx].neibor_idx, token_num, max_match_len, out))
                {
                    aidx = ui_arg_lst[aidx].neibor_idx;
                    continue;
                }

                if( out->err_kind == CLI_ERR_CMD_AMBIGUOUS) /*neighbor return FALSE && ambiguous */
                {
                    out->ret_aidx = aidx;
                    return FALSE;
                }

                if ( max_match_len == 0 )
                {
                    /* error */
                    out->ret_aidx = aidx; //charles
                    return FALSE;
                }
                else
                {
                    break;   /* match current arg-node, continue to match next arg-node */
                }

            case SubMatch_Nessary_PreMatch:  /* 7 */
            case NoMatch_Nessary_PreMatch:   /* 11 */
                /* check neighbor */
                match_neighbor = Parse_Arguments(tokens, ui_arg_lst[aidx].neibor_idx, token_num, max_match_len, out);

                /* check ambiguous here */
                if (match_neighbor)
                {
                    return TRUE;
                }
                else
                {
                    if( match_len > pre_match_len)
                    {
                        return TRUE;
                    }
                    else
                    {
                        if( match_len == pre_match_len && match_len!=0 )
                        {
                            out->err_kind = CLI_ERR_CMD_AMBIGUOUS;
                        }
                        return FALSE;
                    }
                }
        }

        /* Match_next */
        if (tokens->p [ tokens->len ] == ' ')
        {
            tokens->p [ tokens->len ] = 0;
        }
        else
        {

            int j;
            j = strlen((char *)(tokens->p + tokens->len) );

            for (; j>=0; j--)
            {
                tokens->p[tokens->len + j + 1] = tokens->p[tokens->len + j];
            }

            tokens->p [ tokens->len ] = 0;
            for (j = 0; j< token_num-1; j++)
            {
                tokens++;
                tokens->p ++;
            }

            for (j = 0; j< token_num-1; j++)
            {
                tokens--;
            }

        }


        out->ret_aidx = aidx;
        out->args [ arg_position/*ui_arg_lst[aidx].pos*/ ] = tokens->p;
        arg_position++;

#if 0/*(CLI_SUPPORT_QUOTE_STRING_FEATURE == TRUE)*/
        if((ui_arg_lst[aidx].type == CHAR_DATA) || (ui_arg_lst[aidx].type == LINE_DATA))
        {
            UI32_T pos = arg_position--;
            if(CLI_LIB_Check_Is_QuoteStr(out->args [ pos/*ui_arg_lst[aidx].pos*/ ]))
            {
                CLI_LIB_Str_Remove_Quote(out->args [ pos/*ui_arg_lst[aidx].pos*/ ]);
            }
        }
#endif
        tokens++;
        aidx = ui_arg_lst[aidx].next_idx;
        token_num--;

        if ( (aidx == NULL_ARG) && (token_num != 0) )
        {
            out->err_ptr = tokens->p ;
            out->err_kind = CLI_ERR_CMD_INVALID;
            return FALSE;
        }
        else
        {
            continue;
        }
    } /* End of while */
} /* Parse_Arguments */

static int Check_Arg_By_Data_Type (Token_T *token, UI16_T aidx, UI16_T *match_len, Arg_Ref_T *out)
{
    int     i = 0;
    int     ret;
    UI16_T  tmp_match_len = 0;
    UI16_T  tmp_err_kind = CLI_NO_ERROR;

#if (SYS_CPNT_IPV6 == TRUE) || (SYS_CPNT_MLDSNP == TRUE) || (SYS_CPNT_ACL_IPV6 == TRUE)
    UI8_T buff[IPV6_ADDREESS_STRING_MAX_LEN+1]={0};
#endif
    *match_len = 0;        /* no match */


    ret = CLI_PARS_ParseArgument(token->p, token->len,
                                 ui_arg_lst, aidx,
                                 &tmp_match_len,
                                 &tmp_err_kind);
    if (0 <= ret)
    {
        if (ret == ARG_SUBMATCH)
        {
            *match_len = tmp_match_len;
            out->err_ptr = token->p + tmp_match_len;
        }
        else if(ret == ARG_NOMATCH)
        {
            out->err_kind = tmp_err_kind;
            out->err_ptr = token->p + tmp_match_len;
        }

        return ret;
    }

    switch (ui_arg_lst[aidx].type)
    {
        case NO_DATA:
        case COLON_DATA:
        {
            int  k;
            char *p  = (char *)token->p;
            char *ap = (char *)ui_arg_lst[aidx].name;

            /* to detect every character in the got token
             */
            for (k=0; k<token->len; p++, ap++, k++)
            {
                if ( tolower(*p) != tolower(*ap) ) /* The character in the got token is different from  */
                {                                  /* that in the token of the node in the parsing tree */
                    if ( out->err_ptr==0  &&  k!=0 )
                    {
                        out->err_ptr = p;
                    }

                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
            }

            /* to realize if the got token exactly match or subset
             * match the the token of the node in the parsing tree.
             */
            if ( *ap != 0 )   /* subset match */
            {
                //if(ui_arg_lst[aidx].type == COLON_DATA)
                //{
                //   if ( out->err_ptr==0)
                //      out->err_ptr = p;

                //   out->err_kind = CLI_ERR_CMD_INVALID;
                //   return ARG_NOMATCH;
                //}
                //else
                //{
                *match_len = token->len;
                out->err_ptr = token->p;
                return ARG_SUBMATCH;
                //}
            }
            else              /* exactly match */
            {
                out->err_ptr =  token->p;
                return ARG_MATCH;
            }
        }

        case CHAR_DATA:       /* This type of data must put in the last position among neighbor */
            if ( token->len > ui_arg_lst[aidx].max_range)
            {
                if (  out->err_ptr == 0  )
                {
                    out->err_ptr = ((token->p) + ui_arg_lst[aidx].max_range);
                }
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }
            else if(token->len < ui_arg_lst[aidx].min_range)
            {
                out->err_kind = CLI_ERR_CMD_INVALID_RANGE;
                return ARG_NOMATCH;
            }
            else
            {
                *match_len = token->len;
                return ARG_MATCH;
            }

        case QUOT_DATA :
        case LINE_DATA :
            return (CLI_LIB_CheckLineData(token->p, match_len) == CLI_NO_ERROR)? TRUE : FALSE;

        case WORD_DATA :
            for ( i=0; i< token->len; i++)
            {
                if ( !isdigit(token->p[i] ) )
                {
                    if ( out->err_ptr == 0 )
                    {
                        out->err_ptr = ((token->p)+i);
                    }
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
            }

            if ( CLI_LIB_AtoUll(token->p) > ui_arg_lst[aidx].max_range ||  CLI_LIB_AtoUll(token->p) < ui_arg_lst[aidx].min_range)
            {
                if ( out->err_ptr == 0 )
                {
                    out->err_ptr = token->p;
                }
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                return ARG_NOMATCH;
            }
            else
            {
                return ARG_MATCH;
            }

        case VOICE_PORT_WORD_DATA:
        {
            UI8_T port_num = 0;

            for ( i=0; i< token->len; i++)
            {
                if ( !isdigit(token->p[i] ) )
                {
                    if ( out->err_ptr == 0 )
                    {
                        out->err_ptr = ((token->p)+i);
                    }
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
            }

            //RS_POTS_Ports_Detected (&port_num);

            if ( atoi((char *)token->p) > port_num ||  atoi((char *)token->p) < ui_arg_lst[aidx].min_range)
            {
                if ( out->err_ptr == 0 )
                {
                    out->err_ptr = token->p;
                }
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                return ARG_NOMATCH;
            }
            else
            {
                return ARG_MATCH;
            }
        }
            break;

        case HEX_DATA:
        {
            UI32_T   value = 0;
            char     token_buf[CLI_DEF_MAX_BUFSIZE] = {0};

            for (i = 0; i<token->len; i++)
            {
                if ( !CLI_LIB_IsLess16(token->p[i]) )
                {
                    if (( out->err_ptr == 0 )&&( i !=0 ))  // i == 0 means no character is match this
                    {
                        out->err_ptr = &(token->p[i]);
                    }
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
            }

            strncpy((char *)token_buf, token->p, token->len);
            value = strtoul(token_buf, (char **)NULL, 16);

            if (value > ui_arg_lst[aidx].max_range || value < ui_arg_lst[aidx].min_range)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                return ARG_NOMATCH;
            }
            else
            {
                return ARG_MATCH;
            }
        }

        case  MAC_ADDR_DATA:
        {
            int     MacAddrMinusCount=0;

            char    *p= token->p;
            char    temp_string [12+5+1];
            /* temporal buffer for converting Mac address
             XXXXXXXXXXXX into XX-XX-XX-XX-XX-XX

             12 -> for 'X'
             5 -> for '-'
             1 -> for '\0'
             */

            for ( i=0; i< token->len; i++)
            {
                if ( !CLI_LIB_IsLess16(token->p[i]) && token->p[i] != '-' )
                {
                    if (( out->err_ptr == 0 )&&( i !=0 ))  // i == 0 means no character is match this
                    {
                        out->err_ptr = &(token->p[i]);
                    }
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }

                if(token->p[i] == '-')
                    MacAddrMinusCount++;

            }

            /* Convert Mac address XXXXXXXXXXXX into XX-XX-XX-XX-XX-XX
             */
            if (MacAddrMinusCount==0)
            {
                int count=0;
                int i=0;

                if(token->len !=12)
                {
                    if ( out->err_ptr == 0 )
                    {
                        out->err_ptr  = (token->p + token->len -1);
                    }
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
                else
                {
                    for(i=0;i<12;i++)
                    {
                        if(token->p[i]=='\b')
                        {
                            if ( out->err_ptr == 0 )
                            {
                                out->err_ptr  = (token->p +i);
                            }
                            out->err_kind = CLI_ERR_CMD_INVALID;
                            return ARG_NOMATCH;
                        }
                        else
                        {
                            if((i%2)==1)
                            {
                                temp_string[count]=token->p[i];
                                if(i!=11)
                                {
                                    count++;
                                    temp_string[count]='-';
                                }
                                else
                                {
                                    break;
                                }
                            }
                            else
                            {
                                temp_string[count]=token->p[i];
                            }
                        }
                        count++;
                    }
                    temp_string[count+1]='\0';
                    p = temp_string;
                }
                MacAddrMinusCount=5;
            } /* end if (MacAddrMinusCount==0) */

            /* Check Mac address XX-XX-XX-XX-XX-XX
             */
            if (MacAddrMinusCount!=0)
            {
                char *p1;
                int  len;
                for ( i=0; i<5; i++)
                {
                    p1 = strstr ( p, "-");

                    if ( p1==NULL)   // (1)p1==NULL => no "-" found
                    {
                        if ( (out->err_ptr == 0) )
                        {
                            out->err_ptr = (p + strlen(p));
                        }
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }

                    if (p1==p)       // (2)p1==p => The string is zero length
                    {
                        if ( (out->err_ptr == 0) && (i !=0) )
                        {
                            out->err_ptr = p;
                        }
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }


                    /* UI32_T CLI_LIB_AtoUl(UI8_T *s, int radix); */
                    if ( CLI_LIB_AtoUl(p, 16) > 255 )               /*!!!*/
                    {
                        if ( (out->err_ptr == 0) && ( i !=0 ) )
                        {
                            out->err_ptr = p ;
                        }

                        out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                        return ARG_NOMATCH;
                    }
                    p=p1+1;
                }

                if ( My_atoi(p, &len, 16) > 255)               /*!!!*/
                {
                    if ( out->err_ptr == 0 )
                    {
                        out->err_ptr  = p;
                    }
                    out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                    return ARG_NOMATCH;
                }

                if ( !CLI_LIB_IsLess16 (*p) )
                {
                    if ( (out->err_ptr == 0) )
                    {
                        out->err_ptr  = p;
                    }
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
                else if (p[len] >' ')
                {
                    if ( (out->err_ptr == 0) )
                    {
                        out->err_ptr  = (p + len);
                    }
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
                else
                {
                    if(MacAddrMinusCount==5)
                    {
                        return ARG_MATCH;
                    }
                    else
                    {
                        out->err_ptr  = (p + len);
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }
                }

            } /* end if (MacAddrMinusCount!=0) */

        } /* case MAC_ADDR_DATA */


        case MINUS_DATA:
        {
            if ( isdigit(token->p[0]) || token->p[0]=='-')
            {
                for ( i=1; i< token->len; i++)
                {
                    if ( !isdigit(token->p[i]) )
                    {
                        if ( out->err_ptr == 0 )
                        {
                            out->err_ptr = ((token->p)+i);
                        }

                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }
                }
            }
            else
            {
                if ( out->err_ptr == 0 )
                {
                    out->err_ptr = (token->p);
                }
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            if ( token->p[0] == '-' )
            {
                //printf("atoi(token->p + 1): %d\n", atoi(token->p + 1));
                //printf("ui_arg_lst[aidx].min_range: %ld\n", ui_arg_lst[aidx].min_range);
                if ( atoi((char *)(token->p + 1)) > ui_arg_lst[aidx].min_range) /* absolute value */
                {
                    out->err_ptr = (token->p);
                    out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                    return ARG_NOMATCH;
                }
            }
            else
            {
                //printf("atoi(token->p): %d\n", atoi(token->p));
                //printf("ui_arg_lst[aidx].max_range: %ld\n", ui_arg_lst[aidx].max_range);
                if (atoi((char *)token->p) > ui_arg_lst[aidx].max_range)
                {
                    out->err_ptr = ((token->p)+i);
                    out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                    return ARG_NOMATCH;
                }
            }
            return ARG_MATCH;
        }
            break;

        case DOT_DATA:
        {
            I8_T    dot_num = 0;
            BOOL_T  dot_meet  = FALSE;
            UI8_T   after_dot = 0;
            UI32_T  value = 0;


            /* to detect every character in the got token
             */
            for (i=0; i<token->len; i++)
            {
                if ( !isdigit(token->p[i]) )
                {
                    if (token->p[i] != '.')
                    {
                        if ( out->err_ptr == 0 )
                            out->err_ptr = ((token->p)+i);

                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }
                    else
                    {
                        dot_num ++;
                        dot_meet = TRUE;

                        if(dot_num >=2)
                        {
                            if ( out->err_ptr == 0 )
                                out->err_ptr = ((token->p)+i);

                            out->err_kind = CLI_ERR_CMD_INVALID;
                            return ARG_NOMATCH;
                        }
                    }
                }
                else if(dot_meet)
                {
                    after_dot ++;

                    if(after_dot > 3 )
                    {
                        if ( out->err_ptr == 0 )
                            out->err_ptr = ((token->p)+i);

                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }
                }
            }

            value = CLI_LIB_Get_DOTDATA_1000Times_Value(token->p, token->len);

            if(value>ui_arg_lst[aidx].max_range || value<ui_arg_lst[aidx].min_range)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                return ARG_NOMATCH;
            }
            return ARG_MATCH;
        }
            break;

        case LIST_HAVE_UNIT_DATA:
        {
            char  *op_ptr = token->p;
            char  *pre_op_ptr;
            int    slash_count = 0;
            char  *slash_ptr;
            char  Token[CLI_DEF_MAX_BUFSIZE] = {0};
            char  delemiters[3] = {0};
            char  *pre_commom_ptr = (char *)0xffffffff;
            char  *commom_ptr     = (char *)0xffffffff;
            char  token_num = 0;
            UI32_T lower_val;
            UI32_T upper_val;
            UI32_T err_idx = NO_INVALID_CHAR_TOKEN;
            UI32_T mid_ptr = 0;
#if (SYS_CPNT_STACKING == TRUE)/*pttch stacking issue*/
            CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();
            UI32_T num_of_unit = 0;

            if( ctrl_P == NULL )
            {
                SYSFUN_Debug_Printf("\r\n%sFailed to get working area\r\n", __FUNCTION__);
                /*jingyan zheng  ES3628BT-FLF-ZZ-00231*/
                return -1;
            }

            if (ctrl_P->sess_type == CLI_TYPE_PROVISION || ctrl_P->sess_type == CLI_TYPE_MODULE_PROVISION)
            {
                num_of_unit = SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;
            }
            else
            {
                //STKTPLG_MGR_GetNumberOfUnit(&num_of_unit);
                num_of_unit = SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;
            }
#endif
            delemiters[0] = ',';
            delemiters[1] = ' ';


            /* check digit ',' '-' '/' */
            for (i=0; i<token->len; i++)
            {
                if(token->p[i] != ' ' && token->p[i] != ',' && token->p[i] != '-' && token->p[i] != '/' && !isdigit(token->p[i]) )
                {
                    out->err_ptr = ((token->p)+i);
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
                else if(token->p[i] == '/') /*check the number of slash*/
                {
                    if(slash_count >= 1)
                    {
                        out->err_ptr= (token->p + i);
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }
                    else
                        mid_ptr = i;
                    slash_count ++;
                }
                else if(token->p[i] == ',') /*check if the commom is the neighbor of common*/
                {
                    commom_ptr = (token->p)+i;

                    if(commom_ptr == pre_commom_ptr + 1)
                    {
                        out->err_ptr= commom_ptr;
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }
                    else
                    {
                        pre_commom_ptr = commom_ptr;
                    }
                }
                //else if(token->p[i] == '-') /*check if the dash is the neighbor of dash*/
                //{
                //   pre_dash_ptr = (token->p)+i;
                //
                //   if(dash_ptr == pre_dash_ptr + 1)
                //   {
                //      out->err_ptr= dash_ptr;
                //      out->err_kind = CLI_ERR_CMD_INVALID;
                //      return ARG_NOMATCH;
                //   }
                //   else
                //   {
                //      pre_dash_ptr = dash_ptr;
                //   }
                //}
            }

            /*the character just after comma must be digit*/
            if(commom_ptr != (char *)0xffffffff && !isdigit(commom_ptr[1]))
            {
                if(isspace(commom_ptr[1])) /*space*/
                {
                    out->err_ptr= commom_ptr;
                }
                else if(commom_ptr[1] == 0)  /*NULL*//*enter or ?*/
                {
                    out->err_kind = CLI_ERR_CMD_INCOMPLETE;
                    return ARG_NOMATCH;
                }
                else
                    out->err_ptr= (commom_ptr + 1);

                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            /*check if the slash exists*/
            if((slash_ptr = strchr(token->p, (int)'/')) == NULL || (slash_ptr - token->p) > token->len)
            {
                char *non_digit_ptr = (char *)0xffffffff;

                for (i=0; i<token->len; i++)
                {
                    if(!isdigit(token->p[i]))
                    {
                        non_digit_ptr = token->p + i;
                        break;
                    }
                }

                if(non_digit_ptr == (char *)0xffffffff) /*no non-digit character => incomplete*/
                {
#if (SYS_CPNT_STACKING == TRUE)/*pttch stacking issue*/
                    if(atoi(token->p) < 1 || atoi(token->p) > num_of_unit)/*pttch , must get max unit number*/
#else
                        if(atoi(token->p) != 1)
#endif
                        {
                            out->err_ptr = (token->p);
                            out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                        }
                        else
                        {
                            out->err_kind = CLI_ERR_CMD_INCOMPLETE;
                        }

                    return ARG_NOMATCH;
                }
                else /*have non-digit character => invalid at the first non-digit*/
                {
                    out->err_ptr= non_digit_ptr;
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
            }
            else if(slash_ptr - token->p == token->len - 1)
            {
                out->err_kind = CLI_ERR_CMD_INCOMPLETE;
                return ARG_NOMATCH;
            }

            /*check if the characters before slash are all digits*/
            for(i=0; i<(slash_ptr - token->p); i++)
            {
                if(!isdigit(token->p[i]))
                {
                    out->err_ptr= (token->p + i);
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
            }

            /*check the unit value*/
            if(!isdigit(token->p[0]))
            {
                out->err_ptr  = token->p;
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

#if (SYS_CPNT_STACKING == TRUE)/*pttch stacking issue*/
            if(atoi((char *)token->p) < 1 || atoi((char *)token->p) > num_of_unit)/*pttch , must get max unit number*/
#else
                if(atoi((char *)token->p) != 1)
#endif
                {
                    out->err_ptr = (token->p);
                    out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                    return ARG_NOMATCH;
                }

            /*check if the character just after slash is digit*/
            if(!isdigit(slash_ptr[1]))
            {
                out->err_ptr = (slash_ptr + 1);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }



            op_ptr    = slash_ptr+1;
            token_num = 0;
            do
            {
                pre_op_ptr = op_ptr;

                memset(Token, 0, CLI_DEF_MAX_BUFSIZE);
                op_ptr =  CLI_LIB_Get_Token(op_ptr, Token, delemiters);

                if(!CLI_LIB_Get_Lower_Upper_Value(Token, &lower_val, &upper_val, &err_idx))
                {
                    if(err_idx == NO_INVALID_CHAR_TOKEN)
                    {
                        out->err_ptr = token->p;
                        out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                        return ARG_NOMATCH;
                    }
                    else
                    {
                        out->err_ptr= (pre_op_ptr + err_idx + (token_num == 0 ? 0:1)); /*????*/
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return LISTDATA_INVALID_CHAR;
                    }
                }
                else
                {
                    UI32_T min_range = 1;
                    UI32_T max_range = SWCTRL_POM_UIGetUnitPortNumber(atoi((char *)token->p));

                    if(lower_val < min_range || upper_val>max_range)
                    {
                        out->err_ptr = (token->p+mid_ptr+1);
                        out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                        return ARG_NOMATCH;
                    }
                }
                token_num ++;
            } while(op_ptr != NULL && !isspace(*op_ptr));

            return ARG_MATCH;
        }
            break;



        case LIST_NO_UNIT_DATA:
        {
            char  *comma_ptr     = (char*)0xffffffff;
            char  *pre_comma_ptr = (char*)0xffffffff;
            UI8_T  token_num = 0;
            char  *op_ptr;
            char  *pre_op_ptr;
            char   delemiters[3] = {0};
            UI32_T lower_val;
            UI32_T upper_val;
            UI32_T err_idx = NO_INVALID_CHAR_TOKEN;
            char   Token[CLI_DEF_MAX_BUFSIZE] = {0};

            delemiters[0] = ',';
            delemiters[1] = ' ';

            for (i=0; i<token->len; i++)
            {
                if(token->p[i] != ' ' && token->p[i] != ',' && token->p[i] != '-' && !isdigit(token->p[i]) )
                {
                    out->err_ptr = ((token->p)+i);
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
                else if(token->p[i] == ',') /*check if the commom is the neighbor of common*/
                {
                    comma_ptr = (token->p)+i;

                    if(comma_ptr == pre_comma_ptr + 1)
                    {
                        out->err_ptr= comma_ptr;
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }
                    else
                    {
                        pre_comma_ptr = comma_ptr;
                    }
                }
            }

            /*the first digit must be digit*/
            if(!isdigit(token->p[0]))
            {
                out->err_ptr= token->p;
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            /*the character just after comma must be digit*/
            if(comma_ptr != (char*)0xffffffff && !isdigit(comma_ptr[1]))
            {
                if(isspace(comma_ptr[1]))   /*space*/
                {
                    out->err_ptr= comma_ptr;
                }
                else if(comma_ptr[1] == 0)  /*NULL*//*enter or ?*/
                {
                    out->err_kind = CLI_ERR_CMD_INCOMPLETE;
                    return ARG_NOMATCH;
                }
                else
                    out->err_ptr= (comma_ptr + 1);

                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            op_ptr = token->p;

            do
            {
                pre_op_ptr = op_ptr;

                memset(Token, 0, CLI_DEF_MAX_BUFSIZE);
                op_ptr =  CLI_LIB_Get_Token(op_ptr, Token, delemiters);

                if(!CLI_LIB_Get_Lower_Upper_Value(Token, &lower_val, &upper_val, &err_idx))
                {
                    if(err_idx == NO_INVALID_CHAR_TOKEN)
                    {
                        out->err_ptr = (token->p);
                        out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                        return ARG_NOMATCH;
                    }
                    else
                    {
                        out->err_ptr= (pre_op_ptr + err_idx + (token_num == 0 ? 0:1)); /*????*/
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return LISTDATA_INVALID_CHAR;
                    }
                }
                else if(lower_val<ui_arg_lst[aidx].min_range || upper_val>ui_arg_lst[aidx].max_range)
                {
                    out->err_ptr = (token->p);
                    out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                    return ARG_NOMATCH;
                }
                token_num ++;
            } while(op_ptr != NULL && !isspace(*op_ptr));

            return ARG_MATCH;
        }
            break;

        case PVC_DATA:
        {
            char  *dor_ptr;
            UI8_T  dot_count = 0;
            char  delimeter = 0;
            UI32_T mid_ptr = 0;
#if (SYS_CPNT_STACKING == TRUE)/*pttch stacking issue*/
            CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();
            UI32_T num_of_unit = 0;

            if( ctrl_P == NULL )
            {
                SYSFUN_Debug_Printf("\r\n%sFailed to get working area\r\n", __FUNCTION__);
                /*jingyan zheng  ES3628BT-FLF-ZZ-00231*/
                return -1;
            }

            if (ctrl_P->sess_type == CLI_TYPE_PROVISION || ctrl_P->sess_type == CLI_TYPE_MODULE_PROVISION)
            {
                num_of_unit = SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;
            }
            else
            {
                //STKTPLG_MGR_GetNumberOfUnit(&num_of_unit);
                num_of_unit = SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;
            }
#endif

            delimeter = '/';

            /* check digit and delimeter */
            if(!isdigit(token->p[0]))
            {
                out->err_ptr= token->p;
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            for ( i=1; i< token->len; i++)
            {
                if (token->p[i] !=delimeter && !isdigit(token->p[i]))
                {
                    out->err_ptr= (token->p+i);
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
                else if(token->p[i] == delimeter)
                {
                    if(dot_count >= 1)
                    {
                        out->err_ptr= (token->p + i);
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }
                    else
                        mid_ptr = i;
                    dot_count ++;

                }
            }

            /* check value: left */
#if (SYS_CPNT_STACKING == TRUE)/*pttch stackng issue*/
            if ( atoi(token->p) > num_of_unit/*(ui_arg_lst[aidx].max_range / 65536 )*/ || atoi(token->p) < 1 /*(ui_arg_lst[aidx].min_range / 65536 )*/)
#else
                if ( atoi(token->p) > (ui_arg_lst[aidx].max_range / 65536 ) || atoi(token->p) < (ui_arg_lst[aidx].min_range / 65536 ))
#endif
                {
                    out->err_ptr= token->p;
                    out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                    return ARG_NOMATCH;
                }

            if((dor_ptr = strchr(token->p, (int)delimeter)) == NULL)
            {
                if( (  ui_arg_lst[aidx].necessary & 0x01   &&   ui_arg_lst[aidx].necessary & (1 << 4) ) ||    /*1 1*/
                    (!(ui_arg_lst[aidx].necessary & 0x01)  && !(ui_arg_lst[aidx].necessary & (1 << 4) ) ) )   /*0 0*/
                {
                    /* check partial necessary */
                    //out->err_kind = CLI_ERR_CMD_INCOMPLETE; //nov 9, 2001

                    /*check the character just after the left*/
                    if(token->p[token->len] == '\0')
                    {
                        out->err_kind = CLI_ERR_CMD_INCOMPLETE;
                        return ARG_NOMATCH;
                    }
                    else
                    {
                        out->err_ptr= (token->p + token->len);
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }

                    return ARG_NOMATCH;
                }
            }
            else
            {
                /* check the character after the delimeter */
                if(dor_ptr[1] == '\0')
                {
                    out->err_kind = CLI_ERR_CMD_INCOMPLETE;
                    return ARG_NOMATCH;
                }
                else if(dor_ptr[1] == ' ')
                {
                    out->err_ptr= (dor_ptr + 1);
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }

                /* check value: right */
                if(atoi((dor_ptr+1))> SWCTRL_POM_UIGetUnitPortNumber(atoi(token->p)) || atoi((dor_ptr+1))< (ui_arg_lst[aidx].min_range & 0x0000FFFF))
                {
                    out->err_ptr= (token->p+mid_ptr+1);
                    out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                    return ARG_NOMATCH;
                }
            }
            return ARG_MATCH;
        }
            break;

        case RANGE_DATA:
        {
            char *dor_ptr;
            UI8_T dot_count = 0;
            char delimeter = 0;

            delimeter = '-';

            /* check digit and delimeter */
            if(!isdigit(token->p[0]))
            {
                out->err_ptr= token->p;
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            for ( i=1; i< token->len; i++)
            {
                if (token->p[i] !=delimeter && !isdigit(token->p[i]))
                {
                    out->err_ptr= token->p;
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
                else if(token->p[i] == delimeter)
                {
                    if(dot_count >= 1)
                    {
                        out->err_ptr= (token->p + i);
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }
                    else
                        dot_count ++;
                }
            }

            /* check value: left */
            if ( atoi(token->p) > (ui_arg_lst[aidx].max_range) || atoi(token->p) < (ui_arg_lst[aidx].min_range))
            {
                out->err_ptr= token->p;
                out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                return ARG_NOMATCH;
            }

            if((dor_ptr = strchr(token->p, (int)delimeter)) == NULL)
            {/*only set one number*/
                return ARG_MATCH;
            }
            else
            {
                /* check the character after the delimeter */
                if(dor_ptr[1] == '\0')
                {
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
                else if(dor_ptr[1] == ' ')
                {
                    out->err_ptr= (dor_ptr + 1);
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }

                /* check value: right */
                if(atoi((dor_ptr+1))> (ui_arg_lst[aidx].max_range) || atoi((dor_ptr+1))< (ui_arg_lst[aidx].min_range))
                {
                    out->err_ptr= token->p;
                    out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                    return ARG_NOMATCH;
                }
            }
            return ARG_MATCH;
        }
            break;

        case PORT_NAME_DATA:/*add for port name, this data must not have digit on the first char */
        {

            if (isdigit(token->p[0]))
            {
                if (  out->err_ptr == 0  )
                {
                    out->err_ptr = ((token->p) + ui_arg_lst[aidx].max_range);
                }
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }
            else if ( token->len > ui_arg_lst[aidx].max_range)
            {
                if (  out->err_ptr == 0  )
                {
                    out->err_ptr = ((token->p) + ui_arg_lst[aidx].max_range);
                }
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }
            else if(token->len < ui_arg_lst[aidx].min_range)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                return ARG_NOMATCH;
            }
            else
            {
                *match_len = token->len;
                return ARG_MATCH;
            }
        }
            break;

        case HEX_STR_DATA:
        {

            for (i = 0;i < token->len ; i++)
            {
                if (!(( token->p[i] <= '9' && token->p[i] >= '0') ||
                      (token->p[i] <= 'F' && token->p[i] >= 'A') ||
                      (token->p[i] <= 'f' && token->p[i] >= 'a') ))
                {
                    if (  out->err_ptr == 0  )
                    {
                        out->err_ptr = ((token->p) + i);
                    }
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
            }

            if ( token->len > ui_arg_lst[aidx].max_range)
            {
                if (  out->err_ptr == 0  )
                {
                    out->err_ptr = ((token->p) + ui_arg_lst[aidx].max_range);
                }
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }
            else if(token->len < ui_arg_lst[aidx].min_range)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                return ARG_NOMATCH;
            }
            else
            {
                *match_len = token->len;
                return ARG_MATCH;
            }
        }
            break;

        case PORT_DATA:
        {
            char *dor_ptr;
            UI8_T dot_count = 0;
            char delimeter = 0;

            /*determine the delimeter is '.' or '/'*/
            //if (ui_arg_lst[aidx].type == PORT_DATA)
            delimeter = '.';
            //else if (ui_arg_lst[aidx].type == PVC_DATA)
            //   delimeter = '/';

            /* check digit and delimeter */
            if(!isdigit(token->p[0]))
            {
                out->err_ptr= token->p;
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            for ( i=1; i< token->len; i++)
            {
                if (token->p[i] !=delimeter && !isdigit(token->p[i]))
                {
                    out->err_ptr= token->p;
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
                else if(token->p[i] == delimeter)
                {
                    if(dot_count >= 1)
                    {
                        out->err_ptr= (token->p + i);
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }
                    else
                        dot_count ++;
                }
            }

            /* check value: left */
            if ( atoi(token->p) > (ui_arg_lst[aidx].max_range / 65536 ) || atoi(token->p) < (ui_arg_lst[aidx].min_range / 65536 ))
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                return ARG_NOMATCH;
            }

            if((dor_ptr = strchr(token->p, (int)delimeter)) == NULL)
            {
                if( (  ui_arg_lst[aidx].necessary & 0x01   &&   ui_arg_lst[aidx].necessary & (1 << 4) ) ||    /*1 1*/
                    (!(ui_arg_lst[aidx].necessary & 0x01)  && !(ui_arg_lst[aidx].necessary & (1 << 4) ) ) )   /*0 0*/
                {
                    /* check partial necessary */
                    //out->err_kind = CLI_ERR_CMD_INCOMPLETE; //nov 9, 2001

                    /*check the character just after the left*/
                    if(token->p[token->len] == '\0')
                    {
                        out->err_kind = CLI_ERR_CMD_INCOMPLETE;
                        return ARG_NOMATCH;
                    }
                    else
                    {
                        out->err_ptr= (token->p + token->len);
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }

                    return ARG_NOMATCH;
                }
            }
            else
            {
                /* check the character after the delimeter */
                if(dor_ptr[1] == '\0')
                {
                    out->err_kind = CLI_ERR_CMD_INCOMPLETE;
                    return ARG_NOMATCH;
                }
                else if(dor_ptr[1] == ' ')
                {
                    out->err_ptr= (dor_ptr + 1);
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }

                /* check value: right */
                if(atoi((dor_ptr+1))> (ui_arg_lst[aidx].max_range & 65535 ) || atoi((dor_ptr+1))< (ui_arg_lst[aidx].min_range & 0x0000FFFF))
                {
                    out->err_ptr = (token->p);
                    out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                    return ARG_NOMATCH;
                }
            }
            return ARG_MATCH;
        }
        case UNIT_COLON_DATA :/*this can not use with char_data*/
        {
            UI32_T max_unit_num = 0;
            BOOL_T is_have_colon = FALSE;

            //STKTPLG_MGR_GetNumberOfUnit(&max_unit_num);
            max_unit_num = SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;
            for ( i=0; i< token->len; i++)
            {
                if ( !isdigit(token->p[i] ) && token->p[i] != ':')/*check data is digit and ':'*/
                {
                    if ( out->err_ptr == 0 )
                    {
                        out->err_ptr = ((token->p)+i);
                    }
                    out->err_kind = CLI_ERR_CMD_INVALID;
                    return ARG_NOMATCH;
                }
                else if (token->p[i] == ':')
                {
                    is_have_colon = TRUE;
                    if (i == 0 || i != token->len - 1)/*':' can not place in first position, only place in last position*/
                    {                  /*because i count from 0, need -1*/
                        if ( out->err_ptr == 0 )
                        {
                            out->err_ptr = ((token->p)+i);
                        }
                        out->err_kind = CLI_ERR_CMD_INVALID;
                        return ARG_NOMATCH;
                    }
                }
            }

            if (!is_have_colon)/*if do not input colon*/
            {
                if ( out->err_ptr == 0 )
                {
                    out->err_ptr = ((token->p)+token->len);
                }
                out->err_kind = CLI_ERR_CMD_INCOMPLETE;
                return ARG_NOMATCH;
            }

            if ( CLI_LIB_AtoUll(token->p) > max_unit_num ||  CLI_LIB_AtoUll(token->p) < 1)
            {
                if ( out->err_ptr == 0 )
                {
                    out->err_ptr = token->p;
                }
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID_RANGE_PLUS;
                return ARG_NOMATCH;
            }
            else
            {
                return ARG_MATCH;
            }
        }
            break;

            /* new define is for ipv4 and ipv6 address type , and
             it is different from previous define only for ipv4 address type */
        case IP_ADDR_DATA :
        {
            if(FALSE==L_IPV4V6_Is_Valid_IP_AddressString(token->p,token->len))
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }
            else
            {
                return ARG_MATCH;
            }
        }
            break;

        case IPV4_ADDR_DATA:
        {
            if(FALSE==L_IPV4V6_Is_Valid_IPv4_AddressString(token->p,token->len))
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }
            else
            {
                return ARG_MATCH;
            }
        }
            break;

#if (SYS_CPNT_IPV6 == TRUE) || (SYS_CPNT_MLDSNP == TRUE) || (SYS_CPNT_ACL_IPV6 == TRUE)
        case IPV6_ADDR_DATA:
        {
            if (IPV6_ADDREESS_STRING_MAX_LEN <= token->len)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            /* this case (IPV6_ADDR_DATA),must not include '%' and '/'
             check '%' and '/' both not exist, because L_IPV4V6_Is_Valid_IPv6_AddressString() not check two characters */
            strncpy((char *)buff, token->p, token->len);
            if( strchr((char *)buff, '%') != NULL)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            if( strchr((char *)buff, '/') != NULL)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            if(FALSE==L_IPV4V6_Is_Valid_IPv6_AddressString(token->p,token->len))
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }
            else
            {
                return ARG_MATCH;
            }
        }
            break;

        case IPV6_ADDR_DATA_WITH_SCOPE_ID:
        {
            if (IPV6_ADDREESS_STRING_MAX_LEN <= token->len)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            /* this case (with scode),must '%' exist.
             check '%' exist, and '/' not exist, because L_IPV4V6_Is_Valid_IPv6_AddressString() not check two characters */
            strncpy((char *)buff, token->p, token->len);
            if( strchr((char *)buff, '%') == NULL)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            if( strchr((char *)buff, '/') != NULL)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            if(FALSE==L_IPV4V6_Is_Valid_IPv6_AddressString(token->p,token->len))
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }
            else
            {
                return ARG_MATCH;
            }

        }
            break;

        case IPV6_PREFIX_DATA:
        {
            if (IPV6_ADDREESS_STRING_MAX_LEN <= token->len)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            /* this case (prefix),must '/' exist.
             check '%' not exist, and '/' exist, because L_IPV4V6_Is_Valid_IPv6_AddressString() not check two characters */
            strncpy((char *)buff, token->p, token->len);
            if( strchr((char *)buff, '%') != NULL)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            if( strchr((char *)buff, '/') == NULL)
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }

            if(FALSE==L_IPV4V6_Is_Valid_IPv6_PrefixString(token->p,token->len))
            {
                out->err_ptr = (token->p);
                out->err_kind = CLI_ERR_CMD_INVALID;
                return ARG_NOMATCH;
            }
            else
            {
                return ARG_MATCH;
            }
        }
            break;

#endif /* #if (SYS_CPNT_IPV6 == TRUE) || (SYS_CPNT_MLDSNP == TRUE) || (SYS_CPNT_ACL_IPV6 == TRUE) */

        case MODE_DATA:
        {
            char * _arg = calloc(token->len+1, 1);

            if (_arg)
            {
                enum CLI_ACCESS_MODE_E _mode;
                int  _partial_match_count = 0;

                memcpy(_arg, token->p, token->len);
                _arg[token->len] = '\0';

                for (_mode=0; _mode < CLI_ACCESS_MODE_MAX_NUM; ++_mode)
                {
                    char * _mode_str = NULL;
                    char * _help_msg = NULL;
                    I16_T  _cmp_flag;
                    UI16_T _match_num;

    #if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
                    if (NORMAL_EXEC_MODE == _mode)
                    {
                        continue;
                    }
    #endif

                    if (FALSE == CLI_LIB_GetCommandModeStringPtr(_mode, &_mode_str, &_help_msg))
                    {
                        continue;
                    }

                    _cmp_flag = CLI_LIB_StrCmp(_arg, _mode_str, &_match_num);

                    if (0 == _cmp_flag)
                    {
                        *match_len = token->len;

                        free(_arg);
                        return ARG_MATCH;
                    }
                    else if (_cmp_flag < 0 && _match_num == token->len)
                    {
                        if (0 < _partial_match_count++)
                        {
                            *match_len = token->len;
                            out->err_ptr  = token->p;
                            out->err_kind = CLI_ERR_CMD_ARG_AMBIGUOUS;

                            free(_arg);
                            return ARG_SUBMATCH;
                        }
                    }
                }

                /* If it has return before here must free _arg first.
                 */
                free(_arg);

                if (1 == _partial_match_count)
                {
                    *match_len = token->len;
                    return ARG_MATCH;
                }
            }

            out->err_kind = CLI_ERR_CMD_INVALID;
            break;
        }

        default:
        case BYPASS_DATA:
            ;
    } /* switch */

    return ARG_NOMATCH; //charles
} /* Check_Arg_By_Data_Type */


static int Get_Tokens (char *s, Token_T *tokens, int max)
{
    int i;
    char delimiter[1] = {0};

    for (i=0; i<max && s; i++, tokens++)
    {
        s = Get_Token1(s, tokens, delimiter);
        if ( tokens->len == 0)
            return i;
    }

    return i;
}

static int Get_Tokens_Without_Delimit_By_Colon (char *s, Token_T *tokens, int max)
{
    int i;
    char delimiter[1] = {0};

    for (i=0; i<max && s; i++, tokens++)
    {
        s = Get_Token1_Without_Delimit(s, tokens, delimiter);
        if ( tokens->len == 0)
            return i;
    }

    return i;
}

/*
 * INPUT:   s
 * OUTPUT   token
 * Return:  The address that after got tring , return 0 if no next-string
 */
static char *Get_Token1 (char *s, Token_T *token, const char *delimeter)
{
    int state = 0;
    int string_flag = 0;
    const char *d;

    token->p = 0;
    token->len = 0;

    for(; *s !=0; s++)
    {
        switch ( state )
        {
            case 0: /* skip while space */
                if ( isspace ((I16_T)*s) || (*s == 0x0a) ||  (*s == 0x0d) )
                {
                    continue;
                }

                for (d=delimeter;  *d != 0 ;  d++) // charles,
                {                                  //
                    if ( *s == *d )                // add for the situation of
                    {                              // token that leading with
                        token->p = s;              // delimeter.
                        token->len = 1;            //
                        s++;                       //
                        return s;                  //
                    }                              //
                }                                  //



                if ( *s == '"' )
                {
                    string_flag = 1;
                }

                token->p = s;
                state = 1;
                break;

            case 1:
                if ( string_flag )
                {
#if (CLI_SUPPORT_QUOTE_STRING_FEATURE == TRUE)
                    if((*s == '"' ) && (*(s+1) == '"' ))
                    {
                        token->len++;
                        s++;
                        break;
                    }
#endif
                    if ( *s =='"' )
                    {
                        token->len++;
                        return ++s;
                    }
                    break;
                }
                else
                {
                    if ( isspace((I16_T)*s))
                    {
                        return s;
                    }

                    if ( *s == ':' )              // Let xxx:
                    {                             // of xxx:yyy
                        s++;                      // as a token
                        token->len++;             //
                        return s;                 //
                    }                             //

                    for (d=delimeter;  *d != 0 ;  d++)
                    {
                        if ( *s == *d )
                        {
                            return s;
                        }
                    }
                }
                break;

        } /* End of switch */
        token->len++;
    } /* End of for */

    return 0;
} /* Get_Token1 */


/*
 * INPUT:   s
 * OUTPUT   token
 * Return:  The address that after got tring , return 0 if no next-string
 */
static char *Get_Token1_Without_Delimit (char *s, Token_T *token, const char *delimeter)
{
    int state = 0;
    int string_flag = 0;
    const char *d;

    token->p = 0;
    token->len = 0;

    for(; *s !=0; s++)
    {
        switch ( state )
        {
            case 0: /* skip while space */
                if ( isspace ((I16_T)*s) || (*s == 0x0a) ||  (*s == 0x0d) )
                {
                    continue;
                }

                for (d=delimeter;  *d != 0 ;  d++) // charles,
                {                                  //
                    if ( *s == *d )                // add for the situation of
                    {                              // token that leading with
                        token->p = s;              // delimeter.
                        token->len = 1;            //
                        s++;                       //
                        return s;                  //
                    }                              //
                }                                  //



                if ( *s == '"' )
                {
                    string_flag = 1;
                }

                token->p = s;
                state = 1;
                break;

            case 1:
                if ( string_flag )
                {
#if (CLI_SUPPORT_QUOTE_STRING_FEATURE == TRUE)
                    if((*s == '"' ) && (*(s+1) == '"' ))
                    {
                        token->len++;
                        s++;
                        break;
                    }
#endif
                    if ( *s =='"' )
                    {
                        token->len++;
                        return ++s;
                    }
                    break;
                }
                else
                {
                    if ( isspace((I16_T)*s))
                    {
                        return s;
                    }

                    for (d=delimeter;  *d != 0 ;  d++)
                    {
                        if ( *s == *d )
                        {
                            return s;
                        }
                    }
                }
                break;

        } /* End of switch */
        token->len++;
    } /* End of for */

    return 0;
} /* Get_Token1 */


static int My_atoi(char *s, int *digit_len, int radidx)
{
    int i;
    char *p=s;

    switch (radidx)
    {
        case 10:
            for(i=0;;i++, p++)
            {
                if ( !isdigit((I16_T)*p) )
                {
                    *digit_len = i;
                    return atoi(s);
                }
            }
            break;

        case 16:
            for(i=0;;i++, p++)
            {
                if ( !CLI_LIB_IsLess16(*p) )
                {
                    *digit_len = i;
                    return (int)CLI_LIB_AtoUl(s, 16);
                }
            }
            break;

        default:
            return -1;
    }
}


/******************************************************************************/
/* CLI_MAIN_ReadLine_From_XferBuf                                             */
/*                                                                            */
/* Description:                                                               */
/*     Read command line from Xfer buffer and check if config file correct    */
/*                                                                            */
/* INPUT:                                                                     */
/*     char  **xbuf_p     : Xfer buffer                                       */
/*     CLI_TASK_WorkingArea_T *ctrl_P: Task info                              */
/* OUTPUT:                                                                    */
/*     char *cmd_buf : Command from Xfer buffer                               */
/*     char  **xbuf_p: point to next command Xfer buffer                      */
/*                                                                            */
/* Return:                                                                    */
/*     CLI_MAIN_CONFIG_FILE_FAIL  => fail config file                         */
/*     CLI_MAIN_CONFIG_FILE_WRONG => command with wrong length                */
/*     CLI_MAIN_CONFIG_FILE_OK    => correct config file                      */
/******************************************************************************/
static UI32_T CLI_MAIN_ReadLine_From_XferBuf(char *cmd_buf, char **xbuf_p, CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T i;

    assert(ctrl_P != NULL);
    memset(cmd_buf, 0, CLI_DEF_MAX_BUFSIZE);

#if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE)
    if(ctrl_P->partial_provision == TRUE)
    {
        if ( (*xbuf_p - ctrl_P->partial_prov_buffer) >= ctrl_P->partial_prov_length )
        {
            return CLI_MAIN_CONFIG_FILE_FAIL;
        }
    }
    else
#endif  /* #if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE) */
    {
        if ( (*xbuf_p - ctrl_P->prov_buffer) >= ctrl_P->prov_length )
        {
            return CLI_MAIN_CONFIG_FILE_FAIL;
        }
    }

#if (SYS_CPNT_MOTD == TRUE)
    if( (operate_sys_banner_msg_status == TRUE) &&
    (operate_sys_banner_msg_by_session_id == CLI_TASK_GetMySessId()))
    {
        memset(cli_sys_bnr_msg, 0, SYS_ADPT_MAX_MOTD_LENGTH + 1);
        for ( i=0; i<SYS_ADPT_MAX_MOTD_LENGTH; i++ )
        {
            if( *(*xbuf_p+i) == operate_sys_banner_msg_delimiting_character) /*end the input*/
            {
                *xbuf_p = *xbuf_p + 1;
                break;
            }
            else
            {
                cli_sys_bnr_msg[i] = *(*xbuf_p+i);
                CLI_LIB_PrintStr(&cli_sys_bnr_msg[i]);
            }
        }

        cli_sys_bnr_msg[i] = '\0';
        *xbuf_p = *xbuf_p + i;


        if (SYS_BNR_PMGR_SetSysBnrMsgDelimitingChar(SYS_BNR_MGR_MOTD_TYPE, operate_sys_banner_msg_delimiting_character) != TRUE)
        {
            CLI_LIB_PrintStr("\r\nFailed to set system banner message.\r\n");
        }
        else if (SYS_BNR_PMGR_SetSysBnrMsg(SYS_BNR_MGR_MOTD_TYPE, cli_sys_bnr_msg) != TRUE)
        {
            CLI_LIB_PrintStr("\r\nFailed to set system banner message.\r\n");
        }
        else
        {
            CLI_LIB_PrintStr("\r\n");
        }

        operate_sys_banner_msg_status = FALSE;
    }
#endif /*#if (SYS_CPNT_MOTD == TRUE)*/

    for ( i=0; i<CLI_DEF_MAX_BUFSIZE; i++ )
    {
        /*
        Don't copy \n \r \0 into buffer, modified by Jason Yang at 2004-11-02
        */
        if ( *(*xbuf_p+i) == '\n' || *(*xbuf_p+i) == '\r' || *(*xbuf_p+i)=='\0' )
          break;
/*
   pttch 92.04.24 add for check config file is valid or not
*/
       if (*(*xbuf_p+i) > 127)
       {
          return CLI_MAIN_CONFIG_FILE_WRONG;
       }
       *(cmd_buf+i) = *(*xbuf_p+i);
    }

    *(cmd_buf+i) = 0;
    *xbuf_p = *xbuf_p + (i+1);

    return CLI_MAIN_CONFIG_FILE_OK;
}



void CLI_MAIN_Enter_DebugMode()
{
#if 1 /* rich */
   UI8_T i, slength;
   char ch;
   char product_name[MAXSIZE_swProdName+1];
   #if 1 /* rich */
   //UI8_T backDoorStringBCM[] = SYS_ADPT_CLI_BACKDOOR_PASSWORD_BROADCOM;
   char backDoorStringBCM[] = "bcm";
   #endif
   char backDoorStringCLI[] = SYS_ADPT_CLI_BACKDOOR_PASSWORD_COMPONENT;
   char debugStringCLI[] = "vdsl";

   BOOL_T backDoorStatus = FALSE;

   UI16_T ret_val;
   UI8_T key_in_password[30] = {0};

   CLI_TASK_WorkingArea_T *ctrl_P;
   UI32_T sess_id;

   slength = (strlen(debugStringCLI) > strlen(backDoorStringCLI)) ?
              strlen(debugStringCLI) : strlen(backDoorStringCLI);

   for (i=0; i<slength; i++)
   {
      ret_val = CLI_IO_GetKey(&ch);
      key_in_password[i] = ch;

      if ( (ch==backDoorStringCLI[i] && i<strlen((char *)backDoorStringCLI))
         ||(ch==debugStringCLI[i] && i<strlen((char *)debugStringCLI)))
      {
         #if 1 /* rich */
         /*the same as Mercury backdoor password*/
         if (ch != backDoorStringBCM[i] && ch == backDoorStringCLI[i] && ch != debugStringCLI[i])
            slength = strlen((char *)backDoorStringCLI);


         /*the same as debug for vdsl password*/
         if (ch != backDoorStringBCM[i] && ch != backDoorStringCLI[i] && ch == debugStringCLI[i])
            slength = strlen((char *)debugStringCLI);
         #endif

         backDoorStatus = TRUE;
         continue;
      }
      else
      {
         backDoorStatus = FALSE;
         break;
      }
   }

   if (backDoorStatus == TRUE)
   {
      /*match Mercury backdoor password*/
      if (i == strlen((char *)backDoorStringCLI))
      {
         if (!SYS_PMGR_GetProductName(SYS_VAL_LOCAL_UNIT_ID, (UI8_T *)product_name))
         {
            product_name[0] = '\0';
         }

         sess_id = CLI_TASK_GetMySessId();

         CLI_LIB_PrintStr("\r\nEnter ");
         CLI_LIB_PrintStr(product_name);
         CLI_LIB_PrintStr(" debug backdoor\r\n");

         ctrl_P = CLI_TASK_GetWorkingAreaByIndex(sess_id);
         ctrl_P->enter_debug_mode = TRUE;

         BACKDOOR_MGR_Main(
            FALSE,
            SYS_BLD_CLI_GROUP_IPCMSGQ_KEY,
            CLI_IO_DebugMode_GetChar,
            CLI_IO_DebugMode_Print,
            NULL,
            (void *)sess_id);

         CLI_LIB_PrintStr("\r\nExit from ");
         CLI_LIB_PrintStr(product_name);
         CLI_LIB_PrintStr(" debug backdoor\r\n");

         ctrl_P->enter_debug_mode = FALSE;

         ch = 0xD;
      }
      else
      {
#if (SYS_CPNT_VDSL == TRUE)
         USERAUTH_LoginLocal_T login_user;
         CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();

         if( ctrl_P == NULL )
         {
             SYSFUN_Debug_Printf("\r\n%sFailed to get working area\r\n", __FUNCTION__);
            /*jingyan zheng  ES3628BT-FLF-ZZ-00231*/
            return;
         }

         memset(&login_user, 0, sizeof(login_user));
         memcpy (login_user.username,ctrl_P->CMenu.UserName,sizeof(login_user.username));
         if(USERAUTH_PMGR_GetLoginLocalUser(&login_user))
         {   /*must in console and user level 15 can access this vdsl debug mode*/
            if ( ctrl_P->sess_type == CLI_TYPE_UART && login_user.privilege == 15)
               CLI_Debug_Enter();
         }
#endif
      }
   }
#endif /* rich */
}


static void cli_main_parse_file_header(char *token)
{
    char  type_str[30] = {0};
    char  content_str[30] = {0};
    UI32_T len = 0;
    UI32_T i = 0, j = 0, k = 0;
    //CLI_MGR_Set_PrintProvisionMsg(TRUE);
    len = strlen(token);

    for (i = 1; i < len; i++ )
    {
        if (token[i] == '>')
        {
            break;
        }
        if (token[i] == '<')
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
            type_str[j] = token[i];
            j++;
        }
    }

    j = 0;
    if (strcmp(type_str, "mode") == 0)
    {
        for (k = i+1; k < len; k++ )
        {
            if (token[k] == '<')
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
                content_str[j] = token[k];
                j++;
            }
        }
    }

    if (strcmp(content_str,"ERROR_CHECK_OFF") == 0)
    {
        is_continue_provision = TRUE;
    }
    else if (strcmp(content_str,"ERROR_CHECK_ON") == 0)
    {
        is_continue_provision = FALSE;
    }

    k = 0;
    j = 0;
    if (strcmp(type_str, "stackingDB") == 0)
    {
        for (k = i+1; k < len; k++ )
        {
            if (token[k] == '<')
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
                content_str[j] = token[k];
                j++;
            }
        }
    }

    if (strcmp(type_str, "stackingDB") == 0)
    {
        UI32_T  unit_id = 0;
        UI32_T  module_types[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];
        char    module_types_str[4] = {0};

        for(unit_id = 0 ;unit_id <SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ; unit_id++)
        {
            memcpy(module_types_str,content_str + (unit_id*2 ),2);
            module_types[unit_id] = CLI_LIB_AtoUl(module_types_str ,10 );
            //printf("stacking DB: unit = %lu, module type = %lu\n", unit_id, module_types[unit_id]);
        }

        is_continue_provision = TRUE;
    }
    return;
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MAIN_ChangeToGlobalMode
 * ------------------------------------------------------------------------
 * PURPOSE  : Change to global configuration mode
 * INPUT    : ctrl_P - CLI working area pointer
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
static void CLI_MAIN_ChangeToGlobalMode(CLI_TASK_WorkingArea_T *ctrl_P)
{
    ctrl_P->CMenu.AccMode  = PRIVILEGE_CFG_GLOBAL_MODE;

    /* for <interface ethernet x/a-b,c,d-e>
     */
    ctrl_P->CMenu.unit_id = 0;
    memset(ctrl_P->CMenu.port_id_list, 0, sizeof(ctrl_P->CMenu.port_id_list));

    /* for <interface vlan xxx>
     */
    ctrl_P->CMenu.vlan_ifindex = 0;

    /* for <interface port-channel xxx>
     */
    ctrl_P->CMenu.pchannel_id = 0;

    /* for <line xxx>
     */
    ctrl_P->CMenu.line_type = 0;

#if (CLI_SUPPORT_L3_FEATURE == 1)
    /* for dhcp pool name
     */
    memset(ctrl_P->CMenu.Pool_name, 0, sizeof(ctrl_P->CMenu.Pool_name));
#endif

#if (SYS_CPNT_AAA == TRUE)
    /* for AAA Group Server Name
     */
    memset(ctrl_P->CMenu.aaa_sg_name, 0, sizeof(ctrl_P->CMenu.aaa_sg_name));
    ctrl_P->CMenu.aaa_sg_type = 0;
    ctrl_P->CMenu.aaa_sg_group_index = 0;
#endif

} /* CLI_MAIN_ChangeToGlobalMode */

/*------------------------------------------------------------------------|
 * ROUTINE NAME - cli_main_store_module_command                           |
 *------------------------------------------------------------------------|
 * FUNCTION: To save module command from config file to buffer            |
 * INPUT   : cmd_buff      - command line buffer from config file         |
 * OUTPUT  : None                                                         |
 * RETURN  : TRUE/FALSE                                                   |
 * NOTE    : Do not save module config condition                          |
             1.Get "!"                                                    |
             2.Change interface                                           |
 *------------------------------------------------------------------------*/
static BOOL_T cli_main_store_module_command( char * cmd_buff )
{
#if 0 /* rich */
#if ( SYS_CPNT_UNIT_HOT_SWAP == TRUE )
    UI8_T  * ptr;
    UI32_T unit = 0, port = 0, ifindex = 0;
    UI8_T  master_id = 0;

    STKTPLG_PMGR_GetMasterUnitId( & master_id );

/*if get "!" and change interface need to check if it is necessary to store module config*/
    if ( cmd_buff[ 0 ] == '!' && strlen(cmd_buff) == 1 )
    {
        CLI_MGR_AddDeviceCfg( which_module, "!\n" );
        which_module = 0;
        is_store_module_config = FALSE;
        return FALSE;
    }
    else if ( cli_main_SubStrLen( cmd_buff, "interface ethernet" ) == 18 )
    {
        ptr = strchr( (char *)cmd_buff, '/' );
        unit = atoi( ptr - 1 );
        port = atoi( ptr + 1 );

        ifindex = CLI_MGR_UPORT_TO_IFINDEX( unit, port );

        if ( TRUE == STKTPLG_PMGR_IsModulePort( unit, port ) )
        {
            CLI_MGR_AddDeviceCfg( which_module, "!\n" );
            CLI_MGR_AddDeviceCfg( unit + SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK, cmd_buff );
            which_module = unit + SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;
            is_store_module_config = TRUE;
            return TRUE;
        }
        else
        {
            which_module = 0;
            is_store_module_config = FALSE;
            return FALSE;
        }
    }
    else
    {
        if ( is_store_module_config == TRUE )
        {
            CLI_MGR_AddDeviceCfg( which_module, cmd_buff );
            return TRUE;
        }
    }
#else
    UI8_T *ptr;
    UI32_T unit = 0, port = 0, ifindex = 0;
    BOOL_T is_inherit        =TRUE;

/*if get "!" and change interface need to check if it is necessary to store module config*/
    if (cmd_buff[0] == '!' && strlen(cmd_buff) == 1)
    {
        CLI_MGR_AddModuleCfg(which_module, "!\n");
        which_module = 0;
        is_store_module_config = FALSE;
        return FALSE;
    }
    else if (cli_main_SubStrLen(cmd_buff, "interface ethernet") == 18)
    {
        ptr = strchr((char *)cmd_buff,'/');
        unit = atoi(ptr-1);
        port = atoi(ptr+1);
        SWCTRL_POM_UIUserPortToIfindex(unit, port, &ifindex, &is_inherit);
        if (TRUE == CLI_MGR_IsModulePort(ifindex))
        {
            CLI_MGR_AddModuleCfg(which_module, "!\n");
            CLI_MGR_AddModuleCfg(unit, cmd_buff);
            which_module = unit;
            is_store_module_config = TRUE;
            return TRUE;
        }
        else
        {
            which_module = 0;
            is_store_module_config = FALSE;
            return FALSE;
        }
    }
    else
    {
        if (is_store_module_config == TRUE)
        {
            CLI_MGR_AddModuleCfg(which_module, cmd_buff);
            return TRUE;
        }
    }
#endif
#endif /* rich */
    return FALSE;
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_MAIN_CheckWhetherPrintProvisionMsg
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to check if user press [p]
 *            to print provision detail
 * INPUT    : CLI_TASK_WorkingArea_T *ctrl_P
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : waiting time - provision 90*2 ticks (original:6*30)
 *                         - partial provision 100*2 ticks
 * ------------------------------------------------------------------------
 */
static void CLI_MAIN_CheckWhetherPrintProvisionMsg(
    CLI_TASK_WorkingArea_T *ctrl_P)
{
    #define MESSAGE "Press [P] to print out the detail provision message "

    UI32_T count = 0;
    UI32_T len   = strlen(MESSAGE);
    BOOL_T is_print = FALSE;

    CLI_MGR_Set_PrintProvisionMsg(TRUE);
    CLI_LIB_PrintStr(MESSAGE);

    for(; count<= 2; count ++)    /* check key press twice */
    {
        UI8_T   key = 0;

        switch(ctrl_P->sess_type)
        {
#if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE)
            case CLI_TYPE_UART:      /* partial provision */
                key = CLI_IO_ReadACharFromConsole(ctrl_P);
                if( key == 'p' || key == 'P')
                {
                    is_print = TRUE;
                    break;
                }
                SYSFUN_Sleep(100);
                break;

            case CLI_TYPE_TELNET:    /* partial provision */
                key = CLI_IO_ReadACharFromTelnet(ctrl_P);
                if( key == 'p' || key == 'P')
                {
                    is_print = TRUE;
                    break;
                }
                break;
#endif  /* #if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE) */

            case CLI_TYPE_PROVISION:
            case CLI_TYPE_MODULE_PROVISION:
                key = CLI_IO_ReadACharFromConsole(ctrl_P);
                if( key == 'p' || key == 'P')
                {
                    is_print = TRUE;
                    break;
                }

                SYSFUN_Sleep(90);
                break;

            default:
                /* should not happen */
                break;
        }

        if (TRUE == is_print)
        {
            break;
        }
    }

    for(count = 0; count<len; count++)
        CLI_LIB_PrintStr(BACKSPACE);

    CLI_LIB_PrintStr("\r\n");

    CLI_MGR_Set_PrintProvisionMsg(is_print);

#undef MESSAGE
}

#if 0
/*------------------------------------------------------------------------|
 * ROUTINE NAME - cli_main_SubStrLen                                           |
 *------------------------------------------------------------------------|
 * FUNCTION: Calculate how many chars are matched between substr and s    |
 * INPUT   : s      - string pointer (to be matched with)                 |
 *           substr - string pointer                                      |
 * OUTPUT  : None                                                         |
 * RETURN  : number of matched chars between s & substr                   |
 * NOTE    : assume menu_tree data structure is globally available        |
 *------------------------------------------------------------------------*/
static UI16_T cli_main_SubStrLen(char *s,char *substr)
{
    UI16_T i,sublen = (UI16_T)strlen(substr);
    UI8_T sch,subch;
    if (sublen > strlen(s))
    {
        return 0;
    }

    for (i = 0;i < sublen;i++)
    {
//      if (*(substr+i) != *(s+i))
    if(*(s+i)>=0x61&&*(s+i)<=0x7A)
       sch=*(s+i)-0x20;
    else
       sch=*(s+i);
     if(*(substr+i)>=0x61&&*(substr+i)<=0x7A)
       subch=*(substr+i)-0x20;
    else
       subch=*(substr+i);
    if(sch!=subch)
            return 0;
    }

    return sublen;
} /* end cli_SubStrLen */
#endif

void CLI_IPCIO_PRINT(char *str)
{
    SYSFUN_Debug_Printf("%s %d\n",__FUNCTION__,__LINE__);
    SYSFUN_Debug_Printf("%s",str);
    fflush(stdout);
}


static UI32_T need_wait_key=0;

void CLI_MAIN_GET_WAIT_KEY(UI32_T *key)
{
    *key = need_wait_key;
}


void CLI_MAIN_SET_WAIT_KEY(UI32_T key)
{
    need_wait_key = key;
}

void CLI_MAIN_HandleMGR_Thread(UI32_T ticks, UI8_T ch, UI8_T * buff)
{
    SYSFUN_Msg_T    *msgbuf_p=(SYSFUN_Msg_T*)CLI_MAIN_mgrtd_ipc_buf;
    static UI32_T local_events  = SYSFUN_SYSTEM_EVENT_IPCMSG;
    UI32_T               received_events,sysfun_ret, w_key;
    UI32_T               cli_main_ipc_msgq_handle;
    BOOL_T               need_resp;

#if 1
/* wakka, move to cli_group.c
 */
return;
#endif


    CLI_MAIN_GET_WAIT_KEY(&w_key);
    CLI_TASK_GET_IPC_Handle(&cli_main_ipc_msgq_handle);



        SYSFUN_ReceiveEvent (SYSFUN_SYSTEM_EVENT_IPCFAIL|
                             SYSFUN_SYSTEM_EVENT_IPCMSG |
                             SYSFUN_SYSTEM_EVENT_SET_TRANSITION_MODE,
                             SYSFUN_EVENT_WAIT_ANY,
                             SYSFUN_TIMEOUT_NOWAIT,
                             &received_events);
        local_events |= received_events;

        if(local_events & SYSFUN_SYSTEM_EVENT_SET_TRANSITION_MODE)
        {
            CLI_INIT_SetTransitionMode();
            local_events ^= SYSFUN_SYSTEM_EVENT_SET_TRANSITION_MODE;
            /* need not to do IPCFAIL recovery in transition mode
             */
            if(local_events & SYSFUN_SYSTEM_EVENT_IPCFAIL)
            {
                /* clear fail info in IPCFAIL
                 */
                local_events ^= SYSFUN_SYSTEM_EVENT_IPCFAIL;
            }

        }

        /* handle IPCMSG
         */
        if(local_events & SYSFUN_SYSTEM_EVENT_IPCMSG)
        {
//            printf("%s %d\n", __FUNCTION__, __LINE__);
            sysfun_ret = SYSFUN_ReceiveMsg(cli_main_ipc_msgq_handle, 0, SYSFUN_TIMEOUT_NOWAIT,
                CLI_MAIN_PMGR_MSGBUF_MAX_REQUIRED_SIZE, msgbuf_p);
           // printf("%lu\n", sysfun_ret);
            if(sysfun_ret == SYSFUN_OK)
            {

                /* handle request message based on cmd
                 */
                switch(msgbuf_p->cmd)
                {
                    /* module id cmd
                     */
#if (SYS_CPNT_CLI == TRUE)
                    case SYS_MODULE_CLI:
                   // printf("%s %d\n",__FUNCTION__,__LINE__);
                        need_resp=CLI_MGR_HandleIPCReqMsg(msgbuf_p);
                     //   printf("%s %d\n",__FUNCTION__,__LINE__);
                        break;
#endif
                    case SYS_MODULE_SYS_CALLBACK:
                        /* SYS_CALLBACK ipc message can only be uni-direction
                         * just set need_resp as FALSE
                         */
                        CLI_MGR_HandleSysCallbackIPCMsg(msgbuf_p);
                        need_resp=FALSE;
                        break;
                    /* global cmd
                     */

                    case SYS_MODULE_BACKDOOR:
                        need_resp = BACKDOOR_MGR_HandleIPCMsg(msgbuf_p);
                        break;
                    case SYS_TYPE_CMD_IPCIO:
                        /*need_resp = CLI_IPCIO_HandleIPCReqMsg(msgbuf_p, &need_wait_key);*/
                        need_resp = BACKDOOR_MGR_HandleIPCIOMsg(SYS_BLD_CLI_GROUP_IPCMSGQ_KEY, msgbuf_p);
                        break;
                    #if 0
                    case SYS_TYPE_CMD_IPCIO:
                    SYSFUN_Debug_Printf(" recieve L_IPCIO_CMD_GETCHAR\n");

                    need_resp = L_IPCIO_HandleIPCReqMsg(msgbuf_p,CLI_IPCIO_PRINT , &need_wait_key);

                    /* when receive this msg, means backdoor need print out some buffer
                     * this is async msg
                     */
                        {
                         L_IPCIO_Msg_T* ipc_io_q =( L_IPCIO_Msg_T*)  msgbuf_p->msg_buf;

                           if(ipc_io_q->ipcio_arg_size >(L_IPCIO_MAX_LENGTH_OF_PRINTF+1))
                               SYSFUN_Debug_Printf("\n %s: backdoor print msg too large",__FUNCTION__);
                           else
                               SYSFUN_Debug_Printf("%s",(char *)ipc_io_q->ipcio_arg_buffer);
                        }
                     fflush(stdout);

                     break;
                     #endif
                    case SYS_TYPE_CMD_PROVISION_COMPLETE:
                    case SYS_TYPE_CMD_MODULE_PROVISION_COMPLETE:
                        msgbuf_p->msg_size = 0;
                        need_resp = TRUE;
                        break;
                    case SYS_TYPE_CMD_ENTER_MASTER_MODE:
                        CLI_INIT_EnterMasterMode();
                        SYSFUN_Debug_Printf("\nCLI enter master mode\n");
                        /* SYS_TYPE_CMD_ENTER_MASTER_MODE
                         * need a response which contains nothing
                         */
                        msgbuf_p->msg_size=0;
                        need_resp=TRUE;

                        /* IPCFAIL might happen when a upper layer cscgroup has
                         * entered transition mode but lower layer cscgroups still
                         * in master mode so async callback msg will keep sending
                         * from lower layer cscgroups. In this case, the IPCFAIL
                         * event can be ignored.
                         */
                        if(local_events & SYSFUN_SYSTEM_EVENT_IPCFAIL)
                        {
                            /* clear fail info in IPCFAIL
                             */
                            local_events ^= SYSFUN_SYSTEM_EVENT_IPCFAIL;
                        }

                        break;

                    case SYS_TYPE_CMD_ENTER_SLAVE_MODE:
                        CLI_INIT_EnterSlaveMode();

                        /* SYS_TYPE_CMD_ENTER_SLAVE_MODE
                         * need a response which contains nothing
                         */
                        msgbuf_p->msg_size=0;
                        need_resp=TRUE;

                        /* IPCFAIL might happen when a upper layer cscgroup has
                         * entered transition mode but lower layer cscgroups still
                         * in master mode so async callback msg will keep sending
                         * from lower layer cscgroups. In this case, the IPCFAIL
                         * event can be ignored.
                         */
                        if(local_events & SYSFUN_SYSTEM_EVENT_IPCFAIL)
                        {
                            /* clear fail info in IPCFAIL
                             */
                            local_events ^= SYSFUN_SYSTEM_EVENT_IPCFAIL;
                        }
                        break;

                    case SYS_TYPE_CMD_ENTER_TRANSITION_MODE:
                        CLI_INIT_EnterTransitionMode();

                        /* SYS_TYPE_CMD_ENTER_TRANSITION_MODE
                         * need a response which contains nothing
                         */
                        msgbuf_p->msg_size=0;
                        need_resp=TRUE;
                        break;


#if (SYS_CPNT_UNIT_HOT_SWAP == TRUE)
                  case SYS_TYPE_CMD_HANDLE_HOT_INSERTION:
                     CLI_MAIN_HandleHotInertion(msgbuf_p);
                     msgbuf_p->msg_size=0;
                     need_resp=TRUE;
                     break;

                  case SYS_TYPE_CMD_HANDLE_HOT_REMOVAL:
                     CLI_MAIN_HandleHotRemoval(msgbuf_p);
                     msgbuf_p->msg_size=0;
                     need_resp=TRUE;
                      break;
#endif
/*add by fen.wang,to process reload */
                   case  SYS_TYPE_CMD_RELOAD_SYSTEM:

                     msgbuf_p->msg_size=0;
                     need_resp=TRUE;
/*jingyan zheng add for defect ES3628BT-FLF-ZZ-00275*/
#if (SYS_CPNT_TELNET == TRUE)
                     CLI_TASK_CloseTelnetSessions();
#endif
                     break;
                   default:
                        /* Unknow command. There is no way to idntify whether this
                         * ipc message need or not need a response. If we response to
                         * a asynchronous msg, then all following synchronous msg will
                         * get wrong responses and that might not easy to debug.
                         * If we do not response to a synchronous msg, the requester
                         * will be blocked forever. It should be easy to debug that
                         * error.
                         */
                        SYSFUN_Debug_Printf("%s: Invalid IPC req cmd.\n", __FUNCTION__);
                        need_resp=FALSE;
                }

                if((need_resp==TRUE) && (SYSFUN_SendResponseMsg(cli_main_ipc_msgq_handle, msgbuf_p)!=SYSFUN_OK))
                    SYSFUN_Debug_Printf("%s: SYSFUN_SendResponseMsg fail.\n", __FUNCTION__);

            } /* end of if(SYSFUN_ReceiveMsg... */
            else
                local_events ^= SYSFUN_SYSTEM_EVENT_IPCMSG;
        } /* end of if(local_events & SYSFUN_SYSTEM_EVENT_IPCMSG) */

        /* handle IPC Async Callback fail when IPC Msgq is empty
         */
        if((local_events & SYSFUN_SYSTEM_EVENT_IPCFAIL) &&
            !(local_events & SYSFUN_SYSTEM_EVENT_IPCMSG))
        {
            /* read fail info from IPCFAIL
             */

            /* do recovery action
             */
            local_events ^= SYSFUN_SYSTEM_EVENT_IPCFAIL;
        }

}

#if (SYS_CPNT_UNIT_HOT_SWAP == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_MAIN_HandleHotInertion
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This function will invoke a new dut insertion in CSCGroup1.
 *
 * INPUT:
 *    starting_port_ifindex.
      number_of_port
      use_default
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    None.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
void CLI_MAIN_HandleHotInertion(SYSFUN_Msg_T* msgbuf_p)
{
 SYS_TYPE_HandleHotSwapArg_T *msg_p;

 if (msgbuf_p == NULL)
    return;

    msg_p = (SYS_TYPE_HandleHotSwapArg_T*)msgbuf_p->msg_buf;

    CLI_MGR_HandleHotInsertion(msg_p->starting_port_ifindex, msg_p->number_of_port, msg_p->use_default);

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_MAIN_ HandleHotRemoval
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This function will invoke a new dut removal in CSCGroup1.
 *
 * INPUT:
 *    starting_port_ifindex.
      number_of_port
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    None.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
void CLI_MAIN_HandleHotRemoval(SYSFUN_Msg_T* msgbuf_p)
{
 SYS_TYPE_HandleHotSwapArg_T *msg_p;

 if (msgbuf_p == NULL)
    return;

    msg_p = (SYS_TYPE_HandleHotSwapArg_T*)msgbuf_p->msg_buf;


    CLI_MGR_HandleHotRemoval(msg_p->starting_port_ifindex, msg_p->number_of_port);

}
#endif

