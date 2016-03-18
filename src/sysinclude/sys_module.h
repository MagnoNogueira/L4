/* Module Name: SYS_MODULE.H
 * Purpose:
 *              Define system wise module ID, each CSC own an unique ID.
 *
 * Notes:
 *  None.
 *
 * History:
 *      Date        -- Modifier,        Reason
 *      20020416    -- William Chiang,  Creation.
 *      2003.01.02  -- Jason Hsue,      Merge syslog_module_type to sys_module_type.
 *
 * Copyright(C)      Accton Corporation, 2002.
 */

#ifndef         _SYS_MODULE_H
#define         _SYS_MODULE_H

/* INCLUDE FILE DECLARATIONS
 */

/* NAME CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */
#define MODULE_ID(id, name)     id,
#define MODULE_NAME(id, name)   name,

/* IMPORTANT: Module ID is defined as follows.
 * SYS_MODULE_UNKNOWN will be treated as maximal id used so far.
 * There could be a new component which will assign id from SYS_MODULE_UNKNOWN
 * and of course SYS_MODULE_UNKNOWN value will be assigned to be larger value
 * and must add a new module name at sys_module_string in l_mname.c at the same index.
 * Note: The sequence of module id should not be changed.
 */
/* _(SYS_MODULE_XXX,           "xxx")
 *   ^                         ^
 *   |                         |
 *   ENUM for MODULD ID        Display name for the MODULE (used by l_mname.c)
 *                             maximum length is L_MNAME_MAX_NAME_LEN
 *
 *                             "no-use" means this module id is out of date
 *                             or the main CSC module id should be used for display.
 *
 *                             BOOTP            -> use dhcp_mgr if needed
 *                             PROTOCOL_VLAN    -> use vlan_mgr if needed
 *                             EFM_OAM_LOOPBACK -> use oam_mgr  if needed
 *
 */
#define SYSMOD_LIST(_) \
    _(SYS_MODULE_SYSTEM,                "Global System")    \
    _(SYS_MODULE_CLI,                   "CLI")              \
    _(SYS_MODULE_LEDMGMT,               "LED Mgmt")         \
    _(SYS_MODULE_RMON,                  "RMON")             \
    _(SYS_MODULE_RMON2,                 "RMON2")            \
    _(SYS_MODULE_SNMP,                  "SNMP")             \
    _(SYS_MODULE_TRAPMGMT,              "Trap Mgmt")        \
    _(SYS_MODULE_WEB,                   "Web")              \
    _(SYS_MODULE_BOOTP,                 "BootP")            \
    _(SYS_MODULE_DHCP,                  "DHCP")             \
    _(SYS_MODULE_DHCPv6,                "DHCPv6")           \
    _(SYS_MODULE_XFER,                  "File Tranfer")     \
    _(SYS_MODULE_BSP,                   "BSP")              \
    _(SYS_MODULE_EXTBRG,                "Extend Brg")       \
    _(SYS_MODULE_PRIMGMT,               "Priority Mgmt")    \
    _(SYS_MODULE_STA,                   "Spanning Tree")    \
    _(SYS_MODULE_DATABASE,              "DataBase")         \
    _(SYS_MODULE_IGMPSNP,               "IGMP Snooping")    \
    _(SYS_MODULE_MLDSNP,                "MLD Snooping")     \
    _(SYS_MODULE_MIB2MGMT,              "MIB2 Mgmt")        \
    _(SYS_MODULE_NETWORK,               "Network")          \
    _(SYS_MODULE_ROOT,                  "Root")             \
    _(SYS_MODULE_SECURITY,              "Security")         \
    _(SYS_MODULE_STKMGMT,               "Stack Mgmt")       \
    _(SYS_MODULE_SWCTRL,                "Swicth Control")   \
    _(SYS_MODULE_SYSLOG,                "System Log")       \
    _(SYS_MODULE_SYSMGMT,               "System Mgmt")      \
    _(SYS_MODULE_USERAUTH,              "User Auth")        \
    _(SYS_MODULE_VLAN,                  "VLAN")             \
    _(SYS_MODULE_GVRP,                  "GVRP")             \
    _(SYS_MODULE_GARP,                  "GARP")             \
    _(SYS_MODULE_L2MCAST,               "L2 Multicast")     \
    _(SYS_MODULE_LACP,                  "LACP")             \
    _(SYS_MODULE_FLASH,                 "Flash Driver")     \
    _(SYS_MODULE_IMC,                   "IMC")              \
    _(SYS_MODULE_LED,                   "LED")              \
    _(SYS_MODULE_LEDDRV,                "LED Driver")       \
    _(SYS_MODULE_NIC,                   "NIC Driver")       \
    _(SYS_MODULE_SEPM,                  "Serial Eprom")     \
    _(SYS_MODULE_SWITCH,                "Switch Driver")    \
    _(SYS_MODULE_FS,                    "File System")      \
    _(SYS_MODULE_COS,                   "CoS")              \
    _(SYS_MODULE_CGI,                   "CGI")              \
    _(SYS_MODULE_HTTP,                  "HTTP")             \
    _(SYS_MODULE_WEBAUTH,               "Web Auth")         \
    _(SYS_MODULE_VDSL,                  "VDSL Driver")      \
    _(SYS_MODULE_XSTP,                  "X Spanning Tree")  \
    _(SYS_MODULE_RADIUS,                "RADIUS")           \
    _(SYS_MODULE_DOT1X,                 "802.1X")           \
    _(SYS_MODULE_SSL,                   "SSL")              \
    _(SYS_MODULE_SSH,                   "SSH")              \
    _(SYS_MODULE_PROTOCOL_VLAN,         "Protocol VLAN")    \
    _(SYS_MODULE_RIP,                   "RIP")              \
    _(SYS_MODULE_OSPF,                  "OSPF")             \
    _(SYS_MODULE_OSPF6,                 "OSPF6")            \
    _(SYS_MODULE_DVMRP,                 "DVMRP")            \
    _(SYS_MODULE_PIM,                   "PIM")              \
    _(SYS_MODULE_LAN,                   "LAN")              \
    _(SYS_MODULE_IML,                   "IML")              \
    _(SYS_MODULE_P2,                    "Phase 2")          \
    _(SYS_MODULE_VENDER_DRV,            "Device Driver")    \
    _(SYS_MODULE_IFMGR,                 "IF")               \
    _(SYS_MODULE_NETCFG,                "Net Config")       \
    _(SYS_MODULE_ARPCFG,                "ARP")              \
    _(SYS_MODULE_NDCFG,                 "ND Config")        \
    _(SYS_MODULE_ICMPCFG,               "ICMP")             \
    _(SYS_MODULE_IGMPCFG,               "IGMP")             \
    _(SYS_MODULE_IPCFG,                 "IP Config")        \
    _(SYS_MODULE_ROUTECFG,              "Route Config")     \
    _(SYS_MODULE_RIPCFG,                "RIP Config")       \
    _(SYS_MODULE_OSPFCFG,               "OSPF Config")      \
    _(SYS_MODULE_NETIF,                 "L3 Intf Info")     \
    _(SYS_MODULE_PSEC,                  "Port Security")    \
    _(SYS_MODULE_TACACS,                "TACACS")           \
    _(SYS_MODULE_STKCTRL,               "Stack Control")    \
    _(SYS_MODULE_STKTPLG,               "Stack Topology")   \
    _(SYS_MODULE_AMTR,                  "AMTR")             \
    _(SYS_MODULE_NMTR,                  "NMTR")             \
    _(SYS_MODULE_PVLAN,                 "Private VLAN")     \
    _(SYS_MODULE_MFLT,                  "MFLT")             \
    _(SYS_MODULE_ACL,                   "ACL")              \
    _(SYS_MODULE_EH,                    "ExceptionHandler") \
    _(SYS_MODULE_SNTP,                  "SNTP")             \
    _(SYS_MODULE_UCMGMT,                "UC Management")    \
    _(SYS_MODULE_HRDRV,                 "HardwareResource") \
    _(SYS_MODULE_I2CDRV,                "I2C Drv")          \
    _(SYS_MODULE_ISCDRV,                "ISC Driver")       \
    _(SYS_MODULE_SYSDRV,                "System Driver")    \
    _(SYS_MODULE_AMTRDRV,               "AMTR Driver")      \
    _(SYS_MODULE_NMTRDRV,               "nmtrdrv")          \
    _(SYS_MODULE_CMNLIB,                "Common Lib")       \
    _(SYS_MODULE_TELNET,                "Telnet")           \
    _(SYS_MODULE_TRUNK,                 "Trunk Management") \
    _(SYS_MODULE_L2MUX,                 "L2 MUX")           \
    _(SYS_MODULE_AMTRL3,                "AMTR Layer 3")     \
    _(SYS_MODULE_IUC,                   "IUC")              \
    _(SYS_MODULE_MGMT_IP_FLT,           "Mgmt IP Filter")   \
    _(SYS_MODULE_SMTP,                  "SMTP")             \
    _(SYS_MODULE_KEYGEN,                "Key Generation")   \
    _(SYS_MODULE_HSRP,                  "HSRP")             \
    _(SYS_MODULE_VRRP,                  "VRRP")             \
    _(SYS_MODULE_SWDRVL3,               "L3 swdrv")         \
    _(SYS_MODULE_SWDRVL4,               "L4 swdrv")         \
    _(SYS_MODULE_NETACCESS,             "Network Access")   \
    _(SYS_MODULE_DISCP,                 "DISCP")            \
    _(SYS_MODULE_DNS,                   "DNS")              \
    _(SYS_MODULE_PORTAUTHSRVC,          "PortAuthSrv")      \
    _(SYS_MODULE_POEDRV,                "PoE Driver")       \
    _(SYS_MODULE_SWDRV_CACHE,           "SWDRV Cache")      \
    _(SYS_MODULE_CFGDB_UTIL,            "CFGDB Utility")    \
    _(SYS_MODULE_SWDRV,                 "Switch Driver")    \
    _(SYS_MODULE_LLDP,                  "LLDP")             \
    _(SYS_MODULE_UPNP,                  "uPNP")             \
    _(SYS_MODULE_CLUSTER,               "Clustering")       \
    _(SYS_MODULE_UDPHELPER,             "UDP helper")       \
    _(SYS_MODULE_DIFFSERV,              "DiffServ")         \
    _(SYS_MODULE_NSMCFG,                "NSM Config")       \
    _(SYS_MODULE_SYS_CALLBACK,          "System Callback")  \
    _(SYS_MODULE_BACKDOOR,              "Backdoor")         \
    _(SYS_MODULE_DEV_SWDRV,             "Switch Device")    \
    _(SYS_MODULE_DEV_AMTRDRV,           "AMTR Device")      \
    _(SYS_MODULE_DEV_SWDRVL4,           "SWDRVL4 Device")   \
    _(SYS_MODULE_L4,                    "L4")               \
    _(SYS_MODULE_NSM,                   "NSM")              \
    _(SYS_MODULE_IGMP,                  "IGMP")             \
    _(SYS_MODULE_DEV_NMTRDRV,           "NMTR Device")      \
    _(SYS_MODULE_DEV_RM,                "RM Device")        \
    _(SYS_MODULE_DEV_SWDRVL3,           "SWDRVL3 Device")   \
    _(SYS_MODULE_RULE_CTRL,             "Rule Control")     \
    _(SYS_MODULE_AAA,                   "AAA")              \
    _(SYS_MODULE_EFM_OAM,               "EFM OAM")          \
    _(SYS_MODULE_EFM_OAM_LOOPBACK,      "EFM OAM Loopback") \
    _(SYS_MODULE_PING,                  "Ping")             \
    _(SYS_MODULE_TRACEROUTE,            "Traceroute")       \
    _(SYS_MODULE_DHCPSNP,               "DHCP Snooping")    \
    _(SYS_MODULE_MSL,                   "MSL")              \
    _(SYS_MODULE_SYS_TIME,              "SYSTEM TIME")      \
    _(SYS_MODULE_ADD,                   "Add")              \
    _(SYS_MODULE_CFM,                   "CFM")              \
    _(SYS_MODULE_MVR,                   "MVR")              \
    _(SYS_MODULE_DAI,                   "DAI")              \
    _(SYS_MODULE_RSPAN,                 "RSPAN")            \
    _(SYS_MODULE_MLD,                   "MLD")              \
    _(SYS_MODULE_PIM6,                  "PIM6")             \
    _(SYS_MODULE_POE,                   "POE")              \
    _(SYS_MODULE_SFLOW,                 "SFLOW")            \
    _(SYS_MODULE_DOT1X_SUP,             "DOT1X_SUP")        \
    _(SYS_MODULE_ERPS,                  "ERPS")             \
    _(SYS_MODULE_PPPOE_IA,              "PPPOE_IA")         \
    _(SYS_MODULE_MVR6,                  "MVR6")             \
    _(SYS_MODULE_IPAL,                  "IPAL")             \
    _(SYS_MODULE_WDT,                   "Watchdog")         \
    _(SYS_MODULE_DOS,                   "DoS Protection")   \
    _(SYS_MODULE_LBD,                   "LBD")              \
    _(SYS_MODULE_DHCPV6SNP,             "DHCP6SNP")         \
    _(SYS_MODULE_TIME_RANGE,            "Time Range")       \
    _(SYS_MODULE_UNKNOWN,               "Unknown")

/* DATA TYPE DECLARATIONS
 */
/* Declare the enum of SYS_MODULE_ID for global usage.
 */
typedef enum SYS_MODULE_ID_E
{
    SYSMOD_LIST(MODULE_ID)
} SYS_MODULE_ID_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

#endif  /*      _SYS_MODULE     */

