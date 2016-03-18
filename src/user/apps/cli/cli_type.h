/*
 * Project Name: Mercury 
 *
 * Module Name : cli_type.h
 *
 * Purpose     : Provide CLI session type definition
 *
 * History :                                                               
 *          Date        Modifier        Reason
 *          2001/10/3   Chin-Ju Chu     Create this file
 *
 * Copyright(C)      Accton Corporation, 2001
 *
 * Note    : Inherit from Poppadom by Orlando
 */
#ifndef CLI_TYPE_H
#define CLI_TYPE_H

/* for trace_id of user_id when allocate buffer with l_mm
 */
enum
{
    CLI_TYPE_TRACE_ID_CLI_TASK_REPRODUCE_CONFIG = 0,
    CLI_TYPE_TRACE_ID_CLI_TASK_SETSTACKINGDB,
    CLI_TYPE_TRACE_ID_CLI_MGR_GETSTACKINGDB,
    CLI_TYPE_TRACE_ID_CLI_MGR_GETCONFIGUARTIONMODULETYPES,
    CLI_TYPE_TRACE_ID_CLI_API_SHOW_SYSTEM,
    CLI_TYPE_TRACE_ID_CLI_API_SHOW_HOSTS,
    CLI_TYPE_TRACE_ID_CLI_API_SHOW_PROCESS_CPU,
    CLI_TYPE_TRACE_ID_CLI_CRAR_IPUDPHELPERSUMMARY,
    CLI_TYPE_TRACE_ID_CLI_RUNCFG_GET_VLANDATABASE_PARAMETER,
    CLI_TYPE_TRACE_ID_CLI_CRSE_GETDUDBYVALUE,
    CLI_TYPE_TRACE_ID_CLI_CRDE_SSHHOSTKEY,
    CLI_TYPE_TRACE_ID_CLI_CRDE_TRUSTEDIPHOSTSUMMARY,
    CLI_TYPE_TRACE_ID_CLI_CRSY_DISPLAY,
    CLI_TYPE_TRACE_ID_CLI_CRGS_GETTINGSTARTED
};
 
typedef enum CLI_TYPE_SessionType_E
{
   CLI_TYPE_UART = 1,         		   /* cli connection type: uart */
   CLI_TYPE_TELNET,			   /* cli connection type: telnet */
/*92.03.27 pttch add for disconnect session*/
   CLI_TYPE_SSH,			   /* cli connection type: SSH */
   CLI_TYPE_PROVISION,                     /* cli connection type: provision */
/*pttch add for accton backdoor*/
#if (SYS_CPNT_3COM_CLI == TRUE)     	
   CLI_TYPE_BACKDOOR,
#endif
   CLI_TYPE_MODULE_PROVISION,

#if (SYS_CPNT_MSTP_SUPPORT_PVST==TRUE)
/*93.2.9 Eric add for vlan database type*/
   CLI_TYPE_BY_NULL,
   CLI_TYPE_BY_VLAN_PORT,
   CLI_TYPE_BY_VLAN_NAME,
#endif

#if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE)
    CLI_TYPE_PARTIAL_PROV,
#endif  /* #if (SYS_CPNT_CLI_ADD_TO_RUNNING_CONFIG == TRUE) */

} CLI_TYPE_SessionType_T;

#endif
