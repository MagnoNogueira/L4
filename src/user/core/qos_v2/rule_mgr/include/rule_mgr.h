/* FUNCTION NAME: RULE_MGR.H
* PURPOSE:
*	manage/save DiffServ, ACL database(OM) and hardware resource(FP)
*
* NOTES:
*   1,  The parameters of APIs exported by RULE_MGR are object-index.
*       That is, caller MUST translate object-name to object-index then manipulate those objects.
*   2,  This component designed without SEMAPHORE protection and CSC using.
*
*REASON:
*    DESCRIPTION:
*    CREATOR:       pttch
*    Date 2005/5/1
*
* Copyright(C)      Accton Corporation, 2005
*/

#ifndef _RULE_MGR_H
#define _RULE_MGR_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_cpnt.h"
#include "rule_type.h"

/* NAMING CONSTANT DECLARATIONS
 */
/* MACRO FUNCTION DECLARATIONS
 */
/* DATA TYPE DECLARATIONS
 */


 /* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * Routine Name : RULE_MGR_InitiateSystemResources
 *------------------------------------------------------------------------------
 * Function : Initialize RULE MGR
 * Input    : None
 * Output   : None
 * Return   : TRUE/FALSE
 * Note     : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_MGR_InitiateSystemResources();

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_MGR_Create_InterCSC_Relation
 *------------------------------------------------------------------------------
 * PURPOSE: This function initializes all function pointer registration operations.
 * INPUT :  None
 * OUTPUT:  None
 * RETURN:  None
 * NOTES :  None
 *------------------------------------------------------------------------------
 */
void RULE_MGR_Create_InterCSC_Relation();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_EnterMasterMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will make the rule mgr enter the master mode.
 * INPUT    : None.
 * OUTPUT   : None.
 * RETURN   : None.
 * NOTE     : None.
 *------------------------------------------------------------------------------
 */
void RULE_MGR_EnterMasterMode();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_EnterSlaveMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will make the rule mgr enter the slave mode.
 * INPUT    : None.
 * OUTPUT   : None.
 * RETURN   : None.
 * NOTE     : None.
 *------------------------------------------------------------------------------
 */
void RULE_MGR_EnterSlaveMode();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE  : Set transition mode
 * INPUT    : None.
 * OUTPUT   : None.
 * RETURN   : None.
 * NOTE     : None.
 *------------------------------------------------------------------------------
 */
void RULE_MGR_SetTransitionMode();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_EnterTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will make the rule mgr enter the transition mode.
 * INPUT    : None.
 * OUTPUT   : None.
 * RETURN   : None.
 * NOTE     : None.
 *------------------------------------------------------------------------------
 */
void RULE_MGR_EnterTransitionMode();

/* Handle Timer Event */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_ProcessEventTimer
 *------------------------------------------------------------------------------
 * PURPOSE: Process timer alarm event from timer
 * INPUT:  None.
 * OUTPUT: None.
 * RETURN: TRUE/FALSE.
 * NOTES:  None.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_MGR_ProcessEventTimer();

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
/*--------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_ProcessTimeRangeStatusChange
 *---------------------------------------------------------------------------
 * PURPOSE: Process timer alarm event from timer
 * INPUT:  None.
 * OUTPUT: None.
 * RETURN: TRUE/FALSE.
 * NOTES:  None.
 *---------------------------------------------------------------------------
 */
BOOL_T RULE_MGR_ProcessTimeRangeStatusChange(UI8_T *isChanged_list, UI8_T *status_list);
#endif /* SYS_CPNT_TIME_BASED_ACL */


/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_HandleHotInsertion
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will set the port OM of the module ports when
 *            the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is inserted at a time.
 *------------------------------------------------------------------------------
 */
void RULE_MGR_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_HandleHotRemoval
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     removed
 *            number_of_port        -- the number of ports on the removed
 *                                     module
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is removed at a time.
 *------------------------------------------------------------------------------
 */
void RULE_MGR_HandleHotRemoval(UI32_T starting_port_ifindex,UI32_T number_of_port);

/* DiffServ port table */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_BindPort2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to policy map.
 * INPUT:    uport_ifindex, policy_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_BindPort2PolicyMap(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_UnBindPortFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from policy map.
 * INPUT:    uport_ifindex, policy_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_UnBindPortFromPolicyMap(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index
);

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_BindPort2DynamicPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to dynamic policy map.
 * INPUT:    uport_ifindex, policy_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     1. The dynamic policy map is got from authentication server.
 *           2. When apply dynamic policy map, the user configured policy map
 *              on this port should be disabled.
 *           3. If the port already bind a dynamic policy map, then the bind
 *              or unbind action shall be effected after the dynamic policy
 *              map be unbinded.
 *           4. The user policy map should be restored to enable when dynamic
 *              policy be unbinded.
 *           5. The changes by this function should not be saved to configuration
 *              file.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_BindPort2DynamicPolicyMap(UI32_T uport_ifindex, UI32_T policy_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_UnBindPortFromDynamicPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from dynamic policy map.
 * INPUT:    uport_ifindex, policy_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     1. The dynamic policy map is got from authentication server.
 *           2. When apply dynamic policy map, the user configured policy map
 *              on this port should be disabled.
 *           3. If the port already bind a dynamic policy map, then the bind
 *              or unbind action shall be effected after the dynamic policy
 *              map be unbinded.
 *           4. The user policy map should be restored to enable when dynamic
 *              policy be unbinded.
 *           5. The changes by this function should not be saved to configuration
 *              file.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_UnBindPortFromDynamicPolicyMap(UI32_T uport_ifindex);
#endif /* #if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetPolicyMapNameByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get binding policy map's name of port.
 * INPUT:    uport_ifinde
 * OUTPUT:   policy_map_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetPolicyMapNameByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    char *policy_map_name
);

/* DiffServ policy-map*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetPolicyMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of policy map by it's index.
 * INPUT:    policy_map_id
 * OUTPUT:   policy_map_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetPolicyMapNameById(UI32_T policy_map_id, char *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetPolicyMapIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of policy map by it's name.
 * INPUT:    policy_map_name
 * OUTPUT:   policy_map_id.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetPolicyMapIdByName(const char *policy_map_name, UI32_T *policy_map_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_CreatePolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a policy map entry by it's name
 * INPUT:    policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL / POLICY_MAP_EXISTED)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_CreatePolicyMap(const char *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetPolicyMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy map name by it's index
 * INPUT:    policy_map_index, policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetPolicyMapNameById(UI32_T policy_map_index, const char *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetPolicyMapDescById
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy map description by it's index
 * INPUT:    policy_map_index, policy_map_desc
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetPolicyMapDescById(UI32_T policy_map_index, const char *policy_map_desc);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetPolicyMapRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy map row status by it's index
 * INPUT:    policy_map_index, row_status
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetPolicyMapRowStatus(UI32_T policy_map_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetPolicyMapAttachCtrl
 *------------------------------------------------------------------------------
 * PURPOSE  : dissociate/associate policy-map and policy-map element
 * INPUT    : policy_map_index, policy_element_index, attach_action
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : attach_action (1:notCombining,2:Combine,3:Discombine)
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetPolicyMapAttachCtrl(UI32_T policy_map_index, UI32_T policy_element_index, UI32_T attach_action);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_DelPolicyMapById
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_DelPolicyMapById(UI32_T policy_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: policy_map_index.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetPolicyMap(UI32_T policy_map_index, RULE_TYPE_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetMibPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a mib policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: policy_map_index.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetMibPolicyMap(UI32_T policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: policy_map_index.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextPolicyMap(UI32_T *policy_map_index, RULE_TYPE_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextMibPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next mib policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: policy_map_index.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextMibPolicyMap(UI32_T *policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextUIPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: policy_map_index.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextUIPolicyMap(UI32_T *policy_map_index, RULE_TYPE_UI_PolicyMap_T *policy_map);

/* DiffServ policy-map element*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_AddElement2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add a element to policy map by policy_map_index
 * INPUT:    policy_map_index, class_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_AddElement2PolicyMap(UI32_T policy_map_index, UI32_T class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetElementMeterFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Set meter of element from policy map
 * INPUT:    policy_map_index, class_map_index, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetElementMeterFromPolicyMap(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_TBParamEntry_T *meter_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetElementActionFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Set action of element from policy map
 * INPUT:    policy_map_index, class_map_index, action_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetElementActionFromPolicyMap(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_UI_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_DelElementMeterFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete meter of element from policy map
 * INPUT:    policy_map_index, class_map_index, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_DelElementMeterFromPolicyMap(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_TBParamEntry_T *meter_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_DelElementActionFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete action of element from policy map
 * INPUT:    policy_map_index, class_map_index, action_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_DelElementActionFromPolicyMap(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_UI_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_DelElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a element from policy map
 * INPUT:    policy_map_index, class_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_DelElementFromPolicyMap(UI32_T policy_map_index, UI32_T class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  get next element of policy map
 * INPUT:    policy_map_index, element_entry->precedence
 * OUTPUT:   element_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: policy_map_index, element_entry->precedence
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextElementFromPolicyMap(UI32_T policy_map_index, RULE_TYPE_UI_PolicyElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy-map element by index
 * INPUT:    policy_element_index, policy_element
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetPolicyMapElement(UI32_T policy_element_index, const RULE_TYPE_PolicyElement_T *policy_element);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetPolicyMapElementRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy map element row status by it's index
 * INPUT:    policy_map_index, row_status
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetPolicyMapElementRowStatus(UI32_T policy_element_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  get policy-map element by index
 * INPUT:    policy_element_index
 * OUTPUT:   policy_element
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetPolicyMapElement(UI32_T policy_element_index, RULE_TYPE_PolicyElement_T *policy_element);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  get policy-map element by index
 * INPUT:    policy_element_index
 * OUTPUT:   policy_element_index, policy_element
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextPolicyMapElement(UI32_T *policy_element_index, RULE_TYPE_PolicyElement_T *policy_element);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  set meter entry by meter_index
 * INPUT:    meter_index, meter_entry
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetMeterEntry(UI32_T meter_index, const RULE_TYPE_TBParamEntry_T *meter_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetMeterRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set meter row status by meter_index
 * INPUT:    meter_index, row_status
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetMeterRowStatus(UI32_T meter_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetMeterEntryByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get Meter entry by meter_index
 * INPUT:    meter_index
 * OUTPUT:   meter_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetMeterEntryByIndex(UI32_T meter_index, RULE_TYPE_TBParamEntry_T *meter_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get next meter entry by meter_index
 * INPUT:    meter_index
 * OUTPUT:   meter_index, meter_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextMeterEntry(UI32_T *meter_index, RULE_TYPE_TBParamEntry_T *meter_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  set action entry by action_index
 * INPUT:    action_index, action_entry
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetActionEntry(UI32_T action_index, const RULE_TYPE_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetActionRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set action row status by action_index
 * INPUT:    action_index, row_status
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetActionRowStatus(UI32_T action_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetActionEntryByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get action entry by action_index
 * INPUT:    action_index
 * OUTPUT:   action_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetActionEntryByIndex(UI32_T action_index, RULE_TYPE_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetMibActionEntryByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get mib action entry by action_index
 * INPUT    : action_index
 * OUTPUT   : action_entry
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetMibActionEntryByIndex(UI32_T action_index, RULE_TYPE_MIB_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get next action entry by action_index
 * INPUT:    action_index
 * OUTPUT:   action_index, action_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextActionEntry(UI32_T *action_index, RULE_TYPE_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextMibActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next mib action entry by action_index
 * INPUT    : action_index
 * OUTPUT   : action_index, action_entry
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextMibActionEntry(UI32_T *action_index, RULE_TYPE_MIB_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetActionBitmap
 *------------------------------------------------------------------------------
 * PURPOSE  : set specified action entry's bitmap by action_index
 * INPUT    : action_index, action_bitmap
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetActionBitmap(UI32_T action_index, const UI8_T *action_bitmap);

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetUIBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create/update a bundle group
 * INPUT:    policy_map_name    - policy-map name
 *           index              - bundle group index
 *           ui_bundle_group_p  - bundle group object
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     Create/update a bundle by
 *             valid_status = TRUE and index is which entry want to create/update.
 *           Delete a bundle by
 *             valid_status = TRUE and index is which entry want to delete.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_SetUIBundleGroup(
    const char *policy_map_name,
    RULE_TYPE_UI_BundleGroup_T *ui_bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetUIBundleGroup
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
RULE_MGR_GetUIBundleGroup(
    UI32_T policy_map_index,
    RULE_TYPE_UI_BundleGroup_T *ui_bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextUIBundleGroup
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
RULE_MGR_GetNextUIBundleGroup(
    UI32_T *policy_map_index_p,
    RULE_TYPE_UI_BundleGroup_T *ui_bundle_group_p
);

#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

/* DiffServ class-map */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetClassMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of class map by it's index.
 * INPUT:    class_map_id
 * OUTPUT:   class_map_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetClassMapNameById(UI32_T class_map_id, char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetClassMapIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of class map by it's name.
 * INPUT:    class_map_id
 * OUTPUT:   class_map_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetClassMapIdByName(const char *class_map_name, UI32_T *class_map_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_CreateClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a class map
 * INPUT:    class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_CreateClassMap(const char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a class map
 * INPUT:    class_map_index, class_map
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetClassMap(UI32_T class_map_index, RULE_TYPE_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetUIClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a class map
 * INPUT:    class_map_index, class_map
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetUIClassMap(UI32_T class_map_index, const RULE_TYPE_UI_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetClassMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  set class map name by it's index
 * INPUT:    class_map_index, class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetClassMapNameById(UI32_T class_map_index, const char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetClassMapDescById
 *------------------------------------------------------------------------------
 * PURPOSE:  set class map description by it's index
 * INPUT:    class_map_index, class_map_desc
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetClassMapDescById(UI32_T class_map_index, const char *class_map_desc);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetClassMapRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set class map row status by index
 * INPUT:    class_map_index, row_status
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetClassMapRowStatus(UI32_T class_map_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetClassMapAttachCtrl
 *------------------------------------------------------------------------------
 * PURPOSE  : dissociate/associate class-map and class-map element
 * INPUT    : class_map_index, index_type, class_index, attach_action
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : 1, attach_action (1:notCombining,2:Combine,3:Discombine)
 *            2, class_index MAY be an ACL index or a MF index, depends on index_type
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetClassMapAttachCtrl(UI32_T class_map_index, UI32_T index_type, UI32_T class_index, UI32_T attach_action);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_DelClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a class map
 * INPUT:    class_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_DelClassMap(UI32_T class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetClassMapById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetClassMapById(UI32_T class_map_index, RULE_TYPE_UI_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a class map by its index
 * INPUT:    class_map_index
 * OUTPUT:   class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetClassMap(UI32_T class_map_index, RULE_TYPE_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetMibClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a mib class map by its index
 * INPUT:    class_map_index
 * OUTPUT:   class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetMibClassMap(UI32_T class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextClassMap(UI32_T *class_map_index, RULE_TYPE_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextMibClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next mib class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextMibClassMap(UI32_T *class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextUIClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextUIClassMap(UI32_T *class_map_index, RULE_TYPE_UI_ClassMap_T *class_map);

/* DiffServ class-map element*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_AddElement2ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add a element to class map
 * INPUT:    class_map_index, element_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL / NOT_ALLOW_MIX_SELECTOR)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_AddElement2ClassMap(UI32_T class_map_index, RULE_TYPE_UI_ClassMapElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_DelElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a element from class map
 * INPUT:    class_map_index, element_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_DelElementFromClassMap(UI32_T class_map_index, RULE_TYPE_UI_ClassMapElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next element from class map
 * INPUT:    class_map_index, element_index
 * OUTPUT:   element_index, element_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key:class_map_index, element_index (set element_index=0 to get first element_entry)
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextElementFromClassMap(UI32_T class_map_index, UI32_T *element_index, RULE_TYPE_UI_ClassMapElement_T *element_entry);

/* diffServAccessGroupTable */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAccessGroupAclIndex
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
RULE_MGR_SetAccessGroupAclIndex(
    UI32_T ifindex,
    UI32_T mib_direction,
    UI32_T mib_acl_type,
    UI32_T acl_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAccessGroupTimeRangeName
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
RULE_MGR_SetAccessGroupTimeRangeName(
    UI32_T ifindex,
    UI32_T mib_direction,
    UI32_T mib_acl_type,
    char   *time_range_name
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAccessGroupCounterStatus
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
RULE_MGR_SetAccessGroupCounterStatus(
    UI32_T ifindex,
    UI32_T mib_direction,
    UI32_T mib_acl_type,
    UI32_T counter_status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAccessGroupRowStatus
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
RULE_MGR_SetAccessGroupRowStatus(
    UI32_T ifindex,
    UI32_T mib_direction,
    UI32_T mib_acl_type,
    UI32_T row_status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetAccessGroupEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get AccessGroupTable
 * INPUT:    entry_p->ifindex, entry->direction, entry->type
 * OUTPUT:   entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_MGR_GetAccessGroupEntry(
    RULE_TYPE_MIB_AccessGroupEntry_T *entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextAccessGroupEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get next AccessGroupTable
 * INPUT:    entry_p->ifindex, entry->direction, entry->type
 * OUTPUT:   entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Using entry_p->ifindex = 0 to get first entry
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_MGR_GetNextAccessGroupEntry(
    RULE_TYPE_MIB_AccessGroupEntry_T *entry_p
);

/* diffservPolicyMapPortTable
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetPolicyMapPortPolcyMapIndex
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
RULE_MGR_SetPolicyMapPortPolcyMapIndex(
   UI32_T ifindex,
   UI32_T mib_direction,
   UI32_T policy_map_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetPolicyMapPortRowStatus
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
RULE_MGR_SetPolicyMapPortRowStatus(
   UI32_T ifindex,
   UI32_T mib_direction,
   UI32_T row_status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetPolicyMapPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get diffservPolicyMapPortTable
 * INPUT:    entry_p->ifindex, entry->direction, entry->type
 * OUTPUT:   entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_MGR_GetPolicyMapPortEntry(
    RULE_TYPE_MIB_PolicyMapPortEntry_T *entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetPolicyMapPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get next diffservPolicyMapPortTable
 * INPUT:    entry_p->ifindex, entry->direction, entry->type
 * OUTPUT:   entry_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_MGR_GetNextPolicyMapPortEntry(
    RULE_TYPE_MIB_PolicyMapPortEntry_T *entry_p
);

/* ACL port table*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_BindPort2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to ACL
 * INPUT:    uport_ifindex, acl_index, acl_type, ingress_flag, time_range_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     ingress_flag = FALSE implies egress
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_BindPort2Acl(
    UI32_T uport_ifindex,
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type,
    BOOL_T ingress_flag,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_UnBindPortFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from ACL
 * INPUT:    uport_ifindex, acl_index, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_UnBindPortFromAcl(
    UI32_T uport_ifindex,
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type,
    BOOL_T ingress_flag
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_BindPort2DynamicAcl
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
UI32_T RULE_MGR_BindPort2DynamicAcl(UI32_T uport_ifindex, UI32_T acl_index, RULE_TYPE_AclType_T acl_type, BOOL_T ingress_flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_UnBindPortFromDynamicAcl
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
UI32_T
RULE_MGR_UnBindPortFromDynamicAcl(
    UI32_T uport_ifindex,
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type,
    BOOL_T ingress_flag
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetAclByTypeAndDirection
 *------------------------------------------------------------------------------
 * PURPOSE:  get binding ACL by type and direction
 * INPUT:    uport_ifindex, acl_type, ingress_flag
 * OUTPUT:   acl_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetAclByTypeAndDirection(UI32_T uport_ifindex, RULE_TYPE_UI_AclEntry_T *acl_entry, BOOL_T ingress_flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next binding ACL by port
 * INPUT:    uport_ifindex, acl_index, precedence
 * OUTPUT:   acl_index, acl_entry, precedence, p_time_range_index.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextAclByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI32_T *precedence,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *couonter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextUIAclByPortAndBindingType
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next same binding type ACL by binding type and port
 * INPUT:    precedence, uport_ifindex
 * OUTPUT:   uport_ifindex, acl_entry, time_range_name
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextUIAclByPortAndBindingType(
    UI32_T precedence,
    UI32_T *uport_ifindex,
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI8_T *time_range_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextRunningAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running binding ACL by port
 * INPUT:    uport_ifindex, acl_index, precedence
 * OUTPUT:   acl_index, acl_entry, precedence, p_time_range_index.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
RULE_MGR_GetNextRunningAclByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI32_T *precedence,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable
);

/* ACL table*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetAclNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of ACL by it's index.
 * INPUT:    acl_index
 * OUTPUT:   acl_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetAclNameById(UI32_T acl_index, char *acl_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetAclIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name.
 * INPUT:    acl_name
 * OUTPUT:   acl_index.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetAclIdByName(const char *acl_name, UI32_T *acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_CreateAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Create an ACL.
 * INPUT:    acl_name, acl_type
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_CreateAcl(const char *acl_name, RULE_TYPE_AclType_T acl_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAclTypeByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  change an ACL's type by it's index
 * INPUT:    acl_index, mib_acl_type
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetAclTypeByIndex(UI32_T acl_index, UI32_T mib_acl_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAclNameByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  set acl name by it's index
 * INPUT:    acl_index, acl_name
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetAclNameByIndex(UI32_T acl_index, const char *acl_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAclRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set acl row status by index
 * INPUT:    acl_index, row_status
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetAclRowStatus(UI32_T acl_index, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAclAttachCtrl
 *------------------------------------------------------------------------------
 * PURPOSE  : dissociate/associate acl and ace
 * INPUT    : acl_index, ace_type, ace_index, attach_action
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : attach_action (1:notCombining,2:Combine,3:Discombine)
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetAclAttachCtrl(UI32_T mib_acl_index, UI32_T mib_ace_type, UI32_T mib_ace_index, UI32_T mib_attach_action);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_DelAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete an ACL.
 * INPUT:    acl_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_DelAcl(UI32_T acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetAclByIndex(UI32_T acl_index, RULE_TYPE_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetMibAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get MIB ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetMibAclByIndex(UI32_T acl_index, RULE_TYPE_MIB_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_index, acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextAclByIndex(UI32_T *acl_index, RULE_TYPE_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextMibAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next MIB ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_index, acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextMibAclByIndex(UI32_T *acl_index, RULE_TYPE_MIB_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACL which (acl_type == acl_entry->acl_type)
 * INPUT:    acl_index, acl_entry->acl_type
 * OUTPUT:   acl_index, acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextAcl(UI32_T *acl_index, RULE_TYPE_UI_AclEntry_T *acl_entry);

/* ACL ACE*/
/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_InitAce
 *------------------------------------------------------------------------------
 * PURPOSE  : for SNMP and WWW and CLI to make a default ACE
 * INPUT    : ace_entry->ace_type
 * OUTPUT   : ace_entry
 * RETURN   : Error code
 * NOTES    : this API is used to init the ace before use it, need to set ace_entry->ace_type
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_InitAce(RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_InitUIAce
 *------------------------------------------------------------------------------
 * PURPOSE  : for UI to make a default UI ACE entry
 * INPUT    : ace_entry_p->ace_type
 * OUTPUT   : ace_entry_p
 * RETURN   : Error code
 * NOTES    : this API is used to init the ace before use it
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_MGR_InitUIAce(
    RULE_TYPE_UI_Ace_Entry_T *ace_entry_p
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_SetAcePatternToDefaultByType
 *------------------------------------------------------------------------------
 * PURPOSE  : For UI to initial ace.u.{mac|ip|ipv6|arp} to default
 * INPUT    : ace_type
 * OUTPUT   : ace_entry_p
 * RETURN   : Error code
 * NOTES    : Initial the ace pattern by ace_type
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_MGR_SetAcePatternToDefaultByType(
    RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_AclType_T ace_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetUIAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a UI ACE to ACL.
 * INPUT:    acl_name, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_SetUIAce2Acl(
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_UI_Ace_Entry_T *ace_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_DelUIAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a UI ACE from ACL.
 * INPUT:    acl_index, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_DelUIAceFromAcl(
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_UI_Ace_Entry_T *ace_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next UI ACE from ACL.
 * INPUT:    acl_index, ace_index_p
 * OUTPUT:   ace_p
 * RETURN:   Error code.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_GetNextUIAceByAcl(
    UI32_T acl_index,
    UI32_T *ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetAceByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ACE by its index
 * INPUT:    ace_index
 * OUTPUT:   ace_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_GetUIAceByIndex(
    UI32_T ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextUIAceByAceTypeAndIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next UI ACE by its index and ace type
 * INPUT:    ace_index_p, ace_type (RULE_TYPE_IP_ACL, RULE_TYPE_MAC_ACL, RULE_TYPE_IPV6_ACL)
 * OUTPUT:   ace_index_p, ace_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_GetNextUIAceByAceTypeAndIndex(
    UI32_T ace_type,
    UI32_T *ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextHardwareUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next hardware UI ACE from ACL.
 * INPUT:    acl_index, acl_type, ace_index
 * OUTPUT:   ace_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_GetNextHardwareUIAceByAcl(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T acl_index,
    RULE_TYPE_AclType_T *acl_type,
    UI32_T *ace_index,
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a ACE to ACL.
 * INPUT:    acl_index, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetAce2Acl(UI32_T acl_index, RULE_TYPE_AclType_T acl_type, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_DelAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a ACE from ACL.
 * INPUT:    acl_index, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_DelAceFromAcl(UI32_T acl_index, RULE_TYPE_AclType_T acl_type, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_BindAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set an ACE to ACL.
 * INPUT:    acl_index, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_BindAce2Acl(UI32_T acl_index, UI32_T ace_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_UnbindAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete an ACE from ACL.
 * INPUT:    acl_index, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_UnbindAceFromAcl(UI32_T acl_index, RULE_TYPE_AclType_T acl_type, UI32_T ace_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE from ACL.
 * INPUT:    acl_index, acl_type, ace_index
 * OUTPUT:   ace_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextAceByAcl(UI32_T acl_index, RULE_TYPE_AclType_T *acl_type, UI32_T *ace_index, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetHardwareAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next hardware ACE from ACL.
 * INPUT:    acl_index, acl_type, ace_index
 * OUTPUT:   ace_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_GetHardwareAceByAcl(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T acl_index,
    UI32_T ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextHardwareAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next hardware ACE from ACL.
 * INPUT:    acl_index, acl_type, ace_index
 * OUTPUT:   ace_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_GetNextHardwareAceByAcl(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T acl_index,
    RULE_TYPE_AclType_T *acl_type,
    UI32_T *ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextMibAceByPort
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
RULE_MGR_GetNextMibAceByPort(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index_p,
    UI32_T *ace_index_p,
    RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable_p
);

#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetQosCounter
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
RULE_MGR_GetQosCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id,
    RULE_TYPE_CounterRec_T *counter
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_ClearQosCounter
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
RULE_MGR_ClearQosCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_id,
    UI32_T class_id
);
#endif

#if (SYS_CPNT_ACL_COUNTER == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_ClearAclRuleCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Clear ACE counter from ACL.
 * INPUT:    acl_index
 * OUTPUT:   none.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_ClearAclRuleCounter(UI32_T acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_ClearAclCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Clear ACL counter
 * INPUT:    acl_index  - ACL index
 *                        If this parameter is 0xffffffff, clear counter for all
 *                        ACLs.
 *                        If this parameter is 0xffffffff, ignores ace_index.
 *           ace_index  - ACE index.
 *                        If this parameter is 0xffffffff, clear counter for all
 *                        ACEs of the specified ACL.
 *           interface  - The interface.
 *                        This parameter can be one of the following values,
 *                        RULE_TYPE_INTERFACE_LPORT     - One port
 *                        RULE_TYPE_INTERFACE_UPORT     - One port
 *                        RULE_TYPE_INTERFACE_ALL_UNIT  - All port
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_MGR_ClearAclCounter(
    UI32_T acl_index,
    UI32_T ace_index,
    const RULE_TYPE_INTERFACE_INFO_T *interface
);
#endif /* #if (SYS_CPNT_ACL_COUNTER == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAceTypeByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  set ace_type of ACE by its index
 * INPUT:    ace_index, ace_type
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetAceTypeByIndex(UI32_T ace_index, RULE_TYPE_AclType_T ace_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAceFieldByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Set ACE Field by its index
 * INPUT:    ace_index, ace_entry, field_index (which leaf)
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetAceFieldByIndex(UI32_T ace_index, RULE_TYPE_Ace_Entry_T *ace_entry, UI32_T field_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetUIAceFieldByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Set ACE Field by its index for UI
 * INPUT:    ace_index, ace_entry_p, field_index (which leaf)
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_SetUIAceFieldByIndex(
    UI32_T ace_index,
    const RULE_TYPE_UI_Ace_Entry_T *ace_entry_p,
    UI32_T field_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAceRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set ace row status by index
 * INPUT:    acl_index, ace_type, row_status
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     ace_index and ace_type MUST ALL MATCH if not create
 *           create a specified ace (index + type) if row status is createAndGo or createAndWait
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetAceRowStatus(UI32_T ace_index, RULE_TYPE_AclType_T ace_type, UI32_T row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetAceByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ACE by its index
 * INPUT:    ace_index
 * OUTPUT:   ace_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetAceByIndex(UI32_T ace_index, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextAceByAceTypeAndIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE by its index and ace type
 * INPUT:    ace_index, ace_type
 * OUTPUT:   ace_index, ace_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextAceByAceTypeAndIndex(UI32_T ace_type, UI32_T *ace_index, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_AddAclMirrorEntry
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
RULE_MGR_AddAclMirrorEntry(
    UI32_T ifindex_dest,
    UI32_T acl_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_RemoveAclMirrorEntry
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
RULE_MGR_RemoveAclMirrorEntry(
    UI32_T ifindex_dest,
    UI32_T acl_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextAclMirrorEntry
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
RULE_MGR_GetNextAclMirrorEntry(
    UI32_T *ifindex_dest,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_GetTCAMUtilization
 *------------------------------------------------------------------------------
 * PURPOSE  : get total pce and free_pce counts
 * INPUT    : none
 * OUTPUT   : tcam_util_p
 * RETURN   : TRUE/FALSE.
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_MGR_GetTCAMUtilization(RULE_TYPE_TCAMUtilization_T *tcam_util_p);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_GetTcamEntry
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
RULE_MGR_GetTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_GetNextTcamEntry
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
RULE_MGR_GetNextTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetSupportedTcamCapabilityBitMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Gets TCAM all supporting capability
 * INPUT    :
 * OUTPUT   : bit_map_p
 * RETURN   : Error code.
 * NOTE     :
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_MGR_GetSupportedTcamCapabilityBitMap(
    RULE_TYPE_TCAM_CAP_BITMAP_T *bit_map_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetUIPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Get port entry by ifindex
 * INPUT:    ifindex
 *           inout_profile  - direction
 * OUTPUT:   port_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetUIPortEntry(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    RULE_TYPE_UI_PortEntry_T *port_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextUIPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next port entry by ifindex
 * INPUT:    ifindex
 * OUTPUT:   ifindex, inout_profile, port_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextUIPortEntry(
    UI32_T *ifindex_p,
    RULE_TYPE_InOutDirection_T *inout_profile_p,
    RULE_TYPE_UI_PortEntry_T *port_entry_p
    );

/* For CPU interface */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_TrapPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  trap packet to CPU or not
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_TrapPacket2Cpu(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_MirrorPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  copy packet to CPU or not
 * INPUT:    enable_flag, packet_type, da, dip
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_MirrorPacket2Cpu(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info);

/* For CoS */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetTcpPortCosMap
 *------------------------------------------------------------------------------
 * PURPOSE:  set tcp port vs. cos-value mapping
 * INPUT:    unit, port, tcp_port, cos_value, mapping_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     mapping_index is 1 .. RULE_TYPE_ADPT_MAX_RULE_NBR_OF_TCP_COS_MAPPING
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetTcpPortCosMap(UI32_T unit, UI32_T port, UI16_T tcp_port, UI8_T cos_value, UI8_T mapping_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_DeleteTcpPortCosMap
 *------------------------------------------------------------------------------
 * PURPOSE:  delete tcp port vs. cos-value mapping
 * INPUT:    unit, port, tcp_port, cos_value, mapping_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     mapping_index is 1 .. RULE_TYPE_ADPT_MAX_RULE_NBR_OF_TCP_COS_MAPPING
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_DeleteTcpPortCosMap(UI32_T unit, UI32_T port, UI16_T tcp_port, UI8_T cos_value, UI8_T mapping_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_NotifyIpDscp2CosChangeByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : caller notify rule_mgr which dscp to cos mapping had been changed
 * INPUT    : lport, ip_dscp
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : for policy-map CLI command "set ip dscp xxx"
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_NotifyIpDscp2CosChangeByLport(UI32_T lport, UI8_T ip_dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_NotifyIpPrecedence2CosChangeByLport
 *------------------------------------------------------------------------------
 * PURPOSE  : caller notify rule_mgr which ip precedence to cos mapping had been changed
 * INPUT    : lport, ip_precedence
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : for policy-map CLI command "set ip precedence xxx"
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_NotifyIpPrecedence2CosChangeByLport(UI32_T lport, UI8_T ip_precedence);

#if (SYS_CPNT_IP_SOURCE_GUARD == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_SetSrcIpFilterByUport
 *------------------------------------------------------------------------------
 * PURPOSE : for IP Source Guard
 *            config chip to permit switching traffic which contain
 *            specified Source IP and VID
 *   INPUT : unit - unit
 *           port - port
 *            sip - source ip
 *            vid - vid
 *      is_enable - TRUE   set rule to chip
 *                  FALSE  delete rule from chip
 *  OUTPUT : None
 *  RETURN : TRUE/FALSE
 *    NOTE : 1. if chip is already configured, update/delete config from chip
 *              if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_MGR_SetSrcIpFilterByUport(UI32_T unit, UI32_T port, UI32_T sip, UI32_T vid, BOOL_T is_enable);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_SetSrcMacSrcIpFilterByUport
 *------------------------------------------------------------------------------
 * PURPOSE : for IP Source Guard
 *            config chip to permit switching traffic which contain
 *            specified Source MAC and Source IP and VID
 *   INPUT :    unit - unit
 *              port - port
 *           src_mac - source mac (could be NULL)
 *               sip - source ip
 *               vid - vid
 *         is_enable - TRUE   set rule to chip
 *                     FALSE  delete rule from chip
 *  OUTPUT : None
 *  RETURN : TRUE/FALSE
 *    NOTE : 1. if chip is already configured, update/delete config from chip
 *              if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_MGR_SetSrcMacSrcIpFilterByUport(
    UI32_T unit, UI32_T port, const UI8_T *src_mac,
    UI32_T sip,  UI32_T vid,        BOOL_T is_enable);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_SetIpPktActionByUport
 *------------------------------------------------------------------------------
 * PURPOSE : for IP Source Guard
 *            config chip to drop IP packets
 *   INPUT :   unit - unit
 *             port - port
 *        is_enable - TRUE   set rule to chip
 *                    FALSE  delete rule from chip
 *  OUTPUT : None
 *  RETURN : TRUE/FALSE
 *    NOTE : 1, drop action in this API has lowest priority. That is,
 *               L4_MGR_SetDhcpClientPktActionByLport(...),
 *               L4_MGR_SetDhcpServerPktActionByLport(...),
 *               L4_MGR_SetSrcIpFilterByLport(...),
 *               L4_MGR_SetSrcMacSrcIpFilterByLport(...)
 *               dominate some IP packets do not be drop
 *            2, if chip is already configured, update/delete config from chip
 *               if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_MGR_SetIpPktActionByUport(UI32_T unit, UI32_T port, BOOL_T is_enable);
#endif /* #if (SYS_CPNT_IP_SOURCE_GUARD == TRUE) */

#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_SetPermitIpv6LinkLocal
 *------------------------------------------------------------------------------
 * PURPOSE  : Permit all IPv6 link local packet
 * INPUT    : is_enable - TRUE / FALSE
 * OUTPUT   : None
 * RETURN   : RULE_TYPE_OK / RULE_TYPE_FAIL
 * NOTE     :
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_MGR_SetPermitIpv6LinkLocal(
    BOOL_T  is_enable
);

#endif /* #if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE) */

#if (SYS_CPNT_WEBAUTH == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_SetRedirectHTTPClientPackets
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs to redirect HTTP client packets to CPU
 *             for specified logical port. (DPORT == HTTP server)
 *   INPUT : lport, is_enable
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_MGR_SetRedirectHTTPClientPackets(UI32_T unit, UI32_T port, BOOL_T is_enable);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_SetPermitSrcIpPacketsByUport
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs to permit IP packets with specified sip
 *             for specified unit/port.
 *   INPUT : unit, port, sip, is_enable
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_MGR_SetPermitSrcIpPacketsByUport(
    UI32_T unit, UI32_T port, UI32_T sip, BOOL_T is_enable);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_SetDenyIpPacketsByUport
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs to deny IP packets for specified unit/port.
 *   INPUT : unit, port, is_enable
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : 1, share the same pce with IP SourceGuard
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_MGR_SetDenyIpPacketsByUport(
    UI32_T unit, UI32_T port, BOOL_T is_enable);
#endif /* #if (SYS_CPNT_WEBAUTH == TRUE) */

#if (SYS_CPNT_IPV6 == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_TrapLinkLocalToCPU
 *------------------------------------------------------------------------------
 * PURPOSE : Add/Delete PCEs to trap my ipv6 link-local address for the vlan to CPU.
 *   INPUT : is_enable, ipv6 link-local addr, vlan_id
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE : None
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_MGR_TrapLinkLocalToCPU(BOOL_T is_enable, UI8_T link_local_addr[SYS_ADPT_IPV6_ADDR_LEN], UI32_T vlan_id);
#endif /* #if (SYS_CPNT_IPV6 == TRUE) */

#if (SYS_CPNT_DAI == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_AddTrapArp2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE : for ARP INSPETCION (DAI)
 *            config chip to trap ARP(BC+REPLY) packets to CPU
 *   INPUT : enable_flag - TRUE / ADD,
 *                        FALSE / REMOVE
 *  OUTPUT : None
 *  RETURN : TRUE / FALSE
 *    NOTE :
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_AddTrapArp2Cpu(BOOL_T enable_flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_ArpACLCheckForDai
 *------------------------------------------------------------------------------
 * PURPOSE:  for ARP INSPETCION (DAI)
 *            to check if match the existed arp rules with arp packets
 * INPUT:    arp_acl_name - check which arp acl
 *           arp_pkt_p    - compared arp packet
 * OUTPUT:   b_acl_drop   - is drop
 * RETURN:   TRUE/FALSE
 * NOTE:     TRUE means match rule; FALSE means no match rule
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_ArpACLCheckForDai(const char *arp_acl_name, RULE_TYPE_ArpPktFormat_T *arp_pkt_p, BOOL_T *b_acl_drop);

#if (SYS_CPNT_DAI_RATE_LIMIT_BY_RULE == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_MGR_DAI_TrapArpPacketToCpuWithRate
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
RULE_MGR_DAI_TrapArpPacketToCpuWithRate(
    BOOL_T enable,
    RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T rate
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_MGR_DAI_GetArpPacketCounter
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
RULE_MGR_DAI_GetArpPacketCounter(
    RULE_TYPE_INTERFACE_INFO_T *interface_p,
    RULE_TYPE_TRUNK_MEMBER_T *trunk_member_p,
    BOOL_T reset,
    UI32_T *total_cnt_p
);
#endif /* SYS_CPNT_DAI_RATE_LIMIT_BY_RULE */
#endif /* SYS_CPNT_DAI */

#if (SYS_CPNT_PBR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetPbrAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set ACL rules for PBR
 * INPUT:    is_add   - Add/Delete rule for PBR
 *           vlan     - The vlan for binding.
 *           acl_id   - The ACL for binding.
 *           action_p - The PBR action for this ACL.
 * OUTPUT:   None.
 * RETURN:   RULE_TYPE_OK / Error code (RULE_TYPE_RETURN_TYPE_T)
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_MGR_SetPbrAcl(
    BOOL_T is_add,
    UI32_T vlan,
    UI32_T seq_num,
    UI32_T acl_id,
    const RULE_TYPE_PBR_ACTION_T *action_p
);
#endif /* SYS_CPNT_PBR */

#endif   /* End of _RULE_MGR_H */

