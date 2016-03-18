#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "cli_def.h"
#include "cli_type.h"
#include "cli_task.h"
#include "sysfun.h"
#include "cli_lib.h"
#include "l_stdlib.h"


static void SYS_PMGR_GetPromptString(UI8_T *prompt)
{
    prompt[0] = '\0';
}

UI8_T CLI_TASK_GetMySessId()
{
    return 1;
}


#pragma mark -
#pragma mark #if 0
#if 0
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "sys_type.h"
#include "sysfun.h"
#include "sys_cpnt.h"
#include "l_mm.h"
#include "l_stdlib.h"
#include "l_base64.h"
//#include "sys_mgr.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "cli_api.h"
#include "cli_task.h"
#include "cli_lib.h"
#include "cli_main.h"
#include "cli_pars.h"
#include "cli_msg.h"
#include "cli_io.h"

#if (SYS_CPNT_XFER == TRUE)
#include "xfer_mgr.h"
#endif

#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)
#include "cli_api_vlan.h"
#endif

#if (SYS_CPNT_DNS == TRUE)
#include "dns_pmgr.h"
#endif

#if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE)
#include "sys_pmgr.h"
#endif
#if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE)
#include "sw_watchdog_mgr.h"
#endif /* #if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE) */

#define CLI_LIB_PRINTSTRLINE_CTRL_C 0xffff
#define PROMPT_LEN  300

#ifndef _countof
#define _countof(ary)   (sizeof(ary)/sizeof(*ary))
#endif

/*--------------------- cli_msg.c  -------------------------------------*/
extern I8_T      *Cli_ErrMsg[];
extern I8_T      *ui_HelpMsg[];

typedef struct
{
    enum CLI_ACCESS_MODE_E  acc_mode;
    char                    *name;
    CMDLIST                 *cmd_list;
    char                    *prompt;
    char                    *help_msg;
} CLI_COMMAND_TREE_ROOT_ENTRY;

static CLI_COMMAND_TREE_ROOT_ENTRY cmd_tree_root[CLI_ACCESS_MODE_MAX_NUM];

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
static void
CLI_LIB_SetCommandPrivilegeRecursive(CMDLIST *cmd_list,
                                     UI16_T  cmd_idx,
                                     UI8_T  new_privilege);
#endif /* SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL */

/*--------------------- cli_cmd.c  --------------------------------------*/
extern CMDLIST   ui_normal_cmd_lst[];
extern CMDLIST   ui_privilege_exec_cmd_lst[];
extern CMDLIST   ui_privilege_cfg_global_cmd_lst[];
#if (SYS_CPNT_FILTER_THROOTTLE == TRUE)
extern CMDLIST   ui_privilege_cfg_igmp_profile_cmd_lst[];
#endif
#if (SYS_CPNT_LOOPBACK_IF_VISIBLE == TRUE)
extern CMDLIST   ui_privilege_cfg_interface_loopback_cmd_lst[];
#endif
extern CMDLIST   ui_privilege_cfg_interface_eth_cmd_lst[];
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
extern CMDLIST   ui_privilege_cfg_interface_eth0_cmd_lst[];  /*wenfang add 20090106 */
#endif
#if (SYS_CPNT_CRAFT_PORT == TRUE)
extern CMDLIST   ui_privilege_cfg_interface_craft_cmd_lst[];
#endif
extern CMDLIST   ui_privilege_cfg_interface_vlan_cmd_lst[];
#if (SYS_CPNT_IP_TUNNEL == TRUE)
extern CMDLIST   ui_privilege_cfg_interface_tunnel_cmd_lst[];
#endif
extern CMDLIST   ui_privilege_cfg_interface_pchannel_mode[];
extern CMDLIST   ui_privilege_cfg_vlan_database_mode[];
extern CMDLIST   ui_privilege_cfg_line_cmd_lst[];

#if (SYS_CPNT_CFM == TRUE)
extern CMDLIST   ui_privilege_cfg_domain_cmd_lst[];
#endif

#if (CLI_SUPPORT_L3_FEATURE == 1)
extern CMDLIST   ui_privilege_cfg_router_cmd_lst[];
extern CMDLIST   ui_privilege_cfg_routerdvmrp_cmd_lst[];
extern CMDLIST   ui_privilege_cfg_routerospf_cmd_lst[];
#if (SYS_CPNT_OSPF6 == TRUE)
extern CMDLIST   ui_privilege_cfg_routerospf6_cmd_lst[];
#endif
extern CMDLIST   ui_privilege_cfg_dhcppool_cmd_lst[]; //<<-- CLI_SUPPORT_L3_FEATURE == 1 or SYS_CPNT_DHCP_SERVER == TRUE ?
#if (SYS_CPNT_BGP == TRUE)
extern CMDLIST   ui_privilege_cfg_router_bgp_cmd_lst[];
#endif
#if (SYS_CPNT_NSM_POLICY== TRUE)
extern CMDLIST   ui_privilege_cfg_route_map_cmd_lst[];
#endif

#endif // CLI_SUPPORT_L3_FEATURE == 1


#if (SYS_CPNT_STP == SYS_CPNT_STP_TYPE_MSTP)
extern CMDLIST   ui_privilege_cfg_mstp_cmd_lst[];
#endif
#if (SYS_CPNT_ACL == TRUE)
extern CMDLIST   ui_privilege_cfg_acl_mac_cmd_lst[];
extern CMDLIST   ui_privilege_cfg_acl_standard_cmd_lst[];
extern CMDLIST   ui_privilege_cfg_acl_extended_cmd_lst[];
extern CMDLIST   ui_privilege_cfg_acl_ip_mask_cmd_lst[];
extern CMDLIST   ui_privilege_cfg_acl_mac_mask_cmd_lst[];
extern CMDLIST   ui_privilege_cfg_acl_arp_cmd_lst[];
#if (SYS_CPNT_ACL_IPV6 == TRUE)
extern CMDLIST   ui_privilege_cfg_acl_standard_ipv6_cmd_lst[];
extern CMDLIST   ui_privilege_cfg_acl_extended_ipv6_cmd_lst[];
#endif
#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_AAA == TRUE)
extern CMDLIST   ui_privilege_cfg_aaa_sg_cmd_lst[];

#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)
extern CMDLIST   ui_privilege_cfg_aaa_sg_radius_cmd_lst[];
#endif /*#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)*/

#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
extern CMDLIST   ui_privilege_cfg_class_map_cmd_lst[];
extern CMDLIST   ui_privilege_cfg_policy_map_cmd_lst[];
extern CMDLIST   ui_privilege_cfg_policy_map_class_cmd_lst[];
#endif

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
extern CMDLIST   ui_privilege_cfg_time_range_cmd_lst[];
#endif

#if (SYS_CPNT_VDSL == TRUE)
extern CMDLIST   ui_debug_cmd_lst[];
#endif

#if (SYS_CPNT_ERPS == TRUE)
extern CMDLIST   ui_privilege_cfg_erps_cmd_lst[];
#endif

extern ARGLIST   ui_arg_lst[];
/*-----------------------------------------------------------------------*/

void CLI_LIB_Init()
{
#define _SET_MODE(_mode, _name, _cmd_list, _prompt, _help_msg)              \
    {                                                                       \
        if (cmd_tree_root[_mode].name)                                      \
        {                                                                   \
            free(cmd_tree_root[_mode].name);                                \
            cmd_tree_root[_mode].name = NULL;                               \
        }                                                                   \
        if (cmd_tree_root[_mode].prompt)                                    \
        {                                                                   \
            free(cmd_tree_root[_mode].prompt);                              \
            cmd_tree_root[_mode].prompt = NULL;                             \
        }                                                                   \
        if (cmd_tree_root[_mode].help_msg)                                  \
        {                                                                   \
            free(cmd_tree_root[_mode].help_msg);                            \
            cmd_tree_root[_mode].help_msg = NULL;                           \
        }                                                                   \
                                                                            \
        cmd_tree_root[_mode].acc_mode = _mode;                              \
        cmd_tree_root[_mode].cmd_list = _cmd_list;                          \
        cmd_tree_root[_mode].name   = calloc(strlen(_name)+1, 1);           \
        if (cmd_tree_root[_mode].name)                                      \
        {                                                                   \
            strcpy(cmd_tree_root[_mode].name, _name);                       \
        }                                                                   \
        cmd_tree_root[_mode].prompt   = calloc(strlen(_prompt)+1, 1);       \
        if (cmd_tree_root[_mode].prompt)                                    \
        {                                                                   \
            strcpy(cmd_tree_root[_mode].prompt, _prompt);                   \
        }                                                                   \
        cmd_tree_root[_mode].help_msg   = calloc(strlen(_help_msg)+1, 1);   \
        if (cmd_tree_root[_mode].help_msg)                                  \
        {                                                                   \
            strcpy(cmd_tree_root[_mode].help_msg, _help_msg);               \
        }                                                                   \
    }

    UI32_T i;

    for (i=0; i<sizeof(cmd_tree_root)/sizeof(*cmd_tree_root); ++i)
    {
        cmd_tree_root[i].acc_mode = CLI_ACCESS_MODE_MAX_NUM;
    }


    /*
     * When this component is TRUE. Take normal-exec like as
     * privilege-exec.
     *
     * Set NORMAL_EXEC_MODE as the same as PRIVILEGE_EXEC_MODE,
     * So use NORMAL_EXEC_MODE can access to the same value of
     * PRIVILEGE_EXEC_MODE.
     *
     */
    _SET_MODE(NORMAL_EXEC_MODE,
              "normal-exec",
#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
              ui_privilege_exec_cmd_lst,
#else
              ui_normal_cmd_lst,
#endif
              NORMAL_EXEC_MODE_PROMPT,
              NORMAL_EXEC_MODE_HELP_MSG);

    _SET_MODE(PRIVILEGE_EXEC_MODE,
              "exec",
              ui_privilege_exec_cmd_lst,
              PRIVILEGE_EXEC_MODE_PROMPT,
              PRIVILEGE_EXEC_MODE_HELP_MSG);

    _SET_MODE(PRIVILEGE_CFG_GLOBAL_MODE,
              "configure",
              ui_privilege_cfg_global_cmd_lst,
              PRIVILEGE_CFG_GLOBAL_MODE_PROMPT,
              PRIVILEGE_CFG_GLOBAL_MODE_HELP_MSG);

#if (SYS_CPNT_LOOPBACK_IF_VISIBLE == TRUE)
    _SET_MODE(PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE,
              "interface-loopback",
              ui_privilege_cfg_interface_loopback_cmd_lst,
              PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE_PROMPT,
              PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE_HELP_MSG);
#endif

    _SET_MODE(PRIVILEGE_CFG_INTERFACE_ETH_MODE,
              "interface-eth",
              ui_privilege_cfg_interface_eth_cmd_lst,
              PRIVILEGE_CFG_INTERFACE_ETH_MODE_PROMPT,
              PRIVILEGE_CFG_INTERFACE_ETH_MODE_HELP_MSG);

#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
    _SET_MODE(PRIVILEGE_CFG_INTERFACE_ETH0_MODE,
              "interface-eth0",
              ui_privilege_cfg_interface_eth0_cmd_lst,
              PRIVILEGE_CFG_INTERFACE_ETH0_MODE_PROMPT,
              PRIVILEGE_CFG_INTERFACE_ETH0_MODE_HELP_MSG);
#endif

#if (SYS_CPNT_CRAFT_PORT == TRUE)
    _SET_MODE(PRIVILEGE_CFG_INTERFACE_CRAFT_MODE,
              "interface-craft",
              ui_privilege_cfg_interface_craft_cmd_lst,
              PRIVILEGE_CFG_INTERFACE_CRAFT_MODE_PROMPT,
              PRIVILEGE_CFG_INTERFACE_CRAFT_MODE_HELP_MSG);
#endif

    _SET_MODE(PRIVILEGE_CFG_INTERFACE_VLAN_MODE,
              "interface-vlan",
              ui_privilege_cfg_interface_vlan_cmd_lst,
              PRIVILEGE_CFG_INTERFACE_VLAN_MODE_PROMPT,
              PRIVILEGE_CFG_INTERFACE_VLAN_MODE_HELP_MSG);

#if (SYS_CPNT_IP_TUNNEL == TRUE)
    _SET_MODE(PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE,
              "interface-tunnel",
              ui_privilege_cfg_interface_tunnel_cmd_lst,
              PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE_PROMPT,
              PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE_HELP_MSG);
#endif

    _SET_MODE(PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE,
              "interface-pchannel",
              ui_privilege_cfg_interface_pchannel_mode,
              PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE_PROMPT,
              PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE_HELP_MSG);

    _SET_MODE(PRIVILEGE_CFG_VLAN_DATABASE_MODE,
              "vlan-database",
              ui_privilege_cfg_vlan_database_mode,
              PRIVILEGE_CFG_VLAN_DATABASE_MODE_PROMPT,
              PRIVILEGE_CFG_VLAN_DATABASE_MODE_HELP_MSG);

    _SET_MODE(PRIVILEGE_CFG_LINE_MODE,
              "line",
              ui_privilege_cfg_line_cmd_lst,
              PRIVILEGE_CFG_LINE_MODE_PROMPT,
              PRIVILEGE_CFG_LINE_MODE_HELP_MSG);

    _SET_MODE(PRIVILEGE_CFG_MSTP_MODE,
              "mstp",
              ui_privilege_cfg_mstp_cmd_lst,
              PRIVILEGE_CFG_MSTP_MODE_PROMPT,
              PRIVILEGE_CFG_MSTP_MODE_HELP_MSG);

#if (SYS_CPNT_CFM == TRUE)
    _SET_MODE(PRIVILEGE_CFG_DOMAIN_MODE,
              "domain",
              ui_privilege_cfg_domain_cmd_lst,
              PRIVILEGE_CFG_DOMAIN_MODE_PROMPT,
              PRIVILEGE_CFG_DOMAIN_MODE_HELP_MSG);
#endif

#if (CLI_SUPPORT_L3_FEATURE == 1)
    _SET_MODE(PRIVILEGE_CFG_ROUTER_MODE,
              "router",
              ui_privilege_cfg_router_cmd_lst,
              PRIVILEGE_CFG_ROUTER_MODE_PROMPT,
              PRIVILEGE_CFG_ROUTER_MODE_HELP_MSG);

#if (SYS_CPNT_DVMRP == TRUE)
    _SET_MODE(PRIVILEGE_CFG_ROUTERDVMRP_MODE,
              "router-dvmrp",
              ui_privilege_cfg_routerdvmrp_cmd_lst,
              PRIVILEGE_CFG_ROUTERDVMRP_MODE_PROMPT,
              PRIVILEGE_CFG_ROUTERDVMRP_MODE_HELP_MSG);
#endif

#if (SYS_CPNT_OSPF == TRUE)
    _SET_MODE(PRIVILEGE_CFG_ROUTEROSPF_MODE,
              "router-ospf",
              ui_privilege_cfg_routerospf_cmd_lst,
              PRIVILEGE_CFG_ROUTEROSPF_MODE_PROMPT,
              PRIVILEGE_CFG_ROUTEROSPF_MODE_HELP_MSG);
#endif

#if (SYS_CPNT_OSPF6 == TRUE)
    _SET_MODE(PRIVILEGE_CFG_ROUTEROSPF6_MODE,
              "router-ospf6",
              ui_privilege_cfg_routerospf6_cmd_lst,
              PRIVILEGE_CFG_ROUTEROSPF_MODE_PROMPT,
              PRIVILEGE_CFG_ROUTEROSPF6_MODE_HELP_MSG);
#endif

#if (SYS_CPNT_BGP == TRUE)
    _SET_MODE(PRIVILEGE_CFG_ROUTER_BGP_MODE,
              "router-bgp",
              ui_privilege_cfg_router_bgp_cmd_lst,
              PRIVILEGE_CFG_ROUTER_BGP_MODE_PROMPT,
              PRIVILEGE_CFG_ROUTER_BGP_MODE_HELP_MSG);
#endif

#if (SYS_CPNT_NSM_POLICY == TRUE)
    _SET_MODE(PRIVILEGE_CFG_ROUTE_MAP_MODE,
              "route-map",
              ui_privilege_cfg_route_map_cmd_lst,
              PRIVILEGE_CFG_ROUTE_MAP_MODE_PROMPT,
              PRIVILEGE_CFG_ROUTE_MAP_MODE_HELP_MSG);
#endif

#if (SYS_CPNT_DHCP_SERVER == TRUE)
    _SET_MODE(PRIVILEGE_CFG_DHCPPOOL_MODE,
              "dhcp-pool",
              ui_privilege_cfg_dhcppool_cmd_lst,
              PRIVILEGE_CFG_DHCPPOOL_MODE_PROMPT,
              PRIVILEGE_CFG_DHCPPOOL_MODE_HELP_MSG);
#endif
#endif /* CLI_SUPPORT_L3_FEATURE */


#if (SYS_CPNT_ACL == TRUE)
    _SET_MODE(PRIVILEGE_CFG_ACL_MAC_MODE,
              "acl-mac",
              ui_privilege_cfg_acl_mac_cmd_lst,
              PRIVILEGE_CFG_ACL_MAC_MODE_PROMPT,
              PRIVILEGE_CFG_ACL_MAC_MODE_HELP_MSG);

    _SET_MODE(PRIVILEGE_CFG_ACL_STANDARD_MODE,
              "acl-ip-standard",
              ui_privilege_cfg_acl_standard_cmd_lst,
              PRIVILEGE_CFG_ACL_STANDARD_MODE_PROMPT,
              PRIVILEGE_CFG_ACL_STANDARD_MODE_HELP_MSG);

    _SET_MODE(PRIVILEGE_CFG_ACL_EXTENDED_MODE,
              "acl-ip-extended",
              ui_privilege_cfg_acl_extended_cmd_lst,
              PRIVILEGE_CFG_ACL_EXTENDED_MODE_PROMPT,
              PRIVILEGE_CFG_ACL_EXTENDED_MODE_HELP_MSG);

    _SET_MODE(PRIVILEGE_CFG_ACL_IP_MASK_MODE,
              "acl-ip-mask",
              ui_privilege_cfg_acl_ip_mask_cmd_lst,
              PRIVILEGE_CFG_ACL_IP_MASK_MODE_PROMPT,
              PRIVILEGE_CFG_ACL_IP_MASK_MODE_HELP_MSG);

    _SET_MODE(PRIVILEGE_CFG_ACL_MAC_MASK_MODE,
              "acl-mac-mask",
              ui_privilege_cfg_acl_mac_mask_cmd_lst,
              PRIVILEGE_CFG_ACL_MAC_MASK_MODE_PROMPT,
              PRIVILEGE_CFG_ACL_MAC_MASK_MODE_HELP_MSG);

#if (SYS_CPNT_DAI == TRUE)
    _SET_MODE(PRIVILEGE_CFG_ACL_ARP_MODE,
              "acl-arp",
              ui_privilege_cfg_acl_arp_cmd_lst,
              PRIVILEGE_CFG_ACL_ARP_MODE_PROMPT,
              PRIVILEGE_CFG_ACL_ARP_MODE_HELP_MSG);
#endif

#if (SYS_CPNT_ACL_IPV6 == TRUE)
    _SET_MODE(PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE,
              "acl-ip6-standard",
              ui_privilege_cfg_acl_standard_ipv6_cmd_lst,
              PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE_PROMPT,
              PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE_HELP_MSG);

    _SET_MODE(PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE,
              "acl-ip6-extended",
              ui_privilege_cfg_acl_extended_ipv6_cmd_lst,
              PRIVILEGE_CFG_ACL_EXTEND_IPV6_MODE_PROMPT,
              PRIVILEGE_CFG_ACL_EXTEND_IPV6_MODE_HELP_MSG);
#endif

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    _SET_MODE(PRIVILEGE_CFG_TIME_RANGE_MODE,
              "time-range",
              ui_privilege_cfg_time_range_cmd_lst,
              PRIVILEGE_CFG_TIME_RANGE_MODE_PROMPT,
              PRIVILEGE_CFG_TIME_RANGE_MODE_HELP_MSG);
#endif
#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_AAA == TRUE)
    _SET_MODE(PRIVILEGE_CFG_AAA_SG_MODE,
              "sg-tacacs+",
              ui_privilege_cfg_aaa_sg_cmd_lst,
              PRIVILEGE_CFG_AAA_SG_TACACS_PLUS_MODE_PROMPT,
              PRIVILEGE_CFG_AAA_SG_TACACS_PLUS_MODE_HELP_MSG);

#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)
    _SET_MODE(PRIVILEGE_CFG_AAA_SG_RADIUS_MODE,
              "sg-radius",
              ui_privilege_cfg_aaa_sg_radius_cmd_lst,
              PRIVILEGE_CFG_AAA_SG_RADIUS_MODE_PROMPT,
              PRIVILEGE_CFG_AAA_SG_RADIUS_MODE_HELP_MSG);
#endif
#endif /* SYS_CPNT_AAA */

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    _SET_MODE(PRIVILEGE_CFG_CLASS_MAP_MODE,
              "class-map",
              ui_privilege_cfg_class_map_cmd_lst,
              PRIVILEGE_CFG_CLASS_MAP_MODE_PROMPT,
              PRIVILEGE_CFG_CLASS_MAP_MODE_HELP_MSG);

    _SET_MODE(PRIVILEGE_CFG_POLICY_MAP_MODE,
              "policy-map",
              ui_privilege_cfg_policy_map_cmd_lst,
              PRIVILEGE_CFG_POLICY_MAP_MODE_PROMPT,
              PRIVILEGE_CFG_POLICY_MAP_MODE_HELP_MSG);

    _SET_MODE(PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE,
              "policy-class",
              ui_privilege_cfg_policy_map_class_cmd_lst,
              PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE_PROMPT,
              PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE_HELP_MSG);
#endif

#if (SYS_CPNT_FILTER_THROOTTLE == TRUE)
    _SET_MODE(PRIVILEGE_CFG_IGMP_PROFILE_MODE,
              "igmp-profile",
              ui_privilege_cfg_igmp_profile_cmd_lst,
              PRIVILEGE_CFG_INTERFACE_IGMP_PROFILE_PROMPT,
              PRIVILEGE_CFG_INTERFACE_IGMP_PROFILE_HELP_MSG);
#endif

#if (SYS_CPNT_ERPS == TRUE)
    _SET_MODE(PRIVILEGE_CFG_ERPS_MODE,
              "erps",
              ui_privilege_cfg_erps_cmd_lst,
              PRIVILEGE_CFG_ERPS_MODE_PROMPT,
              PRIVILEGE_CFG_ERPS_MODE_HELP_MSG);
#endif

#if (SYS_CPNT_VDSL == TRUE)
    _SET_MODE(DEBUG_MODE,
              "debug",
              ui_debug_cmd_lst,
              DEBUG_MODE_PROMPT,
              DEBUG_MODE_HELP_MSG);
#endif


#undef _SET_MODE

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
    for (i=0; i<sizeof(cmd_tree_root)/sizeof(*cmd_tree_root); ++i)
    {
        if (cmd_tree_root[i].cmd_list)
        {
            CLI_LIB_SetCommandPrivilegeRecursive(cmd_tree_root[i].cmd_list,
                                                 0,
                                                 CLI_DEF_RETURN_DEFAULT_PRIVILEGE_LEVEL);
        }
    }
#endif /* SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL */

    CLI_PARS_RegisterArgumentParseHandler();

}


static CLI_COMMAND_TREE_ROOT_ENTRY *
CLI_LIB_GetCommandTreeRoot(enum CLI_ACCESS_MODE_E acc_mode)
{
    if (sizeof(cmd_tree_root)/sizeof(*cmd_tree_root) <= acc_mode)
    {
        return NULL;
    }

    if (CLI_ACCESS_MODE_MAX_NUM == cmd_tree_root[acc_mode].acc_mode)
    {
        return NULL;
    }

    assert(cmd_tree_root[acc_mode].acc_mode == acc_mode);

    return &cmd_tree_root[acc_mode];
}

static CLI_COMMAND_TREE_ROOT_ENTRY *
CLI_LIB_GetNextCommandTreeRoot(
    enum CLI_ACCESS_MODE_E *acc_mode_p)
{
    enum CLI_ACCESS_MODE_E i;

    if (NULL == acc_mode_p)
    {
        return NULL;
    }

    if (CLI_ACCESS_MODE_MAX_NUM <= *acc_mode_p)
    {
        return NULL;
    }

    for (i=*acc_mode_p; i < CLI_ACCESS_MODE_MAX_NUM; ++i)
    {
        CLI_COMMAND_TREE_ROOT_ENTRY *entry_p = CLI_LIB_GetCommandTreeRoot(i);

        if (entry_p)
        {
            *acc_mode_p = i+1;
            return entry_p;
        }
    }

    *acc_mode_p = CLI_ACCESS_MODE_MAX_NUM;
    return NULL;
}

#if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE)
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_ShowReloadInfo
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to show reload info
 * INPUT    : BOOL_T login - True (user login)
 * OUTPUT   :
 * RETURN   :
 * NOTE     : user login case - No messages when no reload configuration
 * ------------------------------------------------------------------------
 */
void CLI_LIB_ShowReloadInfo(BOOL_T login)
{
    I32_T remain_temp, remain_days, remain_hours, remain_minutes, remain_seconds;
    SYS_RELOAD_MGR_RELOAD_TIME_INFO_E ret;

    SYS_TIME_DST reload_time;
    SYS_RELOAD_OM_RELOADAT_DST reload_at;
    SYS_RELOAD_OM_RELOADREGULARITY_DST reload_regularity;

    char *month_ar[12] = {
        "January", "February", "March", "April", "May", "June",
        "July",  "August", "September", "October", "November", "December"
    };
    char *day_ar[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    BOOL_T is_get_reload_at_info, is_get_reload_in_info, is_get_reload_regularity_info,
           is_active_at = TRUE, is_active_regularity = TRUE;

    is_get_reload_in_info = SYS_PMGR_GetReloadInInfo(&remain_seconds, &reload_time);
    if (is_get_reload_in_info == TRUE)
    {
        if (login == FALSE)
        {
            remain_temp = remain_seconds;
            remain_hours = remain_temp / 3600;
            remain_temp %= 3600;
            remain_minutes = remain_temp / 60;
            CLI_LIB_PrintStr_2("Reloading switch in time:\t\t\t%ld hours %ld minutes.\r\n",
                                remain_hours, remain_minutes);
        }
    }

    is_get_reload_at_info = SYS_PMGR_GetReloadAtInfo(&reload_at, &reload_time, &remain_seconds, &is_active_at);
    if (is_get_reload_at_info == TRUE)
    {
        if (login == FALSE)
        {
            CLI_LIB_PrintStr_2("Reloading switch at time:\t\t\t%s %2ld ",
                                month_ar[reload_at.month-1], reload_at.day);
            CLI_LIB_PrintStr_3("%02ld:%02ld %ld.\r\n",
                                reload_at.hour, reload_at.minute, reload_at.year);
        }
    }

    is_get_reload_regularity_info = SYS_PMGR_GetReloadRegularityInfo(&reload_regularity, &reload_time, &remain_seconds, &is_active_regularity);
    if (is_get_reload_regularity_info == TRUE)
    {
        if(login == FALSE)
        {
            CLI_LIB_PrintStr_2("Reload switch regularity time:\t\t\t%02ld:%02ld",
                                reload_regularity.hour,
                                reload_regularity.minute);
            if (reload_regularity.period == SYS_RELOAD_TYPE_REGULARITY_PERIOD_DAILY)
            {
                CLI_LIB_PrintStr(" everyday.\r\n");
            }
            else if (reload_regularity.period == SYS_RELOAD_TYPE_REGULARITY_PERIOD_WEEKLY)
            {
                CLI_LIB_PrintStr_1(" every %s.\r\n", day_ar[reload_regularity.day_of_week]);
            }
            else
            {
                CLI_LIB_PrintStr_1(", at day %ld every month.\r\n", reload_regularity.day_of_month);
            }
        }
    }

    if ((is_get_reload_in_info == FALSE) && (is_get_reload_at_info == FALSE) && (is_get_reload_regularity_info == FALSE))
    {
        if (login == FALSE)
        {
            CLI_LIB_PrintStr("No configured settings for reloading.\r\n");
        }

        return;
    }

    ret = SYS_PMGR_GetReloadTimeInfo(&remain_seconds, &reload_time);
    switch (ret)
    {
        case SYS_RELOAD_MGR_RELOAD_TIME_INFO_OK:
            remain_days = remain_seconds / 86400;
            remain_seconds %= 86400;
            remain_hours = remain_seconds / 3600;
            remain_seconds %= 3600;
            remain_minutes = remain_seconds / 60;
            remain_seconds = remain_seconds % 60;
            CLI_LIB_PrintStr_2("\r\nThe switch will be rebooted at %s %2ld ",
                                month_ar[reload_time.month-1], reload_time.day);
            CLI_LIB_PrintStr_4("%02ld:%02ld:%02ld %ld.\r\n",
                                reload_time.hour,
                                reload_time.minute,
                                reload_time.second,
                                reload_time.year);
            CLI_LIB_PrintStr_4("Remaining Time: %ld days, %ld hours, %ld minutes, %ld seconds.\r\n",
                                remain_days, remain_hours, remain_minutes, remain_seconds);
            return;

        case SYS_RELOAD_MGR_RELOAD_TIME_INFO_INVAILD:
            CLI_LIB_PrintStr("\r\nCannot get any reload time information.\r\n");
            return;

        case SYS_RELOAD_MGR_RELOAD_TIME_INFO_NO_RELOAD_FUNCTION_ON:
            CLI_LIB_PrintStr("\r\nNo configured settings for reloading.\r\n");
            return;

        case SYS_RELOAD_MGR_RELOAD_TIME_INFO_NO_RELOAD_FUNCTION_WORK:
            CLI_LIB_PrintStr("\r\nWarning: \r\n\tBecause you don't setup system time first, \r\n\tthe Reload function won't work properly!\r\n");
            return;

        case SYS_RELOAD_MGR_RELOAD_TIME_INFO_RELOAD_AT_TIME_PASSED:
            CLI_LIB_PrintStr("\r\nThe switch reboot time was passed...\r\n");
            return;
    }
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_ShowReloadNotify
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to show reload notify
 * INPUT    : CLI_TASK_WorkingArea_T *ctrl_P
 * OUTPUT   :
 * RETURN   :
 * NOTE     : ctrl_P used to display notification in every login user
 * ------------------------------------------------------------------------
 */
void CLI_LIB_ShowReloadNotify(CLI_TASK_WorkingArea_T *ctrl_P)
{
    char scratch[100] = {0};

    if(ctrl_P->reload_countdown!=0)
    {
        SYSFUN_Sprintf(scratch,
            "\r\n\r\n***\r\n*** --- Reboot in %lu minutes ---\r\n***\r\n",ctrl_P->reload_countdown);
        ctrl_P->reload_countdown = 0;

        strcat((char *)ctrl_P->send_buf, (char *)scratch);
        CLI_IO_PrintOut(ctrl_P);
    }
}
#endif  /* #if (SYS_CPNT_SYSMGMT_DEFERRED_RELOAD == TRUE) */


BOOL_T CLI_LIB_Print_CopyInProgressSign(void)
{
#if (SYS_CPNT_XFER == TRUE)
    #define WORST_COUNT 20000 /*prevent from bug*/

    #define ERASE_CHAR(CHAR_NUM)  {\
       UI8_T i;                    \
       for(i=0; i<CHAR_NUM; i++)   \
          CLI_LIB_PrintStr(BACK);  \
    }

    UI32_T sign_count = 0;
    UI32_T previous_callback_status = 0xffff;
    char   tftp_error_msg[MAXSIZE_fileCopyTftpErrMsg+1] = {0};



    while(1)
    {
        BOOL_T Is_Xfer_In_Progress;
        UI32_T  callback_status = 0;

        CLI_MAIN_HandleMGR_Thread(SYSFUN_TIMEOUT_NOWAIT, 0 , NULL);
        CLI_MGR_Get_XferCallBackStatus(&callback_status);
        CLI_MGR_Get_XferProgressStatus(&Is_Xfer_In_Progress);

        if(!Is_Xfer_In_Progress)
        {
            BOOL_T flag;
            CLI_MGR_Get_XferProgressResult(&flag);

            if (flag == TRUE)
                return TRUE;
            else
                return FALSE;
        }
        switch(sign_count%4)
        {
        case 0:
        default:
            CLI_LIB_PrintStr("-");
            CLI_LIB_PrintStr("\r");
            break;

        case 1:
            CLI_LIB_PrintStr("/");
            CLI_LIB_PrintStr("\r");
            break;

        case 2:
            CLI_LIB_PrintStr("-");
            CLI_LIB_PrintStr("\r");
            break;

        case 3:
            CLI_LIB_PrintStr("\\");
            CLI_LIB_PrintStr("\r");
            break;
        }
        if (previous_callback_status != callback_status)
        {
            previous_callback_status = callback_status;

            switch(callback_status)
            {
            case XFER_MGR_FILE_COPY_COMPLETED: /*The final message*/
                //CLI_MGR_Set_XferProgressStatus(FALSE);
                //CLI_MGR_Set_XferCallBackStatus(0);
                break;

            case XFER_MGR_FILE_COPY_UNCLASSIFIED_ERROR:
#if (SYS_CPNT_XFER == TRUE)
                if (XFER_PMGR_GetTftpErrorMsg((UI8_T *)tftp_error_msg) == TRUE)
                {
                    CLI_LIB_PrintStr(tftp_error_msg);
                    CLI_LIB_PrintStr("\r\n");
                }
                else
#endif /* SYS_CPNT_XFER */
                {
                    CLI_LIB_PrintStr("Unclassified error.\r\n");
                }
                break;

            case XFER_MGR_FILE_COPY_FILE_NOT_FOUND:
                CLI_LIB_PrintStr("File not found.\r\n");
                break;

            case XFER_MGR_FILE_COPY_SERVER_PERMISSION_DENIED:
                CLI_LIB_PrintStr("Server permission denied.\r\n");
                break;

            case XFER_MGR_FILE_COPY_STORAGE_FULL:
                CLI_LIB_PrintStr("Storage full.\r\n");
                break;

            case XFER_MGR_FILE_COPY_TFTP_ILLEGAL_OPERATION:
                CLI_LIB_PrintStr("Illegal operation.\r\n");
                break;

            case XFER_MGR_FILE_COPY_TFTP_UNKNOWN_TRANSFER_ID:
                CLI_LIB_PrintStr("Unknown transfer ID.\r\n");
                break;

            case XFER_MGR_FILE_COPY_TFTP_FILE_EXISTED:
                CLI_LIB_PrintStr("File already exists.\r\n");
                break;

            case XFER_MGR_FILE_COPY_TFTP_NO_SUCH_USER:
                CLI_LIB_PrintStr("No such user.\r\n");
                break;

            case XFER_MGR_FILE_COPY_TIMEOUT:
                CLI_LIB_PrintStr("Timeout.\r\n");
                break;

            case XFER_MGR_FILE_COPY_TFTP_SEND_ERROR:
                CLI_LIB_PrintStr("Send error.\r\n");
                break;

            case XFER_MGR_FILE_COPY_TFTP_RECEIVE_ERROR:
                CLI_LIB_PrintStr("Receive error.\r\n");
                break;

            case XFER_MGR_FILE_COPY_TFTP_SOCKET_OPEN_ERROR:
                CLI_LIB_PrintStr("Socket open error.\r\n");
                break;

            case XFER_MGR_FILE_COPY_TFTP_SOCKET_BIND_ERROR:
                CLI_LIB_PrintStr("Socket bind error.\r\n");
                break;

            case XFER_MGR_FILE_COPY_TFTP_USER_CANCELED:
                CLI_LIB_PrintStr("Operation canceled.\r\n");
                break;

            case XFER_MGR_FILE_COPY_SET_STARTUP_ERROR:
                CLI_LIB_PrintStr("Failed to change startup file.\r\n");
                break;

            case XFER_MGR_FILE_COPY_FILE_SIZE_EXCEED:
                CLI_LIB_PrintStr("File size exceeded.\r\n");
                break;

            case XFER_MGR_FILE_COPY_MAGIC_WORD_ERROR:
                CLI_LIB_PrintStr("Unanticipated magic word.\r\n");
                break;

            case XFER_MGR_FILE_COPY_SUCCESS:
                CLI_LIB_PrintStr("Success.\r\n");
                CLI_MGR_Set_XferProgressResult(TRUE);
                break;

            case XFER_MGR_FILE_COPY_ERROR:
                CLI_LIB_PrintStr("Error.\r\n");
                CLI_MGR_Set_XferProgressResult(FALSE);
                break;

            case XFER_MGR_FILE_COPY_HEADER_CHECKSUM_ERROR:
                CLI_LIB_PrintStr("Header checksum error.\r\n");
                break;

            case XFER_MGR_FILE_COPY_IMAGE_CHECKSUM_ERROR:
                CLI_LIB_PrintStr("Image checksum error.\r\n");
                break;

            case XFER_MGR_FILE_COPY_IMAGE_TYPE_ERROR:
                CLI_LIB_PrintStr("Image type error.\r\n");
                break;

            case XFER_MGR_FILE_COPY_BUSY:
                CLI_LIB_PrintStr("Busy.\r\n");
                break;

            case XFER_MGR_FILE_COPY_PARA_ERROR:
                CLI_LIB_PrintStr("Parameter error.\r\n");
                break;

            case XFER_MGR_FILE_COPY_WRITE_FLASH_FINISH:
                CLI_LIB_PrintStr("Flash programming started.\r\n");
                break;

            case XFER_MGR_FILE_COPY_WRITE_FLASH_ERR:
                CLI_LIB_PrintStr("Flash programming error.\r\n");
                break;

            case XFER_MGR_FILE_COPY_WRITE_FLASH_PROGRAMMING:
                CLI_LIB_PrintStr("Writing to flash.\r\n");
                break;

            case XFER_MGR_FILE_COPY_READ_FILE_ERROR:
                CLI_LIB_PrintStr("File read error.\r\n");
                break;

            case XFER_MGR_FILE_COPY_UNKNOWN:
                CLI_LIB_PrintStr("Unknown error.\r\n");
                break;



            case XFER_MGR_FILE_COPY_PROGRESS_UNIT1:
                CLI_LIB_PrintStr("Synchronizing to unit 1.\r\n");
                break;

            case XFER_MGR_FILE_COPY_PROGRESS_UNIT2:
                CLI_LIB_PrintStr("Synchronizing to Unit 2.\r\n");
                break;

            case XFER_MGR_FILE_COPY_PROGRESS_UNIT3:
                CLI_LIB_PrintStr("Synchronizing to Unit 3.\r\n");
                break;

            case XFER_MGR_FILE_COPY_PROGRESS_UNIT4:
                CLI_LIB_PrintStr("Synchronizing to Unit 4.\r\n");
                break;

            case XFER_MGR_FILE_COPY_PROGRESS_UNIT5:
                CLI_LIB_PrintStr("Synchronizing to Unit 5.\r\n");
                break;

            case XFER_MGR_FILE_COPY_PROGRESS_UNIT6:
                CLI_LIB_PrintStr("Synchronizing to Unit 6.\r\n");
                break;

            case XFER_MGR_FILE_COPY_PROGRESS_UNIT7:
                CLI_LIB_PrintStr("Synchronizing to Unit 7.\r\n");
                break;

            case XFER_MGR_FILE_COPY_PROGRESS_UNIT8:
                CLI_LIB_PrintStr("Synchronizing to Unit 8.\r\n");
                break;

            default:
                CLI_LIB_PrintStr("Unknown error.\r\n");
                break;
            }
        }

        SYSFUN_Sleep(10);

        if(sign_count++ > WORST_COUNT)
            break;
    }
    CLI_LIB_PrintStr("\r\n");
    return TRUE;

#else
    return FALSE;
#endif /* #if (SYS_CPNT_XFER == TRUE) */
}
#endif // #if 0
#pragma mark #endif
#pragma mark -


void CLI_LIB_PrintStr(char *str)
{
#if 1
    printf("%s", str);

#else

#define SCRATCH_BUF_LEN  1500
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();

    char scratch[SCRATCH_BUF_LEN] = {0};/*pttch 2002.06.03 initial the string for jobs
                                         to prevent print message bug*/

    if( ctrl_P == NULL )
    {
        SYSFUN_Debug_Printf("\r\n%s():Failed to get working area\r\n", __FUNCTION__);
        return;
    }

    SYSFUN_Sprintf(scratch,"%s",str);
    strcat(ctrl_P->send_buf, scratch);

    CLI_IO_PrintOut(ctrl_P);
#endif

}




/*
 *==============================================================
 *         CLI_LIB_Printf   derived from  ipcom_debug_printf
 *==============================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
int CLI_LIB_Printf(const char *format, /* args*/ ...)
{
    char buf[2048]={0};
    va_list   ap;
    UI32_T  retval;

    /* Here we would like to use vsnprintf, but vxWorks doesn't support that.
     * we cannot use ipcom_vsnprintf either, since that function uses the
     * ip_assert macro (may cause endless recursion). */
    va_start(ap, format);
    retval = vsprintf(buf, format, ap);
    va_end(ap);
    if(retval >= 2048)
    {
        CLI_LIB_PrintStr(buf);
        SYSFUN_Debug_Printf("CLI_LIB_Printf: buffer overwrite!!!\r\n");
    }

    (void)CLI_LIB_PrintStr(buf);
    return retval;
}

#pragma mark -
#pragma mark #if 0
#if 0
void CLI_LIB_PrintStrForCallback(CLI_TASK_WorkingArea_T *ctrl_P, char *str)
{
    char scratch[SCRATCH_BUF_LEN];

    assert(ctrl_P != NULL);
    SYSFUN_Sprintf(scratch, "%s", str);
    strcat(ctrl_P->send_buf, scratch);

    CLI_IO_PrintOut(ctrl_P);

}

void CLI_LIB_PrintLongStr(char *str)
{
    #define SCRATCH_BUF_LEN  1500
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();
    UI32_T print_time = 0;
    UI32_T i = 0;
    UI32_T print_len = 0;
    char  scratch[SCRATCH_BUF_LEN+1] = {0};/*pttch 2002.06.03 initial the string for jobs*/


    if( ctrl_P == NULL )
    {
        SYSFUN_Debug_Printf("\r\n%s(): Failed to get working area\r\n", __FUNCTION__);
        return;
    }

    if(strlen(str)%SCRATCH_BUF_LEN == 0)
    {
        print_time = strlen(str)/SCRATCH_BUF_LEN;
    }
    else
    {
        print_time = strlen(str)/SCRATCH_BUF_LEN + 1 ;
    }

    for(i=0;i<print_time;i++)
    {
        memset(scratch,0,sizeof(scratch));
        if(strlen((str+SCRATCH_BUF_LEN*i))>=SCRATCH_BUF_LEN)
        {
            print_len = SCRATCH_BUF_LEN;
            memcpy(scratch,str+SCRATCH_BUF_LEN*i,SCRATCH_BUF_LEN);
            scratch[SCRATCH_BUF_LEN] = '\0';
            strcat(ctrl_P->send_buf, scratch);

            CLI_IO_PrintOut(ctrl_P);
        }
        else
        {
            print_len = strlen((str+SCRATCH_BUF_LEN*i));
            memcpy(scratch,str+SCRATCH_BUF_LEN*i,print_len);
            scratch[print_len] = '\0';
            strcat(ctrl_P->send_buf, scratch);

            CLI_IO_PrintOut(ctrl_P);
        }
    }
}

void CLI_LIB_PutChar(char ch)
{
#define SCRATCH_BUF_LEN1 60
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();

    char scratch[SCRATCH_BUF_LEN1];

    if( ctrl_P == NULL )
    {
        SYSFUN_Debug_Printf("\r\n%s(): Failed to get working area\r\n", __FUNCTION__);
        return;
    }

    SYSFUN_Sprintf(scratch, "%c", ch);
    strcat(ctrl_P->send_buf, scratch);

    CLI_IO_PrintOut(ctrl_P);
}


void CLI_LIB_PrintNullStr(int line_num)
{

#define SCRATCH_BUF_LEN2 256
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();

    int i;
    char scratch[SCRATCH_BUF_LEN2];

    if( ctrl_P == NULL )
    {
        SYSFUN_Debug_Printf("\r\n%s(): Failed to get working area\r\n", __FUNCTION__);
        return;
    }

    scratch[0] = '\0';

    for ( i=0; i < line_num; i++)
    {
        SYSFUN_Sprintf((char *)scratch,"\r\n");
        strcat((char *)ctrl_P->send_buf, (char *)scratch);
    }
    CLI_IO_PrintOut(ctrl_P);
}


/* ------------------------------------------------------------------------- */
/* Erase all or part of the display                                          */
/* ------------------------------------------------------------------------- */
void CLI_LIB_EraseDisp()
{
    UI16_T i;

    for( i=0;i <= 24; i++)
        CLI_LIB_PrintNullStr(1);
}





/************************************************************************/
/* Process command history buffer                                       */
/************************************************************************/
void CLI_LIB_InitialHistBuf(CLI_TASK_WorkingArea_T *ctrl_P)
{

    UI16_T i;

    assert(ctrl_P != NULL);

    for (i=0; i<CLI_DEF_MAX_HIST+1; i++)
    {
        ctrl_P->CMenu.HistBuf[i][0] = '\0';
        ctrl_P->CMenu.CfgHistBuf[i][0] = '\0';
    }

    ctrl_P->CMenu.HistHeader = 0;
    ctrl_P->CMenu.HistTail= 0;
    ctrl_P->CMenu.CfgHistHeader = 0;
    ctrl_P->CMenu.CfgHistTail= 0;
}


UI16_T CLI_LIB_IsHistBufEmpty(CLI_TASK_WorkingArea_T *ctrl_P)
{
    assert(ctrl_P != NULL);

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    if(0 == ctrl_P->CMenu.histsize) /* disable history */
    {
        return 1;
    }
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    switch(ctrl_P->CMenu.AccMode)
    {
    case NORMAL_EXEC_MODE:
    case PRIVILEGE_EXEC_MODE:
    default:
        if (ctrl_P->CMenu.HistHeader == ctrl_P->CMenu.HistTail)/* buffer empty */
            return 1;
        break;

    case PRIVILEGE_CFG_GLOBAL_MODE:
    case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
    case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
    case PRIVILEGE_CFG_INTERFACE_ETH0_MODE:   /*wf 20090106*/
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
    case PRIVILEGE_CFG_MSTP_MODE:
    case PRIVILEGE_CFG_DOMAIN_MODE:
    case PRIVILEGE_CFG_ROUTER_MODE:
    case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
    case PRIVILEGE_CFG_DHCPPOOL_MODE:
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
    case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
    case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
#endif
#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_AAA == TRUE)
    case PRIVILEGE_CFG_AAA_SG_MODE:

#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)
    case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
#endif /*#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)*/

#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    case PRIVILEGE_CFG_CLASS_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE:
#endif

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
    case PRIVILEGE_CFG_TIME_RANGE_MODE:
#endif

#if (SYS_CPNT_ERPS == TRUE)
    case PRIVILEGE_CFG_ERPS_MODE:
#endif

        if (ctrl_P->CMenu.CfgHistHeader == ctrl_P->CMenu.CfgHistTail)/* buffer empty */
            return 1;
        break;
    }

    return 0;
}


UI16_T CLI_LIB_AddHistBuf(char *s, CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T  histsize = 0;
    UI16_T  prevIdx;
    UI8_T   i;

    assert(ctrl_P != NULL);

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    histsize = ctrl_P->CMenu.histsize;
#else
    histsize = CLI_DEF_MAX_HIST;
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    switch(ctrl_P->CMenu.AccMode)
    {
    case NORMAL_EXEC_MODE:
    case PRIVILEGE_EXEC_MODE:
    default:
        prevIdx = (ctrl_P->CMenu.HistTail + histsize - 1) % histsize;

        if ( /*(ctrl_P->CMenu.HistHeader <= ctrl_P->CMenu.HistTail) &&*/ (ctrl_P->CMenu.HistTail < histsize) ) /* empty buffer */
        {
            if ( (strcmp(s, (char *)ctrl_P->CMenu.HistBuf[prevIdx]) == 0 ) && (ctrl_P->CMenu.HistTail > 0) )
                break;
            strcpy(ctrl_P->CMenu.HistBuf[ctrl_P->CMenu.HistTail], s);
            ctrl_P->CMenu.HistTail++;
        }
        else /* non empty */
        {
            if (strcmp(s, (char *)ctrl_P->CMenu.HistBuf[prevIdx]) != 0)
            {
                for(i=1; i<=histsize; i++)
                    strcpy(ctrl_P->CMenu.HistBuf[i-1], ctrl_P->CMenu.HistBuf[i]);

                ctrl_P->CMenu.HistTail     =   histsize;
                ctrl_P->CMenu.HistHeader   =   0;
                strcpy(ctrl_P->CMenu.HistBuf[ctrl_P->CMenu.HistTail - 1], s);

                //cjchu 2001/7/5 ctrl_P->CMenu.HistTail = (ctrl_P->CMenu.HistTail + 1) % CLI_DEF_MAX_HIST;
                /* to prevent Histtail wrap back */
                /*if( ctrl_P->CMenu.HistTail == ctrl_P->CMenu.HistHeader)
                ctrl_P->CMenu.HistHeader = (ctrl_P->CMenu.HistHeader + 1) % CLI_DEF_MAX_HIST;*/
            }

        }
        break;

    case PRIVILEGE_CFG_GLOBAL_MODE:
    case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
    case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
    case PRIVILEGE_CFG_INTERFACE_ETH0_MODE:  /*wf 20090106*/
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
    case PRIVILEGE_CFG_MSTP_MODE:
    case PRIVILEGE_CFG_DOMAIN_MODE:
    case PRIVILEGE_CFG_ROUTER_MODE:
    case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
    case PRIVILEGE_CFG_DHCPPOOL_MODE:
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
    case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
    case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
#endif
#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_AAA == TRUE)
    case PRIVILEGE_CFG_AAA_SG_MODE:

#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)
    case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
#endif /*#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)*/

#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    case PRIVILEGE_CFG_CLASS_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE:
#endif

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
    case PRIVILEGE_CFG_TIME_RANGE_MODE:
#endif

#if (SYS_CPNT_ERPS == TRUE)
    case PRIVILEGE_CFG_ERPS_MODE:
#endif

        prevIdx = (ctrl_P->CMenu.CfgHistHeader + histsize - 1) % histsize;

        if ( /*(ctrl_P->CMenu.HistHeader <= ctrl_P->CMenu.HistTail) &&*/ (ctrl_P->CMenu.CfgHistTail < histsize) ) /* empty buffer */
        {
            if ( (strcmp(s, (char *)ctrl_P->CMenu.CfgHistBuf[prevIdx]) == 0 ) && (ctrl_P->CMenu.CfgHistTail > 0) )
                break;
            strcpy(ctrl_P->CMenu.CfgHistBuf[ctrl_P->CMenu.CfgHistTail], s);
            ctrl_P->CMenu.CfgHistTail++;
        }
        else /* non empty */
        {
            if (strcmp(s, ctrl_P->CMenu.HistBuf[prevIdx]) != 0)
            {
                for(i=1; i<=histsize; i++)
                    strcpy(ctrl_P->CMenu.CfgHistBuf[i-1], ctrl_P->CMenu.CfgHistBuf[i]);

                ctrl_P->CMenu.CfgHistTail     =   histsize;
                ctrl_P->CMenu.CfgHistHeader   =   0;
                strcpy(ctrl_P->CMenu.CfgHistBuf[ctrl_P->CMenu.CfgHistTail - 1], s);

            }
        }

#if 0
        if (ctrl_P->CMenu.CfgHistHeader == ctrl_P->CMenu.CfgHistTail) /* empty buffer*/
        {
            strcpy(ctrl_P->CMenu.CfgHistBuf[ctrl_P->CMenu.CfgHistTail], s);
            ctrl_P->CMenu.CfgHistTail++;
        }
        else /* non empty */
        {
            prevIdx = (ctrl_P->CMenu.CfgHistTail+ histsize-1) % histsize;

            if (strcmp(s, ctrl_P->CMenu.CfgHistBuf[prevIdx]) != 0)
            {
                strcpy(ctrl_P->CMenu.CfgHistBuf[ctrl_P->CMenu.CfgHistTail], s);
                ctrl_P->CMenu.CfgHistTail = (ctrl_P->CMenu.CfgHistTail + 1) % histsize;

                /* to prevent Histtail wrap back */
                if( ctrl_P->CMenu.CfgHistTail == ctrl_P->CMenu.CfgHistHeader)
                    ctrl_P->CMenu.CfgHistHeader = (ctrl_P->CMenu.CfgHistHeader + 1) % histsize;
            }
        }
#endif
        break;
    }
    return 0;
}


UI16_T CLI_LIB_GetHistHeadIdx(CLI_TASK_WorkingArea_T *ctrl_P)
{

    assert(ctrl_P != NULL);
    switch(ctrl_P->CMenu.AccMode)
    {
    case NORMAL_EXEC_MODE:
    case PRIVILEGE_EXEC_MODE:
    default:
        return ctrl_P->CMenu.HistHeader;

    case PRIVILEGE_CFG_GLOBAL_MODE:
    case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
    case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
    case PRIVILEGE_CFG_INTERFACE_ETH0_MODE:    /*wf 20090106*/
#endif
#if (SYS_CPNT_CRAFT_PORT == TRUE)
    case PRIVILEGE_CFG_INTERFACE_CRAFT_MODE:
#endif
#if (SYS_CPNT_FILTER_THROOTTLE == TRUE)
    case PRIVILEGE_CFG_IGMP_PROFILE_MODE:
#endif
    case PRIVILEGE_CFG_INTERFACE_VLAN_MODE:
    case PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE:
    case PRIVILEGE_CFG_VLAN_DATABASE_MODE:
    case PRIVILEGE_CFG_LINE_MODE:
    case PRIVILEGE_CFG_MSTP_MODE:
    case PRIVILEGE_CFG_DOMAIN_MODE:
    case PRIVILEGE_CFG_ROUTER_MODE:
    case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
    case PRIVILEGE_CFG_DHCPPOOL_MODE:
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
    case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
    case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
#endif
#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_AAA == TRUE)
    case PRIVILEGE_CFG_AAA_SG_MODE:

#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)
    case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
#endif /*#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)*/

#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    case PRIVILEGE_CFG_CLASS_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE:
#endif

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
    case PRIVILEGE_CFG_TIME_RANGE_MODE:
#endif

#if (SYS_CPNT_ERPS == TRUE)
    case PRIVILEGE_CFG_ERPS_MODE:
#endif

        return ctrl_P->CMenu.CfgHistHeader;
    }
}


UI16_T CLI_LIB_GetHistPrevIdx(UI16_T *currIdx, CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T histsize=0;

    assert(ctrl_P != NULL);

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    histsize = ctrl_P->CMenu.histsize;
#else
    histsize = CLI_DEF_MAX_HIST;
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    switch(ctrl_P->CMenu.AccMode)
    {
    case NORMAL_EXEC_MODE:
    case PRIVILEGE_EXEC_MODE:
    default:
        if (*currIdx == ctrl_P->CMenu.HistHeader) /* touch top history buffer */
            return 1;

        *currIdx = (*currIdx + histsize -1) % histsize;
        break;

    case PRIVILEGE_CFG_GLOBAL_MODE:
    case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
    case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
    case PRIVILEGE_CFG_INTERFACE_ETH0_MODE:  /*wf 20090106*/
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
    case PRIVILEGE_CFG_MSTP_MODE:
    case PRIVILEGE_CFG_DOMAIN_MODE:
    case PRIVILEGE_CFG_ROUTER_MODE:
    case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
    case PRIVILEGE_CFG_DHCPPOOL_MODE:
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
    case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
    case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
#endif
#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_AAA == TRUE)
    case PRIVILEGE_CFG_AAA_SG_MODE:

#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)
    case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
#endif /*#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)*/

#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    case PRIVILEGE_CFG_CLASS_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE:
#endif

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
    case PRIVILEGE_CFG_TIME_RANGE_MODE:
#endif

#if (SYS_CPNT_ERPS == TRUE)
    case PRIVILEGE_CFG_ERPS_MODE:
#endif

        if (*currIdx == ctrl_P->CMenu.CfgHistHeader) /* touch top history buffer */
            return 1;

        *currIdx = (*currIdx + histsize -1) % histsize;
        break;
    }
    return 0;
}


UI16_T CLI_LIB_GetHistNextIdx(UI16_T *currIdx, CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T histsize = 0;

    assert(ctrl_P != NULL);

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    histsize = ctrl_P->CMenu.histsize;
#else
    histsize = CLI_DEF_MAX_HIST;
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    switch(ctrl_P->CMenu.AccMode)
    {
    case NORMAL_EXEC_MODE:
    case PRIVILEGE_EXEC_MODE:
    default:
        if (*currIdx != ctrl_P->CMenu.HistTail)
            *currIdx = (*currIdx + 1) % histsize;

        if (*currIdx == ctrl_P->CMenu.HistTail) /* touch history buttom */
            return 1;

        break;

    case PRIVILEGE_CFG_GLOBAL_MODE:
    case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
    case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
    case PRIVILEGE_CFG_INTERFACE_ETH0_MODE: /*wf 20090106*/
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
    case PRIVILEGE_CFG_MSTP_MODE:
    case PRIVILEGE_CFG_DOMAIN_MODE:
    case PRIVILEGE_CFG_ROUTER_MODE:
    case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
    case PRIVILEGE_CFG_DHCPPOOL_MODE:
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
    case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
    case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
#endif
#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_AAA == TRUE)
    case PRIVILEGE_CFG_AAA_SG_MODE:

#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)
    case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
#endif /*#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)*/

#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    case PRIVILEGE_CFG_CLASS_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE:
#endif

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
    case PRIVILEGE_CFG_TIME_RANGE_MODE:
#endif

#if (SYS_CPNT_ERPS == TRUE)
    case PRIVILEGE_CFG_ERPS_MODE:
#endif

        if (*currIdx != ctrl_P->CMenu.CfgHistTail)
            *currIdx = (*currIdx + 1) % histsize;

        if (*currIdx == ctrl_P->CMenu.CfgHistTail) /* touch history buttom */
            return 1;

        break;
    }
    return 0;
}


UI16_T CLI_LIB_GetHistTailIdx(CLI_TASK_WorkingArea_T *ctrl_P)
{

    assert(ctrl_P != NULL);
    switch(ctrl_P->CMenu.AccMode)
    {
    case NORMAL_EXEC_MODE:
    case PRIVILEGE_EXEC_MODE:
    default:
        return ctrl_P->CMenu.HistTail;

    case PRIVILEGE_CFG_GLOBAL_MODE:
    case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
    case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
    case PRIVILEGE_CFG_INTERFACE_ETH0_MODE: /*wenfang 20090106*/
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
    case PRIVILEGE_CFG_MSTP_MODE:
    case PRIVILEGE_CFG_DOMAIN_MODE:
    case PRIVILEGE_CFG_ROUTER_MODE:
    case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
    case PRIVILEGE_CFG_DHCPPOOL_MODE:
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
    case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
    case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
#endif
#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_AAA == TRUE)
    case PRIVILEGE_CFG_AAA_SG_MODE:

#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)
    case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
#endif /*#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)*/

#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    case PRIVILEGE_CFG_CLASS_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE:
#endif

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
    case PRIVILEGE_CFG_TIME_RANGE_MODE:
#endif

#if (SYS_CPNT_ERPS == TRUE)
    case PRIVILEGE_CFG_ERPS_MODE:
#endif

        return ctrl_P->CMenu.CfgHistTail;
    }
}


char* CLI_LIB_GetHistBuf(UI16_T index, CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI32_T histsize = 0;

    assert(ctrl_P != NULL);

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    histsize = ctrl_P->CMenu.histsize;
#else
    histsize = CLI_DEF_MAX_HIST;
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    if(index >= histsize)
        return 0;

    switch(ctrl_P->CMenu.AccMode)
    {
    case NORMAL_EXEC_MODE:
    case PRIVILEGE_EXEC_MODE:
    default:
        return ctrl_P->CMenu.HistBuf[index];

    case PRIVILEGE_CFG_GLOBAL_MODE:
    case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
    case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
    case PRIVILEGE_CFG_INTERFACE_ETH0_MODE:  /*wenfang  20090106*/
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
    case PRIVILEGE_CFG_MSTP_MODE:
    case PRIVILEGE_CFG_DOMAIN_MODE:
    case PRIVILEGE_CFG_ROUTER_MODE:
    case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF_MODE:
    case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
    case PRIVILEGE_CFG_DHCPPOOL_MODE:
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
    case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
    case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
#endif
#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_AAA == TRUE)
    case PRIVILEGE_CFG_AAA_SG_MODE:

#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)
    case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
#endif /*#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)*/

#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
    case PRIVILEGE_CFG_CLASS_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_MODE:
    case PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE:
#endif

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
    case PRIVILEGE_CFG_TIME_RANGE_MODE:
#endif

#if (SYS_CPNT_ERPS == TRUE)
    case PRIVILEGE_CFG_ERPS_MODE:
#endif

        return ctrl_P->CMenu.CfgHistBuf[index];
    }
}

#endif
#pragma mark #endif
#pragma mark -

/* from 1503 */

/************************************************************************/
/* Process enhanced editing mode                                        */
/*                                                                      */
/************************************************************************/

void CLI_LIB_InitialEditMode(CLI_TASK_WorkingArea_T *ctrl_P)
{

    assert(ctrl_P != NULL);
    ctrl_P->CMenu.EditMode = EDIT_ENABLED; /* enable enhanced editing mode */
}


void CLI_LIB_EnableEditMode(CLI_TASK_WorkingArea_T *ctrl_P)
{

    assert(ctrl_P != NULL);
    ctrl_P->CMenu.EditMode = EDIT_ENABLED;
}


void CLI_LIB_DisableEditMode(CLI_TASK_WorkingArea_T *ctrl_P)
{

    assert(ctrl_P != NULL);
    ctrl_P->CMenu.EditMode = EDIT_DISABLED;
}


UI16_T CLI_LIB_GetEditModeStatus(CLI_TASK_WorkingArea_T *ctrl_P)
{

    assert(ctrl_P != NULL);
    return (ctrl_P->CMenu.EditMode);
}


/************************************************************************/
/* Process command deleted buffer                                       */
/************************************************************************/

void CLI_LIB_InitialDelBuf(CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI16_T i;

    assert(ctrl_P != NULL);
    for (i=0; i<CLI_DEF_MAX_DELNUM; i++)
        ctrl_P->CMenu.DelBuf[i][0] = '\0';

    ctrl_P->CMenu.DelHeader = 0;
    ctrl_P->CMenu.DelNum = 0;
}


UI16_T CLI_LIB_AddDelBuf(char *s, UI16_T sidx, UI16_T eidx, CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI16_T  i;

    assert(ctrl_P != NULL);

    if (ctrl_P->CMenu.DelNum == 0) /* first time */
    {
        for ( i=sidx; i < eidx; i++)
            ctrl_P->CMenu.DelBuf[0][i-sidx] = *(s+i);
        ctrl_P->CMenu.DelNum++;
    }
    else if ((ctrl_P->CMenu.DelNum > 0) && (ctrl_P->CMenu.DelNum < CLI_DEF_MAX_DELNUM))
    {
        for (i=sidx; i<eidx; i++)
            ctrl_P->CMenu.DelBuf[ctrl_P->CMenu.DelNum][i-sidx] = *(s+i);

        ctrl_P->CMenu.DelNum++;
    }
    else if (ctrl_P->CMenu.DelNum == CLI_DEF_MAX_DELNUM)
    {
        for (i=sidx; i<eidx; i++)
            ctrl_P->CMenu.DelBuf[ctrl_P->CMenu.DelHeader][i-sidx] = *(s+i);

        ctrl_P->CMenu.DelHeader = (ctrl_P->CMenu.DelHeader + 1) % CLI_DEF_MAX_DELNUM;
    }

    return 0;
}


UI16_T CLI_LIB_GetDelBufFinalIdx(UI16_T *idx, CLI_TASK_WorkingArea_T *ctrl_P)
{

    assert(ctrl_P != NULL);
    *idx = 0;

    if (ctrl_P->CMenu.DelNum < CLI_DEF_MAX_DELNUM)
        *idx = ctrl_P->CMenu.DelNum;
    else
        *idx = ctrl_P->CMenu.DelHeader;

    *idx = (*idx + CLI_DEF_MAX_DELNUM - 1) % CLI_DEF_MAX_DELNUM;

    if (*idx == ctrl_P->CMenu.DelHeader) /* have only one entry */
        return 0;

    return 1; /* have more than one entries */
}


UI16_T CLI_LIB_GetDelBufPrevIdx(UI16_T currIdx, CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI16_T  ret;

    assert(ctrl_P != NULL);

    if (currIdx == ctrl_P->CMenu.DelHeader) /* now touch buffer top */
    {
        if (ctrl_P->CMenu.DelNum < CLI_DEF_MAX_DELNUM) /* buffer not full */
            currIdx = ctrl_P->CMenu.DelNum;
    }

    ret = (currIdx + CLI_DEF_MAX_DELNUM -1) % CLI_DEF_MAX_DELNUM;

    return ret;
}


char* CLI_LIB_GetDelBuf(UI16_T index, CLI_TASK_WorkingArea_T *ctrl_P)
{

    assert(ctrl_P != NULL);
    return (ctrl_P->CMenu.DelBuf[index]);
}


UI16_T CLI_LIB_IsDelBufEmpty(CLI_TASK_WorkingArea_T *ctrl_P)
{

    assert(ctrl_P != NULL);
    if (ctrl_P->CMenu.DelNum == 0) /* buffer empty */
        return 1;
    return 0;
}


UI16_T CLI_LIB_GetTermDispWidth(CLI_TASK_WorkingArea_T *ctrl_P)
{
    enum
    {
        PROMPT_LEN = 300
    };

    UI16_T        disp_width = CLI_DEF_DISP_WIDTH;
    UI16_T        len=0;
    char prompt[PROMPT_LEN] = {0};

    assert(ctrl_P != NULL);
    SYS_PMGR_GetPromptString((UI8_T*)prompt);

    //MIB2_PMGR_GetSysName(Name);

#if (defined(STRAWMAN) || defined(STRAWMANHD))

    if (strlen((char *)Name) >= 16)
        len = 16;
    else
        len = strlen((char *)Name);
#else
    switch(ctrl_P->sess_type)
    {
    case CLI_TYPE_UART:

        if(strlen(prompt)==0)
        {
            len = strlen("(Console)");
        }
        else
        {
            SYSFUN_Sprintf(prompt, "(%s)",prompt);
            len = strlen(prompt);
        }
        break;

    case CLI_TYPE_TELNET:

        if(strlen(prompt)==0)
        {
            len = strlen("(Vty)");
        }
        else
        {
            SYSFUN_Sprintf(prompt, "(%s)",prompt);
            len = strlen(prompt);
        }
        break;

    default:
        len = 0;
        break;
    }
#endif

    /*
    if (CLI_LIB_IsProvision(ctrl_P) == FALSE)
    {
        CLI_COMMAND_TREE_ROOT_ENTRY *entry_p = CLI_LIB_GetCommandTreeRoot(ctrl_P->CMenu.AccMode);
        if (entry_p)
        {
            if (PRIVILEGE_CFG_VLAN_DATABASE_MODE == ctrl_P->CMenu.AccMode)
            {
#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)
                disp_width = CLI_DEF_DISP_WIDTH - (len + strlen( entry_p->prompt ) + 6);
#else
                disp_width = CLI_DEF_DISP_WIDTH - (len + strlen( entry_p->prompt ));
#endif
            }
            else
            {
                disp_width = CLI_DEF_DISP_WIDTH - (len + strlen( entry_p->prompt ));
            }
        }
    }
     */

    return disp_width-2;
}


/**************************************************************************/
/* Show command prompt message                                            */
/*                                                                        */
/**************************************************************************/

void CLI_LIB_ShowPromptMsg(CLI_TASK_WorkingArea_T *ctrl_P)
{
    enum
    {
        PROMPT_LEN = 300
    };

    char prompt[PROMPT_LEN] = {0};
    SYS_PMGR_GetPromptString((UI8_T *)prompt);

    assert(ctrl_P != NULL);

#if (defined(STRAWMAN) || defined(STRAWMANHD))
    char Name[SYS_ADPT_MAX_SYSTEM_NAME_STR_LEN + 1]     = {0};

    MIB2_POM_GetSysName((UI8_T *)Name);

    if (strlen(Name) >=16 )
        memcpy(prompt,Name,sizeof(char)*16);
    else
        strcpy(prompt, Name);
#else
    switch(ctrl_P->sess_type)
    {
    case CLI_TYPE_UART:

        if(strlen(prompt)==0)
        {
            SYSFUN_Sprintf(prompt, "Console");
        }
        break;

    case CLI_TYPE_TELNET:
        if(strlen(prompt)!=0)
        {
            SYSFUN_Sprintf(prompt, "%s-%d", prompt, CLI_TASK_GetMySessId() - CLI_TASK_MIN_TELNET_SESSION_ID);
        }
        else
        {
            SYSFUN_Sprintf(prompt, "Vty-%d", CLI_TASK_GetMySessId() - CLI_TASK_MIN_TELNET_SESSION_ID);
        }
        break;

    default:
        ;
        break;
    }
#endif

    /*
    if (CLI_LIB_IsProvision(ctrl_P) == FALSE)
    {
        CLI_COMMAND_TREE_ROOT_ENTRY *entry_p = CLI_LIB_GetCommandTreeRoot(ctrl_P->CMenu.AccMode);

        if (NULL != entry_p)
        {
            if (PRIVILEGE_CFG_VLAN_DATABASE_MODE == ctrl_P->CMenu.AccMode)
            {
#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)
                UI32_T vlan_id;
                char vlan_string_temp[20]={0};

                CLI_API_GET_VLAN_DATABASE_VLANID(&vlan_id);
                SYSFUN_Sprintf(vlan_string_temp,"%s%lu)#", entry_p->prompt,vlan_id);
                strcat(prompt, vlan_string_temp);
#else
                strcat(prompt, entry_p->prompt);
#endif
            }
            else
            {
                strcat(prompt, entry_p->prompt);
            }
        }
    }
    else
     */
    {
        prompt[0] = 0;
    }

    CLI_LIB_PrintStr(prompt);
}



/************************************************************************/
/* Check if the *buff string is empty                                   */
/*                                                                      */
/************************************************************************/

UI16_T CLI_LIB_CheckNullStr(char *buff)
{
    while ((*buff == ' ') || (*buff == 0x09))
        buff++;

    if (*buff == '\0')
        return 1; /* null string */
    else
        return 0; /* not null string */
}

UI16_T CLI_LIB_ProcMoreFeature()
{
    char c;
    UI16_T ret;

    printf("--- [Space] Next page, [Enter] Next line, [A] All, Other to exit ---\r\n");

    scanf("%c", &c);

    switch (c)
    {
        case ' ':
            ret = 3;
            break;

        case '\r':
        case '\n':
            ret = 2;
            break;

        case 'A':
        case 'a':
            ret = 4;
            break;

        default:
            ret = 0;
            break;
    }
    
    return ret;
}

#pragma mark -
#pragma mark #if 0
#if 0
UI16_T CLI_LIB_RedisplayCmd(char *buf, UI16_T str_idx, UI16_T disp_width, UI16_T type, CLI_TASK_WorkingArea_T *ctrl_P)
{
    UI16_T  new_shift;
    UI16_T  buf_len;
    UI16_T  sidx, eidx;
    UI16_T  i;


    assert(ctrl_P != NULL);
    buf_len = strlen((char *)buf);
    new_shift = 0;

    while ((str_idx-new_shift) >= disp_width)
        new_shift += 10;

    sidx = new_shift;

    if (new_shift > 0)
    {
        CLI_LIB_PrintStr("$");
        sidx++;
    }

    eidx = sidx;

    while ((eidx<buf_len) && (eidx-new_shift < disp_width))
        eidx++;

    if ((buf_len-new_shift) == (disp_width+1))
        eidx++;

    CLI_LIB_PrintFStr(buf, sidx, eidx);

    if ((buf_len-new_shift) > (disp_width+1))
        CLI_LIB_PrintStr("$");
    else
    {
        if (type == 1) /* for redisplay invalid command */
        {
            for (i=eidx; i<=disp_width; i++)
                CLI_LIB_PrintStr(SPACE);

            for (i=eidx; i<=disp_width; i++)
                CLI_LIB_PrintStr(BACK);
        }
    }

    CLI_IO_PrintOut(ctrl_P);

    return (str_idx-new_shift);
}


void CLI_LIB_PrintFStr(char *str, UI16_T sidx, UI16_T eidx)
{
#define SCRATCH_BUF_LEN3 256

    char  scratch[SCRATCH_BUF_LEN3];
    char  ch;
    UI16_T i;
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();

    if( ctrl_P == NULL )
    {
        SYSFUN_Debug_Printf("\r\n%s(): Failed to get working area\r\n", __FUNCTION__);
        return;
    }

    for (i=0; i<eidx-sidx; i++)
    {
        ch = *(str+sidx+i);

        if ((ch >= 0x01) && (ch <= 0x1B))
        {
            if (ch == 0x09)     /* TAB key */
                ch = 0x20;      /* display as SPACE */
            else
                ch = 0x5E;      /* display as '^' */
        }

        scratch[i] = ch;
    }

    scratch[i] = '\0';
    CLI_LIB_PrintStr(scratch);
    CLI_IO_PrintOut(ctrl_P);
}


/**************************************************************************/
/* Process Command Line scroll (insert or move cursor right)              */
/*                                                                        */
/* Argument:                                                              */
/*      char   *old_buf  :                                               */
/*      char   *buf      :                                               */
/*      UI16_T  sidx      : The starting string index of buf that need to */
/*                         be print                                       */
/*      I16_T si        : The cursor index of buf                         */
/*      I16_T old_shift :                                                 */
/*      I16_T new_shift :                                                 */
/*      UI16_T  disp_width:                                               */
/**************************************************************************/

void CLI_LIB_ScrollAt(char *old_buf, char *buf, UI16_T sidx, I16_T si,
                 I16_T old_shift, I16_T new_shift, UI16_T disp_width)
{
    UI16_T  len1, len2;
    UI16_T  disp_len1, disp_len2;
    UI16_T  more_flag1, more_flag2;
    I16_T   space_cnt;
    UI16_T  same_num;
    UI16_T  cmp_idx;
    UI16_T  eidx;
    UI16_T  i;

    len1 = strlen((char *)old_buf);
    len2 = strlen((char *)buf);

    disp_len1 = len1 - old_shift;
    disp_len2 = len2 - new_shift;

    more_flag1 = more_flag2 = 0;

    if (disp_len1 > disp_width+1)
        more_flag1 = 1; /* have '$' in the line end */

    if (disp_len2 > disp_width+1)
        more_flag2 = 1;

    space_cnt = 0;
    same_num = 0;
    cmp_idx = (sidx > si) ? sidx : si;

    if ( (more_flag1 == 0) && (more_flag2 == 0) )
    {
        if (disp_len1 == disp_len2)
        {
            same_num = CLI_LIB_StrECmp(old_buf, buf, cmp_idx-new_shift, disp_len1, old_shift, new_shift);
        }

        eidx = len2 - same_num;

        space_cnt = (disp_len1 > disp_len2) ? (disp_len1-disp_len2) : 0;
    }
    else if ( (more_flag1 == 1) && (more_flag2 == 1) )
    {
        same_num = CLI_LIB_StrECmp(old_buf, buf, cmp_idx-new_shift, disp_width, old_shift, new_shift);
        eidx = new_shift + (disp_width - same_num);
    }
    else
    {
        if (more_flag2 == 1)
            eidx = new_shift + disp_width;
        else
            eidx = len2;

        if (more_flag1 == 1)
            space_cnt = (disp_width+1) - disp_len2;
    }

    CLI_LIB_PrintFStr(buf, sidx, eidx);

    /* Determine the final char
     */
    if ((more_flag1 == 0) && (more_flag2 == 1))
    {
        CLI_LIB_PrintStr("$");
        eidx++;
    }

    /* Clear the characters behind eidx if there exist any.
     */
    for (i=0; i<space_cnt; i++)
        CLI_LIB_PrintStr(SPACE);

    eidx += space_cnt; /* serve as forward index, for print BACK latter */

    for (i=si; i<eidx; i++)
        CLI_LIB_PrintStr(BACK);
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_ScrollForBS
 * ------------------------------------------------------------------------
 * PURPOSE  : Process Command Line scroll for BS/DEL key
 *
 * INPUT    : *old_buf, *buf, sidx, si, old_shift, new_shift, disp_width
 * OUTPUT   :
 * RETURN   : new_shift
 * NOTE     :
 * ------------------------------------------------------------------------
 */
I16_T CLI_LIB_ScrollForBS(char *old_buf, char *buf, UI16_T sidx, I16_T si,
                 I16_T old_shift, I16_T new_shift, UI16_T disp_width)
{
    UI16_T  len1, len2;
    UI16_T  disp_len1, disp_len2;
    UI16_T  more_flag1, more_flag2;
    UI16_T  same_num;
    UI16_T  cmp_idx;
    UI16_T  eidx;
    UI16_T  i;
    I16_T   space_cnt;
    I16_T   shift_num;

    len1 = strlen((char *)old_buf);
    len2 = strlen((char *)buf);
    shift_num = old_shift;

    /* re-count the shift number when the cursor is in the page of last shift */
    if ((si > ((len2 - disp_width)/10 + 1) * 10) && si <= len2 && new_shift >= (((len2 - disp_width)/10 + 1) * 10) )
    {
        if (len1 >= disp_width)
            old_shift = ((len1 - disp_width)/10 + 1) * 10;
        else
            old_shift = 0;

        if (len2 >= disp_width)
            new_shift = ((len2 - disp_width)/10 + 1) * 10;
        else
            new_shift = 0;
    }

    disp_len1 = len1 - old_shift;
    disp_len2 = len2 - new_shift;

     more_flag1 = more_flag2 = 0;

    if (disp_len1 > disp_width+1)
        more_flag1 = 1; /* have '$' in the line end */

    if (disp_len2 > disp_width+1)
        more_flag2 = 1;

    space_cnt = 0;
    same_num = 0;
    cmp_idx = (sidx > si) ? sidx : si;

    if ( (more_flag1 == 0) && (more_flag2 == 0) )
    {
        if (disp_len1 == disp_len2)
        {
            same_num = CLI_LIB_StrECmp(old_buf, buf, cmp_idx-new_shift, disp_len1, old_shift, new_shift);
        }

        /* when the shift number of old cmd and new cmd is different. */
        if (old_shift > new_shift)
        {
            /* the cursor is not in the last page and the shift num of old cmd is bigger */
            if (si < old_shift)
            {
                /* Move cursor back */
                for (i=new_shift+same_num; i<si; i++)
                {
                    CLI_LIB_PrintStr(BACK);
                }
            }
            else
            {
                if (shift_num < old_shift)
                {
                    /* Move cursor back */
                    for (i=shift_num+same_num; i<si; i++)
                    {
                        CLI_LIB_PrintStr(BACK);
                    }
                    disp_len1 = len1 - shift_num;
                }
                else
                {
                    /* Move cursor back */
                    for (i=old_shift+same_num; i<si; i++)
                    {
                        CLI_LIB_PrintStr(BACK);
                    }
                }
            }

            if (new_shift != 0)
            {
                /* if we shift, we print $ in front of the command */
                CLI_LIB_PrintStr("$");
                sidx = new_shift+1;
            }
            else
            {
                sidx = new_shift;
            }
            eidx = len2 - same_num;
        }
        else
        {
            eidx = len2 - same_num;
        }

        space_cnt = (disp_len1 > disp_len2) ? (disp_len1-disp_len2) : 0;
    }
    else if ( (more_flag1 == 1) && (more_flag2 == 1) )
    {
        same_num = CLI_LIB_StrECmp(old_buf, buf, cmp_idx-new_shift, disp_width, old_shift, new_shift);
        eidx = new_shift + (disp_width - same_num);
    }
    else
    {
        if (more_flag2 == 1)
            eidx = new_shift + disp_width;
        else
            eidx = len2;

        if (more_flag1 == 1)
            space_cnt = (disp_width+1) - disp_len2;
    }

    CLI_LIB_PrintFStr(buf, sidx, eidx);

    /* Determine the final char
     */
    if ((more_flag1 == 0) && (more_flag2 == 1))
    {
        CLI_LIB_PrintStr("$");
        eidx++;
    }

    /* Clear the characters behind eidx if there exist any.
     */
    for (i=0; i<space_cnt; i++)
        CLI_LIB_PrintStr(SPACE);

    eidx += space_cnt; /* serve as forward index, for print BACK latter */

    for (i=si; i<eidx; i++)
        CLI_LIB_PrintStr(BACK);

    return new_shift;
}


/**************************************************************************/
/* Process Command Line scroll (insert or move cursor right)              */
/*                                                                        */
/* Argument:                                                              */
/*      char   *buff     :                                               */
/*      I16_T si          :                                               */
/*      I16_T shift_num   :                                               */
/*      UI16_T  disp_width:                                               */
/**************************************************************************/

I16_T CLI_LIB_ScrollToLeft(char *old_buf, char *buf, I16_T old_si, I16_T si,
                      I16_T old_shift, UI16_T disp_width)
{
    UI16_T  buf_len;
    I16_T   new_shift;
    UI16_T  same_num;
    UI16_T  i;

    buf_len = strlen((char *)buf);

    new_shift = old_shift + 10;

    while (si-new_shift >= disp_width)
        new_shift += 10;

    /* Find out the starting index that need to be printed */

    if (old_shift == 0) /* the first time scroll to head */
        same_num = 0;

    else
        same_num = CLI_LIB_StrNCmp(old_buf, buf, old_shift+1, old_si,
                                            new_shift+1, buf_len) + 1;

    /* Move cursor back */
    for (i=old_shift+same_num; i<old_si; i++)
        CLI_LIB_PrintStr(BACK);

    if (old_shift == 0) /* need to print '$ */
    {
        CLI_LIB_PrintStr("$");
        same_num = 1; /* for skip printing the first char in line head */
    }

    CLI_LIB_ScrollAt(old_buf, buf, new_shift+same_num, si, old_shift, new_shift,
                                                    disp_width);

    return new_shift;
}


/*****************************************************************************/
/* CLI_LIB_ScrollToRight                                                          */
/*                                                                           */
/* Description:                                                              */
/*     Print out the buffer contents after specified starting index.         */
/*     The terminal width is taken into consider.                            */
/*     Then move the cursor to the expected position.                        */
/*                                                                           */
/* Arguments:                                                                */
/*     char   *buf      : The string buffer that to be displayed            */
/*     UI16_T  old_si    : The old cursor index                              */
/*     I16_T   si        : Cursor index based on string index                */
/*     I16_T   shift_num : The number of chars that shifted out in the from  */
/*     UI16_T  disp_width: The number of chars that can be shown in screen   */
/*****************************************************************************/

I16_T CLI_LIB_ScrollToRight(char *old_buf, char *buf, I16_T old_si, I16_T si,
                                   I16_T old_shift, UI16_T disp_width)
{
    I16_T   new_shift;
    UI16_T  same_num;
    UI16_T  i;

    new_shift = old_shift - 10;

    while ((si <= new_shift) && (new_shift > 0))
        new_shift -= 10;

    if (new_shift == 0)
        same_num = 0;

    else
        same_num = CLI_LIB_StrNCmp(old_buf, buf, old_shift+1, old_si,
                                                  new_shift+1, si) + 1;

    for (i=old_shift+same_num; i<old_si; i++)
        CLI_LIB_PrintStr(BACK);

    CLI_LIB_ScrollAt(old_buf, buf, new_shift+same_num, si, old_shift, new_shift,
                                                  disp_width);

    return new_shift;
}


I16_T CLI_LIB_ScrollToEnd(char *s1, char *s2, I16_T si, I16_T shift1, UI16_T disp_width)
{
    I16_T   len1, len2;
    I16_T   disp_len1, disp_len2;
    I16_T   shift2;
    UI16_T  same_num;
    UI16_T  i;

    len1 = strlen((char *)s1);
    len2 = strlen((char *)s2);

    shift2 = 0;

    if (len2 > disp_width)
        shift2 = ((len2 - disp_width)/10 + 1) * 10;

    else if (len2 == disp_width)
        shift2 = 10;

    /* Find out the cursor back count */
    if ( ((shift1==0) && (shift2>0)) || ((shift1>0) && (shift2==0)) )
        same_num = 0;

    else if ((shift1 > 0) && (shift2 > 0))
        same_num = CLI_LIB_StrNCmp(s1, s2, shift1+1, si, shift2+1, len2) + 1;

    else
        same_num = CLI_LIB_StrNCmp(s1, s2, shift1, si, shift2, len2);

    /* Move cursor back */
    for (i=shift1+same_num; i<si; i++)
        CLI_LIB_PrintStr(BACK);

    /* Print out the specified contents */
    if ((shift2 > 0) && (shift1 == 0))
    {
        CLI_LIB_PrintStr("$");
        same_num = 1;       /* for avoiding print the first char in head */
    }

    CLI_LIB_PrintFStr(s2, shift2+same_num, len2);

    /* Clear additional space */
    disp_len1 = len1 - shift1;

    if (disp_len1 > disp_width+1)
        disp_len1 = disp_width+1;

    disp_len2 = len2 - shift2;

    for (i=disp_len2; i<disp_len1; i++)
        CLI_LIB_PrintStr(SPACE);

    for (i=disp_len2; i<disp_len1; i++)
        CLI_LIB_PrintStr(BACK);

    return shift2;
}


/************************************************************************/
/* CLI_LIB_StrCmp                                                            */
/*                                                                      */
/* Description:                                                         */
/*     Compare two strings, return the matched size and compare result  */
/*                                                                      */
/* Arguments:                                                           */
/*     I8_T  *s1  : First string for comparison                         */
/*     I8_T  *s2  : Second string for comparison                        */
/*     UI16_T  *num : Two string matched number for return              */
/*                                                                      */
/* Return:                                                              */
/*     -1: if s1 < s2,                                                  */
/*     0 : if s1 = s2,                                                  */
/*     1 : if s1 > s2                                                   */
/************************************************************************/

I16_T CLI_LIB_StrCmp(const char *s1, const char *s2, UI16_T *num)
{
    char f, l;
    int match_num = -1;

    do
    {
        f = ((*s1 <= 'Z') && (*s1 >= 'A')) ? *s1 + 'a' - 'A' : *s1;
        l = ((*s2 <= 'Z') && (*s2 >= 'A')) ? *s2 + 'a' - 'A' : *s2;
        s1++;
        s2++;
        match_num ++;
    } while ((f) && (f == l));

    *num = match_num;

    if (f < l)
        return -1;
    else if (l < f)
        return 1;

    return 0;
}


/************************************************************************/
/* CLI_LIB_StrECmp                                                           */
/*                                                                      */
/* Description:                                                         */
/*     Compare two strings from tail to header(the starting point)      */
/*                                                                      */
/* Arguments:                                                           */
/*     I8_T  *s1   : First string for comparison                        */
/*     I8_T  *s2   : Second string for comparison                       */
/*     UI16_T  sidx  : The starting index based on display view         */
/*     UI16_T  eidx  : The end string index based on display view       */
/*                                                                      */
/* Return:                                                              */
/*     Return the number of characters that the same in s1 and s2 from  */
/*     string end index                                                 */
/************************************************************************/

UI16_T CLI_LIB_StrECmp(char *s1, char *s2, UI16_T sidx, UI16_T eidx,
                                  I16_T shift1, I16_T shift2)
{
    UI16_T idx;
    UI16_T same_num;

    idx = eidx;

    while (idx > sidx)
    {
        if (s1[idx+shift1-1] != s2[idx+shift2-1])
            break;

        idx--;
    }

    same_num = eidx - idx;

    return same_num;
}


/************************************************************************/
/* CLI_LIB_StrNCmp                                                           */
/*                                                                      */
/* Description:                                                         */
/*     Compare two strings, starting from the specified string index    */
/*     and end with the given end string index                          */
/*                                                                      */
/* Arguments:                                                           */
/*     I8_T  *s1   : First string for comparison                        */
/*     I8_T  *s2   : Second string for comparison                       */
/*     UI16_T  sidx1 : The starting string index of s1                  */
/*     UI16_T  eidx1 : The end string index of s1 that to be compared   */
/*     UI16_T  sidx2 : The starting string index of s2                  */
/*     UI16_T  eidx2 : The end string index of s2 that to be compared   */
/*                                                                      */
/* Return:                                                              */
/*     Return the number of characters that the same in s1 and s2 from  */
/*     string starting index                                            */
/************************************************************************/

UI16_T CLI_LIB_StrNCmp(char *s1, char *s2, UI16_T sidx1, UI16_T eidx1,
                                             UI16_T sidx2, UI16_T eidx2)
{
    UI16_T len1, len2, len;
    UI16_T cnt;
    UI16_T i;

    len1 = eidx1 - sidx1;
    len2 = eidx2 - sidx2;

    len = (len1 < len2) ? len1 : len2;

    cnt = 0;
    for (i=0; i<len; i++)
    {
        if (s1[sidx1+i] == s2[sidx2+i])
            cnt++;
        else
            break;
    }

    return cnt;
}


/* come from 1503 */

/**************************************************************************/
/* Show error messages defined in ui_ErrMsg[]                             */
/*                                                                        */
/**************************************************************************/

void CLI_LIB_ShowErrMsg(UI16_T err_code, char *buff, UI16_T str_idx, CLI_TASK_WorkingArea_T *ctrl_P)
{
#if (SYS_CPNT_EH == TRUE)
    UI32_T module_id;
    UI32_T function_no;
    UI32_T msg_flag;
    char  msg[CLI_DEF_MAX_MSGNUM];
#endif

    UI16_T   i,len, disp_width;
    char prompt[PROMPT_LEN] = {0};
    SYS_PMGR_GetPromptString((UI8_T *)prompt);

    assert(ctrl_P != NULL);

#if (defined(STRAWMAN) || defined(STRAWMANHD))
      if (strlen((char *)Name) >= 16)
         len = 16;
      else
         len = strlen((char *)Name);
#else
    switch(ctrl_P->sess_type)
    {
    case CLI_TYPE_UART:

       if(strlen(prompt)==0)
       {
          len = strlen("Console");
       }
       else
       {
          len = strlen(prompt);
       }

       break;

    case CLI_TYPE_TELNET:

       if(strlen(prompt)==0)
       {
          len = strlen("Vty-x");
       }
       else
       {
          SYSFUN_Sprintf(prompt, "%s-x",prompt);
          len = strlen(prompt);
       }

       break;

    default:
       len = 0;
       break;
    }
#endif

    if (CLI_LIB_IsProvision(ctrl_P) == FALSE)
    {
        CLI_COMMAND_TREE_ROOT_ENTRY *entry_p = CLI_LIB_GetCommandTreeRoot(ctrl_P->CMenu.AccMode);
        if (entry_p && entry_p->prompt)
        {
            len += strlen( entry_p->prompt );

#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)
            if (PRIVILEGE_CFG_VLAN_DATABASE_MODE == ctrl_P->CMenu.AccMode)
            {
                len += 6;
            }
#endif /* SYS_CPNT_MSTP_SUPPORT_PVST */
        }
    }
    else
    {
      len += 0;
    }

    /* added by Dralee, 1998-11-02
     * Rev V1.14
     */
    len = len % CLI_DEF_DISP_WIDTH;

    if (err_code == CLI_ERR_CMD_INVALID)
    {
        for (i=0; i<str_idx+len; i++) /* changed later for prompt length */
        {
            CLI_LIB_PrintStr(SPACE);
        }

        CLI_LIB_PrintStr("^\r\n");
    }
    else if (err_code == CLI_ERR_CMD_INVALID_RANGE_PLUS)
    {
        UI16_T  str_idx_temp = str_idx;
        UI16_T  cmd_len = strlen((char *)buff);
        /* 3 include ( ) # in the command list */
        disp_width = CLI_DEF_DISP_WIDTH - len - 3;
        while (cmd_len >= disp_width - 1)
        {
            /* Fix bug ES3628BT-FLF-ZZ-00048, refer to ES4827G-FLF-22-00194 */
            if ( str_idx_temp < 10 )
            {
                str_idx_temp = 0;
                break;
            }
            str_idx_temp = str_idx_temp - 10;  /* follow command shift 10 each time */
            cmd_len = cmd_len - 10;
        }

        for (i=0; i<str_idx_temp+len; i++) /* changed later for prompt length */
        {
            CLI_LIB_PrintStr(SPACE);
        }
        CLI_LIB_PrintStr("^\r\n");
    }

#if (SYS_CPNT_EH == TRUE)
    switch(err_code)
    {
    case CLI_NO_ERROR:
       break;

    case CLI_USE_DEFAULT:
       break;

    case CLI_ERR_CMD_INCOMPLETE:
       EH_MGR_Handle_Exception(SYS_MODULE_CLI, CLI_LIB_SHOW_ERR_MSG, EH_TYPE_MSG_CMD_INCOMPLETE, SYSLOG_LEVEL_INFO);
       EH_MGR_Get_Exception_Info (&module_id, &function_no, &msg_flag, msg, sizeof(msg));
       /* to know whether this message is a debug message, implement as this. */
       if(EH_MGR_IS_FOR_DEBUG_MSG_PURPOSE(msg_flag))
       {
          /*debug msg do not show*/;
       }
       else
       {
          CLI_LIB_PrintStr(msg);
       }
       break;

    case CLI_ERR_CMD_INVALID:
       EH_MGR_Handle_Exception(SYS_MODULE_CLI, CLI_LIB_SHOW_ERR_MSG, EH_TYPE_MSG_CMD_INPUT_INVALID, SYSLOG_LEVEL_INFO);
       EH_MGR_Get_Exception_Info (&module_id, &function_no, &msg_flag, msg, sizeof(msg));
       /* to know whether this message is a debug message, implement as this. */
       if(EH_MGR_IS_FOR_DEBUG_MSG_PURPOSE(msg_flag))
       {
          /*debug msg do not show*/;
       }
       else
       {
          CLI_LIB_PrintStr(msg);
       }
       break;

    case CLI_ERR_CMD_AMBIGUOUS:
       {
          char ambiguous[CLI_DEF_MAX_MSGNUM] = {0};

          strcat(ambiguous,"\"");
          strcat(ambiguous, (char *)buff);
          strcat(ambiguous,"\"");
          EH_MGR_Handle_Exception1(SYS_MODULE_CLI, CLI_LIB_SHOW_ERR_MSG, EH_TYPE_MSG_CMD_AMBIGUOUS, SYSLOG_LEVEL_INFO, ambiguous);
          EH_MGR_Get_Exception_Info (&module_id, &function_no, &msg_flag, msg, sizeof(msg));
          /* to know whether this message is a debug message, implement as this. */
          if(EH_MGR_IS_FOR_DEBUG_MSG_PURPOSE(msg_flag))
          {
             /*debug msg do not show*/;
          }
          else
          {
             CLI_LIB_PrintStr(msg);
          }
       }
       break;

    case CLI_ERR_CMD_UNRECOGNIZED:
       EH_MGR_Handle_Exception(SYS_MODULE_CLI, CLI_LIB_SHOW_ERR_MSG, EH_TYPE_MSG_CMD_UNRECOGNIIZED, SYSLOG_LEVEL_INFO);
       EH_MGR_Get_Exception_Info (&module_id, &function_no, &msg_flag, msg, sizeof(msg));
       /* to know whether this message is a debug message, implement as this. */
       if(EH_MGR_IS_FOR_DEBUG_MSG_PURPOSE(msg_flag))
       {
          /*debug msg do not show*/;
       }
       else
       {
          CLI_LIB_PrintStr(msg);
       }
       break;

    case CLI_ERR_CMD_SHOW_QUMARK:
       EH_MGR_Handle_Exception(SYS_MODULE_CLI, CLI_LIB_SHOW_ERR_MSG, EH_TYPE_MSG_HELP, SYSLOG_LEVEL_INFO);
       EH_MGR_Get_Exception_Info (&module_id, &function_no, &msg_flag, msg, sizeof(msg));
       /* to know whether this message is a debug message, implement as this. */
       if(EH_MGR_IS_FOR_DEBUG_MSG_PURPOSE(msg_flag))
       {
          /*debug msg do not show*/;
       }
       else
       {
          CLI_LIB_PrintStr(msg);
       }
       break;

    case CLI_ERR_CMD_INVALID_RANGE:
       EH_MGR_Handle_Exception(SYS_MODULE_CLI, CLI_LIB_SHOW_ERR_MSG, EH_TYPE_MSG_CMD_INV_PARAM, SYSLOG_LEVEL_INFO);
       EH_MGR_Get_Exception_Info (&module_id, &function_no, &msg_flag, msg, sizeof(msg));
       /* to know whether this message is a debug message, implement as this. */
       if(EH_MGR_IS_FOR_DEBUG_MSG_PURPOSE(msg_flag))
       {
          /*debug msg do not show*/;
       }
       else
       {
          CLI_LIB_PrintStr(msg);
       }
       break;

    case CLI_ERR_CMD_INVALID_RANGE_PLUS:
       EH_MGR_Handle_Exception(SYS_MODULE_CLI, CLI_LIB_SHOW_ERR_MSG, EH_TYPE_MSG_CMD_INV_PARAM, SYSLOG_LEVEL_INFO);
       EH_MGR_Get_Exception_Info (&module_id, &function_no, &msg_flag, msg, sizeof(msg));
       /* to know whether this message is a debug message, implement as this. */
       if(EH_MGR_IS_FOR_DEBUG_MSG_PURPOSE(msg_flag))
       {
          /*debug msg do not show*/;
       }
       else
       {
          CLI_LIB_PrintStr(msg);
       }
       break;


    case CLI_ERR_CMD_NOT_IMPLEMENT:
       break;

    case CLI_ERR_CMD_BYPASS_MATCHED:
       break;

    case CLI_ERR_INFINITE_LOOP:
       break;

    case CLII_ERR_MEMORY_NOT_ENOUGH:
       EH_MGR_Handle_Exception(SYS_MODULE_CLI, CLI_LIB_SHOW_ERR_MSG, EH_TYPE_MSG_MEM_ALLOCATE, SYSLOG_LEVEL_INFO);
       EH_MGR_Get_Exception_Info (&module_id, &function_no, &msg_flag, msg, sizeof(msg));
       /* to know whether this message is a debug message, implement as this. */
       if(EH_MGR_IS_FOR_DEBUG_MSG_PURPOSE(msg_flag))
       {
          /*debug msg do not show*/;
       }
       else
       {
          CLI_LIB_PrintStr(msg);
       }
       break;

    case CLI_ERR_TIMEOUT:
       EH_MGR_Handle_Exception(SYS_MODULE_CLI, CLI_LIB_SHOW_ERR_MSG, EH_TYPE_MSG_TIMEOUT, SYSLOG_LEVEL_INFO);
       EH_MGR_Get_Exception_Info (&module_id, &function_no, &msg_flag, msg, sizeof(msg));
       /* to know whether this message is a debug message, implement as this. */
       if(EH_MGR_IS_FOR_DEBUG_MSG_PURPOSE(msg_flag))
       {
          /*debug msg do not show*/;
       }
       else
       {
          CLI_LIB_PrintStr(msg);
       }
       break;

    case CLI_ERR_INTERNAL:
       EH_MGR_Handle_Exception(SYS_MODULE_CLI, CLI_LIB_SHOW_ERR_MSG, EH_TYPE_MSG_DEB_MSG, SYSLOG_LEVEL_ERR);
       EH_MGR_Get_Exception_Info (&module_id, &function_no, &msg_flag, msg, sizeof(msg));
       /* to know whether this message is a debug message, implement as this. */
       if(EH_MGR_IS_FOR_DEBUG_MSG_PURPOSE(msg_flag))
       {
          /*debug msg do not show*/;
       }
       else
       {
          CLI_LIB_PrintStr(msg);
       }
       break;

    case CLI_EXIT_SESSION:
       break;

    default:
       break;
    }


#else
    CLI_LIB_PrintStr("% ");
    CLI_LIB_PrintStr((char *)Cli_ErrMsg[err_code]);
    if ((err_code == CLI_ERR_CMD_AMBIGUOUS) || (err_code == CLI_USE_DEFAULT) || (err_code == CLI_ERR_INFINITE_LOOP))
    {
        CLI_LIB_PrintStr("\"");
        CLI_LIB_PrintStr(buff);
        CLI_LIB_PrintStr("\"");
    }
#endif


    CLI_LIB_PrintNullStr(1);
}

typedef struct
{
    char *first;
    char *second;
} CLI_DisplayMessage_T;

static int
CLI_LIB_DisplayMessageCompare(const void *v1,
                              const void *v2)
{
    const CLI_DisplayMessage_T *d1 = (const CLI_DisplayMessage_T*)v1;
    const CLI_DisplayMessage_T *d2 = (const CLI_DisplayMessage_T*)v2;

    return strcmp(d1->first, d2->first);
}

/**************************************************************************/
/* Show help message about '?', 'command ?', and 'command keyword ?'      */
/*                                                                        */
/**************************************************************************/

void CLI_LIB_ShowQuHelp(UI16_T cmd_idx, UI16_T arg_idx, UI16_T final)
{
    enum
    {
        MAX_STRING_LENGTH_OF_POOL = 512
    };

    typedef struct
    {
        char *pool;
        char *str;
        UI32_T capacity;
        UI32_T length;
    } StringPool;

    char **firstCln = NULL;
    char **secondCln = NULL;

    /* cjchu 2001/9/3 UI8_T   idx; */
    UI16_T  idx;
    UI16_T  msg_num;
    UI16_T  line_num;
    UI16_T  j;
    char    name_string[16] = {0};
    char    help_string[32] = {0};
    const CMDLIST *ui_cmd_lst;
    BOOL_T  arg_flag = FALSE;     /* cjchu 2001/8/31 */

    StringPool string_pool;

    ui_cmd_lst = CLI_LIB_Get_Cmd_List();
    msg_num = 0;

    if( (firstCln = calloc(CLI_DEF_MAX_MSGNUM, sizeof(char *))) == NULL )
    {
        return;
    }

    if( (secondCln = calloc(CLI_DEF_MAX_MSGNUM, sizeof(char *))) == NULL)
    {
        free(firstCln);
        return;
    }

    if( (string_pool.pool = calloc(MAX_STRING_LENGTH_OF_POOL, sizeof(char))) == NULL)
    {
        free(firstCln);
        free(secondCln);
        return;
    }

    string_pool.str = string_pool.pool;

    string_pool.capacity = MAX_STRING_LENGTH_OF_POOL;
    string_pool.length   = 0;

    /*
     * Assign firstCln[] and secondCln[] string value
     */
    idx = CLI_PARS_GetCommandIndex(ui_cmd_lst, cmd_idx);

    while (idx != NULL_CMD)
    {
        /* Check user privilege mode, in guest mode skip privilege level
         * command
         */
        /*jingyan zheng defect ES3628BT-FLF-ZZ-00066*/
        /*protect buffer is not enough*/
        if(msg_num >= (CLI_DEF_MAX_MSGNUM - 1))
        {
            firstCln[msg_num] = 0;
            secondCln[msg_num] = 0;
            break;
        }
        firstCln[msg_num] = ui_cmd_lst[idx].name;
        secondCln[msg_num] = (char *)ui_HelpMsg[ui_cmd_lst[idx].help_idx];

        msg_num++;

        idx = CLI_PARS_GetNeiborCommandIndex(ui_cmd_lst, idx);
    }

    idx = arg_idx;

    if (idx != NULL_ARG)
        arg_flag = TRUE;

    while (idx != NULL_ARG)
    {
        /*jingyan zheng defect ES3628BT-FLF-ZZ-00066*/
        /*protect buffer is not enough*/
        if(msg_num >= (CLI_DEF_MAX_MSGNUM - 1 ))
        {
            firstCln[msg_num] = 0;
            secondCln[msg_num] = 0;
            break;
        }
        switch(ui_arg_lst[idx].type)
        {
            case MODE_DATA:
            {
                enum CLI_ACCESS_MODE_E mode;

                for (mode=0; mode < CLI_ACCESS_MODE_MAX_NUM; ++mode)
                {
                    char * mode_str = NULL;
                    char * help_msg = NULL;

                    if (msg_num >= (CLI_DEF_MAX_MSGNUM - 1 ))
                    {
                        firstCln[msg_num] = 0;
                        secondCln[msg_num] = 0;
                        break;
                    }

#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
                    if (NORMAL_EXEC_MODE == mode)
                    {
                        continue;
                    }
#endif

                    if (FALSE == CLI_LIB_GetCommandModeStringPtr(mode, &mode_str, &help_msg))
                    {
                        continue;
                    }

                    firstCln[msg_num] = mode_str;
                    secondCln[msg_num] = help_msg;

                    msg_num ++;
                }
                break;
            }

            case IPV6_ADDR_DATA:
                if (string_pool.length < string_pool.capacity)
                {
                    SYSFUN_Snprintf(string_pool.str,
                                    string_pool.capacity - string_pool.length,
                                    "X:X:X:X::X");
                    string_pool.str[string_pool.capacity - string_pool.length - 1] = '\0';

                    firstCln[msg_num]  = string_pool.str;
                    secondCln[msg_num] = (char *)ui_HelpMsg[ui_arg_lst[idx].help_idx];
                    msg_num ++;

                    string_pool.str += strlen(string_pool.str) +1;
                    string_pool.length = strlen(string_pool.str) +1;
                }
                break;

            case IPV6_PREFIX_DATA:
                if (string_pool.length < string_pool.capacity)
                {
                    SYSFUN_Snprintf(string_pool.str,
                                    string_pool.capacity - string_pool.length,
                                    "X:X:X:X::X/<%lu-%lu>",
                                    ui_arg_lst[idx].min_range,
                                    ui_arg_lst[idx].max_range);
                    string_pool.str[string_pool.capacity - string_pool.length - 1] = '\0';

                    firstCln[msg_num]  = string_pool.str;
                    secondCln[msg_num] = (char *)ui_HelpMsg[ui_arg_lst[idx].help_idx];
                    msg_num ++;

                    string_pool.str += strlen(string_pool.str) +1;
                    string_pool.length = strlen(string_pool.str) +1;
                }
                break;

            case IPV6_ADDR_DATA_WITH_SCOPE_ID:
                if (string_pool.length < string_pool.capacity)
                {
                    SYSFUN_Snprintf(string_pool.str,
                                    string_pool.capacity - string_pool.length,
                                    "X:X:X:X::X%%<%lu-%lu>",
                                    ui_arg_lst[idx].min_range,
                                    ui_arg_lst[idx].max_range);
                    string_pool.str[string_pool.capacity - string_pool.length - 1] = '\0';

                    firstCln[msg_num]  = string_pool.str;
                    secondCln[msg_num] = (char *)ui_HelpMsg[ui_arg_lst[idx].help_idx];
                    msg_num ++;

                    string_pool.str += strlen(string_pool.str) +1;
                    string_pool.length = strlen(string_pool.str) +1;
                }
                break;

            case WORD_DATA:
            case RANGE_DATA:
                if (string_pool.length < string_pool.capacity)
                {
                    SYSFUN_Snprintf(string_pool.str,
                                    string_pool.capacity - string_pool.length,
                                    "<%lu-%lu>",
                                    ui_arg_lst[idx].min_range,
                                    ui_arg_lst[idx].max_range);
                    string_pool.str[string_pool.capacity - string_pool.length - 1] = '\0';

                    firstCln[msg_num]  = string_pool.str;
                    secondCln[msg_num] = (char *)ui_HelpMsg[ui_arg_lst[idx].help_idx];
                    msg_num ++;

                    string_pool.str += strlen(string_pool.str) +1;
                    string_pool.length = strlen(string_pool.str) +1;
                }
                break;

            case HEX_DATA:
                if (string_pool.length < string_pool.capacity)
                {
                    SYSFUN_Snprintf(string_pool.str,
                                    string_pool.capacity - string_pool.length,
                                    "<%lx-%lx>",
                                    ui_arg_lst[idx].min_range,
                                    ui_arg_lst[idx].max_range);
                    string_pool.str[string_pool.capacity - string_pool.length - 1] = '\0';

                    firstCln[msg_num]  = string_pool.str;
                    secondCln[msg_num] = (char *)ui_HelpMsg[ui_arg_lst[idx].help_idx];
                    msg_num ++;

                    string_pool.str += strlen(string_pool.str) +1;
                    string_pool.length = strlen(string_pool.str) +1;
                }
                break;

            case LIST_NO_UNIT_DATA:
                if (string_pool.length < string_pool.capacity)
                {
                    SYSFUN_Snprintf(string_pool.str,
                                    string_pool.capacity - string_pool.length,
                                    "<%lu-%lu>",
                                    ui_arg_lst[idx].min_range,
                                    ui_arg_lst[idx].max_range);
                    string_pool.str[string_pool.capacity - string_pool.length - 1] = '\0';

                    firstCln[msg_num]  = string_pool.str;
                    secondCln[msg_num] = (char *)ui_HelpMsg[ui_arg_lst[idx].help_idx];
                    msg_num ++;

                    string_pool.str += strlen(string_pool.str) +1;
                    string_pool.length = strlen(string_pool.str) +1;
                }
                break;

            case UNIT_COLON_DATA:/*pttch stacking*/
                if (string_pool.length < string_pool.capacity)
                {
                    UI32_T num_of_unit;

                    STKTPLG_POM_GetNumberOfUnit(&num_of_unit);

                    SYSFUN_Snprintf(string_pool.str,
                                    string_pool.capacity - string_pool.length,
                                    "<1-%lu>:",
                                    num_of_unit);
                    string_pool.str[string_pool.capacity - string_pool.length - 1] = '\0';

                    firstCln[msg_num]  = string_pool.str;
                    secondCln[msg_num] = (char *)ui_HelpMsg[ui_arg_lst[idx].help_idx];
                    msg_num ++;

                    string_pool.str += strlen(string_pool.str) +1;
                    string_pool.length = strlen(string_pool.str) +1;
                }
                break;

                /* Show first part of the help message
                 */
            case LIST_HAVE_UNIT_DATA:
            case PVC_DATA:
                if (string_pool.length < string_pool.capacity)
                {
                    UI32_T max_value = 0;
                    UI32_T min_value = 0;

#if (SYS_CPNT_STACKING == TRUE)
                    UI32_T num_of_unit = 0;

                    STKTPLG_POM_GetNumberOfUnit(&num_of_unit);
                    if (num_of_unit == 1)
                    {
                        UI32_T my_unit_id = 0;

                        STKTPLG_POM_GetMyUnitID(&my_unit_id);
                        min_value = max_value = my_unit_id;
                    }
                    else
                    {
                        min_value = 1;
                        max_value = num_of_unit;
                    }
#else
                    min_value = max_value = 1;
#endif /* #if (SYS_CPNT_STACKING == TRUE) */

                    SYSFUN_Snprintf(string_pool.str,
                                    string_pool.capacity - string_pool.length,
                                    "<%lu-%lu>",
                                    min_value, max_value);
                    string_pool.str[string_pool.capacity - string_pool.length - 1] = '\0';

                    firstCln[msg_num]  = string_pool.str;

                    string_pool.str += strlen(string_pool.str) +1;
                    string_pool.length = strlen(string_pool.str) +1;

                    if (string_pool.length < string_pool.capacity)
                    {
                        SYSFUN_Snprintf(string_pool.str,
                                        string_pool.capacity - string_pool.length,
                                        "%s",
                                        "Unit number");
                        string_pool.str[string_pool.capacity - string_pool.length - 1] = '\0';

                        secondCln[msg_num] = string_pool.str;

                        string_pool.str += strlen(string_pool.str) +1;
                        string_pool.length = strlen(string_pool.str) +1;
                    }
                    else
                    {
                        secondCln[msg_num] = "Out of memory!";
                    }

                    msg_num ++;
                }
                break;

            case PORT_DATA:
            {
                char delimeter = 0;

                if (ui_arg_lst[idx].type == PORT_DATA)
                    delimeter = '.';
                else                /* if (ui_arg_lst[idx].type == PVC_DATA) */
                    delimeter = '/';

                for(j=0; j<sizeof(name_string)-1; j++)
                {
                    if(ui_arg_lst[idx].name[j]== delimeter)
                    {
                        name_string[j] = 0;
                        break;
                    }
                    else
                    {
                        name_string[j] = ui_arg_lst[idx].name[j];
                    }
                }

                for(j=0; j<sizeof(help_string)-1; j++)
                {
                    if(ui_HelpMsg[ui_arg_lst[idx].help_idx][j]== '#')
                    {
                        help_string[j] = 0;
                        break;
                    }
                    else
                    {
                        help_string[j] = ui_HelpMsg[ui_arg_lst[idx].help_idx][j];
                    }
                }
                firstCln[msg_num] = name_string;
                secondCln[msg_num] = help_string;
                msg_num++;
            }
                break;

            case BYPASS_DATA:
                idx = ui_arg_lst[idx].next_idx;
                continue; /*while*/

            default:

                if (string_pool.length < string_pool.capacity)
                {
                    int ret;

                    ret = CLI_PARS_GetArgumentHelpMessage(NULL, 0,
                                                          ui_arg_lst, idx,
                                                          string_pool.str,
                                                          string_pool.capacity - string_pool.length,
                                                          NULL, 0);

                    if (0 <= ret)
                    {
                        string_pool.str[string_pool.capacity - string_pool.length - 1] = '\0';

                        firstCln[msg_num]  = string_pool.str;

                        string_pool.str += strlen(string_pool.str) +1;
                        string_pool.length = strlen(string_pool.str) +1;
                    }
                    else
                    {
                        firstCln[msg_num] = ui_arg_lst[idx].name;
                    }
                }

                secondCln[msg_num] = (char *)ui_HelpMsg[ui_arg_lst[idx].help_idx];
                msg_num ++;

                break;
        }
        idx = ui_arg_lst[idx].neibor_idx;
    }

    /* Display command/argument help message
     */
    if ((cmd_idx != NULL_CMD) || (arg_idx != NULL_ARG))
    {
        /* Sort firstCln and secondCln
         */
        CLI_DisplayMessage_T *tmp_ = calloc(sizeof(CLI_DisplayMessage_T), msg_num);
        if (tmp_)
        {
            UI16_T i_;

            for (i_=0; i_<msg_num; ++i_)
            {
                tmp_[i_].first = firstCln[i_];
                tmp_[i_].second = secondCln[i_];
            }

            qsort(tmp_, msg_num, sizeof(CLI_DisplayMessage_T), CLI_LIB_DisplayMessageCompare);

            for (i_=0; i_<msg_num; ++i_)
            {
                firstCln[i_] = tmp_[i_].first;
                secondCln[i_] = tmp_[i_].second;
            }
            free(tmp_);
        }

        line_num = CLI_LIB_DisplayMsg(firstCln, secondCln, CLI_DEF_MAX_MSGNUM);
    }
    else
        line_num = 1;

    free(string_pool.pool);
    string_pool.pool = NULL;

    if (line_num == 255) /* Ctrl_C key to break */
    {
        free(firstCln);
        free(secondCln);
        return;
    }

    /* dralee 98.9.29 */

    if (final == 1)
        line_num = CLI_LIB_PrintStrLine("  <cr>",line_num);

    free(firstCln);
    free(secondCln);
}


/**************************************************************************/
/* Show help message about 'partial command word?' as 'sh pr?'            */
/*                                                                        */
/**************************************************************************/

void CLI_LIB_ShowQuType3Msg(char *words[], UI16_T cnt)
{
#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    char  disp_buf[CLI_DEF_DISP_WIDTH*2+1] = {0};
    char  buff[CLI_DEF_DISP_WIDTH+1] = {0};
#else
    char   disp_buf[CLI_DEF_DISP_WIDTH+1] = {0};
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    UI16_T max_len, len, disp_len;
    UI16_T term_width;
    UI16_T i, j;
    UI32_T line_num = 1;  /*Initialize line num, include one command line*/
#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    UI16_T k, len1, sidx, eidx;
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    /*
     * Find max string length of these words
     */
    max_len = 0;

    for (i=0; i<cnt; i++)
    {
        len = strlen((char *)words[i]);
        if (len > max_len)
            max_len = len;
    }

    max_len += 2;

    /*
     * Print out these words
     */
    disp_buf[0] = '\0';
    disp_len = 0;

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    term_width = ctrl_P->CMenu.width;
#else
    term_width = CLI_DEF_DISP_WIDTH;
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    for (i=0; i<cnt; i++)
    {
        len = strlen((char *)words[i]);

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
        /* can't be sure that first word is less than term_width
         */
        strcat(disp_buf, words[i]);
        disp_len += len;
        if (disp_len > term_width)
        {
            len1 = strlen(disp_buf);
            /* More than one line to display this message
             */
            sidx = 0;
            j=0;

            do
            {
               eidx = sidx + (term_width - 1);
               if (eidx > len1)
                  eidx = len1;

               while (  (disp_buf[eidx] != '\0') && (eidx>sidx) )
               {
                  eidx--;
               }

               if (eidx == sidx)
               {
                  /* Terminal width too short, print one word per line
                   */
                  while (  (disp_buf[eidx] != '\0')  && (eidx - sidx < term_width))
                  {
                     eidx++;
                  }
               }

               /* Process one line
                */
               for (k=sidx; k<eidx; k++)
               {
                  buff[k-sidx] = disp_buf[k];
               }
               buff[eidx-sidx] = '\0';
               line_num = CLI_LIB_PrintStrLine(buff, line_num);
               /*PROCESS_MORE_FUNC(buff);*/
               CLI_LIB_PrintNullStr(1);
               sidx = eidx;/*pttch add 920312*/

            } while ( (len1-eidx) > term_width );

            while(disp_buf[k]!= '\0')
            {
                disp_buf[j]=disp_buf[k];
                j++;
                k++;
            }
            disp_buf[j]='\0';
            j=0;
            disp_len = strlen(disp_buf);
        }/* if (disp_len > term_width)*/

        for (j=len; j<max_len; j++)
        {
            if (disp_len == term_width)
            {
                line_num = CLI_LIB_PrintStrLine(disp_buf, line_num);
                CLI_LIB_PrintNullStr(1);
                disp_buf[0]='\0';
                disp_len=0;
                break;
            }
            strcat(disp_buf, " ");
            disp_len++;
        }
#else /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

        if ((disp_len+len) > term_width)
        {
            line_num = CLI_LIB_PrintStrLine(disp_buf, line_num);
            CLI_LIB_PrintNullStr(1);
            disp_buf[0] = '\0';
            disp_len = 0;
        }

        strcat((char *)disp_buf, (char *)words[i]);
        disp_len += len;

        for (j=len; j<max_len; j++)
        {
            if (++disp_len >= term_width)
                break;

            strcat((char *)disp_buf, " ");
        }
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    } /* for */

    line_num = CLI_LIB_PrintStrLine(disp_buf, line_num);
    CLI_LIB_PrintNullStr(1);
}


/**************************************************************************/
/* CLI_LIB_DisplayMsg()                                                        */
/*                                                                        */
/* Description:                                                           */
/*      1. Combine the first column and the second column message to one  */
/*         message for showing on the screen.                             */
/*      2. When Second column is NULL, we only need to display first      */
/*         column messages.                                               */
/*      3. The terminal width for displaying the messages according to    */
/*         width parameter value                                          */
/*                                                                        */
/* Return:                                                                */
/*      Return total line number that had been displayed                  */
/**************************************************************************/
UI16_T CLI_LIB_DisplayMsg(char **firstColumn, char **secondColumn, UI32_T disp_line_num)
{
    char   buf[CLI_DEF_DISP_WIDTH+1];
    UI16_T  msg_num, line_num, max_len, len, len2, sidx, eidx , scnd_width, first_time, i;
    UI16_T width;

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();
    char   buf_tmp[CLI_DEF_MAX_BUFSIZE+1];
    width = ctrl_P->CMenu.width;
#else
    width = CLI_DEF_DISP_WIDTH;
#endif

   if (secondColumn[0] == NULL)
   {
      msg_num = 0;
      line_num = 1;  // Initialize line num, include one command line

      /* Display one column message
       */
      while ((firstColumn[msg_num] != NULL) && (msg_num < disp_line_num))
      {
         len = strlen((char *)firstColumn[msg_num]);

         /* Consider exceed the terminal width
          */
         if (len < width)
         {
            line_num = CLI_LIB_PrintStrLine(firstColumn[msg_num], line_num);
            if (line_num == CLI_LIB_PRINTSTRLINE_CTRL_C) /* for cancel (Ctrl_C key) */
            {
               return line_num;
            }
         }
         else
         {
         /* More than one line to display this message
          */
            sidx = 0;

            do
            {
               eidx = sidx + (width - 1);
               if (eidx > len)
                  eidx = len;

               while (  /*(firstColumn[msg_num][eidx] != ' ')&&*/ (firstColumn[msg_num][eidx] != '\0') && (eidx>sidx) )
               {
                  eidx--;
               }

               if (eidx == sidx)
               {
               /* Terminal width too short, print one word per line
                */
                  while ( /*(firstColumn[msg_num][eidx] != ' ')&&*/ (firstColumn[msg_num][eidx] != '\0')  && (eidx - sidx < CLI_DEF_DISP_WIDTH))
                  {
                     eidx++;
                  }
               }

               /* Process one line
                */
               for (i=sidx; i<eidx; i++)
               {
                  buf[i-sidx] = firstColumn[msg_num][i];
               }
               buf[eidx-sidx] = '\0';
               line_num = CLI_LIB_PrintStrLine(buf, line_num);

               if (line_num == CLI_LIB_PRINTSTRLINE_CTRL_C) /* for cancel (Ctrl_C key) */
               {
                  return (line_num);
               }
               sidx = eidx;/*pttch add 920312*/
            } while (firstColumn[msg_num][eidx] != '\0');
         }  /* if ... else */
         msg_num++;
      }  /* while */
   }
   else /* show two column message */
   {
      /* Find maximum string length of first column
       */
      max_len = 0;
      msg_num = 0;

      while (firstColumn[msg_num] != NULL)
      {
         len = strlen(firstColumn[msg_num]);
         if (len > max_len)
            max_len = len;
         msg_num++;
      }

      /* Display the messages(first column + second column)
       */
      msg_num = 0;
      line_num = 1;

      while ((firstColumn[msg_num] != NULL) && (msg_num < disp_line_num))
      {
      /* Copy first column string content
       */
         len = strlen((char *)firstColumn[msg_num]);
         buf[0] = buf[1] = ' ';

         for (i=0; i<len; i++)
         {
            buf[i+2] = firstColumn[msg_num][i];
         }

         for (i=len; i<max_len+2; i++)
         {
            buf[i+2] = ' ';
         }
#if (SYS_CPNT_CLI_TERMINAL == TRUE)
         buf[i+3] = '\0';

         len = strlen(buf);
         /* Consider exceed the terminal width
          */
         if (len < width)
         {
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

         /* Copy second column string content
          */
         sidx = 0;
         scnd_width = width - (max_len + 4);

         if (scnd_width <= 0)
            scnd_width = 1;
         first_time = 1;

         do
         {
         /* Process one line
          */
            if (first_time == 1)
            {
               first_time = 0;
            }
            else
            {
               for (i=0; i<max_len+4; i++)
                  buf[i] = ' ';
            }

            len2 = strlen((char *)secondColumn[msg_num]);

            /* Find string break index
             */
            eidx = sidx + (scnd_width - 1);

            if (eidx > len2)
               eidx = len2;

            while ( (secondColumn[msg_num][eidx] != ' ')&& (secondColumn[msg_num][eidx] != '\0') && (eidx > sidx) )
            {
               eidx--;
            }

            if (eidx == sidx)
            {
            /* Terminal width too short, print one word per line
             */
               while ( (secondColumn[msg_num][eidx] != ' ')&&  (secondColumn[msg_num][eidx] != '\0'))
               {
                  eidx++;
               }
            }  /* if */

            /* Copy second column string
             */
            for (i=sidx; i<eidx; i++)
            {
               buf[i-sidx+max_len+4] = secondColumn[msg_num][i];
            }

            buf[eidx-sidx+max_len+4] = '\0';

            line_num = CLI_LIB_PrintStrLine(buf, line_num);

            if (line_num == CLI_LIB_PRINTSTRLINE_CTRL_C) /* for cancel (Ctrl_C key) */
            {
               return (line_num);
            }
            sidx = eidx+1;
#if (SYS_CPNT_CLI_TERMINAL == TRUE)
                buf[0]='\0';
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

         } while (secondColumn[msg_num][eidx] != '\0');

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
         }
         else/*don't care format, width is less than help index word.*/
         {

             len = strlen(firstColumn[msg_num]);
             buf_tmp[0] = buf_tmp[1] = ' ';

             for (i=0; i<len; i++)
             {
                buf_tmp[i+2] = firstColumn[msg_num][i];
             }
             buf_tmp[len+2] = buf_tmp[len+3] = ' ';

             /*Copy second column string content*/
             len2 = strlen(secondColumn[msg_num]);
             for (i=0; i<len2; i++)
             {
                buf_tmp[len+4+i] = secondColumn[msg_num][i];
             }
             buf_tmp[len+4+i] = '\0';

             /* Display one column message
              */
             /* Consider exceed the terminal width
              */
             len = strlen(buf_tmp);

             /* More than one line to display this message
              */
             sidx = 0;

             do
             {
                eidx = sidx + (width - 1);
                if (eidx > len)
                   eidx = len;

                while (  /*(firstColumn[msg_num][eidx] != ' ')&&*/ (buf_tmp[eidx] != '\0') && (eidx>sidx) )
                {
                   eidx--;
                }

                if (eidx == sidx)
                {
                /* Terminal width too short, print one word per line
                 */
                   while ( /*(firstColumn[msg_num][eidx] != ' ')&&*/ (buf_tmp[eidx] != '\0')  && (eidx - sidx < width))
                   {
                      eidx++;
                   }
                }

                /* Process one line
                 */
                for (i=sidx; i<eidx; i++)
                {
                   buf[i-sidx] = buf_tmp[i];
                }
                buf[eidx-sidx] = '\0';
                line_num = CLI_LIB_PrintStrLine(buf, line_num);

                if (line_num == CLI_LIB_PRINTSTRLINE_CTRL_C) /* for cancel (Ctrl_C key) */
                {
                   return (line_num);
                }
                sidx = eidx;/*pttch add 920312*/
                buf[0]='\0';
             } while (buf_tmp[eidx] != '\0');

          } /*else- don't care */
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

         msg_num++;
       } /* while ((firstColumn[msg_num] != NULL) && (msg_num < disp_line_num))*/
    } /*else- secondColumn printf*/
  return line_num;
}


/* PURPOSE: Pause until user key in a character
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  0: Others
 *          1: SPACE
 *          2: ENTER
 *          3: CTR_C
 * NOTE:
 */
UI8_T CLI_LIB_WaitNextPage(void)
{
#define PRT_STR(X);                            \
    {                                          \
        int i;                                 \
        for (i=0; i<strlen("---More---"); i++) \
        CLI_LIB_PrintStr(X);                   \
    }

    char   key;
    UI16_T code;

    CLI_LIB_PrintStr("---More---");

    code = CLI_IO_GetKey(&key);

    PRT_STR(BACK);
    PRT_STR(SPACE);
    PRT_STR(BACK);

    switch (code)
    {
    case (CTRL_C):        /* Ctrl_C bar */
        return 3;

    case (ENTER):         /* Enter bar */
        return 2;

    case (PRNTABLE):
        if (key == 0x20)    /* Space bar */
            return 1;
        else
            return 0;        /* Other bar */

    default:              /* Other bar */
        return  0;
    }
}

/* PURPOSE: Display "---More---" until read a key to continue show the messages
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  0 => Other keys
 *          1 => Space bar
 *          2 => Enter key
 *          3 => Ctrl_C key
 * NOTE:
 */
UI16_T CLI_LIB_ProcMoreFeature()
{
#define MORE_PROMPT_STR "--- [Space] Next page, [Enter] Next line, [A] All, Others to exit ---"

    UI16_T  ret, c;
    char    ch;
    UI16_T  len, i;

   CLI_LIB_PrintStr(MORE_PROMPT_STR);
   len = strlen(MORE_PROMPT_STR);

    c = CLI_IO_GetKey(&ch); //cjchu 2001/6/12

    switch (c)
    {
    case (CTRL_C):        /* Ctrl_C bar */
        ret = 3;
        break;

    case (ENTER):         /* Enter bar */
        ret = 2;
        break;

    case (PRNTABLE):
        if (ch == 0x20)    /* Space bar */
            ret = 1;
        else if ( (ch == 'A') || (ch == 'a') )    /* A keystroke */
            ret = 4;        /* print without pause */
        else
            ret = 0;        /* Other bar */
        break;

    case KEYIN_ERROR:
        return CLI_EXIT_SESSION;

    default:              /* Other bar */
        ret = 0;
        break;
    }

    /* Clear original "---More---" line
     */
    for (i=0; i<len; i++)
        CLI_LIB_PrintStr(BACK);

    for (i=0; i<len; i++)
        CLI_LIB_PrintStr(SPACE);

    for (i=0; i<len; i++)
        CLI_LIB_PrintStr(BACK);

    return ret;
}

#pragma mark -
#pragma mark move from cli_api_show.c
BOOL_T CLI_API_Get_Print_Interactive_Mode()
{
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();
    if(ctrl_P == NULL)
        return FALSE;
    return ctrl_P->print_interactive;
}
void CLI_API_Set_Print_Interactive_Mode(BOOL_T mode)
{
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();

    ctrl_P->print_interactive = mode;
}
#pragma mark -

/* PURPOSE:
 * INPUT:
 * OUTPUT:
 * RETURN:
 * NOTE:
 */
UI16_T CLI_LIB_PrintStrLine(char *str, UI16_T line_num)
{
#define SCRATCH_BUF_LEN4  256
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();

    char  scratch[SCRATCH_BUF_LEN4];
    UI16_T ret_val, ret;

    UI32_T length=0;

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    length=ctrl_P->CMenu.length;
#else
    length=CLI_DEF_MAX_LINENUM;
#endif /*#if (SYS_CPNT_CLI_TERMINAL == TRUE)*/

    if( ctrl_P == NULL )
    {
        SYSFUN_Debug_Printf("\r\n%s(): Failed to get working area\r\n", __FUNCTION__);
        return 0;
    }

    /* Process ' --- More --- ' feature
     */
    ret_val = ++line_num;
    ret = 1;

    if(CLI_API_Get_Print_Interactive_Mode() == TRUE)
    {
        if (line_num >= (length-1))
        {
            ret = CLI_LIB_ProcMoreFeature();

            if (ret == 3)           /* Ctrl_C for cancel */
            {
                ret_val = CLI_LIB_PRINTSTRLINE_CTRL_C;  /* return 255 for cancel */
            }
            else if (ret == 2)      /* ENTER key */
            {
                ret_val = line_num-1;
            }
            else if (ret == 1)      /* Space bar */
            {
                ret_val = 1;
            }
            else if (ret == 4)      /* A keystroke */
            {
                ret_val = 4;
                CLI_API_Set_Print_Interactive_Mode(FALSE);
            }
            else
            {
                ret_val = CLI_LIB_PRINTSTRLINE_CTRL_C;
            }
        }
    }
    else
    {
        /* stopless printing */
    }

    /* Display one string line
     */
    if ((ret == 1) || (ret == 2))
    {
        SYSFUN_Sprintf((char *)scratch,"%s\r\n", str);
        strcat((char *)ctrl_P->send_buf, (char *)scratch);
        CLI_IO_PrintOut(ctrl_P);
    }
    return ret_val;
}


/***********************************************************************
 * CLI_LIB_CheckLineData   dralee 98.11.8 for support quat mark
 *
 *     Description:  check LINE_DATA type
 *
 *      return : CLI_NO_ERROR
 ***********************************************************************/

UI16_T CLI_LIB_CheckLineData(char *token, UI16_T *err_idx)
{
    UI16_T quat_flag=0;
    *err_idx = 0;

    if( *token == '"')
        quat_flag = 1;

    token++;
    (*err_idx)++;

    while(*token != '\0' )
    {
        if( quat_flag == 2 )
        {
            return CLI_ERR_CMD_INVALID;
        }
        else if( *token == ' ' && quat_flag == 0 )
        {
            do
            {
                (*err_idx)++;
                token++;
            } while (*token == ' ');

            if( *token == '\0')
                return CLI_NO_ERROR;
            else
                return CLI_ERR_CMD_INVALID;
        }
        else if( *token == '"' && *(token-1) == '\\')
        {
            ;
        }
        else if( *token == '"' && quat_flag == 1)
        {
            quat_flag = 2;
        }

        token++;
        (*err_idx)++;
    }
    return CLI_NO_ERROR;
}


/************************************************************************/
/* CLI_LIB_CheckNonData                                                      */
/*                                                                      */
/* Description :                                                        */
/*      Check NO_DATA type string                                       */
/*                                                                      */
/* Return: CLI_NO_ERROR          => success                             */
/*         CLI_ERR_CMD_INVALID   => invalid argument string             */
/*         CLI_ERR_CMD_AMBIGUOUS => ambiguous argument string           */
/************************************************************************/
UI16_T CLI_LIB_CheckNonData(char *token, UI16_T *arg_idx, UI16_T *match_idx)
{
    I16_T  len1;
    I16_T  cmp_flag;
    UI16_T match_num, match_num1;
    UI16_T next_idx;
    char   s[CLI_DEF_MAX_BUFSIZE];
    I16_T  i;
    UI16_T max_match_idx=0;

    i=1;
    strcpy(s, token);

    while( s[i] != '\0' && s[i] != '/' )
        i++;

    s[i] = '\0';


    len1 = strlen(s);

    while (1)
    {
        if(ui_arg_lst[*arg_idx].type == BYPASS_DATA )
        {
            /* dralee 98.11.8 */
            *match_idx = max_match_idx;

            return CLI_ERR_CMD_BYPASS_MATCHED;
        }

        cmp_flag = CLI_LIB_StrCmp(s, ui_arg_lst[*arg_idx].name, &match_num);

        /* dralee 98.10.8 for return final match index of arg parsing */
        *match_idx = match_num;

        if (cmp_flag == 0)
        {
            return CLI_NO_ERROR;
        }
        else
        {
            if (match_num == len1)
            {
                next_idx = ui_arg_lst[*arg_idx].neibor_idx;
                if (next_idx != NULL_ARG)
                {
                    CLI_LIB_StrCmp(s, ui_arg_lst[next_idx].name, &match_num1);

                    if (match_num1 == len1)
                        return CLI_ERR_CMD_AMBIGUOUS;
                    else
                        return CLI_NO_ERROR;

                }
                else // found abbreviation token
                {
                    return CLI_NO_ERROR;
                }
            }
            else
            {
                if (ui_arg_lst[*arg_idx].neibor_idx != NULL_ARG)
                {
                    *arg_idx = ui_arg_lst[*arg_idx].neibor_idx;

                    /* dralee 98.11.8 for solving "terminal" index error */
                    if( match_num > max_match_idx)
                        max_match_idx = match_num;

                }
                else
                    return CLI_ERR_CMD_INVALID;
            }
        }

    } /* while */
}


/************************************************************************/
/* CLI_LIB_CheckWordData                                                     */
/*                                                                      */
/* Description :                                                        */
/*      Check WORD_DATA type string to see if the value range is valid  */
/*                                                                      */
/* Return: 0 => success                                                 */
/*         1 => fail, value range is invalid                            */
/*         2 => fail, type is not matched                               */
/************************************************************************/

/* cjchu 2001/9/3 UI16_T CLI_LIB_CheckWordData(UI8_T *token, UI8_T arg_idx)*/
UI16_T CLI_LIB_CheckWordData(char *token, UI16_T arg_idx)
{
    UI32_T  ival;
    UI16_T i;
    char s[CLI_DEF_MAX_BUFSIZE];

    /* dralee 98.8.15 */

    i=1;
    strcpy(s, token);

    while( s[i] != '\0' && s[i] != '/' )
        i++;

    s[i] = '\0';


    /*
     * Check data type
     */
    i = 0;

    while (s[i] != '\0')
    {
        if (!(s[i] >= '0' && s[i] <= '9'))
            return 2;
        i++;
    }

    /*
     * Check value range
     */
    ival = CLI_LIB_AtoUl(s, 10);

#ifdef UI_BAUDRATE_SUPPORT
    if (arg_idx == ARG_W1_BAUD_RATE)
    {
        if (uart_LookupBaudTbl(ival) == ERROR)
            return 1;
    }
    else
    {
        if ((ival < 1) || (ival > ui_arg_lst[arg_idx].max_range))
            return 1;
    }
#else

    if (ival > ui_arg_lst[arg_idx].max_range)
        return 1;
#endif

    return 0;
}

#endif
#pragma mark #endif
#pragma mark -

/************************************************************************/
/* CLI_LIB_CheckIPAddr                                                       */
/*                                                                      */
/* Description :                                                        */
/*      Check IP_ADDR_DATA type string, format be as (xxx.xxx.xxx.xxx)  */
/*                                                                      */
/* Return: 0 => success.                                                */
/*         1 => fail, with *idx refer to invalid place(string index)    */
/************************************************************************/

UI16_T CLI_LIB_CheckIPAddr(char *tkn_buf, UI16_T *idx)
{
    char token[5];
    int  i,j;  /* i for s[]; j for token[] */
    int  k;    /* k for ip[] */
    char s[CLI_DEF_MAX_BUFSIZE];

    /* dralee 98.8.15 */
    i=1;
    strcpy(s, tkn_buf);

    while( s[i] != '\0' && s[i] != '/' )
        i++;

    s[i] = '\0';

    i = 0;
    j = 0;
    k = 0;
    *idx = 0;

    while (s[i] != '\0')
    {
        if (s[i] == '.')
        {
            token[j] = '\0';
            if (strlen(token) < 1 || strlen(token) > 3 ||
                atoi(token) < 0 || atoi(token) > 255)
            {
                return 1;
            }
            else if (k >= 3)      /* too long */
            {
                *idx = i;
                return 1;
            }
            i++; j = 0;
            k++;
            *idx = i;
        }
        else if (!(s[i] >= '0' && s[i] <= '9'))
        {
            return 1;
        }
        else
        {
            if (j >= 3) return 1;
            token[j++] = s[i++];
        }

    } /* while */

    token[j] = '\0';

    if (k != 3)         /* too short */
    {
        *idx = i;
        return 1;
    }

    if (strlen(token) < 1 || strlen(token) > 3 ||
        atoi(token) < 0 || atoi(token) > 255)
    {
        return 1;
    }

    return 0;
}


/*****************************************/
/* Convert string 's' to unsigned long,  */
/* with 'radix' as base.                 */
/*****************************************/

UI32_T CLI_LIB_AtoUl(char *s, int radix)
{
    int i;
    unsigned long n = 0;

    for (i = 0; s[i] == ' ' || s[i] == '\n' || s[i] == '\t'; i++)
        ;       /* skip white space */

    if (s[i] == '+' || s[i] == '-')
    {
        i++;    /* skip sign */
    }

    if (radix == 10)
    {
        for (n = 0; s[i] >= '0' && s[i] <= '9'; i++)
        {
            n = 10 * n + s[i] - '0';
        }
    }
    else if (radix == 16)
    {
        if ( (s[i] == '0') && (s[i+1] == 'x' || s[i+1] == 'X') ) // Charles,
           i=i+2;                                                // To skip the "0x" or "0X"


        for (n = 0;
            (s[i] >= '0' && s[i] <= '9') ||
            (s[i] >= 'A' && s[i] <= 'F') ||
            (s[i] >= 'a' && s[i] <= 'f');
            i++)
        {
            if (s[i] >= '0' && s[i] <= '9')
            {
                n = 16 * n + s[i] - '0';
            }
            else if (s[i] >= 'A' && s[i] <= 'F')
            {
                n = 16 * n + s[i] - 'A'+ 10;
            }
            else if (s[i] >= 'a' && s[i] <= 'f')
            {
                n = 16 * n + s[i] - 'a'+ 10;
            }
        }
    }
    return (n);
}

/*****************************************/
/* Convert string 's' to signed long,  */
/* with 'radix' as base.                 */
/*****************************************/
I32_T CLI_LIB_Atol(char *s, int radix)
{
    int i, sign_neg=0;
    long n = 0;

    for (i = 0; s[i] == ' ' || s[i] == '\n' || s[i] == '\t'; i++)
        ;       /* skip white space */

    if (s[i] == '-')
    {
        i++;    /* skip sign */
        sign_neg = 1;
    }

    if (radix == 10)
    {
        for (n = 0; s[i] >= '0' && s[i] <= '9'; i++)
        {
            n = 10 * n + s[i] - '0';
        }
    }
    else if (radix == 16)
    {
        if ( (s[i] == '0') && (s[i+1] == 'x' || s[i+1] == 'X') ) // Charles,
           i=i+2;                                                // To skip the "0x" or "0X"


        for (n = 0;
            (s[i] >= '0' && s[i] <= '9') ||
            (s[i] >= 'A' && s[i] <= 'F') ||
            (s[i] >= 'a' && s[i] <= 'f');
            i++)
        {
            if (s[i] >= '0' && s[i] <= '9')
            {
                n = 16 * n + s[i] - '0';
            }
            else if (s[i] >= 'A' && s[i] <= 'F')
            {
                n = 16 * n + s[i] - 'A'+ 10;
            }
            else if (s[i] >= 'a' && s[i] <= 'f')
            {
                n = 16 * n + s[i] - 'a'+ 10;
            }
        }
    }

    if(sign_neg)
        return (0-n);
    else
        return n;
}


/*********************************************************************/
/* Convert string s (xxx.xxx.xxx.xxx) to IP address (4-byte object). */
/*                                                                   */
/* Return: 1 => success, result put in ip                            */
/*         0 => fail, content of ip undetermined                     */
/*********************************************************************/

int CLI_LIB_AtoIp(char *s, UI8_T *ip)
{
    char  token[20];
    int   i,j;  /* i for s[]; j for token[] */
    int   k;    /* k for ip[] */

    UI8_T temp[4];

    i = 0;
    j = 0;
    k = 0;

    while (s[i] != '\0')
    {
        if (s[i] == '.')
        {
            token[j] = '\0';
            if (strlen(token) < 1 || strlen(token) > 3 ||
                atoi(token) < 0 || atoi(token) > 255)
            {
                return 0;
            }
            else if (k >= 4)
            {
                return 0;
            }
            else
            {
                temp[k++] =(UI8_T)atoi(token);
                i++; j = 0;
            }
        }
        else if (!(s[i] >= '0' && s[i] <= '9'))
        {
            return 0;
        }
        else
        {
            token[j++] = s[i++];
        }

    } /* while */

    token[j] = '\0';
    if (strlen(token) < 1 || strlen(token) > 3 ||
        atoi(token) < 0 || atoi(token) > 255)
    {
        return 0;
    }
    else if (k != 3)
    {
        return 0;
    }

    temp[k]=(UI8_T)atoi(token);

/*    ip[0] = temp[3];
    ip[1] = temp[2];
    ip[2] = temp[1];
    ip[3] = temp[0];*/

    ip[0] = temp[0];
    ip[1] = temp[1];
    ip[2] = temp[2];
    ip[3] = temp[3];

    return 1;
}


/*****************************************************************************/
/* Convert string s (xx-xx-xx-xx-xx-xx) to Ethernet address (6-byte object). */
/*                                                                           */
/* Return: 1 => success, result put in en                                    */
/*         0 => fail, content of en undetermined                             */
/*****************************************************************************/
int CLI_LIB_AtoEa(char *s, char *en)
{
    char token[20];
    int  i,j;  /* i for s[]; j for token[] */
    int  k;    /* k for en[] */

    i = 0;
    j = 0;
    k = 0;

    while (s[i] != '\0')
    {
        if (s[i] == '-')
        {
            token[j] = '\0';
            if (strlen((char *)token) != 2)
            {
                return 0;
            }
            else if (k >= 6)
            {
                return 0;
            }
            else
            {
                en[k++] = (char)CLI_LIB_AtoUl(token,16);
                i++; j = 0;
            }
        }
        else if (!(s[i] >= '0' && s[i] <= '9') &&
                 !(s[i] >= 'A' && s[i] <= 'F') &&
                 !(s[i] >= 'a' && s[i] <= 'f'))
        {
            return 0;
        }
        else
        {
            token[j++] = s[i++];
        }

    } /* while */

    token[j] = '\0';
    if (strlen((char *)token) != 2)
    {
        return 0;
    }
    else if (k != 5)
    {
        return 0;
    }

    en[k] = (char)CLI_LIB_AtoUl(token,16);

    return 1;
}


/******************************/
/* Reverse string s in place  */
/******************************/

char* CLI_LIB_ReverseStr(char *s)
{
    int c, i, j;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
    return (s);
}


/**************************************************/
/* Convert unsigned long n to characters in s     */
/**************************************************/

void CLI_LIB_UltoA(UI32_T n, char *s)
{
    UI16_T i = 0;

    do        /* generate digits in reverse order */
    {
        s[i++] = n % 10 + '0';    /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    s[i] = '\0';

    CLI_LIB_ReverseStr(s);
}


/****************************************************/
/* Reverse byte order of a 4-byte IP address object */
/****************************************************/

void CLI_LIB_Reverse4Byte(UI8_T *ip_buf)
{
    int  i;
    UI8_T c;

    for (i = 0; i < 2; i++)
    {
        c = ip_buf[i];
        ip_buf[i] = ip_buf[3-i];
        ip_buf[3-i] = c;
    }
}




/* dralee 98.9.26 */
/************************************************************************/
/* CLI_LIB_CheckHexData                                                      */
/*                                                                      */
/* Description :                                                        */
/*      Check Hex data  type string, format be as (0xXXXXX)             */
/*                                                                      */
/* Return: 0 => success.                                                */
/*         1 => fail, with *idx refer to invalid place(string index)    */
/************************************************************************/

UI16_T CLI_LIB_CheckHexData(UI8_T *tkn_buf, UI16_T *idx)
{
    UI8_T len,i;

    *idx = 0;
    if((len = strlen((char *)tkn_buf)) == 0 )
        return 1;

    if(*tkn_buf != '0')
    {
        *idx = 0;
        return 1;
    }

    if( (*(tkn_buf+1) != 'x' && *(tkn_buf+1) != 'X'))
    {
        *idx = 1;
        return 1;
    }

    if( len == 2 ) /* 0x */
    {
        *idx = 2;
        return 1;
    }

    *idx = 1; /* point at 'x' */

    for(i=2;i<len;i++)
    {
        if(!( (*(tkn_buf+i) <= '9' && *(tkn_buf+i) >= '0') ||
            (*(tkn_buf+i) <= 'F' && *(tkn_buf+i) >= 'A') ||
            (*(tkn_buf+i) <= 'f' && *(tkn_buf+i) >= 'a') ))
        {
            *idx = i;
            return 1;
        }
    }

    return 0;
}


/* PURPOSE: Transform 0x?????? data to UI32_T decmal
 * INPUT:   hexstr: Character string of HEX, i.e. 0x????
 * OUTPUT:  data: Decimal value
 * RETURN:  0=> Valid input
 *          1=> Invalid input
 * NOTE:
 */
UI8_T CLI_LIB_HextoUI32_T(char *hexstr, UI32_T *data)
{
    UI8_T   len;
    UI8_T   i;
    UI32_T   onedigit;

    *data = 0;
    len = strlen(hexstr);

    if( len <= 2 && (*(hexstr+1) != 'x' && *(hexstr+1) != 'X'))   /* 0x????? */
        return 1;

    for(i=2; i<len; i++)
    {
        if( *(hexstr+i) <= '9' &&  *(hexstr+i) >= '0')
            onedigit = *(hexstr+i) - 48;
        else if ( *(hexstr+i) <= 'F' &&  *(hexstr+i) >= 'A')
            onedigit = *(hexstr+i) - 65+10;
        else if ( *(hexstr+i) <= 'f' &&  *(hexstr+i) >= 'a')
            onedigit = *(hexstr+i) - 97+10;
        else
            return 1;

        *data = ( (*data) << 4 ) + onedigit;
    }
    return 0;
}


#pragma mark -
#pragma mark #if 0
#if 0


/* PURPOSE: Get command table in each mode
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  Command table
 * NOTE:
 */
const CMDLIST *CLI_LIB_Get_Cmd_List()
{
    CLI_TASK_WorkingArea_T *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();

    if( ctrl_P == NULL )
    {
        SYSFUN_Debug_Printf("\r\n%s(): Failed to get working area\r\n", __FUNCTION__);
        return NULL;
    }

    return CLI_LIB_Get_Cmd_List_By_Privilege(ctrl_P->CMenu.AccMode);
}

/* PURPOSE: Get command table in each mode for privilege
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  Command table
 * NOTE:
 */
CMDLIST *CLI_LIB_Get_Cmd_List_By_Privilege(UI8_T  privilegeMode)
{
    CLI_COMMAND_TREE_ROOT_ENTRY *entry_p = CLI_LIB_GetCommandTreeRoot(privilegeMode);

    if (NULL == entry_p)
    {
        return NULL;
    }

    return entry_p->cmd_list;
}

#endif // #if 0
#pragma mark -

static BOOL_T is_meet_delemiter(char c, char delemiters[]);
/* PURPOSE: Get a string from a line
 * INPUT:   s:     Starting address to search
 * OUTPUT:  Token: Buffer of the found token
 * RETURN:  The address that follow the got string, if 0 => no next string
 * NOTE:
 */
char *CLI_LIB_Get_Token (char *s, char *Token, char delemiters[])
{

   int  state = 0;

   for( ; *s !=0 || *s == 0x0a || *s == 0x0d ; s++)
   {
      switch ( state )
      {
         case 0:   /* skip while space */
         if ( is_meet_delemiter(*s, delemiters) )
         {
            continue;
         }
         else
         {
            state = 1;
            *Token++ = *s;
            break;
         }

         case 1:   /* character occures */
         if ( is_meet_delemiter(*s, delemiters) )
         {
            char *s_op = s + 1;
            *Token = 0;

            for(; ; s_op++) /* search if non-space exists */
            {
               if( *s_op == 0 || *s_op == 0x0a || *s_op == 0x0d )
                  return 0;
               else if (!is_meet_delemiter(*s_op, delemiters))
                  break;
            }
            return s;
         }
         else
         {
            *Token++ = *s;
            break;
         }
      }
   }
   return 0; /* if (*s == 0) occures */
}

/*  Author : cjchu
 *  Notes Used by CLI_MAIN_MONITOR
 *
 */
void CLI_LIB_PrintNullPrompt(CLI_TASK_WorkingArea_T *ctrl_P)
{
#define PROMPT_LEN  300

    char prompt[PROMPT_LEN] = {0};
    char nullPrompt[PROMPT_LEN] = {0};

    assert(ctrl_P != NULL);

    SYS_PMGR_GetPromptString((UI8_T *)prompt);

#if (defined(STRAWMAN) || defined(STRAWMANHD))
    if (strlen((char *)Name) >=16 )
        memcpy(prompt,Name,sizeof(UI8_T)*16);
    else
        strcpy(prompt, (char *)Name);
#else
    switch(ctrl_P->sess_type)
    {
    case CLI_TYPE_UART:
        if(strlen(prompt)==0)
        {
            strcat(prompt, "Console");
        }
        break;

    case CLI_TYPE_TELNET:
        if(strlen(prompt)!=0)
        {
            SYSFUN_Sprintf(prompt, "%s-%d", prompt, CLI_TASK_GetMySessId() - CLI_TASK_MIN_TELNET_SESSION_ID + 1);
        }
        else
        {
            SYSFUN_Sprintf(prompt, "Vty-%d", CLI_TASK_GetMySessId() - CLI_TASK_MIN_TELNET_SESSION_ID + 1);
        }
        break;

    default:
        ;
        break;
    }
#endif

    if (CLI_LIB_IsProvision(ctrl_P) == FALSE)
    {
#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)
        UI32_T vlan_id;
        char  vlan_string_temp[20]={0};
#endif

        switch(ctrl_P->CMenu.AccMode)
        {
        case NORMAL_EXEC_MODE:
        default:
            strcat(prompt, NORMAL_EXEC_MODE_PROMPT);
            break;

        case PRIVILEGE_EXEC_MODE:
            strcat(prompt, PRIVILEGE_EXEC_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_GLOBAL_MODE:
            strcat(prompt, PRIVILEGE_CFG_GLOBAL_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE:
            strcat(prompt, PRIVILEGE_CFG_INTERFACE_LOOPBACK_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_INTERFACE_ETH_MODE:
            strcat(prompt, PRIVILEGE_CFG_INTERFACE_ETH_MODE_PROMPT);
            break;
#if (SYS_CPNT_IP_FOR_ETHERNET0 == TRUE)
        case PRIVILEGE_CFG_INTERFACE_ETH0_MODE: /*wenfang*/
            strcat(prompt, PRIVILEGE_CFG_INTERFACE_ETH0_MODE_PROMPT);
            break;
#endif
#if (SYS_CPNT_CRAFT_PORT == TRUE)
        case PRIVILEGE_CFG_INTERFACE_CRAFT_MODE:
            strcat(prompt, PRIVILEGE_CFG_INTERFACE_CRAFT_MODE_PROMPT);
            break;
#endif
#if (SYS_CPNT_FILTER_THROOTTLE == TRUE)
        case PRIVILEGE_CFG_IGMP_PROFILE_MODE:
#endif
            strcat(prompt, PRIVILEGE_CFG_INTERFACE_IGMP_PROFILE_PROMPT);
            break;
        case PRIVILEGE_CFG_INTERFACE_VLAN_MODE:
            strcat(prompt, PRIVILEGE_CFG_INTERFACE_VLAN_MODE_PROMPT);
            break;
#if (SYS_CPNT_IP_TUNNEL == TRUE)
        case PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE:
            strcat(prompt, PRIVILEGE_CFG_INTERFACE_TUNNEL_MODE_PROMPT);
#endif /*(SYS_CPNT_IP_TUNNEL == TRUE)*/
        case PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE:
            strcat(prompt, PRIVILEGE_CFG_INTERFACE_PCHANNEL_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_VLAN_DATABASE_MODE:
#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)
            CLI_API_GET_VLAN_DATABASE_VLANID(&vlan_id);
            SYSFUN_Sprintf((char *)vlan_string_temp,"%s%lu)#", PRIVILEGE_CFG_VLAN_DATABASE_MODE_PROMPT,vlan_id);
            strcat(prompt, vlan_string_temp);
#else
            strcat(prompt, PRIVILEGE_CFG_VLAN_DATABASE_MODE_PROMPT);
#endif
            break;

        case PRIVILEGE_CFG_LINE_MODE:
            strcat(prompt, PRIVILEGE_CFG_LINE_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_MSTP_MODE:
            strcat(prompt, PRIVILEGE_CFG_MSTP_MODE_PROMPT);
            break;
        case PRIVILEGE_CFG_DOMAIN_MODE:
            strcat(prompt, PRIVILEGE_CFG_DOMAIN_MODE_PROMPT);
            break;


        case DEBUG_MODE:
            strcat(prompt, DEBUG_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_ROUTER_MODE:
            strcat(prompt, PRIVILEGE_CFG_ROUTER_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_ROUTERDVMRP_MODE:
            strcat(prompt, PRIVILEGE_CFG_ROUTERDVMRP_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_ROUTEROSPF_MODE:
            strcat(prompt, PRIVILEGE_CFG_ROUTEROSPF_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_ROUTEROSPF6_MODE:
            strcat(prompt, PRIVILEGE_CFG_ROUTEROSPF_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_DHCPPOOL_MODE:
            strcat(prompt, PRIVILEGE_CFG_DHCPPOOL_MODE_PROMPT);
            break;

#if (SYS_CPNT_ACL == TRUE)
        case PRIVILEGE_CFG_ACL_MAC_MODE:
            strcat(prompt, PRIVILEGE_CFG_ACL_MAC_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_ACL_STANDARD_MODE:
            strcat(prompt, PRIVILEGE_CFG_ACL_STANDARD_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_ACL_EXTENDED_MODE:
            strcat(prompt, PRIVILEGE_CFG_ACL_EXTENDED_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_ACL_IP_MASK_MODE:
            strcat(prompt, PRIVILEGE_CFG_ACL_IP_MASK_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_ACL_MAC_MASK_MODE:
            strcat(prompt, PRIVILEGE_CFG_ACL_MAC_MASK_MODE_PROMPT);
            break;
#if (SYS_CPNT_DAI == TRUE)
        case PRIVILEGE_CFG_ACL_ARP_MODE:
            strcat((char *)prompt, PRIVILEGE_CFG_ACL_ARP_MODE_PROMPT);
            break;
#endif
#if (SYS_CPNT_ACL_IPV6 == TRUE)
        case PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE:
            strcat((char *)prompt, PRIVILEGE_CFG_ACL_STANDARD_IPV6_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_ACL_EXTENDED_IPV6_MODE:
            strcat((char *)prompt, PRIVILEGE_CFG_ACL_EXTEND_IPV6_MODE_PROMPT);
            break;
#endif
#endif /* SYS_CPNT_ACL */

#if (SYS_CPNT_AAA == TRUE)
        case PRIVILEGE_CFG_AAA_SG_MODE:
            strcat(prompt, PRIVILEGE_CFG_AAA_SG_TACACS_PLUS_MODE_PROMPT);
            break;

#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)
        case PRIVILEGE_CFG_AAA_SG_RADIUS_MODE:
            strcat(prompt, PRIVILEGE_CFG_AAA_SG_RADIUS_MODE_PROMPT);
            break;
#endif /*#if (SYS_CPNT_RADIUS_ACCOUNTING == TRUE)*/

#endif /*#if (SYS_CPNT_AAA == TRUE)*/

#if (SYS_CPNT_QOS == SYS_CPNT_QOS_DIFFSERV)
        case PRIVILEGE_CFG_CLASS_MAP_MODE:
            strcat(prompt, PRIVILEGE_CFG_CLASS_MAP_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_POLICY_MAP_MODE:
            strcat(prompt, PRIVILEGE_CFG_POLICY_MAP_MODE_PROMPT);
            break;

        case PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE:
            strcat(prompt, PRIVILEGE_CFG_POLICY_MAP_CLASS_MODE_PROMPT);
            break;
#endif

#if(SYS_CPNT_TIME_BASED_ACL==TRUE)
        case PRIVILEGE_CFG_TIME_RANGE_MODE:
            strcat(prompt, PRIVILEGE_CFG_TIME_RANGE_MODE_PROMPT);
            break;
#endif

#if (SYS_CPNT_ERPS == TRUE)
        case PRIVILEGE_CFG_ERPS_MODE:
            strcat((char *)prompt, PRIVILEGE_CFG_ERPS_MODE_PROMPT);
            break;
#endif
        }
    }
    else
    {
        /*do nothing*/
    }

    memset(nullPrompt, 32, strlen(prompt));
    CLI_LIB_PrintStr(nullPrompt);
}


BOOL_T  CLI_LIB_EAtoStr(char *mac, char *Str)   //cjchu 2001/8/2
{
    SYSFUN_Sprintf(Str, "%2.2x-%2.2x-%2.2x-%2.2x-%2.2x-%2.2x",
                   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return TRUE;
}

/* PURPOSE: Convert 32-bit IP address to dot-and-decimal style character string
 * INPUT:   HexAddr: 32-bit IP address
            is_reverse: reverse the byte order
 * OUTPUT:  OutStr : Dot-and-decimal style character string
 * RETUEN:  None
 * NOTE:
 */
void HexAddrToStr(UI32_T HexAddr, char *OutStr, BOOL_T is_reverse)
{
    char Value[4];

    memcpy(Value, (void *)&HexAddr, 4);

    if (is_reverse)
        SYSFUN_Sprintf(OutStr, "%d.%d.%d.%d", Value[3], Value[2],Value[1],Value[0]);
    else
        SYSFUN_Sprintf(OutStr, "%d.%d.%d.%d", Value[0], Value[1],Value[2],Value[3]);
}

static BOOL_T is_meet_delemiter(char c, char delemiters[])
{
    UI32_T i = 0;

    for(; delemiters[i] != 0; i ++)
    {
        if(c == delemiters[i])
            return TRUE;
    }
    return FALSE;
}

UI64_T CLI_LIB_AtoUll(char *in_str)
{
    UI64_T ret_val  = 0;
    char  *ptr;

    ptr = in_str;

    for(; isdigit(*ptr); ptr++)
    {
        ret_val *= 10;
        ret_val += (*ptr -'0');
    }

    //printf("final ret_val: %llu\n", ret_val);
    return ret_val;
}

/* assume the format is correct
 */
BOOL_T CLI_LIB_Get_Lower_Upper_Value(char *buff, UI32_T *lower_val, UI32_T *upper_val, UI32_T *err_idx)
{
#define WAIT_FOR_VALUE 65535

    UI32_T i;
    UI32_T case_val = 0;

    for(i=0; buff[i]!= 0; i++)
    {
        /*space filter*/
        //if(isspace(buff[i]))
        //   continue;

        /*space filter*/
        //for( ; ; )
        //{
        //   if(isspace(buff[i]))
        //      i++;
        //   else
        //      break;
        //}

        switch(case_val)
        {
        case 0:                  /* find lower value */
            if(isdigit(buff[i]))
            {
                *lower_val = atoi((char *)(buff+i));
                *upper_val = atoi((char *)(buff+i));
                case_val = 1;

                for(;;i++)
                {
                    if(!isdigit(buff[i]))
                    {
                        i--;
                        break;
                    }
                }
            }
            else /*not digit*/
            {
                *err_idx = i;
                return FALSE;
            }
            break;


        case 1:                  /* find dash */
            if(buff[i] == '-')
            {
                case_val = 2;
                *upper_val = WAIT_FOR_VALUE;
            }
            else
            {
                *err_idx = i; /*not dash*/
                return FALSE;
            }
            break;


        case 2:                /* find upper value */
            if(isdigit(buff[i]))
            {
                *upper_val = atoi((char *)(buff+i));
                case_val = 3;

                for(;;i++)
                {
                    if(!isdigit(buff[i]))
                    {
                        i--;
                        break;
                    }
                }
            }
            else /*not digit*/
            {
                *err_idx = i;
                return FALSE;
            }
            break;

        case 3:                /* any thing else? */
            *err_idx = i;
            return FALSE;
        }
    }

    if(*upper_val == WAIT_FOR_VALUE)
        return FALSE;
    else
    {
        //printf("\n*lower_val: %d, *upper_val: %d\n", *lower_val, *upper_val);
        if (*upper_val < *lower_val)
            return FALSE;
        else
            return TRUE;
    }
}
BOOL_T CLI_LIB_Bitmap_To_List(char *bitmap, char *list,UI32_T buffer_len, UI32_T bit_numbers, BOOL_T is_first_bit_use)
{
    UI32_T i;
    UI32_T leading  = 0xffffffff;
    UI8_T case_val = 0;
    char  gen_str[16] = {0};
    UI32_T len = 0;
    *list = 0;

    if (is_first_bit_use)
    {
        for (i=0; i<bit_numbers; i++)
        {
            switch(case_val)
            {
            case 0:
                if( bitmap[i/8] & ( 1 << (7-i%8)) )
                {
                    if(i != bit_numbers-1)
                    {
                        leading     = i;
                        case_val    = 1;
                    }

                    else
                    {
                        SYSFUN_Sprintf((char *)gen_str, "%lu,", i + 1);
                        len = len + strlen((char *)gen_str);
                        if (len >= buffer_len)
                            return FALSE;
                        else
                            strcat((char *)list, (char *)gen_str);
                    }
                }
                break;

            case 1:
                if( !(bitmap[i/8] & ( 1 << (7-i%8))) )
                {

                    case_val = 0;
                    if(i == leading+1) /*individual*/
                        SYSFUN_Sprintf((char *)gen_str, "%lu,", leading + 1);
                    else               /*range*/
                        SYSFUN_Sprintf((char *)gen_str, "%lu-%lu,", leading + 1, i);

                    len = len + strlen((char *)gen_str);
                    if (len >= buffer_len)
                        return FALSE;
                    else
                        strcat((char *)list, (char *)gen_str);
                }
                else if(i == bit_numbers-1) /*exist and last one*/
                {
                    SYSFUN_Sprintf((char *)gen_str, "%lu-%lu,", leading + 1, i + 1);
                    len = len + strlen((char *)gen_str);
                    if (len >= buffer_len)
                        return FALSE;
                    else
                        strcat((char *)list, (char *)gen_str);
                }
                break;

            default:
                return FALSE;
            }
        }
    }
    else  /*first bit not use, and use second bit as port1 or vlan1*/
    {
        for (i=1; i<bit_numbers+1; i++)
        {
            switch(case_val)
            {
            case 0:
                if( bitmap[i/8] & ( 1 << (7-i%8)) )
                {
                    if(i != bit_numbers)
                    {
                        leading     = i;
                        case_val    = 1;
                    }
                    else
                    {
                        SYSFUN_Sprintf((char *)gen_str, "%lu,", i);
                        len = len + strlen((char *)gen_str);
                        if (len >= buffer_len)
                            return FALSE;
                        else
                            strcat((char *)list, (char *)gen_str);
                    }
                }
                break;

            case 1:
                if( !(bitmap[i/8] & ( 1 << (7-i%8))) )
                {
                    case_val = 0;
                    if(i == leading + 1) /*individual*/
                        SYSFUN_Sprintf((char *)gen_str, "%lu,", leading );
                    else               /*range*/
                        SYSFUN_Sprintf((char *)gen_str, "%lu-%lu,", leading , i-1);

                    len = len + strlen((char *)gen_str);
                    if (len >= buffer_len)
                        return FALSE;
                    else
                        strcat((char *)list, (char *)gen_str);
                }
                else if(i == bit_numbers) /*exist and last one*/
                {
                    SYSFUN_Sprintf((char *)gen_str, "%lu-%lu,", leading , i );
                    len = len + strlen((char *)gen_str);
                    if (len >= buffer_len)
                        return FALSE;
                    else
                        strcat((char *)list, (char *)gen_str);
                }
                break;

            default:
                return FALSE;
            }
        }
    }
    len = strlen((char *)list);
    if (list[len-1] == ',')
        list[len-1] = 0;

    return TRUE;
}

BOOL_T CLI_LIB_Stack_Bitmap_To_List(char *bitmap, UI32_T *unit_id, char *list, UI32_T buffer_len, UI32_T bit_numbers, BOOL_T is_first_bit_use)
{  //talor 2004-08-26, add this function.
    UI32_T i;
    UI32_T leading  = 0xffffffff;
    UI8_T case_val = 0;
    char  gen_str[16] = {0};
    UI32_T len = 0;
    *list = 0;

    if (is_first_bit_use)
    {
        for (i=0; i<bit_numbers; i++)
        {
            switch(case_val)
            {
            case 0:
                if( bitmap[i/8] & ( 1 << (7-i%8)) )
                {
                    if(i != bit_numbers-1)
                    {
                        leading     = i;
                        case_val    = 1;
                    }

                    else
                    {
                        SYSFUN_Sprintf((char *)gen_str, "%lu,", (i % SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1);
                        *unit_id = (i / SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1; /* talor 2004-08-31 */
                        len = len + strlen((char *)gen_str);
                        if (len >= buffer_len)
                            return FALSE;
                        else
                            strcat((char *)list, (char *)gen_str);
                    }
                }
                break;

            case 1:
                if( !(bitmap[i/8] & ( 1 << (7-i%8))) )
                {

                    case_val = 0;
                    if(i == leading+1) /*individual*/
                        SYSFUN_Sprintf((char *)gen_str, "%lu,", (leading % SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1);
                    else               /*range*/
                        SYSFUN_Sprintf((char *)gen_str, "%lu-%lu,", (leading % SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1, ((i - 1) % SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1);

                    *unit_id = (leading / SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1; /* talor 2004-08-31 */
                    len = len + strlen((char *)gen_str);
                    if (len >= buffer_len)
                        return FALSE;
                    else
                        strcat((char *)list, (char *)gen_str);
                }
                else if(i == bit_numbers-1) /*exist and last one*/
                {
                    SYSFUN_Sprintf((char *)gen_str, "%lu-%lu,", (leading % SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1, (i % SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1);
                    *unit_id = (leading / SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1; /* talor 2004-08-31 */

                    len = len + strlen((char *)gen_str);
                    if (len >= buffer_len)
                        return FALSE;
                    else
                        strcat((char *)list, (char *)gen_str);
                }
                break;

            default:
                return FALSE;
            }
        }
    }
    else  /*first bit not use, and use second bit as port1 or vlan1*/
    {
        for (i=1; i<bit_numbers+1; i++)
        {
            switch(case_val)
            {
            case 0:
                if( bitmap[i/8] & ( 1 << (7-i%8)) )
                {
                    if(i != bit_numbers)
                    {
                        leading     = i;
                        case_val    = 1;
                    }
                    else
                    {
                        SYSFUN_Sprintf((char *)gen_str, "%lu,", ((i - 1) % SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1);
                        *unit_id = ((i-1) / SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1; /* talor 2004-08-31 */
                        len = len + strlen((char *)gen_str);
                        if (len >= buffer_len)
                            return FALSE;
                        else
                            strcat((char *)list, (char *)gen_str);
                    }
                }
                break;

            case 1:
                if( !(bitmap[i/8] & ( 1 << (7-i%8))) )
                {
                    case_val = 0;
                    if(i == leading + 1) /*individual*/
                        SYSFUN_Sprintf((char *)gen_str, "%lu,", ((leading - 1) % SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1);
                    else               /*range*/
                        SYSFUN_Sprintf((char *)gen_str, "%lu-%lu,", ((leading - 1) % SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1, ((i - 2) % SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1);

                    *unit_id = (leading / SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1; /* talor 2004-08-31 */

                    len = len + strlen((char *)gen_str);
                    if (len >= buffer_len)
                        return FALSE;
                    else
                        strcat((char *)list, (char *)gen_str);
                }
                else if(i == bit_numbers) /*exist and last one*/
                {
                    SYSFUN_Sprintf((char *)gen_str, "%lu-%lu,", ((leading - 1) % SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1, ((i - 1) % SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1);
                    *unit_id = (leading / SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1; /* talor 2004-08-31 */

                    len = len + strlen((char *)gen_str);
                    if (len >= buffer_len)
                        return FALSE;
                    else
                        strcat((char *)list, (char *)gen_str);
                }
                break;

            default:
                return FALSE;
            }
        }
    }
    len = strlen((char *)list);
    if (list[len-1] == ',')
        list[len-1] = 0;

    return TRUE;
}

UI32_T CLI_LIB_Get_DOTDATA_1000Times_Value(char *data, UI8_T data_len)
{
    BOOL_T dot_meet  = FALSE;
    UI8_T  after_dot = 0;
    UI32_T value     = 0;
    UI8_T  i;

    for (i=0; i<data_len; i++)
    {
        if(data[i] == '.')
        {
            dot_meet = TRUE;
            continue;
        }

        value = (value*10) + (data[i] - '0');

        if(dot_meet)
            after_dot++;
    }

    for(i=0; i<(3-after_dot); i++)
        value = value*10 ;

    return value;
}


void CLI_LIB_MSB_To_LSB(char *data, UI32_T data_len)
{
    UI32_T i = 0;
    UI8_T  j = 0;

    for (i = 0; i < data_len ; i ++)
    {
        for (j = 0; j < 4 ; j++)
        {  /*if bit not the same will swap this two bit*/
            if (((data[i] & (UI32_T) 1<<(7-j))>>(7-j)) != ((data[i] & (UI32_T) 1<<j)>>j))
            {
                if (data[i] & (UI32_T) 1<<(7-j))/*if bit value = 1*/
                {
                    data[i] |= (UI32_T) 1<<j;
                    data[i] &= ~((UI32_T) 1<<(7-j));
                }
                else
                {
                    data[i] &= ~((UI32_T) 1<<j);
                    data[i] |= (UI32_T) 1<<(7-j);
                }
            }
        }
    }
    return ;
}

#if (CLI_SUPPORT_PORT_NAME == 1)
void CLI_LIB_Ifindex_To_Name(UI32_T ifindex, char *name)
{
    IF_MGR_IfXEntry_T  if_x_entry;

    memset(&if_x_entry, 0, sizeof(if_x_entry));
    if_x_entry.if_index = ifindex;
    if (!IF_PMGR_GetIfXEntry (&if_x_entry))
    {
        CLI_LIB_PrintStr("Failed to get port name\r\n");
        return;
    }

    strcpy(name, (char *)if_x_entry.if_name);
    return;
}
#endif

#if (CLI_SUPPORT_QUOTE_STRING_FEATURE == TRUE)
BOOL_T CLI_LIB_Check_Is_QuoteStr(char *str)
{
    char *p;
    UI32_T str_len;

    /* check if there is quote in front and end of string */
    p = str;
    str_len = strlen((char *)str);

    if(*p != '"')
    {
        return FALSE;
    }

    if(str_len==1)
    {
        return FALSE;
    }

    /* move pointer to latest char */
    p = p + (str_len - 1);

    if(*p != '"')
    {
        return FALSE;
    }
    return TRUE;
}

/************************************************************************/
/* CLI_LIB_Check_Need_Quote                                             */
/*                                                                      */
/* Description : check the token that should we need to add quote       */
/*               Create by Andrew 2005/12/1                             */
/*                                                                      */
/* Input:  string of token                                              */
/* Return: TRUE => success                                              */
/*                                                                      */
/************************************************************************/
BOOL_T CLI_LIB_Check_Need_Quote(char *str)
{
    char *p;
    UI32_T i = 0;
    UI32_T str_len;

    str_len = strlen(str);
    p = str;

    for(i = 0; i<strlen(str); i++)
    {
        if(p[i] == ' ' || p[i] == '"')
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL_T CLI_LIB_Str_Add_Quote(char *in_str, char *out_str)
{
    char  *p,*q;
    UI32_T i, j;
    UI32_T in_len;

    p = in_str;
    q = out_str;
    in_len = strlen((char *)in_str);

    /* improve by Andrew, adding quote in this part
     */
    if ( CLI_LIB_Check_Need_Quote(in_str) )
    {
        q[0] = '"';
        j = 1;
        for(i = 0; i<in_len; i++)
        {
            if(p[i] == '"')
            {
                q[j] = '"';
                j++;
            }
            q[j] = p[i];
            j++;
        }
        q[j] = '"';
        q[j+1] = '\0';
        return TRUE;
    }
    else
    {
        strcpy((char *)q, (char *)p);
        return TRUE;
    }
}


BOOL_T CLI_LIB_Str_Remove_Quote(char *str)
{
    char *p,*q;

    /* check if there is quote in front and end of string */
    if(!CLI_LIB_Check_Is_QuoteStr(str))
    {
        return FALSE;
    }

    p = str;
    q = str;

    /* remove quote */
    for (++p; *p != '\0'; )
    {
        if((*p == '"') && (*(p+1) == '\0'))
        {
            *q = '\0';
            p++;
        }
        else if((*p == '"') && (*(p+1) == '"'))
        {
            *q = '"';
            p += 2;
        }
        else
        {
            *q = *p;
            p++;
        }
        q++;

        if(*p == '\0')
        {
            *q = '\0';
            return TRUE;
        }
    }
    return TRUE;
}

#pragma mark -
#pragma mark #if 0
#if 0

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_Str_Remove_Quote_NotQuoteString
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to remove double quote to be one
 *
 * INPUT    : str
 * OUTPUT   :
 * RETURN   : TRUE/FALSE
 * NOTE     : if buffer have invalid char, function will return FALSE
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_Str_Remove_Quote_NotQuoteString(char *str)
{
    char *p,*q;

    p = str;
    q = str;

    /* remove quote */
    for (; *p != '\0'; )
    {
        if((*p == '"') && (*(p+1) == '"'))
        {
            *q = '"';
            p += 2;
        }
        else
        {
            *q = *p;
            p++;
        }
        q++;

        if(*p == '\0')
        {
            *q = '\0';
            return TRUE;
        }
    }
    return TRUE;
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_Is_Str_Enclosed_withQuote
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to check if string starts with quote.
 *            If start with quote and not end with quote, add quote at end
 * INPUT    : in_str
 * OUTPUT   : out_str
 * RETURN   : TRUE : start with quote, will be enclosed with quote finally
 *            FALSE : not start with quote
 * NOTE     : if buffer have invalid char, function will return FALSE
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_Is_Str_Enclosed_withQuote(char *in_str, char *out_str)
{
    char *p,*q;
    UI32_T in_len;
    int i,j=0;

    p = in_str;
    q = out_str;
    in_len = strlen(in_str);

    /* check if there is quote in front and end of string */
    if(*p != '"')
    {
        return FALSE;             /* not start with quote */
    }

    for(i = 0; i<in_len; i++)
    {
        q[j] = p[i];
        j++;
    }

    if(p[in_len-1]!='"')
    {
        q[in_len] = '"';
        q[in_len+1] = '\0';
    }
    else
    {
        q[in_len] = '\0';
    }
    return TRUE;
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_Check_Is_Legal_CharString
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to check if string is legal CharString
 *
 * INPUT    : str
 * OUTPUT   : error_index
 * RETURN   : TRUE/FALSE
 * NOTE     : if buffer have invalid char, function will return FALSE
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_Check_Is_Legal_CharString(char *str, UI16_T *error_index)
{
    char *p;
    UI32_T str_len;

    /* check if there is quote in front and end of string */
    p = str;
    str_len = strlen(str);

    if(str_len==1)
    {
        *error_index = 1;
        return FALSE;
    }

    p = str;
    *error_index = 1;

    for (++p; *p != '\0'; )
    {
        if((*p == '"') && (*(p+1) == '\0'))
        {
            (*error_index) += 1 ;
            return TRUE;
        }
        else if((*p == '"') && (*(p+1) == '"'))
        {
            p += 2;
            (*error_index) += 2;
        }
        else if((*p == '"') && (*(p+1) != '"') && (*(p+1) != '\0'))
        {
            (*error_index) += 1 ;
            return FALSE;
        }
        else
        {
            p++;
            (*error_index)++;
        }
    }
    return FALSE;
}

#endif

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_ReadLine_From_XferBuf
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to get a command line from buffer
 *
 * INPUT    : xbuf_p
 * OUTPUT   : cmd_buf
 * RETURN   : TRUE/FALSE
 * NOTE     : if buffer have invalid char, function will return FALSE
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_ReadLine_From_XferBuf(char *cmd_buf, char **xbuf_p)
{
    UI32_T i;

    memset(cmd_buf, 0, CLI_DEF_MAX_BUFSIZE);

    for ( i=0; i<CLI_DEF_MAX_BUFSIZE && ( *(*xbuf_p+i) != '\n' && *(*xbuf_p+i)!='\0'); i++ )
    {
/*
   pttch 92.04.24 add for check config file is valid or not
*/
       if (*(*xbuf_p+i) > 127)
       {
          return FALSE;
       }
       *(cmd_buf+i) = *(*xbuf_p+i);
    }

    *(cmd_buf+i) = 0;
    *xbuf_p = *xbuf_p + (i+1);

    return TRUE;
}
#endif
#pragma mark #endif
#pragma mark -

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_IsProvision
 * ------------------------------------------------------------------------
 * PURPOSE  : Check the CLI session whether is in provision
 * INPUT    : ctrl_P    - A pointer of CLI working area
 * OUTPUT   : None
 * RETURN   : TRUE, in provision; FALSE, not in provision
 * NOTE     : The input parameters should be invalidated by caller
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_IsProvision(
    CLI_TASK_WorkingArea_T *ctrl_P)
{
    if (CLI_TYPE_PROVISION == ctrl_P->sess_type
        || CLI_TYPE_MODULE_PROVISION == ctrl_P->sess_type
#if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE)
        || CLI_TYPE_PARTIAL_PROV == ctrl_P->sess_type
#endif
        )
    {
        return TRUE;
    }

    return FALSE;
}


/*-----------------------local utility-----------------------------------*/
/* PURPOSE: Get pointer of each part in MAC address
 * INPUT: s:  CLI input mac string(xx-xx-xx-xx-xx-xx or xxxxxxxxxxxx)
 * OUTPUT: Vals: fixed six bye mac address
 * RETUEN: TRUE:Convert success
 *         FALSE:Conver fail
 * NOTE:
 */
BOOL_T CLI_LIB_ValsInMac(char *s, UI8_T *Vals)
{
    UI32_T i = 0;
    UI32_T count=0;
    UI32_T MinusCount=0;

    for ( i=0; i< strlen((char *)s); i++)
    {
        if (!CLI_LIB_IsLess16(s[i]) && s[i] != '-')
        {
            return FALSE;
        }
        if (s[i] == '-')
        {
            if (!CLI_LIB_IsLess16(s[i+1]))
                return FALSE;
            MinusCount++;
        }
    }

   /*
     Convert Mac address XXXX-XXXX-XXXX into XX-XX-XX-XX-XX-XX or XXXXXXXXXXXX
     if (MinusCount==2) means Mac address XXXX-XXXX-XXXX
     if (MinusCount==5) means Mac address XX-XX-XX-XX-XX-XX
     if (MinusCount==0) means Mac address XXXXXXXXXXXX
   */
   /*
   if(MinusCount==2)
   {
       int count=0;
       int flag=1;
       UI8_T *temp_string;

       if(strlen(s)!=14)
       {
       return FALSE;
       }

       else
       {
       for(i=0; i<14; i++)
       {
           if(s[i]=='\b')
           {
                return FALSE;
           }
           else
           {
                if(s[i]!='-')
                {
                    if(i==13)
                    {
                         temp_string[count]=s[i];
                         break;
                    }
                    else
                    {
                         temp_string[count]=s[i];
                         flag++;
                         count++;
                    }
                }
                else
                {
                    flag=1;
                }
                if(flag==3)
                {
                     temp_string[count]='-';
                     count++;
                     flag=1;
                }

            }
        } // end for(i=0; i<15; i++)

        temp_string[count+1]='\0';
        s = temp_string;
        MinusCount=5;
    }
   }
   */

    if (MinusCount==0)
    {
        int i=0;
        char temp_string[18] = {0};

        count = 0;
        if(strlen((char *)s)!=12)
        {
            return FALSE;
        }
        else
        {
            for(i=0;i<12;i++)
            {
                if(s[i]=='\b')
                {
                    return FALSE;
                }
                else
                {
                    if((i%2)==1)
                    {
                        temp_string[count]=s[i];
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
                        temp_string[count]=s[i];
                    }
                    count++;
                }
            }
            temp_string[count+1]='\0';
            s = temp_string;
            MinusCount=5;
        }
    }

    if(MinusCount==5)
    {
        for (i=0;i<6;i++)
        {
            Vals[i] =  CLI_LIB_AtoUl(s, 16);
            s = strstr(s, "-");
            s++;
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}


/*-----------------------local utility-----------------------------------*/
/* PURPOSE: Check the input char is lession16
 * INPUT: c:  input chat to check
 * OUTPUT: none
 * RETUEN: TRUE:is lession 16
 *         FALSE:not lession 16
 * NOTE:
 */
BOOL_T CLI_LIB_IsLess16(char c)
{
    if ( (c >= '0' && c<= '9') || (c >= 'A' && c<= 'F') || (c >= 'a' && c<= 'f') )
        return TRUE;
    else
        return FALSE;
}

#pragma mark -
#pragma mark #if 0
#if 0
/* FUNCTION NAME:   CLI_LIB_ConvertSecretKeyByLBase64
 * PURPOSE: convert secret key argument according to ctrl_p's sess_type
 * INPUT:   ctrl_p          -- CLI working area
 *          src_key_p       -- CLI secret argument
 *          dst_buf_size    -- how many bytes of dst_key_p
 * OUTPUT:  dst_key_p       -- output buffer after conversion
 * RETURN:  TRUE -- succeeded / FALSE -- failed
 * NOTES:   None
 */
BOOL_T CLI_LIB_ConvertSecretKeyByLBase64(
            CLI_TASK_WorkingArea_T *ctrl_p,
            char *src_key_p,
            char *dst_key_p,
            UI32_T dst_buf_size)
{
    assert(ctrl_p != NULL);
    /* if this is provision session
     */
    if (CLI_LIB_IsProvision(ctrl_p) == TRUE)
    {
        UI8_T   *out_buffer_p;
        UI32_T  out_len = 0;

        /* if decrypt secret key failed
         */
        if (FALSE == L_Base64((UI8_T *)src_key_p, strlen(src_key_p), &out_buffer_p, &out_len, L_BASE64_DECODE))
        {
            CLI_LIB_PrintStr("\r\nDecrypt key fail.\r\n");
            return FALSE;
        }

        /* if the decrypted key is too long
         */
        if (dst_buf_size < out_len)
        {
            CLI_LIB_PrintStr_1("\r\nSecret key can not large than %lu.\r\n",
                dst_buf_size);
            L_MM_Free(out_buffer_p);
            return FALSE;
        }

        strncpy(dst_key_p, (char *)out_buffer_p, out_len);
        dst_key_p[out_len] = '\0';
        L_MM_Free(out_buffer_p);
    }
    else
    {
        /* if the argument is too long
         */
        if (dst_buf_size < strlen(src_key_p))
        {
            CLI_LIB_PrintStr_1("\r\nSecret key can not large than %lu.\r\n",
                dst_buf_size);
            return FALSE;
        }

        strncpy(dst_key_p, src_key_p, dst_buf_size);
        dst_key_p[dst_buf_size] = '\0';
    }

    return TRUE;
}

#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)

/*
Eric_Huang,2004/6/4,
Convert port list to user port.
If unit/port=1/1 to 1/10 are untagged then analyze port list and convert to user port.
Like that "untagged ethernet 1/1 to 1/10".
*/

BOOL_T CLI_LIB_Portlist_To_Userport(char *bitmap, char *list,UI32_T port_member_status)
{
   UI32_T ifindex;
   UI8_T case_val = 0;
   char  gen_str[40] = {0};
   UI32_T begin_unit,begin_port,end_unit,end_port;
   UI32_T unit,port,max_port_num;

   UI32_T current_max_unit;

   UI32_T leading;
   UI32_T i=0;

   memset(list,0,sizeof(list));


#if (SYS_CPNT_STACKING == TRUE)
            current_max_unit = SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK;
#else
            current_max_unit = 1;
#endif

#if ( SYS_CPNT_UNIT_HOT_SWAP != TRUE )
            for (i=0; STKTPLG_MGR_GetNextUnit(&i); )
            {
                unit = i;
                max_port_num = SWCTRL_POM_UIGetUnitPortNumber(unit);

                /* initialize local variable
                 */
                case_val = 0;
                begin_port = 0;
                end_port = 0;

                for(port = 1;port <= max_port_num; port++)
                {
                    SWCTRL_POM_UIUserPortToLogicalPort(unit,port,&ifindex);
                    memset(gen_str,0,sizeof(gen_str));

                    switch(case_val)
                    {
                        case 0:
                        if( bitmap[(ifindex-1)/8] & ( 1 << (7-(ifindex-1)%8)) )
                        {
                            if((port!=max_port_num))
                            {
                                leading     = port;
                                case_val    = 1;
                            }

                            else
                            {
                                begin_unit = unit;
                                begin_port = port;

                                if(port_member_status==0)
                                {
                                    SYSFUN_Sprintf(gen_str, " untagged ethernet %lu/%lu\r\n",begin_unit,begin_port);
                                }
                                else
                                {
                                    SYSFUN_Sprintf(gen_str, " tagged ethernet %lu/%lu\r\n",begin_unit,begin_port);
                                }
                                strcat(list, gen_str);
                            }
                        }
                        break;

                        case 1:
                        if( !(bitmap[(ifindex-1)/8] & ( 1 << (7-(ifindex-1)%8))) )
                        {

                            case_val = 0;

                            /*individual*/
                            if(port == leading+1)
                            {
                                begin_unit = unit ;
                                begin_port = leading ;

                                if(port_member_status==0)
                                {
                                     SYSFUN_Sprintf(gen_str, " untagged ethernet %lu/%lu\r\n",begin_unit,begin_port);
                                }
                                else
                                {
                                    SYSFUN_Sprintf(gen_str, " tagged ethernet %lu/%lu\r\n",begin_unit,begin_port);
                                }
                            }
                            else               /*range*/
                            {
                                begin_unit = unit;
                                begin_port = leading ;
                                end_unit = unit ;
                                end_port = port-1 ;

                                if(port_member_status==0)
                                {
                                    SYSFUN_Sprintf(gen_str, " untagged ethernet %lu/%lu to %lu/%lu\r\n",begin_unit,begin_port,end_unit,end_port);
                                }
                                else
                                {
                                    SYSFUN_Sprintf(gen_str, " tagged ethernet %lu/%lu to %lu/%lu\r\n",begin_unit,begin_port,end_unit,end_port);
                                }
                            }
                            strcat(list, gen_str);
                         }
                         else if((port==max_port_num))/*exist and last one*/
                         {
                             begin_unit = unit;
                             begin_port = leading;
                             end_unit = unit;
                             end_port = port;

                             if(port_member_status==0)
                             {
                                 SYSFUN_Sprintf(gen_str, " untagged ethernet %lu/%lu to %lu/%lu\r\n",begin_unit,begin_port,end_unit,end_port);
                             }
                             else
                             {
                                 SYSFUN_Sprintf(gen_str, " tagged ethernet %lu/%lu to %lu/%lu\r\n",begin_unit,begin_port,end_unit,end_port);
                             }
                             strcat(list, gen_str);
                        }
                        break;

                        default:
                            return FALSE;
                        break;
                    }
                }
            }
#else
   for ( i = 1; i <= current_max_unit; i++ )
   {
       unit = i;

       /*Need to check all ports whether module port exists or not.*/
       max_port_num = SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT;

       /* initialize local variable
        */
       case_val = 0;
       begin_port = 0;
       end_port = 0;

       for(port = 1;port <= max_port_num; port++)
       {
           SWCTRL_POM_UIUserPortToLogicalPort(unit,port,&ifindex);
           memset(gen_str,0,sizeof(gen_str));

           switch(case_val)
           {
               case 0:
               if( bitmap[(ifindex-1)/8] & ( 1 << (7-(ifindex-1)%8)) )
               {
                   if((port!=max_port_num))
                   {
                       leading     = port;
                       case_val    = 1;
                   }

                   else
                   {
                       begin_unit = unit;
                       begin_port = port;

                       if(port_member_status==0)
                       {
                           SYSFUN_Sprintf(gen_str, " untagged ethernet %lu/%lu\r\n",begin_unit,begin_port);
                       }
                       else
                       {
                           SYSFUN_Sprintf(gen_str, " tagged ethernet %lu/%lu\r\n",begin_unit,begin_port);
                       }
                       strcat(list, gen_str);
                   }
               }
               break;

               case 1:
               if( !(bitmap[(ifindex-1)/8] & ( 1 << (7-(ifindex-1)%8))) )
               {

                   case_val = 0;

                   /*individual*/
                   if(port == leading+1)
                   {
                       begin_unit = unit ;
                       begin_port = leading ;

                        if(port_member_status==0)
                       {
                            SYSFUN_Sprintf(gen_str, " untagged ethernet %lu/%lu\r\n",begin_unit,begin_port);
                       }
                       else
                       {
                           SYSFUN_Sprintf(gen_str, " tagged ethernet %lu/%lu\r\n",begin_unit,begin_port);
                       }
                   }
                   else               /*range*/
                   {
                       begin_unit = unit;
                       begin_port = leading ;
                       end_unit = unit ;
                       end_port = port-1 ;

                       if(port_member_status==0)
                       {
                           SYSFUN_Sprintf(gen_str, " untagged ethernet %lu/%lu to %lu/%lu\r\n",begin_unit,begin_port,end_unit,end_port);
                       }
                       else
                       {
                           SYSFUN_Sprintf(gen_str, " tagged ethernet %lu/%lu to %lu/%lu\r\n",begin_unit,begin_port,end_unit,end_port);
                       }
                   }
                   strcat(list, gen_str);
                }
                else if((port==max_port_num))/*exist and last one*/
                {
                    begin_unit = unit;
                    begin_port = leading;
                    end_unit = unit;
                    end_port = port;

                    if(port_member_status==0)
                    {
                        SYSFUN_Sprintf(gen_str, " untagged ethernet %lu/%lu to %lu/%lu\r\n",begin_unit,begin_port,end_unit,end_port);
                    }
                    else
                    {
                        SYSFUN_Sprintf(gen_str, " tagged ethernet %lu/%lu to %lu/%lu\r\n",begin_unit,begin_port,end_unit,end_port);
                    }
                    strcat(list, gen_str);
               }
               break;

               default:
                   return FALSE;
               break;
           }
       }
   }
#endif
   return TRUE;
}
#endif

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_Find_Tab_String
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to find the minimum matched arg
 *
 * INPUT    : buff => Command line buffer
              word_buf => string to match
              arg_idx => argument index
              word_len => length of word_buf
              str_idx => The start string index of final matched word
 * OUTPUT   : min_len
 * RETURN   : TRUE/FALSE
 * NOTE     : if buffer have invalid char, function will return FALSE
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_Find_Tab_Arg(char *buff, char *word_buf, UI16_T str_idx, UI16_T arg_idx, UI16_T word_len, UI16_T *min_len)
{
    char   min_word[CLI_DEF_MAX_WORDSIZE+1];
    UI16_T  match_num, cnt, i, j;
    UI16_T  arg_use_idx;
    UI16_T  add_num, add_flag;

    /*-------------------------------*/
    /* Find the minimum matched word */
    /*-------------------------------*/
    CLI_LIB_StrCmp((char *)word_buf, ui_arg_lst[arg_idx].name, &match_num);

    if (match_num < word_len)
    {
        return FALSE;
    }
    *min_len = match_num;

    while ((ui_arg_lst[arg_idx].name[*min_len] != '-')
        && (ui_arg_lst[arg_idx].name[*min_len] != '\0'))
    {
        (*min_len)++;
    }

    for (i=0; i<*min_len; i++)
        min_word[i] = ui_arg_lst[arg_idx].name[i];
    min_word[i] = '\0';

    /*--------------------------------*/
    /* Find out those ambiguous words */
    /*--------------------------------*/
    cnt = 1;
    arg_use_idx = ui_arg_lst[arg_idx].neibor_idx;
    while (arg_use_idx != NULL_ARG)
    {
        CLI_LIB_StrCmp((char *)word_buf, ui_arg_lst[arg_use_idx].name, &match_num);
        if (match_num != word_len)
            break;

        CLI_LIB_StrCmp((char *)min_word, ui_arg_lst[arg_use_idx].name, &match_num);
        if (match_num < *min_len)
        {
            *min_len = match_num;
            //return CLI_NO_ERROR; /* pttch, Jun18, 01 */
        }

        arg_use_idx = ui_arg_lst[arg_use_idx].neibor_idx;
        cnt++;
    }

    /*-------------------------------------------*/
    /* Find the maximum matched character string */
    /*-------------------------------------------*/
    add_flag = 1;
    while ((ui_arg_lst[arg_idx].name[*min_len]!='\0') && (add_flag==1))
    {
        /* Capture next want to matched word/characters
        */
        if (ui_arg_lst[arg_idx].name[*min_len] == '-')
            add_num = 1;
        else
        {
            add_num = 0;
            while ((ui_arg_lst[arg_idx].name[(*min_len)+add_num] != '-')
                && (ui_arg_lst[arg_idx].name[(*min_len)+add_num] != '\0'))
            {
                add_num++;
            }
        }

        /* Check if it is matched in all ambiguous words
        */
        arg_use_idx = ui_arg_lst[arg_idx].neibor_idx;
        for (i=1; i<cnt; i++)
        {
            for (j=0; j<add_num; j++)
            {
                if (ui_arg_lst[arg_use_idx].name[(*min_len)+j]
                    != ui_arg_lst[arg_idx].name[(*min_len)+j])
                {
                    add_flag = 0;
                    break;
                }
            }

            if (add_flag == 0)
                break;
            arg_use_idx = ui_arg_lst[arg_use_idx].neibor_idx;
        }

        if (add_flag == 1)
            (*min_len) += add_num;
    }  /* while */

    for (i=word_len; i<*min_len; i++)
        *(buff+str_idx+i) = ui_arg_lst[arg_idx].name[i];
    *(buff+str_idx+i) = '\0';

    return TRUE;
}


/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_SplitString
 * -------------------------------------------------------------------------
 * FUNCTION: Split string to array
 * INPUT   : str            - Input string. After call this function the
 *                            context of the string will be modified.
 *           argc           - String array size.
 * OUTPUT  : argv           - String array.
 *           argc           - How many string be saved in array.
 * RETURN  : TRUE/FALSE
 * NOTE    : If string array is too small, some part of input string will lost.
 * -------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_SplitString(char *str,
                    char *argv[],
                    UI32_T *argc)
{
    UI16_T *widx;

    if (NULL == str || NULL == argc || 0 == *argc)
    {
        return FALSE;
    }

    widx = malloc(sizeof(*widx) * *argc);
    if (NULL == widx)
    {
        return FALSE;
    }

    *argc = CLI_PARS_GetWords((UI8_T *)str, (UI8_T **)argv, widx, *argc);

    free(widx);
    return TRUE;
}

/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_JoinStringArray
 * -------------------------------------------------------------------------
 * FUNCTION: Join string array
 * INPUT   : argv           - String array.
 *           argc           - String array size.
 *           separator      - Separator.
 *           buf_size       - Buffer size of result parameter. Pass result
 *                            parameter = NULL to calculate how many size
 *                            is need for result.
 * OUTPUT  : result         - Result string.
 *           buf_size       - How many size is need for result.
 * RETURN  : TRUE/FALSE
 * NOTE    : None.
 * -------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_JoinStringArray(char *argv[],
                        UI32_T argc,
                        const char *separator,
                        char *result,
                        UI32_T *buf_size_p)
{
    UI32_T i;
    UI32_T remain_size;
    char *p = result;

    if (NULL == buf_size_p)
    {
        return FALSE;
    }

    if (NULL != result && 0 == *buf_size_p)
    {
        return FALSE;
    }

    remain_size = *buf_size_p;
    *buf_size_p = 0;

    if (result)
    {
        *p = '\0';
        ++p; --remain_size;
    }

    ++(*buf_size_p);

    for (i=0; i<argc; ++i)
    {
        UI32_T sz = strlen(argv[i]) + 1;

        if (0 < i)
        {
            UI32_T sep_sz = strlen(separator) + 1;

            --p; ++remain_size; --(*buf_size_p);

            if (result)
            {
                if (remain_size < sep_sz)
                {
                    return FALSE;
                }

                memcpy(p, separator, sep_sz);
            }

            p += sep_sz; remain_size -= sep_sz; (*buf_size_p) += sep_sz;
        }

        --p; ++remain_size; --(*buf_size_p);

        if (result)
        {
            if (remain_size < sz)
            {
                return FALSE;
            }

            memcpy(p, argv[i], sz);
        }

        p += sz; remain_size -= sz; (*buf_size_p) += sz;
    }

    return TRUE;
}


typedef enum
{
    NT_COMMAND,
    NT_ARGUMENT,
} CLI_NodeType_T;

typedef enum
{
    WALK_COMMAND_ONLY,
    WALK_ARGUMENT_ONLY,
    WALK_COMMAND_ARGUMENT
} CLI_WalkType_T;

typedef struct
{
    CLI_NodeType_T  type;

    union
    {
        CMDLIST     *cmd_list;
        ARGLIST     *arg_list;
    };

    UI16_T          idx;

} CLI_TrackEntry_T;


/* -------------------------------------------------------------------------
 * ROUTINE NAME - VisitorFuncPtr
 * -------------------------------------------------------------------------
 * FUNCTION: Visitor function pointer
 * INPUT   : argv       - Track array
 *           argc       - Length of track array
 *           param_p    - Stub parameter which pass from Proxy function
 * OUTPUT  : None
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 * -------------------------------------------------------------------------
 */
typedef BOOL_T (*VisitorFuncPtr) (const CLI_TrackEntry_T argv[],
                                  UI32_T argc,
                                  void *param_p);


static BOOL_T
CLI_LIB_IsNullIndex(CLI_TrackEntry_T *node_p)
{
    return ((NT_COMMAND == node_p->type && NULL_CMD == node_p->idx) ||
            (NT_ARGUMENT == node_p->type && NULL_ARG == node_p->idx)) ? TRUE : FALSE;
}

static BOOL_T
CLI_LIB_IsWalkForCommandNode(CLI_WalkType_T type)
{
    return (WALK_COMMAND_ONLY == type || WALK_COMMAND_ARGUMENT == type) ?
    TRUE : FALSE;
}

static BOOL_T
CLI_LIB_IsWalkForArgumentNode(CLI_WalkType_T type)
{
    return (WALK_ARGUMENT_ONLY == type || WALK_COMMAND_ARGUMENT == type) ?
    TRUE : FALSE;
}

static void
CLI_LIB_PushTrack(CLI_TrackEntry_T track[],
                  UI32_T *curr_depth_p,
                  CLI_TrackEntry_T *node_p)
{
    memcpy(&track[*curr_depth_p], node_p, sizeof(*track));

    *curr_depth_p += 1;
}

static void
CLI_LIB_PopTrack(CLI_TrackEntry_T track[],
                 UI32_T *curr_depth_p)
{
    *curr_depth_p -= 1;

    memset(&track[ *curr_depth_p ], 0, sizeof(*track));
}

static BOOL_T
CLI_LIB_Stub_Inorder(CLI_TrackEntry_T *node_p,
                     CLI_WalkType_T walk_type,
                     CLI_TrackEntry_T track[],
                     UI32_T max_depth,
                     UI32_T *curr_depth_p,
                     VisitorFuncPtr visitor,
                     void *visitor_param_p)
{
    CLI_TrackEntry_T tmp_node;

    if (max_depth < *curr_depth_p+1)
    {
        return FALSE;
    }

    if (CLI_LIB_IsNullIndex(node_p))
    {
        return TRUE;
    }

    /* --> child cmd --> arugment --> visit --> neibor cmd
     */
    if (NT_COMMAND == node_p->type)
    {
loop_cmd:
        if (CLI_LIB_IsWalkForCommandNode(walk_type))
        {
            CLI_LIB_PushTrack(track, curr_depth_p, node_p);
        }

        if (CLI_LIB_IsWalkForCommandNode(walk_type))
        {
            if (NULL_CMD != node_p->cmd_list[ node_p->idx ].child_idx)
            {
                tmp_node.type       = NT_COMMAND;
                tmp_node.cmd_list   = node_p->cmd_list;
                tmp_node.idx        = node_p->cmd_list[ node_p->idx ].child_idx;

                CLI_LIB_Stub_Inorder(&tmp_node,
                                     walk_type,
                                     track,
                                     max_depth,
                                     curr_depth_p,
                                     visitor,
                                     visitor_param_p);
            }
        }

        if (CLI_LIB_IsWalkForArgumentNode(walk_type))
        {
            if (0 != node_p->cmd_list[ node_p->idx ].max_args &&
                NULL_ARG != node_p->cmd_list[ node_p->idx ].arg_idx)
            {
                tmp_node.type       = NT_ARGUMENT;
                tmp_node.arg_list   = ui_arg_lst;
                tmp_node.idx        = node_p->cmd_list[ node_p->idx ].arg_idx;

                CLI_LIB_Stub_Inorder(&tmp_node,
                                     walk_type,
                                     track,
                                     max_depth,
                                     curr_depth_p,
                                     visitor,
                                     visitor_param_p);
            }
        }

        visitor(track, *curr_depth_p, visitor_param_p);

        if (CLI_LIB_IsWalkForCommandNode(walk_type))
        {
            CLI_LIB_PopTrack(track, curr_depth_p);
        }

        if (CLI_LIB_IsWalkForCommandNode(walk_type))
        {
            if (NULL_CMD != node_p->cmd_list[ node_p->idx ].neibor_idx)
            {
                node_p->idx = node_p->cmd_list[ node_p->idx ].neibor_idx;
                goto loop_cmd;
            }
        }
    }
    /* next arg --> visit --> neibor arg
     */
    else if (NT_ARGUMENT == node_p->type)
    {
        if (CLI_LIB_IsWalkForArgumentNode(walk_type))
        {
loop_arg:
            CLI_LIB_PushTrack(track, curr_depth_p, node_p);

            if (NULL_ARG != node_p->arg_list[ node_p->idx ].next_idx)
            {
                tmp_node.type       = NT_ARGUMENT;
                tmp_node.arg_list   = ui_arg_lst;
                tmp_node.idx        = node_p->arg_list[ node_p->idx ].next_idx;

                CLI_LIB_Stub_Inorder(&tmp_node,
                                     walk_type,
                                     track,
                                     max_depth,
                                     curr_depth_p,
                                     visitor,
                                     visitor_param_p);
            }

            visitor(track, *curr_depth_p, visitor_param_p);

            CLI_LIB_PopTrack(track, curr_depth_p);

            if (NULL_ARG != node_p->arg_list[ node_p->idx ].neibor_idx)
            {
                node_p->idx         = node_p->arg_list[ node_p->idx ].neibor_idx;
                goto loop_arg;
            }
        }
    }

    return TRUE;
}

static BOOL_T
CLI_LIB_Inorder(CLI_TrackEntry_T *node_p,       /* starting node */
                CLI_WalkType_T walk_type,
                UI32_T max_depth,
                VisitorFuncPtr visitor,
                void *visitor_param_p)
{
#if 1
    CLI_TrackEntry_T *track_tbl;
#else
    CLI_TrackEntry_T track_tbl[100];
#endif
    UI32_T           curr_depth;
    BOOL_T           ret;

    if (NULL == node_p || NULL == visitor)
    {
        return FALSE;
    }

    if (0 == max_depth)
    {
        if (CLI_LIB_IsNullIndex(node_p))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }

    }

#if 1
    track_tbl = calloc(sizeof(CLI_TrackEntry_T) * (max_depth+1), 1);
    if (NULL == track_tbl)
    {
        return FALSE;
    }
#endif

    curr_depth = 0;
    ret = CLI_LIB_Stub_Inorder(node_p, walk_type, track_tbl, max_depth,
                               &curr_depth, visitor, visitor_param_p);

#if 1
    free(track_tbl);
#endif

    return ret;
}


static BOOL_T
CLI_LIB_Stub_DumpCommandTree(const CLI_TrackEntry_T track[],
                             UI32_T argc,
                             void *param_p)
{
    UI32_T i;

    UI32_T *counter = (UI32_T*)(param_p);

    if (counter)
    {
        *counter += 1;
    }

    for (i=0; i<argc; ++i)
    {
        /* %s => C_ or A_
         * %s => name of command or argument
         * %s => '' or ' '
         */
        if (track[i].type == NT_COMMAND)
        {
#if(SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
            printf("%sC_%s,pl=%hu",
                   i==0 ? "" : " ",
                   track[i].cmd_list[ track[i].idx ].name,
                   track[i].cmd_list[ track[i].idx ].current_privilege);
#else
            printf("%sC_%s",
                   i==0 ? "" : " ",
                   track[i].cmd_list[ track[i].idx ].name);

#endif
        }
        else if (track[i].type == NT_ARGUMENT)
        {
            printf("%sA_%s",
                   i==0 ? "" : " ",
                   track[i].arg_list[ track[i].idx ].name);
        }
        else
        {
            assert(0);
        }
    }

    printf("\r\n");
    fflush(stdout);

    return TRUE;
}

int
CLI_LIB_DumpCommandTree(const char * mode_string)
{
    CLI_TrackEntry_T node;
    enum CLI_ACCESS_MODE_E cmd_mode;
    UI32_T counter=0;

    CLI_LIB_GetCommandMode(mode_string, &cmd_mode);

    node.type = NT_COMMAND;
    node.cmd_list = CLI_LIB_Get_Cmd_List_By_Privilege(cmd_mode);
    node.idx = 0;

    CLI_LIB_Inorder(&node, WALK_COMMAND_ONLY, 100, CLI_LIB_Stub_DumpCommandTree, &counter);

    printf("counter = %lu\r\n", counter);

    return 0;
}

/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_GetCommandMode
 * -------------------------------------------------------------------------
 * FUNCTION: Get access mode
 * INPUT   : str        - mode string
 * OUTPUT  : mode       - command mode
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 * -------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_GetCommandMode(const char *str,
                       enum CLI_ACCESS_MODE_E *mode_p)
{
    enum CLI_ACCESS_MODE_E i = 0;
    UI16_T len;

    if (NULL == str || NULL == mode_p)
    {
        return FALSE;
    }

    len = (UI16_T) strlen(str);

    for (i=0; i<CLI_ACCESS_MODE_MAX_NUM; ++i)
    {
        CLI_COMMAND_TREE_ROOT_ENTRY *entry_p = CLI_LIB_GetCommandTreeRoot(i);
        UI16_T matched;
        I16_T  cmp;

        if (NULL == entry_p || NULL == entry_p->name)
        {
            continue;
        }

        cmp = CLI_LIB_StrCmp(str, entry_p->name, &matched);
        if (0 == cmp)
        {
            *mode_p = i;
            return TRUE;
        }
        else if (cmp < 0 && matched == len)
        {
            enum CLI_ACCESS_MODE_E j;

            for (j=i+1; j<CLI_ACCESS_MODE_MAX_NUM; ++j)
            {
                CLI_COMMAND_TREE_ROOT_ENTRY *next_p = CLI_LIB_GetCommandTreeRoot(j);

                if (NULL == next_p || NULL == next_p->name)
                {
                    continue;
                }

                CLI_LIB_StrCmp(str, next_p->name, &matched);
                if (matched == len)
                {
                    return FALSE; /* Ambiguous */
                }
            }

            *mode_p = i;
            return TRUE;
        }
    }

    return FALSE;
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_GetNextCommandMode
 * ------------------------------------------------------------------------
 * PURPOSE  : Enumerates all command mode and string of mode.
 * INPUT    : key_p     - Index value. Pass it as 0 to start enumerate.
 *            buff_size - The buffer size of mode_str.
 * OUTPUT   : mode_p    - Command mode.
 *            mode_str  - String of command mode.
 * RETURN   : TRUE if output a command mode; otherwise return FALSE
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_GetNextCommandMode(UI32_T *key_p,
                           enum CLI_ACCESS_MODE_E *mode_p,
                           char *mode_str,
                           UI32_T buff_size)
{
    enum CLI_ACCESS_MODE_E idx;
    CLI_COMMAND_TREE_ROOT_ENTRY *entry_p;

    if (NULL == key_p || NULL == mode_p || 0 == buff_size)
    {
        return FALSE;
    }

    idx  = *key_p;
    entry_p = CLI_LIB_GetNextCommandTreeRoot(&idx);

    *key_p = idx;

    /* no next
     */
    if (NULL == entry_p)
    {
        return FALSE;
    }

    *mode_p = entry_p->acc_mode;
    strncpy(mode_str, entry_p->name, buff_size-1);
    mode_str[buff_size-1] = '\0';

    return TRUE;
}

/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_GetCommandModeString
 * -------------------------------------------------------------------------
 * FUNCTION: Get command mode string.
 * INPUT   : mode           - Command mode.
 *           buff_size_p    - The buffer size of mode_str. Pass 0 to test
 *                            how many size need for the string of mode.
 * OUTPUT  : mode_size      - String of mode.
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 * -------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_GetCommandModeString(enum CLI_ACCESS_MODE_E mode,
                             char *mode_str,
                             UI32_T *buff_size_p)
{
    const CLI_COMMAND_TREE_ROOT_ENTRY *entry_p = CLI_LIB_GetCommandTreeRoot(mode);
    UI32_T namelen;

    if (NULL == buff_size_p)
    {
        return FALSE;
    }

    if (NULL == entry_p)
    {
        *buff_size_p = 0;
        return FALSE; /* invalid command mode */
    }

    if (NULL == entry_p->name)
    {
        if (*buff_size_p == 0)
        {
            return FALSE;   /* out of memory */
        }

        mode_str[0] = '\0';
        return TRUE;
    }

    namelen = strlen( entry_p->name );

    if (*buff_size_p < namelen + 1 || NULL == mode_str)
    {
        *buff_size_p = namelen + 1;
        return FALSE;   /* out of memory */
    }

    memcpy(mode_str, entry_p->name, namelen);
    mode_str[namelen] = '\0';
    return TRUE;
}

/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_GetCommandModeStringPtr
 * -------------------------------------------------------------------------
 * FUNCTION: Get command mode string pointer.
 * INPUT   : mode           - Command mode.
 *           buff_size_p    - The buffer size of mode_str. Pass 0 to test
 *                            how many size need for the string of mode.
 * OUTPUT  : mode_size      - String of mode.
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 * -------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_GetCommandModeStringPtr(enum CLI_ACCESS_MODE_E mode,
                                char ** mode_str_p,
                                char ** help_msg_p)
{
    const CLI_COMMAND_TREE_ROOT_ENTRY *entry_p = CLI_LIB_GetCommandTreeRoot(mode);

    if (NULL == entry_p || NULL == entry_p->name)
    {
        *mode_str_p = NULL;
        *help_msg_p = NULL;

        return FALSE;
    }

    *mode_str_p = entry_p->name;
    *help_msg_p = entry_p->help_msg;

    return TRUE;
}



#if (SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL == TRUE)
/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_SetCommandPrivilegeRecursive
 * -------------------------------------------------------------------------
 * FUNCTION: Set privilege value to specified command and its sub-commander
 * INPUT   : cmd_list       - command list
 *           cmd_idx        - command index in command list
 *           new_privilege  - New privilege.
 *                            Pass CLI_DEF_RETURN_DEFAULT_PRIVILEGE_LEVEL
 *                            to reset privilege.
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    : None
 * -------------------------------------------------------------------------
 */
static void
CLI_LIB_SetCommandPrivilegeRecursive(CMDLIST *cmd_list,
                                     UI16_T  cmd_idx,
                                     UI8_T  new_privilege)
{
    while( cmd_idx != NULL_CMD )
    {
        if( CLI_DEF_RETURN_DEFAULT_PRIVILEGE_LEVEL == new_privilege)
        {
            cmd_list[ cmd_idx ].current_privilege = cmd_list[ cmd_idx ].default_privilege;
        }
        else
        {
            cmd_list[ cmd_idx ].current_privilege = new_privilege;
        }

        CLI_LIB_SetCommandPrivilegeRecursive(cmd_list, cmd_list[ cmd_idx ].child_idx, new_privilege);

        cmd_idx = cmd_list[ cmd_idx ].neibor_idx;
    }
}

struct SetCommandPrivilegeParam
{
    struct
    {
        const char          *line_string;
        struct
        {
            char * const    *at;
            UI32_T          length;
        } array;
    } pattern;

    enum CLI_ACCESS_MODE_E  mode;
    UI32_T                  new_privilege;
    UI32_T                  modify_counter;  /* output */

    BOOL_T                  incl_subcmd;

    struct
    {
        BOOL_T              match_in_argument_tree;
    } flag;
};

/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_Stub_SetCommandPrivilege
 * -------------------------------------------------------------------------
 * FUNCTION: Set command privilege
 * INPUT   : argv           - Track array
 *           argc           - Length of track array
 *           param_p        - Input parameter
 * OUTPUT  : None
 * RETURN  : TRUE if succeeded, or FALSE if happen an error.
 * NOTE    : None
 * -------------------------------------------------------------------------
 */
static BOOL_T
CLI_LIB_Stub_SetCommandPrivilege(CLI_TrackEntry_T argv[],
                                 UI32_T argc,
                                 struct SetCommandPrivilegeParam *param_p)
{
    CLI_TrackEntry_T *curr_node_p = &argv[ argc-1 ];
    UI32_T i;
    BOOL_T matched;

    if (0 == argc)
    {
        return FALSE;
    }

    if (NULL == param_p)
    {
        return FALSE;
    }

    if (argc < param_p->pattern.array.length)
    {
        return TRUE;
    }

    if (NT_ARGUMENT == curr_node_p->type)
    {
        if (TRUE == param_p->flag.match_in_argument_tree)
        {
            return TRUE;
        }

        /* for error flag, continue to process argument node
         */
    }

    /* reject some sensitive commands
     */
    if (NT_COMMAND == argv[0].type)
    {
        enum
        {
            ALL_MODE = CLI_ACCESS_MODE_MAX_NUM
        };

        struct
        {
            enum CLI_ACCESS_MODE_E mode;
            char *command;
        } reject_list[] =
        {
            {PRIVILEGE_CFG_GLOBAL_MODE, "enable"},
            {PRIVILEGE_CFG_GLOBAL_MODE, "privilege"},
            {NORMAL_EXEC_MODE,          "enable"},
            {PRIVILEGE_EXEC_MODE,       "enable"},
            {PRIVILEGE_EXEC_MODE,       "disable"},
            {ALL_MODE,                  "exit"},
            {ALL_MODE,                  "end"},
            {ALL_MODE,                  "quit"},
            {ALL_MODE,                  "help"},
        };

        for (i=0; i<_countof(reject_list); ++i)
        {
            if (reject_list[i].mode == ALL_MODE ||
                reject_list[i].mode == param_p->mode)
            {
                if (0 == strcmp(argv[0].cmd_list[ argv[0].idx ].name, reject_list[i].command))
                {
                    return TRUE;
                }
            }
        }
    }

    if (0 < param_p->pattern.array.length)
    {
        /* Match each command/argument from tail
         */
        matched = FALSE;
        i = 1;
        while (i <= argc && i <= param_p->pattern.array.length)
        {
            CLI_TrackEntry_T *node_p = &argv[argc-i];
            const char *cmd_str = node_p->type == NT_ARGUMENT ?
                node_p->arg_list[ node_p->idx ].name :
                node_p->cmd_list[ node_p->idx ].name ;
            const char *word = param_p->pattern.array.at[ param_p->pattern.array.length-i ];

            UI16_T match;
            I16_T cmp = CLI_LIB_StrCmp(cmd_str, word, &match);

            if (0 == cmp || (0 < cmp && match == strlen(word)))
            {
                ++i;
                matched = TRUE;
            }
            else
            {
                matched = FALSE;
                break;
            }
        }
    }
    else
    {
        matched = TRUE;
    }


    if (matched)
    {
        if (NT_COMMAND == curr_node_p->type)
        {
            if (CLI_DEF_RETURN_DEFAULT_PRIVILEGE_LEVEL == param_p->new_privilege)
            {
                curr_node_p->cmd_list[ curr_node_p->idx ].current_privilege =
                    curr_node_p->cmd_list[ curr_node_p->idx ].default_privilege;
            }
            else
            {
                curr_node_p->cmd_list[ curr_node_p->idx ].current_privilege =
                    param_p->new_privilege;
            }

            if (param_p->incl_subcmd)
            {
                CLI_LIB_SetCommandPrivilegeRecursive(curr_node_p->cmd_list,
                                                     curr_node_p->cmd_list[ curr_node_p->idx ].child_idx,
                                                     param_p->new_privilege);
            }

            param_p->modify_counter += 1;

#ifdef _DEBUG
            CLI_LIB_Stub_DumpCommandTree(argv, argc, NULL);
#endif
        }
        else if (NT_ARGUMENT == curr_node_p->type)
        {
            param_p->flag.match_in_argument_tree = TRUE;
        }
    }

    return TRUE;
}


/* -------------------------------------------------------------------------
 * ROUTINE NAME - CLI_LIB_SetCommandPrivilege
 * -------------------------------------------------------------------------
 * FUNCTION: Set command privilege
 * INPUT   : mode           - which mode
 *           privilege      - New privilege for matched command
 *           pattern        - match pattern. Like 'show ip' or 'sh ip'
 *           incl_subcmd    - also modify for sub-command
 * OUTPUT  : None
 * RETURN  : TRUE if succeeded, or FALSE if happen an error or no command
 *           have matched by input pattern string.
 * NOTE    : None
 * -------------------------------------------------------------------------
 */
BOOL_T
CLI_LIB_SetCommandPrivilege(const char *mode,
                            UI32_T privilege,
                            const char *pattern,
                            BOOL_T incl_subcmd)
{
    CLI_TrackEntry_T node;
    enum CLI_ACCESS_MODE_E command_mode;
    struct SetCommandPrivilegeParam param;

    char *pattern_dup;
    char *argv[100];
    UI32_T argc = sizeof(argv)/sizeof(*argv);

    if (CLI_DEF_RETURN_DEFAULT_PRIVILEGE_LEVEL < privilege)
    {
        return FALSE;
    }

    if (TRUE != CLI_LIB_GetCommandMode(mode, &command_mode))
    {
        return FALSE;
    }

    pattern_dup = strdup(pattern);
    if (!pattern_dup)
    {
        return FALSE;
    }

    CLI_LIB_SplitString(pattern_dup, argv, &argc);

    node.type = NT_COMMAND;
    node.cmd_list = CLI_LIB_Get_Cmd_List_By_Privilege(command_mode);
    node.idx = 0;

    memset(&param, 0, sizeof(param));

    param.pattern.line_string = pattern_dup;
    param.pattern.array.length = argc;
    param.pattern.array.at = &argv[0];
    param.mode          = command_mode;
    param.new_privilege = privilege;
    param.incl_subcmd = incl_subcmd;

    CLI_LIB_Inorder(&node, WALK_COMMAND_ARGUMENT, 100, (VisitorFuncPtr)CLI_LIB_Stub_SetCommandPrivilege, &param);

    free(pattern_dup);

    return (param.modify_counter) ? TRUE : FALSE;
}

#endif    /* SYS_CPNT_CLI_MULTI_PRIVILEGE_LEVEL */



/*
 * FUNCTION NAME: CLI_LIB_Get_PortList
 * PURPOSE: Parse the port list string when user type ethernet interface
 *          UNIT/PORTLIST like "1/1,3,5-7" in CLI command.
 * INPUT:   port_list_p -- Port list string like "1,3,5-7".
 * OUTPUT:  port_index_p -- Boolean port index array.
 * RETURN:  count -- Total number of ports in port-list string.
 * NOTES:   For Example,
 *          INPUT -> "1,3,5-7"
 *          OUTPUT -> "TFTFTTTFFF..." (boolean array)
 *          RETURN -> 5
 */
UI32_T CLI_LIB_Get_PortList(char* port_list_p, BOOL_T* port_index_p)
{
    UI32_T port = 0;
    UI32_T count = 0;
    UI32_T lower_bound = 0;
    UI32_T upper_bound = 0;
    char* str1_p = NULL;
    char* str2_p = NULL;
    char str_ar[CLI_DEF_MAX_BUFSIZE];

    strcpy(str_ar, port_list_p);
    memset(port_index_p, FALSE, SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT * sizeof(BOOL_T));

    str1_p = strtok(str_ar, ",");
    while(str1_p != NULL)
    {
        lower_bound = atoi(str1_p);
        if(strchr(str1_p, '-') == NULL) /* case ...,11,... */
        {
            upper_bound = lower_bound;
        }
        else /* case ...,11-13,... */
        {
            str2_p = strchr(str1_p, '-') + 1;
            upper_bound = atoi(str2_p);
        }
        for(port = lower_bound - 1; port < upper_bound; port++)
        {
            port_index_p[port] = TRUE;
        }
        count = count + (upper_bound - lower_bound + 1);
        str1_p = strtok(NULL, ",");
    }

    return count;
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_MaxLineNumPerScreen
 * ------------------------------------------------------------------------
 * PURPOSE  : Get max line number of screen
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : line number
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
UI32_T CLI_LIB_MaxLineNumPerScreen()
{
    return CLI_DEF_MAX_LINENUM;
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_SetFilteringTerminalDefault
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to set filtering and terminal fields
 *            as default values
 * INPUT    : CLI_TASK_WorkingArea_T *ctrl_p
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
void CLI_LIB_SetFilteringTerminalDefault(CLI_TASK_WorkingArea_T *ctrl_p)
{
#if (SYS_CPNT_CLI_FILTERING == TRUE)
    ctrl_p->option_flag = CLI_LIB_OPTION_NONE;
#endif /*#if (SYS_CPNT_CLI_FILTERING == TRUE)*/

#if (SYS_CPNT_CLI_TERMINAL == TRUE)
    ctrl_p->CMenu.length = CLI_DEF_MAX_LINENUM;
    ctrl_p->CMenu.width = CLI_DEF_DISP_WIDTH;
    ctrl_p->CMenu.histsize = CLI_DEF_HIST;
    ctrl_p->CMenu.terminal_type = CLI_DEF_TERMINAL_TYPE;
    ctrl_p->CMenu.escape_character = CLI_DEF_ESCAPE;
#endif  /* #if (SYS_CPNT_CLI_TERMINAL == TRUE) */
}

#endif
#pragma mark #endif
#pragma mark -

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_GetMaxTerminalLineNum
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to get terminal max line number
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : line number
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
UI32_T CLI_LIB_GetMaxTerminalLineNum()
{
#if (SYS_CPNT_CLI_TERMINAL == TRUE)

    CLI_TASK_WorkingArea_T  *ctrl_P = (CLI_TASK_WorkingArea_T *)CLI_TASK_GetMyWorkingArea();

    return (ctrl_P->CMenu.length);

#else

    return CLI_DEF_MAX_LINENUM;

#endif  /* #if (SYS_CPNT_CLI_TERMINAL == TRUE) */
}

#pragma mark -
#pragma mark #if 0
#if 0

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_HostStringToInaddr
 * ------------------------------------------------------------------------
 * PURPOSE  : This function is used to convert host string to inet address
 * INPUT    : hoststr_p
 *            family - AF_INET(V4), AF_INET6(V6), AF_UNSPEC(V4/V6)
 * OUTPUT   : inaddr_p
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_HostStringToInaddr(UI8_T *hoststr_p, UI32_T family, L_INET_AddrIp_T *inaddr_p)
{
#if (SYS_CPNT_DNS == TRUE)
    L_INET_AddrIp_T hostip_ar[SYS_ADPT_MAX_NBR_OF_DNS_HOST_IP];

    memset(hostip_ar, 0, sizeof(hostip_ar));

    if(DNS_OK == DNS_PMGR_HostNameToIp(hoststr_p, family, hostip_ar))
    {
        /* return the first ip as result, need to refine in the future.
         */
        memcpy(inaddr_p, &hostip_ar[0], sizeof(hostip_ar[0]));
        return TRUE;
    }
#endif  /* #if (SYS_CPNT_DNS == TRUE) */

    return FALSE;
}

#endif
#pragma mark #endif
#pragma mark -

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_LIB_GetUnitPortFromString
 * ------------------------------------------------------------------------
 * PURPOSE  : Get unit and port number from a PVC_DATA string
 * INPUT    : str     - a PVC_DATA string
 * OUTPUT   : unit_p  - unit
 *            port_p  - port
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_LIB_GetUnitPortFromString(const char *str, UI32_T *unit_p, UI32_T *port_p)
{
    char *eth;
    char *up;
    char *pp;

    if (NULL == str)
    {
        return FALSE;
    }

    eth = calloc(strlen(str) + 1, 1);
    if (NULL == eth)
    {
        return FALSE;
    }

    strncpy(eth, str, strlen(str));

    up = eth;
    pp = strchr(eth, '/');

    /* no /
     */
    if (NULL == pp)
    {
        free(eth);
        return FALSE;
    }

    /* no unit
     */
    if (up == pp)
    {
        free(eth);
        return FALSE;
    }

    *pp = '\0';
    pp ++;

    /* no port
     */
    if ('\0' == *pp)
    {
        free(eth);
        return FALSE;
    }

    /* unit or port is not digit
     */
    if (FALSE == L_STDLIB_StrIsDigit(up) ||
        FALSE == L_STDLIB_StrIsDigit(pp))
    {
        free(eth);
        return FALSE;
    }

    *unit_p = (UI32_T)atoi(up);
    *port_p = (UI32_T)atoi(pp);

    free(eth);
    return TRUE;
}

#pragma mark -
#pragma mark #if 0
#if 0

#if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE)
/* FUNCTION NAME - CLI_LIB_SW_WatchDogRoutine
 * PURPOSE  : This function is used for the software watch dog routine.
 * INPUT    : ctrl_p
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 */
void CLI_LIB_SwWatchDogRoutine(CLI_TASK_WorkingArea_T *ctrl_p)
{
    UI32_T rcv_event, wait_events;

    if(ctrl_p==NULL)
    {
        printf("\r\n%s(): ctrl_p is NULL.\r\n", __FUNCTION__);
        return;
    }

    wait_events = SYSFUN_SYSTEM_EVENT_SW_WATCHDOG;

    SYSFUN_ReceiveEvent(wait_events, SYSFUN_EVENT_WAIT_ANY, SYSFUN_TIMEOUT_NOWAIT, &rcv_event);

    if (rcv_event & SYSFUN_SYSTEM_EVENT_SW_WATCHDOG)
    {
        SW_WATCHDOG_MGR_ResetTimer(ctrl_p->monitor_id);
    }
}
#endif /* #if (SYS_CPNT_SW_WATCHDOG_TIMER == TRUE) */

#endif
#pragma mark #endif
#pragma mark -


UI32_T CLI_LIB_MaxLineNumPerScreen()
{
    return 80;
}

