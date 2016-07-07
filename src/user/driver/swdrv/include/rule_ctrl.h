/* FUNCTION NAME: RULE_CTRL.H
 * PURPOSE:
 *   {1. What is covered in this file - function and scope.}
 *   {2. Related documents or hardware information}
 * NOTES:
 *     {Something must be known or noticed}
 *   {1. How to use these functions - Give an example.}
 *   {2. Sequence of messages if applicable.}
 *   {3. Any design limitation}
 *   {4. Any performance limitation}
 *   {5. Is it a reusable component}
 *
 * REASON:
 *    DESCRIPTION:
 *    CREATOR:   Your Name here
 *    Date
 *
 * Copyright(C)      Accton Corporation, 2002
 */

#ifndef _RULE_CTRL_H
#define _RULE_CTRL_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_cpnt.h"
#include "sys_type.h"
#include "rule_type.h"
#include "l4_backdoor.h"
#include "sysrsc_mgr.h"
#if (SYS_CPNT_ISCDRV == TRUE)
#include "l_mm.h"
#include "isc.h"
#endif

/* NAMING CONSTANT DECLARATIONS
 */
#define RULE_CTRL_ETHER_TYPE_EAP                0x888e
#define RULE_CTRL_ETHER_TYPE_CDP                0x81fd
#define RULE_CTRL_ETHER_TYPE_IPV4               0x0800
#define RULE_CTRL_ETHER_TYPE_IPV6               0x86dd
#define RULE_CTRL_ETHER_TYPE_NBX                0x8868
#define RULE_CTRL_ETHER_TYPE_ARP                0x0806
#define RULE_CTRL_ETHER_TYPE_CFM                0x8902
#define RULE_CTRL_ETHER_TYPE_CPE_OAM_PDU        0x8809
#define RULE_CTRL_ETHER_TYPE_CO_OAM_LOOPBACK    0x9000
#define RULE_CTRL_ETHER_TYPE_PTP                0x88f7

#define RULE_CTRL_ACTION_FLOOD                  0
#define RULE_CTRL_ACTION_TRAP_TO_CPU            1
#define RULE_CTRL_ACTION_TRAP_AND_FLOOD         2
#define RULE_CTRL_ACTION_DROP                   3

#define SLF_NO_ACTION                           0
#define SLF_DROP                                1
#define SLF_REDIRECT                            2

/* MACRO FUNCTION DECLARATIONS
 */
/* DATA TYPE DECLARATIONS
 */

typedef struct
{
    UI32_T  meter_model;            /* meter model */
    UI32_T  rate;                   /* average commit rate in kbps */
    UI32_T  burst_size;             /* maxi. # of commit bytes in a single Tx burst */
    UI32_T  peak_rate;              /* average peak rate in kbps */
    UI32_T  peak_burst_size;        /* maxi. # of peak bytes in a single Tx burst */
    UI32_T  interval;               /* microseconds, time interval used with the token bucket */
}RULE_CTRL_MeterEntry_T;

typedef struct
{
    UI32_T green_action;
    UI32_T yellow_action;
    UI32_T red_action;
}RULE_CTRL_ActionEntry_T;

typedef enum
{
    RULE_CTRL_WEBAUTH_REDIR_HTTP,
    RULE_CTRL_WEBAUTH_PERMIT_DHCPC,
    RULE_CTRL_WEBAUTH_PERMIT_DNS,
    RULE_CTRL_WEBAUTH_PERMIT_SIP,
    RULE_CTRL_WEBAUTH_DENY_IP,
    RULE_CTRL_IP_SOURCE_GUARD_PERMIT_VID_SIP,
    RULE_CTRL_IP_SOURCE_GUARD_PERMIT_SA_VID_SIP,
    RULE_CTRL_IP_SOURCE_GUARD_DENY_IP,
    RULE_CTRL_IPV6_SOURCE_GUARD_PERMIT_VID_SIP6,
    RULE_CTRL_IPV6_SOURCE_GUARD_DENY_IP6,
    RULE_CTRL_MAX_SERVICE_ID
}RULE_CTRL_ServiceType_T;

typedef struct
{
    UI32_T  unit;
    UI32_T  port;
}RULE_CTRL_UnitPort_T;

typedef struct
{
    UI32_T  unit;
    UI32_T  port;
    UI32_T  sip;
}RULE_CTRL_UnitPortSip_T;

typedef struct
{
    UI32_T  unit;
    UI32_T  port;
    UI32_T  vid;
    UI32_T  sip;
}RULE_CTRL_UnitPortVidSip_T;

typedef struct
{
    UI32_T  unit;
    UI32_T  port;
    UI32_T  vid;
    UI32_T  sip;
    UI8_T   sa[SYS_ADPT_MAC_ADDR_LEN];
}RULE_CTRL_UnitPortSaVidSip_T;

typedef struct
{
    UI32_T  unit;
    UI32_T  port;
    UI32_T  vid;
    UI8_T   sip6[SYS_TYPE_IPV6_ADDR_LEN];
}RULE_CTRL_UnitPortVidSip6_T;

#define RULE_CTRL_INVALID                   0xFFFFFFFF
#define RULE_CTRL_FILTER_MAX_NAME_STR_LEN   63

typedef struct
{
    UI32_T                      unit;
    UI32_T                      device_id;
    UI32_T                      group_id;
    UI32_T                      port;
    RULE_TYPE_FunctionType_T    func_type;
    char                        tag[RULE_CTRL_FILTER_MAX_NAME_STR_LEN + 1];
    UI32_T                      *rule_id_ar;
    UI32_T                      cch_rule_id_ar;

    /* filter by rule attributes
     */
    UI8_T                       rule_src_mac[SYS_ADPT_MAC_ADDR_LEN];
    UI32_T                      rule_outer_vlan;

    union
    {
        UI32_T                  w;
        UI8_T                   b[4];
    } rule_src_ip;

    union
    {
        UI32_T                  w[4];
        UI8_T                   b[16];
    } rule_dst_ip6;

} RULE_CTRL_Filter_T;

typedef struct
{
    // FIXME: merge this structure into options
    RULE_TYPE_CpuRuleInfo_T *cpu_rule_info_p;

} RULE_CTRL_OPTIONS_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/*------------------------------------------------------------------------------
 * FUNCTION NAME: RULE_CTRL_AttachSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for RULE_CTRL in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_AttachSystemResources(void);

/*------------------------------------------------------------------------------
 * FUNCTION NAME: RULE_CTRL_GetShMemInfo
 *------------------------------------------------------------------------------
 * PURPOSE: Get share memory info
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 *------------------------------------------------------------------------------
 * NOTES:
 */
void RULE_CTRL_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_InitiateSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE  : Initial resource
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_InitiateSystemResources();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_EnterMasterMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will make the rule ctrl enter the master mode.
 * INPUT    : None.
 * OUTPUT   : None.
 * RETURN   : None.
 * NOTE     : None.
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_EnterMasterMode();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_EnterSlaveMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will make the rule ctrl enter the slave mode.
 * INPUT    : None.
 * OUTPUT   : None.
 * RETURN   : None.
 * NOTE     : None.
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_EnterSlaveMode();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE  : Set transition mode
 * INPUT    : None.
 * OUTPUT   : None.
 * RETURN   : None.
 * NOTE     : None.
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_SetTransitionMode();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_EnterTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the rule ctrl enter the transition mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_EnterTransitionMode();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_ProcessTimerEvent
 *------------------------------------------------------------------------------
 * PURPOSE: Process timer alarm event from timer
 * INPUT:  None.
 * OUTPUT: None.
 * RETURN: TRUE/FALSE.
 * NOTES:  None.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_ProcessTimerEvent();

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*--------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_ProcessTimeRangeStatusChange
 *---------------------------------------------------------------------------
 * PURPOSE: Process time_range status change
 * INPUT:  isChanged_list, status_list.
 * OUTPUT: None.
 * RETURN: TRUE/FALSE.
 * NOTES:  None.
 *---------------------------------------------------------------------------
 */
BOOL_T 
RULE_CTRL_ProcessTimeRangeStatusChange(
    UI8_T *isChanged_list, UI8_T *status_list
);
#endif

void RULE_CTRL_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port);

void RULE_CTRL_HandleHotRemoval(
    UI32_T starting_port_ifindex,
    UI32_T number_of_port
);

void
RULE_CTRL_HotRemoval_FreeRuleGroup(
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_instance
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_FieldInit
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the rule ctrl clear the chip of the slave.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_FieldInit();

#pragma mark -
#pragma mark ACL / Diffserv Functions

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_AddClassMapConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : add class-map config on chip for every ports bound with specific
 *            policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, if there is nothing to config, return TRUE (do nothing)
 *            2, this func used when a policy-map already config to chip and
 *               dynamically add a class-map
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_AddClassMapConfig(
    UI32_T policy_map_index,
    UI32_T class_map_index
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_AddClassMapElementConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : add class-map element config on chip for every ports bound with
 *            specific class-map
 * INPUT    : class_map_index, class_type, class_index
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to config, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_AddClassMapElementConfig(
    UI32_T class_map_index,
    RULE_TYPE_ClassType_T class_type,
    UI32_T class_index
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_AddMeterConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : add meter config on chip for every ports bound with specific
 *            policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to config, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_AddMeterConfig(
    UI32_T policy_map_index,
    UI32_T class_map_index
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_UpdateMeterConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : update meter config on chip for every ports bound with specific
 *            policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to config, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_UpdateMeterConfig(
    UI32_T policy_map_index,
    UI32_T class_map_index
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_UpdateActionConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : update action config on chip for every ports bound with specific
 *            policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to config, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_UpdateActionConfig(
    UI32_T policy_map_index,
    UI32_T class_map_index
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_RemoveClassMapConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : remove class-map config on chip for every ports bound with
 *            specific policy-map and class-map
 * INPUT    : policy_map_index, class_map_index, free_cos_reference
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to free, return TRUE (do nothing)
 *            free_cos_reference = TRUE implies need to free CoS reference
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_RemoveClassMapConfig(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    BOOL_T free_cos_reference
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_RemoveClassMapElementConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : remove class-map element config on chip for every ports bound with
 *            specific class-map
 * INPUT    : class_map_index, class_type, class_index
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to free, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_RemoveClassMapElementConfig(
    UI32_T class_map_index,
    RULE_TYPE_ClassType_T class_type,
    UI32_T class_index
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_RemoveMeterConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : remove meter config on chip for every ports bound with specific
 *            policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : if there is nothing to free, return TRUE (do nothing)
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_RemoveMeterConfig(
    UI32_T policy_map_index,
    UI32_T class_map_index
);

/* rule */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetAce
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a ACE entry.
 * INPUT:    acl_id, rule_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetAce(
    UI32_T acl_id,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    BOOL_T is_add
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetQoSAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    unit, port, acl_id, meter_id, action_id, is_add
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetQoSAcl(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T acl_id,
    UI32_T meter_id,
    UI32_T action_id,
    BOOL_T is_add
);

#if (SYS_CPNT_PBR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPbrAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set ACL rules for PBR
 * INPUT:    is_add   - Add/Delete rule for PBR
 *           vlan     - The vlan for binding.
 *           acl_id   - The ACL for binding.
 *           action_p - The PBR action for this ACL.
 * OUTPUT:   None.
 * RETURN:   RULE_TYPE_OK / Error code (RULE_TYPE_RETURN_TYPE_T)
 * NOTE:     Now using global interface + filter vlan to apply the ACL on the vlan.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetPbrAcl(
    BOOL_T is_add,
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_id,
    const RULE_TYPE_PBR_ACTION_T *action_p
);
#endif /* SYS_CPNT_PBR == TRUE */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    is_ingress, unit, port, acl_id, is_trunk, is_add, time_range_index
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetAcl(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_id,
    BOOL_T is_add,
    UI32_T time_range_index,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetAclMirror
 *------------------------------------------------------------------------------
 * PURPOSE:  Apply ACL mirror
 * INPUT:    acl_id     - ACL ID
 *           ace_id     - ACE ID
 * OUTPUT:   ace_entry  - ACE object
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetAclMirror(
    UI32_T acl_index
);

#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetQosCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Get diffserv counter
 * INPUT:    ifindex - port index
 *           direction - ingress or egress
 *           policy_id - policy-map id
 *           class_id - class-map id
 * OUTPUT:   counter
 * RETURN:   Error code
 * NOTE: None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_GetQosCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id,
    RULE_TYPE_CounterRec_T *counter
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_ClearQosCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Clear diffserv counter
 * INPUT:    ifindex - port index
 *           direction - ingress or egress
 *           policy_id - policy-map id
 *           class_id - class-map id
 * OUTPUT:   None
 * RETURN:   Error code
 * NOTE: None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_ClearQosCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id
);
#endif

#if (SYS_CPNT_ACL_COUNTER == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetAclRuleCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Get counter for ACE of ACL
 * INPUT:    acl_id     - ACL ID
 *           ace_id     - ACE ID
 * OUTPUT:   ace_entry  - ACE object
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_GetAclRuleCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    UI32_T ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_ClearAclRuleCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Glear counter for ACE of ACL
 * INPUT:    interface  - The interface.
 *                        The type of this parameter should be
 *                        RULE_TYPE_INTERFACE_UPORT
 *                        The direction of this parameter should be one of the
 *                        values:
 *                        RULE_TYPE_INBOUND     - Ingress direction
 *                        RULE_TYPE_OUTBOUND    - Egress direction
 *           acl_id     - ACL ID
 *           ace_id     - ACE ID
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_ClearAclRuleCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    UI32_T ace_index
);
#endif /* SYS_CPNT_ACL_COUNTER */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetMf
 *------------------------------------------------------------------------------
 * PURPOSE  : Set a MF entry.
 * INPUT    : class_map_index, mf_index, is_add
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : is_add == FALSE implies remove MF
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetMf(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T mf_index,
    UI32_T meter_id,
    UI32_T action_id,
    BOOL_T is_add
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for a policy-map.
 * INPUT    : unit              - unit
 *            port              - port
 *            inout_profile     - direction
 *            policy_map_index  - policy-map index
 *            is_add            - TRUE, set rule to chip; FALSE, remove rule
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetPolicyMap(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_map_index,
    BOOL_T is_add
);

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DeleteBundle
 *------------------------------------------------------------------------------
 * PURPOSE  : Delete bundle group
 * INPUT    : policy_map_index      - policy-map index
 *            bundle_group_index    - bundle group index
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_DeleteBundle(
    UI32_T policy_map_index,
    UI32_T bundle_group_index
);
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_NotifyIpDscp2CosChangeByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : caller notify rule_ctrl which dscp to cos mapping had been changed
 * INPUT    : lport, ip_dscp
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : for policy-map CLI command "set ip dscp xxx"
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_NotifyIpDscp2CosChangeByLport(UI32_T lport, UI8_T ip_dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_NotifyIpPrecedence2CosChangeByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : caller notify rule_ctrl which ip precedence to cos mapping had
 *            been changed
 * INPUT    : lport, ip_precedence
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : for policy-map CLI command "set ip precedence xxx"
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_NotifyIpPrecedence2CosChangeByLport(UI32_T lport, UI8_T ip_precedence);

#pragma mark End of ACL / Diffserv Functions
#pragma mark -

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_TrapPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  trap specified packet to CPU
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_TrapPacket2Cpu(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_MirrorPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  This function is used to Copy frame to CPU.
 *           The packet is still forwarded by bridge mechenism.
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     Not implement
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_MirrorPacket2Cpu(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_TrapPacket2CpuForSwdrv
 *------------------------------------------------------------------------------
 * PURPOSE:  trap specified packet to CPU
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     1. dip is used as do_action for DHCPS_BC & DHCPC_BC (DHCPSNP)
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_TrapPacket2CpuForSwdrv(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetTcpUdpPortCosMap
 *------------------------------------------------------------------------------
 * PURPOSE:  set tcp/udp destination port mapped to cos by unit and port
 * INPUT:    unit, port, is_tcp, dest_port, phb, color
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetTcpUdpPortCosMap(UI32_T unit, UI32_T port, BOOL_T is_tcp, UI16_T dest_port, UI8_T phb, UI8_T color, BOOL_T is_update);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DeleteTcpPortCosMap
 *------------------------------------------------------------------------------
 * PURPOSE:  delete tcp/udp destination port mapped to cos by unit and port
 * INPUT:    unit, port, is_tcp, dest_port, phb, color
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteTcpUdpPortCosMap(UI32_T unit, UI32_T port, BOOL_T is_tcp, UI16_T dest_port, UI8_T phb, UI8_T color);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetL4PortCosMap
 *------------------------------------------------------------------------------
 * PURPOSE:  set tcp/udp destination port mapped to cos
 * INPUT:    unit, port, dst_port, cos
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     now this api is for global tcp port to cos not per port
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetL4PortCosMap(
    UI32_T  unit,
    UI32_T  port,
    UI16_T  dst_port,
    UI8_T   cos
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DelL4PortCosMap
 *------------------------------------------------------------------------------
 * PURPOSE:  set tcp/udp destination port mapped to cos by unit and port
 * INPUT:    unit, port, is_tcp, dst_port, phb, color
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     now this api is for global tcp port to cos not per port
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DelL4PortCosMap(
    UI32_T  unit,
    UI32_T  port,
    UI16_T  dst_port,
    UI8_T   cos
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IP6_TrapLinkLocalToCPU
 *------------------------------------------------------------------------------
 * PURPOSE:  Trap IPv6 link local address to CPU
 * INPUT:    enable          - TRUE:  trap to CPU
 *                             FALSE: no trap
 *           link_local_addr - IPv6 link local address
 *           vlan_id         - VLAN ID
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_IP6_TrapLinkLocalToCPU(
    BOOL_T enable,
    UI8_T link_local_addr[SYS_ADPT_IPV6_ADDR_LEN],
    UI32_T vlan_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetTrapLinkLocalRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Get rule of trap IPv6 link local address to CPU
 * INPUT:    rule_printf     - callback function. Use to dump rule
 *                             detail information. Pass NULL if does not want
 *                             to get information.
 *           link_local_addr - IPv6 link local address.
 * OUTPUT:   rule_id         - device rule ID array.
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_GetTrapLinkLocalRule(
    BOOL_T (*rule_printf)(char*, ...),
    UI8_T link_local_addr[SYS_ADPT_IPV6_ADDR_LEN],
    UI32_T vlan_id,
    UI32_T rule_id[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK][SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT]
);

#if (SYS_CPNT_IPV6_DENY_LINK_LOCAL_ROUTING_BY_RULE_AS_TEMPORARY_SOLUTION == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_IP6_DenyLinkLocalAddress
 *------------------------------------------------------------------------------
 * PURPOSE  : Deny IPv6 link local address and L3 routable enabled
 * INPUT    : enable    - TRUE: enable, FALSE: disable
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_IP6_DenyLinkLocalL3RoutablePkt(
    BOOL_T enable
);
#endif /* SYS_CPNT_IPV6_DENY_LINK_LOCAL_ROUTING_BY_RULE_AS_TEMPORARY_SOLUTION */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_WA_TrapHttpToCPU
 *------------------------------------------------------------------------------
 * PURPOSE:  Trap HTTP client packet to CPU for Web authentication
 * INPUT:    enable         - TRUE:  trap to CPU
 *                            FALSE: no trap
 *           unit           - unit
 *           port           - port
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_WA_TrapHttpToCPU(
    BOOL_T  enable,
    UI32_T  unit,
    UI32_T  port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_WA_PermitDhcp
 *------------------------------------------------------------------------------
 * PURPOSE:  Permit DHCP client packet for Web authentication
 * INPUT:    enable         - TRUE:  trap to CPU
 *                            FALSE: no trap
 *           unit           - unit
 *           port           - port
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_WA_PermitDhcp(
    BOOL_T  enable,
    UI32_T  unit,
    UI32_T  port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_WA_PermitDns
 *------------------------------------------------------------------------------
 * PURPOSE:  Permit DNS packet for Web authentication
 * INPUT:    enable         - TRUE:  trap to CPU
 *                            FALSE: no trap
 *           unit           - unit
 *           port           - port
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_WA_PermitDns(
    BOOL_T  enable,
    UI32_T  unit,
    UI32_T  port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_WA_PermitIpPacket
 *------------------------------------------------------------------------------
 * PURPOSE:  Permit IP packet for Web Authentication
 * INPUT:    enable          - TRUE:  permit
 *                             FALSE: no permit
 *           unit            - unit
 *           port            - port
 *           src_ipv4_addr   - source IPv4 address
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_WA_PermitIpPacket(
    BOOL_T  enable,
    UI32_T  unit,
    UI32_T  port,
    UI8_T   *src_ipv4_addr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_WA_GetPermitIpPacketRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Web Authentication. Get permit IP packet rule
 * INPUT:    unit            - unit
 *           port            - port
 *           rule_printf     - callback function. Use to dump rule
 *                             detail information. Pass NULL if does not want
 *                             to get information.
 *           src_ipv4_addr   - source IPv4 address
 * OUTPUT:   enable          - enable/disable
 *           rule_id         - device rule ID
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_WA_GetPermitIpPacketRule(
    UI32_T unit,
    UI32_T port,
    UI8_T *src_ipv4_addr,
    BOOL_T (*rule_printf)(char*, ...),
    BOOL_T *enable,
    UI32_T *rule_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_WA_DenyAnyIpPacket
 *------------------------------------------------------------------------------
 * PURPOSE:  Deny any IP packet for Web authentication
 * INPUT:    enable         - TRUE:  trap to CPU
 *                            FALSE: no trap
 *           unit           - unit
 *           port           - port
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_WA_DenyAnyIpPacket(
    BOOL_T  enable,
    UI32_T  unit,
    UI32_T  port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_1X_TrapDot1xUCToCPU
 *------------------------------------------------------------------------------
 * PURPOSE:  Trap 802.1x untcast to CPU
 * INPUT:    enable          - TRUE:  trap to CPU
 *                             FALSE: no trap
 *           unit            - unit
 *           port            - port
 *           cpu_mac         - CPU MAC address. For each port, the cpu_mac
 *                             should set the same.
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_1X_TrapDot1xUCToCPU(
    BOOL_T  enable,
    UI32_T  unit,
    UI32_T  port,
    UI8_T   cpu_mac[SYS_ADPT_MAC_ADDR_LEN]
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IPSG_PermitIpPacket
 *------------------------------------------------------------------------------
 * PURPOSE:  Permit IP packet for IPSG
 * INPUT:    enable          - TRUE:  permit
 *                             FALSE: no permit
 *           unit            - unit
 *           port            - port
 *           vlan_id         - VLAN ID
 *           src_ipv4_addr   - source IPv4 address
 *           src_mac_addr    - source MAC address
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_IPSG_PermitIpPacket(
    BOOL_T  enable,
    UI32_T  unit,
    UI32_T  port,
    UI32_T  vlan_id,
    UI8_T   *src_ipv4_addr,
    UI8_T   *src_mac_addr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IPSG_GetPermitIpPacketRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Get rule information of IPSG permit IP packet
 * INPUT:    unit            - unit
 *           port            - port
 *           rule_printf     - callback function. Use to dump rule
 *                             detail information. Pass NULL if does not want
 *                             to dump information.
 *           vlan_id         - VLAN ID.
 *           src_ipv4_addr   - source IPv4 address
 *           src_mac_addr    - source MAC address
 * OUTPUT:   rule_id_p       - device rule ID
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_IPSG_GetPermitIpPacketRule(
    UI32_T unit,
    UI32_T port,
    BOOL_T (*rule_printf)(char*, ...),
    UI32_T vlan_id,
    UI8_T  *src_ipv4_addr,
    UI8_T   *src_mac_addr,
    UI32_T *rule_id_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IPSG_DenyAnyIpPacket
 *------------------------------------------------------------------------------
 * PURPOSE:  Deny any IP address for IPSG
 * INPUT:    enable          - TRUE:  deny
 *                             FALSE: no deny
 *           unit            - unit
 *           port            - port
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_IPSG_DenyAnyIpPacket(
    BOOL_T  enable,
    UI32_T  unit,
    UI32_T  port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IPV6SG_PermitIp6LinkLocal
 *------------------------------------------------------------------------------
 * PURPOSE:  Permit all IPv6 link local packets
 * INPUT:    is_enable - TRUE:  permit
 *                       FALSE: no action
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK / RULE_TYPE_FAIL
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IPV6SG_PermitIp6LinkLocal(
    BOOL_T is_enable
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetRapsPduToCpuByPort
 *------------------------------------------------------------------------------
 * PURPOSE : This function will enable R-APS PDU redirect to CPU rule
 *           for specified unit and port
 * INPUT   : unit      -- which unit.
 *           port      -- which port.
 *           eth_type  -- which EtherType to set  (no used)
 *           dmac_p    -- which destination MAC to set  (no used)
 *           is_enable -- enable/disable
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetRapsPduToCpuByPort(
    UI32_T  unit,
    UI32_T  port,
    UI16_T  eth_type,
    UI8_T   *dst_mac_p,
    BOOL_T  is_enable
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_ERPS_TrapRapsPduToCPU
 *------------------------------------------------------------------------------
 * PURPOSE : This function will enable R-APS PDU redirect to CPU rule
 *           for specified unit and port
 * INPUT   : is_enable    -- enable/disable
 *           unit         -- which unit.
 *           port         -- which port.
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_ERPS_TrapRapsPduToCPU(
    BOOL_T  enable,
    UI32_T  unit,
    UI32_T  port
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetOamPduToCpu
 *------------------------------------------------------------------------------
 * PURPOSE : For CPE, redirect EFM OAM PDU to CPU rule.
 * INPUT   : unit       -- which unit
 *           port       -- which port
 *           is_enable  -- enable/disable
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetOamPduToCpu(
    UI32_T unit,
    UI32_T port,
    BOOL_T is_enable
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_OAM_CPE_TrapOamPduToCPU
 *------------------------------------------------------------------------------
 * PURPOSE : For CPE, redirect EFM OAM PDU to CPU rule.
 * INPUT   : enable  -- enable/disable
 *           unit    -- which unit.
 *           port    -- which port.
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_OAM_CPE_TrapOamPduToCPU(
    BOOL_T  enable,
    UI32_T  unit,
    UI32_T  port
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetOamLoopbackToRecPort
 *------------------------------------------------------------------------------
 * PURPOSE : For CPE, redirect EFM OAM loopback packet to receiving port.
 * INPUT   : unit       -- which unit
 *           port       -- which port
 *           is_enable  -- enable/disable
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetOamLoopbackToRecPort(
    UI32_T unit,
    UI32_T port,
    BOOL_T is_enable
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_OAM_CPE_RedirOamLoopbackToRecvPort
 *------------------------------------------------------------------------------
 * PURPOSE : For CPE, redirect EFM OAM loopback packet to receiving port.
 * INPUT   : enable  -- enable/disable
 *           unit    -- which unit.
 *           port    -- which port.
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_OAM_CPE_RedirOamLoopbackToRecvPort(
    BOOL_T  enable,
    UI32_T  unit,
    UI32_T  port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_OAM_CO_TrapOamLoopbackToCPU
 *------------------------------------------------------------------------------
 * FUNCTION : For CO side, trap EFM OAM loopback packet to CPU.
 * INPUT   : enable  -- enable/disable
 *           unit    -- which unit.
 *           port    -- which port.
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_OAM_CO_TrapOamLoopbackToCPU(
    BOOL_T enable,
    UI32_T unit,
    UI32_T port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IP6_RA_Guard_Deny_RA_Packet
 *------------------------------------------------------------------------------
 * PURPOSE:  Deny IPv6 Router-Advertisement packet for RA Guard
 * INPUT:    enable          - TRUE:  deny
 *                             FALSE: no deny
 *           interface_p     - pointer to interface info
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_IP6_RA_Guard_DenyRAPacket(
    BOOL_T                      enable,
    RULE_TYPE_INTERFACE_INFO_T  *interface_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IP6_RA_Guard_Deny_RR_Packet
 *------------------------------------------------------------------------------
 * PURPOSE:  Deny IPv6 Router-Redirect packet for RA Guard
 * INPUT:    enable          - TRUE:  deny
 *                             FALSE: no deny
 *           interface_p     - pointer to interface info
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_IP6_RA_Guard_DenyRRPacket(
    BOOL_T                      enable,
    RULE_TYPE_INTERFACE_INFO_T  *interface_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_AF_DenyCdpPacket
 *------------------------------------------------------------------------------
 * PURPOSE : This function enables/disables drop CDP packeage
 * INPUT   : enable - TRUE:  Enable
 *                    FALSE: Disable
 *           unit   - unit
 *           port   - port
 * OUTPUT  : None
 * RETURN  : RULE_TYPE_RETURN_TYPE_T
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_AF_DenyCdpPacket(
    BOOL_T enable,
    UI32_T unit,
    UI32_T port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_AF_DenyPvstPacket
 *------------------------------------------------------------------------------
 * PURPOSE : This function enables/disables drop CDP packeage
 * INPUT   : enable - TRUE:  Enable
 *                    FALSE: Disable
 *           unit   - unit
 *           port   - port
 * OUTPUT  : None
 * RETURN  : RULE_TYPE_RETURN_TYPE_T
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_AF_DenyPvstPacket(
    BOOL_T enable,
    UI32_T unit,
    UI32_T port
);

void RULE_CTRL_InitFilter(
    RULE_CTRL_Filter_T *filter_p
);

void RULE_CTRL_DumpRule(
    RULE_CTRL_Filter_T *filter_p,
    BOOL_T (*rule_printf)(char*, ...)
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetRateLimit
 *------------------------------------------------------------------------------
 * PURPOSE  : set up rate limit on specified (unit, port)
 * INPUT    : unit, port, k_bps, trunk_id (0 means the port didn't join any trunk)
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : XGSIII: the Src_Port_TGID of fp_tcam rule
 *                    is used for port or trunk id.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetRateLimit(UI32_T unit, UI32_T port, UI32_T trunk_id, UI32_T k_bps);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DeleteRateLimit
 *------------------------------------------------------------------------------
 * PURPOSE  : cancel rate limit on specified (unit, port)
 * INPUT    : unit, port
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteRateLimit(UI32_T unit, UI32_T port, UI32_T trunk_id);

// FIXME: Remove this
BOOL_T RULE_CTRL_Service(BOOL_T enable, RULE_CTRL_ServiceType_T type, void *param_p);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_GetTCAMUtilization
 *------------------------------------------------------------------------------
 * PURPOSE  : get tcam utilization
 * INPUT    : none
 * OUTPUT   : tcam_util_p
 * RETURN   : TRUE/FALSE.
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_GetTCAMUtilization(RULE_TYPE_TCAMUtilization_T *tcam_util_p);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_GetTcamEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get tcam entry
 * INPUT    : tcam_entry_p->unit
 *            tcam_entry_p->device_id
 *            tcam_entry_p->pool_id
 * OUTPUT   : tcam_util_p
 * RETURN   : OK/FAIL
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_GetTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_GetNextTcamEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get next tcam entry
 * INPUT    : tcam_entry_p->unit
 *            tcam_entry_p->device_id
 *            tcam_entry_p->pool_id
 * OUTPUT   : tcam_util_p
 * RETURN   : OK/FAIL
 * NOTE     : For get first entry,
 *            use index with tcam_entry_p->unit = 0
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_GetNextTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetSupportedTcamCapabilityBitMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets TCAM all supporting capability
 * INPUT    :
 * OUTPUT   : bit_map_p
 * RETURN   : Error code.
 * NOTE     :
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_GetSupportedTcamCapabilityBitMap(
    RULE_TYPE_TCAM_CAP_BITMAP_T *bit_map_p
);

#if (L4_SUPPORT_ACCTON_BACKDOOR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DumpFunctionInfo
 *------------------------------------------------------------------------------
 * PURPOSE  : dump all functions' information
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : for debug
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_DumpFunctionInfo(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DumpResourceInfo
 *------------------------------------------------------------------------------
 * PURPOSE  : dump specified group information
 * INPUT    : unit, device_id, group_id
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : for debug
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DumpResourceInfo(UI32_T unit, UI32_T device_id, UI32_T group_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DumpCpuInterfaceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : to see the CPU interface via FP status
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : for debug
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_DumpCpuInterfaceStatus();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Backdoor_DiffServ_DumpClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : show class instance
 * INPUT    : class_instance
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : for debug
 *------------------------------------------------------------------------------
 */
void
RULE_CTRL_Backdoor_DiffServ_DumpClassInstance(
    const RULE_TYPE_CLASS_INSTANCE_PTR_T class_instance
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Backdoor_DiffServ_DumpRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : show rule instance
 * INPUT    : show_title, rule_instance
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : for debug
 *------------------------------------------------------------------------------
 */
void
RULE_CTRL_Backdoor_DiffServ_DumpRuleInstance(
    BOOL_T show_title,
    const RULE_TYPE_RULE_INSTANCE_PTR_T rule_instance
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Backdoor_DiffServ_ShowClassType
 *------------------------------------------------------------------------------
 * PURPOSE  : show class type
 * INPUT    : class_type
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : for debug
 *------------------------------------------------------------------------------
 */
void
RULE_CTRL_Backdoor_DiffServ_ShowClassType(
    RULE_TYPE_INSTANCE_TYPE_T inst_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DumpSelectorInfo
 *------------------------------------------------------------------------------
 * PURPOSE  : dump specified selector information
 * INPUT    : unit, device_id, selector_id
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE.
 * NOTE     : for debug
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DumpSelectorInfo(UI32_T unit, UI32_T device_id, UI32_T selector_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetPhysicalRuleId
 *------------------------------------------------------------------------------
 * PURPOSE  : get physical rule id by device id and logical rule id
 * INPUT    : device_id, logical_id
 * OUTPUT   : physical_id
 * RETURN   : TRUE/FALSE.
 * NOTE     : for debug
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_GetPhysicalRuleId(UI32_T device_id, UI32_T logical_id, UI32_T *physical_id);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DumpGlobalAce_BackDoor
 *------------------------------------------------------------------------------
 * PURPOSE : Dump the database of global ace.
 *            (AF/DHCP Snooping/ ,etc...)
 *   INPUT : None
 *  OUTPUT : None
 *  RETURN : None
 *    NOTE : for backdoor only.
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_DumpGlobalAce_BackDoor(void);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DumpDfltAceByPort_BackDoor
 *------------------------------------------------------------------------------
 * PURPOSE : Dump the database of special usage for backdoor.
 *            (AF/DHCP Snooping/ ,etc...)
 *   INPUT : unit - unit
 *           port - port
 *  OUTPUT : None
 *  RETURN : None
 *    NOTE : for backdoor only.
 *------------------------------------------------------------------------------
 */
void RULE_CTRL_DumpDfltAceByPort_BackDoor(UI32_T unit, UI32_T port);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_BindPcl2Port_BackDoor
 *------------------------------------------------------------------------------
 * PURPOSE : bind a pcl to slot0/slot1 of port for backdoor testing.
 *
 *   INPUT :   unit - unit
 *             port - port
 *           pcl_id - pcl_id
 *         is_trunk - bind to trunk
 *         is_slot1 - bind to slot0 / slot1
 *        is_enable - bind / unbind
 *
 *  OUTPUT : None
 *  RETURN : TRUE - SUCCESS / FALSE - FAIL
 *    NOTE : for l4 backdoor only.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_BindPcl2Port_BackDoor(
    UI32_T unit, UI32_T port, UI32_T pcl_id,
    BOOL_T is_trunk, BOOL_T is_slot1, BOOL_T is_enable);
#endif /* L4_SUPPORT_ACCTON_BACKDOOR */

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetMacVlanEntry
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for MAC based VLAN
 * INPUT   : mac_address        -- MAC address
 *           vid                -- VLAN id
 *           priority           -- priority
 *  OUTPUT : None
 *  RETURN : TRUE - OK / FALSE - FAILED
 *    NOTE : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetMacVlanEntry(UI8_T *mac_address, UI8_T *mask, UI16_T vid, UI8_T priority);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DeleteMacVlanEntry
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for MAC based VLAN
 * INPUT   : mac_address        -- MAC address
 *  OUTPUT : None
 *  RETURN : TRUE - OK / FALSE - FAILED
 *    NOTE : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteMacVlanEntry(UI8_T *mac_address, UI8_T *mask);

#if (SYS_CPNT_PROTOCOL_VLAN_IMPLEMENTED_BY_RULE == TRUE)
/*-------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetProtocolVlanEntry
 * ------------------------------------------------------------------------
 * PURPOSE  :   This funtion returns TRUE if the specified protocol port
 *              entry can be successfully set. Otherwise, FALSE is returned.
 * INPUT    :   enable          --  TRUE: Add rule, FALSE: Delete rule
 *              interface_p     --  interface info.
 *              group_index     --  group index
 *                              --  the range of the value is [1..maxnum_of_group] of UI32_T,
 *              vlan_id         --  vlan identifier
 *                              --  the range of the value is [1..4094] of UI32_T
 *              nbr_of_type_protocol
 *                              --  number of the type-protocol entry indicated
 *                                  by the key {lport,group_index}
 *              frame_type      --  frame type
 *                              --  value of the array with the combination key {lport,group_index}
 *                              --  the range of the value stored in the array is [1..5] of UI32_T,
 *                                  following the definition of MIB value:
 *                                  VAL_dot1vProtocolTemplateFrameType_ethernet     (1),
 *                                  VAL_dot1vProtocolTemplateFrameType_rfc1042      (2),
 *                                  VAL_dot1vProtocolTemplateFrameType_snap8021H    (3),
 *                                  VAL_dot1vProtocolTemplateFrameType_snapOther    (4),
 *                                  VAL_dot1vProtocolTemplateFrameType_llcOther     (5)
 *              protocol_value  --  buffer of the array containing the protocol value
 *                              --  buffer of the array with the combination key {lport,group_index}
 *                              --  size of the buffer is:
 *                                  2 bytes, for frame_type = {1,2,3,5}, or
 *                                  5 bytes, for frame_type = {4}
 *              priority       --   vlan user priority [0~7], ignore when larger than 7
 * OUTPUT   :   None
 * RETURN   :   Success - RULE_TYPE_OK, Fail - error code.
 * NOTES    :   The following constant used in this function is defined in sys_adpt.h
 *              #define SYS_ADPT_1V_MAX_NBR_OF_FRAME_TYPE        3
 *                  -- The number of frame supported in 802.1v is 5.
 *                  -- This constant has to take the chip limitation into consideration.
 *              #define SYS_ADPT_1V_MAX_NBR_OF_PROTOCOL_GROUP    7
 *                  -- This constant has to take the chip limitation into consideration.
  * ------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetProtocolVlanEntry(
    BOOL_T enable,
    RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI32_T group_index,
    UI32_T vlan_id,
    UI32_T nbr_of_type_protocol,
    UI8_T  *frame_type,
    UI8_T  (*protocol_value)[SYS_ADPT_1V_MAX_PROTOCOL_VALUE_BUFFER_LENGTH],
    UI8_T  priority
);
#endif /* SYS_CPNT_PROTOCOL_VLAN_IMPLEMENTED_BY_RULE */

#if (SYS_CPNT_ADD_MODIFY_PRIORITY_BY_RULE == TRUE)
/*-------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetVoiceVlanPriority
 * ------------------------------------------------------------------------
 * PURPOSE  :   This funtion is using to remark the 802.1p priority for voice
 *              vlan.
 * INPUT    :   enable      -- TRUE: Add rule, FALSE: Delete rule
 *              interface_p -- Interface info.
 *              vid         -- VLAN ID
 *              src_addr    -- Source mac address
 *              src_mask    -- Source mac address mask
 *              priority    -- 802.1p priority
 * OUTPUT   :   None
 * RETURN   :   Success - RULE_TYPE_OK, Fail - error code.
 *
 * NOTES    :   priority also shall be input while deleting this rule.
 * ------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_SetVoiceVlanPriority(
    BOOL_T enable,
    RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI32_T vid,
    UI8_T *src_addr,
    UI8_T *src_mask,
    UI8_T  priority
);
#endif /* SYS_CPNT_ADD_MODIFY_PRIORITY_BY_RULE */

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetMacMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for MAC based MIRROR
 * INPUT   : mac_address        -- MAC address
 * OUTPUT  : None
 * RETURN  : TRUE - OK / FALSE - FAILED
 * NOTE    : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetMacMirrorEntry(UI8_T *mac_address);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DeleteMacMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for MAC based MIRROR
 * INPUT   : mac_address        -- MAC address
 * OUTPUT  : None
 * RETURN  : TRUE - OK / FALSE - FAILED
 * NOTE    : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteMacMirrorEntry(UI8_T *mac_address);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetIpSubnetVlanEntry
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for IP subnet based VLAN
 * INPUT   : ip_address         -- IP address
 *           mask               -- IP address subnet
 *           vid                -- VLAN id
 *           priority           -- priority
 *  OUTPUT : None
 *  RETURN : TRUE - OK / FALSE - FAILED
 *    NOTE : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetIpSubnetVlanEntry(UI32_T ip_address, UI32_T mask, UI16_T vid, UI8_T priority);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DeleteIpSubnetVlanEntry
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for IP subnet based VLAN
 * INPUT   : ip_address         -- IP address
 *           mask               -- IP address subnet
 *  OUTPUT : None
 *  RETURN : TRUE - OK / FALSE - FAILED
 *    NOTE : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteIpSubnetVlanEntry(UI32_T ip_address, UI32_T mask);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_SetIpSubnetVlanEntryPerPort
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for IP subnet based VLAN
 * INPUT   : unit               -- unit
 *           port               -- port
 *           ip_address         -- IP address
 *           mask               -- IP address subnet
 *           vid                -- VLAN id
 *           priority           -- priority
 *  OUTPUT : None
 *  RETURN : TRUE - OK / FALSE - FAILED
 *    NOTE : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetIpSubnetVlanEntryPerPort(
    UI32_T unit,
    UI32_T port,
    UI32_T ip_address,
    UI32_T mask,
    UI16_T vid,
    UI8_T priority
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DeleteIpSubnetVlanEntryPerPort
 *------------------------------------------------------------------------------
 * PURPOSE : Add PCEs for IP subnet based VLAN
 * INPUT   : unit               -- unit
 *           port               -- port
 *           ip_address         -- IP address
 *           mask               -- IP address subnet
 *  OUTPUT : None
 *  RETURN : TRUE - OK / FALSE - FAILED
 *    NOTE : PCE will be added /updated if PCE does not exist /exists.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteIpSubnetVlanEntryPerPort(
    UI32_T unit,
    UI32_T port,
    UI32_T ip_address,
    UI32_T mask
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetMvrRecVlanEntry
 *------------------------------------------------------------------------------
 * PURPOSE : This function sets MVR Receiver VLAN Entry
 * INPUT   : src_vid            -- Soruce MVR VLAN
 *           mip                -- multicast IP address
 *           dst_vid            -- Destination MVR VLAN
 * OUTPUT  : none
 * RETURN  : TRUE               -- Success
 *           FALSE              -- Failed
 * NOTE    : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetMvrRecVlanEntry(UI16_T src_vid, UI32_T mip, UI16_T dst_vid);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DeleteMvrRecVlanEntry
 *------------------------------------------------------------------------------
 * PURPOSE : This function deletes MVR Receiver VLAN Entry
 * INPUT   : src_vid            -- Soruce MVR VLAN
 *           mip                -- multicast IP address
 * OUTPUT  : none
 * RETURN  : TRUE               -- Success
 *           FALSE              -- Failed
 * NOTE    : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_DeleteMvrRecVlanEntry(UI16_T src_vid, UI32_T mip);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetIgmpEntry
 *------------------------------------------------------------------------------
 * PURPOSE : This function sets IGMP Entry
 * INPUT   : is_add             -- TRUE, set the IGMP rule; FALSE, remove the
 *                                 rule.
 * OUTPUT  : none
 * RETURN  : TRUE               -- Success
 *           FALSE              -- Failed
 * NOTE    : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetIgmpEntry(BOOL_T is_add);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetMulticastEntry
 *------------------------------------------------------------------------------
 * PURPOSE : This function sets internet multicast Entry
 * INPUT   : unit, port         -- unit/port number
 *           src_vid            -- set the multicast rule on the specified
 *                                 VLAN id
 *           is_add             -- TRUE, set the multicast rule; FALSE, remove
 *                                 the rule
 * OUTPUT  : none
 * RETURN  : TRUE               -- Success
 *           FALSE              -- Failed
 * NOTE    : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetMulticastEntry(UI32_T unit, UI32_T port, UI32_T src_vid, BOOL_T is_add);

#if (SYS_CPNT_BPDU_LOOPBACK_RATELIMIT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetBpduRateLimitEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get rate limit on specified (unit, port/trunk)
 * INPUT    : unit  - unit number
 *            port  - port number
 *            trunk - trunk id (0 means not trunk)
 * OUTPUT   : rate_limit - TRUE: limit rate
 *                        FALSE: not limit rate
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_GetBpduRateLimitEntry(UI32_T unit, UI32_T port, UI32_T trunk, BOOL_T *rate_limit);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetBpduRateLimitEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : set up rate limit on specified (unit, port/trunk)
 * INPUT    : unit  - unit number
 *            port  - port number
 *            trunk - trunk id (0 means not trunk)
 *            rate_limit - TRUE: limit rate
 *                        FALSE: not limit rate
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetBpduRateLimitEntry(UI32_T unit, UI32_T port, UI32_T trunk, BOOL_T rate_limit);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetGlobalBpduRateLimit
 *------------------------------------------------------------------------------
 * PURPOSE  : set up global rate limit
 * INPUT    : rate_limit - TRUE: limit rate
 *                        FALSE: not limit rate
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetGlobalBpdu();
#endif

#if (SYS_CPNT_QINQ == TRUE)
#if (SYS_CPNT_SWCTRL_QINQ_SERVICE == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Service_QinQ
 *------------------------------------------------------------------------------
 * PURPOSE:  for SERVICE QINQ
 *            config chip to add/delete an outter tag to packets with user setting.
 * INPUT:    unit, port - unit/port number
 *           cvid       - inner vid
 *           cpri       - inner pri
 *           svid       - outer vid
 *           spri       - outer pri
 *           flag       -add or delete the rule of S_QINQ
 * OUTPUT:   NONE
 * RETURN:   TRUE/FALSE
 * NOTE:     TRUE means success; FALSE means faile
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_Service_QinQ(UI32_T unit, UI32_T port, UI32_T cvid , UI32_T cpri, UI32_T svid , UI32_T spri, BOOL_T is_enable);
#endif /* SYS_CPNT_SWCTRL_QINQ_SERVICE */

#if (SYS_CPNT_QINQ_L2PT == TRUE)
#if (SYS_CPNT_QINQ_L2PT_DA_REMARK == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetL2PTTunnelPduToCpu
 *------------------------------------------------------------------------------
 * PURPOSE : This function enables/disables trap L2PT tunnel PDU.
 * INPUT   : tunnel_da
 *           is_enable
 * OUTPUT  : None
 * RETURN  : TRUE   -- Success
 *           FALSE  -- If a given port is not available
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetL2PTTunnelPduToCpu(UI8_T *tunnel_da, BOOL_T is_enable);
#endif /* SYS_CPNT_QINQ_L2PT_DA_REMARK */

#if (SYS_CPNT_QINQ_L2PT_CUSTOM_PDU == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetCustomPduTrapStatusForL2PT
 *------------------------------------------------------------------------------
 * PURPOSE : This function enables/disables trap custom PDU for L2PT.
 * INPUT   : unit
 *           port
 *           id
 *           dst_mac
 *           ethertype
 *           pdu_len
 *           pdu_data
 *           pdu_mask
 *           is_enable
 * OUTPUT  : None
 * RETURN  : TRUE   -- Success
 *           FALSE  -- Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetCustomPduTrapStatusForL2PT(UI32_T unit, UI32_T port, UI32_T id, UI8_T *dst_mac, UI16_T ethertype, UI32_T pdu_len, UI8_T *pdu_data, UI8_T *pdu_mask, BOOL_T is_enable);
#endif /* SYS_CPNT_QINQ_L2PT_CUSTOM_PDU */
#endif /* SYS_CPNT_QINQ_L2PT */
#endif /* SYS_CPNT_QINQ */

#if (SYS_CPNT_PPPOE_IA == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPPPoEDPktToCpu
 *------------------------------------------------------------------------------
 * PURPOSE : To enable/disable trap PPPoE discover packet to CPU
 *            for specified unit/port.
 * INPUT   : unit       - unit
 *           port       - port
 *           is_enable  - TRUE to enable
 * OUTPUT  : none
 * RETURN  : TRUE       - Success
 *           FALSE      - Failed
 * NOTE    : 1. there is a problem to install default rule on trunk now,
 *              so change to use global rule temporarily.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetPPPoEDPktToCpu(
    UI32_T  unit,
    UI32_T  port,
    BOOL_T  is_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPPPoEDPktToCpuPerSystem
 *------------------------------------------------------------------------------
 * PURPOSE : To enable/disable trap PPPoE discover packet to CPU
 *            for entire system.
 * INPUT   : is_enable  - TRUE to enable
 * OUTPUT  : none
 * RETURN  : TRUE       - Success
 *           FALSE      - Failed
 * NOTE    : 1. there is a problem to install default rule on trunk now,
 *              so change to use global rule temporarily.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetPPPoEDPktToCpuPerSystem(
    BOOL_T  is_enable
);
#endif /* SYS_CPNT_PPPOE_IA */

#if (SYS_CPNT_EAPS == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetEapsPktToCpu
 *------------------------------------------------------------------------------
 * PURPOSE : To enable/disable to trap EAPS packet to CPU
 *            for specified unit/port.
 * INPUT   : unit       - unit
 *           port       - port
 *           mac_p
 *           vid
 *           eaps_mode
 *           is_enable  - TRUE to enable
 * OUTPUT  : none
 * RETURN  : TRUE       - Success
 *           FALSE      - Failed
 * NOTE    : 1. for master,  redirect to CPU
 *           2. for transit, copy to CPU
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetEapsPktToCpu(
    UI32_T  unit,
    UI32_T  port,
    UI8_T   *mac_p,
    UI16_T  vid,
    UI16_T  eaps_mode,
    BOOL_T  is_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetEapsCtrlVlanPktDrop
 *------------------------------------------------------------------------------
 * PURPOSE : To enable/disable to drop EAPS control vlan packet
 *            for specified unit/port.
 * INPUT   : unit       - unit
 *           port       - port
 *           vid
 *           eaps_mode
 *           is_enable  - TRUE to enable
 * OUTPUT  : none
 * RETURN  : TRUE       - Success
 *           FALSE      - Failed
 * NOTE    : 1. for master,  write rule to block control vlan traffic.
 *           2. for transit, write no rule and only return TRUE.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetEapsCtrlVlanPktDrop(
    UI32_T  unit,
    UI32_T  port,
    UI16_T  vid,
    UI16_T  eaps_mode,
    BOOL_T  is_enable
);
#endif /* #if (SYS_CPNT_EAPS == TRUE) */

#if (SYS_CPNT_ISCDRV == TRUE)
/*------------------------------------------------------------------------------
 * Function : RULE_CTRL_ISC_Handler
 *------------------------------------------------------------------------------
 * Purpose  : This function will manipulte all of RULE_CTRL via ISC
 * INPUT    : *key      -- key of ISC
 *            *mref_handle_p  -- transfer data
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : called by ISC Agent
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_ISC_Handler(ISC_Key_T *key, L_MM_Mref_Handle_T *mref_handle_p);
#endif /* SYS_CPNT_ISCDRV */

#if (SYS_CPNT_IP_MULTICAST_DATA_DROP == TRUE) || (SYS_CPNT_MVR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IGMPSNP_DenyMulticastData
 *------------------------------------------------------------------------------
 * PURPOSE : This function sets internet multicast Entry
 * INPUT   : enable - TRUE to add rule;
 *                    FALSE to remove rule.
 *           unit   - unit
 *           port   - port
 * OUTPUT  : None
 * RETURN  : TRUE   - Success
 *           FALSE  - Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_IGMPSNP_DenyMulticastData(
    BOOL_T enable,
    UI32_T unit,
    UI32_T port
);
#endif /* SYS_CPNT_IP_MULTICAST_DATA_DROP || SYS_CPNT_MVR */

#if (SYS_CPNT_IPV6_MULTICAST_DATA_DROP == TRUE) || (SYS_CPNT_MVR6 == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IGMPSNP_DenyMulticastData
 *------------------------------------------------------------------------------
 * PURPOSE : This function sets internet multicast Entry
 * INPUT   : enable - TRUE to add rule;
 *                    FALSE to remove rule.
 *           unit   - unit
 *           port   - port
 * OUTPUT  : None
 * RETURN  : TRUE   - Success
 *           FALSE  - Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_IPV6_DenyMulticastData(
    BOOL_T enable,
    UI32_T unit,
    UI32_T port
);
#endif /* SYS_CPNT_IPV6_MULTICAST_DATA_DROP || SYS_CPNT_MVR6 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetCdpPktToCpu
 *------------------------------------------------------------------------------
 * PURPOSE : This function enables/disables trap CDP PDU.
 * INPUT   : is_enable
 * OUTPUT  : None
 * RETURN  : TRUE   -- Success
 *           FALSE  -- If a given port is not available
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetCdpPktToCpu(BOOL_T is_enable);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPvstPktToCpu
 *------------------------------------------------------------------------------
 * PURPOSE : This function enables/disables trap CDP PDU.
 * INPUT   : is_enable
 * OUTPUT  : None
 * RETURN  : TRUE   -- Success
 *           FALSE  -- If a given port is not available
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetPvstPktToCpu(BOOL_T is_enable);

#if (SYS_CPNT_PPPOE_IA == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPPPoEDPktToCpu
 *------------------------------------------------------------------------------
 * PURPOSE : To enable/disable trap PPPoE discover packet to CPU
 *            for specified unit/port.
 * INPUT   : unit       - unit
 *           port       - port
 *           is_enable  - TRUE to enable
 * OUTPUT  : none
 * RETURN  : TRUE       - Success
 *           FALSE      - Failed
 * NOTE    : 1. for projects who can install rule on trunk's member ports.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetPPPoEDPktToCpu(
    UI32_T  unit,
    UI32_T  port,
    BOOL_T  is_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPPPoEDPktToCpuPerSystem
 *------------------------------------------------------------------------------
 * PURPOSE : To enable/disable trap PPPoE discover packet to CPU
 *            for entire system.
 * INPUT   : is_enable  - TRUE to enable
 * OUTPUT  : none
 * RETURN  : TRUE       - Success
 *           FALSE      - Failed
 * NOTE    : 1. for projects who encounter problems to install rule on
 *              trunk's member ports.
 *           2. not implemented for bcm, yet.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetPPPoEDPktToCpuPerSystem(
    BOOL_T  is_enable
);
#endif /* SYS_CPNT_PPPOE_IA */

#if (SYS_CPNT_DOS == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetDosProtectionFilter
 *------------------------------------------------------------------------------
 * FUNCTION: This function will config DoS protection
 * INPUT   : type   - the type of DOS protection to config
 *           enable - TRUE to enable; FALSE to disable.
 * OUTPUT  : None
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetDosProtectionFilter(UI32_T type, BOOL_T enable);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetDosProtectionRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: This function will config DoS protection
 * INPUT   : type   - the type of DOS protection to config
 *           rate   - rate in kbps. 0 to disable.
 * OUTPUT  : None
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_CTRL_SetDosProtectionRateLimit(UI32_T type, UI32_T rate);
#endif /* SYS_CPNT_DOS */

#if (SYS_CPNT_VLAN_XLATE == TRUE && SYS_CPNT_VLAN_XLATE_BY_RULE == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetPortVlanXlate
 *------------------------------------------------------------------------------
 * FUNCTION: Set rule for VLAN translation
 * INPUT   : interface      - Which interface
 *           old_vid        - Old VLAN ID
 *           new_vid        - New VLAN ID; 0 means to remove.
 * OUTPUT  : None
 * RETURN  : RULE_TYPE_OK if succeeded; otherwise error code.
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_SetPortVlanXlate(
    RULE_TYPE_INTERFACE_INFO_T *interface,
    UI32_T old_vid,
    UI32_T new_vid
);
#endif /* SYS_CPNT_VLAN_XLATE_BY_RULE */

#if (SYS_CPNT_ND_BY_RULE == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_IP6_SetNdRsPacketToCpu
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for IPv6 ND Router solication packet.
 * INPUT    : enable    - TRUE, install rule;
 *                        FALSE, uninstall rule.
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IP6_SetNdRsPacketToCpu(
    BOOL_T enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_IP6_SetNdRaPacketToCpu
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for IPv6 ND Router Advertisement packet.
 * INPUT    : enable    - TRUE, install rule;
 *                        FALSE, uninstall rule.
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IP6_SetNdRaPacketToCpu(
    BOOL_T enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_IP6_SetNdNsPacketToCpu
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for IPv6 ND Neighbor Solicitation packet.
 * INPUT    : enable    - TRUE, install rule;
 *                        FALSE, uninstall rule.
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IP6_SetNdNsPacketToCpu(
    BOOL_T enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_IP6_SetNdNaPacketToCpu
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for IPv6 ND Neighbor Advertisement packet.
 * INPUT    : enable    - TRUE, install rule;
 *                        FALSE, uninstall rule.
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IP6_SetNdNaPacketToCpu(
    BOOL_T enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_IP6_SetNdRedirectPacketToCpu
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for IPv6 ND Redirect packet.
 * INPUT    : enable    - TRUE, install rule;
 *                        FALSE, uninstall rule.
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IP6_SetNdRedirectPacketToCpu(
    BOOL_T enable
);
#endif /* SYS_CPNT_ND_BY_RULE */

#if (SYS_CPNT_DAI_ARP_ACL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DAI_DropArpPacket
 *------------------------------------------------------------------------------
 * FUNCTION: This function will drop ARP packet by sender IP + source MAC
 * INPUT   : enable         - TREUE: Add the new entry to drop the specify ARP packet.
 *                            FALSE: Delete the specify entry.
 *           src_mac_addr   - Source mac address, src_mac_addr[SYS_ADPT_MAC_ADDR_LEN]
 *           src_ip_addr    - Source IP address
 * OUTPUT  : None
 * RETURN  : RULE_TYPE_RETURN_TYPE_T
 * NOTE    : The total entry for this function is
 *           SYS_ADPT_ARP_INSPECTION_MAX_NBR_OF_DENY_RULE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_DAI_DropArpPacket(
    BOOL_T enable,
    UI8_T *src_mac_addr,
    UI32_T src_ip_addr
);
#endif /* SYS_CPNT_DAI_ARP_ACL */

#if (SYS_CPNT_DOT1X_EAPOL_PASS_THROUGH_BY_RULE == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_1X_SetEapolPassThrough
 *------------------------------------------------------------------------------
 * PURPOSE:  Add/Delete rule for eapol-pass-through
 * INPUT:    is_enable: TRUE - add rule.
 *                      FALSE - delete rule.
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE
 * NOTE:     ethertype = 0x888e, DA = 0x0180c2000003
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_1X_SetEapolPassThrough(
    BOOL_T is_enable
);
#endif /* SYS_CPNT_DOT1X_EAPOL_PASS_THROUGH_BY_RULE */

#if (SYS_ADPT_MODIFY_MGMT_QUEUE_BY_RULE == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_ModifyCpuQueueForMyMacMyIp
 *------------------------------------------------------------------------------
 * PURPOSE  : Modified my mac my ip packet to CPU queue 4
 * INPUT    : enable - TRUE: Add rule
 *                     FALSE: Delete rule
 *            my_ip  - IPv4 address array[SYS_ADPT_IPV4_ADDR_LEN]
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    :
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_ModifyCpuQueueForMyMacMyIp(
    BOOL_T enable,
    UI8_T *my_ip
);
#endif /* SYS_ADPT_MODIFY_MGMT_QUEUE_BY_RULE */

#if (SYS_CPNT_DAI_RATE_LIMIT_BY_RULE == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_DAI_TrapArpPacketToCpuWithRate
 *------------------------------------------------------------------------------
 * PURPOSE  : trap ARP packet to CPU and set rate limit
 * INPUT    : enable    - TRUE:  add rule
 *                        FALSE: Delete rule.
 *            interface - interface information
 *            rate      - packet rate per second
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK / RULE_TYPE_FAIL
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_DAI_TrapArpPacketToCpuWithRate(
    BOOL_T enable,
    RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T rate
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_DAI_GetArpPacketCounter
 *------------------------------------------------------------------------------
 * PURPOSE  : Get the rule counter for the ARP packet trapped to CPU 
 * INPUT    : interface_p    - interface information
 *            trunk_member_p - trunk member port bit map
 *            reset          - TRUE: clear counter, FALSE: not clear
 * OUTPUT   : total_cnt_p - The total counter
 * RETURN   : RULE_TYPE_OK / RULE_TYPE_FAIL
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_DAI_GetArpPacketCounter(
    RULE_TYPE_INTERFACE_INFO_T *interface_p,
    RULE_TYPE_TRUNK_MEMBER_T *trunk_member_p,
    BOOL_T reset,
    UI32_T *total_cnt_p
);
#endif /* SYS_CPNT_DAI_RATE_LIMIT_BY_RULE */

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_MDNS_SetMcastDnsToCpu
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the rule for mDNS packet to CPU
 * INPUT    : is_enable  - TRUE: add rule, FALSE: delete rule
 *            to_cpu     -
 *            flood      -
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK / RULE_TYPE_FAIL
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_MDNS_SetMcastDnsToCpu(
    BOOL_T is_enable,
    BOOL_T to_cpu,
    BOOL_T flood
);

#endif /* End of _RULE_CTRL_H */

