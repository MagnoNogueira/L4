#ifndef CLI_API_H
#define CLI_API_H


/*system*/
#include "sysfun.h"
#include <string.h>
#include <stdlib.h>
#include <sys_type.h>
//#include <time.h>
#include "sys_dflt.h"

/*cli internal*/
#include "clis.h"
#include "cli_def.h"
#include "cli_mgr.h"
#include "cli_io.h"
#include "cli_type.h"
#include "cli_task.h"
#include "cli_func.h"
#include "cli_main.h"
#include "cli_lib.h"
#include "cli_api.h"
#include "cli_pars.h"
#include "cli_msg.h"
#include "cli_cmd.h"
#include "cli_auth.h"
#include "cli_runcfg.h"
//#include "swdrv_type.h"
//#include "extbrg_pmgr.h"
//#if 1 /* rich linux */
/*syb-systems*/
#include "sys_hwcfg.h"
#include "sys_cpnt.h"
//#include "swctrl.h"
//#include "swctrl_pmgr.h"
//#include "leaf_sys.h"
//#include "stktplg_task.h"
//#include "amtr_mgr.h"
//#include "amtr_pmgr.h"
//#include "vlan_mgr.h"
//#include "vlan_pmgr.h"
//#include "vlan_pom.h"
//#include "vlan_pom_1v.h"
//#include "vlan_lib.h"
//#include "leaf_2674p.h"
//#include "leaf_2674q.h"
//#include "leaf_es3626a.h"

//#if (SYS_CPNT_CLUSTER==TRUE)
//#include "cluster_pmgr.h"
//#endif

//#include "igv3snp_mgr.h"
//#include "trk_mgr.h"
//#include "trk_pmgr.h"
//#include "fs.h"
//#include "iml.h"
//#include "userauth.h"
//#include "swctrl.h"
//#include "swctrl_pom.h"
//#include "fs_type.h"
//#include "swdrv_type.h"
//#include "mib2_mgr.h"
//#include "mib2_pmgr.h"
//#include "nmtr_mgr.h"
//#include "nmtr_pmgr.h"
//#include "extbrg_mgr.h"
//rich#include "ip_mgr.h"
//rich#include "sys_time.h"
//#include "stktplg_mgr.h"
//#include "stktplg_pom.h"
//#include "stktplg_pmgr.h"
//#include "stktplg_type.h"
//rich#include "ping_mgr.h"
//#include "http_mgr.h"
//#include "pri_mgr.h"
//#include "pri_pmgr.h"
//#include "lacp_mgr.h"
//#include "lacp_pmgr.h"
//#include "l_stdlib.h"
//rich#include "sysSerial.h"
//#include "syslog_mgr.h"
//#include "syslog_pmgr.h"
//#include "syslog_type.h"
//#include "sys_mgr.h"
//#include "sys_pmgr.h"
//#include "trap_mgr.h"
//#include "snmp_mgr.h"
//#include "snmp_pmgr.h"
//#include "xstp_mgr.h"
//#include "xstp_pom.h"
//#include "uc_mgr.h"
//#include "leddrv.h"
//#include "sys_module.h"

//#include "if_mgr.h"
//#include "if_pmgr.h"
//#endif /* rich linux */
//#include "garp_mgr.h"
//#include "garp_pmgr.h"
//#include "xfer_mgr.h"
//#include "xfer_pmgr.h"
//#if 0//rich
//#include "eh_type.h"
//#include "eh_mgr.h"
//#include "dhcp_mgr.h"
//#include "Xfer_buf_mgr.h"

//#if (SYS_CPNT_COS == TRUE)
//#include "l4_cos_mgr.h"
//#include "cos_vm.h"
//#endif
//#if (SYS_CPNT_ACL == TRUE)
//#include "acl_type.h"
//#include "l4_acl_mgr.h"
//#endif
//#include "l_md5.h"


//#if (CLI_SUPPORT_L2_DHCP_RELAY == 1)
//#include "brg_mgr.h"
//#endif
//#if (SYS_CPNT_COMMUNITY_PRIVATE_VLAN == TRUE)
//#include "pvlan_mgr.h"
//#endif

//#if (SYS_CPNT_PORT_SECURITY == TRUE)
//#include "psec_mgr.h"
//#include "psec_pmgr.h"
//#endif
//#if (SYS_CPNT_VDSL == TRUE)
//#include "vdsl_drv.h"
//#endif
//#if defined(JBOS)
//#include "rs_vlan_adpt.h"
//#endif
//#endif//rich


//#include "cli_api_show.h"
#define DEVELOP_STAGE 0

#define Com_Merge    0
#define PROCESS_MORE_FUNC(STR){            \
    CLI_LIB_PrintStr(STR);                 \
    line_num++;							   \
    memset(buff, 0, sizeof(buff)); }

#define PROCESS_3COM_MORE(STR){            \
   CLI_LIB_PrintStr(STR);                  \
   } memset(buff, 0, CLI_DEF_MAX_BUFSIZE); }


#pragma mark -
#pragma mark Just remove compile warning :-)
BOOL_T CLI_API_Get_Print_Interactive_Mode();
void CLI_API_Set_Print_Interactive_Mode(BOOL_T mode);

#define PROCESS_MORE(STR){                 \
   CLI_LIB_PrintStr(STR);                  \
    line_num++;							   \
    memset(buff, 0, sizeof(buff)); }


#define PING_DEBUG

#define CLI_LIB_PrintStr_1(fmt, arg1)  { \
   char *message;                                       \
   message = calloc(CLI_DEF_MAX_BUFSIZE, sizeof(char)); \
   sprintf((char *)message, fmt, arg1);                          \
   CLI_LIB_PrintStr(message);            \
   free(message);                                        \
}

#define CLI_LIB_PrintStr_2(fmt, arg1, arg2)  { \
   char *message;                                       \
   message = calloc(CLI_DEF_MAX_BUFSIZE, sizeof(char)); \
   sprintf((char *)message, fmt, arg1, arg2);                    \
   CLI_LIB_PrintStr(message);                  \
   free(message);                                        \
}

#define CLI_LIB_PrintStr_3(fmt, arg1, arg2, arg3)  { \
   char *message;                                       \
   message = calloc(CLI_DEF_MAX_BUFSIZE, sizeof(char)); \
   sprintf((char *)message, fmt, arg1, arg2, arg3);              \
   CLI_LIB_PrintStr(message);                        \
   free(message);                                        \
}

#define CLI_LIB_PrintStr_4(fmt, arg1, arg2, arg3, arg4)  { \
   char *message;                                         \
   message = calloc(CLI_DEF_MAX_BUFSIZE, sizeof(char));   \
   sprintf((char *)message, fmt, arg1, arg2, arg3, arg4);          \
   CLI_LIB_PrintStr(message);                              \
   free(message);                                          \
}

#define CLI_LIB_PrintStr_N(fmt, args...)                 { \
    char *message;                                         \
    message = calloc(CLI_DEF_MAX_BUFSIZE, sizeof(char));   \
    sprintf((char *)message, fmt, ##args);                 \
    CLI_LIB_PrintStr(message);                             \
    free(message);                                         \
}


/*PROCESS_MORE*/
#define PROCESS_MORE_1(fmt, arg1)  { \
   char *message;                                       \
   message = calloc(CLI_DEF_MAX_BUFSIZE, sizeof(UI8_T)); \
   sprintf(message, fmt, arg1);                          \
   PROCESS_MORE(message);            \
   free(message);                                        \
}

#define USED_TO_PRT_ARGS                                                     \
        {  int counter;                                                      \
           for (counter=0; arg[counter]!=0; counter++)                       \
           { CLI_LIB_PrintStr_2("arg[%d] = %s\r\n", counter, arg[counter]);} \
        }


#define PORT_TYPE_ETH    1
#define PORT_TYPE_TRUNK	 2

typedef enum{
   CLI_API_ETH_OK = 0,
   CLI_API_ETH_NOT_PRESENT,
   CLI_API_ETH_TRUNK_MEMBER,
   CLI_API_ETH_UNKNOWN_PORT
}CLI_API_EthStatus_T;

typedef enum{
   CLI_API_TRUNK_OK = 0,
   CLI_API_TRUNK_NOT_EXIST,
   CLI_API_TRUNK_NO_MEMBER,
   CLI_API_TRUNK_INVALID_RANGE
}CLI_API_TrunkStatus_T;

typedef enum{
   CLI_LIB_SHOW_ERR_MSG = 0,
   CLI_API_DESCRIPTION_ETH,
   COPY_FROM_LOCAL_FILE,
   COPY_FROM_RUNNING_CONFIG,
   COPY_FROM_STARTUP_CFG,
   COPY_FROM_TFTP_SERVER,
   COPY_FROM_UNIT_FILE,
   IS_FILE_EXISTED,
   STR_TO_NONPRINTABLE,
   CLI_API_EXECTIMEOUT,
   CLI_API_SILENTTIME,
   CLI_API_DATABITS,
   CLI_API_PARITY,
   CLI_API_SPEED,
   CLI_API_STOPBITS,
   CLI_API_PORT_MONITOR,
   _,
   CLI_API_QUEUE_BANDWIDTH_GLOBAL,
   CLI_RUNCFG_GETRUNNINGCFG,
   CLI_AUTH_CHECKLOGINPASSWORD,
   CLI_AUTH_CHECKENABLEPASSWORD,
   CLI_MGR_SAVERUNNINGCFG,
   CLI_MGR_GET_RUNNINGCFG,
   CLI_MGR_SETKILLWORKINGSPACEFLAG,
   CLI_MGR_DECREASEREMOTESESSION,
   CLI_MGR_INCREASEREMOTESESSION,
   CLI_TASK_SETKILLWORKINGSPACEFLAG,
   CLI_IO_READFROMTELNET,
   CLI_IO_READFROMCONSOLE,
   CLI_MGR_SETSYSBANNERMSGSTATUS,
   CLI_MGR_DISABLESYSBANNERMSGSTATUS,
}CLI_API_FunctionNo_T;



#define EXIT_SESSION_MORE    0xffff
#define JUMP_OUT_MORE    0xffffffff
#define DEFAULT_CAPABILITIES 0xffffffff

/*
typedef struct
{
   SWDRV_IfTableStats_T    if_table_stats;
   SWDRV_EtherlikeStats_T  ether_like_stats;
   SWDRV_EtherlikePause_T  ether_like_pause;
   SWDRV_IfXTableStats_T   if_xtable_stats;
   SWDRV_RmonStats_T       rmon_stats;
}CLI_API_ShowIfCounter_T;
*/

/*-----------------local function ------------------*/
BOOL_T Get_Line_Heads_From_Buffer(char *buffer, char **heads);

CLI_API_EthStatus_T   verify_ethernet(UI32_T unit, UI32_T port, UI32_T *lport);
CLI_API_TrunkStatus_T verify_trunk(UI32_T trunk_id, UI32_T *lport);

void display_ethernet_msg(CLI_API_EthStatus_T status, UI32_T unit, UI32_T port);
void display_trunk_msg(CLI_API_TrunkStatus_T status, UI32_T trunk_id);

BOOL_T str_to_nonprintable(char *str, UI8_T *om);
//BOOL_T GetIfCounter(UI32_T ifindex, CLI_API_ShowIfCounter_T* IfCounter);
//#if (SYS_CPNT_EH == TRUE)
//void CLI_API_Show_Exception_Handeler_Msg(void);
//void CLI_API_SetandShow_Exception_Handeler_Msg(EH_TYPE_Msg_T format_id, UI32_T function_no, char *arg);
//#endif
void CLI_API_Conver_Lower_Case(char *inStr, char *outStr);
/**********************************<<GENELAL>>***********************************************/
/*change mode*/
UI32_T CLI_API_Exit(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);
UI32_T CLI_API_End(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);
UI32_T CLI_API_Quit(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);
UI32_T CLI_API_Disable(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);
UI32_T CLI_API_Enable(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);
UI32_T CLI_API_Configure(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);
UI32_T CLI_API_Help(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);

/* SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL
 */
UI32_T CLI_API_Privilege(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);
UI32_T CLI_API_Show_Privilege(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);

UI32_T CLI_API_Show_Privilege_Command(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);

UI32_T CLI_API_Interface_Ethernet(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);

#endif

