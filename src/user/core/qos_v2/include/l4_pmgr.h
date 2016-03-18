/* FUNCTION NAME: L4_PMGR.C
 * PURPOSE:
 * NOTES:
 *
 *REASON:
 *    DESCRIPTION:
 *    CREATOR:
 *    Date       06/22/2007
 *
 * Copyright(C)      Accton Corporation, 2005
 */
#ifndef L4_PMGR_H
#define L4_PMGR_H

#include "l4_mgr.h"

/*------------------------------------------------------------------------------
 * ROUTINE NAME : L4_PMGR_Init
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Do initialization procedures for L4_PMGR.
 * INPUT:
 *    None.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    None.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
void L4_PMGR_Init(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_BindPort2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to policy map.
 * INPUT:    uport_ifindex, policy_map_name, inout_profile
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     One port only can bind one policy map.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_BindPort2PolicyMap(UI32_T uport_ifindex, const char *policy_map_name, RULE_TYPE_InOutDirection_T inout_profile);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_UnBindPortFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from policy map.
 * INPUT:    uport_ifindex, policy_map_name, inout_profile
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_UnBindPortFromPolicyMap(UI32_T uport_ifindex, const char *policy_map_name, RULE_TYPE_InOutDirection_T inout_profile);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetPolicyMapNameByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get binding policy map's name of port.
 * INPUT:    uport_ifinde
 * OUTPUT:   policy_map_name.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_GetPolicyMapNameByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    char *policy_map_name
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_BindPort2DynamicPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to dynamic policy map.
 * INPUT:    uport_ifindex, policy_map_name, inout_profile
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     One port only can bind one policy map.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_BindPort2DynamicPolicyMap(UI32_T uport_ifindex, const char *policy_map_name, RULE_TYPE_InOutDirection_T inout_profile);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_UnBindPortFromDynamicPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from dynamic policy map.
 * INPUT:    uport_ifindex, policy_map_name, inout_profile
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_UnBindPortFromDynamicPolicyMap(UI32_T uport_ifindex);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetRunningPolicyMapNameByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running binding policy map's name of port.
 * INPUT:    uport_ifindex      - ifindex
 *           inout_profile      - direction
 * OUTPUT:   policy_map_name    - name of the policy-map
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_QoS_GetRunningPolicyMapNameByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    char *policy_map_name
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetPolicyMapIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of policy map by it's name.
 * INPUT:    policy_map_name
 * OUTPUT:   policy_map_id.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_GetPolicyMapIdByName(const char *policy_map_name, UI32_T *policy_map_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_CreatePolicyMapByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a policy map entry by it's name
 * INPUT:    policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_CreatePolicyMapByName(const char *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_RenamePolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Rename a policy map
 * INPUT:    old_policy_map_name, new_policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_RenamePolicyMap(const char *old_policy_map_name, const char *new_policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetPolicyMapDescByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Set description of a policy map by it's name
 * INPUT:    policy_map
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     Key: policy_map_name.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_SetPolicyMapDescByName(const char *policy_map_name, const char *description);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_DelPolicyMapByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a policy map entry by it's name
 * INPUT:    policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_DelPolicyMapByName(const char *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's name
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map_index, policy_map.
 * RETURN:   Error code.
 * NOTE:     Key: policy_map_index.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_GetNextPolicyMap(UI32_T *policy_map_index, RULE_TYPE_UI_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextRunningPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running policy map entry.
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map_index, policy_map.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_QoS_GetNextRunningPolicyMap(UI32_T *policy_map_index, RULE_TYPE_UI_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_AddElement2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add a element to policy map
 * INPUT:    policy_map_name, class_map_name, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   RULE_TYPE_OK/RULE_TYPE_FAIL/
 *           RULE_TYPE_METER_FAIL/RULE_TYPE_ACTION_FAIL
 * NOTE:     if meter_entry != NULL, set meter_entry->row_status = L4_PMGR_ROWSTATUS_CREATEANDGO
 *           if action_entry != NULL, set action_entry->row_status = L4_PMGR_ROWSTATUS_CREATEANDGO
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_AddElement2PolicyMap(const char *policy_map_name, const char *class_map_name, RULE_TYPE_TBParamEntry_T *meter_entry, RULE_TYPE_UI_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_DelElementMeterFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete meter of element from policy map
 * INPUT:    policy_map_name, class_map_name, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     set meter_entry->row_status = L4_PMGR_ROWSTATUS_DESTROY
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_DelElementMeterFromPolicyMap(const char *policy_map_name, const char *class_map_name, RULE_TYPE_TBParamEntry_T *meter_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_DelElementActionFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete action of element from policy map
 * INPUT:    policy_map_name, class_map_name, action_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     set action_entry->row_status = L4_PMGR_ROWSTATUS_DESTROY
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_DelElementActionFromPolicyMap(const char *policy_map_name, const char *class_map_name, RULE_TYPE_UI_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_DelElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a element from policy map
 * INPUT:    policy_map_name, class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_DelElementFromPolicyMap(const char *policy_map_name, const char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next element of policy map
 * INPUT:    policy_map_index, element_entry
 * OUTPUT:   element_entry.
 * RETURN:   Error code.
 * NOTE:     Key: policy_map_index, element_entry->precedent
 *           element_entry->precedent = 0 to get first element
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_GetNextElementFromPolicyMap(UI32_T policy_map_index, RULE_TYPE_UI_PolicyElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextRunningElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete next running element of policy map
 * INPUT:    policy_map_index, element_entry
 * OUTPUT:   element_entry.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     Key: policy_map_index, element_entry->precedent
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_QoS_GetNextRunningElementFromPolicyMap(UI32_T policy_map_index, RULE_TYPE_UI_PolicyElement_T *element_entry);

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create/update/delete a bundle group
 * INPUT:    policy_map_name    - policy-map name
 *           index              - bundle group index
 *           bundle_group_p     - bundle group
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     Create/update a bundle by
 *             valid_status = TRUE and index is which entry want to create/update.
 *           Delete a bundle by
 *             valid_status = TRUE and index is which entry want to delete.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QoS_SetBundleGroup(
    const char *policy_map_name,
    RULE_TYPE_UI_BundleGroup_T *bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a bundle group
 * INPUT:    policy_map_index         - policy-map name
 *           ui_bundle_group_p->index - bundle index
 *
 * OUTPUT:   ui_bundle_group_p  - bundle group object
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QoS_GetBundleGroup(
    UI32_T policy_map_index,
    RULE_TYPE_UI_BundleGroup_T *bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next bundle group
 * INPUT:    policy_map_index         - policy-map name
 *           ui_bundle_group_p->index - bundle index
 *
 * OUTPUT:   policy_map_index   - policy-map name
 *           ui_bundle_group_p  - bundle group object
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QoS_GetNextBundleGroup(
    UI32_T *policy_map_index_p,
    RULE_TYPE_UI_BundleGroup_T *bundle_group_p
);

#endif /* #if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetClassMapIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of class map by it's name.
 * INPUT:    class_map_index
 * OUTPUT:   class_map_name.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_GetClassMapIdByName(const char *class_map_name, UI32_T *class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_CreateClassMapByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a class map
 * INPUT:    class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_CreateClassMapByName(const char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_RenameClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Rename a class map
 * INPUT:    old_class_map_name, new_class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_RenameClassMap(const char *old_class_map_name, const char *new_class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a class map
 * INPUT:    class_map
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     Caller need to set:
 *               --- class_map_name
 *               --- desc
 *               --- class_map_match_type
 *               --- row_status             = L4_PMGR_ROWSTATUS_CREATEANDGO
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_SetClassMap(RULE_TYPE_UI_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_DelClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a class map
 * INPUT:    class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_DelClassMap(const char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetClassMapByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a class map
 * INPUT:    class_map
 * OUTPUT:   class_map.
 * RETURN:   Error code.
 * NOTE:     Key: class_map->class_map_name
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_GetClassMapByName(RULE_TYPE_UI_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   Error code.
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_GetNextClassMap(UI32_T *class_map_index, RULE_TYPE_UI_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextRunningClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_QoS_GetNextRunningClassMap(UI32_T *class_map_index, RULE_TYPE_UI_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_AddElement2ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add a element to class map
 * INPUT:    class_map_name, element_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL / NOT_ALLOW_MIX_SELECTOR)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_AddElement2ClassMap(const char *class_map_name, RULE_TYPE_UI_ClassMapElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_DelElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a element from class map
 * INPUT:    class_map_name, element_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_DelElementFromClassMap(const char *class_map_name, RULE_TYPE_UI_ClassMapElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next element from class map
 * INPUT:    class_map_index, element_index
 * OUTPUT:   element_index, element_entry.
 * RETURN:   Error code.
 * NOTE:     Key:class_map_index, element_index
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_QoS_GetNextElementFromClassMap(UI32_T class_map_index, UI32_T *element_index, RULE_TYPE_UI_ClassMapElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextRunningElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running element from class map
 * INPUT:    class_map_index, element_index
 * OUTPUT:   element_index, element_entry.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     Key:class_map_index, element_index
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_QoS_GetNextRunningElementFromClassMap(UI32_T class_map_index, UI32_T *element_index, RULE_TYPE_UI_ClassMapElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_BindPort2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to ACL
 * INPUT:    uport_ifindex, acl_name, acl_type, ingress_flag, p_time_range_name
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     ingress_flag = FALSE implies egress
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_ACL_BindPort2Acl(
    UI32_T uport_ifindex,
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    BOOL_T ingress_flag,
    const char *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_UnBindPortFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from ACL
 * INPUT:    uport_ifindex, acl_name, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_UnBindPortFromAcl(UI32_T uport_ifindex, const char *acl_name, RULE_TYPE_AclType_T acl_type, BOOL_T ingress_flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_BindPort2DynamicAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to dynamic ACL
 * INPUT:    uport_ifindex, acl_name, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     1. One port only can bind one dynamic acl.
 *           2. The dynamic acl is got from authentication server.
 *           3. When apply dynamic acl, the user configured aclon this port
 *              should be disabled.
 *           4. If the port already bind a dynamic acl, then the bind
 *              or unbind action by user shall be effected after the dynamic
 *              acl be unbinded.
 *           5. The user acl should be restored to enable when dynamic acl be
 *              unbinded.
 *           6. The changes by this function should not be saved to configuration
 *              file.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_BindPort2DynamicAcl(UI32_T uport_ifindex, const char *acl_name, RULE_TYPE_AclType_T acl_type, BOOL_T ingress_flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_UnBindPortFromDynamicAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from dynamic ACL
 * INPUT:    uport_ifindex, acl_name, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     1. One port only can bind one dynamic acl.
 *           2. The dynamic acl is got from authentication server.
 *           3. When apply dynamic acl, the user configured aclon this port
 *              should be disabled.
 *           4. If the port already bind a dynamic acl, then the bind
 *              or unbind action by user shall be effected after the dynamic
 *              acl be unbinded.
 *           5. The user acl should be restored to enable when dynamic acl be
 *              unbinded.
 *           6. The changes by this function should not be saved to configuration
 *              file.
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_UnBindPortFromDynamicAcl(UI32_T uport_ifindex, RULE_TYPE_AclType_T acl_type, BOOL_T ingress_flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next binding ACL by port
 * INPUT:    uport_ifindex, acl_index, precedence
 * OUTPUT:   acl_index, acl_entry, precedence.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_GetNextAclByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI32_T *precedence,
    char *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextUIAclByPortAndBindingType
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next same binding type ACL by port, and binding type(ingress/egress ip/mac)
 * INPUT:    precedence, uport_ifindex
 * OUTPUT:   uport_ifindex, acl_entry, p_time_range_index.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_GetNextUIAclByPortAndBindingType(UI32_T precedence, UI32_T *uport_ifindex, RULE_TYPE_UI_AclEntry_T *acl_entry, char *p_time_range_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextRunningAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running binding ACL by port
 * INPUT:    uport_ifindex, acl_entry, precedence
 * OUTPUT:   uport_ifindex, acl_entry, precedence, p_time_range_index.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_PMGR_ACL_GetNextRunningAclByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI32_T *precedence,
    char *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetAclIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name.
 * INPUT:    acl_name
 * OUTPUT:   acl_index.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_GetAclIdByName(const char *acl_name, UI32_T *acl_index);


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetAclIdByNameAndType
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name and type.
 * INPUT:    acl_name, acl_type
 * OUTPUT:   acl_index.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_GetAclIdByNameAndType(const char *acl_name, RULE_TYPE_AclType_T acl_type, UI32_T *acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_CreateAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Create an ACL.
 * INPUT:    acl_name, acl_type
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_CreateAcl(const char *acl_name, RULE_TYPE_AclType_T acl_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_DelAclByNameAndAclType
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete an ACL by name and acl_type
 * INPUT:    acl_name, acl_type
 * OUTPUT:   NONE
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_DelAclByNameAndAclType(const char *acl_name, RULE_TYPE_AclType_T acl_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACL.
 * INPUT:    acl_entry
 * OUTPUT:   acl_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_GetNextAcl(UI32_T *acl_index, RULE_TYPE_UI_AclEntry_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextRunningAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running ACL.
 * INPUT:    acl_entry
 * OUTPUT:   acl_entry.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_ACL_GetNextRunningAcl(UI32_T *acl_index, RULE_TYPE_UI_AclEntry_T *acl_entry);

/*------------------------------------------------------------------------------
 * FUNCTION NAME -L4_PMGR_ACL_InitAce
 *------------------------------------------------------------------------------
 * PURPOSE  : for SNMP and WWW and CLI to make a default ACE
 * INPUT    : ace_entry->ace_type
 * OUTPUT   : ace_entry
 * RETURN   : Error code
 * NOTES    : this API is used to init the ace before use it, need to set ace_entry->ace_type
 *------------------------------------------------------------------------------*/
/* wuli??*/
UI32_T L4_PMGR_ACL_InitAce(RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_PMGR_ACL_InitUIAce
 *------------------------------------------------------------------------------
 * PURPOSE  : for UI to make a default UI ACE entry
 * INPUT    : ace_entry_p->ace_type
 * OUTPUT   : ace_entry_p
 * RETURN   : Error code
 * NOTES    : this API is used to init the ace before use it
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_ACL_InitUIAce(
    RULE_TYPE_UI_Ace_Entry_T *ace_entry_p
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_PMGR_ACL_SetAcePatternToDefaultByType
 *------------------------------------------------------------------------------
 * PURPOSE  : For UI to initial ace.u.{mac|ip|ipv6|arp} to default
 * INPUT    : ace_type
 * OUTPUT   : ace_entry_p
 * RETURN   : Error code
 * NOTES    : Initial the ace pattern by ace_type
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_ACL_SetAcePatternToDefaultByType(
    RULE_TYPE_Ace_Entry_T *ace_entry,
    RULE_TYPE_AclType_T ace_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_SetUIAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a UI ACE to ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_ACL_SetUIAce2Acl(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_UI_Ace_Entry_T *ace_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_DelUIAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a UI ACE from ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_ACL_DelUIAceFromAcl(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_UI_Ace_Entry_T *ace_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next UI ACE from ACL.
 * INPUT:    acl_index, ace_index_p
 * OUTPUT:   ace_index_p, ace_p
 * RETURN:   Error code.
 * NOTE:     Key: acl_index, ace_index
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_ACL_GetNextUIAceByAcl(
    UI32_T acl_index,
    UI32_T *ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextHardwareUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next hardware UI ACE from ACL.
 * INPUT:    acl_index, acl_type, ace_index
 * OUTPUT:   ace_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_ACL_GetNextHardwareUIAceByAcl(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T acl_index,
    UI32_T *ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextRunningUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running UI ACE from ACL.
 * INPUT:    acl_index, ace_index_p, ace_entry
 * OUTPUT:   ace_entry.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_PMGR_ACL_GetNextRunningUIAceByAcl(
    UI32_T acl_index,
    UI32_T *ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_SetAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a ACE to ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_SetAce2Acl(const char *acl_name, RULE_TYPE_AclType_T acl_type, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_DelAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a ACE from ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_DelAceFromAcl(const char *acl_name, RULE_TYPE_AclType_T acl_type, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE from ACL.
 * INPUT:    acl_index, ace_index, ace_entry
 * OUTPUT:   ace_entry.
 * RETURN:   Error code.
  * NOTE:     Key: acl_index, ace_index
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_ACL_GetNextAceByAcl(UI32_T acl_index, UI32_T *ace_index, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextHardwareAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE from ACL.
 * INPUT:    acl_index, ace_index, ace_entry
 * OUTPUT:   ace_entry.
 * RETURN:   Error code.
 * NOTE:     Key: acl_index, ace_index
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_ACL_GetNextHardwareAceByAcl(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T acl_index,
    UI32_T *ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextMibAceByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE by port
 * INPUT:    ifindex       - interface index
 *           inout_profile - direction
 *           acl_index     - user config ACL index
 *           ace_index     - hardware ACE index
 * OUTPUT:   ace_entry     - ACE entry
 * RETURN:   Error code.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_ACL_GetNextMibAceByPort(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index_p,
    UI32_T *ace_index_p,
    RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable_p
);

#if (SYS_CPNT_ACL_COUNTER == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_ClearAclCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Clear ACL counter
 * INPUT:    acl_name   - ACL name
 *                        If this parameter is zero string, clear counter for all
 *                        ACLs.
 *                        If this parameter is zero string, ignores ace_index.
 *           ace_index  - ACE index.
 *                        If this parameter is 0xffffffff, clear counter for all
 *                        ACEs of the specified ACL.
 *           interface  - The interface.
 *                        This parameter can be one of the following values,
 *                        RULE_TYPE_INTERFACE_LPORT     - One port
 *                        RULE_TYPE_INTERFACE_UPORT     - One port
 *                        RULE_TYPE_INTERFACE_ALL_UNIT  - All port
 * OUTPUT:   none
 * RETURN:   Error code.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_ACL_ClearAclCounter(
    const char *acl_name,
    UI32_T ace_index,
    RULE_TYPE_INTERFACE_INFO_T *interface
);
#endif /* #if (SYS_CPNT_ACL_COUNTER == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_AddAclMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Add ACL mirror entry
 * INPUT:    ifindex_dest   - Destination port
 *           acl_name       - Source ACL name
 * OUTPUT:   NONE
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_ACL_AddAclMirrorEntry(
    UI32_T ifindex_dest,
    const char *acl_name
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_RemoveAclMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Remove ACL mirror entry
 * INPUT:    ifindex_dest   - Destination port
 *           acl_name       - Source ACL name
 * OUTPUT:   NONE
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_ACL_RemoveAclMirrorEntry(
    UI32_T ifindex_dest,
    const char *acl_name
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextAclMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  GetNext ACL mirror entry
 * INPUT:    ifindex_dest   - Destination port. Pass 0 to start.
 *           acl_index      - ACL index
 * OUTPUT:   acl_entry      - ACL entry
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_ACL_GetNextAclMirrorEntry(
    UI32_T *ifindex_dest,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_ACL_GetNextAclMirrorEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  GetNext ACL mirror entry
 * INPUT:    ifindex_dest   - Destination port. Pass 0 to start.
 *           acl_index      - ACL index
 * OUTPUT:   acl_entry      - ACL entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_PMGR_ACL_GetNextRunningAclMirrorEntry(
    UI32_T *ifindex_dest,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_GetTCAMUtilization
 *------------------------------------------------------------------------------
 * PURPOSE :  get total pce, free_pce counts, and utilization
 * INPUT   :  none
 * OUTPUT  :  tcam_util_p
 * RETURN  :  True/False
 * NOTE    :  Shall not using anymore.
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_GetTCAMUtilization(RULE_TYPE_TCAMUtilization_T *tcam_util_p);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_PMGR_GetTcamEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get tcam entry
 * INPUT    : tcam_entry_p->unit
 *            tcam_entry_p->device_id
 *            tcam_entry_p->pool_id
 * OUTPUT   : tcam_util_p
 * RETURN   : TRUE/FALSE.
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_GetTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_PMGR_GetNextTcamEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : Get next tcam entry
 * INPUT    : tcam_entry_p->unit
 *            tcam_entry_p->device_id
 *            tcam_entry_p->pool_id
 * OUTPUT   : tcam_util_p
 * RETURN   : TRUE/FALSE.
 * NOTE     : For get first entry,
 *            use index with tcam_entry_p->unit = 0
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_GetNextTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_GetSupportedTcamCapabilityBitMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets TCAM all supporting capability
 * INPUT    :
 * OUTPUT   : bit_map
 * RETURN   : Error code.
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_GetSupportedTcamCapabilityBitMap(
    RULE_TYPE_TCAM_CAP_BITMAP_T *bit_map
);

/* ========================================================================== */
#if (SYS_CPNT_COS == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_SetIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will enable or disable TOS/DSCP/COS mapping of system
 * INPUT:    flag -- COS_VM_ipprecedence_MAPPING_ENABLED/COS_VM_DSCP_MAPPING_ENABLED
 *                   /COS_VM_DISABLE_BOTH
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:  TOS/COS mapping and DSCP/COS mapping can not be enabled simultaneously
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_SetIpPrecedenceDscpMappingStatus (COS_VM_MAPPING_FLAG_T flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_GetIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get TOS/DSCP/COS mapping status of system
 * INPUT:    none
 * OUTPUT:   flag -- COS_PMGR_ipprecedence_MAPPING_ENABLED/COS_PMGR_DSCP_MAPPING_ENABLED
 *                   /COS_PMGR_DISABLE_BOTH
 * RETURN:   TRUE/FALSE
 * NOTE:  tos/COS mapping and DSCP/COS mapping can not be enabled simultaneously
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_GetIpPrecedenceDscpMappingStatus (COS_VM_MAPPING_FLAG_T *flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_GetRunningIpPrecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of tos/DSCP/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_PMGR_MAPPING_ENABLED/COS_PMGR_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE: default value is COS_PMGR_MAPPING_DISABLED
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_COS_GetRunningIpPrecedenceDscpMappingStatus(COS_VM_MAPPING_FLAG_T *flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_SetTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will enable or disable TcpPort/COS mapping of system
 * INPUT:    flag -- COS_PMGR_MAPPING_ENABLED/COS_PMGR_MAPPING_DISABLED
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_SetTcpPortMappingStatus (UI32_T flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_GetTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of TcpPort/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_PMGR_MAPPING_ENABLED/COS_PMGR_MAPPING_DISABLED
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_GetTcpPortMappingStatus(UI32_T *flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_GetRunningTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of TcpPort/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_PMGR_MAPPING_ENABLED/COS_PMGR_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE: default value is COS_PMGR_MAPPING_DISABLED
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_COS_GetRunningTcpPortMappingStatus(COS_VM_TCPPORT_MAPPING_FLAG_T *flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_SetIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a tos/CoS map in tos_map_table of specific
 *           logical port
 * INPUT:    l_port, tos (0..7), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_SetIpPrecedenceToCos(UI32_T l_port, UI8_T tos, UI32_T cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_GetIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a tos/CoS map in tos_map_table of specific
 *           logical port
 * INPUT:    l_port, tos (0..7)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_GetIpPrecedenceToCos(UI32_T l_port, UI8_T tos, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_DelIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tos/CoS map to its default value
 *           in tos_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_DelIpPrecedenceToCos(UI32_T l_port, UI8_T tos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_DelIpPrecedenceLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tos/CoS map to its default value
 *           in tos_map_table of specific logical port
 * INPUT:    l_port, tos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_DelIpPrecedenceLportAllEntry(UI32_T l_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_SetDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a Dscp/CoS map in dscp_map_table of specific
 *           logical port
 * INPUT:    l_port, dscp (0..63), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_SetDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_PMGR_COS_SetDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: this function will set a dscp entry's status
 * INPUT:   l_port
 *          dscp, cos status
 * OUTPUT:
 * RETURN:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_SetDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_PMGR_COS_GetDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function will get a dscp entry's status
 * INPUT:   l_port
 *          dscp
 * OUTPUT:  cos status
 * RETURN:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_GetDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T *status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_PMGR_COS_GetRunningDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function will get a running dscp entry's status
 * INPUT:    l_port, dscp
 * OUTPUT:   status -- COS_PMGR_MAPPING_ENABLED/COS_PMGR_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_COS_GetRunningDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T *status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_GetDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a Dscp/CoS map in dscp_map_table of specific
 *           logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_GetDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_DelDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a Dscp/CoS map to its default value
 *           in dscp_map_table of specific logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_DelDSCPToCos(UI32_T l_port, UI8_T dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_DelDscpLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a Dscp/CoS map to its default value
 *           in dscp_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_DelDscpLportAllEntry(UI32_T l_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_SetPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a TCP Port/CoS map in tcp_port_map_table
 *           of specific logical port
 * INPUT:    l_port, tcp port (1..65535), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE: 1. For GetNext functions, the entries in table need to be sorted
 *          according to tcp_port number
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_SetPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_GetPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a tcp_port/CoS map in tcp_port_map_table of specific
 *           logical port
 * INPUT:    l_port, tcp_port (0..65535)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_GetPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_DelPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tcp_port/CoS map to its default value
 *           in tcp_port of specific logical port
 * INPUT:    l_port, tcp_port (0..65535)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_DelPortToCos(UI32_T l_port, UI16_T tcp_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_GetNextIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in tos_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           tos -- tos index
 *           cos -- cos value
 * OUTPUT:   l_port, tos -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_GetNextIpPrecedenceToCos(UI32_T *l_port, UI8_T *tos, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_GetNextDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in dscp_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           dscp -- dscp index
 *           cos -- cos value
 * OUTPUT:   l_port, dscp -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_GetNextDSCPToCos(UI32_T *l_port, UI8_T *dscp, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_GetNextPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in tcp_port_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           tcp_port -- dscp index
 *           cos -- cos value
 * OUTPUT:   l_port, tcp_port -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_GetNextPortToCos(UI32_T *l_port, I32_T *tcp_port, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_PMGR_COS_GetRunningIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default tos/COS mapping is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT:   l_port
 *          tos
 * OUTPUT:  cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_COS_GetRunningIpPrecedenceToCos(UI32_T l_port, UI8_T tos, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_PMGR_COS_GetRunningDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default DSCP/COS mapping is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT:   l_port
 *          tos
 * OUTPUT:  cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_COS_GetRunningDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_PMGR_COS_GetRunningPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default TCPPort/COS mapping is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT:   l_port
 *          tos
 * OUTPUT:  cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_COS_GetRunningPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_PMGR_COS_GetNextRunningIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function same as GetRunning but also output next index
 * INPUT:   l_port
 *          tos
 * OUTPUT:  l_port
 *          tos
 *          cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_COS_GetNextRunningIpPrecedenceToCos(UI32_T *l_port , UI8_T *tos, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_PMGR_COS_GetNextRunningDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function same as GetRunning but also output next index
 * INPUT:   l_port
 *          dscp
 * OUTPUT:  l_port
 *          dscp
 *          cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_COS_GetNextRunningDSCPToCos(UI32_T *l_port, UI8_T *dscp, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_PMGR_COS_GetNextRunningPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function same as GetRunning but also output next index
 * INPUT:   l_port
 *          tcp_port
 * OUTPUT:  l_port
 *          tcp_port
 *          cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_COS_GetNextRunningPortToCos(UI32_T *l_port, UI16_T *tcp_port, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_PortCopy
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will copy the source port set to destport
 * INPUT:    map_table_type, src_port, dst_port_list
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE: Since it have to support multi-thread, so it is not possible to separate
 *       parameters configuration from copy operation
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_PortCopy(UI32_T map_table_type, UI32_T src_port, UI8_T *dst_port_list);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_AddCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    ifindex, acl name, cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is cos_entry.acl_name
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_AddCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_DelCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    ifindex, acl name, cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is cos_entry.acl_name
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_DelCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_SetCosEntryRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    ifindex, cos_entry
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is (ifindex, cos_entry.acl_name)
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_SetCosEntryRowStatus(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_SetCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    ifindex, cos_entry
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is (ifindex, cos_entry.acl_name)
 *
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_SetCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_GetCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL to cos mapping
 * INPUT:    ifindex,
 * OUTPUT:   acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     the key is (ifindex, acl_mapping->acl_name)
 *           supposely only the SNMP will call this fuction
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_GetCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_GetNextCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  for CLI and WWW to get the next ACL to cos
 * INPUT:    ifindex,
 * OUTPUT:   acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     use the *ifidnex, acl_mapping->acl_name[0]='\0' to get the first entry
 *           the key is (*ifindex, acl_mapping->acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_GetNextCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_COS_SNMPGetNextCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to get the next acl map to cos
 * INPUT:    ifindex
 * OUTPUT:   ifindex, acl_mapping
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     use the *ifidnex, acl_mapping->acl_name[0]='\0' to get the first entry
 *           the key is (*ifindex, acl_mapping->acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_COS_SNMPGetNextCosEntry(UI32_T *ifindex, COS_TYPE_AclCosEntry_T *cos_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  -L4_PMGR_COS_GetNextRunningCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  for CLI to get the port acl binding
 * INPUT  :  if_index, entry
 * OUTPUT :  entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES  :  use the ifidnex, acl_mapping->acl_name[0]='\0' to get the first entry
 *           the key is (ifindex, acl_mapping->acl_name)
 *
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T L4_PMGR_COS_GetNextRunningCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry );
#endif //#if (SYS_CPNT_COS == TRUE)
/* ========================================================================== */


#if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_MARKER_SetMarkerEntry_Dscp
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    ifindex          -- is one of the search key
 *           marker_entry  --
 *
 * OUTPUT :
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (intf, marker_entry->acl_name)
 *           SNMP do not need to care of marker_entry->acl_type
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_MARKER_SetMarkerEntry_Dscp(UI32_T ifindex, UI8_T acl_name[], UI32_T value);

#endif  /* #if(SYS_CPNT_QOS == SYS_CPNT_QOS_MARKER) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get port entry by ifindex
 * INPUT    : ifindex
 *            inout_profile - direction
 * OUTPUT   : port_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */

BOOL_T L4_PMGR_QoS_GetDiffServPortEntry(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    RULE_TYPE_UI_PortEntry_T *port_entry
    );

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextDiffServPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next port entry by ifindex
 * INPUT    : ifindex
 * OUTPUT   : ifindex, inout_profile, port_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetNextDiffServPortEntry(
    UI32_T *ifindex,
    RULE_TYPE_InOutDirection_T *inout_profile,
    RULE_TYPE_UI_PortEntry_T *port_entry
    );

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServPortPolicyMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to policy-map(policy_map_index)
 * INPUT    : ifindex, inout_profile, policy_map_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServPortPolicyMapIndex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index
    );

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServPortIngressIpAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to ingress IP ACL(acl_index)
 * INPUT    : ifindex, acl_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServPortIngressIpAclIndex(UI32_T ifindex, UI32_T acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServPortIngressIpv6AclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to ingress IPv6 ACL(acl_index)
 * INPUT    : ifindex, acl_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServPortIngressIpv6AclIndex(UI32_T ifindex, UI32_T acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServPortIngressMacAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : bind port(ifindex) to ingress MAC ACL(acl_index)
 * INPUT    : ifindex, acl_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServPortIngressMacAclIndex(UI32_T ifindex, UI32_T acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServPolicyMapEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map by policy_map_index
 * INPUT    : policy_map_index
 * OUTPUT   : policy_map
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetDiffServPolicyMapEntry(UI32_T policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextDiffServPolicyMapEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next policy-map by policy_map_index
 * INPUT    : policy_map_index
 * OUTPUT   : policy_map_index, policy_map
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetNextDiffServPolicyMapEntry(UI32_T *policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServPolicyMapName
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map name by policy_map_index
 * INPUT    : policy_map_index, policy_map_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServPolicyMapName(UI32_T policy_map_index, const char *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServPolicyMapDescription
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map description by policy_map_index
 * INPUT    : policy_map_index, policy_map_desc
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServPolicyMapDescription(UI32_T policy_map_index, const char *policy_map_desc);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServPolicyMapStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map row status by policy_map_index
 * INPUT    : policy_map_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServPolicyMapStatus(UI32_T policy_map_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetPolicydiffServPolicyMapAttachCtlAction
 *------------------------------------------------------------------------------
 * PURPOSE  : dissociate/associate policy-map and policy-map element
 * INPUT    : policy_map_index, policy_element_index, attach_action
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : attach_action (1:notCombining,2:Combine,3:Discombine)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetPolicydiffServPolicyMapAttachCtlAction(UI32_T policy_map_index, UI32_T policy_element_index, UI32_T attach_action);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServPolicyMapElementEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map element by policy_element_index
 * INPUT    : policy_element_index
 * OUTPUT   : policy_element
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetDiffServPolicyMapElementEntry(UI32_T policy_element_index, RULE_TYPE_PolicyElement_T *policy_element);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextDiffServPolicyMapElementEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next policy-map element by policy_element_index
 * INPUT    : policy_element_index
 * OUTPUT   : policy_element_index, policy_element
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetNextDiffServPolicyMapElementEntry(UI32_T *policy_element_index, RULE_TYPE_PolicyElement_T *policy_element);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServPolicyMapElementClassMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map index by policy_element_index
 * INPUT    : policy_element_index, class_map_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServPolicyMapElementClassMapIndex(UI32_T policy_element_index, UI32_T class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServPolicyMapElementMeterIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter index by policy_element_index
 * INPUT    : policy_element_index, meter_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServPolicyMapElementMeterIndex(UI32_T policy_element_index, UI32_T meter_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServPolicyMapElementActionIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : change action index by policy_element_index
 * INPUT    : policy_element_index, action_index
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServPolicyMapElementActionIndex(UI32_T policy_element_index, UI32_T action_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServPolicyMapElementStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map element row status by policy_element_index
 * INPUT    : policy_element_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServPolicyMapElementStatus(UI32_T policy_element_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServClassMapEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get class-map by class_map_index
 * INPUT    : class_map_index
 * OUTPUT   : class_map
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetDiffServClassMapEntry(UI32_T class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextDiffServClassMapEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next class-map by class_map_index
 * INPUT    : class_map_index
 * OUTPUT   : class_map_index, class_map
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetNextDiffServClassMapEntry(UI32_T *class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServClassMapName
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map name by class_map_index
 * INPUT    : class_map_index, class_map_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServClassMapName(UI32_T class_map_index, const char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServClassMapDescription
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map description by class_map_index
 * INPUT    : class_map_index, class_map_desc
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServClassMapDescription(UI32_T class_map_index, const char *class_map_desc);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServClassMapMatchType
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map matching type by class_map_index
 * INPUT    : class_map_index, match_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServClassMapMatchType(UI32_T class_map_index, UI32_T match_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServPolicyMapStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change class-map row status by class_map_index
 * INPUT    : class_map_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServClassMapStatus(UI32_T class_map_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServClassMapAttachCtlAction
 *------------------------------------------------------------------------------
 * PURPOSE  : dissociate/associate class-map and class-map element
 * INPUT    : class_map_index, index_type, class_element_index, attach_action
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : index_type(??)
 *            attach_action (1:notCombining,2:Combine,3:Discombine)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServClassMapAttachCtlAction(UI32_T class_map_index, UI32_T index_type, UI32_T class_element_index, UI32_T attach_action);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServAclEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get acl by acl_index
 * INPUT    : acl_index
 * OUTPUT   : acl_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetDiffServAclEntry(UI32_T acl_index, RULE_TYPE_MIB_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextDiffServAclEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next acl by acl_index
 * INPUT    : acl_index
 * OUTPUT   : acl_index, acl_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetNextDiffServAclEntry(UI32_T *acl_index, RULE_TYPE_MIB_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServAclType
 *------------------------------------------------------------------------------
 * PURPOSE  : change ACL type by acl_index
 * INPUT    : acl_index, acl_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServAclType(UI32_T acl_index, UI32_T acl_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServAclName
 *------------------------------------------------------------------------------
 * PURPOSE  : change policy-map name by acl_index
 * INPUT    : acl_index, acl_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServAclName(UI32_T acl_index, const UI8_T *acl_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServAclStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change acl row status by acl_index
 * INPUT    : acl_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServAclStatus(UI32_T acl_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServAclAttachCtlAction
 *------------------------------------------------------------------------------
 * PURPOSE  : dissociate/associate class-map and class-map element
 * INPUT    : acl_index, ace_type, ace_index, attach_action
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : attach_action (1:notCombining,2:Combine,3:Discombine)
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServAclAttachCtlAction(UI32_T acl_index, UI32_T ace_type, UI32_T ace_index, UI32_T attach_action);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServIpAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get ace by ip_ace_index
 * INPUT    : ip_ace_index
 * OUTPUT   : ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_GetDiffServIpAceEntry(
    UI32_T ip_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextDiffServIpAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next ace by ip_ace_index
 * INPUT    : ip_ace_index
 * OUTPUT   : ip_ace_index, ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_GetNextDiffServIpAceEntry(
    UI32_T *ip_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceType
 *------------------------------------------------------------------------------
 * PURPOSE  : Set ace type by ip_ace_index
 * INPUT    : ip_ace_index, mib_ace_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceType(UI32_T ip_ace_index, UI32_T mib_ace_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceAccess
 *------------------------------------------------------------------------------
 * PURPOSE  : change access type by ip_ace_index
 * INPUT    : ip_ace_index, access_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceAccess(UI32_T ip_ace_index, UI32_T access_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceSourceIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by ip_ace_index
 * INPUT    : ip_ace_index, src_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceSourceIpAddr(UI32_T ip_ace_index, UI32_T src_ip);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceSourceIpAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip mask by ip_ace_index
 * INPUT    : ip_ace_index, src_ip_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceSourceIpAddrBitmask(UI32_T ip_ace_index, UI32_T src_ip_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceDestIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip by ip_ace_index
 * INPUT    : ip_ace_index, dst_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceDestIpAddr(UI32_T ip_ace_index, UI32_T dst_ip);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceDestIpAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip mask by ip_ace_index
 * INPUT    : ip_ace_index, dst_ip_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceDestIpAddrBitmask(UI32_T ip_ace_index, UI32_T dst_ip_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceProtocol
 *------------------------------------------------------------------------------
 * PURPOSE  : change protocol_type by ip_ace_index
 * INPUT    : ip_ace_index, protocol_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceProtocol(UI32_T ip_ace_index, UI32_T protocol_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAcePrec
 *------------------------------------------------------------------------------
 * PURPOSE  : change ip precedence by ip_ace_index
 * INPUT    : ip_ace_index, ip_precedence
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAcePrec(UI32_T ip_ace_index, UI32_T ip_precedence);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceTos
 *------------------------------------------------------------------------------
 * PURPOSE  : change tos by ip_ace_index
 * INPUT    : ip_ace_index, tos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceTos(UI32_T ip_ace_index, UI32_T tos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dscp by ip_ace_index
 * INPUT    : ip_ace_index, dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceDscp(UI32_T ip_ace_index, UI32_T dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceSourcePortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change source port operator by ip_ace_index
 * INPUT    : ip_ace_index, src_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceSourcePortOp(UI32_T ip_ace_index, UI32_T src_port_operator);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceMinSourcePort
 *------------------------------------------------------------------------------
 * PURPOSE  : change min src port by ip_ace_index
 * INPUT    : ip_ace_index, min_src_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceMinSourcePort(UI32_T ip_ace_index, UI32_T min_src_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceMaxSourcePort
 *------------------------------------------------------------------------------
 * PURPOSE  : change max src port by ip_ace_index
 * INPUT    : ip_ace_index, max_src_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceMaxSourcePort(UI32_T ip_ace_index, UI32_T max_src_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceSourcePortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port mask by ip_ace_index
 * INPUT    : ip_ace_index, src_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceSourcePortBitmask(UI32_T ip_ace_index, UI32_T src_port_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceDestPortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port operator by ip_ace_index
 * INPUT    : ip_ace_index, dst_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceDestPortOp(UI32_T ip_ace_index, UI32_T dst_port_operator);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceMinDestPort
 *------------------------------------------------------------------------------
 * PURPOSE  : change min dst port by ip_ace_index
 * INPUT    : ip_ace_index, min_dst_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceMinDestPort(UI32_T ip_ace_index, UI32_T min_dst_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceMaxDestPort
 *------------------------------------------------------------------------------
 * PURPOSE  : change max dst port by ip_ace_index
 * INPUT    : ip_ace_index, max_dst_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceMaxDestPort(UI32_T ip_ace_index, UI32_T max_dst_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceDestPortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port mask by ip_ace_index
 * INPUT    : ip_ace_index, dst_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceDestPortBitmask(UI32_T ip_ace_index, UI32_T dst_port_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceControlCode
 *------------------------------------------------------------------------------
 * PURPOSE  : change control_code by ip_ace_index
 * INPUT    : ip_ace_index, control_code
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceControlCode(UI32_T ip_ace_index, UI32_T control_code);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceControlCodeBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change control_code mask by ip_ace_index
 * INPUT    : ip_ace_index, control_code_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceControlCodeBitmask(UI32_T ip_ace_index, UI32_T control_code_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change ip ace row_status by ip_ace_index
 * INPUT    : ip_ace_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpAceStatus(UI32_T ip_ace_index, UI32_T row_status);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpAceTimeRange
 *------------------------------------------------------------------------------
 * PURPOSE  : change ip ace time_range by ip_ace_index
 * INPUT    : ip_ace_index, time_range_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServIpAceTimeRange(
    UI32_T ip_ace_index,
    UI8_T *time_range_name
);
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServMacAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get ace by mac_ace_index
 * INPUT    : mac_ace_index
 * OUTPUT   : ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QoS_GetDiffServMacAceEntry(
    UI32_T mac_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextDiffServMacAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next mac ace by mac_ace_index
 * INPUT    : mac_ace_index_p
 * OUTPUT   : mac_ace_index_p, ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QoS_GetNextDiffServMacAceEntry(
    UI32_T *mac_ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceAccess
 *------------------------------------------------------------------------------
 * PURPOSE  : change access type by mac_ace_index
 * INPUT    : mac_ace_index, access_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceAccess(UI32_T mac_ace_index, UI32_T access_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAcePktformat
 *------------------------------------------------------------------------------
 * PURPOSE  : change packet format by mac_ace_index
 * INPUT    : mac_ace_index, pkt_format
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAcePktformat(UI32_T mac_ace_index, UI32_T pkt_format);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceSourceMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src mac by mac_ace_index
 * INPUT    : mac_ace_index, src_mac
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceSourceMacAddr(UI32_T mac_ace_index, const UI8_T *src_mac);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceSourceMacAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src mac mask by mac_ace_index
 * INPUT    : mac_ace_index, src_mac_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceSourceMacAddrBitmask(UI32_T mac_ace_index, const UI8_T *src_mac_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceDestMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst mac by mac_ace_index
 * INPUT    : mac_ace_index, dst_mac
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceDestMacAddr(UI32_T mac_ace_index, const UI8_T *dst_mac);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceDestMacAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst mac mask by mac_ace_index
 * INPUT    : mac_ace_index, dst_mac_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceDestMacAddrBitmask(UI32_T mac_ace_index, const UI8_T *dst_mac_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceVidOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change vid operator by mac_ace_index
 * INPUT    : mac_ace_index, vid_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceVidOp(UI32_T mac_ace_index, UI32_T vid_operator);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceMinVid
 *------------------------------------------------------------------------------
 * PURPOSE  : change min vid by mac_ace_index
 * INPUT    : mac_ace_index, min_vid
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceMinVid(UI32_T mac_ace_index, UI32_T min_vid);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceVidBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change vid mask by mac_ace_index
 * INPUT    : mac_ace_index, vid_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceVidBitmask(UI32_T mac_ace_index, UI32_T vid_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceMaxVid
 *------------------------------------------------------------------------------
 * PURPOSE  : change max vid by mac_ace_index
 * INPUT    : mac_ace_index, max_vid
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceMaxVid(UI32_T mac_ace_index, UI32_T max_vid);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceEtherTypeOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change ether-type operator by mac_ace_index
 * INPUT    : mac_ace_index, ether_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceEtherTypeOp(UI32_T mac_ace_index, UI32_T ether_operator);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceEtherTypeBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change ether-type mask by mac_ace_index
 * INPUT    : mac_ace_index, ether_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceEtherTypeBitmask(UI32_T mac_ace_index, UI32_T ether_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceMinEtherType
 *------------------------------------------------------------------------------
 * PURPOSE  : change min ether-type by mac_ace_index
 * INPUT    : mac_ace_index, min_ether_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceMinEtherType(UI32_T mac_ace_index, UI32_T min_ether_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceMaxEtherType
 *------------------------------------------------------------------------------
 * PURPOSE  : change max ether-type by mac_ace_index
 * INPUT    : mac_ace_index, max_ether_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceMaxEtherType(UI32_T mac_ace_index, UI32_T max_ether_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change row status by mac_ace_index
 * INPUT    : mac_ace_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceStatus(UI32_T mac_ace_index, UI32_T row_status);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceTimeRange
 *------------------------------------------------------------------------------
 * PURPOSE  : change ace time_range by mac_ace_index
 * INPUT    : mac_ace_index, time_range_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceTimeRange(
    UI32_T mac_ace_index,
    UI8_T *time_range_name
);
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceSourceIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by mac_ace_index
 * INPUT    : mac_ace_index, addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceSourceIpAddr(
    UI32_T mac_ace_index,
    UI32_T addr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceSourceIpAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip mask by mac_ace_index
 * INPUT    : ip_ace_index, addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceSourceIpAddrBitmask(
    UI32_T mac_ace_index,
    UI32_T addr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceDestIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dest ip by mac_ace_index
 * INPUT    : mac_ace_index, addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceDestIpAddr(
    UI32_T mac_ace_index,
    UI32_T addr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceDestIpAddrBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dest ip mask by mac_ace_index
 * INPUT    : ip_ace_index, addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceDestIpAddrBitmask(
    UI32_T mac_ace_index,
    UI32_T addr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMaceAceSourceIpv6Addr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by ipv6_ace_index
 * INPUT    : mac_ace_index, src_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMaceAceSourceIpv6Addr(
    UI32_T mac_ace_index,
    const UI8_T *src_ip
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceSourceIpv6AddrPrefix
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip prefix length by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_ip_prefix
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceSourceIpv6AddrPrefix(
    UI32_T mac_ace_index,
    UI32_T src_ip_prefix
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceDestIpv6Addr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip by mac_ace_index
 * INPUT    : ipv6_ace_index, dst_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceDestIpv6Addr(
    UI32_T mac_ace_index,
    const UI8_T *dst_ip
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceDestIpv6AddrPrefix
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip prefix length by mac_ace_index
 * INPUT    : ipv6_ace_index, dst_ip_prefix
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceDestIpv6AddrPrefix(
    UI32_T mac_ace_index,
    UI32_T dst_ip_prefix
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceProtocol
 *------------------------------------------------------------------------------
 * PURPOSE  : change protocol_type by mac_ace_index
 * INPUT    : mac_ace_index, protocol_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceProtocol(
    UI32_T mac_ace_index,
    UI32_T protocol_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceSourcePortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change source port operator by mac_ace_index
 * INPUT    : mac_ace_index, src_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceSourcePortOp(
    UI32_T mac_ace_index,
    UI32_T src_port_operator
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceSourcePort
 *------------------------------------------------------------------------------
 * PURPOSE  : change min src port by mac_ace_index
 * INPUT    : ip_ace_index, src_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceSourcePort(
    UI32_T mac_ace_index,
    UI32_T src_port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceSourcePortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port mask by mac_ace_index
 * INPUT    : ip_ace_index, src_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceSourcePortBitmask(
    UI32_T mac_ace_index,
    UI32_T src_port_mask
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceDestPortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port operator by mac_ace_index
 * INPUT    : ip_ace_index, dst_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceDestPortOp(
    UI32_T mac_ace_index,
    UI32_T dst_port_operator
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceDestPort
 *------------------------------------------------------------------------------
 * PURPOSE  : change min dst port by mac_ace_index
 * INPUT    : ip_ace_index, dst_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceDestPort(
    UI32_T mac_ace_index,
    UI32_T dst_port
);


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceDestPortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port mask by mac_ace_index
 * INPUT    : ip_ace_index, dst_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServMacAceDestPortBitmask(
    UI32_T mac_ace_index,
    UI32_T dst_port_mask
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServIpv6AceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get ui ace by ipv6_ace_index
 * INPUT    : ipv6_ace_index
 * OUTPUT   : ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_GetDiffServIpv6AceEntry(
    UI32_T ipv6_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextDiffServIpv6AceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next ui ace by ipv6_ace_index
 * INPUT    : ipv6_ace_index
 * OUTPUT   : ipv6_ace_index, ace_p
 * RETURN   : Error code
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_GetNextDiffServIpv6AceEntry(
    UI32_T *ipv6_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceType
 *------------------------------------------------------------------------------
 * PURPOSE  : set ace type by ipv6_ace_index
 * INPUT    : ip_ace_index, mib_ace_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpv6AceType(UI32_T ipv6_ace_index, UI32_T mib_ace_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceAccess
 *------------------------------------------------------------------------------
 * PURPOSE  : change access type by ipv6_ace_index
 * INPUT    : ipv6_ace_index, access_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpv6AceAccess(UI32_T ipv6_ace_index, UI32_T access_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceSourceIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpv6AceSourceIpAddr(UI32_T ipv6_ace_index, const UI8_T *src_ip);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceSourceIpAddrPrefix
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip prefix length by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_ip_prefix
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpv6AceSourceIpAddrPrefix(UI32_T ipv6_ace_index, UI32_T src_ip_prefix);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceDestIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_ip
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpv6AceDestIpAddr(UI32_T ipv6_ace_index, const UI8_T *dst_ip);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceDestIpAddrPrefix
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip prefix length by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_ip_prefix
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpv6AceDestIpAddrPrefix(UI32_T ipv6_ace_index, UI32_T dst_ip_prefix);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceNextHeader
 *------------------------------------------------------------------------------
 * PURPOSE  : change next header by ipv6_ace_index
 * INPUT    : ipv6_ace_index, next_header
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpv6AceNextHeader(UI32_T ipv6_ace_index, UI32_T next_header);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dscp by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpv6AceDscp(UI32_T ipv6_ace_index, UI32_T dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceFlowLabel
 *------------------------------------------------------------------------------
 * PURPOSE  : change flow label by ipv6_ace_index
 * INPUT    : ipv6_ace_index, flow_label
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpv6AceFlowLabel(UI32_T ipv6_ace_index, UI32_T flow_label);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change row status by ipv6_ace_index
 * INPUT    : ipv6_ace_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServIpv6AceStatus(UI32_T ipv6_ace_index, UI32_T row_status);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceTimeRange
 *------------------------------------------------------------------------------
 * PURPOSE  : change ace time_range by mac_ace_index
 * INPUT    : mac_ace_index, time_range_name
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServIpv6AceTimeRange(
    UI32_T ipv6_ace_index,
    UI8_T *time_range_name
);
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

#if (SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceSourcePortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change source port operator by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServIpv6AceSourcePortOp(
    UI32_T ipv6_ace_index,
    UI32_T src_port_operator
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceSourcePort
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServIpv6AceSourcePort(
    UI32_T ipv6_ace_index,
    UI32_T src_port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceSourcePortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src port mask by ipv6_ace_index
 * INPUT    : ipv6_ace_index, src_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServIpv6AceSourcePortBitmask(
    UI32_T ipv6_ace_index,
    UI32_T src_port_mask
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceDestPortOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port operator by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_port_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServIpv6AceDestPortOp(
    UI32_T ipv6_ace_index,
    UI32_T dst_port_operator
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceDestPort
 *------------------------------------------------------------------------------
 * PURPOSE  : change min dst port by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_port
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServIpv6AceDestPort(
    UI32_T ipv6_ace_index,
    UI32_T dst_port
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServIpv6AceDestPortBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst port mask by ipv6_ace_index
 * INPUT    : ipv6_ace_index, dst_port_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QoS_SetDiffServIpv6AceDestPortBitmask(
    UI32_T ipv6_ace_index,
    UI32_T dst_port_mask
);
#endif /* SYS_CPNT_ACL_IPV6_EXT_TCP_UDP_PORT */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get action entry by action_index
 * INPUT    : action_index
 * OUTPUT   : action_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetDiffServActionEntry(UI32_T action_index, RULE_TYPE_MIB_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextDiffServActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next action entry by action_index
 * INPUT    : action_index
 * OUTPUT   : action_index, action_entry
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetNextDiffServActionEntry(UI32_T *action_index, RULE_TYPE_MIB_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionBitmap
 *------------------------------------------------------------------------------
 * PURPOSE  : change action bitmap by action_index
 * INPUT    : action_index, action_bitmap
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionBitmap(UI32_T action_index, const UI8_T *action_bitmap);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionPktNewPri
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) packet new priority by action_index
 * INPUT    : action_index, pkt_new_pri
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionPktNewPri(UI32_T action_index, UI32_T pkt_new_pri);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionPktNewPhb
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) packet new priority by action_index
 * INPUT    : action_index, pkt_new_phb
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionPktNewPhb(UI32_T action_index, UI32_T pkt_new_phb);


/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionPktNewIpPrec
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) packet new ip precedence by action_index
 * INPUT    : action_index, pkt_new_ip_prec
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionPktNewIpPrec(UI32_T action_index, UI32_T pkt_new_ip_prec);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionPktNewDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change (in-profile) packet new dscp by action_index
 * INPUT    : action_index, pkt_new_dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionPktNewDscp(UI32_T action_index, UI32_T pkt_new_dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionGreenPktNewDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change (green) packet new dscp by action_index
 * INPUT    : action_index, pkt_new_dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionGreenPktNewDscp(UI32_T action_index, UI32_T pkt_new_dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionYellowPktNewDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change (Yellow) packet new dscp by action_index
 * INPUT    : action_index, pkt_new_dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionYellowPktNewDscp(UI32_T action_index, UI32_T pkt_new_dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionRedPktNewDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : change (red) packet new dscp by action_index
 * INPUT    : action_index, pkt_new_dscp
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionRedPktNewDscp(UI32_T action_index, UI32_T pkt_new_dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionGreenPktTransmit
 *------------------------------------------------------------------------------
 * PURPOSE  : change the action of (Green) packet by action_index
 * INPUT    : action_index, transmit
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionGreenPktTransmit(UI32_T action_index, UI32_T transmit);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionYellowPktDrop
 *------------------------------------------------------------------------------
 * PURPOSE  : change the action of (Yellow) packet by action_index
 * INPUT    : action_index, yellow_drop
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionYellowPktDrop(UI32_T action_index, UI32_T yellow_drop);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionRedPktDrop
 *------------------------------------------------------------------------------
 * PURPOSE  : change the action of (Red) packet by action_index
 * INPUT    : action_index, red_drop
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionRedPktDrop(UI32_T action_index, UI32_T red_drop);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionPktPri
 *------------------------------------------------------------------------------
 * PURPOSE  : Change the packet priority for bundle group
 * INPUT    : action_index, priority
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionPktPri(UI32_T action_index, UI32_T priority);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServActionStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change action row_status by action_index
 * INPUT    : action_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServActionStatus(UI32_T action_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get meter entry by meter_index
 * INPUT    : meter_index
 * OUTPUT   : meter_param
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetDiffServMeterEntry(UI32_T meter_index, RULE_TYPE_TBParamEntry_T *meter_param);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next meter entry by meter_index
 * INPUT    : meter_index
 * OUTPUT   : meter_index, meter_param
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_GetNextDiffServMeterEntry(UI32_T *meter_index,RULE_TYPE_TBParamEntry_T *meter_param);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMeterModel
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter model by meter_index
 * INPUT    : meter_index, meter_model
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMeterModel(UI32_T meter_index, UI32_T meter_model);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMeterRate
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter rate by meter_index
 * INPUT    : meter_index, meter_rate
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMeterRate(UI32_T meter_index, UI32_T meter_rate);

/*zhangwei add */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMeterPeakRate
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter rate by meter_index
 * INPUT    : meter_index, meter_rate
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMeterPeakRate(UI32_T meter_index, UI32_T meter_rate);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMeterBurstSize
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter burst size by meter_index
 * INPUT    : meter_index, burst_size
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMeterBurstSize(UI32_T meter_index, UI32_T burst_size);


/*zhangwei add */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMeterPeakBurstSize
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter burst size by meter_index
 * INPUT    : meter_index, peak_burst_size
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMeterPeakBurstSize(UI32_T meter_index, UI32_T burst_size);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMeterInterval
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter update interval by meter_index
 * INPUT    : meter_index, meter_interval
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMeterInterval(UI32_T meter_index, UI32_T meter_interval);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMeterStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change meter row_status by meter_index
 * INPUT    : meter_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMeterStatus(UI32_T meter_index, UI32_T row_status);

#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServPolicyMapHwCounter
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
L4_PMGR_QoS_GetDiffServPolicyMapHwCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id,
    RULE_TYPE_CounterRec_T *counter
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_ClearDiffServPolicyMapHwCounter
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
L4_PMGR_QoS_ClearDiffServPolicyMapHwCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id
);
#endif /*#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)*/

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServAclHwCounterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next hardware ACE from ACL.
 * INPUT:    ifindex, direction, acl_index, ace_index
 * OUTPUT:   ace_entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_GetDiffServAclHwCounterEntry(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    UI32_T ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextDiffServAclHwCounterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next hardware ACE from ACL.
 * INPUT:    ifindex_p, direction_p, acl_index_p, ace_index, ace_entry_p
 * OUTPUT:   ifindex_p, direction_p, acl_index_p, ace_index, ace_entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_GetNextDiffServAclHwCounterEntry(
    UI32_T *ifindex_p,
    RULE_TYPE_InOutDirection_T *direction_p,
    UI32_T *acl_index_p,
    UI32_T *ace_index_p,
    RULE_TYPE_Ace_Entry_T *ace_entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetAccessGroupAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Set aclIndex of AccessGroupTable
 * INPUT:    ifindex, mib_direction, mib_acl_type
 *           acl_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_SetAccessGroupAclIndex(
    UI32_T ifindex,
    UI32_T mib_direction,
    UI32_T mib_acl_type,
    UI32_T acl_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetAccessGroupTimeRangeName
 *------------------------------------------------------------------------------
 * PURPOSE:  Set timeRangeName of AccessGroupTable
 * INPUT:    ifindex, mib_direction, mib_acl_type
 *           time_range_name
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_SetAccessGroupTimeRangeName(
    UI32_T ifindex,
    UI32_T mib_direction,
    UI32_T mib_acl_type,
    char   *time_range_name
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetAccessGroupCounterStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  Set counter_status of AccessGroupTable
 * INPUT:    ifindex, mib_direction, mib_acl_type
 *           counter_status
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_SetAccessGroupCounterStatus(
    UI32_T ifindex,
    UI32_T mib_direction,
    UI32_T mib_acl_type,
    UI32_T counter_status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetAccessGroupRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  Set status of AccessGroupTable
 * INPUT:    ifindex, mib_direction, mib_acl_type
 *           row_status
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_SetAccessGroupRowStatus(
    UI32_T ifindex,
    UI32_T mib_direction,
    UI32_T mib_acl_type,
    UI32_T row_status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetAccessGroupEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get AccessGroupTable
 * INPUT:    entry_p->ifindex, entry->direction, entry->type
 * OUTPUT:   entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_GetAccessGroupEntry(
    RULE_TYPE_MIB_AccessGroupEntry_T *entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextAccessGroupEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get next AccessGroupTable
 * INPUT:    entry_p->ifindex, entry->direction, entry->type
 * OUTPUT:   entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Using entry_p->ifindex = 0 to get first entry
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_GetNextAccessGroupEntry(
    RULE_TYPE_MIB_AccessGroupEntry_T *entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetPolicyMapPortPolcyMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Set policy map index of diffservPolicyMapPortTable
 * INPUT:    ifindex, mib_direction
 *           policy_map_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_SetPolicyMapPortPolcyMapIndex(
   UI32_T ifindex,
   UI32_T mib_direction,
   UI32_T policy_map_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetPolicyMapPortRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  Set status of diffservPolicyMapPortTable
 * INPUT:    ifindex, mib_direction
 *           row_status
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_SetPolicyMapPortRowStatus(
   UI32_T ifindex,
   UI32_T mib_direction,
   UI32_T row_status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetPolicyMapPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get diffservPolicyMapPortTable
 * INPUT:    entry_p->ifindex, entry->direction
 * OUTPUT:   entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_GetPolicyMapPortEntry(
   RULE_TYPE_MIB_PolicyMapPortEntry_T *entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextPolicyMapPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get next diffservPolicyMapPortTable
 * INPUT:    entry_p->ifindex, entry->direction
 * OUTPUT:   entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_GetNextPolicyMapPortEntry(
    RULE_TYPE_MIB_PolicyMapPortEntry_T *entry_p
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_PMGR_SetSrcIpFilterByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to permit switching traffic which contain
 *            specified Source IP
 * INPUT    : lport, sip, vid
 *            action        -- could be:
 *                          0   L4_PMGR_IPSRCGUARD_ACTION_DELETE_RULE
 *                          1   L4_PMGR_IPSRCGUARD_ACTION_PERMIT
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : 1, this API is common interface to IP Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_SetSrcIpFilterByLport(UI32_T lport, UI32_T sip, UI32_T vid, UI32_T action);


/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_PMGR_SetSrcMacSrcIpFilterByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to permit switching traffic which contain
 *            specified Source IP and Source MAC
 * INPUT    : lport, src_mac, sip, vid
 *            action        -- could be:
 *                          0   L4_PMGR_IPSRCGUARD_ACTION_DELETE_RULE
 *                          1   L4_PMGR_IPSRCGUARD_ACTION_PERMIT
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : 1, this API is common interface to IP Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_SetSrcMacSrcIpFilterByLport(UI32_T lport, const UI8_T *src_mac, UI32_T sip, UI32_T vid, UI32_T action);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_PMGR_SetIpPktActionByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : for IP Source Guard
 *            config chip to drop IP packets
 * INPUT    : lport, src_mac, sip
 *            action        -- could be:
 *                          0   L4_PMGR_IPSRCGUARD_ACTION_DELETE_RULE
 *                          4   L4_PMGR_IPSRCGUARD_ACTION_DROP
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : 1, this API is common interface to IP Source Guard
 *               internally it call different Driver API (hrdrv, rule_ctrl... etc.)
 *            2, drop action in this API has lowest priority. That is,
 *               L4_PMGR_SetDhcpClientPktActionByLport(...),
 *               L4_PMGR_SetDhcpServerPktActionByLport(...),
 *               L4_PMGR_SetSrcIpFilterByLport(...),
 *               L4_PMGR_SetSrcMacSrcIpFilterByLport(...)
 *               dominate some IP packets do not be drop
 *            3, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_SetIpPktActionByLport(UI32_T lport, UI32_T action);




/* ========================================================================== */
#if (SYS_CPNT_COS == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_EnablePortPriorityMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function uses to enable configuration of each protocol(CN,
 *            ETS, ...) by their priority.
 * INPUT    : l_port   - logic port
 *            priority - COS_VM_PRIORITY_XX, the priority of COS config.
 * OUTPUT   : NONE
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_EnablePortPriorityMode(
    UI32_T l_port,
    UI32_T priority
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_DisablePortPriorityMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function uses to disable configuration of each protocol(CN,
 *            ETS, ...) by their priority.
 * INPUT    : l_port   - logic port
 *            priority - COS_VM_PRIORITY_XX, the priority of COS config.
 * OUTPUT   : NONE
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_DisablePortPriorityMode(
    UI32_T l_port,
    UI32_T priority
);

//:TRUST_MODE
BOOL_T L4_PMGR_QOS_SetQosIfMappingMode(UI32_T lport, UI32_T mode);
BOOL_T L4_PMGR_QOS_GetQosIfMappingMode(UI32_T lport, UI32_T *mode);
UI32_T L4_PMGR_QOS_GetRunningTrustStatus(UI32_T lport_ifindex, UI32_T *status_p);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPortPriorityTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the COS trust mode
 * INPUT    : l_port   - logic port
 *            priority - the priority of COS config, ex: COS_VM_PRIORITY_XX
 *            mode     - trust mode, qos_if_trust_mode_t
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_SetPortPriorityTrustMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortPriorityTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the COS trust mode
 * INPUT    : l_port   - logic port
 *            priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT   : mode     - trust mode, qos_if_trust_mode_t
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetPortPriorityTrustMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T *mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortActiveTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets actived trust mode by port.
 * INPUT    : l_port     - Logic port.
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the actived priority.
 *            mode_p     - Trust mode, qos_if_trust_mode_t
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortActiveTrustMode(
    UI32_T l_port,
    UI32_T *priority_p,
    UI32_T *mode_p
);

//:COS_TO_DSCP
UI32_T L4_PMGR_QOS_SetIngressCos2Dscp(UI32_T ifindex,UI32_T cos,UI32_T cfi,UI32_T phb,UI32_T color);
UI32_T L4_PMGR_QOS_GetIngressCos2Dscp(UI32_T ifindex,UI32_T cos,UI32_T cfi,UI32_T *phb,UI32_T *color);
UI32_T L4_PMGR_QOS_GetNextIngressCos2Dscp(UI32_T *ifindex,UI32_T *cos,UI32_T *cfi,UI32_T *phb,UI32_T *color);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPortPriorityIngressCos2Dscp
 *------------------------------------------------------------------------------
 * PURPOSE:  This function sets the mapping of ingress cos to internal dscp
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           cos      - cos vlaue
 *           cfi      - cfi value
 *           phb      - phb value
 *           color    - color vlaue
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     Removed in future.
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_SetPortPriorityIngressCos2Dscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortPriorityIngressCos2Dscp
 *------------------------------------------------------------------------------
 * PURPOSE:  This function gets the mapping of ingress cos to internal dscp
 * INPUT:    l_port   - logic port
 *           priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *           cos      - cos vlaue
 *           cfi      - cfi value
 * OUTPUT:   phb_p    - phb value
 *           color_p  - color vlaue
 * RETURN:   TRUE/FALSE
 * NOTE:     Removed in future.
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetPortPriorityIngressCos2Dscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the global mapping of ingress cos to phb and
 *            color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_SetCos2InternalDscp(
    UI32_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the global mapping of ingress cos to phb and
 *            color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 * OUTPUT   : phb_p    - phb value
 *            color_p  - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetCos2InternalDscp(
    UI32_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next the global mapping of ingress cos to
 *            phb and color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue.
 *            cfi      - cfi value.
 * OUTPUT   : cos      -
 *            cfi      -
 *            phb      - phb value.
 *            color    - color vlaue.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using cos = COS_TYPE_UNKNOWN_COS, cfi = COS_TYPE_UNKNOWN_CFI
 *            to get first entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetNextCos2InternalDscp(
    UI32_T priority,
    UI32_T *cos_p,
    UI32_T *cfi_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetActiveCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the active mapping of ingress cos to phb and
 *            color.
 * INPUT    : cos      - cos vlaue
 *            cfi      - cfi value
 * OUTPUT   : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetActiveCos2InternalDscp(
    UI32_T cos,
    UI32_T cfi,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextRunningCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of cos and cfi which is
 *            mapping to the input phb and color.
 * INPUT    : phb   - phb vlaue.
 *            color - color value.
 * OUTPUT   : cos_p - cos value mapping to the input (phb, color).
 *            cfi_p - cfi vlaue mapping to the input (phb, color).
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using cos = COS_TYPE_UNKNOWN_COS, cfi = COS_TYPE_UNKNOWN_CFI
 *            to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_PMGR_QOS_GetNextRunningCos2InternalDscp(
    UI32_T phb,
    UI32_T color,
    UI32_T *cos_p,
    UI32_T *cfi_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the per-port mapping of ingress cos to phb and
 *            color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_SetPortCos2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the per-port mapping of ingress cos to phb and
 *            color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 * OUTPUT   : phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortCos2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next the per-port mapping of ingress cos to
 *            phb and color.
 * INPUT    : l_port_p - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            cos_p    - cos vlaue
 *            cfi_p    - cfi value
 * OUTPUT   : l_port_p -
 *            cos_p    -
 *            cfi_p    -
 *            phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using l_port = 0, cos = COS_TYPE_UNKNOWN_COS,
 *            cfi = COS_TYPE_UNKNOWN_CFI to get first entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetNextPortCos2InternalDscp(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *cos_p,
    UI32_T *cfi_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortActiveCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the per-port actived mapping of ingress cos to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            cos      - cos vlaue
 *            cfi      - cfi value
 * OUTPUT   : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortActiveCos2InternalDscp(
    UI32_T l_port,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextRunningPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of ingress cos to
 *            internal dscp
 * INPUT    : l_port   - Logical port id.
 *            phb   - phb vlaue.
 *            color - color value.
 *            cos_p - The start cos to get next.
 *            cfi_p - The start cfi to get next.
 * OUTPUT   : cos_p - cos value mapping to the input (phb, color).
 *            cfi_p - cfi vlaue mapping to the input (phb, color).
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using cos = COS_TYPE_UNKNOWN_COS, cfi = COS_TYPE_UNKNOWN_CFI
 *            to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_PMGR_QOS_GetNextRunningPortCos2InternalDscp(
    UI32_T l_port,
    UI32_T phb,
    UI32_T color,
    UI32_T *cos_p,
    UI32_T *cfi_p
);

//:PRE_2_DSCP
UI32_T L4_PMGR_QOS_SetIngressPre2Dscp(UI32_T lport, UI32_T pre, UI32_T  phb, UI32_T  color);
UI32_T L4_PMGR_QOS_ResetIngressPre2Dscp(UI32_T lport);
UI32_T L4_PMGR_QOS_GetIngressPre2Dscp(UI32_T lport,UI8_T phb[],UI8_T color[]);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            pre      - IP precednece vlaue
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   :
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_SetPortPre2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T pre,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            pre      - IP precednece vlaue
 * OUTPUT   : phb   - phb value
 *            color - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortPre2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T pre,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next the mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port_p - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            pre      - IP precednece vlaue
 *            phb_p    - phb value
 *            color_p  - color vlaue
 * OUTPUT   : l_port_p -
 *            phb_p    -
 *            color_p  -
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using l_port = 0, pre = COS_TYPE_UNKNOWN_PRE to get first entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetNextPortPre2InternalDscp(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *pre_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortActivePre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the actived mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            pre      - IP precednece vlaue
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb_p      - phb value
 *            color_p    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortActivePre2InternalDscp(
    UI32_T l_port,
    UI32_T pre,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetRunningPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets running config of IP precedence to internal DSCP
 *          mapping table.
 * INPUT:   l_port - Logical port id.
 * OUTPUT:  phb_ar[MAX_PRE_VAL+1]   - Array for the mapping of pre to phb.
 *          color_ar[MAX_PRE_VAL+1] - Array for the mapping of pre to color.
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_PMGR_QOS_GetRunningPortPre2InternalDscp(
    UI32_T l_port,
    UI32_T *phb_ar,
    UI32_T *color_ar
);

//:DSCP_2_DSCP
UI32_T L4_PMGR_QOS_SetIngressDscp2Dscp(UI32_T ifindex,UI32_T o_dscp,UI32_T phb,UI32_T color);
UI32_T L4_PMGR_QOS_GetIngressDscp2Dscp(UI32_T lport,UI32_T dscp,UI32_T *phb,UI32_T *color);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the global mapping of ingress dscp to
 *            phb and color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp     - dscp value
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_SetDscp2InternalDscp(
    UI32_T priority,
    UI32_T dscp,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the global mapping of ingress dscp to
 *            phb and color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp     - dscp value
 * OUTPUT   : phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetDscp2InternalDscp(
    UI32_T priority,
    UI32_T dscp,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next global mapping of ingress dscp to
 *            phb and color.
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp_p   - dscp value
 * OUTPUT   : dscp_p   -
 *            phb_p    - phb value
 *            color_p  - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using dscp_p = COS_TYPE_UNKNOWN_DSCP to get first entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetNextDscp2InternalDscp(
    UI32_T priority,
    UI32_T *dscp_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetActiveDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the actived global mapping of ingress dscp to
 *            phb and color.
 * INPUT    : dscp       - dscp value
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb_p      - phb value
 *            color_p    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetActiveDscp2InternalDscp(
    UI32_T dscp,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextRunningDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of ingress dscp which is
 *            mapping to input phb and color.
 * INPUT    : phb    - phb vlaue.
 *            color  - color value.
 *            dscp_p - dscp value to get next mapping.
 * OUTPUT   : dscp_p - dscp value mapping to the input phb and color.
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using dscp = COS_TYPE_UNKNOWN_DSCP to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_PMGR_QOS_GetNextRunningDscp2InternalDscp(
    UI32_T phb,
    UI32_T color,
    UI32_T *dscp_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the per-port mapping of ingress dscp to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp     - dscp value
 *            phb      - phb value
 *            color    - color vlaue
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_SetPortDscp2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T dscp,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the per-port mapping of ingress dscp to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp     - dscp value
 * OUTPUT   : phb      - phb value
 *            color    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortDscp2InternalDscp(
    UI32_T l_port,
    UI32_T priority,
    UI32_T dscp,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next per-port mapping of ingress dscp to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            dscp_p   - dscp value
 * OUTPUT   : l_port  -
 *            dscp_p  -
 *            phb_p   - phb value
 *            color_p - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using l_port = 0, dscp_p = COS_TYPE_UNKNOWN_DSCP to get first
 *            entry.
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetNextPortDscp2InternalDscp(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *dscp_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortActiveDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets actived per-port mapping of ingress dscp to
 *            phb and color.
 * INPUT    : l_port   - Logical port id.
 *            dscp     - dscp value
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_X, the actived priority.
 *            phb_p      - phb value
 *            color_p    - color vlaue
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortActiveDscp2InternalDscp(
    UI32_T l_port,
    UI32_T dscp,
    UI32_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextRunningPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of ingress dscp to
 *            internal dscp
 * INPUT    : l_port - Logical port id.
 *            phb    - phb vlaue.
 *            color  - color value.
 *            dscp_p - The dscp value using for get next.
 * OUTPUT   : dscp_p - dscp value mapping to the input phb and color.
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using l_port = 0, dscp = COS_TYPE_UNKNOWN_DSCP to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_PMGR_QOS_GetNextRunningPortDscp2InternalDscp(
    UI32_T l_port,
    UI32_T phb,
    UI32_T color,
    UI32_T *dscp_p
);

//:PHB_TO_QUEUE
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the global mapping of ingress DSCP to Queue
 * INPUT    : l_port
 *            phb
 *            queue - if queue == 8, set the phb mapping to default queue.
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : This API only for user config
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_SetIngressDscp2Queue(
    UI32_T phb,
    UI32_T queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPortIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the mapping of ingress DSCP to Queue by port
 * INPUT    : l_port
 *            phb
 *            queue - if queue == 8, set the phb mapping to default queue.
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : This API only for user config
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_SetPortIngressDscp2Queue(
    UI32_T ifindex,
    UI32_T phb,
    UI32_T queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Get the global mapping of ingress DSCP to Queue
 * INPUT    : None
 * OUTPUT   : queue[MAX_PHB_VAL + 1]
 * RETURN   : TRUE/FALSE
 * NOTE     : This API gets the active config (which is running on switch)
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetIngressDscp2Queue(
    UI8_T *queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Get the mapping of ingress DSCP to Queue by port
 * INPUT    : l_port
 * OUTPUT   : queue[MAX_PHB_VAL + 1]
 * RETURN   : TRUE/FALSE
 * NOTE     : This API gets the active config (which is running on switch)
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortIngressDscp2Queue(
    UI32_T lport,
    UI8_T *queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPriorityIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue for all Dscp
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  queue    - mapping table of dscp to queue , queue[MAX_PHB_VAL+1]
 * RETURN:  TRUE/FALSE
 * NOTE:    Return config value by input priority
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetPriorityIngressDscp2Queue(
    UI16_T priority,
    UI8_T *queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortPriorityIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue for all Dscp by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  queue    - mapping table of dscp to queue , queue[MAX_PHB_VAL+1]
 * RETURN:  TRUE/FALSE
 * NOTE:    Return config value by input priority
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetPortPriorityIngressDscp2Queue(
    UI32_T l_port,
    UI16_T priority,
    UI8_T *queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the global mapping of PHB to queue
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - PHB value.
 *            queue    - Queue id.
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_SetIngressPhb2Queue(
    UI32_T priority,
    UI32_T phb,
    UI32_T queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets the global mapping of PHB to queue
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - PHB value.
 * OUTPUT   : queue_p  - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetIngressPhb2Queue(
    UI32_T priority,
    UI32_T phb,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetActiveIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets actived global mapping of PHB to queue
 * INPUT    : phb        - PHB value.
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            queue_p    - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetActiveIngressPhb2Queue(
    UI32_T phb,
    UI32_T *priority_p,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextRunningIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of PHB to queue.
 * INPUT    : queue_p - Queue id.
 *            phb_p   - PHB value.
 * OUTPUT   : phb_p -
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using phb = COS_TYPE_UNKNOWN_PHB to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_PMGR_QOS_GetNextRunningIngressPhb2Queue(
    UI32_T queue,
    UI32_T *phb_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Set the per-port mapping of PHB to queue.
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - PHB value.
 *            queue    - Queue id.
 * OUTPUT   : None
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_SetPortIngressPhb2Queue(
    UI32_T l_port,
    UI32_T priority,
    UI32_T phb,
    UI32_T queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets the per-port mapping of PHB to queue
 * INPUT    : l_port   - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb      - PHB value.
 * OUTPUT   : queue_p  - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortIngressPhb2Queue(
    UI32_T l_port,
    UI32_T priority,
    UI32_T phb,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets the next per-port mapping of PHB to queue
 * INPUT    : l_port_p - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            phb_p    - PHB value.
 * OUTPUT   : queue_p  - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using ifindex = 0, phb = COS_TYPE_UNKNOWN_PHB to get first entry
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetNextPortIngressPhb2Queue(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *phb_p,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortActiveIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets actived global mapping of PHB to queue
 * INPUT    : l_port     - Logical port id.
 *            phb        - PHB value.
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            queue_p    - Queue id.
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortActiveIngressPhb2Queue(
    UI32_T l_port,
    UI32_T phb,
    UI32_T *priority_p,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextRunningPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next running config of PHB to queue.
 * INPUT    : l_port     - Logical port id.
 *            queue_p - Queue id.
 *            phb_p   - PHB value.
 * OUTPUT   : phb_p -
 * RETURN   : SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE     : Using phb = COS_TYPE_UNKNOWN_PHB to get first entry.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_PMGR_QOS_GetNextRunningPortIngressPhb2Queue(
    UI32_T l_port,
    UI32_T queue,
    UI32_T *phb_p
);

//:DSCP_TO_COLOR
UI32_T L4_PMGR_QOS_SetIngressDscp2Color(UI32_T ifindex,UI32_T phb,UI32_T color);
UI32_T L4_PMGR_QOS_GetIngressDscp2Color(UI32_T lport,UI8_T *color);
UI32_T L4_PMGR_QOS_GetRunningIngressDscp2Color(UI32_T lport,UI8_T *color);

//:DSCP_TO_COS
UI32_T L4_PMGR_QOS_SetIngressDscp2Cos(UI32_T ifindex,UI32_T phb,UI32_T color,UI32_T cos,UI32_T cfi);
UI32_T L4_PMGR_QOS_GetIngressDscp2Cos(UI32_T ifindex,UI32_T phb,UI32_T color, UI32_T *cos,UI32_T *cfi);
UI32_T L4_PMGR_QOS_GetNextIngressDscp2Cos(UI32_T *ifindex,UI32_T *phb,UI32_T *color,UI32_T *cos,UI32_T *cfi);
UI32_T L4_PMGR_QOS_GetRunningIngressDscp2Cos(UI32_T lport,UI8_T cos,UI8_T cfi,UI8_T *l_phb,UI8_T *l_color,UI8_T cos_phb[],UI8_T cos_color[]);

//:IP_PORT_TO_DSCP
UI32_T L4_PMGR_QOS_SetTcpUdp2Dscp(UI32_T ifindex,UI32_T protocol,UI32_T dport,UI32_T phb,UI32_T color,UI32_T add);
UI32_T L4_PMGR_QOS_GetTCPUDP2Dscp(UI32_T ifindex,UI32_T protocol,UI32_T dport,UI32_T *phb,UI32_T *color);
UI32_T L4_PMGR_QOS_GetNextTCPUDP2Dscp(UI32_T *ifindex,UI32_T *protocol,UI32_T *dport,UI32_T *phb,UI32_T *color);
UI32_T L4_PMGR_QOS_GetRunningTCPUDP2Dscp(UI32_T ifindex,UI32_T *protocol,UI32_T *dport,UI32_T *phb,UI32_T *color);


//:QUEUE_MODE
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the queue mode
 * INPUT    : l_port   - logic port
 *            mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : This function will set the config on user priority
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_SetPortEgressQueueMode(
    UI32_T l_port,
    UI32_T mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the queue mode
 * INPUT    : l_port   - logic port
 * OUTPUT   : mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetPortEgressQueueMode(
    UI32_T l_port,
    UI32_T *mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the next queue mode
 * INPUT    : ifindex_p   - logic port
 * OUTPUT   : mode_p     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetNextPortEgressQueueMode(
    UI32_T *ifindex_p,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the queue mode
 * INPUT    : priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *            mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_SetPriorityEgressQueueMode(
    UI16_T priority,
    UI32_T mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the queue mode
 * INPUT    : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 * OUTPUT   : mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetPriorityEgressQueueMode(
    UI16_T priority,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetActiveEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets actived queue mode
 * INPUT    :
 * OUTPUT   : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            mode     - COS_VM_SCHEDULING_METHOD_E, scheduling mode,
 * RETURN   : COS_TYPE_ERROR_CODE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetActiveEgressQueueMode(
    UI32_T *priority_p,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPortPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the queue mode
 * INPUT    : l_port   - logic port
 *            priority - the priority of COS config, ex: COS_VM_PRIORITY_XX
 *            mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_SetPortPriorityEgressQueueMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the queue mode
 * INPUT    : l_port   - logic port
 *            priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT   : mode     - scheduling mode, COS_VM_SCHEDULING_METHOD_E
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetPortPriorityEgressQueueMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortActiveEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets per-port actived queue mode
 * INPUT    : l_port   - Logical port id.
 * OUTPUT   : priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            mode     - COS_VM_SCHEDULING_METHOD_E, scheduling mode,
 * RETURN   : COS_TYPE_ERROR_CODE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortActiveEgressQueueMode(
    UI32_T l_port,
    UI32_T *priority_p,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_ResetPortPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function resets the queue mode to default value
 * INPUT    : l_port   - logic port
 *            priority - the priority of COS config, ex: COS_VM_PRIORITY_XX
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_ResetPortPriorityEgressQueueMode(
    UI32_T l_port,
    UI16_T priority
);

//:QUEUE_WEIGHT
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPortEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the wrr queue weight
 * INPUT    : l_port   - logic port
 *            q_id     - queue id
 *            weight   - weight
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : Removed in future.
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_SetPortEgressWrrQueueWeight(
    UI32_T l_port,
    UI32_T q_id,
    UI32_T weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the wrr queue weight
 * INPUT    : l_port   - logic port
 *            q_id     - queue id
 * OUTPUT   : weight   - weight
 * RETURN   : TRUE/FALSE
 * NOTE     : Removed in future.
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetPortEgressWrrQueueWeight(
    UI32_T l_port,
    UI32_T q_id,
    UI32_T *weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next wrr queue weight
 * INPUT:   queue_p     -
 *
 * OUTPUT:  queue_p     -
 *          weight_p    - queue weight
 * RETURN:  TRUE / FALSE
 * NOTE:    Input queue (COS_TYPE_UNKNOWN_QUEUE) to get first queue weight,
 *
 *          In hybrid mode, this function still gets the configured wrr weight
 *          for strict-queue
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetNextEgressQueueWeight(
    UI32_T *queue_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetNextPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next wrr queue weight and queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex_p   -
 *          queue_p     -
 *
 * OUTPUT:  ifindex_p   -
 *          queue_p     -
 *          weight      - queue weight
 * RETURN:  TRUE / FALSE
 * NOTE:    Input ifindex, queue (0, COS_TYPE_UNKNOWN_QUEUE)
 *          to get first queue weight.
 *
 *          In hybrid mode, this function still gets the configured wrr weight
 *          for strict-queue
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetNextPortEgressQueueWeight(
    UI32_T *ifindex_p,
    UI32_T *queue_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the wrr queue weight
 * INPUT    : priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *            q_id     - queue id
 *            weight   - weight
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_SetPriorityEgressWrrQueueWeight(
    UI16_T priority,
    UI32_T q_id,
    UI32_T weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the wrr queue weight
 * INPUT    : priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *            q_id     - queue id
 * OUTPUT   : weight   - weighte
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetPriorityEgressWrrQueueWeight(
    UI16_T priority,
    UI32_T q_id,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetActiveQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived queue weight.
 * INPUT:   queue      - queue
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX, the actived priority of config.
 *          weight_p   - weight.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetActiveQueueWeight(
    UI32_T queue,
    UI32_T *priority_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetRunningEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the config of wrr queue weight
 * INPUT:   queue     - queue id
 * OUTPUT:  weight_p    - queue weight
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_PMGR_QOS_GetRunningEgressQueueWeight(
    UI32_T queue,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPortPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE  : This function sets the wrr queue weight
 * INPUT    : l_port   - logic port
 *            priority - the priority of COS config, ex: COS_VM_PRIORITY_XX
 *            q_id     - queue id
 *            weight   - weight
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_SetPortPriorityEgressWrrQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id,
    UI32_T weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the wrr queue weight
 * INPUT    : l_port   - logic port
 *            priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *            q_id     - queue id
 * OUTPUT   : weight   - weighte
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetPortPriorityEgressWrrQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id,
    UI32_T *weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortActiveQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets the wrr queue weight
 * INPUT    : l_port     - logic port
 *            queue      - queue id
 * OUTPUT   : priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            weight     - weight
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortActiveQueueWeight(
    UI32_T l_port,
    UI32_T queue,
    UI32_T *priority_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_ResetPortPriorityEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE  : This function resets the wrr queue weight to default value
 * INPUT    : l_port   - logic port
 *            priority - the priority of COS config, ex: COS_VM_PRIORITY_XX
 *            q_id     - queue id
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
*/
BOOL_T
L4_PMGR_QOS_ResetPortPriorityEgressWrrQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetRunningPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the config of wrr queue weight by port
 * INPUT:   ifindex   - logic port
 *          queue     - queue id
 * OUTPUT:  weight_p    - queue weight
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
L4_PMGR_QOS_GetRunningPortEgressQueueWeight(
    UI32_T ifindex,
    UI32_T queue,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPriorityEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function only used for hybrid queue mode (ex: strict+wrr)
 *          to set the queue to be strict queue or wrr queue
 * INPUT:   priority      - The priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue         - Queue id
 *          status -
 * OUTPUT:  None
 * RETURN:  TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_SetPriorityEgressQueueStrictStatus(
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPriorityEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue status(strict/wrr)
 * INPUT:   priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue
 * OUTPUT:  status_p -
 * RETURN:  TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetPriorityEgressQueueStrictStatus(
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetActiveQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived queue status(strict/wrr)
 * INPUT:   queue      - queue
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *          status_p   - COS_VM_QUEUE_STRICT_STATUS_T.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetActiveQueueStrictStatus(
    UI32_T queue,
    UI32_T *priority_p,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_SetPortPriorityEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function only used for hybrid queue mode (ex: strict+wrr)
 *          to set the queue to be strict queue or wrr queue
 * INPUT:   ifindex
 *          priority      - The priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue         - Queue id
 *          status -
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_SetPortPriorityEgressQueueStrictStatus(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortPriorityEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex   - logic port
 *          priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue id
 * OUTPUT:  status_p -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
L4_PMGR_QOS_GetPortPriorityEgressQueueStrictStatus(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QOS_GetPortActiveQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived queue status(strict/wrr) by port
 * INPUT:   l_port     - Logic port id.
 *          queue      - Queue id.
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *          status_p   - COS_VM_QUEUE_STRICT_STATUS_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_QOS_GetPortActiveQueueStrictStatus(
    UI32_T l_port,
    UI32_T queue,
    UI32_T *priority_p,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p
);

#endif //#if (SYS_CPNT_COS == TRUE)
/* ========================================================================== */


/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_PMGR_SetRedirectHTTPClientPackets
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs to redirect HTTP client packets to CPU
 *             for specified logical port. (DPORT == HTTP server)
 *   INPUT : lport, is_enable
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_SetRedirectHTTPClientPackets(UI32_T lport, BOOL_T is_enable);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_PMGR_SetPermitSrcIpPacketsByLport
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs to permit IP packets with specified sip
 *             for specified logical port.
 *   INPUT : lport, sip, is_enable
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_SetPermitSrcIpPacketsByLport(
    UI32_T lport, UI32_T sip, BOOL_T is_enable);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_PMGR_SetDenyIpPacketsByLport
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs to deny IP packets for specified unit/port.
 *   INPUT : lport, is_enable
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : 1, share the same pce with IP SourceGuard
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_SetDenyIpPacketsByLport(UI32_T lport, BOOL_T is_enable);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_TrapPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  trap packet to CPU or not
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T L4_PMGR_TrapPacket2Cpu(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type);

#if (SYS_CPNT_DAI == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetDiffServARPAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get ui ace by arp_ace_index
 * INPUT    : arp_ace_index
 * OUTPUT   : ace_p
 * RETURN   : Error code
 * NOTE     : for SNMP - diffServArpAceTable
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_GetDiffServARPAceEntry(
    UI32_T arp_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_GetNextDiffServArpAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next ui ace by arp_ace_index
 * INPUT    : arp_ace_index
 * OUTPUT   : arp_ace_index, ace_p
 * RETURN   : Error code
 * NOTE     : for SNMP - diffServArpAceTable
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
L4_PMGR_QoS_GetNextDiffServArpAceEntry(
    UI32_T *arp_ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServArpAceAction
 *------------------------------------------------------------------------------
 * PURPOSE  : change action by arp_ace_index
 * INPUT    : arp_ace_index, action_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceAction
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServArpAceAction(UI32_T arp_ace_index, UI32_T action_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServArpAcePktType
 *------------------------------------------------------------------------------
 * PURPOSE  : change pkt type by arp_ace_index
 * INPUT    : arp_ace_index, pkt_type
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAcePktType
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServArpAcePktType(UI32_T arp_ace_index, UI32_T pkt_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServArpAceSrcIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip by arp_ace_index
 * INPUT    : arp_ace_index, src_ip_addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceSourceIpAddr
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServArpAceSrcIpAddr(UI32_T arp_ace_index, UI32_T src_ip_addr);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServArpAceSrcIpAddrMask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip mask by arp_ace_index
 * INPUT    : arp_ace_index, src_ip_addr_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceSourceIpAddrBitmask
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServArpAceSrcIpAddrMask(UI32_T arp_ace_index, UI32_T src_ip_addr_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServArpAceDstIpAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst ip by arp_ace_index
 * INPUT    : arp_ace_index, dst_ip_addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceDestIpAddr
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServArpAceDstIpAddr(UI32_T arp_ace_index, UI32_T dst_ip_addr);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServArpAceSrcIpAddrMask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src ip mask by arp_ace_index
 * INPUT    : arp_ace_index, src_ip_addr_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceDestIpAddrBitmask
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServArpAceDstIpAddrMask(UI32_T arp_ace_index, UI32_T dst_ip_addr_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServArpAceSrcMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change src mac address by arp_ace_index
 * INPUT    : arp_ace_index, dst_mac_addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceSourceMacAddr
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServArpAceSrcMacAddr(UI32_T arp_ace_index, UI8_T *src_mac_addr);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServArpAceSrcMacAddrMask
 *------------------------------------------------------------------------------
 * PURPOSE  : change src mac address mask by arp_ace_index
 * INPUT    : arp_ace_index, src_mac_addr_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceSourceMacAddrBitmask
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServArpAceSrcMacAddrMask(UI32_T arp_ace_index, UI8_T *src_mac_addr_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServArpAceDstMacAddr
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst mac address by arp_ace_index
 * INPUT    : arp_ace_index, dst_mac_addr
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceDestMacAddr
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServArpAceDstMacAddr(UI32_T arp_ace_index, UI8_T *dst_mac_addr);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServArpAceDstMacAddrMask
 *------------------------------------------------------------------------------
 * PURPOSE  : change dst mac address mask by arp_ace_index
 * INPUT    : arp_ace_index, dst_mac_addr_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceDestMacAddrBitmask
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServArpAceDstMacAddrMask(UI32_T arp_ace_index, UI8_T *dst_mac_addr_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServArpAceStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change row status by arp_ace_index
 * INPUT    : arp_ace_index, row_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp - diffServArpAceStatus
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServArpAceStatus(UI32_T arp_ace_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServArpAceLogStatus
 *------------------------------------------------------------------------------
 * PURPOSE  : change log status by arp_ace_index
 * INPUT    : arp_ace_index, log_status
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : for snmp -diffServArpAceLogStatus
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServArpAceLogStatus(UI32_T arp_ace_index, UI32_T log_status);

#endif /* end of SYS_CPNT_DAI */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceCosOp
 *------------------------------------------------------------------------------
 * PURPOSE  : change cos operator by mac_ace_index
 * INPUT    : mac_ace_index, cos_operator
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceCosOp(UI32_T mac_ace_index, UI32_T cos_operator);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceCosBitmask
 *------------------------------------------------------------------------------
 * PURPOSE  : change cos mask by mac_ace_index
 * INPUT    : mac_ace_index, cos_mask
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceCosBitmask(UI32_T mac_ace_index, UI32_T cos_mask);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceMinCos
 *------------------------------------------------------------------------------
 * PURPOSE  : change min cos by mac_ace_index
 * INPUT    : mac_ace_index, min_cos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceMinCos(UI32_T mac_ace_index, UI32_T min_cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_QoS_SetDiffServMacAceMaxCos
 *------------------------------------------------------------------------------
 * PURPOSE  : change max cos by mac_ace_index
 * INPUT    : mac_ace_index, max_cos
 * OUTPUT   : none
 * RETURN   : TRUE -- succeeded / FALSE -- failed
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_QoS_SetDiffServMacAceMaxCos(UI32_T mac_ace_index, UI32_T max_cos);

#if (SYS_CPNT_IPV6 == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_PMGR_TrapLinkLocalToCPU
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs to trap my ipv6 link-local address for the vlan to CPU.
 *   INPUT : is_enable, ipv6 link-local addr, vlan_id
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : None
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_TrapLinkLocalToCPU(BOOL_T is_enable, UI8_T link_local_addr[SYS_ADPT_IPV6_ADDR_LEN], UI32_T vlan_id);
#endif /* #if (SYS_CPNT_IPV6 == TRUE) */


#if (SYS_CPNT_CLUSTER == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - L4_PMGR_SetClusterPktAction
 *------------------------------------------------------------------------------
 * PURPOSE  : for Cluster
 *            config chip to control cluster  packets
 * INPUT    : action        -- could be:
 *                          TRUE    copy cluster packet to cpu
 *                          FALSE   not copy cluster packet to cpu
 *
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTE     : if chip is already configured, update/delete config from chip
 *            if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T L4_PMGR_SetClusterPktAction(BOOL_T action);
#endif

#if (SYS_CPNT_PBR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_PBR_BindAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Bind ACL to vlan for PBR
 * INPUT    : vlan      - (Key) Vlan ID.
 *            seq_num   - (Key) Route map sequence number.
 *            acl_index - (Key) ACL index.
 *            action_p  - Policy based routing action.
 * OUTPUT   : none
 * RETURN   : RULE_TYPE_OK / error_code(RULE_TYPE_RETURN_TYPE_T)
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_PBR_BindAcl(
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_index,
    RULE_TYPE_PBR_ACTION_T *action_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - L4_PMGR_PBR_UnbindAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Unind ACL to vlan for PBR
 * INPUT    : vlan      - (Key) vlan ID
 *            seq_num   - (Key) Route map sequence number
 *            acl_index - (Key) ACL index.
 * OUTPUT   : none
 * RETURN   : RULE_TYPE_OK / error_code(RULE_TYPE_RETURN_TYPE_T)
 * NOTE     :
 *------------------------------------------------------------------------------
 */
UI32_T
L4_PMGR_PBR_UnbindAcl(
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_index
);
#endif /* SYS_CPNT_PBR */

#endif

