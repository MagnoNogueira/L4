/* MODULE NAME: l4_backdoor.c
 * PURPOSE:
 *  Implement l4 backdoor
 *
 * NOTES:
 *
 * History:
 *    2005/07/22 : mfhorng      Create this file
 *
 * Copyright(C)      Accton Corporation, 2005
 */
 
 /* Note
  * 1. Macro "L4_UT" shall only defined on unit test environment,
  *    for passing through the CoS backdoor function.
  * 2. FIXME: "ci" shall be fixed.
  */
/* INCLUDE FILE DECLARATIONS
 */
#ifdef _MSC_VER
#pragma warning(disable:4100) /* unreferenced formal parameter */
#pragma warning(disable:4127)   /* conditional expression is constant */
#endif

#include "l4_backdoor.h"

#if (L4_SUPPORT_ACCTON_BACKDOOR == TRUE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys_cpnt.h"
#include "sys_type.h"
#include "sys_bld.h"
#include "l_threadgrp.h"
#include "l_inet.h"
#include "l_stdlib.h"
#include "backdoor_mgr.h"
#include "l4_mgr.h"
#include "rule_om.h"
#include "rule_ctrl.h"
#include "sysfun.h"
#include "l2_l4_proc_comm.h"
#include "pri_mgr.h"

#if (SYS_CPNT_DOS == TRUE)
#include "swdrv_type.h"
#endif /* SYS_CPNT_DOS */

/* NAMING CONSTANT DECLARATIONS
 */
#define L4_BACKDOOR_PROMPT        "\r\ncommand > "

/* TODO: Use shorted name
 */
#define L4_BACKDOOR_MAX_PARAMETER_VALUE_STR_LEN 125


#define _S_OK                                   (1)
#define _E_FAILED                               (-1)
#define _E_MISS_ARG                             (-2)
#define _E_ERR_PARAM                            (-3)
#define _E_AMBIGUOUS                            (-4)
#define _FAILED(code)                           (code < 0)
#define _SUCCEEDED(code)                        (0 <= code)

#define _NO_ARG                                 0
#define _REQUIRED_ARG                           1
#define _OPTIONAL_ARG                           2

#define _REQUIRED_PARAM                         1
#define _OPTIONAL_PARAM                         2

#define _ARG_FIX                                1
#define _ARG_INC                                2
#define _ARG_DEC                                3

#define _OPT_INSTALL                            0x00000001
#define _OPT_UNINSTALL                          0x00000002
#define _OPT_SHOW                               0x00000004
#define _OPT_DEBUG                              0x00000008
#define _OPT_HELP                               0x00000010
#define _OPT_HELP_SYNTAX                        0x00000020
#define _OPT_HELP_COMMAND                       0x00000040
#define _OPT_HELP_COMMAND_AND_ITS_PARAMETERS    0x00000080
#define _OPT_DFLT_VAL_AS_MISS_REQ_PARAMETER     0x00000100
#define _OPT_DFLT_VAL_AS_ALL_MISS_PARAMETER     0x00000200

#define _T_NONE                                 0
#define _T_INTEGER                              1
#define _T_STRING                               2
#define _T_MAC_ADDR                             3
#define _T_IN_ADDR                              4
#define _T_IP4_ADDR                             5
#define _T_IP6_ADDR                             6


/* MACRO FUNCTION DECLARATIONS
 */
#define GET_TOKEN_AND_MOVE_NEXT(str, token, pos)  \
 {                                                \
    *pos = '\0'; token = str; str = pos + 1;      \
    while (*str == ' ') { str++; }                \
 }

#define IS_DIGIT(c)                     (('0' <= c) && (c <= '9'))
#define IS_UPALPHA(c)                   (('A' <= c) && (c <= 'Z'))
#define IS_LOALPHA(c)                   (('a' <= c) && (c <= 'z'))
#define IS_ALPHA(c)                     (IS_UPALPHA(c) || IS_LOALPHA(c))
#define CHECK_MAC_IS_NULL(mac) ((mac[0]|mac[1]|mac[2]|mac[3]|mac[4]|mac[5])==0)  /*added by Jinhua.Wei,to remove warning*/

#ifndef _countof
#define _countof(ar)                            (sizeof(ar)/sizeof(*ar))
#endif

/* DATA TYPE DECLARATIONS
 */
typedef void (*L4FuncPtr)(int argc, char** argv);

typedef struct L4_Backdoor_Cmd_S
{
    const char*     cmd;
    L4FuncPtr       exec;
} L4_Backdoor_Cmd_T;

typedef struct L4_Backdoor_Menu_S
{
    const char*     name;
    const int       nbr_of_sub_menus;
    const int       nbr_of_cmd_items;

    struct L4_Backdoor_Menu_S *parent_menu;
    struct L4_Backdoor_Menu_S **sub_menus;
    L4_Backdoor_Cmd_T         *cmd_items;
} L4_Backdoor_Menu_T;

/* NOTE.
 * If CHANGE this enum, it also need to CHANGE the opposite value to
 * l4_backdoor_def_param_table[PARAM_COUNT]
 * l4_backdoor_default_parameters[PARAM_COUNT]
 */
enum
{
    PARAM_UNIT  = 0,
    PARAM_PORT,
    PARAM_SA,
    PARAM_DA,
    PARAM_VID,
    PARAM_ETHTYPE,
    PARAM_SRC_IP,
    PARAM_DST_IP,
    PARAM_SRC_IP6,
    PARAM_DST_IP6,
    PARAM_L4_DST_PORT,
    PARAM_DEVICE,
    PARAM_GROUP,
    PARAM_FUNC_TYPE,
    PARAM_TAG,
    PARAM_COS,
    PARAM_TO_CPU,       /* Unknown IP/IP6 Multicast */
    PARAM_FLOOD,        /* Unknown IP/IP6 Multicast */
    PARAM_TRANSITION,   /* EAPS */
    PARAM_REDIR,        /* SLF */
    PARAM_DROP,         /* SLF */
    PARAM_HELP,
    PARAM_COUNT
};

typedef struct
{
    char    *arg;
    int     ind;
    int     opt;

    int     __sp;
} L4_BACKDOOR_Opt_T;

typedef struct
{
    char    *name;
    char    has_arg;
    int     *flag;
    int     val;
    int     arg_type;
} L4_BACKDOOR_Option_T;

typedef struct
{
    char    *name;
    char    *desc;
    BOOL_T  (*exec) (void*, UI32_T, void*);
    UI32_T  param_num;
    struct
    {
        UI32_T  index;
        UI32_T  required;       /* _REQUIRED_PARAM or _OPTIONAL_PARAM */
        UI32_T  vis_flag;
    } parameters[PARAM_COUNT * 2]; /* TODO: x2 OR x 3(user flags) */
} L4_BACKDOOR_Command_T;

typedef struct
{
    BOOL_T  valid;
    BOOL_T  has_value;
    char    value[L4_BACKDOOR_MAX_PARAMETER_VALUE_STR_LEN+1];
} L4_BACKDOOR_Parameter_T;

typedef BOOL_T (*L4_BACKDOOR_DummyCommandFuncPtr_T) (
    void * command,
    UI32_T options,
    void * parameters
);

typedef BOOL_T (*L4_BACKDOOR_CommandFuncPtr_T)(
    L4_BACKDOOR_Command_T * command,
    UI32_T options,
    L4_BACKDOOR_Parameter_T * parameters
);

/* LOCAL SUBPROGRAM DECLARATIONS
 */

/*------------------------------------------------------------------------------
 * common functions
 *------------------------------------------------------------------------------
 */
#ifdef RUN_UNIT_TEST
static void L4_Backdoor_UnitTest();
#endif

static void L4_Backdoor_ShowIpAddr(UI8_T *addr);
static BOOL_T L4_Backdoor_Str2IpAddr(const char *str, UI8_T *addr);
static BOOL_T L4_Backdoor_Str2Ip6Addr(const char *str, UI8_T *addr);

static void L4_Backdoor_ShowMacAddr(UI8_T *addr);
static BOOL_T L4_Backdoor_Str2MacAddr(const char *str, UI8_T *addr);

static void L4_Backdoor_ShowRowStatus(UI32_T row_status);

static void L4_Backdoor_Help(int argc, char** argv);
static int L4_Backdoor_StrCmpIgnoreCase(const char *dst, const char *src);
static UI32_T L4_Backdoor_StrCmpIgnoreCase2(const char *dst, const char *src);
static BOOL_T L4_Backdoor_ChopString(char *str);
static char* L4_Backdoor_StrStrIgnoreCase(const char *str, const char *substr);

static void L4_Backdoor_DumpRawDataInHex(const UI8_T *data, UI32_T size, BOOL_T dont_dump_if_empty);

/*------------------------------------------------------------------------------
 * backdoor command functions
 *------------------------------------------------------------------------------
 */
/* cpu interface backdoor command functions */
static void L4_Backdoor_CpuIf_TrapPacket2Cpu(int argc, char** argv);
static void L4_Backdoor_CpuIf_ShowStatus(int argc, char** argv);
static void L4_Backdoor_CpuIf_DHCPSnooping2Cpu(int argc, char** argv);
static void L4_Backdoor_CpuIf_IPSrcGuard(int argc, char** argv);
static void L4_Backdoor_CpuIf_IPSrcGuardDenyIpAny(int argc, char** argv);
static void L4_Backdoor_CpuIf_IPv6SrcGuard(int argc, char** argv);
static void L4_Backdoor_CpuIf_IPv6SrcGuardPermitLinkLocal(int argc, char** argv);
static void L4_Backdoor_CpuIf_IPv6SrcGuardDenyIpv6Any(int argc, char** argv);
static void L4_Backdoor_CpuIf_WEBAUTH_RedirectHTTPClient(int argc, char** argv);
static void L4_Backdoor_CpuIf_WEBAUTH_RermitSrcIP(int argc, char** argv);
static void L4_Backdoor_CpuIf_WEBAUTH_DenyIP(int argc, char** argv);
static void L4_Backdoor_CpuIf_SetPortVlanTranslation(int argc, char** argv);
static void L4_Backdoor_CpuIf_SetMacBasedVLan(int argc, char** argv);
static void L4_Backdoor_CpuIf_SetProtocolBasedVLan(int argc, char** argv);
static void L4_Backdoor_CpuIf_SetVoiceVLan(int argc, char** argv);
static void L4_Backdoor_CpuIf_SetIpSubnetVlan(int argc, char** argv);
static void L4_Backdoor_CpuIf_ModifieCpuQueueForMyMacMyIp(int argc, char** argv);
static void L4_Backdoor_CpuIf_TrapArpPacketToCpuWithRate(int argc, char** argv);
static void L4_Backdoor_CpuIf_DosProtectionRateLimit(int argc, char** argv);

static void L4_Backdoor_TcpUdpPortMapping(int argc, char** argv);
static void L4_Backdoor_DHCP6Cient2CPU(int argc, char** argv);
static void L4_Backdoor_DHCP6Server2CPU(int argc, char** argv);
static void L4_Backdoor_TrapMLD2CPU(int argc, char** argv);
static void L4_Backdoor_UnknownIPv6MCast2CPU(int argc, char** argv);
static void L4_Backdoor_CpuIf_TrapLinkLocalAddress2Cpu(int argc, char** argv);
static void L4_Backdoor_CpuIf_DumpIpsgDb(int argc, char** argv);
static void L4_Backdoor_CpuIf_DumpFunctionInfo(int argc, char** argv);
static void L4_Backdoor_CpuIf_DumpOamInfo(int argc, char** argv);
static void L4_Backdoor_CpuIf_TrapOamPdu2Cpu(int argc, char** argv);
static void L4_Backdoor_CpuIf_SetOamLoopbk2RecPort(int argc, char** argv);
static void L4_Backdoor_CpuIf_TrapPIM62Cpu(int argc, char** argv);
static void L4_Backdoor_CpuIf_TrapUknIPv6McData2Cpu(int argc, char** argv);
static void L4_Backdoor_CpuIf_SQinQ(int argc, char** argv);
static void L4_Backdoor_CpuIf_Ci(int argc, char** argv);
static void L4_Backdoor_CpuIf_Set(int argc, char** argv);

/* diffserv backdoor command functions */
static void L4_Backdoor_DiffServ_SetDebugFlag(int argc, char** argv);
static void L4_Backdoor_DiffServ_ShowPolicyMapTable(int argc, char** argv);
static void L4_Backdoor_DiffServ_ShowPolicyMapElementTable(int argc, char** argv);
static void L4_Backdoor_DiffServ_ShowClassMapTable(int argc, char** argv);
static void L4_Backdoor_DiffServ_ShowAclTable(int argc, char** argv);
static void L4_Backdoor_DiffServ_ShowAceTable(int argc, char** argv);
static void L4_Backdoor_DiffServ_ShowMeterTable(int argc, char** argv);
static void L4_Backdoor_DiffServ_ShowActionTable(int argc, char** argv);
static void L4_Backdoor_DiffServ_ShowPortTable(int argc, char** argv);
static void L4_Backdoor_DiffServ_ShowResourceInfo(int argc, char** argv);
static void L4_Backdoor_DiffServ_ShowCosReference(int argc, char** argv);

#if (SYS_CPNT_ACL_MIRROR == TRUE)
static void L4_Backdoor_DiffServ_AddAclMirror(int argc, char** argv);
static void L4_Backdoor_DiffServ_RemoveAclMirror(int argc, char ** argv);
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

#ifndef L4_UT
/* cos backdoor command functions */
static void L4_Backdoor_Cos_CosMode(int argc, char **argv);
static void L4_Backdoor_Cos_NoCosMode(int argc, char **argv);
static void L4_Backdoor_Cos_ShowCosMode(int argc, char **argv);
static void L4_Backdoor_Cos_Dscp2Cos(int argc, char **argv);
static void L4_Backdoor_Cos_NoDscp2Cos(int argc, char **argv);
static void L4_Backdoor_Cos_TcpPort2Cos(int argc, char **argv);
static void L4_Backdoor_Cos_NoTcpPort2Cos(int argc, char **argv);
static void L4_Backdoor_Cos_Precedence2Cos(int argc, char **argv);
static void L4_Backdoor_Cos_NoPrecedence2Cos(int argc, char **argv);
static void L4_Backdoor_Cos_Cos2Queue(int argc, char **argv);
static void L4_Backdoor_Cos_NoCos2Queue(int argc, char **argv);
static void L4_Backdoor_Cos_ShowCosMapping(int argc, char **argv);
static void L4_Backdoor_Cos_ShowPriorityCosMappingTable(int argc, char **argv);
static void L4_Backdoor_Cos_SetDebugMode(int argc, char **argv);
#endif /* L4_UT */

static int L4_Backdoor_CpuIf_installed_rules(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_l4_dport_cos(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_dot1x(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_ipsg_deny_ip(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_ipsg_permit_ip(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_wa_trap_http(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_wa_deny_ip(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_wa_permit_ip(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_wa_permit_dhcp(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_wa_permit_dns(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_erps(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_oam_cpe_oam_pdu(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_oam_cpe_oam_loopback(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_oam_co_oam_loopback(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_link_local_addr(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_eaps(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_arp_req_copy_to_cpu(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_arp_req_redir_to_cpu(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_arp_reply_copy_to_cpu(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_arp_reply_redir_to_cpu(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_rip(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_ospf(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

#if (SYS_CPNT_OSPF6 == TRUE)
static int L4_Backdoor_CpuIf_ospf6(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );
#endif /* SYS_CPNT_OSPF6 */

static int L4_Backdoor_CpuIf_igmp(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_l2_slf(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_slf(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_ip_unknown_mc(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_ip_bcast(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_pim(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_vrrp(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_ip_all_host(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_ip_all_routers(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_ip_option(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_hbt(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_bpdu(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_l2cp(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

#if (SYS_CPNT_CFM == TRUE)
static int L4_Backdoor_CpuIf_cfm(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );
#endif

static int L4_Backdoor_CpuIf_ip6_all_nodes(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_ip6_all_routers(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_pim6(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

static int L4_Backdoor_CpuIf_ip6_unknown_mc(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters_p
    );

/* STATIC VARIABLE DECLARATIONS
 */

/*------------------------------------------------------------------------------
 * backdoor menu tree
 *------------------------------------------------------------------------------
 */

/* cpu interface backdoor command items */
static L4_Backdoor_Cmd_T  cpu_if_command_items[] = {
    {"help",                        L4_Backdoor_Help},
    {"trap_packet_to_cpu",          L4_Backdoor_CpuIf_TrapPacket2Cpu},
    {"show_cpuif_status",           L4_Backdoor_CpuIf_ShowStatus},
    {"dump function info",          L4_Backdoor_CpuIf_DumpFunctionInfo},
    {"dhcpsnp redirect dhcp",       L4_Backdoor_CpuIf_DHCPSnooping2Cpu},
    {"[IPSG] permit host",          L4_Backdoor_CpuIf_IPSrcGuard},
    {"[IPSG] deny ip",              L4_Backdoor_CpuIf_IPSrcGuardDenyIpAny},

#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
    {"[IPv6SG] permit host",        L4_Backdoor_CpuIf_IPv6SrcGuard},
    {"[IPv6SG] permit link local",  L4_Backdoor_CpuIf_IPv6SrcGuardPermitLinkLocal},
    {"[IPv6SG] deny ip",            L4_Backdoor_CpuIf_IPv6SrcGuardDenyIpv6Any},
#endif /* SYS_CPNT_IPV6_SOURCE_GUARD */

    {"webauth redirect http client",L4_Backdoor_CpuIf_WEBAUTH_RedirectHTTPClient},
    {"webauth permit sip",          L4_Backdoor_CpuIf_WEBAUTH_RermitSrcIP},
    {"webauth deny ip",             L4_Backdoor_CpuIf_WEBAUTH_DenyIP},

#if (SYS_CPNT_VLAN_XLATE == TRUE && SYS_CPNT_VLAN_XLATE_BY_RULE == TRUE)
    {"vlan translation",            L4_Backdoor_CpuIf_SetPortVlanTranslation},
#endif /* #if (SYS_CPNT_VLAN_XLATE == TRUE && SYS_CPNT_VLAN_XLATE_BY_RULE == TRUE) */
#if (SYS_CPNT_MAC_VLAN == TRUE) && (SYS_CPNT_MAC_VLAN_IMPLEMENTED_BY_RULE == TRUE)
    {"Mac based vlan",              L4_Backdoor_CpuIf_SetMacBasedVLan},
#endif /* SYS_CPNT_MAC_VLAN && SYS_CPNT_MAC_VLAN_IMPLEMENTED_BY_RULE */
#if (SYS_CPNT_PROTOCOL_VLAN == TRUE) && (SYS_CPNT_PROTOCOL_VLAN_IMPLEMENTED_BY_RULE == TRUE)
    {"Protocol based vlan",         L4_Backdoor_CpuIf_SetProtocolBasedVLan},
#endif /* SYS_CPNT_PROTOCOL_VLAN && SYS_CPNT_PROTOCOL_VLAN_IMPLEMENTED_BY_RULE */
#if (SYS_CPNT_IP_SUBNET_VLAN == TRUE) && (SYS_CPNT_IP_SUBNET_VLAN_IMPLEMENTED_BY_RULE  == TRUE)
    #if (SYS_CPNT_IP_SUBNET_VLAN_BIND_PORT == FALSE)
    {"IP subnet vlan",              L4_Backdoor_CpuIf_SetIpSubnetVlan},
    #endif /* SYS_CPNT_IP_SUBNET_VLAN_BIND_PORT == FALSE */
#endif /* SYS_CPNT_IP_SUBNET_VLAN && SYS_CPNT_IP_SUBNET_VLAN_IMPLEMENTED_BY_RULE */
#if (SYS_CPNT_ADD_MODIFY_PRIORITY_BY_RULE)
    {"Voice vlan",                  L4_Backdoor_CpuIf_SetVoiceVLan},
#endif /* SYS_CPNT_ADD_MODIFY_PRIORITY_BY_RULE */
#if (SYS_ADPT_MODIFY_MGMT_QUEUE_BY_RULE == TRUE)
    {"my_mac_my_ip change queue",   L4_Backdoor_CpuIf_ModifieCpuQueueForMyMacMyIp},
#endif /* SYS_ADPT_MODIFY_MGMT_QUEUE_BY_RULE */
    {"dump ipsg db",                L4_Backdoor_CpuIf_DumpIpsgDb},
    {"trap_oam_pdu to cpu",         L4_Backdoor_CpuIf_TrapOamPdu2Cpu},
    {"set_oam_loopbk to rec port",  L4_Backdoor_CpuIf_SetOamLoopbk2RecPort},
    {"dump_oam_info",               L4_Backdoor_CpuIf_DumpOamInfo},
    {"trap_ukn_ipv6_mc to cpu",     L4_Backdoor_CpuIf_TrapUknIPv6McData2Cpu},
    {"tcp/udp port mapping",        L4_Backdoor_TcpUdpPortMapping},
    {"dhcp6 client to cpu",         L4_Backdoor_DHCP6Cient2CPU},
    {"dhcp6 server to cpu",         L4_Backdoor_DHCP6Server2CPU},
    {"mld to cpu",                  L4_Backdoor_TrapMLD2CPU},
    {"unknown ipv6 mcast to cpu",   L4_Backdoor_UnknownIPv6MCast2CPU},
    {"ipv6 link local to cpu",      L4_Backdoor_CpuIf_TrapLinkLocalAddress2Cpu},
    {"pim6 to cpu",                 L4_Backdoor_CpuIf_TrapPIM62Cpu},
    {"service QinQ",                L4_Backdoor_CpuIf_SQinQ},
#if (SYS_CPNT_DAI_RATE_LIMIT_BY_RULE == TRUE)
    {"arp to cpu with meter",       L4_Backdoor_CpuIf_TrapArpPacketToCpuWithRate},
#endif /* SYS_CPNT_DAI_RATE_LIMIT_BY_RULE */
#if (SYS_CPNT_DOS == TRUE)
    {"dos protection rate limit",   L4_Backdoor_CpuIf_DosProtectionRateLimit},
#endif /* SYS_CPNT_DOS */
    {"ci",                          L4_Backdoor_CpuIf_Ci},
    {"set",                         L4_Backdoor_CpuIf_Set},
};

/* cpu interface backdoor menu */
static L4_Backdoor_Menu_T cpu_if_menu[] = {
    {"cpu_interface", 0, sizeof(cpu_if_command_items)/sizeof(L4_Backdoor_Cmd_T), NULL, NULL, cpu_if_command_items},
};

/* diffserv backdoor command items */
static L4_Backdoor_Cmd_T  diffserv_command_items[] = {
    {"help",                            L4_Backdoor_Help},
    {"set_debug_flag",                  L4_Backdoor_DiffServ_SetDebugFlag},
    {"show_policy_map_table",           L4_Backdoor_DiffServ_ShowPolicyMapTable},
    {"show_policy_map_element_table",   L4_Backdoor_DiffServ_ShowPolicyMapElementTable},
    {"show_class_map_table",            L4_Backdoor_DiffServ_ShowClassMapTable},
    {"show_acl_table",                  L4_Backdoor_DiffServ_ShowAclTable},
    {"show_ace_table",                  L4_Backdoor_DiffServ_ShowAceTable},
    {"show_meter_table",                L4_Backdoor_DiffServ_ShowMeterTable},
    {"show_action_table",               L4_Backdoor_DiffServ_ShowActionTable},
    {"show_port_table",                 L4_Backdoor_DiffServ_ShowPortTable},
    {"show_resource_info",              L4_Backdoor_DiffServ_ShowResourceInfo},
    {"show_cos_reference_status",       L4_Backdoor_DiffServ_ShowCosReference},

#if (SYS_CPNT_ACL_MIRROR == TRUE)
    {"add_acl_mirror",                  L4_Backdoor_DiffServ_AddAclMirror},
    {"del_acl_mirror",                  L4_Backdoor_DiffServ_RemoveAclMirror},
#endif
};

/* diffserv backdoor menu */
static L4_Backdoor_Menu_T diffserv_menu[] = {
    {"diffserv", 0, sizeof(diffserv_command_items)/sizeof(L4_Backdoor_Cmd_T), NULL, NULL, diffserv_command_items},
};

/* cos backdoor command items */
static L4_Backdoor_Cmd_T  cos_command_items[] = {
    {"help", L4_Backdoor_Help},
#ifndef L4_UT
    {"cos_mode",                        L4_Backdoor_Cos_CosMode},
    {"no_cos_mode",                     L4_Backdoor_Cos_NoCosMode},
    {"show_cos_mode",                   L4_Backdoor_Cos_ShowCosMode},
    {"dscp_to_cos",                     L4_Backdoor_Cos_Dscp2Cos},
    {"no_dscp_to_cos",                  L4_Backdoor_Cos_NoDscp2Cos},
    {"tcp_port_to_cos",                 L4_Backdoor_Cos_TcpPort2Cos},
    {"no_tcp_port_to_cos",              L4_Backdoor_Cos_NoTcpPort2Cos},
    {"precedence_to_cos",               L4_Backdoor_Cos_Precedence2Cos},
    {"no_precedence_to_cos",            L4_Backdoor_Cos_NoPrecedence2Cos},
    {"cos_to_queue",                    L4_Backdoor_Cos_Cos2Queue},
    {"no_cos_to_queue",                 L4_Backdoor_Cos_NoCos2Queue},
    {"show_cos_mapping",                L4_Backdoor_Cos_ShowCosMapping},
    {"show_cos_priority_mapping_table", L4_Backdoor_Cos_ShowPriorityCosMappingTable},
    {"set cos debug mode",              L4_Backdoor_Cos_SetDebugMode},
#endif /* L4_UT */
};

/* cos backdoor menu */
static L4_Backdoor_Menu_T cos_menu[] = {
    {"cos", 0, sizeof(cos_command_items)/sizeof(L4_Backdoor_Cmd_T), NULL, NULL, cos_command_items},
};

/* l4 backdoor main command items */
static L4_Backdoor_Cmd_T  main_command_items[] = {
    {"help", L4_Backdoor_Help},
};

/* l4 backdoor sub menu of main menu */
static L4_Backdoor_Menu_T *sub_menu_of_main_menu[] = {
    cpu_if_menu,
    diffserv_menu,
    cos_menu,
};

/* l4 backdoor main menu */
static L4_Backdoor_Menu_T main_menu[] = {
    {"l4",
     sizeof(sub_menu_of_main_menu)/sizeof(L4_Backdoor_Menu_T*),
     sizeof(main_command_items)/sizeof(L4_Backdoor_Cmd_T),
     NULL,
     sub_menu_of_main_menu,
     main_command_items},
};

static struct _UserOption
{
    UI32_T  flag;
    char    *name;
    int     val;
} l4_backdoor_user_options[] =
    {
        {
            _OPT_INSTALL,
            "install",
            'i',
        },
        {
            _OPT_UNINSTALL,
            "uninstall",
            'u',
        },
        {
            _OPT_SHOW,
            "show",
            's'
        }
    };

static char *l4_backdoor_arg_format[] =
{
    {""},                   /* _T_NONE */
    {"dd"},                 /* _T_INTEGER */
    {"s..s"},               /* _T_STRING */
    {"x-x-x-x-x-x"},        /* _T_MAC_ADDR */
    {"d.d.d.d | x::x"},     /* _T_IN_ADDR */
    {"d.d.d.d"},            /* _T_IP4_ADDR */
    {"x::x"},               /* _T_IP6_ADDR */
};

static L4_BACKDOOR_Option_T l4_backdoor_def_param_table[PARAM_COUNT] =
{
    {"unit",        _REQUIRED_ARG, 0, 0, _T_INTEGER},   /* PARAM_UNIT */
    {"port",        _REQUIRED_ARG, 0, 0, _T_INTEGER},   /* PARAM_PORT */
    {"src-mac",     _REQUIRED_ARG, 0, 0, _T_MAC_ADDR},  /* PARAM_SA */
    {"dst-mac",     _REQUIRED_ARG, 0, 0, _T_MAC_ADDR},  /* PARAM_DA */
    {"vid",         _REQUIRED_ARG, 0, 0, _T_INTEGER},   /* PARAM_VID */
    {"ethtype",     _REQUIRED_ARG, 0, 0, _T_INTEGER},   /* PARAM_ETHTYPE */
    {"src-ip",      _REQUIRED_ARG, 0, 0, _T_IP4_ADDR},  /* PARAM_SRC_IP */
    {"dst-ip",      _REQUIRED_ARG, 0, 0, _T_IP4_ADDR},  /* PARAM_DST_IP */
    {"src-ip6",     _REQUIRED_ARG, 0, 0, _T_IP6_ADDR},  /* PARAM_SRC_IP6 */
    {"dst-ip6",     _REQUIRED_ARG, 0, 0, _T_IP6_ADDR},  /* PARAM_DST_IP6 */
    {"l4-dst-port", _REQUIRED_ARG, 0, 0, _T_INTEGER},   /* PARAM_L4_DST_PORT */
    {"device",      _REQUIRED_ARG, 0, 0, _T_INTEGER},   /* PARAM_DEVICE */
    {"group",       _REQUIRED_ARG, 0, 0, _T_INTEGER},   /* PARAM_GROUP */
    {"func-type",   _REQUIRED_ARG, 0, 0, _T_INTEGER},   /* PARAM_FUNC_TYPE */
    {"tag",         _REQUIRED_ARG, 0, 0, _T_STRING},    /* PARAM_TAG */
    {"cos",         _REQUIRED_ARG, 0, 0, _T_INTEGER},   /* PARAM_COS */
    {"to_cpu",      _NO_ARG,       0, 0, _T_NONE},      /* PARAM_TO_CPU */
    {"flood",       _NO_ARG,       0, 0, _T_NONE},      /* PARAM_FLOOD */
    {"transition",  _NO_ARG,       0, 0, _T_NONE},      /* PARAM_TRANSITION */
    {"redir",       _NO_ARG,       0, 0, _T_NONE},      /* PARAM_REDIR */
    {"drop",        _NO_ARG,       0, 0, _T_NONE},      /* PARAM_DROP */
    {"help",        _NO_ARG,       0, 0, _T_NONE},      /* PARAM_HELP */
};

typedef struct
{
    char value[L4_BACKDOOR_MAX_PARAMETER_VALUE_STR_LEN + 1];
    UI8_T mode;
    UI8_T valid;
} L4_BACKDOOR_DefaultArgument_T;

static L4_BACKDOOR_DefaultArgument_T l4_backdoor_default_parameters[PARAM_COUNT] =
{
    {"1",                   _ARG_FIX, TRUE},    /* PARAM_UNIT */
    {"1",                   _ARG_FIX, TRUE},    /* PARAM_PORT */
    {"00-00-00-00-0C-01",   _ARG_FIX, TRUE},    /* PARAM_SA */
    {"00-00-00-00-0C-01",   _ARG_FIX, TRUE},    /* PARAM_DA */
    {"1",                   _ARG_FIX, TRUE},    /* PARAM_VID */
    {"0x0800",              _ARG_FIX, TRUE},    /* PARAM_ETHTYPE */
    {"192.168.1.1",         _ARG_FIX, TRUE},    /* PARAM_SRC_IP */
    {"192.168.1.1",         _ARG_FIX, TRUE},    /* PARAM_DST_IP */
    {"FE80::1",             _ARG_FIX, TRUE},    /* PARAM_SRC_IP6 */
    {"FE80::1",             _ARG_FIX, TRUE},    /* PARAM_DST_IP6 */
    {"",                    _ARG_FIX, FALSE},   /* PARAM_L4_DST_PORT */
    {"0",                   _ARG_FIX, TRUE},    /* PARAM_DEVICE */
    {"0",                   _ARG_FIX, TRUE},    /* PARAM_GROUP */
    {"1",                   _ARG_FIX, TRUE},    /* PARAM_FUNC_TYPE */
    {"",                    _ARG_FIX, FALSE},   /* PARAM_TAG */
    {"",                    _ARG_FIX, FALSE},   /* PARAM_COS */
    {"",                    _ARG_FIX, FALSE},   /* PARAM_TO_CPU */
    {"",                    _ARG_FIX, FALSE},   /* PARAM_FLOOD */
    {"",                    _ARG_FIX, FALSE},   /* PARAM_TRANSITION */
    {"",                    _ARG_FIX, FALSE},   /* PARAM_REDIR */
    {"",                    _ARG_FIX, FALSE},   /* PARAM_DROP */
    {"",                    _ARG_FIX, FALSE},   /* PARAM_HELP */
};

/*------------------------------------------------------------------------------
 * backdoor operation variables
 *------------------------------------------------------------------------------
 */

static L4_Backdoor_Menu_T *current_menu = NULL;


/* EXPORTED SUBPROGRAM BODIES
 */

/*------------------------------------------------------------------------------
 * FUNCTION NAME:  L4_Backdoor_CallBack
 *------------------------------------------------------------------------------
 * PURPOSE  : l4 backdoor callback function
 * INPUT    : none
 * OUTPUT   : none.
 * RETURN   : none
 * NOTES    : none.
 *------------------------------------------------------------------------------
 */
void L4_Backdoor_CallBack()
{
    enum
    {
        BUFFER_LEN = 512,
        TOKEN_SIZE = 20,
    };

    char        buffer[BUFFER_LEN];
    char        *pos, *ptr, *token[TOKEN_SIZE];
    int         cnt, i, j, max;
#ifndef L4_UT
    L_THREADGRP_Handle_T tg_handle;
    UI32_T backdoor_member_id;
#endif /* L4_UT */

#ifdef RUN_UNIT_TEST
    L4_Backdoor_UnitTest();
#endif

    /* initialize parent menu for sub_menu_of_main_menu */
    max = sizeof(sub_menu_of_main_menu)/sizeof(L4_Backdoor_Menu_T*);
    for (i = 0; i < max; ++i)
        sub_menu_of_main_menu[i]->parent_menu = main_menu;

#ifndef L4_UT
    tg_handle =(L_THREADGRP_Handle_T) L2_L4_PROC_COMM_GetL4GroupTGHandle();

    /* Join thread group
     */

    if(L_THREADGRP_Join(tg_handle, SYS_BLD_BACKDOOR_THREAD_PRIORITY, &backdoor_member_id)==FALSE)
    {
        BACKDOOR_MGR_Printf("%s: L_THREADGRP_Join fail.\n", __FUNCTION__);
        return;
    }
#endif /* L4_UT */

    BACKDOOR_MGR_Printf("\r\nl4 backdoor");
    L4_Backdoor_Help(0, NULL);

    while (TRUE)
    {
        BACKDOOR_MGR_Printf(L4_BACKDOOR_PROMPT);
        BACKDOOR_MGR_RequestKeyIn(buffer, BUFFER_LEN);

        if (!L4_Backdoor_ChopString(buffer))
            continue;

        if (!L4_Backdoor_StrCmpIgnoreCase(buffer, "?"))
        {
            L4_Backdoor_Help(0, NULL);
            continue;
        }

        if (!L4_Backdoor_StrCmpIgnoreCase(buffer, "quit"))
            return;

        if (!L4_Backdoor_StrCmpIgnoreCase(buffer, "exit"))
        {
            if ((NULL == current_menu) || (NULL == current_menu->parent_menu))
                return;

            current_menu = current_menu->parent_menu;
            L4_Backdoor_Help(0, NULL);
            continue;
        }

        ptr = buffer;
        memset(token, 0, sizeof(token));

        for (cnt = 0; cnt < TOKEN_SIZE; ++cnt)
        {

            pos = strchr(ptr, ' ');
            if (pos)
            {
                GET_TOKEN_AND_MOVE_NEXT(ptr, token[cnt], pos);
            }
            else
            {
                token[cnt] = ptr;
                break;
            }
        }

        if (!token[0] || !token[0][0])
            continue;

        /* sub menus */
        for (i = 0, max = current_menu->nbr_of_sub_menus; max > i; ++i)
        {
            if (L4_Backdoor_StrCmpIgnoreCase(token[0], current_menu->sub_menus[i]->name))
                continue;

            current_menu = current_menu->sub_menus[i];
            L4_Backdoor_Help(0, NULL);
            break;
        }

        if (max > i)
            continue;

        /* command items */
        for (j = 0, max = current_menu->nbr_of_cmd_items; max > j; ++j)
        {
            if (L4_Backdoor_StrCmpIgnoreCase(token[0], current_menu->cmd_items[j].cmd))
                continue;

            current_menu->cmd_items[j].exec(cnt, token + 1);
            break;
        }

        if (max > j)
            continue;

        /* qqq command */
        max = strlen(token[0]);
        for (i = 0; max > i; ++i)
        {
            if (('q' != token[0][i]) && ('Q' != token[0][i]))
                break;
        }

        if (max <= i)
        {
            for (i = 0; max > i; ++i)
            {
                current_menu = current_menu->parent_menu;
                if (NULL == current_menu)
                    return;
            }

            L4_Backdoor_Help(0, NULL);
        }
        else
        {
            /* number selection */
            i = atoi(token[0]);
            if ((0 <= i) && (IS_DIGIT(token[0][0])))
            {
                if (current_menu->nbr_of_sub_menus > i)
                {
                    current_menu = current_menu->sub_menus[i];
                    L4_Backdoor_Help(0, NULL);
                    continue;
                }

                i -= current_menu->nbr_of_sub_menus;
                if (current_menu->nbr_of_cmd_items > i)
                {
#ifndef L4_UT
                    /* Get execution permission from the thread group handler if necessary
                     */
                    L_THREADGRP_Execution_Request(tg_handle, backdoor_member_id);
#endif /* L4_UT */

                    current_menu->cmd_items[i].exec(cnt, token + 1);

#ifndef L4_UT
                    /* Release execution permission from the thread group handler if necessary
                     */
                    L_THREADGRP_Execution_Release(tg_handle, backdoor_member_id);
#endif /* L4_UT */
                    continue;
                }
            }

            BACKDOOR_MGR_Printf("\r\nbackdoor: %s: command not found", token[0]);
        }
    }

#ifndef L4_UT
    L_THREADGRP_Leave(tg_handle, backdoor_member_id);
#endif /* L4_UT */
}

/* LOCAL SUBPROGRAM BODIES
 */
static void L4_Backdoor_ShowIpAddr(UI8_T *addr)
{
    if (NULL == addr)
        return;

    BACKDOOR_MGR_Printf("%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
}

static BOOL_T L4_Backdoor_Str2IpAddr(const char *str, UI8_T *addr)
{
    L_INET_AddrIp_T ip;

    if (L_INET_RETURN_SUCCESS != L_INET_StringToInaddr(L_INET_FORMAT_IPV4_UNSPEC,
                                                       str,
                                                       (L_INET_Addr_T *)&ip,
                                                       sizeof(ip)))
    {
        return FALSE;
    }

    memcpy(addr, ip.addr, ip.addrlen);

    return TRUE;
}

static BOOL_T L4_Backdoor_Str2Ip6Addr(const char *str, UI8_T *addr)
{
    L_INET_AddrIp_T ip;

    if (L_INET_RETURN_SUCCESS != L_INET_StringToInaddr(L_INET_FORMAT_IPV6_UNSPEC,
                                                       str,
                                                       (L_INET_Addr_T *)&ip,
                                                       sizeof(ip)))
    {
        return FALSE;
    }

    memcpy(addr, ip.addr, ip.addrlen);

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_Backdoor_ShowMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : Show MAC address
 * INPUT    : addr  - MAC address
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static void L4_Backdoor_ShowMacAddr(UI8_T *addr)
{
    if (NULL == addr)
        return;

    BACKDOOR_MGR_Printf("%02x-%02x-%02x-%02x-%02x-%02x",
            addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_Backdoor_Str2MacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : Convert string to MAC address
 * INPUT    : str   - string
 * OUTPUT   : addr  - MAC address
 * RETURN   : TRUE/FALSE
 * NOTE     : str format support
 *            1. XX-XX-XX-XX-XX-XX 2. XXXXXX-XXXXXX 3. XXXXXXXXXXXX
 *------------------------------------------------------------------------------
 */
static BOOL_T L4_Backdoor_Str2MacAddr(const char *str, UI8_T *addr)
{
    enum
    {
        MAC_ADDR_SHORT_STR_LEN = 12,
        MAC_ADDR_STR_LEN = 17,
    };

    int i;
    char dupstr[MAC_ADDR_STR_LEN+1];
    char *p;

    if ((str == NULL) || (addr == NULL))
    {
        return FALSE;
    }

    if (strlen(str) < MAC_ADDR_SHORT_STR_LEN ||
        MAC_ADDR_STR_LEN < strlen(str))
    {
        return FALSE;
    }

    strncpy(dupstr, str, MAC_ADDR_STR_LEN);
    dupstr[MAC_ADDR_STR_LEN] = '\0';
    p = &dupstr[0];

    for (i = 0; i < SYS_ADPT_MAC_ADDR_LEN; ++i)
    {
        char byte_buf[3];

        addr[i] = 0;
        memcpy(byte_buf, p, 2);
        byte_buf[2] = '\0';

        if (IS_DIGIT(byte_buf[0]))
        {
            addr[i] += (byte_buf[0] - '0') * 16;
        }
        else if (IS_ALPHA(byte_buf[0]))
        {
            addr[i] += (byte_buf[0] - 'a' + 10) * 16;
        }
        else
            return FALSE;

        if (IS_DIGIT(byte_buf[1]))
        {
            addr[i] += (byte_buf[1] - '0');
        }
        else if (IS_ALPHA(p[1]))
        {
            addr[i] += (byte_buf[1] - 'a' + 10);
        }
        else
            return FALSE;

        p += 2;
        if (*p == '-')
        {
            p+=1;
        }
    }

    return TRUE;
}

static void L4_Backdoor_ShowRowStatus(UI32_T row_status)
{
    BACKDOOR_MGR_Printf("%s",
        (1 == row_status) ? "active" :
        (2 == row_status) ? "notInService" :
        (3 == row_status) ? "notReady" :
        (4 == row_status) ? "createAndGo" :
        (5 == row_status) ? "createAndWait" :
        (6 == row_status) ? "destroy" :
        "unknown");
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_Backdoor_Help
 *------------------------------------------------------------------------------
 * PURPOSE  : show current backdoor menu
 * INPUT    : argc, argv
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static void L4_Backdoor_Help(int argc, char** argv)
{
    const char sub_menu_fmt[]  = "\r\n      <sub menu>";
    const char menu_edge_fmt[] = "\r\n  |   +--(%02d) %s";
    const char menu_mide_fmt[] = "\r\n  |   +--(%02d) %s";
    const char menu_cmd_fmt[]  = "\r\n  |";
    const char cmd_edge_fmt[]  = "\r\n  +------(%02d) %s";
    const char cmd_mide_fmt[]  = "\r\n  +------(%02d) %s";

    int     i, j, max, tmp;
    const char *fmt;

    if (NULL == current_menu)
        current_menu = main_menu;

    BACKDOOR_MGR_Printf("\r\n-----------------------------------------------");

    if (0 < current_menu->nbr_of_sub_menus)
        BACKDOOR_MGR_Printf((char *)sub_menu_fmt);

    for (i = 0, max = current_menu->nbr_of_sub_menus, tmp = max - 1; max > i; ++i)
    {
        if ((0 == i) || (tmp == i))
            fmt = menu_edge_fmt;
        else
            fmt = menu_mide_fmt;

        BACKDOOR_MGR_Printf((char *)fmt, i, current_menu->sub_menus[i]->name);
    }

    BACKDOOR_MGR_Printf((char *)menu_cmd_fmt);
    for (j = 0, max = current_menu->nbr_of_cmd_items, tmp = max - 1; max > j; ++j)
    {
        if ((0 == j) || (tmp == j))
            fmt = cmd_edge_fmt;
        else
            fmt = cmd_mide_fmt;

        BACKDOOR_MGR_Printf((char *)fmt, i + j, current_menu->cmd_items[j].cmd);
    }

    BACKDOOR_MGR_Printf("\r\n\r\nexit -- back to up menu, quit -- back to accton backdoor");
    BACKDOOR_MGR_Printf("\r\nq[q..] -- shortcut");
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_StrCmpIgnoreCase
 *------------------------------------------------------------------------------
 * PURPOSE  : compare two strings case-insensitivly
 * INPUT    : dst, src (terminate by '\0')
 * OUTPUT   : none
 * RETURN   : 0 -- equal, > 0 -- dst > src, < 0 -- small
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static int L4_Backdoor_StrCmpIgnoreCase(const char *dst, const char *src)
{
    const UI8_T diff = 'a' - 'A';
    UI8_T       s, d;

    if ((NULL == dst) || (NULL == src)) /* can't compare */
        return -1;

    do
    {
        s = (('A' <= *src) && ('Z' >= *src)) ? (*src + diff) : *src;
        d = (('A' <= *dst) && ('Z' >= *dst)) ? (*dst + diff) : *dst;

        dst++;
        src++;
    }
    while (d && d == s);

    return (d - s);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_StrCmpIgnoreCase
 *------------------------------------------------------------------------------
 * PURPOSE  : compare two strings case-insensitivly
 * INPUT    : dst, src (terminate by '\0')
 * OUTPUT   : none
 * RETURN   : how many characters are the same
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static UI32_T L4_Backdoor_StrCmpIgnoreCase2(const char *dst, const char *src)
{
    UI32_T      len = 0;
    const UI8_T diff = 'a' - 'A';
    UI8_T       s, d;

    if ((NULL == dst) || (NULL == src)) /* can't compare */
        return 0;

    do
    {
        s = (('A' <= *src) && ('Z' >= *src)) ? (*src + diff) : *src;
        d = (('A' <= *dst) && ('Z' >= *dst)) ? (*dst + diff) : *dst;

        dst++;
        src++;

        len++;
    }
    while (d && d == s);

    return --len;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_ChopString
 *------------------------------------------------------------------------------
 * PURPOSE  : chop source string then put the result back
 * INPUT    : src_str (terminate by '\0')
 * OUTPUT   : src_str
 * RETURN   : TRUE - succeeded; FALSE - failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static BOOL_T L4_Backdoor_ChopString(char *str)
{
    char       *src_iter, *dst_iter, *dst_end;

    if (NULL == str)
        return FALSE;

    dst_iter = dst_end = NULL;
    src_iter = str;

    for ( ; '\0' != *src_iter; ++src_iter)
    {
        switch (*src_iter)
        {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if (NULL == dst_iter)
                    continue;

                if (NULL == dst_end)
                    dst_end = dst_iter;

                break;

            default:
                if (NULL != dst_end) /* e.g. "substring substring" or "substring\tsubsting" */
                    dst_end = NULL;
                break;
        }

        if (NULL == dst_iter)
            dst_iter = str;

        *dst_iter++ = *src_iter;
    }

    if (NULL != dst_end) /* e.g. "substring " or "substring substring/r/n" */
        *dst_end = '\0';
    else if ((NULL != dst_iter) && (dst_iter != src_iter)) /* e.g. " a" */
        *dst_iter = '\0';


    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_StrStrIgnoreCase
 *------------------------------------------------------------------------------
 * PURPOSE  : chop source string then put the result back
 * INPUT    : src_str (terminate by '\0')
 * OUTPUT   : src_str
 * RETURN   : TRUE - succeeded; FALSE - failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static char* L4_Backdoor_StrStrIgnoreCase(const char *str, const char *substr)
{
    return strstr(str, substr);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_LastChar
 *------------------------------------------------------------------------------
 * PURPOSE  : Get last char of string
 * INPUT    : str
 * OUTPUT   : none
 * RETURN   : last char
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static char L4_Backdoor_LastChar(const char *str)
{
    size_t len;

    if ('\0' == str[0])
    {
        return str[0];
    }

    len = strlen(str);
    return str[len-1];
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_DumpRawDataInHex
 *------------------------------------------------------------------------------
 * PURPOSE  : dump data in HEX format
 * INPUT    : data, size
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static void L4_Backdoor_DumpRawDataInHex(const UI8_T *data, UI32_T size, BOOL_T dont_dump_if_empty)
{
    UI32_T  col_idx, index;

    if (NULL == data)
        return;

    if (TRUE == dont_dump_if_empty)
    {
        for (index = 0; size > index; ++index, ++data)
        {
            if (0 != *data)
                break;
        }

        if (size <= index)
        {
            BACKDOOR_MGR_Printf("\r\n\t<< EMPTY >>");
            return;
        }
    }

    for (col_idx = 0, index = 0; size > index; ++col_idx, ++index, ++data)
    {
        if (16 <= col_idx)
            col_idx = 0;

        if (0 == col_idx)
            BACKDOOR_MGR_Printf("\r\n");

        BACKDOOR_MGR_Printf("%02X ", *data);
    }
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_BACKDOOR_GetArgumentFomat
 *------------------------------------------------------------------------------
 * PURPOSE  : Get argument format
 * INPUT    : type - argument type
 * OUTPUT   : none
 * RETURN   : Argument format/NULL
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static char *L4_BACKDOOR_GetArgumentFomat(int type)
{
    if (_countof(l4_backdoor_arg_format) <= type)
    {
        return NULL;
    }

    return l4_backdoor_arg_format[type];
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_BACKDOOR_GetDefaultArgument
 *------------------------------------------------------------------------------
 * PURPOSE  : Get default argument
 * INPUT    : param_index   - index of parameter
 * OUTPUT   : da_p          - default argument
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static BOOL_T L4_BACKDOOR_GetDefaultArgument(
    UI32_T param_index,
    L4_BACKDOOR_DefaultArgument_T *da_p
    )
{
    if ((PARAM_COUNT <= param_index) ||
        (NULL == da_p))
    {
        return FALSE;
    }

    memcpy(da_p, &l4_backdoor_default_parameters[param_index], sizeof(*da_p));

    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_BACKDOOR_SetDefaultArgument
 *------------------------------------------------------------------------------
 * PURPOSE  : Set default parameter
 * INPUT    : param_index   - index of parameter
 *            da_p          - default argument
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static BOOL_T L4_BACKDOOR_SetDefaultArgument(
    UI32_T param_index,
    L4_BACKDOOR_DefaultArgument_T *da_p
    )
{
    if ((PARAM_COUNT <= param_index) ||
        (NULL == da_p))
    {
        return FALSE;
    }

    memcpy(&l4_backdoor_default_parameters[param_index], da_p, sizeof(*l4_backdoor_default_parameters));

    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_BACKDOOR_GetDefaultArgumentString
 *------------------------------------------------------------------------------
 * PURPOSE  : Get argument format
 * INPUT    : type - argument type
 * OUTPUT   : none
 * RETURN   : Argument format/NULL
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static BOOL_T L4_BACKDOOR_GetDefaultArgumentString(
    UI32_T param_index,
    char* var,
    UI32_T cch_var
    )
{
    L4_BACKDOOR_Option_T *param_p;
    L4_BACKDOOR_DefaultArgument_T *env_p;

    if (NULL == var)
    {
        return FALSE;
    }

    memset(var, 0, cch_var * sizeof(*var));

    if (PARAM_COUNT <= param_index)
    {
        return FALSE;
    }

    env_p = &l4_backdoor_default_parameters[param_index];
    param_p = &l4_backdoor_def_param_table[param_index];

    if (FALSE == env_p->valid)
    {
        return FALSE;
    }

    if (_ARG_FIX == env_p->mode)
    {
        strncpy(var, env_p->value, cch_var-1);
        var[cch_var-1] = '\0';
    }
    else
    {

    }

    return TRUE;
}

typedef struct
{
    const char *name;
    const char *conform;

    UI32_T last_index;
} L4_BACKDOOR_GetNextConformString_T;

typedef struct
{
    const char *name;
    const void *conform;

    UI32_T last_index;
} _GetNextObj_T;

/*------------------------------------------------------------------------------
 * FUNCTION NAME - _GET_NEXT_OBJ_BY_NAME
 *------------------------------------------------------------------------------
 * PURPOSE  : Get next object if the prefix of object.name is the same as no.name
 * INPUT    : no            - a object of _GetNextObj_T.
 *                            Set data member conform to zeor to start
 *                            scan object array.
 *            obj_ar        - object array. Object in this array must a name
 *                            data member.
 *            cch_obj_ar    - the number of element of obj_ar
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : 1. If no.name is "?" then return all objects.
 *            2. If last char of no.name is '?', then it will be ignored.
 *------------------------------------------------------------------------------
 */
#define _GET_NEXT_OBJ_BY_NAME(no, obj_ar, cch_obj_ar)               \
{                                                                   \
    if (no.name && '\0' != no.name[0])                              \
    {                                                               \
        UI32_T i;                                                   \
                                                                    \
        if (no.conform == NULL)                                     \
        {                                                           \
            no.last_index = 0;                                      \
        }                                                           \
        else                                                        \
        {                                                           \
            no.last_index += 1;                                     \
        }                                                           \
                                                                    \
        no.conform = NULL;                                          \
        for (i=no.last_index; i<cch_obj_ar; i++)                    \
        {                                                           \
            if ('?' == no.name[0] && no.name[1] == '\0')            \
            {                                                       \
                no.conform = &obj_ar[i];                            \
                break;                                              \
            }                                                       \
            else                                                    \
            {                                                       \
                size_t len = L4_Backdoor_StrCmpIgnoreCase2(         \
                                no.name, obj_ar[i].name);           \
                                                                    \
                if (( len == strlen(no.name) ) ||                   \
                    ( '?' == L4_Backdoor_LastChar(no.name) &&       \
                      len == (strlen(no.name)-1) ) )                \
                {                                                   \
                    no.conform = &obj_ar[i];                        \
                    break;                                          \
                }                                                   \
            }                                                       \
        }                                                           \
                                                                    \
        no.last_index = i;                                          \
    }                                                               \
    else                                                            \
    {                                                               \
        no.conform = NULL;                                          \
        no.last_index = cch_obj_ar;                                 \
    }                                                               \
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - _GET_OBJ_BY_NAME
 *------------------------------------------------------------------------------
 * PURPOSE  : Get object if the prefix of object.name is the same as no.name.
 * INPUT    : no            - a structure of L4_BACKDOOR_GetNextObj_T.
 *                            Set all data member conform to zeor to start
 *                            scan object array.
 *            obj_ar        - object array.
 *            cch_obj_ar    - the number of element of obj_ar
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
#define _GET_OBJ_BY_NAME(no, obj_ar, cch_obj_ar)                    \
{                                                                   \
    void    *candidate = NULL;                                      \
                                                                    \
    no.conform = NULL;                                              \
    _GET_NEXT_OBJ_BY_NAME(no, obj_ar, cch_obj_ar);                  \
    while (no.conform)                                              \
    {                                                               \
        if (0 == L4_Backdoor_StrCmpIgnoreCase(no.name,              \
                    obj_ar[no.last_index].name))                    \
        {                                                           \
            candidate = no.conform;                                 \
            break;                                                  \
        }                                                           \
                                                                    \
        if (candidate)                                              \
        {                                                           \
            candidate = NULL;                                       \
            break;                                                  \
        }                                                           \
                                                                    \
        candidate = no.conform;                                     \
        _GET_NEXT_OBJ_BY_NAME(no, obj_ar, cch_obj_ar);              \
    }                                                               \
                                                                    \
    no.conform = candidate;                                         \
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_GetNextConformString
 *------------------------------------------------------------------------------
 * PURPOSE  : Get conform string from a string array
 * INPUT    : cs            - a structure of L4_BACKDOOR_GetNextConformString_T.
 *                            Set all data member conform to zeor to start
 *                            scan string array.
 *            strings       - string array.
 *            cch_strings   - the number of element of string array
 * OUTPUT   : none
 * RETURN   : TRUE, if find a conform string. The index for conform string
 *            in string array is be save in cs.last_index.
 *            FALSE, if no find a conform string.
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static BOOL_T L4_Backdoor_GetNextConformString(
    L4_BACKDOOR_GetNextConformString_T *cs,
    const char **strings,
    UI32_T cch_strings
    )
{
    UI32_T i;

    if (NULL == cs ||
        NULL == cs->name ||
        NULL == strings ||
        cch_strings == 0)
    {
        return FALSE;
    }

    if (cs->conform == NULL)
    {
        cs->last_index = 0;
    }
    else
    {
        cs->last_index += 1;
    }

    for (i=cs->last_index; i<cch_strings; i++)
    {
        const char *str = strings[i];

        /* query all command for '?'
         */
        if ('?' == cs->name[0] && cs->name[1] == '\0')
        {
            cs->last_index = i;
            cs->conform = str;
            return TRUE;
        }
        else
        {
            size_t len = L4_Backdoor_StrCmpIgnoreCase2(cs->name, str);

            /* full match */
            if (( len == strlen(cs->name) ) ||
            /* full match except '?' postfix */
                ( '?' == L4_Backdoor_LastChar(cs->name) && len == (strlen(cs->name)-1) ) )
            {
                cs->last_index = i;
                cs->conform = str;
                return TRUE;
            }
        }
    }

    cs->last_index = i;
    cs->conform = NULL;
    return FALSE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_GetBestConformString
 *------------------------------------------------------------------------------
 * PURPOSE  : Get best conform string from a string array
 * INPUT    : str             - input string.
 *            strings         - string array used.
 *            cch_strings     - the number of element of string array
 * OUTPUT   : conform_index_p - conform index of string array.
 * RETURN   : TRUE, if find a conform string. The index for conform string
 *            in string array is be save in cs.last_index.
 *            FALSE, if no find a conform string.
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static BOOL_T L4_Backdoor_GetBestConformString(
    const char *str,
    const char **strings,
    UI32_T cch_strings,
    UI32_T *conform_index_p
    )
{
    UI32_T candidate_index = 0xffffffff;
    L4_BACKDOOR_GetNextConformString_T cs = {0};

    if (NULL == str ||
        NULL == strings ||
        0 == cch_strings ||
        NULL == conform_index_p)
    {
        return FALSE;
    }

    /* Check illegal character
     */
    if (1 <= strspn(str, "?"))
    {
        return FALSE;
    }

    cs.name = str;
    while (L4_Backdoor_GetNextConformString(&cs, strings, cch_strings))
    {
        /* full match
         */
        if (0 == L4_Backdoor_StrCmpIgnoreCase(cs.conform, str))
        {
            *conform_index_p = cs.last_index;
            return TRUE;
        }

        if (0xffffffff != candidate_index)
        {
            /* Ambiguous
             */
            return FALSE;
        }

        candidate_index = cs.last_index;
    }

    if (0xffffffff == candidate_index)
    {
        return FALSE;
    }

    *conform_index_p = candidate_index;
    return TRUE;
}

typedef struct
{
    const char *name;
    L4_BACKDOOR_Command_T   *command;

    int _last_index;
} L4_Backdoor_GetNextCommand_T;

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_GetNextCommand
 *------------------------------------------------------------------------------
 * PURPOSE  : Get next command from command array
 * INPUT    : rc
 *            commands      - command array
 *            cch_commands  - element number of command array
 * OUTPUT   : none
 * RETURN   : command pointer/NULL
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static L4_BACKDOOR_Command_T* L4_Backdoor_GetNextCommand(
    L4_Backdoor_GetNextCommand_T *nc,
    L4_BACKDOOR_Command_T *commands,
    int cch_commands
    )
{
    _GetNextObj_T no = {0};

    no.name = nc->name;
    no.conform = nc->command;
    no.last_index = nc->_last_index;

#if 1
    _GET_NEXT_OBJ_BY_NAME(no, commands, cch_commands);
#else
    {
#define obj_ar commands
#define cch_obj_ar cch_commands
        UI32_T i;

        if (no.name && '\0' != no.name[0])
        {
            if (no.conform == NULL)
            {
                no.last_index = 0;
            }
            else
            {
                no.last_index += 1;
            }

            no.conform = NULL;
            for (i=no.last_index; i<cch_obj_ar; i++)
            {
                if ('?' == no.name[0] && no.name[1] == '\0')
                {
                    no.conform = &obj_ar[i];
                    break;
                }
                else
                {
                    UI32_T len = L4_Backdoor_StrCmpIgnoreCase2(no.name, obj_ar[i].name);

                    if ((0 < len) &&
                        (( len == strlen(no.name) ) ||
                         ( '?' == L4_Backdoor_LastChar(no.name) && len == (strlen(no.name)-1) ) ))
                    {
                        no.conform = &obj_ar[i];
                        break;
                    }
                }
            }

            no.last_index = i;
        }
        else
        {
            no.conform = NULL;
            no.last_index = cch_obj_ar;
        }
#undef obj_ar
#undef cch_obj_ar
    }
#endif

    nc->command = (L4_BACKDOOR_Command_T *) no.conform;
    nc->_last_index = no.last_index;

    return nc->command;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_FindCommand
 *------------------------------------------------------------------------------
 * PURPOSE  : Find command by command name
 * INPUT    : name          - command name
 *            commands      - command array
 *            cch_commands  - element number of command array
 * OUTPUT   : none
 * RETURN   : command pointer/NULL
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static L4_BACKDOOR_Command_T* L4_Backdoor_FindCommand(
    const char *name,
    L4_BACKDOOR_Command_T *commands,
    int cch_commands
    )
{
    L4_BACKDOOR_Command_T *command_p = NULL;
    _GetNextObj_T no = {0};


    no.name = name;

    _GET_OBJ_BY_NAME(no, commands, cch_commands);
    /*
    {
    void    *candidate = NULL;

    no.conform = NULL;
    _GET_NEXT_OBJ_BY_NAME(no, commands, cch_commands);
    while (no.conform)
    {
        if (0 == L4_Backdoor_StrCmpIgnoreCase(no.name, commands[no.last_index].name))
        {
            break;
        }

        if (candidate)
        {
            candidate = NULL;
            break;
        }

        candidate = no.conform;
        _GET_NEXT_OBJ_BY_NAME(no, commands, cch_commands);
    }

    no.conform = candidate;
    }
    */

    command_p = (L4_BACKDOOR_Command_T *) no.conform;
    return command_p;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_GetLongOptionsFomeCommand
 *------------------------------------------------------------------------------
 * PURPOSE  : Get long option from a command
 * INPUT    : vis_flags                 - visual flags used to check which
 *                                        parameter is need from a command
 *            command_p                 - command
 *            parameter_def_tbl         - parameter defined table
 *            cch_parameter_def_tbl     - cch of parameter_def_tbl
 *            cch_long_options          - cch of long_options
 * OUTPUT   : long_options              - output buffer
 *            cch_long_options_output_p - how many options be got
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static BOOL_T L4_Backdoor_GetLongOptionsFomeCommand(
    UI32_T  flags,
    L4_BACKDOOR_Command_T *command_p,
    L4_BACKDOOR_Option_T *parameter_def_tbl,
    UI32_T cch_parameter_def_tbl,
    L4_BACKDOOR_Option_T *long_options,
    UI32_T cch_long_options,
    UI32_T *cch_long_options_output_p
    )
{
    UI32_T ind;
    UI32_T i;
    L4_BACKDOOR_Option_T *opt_p;

    if (NULL == long_options)
    {
        return FALSE;
    }

    if (cch_long_options < 1)
    {
        return FALSE;
    }

    memset(long_options, 0, sizeof(*long_options) * cch_long_options);

    if (cch_long_options_output_p)
    {
        *cch_long_options_output_p = 0;
    }

    ind = 0;

    /* Add parameter --help automatically
     */
    opt_p = &parameter_def_tbl[ PARAM_HELP ];
    long_options[ind] = *opt_p;

    if (cch_long_options_output_p)
    {
        *cch_long_options_output_p += 1;
    }

    ind ++;

    for (i=0; i < command_p->param_num; i++)
    {
        if (command_p->parameters[i].vis_flag & flags)
        {
            if (cch_long_options <= ind)
            {
                BACKDOOR_MGR_Printf("\nNo enought memory to save more long option\n\n");
                return FALSE;
            }

            opt_p = &parameter_def_tbl[ command_p->parameters[i].index ];

            long_options[ind] = *opt_p;

            if (cch_long_options_output_p)
            {
                *cch_long_options_output_p += 1;
            }

            ind ++;
        }
    }

    return TRUE;
}

static BOOL_T  L4_BACKDOOR_GetOptionStringFromLongOptions(
    L4_BACKDOOR_Option_T *long_options,
    UI32_T cch_long_options,
    char *output,
    UI32_T cch_output
    )
{
    UI32_T ind;
    UI32_T i;
    L4_BACKDOOR_Option_T *opt_p;

    if (NULL == long_options || NULL == output)
    {
        return FALSE;
    }

    ind = 0;

    for (i=0; i < cch_long_options; i++)
    {
        opt_p = &long_options[i];

        switch (opt_p->has_arg)
        {
        case _NO_ARG:
        case _OPTIONAL_ARG:
            if (cch_output <= ind+1)
            {
                BACKDOOR_MGR_Printf("\nNo enought memory to save option string\n\n");
                return FALSE;
            }

            output[ind++] = (char)opt_p->val;
            break;

        case _REQUIRED_ARG:
            if (cch_output <= ind+2)
            {
                BACKDOOR_MGR_Printf("\nNo enought memory to save option string\n\n");
                return FALSE;
            }

            output[ind++] = (char)opt_p->val;
            output[ind++] = ':';
            break;
        }
    }

    return TRUE;
}

static BOOL_T L4_Backdoor_GetCommandShortOptionsString(
    UI32_T vis_options,
    L4_BACKDOOR_Command_T *command_p,
    char *short_options,
    UI32_T cch_short_options,
    UI32_T *cch_output_p
    )
{
    UI32_T ind;
    UI32_T i;
    L4_BACKDOOR_Option_T *opt_p;

    if (NULL == short_options)
    {
        return FALSE;
    }

    if (0 == cch_short_options)
    {
        return FALSE;
    }

    memset(short_options, 0, sizeof(char) * cch_short_options);

    if (cch_output_p)
    {
        *cch_output_p = 0;
    }

    ind = 0;

    for (i=0; i < command_p->param_num; i++)
    {
        if (command_p->parameters[i].vis_flag & vis_options)
        {
            opt_p = &l4_backdoor_def_param_table[ command_p->parameters[i].index ];

            switch (opt_p->has_arg)
            {
            case _NO_ARG:
            case _OPTIONAL_ARG:
                if (cch_short_options <= ind+1)
                {
                    BACKDOOR_MGR_Printf("\nNo enought memory to save more option\n\n");
                    return FALSE;
                }

                short_options[ind++] = (char)opt_p->val;
                break;

            case _REQUIRED_ARG:
                if (cch_short_options <= ind+2)
                {
                    BACKDOOR_MGR_Printf("\nNo enought memory to save more option\n\n");
                    return FALSE;
                }

                short_options[ind++] = (char)opt_p->val;
                short_options[ind++] = ':';
                break;
            }

            if (cch_output_p)
            {
                *cch_output_p += 1;
            }
        }
    }

    BACKDOOR_MGR_Printf("options = %s\n", short_options);

    return TRUE;
}

static BOOL_T L4_BACKDOOR_GetParameterIndexFromName(
    const char *name,
    UI32_T *param_idx
    )
{
    UI32_T i;

    for (i=0; i < _countof(l4_backdoor_def_param_table); i++)
    {
        L4_BACKDOOR_Option_T *opt_p = &l4_backdoor_def_param_table[ i ];

        if (0 == strcmp(opt_p->name, name))
        {
            *param_idx = i;
            return TRUE;
        }
    }

    return FALSE;
}

static BOOL_T L4_BACKDOOR_FillParameterByDfltValue(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T vis_flags,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    UI32_T ind;
    UI32_T i;
    BOOL_T res = TRUE;

    if (NULL == command_p || NULL == parameters)
    {
        return FALSE;
    }

    ind = 0;

    for (i=0; i < command_p->param_num; i++)
    {
        if (command_p->parameters[i].vis_flag & vis_flags)
        {
            L4_BACKDOOR_Option_T *opt_p = &l4_backdoor_def_param_table[ command_p->parameters[i].index ];
            L4_BACKDOOR_Parameter_T *param_p = &parameters[command_p->parameters[i].index];

            BOOL_T required_param = (command_p->parameters[i].required == _REQUIRED_PARAM) ? TRUE : FALSE;

            if (TRUE == param_p->valid)
            {
                continue;
            }

            if ((vis_flags & _OPT_DFLT_VAL_AS_ALL_MISS_PARAMETER) ||
                (vis_flags & _OPT_DFLT_VAL_AS_MISS_REQ_PARAMETER && required_param))
            {
                if (_REQUIRED_ARG == opt_p->has_arg ||
                    _OPTIONAL_ARG == opt_p->has_arg)
                {
                    char dflt_val[L4_BACKDOOR_MAX_PARAMETER_VALUE_STR_LEN + 1];

                    if (FALSE == L4_BACKDOOR_GetDefaultArgumentString(command_p->parameters[i].index, dflt_val, _countof(dflt_val)))
                    {
                        /* not default value
                         */
                        continue;
                    }

                    strncpy(param_p->value, dflt_val, sizeof(param_p->value)-1);
                    param_p->value[sizeof(param_p->value)-1] = '\0';

                    param_p->has_value = TRUE;
                }

                param_p->valid = TRUE;
            }
        }
    }

    return res;
}

static BOOL_T L4_BACKDOOR_ValidateParameters(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T vis_flags,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    UI32_T ind;
    UI32_T i;
    BOOL_T res = TRUE;

    if (NULL == command_p || NULL == parameters)
    {
        return FALSE;
    }

    ind = 0;

    for (i=0; i < command_p->param_num; i++)
    {
        if (command_p->parameters[i].vis_flag & vis_flags)
        {
            L4_BACKDOOR_Option_T *opt_p = &l4_backdoor_def_param_table[ command_p->parameters[i].index ];
            L4_BACKDOOR_Parameter_T *param_p = &parameters[command_p->parameters[i].index];
            BOOL_T required_param = (command_p->parameters[i].required == _REQUIRED_PARAM) ? TRUE : FALSE;

            if (FALSE == param_p->valid && TRUE == required_param)
            {
                BACKDOOR_MGR_Printf("Required parameter: --%s\n", opt_p->name);
                res = FALSE;
                continue;
            }

            if (FALSE == param_p->valid)
            {
                continue;
            }

            switch (opt_p->has_arg)
            {
            case _NO_ARG:
                break;

            case _REQUIRED_ARG:
                if (FALSE == param_p->has_value)
                {
                    BACKDOOR_MGR_Printf("Required argument for parameter --%s\n", opt_p->name);
                    res = FALSE;
                    continue;
                }
                break;

            case _OPTIONAL_ARG:
                break;
            }

            /* check type
             */
            if (param_p->has_value)
            {
                switch (opt_p->arg_type)
                {
                case _T_NONE:
                    break;

                case _T_INTEGER:
                    break;

                case _T_STRING:
                    break;

                case _T_MAC_ADDR:
                    {
                        UI8_T addr[SYS_ADPT_MAC_ADDR_LEN];

                        if (FALSE == L4_Backdoor_Str2MacAddr(param_p->value, addr))
                        {
                            BACKDOOR_MGR_Printf("%s is not MAC address format for \'%s\' ", param_p->value, opt_p->name);
                            BACKDOOR_MGR_Printf("MAC address format is %s\n", L4_BACKDOOR_GetArgumentFomat(opt_p->arg_type));
                            res = FALSE;
                        }
                    }
                    break;

                case _T_IN_ADDR:
                    {
                        UI8_T addr[4];

                        if (FALSE == L4_Backdoor_Str2IpAddr(param_p->value, addr))
                        {
                            BACKDOOR_MGR_Printf("%s is not IP address format for \'%s\' ", param_p->value, opt_p->name);
                            BACKDOOR_MGR_Printf("IP address format is %s\n", L4_BACKDOOR_GetArgumentFomat(opt_p->arg_type));
                            res = FALSE;
                        }
                    }
                    break;

                case _T_IP4_ADDR:
                    {
                        UI8_T addr[4];

                        if (FALSE == L4_Backdoor_Str2IpAddr(param_p->value, addr))
                        {
                            BACKDOOR_MGR_Printf("%s is not IP address format for \'%s\' ", param_p->value, opt_p->name);
                            BACKDOOR_MGR_Printf("IP address format is %s\n", L4_BACKDOOR_GetArgumentFomat(opt_p->arg_type));
                            res = FALSE;
                        }
                    }
                    break;

                case _T_IP6_ADDR:
                    {
                        UI8_T addr[16];

                        if (FALSE == L4_Backdoor_Str2Ip6Addr(param_p->value, addr))
                        {
                            BACKDOOR_MGR_Printf("%s is not IP6 address format for \'%s\' ", param_p->value, opt_p->name);
                            BACKDOOR_MGR_Printf("IP address format is %s\n", L4_BACKDOOR_GetArgumentFomat(opt_p->arg_type));
                            res = FALSE;
                        }
                    }
                    break;
                }
            }
        }
    }

    return res;
}

static void L4_BACKDOOR_InitOpt(
    L4_BACKDOOR_Opt_T *opt
    )
{
    opt->ind  = 0;
    opt->arg  = NULL;
    opt->opt  = 0;

    opt->__sp = 1;
}

static int L4_BACKDOOR_GetOpt(
    int argc,
    char **argv,
    const char *options,
    L4_BACKDOOR_Opt_T *opt
    )
{
    int ch;

    opt->arg = NULL;

    if (1 == opt->__sp)
    {
        if (argc <= opt->ind)
        {
            return -1;
        }

        if (argv[opt->ind][0] != '/' &&
            argv[opt->ind][0] != '-')
        {
            return -1;
        }

        if (argv[opt->ind][1] == '\0')
        {
            return -1;
        }
    }

    ch = argv[opt->ind][opt->__sp];

    if (NULL == strchr(options, ch))
    {
        BACKDOOR_MGR_Printf("illegal option -- %c\n", ch);
        return '?';
    }

    if ('\0' == argv[opt->ind][opt->__sp+1])
    {
        opt->ind++;
        opt->__sp = 1;
    }
    else
    {
        opt->__sp++;
    }

    return ch;
}

static int L4_BACKDOOR_GetOptLong(
    int argc,
    char **argv,
    const char *options,
    L4_BACKDOOR_Option_T *long_options,
    size_t cch_long_options,
    int *opt_index,
    L4_BACKDOOR_Opt_T *opt
    )
{
    L4_BACKDOOR_Option_T *best_op;
    int best_match;
    L4_BACKDOOR_Option_T *iter_op;
    int i;
    BOOL_T found_arg;

    opt->arg = NULL;
    *opt_index = -1;

    if (1 == opt->__sp)
    {
        if (argc <= opt->ind)
        {
            return -1;
        }

        if (strlen(argv[opt->ind]) < 3)
        {
            return -1;
        }

        if (argv[opt->ind][0] != '-' ||
            argv[opt->ind][1] != '-')
        {
            return -1;
        }

        opt->__sp = 2;
    }

    best_op = NULL;
    best_match = 0;

    for (iter_op = &long_options[0], i = 0;
         i < cch_long_options;
         iter_op ++, i++)
    {
        char *arg = &argv[opt->ind][opt->__sp];
        char *iter = iter_op->name;

        size_t arg_len = strlen(arg);
        size_t iter_len = strlen(iter);

        if (arg_len < iter_len)
        {
            // TODO: The checking for long option is incorrect
            size_t len = strspn(arg, iter);

            /* match !!
             */
            if (len == arg_len)
            {
                if (best_op)
                {
                    if (best_match < len)
                    {
                        best_op = iter_op;
                        best_match = len;
                        *opt_index = i;
                    }
                    else if (best_match == len)
                    {
                        BACKDOOR_MGR_Printf("ambiguous option -- %s\n", arg);
                        opt->__sp = 1;
                        *opt_index = -1;
                        return -1;
                    }
                }
                else
                {
                    best_op = iter_op;
                    best_match = len;
                    *opt_index = i;
                }
            }
        }
        else if (arg_len == iter_len)
        {
            if (0 == strcmp(arg, iter))
            {
                best_op = iter_op;
                best_match = iter_len;
                *opt_index = i;
                break;
            }
        }
        else /* op_len < in_len */
        {
        }
    }

    if (NULL == best_op)
    {
        BACKDOOR_MGR_Printf("illegal option -- %s\n", argv[opt->ind]);
        opt->__sp = 1;
        return -1;
    }

    opt->ind++;
    opt->__sp = 1;

    found_arg = FALSE;

    while (1)
    {
        char *next_arg = argv[opt->ind];

        if (best_op->has_arg == _REQUIRED_ARG)
        {
            if (argc <= opt->ind)
            {
                break;
            }

            if (2 <= strlen(next_arg) && (next_arg[0] == '-' && next_arg[1] == '-'))
            {
                break;
            }

            found_arg = TRUE;
            opt->arg = next_arg;
            opt->ind ++;
        }
        else if (best_op->has_arg == _OPTIONAL_ARG)
        {
            found_arg = TRUE;

            if (argc <= opt->ind)
            {
                break;
            }

            if (2 <= strlen(next_arg) && (next_arg[0] == '-' && next_arg[1] == '-'))
            {
                break;
            }

            opt->arg = next_arg;
            opt->ind ++;
        }
        else if (best_op->has_arg == _NO_ARG)
        {
            found_arg = TRUE;
        }

        break;
    }

    if (FALSE == found_arg)
    {
        BACKDOOR_MGR_Printf("required argument for %s\n", best_op->name);
        opt->__sp = 1;
        return -1;
    }

    return best_op->val;
}

/*------------------------------------------------------------------------------
 *  CPU Interface
 *------------------------------------------------------------------------------
 */
static void L4_Backdoor_CpuIf_TrapPacket2Cpu(int argc, char** argv)
{
    BOOL_T                      enable_flag;
    RULE_TYPE_PacketType_T		packet_type;
    RULE_TYPE_CpuRuleInfo_T     rule_info;

    if ((2 > argc) || (4 < argc))
    {
        BACKDOOR_MGR_Printf("\r\nparameters   : {enable_flag} {packet_type} [param1] [param2]");
        BACKDOOR_MGR_Printf("\r\n\tenable_flag: 1(true) 0(false)");
        BACKDOOR_MGR_Printf("\r\n\tDA format  : XX-XX-XX-XX-XX-XX");
        BACKDOOR_MGR_Printf("\r\n\tDIP format : XXX.XXX.XXX.XXX");
        BACKDOOR_MGR_Printf("\r\n\tpacket_type: ");
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (eaps, param1:[vid], param2[mode: transition(1), master(2)])", RULE_TYPE_PacketType_EAPS);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (dot1x)",       RULE_TYPE_PacketType_DOT1X);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (arp-reply)",   RULE_TYPE_PacketType_ARP_REPLY);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (arp-request)", RULE_TYPE_PacketType_ARP_REQUEST);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (rip)",         RULE_TYPE_PacketType_RIP);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (ospf)",        RULE_TYPE_PacketType_OSPF);
#if (SYS_CPNT_OSPF6 == TRUE)
        /* For OSPFv3 */
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (ospfv3)",      RULE_TYPE_PacketType_OSPF6);
#endif
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (dvmrp)",       RULE_TYPE_PacketType_DVMRP);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (pim)",         RULE_TYPE_PacketType_PIM);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (vrrp)",        RULE_TYPE_PacketType_VRRP);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (unknown-ipmc, param1[to_cpu], param2[flood])", RULE_TYPE_PacketType_UNKNOWN_IPMC);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (ip-bcast)",    RULE_TYPE_PacketType_IP_BCAST);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (l2-slf)",      RULE_TYPE_PacketType_L2_SLF);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (all-host)",    RULE_TYPE_PacketType_ALL_HOST);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (all-router)",  RULE_TYPE_PacketType_ALL_ROUTER);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (mymac-myip)",  RULE_TYPE_PacketType_MY_MAC_MY_IP);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (ip-option)",   RULE_TYPE_PacketType_IP_OPTION);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (slf)",         RULE_TYPE_PacketType_SLF);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (igmp)",        RULE_TYPE_PacketType_IGMP);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (dhcp client)", RULE_TYPE_PacketType_DHCP_CLIENT);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (dhcp server)", RULE_TYPE_PacketType_DHCP_SERVER);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (hbt)",         RULE_TYPE_PacketType_HBT);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (bpdu)",        RULE_TYPE_PacketType_BPDU);
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (l2cp)",        RULE_TYPE_PacketType_L2CP);
#if (SYS_CPNT_MDNS == TRUE)
        BACKDOOR_MGR_Printf("\r\n\t\t%2d (mdns, param1[to_cpu], param2[flood]))", RULE_TYPE_PacketType_MDNS);
#endif /* SYS_CPNT_MDNS */
        return;
    }

    switch (atoi(argv[0]))
    {
        case 0:
            enable_flag = FALSE;
            break;
        case 1:
            enable_flag = TRUE;
            break;
        default:
            BACKDOOR_MGR_Printf("\r\nbad enable_flag value");
            return;
    }

    packet_type = atoi(argv[1]);
    if(RULE_TYPE_PacketType_MAX <= packet_type)
    {
        BACKDOOR_MGR_Printf("\r\nbad packet_type value");
        return;
    }

    switch(packet_type)
    {
        case RULE_TYPE_PacketType_DOT1X:
        case RULE_TYPE_PacketType_SLF:
            BACKDOOR_MGR_Printf("\r\n not support yet!");
            return;

        case RULE_TYPE_PacketType_ARP_REQUEST:
        case RULE_TYPE_PacketType_ARP_REPLY:
            rule_info.arp.all_arp_to_cpu = enable_flag;
            break;

        case RULE_TYPE_PacketType_EAPS:
            if(4 != argc)
                return;

            rule_info.eaps.vid = atoi(argv[2]);
            rule_info.eaps.mode = atoi(argv[3]);
            break;

        case RULE_TYPE_PacketType_UNKNOWN_IPMC:
        case RULE_TYPE_PacketType_DHCP_CLIENT:
        case RULE_TYPE_PacketType_DHCP_SERVER:
            if(4 != argc)
                return;

            rule_info.common.to_cpu = atoi(argv[2]);
            rule_info.common.flood = atoi(argv[3]);
            break;

        case RULE_TYPE_PacketType_MDNS:
            rule_info.common.to_cpu = TRUE;
            rule_info.common.flood = TRUE;
            if(4 == argc)
            {
                rule_info.common.to_cpu = atoi(argv[2]);
                rule_info.common.flood = atoi(argv[3]);
            }
            break;

        default:
            break;
    }

    if (RULE_TYPE_OK != L4_MGR_TrapPacket2Cpu(enable_flag, packet_type, &rule_info))
        BACKDOOR_MGR_Printf("\r\nfail to call L4_MGR_TrapPacket2Cpu");
}

static void L4_Backdoor_CpuIf_ShowStatus(int argc, char** argv)
{
    RULE_CTRL_DumpCpuInterfaceStatus();
}

/*------------------------------------------------------------------------------
 *  DHCPSNP
 *------------------------------------------------------------------------------
 */
static void L4_Backdoor_CpuIf_DHCPSnooping2Cpu(int argc, char** argv)
{
#if 0//(SYS_CPNT_DHCPSNP == TRUE)
    UI32_T  en_flag, type;
    BOOL_T  ret;

    if (argc != 2)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag} {type}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(true)   0(false)");
        BACKDOOR_MGR_Printf("\r\n       type: 1(client) 0(server)");
        return;
    }

    en_flag = atoi(argv[0]);
    type    = atoi(argv[1]);

    if (1 == en_flag)
    {
        en_flag = L4_MGR_DO_ACTION_REDIRECT_TO_CPU;
    }
    else
    {
        en_flag = L4_MGR_DO_ACTION_DELETE_RULE;
    }

    if (0 == type)
    {
        BACKDOOR_MGR_Printf ("\r\n L4_MGR_SetDhcpServerPktAction");
        ret = L4_MGR_SetDhcpServerPktAction (en_flag);
    }
    else
    {
        BACKDOOR_MGR_Printf ("\r\n L4_MGR_SetDhcpClientPktAction");
        ret = L4_MGR_SetDhcpClientPktAction (en_flag);
    }

    if (FALSE == ret)
    {
        BACKDOOR_MGR_Printf (" failed.");
    }

#else
    BACKDOOR_MGR_Printf ("\r\n SYS_CPNT_DHCPSNP != TRUE\r\n");
#endif /* #if (SYS_CPNT_DHCPSNP == TRUE) */
}

/*------------------------------------------------------------------------------
 *  IP Source Guard
 *------------------------------------------------------------------------------
 */
static void L4_Backdoor_CpuIf_IPSrcGuard(int argc, char** argv)
{
#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
    UI32_T                  en_flag, port, sip, vid=1;
    UI8_T                   sa_buf[SYS_ADPT_MAC_ADDR_LEN], *sa =NULL;
    BOOL_T                  ret;

    if ((argc <3) || (argc > 5))
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag} {port} {ip} [mac] [vid]");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(true)   0(false)");
        BACKDOOR_MGR_Printf("\r\n       port: dut lport id");
        BACKDOOR_MGR_Printf("\r\n         ip: XXX.XXX.XXX.XXX");
        BACKDOOR_MGR_Printf("\r\n        mac: XX-XX-XX-XX-XX-XX");
        BACKDOOR_MGR_Printf("\r\n        vid: vlan id");
        return;
    }

    en_flag = atoi(argv[0]);
    port    = atoi(argv[1]);

    if (FALSE == L4_Backdoor_Str2IpAddr(argv[2], (UI8_T*)&sip))
        return;

    /* need to improve */
    if (4 <= argc)
    {
        if (FALSE == L4_Backdoor_Str2MacAddr(argv[3], sa_buf))
            return;

        sa = sa_buf;
    }

    if (5 <= argc)
    {
        vid = atoi (argv[4]);
    }

    if (en_flag)
    {
        en_flag = L4_MGR_DO_ACTION_PERMIT;
    }
    else
    {
        en_flag = L4_MGR_DO_ACTION_DELETE_RULE;
    }

    if (NULL == sa)
    {
        BACKDOOR_MGR_Printf ("\r\n L4_MGR_SetSrcIpFilterByLport");
        ret = L4_MGR_SetSrcIpFilterByLport (port, sip, vid, en_flag);
    }
    else
    {
        BACKDOOR_MGR_Printf ("\r\n L4_MGR_SetSrcMacSrcIpFilterByLport");
        ret = L4_MGR_SetSrcMacSrcIpFilterByLport (port, sa, sip, vid, en_flag);
    }

    if (RULE_TYPE_OK != ret)
        BACKDOOR_MGR_Printf (" failed.");

#else
    BACKDOOR_MGR_Printf ("\r\n SYS_CPNT_IP_SOURCE_GUARD != TRUE\r\n");
#endif /* #if (SYS_CPNT_IP_SOURCE_GUARD == TRUE) */
}

static void L4_Backdoor_CpuIf_IPSrcGuardDenyIpAny(int argc, char** argv)
{
#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
    UI32_T  en_flag, port;

    if (argc != 2)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag} {port}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(true)   0(false)");
        BACKDOOR_MGR_Printf("\r\n       port: dut lport id");
        return;
    }

    en_flag = atoi(argv[0]);
    port    = atoi(argv[1]);

    if (en_flag)
    {
        en_flag = L4_MGR_DO_ACTION_DROP;
    }
    else
    {
        en_flag = L4_MGR_DO_ACTION_DELETE_RULE;
    }

    BACKDOOR_MGR_Printf ("\r\n L4_MGR_SetIpPktActionByLport");
    if (RULE_TYPE_OK != L4_MGR_SetIpPktActionByLport (port, en_flag))
        BACKDOOR_MGR_Printf (" failed.");
#else
    BACKDOOR_MGR_Printf ("\r\n SYS_CPNT_IP_SOURCE_GUARD != TRUE\r\n");
#endif /* #if (SYS_CPNT_IP_SOURCE_GUARD == TRUE) */
}

/*------------------------------------------------------------------------------
 *  IPv6 Source Guard
 *------------------------------------------------------------------------------
 */
static void L4_Backdoor_CpuIf_IPv6SrcGuard(int argc, char** argv)
{
#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
    UI32_T                  en_flag, port, vid=1;
    UI8_T                   sip6[SYS_TYPE_IPV6_ADDR_LEN];
    BOOL_T                  ret;

    if ((argc <3) || (argc > 4))
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag} {port} {ipv6} [vid]");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(true)   0(false)");
        BACKDOOR_MGR_Printf("\r\n       port: dut lport id");
        BACKDOOR_MGR_Printf("\r\n       ipv6: XXXX:XXXX::XXXX");
        BACKDOOR_MGR_Printf("\r\n        vid: vlan id");
        return;
    }

    en_flag = atoi(argv[0]);
    port    = atoi(argv[1]);

    if (FALSE == L4_Backdoor_Str2Ip6Addr(argv[2], sip6))
    {
        BACKDOOR_MGR_Printf("\r\n [%s] is not a valid ipv6 address", argv[2]);
        return;
    }

    /* need to improve */
    if (4 <= argc)
    {
        vid = atoi (argv[3]);
    }

    if (en_flag)
    {
        en_flag = L4_MGR_DO_ACTION_PERMIT;
    }
    else
    {
        en_flag = L4_MGR_DO_ACTION_DELETE_RULE;
    }

    BACKDOOR_MGR_Printf ("\r\n L4_MGR_SetSrcIpv6FilterByLport");
    ret = L4_MGR_SetSrcIpv6FilterByLport(port, sip6, vid, en_flag);

    if (RULE_TYPE_OK != ret)
        BACKDOOR_MGR_Printf (" failed.");
#else
    BACKDOOR_MGR_Printf ("\r\n SYS_CPNT_IPV6_SOURCE_GUARD != TRUE\r\n");
#endif /* #if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE) */
}

static void L4_Backdoor_CpuIf_IPv6SrcGuardPermitLinkLocal(int argc, char** argv)
{
#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
    BOOL_T  en_flag;
    UI32_T  ret;

    if (argc != 1)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(true)   0(false)\r\n");
        return;
    }

    en_flag = (atoi(argv[0]) == 0) ? FALSE : TRUE;
    BACKDOOR_MGR_Printf("%s PermitIpv6LinkLocal ", en_flag ? "Enable" : "Disable");

    ret = L4_MGR_SetPermitIpv6LinkLocal(en_flag);
    if (RULE_TYPE_OK != ret)
        BACKDOOR_MGR_Printf (" failed.");

    BACKDOOR_MGR_Printf ("\r\n");
#else
    BACKDOOR_MGR_Printf ("\r\n SYS_CPNT_IPV6_SOURCE_GUARD != TRUE\r\n");
#endif /* #if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE) */
}

static void L4_Backdoor_CpuIf_IPv6SrcGuardDenyIpv6Any(int argc, char** argv)
{
#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
    UI32_T  en_flag, port;

    if (argc != 2)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag} {port}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(true)   0(false)");
        BACKDOOR_MGR_Printf("\r\n       port: dut lport id");
        return;
    }

    en_flag = atoi(argv[0]);
    port    = atoi(argv[1]);

    if (en_flag)
    {
        en_flag = L4_MGR_DO_ACTION_DROP;
    }
    else
    {
        en_flag = L4_MGR_DO_ACTION_DELETE_RULE;
    }

    BACKDOOR_MGR_Printf ("\r\n L4_MGR_SetIpv6PktActionByLport");
    if (RULE_TYPE_OK != L4_MGR_SetIpv6PktActionByLport(port, en_flag))
        BACKDOOR_MGR_Printf (" failed.");
#else
    BACKDOOR_MGR_Printf ("\r\n SYS_CPNT_IPV6_SOURCE_GUARD != TRUE\r\n");
#endif /* #if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE) */
}

/*------------------------------------------------------------------------------
 *  WEBAUTH
 *------------------------------------------------------------------------------
 */
#define L4_BACKDOOR_WEBAUTH_PERMIT_DHCPC   0
#define L4_BACKDOOR_WEBAUTH_PERMIT_DNS     1

#define L4_BACKDOOR_WEBAUTH_REDIRECT_HTTPC 3
#define L4_BACKDOOR_WEBAUTH_PERMIT_SIP     4
#define L4_BACKDOOR_WEBAUTH_DENY_IP        5

static void L4_Backdoor_CpuIf_WEBAUTH_SetPceByTypeAndUport(
    int web_ptype, int argc, char** argv)
{
#if (SYS_CPNT_WEBAUTH == TRUE)
    UI32_T      en_flag, port, sip;

    char        *fun_tag_ar [3] = {
                "L4_MGR_SetRedirectHTTPClientPackets",
                "L4_MGR_SetPermitSrcIpPacketsByLport",
                "L4_MGR_SetDenyIpPacketsByLport"};

    BOOL_T      ret = TRUE;

    if ((argc != 2) && (argc != 3))
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag} {lport}");

        if (L4_BACKDOOR_WEBAUTH_PERMIT_SIP == web_ptype)
            BACKDOOR_MGR_Printf(" {ip}");

        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(true)   0(false)");
        BACKDOOR_MGR_Printf("\r\n      lport: dut logical port id");

        if (L4_BACKDOOR_WEBAUTH_PERMIT_SIP == web_ptype)
            BACKDOOR_MGR_Printf("\r\n         ip: XXX.XXX.XXX.XXX");

        return;
    }

    en_flag = atoi(argv[0]);
    port    = atoi(argv[1]);

    if (0 == en_flag)
    {
        ret = FALSE;
    }

    BACKDOOR_MGR_Printf ("\r\n %s ->", fun_tag_ar[web_ptype -3]);

    switch (web_ptype)
    {
    case L4_BACKDOOR_WEBAUTH_REDIRECT_HTTPC:
        ret = L4_MGR_SetRedirectHTTPClientPackets (port, ret);
        break;

    case L4_BACKDOOR_WEBAUTH_PERMIT_SIP:

        if (FALSE == L4_Backdoor_Str2IpAddr(argv[2], (UI8_T*)&sip))
            return;

        ret = L4_MGR_SetPermitSrcIpPacketsByLport (port, sip, ret);
        break;

    case L4_BACKDOOR_WEBAUTH_DENY_IP:
        ret = L4_MGR_SetDenyIpPacketsByLport (port, ret);
        break;
    }

    if (FALSE == ret)
    {
        BACKDOOR_MGR_Printf ("\r\n failed.");
    }
#else

    BACKDOOR_MGR_Printf ("\r\n SYS_CPNT_WEBAUTH != TRUE\r\n");

#endif /* #if (SYS_CPNT_WEBAUTH == TRUE) */
}

static void L4_Backdoor_CpuIf_WEBAUTH_RedirectHTTPClient(int argc, char** argv)
{
    L4_Backdoor_CpuIf_WEBAUTH_SetPceByTypeAndUport (
        L4_BACKDOOR_WEBAUTH_REDIRECT_HTTPC, argc, argv);
}

static void L4_Backdoor_CpuIf_WEBAUTH_RermitSrcIP(int argc, char** argv)
{
    L4_Backdoor_CpuIf_WEBAUTH_SetPceByTypeAndUport (
        L4_BACKDOOR_WEBAUTH_PERMIT_SIP, argc, argv);
}

static void L4_Backdoor_CpuIf_WEBAUTH_DenyIP(int argc, char** argv)
{
    L4_Backdoor_CpuIf_WEBAUTH_SetPceByTypeAndUport (
        L4_BACKDOOR_WEBAUTH_DENY_IP, argc, argv);
}

/*------------------------------------------------------------------------------
 *  VLAN Translation
 *------------------------------------------------------------------------------
 */
#if (SYS_CPNT_VLAN_XLATE == TRUE && SYS_CPNT_VLAN_XLATE_BY_RULE == TRUE)
static void L4_Backdoor_CpuIf_SetPortVlanTranslation(int argc, char** argv)
{
    RULE_TYPE_INTERFACE_INFO_T interface;
    UI32_T                     result;
    UI32_T                     oldVid, newVid;

    if (argc != 5)
    {
        BACKDOOR_MGR_Printf("\r\nVlan translation");
        BACKDOOR_MGR_Printf("\r\n parameters: {interface} {port/trunk_id} {direction} {oldVID} {newVID}");
        BACKDOOR_MGR_Printf("\r\n interface: 0 - port, 1 - trunk");
        BACKDOOR_MGR_Printf("\r\n direction: 0 - ingress, 1 - egress");
        BACKDOOR_MGR_Printf("\r\n newVID   : 0 - delete rule, 1~4095 - new vlan ID");
        BACKDOOR_MGR_Printf("\r\n");
        return;
    }

    switch (atoi(argv[0]))
    {
        case 0:
            interface.type = RULE_TYPE_INTERFACE_UPORT;
            interface.uport.unit = 1;
            interface.uport.port = atoi(argv[1]);
            BACKDOOR_MGR_Printf("\r\n\tunit = %lu, port = %lu, ",
                interface.uport.unit, interface.uport.port);
            break;

        case 1:
            interface.type = RULE_TYPE_INTERFACE_TRUNK;
            interface.trunk_id = atoi(argv[1]);
            BACKDOOR_MGR_Printf("\r\n\ttrunk_id = %lu, ",
                                interface.trunk_id);
            break;

        default:
            BACKDOOR_MGR_Printf("\r\n Unknown interface.\r\n");
            return;
    }

    interface.direction = (atoi(argv[2]) == 0) ? RULE_TYPE_INBOUND : RULE_TYPE_OUTBOUND;
    oldVid = atoi(argv[3]);
    newVid = atoi(argv[4]);

    BACKDOOR_MGR_Printf("dir = %s, old_vid = %lu, new_vid = %lu \r\n",
        interface.direction == RULE_TYPE_INBOUND ? "ingress" : "egress",
        oldVid, newVid);

    result = RULE_CTRL_SetPortVlanXlate(&interface, oldVid, newVid);
    if (result != RULE_TYPE_OK)
    {
        BACKDOOR_MGR_Printf("\tRULE_CTRL_SetPortVlanXlate failed\r\n");
        return;
    }

    BACKDOOR_MGR_Printf("\tSet rule success\r\n");
}
#endif /* #if (SYS_CPNT_VLAN_XLATE == TRUE && SYS_CPNT_VLAN_XLATE_BY_RULE == TRUE) */

/*------------------------------------------------------------------------------
 *  MAC based VLAN
 *------------------------------------------------------------------------------
 */
#if (SYS_CPNT_MAC_VLAN == TRUE) && (SYS_CPNT_MAC_VLAN_IMPLEMENTED_BY_RULE == TRUE)
static void L4_Backdoor_CpuIf_SetMacBasedVLan(int argc, char** argv)
{
    BOOL_T ret, enable;
    UI8_T  src_mac[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T  mask[SYS_ADPT_MAC_ADDR_LEN];
    UI16_T vid;
    UI8_T  priority;

    if (argc != 5)
    {
        BACKDOOR_MGR_Printf("\r\nMac Based VLAN");
        BACKDOOR_MGR_Printf("\r\n parameters: {enable} {SA} {SA_mask} {vid} {priority}");
        BACKDOOR_MGR_Printf("\r\n   enable  : 0 - Delete rule, 1 - Add rule");
        BACKDOOR_MGR_Printf("\r\n   SA      : XXXXXXXXXXXX or XXXXXX-XXXXXX");
        BACKDOOR_MGR_Printf("\r\n   SA_mask : XXXXXXXXXXXX or XXXXXX-XXXXXX");
        BACKDOOR_MGR_Printf("\r\n   vid     : 1~%lu", SYS_ADPT_MAX_VLAN_ID);
        BACKDOOR_MGR_Printf("\r\n   priority: 0~7, other ignore");
        BACKDOOR_MGR_Printf("\r\n");
        return;
    }

    BACKDOOR_MGR_Printf("\r\n");

    enable = (atoi(argv[0]) == 1) ? TRUE : FALSE;

    if (FALSE == L4_Backdoor_Str2MacAddr(argv[1], src_mac))
    {
        BACKDOOR_MGR_Printf("Wrong input: SA\r\n");
        return;
    }

    if (FALSE == L4_Backdoor_Str2MacAddr(argv[2], mask))
    {
        BACKDOOR_MGR_Printf("Wrong input: SA_mask\r\n");
        return;
    }

    vid = (UI16_T)atoi(argv[3]);
    if (SYS_ADPT_MAX_VLAN_ID < vid)
    {
        BACKDOOR_MGR_Printf("Wrong input: vid\r\n");
        return;
    }
    
    priority = (UI8_T) atoi(argv[4]);

    if (enable)
    {
        BACKDOOR_MGR_Printf("Add mac based vlan rule\r\n");
        ret = RULE_CTRL_SetMacVlanEntry(src_mac, mask, vid, priority);
    }
    else
    {
        BACKDOOR_MGR_Printf("Delete mac based vlan rule\r\n");
        ret = RULE_CTRL_DeleteMacVlanEntry(src_mac, mask);
    }


    BACKDOOR_MGR_Printf("%s!!\r\n", (ret == TRUE) ? "Success" : "Failed");

    return;
}
#endif /* SYS_CPNT_MAC_VLAN && SYS_CPNT_MAC_VLAN_IMPLEMENTED_BY_RULE */

/*------------------------------------------------------------------------------
 *  Protocol based VLAN
 *------------------------------------------------------------------------------
 */
#if (SYS_CPNT_PROTOCOL_VLAN == TRUE) && (SYS_CPNT_PROTOCOL_VLAN_IMPLEMENTED_BY_RULE == TRUE)
static void L4_Backdoor_CpuIf_SetProtocolBasedVLan(int argc, char** argv)
{
    enum
    {
        BUFFER_LEN = 10
    };

#ifndef VAL_dot1vProtocolTemplateFrameType_ethernet
    enum
    {
        /* leaf_vbridge.h
         */
        VAL_dot1vProtocolTemplateFrameType_ethernet = 1,
        VAL_dot1vProtocolTemplateFrameType_rfc1042 = 2,
        VAL_dot1vProtocolTemplateFrameType_llcOther = 5
    };
#endif /* VAL_dot1vProtocolTemplateFrameType_ethernet */

    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_INTERFACE_INFO_T interface;    
    UI32_T nbr_of_type_protocol;
    UI16_T vid;
    BOOL_T enable;
    UI8_T  frame_type[SYS_ADPT_1V_MAX_NBR_OF_PROTOCOL_GROUP_ENTRY];
    UI8_T  protocol_value[SYS_ADPT_1V_MAX_NBR_OF_PROTOCOL_GROUP_ENTRY]
                         [SYS_ADPT_1V_MAX_PROTOCOL_VALUE_BUFFER_LENGTH];
    UI8_T  priority;
    char buffer[BUFFER_LEN];

    if (argc != 5)
    {
        BACKDOOR_MGR_Printf("\r\nProtocol Based VLAN");
        BACKDOOR_MGR_Printf("\r\n parameters: {enable} {unit} {port} {vid} {priority}");
        BACKDOOR_MGR_Printf("\r\n   enable  : 0 - Delete rule, 1 - Add rule");
        BACKDOOR_MGR_Printf("\r\n   unit    : Unit ID");
        BACKDOOR_MGR_Printf("\r\n   port    : Port ID");
        BACKDOOR_MGR_Printf("\r\n   vid     : 0~%lu", SYS_ADPT_MAX_VLAN_ID);
        BACKDOOR_MGR_Printf("\r\n   priority: 0~7, other ignore");
        BACKDOOR_MGR_Printf("\r\n");
        return;
    }

    BACKDOOR_MGR_Printf("\r\n");

    enable = (atoi(argv[0]) == 1) ? TRUE : FALSE;

    memset(&interface, 0, sizeof(interface));
    interface.type = RULE_TYPE_INTERFACE_UPORT;
    interface.direction = RULE_TYPE_INBOUND;
    interface.uport.unit = atoi(argv[1]);
    interface.uport.port = atoi(argv[2]);

    vid = (UI16_T)atoi(argv[3]);
    if (SYS_ADPT_MAX_VLAN_ID < vid)
    {
        BACKDOOR_MGR_Printf("Wrong input: vid\r\n");
        return;
    }
    
    priority = (UI8_T) atoi(argv[4]);

    nbr_of_type_protocol = 0;

    BACKDOOR_MGR_Printf("\r\nEnter protocol type (max=%lu), 'q' to leave, 'exit' to return",
        SYS_ADPT_1V_MAX_NBR_OF_PROTOCOL_GROUP_ENTRY);

    while (nbr_of_type_protocol < SYS_ADPT_1V_MAX_NBR_OF_PROTOCOL_GROUP_ENTRY)
    {
        UI16_T ethertype;
       
        BACKDOOR_MGR_Printf("\r\nEnter l2_format (0-ethernet 1-rfc1042 2-llcOther) :");

        BACKDOOR_MGR_RequestKeyIn(buffer, BUFFER_LEN);
  
        if (!L4_Backdoor_ChopString(buffer))
            continue;

        if (!L4_Backdoor_StrCmpIgnoreCase(buffer, "q"))
        {
            break;;
        }

        if (!L4_Backdoor_StrCmpIgnoreCase(buffer, "exit"))
        {
            return;
        }

        switch(buffer[0])
        {
            case '0':
                frame_type[nbr_of_type_protocol] = VAL_dot1vProtocolTemplateFrameType_ethernet;
                break;
            case '1':
                frame_type[nbr_of_type_protocol] = VAL_dot1vProtocolTemplateFrameType_rfc1042;
                break;
            case '2':
                frame_type[nbr_of_type_protocol] = VAL_dot1vProtocolTemplateFrameType_llcOther;
                break;
            default:
                continue;
        }

        BACKDOOR_MGR_Printf("\r\nEnter ethertype(0xXXXX) :");
        BACKDOOR_MGR_RequestKeyIn(buffer, BUFFER_LEN);
  
        if (!L4_Backdoor_ChopString(buffer))
            continue;

        if (!L4_Backdoor_StrCmpIgnoreCase(buffer, "q"))
        {
            break;;
        }

        {
            char *end_p;
            ethertype = (UI16_T) strtol(buffer, &end_p, 16);
        }

        ethertype = L_STDLIB_Hton16(ethertype);

        protocol_value[nbr_of_type_protocol][0] = ((UI8_T *) &ethertype)[0];
        protocol_value[nbr_of_type_protocol][1] = ((UI8_T *) &ethertype)[1];

        BACKDOOR_MGR_Printf("\r\n    [%lu]l2_format=%u, ethertype=0x%02x%02x",
            nbr_of_type_protocol, frame_type[nbr_of_type_protocol],
            protocol_value[nbr_of_type_protocol][0],
            protocol_value[nbr_of_type_protocol][1]);
            
        nbr_of_type_protocol++;
    }

    BACKDOOR_MGR_Printf("\r\n\r\nRULE_CTRL_SetProtocolVlanEntry");

    result = RULE_CTRL_SetProtocolVlanEntry(enable, &interface, 1, vid, nbr_of_type_protocol,
                 &frame_type, &protocol_value, priority);

    BACKDOOR_MGR_Printf("\r\n%s!!\r\n", (result == RULE_TYPE_OK) ? "Success" : "Failed");

    return;
}
#endif /* SYS_CPNT_PROTOCOL_VLAN && SYS_CPNT_PROTOCOL_VLAN_IMPLEMENTED_BY_RULE */

/*------------------------------------------------------------------------------
 *  IP subnet VLAN
 *------------------------------------------------------------------------------
 */
#if (SYS_CPNT_IP_SUBNET_VLAN == TRUE) && (SYS_CPNT_IP_SUBNET_VLAN_IMPLEMENTED_BY_RULE  == TRUE)
#if (SYS_CPNT_IP_SUBNET_VLAN_BIND_PORT == FALSE)
static void L4_Backdoor_CpuIf_SetIpSubnetVlan(int argc, char** argv)
{
    BOOL_T ret, enable;
    UI8_T  sip_addr[SYS_ADPT_IPV4_ADDR_LEN];
    UI8_T  sip_mask[SYS_ADPT_IPV4_ADDR_LEN];
    UI32_T ip, mask;
    UI16_T vid;
    UI8_T  priority;

    if (argc != 5)
    {
        BACKDOOR_MGR_Printf("\r\nIP Subnet VLAN");
        BACKDOOR_MGR_Printf("\r\n parameters: {enable} {SIP} {SIP_mask} {vid} {priority}");
        BACKDOOR_MGR_Printf("\r\n   enable  : 0 - Delete rule, 1 - Add rule");
        BACKDOOR_MGR_Printf("\r\n   SIP      : xxx.xxx.xxx.xxx");
        BACKDOOR_MGR_Printf("\r\n   SIP_mask : xxx.xxx.xxx.xxx");
        BACKDOOR_MGR_Printf("\r\n   vid     : 1~%lu", SYS_ADPT_MAX_VLAN_ID);
        BACKDOOR_MGR_Printf("\r\n   priority: 0~7, other ignore");
        BACKDOOR_MGR_Printf("\r\n");
        return;
    }

    BACKDOOR_MGR_Printf("\r\n");

    enable = (atoi(argv[0]) == 1) ? TRUE : FALSE;

    if (FALSE == L4_Backdoor_Str2IpAddr(argv[1], sip_addr))
    {
        BACKDOOR_MGR_Printf("Wrong input: SIP\r\n");
        return;
    }

    if (FALSE == L4_Backdoor_Str2IpAddr(argv[2], sip_mask))
    {
        BACKDOOR_MGR_Printf("Wrong input: SIP_mask\r\n");
        return;
    }

    vid = (UI16_T)atoi(argv[3]);
    if (SYS_ADPT_MAX_VLAN_ID < vid)
    {
        BACKDOOR_MGR_Printf("Wrong input: vid\r\n");
        return;
    }

    priority = (UI8_T) atoi(argv[4]);

    ip = (sip_addr[0] << 24) + (sip_addr[1] << 16) + (sip_addr[2] << 8) + sip_addr[3];
    mask = (sip_mask[0] << 24) + (sip_mask[1] << 16) + (sip_mask[2] << 8) + sip_mask[3];

    if (enable)
    {
        BACKDOOR_MGR_Printf("Add IP subnet vlan rule\r\n");
        ret = RULE_CTRL_SetIpSubnetVlanEntry(ip, mask, vid, priority);
    }
    else
    {
        BACKDOOR_MGR_Printf("Delete IP subnet vlan rule\r\n");
        ret = RULE_CTRL_DeleteIpSubnetVlanEntry(ip, mask);
    }

    BACKDOOR_MGR_Printf("%s!!\r\n", (ret == TRUE) ? "Success" : "Failed");

    return;
}
#endif /* SYS_CPNT_IP_SUBNET_VLAN_BIND_PORT == FALSE */
#endif /* SYS_CPNT_IP_SUBNET_VLAN && SYS_CPNT_IP_SUBNET_VLAN_IMPLEMENTED_BY_RULE */

/*------------------------------------------------------------------------------
 *  Voice VLAN
 *------------------------------------------------------------------------------
 */
#if (SYS_CPNT_ADD_MODIFY_PRIORITY_BY_RULE)
static void L4_Backdoor_CpuIf_SetVoiceVLan(int argc, char** argv)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_INTERFACE_INFO_T interface; 
    BOOL_T enable;
    UI8_T  mac[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T  mask[SYS_ADPT_MAC_ADDR_LEN];
    UI16_T vid;
    UI8_T  priority;


    if (argc != 7)
    {
        BACKDOOR_MGR_Printf("\r\nMac Based VLAN");
        BACKDOOR_MGR_Printf("\r\n parameters: {enable} {unit} {port} {SA} {SA_mask} {vid} {priority}");
        BACKDOOR_MGR_Printf("\r\n   enable  : 0 - Delete rule, 1 - Add rule");
        BACKDOOR_MGR_Printf("\r\n   unit    : Unit ID");
        BACKDOOR_MGR_Printf("\r\n   port    : Port ID");
        BACKDOOR_MGR_Printf("\r\n   SA      : XXXXXXXXXXXX or XXXXXX-XXXXXX");
        BACKDOOR_MGR_Printf("\r\n   SA_mask : XXXXXXXXXXXX or XXXXXX-XXXXXX");
        BACKDOOR_MGR_Printf("\r\n   vid     : voice vlan id, 1~%lu", SYS_ADPT_MAX_VLAN_ID);
        BACKDOOR_MGR_Printf("\r\n   priority: 0~7, other ignore");
        BACKDOOR_MGR_Printf("\r\n");
        return;
    }

    BACKDOOR_MGR_Printf("\r\n");

    enable = (atoi(argv[0]) == 1) ? TRUE : FALSE;

    memset(&interface, 0, sizeof(interface));
    interface.type = RULE_TYPE_INTERFACE_UPORT;
    interface.direction = RULE_TYPE_INBOUND;
    interface.uport.unit = atoi(argv[1]);
    interface.uport.port = atoi(argv[2]);

    if (FALSE == L4_Backdoor_Str2MacAddr(argv[3], mac))
    {
        BACKDOOR_MGR_Printf("Wrong inpug: SA\r\n");
        return;
    }

    if (FALSE == L4_Backdoor_Str2MacAddr(argv[4], mask))
    {
        BACKDOOR_MGR_Printf("Wrong inpug: SA_mask\r\n");
        return;
    }

    vid = (UI16_T)atoi(argv[5]);
    if (SYS_ADPT_MAX_VLAN_ID < vid)
    {
        BACKDOOR_MGR_Printf("Wrong inpug: vid\r\n");
        return;
    }
    
    priority = (UI8_T) atoi(argv[6]);

    BACKDOOR_MGR_Printf("RULE_CTRL_SetVoiceVlanPriority\r\n");
    result = RULE_CTRL_SetVoiceVlanPriority(enable, &interface, vid, mac, mask, priority);

    BACKDOOR_MGR_Printf("%s!!\r\n", (result == RULE_TYPE_OK) ? "Success" : "Failed");

    return;
}
#endif /* SYS_CPNT_ADD_MODIFY_PRIORITY_BY_RULE */

#if (SYS_ADPT_MODIFY_MGMT_QUEUE_BY_RULE == TRUE)
static void L4_Backdoor_CpuIf_ModifieCpuQueueForMyMacMyIp(int argc, char** argv)
{
    BOOL_T enable;
    UI8_T dip[SYS_ADPT_IPV4_ADDR_LEN];

    if (argc != 2)
    {
        BACKDOOR_MGR_Printf("\r\nModify CPU queue for my mac my ip");
        BACKDOOR_MGR_Printf("\r\n parameters: {enable} {dst_ip}");
        BACKDOOR_MGR_Printf("\r\n   enable  : 0 - Delete rule, 1 - Add rule");
        BACKDOOR_MGR_Printf("\r\n   dst_ip  : xxx.xxx.xxx.xxx");
        BACKDOOR_MGR_Printf("\r\n");
        return;
    }

    enable = (atoi(argv[0]) == 1) ? TRUE : FALSE;
    L4_Backdoor_Str2IpAddr(argv[1], dip);

    if (TRUE != RULE_CTRL_ModifyCpuQueueForMyMacMyIp(enable, &dip))
    {
        BACKDOOR_MGR_Printf("Failed!!");
    }

    BACKDOOR_MGR_Printf("\r\n");

    return;
}
#endif /* SYS_ADPT_MODIFY_MGMT_QUEUE_BY_RULE */

#if (SYS_CPNT_DAI_RATE_LIMIT_BY_RULE == TRUE)
static void
L4_Backdoor_CpuIf_TrapArpPacketToCpuWithRate(int argc, char** argv)
{
    BOOL_T enable;
    UI32_T unit, port, rate;
    RULE_TYPE_INTERFACE_INFO_T interface;
    RULE_TYPE_RETURN_TYPE_T ret;

    if (argc != 4)
    {
        BACKDOOR_MGR_Printf("\r\nTrap ARP packet to CPU with rate limit");
        BACKDOOR_MGR_Printf("\r\n parameters: {enable} {unit} {port} {rate}");
        BACKDOOR_MGR_Printf("\r\n   enable : 0 - Delete rule, 1 - Add rule");
        BACKDOOR_MGR_Printf("\r\n   unit   : Unit Id");
        BACKDOOR_MGR_Printf("\r\n   port   : Port number");
        BACKDOOR_MGR_Printf("\r\n   rate   : Packets per second");
        BACKDOOR_MGR_Printf("\r\n");
        return;
    }

    enable = (atoi(argv[0]) == 1) ? TRUE : FALSE;
    unit = atol(argv[1]);
    port = atol(argv[2]);
    rate = atol(argv[3]);
    if (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK < unit ||
        SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT < port)
    {
        BACKDOOR_MGR_Printf("Wrong parameter\r\n");
        return;
    }

    memset(&interface, 0, sizeof(interface));
    interface.type = RULE_TYPE_INTERFACE_UPORT;
    interface.uport.unit = unit;
    interface.uport.port = port;

    ret = RULE_CTRL_DAI_TrapArpPacketToCpuWithRate(enable, &interface, rate);

    BACKDOOR_MGR_Printf("Set rule %s!!\r\n", (ret == RULE_TYPE_OK) ? "success" : "fail");
    BACKDOOR_MGR_Printf("\r\n");
    return;
}
#endif /* SYS_CPNT_DAI_RATE_LIMIT_BY_RULE */

#if (SYS_CPNT_DOS == TRUE)
static void
L4_Backdoor_CpuIf_DosProtectionRateLimit(int argc, char** argv)
{
    UI32_T type, rate;

    if (argc != 2)
    {
        BACKDOOR_MGR_Printf("\r\nDoS protection rate limit");
        BACKDOOR_MGR_Printf("\r\n parameters: {type} {rate}");
        BACKDOOR_MGR_Printf("\r\n   type : 0 - ECHO_CHARGEN");
        BACKDOOR_MGR_Printf("\r\n          1 - TCP_Flooding");
        BACKDOOR_MGR_Printf("\r\n          2 - UDP_Flooding");
        BACKDOOR_MGR_Printf("\r\n          3 - WIN_NUKE");
        BACKDOOR_MGR_Printf("\r\n   rate : 0     - disable rule");
        BACKDOOR_MGR_Printf("\r\n          other - Kbit/second");
        BACKDOOR_MGR_Printf("\r\n");
        return;
    }

    switch (atol(argv[0]))
    {
        case 0:
            type = SWDRV_DOS_RATELIMIT_ECHO_CHARGEN;
            break;
        case 1:
            type = SWDRV_DOS_RATELIMIT_TCP_FLOODING;
            break;
        case 2:
            type = SWDRV_DOS_RATELIMIT_UDP_FLOODING;
            break;
        case 3:
            type = SWDRV_DOS_RATELIMIT_WIN_NUKE;
            break;

        default:
            BACKDOOR_MGR_Printf("Wrong type!!!!\r\n");
            return;
    }

    rate = atol(argv[1]);

    if (TRUE != RULE_CTRL_SetDosProtectionRateLimit(type, rate))
    {
        BACKDOOR_MGR_Printf("RULE_CTRL_SetDosProtectionRateLimit fail!!!!\r\n");
    }

    return;
}
#endif /* SYS_CPNT_DOS */

/*------------------------------------------------------------------------------
 *  tcp/udp destination port mapping.
 *------------------------------------------------------------------------------
 */
static void L4_Backdoor_TcpUdpPortMapping(int argc, char** argv)
{
    UI32_T                  is_add, unit, port;
    UI16_T                  dest_port;
    UI8_T                   phb, color, is_tcp;
    BOOL_T                  ret;

    if ( 7 != argc)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {is_add} {unit} {port} {is_tcp} {dest_port} {phb} {color}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(add)   0(delete)");
        BACKDOOR_MGR_Printf("\r\n       unit: unit id");
        BACKDOOR_MGR_Printf("\r\n       port: dut lport id");
        BACKDOOR_MGR_Printf("\r\n     is_tcp: 1(tcp)   0(udp)");
        BACKDOOR_MGR_Printf("\r\n  dest_port: 0-65535");
        BACKDOOR_MGR_Printf("\r\n        phb: 0-7");
        BACKDOOR_MGR_Printf("\r\n      color: 0(green) 1(red) 3(yellow)");
        return;
    }

    is_add = atoi(argv[0]);
    unit = atoi(argv[1]);
    port = atoi(argv[2]);
    is_tcp = atoi(argv[3]);
    dest_port = atoi(argv[4]);
    phb = atoi(argv[5]);
    color = atoi(argv[6]);


    if (1 == is_add)
    {
        BACKDOOR_MGR_Printf ("\r\n RULE_CTRL_SetTcpUdpPortCosMap");
        ret = RULE_CTRL_SetTcpUdpPortCosMap(unit, port, is_tcp, dest_port, phb, color, FALSE);
    }
    else
    {
        BACKDOOR_MGR_Printf ("\r\n RULE_CTRL_DeleteTcpUdpPortCosMap");
        ret = RULE_CTRL_DeleteTcpUdpPortCosMap(unit, port, is_tcp, dest_port, phb, color);
    }

    if (RULE_TYPE_OK != ret)
        BACKDOOR_MGR_Printf (" failed.");

}

static void L4_Backdoor_DHCP6Cient2CPU(int argc, char** argv)
{
    UI32_T                  is_add, unit, port;
    UI16_T                  dest_port;
    UI8_T                   phb, color, is_tcp;
    BOOL_T                  ret;

    if ( 1 != argc)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(add)   0(delete)");
        return;
    }

    is_add = (atoi(argv[0]) == 1) ? 1 : 0;

    BACKDOOR_MGR_Printf ("\r\n L4_Backdoor_DHCP6Cient2CPU");
    ret = RULE_CTRL_TrapPacket2Cpu(is_add, RULE_TYPE_PacketType_DHCP6_CLIENT, NULL);

    if (RULE_TYPE_OK != ret)
        BACKDOOR_MGR_Printf (" failed.");
}

static void L4_Backdoor_DHCP6Server2CPU(int argc, char** argv)
{
    UI32_T                  is_add, unit, port;
    UI16_T                  dest_port;
    UI8_T                   phb, color, is_tcp;
    BOOL_T                  ret;

    if ( 1 != argc)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(add)   0(delete)");
        return;
    }

    is_add = (atoi(argv[0]) == 1) ? 1 : 0;

    BACKDOOR_MGR_Printf ("\r\n L4_Backdoor_DHCP6Server2CPU");
    ret = RULE_CTRL_TrapPacket2Cpu(is_add, RULE_TYPE_PacketType_DHCP6_SERVER, NULL);

    if (RULE_TYPE_OK != ret)
        BACKDOOR_MGR_Printf (" failed.");
}

static void L4_Backdoor_TrapMLD2CPU(int argc, char** argv)
{
    UI32_T                  is_add, unit, port;
    UI16_T                  dest_port;
    UI8_T                   phb, color, is_tcp;
    BOOL_T                  ret;

    if ( 1 != argc)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(add)   0(delete)");
        return;
    }

    is_add = (atoi(argv[0]) == 1) ? 1 : 0;

    BACKDOOR_MGR_Printf ("\r\n L4_Backdoor_TrapMLD2CPU");
    ret = RULE_CTRL_TrapPacket2Cpu(is_add, RULE_TYPE_PacketType_MLD_Query, NULL);
    ret |= RULE_CTRL_TrapPacket2Cpu(is_add, RULE_TYPE_PacketType_MLD_Report, NULL);
    ret |= RULE_CTRL_TrapPacket2Cpu(is_add, RULE_TYPE_PacketType_MLD_Done, NULL);
    ret |= RULE_CTRL_TrapPacket2Cpu(is_add, RULE_TYPE_PacketType_MLDV2_Reports, NULL);

    if (RULE_TYPE_OK != ret)
        BACKDOOR_MGR_Printf (" failed.");
}

static void L4_Backdoor_UnknownIPv6MCast2CPU(int argc, char** argv)
{
    UI32_T                  is_add, unit, port;
    UI16_T                  dest_port;
    UI8_T                   phb, color, is_tcp;
    BOOL_T                  ret;
    RULE_TYPE_CpuRuleInfo_T info;

    if ( 3 != argc)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag} {to_cpu} {flood}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(add)   0(delete)");
        BACKDOOR_MGR_Printf("\r\n    to_cpu : 1(yes)   0(no)");
        BACKDOOR_MGR_Printf("\r\n    flood  : 1(yes)   0(no)");
        return;
    }

    is_add = (atoi(argv[0]) == 1) ? 1 : 0;
    info.common.to_cpu = (atoi(argv[1]) == 1) ? 1 : 0;
    info.common.flood  = (atoi(argv[2]) == 1) ? 1 : 0;

    BACKDOOR_MGR_Printf ("\r\n L4_Backdoor_UnknownIPv6MCast2CPU");
    ret = RULE_CTRL_TrapPacket2Cpu(is_add, RULE_TYPE_PacketType_UNKNOWN_IPV6MC, &info);

    if (RULE_TYPE_OK != ret)
        BACKDOOR_MGR_Printf (" failed.");
}

static void L4_Backdoor_CpuIf_TrapLinkLocalAddress2Cpu(int argc, char** argv)
{
    UI32_T                  is_add, unit, port;
    UI16_T                  dest_port;
    UI8_T                   phb, color, is_tcp;
    BOOL_T                  ret;
    RULE_TYPE_CpuRuleInfo_T info;
    L_INET_AddrIp_T         inet_ip;

    if ( 3 != argc)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag} {link-local-address} {vlan_id}");
        BACKDOOR_MGR_Printf("\r\n    en_flag            : 1(add)   0(delete)");
        BACKDOOR_MGR_Printf("\r\n    link-local-address : IPv6 address");
        BACKDOOR_MGR_Printf("\r\n    vlan_id            : VLAN ID");
        return;
    }

    BACKDOOR_MGR_Printf("\n");

    is_add = (atoi(argv[0]) == 1) ? 1 : 0;

    if (L_INET_RETURN_SUCCESS != L_INET_StringToInaddr(L_INET_FORMAT_IPV6_UNSPEC,
                                                       argv[1],
                                                       (L_INET_Addr_T *)&inet_ip,
                                                       sizeof(inet_ip)))
    {
        BACKDOOR_MGR_Printf("Invalid IPv6 address\n");
        return;
    }

    memcpy(info.linklocal.addr, inet_ip.addr, inet_ip.addrlen);
    info.linklocal.vlan_id = atoi(argv[2]);

    BACKDOOR_MGR_Printf ("\r\n L4_Backdoor_CpuIf_TrapIPv6Address2Cpu");
    ret = RULE_CTRL_TrapPacket2Cpu(is_add, RULE_TYPE_PacketType_LinkLocal, &info);

    if (RULE_TYPE_OK != ret)
        BACKDOOR_MGR_Printf (" failed.");
}

static void L4_Backdoor_CpuIf_DumpIpsgDb(int argc, char** argv)
{
#if 0//(SYS_CPNT_IP_SOURCE_GUARD == TRUE)
    UI32_T  uid, dev;

    if ((argc != 2) && (argc != 0))
    {
        BACKDOOR_MGR_Printf("\r\n parameters: [unit] [device]");
        return;
    }

    if (argc  != 0)
    {
        uid = atoi(argv[0]);
        dev = atoi(argv[1]);

        RULE_CTRL_DumpIpSrcGrdPermitIpDb_Backdoor(uid, dev);
    }
    else
    {
        for (uid =0; uid <SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK ; uid++)
        {
            for (dev=0; dev <SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; dev++)
            {
                RULE_CTRL_DumpIpSrcGrdPermitIpDb_Backdoor(uid+1, dev);
            }
        }
    }
#else
    BACKDOOR_MGR_Printf ("\r\n SYS_CPNT_IP_SOURCE_GUARD != TRUE\r\n");
#endif /* #if (SYS_CPNT_IP_SOURCE_GUARD == TRUE) */
}

static void L4_Backdoor_CpuIf_DumpFunctionInfo(int argc, char** argv)
{
    RULE_CTRL_DumpFunctionInfo();
}

/*------------------------------------------------------------------------------
 *  OAM
 *------------------------------------------------------------------------------
 */
static void L4_Backdoor_CpuIf_DumpOamInfo(int argc, char** argv)
{
#if 0//(SYS_CPNT_EFM_OAM == TRUE)
    UI32_T  uid, dev;

    if ((argc != 2) && (argc != 0))
    {
        BACKDOOR_MGR_Printf("\r\n parameters: [unit] [device]");
        return;
    }

    if (argc  != 0)
    {
        uid = atoi(argv[0]);
        dev = atoi(argv[1]);

        RULE_CTRL_DumpOamInfo_Backdoor(uid, dev);
    }
    else
    {
        RULE_CTRL_DumpOamInfo_Backdoor(1, 0);
    }
#else
    BACKDOOR_MGR_Printf ("\r\nSYS_CPNT_EFM_OAM != TRUE\r\n");
#endif /* #if (SYS_CPNT_EFM_OAM == TRUE) */
}

static void L4_Backdoor_CpuIf_TrapOamPdu2Cpu(int argc, char** argv)
{
#if (SYS_CPNT_EFM_OAM_REMOTE_LB_PASSIVELY_BY_RULE == TRUE)
    UI32_T  uid, prt;
    BOOL_T  ret, enf = FALSE;

    if (argc != 3)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag} {unit} {port}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(true)   0(false)");
        return;
    }

    if (atoi(argv[0]) > 0)
        enf = TRUE;

    uid = atoi(argv[1]);
    prt = atoi(argv[2]);

    BACKDOOR_MGR_Printf("\r\n RULE_CTRL_SetOamPduToCpu ");

    ret = RULE_CTRL_SetOamPduToCpu(uid, prt, enf);

    if (TRUE == enf)
        BACKDOOR_MGR_Printf("enable");
    else
        BACKDOOR_MGR_Printf("disable");

    if (TRUE != ret)
        BACKDOOR_MGR_Printf(" failed.");
#else
    BACKDOOR_MGR_Printf ("\r\n SYS_CPNT_EFM_OAM_REMOTE_LB_PASSIVELY != TRUE\r\n");
#endif /* #if (SYS_CPNT_EFM_OAM_REMOTE_LB_PASSIVELY_BY_RULE == TRUE) */
}

static void L4_Backdoor_CpuIf_SetOamLoopbk2RecPort(int argc, char** argv)
{
#if (SYS_CPNT_EFM_OAM_REMOTE_LB_PASSIVELY_BY_RULE == TRUE)
    UI32_T  uid, prt;
    BOOL_T  ret, enf = FALSE;

    if (argc != 3)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag} {unit} {port}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(true)   0(false)");
        return;
    }

    if (atoi(argv[0]) > 0)
        enf = TRUE;

    uid = atoi(argv[1]);
    prt = atoi(argv[2]);

    BACKDOOR_MGR_Printf("\r\n RULE_CTRL_SetOamLoopbackToRecPort ");

    ret = RULE_CTRL_SetOamLoopbackToRecPort(uid, prt, enf);

    if (TRUE == enf)
        BACKDOOR_MGR_Printf("enable");
    else
        BACKDOOR_MGR_Printf("disable");

    if (TRUE != ret)
        BACKDOOR_MGR_Printf(" failed.");
#else
    BACKDOOR_MGR_Printf ("\r\n SYS_CPNT_EFM_OAM_REMOTE_LB_PASSIVELY != TRUE\r\n");
#endif /* #if (SYS_CPNT_EFM_OAM_REMOTE_LB_PASSIVELY_BY_RULE == TRUE) */
}

static void L4_Backdoor_CpuIf_TrapPIM62Cpu(int argc, char** argv)
{
    BOOL_T                  is_add;
    BOOL_T                  ret;

    if ( 1 != argc)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(add)   0(delete)");
        return;
    }

    is_add = (atoi(argv[0]) == 1) ? TRUE : FALSE;

    ret = RULE_CTRL_TrapPacket2Cpu(is_add, RULE_TYPE_PacketType_PIM6, NULL);

    if (RULE_TYPE_OK != ret)
        BACKDOOR_MGR_Printf (" failed.");

}


/*------------------------------------------------------------------------------
 *  MLDSNP
 *------------------------------------------------------------------------------
 */
static void L4_Backdoor_CpuIf_TrapUknIPv6McData2Cpu(int argc, char** argv)
{
#if 0//(SYS_CPNT_MLDSNP == TRUE)
    BOOL_T  ret, enf = FALSE;

    if (argc != 1)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(true)   0(false)");
        return;
    }

    if (atoi(argv[0]) > 0)
        enf = TRUE;

    BACKDOOR_MGR_Printf("\r\n RULE_CTRL_SetUknIPv6McDataToCpu ");

    ret = RULE_CTRL_SetUknIPv6McDataToCpu(enf);

    if (TRUE == enf)
        BACKDOOR_MGR_Printf("enable");
    else
        BACKDOOR_MGR_Printf("disable");

    if (TRUE != ret)
        BACKDOOR_MGR_Printf(" failed.");
#else
    BACKDOOR_MGR_Printf ("\r\n SYS_CPNT_MLDSNP != TRUE\r\n");
#endif /* #if (SYS_CPNT_MLDSNP == TRUE) */
}

/*------------------------------------------------------------------------------
 *  SERVICE QINQ
 *------------------------------------------------------------------------------
 */
static void L4_Backdoor_CpuIf_SQinQ(int argc, char** argv)
{
#if (SYS_CPNT_QINQ == TRUE) && (SYS_CPNT_SWCTRL_QINQ_SERVICE == TRUE) && (SYS_CPNT_SWCTRL_QINQ_SERVICE_VLAN_XLATE_BY_RULE == TRUE)
    UI32_T  en_flag;
    BOOL_T  ret = TRUE;
    UI32_T  unit,port,svid,cvid,spri,cpri;

    if (argc != 7)
    {
        BACKDOOR_MGR_Printf("\r\n parameters: {en_flag} {unit} {port} {svid} {cvid} {spri} {cpri}");
        BACKDOOR_MGR_Printf("\r\n    en_flag: 1(true)   0(false)");
        return;
    }

    en_flag = atoi(argv[0]);
    unit = atoi(argv[1]);
    port = atoi(argv[2]);
    svid = atoi(argv[3]);
    cvid = atoi(argv[4]);
    spri = atoi(argv[5]);
    cpri = atoi(argv[6]);

    ret = RULE_CTRL_Service_QinQ(unit,port,cvid,cpri, svid ,spri,en_flag);

    BACKDOOR_MGR_Printf ("\r\n L4_Backdoor_CpuIf_SQinQ ->");

    if (FALSE == ret)
    {
        BACKDOOR_MGR_Printf ("\r\n failed.");
    }
    else
    {
        BACKDOOR_MGR_Printf ("\r\n succeed.");
    }

#else
    BACKDOOR_MGR_Printf ("\r\n Not support\r\n"
                         " SYS_CPNT_QINQ && SYS_CPNT_SWCTRL_QINQ_SERVICE && SYS_CPNT_SWCTRL_QINQ_SERVICE_VLAN_XLATE_BY_RULE != TRUE");
#endif /* SYS_CPNT_QINQ && SYS_CPNT_SWCTRL_QINQ_SERVICE && SYS_CPNT_SWCTRL_QINQ_SERVICE_VLAN_XLATE_BY_RULE */
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_Backdoor_CpuIf_Ci_Help
 *------------------------------------------------------------------------------
 * PURPOSE  : Show help for ci command
 * INPUT    : argc
 *            argv
 *            flags                 - Flags
 *            command_name          - Command name. For _OPT_HELP_COMMAND_AND_ITS_PARAMETERS.
 *            options               - Options array.
 *            cch_options           - Cch of options array.
 *            commands              - Command array.
 *            cch_commands          - Cch of commands array.
 *            parameter_def_tbl     - Parameter defined table.
 *            cch_parameter_def_tbl - Cch of parameter_def_tbl.
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
static void L4_Backdoor_CpuIf_Ci_Help(
    int argc,
    char **argv,
    UI32_T flags,
    const char *command_name,
    L4_BACKDOOR_Option_T *options,
    int cch_options,
    L4_BACKDOOR_Command_T *commands,
    int cch_commands,
    L4_BACKDOOR_Option_T *parameter_def_tbl,
    int cch_parameter_def_tbl,
    L4_BACKDOOR_Opt_T *opt
    )
{
#define _GET_MAX_COMMAND_LEN(len)                   \
    {                                               \
        UI32_T __i;                                 \
        int __len = 0;                              \
        for (__i=0; __i<cch_commands; __i++) {      \
            size_t l = strlen(commands[__i].name);  \
            if ((size_t)__len < l) {                \
                __len = (int)l;                     \
            }                                       \
        }                                           \
        if (30 < __len) {                           \
            __len = 30;                             \
        }                                           \
        else if (__len < 10) {                      \
            __len = 10;                             \
        }                                           \
        sprintf(len, "%d", __len);                  \
    }


#define PARAM_INDENT_STR "    "

    int i, dump_len;

    if (flags & (_OPT_HELP | _OPT_HELP_SYNTAX))
    {
        BACKDOOR_MGR_Printf("\n"
                            "Ci : CPU Interface configuration\n");
        BACKDOOR_MGR_Printf("  ci flags command [parameters]\n");
    }

    if (flags & _OPT_HELP_SYNTAX)
    {
        BACKDOOR_MGR_Printf("SYNTAX\n");
        BACKDOOR_MGR_Printf("  ci ");

        for (i=0; i<cch_options; i++)
        {
            BACKDOOR_MGR_Printf("[-%c] ", options[i].val);
        }

        BACKDOOR_MGR_Printf("\n%s{", PARAM_INDENT_STR);

        for (i=0; i<cch_commands; i++)
        {
            if (i % 3 == 0)
            {
                BACKDOOR_MGR_Printf("\n  %s", PARAM_INDENT_STR);
            }
            BACKDOOR_MGR_Printf("%-22s%s", commands[i].name,  i != (cch_commands - 1) ? " | " : "");
        }

        BACKDOOR_MGR_Printf("\n%s}", PARAM_INDENT_STR);

        for (i = 0; i < cch_parameter_def_tbl; i++)
        {
            if (i % 4 == 0)
            {
                BACKDOOR_MGR_Printf("\n%s", PARAM_INDENT_STR);
            }

            BACKDOOR_MGR_Printf("[");
            BACKDOOR_MGR_Printf("--%s", parameter_def_tbl[i].name);
            switch (parameter_def_tbl[i].has_arg)
            {
            case _NO_ARG:
                break;

            case _OPTIONAL_ARG:
                BACKDOOR_MGR_Printf(" [%s]",
                                    L4_BACKDOOR_GetArgumentFomat(parameter_def_tbl[i].arg_type));
                break;

            case _REQUIRED_ARG:
                BACKDOOR_MGR_Printf(" %s",
                                    L4_BACKDOOR_GetArgumentFomat(parameter_def_tbl[i].arg_type));
                break;
            }
            BACKDOOR_MGR_Printf("] ");
        }

        BACKDOOR_MGR_Printf("\n");

        BACKDOOR_MGR_Printf(
            "EXAMPLE:\n"
            "  Install and show rule by:\n"
            "    ci -is arp-req-copy-to-cpu\n"
            "\n"
            "  Find command by:\n"
            "    ci arp?\n"
            "\n"
            "  List all parameters for a command by:\n"
            "    ci arp-req-copy-to-cpu ?\n"
            );
    }

    /* show flags
     */
    if (flags & _OPT_HELP)
    {
        char len[3] = {0};
        char fmt[] = "      -%c#### %s\n";
        char *p;

        _GET_MAX_COMMAND_LEN(len);
        p = strchr(fmt, '#');
        *p++ = '%';
        *p++ = len[0];
        *p++ = len[1];
        *p++ = 's';

        BACKDOOR_MGR_Printf("    FLAG\n");
        for (i=0; i<cch_options; i++)
        {
            BACKDOOR_MGR_Printf(fmt,
                options[i].val, "", options[i].name);
        }
    }

    /* show commands
     */
    if (flags & _OPT_HELP || flags & _OPT_HELP_COMMAND)
    {
        char len[3] = {0};
        char fmt[] = "      %-##s   %s\n";
        char *p;

        _GET_MAX_COMMAND_LEN(len);

        p = strchr(fmt, '#');
        *p++ = len[0];
        *p++ = len[1];

        BACKDOOR_MGR_Printf("    COMMAND\n");
        for (i=0; i<cch_commands; i++)
        {
            BACKDOOR_MGR_Printf(fmt,
                commands[i].name, commands[i].desc);
        }
    }

    /* show parameters
     */
    if (flags & _OPT_HELP)
    {
        char len[3] = {0};
        char fmt[] = "      --%-##s ";
        char *p;

        _GET_MAX_COMMAND_LEN(len);

        p = strchr(fmt, '#');
        *p++ = len[0];
        *p++ = len[1];

        BACKDOOR_MGR_Printf("    PARAMETER\n");
        for (i=0; i<cch_parameter_def_tbl; i++)
        {
            BACKDOOR_MGR_Printf(fmt, parameter_def_tbl[i].name);
            switch (parameter_def_tbl[i].has_arg)
            {
            case _NO_ARG:
                break;
            case _OPTIONAL_ARG:
                BACKDOOR_MGR_Printf("Optional. ");
            case _REQUIRED_ARG:
                BACKDOOR_MGR_Printf("Format %s",
                    L4_BACKDOOR_GetArgumentFomat(parameter_def_tbl[i].arg_type));
                break;
            }
            BACKDOOR_MGR_Printf("\n");
        }
    }

    /* show detail
     */
    if (flags & _OPT_HELP_COMMAND_AND_ITS_PARAMETERS)
    {
        int _i;
        L4_Backdoor_GetNextCommand_T nc = {0};
        L4_BACKDOOR_Command_T *command_p = NULL;

        nc.command = NULL;
        nc.name = command_name;

        BACKDOOR_MGR_Printf("USAGE\n");

        while ((command_p = L4_Backdoor_GetNextCommand(&nc, commands, cch_commands)) != NULL)
        {
            L4_BACKDOOR_Option_T *parameter_p;
            int paramind;

            for (_i=0; _i < _countof(l4_backdoor_user_options); _i++)
            {
                UI32_T vis_flag = l4_backdoor_user_options[_i].flag;

                if (flags & vis_flag ||
                    flags == _OPT_HELP_COMMAND_AND_ITS_PARAMETERS)
                {

                    BACKDOOR_MGR_Printf("  %s %s\n", l4_backdoor_user_options[_i].name, command_p->name);
                    BACKDOOR_MGR_Printf("    /%c %s", l4_backdoor_user_options[_i].val, command_p->name);

                    for (paramind=0; paramind < command_p->param_num; paramind++)
                    {
                        if (command_p->parameters[paramind].vis_flag & vis_flag)
                        {
                            BOOL_T optional_param = FALSE;

                            parameter_p = &parameter_def_tbl[ command_p->parameters[paramind].index ];

                            if (_OPTIONAL_PARAM == command_p->parameters[paramind].required)
                            {
                                optional_param = TRUE;
                            }

                            if (optional_param)
                            {
                                BACKDOOR_MGR_Printf(" [--%s", parameter_p->name);
                            }
                            else
                            {
                                BACKDOOR_MGR_Printf(" --%s", parameter_p->name);
                            }

                            if (parameter_p->has_arg == _REQUIRED_ARG)
                            {
                                BACKDOOR_MGR_Printf(" <%s>", L4_BACKDOOR_GetArgumentFomat(parameter_p->arg_type));
                            }
                            else if (parameter_p->has_arg == _OPTIONAL_ARG)
                            {
                                BACKDOOR_MGR_Printf(" [<%s>]", L4_BACKDOOR_GetArgumentFomat(parameter_p->arg_type));
                            }

                            if (optional_param)
                            {
                                BACKDOOR_MGR_Printf("]");
                            }
                        }
                    }

                    BACKDOOR_MGR_Printf("\n");

                }
            }
        }
    }

#undef PARAM_INDENT_STR
#undef _GET_MAX_COMMAND_LEN
}

/* syntax
       ci
       ci ?
       ci command?
       ci command ?
       ci -options command --parameter value --paramerer2 value2 ...
 */
static void L4_Backdoor_CpuIf_Ci(
    int argc,
    char** argv
    )
{
    static L4_BACKDOOR_Command_T l4_backdoor_ci_commands[] =
    {
        {
            "installed-rules",
            "Show all installed rules",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_installed_rules,
            6,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_GROUP,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_FUNC_TYPE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_TAG,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            }
        },

#if 0 /* FIXME: Some function on MARVELL doen't implement, mask it first */
        {
            "l4-dport-cos",
            "Cos. L4 dst-port to cos map",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_l4_dport_cos,
            6,
            {
                {
                    PARAM_L4_DST_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_L4_DST_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_COS,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_COS,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            }
        },

    #if (SYS_CPNT_DOT1X == TRUE)
        {
            "dot1x-uc",
            "802.1x Unticast configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_dot1x,
            6,
            {
                {
                    PARAM_UNIT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DA,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL
                },
            }
        },
    #endif /* SYS_CPNT_DOT1X */

    #if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
        {
            "ipsg-deny-ip",
            "IP Src Guard. Enable/disable to deny all IP4 packet",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_ipsg_deny_ip,
            4,
            {
                {
                    PARAM_UNIT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            }
        },
        {
            "ipsg-permit-ip",
            "IP Src Guard. Enable/disable to permit specified packet",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_ipsg_permit_ip,
            9,
            {
                {
                    PARAM_UNIT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_VID,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_VID,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_SRC_IP,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_SRC_IP,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_SA,
                    _OPTIONAL_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL | _OPT_SHOW
                },
            }
        },
    #endif /* SYS_CPNT_IP_SOURCE_GUARD */

    #if (SYS_CPNT_WEBAUTH == TRUE)
        {
            "wa-trap-http",
            "WebAuth. Enable/disable to redir all HTTP packet",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_wa_trap_http,
            4,
            {
                {
                    PARAM_UNIT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            }
        },
        {
            "wa-deny-ip",
            "WebAuth. Enable/disable to deny all IP4 packet",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_wa_deny_ip,
            4,
            {
                {
                    PARAM_UNIT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            }
        },
        {
            "wa-permit-ip",
            "WebAuth. Enable/disable to permit specified IP4 packet",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_wa_permit_ip,
            6,
            {
                {
                    PARAM_UNIT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_SRC_IP,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_SRC_IP,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },
        {
            "wa-permit-dhcp",
            "WebAuth. Enable/disable to permit DHCP packet",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_wa_permit_dhcp,
            4,
            {
                {
                    PARAM_UNIT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            }
        },
        {
            "wa-permit-dns",
            "WebAuth. Enable/disable to permit DNS packet",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_wa_permit_dns,
            4,
            {
                {
                    PARAM_UNIT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            }
        },
    #endif /* SYS_CPNT_WEBAUTH */

    #if (SYS_CPNT_ERPS == TRUE)
        {
            "erps",
            "ERPS configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_erps,
            4,
            {
                {
                    PARAM_UNIT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            }
        },
    #endif /* SYS_CPNT_ERPS */

    #if (SYS_CPNT_EFM_OAM_REMOTE_LB_PASSIVELY == TRUE) && (SYS_CPNT_EFM_OAM_LOOPBACK_BY_RULE == TRUE)
        {
            "oam-cpe-oam-pdu",
            "OAM PDU configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_oam_cpe_oam_pdu,
            4,
            {
                {
                    PARAM_UNIT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            }
        },
        {
            "oam-cpe-oam-loopback",
            "OAM Lookback configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_oam_cpe_oam_loopback,
            4,
            {
                {
                    PARAM_UNIT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            }
        },
    #endif /* #if (SYS_CPNT_EFM_OAM_REMOTE_LB_PASSIVELY == TRUE) && (SYS_CPNT_EFM_OAM_LOOPBACK_BY_RULE == TRUE) */

    #if (SYS_CPNT_EFM_OAM_REMOTE_LB_ACTIVELY == TRUE)
        {
            "oam-co-oam-loopback",
            "OAM Lookback configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_oam_co_oam_loopback,
            4,
            {
                {
                    PARAM_UNIT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_PORT,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_PORT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },
    #endif /* SYS_CPNT_EFM_OAM_REMOTE_LB_ACTIVELY */

    #if (SYS_CPNT_IPV6 == TRUE)
        {
            "link-local-addr",
            "IP6. Link local address configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_link_local_addr,
            6,
            {
                {
                    PARAM_DST_IP6,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_DST_IP6,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_VID,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_VID,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },
    #endif /* SYS_CPNT_IPV6 */

        {
            "eaps",
            "EAPS configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_eaps,
            4,
            {
                {
                    PARAM_VID,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL
                },
                {
                    PARAM_TRANSITION,
                    _OPTIONAL_PARAM,
                    _OPT_INSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "arp-req-copy-to-cpu",
            "ARP Request configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_arp_req_copy_to_cpu,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "arp-req-redir-to-cpu",
            "ARP Request configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_arp_req_redir_to_cpu,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "arp-reply-copy-to-cpu",
            "ARP Reply configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_arp_reply_copy_to_cpu,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "arp-reply-redir-to-cpu",
            "ARP Reply configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_arp_reply_redir_to_cpu,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "rip",
            "RIP configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_rip,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "ospf",
            "OSPF configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_ospf,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

#if (SYS_CPNT_OSPF6 == TRUE)
        {
            "ospf6",
            "OSPF6 configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_ospf6,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },
#endif /* SYS_CPNT_OSPF6 */

        {
            "igmp",
            "IGMP configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_igmp,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "l2-slf",
            "L2 SLF configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_l2_slf,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "slf",
            "SLF configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_slf,
            6,
            {
                {
                    PARAM_VID,
                    _REQUIRED_PARAM,
                    _OPT_INSTALL | _OPT_UNINSTALL
                },
                {
                    PARAM_REDIR,
                    _OPTIONAL_PARAM,
                    _OPT_INSTALL
                },
                {
                    PARAM_DROP,
                    _OPTIONAL_PARAM,
                    _OPT_INSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_VID,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "ip-unknown-mc",
            "IP Unknown Multicast configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_ip_unknown_mc,
            4,
            {
                {
                    PARAM_TO_CPU,
                    _OPTIONAL_PARAM,
                    _OPT_INSTALL
                },
                {
                    PARAM_FLOOD,
                    _OPTIONAL_PARAM,
                    _OPT_INSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "ip-bcast",
            "IP bcast configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_ip_bcast,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "pim",
            "PIM configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_pim,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "vrrp",
            "VRRP configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_vrrp,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "ip-all-host",
            "IP all host configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_ip_all_host,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "ip-all-routers",
            "IP all routers configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_ip_all_routers,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "ip-option",
            "IP option configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_ip_option,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "hbt",
            "HBT Workaround configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_hbt,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "bpdu",
            "BPDU configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_bpdu,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "l2cp",
            "L2CP configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_l2cp,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

//        {
//            "cluster",
//            "Cluster configuration",
//            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_cluster,
//            2,
//            {
//                {
//                    PARAM_UNIT,
//                    _OPTIONAL_PARAM,
//                    _OPT_SHOW
//                },
//                {
//                    PARAM_DEVICE,
//                    _OPTIONAL_PARAM,
//                    _OPT_SHOW
//                },
//            },
//        },

    #if (SYS_CPNT_CFM == TRUE)
        {
            "cfm",
            "CFM configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_cfm,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },
//        {
//            "cfm-ccm",
//            "CCM configuration",
//            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_cfm_ccm,
//            2,
//            {
//                {
//                    PARAM_UNIT,
//                    _OPTIONAL_PARAM,
//                    _OPT_SHOW
//                },
//                {
//                    PARAM_DEVICE,
//                    _OPTIONAL_PARAM,
//                    _OPT_SHOW
//                },
//            },
//        },
    #endif /* SYS_CPNT_CFM */
        {
            "ip6-all-nodes",
            "IP6 All Nodes configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_ip6_all_nodes,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "ip6-all-routers",
            "IP6 All Routers configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_ip6_all_routers,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "pim6",
            "PIM6 configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_pim6,
            2,
            {
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },

        {
            "ip6-unknown-mc",
            "IP6 Unknown Multicast configuration",
            (L4_BACKDOOR_DummyCommandFuncPtr_T)L4_Backdoor_CpuIf_ip6_unknown_mc,
            4,
            {
                {
                    PARAM_TO_CPU,
                    _OPTIONAL_PARAM,
                    _OPT_INSTALL
                },
                {
                    PARAM_FLOOD,
                    _OPTIONAL_PARAM,
                    _OPT_INSTALL
                },
                {
                    PARAM_UNIT,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
                {
                    PARAM_DEVICE,
                    _OPTIONAL_PARAM,
                    _OPT_SHOW
                },
            },
        },
#endif /* 0 */
    };

    L4_BACKDOOR_Option_T ci_long_options[] =
    {
        {"install",                                 _NO_ARG, 0, 'i', _T_NONE},
        {"uninstall",                               _NO_ARG, 0, 'u', _T_NONE},
        {"show",                                    _NO_ARG, 0, 's', _T_NONE},
        {"help",                                    _NO_ARG, 0, 'h', _T_NONE},
        {"debug mode",                              _NO_ARG, 0, 'd', _T_NONE},
        {"dflt value as miss required parameter",   _NO_ARG, 0, 'a', _T_NONE},
        {"dflt value as all miss parameter",        _NO_ARG, 0, 'A', _T_NONE},
    };

    int opt_index;
    int optind;
    UI32_T  flags = 0;
    int c;
    L4_BACKDOOR_Command_T   *command_p;
    L4_BACKDOOR_Opt_T opt;
    L4_BACKDOOR_Parameter_T parameters[PARAM_COUNT];

    char ci_short_options[ (_countof(ci_long_options) *2 ) + 1 ] = {0};

    /* ci
     * ci ?
     */
    if ((0 == argc) ||
        (1 == argc && 0 == strcmp(argv[0], "?")) )
    {
        flags |= _OPT_HELP_SYNTAX;
        L4_Backdoor_CpuIf_Ci_Help(argc,
                                  argv,
                                  flags,
                                  NULL,
                                  &ci_long_options[0],
                                  _countof(ci_long_options),
                                  l4_backdoor_ci_commands,
                                  _countof(l4_backdoor_ci_commands),
                                  l4_backdoor_def_param_table,
                                  _countof(l4_backdoor_def_param_table),
                                  &opt);
        goto about;
    }

    memset(parameters, 0, sizeof(parameters));

    L4_BACKDOOR_GetOptionStringFromLongOptions(ci_long_options,
                                               _countof(ci_long_options),
                                               ci_short_options,
                                               _countof(ci_short_options));

    L4_BACKDOOR_InitOpt(&opt);

    opt.ind = 0;

    while ((c = L4_BACKDOOR_GetOpt(argc, argv, ci_short_options, &opt)) != -1)
    {
        switch (c)
        {
        case 'i':
            flags |= _OPT_INSTALL;
            break;
        case 'u':
            flags |= _OPT_UNINSTALL;
            break;
        case 's':
            flags |= _OPT_SHOW;
            break;
        case 'd':
            flags |= _OPT_DEBUG;
            break;
        case 'a':
            flags |= _OPT_DFLT_VAL_AS_MISS_REQ_PARAMETER;
            break;
        case 'A':
            flags |= _OPT_DFLT_VAL_AS_ALL_MISS_PARAMETER;
            break;
        case 'h':
        case '?':
        default:
            flags |= _OPT_HELP;
            L4_Backdoor_CpuIf_Ci_Help(argc,
                                      argv,
                                      flags,
                                      NULL,
                                      ci_long_options,
                                      _countof(ci_long_options),
                                      l4_backdoor_ci_commands,
                                      _countof(l4_backdoor_ci_commands),
                                      l4_backdoor_def_param_table,
                                      _countof(l4_backdoor_def_param_table),
                                      &opt);
            goto about;
        }
    }

    /* No more argv[] after parsed the flags.
     * E.g., ci /is
     */
    if (argc <= opt.ind)
    {
        BACKDOOR_MGR_Printf("Command is required\n");

        flags |= _OPT_HELP_COMMAND;
        L4_Backdoor_CpuIf_Ci_Help(argc,
                                  argv,
                                  flags,
                                  NULL,
                                  ci_long_options,
                                  _countof(ci_long_options),
                                  l4_backdoor_ci_commands,
                                  _countof(l4_backdoor_ci_commands),
                                  l4_backdoor_def_param_table,
                                  _countof(l4_backdoor_def_param_table),
                                  &opt);
        goto about;
    }

    command_p = L4_Backdoor_FindCommand(argv[opt.ind], l4_backdoor_ci_commands, _countof(l4_backdoor_ci_commands));
    if (NULL == command_p)
    {
        char last = L4_Backdoor_LastChar(argv[opt.ind]);

        if ('?' == last)
        {
            flags |= _OPT_HELP_COMMAND_AND_ITS_PARAMETERS;

            L4_Backdoor_CpuIf_Ci_Help(argc,
                                      argv,
                                      flags,
                                      argv[opt.ind],
                                      ci_long_options,
                                      _countof(ci_long_options),
                                      l4_backdoor_ci_commands,
                                      _countof(l4_backdoor_ci_commands),
                                      l4_backdoor_def_param_table,
                                      _countof(l4_backdoor_def_param_table),
                                      &opt);
        }
        else
        {
            BACKDOOR_MGR_Printf("Unknown command: %s\n", argv[opt.ind]);

            flags |= _OPT_HELP_COMMAND;
            L4_Backdoor_CpuIf_Ci_Help(argc,
                                      argv,
                                      flags,
                                      argv[opt.ind],
                                      ci_long_options,
                                      _countof(ci_long_options),
                                      l4_backdoor_ci_commands,
                                      _countof(l4_backdoor_ci_commands),
                                      l4_backdoor_def_param_table,
                                      _countof(l4_backdoor_def_param_table),
                                      &opt);
        }
        goto about;
    }

    opt.ind ++;

    optind = opt.ind;

    /* Last elem of argv[] is ?
     * E.g., ci /i /u dot1x ?
     */
    if (opt.ind == argc-1 &&
        0 == strcmp(argv[opt.ind], "?") )
    {
        flags |= _OPT_HELP_COMMAND_AND_ITS_PARAMETERS;

        L4_Backdoor_CpuIf_Ci_Help(argc,
                                  argv,
                                  flags,
                                  command_p->name,
                                  ci_long_options,
                                  _countof(ci_long_options),
                                  l4_backdoor_ci_commands,
                                  _countof(l4_backdoor_ci_commands),
                                  l4_backdoor_def_param_table,
                                  _countof(l4_backdoor_def_param_table),
                                  &opt);
        goto about;
    }

    /* Parse parameters
     */
    if (opt.ind < argc)
    {
        UI32_T  param_idx = 0;
        UI32_T  cch_output;
        L4_BACKDOOR_Option_T    long_options[PARAM_COUNT];
        //char short_options[PARAM_COUNT*2+1];

        //L4_Backdoor_GetCommandShortOptionsString(options, cmd_p, short_options, _countof(short_options), NULL);
        L4_Backdoor_GetLongOptionsFomeCommand(flags,
            command_p,
            l4_backdoor_def_param_table,
            _countof(l4_backdoor_def_param_table),
            long_options,
            _countof(long_options),
            &cch_output);

        L4_BACKDOOR_InitOpt(&opt);
        opt.ind = optind;

        if (0 < cch_output)
        {
            while (1)
            {
                c = L4_BACKDOOR_GetOptLong(argc,
                                           argv,
                                           NULL,//short_options,
                                           &long_options[0],
                                           cch_output,
                                           &opt_index,
                                           &opt);


                if (-1 == c)
                {
                    break;
                }

                switch (c)
                {
                case 0:
                    /* BACKDOOR_MGR_Printf("%s %s\n", long_options[opt_index].name, opt.arg?opt.arg:"(null)"); */

                    if (TRUE == L4_BACKDOOR_GetParameterIndexFromName(long_options[opt_index].name, &param_idx))
                    {
                        if (PARAM_HELP == param_idx)
                        {
                            flags |= _OPT_HELP_COMMAND_AND_ITS_PARAMETERS;

                            L4_Backdoor_CpuIf_Ci_Help(argc,
                                                      argv,
                                                      flags,
                                                      command_p->name,
                                                      ci_long_options,
                                                      _countof(ci_long_options),
                                                      l4_backdoor_ci_commands,
                                                      _countof(l4_backdoor_ci_commands),
                                                      l4_backdoor_def_param_table,
                                                      _countof(l4_backdoor_def_param_table),
                                                      &opt);
                            goto about;
                        }

                        parameters[ param_idx ].valid = TRUE;
                        if (opt.arg)
                        {
#define __STRNCPY(dest, src, cch_dest)  \
    strncpy(dest, src, cch_dest-1);     \
    dest[cch_dest-1] = '\0';

                            parameters[ param_idx ].has_value = TRUE;
                            __STRNCPY(parameters[ param_idx ].value, opt.arg, sizeof(parameters[ param_idx ].value));

#undef __STRNCPY
                        }
                    }
                    break;
                }
            }
        }
    } /* if (optind < argc) have more argv need to parse */

    if ((flags & _OPT_DFLT_VAL_AS_MISS_REQ_PARAMETER) ||
        (flags & _OPT_DFLT_VAL_AS_ALL_MISS_PARAMETER))
    {
        L4_BACKDOOR_FillParameterByDfltValue(command_p, flags, parameters);
    }

    if (TRUE == L4_BACKDOOR_ValidateParameters(command_p, flags, parameters))
    {
        UI32_T res;

        if (flags & _OPT_DEBUG)
        {
            UI32_T i;
            BACKDOOR_MGR_Printf(" Options\n ");
            if (flags & _OPT_INSTALL)
            {
                BACKDOOR_MGR_Printf(" /i");
            }
            if (flags & _OPT_UNINSTALL)
            {
                BACKDOOR_MGR_Printf(" /u");
            }
            if (flags & _OPT_SHOW)
            {
                BACKDOOR_MGR_Printf(" /s");
            }

            BACKDOOR_MGR_Printf("\n Command\n");
            BACKDOOR_MGR_Printf("  %s -- %s", command_p->name, command_p->desc);

            BACKDOOR_MGR_Printf("\n Parameters\n");
            for (i=0; i<PARAM_COUNT; i++)
            {
                if (parameters[i].valid)
                {
                    BACKDOOR_MGR_Printf("  --%s", l4_backdoor_def_param_table[i].name);
                    if (parameters[i].has_value)
                    {
                        BACKDOOR_MGR_Printf(" = %s", parameters[i].value);
                    }
                    BACKDOOR_MGR_Printf("\n");
                }
            }
        }

        res = ((L4_BACKDOOR_CommandFuncPtr_T)command_p->exec)(command_p, flags, parameters);

        if (_FAILED(res))
        {
            BACKDOOR_MGR_Printf("...Failed\n");
        }
    }

about:
    ;
}

/*
 syntax
    set
    set ?
    set parameter value [parameter value ...]
 */
static void L4_Backdoor_CpuIf_Set(
    int argc,
    char** argv
    )
{
    UI32_T i = 0;
    UI32_T opt_index;
    L4_BACKDOOR_Command_T commands[PARAM_COUNT];

    if (argc == 0)
    {
        for (i=0; i<PARAM_COUNT; i++)
        {
            L4_BACKDOOR_DefaultArgument_T *dp_p = &l4_backdoor_default_parameters[i];
            L4_BACKDOOR_Option_T *opt_p = &l4_backdoor_def_param_table[i];

            if (FALSE == dp_p->valid)
            {
                continue;
            }

            BACKDOOR_MGR_Printf("%s=%s\n", opt_p->name, dp_p->value);
        }
    }

    opt_index = 0;

    /* convert parameter list to command list
     */
    for (i=0; i<PARAM_COUNT;i++)
    {
        commands[i].name = l4_backdoor_def_param_table[i].name;
    }

    while (TRUE)
    {
        char *param_p = (opt_index<argc) ? argv[ opt_index++ ] : NULL;
        char *val_p   = (opt_index<argc) ? argv[ opt_index++ ] : NULL;
        L4_BACKDOOR_Command_T *command_p = NULL;
        UI32_T param_index;
        L4_BACKDOOR_DefaultArgument_T dp;

        if (NULL == param_p)
        {
            break;
        }

        command_p = L4_Backdoor_FindCommand(param_p, commands, _countof(commands));
        if (NULL == command_p)
        {
            goto help;
        }

        if (FALSE == L4_BACKDOOR_GetParameterIndexFromName(command_p->name, &param_index))
        {
            goto help;
        }

        memset(&dp, 0, sizeof(dp));

        if (val_p)
        {
            strncpy(dp.value, val_p, sizeof(dp.value)-1);
            dp.value[sizeof(dp.value)-1] = '\0';
            dp.mode = _ARG_FIX;
            dp.valid = TRUE;
        }
        else
        {
            dp.valid = FALSE;
        }

        L4_BACKDOOR_SetDefaultArgument(param_index, &dp);
    }

    return;

help:
    BACKDOOR_MGR_Printf("hi\n");
    ;
}

// TODO: New option ci /s installed-rules [--list] [--action {tocpu}]
/* ci /s installed-rules [--unit d] [--device d] [--group d] [--func-type d] [--port d] [--tag str]
 */
static int L4_Backdoor_CpuIf_installed_rules(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    RULE_CTRL_Filter_T  filter;

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        if (parameters[PARAM_GROUP].valid)
        {
            filter.group_id = (UI32_T)atoi(parameters[PARAM_GROUP].value);
        }

        if (parameters[PARAM_FUNC_TYPE].valid)
        {
            filter.func_type = atoi(parameters[PARAM_FUNC_TYPE].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        if (parameters[PARAM_TAG].valid)
        {
            strncpy(filter.tag, parameters[PARAM_TAG].value, sizeof(filter.tag)-1);
            filter.tag[sizeof(filter.tag)-1] = '\0';
        }

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

/* TODO: ci /s last-installed-rule [--num d]
 */
/* TODO: ci /s last-removed-rule [--num d]
 */
/* TODO: ci /s group [--id d]
 */
#if 0 /* FIXME: Some function on MARVELL doen't implement, mask it first */
static int L4_Backdoor_CpuIf_l4_dport_cos(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    UI32_T dummy_unit = 1;
    UI32_T dummy_port = 1;
    UI16_T l4_dport;
    UI8_T  cos;
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        l4_dport = (UI16_T)atoi(parameters[PARAM_L4_DST_PORT].value);
        cos      = (UI8_T)atoi(parameters[PARAM_COS].value);

        if (options & _OPT_INSTALL)
        {
            if (FALSE == RULE_CTRL_SetL4PortCosMap(dummy_unit, dummy_port, l4_dport, cos))
            {
                res = _E_FAILED;
            }
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "l4-dport-cos", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

#if (SYS_CPNT_DOT1X == TRUE)
static int L4_Backdoor_CpuIf_dot1x(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        UI32_T unit;
        UI32_T port;
        RULE_TYPE_CpuRuleInfo_T ri;
        BOOL_T enable;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        unit = atoi(parameters[PARAM_UNIT].value);
        port = atoi(parameters[PARAM_PORT].value);

        ri.dot1x.ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);
        L4_Backdoor_Str2MacAddr(parameters[PARAM_DA].value, ri.dot1x.cpu_mac);

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_DOT1X, &ri))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        strncpy(filter.tag, "1x-uc-trap", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}
#endif /* SYS_CPNT_DOT1X */

#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
static int L4_Backdoor_CpuIf_ipsg_permit_ip(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        BOOL_T enable;
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (parameters[PARAM_SA].valid)
        {
            RULE_CTRL_UnitPortSaVidSip_T param;

            param.unit = atoi(parameters[PARAM_UNIT].value);
            param.port = atoi(parameters[PARAM_PORT].value);
            param.vid  = atoi(parameters[PARAM_VID].value);

            L4_Backdoor_Str2IpAddr(parameters[PARAM_SRC_IP].value, &param.sip);
            L4_Backdoor_Str2MacAddr(parameters[PARAM_SA].value, param.sa);

            if (FALSE == RULE_CTRL_Service(enable, RULE_CTRL_IP_SOURCE_GUARD_PERMIT_SA_VID_SIP, &param))
            {
                res = _E_FAILED;
            }
        }
        else
        {
            RULE_CTRL_UnitPortVidSip_T param;

            param.unit = atoi(parameters[PARAM_UNIT].value);
            param.port = atoi(parameters[PARAM_PORT].value);
            param.vid  = atoi(parameters[PARAM_VID].value);

            L4_Backdoor_Str2IpAddr(parameters[PARAM_SRC_IP].value, &param.sip);

            if (FALSE == RULE_CTRL_Service(enable, RULE_CTRL_IP_SOURCE_GUARD_PERMIT_VID_SIP, &param))
            {
                res = _E_FAILED;
            }
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        if (parameters[PARAM_VID].valid)
        {
            filter.rule_outer_vlan = (UI32_T)atoi(parameters[PARAM_VID].value);
        }

        if (parameters[PARAM_SRC_IP].valid)
        {
            L4_Backdoor_Str2IpAddr(parameters[PARAM_SRC_IP].value, filter.rule_src_ip.b);
        }

        if (parameters[PARAM_SA].valid)
        {
            L4_Backdoor_Str2MacAddr(parameters[PARAM_SA].value, filter.rule_src_mac);
        }

        strncpy(filter.tag, "ipsg-permit-ip", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_ipsg_deny_ip(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_CTRL_UnitPort_T param;
        BOOL_T enable;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        param.unit = atoi(parameters[PARAM_UNIT].value);
        param.port = atoi(parameters[PARAM_PORT].value);

        if (FALSE == RULE_CTRL_Service(enable, RULE_CTRL_IP_SOURCE_GUARD_DENY_IP, &param))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        strncpy(filter.tag, "ipsg-deny-ip", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}
#endif /* #if (SYS_CPNT_IP_SOURCE_GUARD == TRUE) */

#if (SYS_CPNT_WEBAUTH == TRUE)
static int L4_Backdoor_CpuIf_wa_trap_http(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_CTRL_UnitPort_T param;
        BOOL_T enable;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        param.unit = atoi(parameters[PARAM_UNIT].value);
        param.port = atoi(parameters[PARAM_PORT].value);

        if (FALSE == RULE_CTRL_Service(enable, RULE_CTRL_WEBAUTH_REDIR_HTTP, &param))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        strncpy(filter.tag, "wa-trap-http", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_wa_deny_ip(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_CTRL_UnitPort_T param;
        BOOL_T enable;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        param.unit = atoi(parameters[PARAM_UNIT].value);
        param.port = atoi(parameters[PARAM_PORT].value);

        if (FALSE == RULE_CTRL_Service(enable, RULE_CTRL_WEBAUTH_DENY_IP, &param))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        strncpy(filter.tag, "wa-deny-ip", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_wa_permit_ip(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_CTRL_UnitPortSip_T param;
        BOOL_T                  enable;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        param.unit = atoi(parameters[PARAM_UNIT].value);
        param.port = atoi(parameters[PARAM_PORT].value);
        L4_Backdoor_Str2IpAddr(parameters[PARAM_SRC_IP].value, &param.sip);

        if (FALSE == RULE_CTRL_Service(enable, RULE_CTRL_WEBAUTH_PERMIT_SIP, &param))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        if (parameters[PARAM_SRC_IP].valid)
        {
            L4_Backdoor_Str2IpAddr(parameters[PARAM_SRC_IP].value, filter.rule_src_ip.b);
        }

        strncpy(filter.tag, "wa-permit-ip", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_wa_permit_dhcp(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_CTRL_UnitPort_T param;
        BOOL_T enable;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        param.unit = atoi(parameters[PARAM_UNIT].value);
        param.port = atoi(parameters[PARAM_PORT].value);

        if (FALSE == RULE_CTRL_Service(enable, RULE_CTRL_WEBAUTH_PERMIT_DHCPC, &param))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        strncpy(filter.tag, "wa-permit-dhcp", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_wa_permit_dns(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_CTRL_UnitPort_T param;
        BOOL_T enable;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        param.unit = atoi(parameters[PARAM_UNIT].value);
        param.port = atoi(parameters[PARAM_PORT].value);

        if (FALSE == RULE_CTRL_Service(enable, RULE_CTRL_WEBAUTH_PERMIT_DNS, &param))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        strncpy(filter.tag, "wa-permit-dns", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}
#endif /* SYS_CPNT_WEBAUTH */

#if (SYS_CPNT_ERPS == TRUE)
static int L4_Backdoor_CpuIf_erps(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    BOOL_T enable;
    UI32_T unit;
    UI32_T port;
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        unit = atoi(parameters[PARAM_UNIT].value);
        port = atoi(parameters[PARAM_PORT].value);

        if (FALSE == RULE_CTRL_ERPS_TrapRapsPduToCPU(enable, unit, port))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        strncpy(filter.tag, "erps", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}
#endif /* SYS_CPNT_ERPS */

#if (SYS_CPNT_EFM_OAM_REMOTE_LB_PASSIVELY == TRUE) && (SYS_CPNT_EFM_OAM_LOOPBACK_BY_RULE == TRUE)
static int L4_Backdoor_CpuIf_oam_cpe_oam_pdu(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    BOOL_T enable;
    UI32_T unit;
    UI32_T port;
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        unit = atoi(parameters[PARAM_UNIT].value);
        port = atoi(parameters[PARAM_PORT].value);

        if (FALSE == RULE_CTRL_OAM_CPE_TrapOamPduToCPU(enable, unit, port))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        strncpy(filter.tag, "oam-cpe-oam-pdu", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_oam_cpe_oam_loopback(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    BOOL_T enable;
    UI32_T unit;
    UI32_T port;
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        unit = atoi(parameters[PARAM_UNIT].value);
        port = atoi(parameters[PARAM_PORT].value);

        if (FALSE == RULE_CTRL_OAM_CPE_RedirOamLoopbackToRecvPort(enable, unit, port))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        strncpy(filter.tag, "oam-cpe-oam-loopback", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}
#endif /* #if (SYS_CPNT_EFM_OAM_REMOTE_LB_PASSIVELY == TRUE) && (SYS_CPNT_EFM_OAM_LOOPBACK_BY_RULE == TRUE) */

#if (SYS_CPNT_EFM_OAM_REMOTE_LB_ACTIVELY == TRUE)
static int L4_Backdoor_CpuIf_oam_co_oam_loopback(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    BOOL_T enable;
    UI32_T unit;
    UI32_T port;
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        unit = atoi(parameters[PARAM_UNIT].value);
        port = atoi(parameters[PARAM_PORT].value);

        if (FALSE == RULE_CTRL_OAM_CO_TrapOamLoopbackToCPU(enable, unit, port))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_PORT].valid)
        {
            filter.port = (UI32_T)atoi(parameters[PARAM_PORT].value);
        }

        strncpy(filter.tag, "oam-co-oam-loopback", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}
#endif /* SYS_CPNT_EFM_OAM_REMOTE_LB_ACTIVELY */

#if (SYS_CPNT_IPV6 == TRUE)
static int L4_Backdoor_CpuIf_link_local_addr(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_TYPE_CpuRuleInfo_T ri;
        BOOL_T enable;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        ri.linklocal.vlan_id = (UI32_T)atoi(parameters[PARAM_VID].value);
        L4_Backdoor_Str2Ip6Addr(parameters[PARAM_DST_IP6].value, ri.linklocal.addr);

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_LinkLocal, &ri))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        if (parameters[PARAM_VID].valid)
        {
            filter.rule_outer_vlan = (UI32_T) atoi(parameters[PARAM_VID].value);
        }

        if (parameters[PARAM_DST_IP6].valid)
        {
            L4_Backdoor_Str2Ip6Addr(parameters[PARAM_DST_IP6].value, (UI8_T*)&filter.rule_dst_ip6);
        }

        strncpy(filter.tag, "ip6-link-local", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}
#endif /* SYS_CPNT_IPV6 */

static int L4_Backdoor_CpuIf_eaps(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_TYPE_CpuRuleInfo_T ri;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (options & _OPT_INSTALL)
        {
            ri.eaps.vid  = (UI32_T)atoi(parameters[PARAM_VID].value);
            ri.eaps.mode = parameters[PARAM_TRANSITION].valid ? SYS_ADPT_EAPS_TRANSITION_MODE : SYS_ADPT_EAPS_MASTER_MODE;
        }

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_EAPS, &ri))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "eaps", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_arp_req_copy_to_cpu(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_ARP_REQUEST, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "arp-request-copy-to-cpu", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_arp_req_redir_to_cpu(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_TYPE_CpuRuleInfo_T ri;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        ri.arp.all_arp_to_cpu = enable ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_ARP_REQUEST, &ri))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "arp-request-redir-to-cpu", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_arp_reply_copy_to_cpu(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_ARP_REPLY, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "arp-reply-copy-to-cpu", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_arp_reply_redir_to_cpu(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_TYPE_CpuRuleInfo_T ri;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        ri.arp.all_arp_to_cpu = enable ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_ARP_REPLY, &ri))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "arp-reply-redir-to-cpu", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_rip(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_RIP, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "rip", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_ospf(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_OSPF, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "ospf", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

#if (SYS_CPNT_OSPF6 == TRUE)
static int L4_Backdoor_CpuIf_ospf6(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_OSPF6, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "ospf6", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}
#endif /* SYS_CPNT_OSPF6 */

static int L4_Backdoor_CpuIf_igmp(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_IGMP, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "igmp", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_l2_slf(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_L2_SLF, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "l2-slf", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_slf(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_TYPE_CpuRuleInfo_T ri;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        ri.slf.slf_vid= (UI16_T)atoi(parameters[PARAM_VID].value);

        if (enable)
        {
            ri.slf.action = parameters[PARAM_REDIR].valid ? SLF_REDIRECT :
                            parameters[PARAM_DROP].valid  ? SLF_DROP :
                                                            SLF_NO_ACTION;
        }

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_SLF, &ri))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        if (parameters[PARAM_VID].valid)
        {
            filter.rule_outer_vlan = (UI32_T)atoi(parameters[PARAM_VID].value);
        }

        strncpy(filter.tag, "slf", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_ip_unknown_mc(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_TYPE_CpuRuleInfo_T ri;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        ri.common.to_cpu = parameters[PARAM_TO_CPU].valid ? TRUE : FALSE;
        ri.common.flood  = parameters[PARAM_FLOOD].valid ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_UNKNOWN_IPMC, &ri))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "ip-unknown-mc", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_ip_bcast(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_IP_BCAST, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "ip-bc", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_pim(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_PIM, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "pim", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_vrrp(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_VRRP, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "vrrp", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_ip_all_host(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_ALL_HOST, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "ip-all-host", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_ip_all_routers(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_ALL_ROUTER, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "ip-all-routers", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_ip_option(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_IP_OPTION, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "ip-option", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_hbt(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_HBT, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "hbt", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_bpdu(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_BPDU, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "bpdu", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_l2cp(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_L2CP, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "l2cp", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

#if (SYS_CPNT_CFM == TRUE)
static int L4_Backdoor_CpuIf_cfm(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_CFM, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "cfm-cpu", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}
#endif /* SYS_CPNT_CFM */

static int L4_Backdoor_CpuIf_ip6_all_nodes(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_IPV6_ALL_NODES, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "ip6-all-nodes", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_ip6_all_routers(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_IPV6_ALL_ROUTERS, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "ip6-all-routers", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_pim6(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_PIM6, NULL))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "pim6", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}

static int L4_Backdoor_CpuIf_ip6_unknown_mc(
    L4_BACKDOOR_Command_T *command_p,
    UI32_T options,
    L4_BACKDOOR_Parameter_T *parameters
    )
{
    int res = _S_OK;
    BOOL_T enable;

    if (options & _OPT_INSTALL || options & _OPT_UNINSTALL)
    {
        RULE_TYPE_CpuRuleInfo_T ri;

        enable = (options & _OPT_INSTALL) ? TRUE : FALSE;
        ri.common.to_cpu = parameters[PARAM_TO_CPU].valid ? TRUE : FALSE;
        ri.common.flood  = parameters[PARAM_FLOOD].valid ? TRUE : FALSE;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable, RULE_TYPE_PacketType_UNKNOWN_IPV6MC, &ri))
        {
            res = _E_FAILED;
        }
    }

    if (options & _OPT_SHOW)
    {
        RULE_CTRL_Filter_T  filter;

        RULE_CTRL_InitFilter(&filter);

        if (parameters[PARAM_UNIT].valid)
        {
            filter.unit = (UI32_T)atoi(parameters[PARAM_UNIT].value);
        }

        if (parameters[PARAM_DEVICE].valid)
        {
            filter.device_id = (UI32_T)atoi(parameters[PARAM_DEVICE].value);
        }

        strncpy(filter.tag, "ip6-unknown-mc", sizeof(filter.tag)-1);
        filter.tag[sizeof(filter.tag)-1] = '\0';

        RULE_CTRL_DumpRule(&filter, BACKDOOR_MGR_Printf);
    }

    return res;
}
#endif /* 0 */

/*------------------------------------------------------------------------------
 *  DiffServ
 *------------------------------------------------------------------------------
 */
#if 0
static void L4_Backdoor_DiffServ_ShowClassType(RULE_TYPE_ClassType_T class_type)
{
    BACKDOOR_MGR_Printf("%s",
        (RULE_TYPE_CLASS_MF == class_type) ? "MF" :
        (RULE_TYPE_CLASS_ACL == class_type) ? "ACL" :
        "unknown");
}
#endif // #if 0

static void L4_Backdoor_DiffServ_DumpPolicyMap(const RULE_TYPE_PolicyMap_T *policy_map)
{
    UI32_T  element_cnt;

    if (NULL == policy_map)
        return;

    BACKDOOR_MGR_Printf("\r\n          name: %s", policy_map->name);
    BACKDOOR_MGR_Printf("\r\n          desc: %s", policy_map->desc);
    BACKDOOR_MGR_Printf("\r\n    port count: %u", policy_map->port_count);
    BACKDOOR_MGR_Printf("\r\n     row tatus: ");
    L4_Backdoor_ShowRowStatus(policy_map->row_status);

    BACKDOOR_MGR_Printf("\r\n element count: %u", policy_map->element_count);
    BACKDOOR_MGR_Printf("\r\n----------------- element list -----------------");
    for (element_cnt = 0; element_cnt < policy_map->element_count; ++element_cnt)
        BACKDOOR_MGR_Printf("\r\n%u ", policy_map->policy_map_element_index_list[element_cnt]);
}

static void L4_Backdoor_DiffServ_SetDebugFlag(int argc, char** argv)
{
    if (argc != 1)
    {
        BACKDOOR_MGR_Printf("\r\nparameters: {debug flag}");
        BACKDOOR_MGR_Printf("\r\n\tdebug flag:");
        BACKDOOR_MGR_Printf("\r\n\t\t  OM_ERR = %lu", RULE_OM_DEBUG_OM_ERR);
        BACKDOOR_MGR_Printf("\r\n\t\t MGR_ERR = %lu", RULE_OM_DEBUG_MGR_ERR);
        BACKDOOR_MGR_Printf("\r\n\t\tCTRL_ERR = %lu", RULE_OM_DEBUG_CTRL_ERR);
        return;
    }

    RULE_OM_SetDebugFlag(atoi(argv[0]));
}

static void L4_Backdoor_DiffServ_ShowPolicyMapTable(int argc, char** argv)
{
    UI32_T                  policy_map_index;
    RULE_TYPE_PolicyMap_T   policy_map;

    switch (argc)
    {
        case 1:
            if ('*' == argv[0][0])
            {
                policy_map_index = 0;
                while (TRUE == RULE_OM_GetNextPolicyMap(&policy_map_index, &policy_map))
                {
                    BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                    BACKDOOR_MGR_Printf("\r\n         index: %lu", policy_map_index);
                    L4_Backdoor_DiffServ_DumpPolicyMap(&policy_map);
                }
            }
            else
            {
                policy_map_index = atoi(argv[0]);
                if (FALSE == RULE_OM_GetPolicyMap(policy_map_index, &policy_map))
                {
                    BACKDOOR_MGR_Printf("\r\nfailed to call L4_MGR_QoS_GetDiffServPolicyMapEntry");
                    return;
                }
                BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                BACKDOOR_MGR_Printf("\r\n         index: %lu", policy_map_index);
                L4_Backdoor_DiffServ_DumpPolicyMap(&policy_map);
            }
            break;

        case 0:
        default:
            BACKDOOR_MGR_Printf("\r\nparameters: * | [policy_map_index]");
            return;
    }
}

static void L4_Backdoor_DiffServ_DumpPolicyMapElement(const RULE_TYPE_PolicyElement_T *policy_element)
{
    if (NULL == policy_element)
        return;

    BACKDOOR_MGR_Printf("\r\n   class-map index: %lu", policy_element->class_map_index);
    BACKDOOR_MGR_Printf("\r\n       meter index: %lu", policy_element->meter_index);
    BACKDOOR_MGR_Printf("\r\n      action index: %lu", policy_element->action_index);
    BACKDOOR_MGR_Printf("\r\n        row status: ");
    L4_Backdoor_ShowRowStatus(policy_element->row_status);
}

static void L4_Backdoor_DiffServ_ShowPolicyMapElementTable(int argc, char** argv)
{
    UI32_T                      element_index;
    RULE_TYPE_PolicyElement_T   element_entry;

    switch (argc)
    {
        case 1:
            if ('*' == argv[0][0])
            {
                element_index = 0;
                while (TRUE == L4_MGR_QoS_GetNextDiffServPolicyMapElementEntry(&element_index, &element_entry))
                {
                    BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                    BACKDOOR_MGR_Printf("\r\n     element index: %lu", element_index);
                    L4_Backdoor_DiffServ_DumpPolicyMapElement(&element_entry);
                }
            }
            else
            {
                element_index = atoi(argv[0]);
                if (FALSE == L4_MGR_QoS_GetDiffServPolicyMapElementEntry(element_index, &element_entry))
                {
                    BACKDOOR_MGR_Printf("\r\nfailed to call L4_MGR_QoS_GetDiffServPolicyMapElementEntry");
                    return;
                }
                BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                BACKDOOR_MGR_Printf("\r\n     element index: %lu", element_index);
                L4_Backdoor_DiffServ_DumpPolicyMapElement(&element_entry);
            }
            break;

        case 0:
        default:
            BACKDOOR_MGR_Printf("\r\nparameters: * | [element_index]");
            return;
    }
}

static void L4_Backdoor_DiffServ_ShowMibClassType(UI16_T class_type)
{
    BACKDOOR_MGR_Printf("%s",
        (VAL_diffServClassMapAttachCtlElementIndexType_macAce == class_type) ? "mac-ace" :
        (VAL_diffServClassMapAttachCtlElementIndexType_ipAce == class_type) ? "ip-ace" :
        (VAL_diffServClassMapAttachCtlElementIndexType_acl == class_type) ? "acl" :
        (VAL_diffServClassMapAttachCtlElementIndexType_ipv6Ace == class_type) ? "ipv6-ace" :
        "unknown");
}

static void L4_Backdoor_DiffServ_DumpClassMap(const RULE_TYPE_MIB_ClassMap_T *class_map)
{
    UI32_T  element_cnt, total_element;
    UI16_T  *class_type_p, *class_index_p;

    if (NULL == class_map)
        return;

    BACKDOOR_MGR_Printf("\r\n            name: %s", class_map->class_map_name);
    BACKDOOR_MGR_Printf("\r\n            desc: %s", class_map->desc);
    BACKDOOR_MGR_Printf("\r\n      match type: %s",
        (VAL_diffServClassMapMatchType_matchAny == class_map->class_map_match_type) ? "match-any" :
        (VAL_diffServClassMapMatchType_matchAll == class_map->class_map_match_type) ? "match-all" :
        "unknown");

    BACKDOOR_MGR_Printf("\r\n       row tatus: ");
    L4_Backdoor_ShowRowStatus(class_map->row_status);

    total_element = class_map->size_of_list / 2;

    BACKDOOR_MGR_Printf("\r\n   element count: %lu", total_element);
    BACKDOOR_MGR_Printf("\r\n----------------- element list -----------------");

    class_type_p = (UI16_T*)class_map->class_type;
    class_index_p  = (UI16_T*)class_map->class_index_list;

    for (element_cnt = 0; element_cnt < total_element; ++element_cnt)
    {
        BACKDOOR_MGR_Printf("\r\n(");
        L4_Backdoor_DiffServ_ShowMibClassType(class_type_p[element_cnt]);
        BACKDOOR_MGR_Printf(":%u) ", class_index_p[element_cnt]);
    }
}

static void L4_Backdoor_DiffServ_ShowClassMapTable(int argc, char** argv)
{
    UI32_T                      class_map_index;
    RULE_TYPE_MIB_ClassMap_T    class_map;

    switch (argc)
    {
        case 1:
            if ('*' == argv[0][0])
            {
                class_map_index = 0;
                while (TRUE == L4_MGR_QoS_GetNextDiffServClassMapEntry(&class_map_index, &class_map))
                {
                    BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                    BACKDOOR_MGR_Printf("\r\n class-map index: %lu", class_map_index);
                    L4_Backdoor_DiffServ_DumpClassMap(&class_map);
                }
            }
            else
            {
                class_map_index = atoi(argv[0]);
                if (FALSE == L4_MGR_QoS_GetDiffServClassMapEntry(class_map_index, &class_map))
                {
                    BACKDOOR_MGR_Printf("\r\nfailed to call L4_MGR_QoS_GetDiffServClassMapEntry");
                    return;
                }
                BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                BACKDOOR_MGR_Printf("\r\n class-map index: %lu", class_map_index);
                L4_Backdoor_DiffServ_DumpClassMap(&class_map);
            }
            break;

        case 0:
        default:
            BACKDOOR_MGR_Printf("\r\nparameters: * | [class_map_index]");
            return;
    }
}

#if 0
static void L4_Backdoor_DiffServ_DumpRuleInstance(BOOL_T show_title, const RULE_TYPE_RuleInstance_T *rule_instance)
{
    const char title_str[] = "\r\nrule      meter     action    ace ";
    const char edge_line[] = "\r\n--------- --------- --------- ---------";
    const char data_fmt[] = "\r\n%9u%10u%10u%10u";

    if (TRUE == show_title)
    {
        BACKDOOR_MGR_Printf((char *)title_str);
        BACKDOOR_MGR_Printf((char *)edge_line);
    }

    if (NULL == rule_instance)
        return;

    BACKDOOR_MGR_Printf((char *)data_fmt, rule_instance->dev_rule_id, rule_instance->meter_id,
        rule_instance->action_id, rule_instance->ace_id);
}

static void L4_Backdoor_DiffServ_DumpClassInstance(const RULE_TYPE_ClassInstance_T *class_instance)
{
    if (NULL == class_instance)
        return;

    BACKDOOR_MGR_Printf("\r\n       << class instance >>");
    BACKDOOR_MGR_Printf("\r\n            valid: %s", (TRUE == class_instance->valid) ? "TRUE" : "FALSE");
    BACKDOOR_MGR_Printf("\r\n         group id: %u", class_instance->group_id);
    BACKDOOR_MGR_Printf("\r\n   class template: type=");
    L4_Backdoor_DiffServ_ShowClassType(class_instance->class_template.class_type);
    BACKDOOR_MGR_Printf(" id=%u", class_instance->class_template.class_id);
    BACKDOOR_MGR_Printf("\r\n        rule num : %lu", class_instance->rule_num);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    BACKDOOR_MGR_Printf("\r\ntime_range_index : %u" , class_instance->time_range_index);
#endif /*#if (SYS_CPNT_TIME_BASED_ACL == TRUE)*/

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    BACKDOOR_MGR_Printf("\r\n  counter enable : %u" , RULE_TYPE_COUNTER_ENABLE_STR(class_instance->counter_enable));
#endif /* #if (SYS_CPNT_ACL_COUNTER == TRUE) */

    switch (class_instance->class_template.class_type)
    {
        case RULE_TYPE_CLASS_MF:
            L4_Backdoor_DiffServ_DumpRuleInstance(TRUE, &class_instance->instance.mf_rule_id);
            break;

        case RULE_TYPE_CLASS_ACL:
        {
            UI32_T  ace_cnt;

            for (ace_cnt = 0; ace_cnt < class_instance->rule_num; ++ace_cnt)
                L4_Backdoor_DiffServ_DumpRuleInstance((0 == ace_cnt), &class_instance->instance.acl_instance[ace_cnt]);
            break;
        }
        default:
            BACKDOOR_MGR_Printf("\r\nunknown class type");
            return;
    }
}
#endif // #if 0

static void L4_Backdoor_DiffServ_DumpAcl(UI32_T acl_index, const RULE_TYPE_Acl_T *acl_entry)
{
    RULE_TYPE_AclType_T ace_type;

    UI32_T  ace_cnt, unit_cnt, chip_cnt;
    UI32_T  ace_index;

    RULE_TYPE_Ace_Entry_T ace_entry;

    if (NULL == acl_entry)
        return;

    BACKDOOR_MGR_Printf("\r\n        acl type: %s",
        (RULE_TYPE_MAC_ACL == acl_entry->acl_type) ? "mac-acl" :
        (RULE_TYPE_IP_ACL == acl_entry->acl_type) ? "ip-acl" :
        (RULE_TYPE_IP_STD_ACL == acl_entry->acl_type) ? "ip-std-acl" :
        (RULE_TYPE_IP_EXT_ACL == acl_entry->acl_type) ? "ip-ext-acl" :
        (RULE_TYPE_MF == acl_entry->acl_type) ? "mf" :
        (RULE_TYPE_IPV6_ACL == acl_entry->acl_type) ? "ipv6-acl" :
        (RULE_TYPE_IPV6_STD_ACL == acl_entry->acl_type) ? "ipv6-std-acl" :
        (RULE_TYPE_IPV6_EXT_ACL == acl_entry->acl_type) ? "ipv6-ext-acl" :
        "unknown");

    BACKDOOR_MGR_Printf("\r\n            name: %s", acl_entry->acl_name);
    BACKDOOR_MGR_Printf("\r\n       row tatus: ");
    L4_Backdoor_ShowRowStatus(acl_entry->row_status);

#if (SYS_CPNT_ACL_MIRROR == TRUE)
    BACKDOOR_MGR_Printf("\r\n   mirror enable: %s", RULE_TYPE_MIRROR_ENABLE_STR(acl_entry->mirror.enable));
    BACKDOOR_MGR_Printf("\r\n       dest port: %lu", acl_entry->mirror.dest_ifindex);
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

    BACKDOOR_MGR_Printf("\r\n------- dump ingress port list -------");
    L4_Backdoor_DumpRawDataInHex(acl_entry->filtering_in_port_list, SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST, TRUE);
    BACKDOOR_MGR_Printf("\r\n------- dump egress port list -------");
    L4_Backdoor_DumpRawDataInHex(acl_entry->filtering_out_port_list, SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST, TRUE);

    BACKDOOR_MGR_Printf("\r\n class-map count: %lu", acl_entry->class_map_count);
    BACKDOOR_MGR_Printf("\r\n------- dump class-map list -------");
    L4_Backdoor_DumpRawDataInHex(acl_entry->class_map_bit_list, RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_CLASS_MAP_LIST, TRUE);

    BACKDOOR_MGR_Printf("\r\n       ace count: %lu", acl_entry->ace_count);
    BACKDOOR_MGR_Printf("\r\n----------------- ace index list -----------------");

    for (ace_index = 0, ace_cnt = 0;
         RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry);
         ++ace_cnt)
    {
        if ((ace_cnt % 8) == 0)
            BACKDOOR_MGR_Printf("\r\n");
        BACKDOOR_MGR_Printf("(%03lu) ", ace_index);
    }

    BACKDOOR_MGR_Printf("\r\n------------------- instance ---------------------");
    for (unit_cnt = 0; SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK > unit_cnt; ++unit_cnt)
    {
        for (chip_cnt = 0; SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > chip_cnt; ++chip_cnt)
        {
            if (acl_entry->filtering_acl_instance_ref_count[unit_cnt][chip_cnt])
            {
                BACKDOOR_MGR_Printf("\r\nunit(%lu) chip(%lu)", unit_cnt + 1, chip_cnt);
                BACKDOOR_MGR_Printf("\r\nreference count: %u", acl_entry->filtering_acl_instance_ref_count[unit_cnt][chip_cnt]);
            }
            RULE_CTRL_Backdoor_DiffServ_DumpClassInstance(acl_entry->filtering_acl_instance[unit_cnt][chip_cnt]);
        }
    }
}

static void L4_Backdoor_DiffServ_ShowAclTable(int argc, char** argv)
{
    UI32_T              acl_index;
    RULE_TYPE_Acl_T     acl_entry;

    switch (argc)
    {
        case 1:
            if ('*' == argv[0][0])
            {
                acl_index = 0;
                while (RULE_TYPE_OK == RULE_OM_GetNextAclByIndex(&acl_index, &acl_entry))
                {
                    BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                    BACKDOOR_MGR_Printf("\r\n       acl index: %lu", acl_index);
                    L4_Backdoor_DiffServ_DumpAcl(acl_index, &acl_entry);
                }
            }
            else
            {
                acl_index = atoi(argv[0]);
                if (RULE_TYPE_OK != RULE_OM_GetAclByIndex(acl_index, &acl_entry))
                {
                    BACKDOOR_MGR_Printf("\r\nfailed to call RULE_OM_GetAclByIndex");
                    return;
                }
                BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                BACKDOOR_MGR_Printf("\r\n       acl index: %lu", acl_index);
                L4_Backdoor_DiffServ_DumpAcl(acl_index, &acl_entry);
            }
            break;

        case 0:
        default:
            BACKDOOR_MGR_Printf("\r\nparameters: * | [acl_index]");
            return;
    }
}


static BOOL_T L4_Backdoor_IsNullMac(const UI8_T *mac)
{
    #if 0
    UI8_T   null_mac[SYS_ADPT_MAC_ADDR_LEN];

    memset(null_mac, 0, SYS_ADPT_MAC_ADDR_LEN);
    #endif

    return (CHECK_MAC_IS_NULL(mac));
}

static void L4_Backdoor_ShowAceAction (UI8_T access)
{
    if (RULE_TYPE_ACE_PERMIT == access)
        BACKDOOR_MGR_Printf ("/ permit");
    else
        BACKDOOR_MGR_Printf ("/ deny");
}

static void L4_Backdoor_DiffServ_DumpMacAce(const RULE_TYPE_Ace_Entry_T *ace_p)
{
    const RULE_TYPE_MacAce_Entry_T *mac_ace = &ace_p->u.mac;
    L4_Backdoor_ShowAceAction(ace_p->access);

    switch (mac_ace->acePktformat)
    {
    case VAL_aclMacAcePktformat_untagged_Eth2:
        BACKDOOR_MGR_Printf("/ untagged-eth2");
        break;

    case VAL_aclMacAcePktformat_untagged802Dot3:
        BACKDOOR_MGR_Printf("/ untagged-802.3");
        break;

    case VAL_aclMacAcePktformat_tagggedEth2:
        BACKDOOR_MGR_Printf("/ tagged-eth2");
        break;

    case VAL_aclMacAcePktformat_tagged802Dot3:
        BACKDOOR_MGR_Printf("/ tagged-802.3");
        break;

    case VAL_aclMacAcePktformat_any:
    default:
        break; /* ignore this field */
    }

    if (FALSE == L4_Backdoor_IsNullMac((UI8_T *)mac_ace->aceSourceMacAddr))
    {
        BACKDOOR_MGR_Printf ("/ sa:");
        L4_Backdoor_ShowMacAddr ((UI8_T *)mac_ace->aceSourceMacAddr);
        BACKDOOR_MGR_Printf (" sa-mask:");
        L4_Backdoor_ShowMacAddr ((UI8_T *)mac_ace->aceSourceMacAddrBitmask);
    }

    if (FALSE == L4_Backdoor_IsNullMac((UI8_T *)mac_ace->aceDestMacAddr))
    {
        BACKDOOR_MGR_Printf ("/ da:");
        L4_Backdoor_ShowMacAddr ((UI8_T *)mac_ace->aceDestMacAddr);
        BACKDOOR_MGR_Printf (" da-mask:");
        L4_Backdoor_ShowMacAddr ((UI8_T *)mac_ace->aceDestMacAddrBitmask);
    }

    if (mac_ace->aceVidOp == VAL_diffServMacAceVidOp_equal)
    {
        BACKDOOR_MGR_Printf ("/ vid %04lx vid-mask %04lx", mac_ace->aceMinVid, mac_ace->aceVidBitmask);
    }

    if (mac_ace->aceEtherTypeOp == VAL_diffServMacAceEtherTypeOp_equal)
    {
        BACKDOOR_MGR_Printf ("/ eth %04lx eth-mask %04lx", mac_ace->aceMinEtherType, mac_ace->aceEtherTypeBitmask);
    }
}

static void L4_Backdoor_DiffServ_DumpIpAce(const RULE_TYPE_Ace_Entry_T *ace_p)
{
    const RULE_TYPE_IpAce_Entry_T *ip_ace = &ace_p->u.ip;
    L4_Backdoor_ShowAceAction(ace_p->access);

    if(ip_ace->aceSourceIpAddrBitmask != RULE_TYPE_ACL_IP_BITMASK_ANY)
    {
        BACKDOOR_MGR_Printf ("/ sip:");
        L4_Backdoor_ShowIpAddr ((UI8_T *)&ip_ace->aceSourceIpAddr);
        BACKDOOR_MGR_Printf (" sip-mask:");
        L4_Backdoor_ShowIpAddr ((UI8_T *)&ip_ace->aceSourceIpAddrBitmask);
    }

    if(ip_ace->aceDestIpAddrBitmask != RULE_TYPE_ACL_IP_BITMASK_ANY)
    {
        BACKDOOR_MGR_Printf ("/ dip:");
        L4_Backdoor_ShowIpAddr ((UI8_T *)&ip_ace->aceDestIpAddr);
        BACKDOOR_MGR_Printf (" dip-mask:");
        L4_Backdoor_ShowIpAddr ((UI8_T *)&ip_ace->aceDestIpAddrBitmask);
    }

    if (ip_ace->aceProtocol != RULE_TYPE_UNDEF_IP_PROTOCOL)
    {
        BACKDOOR_MGR_Printf ("/ pro %02lx pro-mask %02x", ip_ace->aceProtocol, 0xFF);
    }

    if (ip_ace->acePrec != RULE_TYPE_UNDEF_IP_PRECEDENCE)
    {
        BACKDOOR_MGR_Printf ("/ prec %02lx prec-mask %02x", ip_ace->acePrec, 0x7);
    }

    if (ip_ace->aceTos != RULE_TYPE_UNDEF_IP_TOS)
    {
        BACKDOOR_MGR_Printf ("/ prec %02lx prec-mask %02x", ip_ace->aceTos, 0xf);
    }

    if (ip_ace->aceDscp != RULE_TYPE_UNDEF_IP_DSCP)
    {
        BACKDOOR_MGR_Printf ("/ dscp %02lx dscp-mask %02x", ip_ace->aceDscp, 63);
    }

    if (ip_ace->aceSourcePortOp == VAL_diffServIpAceSourcePortOp_equal) /* currently only support this operator */
    {
        BACKDOOR_MGR_Printf ("/ sport %04lx sport-mask %04lx", ip_ace->aceMinSourcePort, ip_ace->aceSourcePortBitmask);
    }

    if (ip_ace->aceDestPortOp == VAL_diffServIpAceDestPortOp_equal) /* currently only support this operator */
    {
        BACKDOOR_MGR_Printf ("/ dport %04lx dport-mask %04lx", ip_ace->aceMinDestPort, ip_ace->aceDestPortBitmask);
    }

    if (ip_ace->aceControlCode != RULE_TYPE_UNDEF_IP_CONTROL_CODE)
    {
        BACKDOOR_MGR_Printf ("/ ctrl %02lx ctrl-mask %02lx", ip_ace->aceControlCode, ip_ace->aceControlCodeBitmask);
    }

    {
        /* BCM_FIELD_PKT_FMT_IPV4 */
        BACKDOOR_MGR_Printf ("/ [pkformat %02x pktformat-mask %02x]", 0x0010, 0x0010);
    }
}

static BOOL_T L4_Backdoor_IsNullIpv6Addr(const UI8_T *ipv6_addr)
{
    UI8_T   null_ipv6[SYS_ADPT_IPV6_ADDR_LEN];

    memset(null_ipv6, 0, SYS_ADPT_IPV6_ADDR_LEN);

    return (0 == memcmp(ipv6_addr, null_ipv6, SYS_ADPT_IPV6_ADDR_LEN));
}

static void L4_Backdoor_ShowIpv6Addr(const UI8_T *addr,const UI8_T *mask)
{
    UI32_T idx;
    UI32_T prefix_len, byte_cnt, bit_cnt;

    if (NULL == addr)
        return;

    for (idx =0; idx <SYS_ADPT_IPV6_ADDR_LEN; idx++)
    {
      BACKDOOR_MGR_Printf("%02x", addr[idx]);

      if (((idx %4)==3) && (idx != SYS_ADPT_IPV6_ADDR_LEN-1))
        BACKDOOR_MGR_Printf ("-");
    }

    prefix_len = 0;

    for (byte_cnt = 0; byte_cnt < SYS_ADPT_IPV6_ADDR_LEN; byte_cnt++)
    {
        if (mask[byte_cnt] == 0xff)
        {
            prefix_len += 8;
        }
        else
        {
            for (bit_cnt = 7; bit_cnt > 0 ; bit_cnt--)
            {
                if (0 == (mask[byte_cnt] & (1 << bit_cnt)))
                {
                    break;
                }

                 prefix_len += 1;
            }

            break;
        }
    }

    BACKDOOR_MGR_Printf(" /0x%02x", prefix_len);
}

static void L4_Backdoor_DiffServ_DumpIpv6Ace(const RULE_TYPE_Ace_Entry_T *ace_p)
{
	const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace = &ace_p->u.ipv6;
    L4_Backdoor_ShowAceAction(ace_p->access);

    if (FALSE == L4_Backdoor_IsNullIpv6Addr(ipv6_ace->aceSourceIpAddr))
    {
        BACKDOOR_MGR_Printf ("/ sipv6:");
        L4_Backdoor_ShowIpv6Addr (ipv6_ace->aceSourceIpAddr, ipv6_ace->aceSourceIpAddrBitmask);
    }

    if (FALSE == L4_Backdoor_IsNullIpv6Addr(ipv6_ace->aceDestIpAddr))
    {
        BACKDOOR_MGR_Printf ("/ dipv6:");
        L4_Backdoor_ShowIpv6Addr (ipv6_ace->aceDestIpAddr, ipv6_ace->aceDestIpAddrBitmask);
    }

    if (ipv6_ace->aceNextHeader != RULE_TYPE_UNDEF_IPV6_NEXT_HEADER)
    {
        BACKDOOR_MGR_Printf ("/ nh %04x nh-mask %04x", ipv6_ace->aceNextHeader, 0xffff);
    }

    if (ipv6_ace->aceDscp != RULE_TYPE_UNDEF_IPV6_DSCP)
    {
        BACKDOOR_MGR_Printf ("/ tc %02lx tc-mask %02x", ipv6_ace->aceDscp, 63);
    }

    if (ipv6_ace->aceFlowLabel != RULE_TYPE_UNDEF_IPV6_FLOW_LABEL)
    {
        /* flow-label only has 20 bits */
        BACKDOOR_MGR_Printf ("/ fl %05lx fl-mask %05x", ipv6_ace->aceFlowLabel, 0xFFFFF);
    }

    {
        /* BCM_FIELD_PKT_FMT_IPV6 */
        BACKDOOR_MGR_Printf ("/ [pkformat %02x pktformat-mask %02x]", 0x0020, 0x0020);
    }
}

static void L4_Backdoor_DiffServ_DumpAce(const RULE_TYPE_Ace_Entry_T *ace_entry)
{
    char   *tag_ary[] = {"mac", "ip", "ip-std", "ip-ext", "mf",
                          "ipv6", "ipv6-std", "ipv6-ext"};

    if (ace_entry->ace_type <= RULE_TYPE_IPV6_EXT_ACL)
    {
        BACKDOOR_MGR_Printf ("\r\n ace_type: %s-ace\r\n", tag_ary[ace_entry->ace_type]);

        switch (ace_entry->ace_type)
        {
        case RULE_TYPE_MAC_ACL:
            L4_Backdoor_DiffServ_DumpMacAce(ace_entry);
            break;

        case RULE_TYPE_IP_ACL:
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            L4_Backdoor_DiffServ_DumpIpAce(ace_entry);
            break;

        case RULE_TYPE_IPV6_ACL:
        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            L4_Backdoor_DiffServ_DumpIpv6Ace(ace_entry);
            break;

        case RULE_TYPE_MF:
        default:
            BACKDOOR_MGR_Printf ("NA");
            break;
        }
    }

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    BACKDOOR_MGR_Printf ("/ time range index : %d ", ace_entry->time_range_index);
#endif /* #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */


    BACKDOOR_MGR_Printf ("\r\n");
}

static void L4_Backdoor_DiffServ_ShowAceTable(int argc, char** argv)
{
    UI32_T                  ace_index;
    RULE_TYPE_Ace_Entry_T   ace_entry;

    switch (argc)
    {
        case 1:
            if ('*' == argv[0][0])
            {

                for (ace_index = 0; ace_index <SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM; ace_index ++)
                {
                    if (RULE_TYPE_OK == RULE_OM_GetAceByIndex (ace_index, &ace_entry))
                    {
                        BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                        BACKDOOR_MGR_Printf("\r\n       ace index: %lu", ace_index);
                        L4_Backdoor_DiffServ_DumpAce(&ace_entry);
                    }
                }
            }
            else
            {
                ace_index = atoi(argv[0]);
                if (RULE_TYPE_OK != RULE_OM_GetAceByIndex (ace_index, &ace_entry))
                {
                    BACKDOOR_MGR_Printf("\r\nfailed to call RULE_MGR_GetAceByIndex");
                    return;
                }
                BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                BACKDOOR_MGR_Printf("\r\n       ace index: %lu", ace_index);
                L4_Backdoor_DiffServ_DumpAce(&ace_entry);
            }
            break;

        case 0:
        default:
            BACKDOOR_MGR_Printf("\r\nparameters: * | [ace_index]");
            return;
    }
}

static void L4_Backdoor_DiffServ_DumpMeter(const RULE_TYPE_TBParamEntry_T *meter_entry)
{
    if (NULL == meter_entry)
        return;

    BACKDOOR_MGR_Printf("\r\n reference count: %u", meter_entry->ref_count);
    BACKDOOR_MGR_Printf("\r\n           model: %s",
        (VAL_diffServMeterModel_default == meter_entry->meter_model) ? "default" :
        (VAL_diffServMeterModel_flow == meter_entry->meter_model) ? "flow" :
        (VAL_diffServMeterModel_trTcmColorBlind == meter_entry->meter_model) ? "trColorBlind" :
        (VAL_diffServMeterModel_trTcmColorAware == meter_entry->meter_model) ? "trColorAware" :
        (VAL_diffServMeterModel_srTcmColorBlind == meter_entry->meter_model) ? "srColorBlind" :
        (VAL_diffServMeterModel_srTcmColorAware == meter_entry->meter_model) ? "srColorAware" :
        "unknown");

    BACKDOOR_MGR_Printf("\r\n            rate: %lu", meter_entry->rate);
    BACKDOOR_MGR_Printf("\r\n      burst size: %lu", meter_entry->burst_size);
    BACKDOOR_MGR_Printf("\r\n       peak rate: %lu", meter_entry->peak_rate);
    BACKDOOR_MGR_Printf("\r\n peak burst size: %lu", meter_entry->peak_burst_size);
    BACKDOOR_MGR_Printf("\r\n        interval: %lu", meter_entry->interval);
    BACKDOOR_MGR_Printf("\r\n       row tatus: ");
    L4_Backdoor_ShowRowStatus(meter_entry->row_status);
}

static void L4_Backdoor_DiffServ_ShowMeterTable(int argc, char** argv)
{
    UI32_T                      meter_index;
    RULE_TYPE_TBParamEntry_T    meter_entry;

    switch (argc)
    {
        case 1:
            if ('*' == argv[0][0])
            {
                meter_index = 0;
                while (TRUE == L4_MGR_QoS_GetNextDiffServMeterEntry(&meter_index, &meter_entry))
                {
                    BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                    BACKDOOR_MGR_Printf("\r\n     meter index: %lu", meter_index);
                    L4_Backdoor_DiffServ_DumpMeter(&meter_entry);
                }
            }
            else
            {
                meter_index = atoi(argv[0]);
                if (FALSE == L4_MGR_QoS_GetDiffServMeterEntry(meter_index, &meter_entry))
                {
                    BACKDOOR_MGR_Printf("\r\nfailed to call L4_MGR_QoS_GetDiffServAclEntry");
                    return;
                }
                BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                BACKDOOR_MGR_Printf("\r\n     meter index: %lu", meter_index);
                L4_Backdoor_DiffServ_DumpMeter(&meter_entry);
            }
            break;

        case 0:
        default:
            BACKDOOR_MGR_Printf("\r\nparameters: * | [meter_index]");
            return;
    }
}

static void L4_Backdoor_DiffServ_TranslateActionBmp (UI32_T action_bmp)
{
//#define L4_BACKDOOR_MAX_UI_ACTION_TYPE      5
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

    UI32_T      i, out_mask =0, act_bary[] =
                                {RULE_TYPE_ACTION_PKT_NEW_PRI,
                                 RULE_TYPE_ACTION_PKT_NEW_TOS,
                                 RULE_TYPE_ACTION_PKT_NEW_DSCP,
                                 RULE_TYPE_ACTION_RED_DROP,
                                 RULE_TYPE_ACTION_RED_PKT_NEW_DSCP,
                                 RULE_TYPE_ACTION_PKT_NEW_PHB};

    char       *action_tag[]=
                                {"in-cos", "in-prec", "in-dscp",
                                 "out-drop", "out-dscp", "in-phb"};

    BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
    BACKDOOR_MGR_Printf("\r\n             ui action:");

    for (i =0 ; i < ARRAY_SIZE(act_bary); i++)
    {
        if (action_bmp & act_bary [i])
        {
            out_mask |= act_bary [i];
            BACKDOOR_MGR_Printf (" %s,", action_tag [i]);
        }
    }

    BACKDOOR_MGR_Printf (" [bmp(hex): %08lx]", out_mask);
}

static void L4_Backdoor_DiffServ_DumpAction(const RULE_TYPE_Action_T *action_entry)
{
    if (NULL == action_entry)
        return;

    BACKDOOR_MGR_Printf("\r\n           bitmap(hex): %08lx", action_entry->action_bitmap);
    BACKDOOR_MGR_Printf("\r\n               new pri: %lu", action_entry->pkt_new_pri);
    BACKDOOR_MGR_Printf("\r\n               new tos: %lu", action_entry->pkt_new_tos);
    BACKDOOR_MGR_Printf("\r\n              new dscp: %lu", action_entry->pkt_new_dscp);
    BACKDOOR_MGR_Printf("\r\n          red new dscp: %lu", action_entry->red_pkt_new_dscp);
    BACKDOOR_MGR_Printf("\r\n       yellow new dscp: %lu", action_entry->yellow_pkt_new_dscp);
    BACKDOOR_MGR_Printf("\r\n       drop precedence: %lu", action_entry->drop_precedence);
    BACKDOOR_MGR_Printf("\r\n   red drop precedence: %lu", action_entry->red_drop_precedence);
    BACKDOOR_MGR_Printf("\r\nyellow drop precedence: %lu", action_entry->yellow_drop_precedence);
    BACKDOOR_MGR_Printf("\r\n             row tatus: ");
    L4_Backdoor_ShowRowStatus(action_entry->row_status);
    L4_Backdoor_DiffServ_TranslateActionBmp(action_entry->action_bitmap);
}

static void L4_Backdoor_DiffServ_ShowActionTable(int argc, char** argv)
{
    UI32_T              action_index;
    RULE_TYPE_Action_T  action_entry;

    switch (argc)
    {
        case 1:
            if ('*' == argv[0][0])
            {
                action_index = 0;
                while (TRUE == RULE_OM_GetNextActionEntry(&action_index, &action_entry))
                {
                    BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                    BACKDOOR_MGR_Printf("\r\n          action index: %lu", action_index);
                    L4_Backdoor_DiffServ_DumpAction(&action_entry);
                }
            }
            else
            {
                action_index = atoi(argv[0]);
                if (FALSE == RULE_OM_GetActionEntryByIndex(action_index, &action_entry))
                {
                    BACKDOOR_MGR_Printf("\r\nfailed to call L4_MGR_QoS_GetDiffServAclEntry");
                    return;
                }
                BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                BACKDOOR_MGR_Printf("\r\n          action index: %lu", action_index);
                L4_Backdoor_DiffServ_DumpAction(&action_entry);
            }
            break;

        case 0:
        default:
            BACKDOOR_MGR_Printf("\r\nparameters: * | [action_index]");
            return;
    }
}

static void L4_Backdoor_DiffServ_DumpUiPort(const RULE_TYPE_UI_PortEntry_T *port_entry)
{
    if (NULL == port_entry)
        return;

    if ((0 == port_entry->policy_map_index)     &&
        (0 == port_entry->ingress_mac_acl_index)&&
        (0 == port_entry->ingress_ip_acl_index) &&
        (0 == port_entry->ingress_ipv6_acl_index))
    {
        BACKDOOR_MGR_Printf("\r\n  empty");
    }
    else
    {
        BACKDOOR_MGR_Printf("\r\n           ingress mac acl index: %u %s",
            port_entry->ingress_mac_acl_index, (port_entry->bind_dynamic_acl)?"(dynamic)":"");
        BACKDOOR_MGR_Printf("\r\n            ingress ip acl index: %u %s",
            port_entry->ingress_ip_acl_index, (port_entry->bind_dynamic_acl)?"(dynamic)":"");
        BACKDOOR_MGR_Printf("\r\n          ingress ipv6 acl index: %u %s",
            port_entry->ingress_ipv6_acl_index, (port_entry->bind_dynamic_acl)?"(dynamic)":"");
        BACKDOOR_MGR_Printf("\r\n                policy-map index: %u %s",
            port_entry->policy_map_index, (port_entry->bind_dynamic_policy_map)?"(dynamic)":"");

        if (port_entry->bind_dynamic_acl)
        {
            BACKDOOR_MGR_Printf("\r\n  user cfg ingress mac acl index: %u", port_entry->user_cfg_ingress_mac_acl_index);
            BACKDOOR_MGR_Printf("\r\n   user cfg ingress ip acl index: %u", port_entry->user_cfg_ingress_ip_acl_index);
            BACKDOOR_MGR_Printf("\r\n user cfg ingress ipv6 acl index: %u", port_entry->user_cfg_ingress_ipv6_acl_index);
        }

        if (port_entry->bind_dynamic_policy_map)
        {
            BACKDOOR_MGR_Printf("\r\n       user cfg policy-map index: %u", port_entry->user_cfg_policy_map_index);
        }
    }

    SYSFUN_Sleep(20);
}

static void L4_Backdoor_DiffServ_DumpPort(const RULE_TYPE_PortEntry_T *port_entry)
{
    UI32_T      i, j;

    if (NULL == port_entry)
        return;

    /* FIXME: implement or remove ??
     */
    BACKDOOR_MGR_Printf("\r\nNot implement yet!");
//    BACKDOOR_MGR_Printf("\r\nmac acl instance");
//    RULE_CTRL_Backdoor_DiffServ_DumpClassInstance(port_entry->ingress_mac_acl);
//
//    BACKDOOR_MGR_Printf("\r\nip acl instance");
//    RULE_CTRL_Backdoor_DiffServ_DumpClassInstance(port_entry->ingress_ip_acl);
//
//    BACKDOOR_MGR_Printf("\r\nipv6 acl instance");
//    RULE_CTRL_Backdoor_DiffServ_DumpClassInstance(port_entry->ingress_ipv6_acl);
//
//    BACKDOOR_MGR_Printf("\r\npolicy-map index: %u", port_entry->policy_map_index);
//    RULE_CTRL_Backdoor_DiffServ_DumpClassInstance(port_entry->policy_map_instance_ptr);
}

static void L4_Backdoor_DiffServ_ShowPortTable(int argc, char** argv)
{
    UI32_T                      ifindex;
    RULE_TYPE_InOutDirection_T  inout_profile;
    RULE_TYPE_UI_PortEntry_T    ui_port_entry;

    switch (argc)
    {
        case 1:
            if ('*' == argv[0][0])
            {
                ifindex = 0;
                while (TRUE == L4_MGR_QoS_GetNextDiffServPortEntry(&ifindex, &inout_profile, &ui_port_entry))
                {
                    BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                    BACKDOOR_MGR_Printf("\r\n                ifindex: %lu", ifindex);
                    BACKDOOR_MGR_Printf("\r\n              direction:%s",
                                        RULE_TYPE_INBOUND == inout_profile? "ingress" : "egress");
                    L4_Backdoor_DiffServ_DumpUiPort(&ui_port_entry);
                }
            }
            else
            {
                RULE_TYPE_PortEntry_T   port_entry;
                RULE_TYPE_InOutDirection_T last_direction = RULE_TYPE_INBOUND;

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
                last_direction = RULE_TYPE_OUTBOUND;
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */

                ifindex = atoi(argv[0]);

                for (inout_profile = RULE_TYPE_INBOUND; inout_profile <= last_direction; ++inout_profile)
                {
                    if ((FALSE == L4_MGR_QoS_GetDiffServPortEntry(ifindex, inout_profile, &ui_port_entry)) ||
                        (RULE_TYPE_OK != RULE_OM_Get_PortEntryEx(ifindex, inout_profile, &port_entry)))
                    {
                        BACKDOOR_MGR_Printf("\r\nfailed to call L4_MGR_QoS_GetDiffServAclEntry or RULE_OM_Get_PortEntry");
                        return;
                    }
                    BACKDOOR_MGR_Printf("\r\n-------------------------------------------------");
                    BACKDOOR_MGR_Printf("\r\n                ifindex: %lu", ifindex);
                    BACKDOOR_MGR_Printf("\r\n              direction:%s",
                                        RULE_TYPE_INBOUND == inout_profile? "ingress" : "egress");
                    BACKDOOR_MGR_Printf("\r\nUI PortEntry");
                    L4_Backdoor_DiffServ_DumpUiPort(&ui_port_entry);

                    BACKDOOR_MGR_Printf("\r\nPortEntry");
                    L4_Backdoor_DiffServ_DumpPort(&port_entry);
                }
            }
            break;

        case 0:
        default:
            BACKDOOR_MGR_Printf("\r\nparameters: * | [ifindex]");
            return;
    }
}

static void L4_Backdoor_DiffServ_ShowResourceInfo(int argc, char** argv)
{
    if (1 != argc && 3 != argc)
    {
        BACKDOOR_MGR_Printf("\r\nparameters: * | {unit} {device_id} {selector_id}");
        return;
    }

    if (1 == argc)
    {
        UI32_T unit;

        if (0 != strcmp(argv[0], "*"))
            return;

#ifdef MARVELL_CPSS
        for (unit = 1; unit < SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
        {
            UI32_T port;

            for (port = 0; port < SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT; ++port)
            {
                if (FALSE == RULE_CTRL_DumpResourceInfo(unit, port, 0))
                {
                    BACKDOOR_MGR_Printf("\r\nFailed to call RULE_CTRL_DumpResourceInfo(%lu, %lu, %lu)",
                        unit, port, 0);
                }
            }
        }
#else 
        for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
        {
            UI32_T device_id;
            UI32_T group;

            for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; ++device_id)
            {
                for (group = 0; group < SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP; ++group)
                {
                    if (FALSE == RULE_CTRL_DumpResourceInfo(unit, device_id, group))
                    {
                        BACKDOOR_MGR_Printf("\r\nFailed to call RULE_CTRL_DumpResourceInfo(%lu, %lu, %lu)",
                            unit, device_id, group);
                    }
                }
            }
        }
#endif /* MARVELL_CPSS */
    }
    else if (3 == argc)
    {
        if (FALSE == RULE_CTRL_DumpResourceInfo(atoi(argv[0]), atoi(argv[1]), atoi(argv[2])))
        {
            BACKDOOR_MGR_Printf("\r\nFailed to call RULE_CTRL_DumpResourceInfo(%lu, %lu, %lu)",
                atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
        }
    }
}

static void L4_Backdoor_DiffServ_ShowCosReference(int argc, char** argv)
{
    BOOL_T  show_title;
    UI32_T  key_value, index, policy_map_index, class_map_index;

    for (key_value = 0; RULE_TYPE_MAX_OF_IP_DSCP > key_value; key_value++)
    {
        show_title = TRUE;
        index = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextIpDscpReference(key_value, &index, &policy_map_index, &class_map_index))
        {
            if (TRUE == show_title)
            {
                BACKDOOR_MGR_Printf("\r\nIP DSCP (%lu)", key_value);
                BACKDOOR_MGR_Printf("\r\npolicy-map class-map");
                BACKDOOR_MGR_Printf("\r\n---------- ----------");
                show_title = FALSE;
            }
            BACKDOOR_MGR_Printf("\r\n%10lu %10lu", policy_map_index, class_map_index);
        }
    }

    for (key_value = 0; RULE_TYPE_MAX_OF_IP_PRECEDENCE > key_value; key_value++)
    {
        show_title = TRUE;
        index = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextIpPrecedenceReference(key_value, &index, &policy_map_index, &class_map_index))
        {
            if (TRUE == show_title)
            {
                BACKDOOR_MGR_Printf("\r\nIP Precedence (%lu)", key_value);
                BACKDOOR_MGR_Printf("\r\npolicy-map class-map");
                BACKDOOR_MGR_Printf("\r\n---------- ----------");
                show_title = FALSE;
            }
            BACKDOOR_MGR_Printf("\r\n%10lu %10lu", policy_map_index, class_map_index);
        }
    }
}

#if (SYS_CPNT_ACL_MIRROR == TRUE)
static void L4_Backdoor_DiffServ_AddAclMirror(int argc, char** argv)
{
    UI32_T ret;
    UI32_T ifindex_dest;
    char *acl_name;

    if (argc != 2)
    {
        BACKDOOR_MGR_Printf("\r\nparameters: {ifindex_dest} {acl_name}");
        return;
    }

    ifindex_dest = atol(argv[0]);
    acl_name = argv[1];

    BACKDOOR_MGR_Printf("AddAclMirror, ifindex=%lu, acl_name=%s\r\n",
        ifindex_dest, acl_name);

    ret = L4_MGR_ACL_AddAclMirrorEntry(ifindex_dest, acl_name);

    if (ret != RULE_TYPE_OK)
    {
        BACKDOOR_MGR_Printf("RemoveAclMirror failed (%lu)\r\n", ret);
    }
}

static void L4_Backdoor_DiffServ_RemoveAclMirror(int argc, char ** argv)
{
    UI32_T ret;
    UI32_T ifindex_dest;
    char *acl_name;

    if (argc != 2)
    {
        BACKDOOR_MGR_Printf("\r\nparameters: {ifindex_dest} {acl_name}");
        return;
    }

    ifindex_dest = atol(argv[0]);
    acl_name = argv[1];

    BACKDOOR_MGR_Printf("RemoveAclMirror, ifindex=%lu, acl_name=%s\r\n",
        ifindex_dest, acl_name);

    ret = L4_MGR_ACL_RemoveAclMirrorEntry(ifindex_dest, acl_name);

    if (ret != RULE_TYPE_OK)
    {
        BACKDOOR_MGR_Printf("RemoveAclMirror failed (%lu)\r\n", ret);
    }

}
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */


#ifndef L4_UT
static void L4_Backdoor_Cos_CosMode(int argc, char **argv)
{
    enum
    {
        DSCP = 0,
        PRECEDENCE,
        PORT
    };

    if (argc != 1)
    {
        BACKDOOR_MGR_Printf("\r\n Set cos mode");
        BACKDOOR_MGR_Printf("\r\n parameters: {mode}");
        BACKDOOR_MGR_Printf("\r\n       mode: %d(dscp)   %d(port)   %d(precedence)",
                             DSCP, PORT, PRECEDENCE);
    }
    else
    {
        UI32_T  mode    = atoi(argv[0]);

        if (mode == DSCP || mode == PRECEDENCE)
        {
            COS_VM_MAPPING_FLAG_T flag;

            flag = (mode == DSCP)
                 ? COS_VM_DSCP_MAPPING_ENABLED
                 : COS_VM_IPPRECEDENCE_MAPPING_ENABLED
                 ;


            if (FALSE == L4_MGR_COS_SetIpPrecedenceDscpMappingStatus(flag))
            {
                BACKDOOR_MGR_Printf("\r\nL4_MGR_COS_SetIpPrecedenceDscpMappingStatus(%d) failed", flag);
                return;
            }
        }
        else if (mode == PORT)
        {
            if (FALSE == L4_MGR_COS_SetTcpPortMappingStatus(COS_VM_MAPPING_ENABLED))
            {
                BACKDOOR_MGR_Printf("\r\nL4_MGR_COS_SetTcpPortMappingStatus(enable) failed");
                return;
            }
        }
        else
        {
            BACKDOOR_MGR_Printf("\r\nUnknown mode(%d)", mode);
        }
    }
}

static void L4_Backdoor_Cos_NoCosMode(int argc, char **argv)
{
    enum
    {
        DSCP = 0,
        PRECEDENCE,
        PORT
    };

    if (argc != 1)
    {
        BACKDOOR_MGR_Printf("\r\n Disable cos mode");
        BACKDOOR_MGR_Printf("\r\n parameters: {mode}");
        BACKDOOR_MGR_Printf("\r\n       mode: %d(dscp)   %d(port)   %d(precedence)",
                             DSCP, PORT, PRECEDENCE);
        return;
    }
    else
    {
        UI32_T  mode    = atoi(argv[0]);

        if (mode == DSCP || mode == PRECEDENCE)
        {
            COS_VM_MAPPING_FLAG_T flag;

            L4_MGR_COS_GetIpPrecedenceDscpMappingStatus(&flag);

            if ((mode == DSCP && flag == COS_VM_DSCP_MAPPING_ENABLED)
                || (mode == PRECEDENCE && flag == COS_VM_IPPRECEDENCE_MAPPING_ENABLED))
            {
                flag = COS_VM_DISABLE_BOTH;
            }

            if (FALSE == L4_MGR_COS_SetIpPrecedenceDscpMappingStatus(flag))
            {
                BACKDOOR_MGR_Printf("\r\nL4_MGR_COS_SetIpPrecedenceDscpMappingStatus(%d) failed", flag);
                return;
            }
        }
        else if (mode == PORT)
        {
            if (FALSE == L4_MGR_COS_SetTcpPortMappingStatus(COS_VM_MAPPING_DISABLED))
            {
                BACKDOOR_MGR_Printf("\r\nL4_MGR_COS_SetTcpPortMappingStatus(disabled) failed");
                return;
            }
        }
        else
        {
            BACKDOOR_MGR_Printf("\r\nUnknown mode(%d)", mode);
        }
    }
}

static void L4_Backdoor_Cos_ShowCosMode(int argc, char **argv)
{
    COS_VM_MAPPING_FLAG_T   dscp_flag;
    UI32_T                  port_flag;

    L4_MGR_COS_GetIpPrecedenceDscpMappingStatus(&dscp_flag);

    BACKDOOR_MGR_Printf("\r\nIP DSCP       : %s",
        (dscp_flag == COS_VM_DSCP_MAPPING_ENABLED) ? "Enabled" : "Disabled");
    BACKDOOR_MGR_Printf("\r\nIP Precedence : %s",
        (dscp_flag == COS_VM_IPPRECEDENCE_MAPPING_ENABLED) ? "Enabled" : "Disabled");


    L4_MGR_COS_GetTcpPortMappingStatus(&port_flag);

    BACKDOOR_MGR_Printf("\r\nTCP port      : %s",
        (port_flag == COS_VM_MAPPING_ENABLED) ? "Enabled" : "Disabled");
}

static void L4_Backdoor_Cos_Dscp2Cos(int argc, char **argv)
{
    if (argc != 3)
    {
        BACKDOOR_MGR_Printf("\r\n Set DSCP to COS mapping");
        BACKDOOR_MGR_Printf("\r\n parameters: {l_port} {dscp} {cos}");
        BACKDOOR_MGR_Printf("\r\n     l_port: logic port");
        BACKDOOR_MGR_Printf("\r\n       dscp: 0-63");
        BACKDOOR_MGR_Printf("\r\n        cos: 0-7");
    }
    else
    {
        UI32_T  l_port  = atoi(argv[0]);
        UI8_T   dscp    = atoi(argv[1]);
        UI32_T  cos;

        if (TRUE == L4_MGR_COS_GetDSCPToCos(l_port, dscp, &cos))
        {
            BACKDOOR_MGR_Printf("\r\nChange from DSCP(%u) COS(%lu)", dscp, cos);
        }

        cos = atoi(argv[2]);

        if (FALSE == L4_MGR_COS_SetDSCPToCos(l_port, dscp, cos))
        {
            BACKDOOR_MGR_Printf("\r\nL4_MGR_COS_SetDSCPToCos(l_port=%lu, dscp=%u, cos=%lu) failed",
                                 l_port, dscp, cos);
            return;
        }

        if (TRUE == L4_MGR_COS_GetDSCPToCos(l_port, dscp, &cos))
        {
            BACKDOOR_MGR_Printf(" to DSCP(%u) COS(%lu)", dscp, cos);
        }
    }
}

static void L4_Backdoor_Cos_NoDscp2Cos(int argc, char **argv)
{
    if (argc != 2)
    {
        BACKDOOR_MGR_Printf("\r\n Set DSCP to COS mapping to default value");
        BACKDOOR_MGR_Printf("\r\n parameters: {l_port} {dscp}");
        BACKDOOR_MGR_Printf("\r\n     l_port: logic port");
        BACKDOOR_MGR_Printf("\r\n       dscp: 0-63");
    }
    else
    {
        UI32_T  l_port  = atoi(argv[0]);
        UI8_T   dscp    = atoi(argv[1]);
        UI32_T  cos;

        if (TRUE == L4_MGR_COS_GetDSCPToCos(l_port, dscp, &cos))
        {
            BACKDOOR_MGR_Printf("\r\nChange from DSCP(%u) COS(%lu)", dscp, cos);
        }

        if (FALSE == L4_MGR_COS_DelDSCPToCos(l_port, dscp))
        {
            BACKDOOR_MGR_Printf("\r\nL4_MGR_COS_DelDSCPToCos(l_port=%lu, dscp=%u) failed",
                                 l_port, dscp);
            return;
        }

        if (TRUE == L4_MGR_COS_GetDSCPToCos(l_port, dscp, &cos))
        {
            BACKDOOR_MGR_Printf(" to DSCP(%u) COS(%lu)", dscp, cos);
        }
    }
}

static void L4_Backdoor_Cos_TcpPort2Cos(int argc, char **argv)
{
    if (argc != 3)
    {
        BACKDOOR_MGR_Printf("\r\n Set TCP port to COS mapping");
        BACKDOOR_MGR_Printf("\r\n parameters: {l_port} {dst_port} {cos}");
        BACKDOOR_MGR_Printf("\r\n     l_port: logic port");
        BACKDOOR_MGR_Printf("\r\n   dst_port: 0-65535");
        BACKDOOR_MGR_Printf("\r\n        cos: 0-7");
    }
    else
    {
        UI32_T  l_port;
        UI16_T  dst_port;
        UI32_T  cos;

        l_port      = atoi(argv[0]);
        dst_port    = atoi(argv[1]);
        cos         = atoi(argv[2]);

        if (FALSE == L4_MGR_COS_SetPortToCos(l_port,
                                             dst_port,
                                             cos))
        {
            BACKDOOR_MGR_Printf ("\r\n L4_MGR_COS_SetPortToCos");
            BACKDOOR_MGR_Printf ("\r\n (");
            BACKDOOR_MGR_Printf ("\r\n   l_port    = %lu", l_port);
            BACKDOOR_MGR_Printf ("\r\n   dst_port  = %hu", dst_port);
            BACKDOOR_MGR_Printf ("\r\n   cos       = %lu", cos);
            BACKDOOR_MGR_Printf ("\r\n )");
            BACKDOOR_MGR_Printf ("\r\n failed");
        }
    }
}

static void L4_Backdoor_Cos_NoTcpPort2Cos(int argc, char **argv)
{
    if (argc != 3)
    {
        BACKDOOR_MGR_Printf("\r\n Set TCP port to COS mapping to default value");
        BACKDOOR_MGR_Printf("\r\n parameters: {l_port} {dst_port} {cos}");
        BACKDOOR_MGR_Printf("\r\n     l_port: logic port");
        BACKDOOR_MGR_Printf("\r\n   dst_port: 0-65535");
        BACKDOOR_MGR_Printf("\r\n        cos: 0-7");
    }
    else
    {
        UI32_T  l_port;
        UI16_T  dst_port;
        UI32_T  cos;

        l_port      = atoi(argv[0]);
        dst_port    = atoi(argv[1]);

        if (FALSE == L4_MGR_COS_DelPortToCos(l_port,
                                             dst_port))
        {
            BACKDOOR_MGR_Printf ("\r\n L4_MGR_COS_DelPortToCos");
            BACKDOOR_MGR_Printf ("\r\n (");
            BACKDOOR_MGR_Printf ("\r\n   l_port    = %lu", l_port);
            BACKDOOR_MGR_Printf ("\r\n   dst_port  = %hu", dst_port);
            BACKDOOR_MGR_Printf ("\r\n )");
            BACKDOOR_MGR_Printf ("\r\n failed");
        }
    }
}

static void L4_Backdoor_Cos_Precedence2Cos(int argc, char **argv)
{
    if (argc != 3)
    {
        BACKDOOR_MGR_Printf("\r\n Set TOS to COS mapping");
        BACKDOOR_MGR_Printf("\r\n parameters: {l_port} {tos} {cos}");
        BACKDOOR_MGR_Printf("\r\n     l_port: logic port");
        BACKDOOR_MGR_Printf("\r\n        tos: 0-7");
        BACKDOOR_MGR_Printf("\r\n        cos: 0-7");
    }
    else
    {
        UI32_T  l_port  = atoi(argv[0]);
        UI8_T   tos     = atoi(argv[1]);
        UI32_T  cos;

        if (TRUE == L4_MGR_COS_GetIpPrecedenceToCos(l_port, tos, &cos))
        {
            BACKDOOR_MGR_Printf("\r\nChange from TOS(%u) COS(%lu)", tos, cos);
        }

        cos = atoi(argv[2]);

        if (FALSE == L4_MGR_COS_SetIpPrecedenceToCos(l_port, tos, cos))
        {
            BACKDOOR_MGR_Printf("\r\nL4_MGR_COS_SetIpPrecedenceToCos(l_port=%lu, tos=%u, cos=%lu) failed",
                                 l_port, tos, cos);
            return;
        }

        if (TRUE == L4_MGR_COS_GetIpPrecedenceToCos(l_port, tos, &cos))
        {
            BACKDOOR_MGR_Printf(" to TOS(%u) COS(%lu)", tos, cos);
        }
    }
}

static void L4_Backdoor_Cos_NoPrecedence2Cos(int argc, char **argv)
{
    if (argc != 3)
    {
        BACKDOOR_MGR_Printf("\r\n Set TOS to COS mapping to default value");
        BACKDOOR_MGR_Printf("\r\n parameters: {l_port} {tos} {cos}");
        BACKDOOR_MGR_Printf("\r\n     l_port: logic port");
        BACKDOOR_MGR_Printf("\r\n        tos: 0-7");
        BACKDOOR_MGR_Printf("\r\n        cos: 0-7");
    }
    else
    {
        UI32_T  l_port  = atoi(argv[0]);
        UI8_T   tos     = atoi(argv[1]);
        UI32_T  cos;

        if (TRUE == L4_MGR_COS_GetIpPrecedenceToCos(l_port, tos, &cos))
        {
            BACKDOOR_MGR_Printf("\r\nChange from TOS(%u) COS(%lu)", tos, cos);
        }

        if (FALSE == L4_MGR_COS_DelIpPrecedenceToCos(l_port, tos))
        {
            BACKDOOR_MGR_Printf("\r\nL4_MGR_COS_DelIpPrecedenceToCos(l_port=%lu, tos=%u) failed",
                                 l_port, tos);
            return;
        }

        if (TRUE == L4_MGR_COS_GetIpPrecedenceToCos(l_port, tos, &cos))
        {
            BACKDOOR_MGR_Printf(" to TOS(%u) COS(%lu)", tos, cos);
        }
    }
}


static void L4_Backdoor_Cos_Cos2Queue(int argc, char **argv)
{
    if (argc != 3)
    {
        BACKDOOR_MGR_Printf("\r\n Set COS to Queue mapping");
        BACKDOOR_MGR_Printf("\r\n parameters: {l_port} {cos} {queue}");
        BACKDOOR_MGR_Printf("\r\n     l_port: logic port");
        BACKDOOR_MGR_Printf("\r\n        cos: 0-7");
        BACKDOOR_MGR_Printf("\r\n      queue: 0-7");
    }
    else
    {
        UI32_T                              l_port;
        UI32_T                              cos;
        UI32_T                              queue;
        PRI_MGR_Dot1dTrafficClassEntry_T    _1d_entry;

        l_port  = atoi(argv[0]);
        cos     = atoi(argv[1]);
        queue   = atoi(argv[2]);

        if (TRUE == PRI_MGR_GetDot1dTrafficClassEntry(l_port, cos, &_1d_entry))
        {
            BACKDOOR_MGR_Printf("\r\nChange from COS(%u) Queue(%lu)", cos, _1d_entry.dot1d_traffic_class);
        }

        if (FALSE == PRI_MGR_SetDot1dTrafficClass(l_port, cos, queue))
        {
            BACKDOOR_MGR_Printf("\r\nPRI_MGR_SetDot1dTrafficClass(l_port=%lu, cos=%lu, queue=%lu) failed",
                                 l_port, cos, queue);
            return;
        }


        if (TRUE == PRI_MGR_GetDot1dTrafficClassEntry(l_port, cos, &_1d_entry))
        {
            BACKDOOR_MGR_Printf(" to COS(%u) Queue(%lu)", cos, _1d_entry.dot1d_traffic_class);
        }
    }
}

static void L4_Backdoor_Cos_NoCos2Queue(int argc, char **argv)
{
    if (argc != 2)
    {
        BACKDOOR_MGR_Printf("\r\n Set COS to Queue mapping to default value");
        BACKDOOR_MGR_Printf("\r\n parameters: {l_port} {cos}");
        BACKDOOR_MGR_Printf("\r\n     l_port: logic port");
        BACKDOOR_MGR_Printf("\r\n        cos: 0-7");
    }
    else
    {
        UI32_T dflt_queue[] =
        {
            SYS_DFLT_1P_0_MAPPED_TRAFFIC_CLASS,
            SYS_DFLT_1P_1_MAPPED_TRAFFIC_CLASS,
            SYS_DFLT_1P_2_MAPPED_TRAFFIC_CLASS,
            SYS_DFLT_1P_3_MAPPED_TRAFFIC_CLASS,
            SYS_DFLT_1P_4_MAPPED_TRAFFIC_CLASS,
            SYS_DFLT_1P_5_MAPPED_TRAFFIC_CLASS,
            SYS_DFLT_1P_6_MAPPED_TRAFFIC_CLASS,
            SYS_DFLT_1P_7_MAPPED_TRAFFIC_CLASS,
            };

        UI32_T                              l_port;
        UI32_T                              cos;
        PRI_MGR_Dot1dTrafficClassEntry_T    _1d_entry;

        l_port  = atoi(argv[0]);
        cos     = atoi(argv[1]);

        if (sizeof(dflt_queue)/sizeof(dflt_queue[0]) <= cos)
        {
            BACKDOOR_MGR_Printf("\r\nCOS(%lu) too big", cos);
            return;
        }

        if (TRUE == PRI_MGR_GetDot1dTrafficClassEntry(l_port, cos, &_1d_entry))
        {
            BACKDOOR_MGR_Printf("\r\nChange from COS(%u) Queue(%lu)", cos, _1d_entry.dot1d_traffic_class);
        }

        if (FALSE == PRI_MGR_SetDot1dTrafficClass(l_port, cos, dflt_queue[cos]))
        {
            BACKDOOR_MGR_Printf("\r\nPRI_MGR_SetDot1dTrafficClass(l_port=%lu, cos=%lu, queue=%lu) failed",
                                 l_port, cos, dflt_queue[cos]);
            return;
        }

        if (TRUE == PRI_MGR_GetDot1dTrafficClassEntry(l_port, cos, &_1d_entry))
        {
            BACKDOOR_MGR_Printf(" to COS(%u) Queue(%lu)", cos, _1d_entry.dot1d_traffic_class);
        }
    }
}

static void L4_Backdoor_Cos_ShowCosMapping(int argc, char **argv)
{
    if (argc != 1)
    {
        BACKDOOR_MGR_Printf("\r\n Show COS mapping");
        BACKDOOR_MGR_Printf("\r\n parameters: {l_port}");
    }
    else
    {
        UI32_T                  l_port = atoi(argv[0]);
        COS_VM_MAPPING_FLAG_T   dscp_flag;
        UI32_T                  port_flag;
        UI32_T                  cos;

        L4_MGR_COS_GetIpPrecedenceDscpMappingStatus(&dscp_flag);
        L4_MGR_COS_GetTcpPortMappingStatus(&port_flag);

        if (dscp_flag == COS_VM_DSCP_MAPPING_ENABLED)
        {
            UI8_T   dscp;

            BACKDOOR_MGR_Printf("\r\nIP DSCP to Cos:");
            for (dscp=0; dscp <= COS_VM_DSCP_MAX; dscp++)
            {
                L4_MGR_COS_GetDSCPToCos(l_port, dscp, &cos);
                BACKDOOR_MGR_Printf("\r\n DSCP %2u ==> Cos %lu", dscp, cos);
            }
        }

        if (dscp_flag == COS_VM_IPPRECEDENCE_MAPPING_ENABLED)
        {
            UI8_T   tos;

            BACKDOOR_MGR_Printf("\r\nIP Precedence to Cos:");
            for (tos=0; tos <= COS_VM_IPPRECEDENCE_MAX; tos++)
            {
                L4_MGR_COS_GetIpPrecedenceToCos(l_port, tos, &cos);
                BACKDOOR_MGR_Printf("\r\n TOS %2u ==> Cos %lu", tos, cos);
            }
        }

        if (port_flag == COS_VM_MAPPING_ENABLED)
        {
            UI32_T lport = l_port;
            I32_T  tcp_port = -1;

            BACKDOOR_MGR_Printf("\r\nTCP port to Cos:");
            while (L4_MGR_COS_GetNextPortToCos(&lport, &tcp_port, &cos))
            {
                if (lport != l_port) break;
                BACKDOOR_MGR_Printf("\r\n TCP port %2lu ==> Cos %lu", tcp_port, cos);
            }
        }

        BACKDOOR_MGR_Printf("\r\n");
        BACKDOOR_MGR_Printf("\r\nCos to Queue:");
        for (cos=0; cos <= MAX_dot1dTrafficClassPriority; cos++)
        {
            PRI_MGR_Dot1dTrafficClassEntry_T    _1d_entry;

            PRI_MGR_GetDot1dTrafficClassEntry(l_port, cos, &_1d_entry);
            BACKDOOR_MGR_Printf("\r\n COS %lu ==> Queue %lu", cos, _1d_entry.dot1d_traffic_class);
        }

    }
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME:  L4_Backdoor_CleanWarningDummyFunc
 *------------------------------------------------------------------------------
 * PURPOSE  : to avoid compiler warnings because some backdoor may be disabled and static functions may not be used
 * INPUT    : none
 * OUTPUT   : none.
 * RETURN   : none
 * NOTES    : none.
 *------------------------------------------------------------------------------
 */
void L4_Backdoor_CleanWarningDummyFunc()
{
    L4_Backdoor_ShowIpAddr(NULL);
    L4_Backdoor_Str2IpAddr(NULL, NULL);
    L4_Backdoor_ShowMacAddr(NULL);
    L4_Backdoor_Str2MacAddr(NULL, NULL);
    L4_Backdoor_ShowRowStatus(0);
    L4_Backdoor_Help(0, NULL);
    L4_Backdoor_StrCmpIgnoreCase(NULL, NULL);
    L4_Backdoor_ChopString(NULL);
    L4_Backdoor_DumpRawDataInHex(NULL, 0, TRUE);
}

static void
L4_Backdoor_Cos_ShowPriorityCosMappingTable(
    int argc,
    char **argv)
{
    if (argc != 3)
    {
        BACKDOOR_MGR_Printf("\r\n Show cos_vm mapping table");
        BACKDOOR_MGR_Printf("\r\n parameters: {lport} {priority} {mapping_table}");
        BACKDOOR_MGR_Printf("\r\n  - lport        : logic port");
        BACKDOOR_MGR_Printf("\r\n     1 ~ %d", SYS_ADPT_TOTAL_NBR_OF_LPORT);
        BACKDOOR_MGR_Printf("\r\n  - priority     : ");
        BACKDOOR_MGR_Printf("\r\n     USER %d", COS_VM_PRIORITY_USER);
        BACKDOOR_MGR_Printf("\r\n     ETS  %d", COS_VM_PRIORITY_ETS);
        BACKDOOR_MGR_Printf("\r\n     CN   %d", COS_VM_PRIORITY_CN);
        BACKDOOR_MGR_Printf("\r\n  - mapping_table: ");
        BACKDOOR_MGR_Printf("\r\n     COS_TO_DSCP  %d", COS_VM_CONFIG_TABLE_COS2DSCP);
        BACKDOOR_MGR_Printf("\r\n     DSCP_TO_DSCP %d", COS_VM_CONFIG_TABLE_DSCP2DSCP);
        BACKDOOR_MGR_Printf("\r\n     DSCP_TO_QUEUE %d",COS_VM_CONFIG_TABLE_DSCP2QUEUE);
        BACKDOOR_MGR_Printf("\r\n     Queue weight %d", COS_VM_CONFIG_TABLE_QUEUEWEIGHT);
        BACKDOOR_MGR_Printf("\r\n     Queue mode   %d", COS_VM_CONFIG_TABLE_QUEUEMODE);
        BACKDOOR_MGR_Printf("\r\n");
    }
    else
    {
        UI32_T l_port = atoi(argv[0]);
        UI16_T priority = (UI16_T) atoi(argv[1]);
        UI32_T table = atoi(argv[2]);

        COS_VM_Backdoor_ShowCosMappingTable(l_port, priority, table);
    }
}

static void
L4_Backdoor_Cos_SetDebugMode(
    int argc,
    char **argv)
{
    if (argc != 1)
    {
        BACKDOOR_MGR_Printf("\r\n Set cos debug mode");
        BACKDOOR_MGR_Printf("\r\n parameters: {enable}");
        BACKDOOR_MGR_Printf("\r\n  - enable :");
        BACKDOOR_MGR_Printf("\r\n      TRUE   1");
        BACKDOOR_MGR_Printf("\r\n      FALSE  0");
        BACKDOOR_MGR_Printf("\r\n");
    }
    else
    {
        BOOL_T enable = (1 ==atoi(argv[0])) ? TRUE : FALSE;

        COS_VM_SetDebugMode(enable);
    }
}

#endif /* L4_UT */

#ifdef RUN_UNIT_TEST
#include "l4_backdoor_test.c"
#endif /* RUN_UNIT_TEST */

#endif /* L4_SUPPORT_ACCTON_BACKDOOR == TRUE */
