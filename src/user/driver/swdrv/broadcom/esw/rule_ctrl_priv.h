
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_LocalInitialize
 *------------------------------------------------------------------------------
 * PURPOSE  : initialize local unit driver
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_LocalInitialize();

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_CreateVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Create virtual group on device
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_CreateVirtualGroup();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_UpdateDevInfo
 *------------------------------------------------------------------------------
 * PURPOSE:  Update device information
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     Invoke when unit received a enter master mode, a hot insertion,
 *           and hot removal event
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_UpdateDevInfo();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalIsNeedToConfigCpuRule
 *------------------------------------------------------------------------------
 * PURPOSE:  need to config chip or not
 * INPUT:    enable_flag, packet_type
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     to avoid add an existed rule or remove a non-existed rule
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_LocalIsNeedToConfigCpuRule(
    BOOL_T enable_flag,
    RULE_TYPE_PacketType_T packet_type
);

static BOOL_T RULE_CTRL_TrapDhcpToCPU(BOOL_T enable);

static BOOL_T RULE_CTRL_TrapRipToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapOspfToCPU(BOOL_T enable);
/* 2008-07-21, Jinfeng Chen:
    Will use it later.
 */
#if 0
static BOOL_T RULE_CTRL_TrapDvmrpToCPU(BOOL_T enable);
#endif
static BOOL_T RULE_CTRL_TrapPimToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapVrrpToCPU(BOOL_T enable);


/* added by steven.gao for OSPFv3 */
#if (SYS_CPNT_OSPF6 == TRUE)
static BOOL_T RULE_CTRL_TrapOspf6ToCPU(BOOL_T enable);
#endif

static BOOL_T RULE_CTRL_TrapSlfToCPU(BOOL_T enable, UI16_T vid, UI8_T action);
static BOOL_T RULE_CTRL_TrapCdpToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapPvstToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapAllHostToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapAllRouterToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapMyMacMyIpToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_ModifyCpuQueueForL2Slf(BOOL_T enable);
static BOOL_T RULE_CTRL_CopyArpReplyToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_CopyArpRequestToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapAllArpReplyToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapAllArpRequestToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapAllArpToCPUWithMeter(BOOL_T enable, UI32_T packet_rate);
static BOOL_T RULE_CTRL_IGMPSNP_SetIgmpRule(BOOL_T enable, BOOL_T to_cpu);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetOrgSpecific1Packet
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for Organization Specific 1 packet
 * INPUT    : enable - Set TRUE as add rule
 *                     Set FLASE as remove
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_SetOrgSpecific1Packet(
    BOOL_T enable,
    BOOL_T to_cpu,
    BOOL_T flood
);

#if (SYS_CPNT_UDLD == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_UDLD_SetUdldPacket
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for UDLP packet
 * INPUT    : enable - Set TRUE as add rule
 *                     Set FLASE as remove
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_UDLD_SetUdldPacket(
    BOOL_T enable,
    BOOL_T to_cpu,
    BOOL_T flood
);
#endif /* SYS_CPNT_UDLD */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetOrgSpecific3Packet
 *------------------------------------------------------------------------------
 * PURPOSE  : Set rule for Organization Specific 3 packet
 * INPUT    : enable - Set TRUE as add rule
 *                     Set FLASE as remove
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_SetOrgSpecific3Packet(
    BOOL_T enable,
    BOOL_T to_cpu,
    BOOL_T flood
);

static BOOL_T RULE_CTRL_IGMPSNP_DO_SetIgmpRule(RULE_CTRL_PARAM_PTR param_p);
static BOOL_T RULE_CTRL_TrapHbtWorkAroundToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapIpMcastToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_ModifyCpuQueueForBpdu(BOOL_T enable);
static BOOL_T RULE_CTRL_ModifyCpuQueueForL2cp(BOOL_T enable);

#if (SYS_CPNT_IGMPSNP == TRUE && SYS_CPNT_IGMPSNP_RESERVE_ADDRESS_PACKET_CHIP_TRAP_TO_CPU_BUT_NOT_FORWARD == FALSE)
static BOOL_T RULE_CTRL_IGMPSNP_CancelReservedMulticastToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_IGMPSNP_DO_CancelReservedMulticastToCPU(RULE_CTRL_PARAM_PTR param_p);
#endif /* SYS_CPNT_IGMPSNP && !SYS_CPNT_IGMPSNP_RESERVE_ADDRESS_PACKET_CHIP_TRAP_TO_CPU_BUT_NOT_FORWARD */

/* 2008-07-21, Jinfeng Chen:
    Will use it later.
 */
#if 1 /*fuzhimin,20090505*/
static BOOL_T RULE_CTRL_TrapDhcpSnoopingToCPU(BOOL_T enable/*, UI32_T vid*/);
#endif
static BOOL_T RULE_CTRL_TrapOrgSpecificToCPU(BOOL_T enable);
#if (SYS_CPNT_CFM == TRUE)
    static BOOL_T RULE_CTRL_TrapCfmCtrlPktToCpu(BOOL_T enable);
#endif
static BOOL_T RULE_CTRL_TrapIpv6AllNodesToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapIpv6AllRoutersToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapUnknownIpv6McastToCPU(BOOL_T enable, BOOL_T to_cpu, BOOL_T flood);
static BOOL_T RULE_CTRL_TrapPim6ToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapEapsToCPU(BOOL_T enable, UI32_T vid, UI32_T mode);
static BOOL_T RULE_CTRL_TrapIpBcastToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapIpOptionToCPU(BOOL_T enable);
static BOOL_T RULE_CTRL_TrapUnknownIpMcastToCPU(BOOL_T enable, BOOL_T to_cpu, BOOL_T flood);
static BOOL_T RULE_CTRL_GetRuleNumberByClassMap(const RULE_TYPE_ClassMap_T *class_map, UI32_T *rule_nbr);

#if (SYS_CPNT_DHCPV6 == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_DHCP6_TrapDHCP6ClientPacketToCPU
 *------------------------------------------------------------------------------
 * PURPOSE  : trap DHCPv6 client packet to CPU
 * INPUT    : enable
 *            to_cpu
 *            flood
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_DHCP6_TrapDHCP6ClientPacketToCPU(
  BOOL_T enable, BOOL_T to_cpu, BOOL_T flood);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_DHCP6_TrapDHCP6ServerPacketToCPU
 *------------------------------------------------------------------------------
 * PURPOSE  : trap DHCPv6 server packet to CPU
 * INPUT    : enable
 *            to_cpu
 *            flood
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_DHCP6_TrapDHCP6ServerPacketToCPU(
  BOOL_T enable, BOOL_T to_cpu, BOOL_T flood);
#endif /* #if (SYS_CPNT_DHCPV6 == TRUE) */

#if (SYS_CPNT_PTP == TRUE)
static BOOL_T
RULE_CTRL_PTP_TrapPtpEventUdpToCPU(
    BOOL_T enable
);

static BOOL_T
RULE_CTRL_PTP_TrapPtpEventEthToCPU(
    BOOL_T enable
);

static BOOL_T
RULE_CTRL_PTP_TrapPtpGeneralUdpToCPU(
    BOOL_T enable
);
#endif /* SYS_CPNT_PTP */

#if (SYS_CPNT_MLD == TRUE || SYS_CPNT_MLDSNP == TRUE || SYS_CPNT_MVR6 == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IP6_SetMldQueryRule
 *------------------------------------------------------------------------------
 * PURPOSE  : trap MLD event packet to CPU
 * INPUT    : enable
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_IP6_SetMldQueryRule(
    BOOL_T enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IP6_SetMldReportRule
 *------------------------------------------------------------------------------
 * PURPOSE  : trap MLD event packet to CPU
 * INPUT    : enable
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_IP6_SetMldReportRule(
    BOOL_T enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IP6_SetMldDoneRule
 *------------------------------------------------------------------------------
 * PURPOSE  : trap MLD event packet to CPU
 * INPUT    : enable
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_IP6_SetMldDoneRule(
    BOOL_T enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IP6_SetMldv2ReportsRule
 *------------------------------------------------------------------------------
 * PURPOSE  : trap MLD event packet to CPU
 * INPUT    : enable
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_IP6_SetMldv2ReportsRule(
    BOOL_T enable
);
#endif /* SYS_CPNT_MLD || SYS_CPNT_MLDSNP || SYS_CPNT_MVR6 */

#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_IPSG_GETOM_DenyAnyIpPacket
 *------------------------------------------------------------------------------
 * PURPOSE:  Get rule storage
 * INPUT:    param_p - General parameter
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
static void*
RULE_CTRL_IPSG_GETOM_DenyAnyIpPacket(
    RULE_CTRL_PARAM_PTR param_p
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_IPSG_SETRULE_DenyAnyIpPacket
 *------------------------------------------------------------------------------
 * PURPOSE:  Deny any IP packet for IPSG
 * INPUT:    param_p - General parameter
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_IPSG_SETRULE_DenyAnyIpPacket(
    RULE_CTRL_PARAM_PTR param_p,
    bcm_pbmp_t new_pbmp
);
#endif /* #if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE) */

static void
RULE_CTRL_GetTcamPoolCapabilityBitMap(
    UI32_T pool_id,
    RULE_TYPE_TCAM_CAP_BITMAP_T *bit_map_p
);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalUpdateTimeACLRelationRulePerInterface(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI8_T *state_change_list
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalUpdateTimeACLRelationRulePerInterface_ACL(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI8_T *state_change_list,
    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p
);
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_DAI_RATE_LIMIT_BY_RULE == TRUE)
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_DAI_GetArpPacketCounterByUport(
  UI32_T unit,
  UI32_T port,
  RULE_TYPE_CounterRec_T *counter_rec_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_DAI_ClearArpPacketCounterByUport(
    UI32_T unit,
    UI32_T port
);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - RULE_CTRL_DAI_DO_TrapArpPacketToCpuWithRate
 * ---------------------------------------------------------------------
 * PURPOSE  : trap ARP packet to CPU and set rate limit
 * INPUT    : param_p
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 * ---------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_DAI_DO_TrapArpPacketToCpuWitherRate(
    RULE_CTRL_PARAM_PTR param_p
);
#endif /* SYS_CPNT_DAI_RATE_LIMIT_BY_RULE */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_CheckResourceOnPortListForQosPurpose
 *------------------------------------------------------------------------------
 * PURPOSE  : check every port in port list free rule number for QoS purpose
 * INPUT    : group_id, req_nbr, port_list
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_CheckResourceOnPortListForQosPurpose(
    UI32_T group_id,
    UI32_T req_nbr,
    const UI8_T *port_list
);

static BOOL_T RULE_CTRL_Convert_Ipv6Prefix2Mask(UI32_T prefix_length, UI8_T *mask);

/* 2008-07-20, Jinfeng Chen:
   maybe will use it later, hide it to remove warning when compiling
 */
static BOOL_T
RULE_CTRL_ConvertAce2DevForAclPurpose(
    RULE_TYPE_Ace_Entry_T *ace_entry,
    DEVRM_AceEntry_T *dev_ace_entry,
    DEVRM_ActionEntry_T *action_entry
);

static BOOL_T
RULE_CTRL_ConvertAce2DevForQosPurpose(
    UI32_T unit,
    UI32_T port,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    DEVRM_AceEntry_T *dev_ace_entry
);

static BOOL_T RULE_CTRL_ConvertMacAce2DevAce(const RULE_TYPE_MacAce_Entry_T *mac_ace, DEVRM_AceEntry_T *dev_ace);
static BOOL_T RULE_CTRL_ConvertIpAce2DevAce(const RULE_TYPE_IpAce_Entry_T *ip_ace, DEVRM_AceEntry_T *dev_ace);
static BOOL_T RULE_CTRL_ConvertIpv6Ace2DevAce(const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace, DEVRM_AceEntry_T *dev_ace);
static BOOL_T RULE_CTRL_ConvertIpv6Ace2ExtDevAce(const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace, DEVRM_AceEntry_T *dev_ace);
static BOOL_T RULE_CTRL_ConvertIpv6Ace2StdDevAce(const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace, DEVRM_AceEntry_T *dev_ace);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalSetACL2DEV
 *------------------------------------------------------------------------------
 * PURPOSE:  Allocate and set rule for an ACL
 * INPUT:    is_ingress, unit, port, acl_id, is_trunk, is_add, time_range_index
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetACL2DEV(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI32_T acl_id,
    const RULE_TYPE_Acl_T *acl_entry_p,
    UI32_T time_range_index,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalRemoveACLFromDEV
 *------------------------------------------------------------------------------
 * PURPOSE:  Remove ACL rule
 * INPUT:    is_ingress, unit, port, acl_id, is_trunk, is_add, time_range_index
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveACLFromDEV(
    const RULE_TYPE_INTERFACE_INFO_PTR_T interface_p,
    UI32_T acl_id,
    const RULE_TYPE_Acl_T *acl_entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalFreeRulesByRuleGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : free rules from chip and selector_info
 * INPUT    : unit, port, class_instance, acl_type, is_qos
 * OUTPUT   : None
 * RETURN   : TRUE -- succeeded / FALS -- failed
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalFreeRulesByRuleGroup(
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_instance
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalFreeRuleInRuleGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : free a rule from chip and selector_info
 * INPUT    : unit, device_id, class_instance, dev_rule_id, ace_index, acl_type, is_qos
 * OUTPUT   : None
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalFreeRuleInRuleGroup(
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_instance,
    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p
);

static RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_CTRL_Priv_GetAclInstanceById(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T acl_index
);

static RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_CTRL_Priv_GetSharingAclInstance_2(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T acl_id,
    const RULE_TYPE_Acl_T *acl_entry_p,
    UI32_T time_range_index,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_SetPolicyMapInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Set policy map instance
 * INPUT    : unit              - unit
 *            port              - port
 *            direction         - direction
 *            policy_map_inst_p - a pointer to policy map instance
 * OUTPUT   : None
 * RETURN   : policy map instance; NULL
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_CTRL_Priv_SetPolicyMapInstance(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetPolicyMapInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Get policy map instance
 * INPUT    : unit      - unit
 *            port      - port
 *            direction - direction
 * OUTPUT   : None
 * RETURN   : policy map instance; NULL
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_CTRL_Priv_GetPolicyMapInstance(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetAcePriorityInAcl(
    UI32_T acl_index,
    UI32_T ace_index,
    int *priority_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetClassMapElementPriorityInClassMap(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    RULE_TYPE_ClassType_T class_type,
    UI16_T class_id,
    int *priority_p
);

#pragma mark -
#pragma mark Instance Functions

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalGetRuleInstanceFromClassInstanceByAceId
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a rule instance from class instance
 * INPUT:    class_inst_p   - class instance
 *           ace_id         - ACE ID
 * OUTPUT:   None
 * RETURN:   Object pointer of rule instance
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RULE_INSTANCE_PTR_T
RULE_CTRL_LocalGetRuleInstanceFromClassInstanceByAceId(
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p,
    UI32_T  ace_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_InitInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Initialize instance table
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Initialize before any instance operations
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_InitInstance();

#ifdef RULE_CTRL_INSTANCE_DEBUG_ON
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ValidateClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Validate class instance
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only for debug
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ValidateClassInstance(
    const RULE_TYPE_CLASS_INSTANCE_T *in
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ValidateRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Validate rule instance
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only for debug
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ValidateRuleInstance(
    const RULE_TYPE_RULE_INSTANCE_T *in
);
#endif /* RULE_CTRL_INSTANCE_DEBUG_ON */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_IndexOf
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_IN_IndexOf(
    RULE_TYPE_INSTANCE_T *in_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_AtIndex
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
void *
RULE_CTRL_IN_AtIndex(
    RULE_TYPE_INSTANCE_TYPE_T type,
    UI32_T index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_IndexOfClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_IN_IndexOfClassInstance(
    RULE_TYPE_CLASS_INSTANCE_T *in_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_IndexOfRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_IN_IndexOfRuleInstance(
    RULE_TYPE_RULE_INSTANCE_T *in_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Ptr2Instance
 *------------------------------------------------------------------------------
 * PURPOSE  :
 * INPUT    :
 * OUTPUT   :
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
void *
RULE_CTRL_IN_Ptr2Instance(
    RULE_TYPE_SHM_POINTER_T ptr
);

RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Instance2Ptr(
    const RULE_TYPE_INSTANCE_PTR_T in
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Create instance
 * INPUT    : base_addr - base address of entry table
 *            type      - type of instance
 * OUTPUT   : None
 * RETURN   : Pointer of instance entry
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Priv_CreateInstance(
    void *base_addr,
    RULE_TYPE_INSTANCE_TYPE_T type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy instance
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_DestroyInstance(
    RULE_TYPE_SHM_POINTER_T ptr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Create class instance
 * INPUT    : type  - type of instance
 * OUTPUT   : None
 * RETURN   : Pointer of instance entry
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Priv_CreateClassInstance(
    RULE_TYPE_INSTANCE_TYPE_T type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Create rule instance
 * INPUT    : type  - type of instance
 * OUTPUT   : None
 * RETURN   : Pointer of instance entry
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_SHM_POINTER_T
RULE_CTRL_IN_Priv_CreateRuleInstance(
    RULE_TYPE_INSTANCE_TYPE_T type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyContainerInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy a container object
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : All child instance also be destroied
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_DestroyContainer(
    RULE_TYPE_SHM_POINTER_T ptr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy a class instance
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : All child instance also be destroied
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_DestroyClassInstance(
    RULE_TYPE_SHM_POINTER_T ptr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyRuleInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy a rule instance
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_DestroyRuleInstance(
    RULE_TYPE_SHM_POINTER_T ptr
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_Container_AddInstance(
    RULE_TYPE_CONTAINER_OBJECT_T *this,
    const RULE_TYPE_INSTANCE_PTR_T in
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_Container_RemoveInstance(
    RULE_TYPE_CONTAINER_OBJECT_T *this,
    const RULE_TYPE_INSTANCE_PTR_T in
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct class instance object from pointer of instance entry
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructContainerObjectFromPointer(
    RULE_TYPE_SHM_POINTER_T ptr,
    RULE_TYPE_CONTAINER_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructContainerObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct container object from class instance
 * INPUT    : in    - source container
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructContainerObject(
    const RULE_TYPE_CONTAINER_T *in,
    RULE_TYPE_CONTAINER_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateContainerObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Create container object
 * INPUT    : type  - type of instance
 * OUTPUT   : out   - instance object
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_CreateContainerObject(
    RULE_TYPE_CONTAINER_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyContainerObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy container
 * INPUT    : this  - instance object
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : All child instance also be destroied
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_DestroyContainerObject(
    RULE_TYPE_CONTAINER_OBJECT_T *this
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct class instance object from pointer of instance entry
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(
    RULE_TYPE_SHM_POINTER_T ptr,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructClassInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct class instance object from class instance
 * INPUT    : in    - source class instance
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructClassInstanceObject(
    const RULE_TYPE_CLASS_INSTANCE_T *in,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateClassInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Create class instance object
 * INPUT    : type  - type of instance
 * OUTPUT   : out   - instance object
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_CreateClassInstanceObject(
    RULE_TYPE_INSTANCE_TYPE_T type,
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyClassInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy class instance
 * INPUT    : this  - instance object
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : All child instance also be destroied
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_DestroyClassInstanceObject(
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructRuleInstanceObjectFromPointer
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct rule instance object from pointer of instance entry
 * INPUT    : ptr   - pointer of instance entry
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructRuleInstanceObjectFromPointer(
    RULE_TYPE_SHM_POINTER_T ptr,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_ConstructRuleInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Consturct rule instance object from rule instance
 * INPUT    : in    - source rule instance
 * OUTPUT   : out   - instance object
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_IN_Priv_ConstructRuleInstanceObject(
    RULE_TYPE_RULE_INSTANCE_T *in,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_CreateRuleInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Create rule instance object
 * INPUT    : type  - type of instance
 * OUTPUT   : out   - instance object
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_CreateRuleInstanceObject(
    RULE_TYPE_INSTANCE_TYPE_T type,
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *out
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_IN_Priv_DestroyRuleInstanceObject
 *------------------------------------------------------------------------------
 * PURPOSE  : Destroy rule instance
 * INPUT    : this  - instance object
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK; error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_IN_Priv_DestroyRuleInstanceObject(
    RULE_TYPE_RULE_INSTANCE_OBJECT_T *this
);

static void
RULE_CTRL_IN_Priv_Walk(
    RULE_TYPE_INSTANCE_PTR_T in,
    void(*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cooike
);

static RULE_TYPE_INSTANCE_PTR_T
RULE_CTRL_IN_Priv_FindIf(
    RULE_TYPE_INSTANCE_PTR_T in,
    BOOL_T (*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cookie
);

static UI32_T
RULE_CTRL_IN_Priv_CountIf(
    RULE_TYPE_INSTANCE_PTR_T in,
    BOOL_T (*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cookie
);

#pragma mark End of Instance Functions
#pragma mark -

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalAddAce2DEV
 *------------------------------------------------------------------------------
 * PURPOSE  : add a rule to chip and check whether the acl use on ACL or QoS
 * INPUT    : acl_index, acl_entry, rule_entry
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK ; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddAce2DEV(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddAce2DevForAclPurpose(
    BOOL_T is_ingress,
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalAddAce2DevForQosPurpose(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    const UI8_T *port_list
);

static void
RULE_CTRL_LocalAddAce2DevForQosPurpose_Hello(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

static BOOL_T
RULE_CTRL_AddRule2DevForAclPurpose(
    BOOL_T is_ingress,
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T rule_id,
    bcm_pbmp_t port_bit_map,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    UI32_T ace_index,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

static BOOL_T
RULE_CTRL_AddMacRule2DevForAclPurpose(
    BOOL_T is_ingress,
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T rule_id,
    bcm_pbmp_t port_bit_map,
    const RULE_TYPE_AceAction_T action,
    const RULE_TYPE_MacAce_Entry_T *mac_ace,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

static BOOL_T
RULE_CTRL_AddIpRule2DevForAclPurpose(
    BOOL_T is_ingress,
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T rule_id,
    bcm_pbmp_t port_bit_map,
    const RULE_TYPE_AceAction_T action,
    const RULE_TYPE_IpAce_Entry_T *ip_ace,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

static BOOL_T
RULE_CTRL_AddIpv6Rule2DevForAclPurpose(
    BOOL_T is_ingress,
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T rule_id,
    bcm_pbmp_t port_bit_map,
    const RULE_TYPE_AceAction_T action,
    const RULE_TYPE_Ipv6Ace_Entry_T *ipv6_ace,
    UI32_T ace_type,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveAceFromDEV(
    UI32_T acl_id,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveAceFromDevForAclPurpose(
    BOOL_T is_ingress,
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveAceFromDevForQosPurpose(
    UI32_T acl_index,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p
);

static BOOL_T
RULE_CTRL_SetAce2Dev(
    BOOL_T is_ingress,
    UI32_T unit,
    UI32_T port,
    UI32_T group_id,
    UI32_T dev_rule_id,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    UI32_T ace_index,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

static BOOL_T
RULE_CTRL_GetFunctionTypeForACLAndQoS(
    BOOL_T is_acl,
    BOOL_T is_ingress,
    UI32_T ace_type,
    RULE_TYPE_FunctionType_T *fun_type_p
);

static BOOL_T
RULE_CTRL_GetFunctionInfoByAceTypeAndDirection(
    BOOL_T is_acl,
    BOOL_T is_ingress,
    UI32_T ace_type,
    UI32_T *group_id,
    int *rule_pri
);

static BOOL_T
RULE_CTRL_GetFunctionInfoByFunctionType(
    UI32_T fun_type,
    UI32_T *group_id,
    int *rule_pri
);

// PATCH CODE !!
// FIXME: remove this function
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalCheckAclOnBinding(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T acl_index
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalCheckPolicyMapOnBinding(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetPolicyMap2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemovePolicyMap2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetClassMap2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    UI32_T class_map_index,
    UI32_T meter_id,
    UI32_T action_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveClassMap2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T  policy_map_index,
    const RULE_TYPE_PolicyMap_T *policy_map_p,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p,
    UI32_T  class_map_index,
    UI32_T  meter_id,
    UI32_T  action_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSyncClassMap2DEV(
    UI32_T policy_map_index,
    UI32_T class_map_index
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetQoSACL2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T acl_id,
    UI32_T meter_id,
    UI32_T action_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveQoSACL2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T acl_id,
    UI32_T meter_id,
    UI32_T action_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalSetMF2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T mf_index,
    UI32_T meter_id,
    UI32_T action_id
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_LocalRemoveMF2DEV(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    UI32_T mf_index,
    UI32_T meter_id,
    UI32_T action_id
);

static BOOL_T
RULE_CTRL_SetQoSAce2Dev(
    BOOL_T is_ingress,
    UI32_T unit,
    UI32_T port,
    UI32_T group_id,
    UI32_T rule_id,
    const RULE_TYPE_Ace_Entry_T *ace_entry_p,
    const RULE_TYPE_TBParamEntry_T *meter_entry_p,
    const RULE_TYPE_Action_T *action_entry_p,
    UI32_T sm_rid
);

static BOOL_T
RULE_CTRL_LocalConvertMeter2Dev(
    const RULE_TYPE_TBParamEntry_T *meter_entry_p,
    DEVRM_MeterEntry_T *dev_meter_entry
);

static BOOL_T RULE_CTRL_ConvertDev2IscAction(const DEVRM_ActionEntry_T *dev_action, RULE_CTRL_IscBuf_T *isc_buf);
static BOOL_T RULE_CTRL_ConvertIsc2DevAction(RULE_CTRL_IscBuf_T *isc_buf);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Update_PortBitMap
 *------------------------------------------------------------------------------
 * PURPOSE  : update IPBM field on chip by unit, port, acl
 * INPUT    : is_ingress, is_add, unit, port, acl_entry
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Update_PortBitMap(
    BOOL_T is_ingress,
    BOOL_T is_add,
    UI32_T unit,
    UI32_T port,
    const RULE_TYPE_Acl_T *acl_entry_p,
    const RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p
);

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_TakeSnapshot
 *------------------------------------------------------------------------------
 * PURPOSE:  Take snapshot
 * INPUT:    snapshot_p - Snapshot
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_Priv_TakeSnapshot(
    RULE_CTRL_Snapshot_T *snapshot_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_RevertToSnapshot
 *------------------------------------------------------------------------------
 * PURPOSE:  Revert to snapshot
 * INPUT:    snapshot_p - Snapshot
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_Priv_RevertToSnapshot(
    RULE_CTRL_Snapshot_T *snapshot_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_DeleteSnapshot
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete snapshot
 * INPUT:    snapshot_p - Snapshot
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_Priv_DeleteSnapshot(
    RULE_CTRL_Snapshot_T *snapshot_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_InitFunTypeFreeTable
 *------------------------------------------------------------------------------
 * PURPOSE:  Initializes the free function type table
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_InitFunTypeFreeTable();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_DefragRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Defrag rule
 * INPUT:    direction  - Which direction of rule need to defrag
 *           acl_id     - Additional ACL need to add during rule defraging
 *           fn_commit  - Commit function for add the additional ACL
 *           fn_remove  - Remove function for del the additional ACL
 *           cookie     - The parameter for fn_commit and fn_remove
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_DefragRule(
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_id,
    RULE_TYPE_RETURN_TYPE_T (*fn_commit)(void *cookie),
    RULE_TYPE_RETURN_TYPE_T (*fn_remove)(void *cookie),
    void *cookie
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_Wrap_SetACL2DEV
 *------------------------------------------------------------------------------
 * PURPOSE:  The wrapped function for RULE_CTRL_Priv_DefragRule to set ACL to
 *           device
 * INPUT:    param_ptr  - An object pointer to RULE_CTRL_WRAP_FN_PARAM_T
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_Wrap_SetACL2DEV(
    void *param_ptr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_Wrap_RemoveACLFromDEV
 *------------------------------------------------------------------------------
 * PURPOSE:  The wrapped function for RULE_CTRL_Priv_DefragRule to remove ACL
 *           from device
 * INPUT:    param_ptr  - An object pointer to RULE_CTRL_WRAP_FN_PARAM_T
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_Wrap_RemoveACLFromDEV(
    void *param_ptr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetFunTypeFreeEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Get free function type entry
 * INPUT:    fun_type   - function type
 * OUTPUT:   None
 * RETURN:   entry
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_CTRL_FunTypeFreeEntry_T*
RULE_CTRL_Priv_GetFunTypeFreeEntry(
    RULE_TYPE_FunctionType_T fun_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_IsEnabledFuncTypeRemap
 *------------------------------------------------------------------------------
 * PURPOSE:  Does enable function type remap
 * INPUT:    fun_type       - function type
 *           remap_fun_type - remap to function type
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_Priv_IsEnabledFuncTypeRemap(
    RULE_TYPE_FunctionType_T fun_type,
    RULE_TYPE_FunctionType_T remap_fun_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_EnableFuncTypeRemap
 *------------------------------------------------------------------------------
 * PURPOSE:  Enable function type remap
 * INPUT:    fun_type       - function type
 *           remap_fun_type - remap to function type
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_EnableFuncTypeRemap(
    RULE_TYPE_FunctionType_T fun_type,
    RULE_TYPE_FunctionType_T remap_fun_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_DisableFuncTypeRemap
 *------------------------------------------------------------------------------
 * PURPOSE:  Disable function type remapping
 * INPUT:    fun_type       - function type
 *           remap_fun_type - remap to function type
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_DisableFuncTypeRemap(
    RULE_TYPE_FunctionType_T fun_type,
    RULE_TYPE_FunctionType_T remap_fun_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_BuildRemapFunTypeList
 *------------------------------------------------------------------------------
 * PURPOSE  : Get used function type list
 * INPUT    : fun_type                      - function type
 *            enumber_of_entries_to_write   - max entries of list
 * OUTPUT   : list                          - function list
 *            number_of_entries_written     - how many entries be written
 * RETURN   : RULE_TYPE_OK ; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_BuildRemapFunTypeList(
    RULE_TYPE_FunctionType_T fun_type,
    RULE_TYPE_FunctionType_T *list,
    UI32_T number_of_entries_to_write,
    UI32_T *number_of_entries_written
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_SetGroupProperty
 *------------------------------------------------------------------------------
 * PURPOSE:  Set group properties
 * INPUT:    fun_type       - original function type
 *           remap_fun_type - remapped function type
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_SetGroupProperty(
    RULE_TYPE_FunctionType_T fun_type,
    RULE_TYPE_FunctionType_T remap_fun_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_ChangeGroupQualifySet
 *------------------------------------------------------------------------------
 * PURPOSE:  Set group qualify set
 * INPUT:    fun_type       - original function type
 *           remap_fun_type - remapped function type
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_ChangeGroupQualifySet(
    RULE_TYPE_FunctionType_T fun_type,
    RULE_TYPE_FunctionType_T remap_fun_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_ChangeGroupPriority
 *------------------------------------------------------------------------------
 * PURPOSE:  Set group priority
 * INPUT:    fun_type       - original function type
 *           remap_fun_type - remapped function type
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_ChangeGroupPriority(
    RULE_TYPE_FunctionType_T fun_type,
    RULE_TYPE_FunctionType_T remap_fun_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetFuncTypeRefCountFromRule
 *------------------------------------------------------------------------------
 * PURPOSE  : Get the reference count of function type from rule
 * INPUT    : fun_type  - the function type
 * OUTPUT   : None
 * RETURN   : Reference count
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_CTRL_Priv_GetFuncTypeRefCountFromRule(
    RULE_TYPE_FunctionType_T fun_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetFuncTypeRefCountFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Get the reference count of function type from ACL
 * INPUT    : fun_type  - the function type
 * OUTPUT   : None
 * RETURN   : Reference count
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_CTRL_Priv_GetFuncTypeRefCountFromAcl(
    RULE_TYPE_FunctionType_T fun_type
);
#endif /* SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_SyncGroupPropertyToSlaverUnit
 *------------------------------------------------------------------------------
 * PURPOSE:  Synchronizes the property of group(selector) to slaver unit
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     Invoke this function when received hot insertion event
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_SyncGroupPropertyToSlaverUnit();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_FindBestFitFunctionType
 *------------------------------------------------------------------------------
 * PURPOSE:  Find best-fit function type for binding an ACL to port
 * INPUT:    unit           - unit
 *           device_id      - device_id
 *           direction      - direction
 *           acl_entry_p    - ACL entry
 * OUTPUT:   fun_type_p     - function type
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_FindBestFitFunctionType(
    UI32_T unit,
    UI32_T device_id,
    RULE_TYPE_InOutDirection_T direction,
    const RULE_TYPE_Acl_T *acl_entry_p,
    RULE_TYPE_FunctionType_T *fun_type_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_SortByRuleCount
 *------------------------------------------------------------------------------
 * PURPOSE:  Sort the list of function type by rule counter. From min to max.
 * INPUT:    fun_type_list      - Function type list
 *           number_of_entries  - How many element in the list
 *           unit               - unit. Use for obtaining rule count
 *           device_id          - device ID. Used for obtaining rule count
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static void
RULE_CTRL_Priv_SortByRuleCount(
    RULE_TYPE_FunctionType_T *fun_type_list,
    UI32_T number_of_entries,
    UI32_T unit,
    UI32_T device_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_CompareFunctionTypeByRuleCount
 *------------------------------------------------------------------------------
 * PURPOSE:  Compare two function type by rule count
 * INPUT:    fun_type_1         - Function type 1
 *           fun_type_2         - Function type 2
 *           unit               - unit. Use for obtaining rule count
 *           device_id          - device ID. Used for obtaining rule count
 * OUTPUT:   None
 * RETURN:   Return 1, if rule counter of fun_type_1 is greater than fun_type_2
 *           Return -1, if rule counter of fun_type_2 is greater than fun_type_1
 *           Return 0, if rule counter of fun_type_1 is equal fun_type_2
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
RULE_CTRL_Priv_CompareFunctionTypeByRuleCount(
    RULE_TYPE_FunctionType_T fun_type_1,
    RULE_TYPE_FunctionType_T fun_type_2,
    UI32_T unit,
    UI32_T device_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetAvailableFunctionTypeList
 *------------------------------------------------------------------------------
 * PURPOSE  : Get available function type list
 * INPUT    : direction                     - direction
 *            acl_entry                     - ACL
 *            enumber_of_entries_to_write   - max entries of list
 * OUTPUT   : fun_tye_list                  - available function list for the
 *                                            input ACL
 *            number_of_entries_written     - how many entries be written
 * RETURN   : RULE_TYPE_OK ; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetAvailableFunctionTypeList(
    RULE_TYPE_InOutDirection_T direction,
    const RULE_TYPE_Acl_T *acl_entry,
    RULE_TYPE_FunctionType_T *fun_stats,
    UI32_T number_of_entries_to_write,
    UI32_T *number_of_entries_written
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_SetupGroupByFunctionType
 *------------------------------------------------------------------------------
 * PURPOSE:  Setup group(seector) by function type
 * INPUT:    fun_type       - original function type
 *           remap_fun_type - remapped (really) function type
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_SetupGroupByFunctionType(
    RULE_TYPE_FunctionType_T fun_type,
    RULE_TYPE_FunctionType_T remap_fun_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_RecycleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Recycle group(selector).
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK ; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_RecycleGroup();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetFunctionTypeFromClassInstance
 *------------------------------------------------------------------------------
 * PURPOSE  : Get function type from class instance of ACL of port
 * INPUT    : unit          - unit
 *            port          - port
 *            direction     - direction
 *            acl_entry_p   - ACL
 * OUTPUT   : fun_type_p    - function type
 * RETURN   : RULE_TYPE_OK ; Error code
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_GetFunctionTypeFromClassInstance(
    UI32_T unit,
    UI32_T port,
    RULE_TYPE_InOutDirection_T direction,
    const RULE_TYPE_Acl_T *acl_entry_p,
    RULE_TYPE_FunctionType_T *fun_type_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_UPortToDeviceId
 *------------------------------------------------------------------------------
 * PURPOSE  : Convert unit/port to device ID
 * INPUT    : unit  - unit
 *            port  - port
 * OUTPUT   : None
 * RETURN   : device ID
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_CTRL_Priv_UPortToDeviceId(
    UI32_T unit,
    UI32_T port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetNextPort
 *------------------------------------------------------------------------------
 * PURPOSE  : Get next port from a port list
 * INPUT    : port_list - port list
 * OUTPUT   : ifindex_p - ifindex. Pass this value to 0 to start
 * RETURN   : NULL -- failed
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_Priv_GetNextPort(
    const UI8_T port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST],
    UI32_T *ifindex_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Ifindex_To_Interface(
    UI32_T ifindex,
    RULE_TYPE_INTERFACE_INFO_T *interface_p
);

static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Interface_To_Ifindex(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T *ifindex_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetResourceInfo
 *------------------------------------------------------------------------------
 * PURPOSE  : get selector resource info
 * INPUT    : unit, device_id, selector_id
 * OUTPUT   : None
 * RETURN   : NULL -- failed
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_CTRL_ResourceInfo_T*
RULE_CTRL_GetResourceInfo(
    UI32_T unit,
    UI32_T device_id,
    UI32_T group_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_FreeResourceInfo
 *------------------------------------------------------------------------------
 * PURPOSE  : increase free rule number
 * INPUT    : unit, device_id, group_id, rule_id
 * OUTPUT   : None
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_FreeResourceInfo(
    UI32_T unit,
    UI32_T device_id,
    UI32_T group_id
);

static BOOL_T RULE_CTRL_GetCpuRule(UI32_T device_id, UI32_T rule_type, BOOL_T *is_enabled, UI32_T *dev_rule_id);
static BOOL_T RULE_CTRL_SetCpuRule(UI32_T device_id, UI32_T rule_type, BOOL_T enable, UI32_T dev_rule_id);
/* 2008-07-22, Jinfeng Chen:
    Hide it to remove warning when compiling.
 */
#if 0
static BOOL_T RULE_CTRL_GetDhcpSnoopingRule(UI32_T device_id, UI32_T vid, BOOL_T is_client, UI32_T *dev_rule_id);
static BOOL_T RULE_CTRL_SetDhcpSnoopingRule(UI32_T device_id, UI32_T vid, BOOL_T is_client, UI32_T dev_rule_id);
#endif
static BOOL_T RULE_CTRL_GetSlfRule(UI32_T device_id, UI32_T vid, BOOL_T *is_enabled, UI8_T *slf_action, UI32_T *dev_rule_id);
static BOOL_T RULE_CTRL_SetSlfRule(UI32_T device_id, UI32_T vid, BOOL_T enable, UI8_T slf_action, UI32_T dev_rule_id);
static BOOL_T RULE_CTRL_IsNullMac(const UI8_T *mac);

/*should free this function allocate pointer*/
static DEVRM_ActionEntry_T*
RULE_CTRL_Convert_Action2DEV(
    UI32_T lport,
    BOOL_T is_ingress,
    const RULE_TYPE_Action_T *action_entry_p
);

static void RULE_CTRL_FreeDevActionListMemory(DEVRM_ActionEntry_T *dev_action);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_IfEntry_Index_To_Interface
 *------------------------------------------------------------------------------
 * PURPOSE  : Converts index of ifEntry to interface object
 * INPUT    : idx - index of ifEntry
 * OUTPUT   : interface_p - interface object
 * RETURN   : RULE_TYPE_OK; error code
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_Priv_IfEntry_Index_To_Interface(
    UI32_T idx,
    RULE_TYPE_INTERFACE_INFO_PTR_T interface_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Priv_GetIfEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get an ifEntry by interface
 * INPUT    : interface_p - interface
 * OUTPUT   : None
 * RETURN   : Pointer of ifEntry
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
static RULE_CTRL_IF_ENTRY_PTR_T
RULE_CTRL_Priv_GetIfEntry(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p
);

RULE_CTRL_RULE_ENTRY_PTR_T
RULE_CTRL_Priv_GetRuleEntryOfRule(
    const RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p
);

BOOL_T
RULE_CTRL_Priv_IsRuleInstanceMatchUnitDevice(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

BOOL_T
RULE_CTRL_Priv_IsClassInstanceMatchId(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

BOOL_T
RULE_CTRL_Priv_IsClassInstanceMatchTypeId(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

BOOL_T
RULE_CTRL_Priv_IsClassInstanceMatchType(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

BOOL_T
RULE_CTRL_Priv_IsMatchTypeId(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

BOOL_T
RULE_CTRL_Priv_IsAceInstanceWithId(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie
);

static UI32_T
RULE_CTRL_Priv_CountOfAceInstance(
    RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p,
    UI32_T id
);

static RULE_TYPE_INSTANCE_PTR_T
RULE_CTRL_Priv_GetFirstChildIf(
    RULE_TYPE_INSTANCE_PTR_T in,
    BOOL_T (*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cookie
);

static RULE_TYPE_INSTANCE_PTR_T
RULE_CTRL_Priv_GetNextSiblingIf(
    RULE_TYPE_INSTANCE_PTR_T in,
    BOOL_T (*fn)(RULE_TYPE_INSTANCE_PTR_T, void*),
    void *cookie
);

#if ((SWDRV_FIX_STANDALONE_TYPE == FALSE) && (SYS_CPNT_STACKING == TRUE))
    static BOOL_T RULE_CTRL_Slave_Allocate_Rule(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Slave_TrapPacket2Cpu(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Slave_Set_Rule(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Slave_Destroy_Rule(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Slave_Create_Meter(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Slave_Update_Meter(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Slave_Share_Meter(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Slave_Destroy_Meter(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Slave_Set_Action(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Slave_Destroy_Action(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Slave_Update_Rule(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Slave_GetRuleCounter(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Slave_ClearRuleCounter(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
    static BOOL_T RULE_CTRL_Master_Set_Allocated_Rule(ISC_Key_T *key, RULE_CTRL_IscBuf_T *ptr);
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_LocalTrapPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  trap specified packet to CPU on local unit
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_LocalTrapPacket2Cpu(
    BOOL_T enable_flag,
    RULE_TYPE_PacketType_T packet_type,
    RULE_TYPE_CpuRuleInfo_T *rule_info
);

static BOOL_T
RULE_CTRL_Allocate_Rule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T fun_type,
    int rule_pri,
    UI32_T *dev_rule_id
);

static BOOL_T
RULE_CTRL_Set_Rule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    DEVRM_MeterEntry_T *meter_entry,
    DEVRM_ActionEntry_T *action_entry,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

static BOOL_T
RULE_CTRL_Local_Allocate_Rule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T fun_type,
    int rule_pri,
    UI32_T *dev_rule_id
);

static BOOL_T
RULE_CTRL_Local_Set_Rule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    DEVRM_MeterEntry_T *meter_entry,
    DEVRM_ActionEntry_T *action_entry,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

static BOOL_T
RULE_CTRL_Destroy_Rule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T fun_type,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_Local_Destroy_Rule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T fun_type,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_GetRuleCounter(
    UI32_T unit,
    UI32_T device_id,
    UI32_T dev_rule_id,
    RULE_TYPE_CounterRec_T *counter_p
);

static BOOL_T
RULE_CTRL_ClearRuleCounter(
    UI32_T unit,
    UI32_T device_id,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_Local_GetRuleCounter(
    UI32_T device_id,
    UI32_T dev_rule_id,
    RULE_TYPE_CounterRec_T *counter_p
);

static BOOL_T
RULE_CTRL_Local_ClearRuleCounter(
    UI32_T device_id,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_temp_Allocate_Rule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T fun_type,
    int rule_pri,
    UI32_T *dev_rule_id
);

static BOOL_T
RULE_CTRL_temp_Destroy_Rule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T fun_type,
    UI32_T dev_rule_id
);

static BOOL_T RULE_CTRL_Create_Meter(UI32_T unit, UI32_T port, UI32_T device_id,UI32_T dev_rule_id, DEVRM_MeterEntry_T *meter_entry);
static BOOL_T RULE_CTRL_Local_Create_Meter(UI32_T device_id,UI32_T dev_rule_id, DEVRM_MeterEntry_T *meter_entry);

static BOOL_T RULE_CTRL_Update_Meter(UI32_T unit, UI32_T port, UI32_T device_id,UI32_T dev_rule_id, DEVRM_MeterEntry_T *meter_entry);
static BOOL_T RULE_CTRL_Local_Update_Meter(UI32_T device_id,UI32_T dev_rule_id, DEVRM_MeterEntry_T *meter_entry);

static BOOL_T RULE_CTRL_Share_Meter(UI32_T unit, UI32_T port, UI32_T device_id, UI32_T src_rule_id, UI32_T dev_rule_id);
static BOOL_T RULE_CTRL_Local_Share_Meter(UI32_T device_id, UI32_T src_rule_id, UI32_T dev_rule_id);

static BOOL_T RULE_CTRL_Destroy_Meter(UI32_T unit, UI32_T port, UI32_T device_id, UI32_T dev_rule_id);
static BOOL_T RULE_CTRL_Local_Destroy_Meter(UI32_T device_id, UI32_T dev_rule_id);

static BOOL_T RULE_CTRL_Set_Action(UI32_T unit, UI32_T port, UI32_T device_id, UI32_T rule_id, DEVRM_ActionEntry_T *action_entry);
static BOOL_T RULE_CTRL_Local_Set_Action(UI32_T device_id, UI32_T rule_id, DEVRM_ActionEntry_T *action_entry);

static BOOL_T RULE_CTRL_Local_Destroy_Action(UI32_T unit, UI32_T action_id);

static BOOL_T
RULE_CTRL_Update_Rule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    DEVRM_MeterEntry_T *meter_entry,
    DEVRM_ActionEntry_T *action_entry
);

static BOOL_T
RULE_CTRL_Local_Update_Rule(
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    DEVRM_MeterEntry_T *meter_entry,
    DEVRM_ActionEntry_T *action_entry
);

static BOOL_T
RULE_CTRL_GetDevInfo(
    UI32_T unit,
    DEVRM_DevInfo_T *dev_info_p
);

static BOOL_T
RULE_CTRL_Local_GetDevInfo(
    DEVRM_DevInfo_T *dev_info_p
);

#if ((SWDRV_FIX_STANDALONE_TYPE == FALSE) && (SYS_CPNT_STACKING == TRUE))
static BOOL_T
RULE_CTRL_Slave_GetDevInfo(
    ISC_Key_T *key,
    RULE_CTRL_IscBuf_T *ptr
);
#endif /* #if ((SWDRV_FIX_STANDALONE_TYPE == FALSE) && (SYS_CPNT_STACKING == TRUE)) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Set_Group_Qualify_Set
 *------------------------------------------------------------------------------
 * PURPOSE:  ISC function for set qualify set of group
 * INPUT:    unit       - unit
 *           device_id  - device_id
 *           group      - group
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_Set_Group_Qualify_Set(
    DEVRM_GroupEntry_T *group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Local_Set_Group_Qualify_Set
 *------------------------------------------------------------------------------
 * PURPOSE:  ISC function for set qualify set of group on local side
 * INPUT:    device_id  - device ID
 *           group      - group
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_Local_Set_Group_Qualify_Set(
    DEVRM_GroupEntry_T *group_p
);

#if ((SWDRV_FIX_STANDALONE_TYPE == FALSE) && (SYS_CPNT_STACKING == TRUE))
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Slave_Set_Group_Qualify_Set
 *------------------------------------------------------------------------------
 * PURPOSE:  Callback function for ISC function to set the qualify set of group
 * INPUT:    key        - ISC key
 *           ptr        - payload
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_Slave_Set_Group_Qualify_Set(
    ISC_Key_T *key,
    RULE_CTRL_IscBuf_T *ptr
);
#endif /* #if ((SWDRV_FIX_STANDALONE_TYPE == FALSE) && (SYS_CPNT_STACKING == TRUE)) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Set_Group_Priority
 *------------------------------------------------------------------------------
 * PURPOSE:  ISC function for set priority of group
 * INPUT:    unit       - unit
 *           device_id  - device ID
 *           group_id   - group ID
 *           priority   - priority value
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_Set_Group_Priority(
    UI32_T group_id,
    int priority
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Local_Set_Group_Priority
 *------------------------------------------------------------------------------
 * PURPOSE:  ISC function for set priority of group on local side
 * INPUT:    device_id  - device ID
 *           group_id   - group ID
 *           priority   - priority value
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_Local_Set_Group_Priority(
    UI32_T group_id,
    int priority
);

#if ((SWDRV_FIX_STANDALONE_TYPE == FALSE) && (SYS_CPNT_STACKING == TRUE))
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_Slave_Set_Group_Priority
 *------------------------------------------------------------------------------
 * PURPOSE:  Callback function for ISC function to set the group priority
 * INPUT:    key        - ISC key
 *           ptr        - payload
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_Slave_Set_Group_Priority(
    ISC_Key_T *key,
    RULE_CTRL_IscBuf_T *ptr
);
#endif /* #if ((SWDRV_FIX_STANDALONE_TYPE == FALSE) && (SYS_CPNT_STACKING == TRUE)) */

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
static BOOL_T
RULE_CTRL_Nil_Allocate_Rule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T fun_type,
    int rule_pri,
    UI32_T *dev_rule_id
);

static BOOL_T
RULE_CTRL_Nil_Destroy_Rule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T fun_type,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_Nil_Set_Rule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    DEVRM_MeterEntry_T *meter_entry,
    DEVRM_ActionEntry_T *action_entry
);

static BOOL_T
RULE_CTRL_Nil_Update_Rule(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    DEVRM_MeterEntry_T *meter_entry,
    DEVRM_ActionEntry_T *action_entry
);

static BOOL_T
RULE_CTRL_Nil_Create_Meter(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_MeterEntry_T *meter_entry
);

static BOOL_T
RULE_CTRL_Nil_Destroy_Meter(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_Nil_Share_Meter(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T src_dev_rule_id,
    UI32_T dev_rule_id
);

static BOOL_T
RULE_CTRL_Nil_Update_Meter(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_MeterEntry_T *meter_entry
);

static BOOL_T
RULE_CTRL_Nil_Set_Action(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T rule_id,
    DEVRM_ActionEntry_T *action_entry
);

static BOOL_T
RULE_CTRL_Nil_Set_Group_Qualify_Set(
    DEVRM_GroupEntry_T *group_p
);

static BOOL_T
RULE_CTRL_Nil_Set_Group_Priority(
    UI32_T group_id,
    int priority
);
#endif /* SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT */

static UI32_T
RULE_CTRL_LocalGetFirstDevRuleIDofOneClassMap(
    const RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p,
    UI32_T unit,
    UI32_T device_id
);

static RULE_CTRL_TcpUdpPortMapping_T* RULE_CTRL_GetTcpUdpPortMappingStatus(UI32_T port, UI8_T index);
static BOOL_T RULE_CTRL_LocalSetTcpUdpPortCosMap(BOOL_T enable, UI32_T unit, UI32_T port, BOOL_T is_tcp, UI16_T dest_port, UI8_T phb, UI8_T color);
static BOOL_T RULE_CTRL_LocalDeleteTcpUdpPortCosMap(UI32_T unit, UI32_T port, BOOL_T is_tcp, UI16_T dest_port, UI8_T phb, UI8_T color);
static RULE_CTRL_PortConfigStatus_T* RULE_CTRL_GetPortRatelimimtStatus(UI32_T port);
static BOOL_T RULE_CTRL_LocalSetRateLimit(UI32_T unit, UI32_T port, UI32_T k_bps);
static BOOL_T RULE_CTRL_LocalDeleteRateLimit(UI32_T unit, UI32_T port);
static BOOL_T RULE_CTRL_TrapPacket2CpuByUnit(UI32_T unit,
    BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info);
static BOOL_T RULE_CTRL_LocalCheckRuleInActiveByTimeRange(UI32_T ace_time_range_id, UI32_T acl_time_range_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_GetRuleQuota
 *------------------------------------------------------------------------------
 * PURPOSE:  Get rule quota by function type
 * INPUT:    func_type - Function type
 * OUTPUT:   None
 * RETURN:   Succeeded - Quota object pointer/Failed - NULL
 * NOTE:
 *------------------------------------------------------------------------------
 */
static RULE_CTRL_RuleQuota_T*
RULE_CTRL_GetRuleQuota(
    UI32_T unit,
    UI32_T device_id,
    RULE_TYPE_FunctionType_T func_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_CheckGroupResourceForAddRuleQuota
 *------------------------------------------------------------------------------
 * PURPOSE:  Check hware resource when add rule quota(function type) into a
 *           group
 * INPUT:    group_id         - Add rule quota into group
 *           group_total_rule - How many rule in the group
 *           func_type        - Function type
 *           rule_quota       - How many rule need for the function type
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_CheckGroupResourceForAddRuleQuota(
    UI32_T unit,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T group_total_rule,
    RULE_TYPE_FunctionType_T func_type,
    UI32_T rule_quota
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_AddRuleQuota
 *------------------------------------------------------------------------------
 * PURPOSE:  Add rule quota(function type) into a group
 * INPUT:    group_id         - Add rule quota into group
 *           group_total_rule - How many rule in the group
 *           func_type        - Function type
 *           rule_quota       - How many rule need for the function type
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_AddRuleQuota(
    UI32_T unit,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T group_total_rule,
    RULE_TYPE_FunctionType_T func_type,
    UI32_T rule_quota
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_RequestRuleByQuota
 *------------------------------------------------------------------------------
 * PURPOSE:  Request rule from the specified function type
 * INPUT:    func_type        - Function type
 *           allocate         - Request type
 *           rule_count       - How many rule need
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_RequestRuleByQuota(
    UI32_T unit,
    UI32_T device_id,
    RULE_TYPE_FunctionType_T fun_type,
    RULE_CTRL_RequestResourceType_T request_type,
    UI32_T rule_count
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_InitParam
 *------------------------------------------------------------------------------
 * PURPOSE:  Init RULE_CTRL_PARAM
 * INPUT:    param_p - Parameter
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_InitParam(
    RULE_CTRL_PARAM_PTR param_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_MakeParamTag
 *------------------------------------------------------------------------------
 * PURPOSE:  Set name of param
 * INPUT:    param_p    - param
 *           str1       - tag 1
 *           str2       - tag 2
 *           str3       - tag 3
 *           str4       - tag 4
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_MakeParamTag(
    RULE_CTRL_PARAM_PTR param_p,
    char *tag1,
    char *tag2,
    char *tag3,
    char *tag4
);

static char *
RULE_CTRL_GetLastTag(
    RULE_CTRL_PARAM_PTR param_p
);

static BOOL_T
RULE_CTRL_IsTagMatch(
    RULE_CTRL_PARAM_PTR param_p,
    char *tag
);

static BOOL_T
RULE_CTRL_RemoveRuleFromLookupTable(
    UI32_T unit,
    UI32_T port,
    UI32_T device_id,
    UI32_T group_id,
    UI32_T rule_id
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_CTRL_DumpParam
 *------------------------------------------------------------------------------
 * PURPOSE : Dump rule information
 * INPUT   : param_p     - rule information
 *           rule_printf - callback function for print
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_DumpParam(
    RULE_CTRL_PARAM_PTR param_p,
    BOOL_T (*rule_printf)(char*, ...)
);

static BOOL_T
RULE_CTRL_FindRuleLookupTable(
    RULE_CTRL_FindRuleLookupTable_T *find_p
);

static BOOL_T
RULE_CTRL_GetNextRuleEntryInLookupTable(
    RULE_CTRL_FindRuleLookupTable_T *find_p
);

static BOOL_T
RULE_CTRL_GetParamFromLookupEntry(
    const RULE_CTRL_RuleLookupEntry_T *rl_p,
    RULE_CTRL_PARAM_PTR param_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DO_SetSharedPortRule_CallBack
 *------------------------------------------------------------------------------
* PURPOSE:  Set single rule to port bitmap
 * INPUT:    param_p - Parameter
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_DO_SetSharedPortRule_CallBack(
    RULE_CTRL_PARAM_PTR param_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_DO_SetSharedPortRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Set single rule to port bitmap
 * INPUT:    param_p - Parameter
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_DO_SetSharedPortRule(
    RULE_CTRL_PARAM_PTR param_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_CTRL_SetSharedPortRule
 *------------------------------------------------------------------------------
 * PURPOSE:  This function is used to set rule for SRBP
 * INPUT:    param_p - Parameter
 *           get_om_ptr_handler - Function pointer, used to get OM
 *           cfg_rule_handler   - Function pointer, used to set RULE
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_CTRL_SetSharedPortRule(
    RULE_CTRL_PARAM_PTR param_p,
    RULE_CTRL_SPR_GET_OM_HANDLER get_om_ptr_handler,
    RULE_CTRL_SPR_SET_RULE_HANDLER set_rule_handler
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _rule_ctrl_set_rule
 *------------------------------------------------------------------------------
 * PURPOSE:  This function is used to set a rule(the rule will be set
 *           for one device only).
 * INPUT:    param_p, handler
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_rule_ctrl_set_rule(
    RULE_CTRL_PARAM_PTR param_p,
    RULE_CTRL_HANDLER handler
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _rule_ctrl_set_rule_for_all_devices
 *------------------------------------------------------------------------------
 * PURPOSE:  This function is used to set a global rule(the rule will be set
 *           for all devices).
 * INPUT:    param_p, handler
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_rule_ctrl_set_rule_for_all_devices(
    RULE_CTRL_PARAM_PTR param_p,
    RULE_CTRL_HANDLER handler
);
