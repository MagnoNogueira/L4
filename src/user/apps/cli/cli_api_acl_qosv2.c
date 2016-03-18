
/*system*/
#include "sysfun.h"
#include <string.h>
#include <stdlib.h>
#include <sys_type.h>
#include "sys_cpnt.h"
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include "sys_dflt.h"
#include "l_inet.h"
/*cli internal*/
#include "clis.h"
#include "cli_def.h"
#include "cli_mgr.h"
#include "cli_io.h"
#include "cli_api.h"
#include "cli_task.h"
#include "cli_func.h"
#include "cli_main.h"
#include "cli_lib.h"
#include "cli_api.h"
#include "cli_pars.h"
#include "cli_msg.h"
#include "cli_runcfg.h"
#include "sys_adpt.h"
#include "cli_api_acl.h"
#include "cli_tbl.h"

#include "swctrl.h"

#include "l4_pmgr.h"

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
#include "l4_marker_mgr.h"
#include "marker_type.h"
#endif

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
#include "time_range_pmgr.h"
#include "time_range_type.h"
#endif

#if (SYS_CPNT_ACL_IPV6 == TRUE)
//#include "ipnet_port.h"
//#include "ipcom_sock.h"
#endif
#if (SYS_CPNT_DAI == TRUE)
#include "dai_pmgr.h"
#endif /* #if (SYS_CPNT_DAI == TRUE) */

#define CLI_ACL_TEST 0
#define CLI_UNITEST 0
#define MASK255 0xffffffff
#define ALL_ANY 0
#define ANY_MASK 2
#define HOST_ONLY 1
#define HOST_MASK 3

#define CLI_API_ACL_QUERY_IFINDEX_UN_SPEC       0xffffffff
#define CLI_API_ACL_QUERY_ACL_TYPE_UN_SPEC      RULE_TYPE_MAX_NBR_OF_ACL_TYPE

#define ERR_MSG_FREE_SELECTOR_UNAVAILABLE       "Failed to allocate the selector for installing rules.\r\n"

#define CLI_API_SHOW_MATCHES_COUNTER            1L

#ifndef ASSERT
#define ASSERT(eq)
#endif

#define CLI_ACL_PRIV_FLUSH_LINE(string, line_num)               \
    {   line_num = CLI_ACL_Priv_FlushLine(string, line_num);    \
        if (EXIT_SESSION_MORE == line_num ||                    \
            JUMP_OUT_MORE == line_num)                          \
        {                                                       \
            return line_num;                                    \
        }                                                       \
    }

#define CLI_ACL_PRIV_ACL_TYPE_STR(type)                         \
    (                                                           \
        (RULE_TYPE_MAC_ACL == type) ? "MAC" :                   \
        (RULE_TYPE_IP_ACL == type)  ? "IP" :                    \
        (RULE_TYPE_IPV6_ACL == type)? "IPv6" : "Unknown"        \
    )

typedef UI32_T CLI_API_SHOW_OPTION;

typedef enum
{
    CLI_ACL_STRING_OK,
    CLI_ACL_STRING_E_INSUFFICIENT_BUFFER,
} CLI_ACL_STRING_RETURN_T;

typedef struct
{
    char *buff;
    char *cur;

    UI32_T capacity;
    UI32_T used;
} CLI_ACL_STRING_T;

#if (SYS_CPNT_ACL == TRUE)

typedef struct
{
    UI32_T                  ifindex;
    RULE_TYPE_AclType_T     acl_type;
} CLI_API_ACL_QUERY_T;


static CLI_API_EthStatus_T
CLI_ACL_VerifyEthernet(
    UI32_T unit,
    UI32_T port,
    UI32_T *lport
);

static void
CLI_ACL_DisplayEthernetMsg(
    CLI_API_EthStatus_T status,
    UI32_T unit,
    UI32_T port
);

static UI32_T
CLI_ACL_ShowOne_ACL_ACE_List(
    UI32_T type,
    char *name,
    RULE_TYPE_UI_Ace_Entry_T ui_ace,
    CLI_API_SHOW_OPTION option,
    UI32_T line_num
);

static UI32_T
CLI_API_SET_ACL_MAC(
    UI32_T acl_action,
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P
);

static UI32_T
CLI_API_SET_ACL_Extended(
    UI32_T acl_action,
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P
);

static UI32_T
CLI_API_SET_ACL_Standard(
    UI32_T acl_action,
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P
);


static UI32_T
CLI_API_SET_ACL_Extended_IPV6 (
    UI32_T acl_action,
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P
);

static UI32_T
CLI_API_SET_ACL_Standard_IPV6(
    UI32_T acl_action,
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P
);

#if (SYS_CPNT_DAI == TRUE)
static UI32_T
CLI_API_SET_ACL_ARP(
    UI32_T acl_action,
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P
);
#endif

static UI32_T
CLI_ACL_PrivGetMacAddr(
    char *arg[],
    UI32_T *position_p,
    UI8_T sourceAddr[SYS_ADPT_MAC_ADDR_LEN],
    UI8_T destAddr[SYS_ADPT_MAC_ADDR_LEN]
);

static UI32_T
CLI_ACL_PrivGetIpAddr(
    char *arg[],
    UI32_T *position_p,
    UI8_T addr[SYS_ADPT_IPV4_ADDR_LEN],
    UI8_T mask[SYS_ADPT_IPV4_ADDR_LEN]
);

static UI32_T
CLI_ACL_PrivGetVid(
    char *arg[],
    UI32_T *position_p,
    UI32_T *operation_p,
    UI32_T *maxVid_p,
    UI32_T *minVid_p,
    UI32_T *bitmask_p
);

#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
static UI32_T
CLI_ACL_PrivGetL4Port(
    char *arg[],
    UI32_T *position_p,
    RULE_TYPE_UI_Ace_L4_Port_T *l4_port_p
);
#endif /* SYS_CPNT_ACL_MAC_IP_MIX */

#if (SYS_CPNT_ACL_IPV6 == TRUE)
static UI32_T
CLI_ACL_PrivGetIp6Address(
    char *arg[],
    UI32_T *arg_pos_p,
    UI8_T addr[SYS_ADPT_IPV6_ADDR_LEN],
    UI8_T *preflen_p
);
#endif /* SYS_CPNT_ACL_IPV6 */

static UI32_T
CLI_ACL_FillMAC(
    RULE_TYPE_UI_Ace_Entry_T *ace_p,
	char *arg[]
);

static UI32_T
CLI_ACL_FillStandard(
    RULE_TYPE_UI_Ace_Entry_T *ace_p,
    char *arg[]
);

static UI32_T
CLI_ACL_FillExtended(
    RULE_TYPE_UI_Ace_Entry_T *ace_p,
    char *arg[]
);

static UI32_T
CLI_ACL_FillIPv6Standard(
    RULE_TYPE_UI_Ace_Entry_T *ace_p,
    char *arg[]
);

static UI32_T
CLI_ACL_FillIPv6Extended(
    RULE_TYPE_UI_Ace_Entry_T *ace_p,
    char *arg[]
);

#if ( (SYS_CPNT_DAI == TRUE) && (SYS_CPNT_ACL == TRUE) )
static UI32_T
CLI_ACL_FillARP(
    RULE_TYPE_UI_Ace_Entry_T *ace_p,
    char *arg[]
);
#endif /* (SYS_CPNT_DAI == TRUE) && (SYS_CPNT_ACL == TRUE) */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
static UI32_T
CLI_ACL_PrivFillAclName(
    UI32_T *index,
    const char *arg[],
    const char **acl_name
);

static UI32_T
CLI_ACL_PrivFillDirection(
    UI32_T *index,
    const char *arg[],
    RULE_TYPE_InOutDirection_T *direction
);

static UI32_T
CLI_ACL_PrivFillUnitPort(
    UI32_T *index,
    const char *arg[],
    UI32_T *unit,
    UI32_T *port
);
#endif /* SYS_CPNT_ACL_COUNTER */

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
static UI32_T
CLI_ACL_PrivFillTimeRangeName(
    UI32_T *index,
    const char *arg[],
    const char **time_range_name
);
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
static UI32_T
CLI_ACL_PrivFillCounter(
    UI32_T *index,
    const char *arg[],
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable
);
#endif /* SYS_CPNT_ACL_COUNTER */

static UI8_T
CLI_ACL_PrivGetAccessModeFromAclType(
    RULE_TYPE_AclType_T acl_type
);

static BOOL_T
CLI_ACL_PrivEnterAccessListMode(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    CLI_TASK_WorkingArea_T *ctrl_P
);

static UI32_T
CLI_API_PrivAccessGroup_Eth(
    UI16_T cmd_idx,
    const char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P
);

static UI32_T
CLI_API_PrivAccessGroup_Pch(
    UI16_T cmd_idx,
    const char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P
);

static UI32_T
CLI_API_PrivAccessGroup_Global(
    UI16_T cmd_idx,
    const char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P
);

static UI32_T
CLI_ACL_PrivShowAccessGroup(
    const CLI_API_ACL_QUERY_T *acl_filter
);

#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
static UI32_T
CLI_ACL_TIME_RANGE_FillTimeRangeName(
    char *arg[],
    UI8_T *p_time_range_name
);
#endif

void
CLI_ACL_STRING_InitString(
    char *buff,
    UI32_T buff_size,
    CLI_ACL_STRING_T *string)
{
    string->buff = buff;
    string->capacity = buff_size;

    string->used = strlen(buff);

    ASSERT(string->used < string->capacity);

    string->cur = &string->buff[ string->used ];
}

CLI_ACL_STRING_RETURN_T
CLI_ACL_STRING_StrCat(
    CLI_ACL_STRING_T *string,
    const char *str)
{
    UI32_T len = strlen(str);

    if ((1 <= len) && (1 == (string->capacity - string->used)))
    {
        return CLI_ACL_STRING_E_INSUFFICIENT_BUFFER;
    }

    if ((string->capacity - string->used -1) < len)
    {
        return CLI_ACL_STRING_E_INSUFFICIENT_BUFFER;
    }

    strncat(string->cur, str, string->capacity - string->used - 1);

    string->used += len;
    string->cur = &string->buff[ string->used ];

    return CLI_ACL_STRING_OK;
}

CLI_ACL_STRING_RETURN_T
CLI_ACL_STRING_StrCatAtHead(
    CLI_ACL_STRING_T *string,
    const char *str)
{
    UI32_T len = strlen(str);

    if ((1 <= len) && (1 == (string->capacity - string->used)))
    {
        return CLI_ACL_STRING_E_INSUFFICIENT_BUFFER;
    }

    if ((string->capacity - string->used -1) < len)
    {
        return CLI_ACL_STRING_E_INSUFFICIENT_BUFFER;
    }

    memmove(string->buff+len, string->buff, string->capacity - len);
    memcpy(string->buff, str, len);

    string->used += len;
    string->cur = &string->buff[ string->used ];

    return CLI_ACL_STRING_OK;
}

CLI_ACL_STRING_RETURN_T
CLI_ACL_STRING_StrShift(
    CLI_ACL_STRING_T *string,
    UI32_T cch)
{
    if (string->used < cch)
    {
        cch = string->used;
    }

    memmove(string->buff, string->buff+cch, string->capacity - cch);

    string->used -= cch;
    string->cur = &string->buff[ string->used ];

    return CLI_ACL_STRING_OK;
}

static UI32_T
CLI_ACL_Priv_FlushLine(
    CLI_ACL_STRING_T *string,
    UI32_T line_num)
{
    enum
    {
        EOL = sizeof("\r\n")-1
    };

    CLI_TASK_WorkingArea_T *ctrl_P;
    UI32_T width;

    char  buff[CLI_DEF_MAX_BUFSIZE] = {0};

    ctrl_P = CLI_TASK_GetMyWorkingArea();

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    width = ctrl_P->CMenu.width;
#else
    width = CLI_DEF_DISP_WIDTH;
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    while (width+EOL < string->used)
    {
        memcpy(buff, string->buff, width);
        buff[width]   = '\r';
        buff[width+1] = '\n';
        buff[width+2] = '\0';

        CLI_ACL_STRING_StrShift(string, width);
        CLI_ACL_STRING_StrCatAtHead(string, "  ");

        PROCESS_MORE_FUNC(buff);
    }

    return line_num;
}


void CLI_ACL_Check_AnyIPV6 (char *buff, UI8_T *ipv6, UI32_T pfx_len)
{
#if (SYS_CPNT_ACL_IPV6 == TRUE)
    UI32_T  i, ret =0;
    char   ipv6_addr_str[L_INET_MAX_IP6ADDR_STR_LEN+1] = {0};

    ASSERT(buff != NULL);
    ASSERT(ipv6 != NULL);

    buff[0] = '\0';

    for ( i =0; i < SYS_ADPT_IPV6_ADDR_LEN; i++ )
    {
        if ( ipv6 [i] != 0 )
            ret =1;
    }

    if ( ret == 0 && pfx_len == 0)
        strcat (buff, " any");
    else
    {
        L_INET_Ntop(L_INET_AF_INET6, ipv6, ipv6_addr_str, sizeof(ipv6_addr_str));

        /* may need to improve for dst-prefix-length */
        if ( pfx_len == RULE_TYPE_MAX_IPV6_PREFIX_LEN )
        {
            strcat (buff, " host ");
            strcat (buff,  ipv6_addr_str);
        }
        else
        {   /* ipv6-adr/prefix-len */
            strcat (buff, " " );
            strcat (buff,  ipv6_addr_str);

            sprintf(ipv6_addr_str, "/%lu", pfx_len);
            strcat(buff,  ipv6_addr_str);
        }
    }
#endif
}

UI32_T CLI_API_AccessList_IPV6 (UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACL_IPV6 == TRUE)
    UI32_T  acl_mode = PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE;
    UI32_T  acl_type = RULE_TYPE_IPV6_STD_ACL;
    char   acl_mode_str [12];

    switch (cmd_idx )
    {
    case PRIVILEGE_CFG_GLOBAL_CMD_W3_ACCESSLIST_IPV6_EXTENDED:
    case PRIVILEGE_CFG_GLOBAL_CMD_W4_NO_ACCESSLIST_IPV6_EXTENDED:

        acl_mode = PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE;
        acl_type = RULE_TYPE_IPV6_EXT_ACL;
        strcpy (acl_mode_str, "extended");
        break;

    case PRIVILEGE_CFG_GLOBAL_CMD_W3_ACCESSLIST_IPV6_STANDARD:
    case PRIVILEGE_CFG_GLOBAL_CMD_W4_NO_ACCESSLIST_IPV6_STANDARD:

        acl_mode = PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE;
        acl_type = RULE_TYPE_IPV6_STD_ACL;
        strcpy (acl_mode_str, "standard");
        break;
    }

    switch ( cmd_idx )
    {
    case PRIVILEGE_CFG_GLOBAL_CMD_W3_ACCESSLIST_IPV6_EXTENDED:
    case PRIVILEGE_CFG_GLOBAL_CMD_W3_ACCESSLIST_IPV6_STANDARD:

        CLI_ACL_PrivEnterAccessListMode(arg[0], acl_type, ctrl_P);

        break;

    case PRIVILEGE_CFG_GLOBAL_CMD_W4_NO_ACCESSLIST_IPV6_EXTENDED:
    case PRIVILEGE_CFG_GLOBAL_CMD_W4_NO_ACCESSLIST_IPV6_STANDARD:

        if ( L4_PMGR_ACL_DelAclByNameAndAclType(arg[0], acl_type) == RULE_TYPE_FAIL )
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            CLI_LIB_PrintStr_1 ("Failed to delete IPv6 %s access-list.\r\n", acl_mode_str);
#endif
        }

        break;

    default:
        break;
    }
#endif /* SYS_CPNT_ACL_IPV6 */

    return CLI_NO_ERROR;
}

UI32_T CLI_API_Deny_ACL_Extended_IPV6 (UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    return CLI_API_SET_ACL_Extended_IPV6 (RULE_TYPE_ACE_DENY, cmd_idx, arg, ctrl_P);
}

UI32_T CLI_API_Deny_ACL_Standard_IPV6 (UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    return CLI_API_SET_ACL_Standard_IPV6 (RULE_TYPE_ACE_DENY, cmd_idx, arg, ctrl_P);
}

UI32_T CLI_API_Permit_ACL_Extended_IPV6 (UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    return CLI_API_SET_ACL_Extended_IPV6 (RULE_TYPE_ACE_PERMIT, cmd_idx, arg, ctrl_P);
}

UI32_T CLI_API_Permit_ACL_Standard_IPV6 (UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    return CLI_API_SET_ACL_Standard_IPV6 (RULE_TYPE_ACE_PERMIT, cmd_idx, arg, ctrl_P);
}

static UI32_T
CLI_API_SET_ACL_Extended_IPV6 (
    UI32_T acl_action,
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P)
{
    /*
     *  deny/permit [ mac {SA SABitmask DA DABitmask} [vid value bitmask] ]
     *              { any | source-ipv6-prefix/prefix-length | host source-ipv6-address }
     *              { any | destination-ipv6-prefix/prefix-length | host destination-ipv6-address}
     *              [dscp value] [flow-label value] [next-header value] [time-range name]
     */

    UI32_T  cli_ret = CLI_NO_ERROR;

#if (SYS_CPNT_ACL_IPV6 == TRUE)
    RULE_TYPE_UI_Ace_Entry_T ace;

    ace.ace_type = RULE_TYPE_IPV6_EXT_ACL;
    L4_PMGR_ACL_InitUIAce(&ace);
    ace.access = acl_action;

    if (CLI_NO_ERROR != CLI_ACL_FillIPv6Extended(&ace, arg))
    {
        return CLI_ERR_INTERNAL;
    }

    if ( cli_ret == CLI_NO_ERROR )
    {
        switch ( cmd_idx )
        {
            case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_CMD_W1_DENY:
            case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_CMD_W1_PERMIT:
                switch (L4_PMGR_ACL_SetUIAce2Acl(ctrl_P->CMenu.acl_name, ctrl_P->CMenu.acl_type, &ace))
                {
                    case RULE_TYPE_OK:
                        break;

                    case RULE_TYPE_FREE_SELECTOR_UNAVAILABLE:
#if (SYS_CPNT_EH == TRUE)
                        CLI_API_Show_Exception_Handeler_Msg ();
#else
                        CLI_LIB_PrintStr (ERR_MSG_FREE_SELECTOR_UNAVAILABLE);
#endif
                        break;

                    default:
#if (SYS_CPNT_EH == TRUE)
                        CLI_API_Show_Exception_Handeler_Msg ();
#else
                        CLI_LIB_PrintStr("Failed to set IPv6 extended ACE.\r\n");
#endif
                        break;
                    }
                    break;

            case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_CMD_W2_NO_DENY:
            case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_CMD_W2_NO_PERMIT:

                if (L4_PMGR_ACL_DelUIAceFromAcl(ctrl_P->CMenu.acl_name, ctrl_P->CMenu.acl_type, &ace) != RULE_TYPE_OK)
                {
    #if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg ();
    #else
                    CLI_LIB_PrintStr("Failed to delete IPv6 extended ACE.\r\n");
    #endif
                }

                break;
        }
    }
#endif /* SYS_CPNT_ACL_IPV6 */

    return cli_ret;
}

static UI32_T
CLI_API_SET_ACL_Standard_IPV6 (
    UI32_T acl_action,
    UI16_T cmd_idx,
    char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    /*
     *  [no] {deny | permit} {any | source-ipv6-prefix/prefix-length | host source-ipv6-address}
     *                       [time-range name]
     */
    UI32_T          cli_ret = CLI_NO_ERROR;

#if (SYS_CPNT_ACL_IPV6 == TRUE)
    RULE_TYPE_UI_Ace_Entry_T ace;

    ace.ace_type = RULE_TYPE_IPV6_STD_ACL;
    L4_PMGR_ACL_InitUIAce (&ace);
    ace.access = acl_action;

    if ( CLI_NO_ERROR != CLI_ACL_FillIPv6Standard(&ace, arg))
    {
        return CLI_ERR_INTERNAL;
    }

    if ( cli_ret == CLI_NO_ERROR )
    {
        switch ( cmd_idx )
        {
            case PRIVILEGE_CFG_ACL_STANDARD_IPV6_CMD_W1_DENY:
            case PRIVILEGE_CFG_ACL_STANDARD_IPV6_CMD_W1_PERMIT:
                switch (L4_PMGR_ACL_SetUIAce2Acl(ctrl_P->CMenu.acl_name, ctrl_P->CMenu.acl_type, &ace))
                {
                case RULE_TYPE_OK:
                    break;

                case RULE_TYPE_FREE_SELECTOR_UNAVAILABLE:
#if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg ();
#else
                    CLI_LIB_PrintStr (ERR_MSG_FREE_SELECTOR_UNAVAILABLE);
#endif
                    break;

                default:
#if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg ();
#else
                    CLI_LIB_PrintStr("Failed to set IPv6 standard ACE.\r\n");
#endif
                    break;
                }
                break;

            case PRIVILEGE_CFG_ACL_STANDARD_IPV6_CMD_W2_NO_DENY:
            case PRIVILEGE_CFG_ACL_STANDARD_IPV6_CMD_W2_NO_PERMIT:

                if (L4_PMGR_ACL_DelUIAceFromAcl(ctrl_P->CMenu.acl_name, ctrl_P->CMenu.acl_type, &ace) != RULE_TYPE_OK)
                {
#if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg ();
#else
                    CLI_LIB_PrintStr("Failed to delete IPv6 standard ACE.\r\n");
#endif
                }

                break;
        }
    }
#endif /* SYS_CPNT_ACL_IPV6 */

    return cli_ret;
}

UI32_T CLI_API_AccessList_IP_Extended(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACL == TRUE)

    switch(cmd_idx)
    {
    case PRIVILEGE_CFG_GLOBAL_CMD_W3_ACCESSLIST_IP_EXTENDED:

        CLI_ACL_PrivEnterAccessListMode(arg[0], RULE_TYPE_IP_EXT_ACL, ctrl_P);

        break;

    case PRIVILEGE_CFG_GLOBAL_CMD_W4_NO_ACCESSLIST_IP_EXTENDED:

        if ( L4_PMGR_ACL_DelAclByNameAndAclType(arg[0], RULE_TYPE_IP_EXT_ACL) == RULE_TYPE_FAIL )
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg ();
#else
            CLI_LIB_PrintStr("Failed to delete IP extended access-list.\r\n");
#endif
        }
        break;

    default:
        break;
    }
#endif

    return CLI_NO_ERROR;
}

UI32_T CLI_API_AccessList_IP_Standard(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACL == TRUE)

    switch(cmd_idx)
    {
    case PRIVILEGE_CFG_GLOBAL_CMD_W3_ACCESSLIST_IP_STANDARD:

        CLI_ACL_PrivEnterAccessListMode(arg[0], RULE_TYPE_IP_STD_ACL, ctrl_P);

        break;

    case PRIVILEGE_CFG_GLOBAL_CMD_W4_NO_ACCESSLIST_IP_STANDARD:

        if ( L4_PMGR_ACL_DelAclByNameAndAclType(arg[0], RULE_TYPE_IP_STD_ACL) == RULE_TYPE_FAIL )
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg ();
#else
            CLI_LIB_PrintStr("Failed to delete IP standard access-list.\r\n");
#endif
        }

        break;

    default:
        break;
    }
#endif

    return CLI_NO_ERROR;
}

UI32_T CLI_API_ACCESSLIST_MAC(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACL == TRUE)

    switch(cmd_idx)
    {
    case PRIVILEGE_CFG_GLOBAL_CMD_W2_ACCESSLIST_MAC:

        CLI_ACL_PrivEnterAccessListMode(arg[0], RULE_TYPE_MAC_ACL, ctrl_P);

        break;

    case PRIVILEGE_CFG_GLOBAL_CMD_W3_NO_ACCESSLIST_MAC:

        if ( L4_PMGR_ACL_DelAclByNameAndAclType(arg[0], RULE_TYPE_MAC_ACL) == RULE_TYPE_FAIL )
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg ();
#else
            CLI_LIB_PrintStr("Failed to delete MAC access-list.\r\n");
#endif
        }

        break;

    default:
        break;
    }
#endif

    return CLI_NO_ERROR;
}

UI32_T CLI_API_AccessList_Arp(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if ( (SYS_CPNT_DAI == TRUE) && (SYS_CPNT_ACL == TRUE) )

    switch(cmd_idx)
    {
        case PRIVILEGE_CFG_GLOBAL_CMD_W2_ACCESSLIST_ARP:

            CLI_ACL_PrivEnterAccessListMode(arg[0], RULE_TYPE_ARP_ACL, ctrl_P);

            break;

        case PRIVILEGE_CFG_GLOBAL_CMD_W3_NO_ACCESSLIST_ARP:
            if ( L4_PMGR_ACL_DelAclByNameAndAclType(arg[0], RULE_TYPE_ARP_ACL) == RULE_TYPE_FAIL )
            {
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg ();
#else
                CLI_LIB_PrintStr("Failed to delete ARP access-list.\r\n");
#endif
            }
            else
            {
                DAI_PMGR_ClearVlanFilterByAclName((UI8_T *)arg[0]);
            }

            break;

        default:
            break;
    }
#endif /* end of #if ( (SYS_CPNT_DAI == TRUE) && (SYS_CPNT_ACL == TRUE) ) */

    return CLI_NO_ERROR;
}

UI32_T CLI_API_IP_AccessGroup(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T result = CLI_NO_ERROR;

#if (SYS_CPNT_ACL == TRUE)
    result = CLI_API_PrivAccessGroup_Eth(cmd_idx, (const char **) arg, ctrl_P);
#endif /* SYS_CPNT_ACL */

   return CLI_NO_ERROR;
}

UI32_T CLI_API_MAC_AccessGroup_Eth(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T result = CLI_NO_ERROR;

#if (SYS_CPNT_ACL == TRUE)
    result = CLI_API_PrivAccessGroup_Eth(cmd_idx, (const char **) arg, ctrl_P);
#endif /* SYS_CPNT_ACL */

    return result;
}

UI32_T CLI_API_IP_AccessGroup_Pch(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T result = CLI_NO_ERROR;

#if (SYS_CPNT_ACL == TRUE)
    result = CLI_API_PrivAccessGroup_Pch(cmd_idx, (const char **) arg, ctrl_P);
#endif /* SYS_CPNT_ACL */
    return result;
}

UI32_T CLI_API_MAC_AccessGroup_Pch(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T result = CLI_NO_ERROR;

#if (SYS_CPNT_ACL == TRUE)
    result = CLI_API_PrivAccessGroup_Pch(cmd_idx, (const char **) arg, ctrl_P);
#endif /* SYS_CPNT_ACL */
    return result;
}

UI32_T CLI_API_IP_AccessGroup_Global(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T result = CLI_NO_ERROR;

#if (SYS_CPNT_ACL == TRUE)
    result = CLI_API_PrivAccessGroup_Global(cmd_idx, (const char **) arg, ctrl_P);
#endif /* SYS_CPNT_ACL */
    return result;
}

UI32_T CLI_API_MAC_AccessGroup_Global(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T result = CLI_NO_ERROR;

#if (SYS_CPNT_ACL == TRUE)
    result = CLI_API_PrivAccessGroup_Global(cmd_idx, (const char **) arg, ctrl_P);
#endif /* SYS_CPNT_ACL */
    return result;
}

UI32_T CLI_API_Deny_ACL_MAC(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    return CLI_API_SET_ACL_MAC (RULE_TYPE_ACE_DENY, cmd_idx, arg, ctrl_P);
}

UI32_T CLI_API_Deny_ACL_Standard(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    return CLI_API_SET_ACL_Standard (RULE_TYPE_ACE_DENY, cmd_idx, arg, ctrl_P);
}

UI32_T CLI_API_Deny_ACL_Extended(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    return CLI_API_SET_ACL_Extended (RULE_TYPE_ACE_DENY, cmd_idx, arg, ctrl_P);
}

UI32_T CLI_API_Deny_ACL_ARP(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_DAI == TRUE)
    return CLI_API_SET_ACL_ARP(RULE_TYPE_ACE_DENY, cmd_idx, arg, ctrl_P);
#else
    return CLI_NO_ERROR;
#endif
}

UI32_T CLI_API_Permit_ACL_MAC(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    return CLI_API_SET_ACL_MAC (RULE_TYPE_ACE_PERMIT, cmd_idx, arg, ctrl_P);
}

UI32_T CLI_API_Permit_ACL_Standard(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    return CLI_API_SET_ACL_Standard (RULE_TYPE_ACE_PERMIT, cmd_idx, arg, ctrl_P);
}

UI32_T CLI_API_Permit_ACL_Extended(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    return CLI_API_SET_ACL_Extended (RULE_TYPE_ACE_PERMIT, cmd_idx, arg, ctrl_P);
}
UI32_T CLI_API_Permit_ACL_ARP(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_DAI == TRUE)
    return CLI_API_SET_ACL_ARP(RULE_TYPE_ACE_PERMIT, cmd_idx, arg, ctrl_P);
#else
    return CLI_NO_ERROR;
#endif
}

UI32_T CLI_API_Show_AccessGroup(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    CLI_API_ACL_QUERY_T query;

    query.ifindex = CLI_API_ACL_QUERY_IFINDEX_UN_SPEC;
    query.acl_type = CLI_API_ACL_QUERY_ACL_TYPE_UN_SPEC;

    CLI_ACL_PrivShowAccessGroup(&query);

    return CLI_NO_ERROR;
}

UI32_T CLI_API_Show_AccessList(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACL == TRUE)
    UI32_T  i = 0;
    UI32_T  line_num = 1;
    UI32_T  acl_index;
    UI32_T  ace_index;

    RULE_TYPE_UI_AclEntry_T acl_entry;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    char                    buff[CLI_DEF_MAX_BUFSIZE] = {0};

    for (i=0; i < RULE_TYPE_MAX_NBR_OF_ACL_TYPE; ++i)
    {
        memset (&acl_entry, 0, sizeof(acl_entry));
        acl_index = 0;
        acl_entry.acl_type = i;

        while ( L4_PMGR_ACL_GetNextAcl (&acl_index, &acl_entry) == RULE_TYPE_OK )
        {
            if (RULE_TYPE_IP_STD_ACL == acl_entry.acl_type)
            {
                SYSFUN_Snprintf(buff, sizeof(buff), "IP standard access-list %s:\r\n", acl_entry.acl_name);
                buff[sizeof(buff)-1] = '\0';
            }
            else if (RULE_TYPE_IP_EXT_ACL == acl_entry.acl_type)
            {
                SYSFUN_Snprintf(buff, sizeof(buff), "IP extended access-list %s:\r\n", acl_entry.acl_name);
                buff[sizeof(buff)-1] = '\0';
            }
            else if (RULE_TYPE_MAC_ACL == acl_entry.acl_type)
            {
                SYSFUN_Snprintf(buff, sizeof(buff), "MAC access-list %s:\r\n", acl_entry.acl_name);
                buff[sizeof(buff)-1] = '\0';
            }
#if (SYS_CPNT_ACL_IPV6 == TRUE)
            else if (RULE_TYPE_IPV6_STD_ACL == acl_entry.acl_type)
            {
                SYSFUN_Snprintf(buff, sizeof(buff), "IPv6 standard access-list %s:\r\n", acl_entry.acl_name);
                buff[sizeof(buff)-1] = '\0';
            }
            else if (RULE_TYPE_IPV6_EXT_ACL == acl_entry.acl_type)
            {
                SYSFUN_Snprintf(buff, sizeof(buff), "IPv6 extended access-list %s:\r\n", acl_entry.acl_name);
                buff[sizeof(buff)-1] = '\0';
            }
#endif /* SYS_CPNT_ACL_IPV6 */

#if (SYS_CPNT_DAI == TRUE)
            else if (RULE_TYPE_ARP_ACL == acl_entry.acl_type)
            {
                SYSFUN_Snprintf(buff, sizeof(buff), "ARP access-list %s:\r\n", acl_entry.acl_name);
                buff[sizeof(buff)-1] = '\0';
            }
#endif /* SYS_CPNT_DAI */
            else
            {
                continue;
            }

            /* ACL name
             */
            PROCESS_MORE(buff);

            memset (&ace, 0, sizeof(ace));
            ace_index = 0;

            while (L4_PMGR_ACL_GetNextUIAceByAcl(acl_index, &ace_index, &ace) == RULE_TYPE_OK)
            {
                line_num = CLI_ACL_ShowOne_ACL_ACE_List(i, acl_entry.acl_name,
                                                        ace,
                                                        0,
                                                        line_num);

                if (line_num == JUMP_OUT_MORE)
                {
                    return CLI_NO_ERROR;
                }
                else if (line_num == EXIT_SESSION_MORE)
                {
                    return CLI_EXIT_SESSION;
                }
            }
        }
    }
#endif

    return CLI_NO_ERROR;
}

UI32_T CLI_API_Show_Arp_AccessList(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if ( (SYS_CPNT_DAI == TRUE) && (SYS_CPNT_ACL == TRUE) )
    UI32_T  line_num = 0;
    UI32_T  ace_index;
    UI32_T  acl_index =0;
    char   tmp_buffer_output[200];
    RULE_TYPE_UI_Ace_Entry_T ace;
    RULE_TYPE_UI_AclEntry_T acl_entry;
    char buff[1024];
    BOOL_T is_show_all_acl = FALSE;



#if (SYS_CPNT_CLI_FILTERING == TRUE)
    if(arg[0]!=NULL)
    {
        if(arg[0][0]=='|')
        {
            switch(arg[1][0])
            {
                case 'b':
                case 'B':
                    ctrl_P->option_flag=CLI_API_OPTION_BEGIN;
                    break;
                case 'e':
                case 'E':
                    ctrl_P->option_flag=CLI_API_OPTION_EXCLUDE;
                    break;

                case 'i':
                case 'I':
                    ctrl_P->option_flag=CLI_API_OPTION_INCLUDE;
                    break;
                default:
                    return CLI_ERR_INTERNAL;
            }
            strcpy(ctrl_P->option_buf,arg[2]);
        }
    }

    if( arg[1]!=NULL )
    {
        if(arg[1][0]=='|')
        {
            switch(arg[2][0])
            {
                case 'b':
                case 'B':
                    ctrl_P->option_flag=CLI_API_OPTION_BEGIN;
                    break;
                case 'e':
                case 'E':
                    ctrl_P->option_flag=CLI_API_OPTION_EXCLUDE;
                    break;

                case 'i':
                case 'I':
                    ctrl_P->option_flag=CLI_API_OPTION_INCLUDE;
                    break;
                default:
                    return CLI_ERR_INTERNAL;
            }
            strcpy(ctrl_P->option_buf,arg[3]);
        }
    }
#endif /*#if (SYS_CPNT_CLI_FILTERING == TRUE)*/

    if (arg[0] == NULL ||arg[0][0]=='|')
    {
        is_show_all_acl = TRUE;
    }
    else
    {
        if ( L4_PMGR_ACL_GetAclIdByName(arg [0],  &acl_index) != RULE_TYPE_OK )
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg ();
#else
            CLI_LIB_PrintStr_1 ("Access-list %s does not exist.\r\n", arg[0]);
#endif
            return CLI_NO_ERROR;
        }

        strcpy (acl_entry.acl_name, arg[0]);
    }

    acl_entry.acl_type = RULE_TYPE_ARP_ACL;
    while(1)
    {
        if (is_show_all_acl)
        {
            if (L4_PMGR_ACL_GetNextAcl (&acl_index, &acl_entry) != RULE_TYPE_OK)
            {
                break;
            }
            else
            {
                if ( acl_entry.acl_type != RULE_TYPE_ARP_ACL )
                {
                    continue;
                }
            }
        }

        sprintf (tmp_buffer_output, "ARP access-list %s:\r\n", acl_entry.acl_name);
        PROCESS_MORE(tmp_buffer_output);
        memset (&ace, 0, sizeof(ace));
        ace_index = 0;

        while (L4_PMGR_ACL_GetNextUIAceByAcl(acl_index, &ace_index, &ace) == RULE_TYPE_OK)
        {
            line_num = CLI_ACL_ShowOne_ACL_ACE_List(acl_entry.acl_type,
                                                    acl_entry.acl_name,
                                                    ace,
                                                    0,
                                                    line_num);
            if (line_num == JUMP_OUT_MORE)
            {
                is_show_all_acl = FALSE;
                break;
            }
            else if (line_num == EXIT_SESSION_MORE)
            {
                return CLI_EXIT_SESSION;
            }
        }


        /* if only show one ACL
         */
        if (!is_show_all_acl)
        {
            break;
        }
    }/*end of while get next ACL*/
#endif /* end of #if ( (SYS_CPNT_DAI == TRUE) && (SYS_CPNT_ACL == TRUE) ) */

    return CLI_NO_ERROR;
}

UI32_T CLI_API_Show_IP_AccessGroup(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    CLI_API_ACL_QUERY_T query = {0};

    query.ifindex  = CLI_API_ACL_QUERY_IFINDEX_UN_SPEC;
    query.acl_type = RULE_TYPE_IP_ACL;

#if (SYS_CPNT_ACL_IPV6 == TRUE)
    if (PRIVILEGE_EXEC_CMD_W3_SHOW_IPV6_ACCESSGROUP == cmd_idx ||
        PRIVILEGE_EXEC_CMD_W3_SHOW_ACCESSGROUP_IPV6 == cmd_idx)
    {
        query.acl_type = RULE_TYPE_IPV6_ACL;
    }
#endif /* #if (SYS_CPNT_ACL_IPV6 == TRUE) */

    CLI_ACL_PrivShowAccessGroup(&query);

    return CLI_NO_ERROR;
}

UI32_T CLI_API_Show_IP_AccessList(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACL == TRUE)
    UI32_T line_num = 0;

    UI32_T                   ace_index;
    UI32_T                   acl_index =0;

    RULE_TYPE_UI_Ace_Entry_T ace;
    RULE_TYPE_AclType_T      acl_type = RULE_TYPE_MAC_ACL;

    RULE_TYPE_UI_AclEntry_T  acl_entry;

    BOOL_T                   is_show_all_acl = FALSE;

    switch(arg[0][0])/*check acl type*/
    {
    case 'e':
    case 'E':
        if ((PRIVILEGE_EXEC_CMD_W3_SHOW_IP_ACCESSLIST == cmd_idx) ||
            (PRIVILEGE_EXEC_CMD_W3_SHOW_ACCESSLIST_IP == cmd_idx))
        {
            acl_type = RULE_TYPE_IP_EXT_ACL;
        }
#if (SYS_CPNT_ACL_IPV6 == TRUE)
        else
        {
            acl_type = RULE_TYPE_IPV6_EXT_ACL;
        }
#endif /* SYS_CPNT_ACL_IPV6 */

        break;

    case 's':
    case 'S':
        if ((PRIVILEGE_EXEC_CMD_W3_SHOW_IP_ACCESSLIST == cmd_idx) ||
            (PRIVILEGE_EXEC_CMD_W3_SHOW_ACCESSLIST_IP == cmd_idx))
        {
            acl_type = RULE_TYPE_IP_STD_ACL;
        }
#if (SYS_CPNT_ACL_IPV6 == TRUE)
        else
        {
            acl_type = RULE_TYPE_IPV6_STD_ACL;
        }
#endif /* SYS_CPNT_ACL_IPV6 */
        break;

    default:
        return CLI_NO_ERROR;
    }

    if (arg[1] == NULL)
    {
        is_show_all_acl = TRUE;
    }
    else
    {
        if ( L4_PMGR_ACL_GetAclIdByNameAndType(arg[1], acl_type, &acl_index) != RULE_TYPE_OK )
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg();
#else
            char   err_str[] = "%s access-list %s does not exist.\r\n";

            switch ( acl_type )
            {
            case RULE_TYPE_IP_EXT_ACL:
                CLI_LIB_PrintStr_2 (err_str, "IP extended", arg [1]);
                break;

            case RULE_TYPE_IP_STD_ACL:
                CLI_LIB_PrintStr_2 (err_str, "IP standard", arg [1]);
                break;

    #if (SYS_CPNT_ACL_IPV6 == TRUE)

            case RULE_TYPE_IPV6_EXT_ACL:
                CLI_LIB_PrintStr_2 (err_str, "IPv6 extended", arg [1]);
                break;

            case RULE_TYPE_IPV6_STD_ACL:
                CLI_LIB_PrintStr_2 (err_str, "IPv6 standard", arg [1]);
                break;
    #endif /* SYS_CPNT_ACL_IPV6 */

            default:
                break;
            }

#endif /* #if (SYS_CPNT_EH == TRUE) */
            return CLI_NO_ERROR;
        }

        strcpy (acl_entry.acl_name,  arg [1]);
    }

    acl_entry.acl_type = acl_type;
    while (1)
    {
        if ( is_show_all_acl )
        {
            if ( L4_PMGR_ACL_GetNextAcl (&acl_index, &acl_entry) != RULE_TYPE_OK )
            {
                break;
            }
            else
            {
                if ( acl_entry.acl_type == acl_type )
                {
                    line_num++;
                }
                else continue;
            }
        }
        else
            line_num = 1;

        switch ( acl_entry.acl_type )
        {
        case RULE_TYPE_IP_EXT_ACL:
            CLI_LIB_PrintStr_1 ("IP extended access-list %s:\r\n", acl_entry.acl_name);
            break;

        case RULE_TYPE_IP_STD_ACL:
            CLI_LIB_PrintStr_1 ("IP standard access-list %s:\r\n", acl_entry.acl_name);
            break;

#if (SYS_CPNT_ACL_IPV6 == TRUE)

        case RULE_TYPE_IPV6_EXT_ACL:
            CLI_LIB_PrintStr_1 ("IPv6 extended access-list %s:\r\n", acl_entry.acl_name);
            break;

        case RULE_TYPE_IPV6_STD_ACL:
            CLI_LIB_PrintStr_1 ("IPv6 standard access-list %s:\r\n", acl_entry.acl_name);
            break;
#endif /* SYS_CPNT_ACL_IPV6 */

        default:
            break;
        }

        memset (&ace, 0, sizeof(ace));


        ace_index = 0;
        while (L4_PMGR_ACL_GetNextUIAceByAcl(acl_index, &ace_index, &ace) == RULE_TYPE_OK)
        {
            line_num = CLI_ACL_ShowOne_ACL_ACE_List(acl_entry.acl_type,
                                                    acl_entry.acl_name,
                                                    ace,
                                                    0,
                                                    line_num);

            if (line_num == JUMP_OUT_MORE)
            {
                is_show_all_acl = FALSE;
                break;
            }
            else if (line_num == EXIT_SESSION_MORE)
            {
                return CLI_EXIT_SESSION;
            }
        }

        if (!is_show_all_acl)/*if only show one ACL*/
            break;

    }/*end of while get next ACL*/
#endif

    return CLI_NO_ERROR;
}

UI32_T CLI_API_Show_MAC_AccessGroup(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    CLI_API_ACL_QUERY_T query = {0};

    query.ifindex  = CLI_API_ACL_QUERY_IFINDEX_UN_SPEC;
    query.acl_type = RULE_TYPE_MAC_ACL;

    CLI_ACL_PrivShowAccessGroup(&query);

    return CLI_NO_ERROR;
}

UI32_T CLI_API_Show_MAC_AccessList(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACL == TRUE)
    UI32_T  line_num = 0;
    UI32_T  ace_index;
    UI32_T  acl_index =0;

    RULE_TYPE_UI_AclEntry_T  acl_entry;
    RULE_TYPE_UI_Ace_Entry_T ace;

    BOOL_T is_show_all_acl = FALSE;

    if (arg[0] == NULL)
    {
        is_show_all_acl = TRUE;
    }
    else
    {
        if ( L4_PMGR_ACL_GetAclIdByNameAndType(arg[0], RULE_TYPE_MAC_ACL, &acl_index) != RULE_TYPE_OK )
        {
#if (SYS_CPNT_EH == TRUE)
            CLI_API_Show_Exception_Handeler_Msg ();
#else
            CLI_LIB_PrintStr_1 ("MAC access-list %s does not exist.\r\n", arg[0]);
#endif
            return CLI_NO_ERROR;
        }

        strcpy (acl_entry.acl_name,  arg [0]);
    }

    acl_entry.acl_type = RULE_TYPE_MAC_ACL;
    while(1)
    {
        if (is_show_all_acl)
        {
            if (L4_PMGR_ACL_GetNextAcl (&acl_index, &acl_entry) != RULE_TYPE_OK)
            {
                break;
            }
            else
            {
                if ( acl_entry.acl_type == RULE_TYPE_MAC_ACL )
                {
                    line_num++;
                }
                else continue;
            }
        }
        else
            line_num = 1;

        CLI_LIB_PrintStr_1 ("MAC access-list %s:\r\n", acl_entry.acl_name);

        memset (&ace, 0, sizeof(ace));
        ace_index = 0;

        while (L4_PMGR_ACL_GetNextUIAceByAcl(acl_index, &ace_index, &ace) == RULE_TYPE_OK)
        {

            line_num = CLI_ACL_ShowOne_ACL_ACE_List(acl_entry.acl_type,
                                                    acl_entry.acl_name,
                                                    ace,
                                                    0,
                                                    line_num);
            if (line_num == JUMP_OUT_MORE)
            {
                is_show_all_acl = FALSE;
                break;
            }
            else if (line_num == EXIT_SESSION_MORE)
            {
                return CLI_EXIT_SESSION;
            }
        }

        if (!is_show_all_acl)/*if only show one ACL*/
            break;
    }/*end of while get next ACL*/
#endif

    return CLI_NO_ERROR;
}

/* show access-list hardware counter
 */
UI32_T
CLI_API_Show_AccessList_Hardware_Counters(
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T                      plist_index;
    RULE_TYPE_InOutDirection_T  inout_profile;

    UI32_T                      acl_index;

    RULE_TYPE_UI_AclEntry_T     ui_acl_entry;
    UI32_T                      precedence;

    RULE_TYPE_COUNTER_ENABLE_T  counter_enable;

    UI32_T                      line_num = 0;

    CLI_API_SHOW_OPTION option = CLI_API_SHOW_MATCHES_COUNTER;

    BOOL_T                      show_acl_name;
    BOOL_T                      show_ext_space_line;

    char                        buff[CLI_DEF_MAX_BUFSIZE] = {0};

    show_ext_space_line = FALSE;

    for (plist_index = 0; plist_index < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ plist_index)
    {
        UI32_T ifindex;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(plist_index);

        for (inout_profile = RULE_TYPE_INBOUND; inout_profile <= RULE_TYPE_OUTBOUND;
                                                                ++inout_profile)
        {
            acl_index = 0;
            precedence = 0;

            while (RULE_TYPE_OK == L4_PMGR_ACL_GetNextAclByPort(ifindex,
                                                                inout_profile,
                                                                &acl_index,
                                                                &ui_acl_entry,
                                                                &precedence,
                                                                NULL,
                                                                &counter_enable) )
            {
                UI32_T ace_index;
                RULE_TYPE_UI_Ace_Entry_T    ace;

                show_acl_name = TRUE;
                ace_index = 0;

                while ( RULE_TYPE_OK == L4_PMGR_ACL_GetNextHardwareUIAceByAcl(
                                                                 ifindex,
                                                                 inout_profile,
                                                                 acl_index,
                                                                 &ace_index,
                                                                 &ace) )
                {
                    if (TRUE == show_acl_name)
                    {
                        if (TRUE == show_ext_space_line)
                        {
                            sprintf(buff, "\r\n");
                            PROCESS_MORE(buff);;
                        }

                        if (RULE_TYPE_IS_UPORT(ifindex))
                        {
                            UI32_T unit, port, trunk_id;

                            SWCTRL_PMGR_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);

                            sprintf(buff, "Interface ethernet %lu/%lu\r\n", unit, port);
                        }
                        else if (RULE_TYPE_IS_TRUNK(ifindex))
                        {
                            UI32_T unit, port, trunk_id;

                            SWCTRL_PMGR_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);

                            sprintf(buff, "Interface port-channel %lu\r\n", trunk_id);
                        }
                        else if (RULE_TYPE_IS_CPU_PORT(ifindex))
                        {
                            sprintf(buff, "Interface control-plan\r\n");
                        }
                        else if (RULE_TYPE_IS_ALL_PORTS(ifindex))
                        {
                            sprintf(buff, "Global\r\n");
                        }
                        else
                        {
                            /* ???
                             */
                            continue;
                        }

                        PROCESS_MORE(buff);

                    	sprintf(buff, "Access-list %s %s\r\n",
                                ui_acl_entry.acl_name,
                                RULE_TYPE_INBOUND==inout_profile?"Ingress":"Egress");
                        PROCESS_MORE(buff);

                        show_acl_name = FALSE;
                    }

                    if (RULE_TYPE_COUNTER_ENABLE == counter_enable)
                    {
                        option |= CLI_API_SHOW_MATCHES_COUNTER;
                    }
                    else
                    {
                        option &= ~CLI_API_SHOW_MATCHES_COUNTER;
                    }

                    line_num = CLI_ACL_ShowOne_ACL_ACE_List(ui_acl_entry.acl_type,
                                                            ui_acl_entry.acl_name,
                                                            ace,
                                                            option,
                                                            line_num);

                    if (line_num == JUMP_OUT_MORE)
                    {
                        return CLI_NO_ERROR;
                    }
                    else if (line_num == EXIT_SESSION_MORE)
                    {
                        return CLI_EXIT_SESSION;
                    }

                    show_ext_space_line = TRUE;
                } /* ace_index */

            } /* acl_index */
        } /* inout_profile */
    } /* uport_ifindex */

    return CLI_NO_ERROR;
}

/* clear access-list hardware counters [name acl-name] [direction {in | out}] [interface ethernet unit/port]
 */
UI32_T CLI_API_Clear_AccessList_Hardware_Counters(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)
    const char *acl_name = NULL;
    UI32_T ace_index = 0xffffffff;
    UI32_T arg_index;
    RULE_TYPE_INTERFACE_INFO_T interface = {0};

    interface.type = RULE_TYPE_INTERFACE_UNSPEC;
    interface.direction = RULE_TYPE_BOTH_DIRECTION;

    arg_index = 0;

    while (NULL != arg[arg_index])
    {
        UI32_T tmp_idx = arg_index;

        if (CLI_NO_ERROR == CLI_ACL_PrivFillAclName(&tmp_idx, (const char **) arg, &acl_name) &&
            tmp_idx != arg_index)
        {
            arg_index = tmp_idx;
        }
        else if (CLI_NO_ERROR == CLI_ACL_PrivFillDirection(&tmp_idx, (const char **) arg, &interface.direction) &&
                 tmp_idx != arg_index)
        {
            arg_index = tmp_idx;
        }
        else if (CLI_NO_ERROR == CLI_ACL_PrivFillUnitPort(&tmp_idx, (const char **) arg, &interface.uport.unit, &interface.uport.port) &&
                 tmp_idx != arg_index)
        {
            interface.type = RULE_TYPE_INTERFACE_UPORT;
            arg_index = tmp_idx;
        }
        else
        {
            return CLI_ERR_INTERNAL;
        }
    }

    L4_PMGR_ACL_ClearAclCounter(acl_name, ace_index, &interface);

#endif /* #if (SYS_CPNT_ACL_COUNTER == TRUE) */

    return CLI_NO_ERROR;
}

#if (SYS_CPNT_ACL == TRUE)
static CLI_API_EthStatus_T CLI_ACL_VerifyEthernet(UI32_T unit, UI32_T port, UI32_T *lport)
{
   SWCTRL_Lport_Type_T ret;
   BOOL_T is_inherit        =TRUE;
   *lport = 0;

   /*check module exist or not*/
   if( !SWCTRL_POM_UserPortExisting (unit, port) )
      return CLI_API_ETH_NOT_PRESENT;

   /*check if this port is trunk member*/
   ret = SWCTRL_POM_UIUserPortToIfindex (unit, port, lport, &is_inherit);

   if( ret == SWCTRL_LPORT_TRUNK_PORT_MEMBER )
      return CLI_API_ETH_TRUNK_MEMBER;

   if( ret == SWCTRL_LPORT_UNKNOWN_PORT )
      return CLI_API_ETH_UNKNOWN_PORT;

   return CLI_API_ETH_OK;
}

static void CLI_ACL_DisplayEthernetMsg(CLI_API_EthStatus_T status, UI32_T unit, UI32_T port)
{
   char  buff[CLI_DEF_MAX_BUFSIZE] = {0};

   switch(status)
   {
   case CLI_API_ETH_NOT_PRESENT:
      sprintf(buff, "Ethernet %lu/%lu is not present.\r\n", unit, port);
      break;

   case CLI_API_ETH_TRUNK_MEMBER:
      sprintf(buff, "Trunk member, ethernet %lu/%lu, could not be configured or displayed.\r\n", unit, port);
      break;

   case CLI_API_ETH_UNKNOWN_PORT:
      sprintf(buff, "Ethernet %lu/%lu is an unknown port.\r\n", unit, port);
      break;

   case CLI_API_ETH_OK:
      sprintf(buff, "Ethernet %lu/%lu is a normal port.\r\n", unit, port);
      break;

   default:
      return;
   }

   CLI_LIB_PrintStr(buff);
}

static UI32_T
CLI_ACL_ShowOne_ACL_ACE_List(
    UI32_T type,
    char *name,
    RULE_TYPE_UI_Ace_Entry_T ui_ace,
    CLI_API_SHOW_OPTION option,
    UI32_T line_num)
{
    UI32_T ace_action;
    CLI_ACL_STRING_T string;

    char addr_str1[80] = {0};
    char buff[CLI_DEF_MAX_BUFSIZE] = {0};

    CLI_ACL_STRING_InitString(buff, sizeof(buff), &string);

    ace_action = ui_ace.access;

    switch (ace_action)
    {
        case RULE_TYPE_ACE_PERMIT:
            CLI_ACL_STRING_StrCat(&string, "  permit");
            break;

        case RULE_TYPE_ACE_DENY:
            CLI_ACL_STRING_StrCat(&string, "  deny");
            break;

        case RULE_TYPE_ACE_MIRROR_TO_ANALYZER_PORT:
            CLI_ACL_STRING_StrCat(&string, "  mirror");
            break;

        default:
            break;
    }

    switch(type)
    {
        case RULE_TYPE_IP_STD_ACL:
            /*source ip*/
            CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.ipv4.sip.addr,
                                             ui_ace.ipv4.sip.mask,
                                             addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);
            break;

        case RULE_TYPE_IP_EXT_ACL:
#if (SYS_CPNT_MAC_IP_MIX == TRUE)
            /*source mac*/
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_Check_AnyMAC(ui_ace.ether.sa.addr, ui_ace.ether.sa.mask, addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);

            /*destination mac*/
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_Check_AnyMAC(ui_ace.ether.da.addr, ui_ace.ether.da.mask, addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);

            CLI_ACL_PRIV_FLUSH_LINE(&string, line_num);

            /*vid*/
            memset (addr_str1, 0, sizeof(addr_str1));
            if (ui_ace.ether.vid.op == VAL_diffServMacAceVidOp_equal)
            {
                if (ui_ace.ether.vid.u.s.mask == MAX_diffServMacAceVidBitmask)
                {
                    sprintf(addr_str1," VID %lu", ui_ace.ether.vid.u.s.data);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
                else
                {
                    sprintf(addr_str1," VID %lu %lu",ui_ace.ether.vid.u.s.data
                            ,ui_ace.ether.vid.u.s.mask);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
            }
#endif /* #if (SYS_CPNT_MAC_IP_MIX == TRUE) */

            /*protocol*/
            memset (addr_str1, 0, sizeof(addr_str1));
            if (ui_ace.ipv4.protocol.op == RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL)
            {
                switch (ui_ace.ipv4.protocol.u.s.data) {
                    case RULE_TYPE_ACL_TCP_PROTOCOL:
                        CLI_ACL_STRING_StrCat(&string, " TCP");
                        break;

                    case RULE_TYPE_ACL_UDP_PROTOCOL:
                        CLI_ACL_STRING_StrCat(&string, " UDP");
                        break;

                    case RULE_TYPE_ACL_ICMP_PROTOCOL:
                        CLI_ACL_STRING_StrCat(&string, " ICMP");
                        break;

                    default:
                        sprintf(addr_str1," %u", ui_ace.ipv4.protocol.u.s.data);
                        CLI_ACL_STRING_StrCat(&string, addr_str1);
                        break;
                }
            }

            /*source ip*/
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.ipv4.sip.addr, ui_ace.ipv4.sip.mask, addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);


            memset (addr_str1, 0 , sizeof(addr_str1));
            /*destination ip*/
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.ipv4.dip.addr, ui_ace.ipv4.dip.mask, addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);

            /*precedence*/
            memset (addr_str1, 0, sizeof(addr_str1));
            switch (ui_ace.ipv4.dscp.op)
            {
                case RULE_TYPE_IPV4_DSCP_OP_TOS:
                    sprintf(addr_str1," tos %u", ui_ace.ipv4.dscp.u.tos.tos);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                    break;
                case RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE:
                    sprintf(addr_str1," precedence %u", ui_ace.ipv4.dscp.u.tos.precedence);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                    break;
                case RULE_TYPE_IPV4_DSCP_OP_DSCP:
                    sprintf(addr_str1," DSCP %u", ui_ace.ipv4.dscp.u.ds);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                    break;
                case RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE_AND_TOS:
                    sprintf(addr_str1," precedence %u tos %u",
						ui_ace.ipv4.dscp.u.tos.precedence, ui_ace.ipv4.dscp.u.tos.tos);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                    break;
                default:
                    break;
            }

            CLI_ACL_PRIV_FLUSH_LINE(&string, line_num);

            /*source port*/
            memset (addr_str1, 0, sizeof(addr_str1));
            if (ui_ace.l4_common.sport.op == VAL_diffServIpAceSourcePortOp_equal)
            {
                if (ui_ace.l4_common.sport.u.s.mask == MAX_diffServIpAceSourcePortBitmask )
                {
                    sprintf(addr_str1," source-port %lu", ui_ace.l4_common.sport.u.s.data);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
                else
                {
                    sprintf(addr_str1," source-port %lu %lu",
                            ui_ace.l4_common.sport.u.s.data,
                            ui_ace.l4_common.sport.u.s.mask);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
            }

            /*destination port*/
            memset (addr_str1, 0, sizeof(addr_str1));
            if (ui_ace.l4_common.dport.op == VAL_diffServIpAceSourcePortOp_equal)
            {
                if (ui_ace.l4_common.dport.u.s.mask == MAX_diffServIpAceSourcePortBitmask )
                {
                    sprintf(addr_str1," destination-port %lu", ui_ace.l4_common.dport.u.s.data);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
                else
                {
                    sprintf(addr_str1," destination-port %lu %lu",
                            ui_ace.l4_common.dport.u.s.data,
                            ui_ace.l4_common.dport.u.s.mask);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
            }

#if (SYS_CPNT_ACL_IP_EXT_ICMP == TRUE)
            /*icmp type*/
            memset (addr_str1, 0, sizeof(addr_str1));
            if (ui_ace.icmp.icmp_type.op == RULE_TYPE_ICMP_ICMPTYPE_OP_EQUAL)
            {
                sprintf(addr_str1, " icmp-type %u",
			   ui_ace.icmp.icmp_type.u.s.data);
                CLI_ACL_STRING_StrCat(&string, addr_str1);
            }
#endif /* SYS_CPNT_ACL_IP_EXT_ICMP */

            /*control code*/
            memset (addr_str1, 0, sizeof(addr_str1));
            if (ui_ace.tcp.flags.mask.u.code != 0)
            {
                sprintf(addr_str1," control-flag %u", ui_ace.tcp.flags.data.u.code);
                CLI_ACL_STRING_StrCat(&string, addr_str1);

                if (ui_ace.tcp.flags.mask.u.code != MAX_diffServIpAceControlCodeBitmask)
                {
                    sprintf(addr_str1," %u", ui_ace.tcp.flags.mask.u.code);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
            }
            break;

        case RULE_TYPE_MAC_ACL:
            /*packet format*/
            memset (addr_str1, 0, sizeof(addr_str1));
            switch (ui_ace.pkt_format)
            {
                case VAL_diffServMacAcePktformat_any:
                    break;

                case VAL_diffServMacAcePktformat_untagged_Eth2:
                    CLI_ACL_STRING_StrCat(&string, " untagged-eth2");
                    break;

                case VAL_diffServMacAcePktformat_untagged802Dot3:
                    CLI_ACL_STRING_StrCat(&string, " untagged-802.3");
                    break;

                case VAL_diffServMacAcePktformat_tagggedEth2:
                    CLI_ACL_STRING_StrCat(&string, " tagged-eth2");
                    break;

                case VAL_diffServMacAcePktformat_tagged802Dot3:
                    CLI_ACL_STRING_StrCat(&string, " tagged-802.3");
                    break;
            }

            /*source mac*/
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_Check_AnyMAC (ui_ace.ether.sa.addr, ui_ace.ether.sa.mask, addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);

            /*destination mac*/
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_Check_AnyMAC (ui_ace.ether.da.addr, ui_ace.ether.da.mask, addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);

            CLI_ACL_PRIV_FLUSH_LINE(&string, line_num);

            /*vid*/
            memset (addr_str1, 0, sizeof(addr_str1));
            if (ui_ace.ether.vid.op == VAL_diffServMacAceVidOp_equal)
            {
                if(ui_ace.ether.vid.u.s.mask == MAX_diffServMacAceVidBitmask)
                {
                    sprintf(addr_str1," VID %lu", ui_ace.ether.vid.u.s.data);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
                else
                {
                    sprintf(addr_str1," VID %lu %lu", ui_ace.ether.vid.u.s.data,
                            ui_ace.ether.vid.u.s.mask);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
            }

            /* ethertype | ipv4 | ipv6 */
            {
                BOOL_T has_l4_pattern = FALSE;
                RULE_TYPE_UI_Ace_Entry_T default_ui_ace;

                L4_PMGR_ACL_InitUIAce(&default_ui_ace);

                if (ui_ace.ether.ethertype.op == VAL_diffServMacAceEtherTypeOp_equal)
                {
                    switch (ui_ace.ether.ethertype.u.s.data &
                            ui_ace.ether.ethertype.u.s.mask)
                    {
                        case RULE_TYPE_ETHERTYPE_IP:
                            if(0 != memcmp(&ui_ace.ipv4, &default_ui_ace.ipv4, sizeof(ui_ace.ipv4)) ||
                               0 != memcmp(&ui_ace.l4_common, &default_ui_ace.l4_common, sizeof(ui_ace.l4_common)))
                            {
                                has_l4_pattern = TRUE;

                                CLI_ACL_STRING_StrCat(&string, " ip");

                                memset (addr_str1, 0, sizeof(addr_str1));
                                CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.ipv4.sip.addr, ui_ace.ipv4.sip.mask, addr_str1);
                                CLI_ACL_STRING_StrCat(&string, addr_str1);

                                memset (addr_str1, 0, sizeof(addr_str1));
                                CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.ipv4.dip.addr, ui_ace.ipv4.dip.mask, addr_str1);
                                CLI_ACL_STRING_StrCat(&string, addr_str1);

                                CLI_ACL_PRIV_FLUSH_LINE(&string, line_num);

                                memset (addr_str1, 0, sizeof(addr_str1));
                                if (ui_ace.ipv4.protocol.op == RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL)
                                {
                                    CLI_ACL_STRING_StrCat(&string, " protocol");
                                    SYSFUN_Sprintf(addr_str1," %u", ui_ace.ipv4.protocol.u.s.data);
                                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                                }
                            }
                            break;
                        case RULE_TYPE_ETHERTYPE_IPV6:
                            if(0 != memcmp(&ui_ace.ipv6, &default_ui_ace.ipv6, sizeof(ui_ace.ipv6)) ||
                               0 != memcmp(&ui_ace.l4_common, &default_ui_ace.l4_common, sizeof(ui_ace.l4_common)))
                            {
                                has_l4_pattern = TRUE;

                                CLI_ACL_STRING_StrCat(&string, " ipv6");

                                memset (addr_str1, 0, sizeof(addr_str1));
                                CLI_ACL_Check_AnyIPV6 (addr_str1,
                                                       ui_ace.ipv6.sip.addr,
                                                       ui_ace.ipv6.sip.prefix_len);
                                CLI_ACL_STRING_StrCat(&string, addr_str1);

                                memset (addr_str1, 0, sizeof(addr_str1));
                                CLI_ACL_Check_AnyIPV6(addr_str1,
                                                      ui_ace.ipv6.dip.addr,
                                                      ui_ace.ipv6.dip.prefix_len);
                                CLI_ACL_STRING_StrCat(&string, addr_str1);

                                CLI_ACL_PRIV_FLUSH_LINE(&string, line_num);

                                memset (addr_str1, 0, sizeof(addr_str1));
                                if (ui_ace.ipv6.next_header.op == RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL)
                                {
                                    CLI_ACL_STRING_StrCat(&string, " protocol");
                                    SYSFUN_Sprintf(addr_str1," %u", ui_ace.ipv6.next_header.u.s.data);
                                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                                }
                            }
                            break;
                        default:
                            break;
                    }

                    if (has_l4_pattern == TRUE)
                    {
                        /* l4 source port */
                        memset (addr_str1, 0, sizeof(addr_str1));
                        if (ui_ace.l4_common.sport.op == VAL_diffServIpAceSourcePortOp_equal)
                        {
                            if (ui_ace.l4_common.sport.u.s.mask == MAX_diffServIpAceSourcePortBitmask )
                            {
                                sprintf(addr_str1," l4-source-port %lu",
                                        ui_ace.l4_common.sport.u.s.data);
                                CLI_ACL_STRING_StrCat(&string, addr_str1);
                            }
                            else
                            {
                                sprintf(addr_str1," l4-source-port %lu %lu",
                                        ui_ace.l4_common.sport.u.s.data,
                                        ui_ace.l4_common.sport.u.s.mask);

                                CLI_ACL_STRING_StrCat(&string, addr_str1);
                            }
                        }

                        /* l4 destination port */
                        memset (addr_str1, 0, sizeof(addr_str1));
                        if (ui_ace.l4_common.dport.op == VAL_diffServIpAceDestPortOp_equal)
                        {
                            if (ui_ace.l4_common.dport.u.s.mask == MAX_diffServIpAceDestPortBitmask )
                            {
                                sprintf(addr_str1," l4-destination-port %lu",
                                        ui_ace.l4_common.dport.u.s.data);
                                CLI_ACL_STRING_StrCat(&string, addr_str1);
                            }
                            else
                            {
                                sprintf(addr_str1," l4-destination-port %lu %lu",
                                        ui_ace.l4_common.dport.u.s.data,
                                        ui_ace.l4_common.dport.u.s.mask);

                                CLI_ACL_STRING_StrCat(&string, addr_str1);
                            }
                        }
                    }
                    else
                    {
                        memset (addr_str1, 0, sizeof(addr_str1));
                        if (ui_ace.ether.ethertype.u.s.mask == MAX_diffServMacAceMinEtherType)
                        {
                            sprintf(addr_str1," ethertype %04lx",
                                    ui_ace.ether.ethertype.u.s.data);
                            CLI_ACL_STRING_StrCat(&string, addr_str1);
                        }
                        else
                        {
                            sprintf(addr_str1," ethertype %04lx %04lx",
                                    ui_ace.ether.ethertype.u.s.data,
                                    ui_ace.ether.ethertype.u.s.mask);
                            CLI_ACL_STRING_StrCat(&string, addr_str1);
                        }
                    }
                }
            }

            /* cos */
            if ( ui_ace.ether.cos.op == VAL_diffServMacAceCosOp_equal )
            {
                memset (addr_str1, 0, sizeof(addr_str1));
                sprintf(addr_str1," cos %u %u",ui_ace.ether.cos.u.s.data, ui_ace.ether.cos.u.s.mask);
                CLI_ACL_STRING_StrCat(&string, addr_str1);
            }
            break;

#if (SYS_CPNT_ACL_IPV6 == TRUE)
        case RULE_TYPE_IPV6_STD_ACL:
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_Check_AnyIPV6 (addr_str1, ui_ace.ipv6.sip.addr,
                                   ui_ace.ipv6.sip.prefix_len);

            CLI_ACL_STRING_StrCat(&string, addr_str1);

            break;

        case RULE_TYPE_IPV6_EXT_ACL:

#if (SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER_NEW_FORMAT == TRUE)
            /*next-header*/
            memset (addr_str1, 0, sizeof(addr_str1));
            if (ui_ace.ipv6.next_header.op == RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL)
            {
                switch (ui_ace.ipv6.next_header.u.s.data) {
                    case RULE_TYPE_ACL_IPV6_TCP_NEXT_HEADER:
                        CLI_ACL_STRING_StrCat(&string, " TCP");
                        break;

                    case RULE_TYPE_ACL_IPV6_UDP_NEXT_HEADER:
                        CLI_ACL_STRING_StrCat(&string, " UDP");
                        break;
#if (SYS_CPNT_ACL_IPV6_EXT_ICMP == TRUE)
                    case RULE_TYPE_ACL_IPV6_ICMP_NEXT_HEADER:
                        CLI_ACL_STRING_StrCat(&string, " ICMP");
                        break;
#endif /* SYS_CPNT_ACL_IPV6_EXT_ICMP */
                    default:
                        sprintf(addr_str1," %u", ui_ace.ipv6.next_header.u.s.data);
                        CLI_ACL_STRING_StrCat(&string, addr_str1);
                        break;
                }
            }
#endif /* SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER_NEW_FORMAT */

#if (SYS_CPNT_MAC_IP_MIX == TRUE)
            /*source mac*/
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_Check_AnyMAC (ui_ace.ether.sa.addr, ui_ace.ether.sa.mask,
                                  addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);

            /*destination mac*/
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_Check_AnyMAC (ui_ace.ether.da.addr, ui_ace.ether.da.mask,
                                  addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);

            CLI_ACL_PRIV_FLUSH_LINE(&string, line_num);

            /*vid*/
            memset (addr_str1, 0, sizeof(addr_str1));
            if (ui_ace.ether.vid.op == VAL_diffServMacAceVidOp_equal)
            {
                if (ui_ace.ether.vid.u.s.mask == MAX_diffServMacAceVidBitmask)
                {
                    sprintf(addr_str1," VID %lu", ui_ace.ether.vid.u.s.data);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
                else
                {
                    sprintf(addr_str1," VID %lu %lu", ui_ace.ether.vid.u.s.data,
                            ui_ace.ether.vid.u.s.mask);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
            }
#endif /* #if (SYS_CPNT_MAC_IP_MIX == TRUE) */

#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_Check_AnyIPV6 (addr_str1, ui_ace.ipv6.sip.addr,
                                   ui_ace.ipv6.sip.prefix_len);
            CLI_ACL_STRING_StrCat(&string, addr_str1);
#endif /* SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR */

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_Check_AnyIPV6 (addr_str1, ui_ace.ipv6.dip.addr,
                                   ui_ace.ipv6.dip.prefix_len);
            CLI_ACL_STRING_StrCat(&string, addr_str1);
#endif /* SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR */

            CLI_ACL_PRIV_FLUSH_LINE(&string, line_num);

#if (SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER == TRUE)
            /* next header */
            if (ui_ace.ipv6.next_header.op == RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL)
            {
                sprintf(addr_str1," next-header %u", ui_ace.ipv6.next_header.u.s.data);
                CLI_ACL_STRING_StrCat(&string, addr_str1);
            }
#endif /* SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER */

            /* dscp */
            if (ui_ace.ipv6.traffic_class.op == RULE_TYPE_IPV6_TRAFFIC_CLASS_OP_EQUAL)
            {
                /* dscp is 2-7 bit on ipv6 traffic class field
                 */
                sprintf(addr_str1, " DSCP %u",
                        (ui_ace.ipv6.traffic_class.u.s.data >> 2));
                CLI_ACL_STRING_StrCat(&string, addr_str1);
            }

            /* flow label */
            if (ui_ace.ipv6.flow_label.op == RULE_TYPE_IPV6_FLOW_LABEL_OP_EQUAL)
            {
                sprintf(addr_str1," flow-label %lu",
                        ui_ace.ipv6.flow_label.u.s.data);
                CLI_ACL_STRING_StrCat(&string, addr_str1);
            }

#if (SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT == TRUE)
            /*source port*/
            memset (addr_str1, 0, sizeof(addr_str1));
            if (ui_ace.l4_common.sport.op == VAL_diffServIpAceSourcePortOp_equal)
            {
                sprintf(addr_str1," source-port %lu", ui_ace.l4_common.sport.u.s.data);
                CLI_ACL_STRING_StrCat(&string, addr_str1);

                if (ui_ace.l4_common.sport.u.s.mask != MAX_diffServIpAceSourcePortBitmask )
                {
                    memset (addr_str1, 0, sizeof(addr_str1));
                    sprintf(addr_str1," %lu", ui_ace.l4_common.sport.u.s.mask);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
            }

            /* destination port
             */
            memset (addr_str1, 0, sizeof(addr_str1));
            if (ui_ace.l4_common.dport.op == VAL_diffServIpAceSourcePortOp_equal)
            {
                sprintf(addr_str1," destination-port %lu", ui_ace.l4_common.dport.u.s.data);
                CLI_ACL_STRING_StrCat(&string, addr_str1);

                if (ui_ace.l4_common.dport.u.s.mask != MAX_diffServIpAceSourcePortBitmask )
                {
                    memset (addr_str1, 0, sizeof(addr_str1));
                    sprintf(addr_str1," %lu", ui_ace.l4_common.dport.u.s.mask);
                    CLI_ACL_STRING_StrCat(&string, addr_str1);
                }
            }
#endif /* SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT */

#if (SYS_CPNT_ACL_IPV6_EXT_ICMP == TRUE)
            /*icmp type*/
            memset (addr_str1, 0, sizeof(addr_str1));
            if (ui_ace.icmp.icmp_type.op == RULE_TYPE_ICMP_ICMPTYPE_OP_EQUAL)
            {
                sprintf(addr_str1, " icmp-type %u", ui_ace.icmp.icmp_type.u.s.data);
                CLI_ACL_STRING_StrCat(&string, addr_str1);
            }
#endif

            break;
#endif /* SYS_CPNT_ACL_IPV6 */

#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
            /* arp type */
            if (ui_ace.arp.arp_type == RULE_TYPE_ARP_REQUEST)
            {
                CLI_ACL_STRING_StrCat(&string, " request");
            }
            else if (ui_ace.arp.arp_type == RULE_TYPE_ARP_REPLY)
            {
                CLI_ACL_STRING_StrCat(&string, " response");
            }

            CLI_ACL_STRING_StrCat(&string, " ip");

            /* sender ip */
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.arp.sender.ip.addr,
                                             ui_ace.arp.sender.ip.mask,
                                             addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);

            /* target ip */
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_ConvertIpv4Addr2UIString(ui_ace.arp.target.ip.addr,
                                             ui_ace.arp.target.ip.mask,
                                             addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);


            CLI_ACL_STRING_StrCat(&string, " mac");

            /* sender mac */
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_Check_AnyMAC (ui_ace.arp.sender.mac.addr,
                                  ui_ace.arp.sender.mac.mask,
                                  addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);

            /* target mac */
            memset (addr_str1, 0, sizeof(addr_str1));
            CLI_ACL_Check_AnyMAC (ui_ace.arp.target.mac.addr,
                                  ui_ace.arp.target.mac.mask,
                                  addr_str1);
            CLI_ACL_STRING_StrCat(&string, addr_str1);

            if (ui_ace.arp.is_log == TRUE)
            {
                CLI_ACL_STRING_StrCat(&string, " log");
            }
            break;
#endif /* SYS_CPNT_DAI */

        default:
            break;
    }

    CLI_ACL_PRIV_FLUSH_LINE(&string, line_num);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    /* time range */
    if (RULE_TYPE_UNDEF_TIME_RANGE != ui_ace.time_range_index)
    {
        char  time_range_string[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+15] = {0};

        sprintf (time_range_string," time-range %s", ui_ace.time_range_name);

        CLI_ACL_STRING_StrCat(&string, time_range_string);
        CLI_ACL_PRIV_FLUSH_LINE(&string, line_num);
    }
#endif /* #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    /* Don't show hit counter for deny ACE due to hardware (Marvell) limitation
     */
    if ( (RULE_TYPE_ACE_DENY != ace_action) &&
        ((option & CLI_API_SHOW_MATCHES_COUNTER) != 0) )
    {
        sprintf(addr_str1, " (%lu hits)", ui_ace.counter.total_packets);

        CLI_ACL_STRING_StrCat(&string, addr_str1);
        CLI_ACL_PRIV_FLUSH_LINE(&string, line_num);
    }
#endif /* #if (SYS_CPNT_ACL_COUNTER == TRUE) */

    strcat (buff, "\r\n");
    PROCESS_MORE_FUNC (buff);

    return line_num;
}

static UI32_T
CLI_ACL_PrivGetMacAddr(
    char *arg[],
    UI32_T *position_p,
    UI8_T addr[SYS_ADPT_MAC_ADDR_LEN],
    UI8_T bitmask[SYS_ADPT_MAC_ADDR_LEN])
{
    ASSERT(arg != NULL);
    ASSERT(position_p != NULL);

    if (NULL == arg[*position_p])
    {
        return CLI_ERR_INTERNAL;
    }

    memset(addr, 0, sizeof(UI8_T) * SYS_ADPT_MAC_ADDR_LEN);
    memset(bitmask, 0, sizeof(UI8_T) * SYS_ADPT_MAC_ADDR_LEN);

    if (CLI_LIB_ValsInMac(arg[*position_p], addr) == TRUE)
    {
        CLI_LIB_ValsInMac(arg[*position_p + 1], bitmask);
        (*position_p) += 2;
    }
    else
    {
        switch(arg[*position_p][0])
        {
            case 'a':/*any source*/
            case 'A':
                (*position_p)++;
                break;

            case 'h':/*host source*/
            case 'H':
                CLI_LIB_ValsInMac(arg[*position_p+1], addr);
                memset(bitmask, 255, sizeof(char) * 6);
                (*position_p) += 2;
                break;

            default:
                return CLI_ERR_CMD_NOT_IMPLEMENT;
        }
    }

    return CLI_NO_ERROR;
}

static UI32_T
CLI_ACL_PrivGetIpAddr(
    char *arg[],
    UI32_T *position_p,
    UI8_T addr[SYS_ADPT_IPV4_ADDR_LEN],
    UI8_T mask[SYS_ADPT_IPV4_ADDR_LEN])
{
    UI16_T ptr;

    ASSERT(arg != NULL);
    ASSERT(position_p != NULL);

    if (NULL == arg[*position_p])
    {
        return CLI_ERR_INTERNAL;
    }

    if (CLI_LIB_CheckIPAddr(arg[(*position_p)], &ptr) == 0)
    {
        if (TRUE != L_INET_Pton(L_INET_AF_INET, arg[(*position_p)], addr))
        {
            return CLI_ERR_INTERNAL;
        }

        if (TRUE != L_INET_Pton(L_INET_AF_INET, arg[(*position_p) + 1], mask))
        {
            return CLI_ERR_INTERNAL;
        }

        (*position_p) += 2;
    }
    else
    {
        switch(arg[(*position_p)][0])
        {
            case 'a':
            case 'A':
                (*position_p)++;
                break;

            case 'h':
            case 'H':
                if (TRUE != L_INET_Pton(L_INET_AF_INET, arg[(*position_p)+1], addr))
                {
                    return CLI_ERR_INTERNAL;
                }

                {
                    int count;

                    for (count = 0; count < SYS_ADPT_IPV4_ADDR_LEN; count++)
                    {
                        mask[count] = 0xff;
                    }

                }

                (*position_p) += 2;
                break;

            default:
                return CLI_ERR_CMD_NOT_IMPLEMENT;
                break;
        }
    }

    return CLI_NO_ERROR;
}

static UI32_T
CLI_ACL_PrivGetVid(
    char *arg[],
    UI32_T *position_p,
    UI32_T *operation_p,
    UI32_T *maxVid_p,
    UI32_T *minVid_p,
    UI32_T *bitmask_p)
{
    ASSERT(arg != NULL);
    ASSERT(position_p != NULL);
    ASSERT(operation_p != NULL);
    ASSERT(maxVid_p != NULL);
    ASSERT(minVid_p != NULL);
    ASSERT(bitmask_p != NULL);

    if (NULL == arg[*position_p])
    {
        return CLI_ERR_INTERNAL;
    }

    *operation_p = VAL_diffServMacAceVidOp_equal;
    *bitmask_p = MAX_diffServMacAceVidBitmask;
    *minVid_p = atoi(arg[(*position_p)]);
    //*maxVid_p = atoi(arg[(*position_p)]);

    (*position_p) += 1;

    if (arg[(*position_p)] != NULL)
    {
        if (isdigit(arg[(*position_p)][0]))
        {
            *bitmask_p = atoi(arg[(*position_p)]);
            (*position_p) += 1;
        }
    }

    return CLI_NO_ERROR;
}

#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
static UI32_T
CLI_ACL_PrivGetL4Port(
    char *arg[],
    UI32_T *position_p,
    RULE_TYPE_UI_Ace_L4_Port_T *l4_port_p)
{

    ASSERT(position_p != NULL);
    ASSERT(l4_port_p != NULL);

    if (NULL == arg[*position_p])
    {
        return CLI_ERR_INTERNAL;
    }

    l4_port_p->op = VAL_diffServIpAceSourcePortOp_equal;
    l4_port_p->u.s.data = atoi(arg[(*position_p)]);
    l4_port_p->u.s.mask = MAX_diffServIpAceSourcePortBitmask;

    (*position_p)++;

    if (arg[(*position_p)] != NULL &&
        isdigit(arg[(*position_p)][0]))
    {
        l4_port_p->u.s.mask = atoi(arg[(*position_p)]);
        (*position_p)++;
    }

    return CLI_NO_ERROR;
}
#endif /* SYS_CPNT_ACL_MAC_IP_MIX */

#if (SYS_CPNT_ACL_IPV6 == TRUE)
static UI32_T
CLI_ACL_PrivGetIp6Address(
    char *arg[],
    UI32_T *arg_pos_p,
    UI8_T addr[SYS_ADPT_IPV6_ADDR_LEN],
    UI8_T *preflen_p)
{
    L_INET_AddrIp_T inet_addr;

    if (NULL == arg_pos_p || NULL == preflen_p)
    {
        return CLI_ERR_INTERNAL;
    }

    if (NULL == arg[*arg_pos_p])
    {
        return CLI_ERR_INTERNAL;
    }

    switch (arg[*arg_pos_p][0])
    {
        case 'h': /* host */
        case 'H':
            if (L_INET_RETURN_SUCCESS == L_INET_StringToInaddr(L_INET_FORMAT_IPV6_GLOBAL,
                                                               arg[*arg_pos_p+1],
                                                               (L_INET_Addr_T *)&inet_addr,
                                                               sizeof(inet_addr)))
            {
                memcpy (addr, inet_addr.addr, SYS_ADPT_IPV6_ADDR_LEN);
                *preflen_p = RULE_TYPE_MAX_IPV6_PREFIX_LEN;
                *arg_pos_p += 2;
            }
            else
            {
                return CLI_ERR_INTERNAL;
            }
            break;

        default:
            if (L_INET_RETURN_SUCCESS == L_INET_StringToInaddr(L_INET_FORMAT_IPV6_GLOBAL_PREFIX,
                                                               arg[*arg_pos_p],
                                                               (L_INET_Addr_T *)&inet_addr,
                                                               sizeof(inet_addr)))
            {
                memcpy (addr, inet_addr.addr, SYS_ADPT_IPV6_ADDR_LEN);
                *preflen_p = inet_addr.preflen;
                *arg_pos_p += 1;
            }
            else
            {
                switch (arg[*arg_pos_p][0])
                {
                    case 'a':   /* any */
                    case 'A':
                        memset(addr, 0, SYS_ADPT_IPV6_ADDR_LEN);
                        *preflen_p = 0;
                        *arg_pos_p += 1;
                        break;

                    default:
                        return CLI_ERR_INTERNAL;
                }
            }

            break;
    }

    return CLI_NO_ERROR;
}
#endif /* SYS_CPNT_ACL_IPV6 */

static UI32_T
CLI_ACL_FillMAC(
    RULE_TYPE_UI_Ace_Entry_T *ace_p,
    char *arg[])
{
    UI32_T arg_position = 0;
    BOOL_T mixed_ip;

    mixed_ip = FALSE;

    switch(arg[arg_position][0])
    {
        case 't':/*tagged*/
        case 'T':
            if (arg[0][7] == 'E' || arg[0][7] == 'e')
            {
                ace_p->pkt_format = VAL_diffServMacAcePktformat_tagggedEth2;
            }
            else
            {
                ace_p->pkt_format = VAL_diffServMacAcePktformat_tagged802Dot3;
            }

            arg_position++;
            break;

        case 'u':/*untagged*/
        case 'U':
            if (arg[0][9] == 'E' || arg[0][9] == 'e')
            {
                ace_p->pkt_format = VAL_diffServMacAcePktformat_untagged_Eth2;
            }
            else
            {
                ace_p->pkt_format = VAL_diffServMacAcePktformat_untagged802Dot3;
            }

            arg_position++;
            break;

        default:
          break;
    }

    if (CLI_NO_ERROR != CLI_ACL_PrivGetMacAddr(arg, &arg_position,
                                               ace_p->ether.sa.addr,
                                               ace_p->ether.sa.mask))
    {
        return CLI_ERR_INTERNAL;
    }

    if (CLI_NO_ERROR != CLI_ACL_PrivGetMacAddr(arg, &arg_position,
                                               ace_p->ether.da.addr,
                                               ace_p->ether.da.mask))
    {
        return CLI_ERR_INTERNAL;
    }

    /*check option*/
    while (arg[arg_position] != NULL)
    {
        switch(arg[arg_position][0])
        {
            case 'v':/*vid*/
            case 'V':
                arg_position++;

                if (CLI_NO_ERROR != CLI_ACL_PrivGetVid(arg, &arg_position,
                                                       &ace_p->ether.vid.op,
                                                       &ace_p->ether.vid.u.s.data,
                                                       &ace_p->ether.vid.u.s.data,
                                                       &ace_p->ether.vid.u.s.mask))
                {
                    return CLI_ERR_INTERNAL;
                }
             break;

            case 'e':/*EtherType*/
            case 'E':

                if (mixed_ip == TRUE)
                {
                    return CLI_ERR_INTERNAL;
                }

                ace_p->ether.ethertype.op = VAL_diffServMacAceEtherTypeOp_equal;
                ace_p->ether.ethertype.u.s.data = CLI_LIB_AtoUl(arg[arg_position+1], 16);
                ace_p->ether.ethertype.u.s.mask = MAX_diffServMacAceEtherTypeBitmask;

                arg_position = arg_position + 2;

                if (arg[arg_position] != NULL)
                {
                    if (arg[arg_position][0] == 'c' || arg[arg_position][0] == 'C')
                    {
                        break;
                    }
                    else
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                    if (arg[arg_position][0] == 't' || arg[arg_position][0] == 'T')
                    {
                        break;
                    }
                    else
#endif /* SYS_CPNT_TIME_BASED_ACL */
                    {
                        ace_p->ether.ethertype.u.s.mask = CLI_LIB_AtoUl(arg[arg_position], 16);

                        arg_position = arg_position + 1;
                    }
                }
                break;

#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
            case 'i':  /* ip | ipv6 */
            case 'I':
            {
                BOOL_T is_ipv6;
                BOOL_T parse_option;

                ASSERT(ace_p->ether.ethertype.op == VAL_diffServMacAceEtherTypeOp_noOperator);

                mixed_ip = TRUE;
                arg_position ++;

                switch (arg[arg_position-1][2])
                {
                    case 'v': /* IPv6 */
                    case 'V':
                        is_ipv6 = TRUE;

                        ace_p->ether.ethertype.op = VAL_diffServMacAceEtherTypeOp_equal;
                        ace_p->ether.ethertype.u.s.data = RULE_TYPE_ETHERTYPE_IPV6;
                        ace_p->ether.ethertype.u.s.mask = MAX_diffServMacAceEtherTypeBitmask;

                        /* ipv6 sip */
                        if (CLI_NO_ERROR != CLI_ACL_PrivGetIp6Address(arg,
                                                &arg_position,
                                                ace_p->ipv6.sip.addr,
                                                &ace_p->ipv6.sip.prefix_len))
                        {
                            return CLI_ERR_INTERNAL;
                        };

                        /* ipv6 dip */
                        if (CLI_NO_ERROR != CLI_ACL_PrivGetIp6Address(arg,
                                                &arg_position,
                                                ace_p->ipv6.dip.addr,
                                                &ace_p->ipv6.dip.prefix_len))
                        {
                            return CLI_ERR_INTERNAL;
                        }

                        break;
                    default: /* IP */
                    {
                        is_ipv6 = FALSE;
                        ace_p->ether.ethertype.op = VAL_diffServMacAceEtherTypeOp_equal;
                        ace_p->ether.ethertype.u.s.mask = MAX_diffServMacAceEtherTypeBitmask;
                        ace_p->ether.ethertype.u.s.data = RULE_TYPE_ETHERTYPE_IP;

                        if(CLI_NO_ERROR != CLI_ACL_PrivGetIpAddr(arg, &arg_position,
                                                                   ace_p->ipv4.sip.addr,
                                                                   ace_p->ipv4.sip.mask))
                        {
                            return CLI_ERR_INTERNAL;
                        }
                        if(CLI_NO_ERROR != CLI_ACL_PrivGetIpAddr(arg, &arg_position,
                                                                 ace_p->ipv4.dip.addr,
                                                                 ace_p->ipv4.dip.mask))
                        {
                            return CLI_ERR_INTERNAL;
                        }

                        break;
                    }
                }

                parse_option = TRUE;

                while (arg[arg_position] != NULL && parse_option == TRUE)
                {
                    switch (arg[arg_position][0])
                    {
                        case 'p': /* protocol */
                        case 'P':
                        {
                            if (is_ipv6 == TRUE)
                            {
                                ace_p->ipv6.next_header.op = RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL;
                                ace_p->ipv6.next_header.u.s.data = (UI8_T) atoi(arg[arg_position+1]);
                                ace_p->ipv6.next_header.u.s.mask = 0xff;
                            }
                            else
                            {
                                ace_p->ipv4.protocol.op = RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL;
                                ace_p->ipv4.protocol.u.s.data = (UI8_T) atoi(arg[arg_position+1]);
                                ace_p->ipv4.protocol.u.s.mask = 0xff;
                            }

                            arg_position+=2;
                            break;
                        }
                        case 'l': /* l4-source-port, l4-destination-port */
                        case 'L':
                            if (arg[arg_position][3] == 's' || arg[arg_position][3] == 'S')
                            {
                                arg_position++;
                                if (CLI_NO_ERROR != CLI_ACL_PrivGetL4Port(arg,
                                                        &arg_position,
                                                        &ace_p->l4_common.sport))
                                {
                                    return CLI_ERR_INTERNAL;
                                }
                            }
                            else if (arg[arg_position][3] == 'd' || arg[arg_position][3] == 'D')
                            {
                                arg_position++;
                                if (CLI_NO_ERROR != CLI_ACL_PrivGetL4Port(arg,
                                                        &arg_position,
                                                        &ace_p->l4_common.dport))
                                {
                                    return CLI_ERR_INTERNAL;
                                }
                            }

                            break;
                        default:
                            parse_option = FALSE;
                            break;
                    }
                }
            }
            break;
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */

            case 'c': /* cos */
            case 'C':
                ace_p->ether.cos.op = VAL_diffServMacAceCosOp_equal;// ACL_OPERATOR_EQUAL;
                ace_p->ether.cos.u.s.data = (UI8_T) atoi(arg[arg_position+1]);
                ace_p->ether.cos.u.s.mask = (UI8_T) atoi(arg[arg_position+2]);
                arg_position = arg_position + 3;
            break;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
            case 't':/*time range*/
            case 'T':
                if (CLI_ERR_INTERNAL == CLI_ACL_TIME_RANGE_FillTimeRangeName(
                                            &arg[arg_position],
                                            ace_p->time_range_name))
                {
                    return CLI_ERR_INTERNAL;
                }

                arg_position = arg_position + 2;
            break;
#endif /* end of #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */

            default:
                return CLI_NO_ERROR;
            break;
        }
    }

#if (CLI_UNITEST == 1)
    CLI_ACL_ShowOne_ACL_ACE_List(RULE_TYPE_MAC_ACL, "123", *ace_p, 0, 1);
#endif

    return CLI_NO_ERROR;
}

static UI32_T
CLI_ACL_FillStandard(
    RULE_TYPE_UI_Ace_Entry_T *ace_p,
    char *arg[])
{
    UI32_T arg_pos = 0;

    if (CLI_NO_ERROR != CLI_ACL_PrivGetIpAddr(arg, &arg_pos,
                                              ace_p->ipv4.sip.addr,
                                              ace_p->ipv4.sip.mask))
    {
        return CLI_ERR_INTERNAL;
    }


#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    if (CLI_ERR_INTERNAL == CLI_ACL_TIME_RANGE_FillTimeRangeName(&arg[arg_pos], ace_p->time_range_name))
    {
        return CLI_ERR_INTERNAL;
    }
#endif /* end of #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */

#if (CLI_UNITEST == 1)
      CLI_ACL_ShowOne_ACL_ACE_List(RULE_TYPE_IP_STD_ACL, "123", *ace_p, 0, 1);
#endif

   return CLI_NO_ERROR;
}

static UI32_T
CLI_ACL_FillExtended(
    RULE_TYPE_UI_Ace_Entry_T *ace_p,
    char *arg[])
{
    UI16_T ptr = 0;
    UI32_T  arg_position = 0;

    /* Not suppport yet
     */
#if 0 && (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
    /* mac */
    if (arg[arg_position][0] == 'm' || arg[arg_position][0] == 'M')
    {
        arg_position++;

        if (CLI_NO_ERROR != CLI_ACL_PrivGetMacAddr(arg, &arg_position,
                                                   ace_p->ether.sa.addr,
                                                   ace_p->ether.sa.mask))
        {
            return CLI_ERR_INTERNAL;
        }

        if (CLI_NO_ERROR != CLI_ACL_PrivGetMacAddr(arg, &arg_position,
                                                   ace_p->ether.da.addr,
                                                   ace_p->ether.da.mask))
        {
            return CLI_ERR_INTERNAL;
        }

        /* vid */
        if (arg[arg_position][0] == 'v' || arg[arg_position][0] == 'V')
        {
            arg_position++;

            if (CLI_NO_ERROR != CLI_ACL_PrivGetVid(arg, &arg_position,
                                                   &ace_p->ether.vid.op,
                                                   &ace_p->ether.vid.u.s.data,
                                                   &ace_p->ether.vid.u.s.data,
                                                   &ace_p->ether.vid.u.s.mask))
            {
                return CLI_ERR_INTERNAL;
            }
        }
    }
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */

    switch(arg[arg_position][0])
    {
        case 't':
        case 'T':
            ace_p->ipv4.protocol.op = RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL;
            ace_p->ipv4.protocol.u.s.data = RULE_TYPE_ACL_TCP_PROTOCOL;
            ace_p->ipv4.protocol.u.s.mask = 255;

            arg_position++;
            break;

        case 'u':
        case 'U':
            ace_p->ipv4.protocol.op = RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL;
            ace_p->ipv4.protocol.u.s.data = RULE_TYPE_ACL_UDP_PROTOCOL;
            ace_p->ipv4.protocol.u.s.mask = 255;

            arg_position++;
            break;

        case 'i':
        case 'I':
            ace_p->ipv4.protocol.op = RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL;
            ace_p->ipv4.protocol.u.s.data = RULE_TYPE_ACL_ICMP_PROTOCOL;
            ace_p->ipv4.protocol.u.s.mask = 255;

            arg_position++;
            break;

        default:
            if (CLI_LIB_CheckIPAddr(arg[arg_position],&ptr) == 0)
            {
                break;
            }
            else if (isdigit(arg[arg_position][0]))
            {
                ace_p->ipv4.protocol.op = RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL;
                ace_p->ipv4.protocol.u.s.data = atoi(arg[arg_position]);
                ace_p->ipv4.protocol.u.s.mask = 255;

                arg_position++;
            }
            break;
    }

    if (CLI_NO_ERROR != CLI_ACL_PrivGetIpAddr(arg, &arg_position,
                                                ace_p->ipv4.sip.addr,
                                                ace_p->ipv4.sip.mask))
    {
        return CLI_ERR_INTERNAL;
    }

    if (CLI_NO_ERROR != CLI_ACL_PrivGetIpAddr(arg, &arg_position,
                                                ace_p->ipv4.dip.addr,
                                                ace_p->ipv4.dip.mask))
    {
        return CLI_ERR_INTERNAL;
    }

    /*check options*/
    {
        while(arg[arg_position] != NULL)
        {

            switch(arg[arg_position][0])
            {
                case 'c':/*control-flag code*/
                case 'C':
                    ace_p->tcp.flags.data.u.code = atoi(arg[arg_position+1]);

                    arg_position = arg_position + 2;

                    ace_p->tcp.flags.mask.u.code = MAX_diffServIpAceControlCodeBitmask;

                    if (arg[arg_position] != NULL)
                    {
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                        if (arg[arg_position][0] == 't' || arg[arg_position][0] == 'T')
                        {
                            break;
                        }
                        else
#endif /* SYS_CPNT_TIME_BASED_ACL */
                        {
                            ace_p->tcp.flags.mask.u.code = atoi(arg[arg_position]);
                            arg_position = arg_position + 1;
                        }
                    }
                    break;

                case 'd':/*destination-port or dscp*/
                case 'D':
                    switch(arg[arg_position][1])
                {
                    case 'e':/*destination-port*/
                    case 'E':
                    default:/*destination-port may be last option and do not have dscp option*/
                        ace_p->l4_common.dport.op = VAL_diffServIpAceDestPortOp_equal;
                        ace_p->l4_common.dport.u.s.data = atoi(arg[arg_position+1]);
                        ace_p->l4_common.dport.u.s.mask = MAX_diffServIpAceDestPortBitmask;

                        arg_position = arg_position + 2;

                        if (arg[arg_position] != NULL)
                        {
                            if (!isdigit(arg[arg_position][0]))
                            {
                                break;
                            }
                            else
                            {
                                ace_p->l4_common.dport.u.s.mask = atoi(arg[arg_position]);
                                arg_position = arg_position + 1;
                            }
                        }
                        break;

                    case 's':/*dscp*/
                    case 'S':
                        ace_p->ipv4.dscp.op = RULE_TYPE_IPV4_DSCP_OP_DSCP;
                        ace_p->ipv4.dscp.u.ds = atoi(arg[arg_position+1]);

                        arg_position = arg_position + 2;

                        break;

                }
                    break;

                case 's':/*sport*/
                case 'S':
                    ace_p->l4_common.sport.op = VAL_diffServIpAceSourcePortOp_equal;
                    ace_p->l4_common.sport.u.s.data = atoi(arg[arg_position+1]);
                    ace_p->l4_common.sport.u.s.mask = MAX_diffServIpAceSourcePortBitmask;

                    arg_position = arg_position + 2;

                    if (arg[arg_position] != NULL)
                    {
                        if (!isdigit(arg[arg_position][0]))
                        {
                            break;
                        }
                        else
                        {
                            ace_p->l4_common.sport.u.s.mask = atoi(arg[arg_position]);
                            arg_position = arg_position + 1;
                        }
                    }
                    break;

                case 'p':/*Precedence*/
                case 'P':
                    ace_p->ipv4.dscp.op = (ace_p->ipv4.dscp.op == RULE_TYPE_IPV4_DSCP_OP_TOS) ?
                    RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE_AND_TOS :
                    RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE;
                    ace_p->ipv4.dscp.u.tos.precedence = atoi(arg[arg_position+1]);

                    arg_position = arg_position + 2;

                    break;

                case 't':/*tos*/
                case 'T':
                    if ( ('o' == arg[arg_position][1]) || ('O' == arg[arg_position][1]) )
                    {
                        ace_p->ipv4.dscp.op = (ace_p->ipv4.dscp.op == RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE) ?
                        RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE_AND_TOS :
                        RULE_TYPE_IPV4_DSCP_OP_TOS;
                        ace_p->ipv4.dscp.u.tos.tos = atoi(arg[arg_position+1]);

                        arg_position = arg_position + 2;
                    }
                    else
                    {
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                        if (CLI_ERR_INTERNAL == CLI_ACL_TIME_RANGE_FillTimeRangeName(
                                                                                      &arg[arg_position],
                                                             ace_p->time_range_name))
                        {
                            return CLI_ERR_INTERNAL;
                        }

                        arg_position = arg_position + 2;
#endif /* end of #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */
                    }
                    break;
                case 'i': /* icmptype */
                case 'I':
                    ace_p->icmp.icmp_type.op = RULE_TYPE_ICMP_ICMPTYPE_OP_EQUAL;
                    ace_p->icmp.icmp_type.u.s.data = atoi(arg[arg_position+1]);
                    ace_p->icmp.icmp_type.u.s.mask = 0xff;

                    arg_position = arg_position + 2;
                    break;

                default:
                    break;
            }
        }
    }
#if (CLI_UNITEST == 1)
    CLI_ACL_ShowOne_ACL_ACE_List(RULE_TYPE_IP_EXT_ACL, "123", *ace_p, 0, 1);
#endif

    return CLI_NO_ERROR;
}

static UI32_T
CLI_ACL_FillIPv6Standard(
    RULE_TYPE_UI_Ace_Entry_T *ace_p,
    char *arg[])
{
    UI32_T          arg_pos;
    L_INET_AddrIp_T src_adr;

    arg_pos = 0;

    ASSERT(ace_p != NULL);

    if (NULL == arg[arg_pos])
    {
        return CLI_ERR_INTERNAL;
    }

    switch ( arg [arg_pos][0] )
    {
        case 'h': /* host src-ipv6 */
        case 'H':
            arg_pos ++;

            if (L_INET_RETURN_SUCCESS == L_INET_StringToInaddr(L_INET_FORMAT_IPV6_GLOBAL,
                                                               arg[arg_pos],
                                                               (L_INET_Addr_T *)&src_adr,
                                                               sizeof(src_adr)))
            {
                memcpy (ace_p->ipv6.sip.addr, src_adr.addr, sizeof(ace_p->ipv6.sip.addr));
                ace_p->ipv6.sip.prefix_len = RULE_TYPE_MAX_IPV6_PREFIX_LEN;
            }
            else
            {
                CLI_LIB_PrintStr_1 ("Failed to get source-ipv6-address %s.\r\n", arg [1]);
                return CLI_ERR_INTERNAL;
            }

            break;

        default: /* src-ipv6/pfx */

            if (L_INET_RETURN_SUCCESS == L_INET_StringToInaddr(L_INET_FORMAT_IPV6_ADDR_RAW,
                                                               arg[arg_pos],
                                                               (L_INET_Addr_T *)&src_adr,
                                                               sizeof(src_adr)))
            {
                memcpy (ace_p->ipv6.sip.addr, src_adr.addr, sizeof(ace_p->ipv6.sip.addr));
                ace_p->ipv6.sip.prefix_len = src_adr.preflen;
            }
            else
            {
                switch ( arg [arg_pos][0] )
                {
                    case 'a': /* any */
                    case 'A':
                        break;

                    default:
                        return CLI_ERR_INTERNAL;
                        break;
                }
            }

            break;
    }

    arg_pos ++;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    if (CLI_ERR_INTERNAL == CLI_ACL_TIME_RANGE_FillTimeRangeName(&arg[arg_pos],
                                                                  ace_p->time_range_name))
    {
        return CLI_ERR_INTERNAL;
    }
#endif /* end of #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */

    return CLI_NO_ERROR;
}

static UI32_T
CLI_ACL_FillIPv6Extended(
    RULE_TYPE_UI_Ace_Entry_T *ace_p,
    char *arg[])
{
    UI32_T arg_pos;

    arg_pos = 0;

    ASSERT(ace_p != NULL);

    if (NULL == arg[arg_pos])
    {
        return CLI_ERR_INTERNAL;
    }

#if (SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER_NEW_FORMAT == TRUE)
    switch(arg[arg_pos][0])
    {
        case 't': /* TCP */
        case 'T':
            ace_p->ipv6.next_header.op = RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL;
            ace_p->ipv6.next_header.u.s.data = RULE_TYPE_ACL_IPV6_TCP_NEXT_HEADER;
            ace_p->ipv6.next_header.u.s.mask = MAX_diffServIpv6AceNextHeader - 1;
            arg_pos++;
            break;

        case 'u': /* UDP */
        case 'U':
            ace_p->ipv6.next_header.op = RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL;
            ace_p->ipv6.next_header.u.s.data = RULE_TYPE_ACL_IPV6_UDP_NEXT_HEADER;
            ace_p->ipv6.next_header.u.s.mask = MAX_diffServIpv6AceNextHeader - 1;
            arg_pos++;
            break;

#if (SYS_CPNT_ACL_IPV6_EXT_ICMP == TRUE)
        case 'i': /* ICMP */
        case 'I':
            ace_p->ipv6.next_header.op = RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL;
            ace_p->ipv6.next_header.u.s.data = RULE_TYPE_ACL_IPV6_ICMP_NEXT_HEADER;
            ace_p->ipv6.next_header.u.s.mask = MAX_diffServIpv6AceNextHeader - 1;
            arg_pos++;
            break;
#endif

        default:
            if (isdigit(arg[arg_pos][0]))
            {
                L_INET_AddrIp_T inet_addr;

                if (L_INET_RETURN_SUCCESS == L_INET_StringToInaddr(L_INET_FORMAT_IPV6_GLOBAL_PREFIX,
                                                               arg[arg_pos],
                                                               (L_INET_Addr_T *)&inet_addr,
                                                               sizeof(inet_addr)))
                {
                    break;
                }
                else
                {
                    ace_p->ipv6.next_header.op = RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL;
                    ace_p->ipv6.next_header.u.s.data = atoi(arg[arg_pos]);
                    ace_p->ipv6.next_header.u.s.mask = MAX_diffServIpv6AceNextHeader - 1;
                    arg_pos++;
                }
            }
            break;
    }
#endif /* SYS_CPNT_ACL_IPV6_EXT_NEXT_HEADER_NEW_FORMAT */

#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
    /* mac SA, DA */
    if (arg[arg_pos][0] == 'm' || arg[arg_pos][0] == 'M')
    {
        arg_pos++;

        if (CLI_NO_ERROR != CLI_ACL_PrivGetMacAddr(arg, &arg_pos,
                                                   ace_p->ether.sa.addr,
                                                   ace_p->ether.sa.mask))
        {
            return CLI_ERR_INTERNAL;
        }

        if (CLI_NO_ERROR != CLI_ACL_PrivGetMacAddr(arg, &arg_pos,
                                                   ace_p->ether.da.addr,
                                                   ace_p->ether.da.mask))
        {
            return CLI_ERR_INTERNAL;
        }

        /* vid */
        if (arg[arg_pos][0] == 'v' || arg[arg_pos][0] == 'V')
        {
            arg_pos++;

            if (CLI_NO_ERROR != CLI_ACL_PrivGetVid(arg, &arg_pos,
                                                   &ace_p->ether.vid.op,
                                                   &ace_p->ether.vid.u.s.data,
                                                   &ace_p->ether.vid.u.s.data,
                                                   &ace_p->ether.vid.u.s.mask))
            {
                return CLI_ERR_INTERNAL;
            }
        }
    }
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */

#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
    if (CLI_NO_ERROR != CLI_ACL_PrivGetIp6Address(arg,
                                                  &arg_pos,
                                                  ace_p->ipv6.sip.addr,
                                                  &ace_p->ipv6.sip.prefix_len))
    {
        return CLI_ERR_INTERNAL;
    }
#endif /* SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR */

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
    if (CLI_NO_ERROR != CLI_ACL_PrivGetIp6Address(arg,
                                                  &arg_pos,
                                                  ace_p->ipv6.dip.addr,
                                                  &ace_p->ipv6.dip.prefix_len))
    {
        return CLI_ERR_INTERNAL;
    }
#endif /* SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR */

    while (arg[arg_pos] != NULL)
    {
        switch ( arg[arg_pos][0] )
        {
            case 'n': /* next-header */
            case 'N':
                ace_p->ipv6.next_header.op = RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL;
                ace_p->ipv6.next_header.u.s.data = atoi(arg[arg_pos + 1]);
                ace_p->ipv6.next_header.u.s.mask =  MAX_diffServIpv6AceNextHeader - 1;
                break;

            case 'f': /* flow-label */
            case 'F':
                ace_p->ipv6.flow_label.op = RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL;
                ace_p->ipv6.flow_label.u.s.data = atoi(arg[arg_pos + 1]);
                ace_p->ipv6.flow_label.u.s.mask = MAX_diffServIpv6AceFlowLabel - 1;
                break;

            case 'd':/* destination-port or dscp */
            case 'D':
                switch (arg[arg_pos][1])
                {
                    case 'e': /* destination-port */
                    case 'E':
                    default:/* destination-port may be last option and do not have dscp option */
                        ace_p->l4_common.dport.op = VAL_diffServIpAceDestPortOp_equal;
                        ace_p->l4_common.dport.u.s.data = atoi(arg[arg_pos+1]);
                        ace_p->l4_common.dport.u.s.mask = MAX_diffServIpAceDestPortBitmask;

                        if (arg[arg_pos+2] != NULL)
                        {
                            if (!isdigit(arg[arg_pos+2][0]))
                            {
                                break;
                            }
                            else
                            {
                                ace_p->l4_common.dport.u.s.mask = atoi(arg[arg_pos+2]);
                                arg_pos++;
                            }
                        }
                        break;

                    case 's':/*dscp*/
                    case 'S':
                        ace_p->ipv6.traffic_class.op = RULE_TYPE_IPV6_TRAFFIC_CLASS_OP_EQUAL;
                        ace_p->ipv6.traffic_class.u.s.data = (UI8_T)(atoi(arg[arg_pos + 1]) << 2);
                        ace_p->ipv6.traffic_class.u.s.mask = (UI8_T)((MAX_diffServIpv6AceDscp - 1) << 2);
                        break;
                }
                break;


            case 's':/* source-port */
            case 'S':
                ace_p->l4_common.sport.op = VAL_diffServIpAceSourcePortOp_equal;
                ace_p->l4_common.sport.u.s.data = atoi(arg[arg_pos+1]);
                ace_p->l4_common.sport.u.s.mask = MAX_diffServIpAceSourcePortBitmask;

                if (arg[arg_pos+2] != NULL)
                {
                    if (!isdigit(arg[arg_pos+2][0]))
                    {
                        break;
                    }
                    else
                    {
                        ace_p->l4_common.sport.u.s.mask = atoi(arg[arg_pos+2]);
                        arg_pos++;
                    }
                }
                break;

            case 'i':/* icmp-type */
            case 'I':
                ace_p->icmp.icmp_type.op = RULE_TYPE_ICMP_ICMPTYPE_OP_EQUAL;
                ace_p->icmp.icmp_type.u.s.data = atoi(arg[arg_pos + 1]);
                ace_p->icmp.icmp_type.u.s.mask = 0xff;
                break;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
            case 't':
            case 'T':    /* time range */
                if (CLI_ERR_INTERNAL == CLI_ACL_TIME_RANGE_FillTimeRangeName(&arg[arg_pos], ace_p->time_range_name))
                {
                    return CLI_NO_ERROR;
                }
                break;
#endif /* end of #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */

            default:
                return CLI_ERR_INTERNAL;
        }

        arg_pos += 2;
    }

    return CLI_NO_ERROR;
}

#if ( (SYS_CPNT_DAI == TRUE) && (SYS_CPNT_ACL == TRUE) )
static UI32_T
CLI_ACL_FillARP(
    RULE_TYPE_UI_Ace_Entry_T *ace_p,
    char *arg[])
{
    RULE_TYPE_UI_ARP_T *arp_ace_p;
    UI32_T arg_position;

    arp_ace_p = &ace_p->arp;
    arg_position = 0;

    if (arg[arg_position][0] == 'i' || arg[arg_position][0] == 'I') /* ip */
    {
	arg_position++;
	arp_ace_p->arp_type = RULE_TYPE_ARP_BOTH;
    }
    else if (arg[arg_position][0] == 'r' || arg[arg_position][0] == 'R') /* request or response */
    {
	if (arg[arg_position][2] == 'q' || arg[arg_position][2] == 'Q') /* request */
	{
 	    arp_ace_p->arp_type = RULE_TYPE_ARP_REQUEST;
	}
	else if (arg[arg_position][2] == 's' || arg[arg_position][2] == 'S') /* response */
	{
	    arp_ace_p->arp_type = RULE_TYPE_ARP_REPLY;
	}
	else
	{
	    return CLI_ERR_INTERNAL;
	}

	arg_position += 2; /*skip 'ip' keyword*/
    }
    else
    {
        return CLI_ERR_INTERNAL;
    }

    if (CLI_NO_ERROR != CLI_ACL_PrivGetIpAddr(arg, &arg_position,
                                                arp_ace_p->sender.ip.addr,
                                                arp_ace_p->sender.ip.mask))
    {
        return CLI_ERR_INTERNAL;
    }

    switch(arg[arg_position][0])
    {
        case 'm':
        case 'M':
            arg_position++;
            if (CLI_NO_ERROR != CLI_ACL_PrivGetMacAddr(arg, &arg_position,
                                                       arp_ace_p->sender.mac.addr,
                                                       arp_ace_p->sender.mac.mask))
            {
                return CLI_ERR_INTERNAL;
            }

            if(arg[arg_position] == NULL)
            {
                break;
            }

            switch(arg[arg_position][0])
            {
                case 'l':
                case 'L':
                    arp_ace_p->is_log = TRUE;
                    break;

                default:
                    if (CLI_NO_ERROR != CLI_ACL_PrivGetMacAddr(arg, &arg_position,
                                                               arp_ace_p->target.mac.addr,
                                                               arp_ace_p->target.mac.mask))
                    {
                        return CLI_ERR_INTERNAL;
                    }

                    if(arg[arg_position] != NULL)
                    {
                        arp_ace_p->is_log = TRUE;
                    }
                    break;
            }

            break;

        default:
            if (CLI_NO_ERROR != CLI_ACL_PrivGetIpAddr(arg, &arg_position,
                                                        arp_ace_p->target.ip.addr,
                                                        arp_ace_p->target.ip.mask))
            {
                return CLI_ERR_INTERNAL;
            }

            arg_position++;/*skip 'mac' keyword*/

            if (CLI_NO_ERROR != CLI_ACL_PrivGetMacAddr(arg, &arg_position,
                                                        arp_ace_p->sender.mac.addr,
                                                        arp_ace_p->sender.mac.mask))
            {
                return CLI_ERR_INTERNAL;
            }

            if(arg[arg_position] == NULL)
            {
                break;
            }

            switch(arg[arg_position][0])
            {
                case 'l':
                case 'L':
                    arp_ace_p->is_log = TRUE;
                    break;

                default:
                    if (CLI_NO_ERROR != CLI_ACL_PrivGetMacAddr(arg, &arg_position,
                                                               arp_ace_p->target.mac.addr,
                                                               arp_ace_p->target.mac.mask))
                    {
                        return CLI_ERR_INTERNAL;
                    }

                    if (arg[arg_position] != NULL)
                    {
                        arp_ace_p->is_log = TRUE;
                    }
                    break;
            }
            break;
    }


#if (CLI_UNITEST == 1)
    //CLI_ACL_ShowOne_ACL_ACE_List(RULE_TYPE_IP_STD_ACL, "123", *src, 0, 1);
#endif
    return CLI_NO_ERROR;
}
#endif /* end of #if ( (SYS_CPNT_DAI == TRUE) && (SYS_CPNT_ACL == TRUE) ) */

static UI32_T
CLI_API_SET_ACL_MAC(
    UI32_T acl_action,
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P)
{
    /* [no] {permit | deny} [tagged-eth2 | untagged-eth2 | tagged-802.3 | untagged-802.3]
     *                      {any | host sa | sa sa_mask} {any | host da | da da_mask}
     *                      [vid vid_data [vid_mask]]
     *                      [ ethertype ethertype_data [ethertype_mask]  |
     *                        [
     *                          ip {any | host sip | sip sip_mask} {any | host dip | dip dip_mask} |
     *                          ipv6 {any | sip/prefix_len} {any | dip/prefix_len}
     *                        ]
     *                        [protocol protocol_data]
     *                        [l4-source-port sport [sport_mask]]
     *                        [l4_destination-port dport [dport_mask]]
     *                      ]
     *                      [time-range name]
     */
    UI32_T  cli_ret = 0;

#if (SYS_CPNT_ACL == TRUE)
    RULE_TYPE_UI_Ace_Entry_T ace;

    ace.ace_type = RULE_TYPE_MAC_ACL;
    L4_PMGR_ACL_InitUIAce(&ace);
    ace.access = acl_action;

    if ((cli_ret = CLI_ACL_FillMAC(&ace, arg)) == CLI_NO_ERROR)
    {
        switch(cmd_idx)
        {
            case PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY:
            case PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT:
                switch (L4_PMGR_ACL_SetUIAce2Acl(ctrl_P->CMenu.acl_name,
                                                 ctrl_P->CMenu.acl_type,
                                                 &ace))
                {
                    case RULE_TYPE_OK:
                        break;

                    case RULE_TYPE_FREE_SELECTOR_UNAVAILABLE:
#if (SYS_CPNT_EH == TRUE)
                        CLI_API_Show_Exception_Handeler_Msg ();
#else
                        CLI_LIB_PrintStr (ERR_MSG_FREE_SELECTOR_UNAVAILABLE);
#endif
                        break;

                    default:
#if (SYS_CPNT_EH == TRUE)
                        CLI_API_Show_Exception_Handeler_Msg();
#else
                        CLI_LIB_PrintStr("Failed to set MAC ACE.\r\n");
#endif
                        break;
                }
                break;

            case PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY:
            case PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT:
                if (RULE_TYPE_OK != L4_PMGR_ACL_DelUIAceFromAcl(
                        ctrl_P->CMenu.acl_name, ctrl_P->CMenu.acl_type, &ace))
                {
#if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg();
#else
                    CLI_LIB_PrintStr("Failed to delete MAC ACE.\r\n");
#endif
                }
                break;

            default:
                break;
        }
    }
#if (CLI_ACL_TEST == 1)
    CLI_ACL_ShowOne_ACL_ACE_List(ctrl_P->CMenu.acl_type, "123", ace, 0, 1);
#endif
#endif

    return CLI_NO_ERROR;
}

#if (SYS_CPNT_DAI == TRUE)
static UI32_T CLI_API_SET_ACL_ARP(UI32_T acl_action, UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T cli_ret = 0;
    RULE_TYPE_UI_Ace_Entry_T ace;


    ace.ace_type = RULE_TYPE_ARP_ACL;
    L4_PMGR_ACL_InitUIAce(&ace);

    ace.access = acl_action;

    if ((cli_ret = CLI_ACL_FillARP(&ace, arg)) == CLI_NO_ERROR)
    {
        switch(cmd_idx)
        {
            case PRIVILEGE_CFG_ACL_ARP_CMD_W1_DENY:
            case PRIVILEGE_CFG_ACL_ARP_CMD_W1_PERMIT:
                if ( L4_PMGR_ACL_SetUIAce2Acl(ctrl_P->CMenu.acl_name, ctrl_P->CMenu.acl_type, &ace) != RULE_TYPE_OK )
                {
                    CLI_LIB_PrintStr("Failed to set ARP ACE.\r\n");
                }
                break;

            case PRIVILEGE_CFG_ACL_ARP_CMD_W2_NO_DENY:
            case PRIVILEGE_CFG_ACL_ARP_CMD_W2_NO_PERMIT:
                if (L4_PMGR_ACL_DelUIAceFromAcl(ctrl_P->CMenu.acl_name, ctrl_P->CMenu.acl_type, &ace) != RULE_TYPE_OK )
                {
                    CLI_LIB_PrintStr("Failed to delete ARP ACE.\r\n");
                }
                break;

            default:
                break;
        }
    }
#if (CLI_ACL_TEST == 1)
    CLI_ACL_ShowOne_ACL_ACE_List(ctrl_P->CMenu.acl_type, "123", ace, 0, 1);
#endif

    return cli_ret;
}
#endif /* end of #if (SYS_CPNT_DAI == TRUE) */

static UI32_T
CLI_API_SET_ACL_Standard(
    UI32_T acl_action,
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T  cli_ret = 0;

#if (SYS_CPNT_ACL == TRUE)
    RULE_TYPE_UI_Ace_Entry_T ace;

    ace.ace_type = RULE_TYPE_IP_STD_ACL;
    L4_PMGR_ACL_InitUIAce(&ace);

    ace.access = acl_action;

    if ((cli_ret = CLI_ACL_FillStandard(&ace, arg)) == CLI_NO_ERROR)
    {
        switch(cmd_idx)
        {
            case PRIVILEGE_CFG_ACL_STANDARD_CMD_W1_DENY:
            case PRIVILEGE_CFG_ACL_STANDARD_CMD_W1_PERMIT:

                switch (L4_PMGR_ACL_SetUIAce2Acl(ctrl_P->CMenu.acl_name,
                            ctrl_P->CMenu.acl_type, &ace))
                {
                    case RULE_TYPE_OK:
                        break;

                    case RULE_TYPE_FREE_SELECTOR_UNAVAILABLE:
#if (SYS_CPNT_EH == TRUE)
                        CLI_API_Show_Exception_Handeler_Msg ();
#else
                        CLI_LIB_PrintStr (ERR_MSG_FREE_SELECTOR_UNAVAILABLE);
#endif
                        break;

                    default:
#if (SYS_CPNT_EH == TRUE)
                        CLI_API_Show_Exception_Handeler_Msg();
#else
                        CLI_LIB_PrintStr("Failed to set IP standard ACE.\r\n");
#endif
                        break;
                }
                break;

            case PRIVILEGE_CFG_ACL_STANDARD_CMD_W2_NO_DENY:
            case PRIVILEGE_CFG_ACL_STANDARD_CMD_W2_NO_PERMIT:

                if (RULE_TYPE_OK != L4_PMGR_ACL_DelUIAceFromAcl(
                        ctrl_P->CMenu.acl_name, ctrl_P->CMenu.acl_type, &ace))
                {
#if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg();
#else
                    CLI_LIB_PrintStr("Failed to delete IP standard ACE.\r\n");
#endif
                }
                break;

            default:
                break;
        }
    }
#if (CLI_ACL_TEST == 1)
    CLI_ACL_ShowOne_ACL_ACE_List(ctrl_P->CMenu.acl_type, "123", ace, 0, 1);
#endif
#endif

    return CLI_NO_ERROR;
}

static UI32_T
CLI_API_SET_ACL_Extended(
    UI32_T acl_action,
    UI16_T cmd_idx,
    char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P)
{
    /*  [no] {permit|deny} [protocl_val | tcp | upd]
     *                     [mac {any | host s_addr | s_addr s_mask}        (not support "mac" yet)
     *                          {any | host d_addr | d_addr d_mask}
     *                          [vid vid_val [vid_mask]]]
     *                     {any | host sip_addr | sip_addr sip_mask}
     *                     {any | host dip_addr | dip_addr dip_mask}
     *                     [precedence precedence_val | tos tos_val | dscp dscp_val]
     *                     [source-port sport_val [sport_mask]]
     *                     [destination-port dport_val [dport_mask]]
     *                     [time-range time_range_name]
     */
    UI32_T cli_ret = CLI_NO_ERROR;

#if (SYS_CPNT_ACL == TRUE)
    RULE_TYPE_UI_Ace_Entry_T ace;

    ace.ace_type = RULE_TYPE_IP_EXT_ACL;
    L4_PMGR_ACL_InitUIAce(&ace);

    ace.access = acl_action;

    if ((cli_ret = CLI_ACL_FillExtended(&ace, arg)) == CLI_NO_ERROR)
    {
        switch(cmd_idx)
        {
        case PRIVILEGE_CFG_ACL_EXTENDED_CMD_W1_DENY:
        case PRIVILEGE_CFG_ACL_EXTENDED_CMD_W1_PERMIT:

            if (L4_PMGR_ACL_SetUIAce2Acl(ctrl_P->CMenu.acl_name,
                    ctrl_P->CMenu.acl_type, &ace)
                != RULE_TYPE_OK )
            {
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr("Failed to set IP extended ACE.\r\n");
#endif
            }
            break;

      case PRIVILEGE_CFG_ACL_EXTENDED_CMD_W2_NO_DENY:
      case PRIVILEGE_CFG_ACL_EXTENDED_CMD_W2_NO_PERMIT:

            if (L4_PMGR_ACL_DelUIAceFromAcl(ctrl_P->CMenu.acl_name,
                    ctrl_P->CMenu.acl_type, &ace)
                != RULE_TYPE_OK )
            {
#if (SYS_CPNT_EH == TRUE)
                CLI_API_Show_Exception_Handeler_Msg();
#else
                CLI_LIB_PrintStr("Failed to delete IP extended ACE.\r\n");
#endif
            }
            break;

        default:
            break;
        }
    }
#if (CLI_ACL_TEST == 1)
    CLI_ACL_ShowOne_ACL_ACE_List(ctrl_P->CMenu.acl_type, "123", ace, 0, 1);
#endif
#endif

    return CLI_NO_ERROR;
}

#if (SYS_CPNT_ACL_COUNTER == TRUE)
static UI32_T
CLI_ACL_PrivFillAclName(
    UI32_T *index,
    const char *arg[],
    const char **acl_name)
{
    /* name acl-name
     */
    if (NULL != arg[*index] &&
        ('n' == arg[*index][0] || 'N' == arg[*index][0]))
    {
        if (NULL == arg[*index + 1])
        {
            return CLI_ERR_INTERNAL;
        }

        *acl_name = arg[*index + 1];

        (*index) += 2;
    }

    return CLI_NO_ERROR;
}

static UI32_T
CLI_ACL_PrivFillDirection(
    UI32_T *index,
    const char *arg[],
    RULE_TYPE_InOutDirection_T *direction)
{
    /* direction {in | out}
     */
    if (NULL != arg[*index] &&
        ('d' == arg[*index][0] || 'D' == arg[*index][0]))
    {
        if (NULL == arg[*index + 1])
        {
            return CLI_ERR_INTERNAL;
        }

        if ('i' == arg[*index + 1][0] ||
            'I' == arg[*index + 1][0])
        {
            *direction = RULE_TYPE_INBOUND;
        }
        else if ('o' == arg[*index + 1][0] ||
                 'O' == arg[*index + 1][0])
        {
            *direction = RULE_TYPE_OUTBOUND;
        }
        else
        {
            return CLI_ERR_INTERNAL;
        }


        (*index) += 2;
    }

    return CLI_NO_ERROR;
}

static UI32_T
CLI_ACL_PrivFillUnitPort(
    UI32_T *index,
    const char *arg[],
    UI32_T *unit,
    UI32_T *port)
{
    /* interface ethernet unit/port
     */
    if (NULL != arg[*index] &&
        ('i' == arg[*index][0] || 'I' == arg[*index][0]))
    {
        if (NULL == arg[*index + 1])
        {
            return CLI_ERR_INTERNAL;
        }

        if (NULL == arg[*index + 1] ||
            ('e' != arg[*index + 1][0] && 'E' != arg[*index + 1][0]) ||
            NULL == arg[*index + 2])
        {
            return CLI_ERR_INTERNAL;
        }

        if (FALSE == CLI_LIB_GetUnitPortFromString(arg[*index + 2], unit, port))
        {
            return CLI_ERR_INTERNAL;
        }

        (*index) += 3;
    }

    return CLI_NO_ERROR;
}
#endif /* SYS_CPNT_ACL_COUNTER */


#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
static UI32_T
CLI_ACL_PrivFillTimeRangeName(
    UI32_T *index,
    const char *arg[],
    const char **time_range_name)
{
    /* time-range time-range-name
     */
    if (NULL != arg[*index] &&
        ('t' == arg[*index][0] || 'T' == arg[*index][0]))
    {
        if (NULL == arg[*index + 1])
        {
            return CLI_ERR_INTERNAL;
        }

        *time_range_name = arg[*index + 1];

        (*index) += 2;
    }

    return CLI_NO_ERROR;
}
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
static UI32_T
CLI_ACL_PrivFillCounter(
    UI32_T *index,
    const char *arg[],
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable)
{
    /* counter
     */
    if (NULL != arg[*index] &&
        ('c' == arg[*index][0] || 'C' == arg[*index][0]))
    {
        *counter_enable = RULE_TYPE_COUNTER_ENABLE;

        (*index) += 1;
    }

    return CLI_NO_ERROR;
}
#endif /* SYS_CPNT_ACL_COUNTER */

/** Convert type of access-list to access-mode
 **/
static UI8_T
CLI_ACL_PrivGetAccessModeFromAclType(
    RULE_TYPE_AclType_T acl_type)
{
    switch (acl_type)
    {
        case RULE_TYPE_MAC_ACL:
            return PRIVILEGE_CFG_ACL_MAC_MODE;

        case RULE_TYPE_IP_STD_ACL:
            return PRIVILEGE_CFG_ACL_STANDARD_MODE;

        case RULE_TYPE_IP_EXT_ACL:
            return PRIVILEGE_CFG_ACL_EXTENDED_MODE;

        case RULE_TYPE_IPV6_STD_ACL:
            return PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE;

        case RULE_TYPE_IPV6_EXT_ACL:
            return PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE;

#if (SYS_CPNT_DAI == TRUE)
        case RULE_TYPE_ARP_ACL:
            return PRIVILEGE_CFG_ACL_ARP_MODE;
#endif /* SYS_CPNT_DAI */

        default:
            break;
    }

    return PRIVILEGE_CFG_GLOBAL_MODE;
}

/** Create ACL and change access-mode
 **/
static BOOL_T
CLI_ACL_PrivEnterAccessListMode(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    CLI_TASK_WorkingArea_T *ctrl_P)
{
    RULE_TYPE_RETURN_TYPE_T result;

    result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);

    switch (result)
    {
        case RULE_TYPE_OK:
        case RULE_TYPE_EXISTED:
        case RULE_TYPE_ACL_EXISTED:

            ctrl_P->CMenu.AccMode = CLI_ACL_PrivGetAccessModeFromAclType(acl_type);
            ctrl_P->CMenu.acl_type = acl_type;
            strncpy(ctrl_P->CMenu.acl_name,  acl_name, sizeof(ctrl_P->CMenu.acl_name)-1 );
            ctrl_P->CMenu.acl_name[ sizeof(ctrl_P->CMenu.acl_name)-1 ] = '\0';

            break;

        case RULE_TYPE_ACL_FULLED:
            CLI_LIB_PrintStr("There are too many ACLs.\r\n");
            return FALSE;

        case RULE_TYPE_FAIL:
        default:
            CLI_LIB_PrintStr_1("Failed to create %s access-list.\r\n", acl_name);
            return FALSE;
    }

    return TRUE;
}

/* bind/unbind access-list to eth
 * mac access-group <acl-name> {in | out} [time-range <time-range-name>] [counter]
 */
static UI32_T
CLI_API_PrivAccessGroup_Eth(
    UI16_T cmd_idx,
    const char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T  i               = 0;
    UI32_T  ifindex         = 0;

    RULE_TYPE_COUNTER_ENABLE_T  counter_enable = RULE_TYPE_COUNTER_DISABLE;

    const char *acl_name = NULL;
    const char *time_range_name = NULL;

#if (SYS_CPNT_STACKING == TRUE)
    UI32_T  verify_unit = ctrl_P->CMenu.unit_id;
#else
    UI32_T  verify_unit = ctrl_P->sys_info.my_unit_id;//stacking must use this value
#endif

    UI32_T  max_port_num = 0;
    UI32_T  verify_port;

    UI32_T  arg_index;

    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_AclType_T acl_type;
    BOOL_T bind_to_port = FALSE;

    char   dir_str [12];

    CLI_API_EthStatus_T verify_ret;

    BOOL_T  acl_inout = FALSE;

    switch (cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_MAC_ACCESSGROUP:
            bind_to_port = TRUE;
            acl_type = RULE_TYPE_MAC_ACL;
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_MAC_ACCESSGROUP:
            bind_to_port = FALSE;
            acl_type = RULE_TYPE_MAC_ACL;
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_IP_ACCESSGROUP:
            bind_to_port = TRUE;
            acl_type = RULE_TYPE_IP_ACL;
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_IP_ACCESSGROUP:
            bind_to_port = FALSE;
            acl_type = RULE_TYPE_IP_ACL;
            break;

#if (SYS_CPNT_ACL_IPV6 == TRUE)
        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_IPV6_ACCESSGROUP:
            bind_to_port = TRUE;
            acl_type = RULE_TYPE_IPV6_ACL;
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_IPV6_ACCESSGROUP:
            bind_to_port = FALSE;
            acl_type = RULE_TYPE_IPV6_ACL;
            break;
#endif /* SYS_CPNT_ACL_IPV6 */

        default:
            return CLI_ERR_INTERNAL;
    }

    acl_name = arg[0];

    switch (arg[1][0])
    {
        case 'i':
        case 'I':
            acl_inout = TRUE;
            sprintf(dir_str, "ingress");
            break;

        case 'o':
        case 'O':
            acl_inout = FALSE;
            sprintf(dir_str, "egress");
            break;

        default:
            break;
    }

    /* optional argument:
     * time-range time-range-name counter
     */
    if (TRUE == bind_to_port)
    {
        arg_index = 2;

    #if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        CLI_ACL_PrivFillTimeRangeName(&arg_index, arg, &time_range_name);
    #endif /* SYS_CPNT_TIME_BASED_ACL */

    #if (SYS_CPNT_ACL_COUNTER == TRUE)
        CLI_ACL_PrivFillCounter(&arg_index, arg, &counter_enable);
    #endif /* SYS_CPNT_ACL_COUNTER */
    }

    max_port_num = SWCTRL_POM_UIGetUnitPortNumber (verify_unit);

    for (i = 1; i <= max_port_num; i++)
    {
        if (ctrl_P->CMenu.port_id_list[(UI32_T)((i-1)/8)]  & (1 << ( 7 - ((i-1)%8))) )
        {
            verify_port = i;
            verify_ret = CLI_ACL_VerifyEthernet (verify_unit, verify_port, &ifindex);
            if( verify_ret == CLI_API_ETH_NOT_PRESENT)
            {
                CLI_ACL_DisplayEthernetMsg (verify_ret, verify_unit, verify_port);
                continue;
            }
            else
            {
                result = RULE_TYPE_FAIL;

                if (TRUE == bind_to_port)
                {
                    result = L4_PMGR_ACL_BindPort2Acl(ifindex, acl_name, acl_type,
                                                      acl_inout, time_range_name,
                                                                counter_enable);
                }
                else
                {
                    result = L4_PMGR_ACL_UnBindPortFromAcl(ifindex, acl_name, acl_type,
                                                           acl_inout);
                }

                if (RULE_TYPE_OK != result)
                {
#if (SYS_CPNT_EH == TRUE)
                    CLI_API_Show_Exception_Handeler_Msg ();
#else
                    CLI_LIB_PrintStr_N("Failed to %s %s %s access-group on eth %lu/%lu.\r\n",
                                       (TRUE == bind_to_port) ? "enable" : "disable",
                                                                        dir_str,
                                            CLI_ACL_PRIV_ACL_TYPE_STR(acl_type),
                                                      verify_unit, verify_port);
#endif /* SYS_CPNT_EH */
                }
            }
        }
    }

    return CLI_NO_ERROR;
}

/** bind/unbind access-list to port-channel
 * mac access-group <acl-name> {in | out} [time-range <time-range-name>] [counter]
 */
static UI32_T
CLI_API_PrivAccessGroup_Pch(
    UI16_T cmd_idx,
    const char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS_V2_TRUNK_PORT == TRUE)
    UI32_T  ifindex;

    RULE_TYPE_COUNTER_ENABLE_T  counter_enable = RULE_TYPE_COUNTER_DISABLE;

    const char *acl_name = NULL;
    const char *time_range_name = NULL;

    UI32_T  verify_trunk_id = ctrl_P->CMenu.pchannel_id;
    CLI_API_EthStatus_T verify_ret;

    UI32_T  arg_index;

    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_AclType_T acl_type;
    BOOL_T bind_to_port = FALSE;

    char   dir_str [12];

    BOOL_T  acl_inout = FALSE;

    switch (cmd_idx)
    {
        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W2_MAC_ACCESSGROUP:
            bind_to_port = TRUE;
            acl_type = RULE_TYPE_MAC_ACL;
            break;

        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_NO_MAC_ACCESSGROUP:
            bind_to_port = FALSE;
            acl_type = RULE_TYPE_MAC_ACL;
            break;

        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W2_IP_ACCESSGROUP:
            bind_to_port = TRUE;
            acl_type = RULE_TYPE_IP_ACL;
            break;

        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_NO_IP_ACCESSGROUP:
            bind_to_port = FALSE;
            acl_type = RULE_TYPE_IP_ACL;
            break;

#if (SYS_CPNT_ACL_IPV6 == TRUE)
        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W2_IPV6_ACCESSGROUP:
            bind_to_port = TRUE;
            acl_type = RULE_TYPE_IPV6_ACL;
            break;

        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_NO_IPV6_ACCESSGROUP:
            bind_to_port = FALSE;
            acl_type = RULE_TYPE_IPV6_ACL;
            break;
#endif /* SYS_CPNT_ACL_IPV6 */

        default:
            return CLI_ERR_INTERNAL;
    }

    acl_name = arg[0];

    switch (arg[1][0])
    {
        case 'i':
        case 'I':
            acl_inout = TRUE;
            sprintf(dir_str, "ingress");
            break;

        case 'o':
        case 'O':
            acl_inout = FALSE;
            sprintf(dir_str, "egress");
            break;

        default:
            break;
    }

    /* optional argument:
     * time-range time-range-name counter
     */
    if (TRUE == bind_to_port)
    {
        arg_index = 2;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        CLI_ACL_PrivFillTimeRangeName(&arg_index, arg, &time_range_name);
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
        CLI_ACL_PrivFillCounter(&arg_index, arg, &counter_enable);
#endif /* SYS_CPNT_ACL_COUNTER */
    }

    if ((verify_ret = verify_trunk(verify_trunk_id, &ifindex)) != CLI_API_TRUNK_OK)
    {
        display_trunk_msg(verify_ret, verify_trunk_id);
        return CLI_NO_ERROR;
    }

    if (TRUE == bind_to_port)
    {
        result = L4_PMGR_ACL_BindPort2Acl(ifindex, acl_name, acl_type,
                                          acl_inout, time_range_name,
                                                    counter_enable);
    }
    else
    {
        result = L4_PMGR_ACL_UnBindPortFromAcl(ifindex, acl_name, acl_type,
                                               acl_inout);
    }

    if (RULE_TYPE_OK != result)
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr_N("Failed to %s %s %s access-group on port-channel %lu.\r\n",
            (TRUE == bind_to_port) ? "enable" : "disable",
            dir_str,
            CLI_ACL_PRIV_ACL_TYPE_STR(acl_type),
            verify_trunk_id);
#endif /* SYS_CPNT_EH */
    }

#endif /* SYS_CPNT_QOS_V2_TRUNK_PORT */

    return CLI_NO_ERROR;
}

/** bind/unbind access-list to global
 * mac access-group <acl-name> {in | out} [time-range <time-range-name>] [counter]
 */
static UI32_T
CLI_API_PrivAccessGroup_Global(
    UI16_T cmd_idx,
    const char *arg[],
    CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
    UI32_T  ifindex = RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER;

    RULE_TYPE_COUNTER_ENABLE_T  counter_enable = RULE_TYPE_COUNTER_DISABLE;

    const char *acl_name = NULL;
    const char *time_range_name = NULL;

    UI32_T  arg_index;

    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_AclType_T acl_type;
    BOOL_T bind_to_port = FALSE;

    char   dir_str [12];

    BOOL_T  acl_inout = FALSE;

    switch (cmd_idx)
    {
        case PRIVILEGE_CFG_GLOBAL_CMD_W2_MAC_ACCESSGROUP:
            bind_to_port = TRUE;
            acl_type = RULE_TYPE_MAC_ACL;
            break;

        case PRIVILEGE_CFG_GLOBAL_CMD_W3_NO_MAC_ACCESSGROUP:
            bind_to_port = FALSE;
            acl_type = RULE_TYPE_MAC_ACL;
            break;

        case PRIVILEGE_CFG_GLOBAL_CMD_W2_IP_ACCESSGROUP:
            bind_to_port = TRUE;
            acl_type = RULE_TYPE_IP_ACL;
            break;

        case PRIVILEGE_CFG_GLOBAL_CMD_W3_NO_IP_ACCESSGROUP:
            bind_to_port = FALSE;
            acl_type = RULE_TYPE_IP_ACL;
            break;

#if (SYS_CPNT_ACL_IPV6 == TRUE)
        case PRIVILEGE_CFG_GLOBAL_CMD_W2_IPV6_ACCESSGROUP:
            bind_to_port = TRUE;
            acl_type = RULE_TYPE_IPV6_ACL;
            break;

        case PRIVILEGE_CFG_GLOBAL_CMD_W3_NO_IPV6_ACCESSGROUP:
            bind_to_port = FALSE;
            acl_type = RULE_TYPE_IPV6_ACL;
            break;
#endif /* SYS_CPNT_ACL_IPV6 */

        default:
            return CLI_ERR_INTERNAL;
    }

    acl_name = arg[0];

    switch (arg[1][0])
    {
        case 'i':
        case 'I':
            acl_inout = TRUE;
            sprintf(dir_str, "ingress");
            break;

        case 'o':
        case 'O':
            acl_inout = FALSE;
            sprintf(dir_str, "egress");
            break;

        default:
            break;
    }

    /* optional argument:
     * time-range time-range-name counter
     */
    if (TRUE == bind_to_port)
    {
        arg_index = 2;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        CLI_ACL_PrivFillTimeRangeName(&arg_index, arg, &time_range_name);
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
        CLI_ACL_PrivFillCounter(&arg_index, arg, &counter_enable);
#endif /* SYS_CPNT_ACL_COUNTER */
    }

    if (TRUE == bind_to_port)
    {
        result = L4_PMGR_ACL_BindPort2Acl(ifindex, acl_name, acl_type,
                                          acl_inout, time_range_name,
                                                    counter_enable);
    }
    else
    {
        result = L4_PMGR_ACL_UnBindPortFromAcl(ifindex, acl_name, acl_type,
                                               acl_inout);
    }

    if (RULE_TYPE_OK != result)
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr_N("Failed to %s %s %s access-group.\r\n",
            (TRUE == bind_to_port) ? "enable" : "disable",
            dir_str,
            CLI_ACL_PRIV_ACL_TYPE_STR(acl_type));
#endif /* SYS_CPNT_EH */
    }

#endif /* SYS_CPNT_QOS_V2_TRUNK_PORT */

    return CLI_NO_ERROR;
}

/** Show access-group
 */
static UI32_T
CLI_ACL_PrivShowAccessGroup(
    const CLI_API_ACL_QUERY_T *acl_query)
{
    UI32_T plist_index;

    UI32_T line_num = 0;

    UI32_T acl_index;
    UI32_T precedence;
    RULE_TYPE_InOutDirection_T  inout_profile;

    RULE_TYPE_COUNTER_ENABLE_T  counter_enable;

    RULE_TYPE_UI_AclEntry_T acl_entry;

    char  buff[CLI_DEF_MAX_BUFSIZE] = {0};
    BOOL_T is_show = FALSE;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    char  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
#else
    char *time_range_name = NULL;
#endif

    for (plist_index = 0; plist_index < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ plist_index)
    {
        UI32_T ifindex;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(plist_index);

        {
            is_show = FALSE;

            if (RULE_TYPE_IS_UPORT(ifindex))
            {
                UI32_T unit, port, trunk_id;
                SWCTRL_Lport_Type_T port_type;

                port_type = SWCTRL_PMGR_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);

                if (SWCTRL_LPORT_NORMAL_PORT != port_type)
                {
                    continue;
                }
            }

            if (acl_query &&
                CLI_API_ACL_QUERY_IFINDEX_UN_SPEC != acl_query->ifindex)
            {
                if (ifindex != acl_query->ifindex)
                {
                    continue;
                }
            }

            for (inout_profile = RULE_TYPE_INBOUND; inout_profile <= RULE_TYPE_OUTBOUND; inout_profile++)
            {

                memset (&acl_entry, 0, sizeof(acl_entry));
                acl_index = 0;
                precedence = 0;

                while ( L4_PMGR_ACL_GetNextAclByPort (ifindex , inout_profile,
                                                      &acl_index, &acl_entry,
                                                      &precedence,
                                                      time_range_name,
                                                      &counter_enable) == RULE_TYPE_OK )
                {
                    /*
                     * ....
                     */
                    if (NULL != acl_query &&
                        CLI_API_ACL_QUERY_ACL_TYPE_UN_SPEC != acl_query->acl_type)
                    {
                        BOOL_T by_pass = TRUE;

                        if (acl_query->acl_type == acl_entry.acl_type)
                        {
                            by_pass = FALSE;
                        }
                        else
                        {
                            switch (acl_query->acl_type)
                            {
                                case RULE_TYPE_MAC_ACL:
                                    if (acl_entry.acl_type == RULE_TYPE_MAC_ACL)
                                    {
                                        by_pass = FALSE;
                                    }

                                    break;

                                case RULE_TYPE_IP_ACL:
                                    if (acl_entry.acl_type == RULE_TYPE_IP_STD_ACL ||
                                        acl_entry.acl_type == RULE_TYPE_IP_EXT_ACL)
                                    {
                                        by_pass = FALSE;
                                    }
                                    break;

                                case RULE_TYPE_IPV6_ACL:
                                    if (acl_entry.acl_type == RULE_TYPE_IPV6_STD_ACL ||
                                        acl_entry.acl_type == RULE_TYPE_IPV6_EXT_ACL)
                                    {
                                        by_pass = FALSE;
                                    }
                                    break;

                                default:
                                    break;
                            }
                        }

                        if (TRUE == by_pass)
                        {
                            continue;
                        }
                    }

                    if ( is_show == FALSE )
                    {
                        if (RULE_TYPE_IS_UPORT(ifindex))
                        {
                            UI32_T unit, port, trunk_id;

                            SWCTRL_PMGR_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);
                            sprintf(buff, "Interface ethernet %lu/%lu\r\n", unit, port);
                        }
                        else if (RULE_TYPE_IS_TRUNK(ifindex))
                        {
                            UI32_T unit, port, trunk_id;

                            SWCTRL_PMGR_LogicalPortToUserPort(ifindex, &unit, &port, &trunk_id);

                            sprintf(buff, "Interface port-channel %lu\r\n", trunk_id);
                        }
                        else if (RULE_TYPE_IS_CPU_PORT(ifindex))
                        {
                            sprintf(buff, "Interface control-plan\r\n");
                        }
                        else if (RULE_TYPE_IS_ALL_PORTS(ifindex))
                        {
                            sprintf(buff, "Global\r\n");
                        }
                        else
                        {
                            /* ???
                             */
                            continue;
                        }

                        PROCESS_MORE (buff);
                        is_show = TRUE;
                    }

                    if ( ( acl_entry.acl_type == RULE_TYPE_IP_EXT_ACL ) ||
                         ( acl_entry.acl_type == RULE_TYPE_IP_STD_ACL ) )
                    {
                        sprintf(buff, " IP access-list %s %s",
                                acl_entry.acl_name,
                                RULE_TYPE_INBOUND==inout_profile?"in":"out");
                    }

                    if ( acl_entry.acl_type == RULE_TYPE_MAC_ACL )
                    {
                        sprintf(buff, " MAC access-list %s %s",
                                acl_entry.acl_name,
                                RULE_TYPE_INBOUND==inout_profile?"in":"out");
                    }
#if (SYS_CPNT_ACL_IPV6 == TRUE)
                    if ( ( acl_entry.acl_type == RULE_TYPE_IPV6_EXT_ACL ) ||
                         ( acl_entry.acl_type == RULE_TYPE_IPV6_STD_ACL ) )
                    {
                        sprintf(buff, " IPv6 access-list %s %s",
                                acl_entry.acl_name,
                                RULE_TYPE_INBOUND==inout_profile?"in":"out");
                    }
#endif /* SYS_CPNT_ACL_IPV6 */

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                    if (strlen(time_range_name) != 0) {
                        strcat(buff, " time-range");
                        strcat(buff, " ");
                        strcat(buff, time_range_name);
                    }
#endif /* #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
                    if (RULE_TYPE_COUNTER_ENABLE == counter_enable)
                    {
                        strcat(buff, " counter");
                    }
#endif /* SYS_CPNT_ACL_COUNTER */

                    strcat(buff, "\r\n");
                    PROCESS_MORE(buff);

                }

            } /* direction */
        } /* ifindex */
    } /* unit */

    return CLI_NO_ERROR;
}

BOOL_T CLI_ACL_Check_MAC_MASK(unsigned char *addr)
{
   UI32_T i = 0;

   for (i = 0; i < 6; i++)
   {
      if (addr[i] != 0x00)
         return FALSE;
   }
   return TRUE;
}

BOOL_T CLI_ACL_Check_HOST_MAC_MASK(unsigned char *addr)
{
   UI32_T i = 0;

   for (i = 0; i < 6; i++)
   {
      if (addr[i] != 0xff)
         return FALSE;
   }
   return TRUE;
}

void CLI_ACL_Check_AnyMAC(UI8_T *mac, UI8_T *mask, char *buff)
{
   char  addr_str1[20] = {0};
   char  addr_str2[20] = {0};
   UI32_T i;
   UI32_T value = 0;
   BOOL_T is_mask_is_zero = FALSE;
   BOOL_T is_mask_is_all = FALSE;

    ASSERT(mac != NULL);
    ASSERT(mask != NULL);
    ASSERT(buff != NULL);

    buff[0] = '\0';

   /*check is zero*/
   for (i = 0; i < 6 ; i++)
   {
      if (mask[i] == 0)
      {
         is_mask_is_zero = TRUE;
      }
      else
      {
         is_mask_is_zero = FALSE;
         break;
      }
   }

   /*check is all*/
   for (i = 0; i < 6 ; i++)
   {
      if (mask[i] == 0xff)
      {
         is_mask_is_all = TRUE;
      }
      else
      {
         is_mask_is_all = FALSE;
         break;
      }
   }

   if (is_mask_is_zero)
   {
      value = ALL_ANY;
   }

   if (is_mask_is_all)
   {
      value = HOST_ONLY;
   }

   if (!is_mask_is_all && !is_mask_is_zero)
   {
      value = HOST_MASK;
   }

   switch(value)
   {
   case ALL_ANY:
      strcat(buff, " any");
      break;

//   case ANY_MASK:
//      strcat(buff, " 00-00-00-00-00-00");
//      sprintf(addr_str1," %02x-%02x-%02x-%02x-%02x-%02x",mask[0], mask[1], mask[2]
//                                                        ,mask[3], mask[4], mask[5]);
//      strcat(buff,  addr_str1);
//      break;

   case HOST_ONLY:
      strcat(buff, " host");
      sprintf(addr_str1," %02x-%02x-%02x-%02x-%02x-%02x",mac[0], mac[1], mac[2]
                                                        ,mac[3], mac[4], mac[5]);
      strcat(buff,  addr_str1);
      break;

   case HOST_MASK:
      sprintf(addr_str1," %02x-%02x-%02x-%02x-%02x-%02x",mac[0], mac[1], mac[2]
                                                        ,mac[3], mac[4], mac[5]);
      sprintf(addr_str2," %02x-%02x-%02x-%02x-%02x-%02x",mask[0], mask[1], mask[2]
                                                        ,mask[3], mask[4], mask[5]);
      strcat(buff,  addr_str1);
      strcat(buff,  addr_str2);
      break;

   default:
      break;
   }
   return;
}

void
CLI_ACL_ConvertIpv4Addr2UIString(
    UI8_T *ip,
    UI8_T *mask,
    char *buff)
{
#define IS_HOST_MASK(a) (a[0] == 255 && a[1] == 255 && a[2] == 255 && a[3] == 255)
#define IS_ANY_MASK(a) (a[0] == 0 && a[1] == 0 && a[2] == 0 && a[3] == 0)

    char  ip_str[16] = {0};
    char  mask_str[16] = {0};
    int type;

    ASSERT(buff != NULL);
    ASSERT(ip != NULL);
    ASSERT(mask != NULL);

    buff[0] = '\0';

    if (IS_HOST_MASK(mask))
    {
        type = HOST_ONLY;
    }
    else if (IS_ANY_MASK(mask))
    {
        type = ALL_ANY;
    }
    else
    {
        type = HOST_MASK;
    }

    switch(type)
    {
        case ALL_ANY:
            strcat(buff, " any");
            break;

        case HOST_ONLY:
            if (TRUE == L_INET_Ntop(L_INET_AF_INET, ip, ip_str, sizeof(ip_str)))
            {
                strcat(buff, " host ");
                strcat(buff, ip_str);
            }
            break;

        case HOST_MASK:
            if (TRUE == L_INET_Ntop(L_INET_AF_INET, ip, ip_str, sizeof(ip_str)) &&
                TRUE == L_INET_Ntop(L_INET_AF_INET, mask, mask_str, sizeof(ip_str)))
            {
                strcat(buff, " ");
                strcat(buff, ip_str);
                strcat(buff, " ");
                strcat(buff, mask_str);
            }
            break;

        default:
            break;
    }
#undef IS_HOST_MASK
#undef IS_ANY_MASK
    return;
}
#endif


UI32_T CLI_API_AccessList_IP_MaskPrecedence(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if 0
   UI32_T mask_mode = 0;
#if (SYS_CPNT_ACL == TRUE)
   switch(arg[0][0])
   {
   case 'i':
   case 'I':
      mask_mode = ACL_INGRESS_MASK;
      break;

   case 'o':
   case 'O':
      mask_mode = ACL_EGRESS_MASK;
      break;

   default:
      return CLI_ERR_INTERNAL;
   }
#endif
   switch(cmd_idx)
   {
   case PRIVILEGE_CFG_GLOBAL_CMD_W3_ACCESSLIST_IP_MASKPRECEDENCE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_ACL_IP_MASK_MODE;
      ctrl_P->CMenu.mask_mode = mask_mode;
      ctrl_P->CMenu.mask_type = ACL_IP_MASK;
      break;

   case PRIVILEGE_CFG_GLOBAL_CMD_W4_NO_ACCESSLIST_IP_MASKPRECEDENCE:
      if (mask_mode == ACL_EGRESS_MASK)
      {
         if (L4_ACL_MGR_DelMaskTbl(ACL_IP_MASK ,mask_mode) == FALSE)
            CLI_LIB_PrintStr("Failed to delete egress IP mask ACL\r\n");
      }
      else
      {
         if (L4_ACL_MGR_DelMaskTbl(ACL_IP_MASK ,mask_mode) == FALSE)
            CLI_LIB_PrintStr("Failed to delete ingress IP mask ACL\r\n");
      }
      break;

   default:
      return CLI_ERR_INTERNAL;
   }
#endif

   return CLI_NO_ERROR;
}

UI32_T CLI_API_AccessList_MAC_MaskPrecedence(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if 0
   UI32_T mask_mode = 0;
#if (SYS_CPNT_ACL == TRUE)
   switch(arg[0][0])
   {
   case 'i':
   case 'I':
      mask_mode = ACL_INGRESS_MASK;
      break;

   case 'o':
   case 'O':
      mask_mode = ACL_EGRESS_MASK;
      break;

   default:
      return CLI_ERR_INTERNAL;
   }
#endif
   switch(cmd_idx)
   {
   case PRIVILEGE_CFG_GLOBAL_CMD_W3_ACCESSLIST_MAC_MASKPRECEDENCE:
      ctrl_P->CMenu.AccMode = PRIVILEGE_CFG_ACL_MAC_MASK_MODE;
      ctrl_P->CMenu.mask_mode = mask_mode;
      ctrl_P->CMenu.mask_type = ACL_MAC_MASK;
      break;

   case PRIVILEGE_CFG_GLOBAL_CMD_W4_NO_ACCESSLIST_MAC_MASKPRECEDENCE:
      if (mask_mode == ACL_EGRESS_MASK)
      {
         if (L4_ACL_MGR_DelMaskTbl(ACL_MAC_MASK, mask_mode) == FALSE)
            CLI_LIB_PrintStr("Failed to delete egress MAC mask ACL\r\n");
      }
      else
      {
         if (L4_ACL_MGR_DelMaskTbl(ACL_MAC_MASK, mask_mode) == FALSE)
            CLI_LIB_PrintStr("Failed to delete ingress MAC mask ACL\r\n");
      }
      break;

   default:
      return CLI_ERR_INTERNAL;
   }
#endif

   return CLI_NO_ERROR;
}

UI32_T CLI_API_Show_AccessList_IP_MaskPrecedence(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACL == TRUE)

#if 0
   ACL_MaskEntry_T ip_mask;
   char buff[CLI_DEF_MAX_BUFSIZE] = {0};
   BOOL_T is_show = FALSE;
   UI32_T line_num = 0;
   memset(&ip_mask, 0, sizeof(ip_mask));
   ip_mask.u.ip_mask.Precedence = 0xffffffff;


   switch(arg[0][0])
   {
   case 'i':
   case 'I':
      while(L4_ACL_MGR_GetNextMaskEntry(ACL_IP_MASK, ACL_INGRESS_MASK, &ip_mask) == TRUE)
      {
         if (is_show == FALSE)
         {
            is_show = TRUE;
            PROCESS_MORE("IP ingress mask ACL:\r\n");
         }
         line_num = CLI_ACL_ShowOne_IP_MASK_List(ip_mask,line_num);
         if (line_num == JUMP_OUT_MORE)
         {
            return CLI_NO_ERROR;
         }
         else if (line_num == EXIT_SESSION_MORE)
         {
            return CLI_EXIT_SESSION;
         }
      }
      break;

   case 'o':
   case 'O':
      while(L4_ACL_MGR_GetNextMaskEntry(ACL_IP_MASK, ACL_EGRESS_MASK, &ip_mask) == TRUE)
      {
         if (is_show == FALSE)
         {
            is_show = TRUE;
            PROCESS_MORE("IP egress mask ACL:\r\n");
         }
         line_num = CLI_ACL_ShowOne_IP_MASK_List(ip_mask,line_num);
         if (line_num == JUMP_OUT_MORE)
         {
            return CLI_NO_ERROR;
         }
         else if (line_num == EXIT_SESSION_MORE)
         {
            return CLI_EXIT_SESSION;
         }
      }
      break;

   default:
      {
         char i = 0;
         for (i = ACL_INGRESS_MASK;i<ACL_MAX_NBR_OF_MASK_INOUT;i++)
         {
            is_show = FALSE;
            memset(&ip_mask, 0, sizeof(ip_mask));
            ip_mask.u.ip_mask.Precedence = 0xffffffff;
            while(L4_ACL_MGR_GetNextMaskEntry(ACL_IP_MASK, i, &ip_mask) == TRUE)
            {
               if (is_show == FALSE)
               {
                  is_show = TRUE;
                  sprintf(buff,"IP %s mask ACL:\r\n",(i == ACL_INGRESS_MASK)?("ingress"):("egress"));
                  PROCESS_MORE(buff);
               }
               line_num = CLI_ACL_ShowOne_IP_MASK_List(ip_mask,line_num);
               if (line_num == JUMP_OUT_MORE)
               {
                  return CLI_NO_ERROR;
               }
               else if (line_num == EXIT_SESSION_MORE)
               {
                  return CLI_EXIT_SESSION;
               }
            }
         }
      }
      break;
   }
#endif
#endif
   return CLI_NO_ERROR;
}

UI32_T CLI_API_Show_AccessList_MAC_MaskPrecedence(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACL == TRUE)
#if 0
   ACL_MaskEntry_T mac_mask;
   char buff[CLI_DEF_MAX_BUFSIZE] = {0};
   BOOL_T is_show = FALSE;
   UI32_T line_num = 0;
   memset(&mac_mask, 0, sizeof(mac_mask));
   mac_mask.u.mac_mask.Precedence = 0xffffffff;


   switch(arg[0][0])
   {
   case 'i':
   case 'I':
      while(L4_ACL_MGR_GetNextMaskEntry(ACL_MAC_MASK, ACL_INGRESS_MASK, &mac_mask) == TRUE)
      {
         if (is_show == FALSE)
         {
            is_show = TRUE;
            PROCESS_MORE("MAC ingress mask ACL:\r\n");
         }
         line_num = CLI_ACL_ShowOne_MAC_MASK_List(mac_mask,line_num);
         if (line_num == JUMP_OUT_MORE)
         {
            return CLI_NO_ERROR;
         }
         else if (line_num == EXIT_SESSION_MORE)
         {
            return CLI_EXIT_SESSION;
         }
      }
      break;

   case 'o':
   case 'O':
      while(L4_ACL_MGR_GetNextMaskEntry(ACL_MAC_MASK, ACL_EGRESS_MASK, &mac_mask) == TRUE)
      {
         if (is_show == FALSE)
         {
            is_show = TRUE;
            PROCESS_MORE("MAC egress mask ACL:\r\n");
         }
         line_num = CLI_ACL_ShowOne_MAC_MASK_List(mac_mask,line_num);
         if (line_num == JUMP_OUT_MORE)
         {
            return CLI_NO_ERROR;
         }
         else if (line_num == EXIT_SESSION_MORE)
         {
            return CLI_EXIT_SESSION;
         }
      }
      break;

   default:
      {
         char i = 0;
         for (i = ACL_INGRESS_MASK;i<ACL_MAX_NBR_OF_MASK_INOUT;i++)
         {
            is_show = FALSE;
            memset(&mac_mask, 0, sizeof(mac_mask));
            mac_mask.u.mac_mask.Precedence = 0xffffffff;
            while(L4_ACL_MGR_GetNextMaskEntry(ACL_MAC_MASK, i, &mac_mask) == TRUE)
            {
               if (is_show == FALSE)
               {
                  is_show = TRUE;
                  sprintf(buff,"MAC %s mask ACL:\r\n",(i == ACL_INGRESS_MASK)?("ingress"):("egress"));
                  PROCESS_MORE(buff);
               }
               line_num = CLI_ACL_ShowOne_MAC_MASK_List(mac_mask,line_num);
               if (line_num == JUMP_OUT_MORE)
               {
                  return CLI_NO_ERROR;
               }
               else if (line_num == EXIT_SESSION_MORE)
               {
                  return CLI_EXIT_SESSION;
               }
            }
         }
      }
      break;
   }
#endif
#endif
   return CLI_NO_ERROR;
}

UI32_T CLI_API_ACL_IP_Mask(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACL == TRUE)
#if 0
   ACL_MaskEntry_T ip_mask;
   UI32_T cli_ret;
   BOOL_T acl_ret;
   //USED_TO_PRT_ARGS;
   memset(&ip_mask,0,sizeof(ip_mask));
   L4_ACL_MGR_InitMask(ACL_IP_MASK, &ip_mask);

   if ((cli_ret = CLI_ACL_FillIP_MASK(&ip_mask, arg)) == CLI_NO_ERROR)
   {
      switch(cmd_idx)
      {
      case PRIVILEGE_CFG_ACL_IP_MASK_CMD_W1_MASK:
         switch(ctrl_P->CMenu.mask_mode)
         {
         case ACL_INGRESS_MASK:
            if (L4_ACL_MGR_AddMaskEntry(ACL_IP_MASK, ACL_INGRESS_MASK, &ip_mask) == TRUE)
            {
               acl_ret = TRUE;
            }
            break;

         case ACL_EGRESS_MASK:
            if (L4_ACL_MGR_AddMaskEntry(ACL_IP_MASK, ACL_EGRESS_MASK, &ip_mask) == TRUE)
            {
               acl_ret = TRUE;
            }
            break;

         default:
            return CLI_ERR_INTERNAL;
         }
         if (acl_ret == FALSE)
         {
            CLI_LIB_PrintStr("Failed to add IP mask ACL\r\n");
         }
         break;

      case PRIVILEGE_CFG_ACL_IP_MASK_CMD_W2_NO_MASK:
         switch(ctrl_P->CMenu.mask_mode)
         {
         case ACL_INGRESS_MASK:
            if (L4_ACL_MGR_DelMaskEntry(ACL_IP_MASK, ACL_INGRESS_MASK, &ip_mask) == TRUE)
            {
               acl_ret = TRUE;
            }
            break;

         case ACL_EGRESS_MASK:
            if (L4_ACL_MGR_DelMaskEntry(ACL_IP_MASK, ACL_EGRESS_MASK, &ip_mask) == TRUE)
            {
               acl_ret = TRUE;
            }
            break;

         default:
            return CLI_ERR_INTERNAL;
         }
         if (acl_ret == FALSE)
         {
            CLI_LIB_PrintStr("Failed to remove IP mask ACL\r\n");
         }
         break;

      default:
         break;
      }
   }
#if (CLI_ACL_TEST == 1)
      CLI_ACL_ShowOne_IP_MASK_List(ip_mask, 1);
#endif
#endif
#endif
   return CLI_NO_ERROR;
}

UI32_T CLI_API_ACL_MAC_Mask(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_ACL == TRUE)
#if 0
   ACL_MaskEntry_T mac_mask;
   UI32_T cli_ret;
   BOOL_T acl_ret;
   //USED_TO_PRT_ARGS;
   memset(&mac_mask,0,sizeof(mac_mask));

   L4_ACL_MGR_InitMask(ACL_MAC_MASK, &mac_mask);

   if ((cli_ret = CLI_ACL_FillMAC_MASK(&mac_mask, arg)) == CLI_NO_ERROR)
   {
      switch(cmd_idx)
      {
      case PRIVILEGE_CFG_ACL_MAC_MASK_CMD_W1_MASK:
         switch(ctrl_P->CMenu.mask_mode)
         {
         case ACL_INGRESS_MASK:
            if (L4_ACL_MGR_AddMaskEntry(ACL_MAC_MASK, ACL_INGRESS_MASK, &mac_mask) == TRUE)
            {
               acl_ret = TRUE;
            }
            break;

         case ACL_EGRESS_MASK:
            if (L4_ACL_MGR_AddMaskEntry(ACL_MAC_MASK, ACL_EGRESS_MASK, &mac_mask) == TRUE)
            {
               acl_ret = TRUE;
            }
            break;

         default:
            return CLI_ERR_INTERNAL;
         }
         if (acl_ret == FALSE)
         {
            CLI_LIB_PrintStr("Failed to add mac mask ACL\r\n");
         }
         break;

      case PRIVILEGE_CFG_ACL_MAC_MASK_CMD_W2_NO_MASK:
         switch(ctrl_P->CMenu.mask_mode)
         {
         case ACL_INGRESS_MASK:
            if (L4_ACL_MGR_DelMaskEntry(ACL_MAC_MASK, ACL_INGRESS_MASK, &mac_mask) == TRUE)
            {
               acl_ret = TRUE;
            }
            break;

         case ACL_EGRESS_MASK:
            if (L4_ACL_MGR_DelMaskEntry(ACL_MAC_MASK, ACL_EGRESS_MASK, &mac_mask) == TRUE)
            {
               acl_ret = TRUE;
            }
            break;

         default:
            return CLI_ERR_INTERNAL;
         }
         if (acl_ret == FALSE)
         {
            CLI_LIB_PrintStr("Failed to remove MAC mask ACL\r\n");
         }
         break;

      default:
         break;
      }
   }
#if (CLI_ACL_TEST == 1)
      CLI_ACL_ShowOne_MAC_MASK_List(mac_mask, 1);
#endif
#endif
#endif
   return CLI_NO_ERROR;
}

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
static UI32_T CLI_API_ACL_Fill_Marker(MARKER_TYPE_MarkerEntry_T *marker_entry, char *arg[])
{
   /*acl_name set priority dscp ...*/
   char position = 2;
   strcpy(marker_entry->acl_name,  arg[0]);
   /*no form will not go to this while loop, because our arg only have acl name*/
   while (arg[position] != NULL)
   {
      switch(arg[position][0])
      {
      case 'd':
      case 'D':
         marker_entry->action |= MARKER_TYPE_REMARK_DSCP;
         marker_entry->dscp = atoi(arg[position+1]);
         position = position + 2;
         break;

      case 'p':
      case 'P':
         if (arg[position][2] == 'i' || arg[position][2] == 'I')
         {
            marker_entry->action |= MARKER_TYPE_REMARK_DOT1P_PRIORITY;
            marker_entry->dot1p = atoi(arg[position+1]);
         }
         else if (arg[position][2] == 'e' || arg[position][2] == 'E')
         {
            marker_entry->action |= MARKER_TYPE_REMARK_TOS_PRECEDENCE;
            marker_entry->precedence = atoi(arg[position+1]);
         }
         position = position + 2;
         break;

      default:
         return CLI_ERR_INTERNAL;
      }
   }
   return CLI_NO_ERROR;
}
#endif

UI32_T CLI_API_Match_AccessList_IP_MAC_Eth(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T cli_ret = 0;
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
    UI32_T i     = 0;
    UI32_T lport = 0;
    UI32_T verify_unit = ctrl_P->CMenu.unit_id;
    UI32_T verify_port;
    CLI_API_EthStatus_T verify_ret;
    MARKER_TYPE_MarkerEntry_T marker_entry;

    if (L4_MARKER_MGR_GetDefaultMarkerEntry(&marker_entry) == FALSE)
    {
#if (SYS_CPNT_EH == TRUE)
        CLI_API_Show_Exception_Handeler_Msg();
#else
        CLI_LIB_PrintStr("Failed to get default marker entry\r\n");
#endif
    }

    if ((cli_ret = CLI_API_ACL_Fill_Marker(&marker_entry, arg)) == CLI_NO_ERROR)
    {
        for (i = 1; i <= ctrl_P->sys_info.max_port_number; i++)
        {
            if (ctrl_P->CMenu.port_id_list[(UI32_T)((i-1)/8)]  & (1 << ( 7 - ((i-1)%8))) )
            {
                verify_port = i;
                verify_ret = verify_ethernet(verify_unit, verify_port, &lport);
                if( verify_ret != CLI_API_ETH_OK && verify_ret != CLI_API_ETH_TRUNK_MEMBER)
                {
                    display_ethernet_msg(verify_ret, verify_unit, verify_port);
                    continue;
                }
                else
                {
                    switch(cmd_idx)
                    {
                    case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_MATCH_ACCESSLIST_IP:
                        marker_entry.acl_type = ACL_IP_ACL;
                        if (L4_MARKER_MGR_AddMarkerEntry(lport, &marker_entry) == FALSE)
                        {
#if (SYS_CPNT_EH == TRUE)
                            CLI_API_Show_Exception_Handeler_Msg();
#else
                            CLI_LIB_PrintStr("Failed to add IP marker entry\r\n");
#endif
                        }
                        break;

                    case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W4_NO_MATCH_ACCESSLIST_IP:
                        if (L4_MARKER_MGR_DelMarkerEntry(lport, arg[0], ACL_IP_ACL) == FALSE)
                        {
#if (SYS_CPNT_EH == TRUE)
                            CLI_API_Show_Exception_Handeler_Msg();
#else
                            CLI_LIB_PrintStr("Failed to delete IP marker entry\r\n");
#endif
                        }
                        break;

                    case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_MATCH_ACCESSLIST_MAC:
                        marker_entry.acl_type = ACL_MAC_ACL;
                        if (L4_MARKER_MGR_AddMarkerEntry(lport, &marker_entry) == FALSE)
                        {
#if (SYS_CPNT_EH == TRUE)
                            CLI_API_Show_Exception_Handeler_Msg();
#else
                            CLI_LIB_PrintStr("Failed to add MAC marker entry\r\n");
#endif
                        }
                        break;

                    case PRIVILEGE_CFG_INTERFACE_ETH_CMD_W4_NO_MATCH_ACCESSLIST_MAC:
                        if (L4_MARKER_MGR_DelMarkerEntry(lport, arg[0], ACL_MAC_ACL) == FALSE)
                        {
#if (SYS_CPNT_EH == TRUE)
                            CLI_API_Show_Exception_Handeler_Msg();
#else
                            CLI_LIB_PrintStr("Failed to delete MAC marker entry\r\n");
#endif
                        }
                        break;

                    default:
                        return CLI_ERR_INTERNAL;
                   }
                }
            }
        }
    }
#endif
    return cli_ret;
}

UI32_T CLI_API_ACL_Show_Marking(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
   UI32_T verify_unit = 0;
   UI32_T verify_port = 0;
   UI32_T ifindex = 0;
   UI32_T verify_ret = 0;
   UI32_T line_num = 0;
   char  buff[CLI_DEF_MAX_BUFSIZE] = {0};
   UI32_T j = 0;
   UI32_T max_port_num = 0;
   UI32_T current_max_unit;
   char  acl_type[4] = {0};
   char  cos_type1[12] = {0};
   char  cos_type2[12] = {0};
   BOOL_T is_show = FALSE;
   MARKER_TYPE_MarkerEntry_T marker_entry;
   memset(&marker_entry, 0, sizeof(marker_entry));
#if (SYS_CPNT_STACKING == TRUE)
   //STKTPLG_MGR_GetNumberOfUnit(&current_max_unit);
   current_max_unit = SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;
#else
   current_max_unit = 1;
#endif

 /*  for (j = 1; j <= current_max_unit; j++) */
      for (j=0; STKTPLG_POM_GetNextUnit(&j); )
   {
      verify_unit = j;
      max_port_num = SWCTRL_POM_UIGetUnitPortNumber(verify_unit);
      for (verify_port = 1; verify_port <= max_port_num; verify_port++)
      {
         is_show = FALSE;
         memset(&marker_entry, 0, sizeof(marker_entry));
         verify_ret = CLI_ACL_VerifyEthernet(verify_unit, verify_port, &ifindex);
         if( verify_ret == CLI_API_ETH_NOT_PRESENT)
         {
            continue;
         }

         while(L4_MARKER_MGR_GetNextMarkerEntry(ifindex, &marker_entry) == TRUE)
         {
            if (is_show == FALSE)
            {
               /*show interface*/
               is_show = TRUE;
               sprintf(buff, "Interface ethernet %lu/%lu\r\n",verify_unit ,verify_port);
               PROCESS_MORE(buff);
            }

            switch(marker_entry.acl_type)
            {
            case ACL_IP_ACL:
               strcpy(acl_type, "IP");
               break;

            case ACL_MAC_ACL:
               strcpy(acl_type, "MAC");
               break;

            default:
               break;
            }

            if ((marker_entry.action & MARKER_TYPE_REMARK_DOT1P_PRIORITY) == MARKER_TYPE_REMARK_DOT1P_PRIORITY)
            {
               sprintf(cos_type1, "priority %lu", marker_entry.dot1p);
            }

            if ((marker_entry.action & MARKER_TYPE_REMARK_DSCP) == MARKER_TYPE_REMARK_DSCP)
            {
               sprintf(cos_type2, "DSCP %lu", marker_entry.dscp);
            }
            if ((marker_entry.action & MARKER_TYPE_REMARK_TOS_PRECEDENCE) == MARKER_TYPE_REMARK_TOS_PRECEDENCE)
            {
               sprintf(cos_type2, "precedence %lu", marker_entry.precedence);
            }

            if (cos_type1[0] == 0)
            {
               sprintf(buff, " match access-list %s %s set %s\r\n", acl_type, marker_entry.acl_name, cos_type2);
            }
            else
            {
               if (cos_type2[0] == 0)
               {
                  sprintf(buff, " match access-list %s %s set %s\r\n", acl_type, marker_entry.acl_name, cos_type1);
               }
               else
               {
                  sprintf(buff, " match access-list %s %s set %s %s\r\n", acl_type, marker_entry.acl_name, cos_type1, cos_type2);
               }
            }
            PROCESS_MORE(buff);
            memset(&acl_type, 0, sizeof(acl_type));
            memset(&cos_type1, 0, sizeof(cos_type1));
            memset(&cos_type2, 0, sizeof(cos_type2));
         }/*end of while*/
      }/*end of port loop*/
   }/*end of unit loop*/
#endif
   return CLI_NO_ERROR;
}

static void
CLI_API_ACL_PRIV_ShowTcamPoolCapabilityCode()
{
    enum
    {
        MAX_PRINT_STR_LEN_PER_LINE = 70
    };

    UI32_T cur_lin_str_len = 0;
    UI32_T cap_cnt;
    RULE_TYPE_TCAM_CAP_BITMAP_T all_bitmap;

    RULE_TYPE_TCAM_CAP_STRING_T cap_str_table[] = RULE_TYPE_TCAM_CAPABILITY_STRING_ARRAY;

    if (RULE_TYPE_OK != L4_PMGR_GetSupportedTcamCapabilityBitMap(&all_bitmap))
    {
        return;
    }

    CLI_LIB_PrintStr("Pool capability code:\r\n ");

    for (cap_cnt = 0; cap_cnt < RULE_TYPE_TCAM_CAP_TOTAL; ++cap_cnt)
    {
        UI32_T print_str_len;
        RULE_TYPE_TCAM_CAP_STRING_T *cap_string_p = &cap_str_table[cap_cnt];

        if (RULE_TYPE_IS_BIT_OFF(all_bitmap.bits, cap_cnt))
        {
            continue;
        }

        print_str_len = strlen(cap_string_p->name) + strlen(cap_string_p->description) + 5;
        if (MAX_PRINT_STR_LEN_PER_LINE < cur_lin_str_len + print_str_len)
        {
            CLI_LIB_PrintStr("\r\n ");
            cur_lin_str_len = 0;
        }

        CLI_LIB_PrintStr_N(" %s - %s,", cap_string_p->name,
                                        cap_string_p->description);

        cur_lin_str_len += print_str_len;
    }

    CLI_LIB_PrintStr("\r\n\r\n");

    return;
}

UI32_T CLI_API_Show_AccessList_TcamUtilization(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_CLI_FILTERING == TRUE)
    if( arg[0]!=NULL)
    {
        if( arg[0][0]=='|' )
        {
            switch(arg[1][0])
            {
                case 'b':
                case 'B':
                    ctrl_P->option_flag=CLI_API_OPTION_BEGIN;
                    break;
                case 'e':
                case 'E':
                    ctrl_P->option_flag=CLI_API_OPTION_EXCLUDE;
                    break;

                case 'i':
                case 'I':
                    ctrl_P->option_flag=CLI_API_OPTION_INCLUDE;
                    break;
                default:
                    return CLI_ERR_INTERNAL;
            }
            strcpy(ctrl_P->option_buf,arg[2]);
        }
    }
#endif /*#if (SYS_CPNT_CLI_FILTERING == TRUE)*/

    {
        enum
        {
            TBL_TCAM_UNIT,
            TBL_TCAM_DEVICE,
            TBL_TCAM_POOL,
            TBL_TCAM_TOTAL,
            TBL_TCAM_USED,
            TBL_TCAM_FREE,
            TBL_TCAM_CAP
        };

        enum
        {
            TBL_TCAM_WIDTH_UNIT   = 4,
            TBL_TCAM_WIDTH_DEVICE = 6,
            TBL_TCAM_WIDTH_POOL   = 4,
            TBL_TCAM_WIDTH_TOTAL  = 5,
            TBL_TCAM_WIDTH_USED   = 5,
            TBL_TCAM_WIDTH_FREE   = 5,
            TBL_TCAM_WIDTH_CAP    = 40
        };

        RULE_TYPE_TCAM_CAP_STRING_T cap_str_table[] = RULE_TYPE_TCAM_CAPABILITY_STRING_ARRAY;
        RULE_TYPE_TCAM_ENTRY_T tcam_entry;

        char   buff[CLI_DEF_MAX_BUFSIZE] = {0};
        UI32_T line_num = 0;
        CLI_TBL_Object_T tcam_tbl;
        CLI_TBL_Temp_T tcam_group_tbl[] =
        {
            {TBL_TCAM_UNIT,    TBL_TCAM_WIDTH_UNIT,   CLI_TBL_ALIGN_RIGHT},
            {TBL_TCAM_DEVICE,  TBL_TCAM_WIDTH_DEVICE, CLI_TBL_ALIGN_RIGHT},
            {TBL_TCAM_POOL,    TBL_TCAM_WIDTH_POOL,   CLI_TBL_ALIGN_RIGHT},
            {TBL_TCAM_TOTAL,   TBL_TCAM_WIDTH_TOTAL,  CLI_TBL_ALIGN_RIGHT},
            {TBL_TCAM_USED,    TBL_TCAM_WIDTH_USED,   CLI_TBL_ALIGN_RIGHT},
            {TBL_TCAM_FREE,    TBL_TCAM_WIDTH_FREE,   CLI_TBL_ALIGN_RIGHT},
            {TBL_TCAM_CAP,     TBL_TCAM_WIDTH_CAP,    CLI_TBL_ALIGN_LEFT}
        };

        CLI_API_ACL_PRIV_ShowTcamPoolCapabilityCode();

        CLI_TBL_InitWithBuf(&tcam_tbl, buff, sizeof(buff));
        CLI_TBL_SetColIndirect(&tcam_tbl,
                               tcam_group_tbl,
                               sizeof(tcam_group_tbl)/sizeof(tcam_group_tbl[0]));
        CLI_TBL_SetLineNum(&tcam_tbl, line_num);

        CLI_TBL_SetColTitle(&tcam_tbl, TBL_TCAM_UNIT, "Unit");
        CLI_TBL_SetColTitle(&tcam_tbl, TBL_TCAM_DEVICE, "Device");
        CLI_TBL_SetColTitle(&tcam_tbl, TBL_TCAM_POOL, "Pool");
        CLI_TBL_SetColTitle(&tcam_tbl, TBL_TCAM_TOTAL, "Total");
        CLI_TBL_SetColTitle(&tcam_tbl, TBL_TCAM_USED, "Used");
        CLI_TBL_SetColTitle(&tcam_tbl, TBL_TCAM_FREE, "Free");
        CLI_TBL_SetColTitle(&tcam_tbl, TBL_TCAM_CAP, "Capability");
        CLI_TBL_Print(&tcam_tbl);

        CLI_TBL_SetLine(&tcam_tbl);
        CLI_TBL_Print(&tcam_tbl);

        memset(&tcam_entry, 0, sizeof(tcam_entry));

        while (TRUE == L4_PMGR_GetNextTcamEntry(&tcam_entry))
        {
            UI32_T capability_cnt;
            char   print_cap_str[TBL_TCAM_WIDTH_CAP+1] = {0};
            int    allow_print_cap_str_len = sizeof(print_cap_str) - 1;

            if (RULE_TYPE_IS_BIT_ON(tcam_entry.cap_bitmap.bits, RULE_TYPE_TCAM_CAP_ALL))
            {
                capability_cnt = RULE_TYPE_TCAM_CAP_ALL;
            }
            else if (RULE_TYPE_IS_BIT_ON(tcam_entry.cap_bitmap.bits, RULE_TYPE_TCAM_CAP_RESERVED))
            {
                capability_cnt = RULE_TYPE_TCAM_CAP_RESERVED;
            }
            else
            {
                capability_cnt = 0;
            }

            for (; capability_cnt < RULE_TYPE_TCAM_CAP_TOTAL; ++capability_cnt)
            {
                if (RULE_TYPE_IS_BIT_ON(tcam_entry.cap_bitmap.bits, capability_cnt))
                {
                    RULE_TYPE_TCAM_CAP_STRING_T *cap_str_p = &cap_str_table[capability_cnt];
                    int str_len;

                    str_len = strlen(cap_str_p->name) + 1;

                    if (allow_print_cap_str_len < str_len)
                    {
                        ASSERT(0);
                        break;
                    }

                    SYSFUN_Strncat(print_cap_str, cap_str_p->name, allow_print_cap_str_len);
                    SYSFUN_Strncat(print_cap_str, " ", allow_print_cap_str_len);
                    allow_print_cap_str_len -= str_len;
                }
            }

            CLI_TBL_SetColInt(&tcam_tbl, TBL_TCAM_UNIT, tcam_entry.unit);
            CLI_TBL_SetColInt(&tcam_tbl, TBL_TCAM_DEVICE, tcam_entry.device_id);
            CLI_TBL_SetColInt(&tcam_tbl, TBL_TCAM_POOL, tcam_entry.pool_id);
            CLI_TBL_SetColInt(&tcam_tbl, TBL_TCAM_TOTAL, tcam_entry.total);
            CLI_TBL_SetColInt(&tcam_tbl, TBL_TCAM_USED, tcam_entry.used);
            CLI_TBL_SetColInt(&tcam_tbl, TBL_TCAM_FREE, tcam_entry.free);
            CLI_TBL_SetColText(&tcam_tbl, TBL_TCAM_CAP, print_cap_str);
            CLI_TBL_Print(&tcam_tbl);
        }

        CLI_TBL_Print(&tcam_tbl);
    }

    return CLI_NO_ERROR;
}

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/* fill time range index */
static UI32_T CLI_ACL_TIME_RANGE_FillTimeRangeName(char *arg[], UI8_T *p_time_range_name)
{
    if (arg [0] != NULL)
    {
        if ('T' == arg[0][0] || 't' == arg[0][0]) /* time range */
        {
            /* assign */
            strncpy((char *)p_time_range_name, arg[1], SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
        }
    }
    return CLI_NO_ERROR;
}
#endif /* #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */

