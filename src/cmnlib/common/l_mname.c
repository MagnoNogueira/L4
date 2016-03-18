/* Module Name: L_MNAME.C
 * Purpose:
 *      Supports API to get module name for all components.
 *
 * Notes:
 *      None.
 *
 * History:
 *      Date        --  Modifier,       Reason
 *      2003.01.02  --  Jason Hsue,     Create
 *      2003.02.07  --  Erica Li,       Modify
 *
 * Copyright(C)      Accton Corporation, 1999 - 2003
 */

#include <string.h>

#include "sys_type.h"
#include "sys_module.h"
#include "l_mname.h"


/* IMPORTANT: Note for the following strings:
 * The maximum length is SYS_MODULE_MAX_NAME_LEN characters.
 */
static char *sys_module_string[] =
{
    "Global System",
    "CLI",
    "LED Mgmt",
    "RMON",
    "RMON2",
    "SNMP",
    "Trap Mgmt",
    "Web",
    "BootP",
    "DHCP",
    "File Tranfer",
    "BSP",
    "Extend Brg",
    "Priority Mgmt",
    "Spanning Tree",
    "DataBase",
    "IGMP Snooping",
    "MLD Snooping",
    "MIB2 Mgmt",
    "Network",
    "Root",
    "Security",
    "Stack Mgmt",
    "Swicth Control",
    "System Log",
    "System Mgmt",
    "User Auth",
    "VLAN",
    "GVRP",
    "GARP",
    "L2 Multicast",
    "LACP",
    "Flash Driver",
    "IMC",
    "LED Driver",
    "LED Driver",
    "NIC Driver",
    "Serial Eprom",
    "Switch Driver",
    "File System",
    "COS",
    "CGI",
    "HTTP",
    "Web Authentication"
    "VDSL Driver",
    "X Spanning Tree",
    "RADIUS",
    "802.1X",
    "SSL",
    "SSH",
    "Protocol VLAN",
    "RIP",
    "OSPF",
    "DVMRP",
    "LAN",
    "IML",
    "Phase 2",
    "Device Driver",
    "IF",           /* Information Management */
    "Net Config",   /* Network Configuration  */
    "ARP",          /* ARP Configuration      */
    "ICMP",         /* ICMP Configuration     */
    "IGMP",         /* IGMP Configuration     */
    "IP Config",    /* IP Configuration       */
    "PIM",          /* PIM Configuration      */
    "Route Config", /* ROUTE Configuration    */
    "L3 Intf Info", /* L3 Interface Related Information */
    "Port Security",
    "TACACS",
    "Stack Control",
    "Stack Topology",
    "AMTR",
    "NMTR",
    "Private VLAN",
    "MFLT",        /* Multicast Filter Management */
    "ACL",
    "ExceptionHandler",
    "SNTP",
    "UC Management",
    "HardwareResource", /* Hardware Resource Driver */
    "I2C Drv",
    "ISC Driver",
    "System Driver",
    "AMTR Driver",
    "NMTR Driver",
    "Common Lib",
    "Telnet",
    "Trunk Management",
    "L2 MUX",
    "AMTR Layer 3",
    "IUC",
    "Mgmt IP Filter", /* Management IP Filter */
    "SMTP",
    "Key Generation",
    "HSRP",
    "VRRP",
    "L3 swdrv",
    "L4 swdrv",
    "Network Access",
    "DISCP",
    "DNS",
    "PortAuthSrv",
    "PoE Driver",
    "SWDRV Cache",
    "CFGDB Utility",
    "Switch Driver",
    "LLDP",
    "uPNP",
    "Clustering",
    "UDP helper",
    "DiffServ",         /* DiffServ or QoS */
    "NSM Config",
    "System Callback",
    "Backdoor"
    "Switch Device",
    "AMTR Device",
    "SWDRVL4 Device",
    "L4",
    "NSM",
    "NMTR Device",
    "RM Device",
    "SWDRVL3 Device",
    "Rule Control",
    "AAA",
    "EFM OAM",
    "EFM OAM Loopback",
    "Ping",
    "Traceroute",
    "DHCP Snooping",
};
/* IMPORTANT: Note for the above strings:
 * The maximum length is SYS_MODULE_MAX_NAME_LEN characters.
 */

/* FUNCTION NAME : L_MNAME_GetModuleName
 * PURPOSE:
 *      Get component module name by module id.
 *
 * INPUT:
 *      module_id   - defined in sys_module.h
 *
 * OUTPUT:
 *      module_name - a string whose length is less than or equal to
 *                    SYS_MODULE_MAX_NAME_LEN.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. Caller must prepare enough string buffer for copy string.
 */
void L_MNAME_GetModuleName(SYS_MODULE_ID_T module_id, UI8_T *module_name)
{
    if (module_id >= SYS_MODULE_UNKNOWN)
    {
        strcpy((char *)module_name, "Unknown");
    }
    else
    {
        strcpy((char *)module_name, sys_module_string[module_id]);
    }
}

