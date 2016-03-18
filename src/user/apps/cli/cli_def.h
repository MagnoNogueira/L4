#ifndef  CLI_DEF_H
#define  CLI_DEF_H

#include "sys_type.h"
#include "sys_adpt.h"
#include "sys_cpnt.h"
#include "l_inet.h"

#if 0/* rich linux */
#include "leaf_es3626a.h"
#endif

#if defined(JBOS)
#define CLI_SUPPORT_PORT_NAME 1
#endif

#if defined(JBOS)
#define CLI_SUPPORT_L2_DHCP_RELAY 0
#endif

#if defined(JBOS)
#define CLI_SUPPORT_DEBUG_HTTPS 1
#endif

#define CLI_SUPPORT_DHCP_CLIENTID 1

#if defined(JBOS)
#define CLI_SUPPORT_DHCP_CLIENTID 1
#endif

#if defined(JBOS) || defined(NOVAL) || defined(GRAHAM) || defined(NOVAL_12G) || defined(MERCURY_DC) || defined(MERCURY_ZZ) || defined(ALPS) || defined(BLANC) || defined(PEGASUS)
#define CLI_SUPPORT_SNMP_V2C 1
#endif

#if defined(VS2512A)
#define CLI_SUPPORT_VDSL 1
#endif

#if defined(JBOS)
#define CLI_SUPPORT_PACKET_FILTER 1
#endif

#if defined(NOVAL) || defined(GRAHAM) || defined(NOVAL_12G)
#define CLI_SHOW_RUNTIME_PROV_WARRING_MSG 1
#endif

#if defined(NOVAL) || defined(GRAHAM) || defined(NOVAL_12G) || defined(MERCURY_ZZ)|| defined(MERCURY_DC)  || defined(ALPS) || defined(BLANC) || defined(PEGASUS)
#define CLI_SUPPORT_INTERFACE_TO_INTERFACE 1
#endif

#if defined(MERCURY) || defined(VS2512A) || defined(CHT_48) || defined(MERCURY_DC) || defined(MERCURY_CHT) || defined(MERCURY_ZZ)
#define CLI_SUPPORT_PVLAN_RATELIMIT 1
#endif

#if defined(JBOS) || defined(GRAHAM) || defined(NOVAL) || defined(MERCURY_DC)|| defined(NOVAL_12G)  || defined(ALPS) || defined(BLANC) || defined(PEGASUS)
#define CLI_SUPPORT_TACACS 1
#endif
#if defined(NOVAL) || defined(GRAHAM) || defined(NOVAL_12G)  || defined(ALPS) || defined(BLANC) || defined(PEGASUS)
#define CLI_SUPPORT_REMOTE_LOG 1
#endif

#if (defined(JBOS) || defined(STRAWMAN) || defined(NOVAL) || defined(UNICORN))|| defined(NOVAL_12G)
#define CLI_SUPPORT_JUMBO_FRAME 1
#endif


#if !(defined(STRAWMAN) || defined(UNICORN))
#define CLI_SUPPORT_COS 1
#endif


#if !(defined(STRAWMAN) || defined(UNICORN))
#define CLI_SUPPORT_GVRP 1
#endif

#if !(defined(UNICORN))
#define CLI_SUPPORT_LACP 1
#endif

#if !(defined(UNICORN))
#define CLI_SUPPORT_RADIUS 1
#endif
#if defined(JBOS) || defined(NOVAL) || defined(GRAHAM) || defined(MERCURY_DC) || defined(NOVAL_12G) || defined(ALPS) || defined(BLANC) || defined(PEGASUS)
#define CLI_SUPPORT_DOT1W_MODE 1
#endif

#if defined(MERCURY_DC) || defined(MERCURY_ZZ) || defined(ALPS) || defined(MERCURY_L3_DC) || defined(BLANC) || defined(XGS_SWITCH) || defined(PEGASUS)
#define CLI_SUPPORT_PSEC_MAC_COUNT 1
#endif

#if (SYS_CPNT_ROUTING == TRUE )
#define CLI_SUPPORT_L3_FEATURE 1
#endif

/*#define    OLD_RUNTIME_PROVISION*/

/*92.03.27 pttch add for disconnect session*/
#if (defined(GRAHAM) || defined(ALPS) || defined(MERCURY_ZZ)) || defined(BLANC) || defined(PEGASUS)
#define CLI_SUPPORT_DISCONNECT_ALL_SESSION 1
#endif

/*#if defined(MERCURY_ZZ)*/
#define CLI_SUPPORT_QUOTE_STRING_FEATURE TRUE
/*#endif*/

#define CLI_DEF_MAX_BUFSIZE     SYS_ADPT_CLI_MAX_BUFSIZE
#define CLI_DEF_DISP_WIDTH      80
#define CLI_DEF_MIN_DISP_WIDTH      80
#define CLI_DEF_MAX_DISP_WIDTH      300
#define CLI_DEF_DISP_LINENUM        24
#define CLI_DEF_MIN_DISP_LINENUM    24
#define CLI_DEF_MAX_DISP_LINENUM    200

#define CLI_DEF_MAX_WORDSIZE    25      /* one command word max size */
#define CLI_DEF_MAX_ARGS_NUM    20      /* maximum argument number   */
/*jingyan zheng defect ES3628BT-FLF-ZZ-00066*/
#define CLI_DEF_MAX_CMD_NUM     16      /* maximum command/argument number matched when type ? command*/

#define CLI_DEF_OUTPUT_BUFFER          2000     /* Output buffer size */

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
#define CLI_DEF_MAX_HIST        256
#define CLI_DEF_HIST            10
#else
#define CLI_DEF_MAX_HIST        10      /* 20+1 add one to prevent boundary error */
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

#define CLI_DEF_MAX_DELNUM      10
#define CLI_DEF_MAX_LINENUM     24      /* maximum screen line number */
#define CLI_DEF_MAX_MSGNUM      256     /* maximum message number for help */
#define CLI_DEF_MAX_HEADER      24     /* maximum message number for help */
#define CLI_DEF_MAX_CONFIG_SIZE      SYS_ADPT_CLI_MAX_CONFIG_SIZE     /* maximum message number for help */

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
#define CLI_DEF_TERMINAL_TYPE VT100
#define VT100 1
#define VT102 2
#define ANSI_BBS 3
#define CLI_DEF_ESCAPE 0x1B
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
/* maximum command number
 */
#define CLI_DEF_MAX_CMDS_NUM                    20

/* maximum command buffer for privilege
 */
#define CLI_DEF_MAX_PRIVILEGE_CMD_STRING        44

/* maximum commands buffer for privilege
 */
#define CLI_DEF_MAX_PRIVILEGE_CMDS_STRING       CLI_DEF_MAX_PRIVILEGE_CMD_STRING

/* maximum command tokens for privilege
 */
#define CLI_DEF_MAX_PRIVILEGE_CMDS_TOKENS       10

/* if the default level of command is CLI_DEF_CONST_LEVEL, it means can't be modify
 */
#define CLI_DEF_CONST_LEVEL                     32

/* use for return to default privilege level
 */
#define CLI_DEF_RETURN_DEFAULT_PRIVILEGE_LEVEL 	255
#endif /* SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL */

#define CLI_DEF_RUNTIME_PROVISION_MAGIC_WORD     CLI_MGR_RUNTIME_PROVISION_MAGIC_WORD
#define CLI_DEF_RUNTIME_PROVISION_UC_MEM_SIZE    CLI_MGR_RUNTIME_PROVISION_UC_MEM_SIZE
#define CLI_DEF_RUNTIME_PROVISION_UC_MEM_BOUNDRY CLI_MGR_RUNTIME_PROVISION_UC_MEM_BOUNDRY
#define CLI_DEF_OEM_LOGO_UC_MEM_SIZE             CLI_MGR_OEM_LOGO_UC_MEM_SIZE

#define NULL_CMD        0xffff  /* max value of UI16_T  */
#define NULL_ARG        0xffff  /* max value of UI16_T */

#define NEWLINE         "\r\n"  /* "\x0D\x0A" */
#define BACKSPACE       "\x08\x20\x08"
#define BACK            "\x08"
#define BEL             "\x07"
#define SPACE           "\x20"
#define SHIFTLEFT       "\x1B\x5B\x44"

/**************************/
/* Key status definitions */
/**************************/
#define UI_KEY_NORMAL         0
#define UI_KEY_CTRL_V_ON      1

#define CLI_DEF_AUTHEN_STATE_NOT_READY 0
#define CLI_DEF_AUTHEN_STATE_READY     1
#define CLI_DEF_AUTHEN_STATE_PASS      2
#if(SYS_CPNT_CLUSTER==TRUE)
/*for cluster telnet*/
#define CLI_DEF_AUTHEN_STATE_NOT_CHECK_USERNAME_PASSWORD     3
#endif
/*************************/
/* Session Struct define */
/*************************/
typedef struct MENU_S
{
   /*Access mode*/
   UI8_T    AccMode;

   /*User Info*/
   UI32_T   AuthenState;
   char     UserName[SYS_ADPT_MAX_USER_NAME_LEN + 1];
   char     Password[SYS_ADPT_MAX_PASSWORD_LEN + 1];

   L_INET_AddrIp_T TelnetIPAddress;

#if(SYS_CPNT_CLUSTER==TRUE)
   /*95.2.16 for telnet relay*/
   UI32_T   TelnetTnpdTaskId;
   BOOL_T   IsTelnetRelaying;
#endif
/*92.03.27 pttch add for disconnect session*/
   UI8_T    RemoteSessionType;
   UI32_T   IdleStartTick;
   UI32_T   AccessPrivilege;

   /*xxxxxxx*/
   UI16_T   CliState;

   /*xxxxxxx*/
   char     OneCmdLine[CLI_DEF_MAX_BUFSIZE+1];
   char     OldEditBuf[CLI_DEF_MAX_BUFSIZE+1];
   UI8_T    EditMode;

   /*all exec mode history buffer and handler*/
   char     HistBuf[CLI_DEF_MAX_HIST+1][CLI_DEF_MAX_BUFSIZE+1];
   UI16_T   HistHeader;
   UI16_T   HistTail;

   /*all cfg mode history buffer and handler*/
   char     CfgHistBuf[CLI_DEF_MAX_HIST+1][CLI_DEF_MAX_BUFSIZE+1];
   UI16_T   CfgHistHeader;
   UI16_T   CfgHistTail;

   /*xxxxxxx*/
   char     DelBuf[CLI_DEF_MAX_DELNUM][CLI_DEF_MAX_BUFSIZE+1];
   UI16_T   DelHeader;
   UI16_T   DelNum;

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
  /*terminal setting*/
   UI32_T   length;
   UI32_T   width;
   UI32_T   histsize;
   UI32_T   terminal_type;
   UI8_T    escape_character;
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

   /*for <interface ethernet x/a-b,c,d-e>*/
   UI32_T  unit_id;
   UI8_T   port_id_list[8];                       /* port id list bit map */

   /*for <interface vlan xxx>*/
   UI32_T  vlan_ifindex;

   /*for <interface tunnel xxx>*/
   UI32_T  tunnel_ifindex;

   /*for <interface port-channel xxx>*/
   UI32_T  pchannel_id;

   /*for <interface loopback xx>*/
   UI32_T loopback_id;

   /*for <line xxx>*/
   UI8_T   line_type;
   UI32_T  domain_index;

   /*xxxxxxx*/
   UI8_T   KeyState;

#if (CLI_SUPPORT_L3_FEATURE == 1)
   char  Pool_name[8];
#endif

#if (SYS_CPNT_ACL == TRUE)
   char acl_name[SYS_ADPT_ACL_MAX_NAME_LEN+1];
   UI32_T acl_type;
   UI32_T mask_type;
   UI32_T mask_mode;
#endif

#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)
   UI8_T vlan_database_type;
#endif

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
   char  cmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH+1];
   char  pmap_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH+1];
#endif

   /*use to keep the pointer of the counter table when "clear counter"*/ /*eth only*/
   void    *counters_table[SYS_ADPT_TOTAL_NBR_OF_LPORT];

    UI8_T uplink_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST]; /*for Pvlan*/
    UI8_T downlink_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];/*for Pvlan*/


#if (SYS_CPNT_AAA == TRUE)
   /*AAA Group Server*/
   char   aaa_sg_name[SYS_ADPT_MAX_LEN_OF_AAA_SERVER_GROUP_NAME + 1];
   UI32_T aaa_sg_type;
   UI16_T aaa_sg_group_index;
#endif   /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_OSPF == TRUE)
   UI32_T process_id; /*xiongyu 20090116*/
#endif /* SYS_CPNT_OSPF */
   UI32_T profile_id;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
   char  time_range_name[SYS_ADPT_MAX_LENGTH_OF_TIME_ACL_TABLE_NAME + 1];
#endif
#if (SYS_CPNT_OSPF6 == TRUE)
   char tag[40];
#endif /* SYS_CPNT_OSPF */

#if (SYS_CPNT_ERPS == TRUE)
   UI8_T erps_domain_id; /*erps domain id */
#endif

} CLI_MENU;

typedef struct CLI_SYS_INFO_S
{
   UI32_T my_unit_id;
   UI32_T max_port_number;
} CLI_SYSTEM_INFO;

/* Add telnet client, Jinnee.Jin, 2008, 3, 5 */
typedef struct CLI_TelnetClient_S
{
    int socket;
    char hisopts[256];
    char myopts[256];
}CLI_TelnetClient_T;

typedef struct CLI_TASK_WorkingArea_S
{
    /*this working area is used or not*/
    BOOL_T  is_empty;

    /*the native characteristics of this session*/
    UI32_T  cli_tid;
    I32_T   socket;
    UI8_T   sess_type;
#if (SYS_CPNT_3COM_CLI == TRUE)
    UI32_T  call_backdoor_cli_tid;
#endif


    /*used to handle provision*/
    UI32_T  unit;
    char   *prov_buffer;                        /* provision buffer */
    UI32_T  prov_length;                        /* provision buffer length */
    UI8_T   prov_status;                        /* provision status */

    /*CLI engine data base*/
    CLI_MENU  CMenu;

    /*system informatiom used in CLI*/
    CLI_SYSTEM_INFO sys_info;

    /*print out buffer*/
    char   send_buf[CLI_DEF_OUTPUT_BUFFER + 1];    /* output buffer */

    /*isiah*/
    /* this working area will kill itself or not*/
/*92.03.27 pttch add for disconnect session*/
#if (SYS_CPNT_SSHD == TRUE || CLI_SUPPORT_DISCONNECT_ALL_SESSION == 1 || SYS_CPNT_SSH2 == TRUE)/*isiah*/
    BOOL_T  kill_itself;
#endif

    /* for stopless printing */
    BOOL_T print_interactive;

#if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE)
    UI32_T reload_countdown;   /* minutes */
#endif  /* SYS_CPNT_SYSMGMT_DEFERRED_RELOAD */

#if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE)
    /* is partial provision */
    UI32_T  previous_sess_type;                 /* record previous session type */
    BOOL_T  partial_provision;                  /* TRUE: is partial provision */
    UI32_T  partial_prov_length;                /* provision buffer length */
    char    *partial_prov_buffer;               /* provision buffer */
#endif  /* #if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE) */

/* Add telnet client, Jinnee.Jin, 2008, 3, 5 */
    BOOL_T is_telnet_client_mode;
    CLI_TelnetClient_T telnet_client;
#if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE)
    UI32_T monitor_id;
#endif /* #if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE) */

    BOOL_T enter_debug_mode;
} CLI_TASK_WorkingArea_T;



/**************************/
/* CLI Configuration Mode */
/**************************/
enum CLI_ACCESS_MODE_E
{
   NORMAL_EXEC_MODE = 0,
   PRIVILEGE_EXEC_MODE,
   PRIVILEGE_CFG_GLOBAL_MODE,
   PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE,
   PRIVILEGE_CFG_INTERFACE_ETH_MODE,
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
   PRIVILEGE_CFG_INTERFACE_ETH0_MODE,  /*wenfang add 20090106 */
#endif
    PRIVILEGE_CFG_CONTROL_PLANE_MODE,
#if (SYS_CPNT_CRAFT_PORT == TRUE)
   PRIVILEGE_CFG_INTERFACE_CRAFT_MODE,
#endif
   PRIVILEGE_CFG_INTERFACE_VLAN_MODE,
#if (SYS_CPNT_IP_TUNNEL == TRUE)
    PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE,
#endif
   PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE,
   PRIVILEGE_CFG_VLAN_DATABASE_MODE,
   PRIVILEGE_CFG_LINE_CONSOLE_MODE,
   PRIVILEGE_CFG_LINE_VTY_MODE,
   PRIVILEGE_CFG_LINE_MODE,
   PRIVILEGE_CFG_MSTP_MODE,
   PRIVILEGE_CFG_DOMAIN_MODE,
   PRIVILEGE_CFG_ROUTER_MODE,
   PRIVILEGE_CFG_ROUTERDVMRP_MODE,
   PRIVILEGE_CFG_ROUTEROSPF_MODE,
   PRIVILEGE_CFG_ROUTEROSPF6_MODE,
#if (SYS_CPNT_BGP == TRUE)
   PRIVILEGE_CFG_ROUTER_BGP_MODE,
#endif
#if (SYS_CPNT_NSM_POLICY == TRUE)
    PRIVILEGE_CFG_ROUTE_MAP_MODE,
#endif
   PRIVILEGE_CFG_DHCPPOOL_MODE,
   PRIVILEGE_CFG_ACL_MAC_MODE,
   PRIVILEGE_CFG_ACL_STANDARD_MODE,
   PRIVILEGE_CFG_ACL_EXTENDED_MODE,
   PRIVILEGE_CFG_ACL_IP_MASK_MODE,
   PRIVILEGE_CFG_ACL_MAC_MASK_MODE,
   PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE,
   PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE,
#if (SYS_CPNT_AAA == TRUE)
   PRIVILEGE_CFG_AAA_SG_MODE,
   PRIVILEGE_CFG_AAA_SG_RADIUS_MODE,
#endif /*#if (SYS_CPNT_AAA == TRUE)*/
#if (SYS_CPNT_DAI == TRUE)
   PRIVILEGE_CFG_ACL_ARP_MODE,
#endif
   PRIVILEGE_CFG_CLASS_MAP_MODE,
   PRIVILEGE_CFG_POLICY_MAP_MODE,
   PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE,
#if (SYS_CPNT_FILTER_THROOTTLE == TRUE)
   PRIVILEGE_CFG_IGMP_PROFILE_MODE,
   PRIVILEGE_CFG_MLD_PROFILE_MODE,
#endif
#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
   PRIVILEGE_CFG_TIME_RANGE_MODE,
#endif

#if(SYS_CPNT_ERPS == TRUE)
   PRIVILEGE_CFG_ERPS_MODE,
#endif
#if(SYS_CPNT_ELPS == TRUE)
   PRIVILEGE_CFG_ELPS_MODE,
#endif
    DEBUG_MODE,
    CLI_ACCESS_MODE_MAX_NUM
};




/**********************************************************/
/* CMDLIST type define                                    */
/*      each command word correspond to an unique ID      */
/**********************************************************/
typedef struct CMDLIST_S
{
    char   *name;                                       /* command string                 */
    UI32_T help_idx;                                    /* help message                   */
    UI32_T (*cmd_func)(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);   /* command function pointer       */
    UI16_T arg_idx;
    UI16_T max_args;                                    /* maximum argument numbers       */
    UI16_T  neibor_idx;                                  /* neighbor index                 */
    UI16_T  child_idx;                                   /* child index                    */

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
    UI16_T  current_privilege;                           /* current command node privilege */
    UI16_T  default_privilege;                           /* default command node privilege */
#endif    /* #if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE) */
} CMDLIST;


/**********************************************************/
/* ARGLIST type define                                    */
/*      each argument correspond to an unique ID          */
/**********************************************************/
typedef struct ARGLIST_S
{
    //UI8_T   pos;              /* position in argument lists */
    char    *name;            /* command string */
    UI32_T  help_idx;         /* help message */
    UI8_T   type;             /* argument data type */
    UI32_T  max_range;  /*May 15*/   /* max data num range (WORD_DATA, CHAR_DATA) */
    UI32_T  min_range;
    UI16_T  neibor_idx;       /* neighbor index */
    UI16_T  next_idx;         /* next index */
    UI16_T   necessary;        /* if necessary argument */

} ARGLIST;


#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
/**********************************************************/
/* CLI_PrivLvlCmdEntry_T type define                      */
/**********************************************************/
typedef struct CLI_PrivLvlCmdEntry_S
{
    BOOL_T  is_all;
    UI8_T   pri_level;
    UI8_T   cmd_mode;
    char    cmd_str [CLI_DEF_MAX_PRIVILEGE_CMD_STRING+1];
} CLI_PrivLvlCmdEntry_T;
#endif    /* #if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE) */


/********************/
/* Data type define */
/********************/
enum DATA_TYPE_E
{
    NO_DATA,
    CHAR_DATA,
    LINE_DATA,
    WORD_DATA,
    VOICE_PORT_WORD_DATA,
    MAC_ADDR_DATA,
    IP_ADDR_DATA, /* allow ipv6 and ipv4 address type */
    IPV4_ADDR_DATA,
    IPV4_PREFIX_DATA,
    IPV6_ADDR_DATA,                 /* only ipv6 address type without scope id */
    IPV6_PREFIX_DATA,               /* ipv6 address type with prefix */
    IPV6_ADDR_DATA_WITH_SCOPE_ID,   /* ipv6 address type with scope id */
    IPV6_LINK_LOCAL_ADDR_DATA_NO_SCOPE_ID,   /* ipv6 link local address type no scope id */
    IPV6_ADDR_RAW_DATA,                      /* ipv6 address raw type */
    HEX_DATA,
    COLON_DATA,
    PORT_DATA,
    PVC_DATA,
    MINUS_DATA,
    BYPASS_DATA,
    QUOT_DATA,
    LIST_DATA,
    DOT_DATA,
    LIST_HAVE_UNIT_DATA,
    LIST_NO_UNIT_DATA,
    RANGE_DATA,
    PORT_NAME_DATA,
    HEX_STR_DATA,
    UNIT_COLON_DATA,
    MODE_DATA,
    BGP_COMMUNITY_DATA,             /* BGP. AA:NN */
    BGP_EXT_COMMUNITY_IP_DATA,      /* BGP. A.B.C.D:EF */
    BGP_EXT_COMMUNITY_ASN_DATA,     /* BGP. ABCD::EF | AB::CDEF  */
    MAX_DATA_TYPE
};

/*************************/
/* Enumeration of STATUS */
/*************************/
enum STATUS_DEF_E
{
     DISABLE_STATUS = 0,
     ENABLE_STATUS
};

/********************************/
/* Enhanced editing mode define */
/********************************/
enum EDIT_MODE_E
{
    EDIT_DISABLED,
    EDIT_ENABLED
};


/*******************************/
/* Command history mode define */
/*******************************/
enum HIST_MODE_E
{
    HIST_DISABLED,
    HIST_ENABLED
};

/*******************/
/* Prompt messages */
/*******************/
enum UI_PROMPT_MSG_E
{
   UI_MSG_COPYRIGHT_STR,
   UI_MSG_PRESS_ENTER_STR,
   UI_MSG_KEY_CONFIRM,
   UI_MSG_KEY_SELECT,
   UI_MSG_KEY_EDIT,
   UI_MSG_SET_OPER_CONF,
   UI_MSG_SET_CONF_NO,
   UI_MSG_TFTP_START,
   UI_MSG_TFTP_COMPLETE,
   UI_MSG_XMODEM_PROCESSING,
   UI_MSG_XMODEM_COMPLETE,
   UI_MSG_WAIT_FLASH_WRITE,
   UI_MSG_EXIT_RESTART,
   UI_MSG_EXIT_RECONNECT,
   UI_MSG_EXIT_REST_CNT,
   UI_MSG_EXIT_REST_ENT,
   UI_MSG_EXIT_TIMEOUT,
   UI_MSG_EXIT_LOGIN,
   UI_MSG_EXIT_FAIL_CON,
   UI_MSG_EXIT_IP_CHANGED,
   UI_MSG_EXIT_EEDEF_OK,
   UI_MSG_EXIT_EEDEF_FAIL,
   UI_MSG_EXIT_USER_ACT,
   UI_MSG_EXIT_CONNECT,
   UI_MSG_EXIT_REENTER,

   TOTAL_PROMPT_MSG_NUM
};

#endif
