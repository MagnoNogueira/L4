/* FUNCTION NAME: RULE_OM.H
 * PURPOSE:
 *   save DiffServ, ACL database(OM)
 *
 * NOTES:
 *   1, object's index (e.g. class-map index, acl index) count from 1 (1-based) and we store objects
 *      in arrays (zero-based). Therefore, The gap between array index and object index is 1.
 *      That is, object index (A) is stored at array (A - 1).
 *
 * REASON:
 *    DESCRIPTION:
 *    CREATOR:       pttch
 *    Date 2005/5/1
 *
 * Copyright(C)      Accton Corporation, 2005
 */

#ifndef _RULE_OM_H
#define _RULE_OM_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sysrsc_mgr.h"
#include "sys_cpnt.h"
#include "rule_type.h"
#include "sysfun.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define RULE_OM_DEBUG_OM_ERR            0x01L
#define RULE_OM_DEBUG_MGR_ERR           0x02L
#define RULE_OM_DEBUG_CTRL_ERR          0x04L

typedef struct
{
    RULE_TYPE_PortEntry_T        rule_mgr_port_table[RULE_TYPE_TOTAL_NBR_OF_PORT];
    RULE_TYPE_PortEntry_T        egress_port_table[RULE_TYPE_TOTAL_NBR_OF_PORT];
    RULE_TYPE_Acl_T              acl_table[RULE_TYPE_MAX_NBRS_OF_ACL];
} RULE_OM_Snapshot_T;

typedef struct
{
    SYSFUN_DECLARE_CSC_ON_SHMEM

    RULE_TYPE_PolicyMap_T        policy_map_table[SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP];
    RULE_TYPE_ClassMap_T         class_map_table[SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP];
    RULE_TYPE_PortEntry_T        rule_mgr_port_table[RULE_TYPE_TOTAL_NBR_OF_PORT]; /* ingress */
    RULE_TYPE_PortEntry_T        egress_port_table[RULE_TYPE_TOTAL_NBR_OF_PORT];   /* egress */

    RULE_TYPE_PolicyElement_T    policy_map_element_table[SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS];
    RULE_TYPE_Acl_T              acl_table[RULE_TYPE_MAX_NBRS_OF_ACL];
    RULE_TYPE_Ace_Entry_T        ace_table[RULE_TYPE_MAX_NBRS_OF_ACE_OF_SYSTEM];
    RULE_TYPE_TBParamEntry_T     meter_table[SYS_ADPT_DIFFSERV_MAX_NBR_OF_METER];
    RULE_TYPE_Action_T           action_table[SYS_ADPT_DIFFSERV_MAX_NBR_OF_ACTIONS];

    BOOL_T                       rule_om_had_been_initialized; /* to know need to free memory or not */

} RULE_OM_ShmemData_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/*------------------------------------------------------------------------------
 * FUNCTION NAME: RULE_OM_AttachSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for RULE_CTRL in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void RULE_OM_AttachSystemResources(void);

/* FUNCTION NAME: RULE_OM_GetShMemInfo
 *------------------------------------------------------------------------------
 * PURPOSE: Get share memory info
 *------------------------------------------------------------------------------
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 *------------------------------------------------------------------------------
 * NOTES:
 */
void RULE_OM_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p);

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
BOOL_T RULE_OM_InitiateSystemResources();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Initialize
 *------------------------------------------------------------------------------
 * PURPOSE:  initialize om
 * INPUT:    none
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_OM_Initialize();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_HandleHotRemoval
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     removed
 *            number_of_port        -- the number of ports on the removed
 *                                     module
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE.
 * NOTE     : Only one module is removed at a time.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_OM_HandleHotRemoval(UI32_T starting_port_ifindex,UI32_T number_of_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_BindPort2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to policy map.
 * INPUT:    uport_ifindex, policy_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_BindPort2PolicyMap(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_UnBindPort2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Unbind a port from policy map.
 * INPUT:    uport_ifindex, policy_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_UnBindPort2PolicyMap(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_IsExist_PolicyMapName
 *------------------------------------------------------------------------------
 * PURPOSE:  Check the policy map name is used or not.
 * INPUT:    policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Duplicate --> RULE_TYPE_OK/ Not Duplicate --> RULE_TYPE_FAIL.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_IsExist_PolicyMapName(UI8_T *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_IsBindDynamicPolicyMapOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Check the specified port whether bind a dynamic policy map.
 * INPUT:    uport_ifindex
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_OM_IsBindDynamicPolicyMapOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetBindDynamicPolicyMapStatusOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Set bind bynamic policy map status.
 * INPUT:    uport_ifindex, status
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_OM_SetBindDynamicPolicyMapStatusOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    BOOL_T on_off
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetUserCfgPolicyMapIndexOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Set user configured policy map index.
 * INPUT:    uport_ifindex, policy_map_index
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_OM_SetUserCfgPolicyMapIndexOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUserCfgPolicyMapNameOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get user configured policy map index.
 * INPUT:    uport_ifindex
 * OUTPUT:   NONE.
 * RETURN:   policy map index.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetUserCfgPolicyMapNameOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    char *policy_map_name
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetDynamicBindPortListByPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get the port list that bind the specified policy map list with
 *           dynamic type.
 * INPUT:    policy_map_list
 * OUTPUT:   port_list, port_nbr
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetDynamicBindPortListByPolicyMap(UI8_T *policymaplist, UI8_T *port_list, UI32_T *port_nbr);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetBindDynamicAclStatusOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Set binding bynamic acl status.
 * INPUT:    uport_ifindex, on_off
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetBindDynamicAclStatusOnPort(UI32_T uport_ifindex, BOOL_T on_off);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_IsBindDyanmicAclOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Check the specified port whether bind a dynamic acl.
 * INPUT:    uport_ifindex
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_OM_IsBindDyanmicAclOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAclMementoOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Set user configured acl index.
 * INPUT:    uport_ifindex, acl_index, time_range_index
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_SetAclMementoOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    RULE_TYPE_AclType_T acl_type,
    const RULE_TYPE_AclMemento_T *acl_mem_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclMementoOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get user configured acl index that bind on the port.
 * INPUT:    uport_ifindex
 * OUTPUT:   NONE.
 * RETURN:   acl index.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetAclMementoOnPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_AclMemento_T *acl_mem_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetDynamicBindPortListByAclName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get the port list that bind the specified acl name with
 *           dynamic type.
 * INPUT:    acl_name
 * OUTPUT:   port_list, port_nbr
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetDynamicBindPortListByAclName(
    const char* acl_name,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI8_T *port_list,
    UI32_T *port_nbr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetDynamicBindPortListByPolicyMapName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get the port list that bind the specified policy map name with
 *           dynamic type.
 * INPUT:    acl_name
 * OUTPUT:   port_list, port_nbr
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetDynamicBindPortListByPolicyMapName(
    const char *policy_map_name,
    UI8_T port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST],
    UI32_T *port_nbr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPortEntryAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Set acl index on bind acl in port entry
 * INPUT:    ifindex, direction, acl_type
 *           acl_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     For mib using, it only could config when the
 *           bind_acl status is notReady.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetPortEntryAclIndex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type,
    UI32_T acl_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPortEntryAclTimeRange
 *------------------------------------------------------------------------------
 * PURPOSE:  Set time_range_index on bind acl in port entry
 * INPUT:    ifindex, direction, acl_type
 *           time_range_name
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     For mib using, it only could config when the
 *           bind_acl status is notReady.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetPortEntryAclTimeRange(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type,
    UI8_T *time_range_name,
    UI32_T time_range_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPortEntryAclCounterEnable
 *------------------------------------------------------------------------------
 * PURPOSE:  Set counter_enable on bind acl in port entry
 * INPUT:    ifindex, direction, acl_type
 *           time_range_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     For mib using, it only could config when the
 *           bind_acl status is notReady.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetPortEntryAclCounterEnable(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPortEntryAclRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  Set acl index on bind acl in port entry
 * INPUT:    ifindex, direction, acl_type
 *           time_range_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     For mib using, it only could config when the
 *           bind_acl status is notReady.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetPortEntryAclRowStatus(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type,
    UI32_T mib_row_status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPortBindedAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get bind acl on port entry
 * INPUT:    ifindex, direction, acl_type
 * OUTPUT:   bind_acl_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Only get success when bind_acl.row_status is avtiev/notReady
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_GetPortBindedAcl(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_AclMemento_T *bind_acl_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPortEntryPolicyMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Set policy_index on port entry
 * INPUT:    ifindex, direction
 *           policy_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     For mib using, it only could config when the
 *           policy_map_status is notReady.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetPortEntryPolicyMapIndex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPortEntryPolicyMapRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  Set policy_index on port entry
 * INPUT:    ifindex, direction
 *           policy_index
 * OUTPUT:
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     For mib using, only coulde set destroy/notReady.
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetPortEntryPolicyMapRowStatus(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T mib_row_status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPortBindedPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get policy_map_index, policy_map_status on port entry
 * INPUT:    ifindex, direction
 * OUTPUT:   policy_map_index, policy_map_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Only get success when policy_map_row_status is avtiev/notReady
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_GetPortBindedPolicyMap(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T *policy_map_index_p,
    UI32_T *policy_map_row_status_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Create_PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Create Policy_map and return index of this name
 * INPUT:    policy_map_name
 * OUTPUT:   policy_map_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Create_PolicyMap(const char *policy_map_name, UI32_T *policy_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreatePolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  create policy_map on specified index
 * INPUT:    policy_map_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreatePolicyMap(UI32_T policy_map_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Delete_PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete Policy_map by index
 * INPUT:    policy_map_index
 * OUTPUT:   none.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Delete_PolicyMap(UI32_T policy_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMap(UI32_T policy_map_index, RULE_TYPE_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetMibPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a mib policy map entry by its index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetMibPolicyMap(UI32_T policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Use *policy_map_index=0 to get first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextPolicyMap(UI32_T *policy_map_index, RULE_TYPE_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextMibPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next mib policy map entry by its index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Use *policy_map_index=0 to get first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextMibPolicyMap(UI32_T *policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Use *policy_map_index=0 to get first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextUIPolicyMap(UI32_T *policy_map_index, RULE_TYPE_UI_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextPolicyMapByClassMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by class map index
 * INPUT:    class_map_index    - class map index
 *           policy_map_index_p - policy map index. pass 0 to start
 * OUTPUT:   policy_map_index_p - policy map index.
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextPolicyMapByClassMapIndex(
    UI32_T class_map_index,
    UI32_T *policy_map_index_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get policy map row status by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapRowStatus(UI32_T policy_map_index, UI32_T *row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapIndexByName
 *------------------------------------------------------------------------------
 * PURPOSE:  get class-map index by class map name
 * INPUT:    class_map_name
 * OUTPUT:   class_map_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapIndexByName(const char *class_map_name, UI32_T *class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapIndexByMf
 *------------------------------------------------------------------------------
 * PURPOSE  : get class-map index by mf index
 * INPUT    : mf_index
 * OUTPUT   : class_map_index
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapIndexByMf(UI32_T mf_index, UI32_T *class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapIndexByMeterIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get class-map index by meter index
 * INPUT    : meter_idx
 * OUTPUT   : cmap_idx
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapIndexByMeterIndex(UI32_T meter_idx, UI32_T *cmap_idx);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Create_ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Create Class_map and return index of this name
 * INPUT:    class_map_name     - class-map name
 *           match_type         - match type
 * OUTPUT:   class_map_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_Create_ClassMap(
    const char *class_map_name,
    RULE_TYPE_MatchType_T match_type,
    UI32_T *class_map_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  Get class map element by element index
 * INPUT:    class_map_index, element_index
 * OUTPUT:   element_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapElement(UI32_T class_map_index, UI32_T element_index, RULE_TYPE_UI_ClassMapElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapElementClassIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : according to element_entry contents to get class-index in specified class-map
 * INPUT    : class_map_index, element_entry
 * OUTPUT   : class_index
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapElementClassIndex(UI32_T class_map_index, RULE_TYPE_UI_ClassMapElement_T *element_entry, UI32_T *class_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Delete_ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete class-map by index
 * INPUT:    class_map_index
 * OUTPUT:   none.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Delete_ClassMap(UI32_T class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Add_ClassMapElement_To_ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add ClassMapElement to ClassMap by index
 * INPUT:    class_map_index, element_entry
 * OUTPUT:   class_index
 * RETURN:   Error code (OK / FAIL / NOT_ALLOW_MIX_SELECTOR)
 * NOTE:     class_index == acl index or ace index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Add_ClassMapElement_To_ClassMap(UI32_T class_map_index, RULE_TYPE_UI_ClassMapElement_T *element_entry, UI32_T *class_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Remove_ClassMapElement_From_ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add ClassMapElement to ClassMap by index
 * INPUT:    class_map_index, element_entry
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Remove_ClassMapElement_From_ClassMap(UI32_T class_map_index, RULE_TYPE_UI_ClassMapElement_T *element_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RemoveClassMapElementByAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : remove a class-map element according to acl index
 * INPUT    : class_map_index, acl_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_RemoveClassMapElementByAclIndex(UI32_T class_map_index, UI32_T acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RemoveClassMapElementByMfIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : remove a class-map element according to mf index
 * INPUT    : class_map_index, mf_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_RemoveClassMapElementByMfIndex(UI32_T class_map_index, UI32_T mf_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Add_PolicyMapElement_To_PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Add PolicyMapElement to PolicyMap by index
 * INPUT:    policy_map_index, class_map_index
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Add_PolicyMapElement_To_PolicyMap(UI32_T policy_map_index, UI32_T class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Set_MeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Set Meter entry
 * INPUT:    policy_map_index, class_map_index, meter_entry
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL / SAME_CFG)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Set_MeterEntry(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_TBParamEntry_T *meter_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Remove_MeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Remove Meter entry
 * INPUT:    policy_map_index, class_map_index
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Remove_MeterEntry(UI32_T policy_map_index, UI32_T class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  create meter on specified index
 * INPUT:    meter_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreateMeter(UI32_T meter_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DestroyMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  destroy meter by index
 * INPUT:    meter_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     rule mgr SHALL remove meter from policy-map element first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DestroyMeter(UI32_T meter_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  set meter entry by meter_index
 * INPUT:    meter_index, meter_entry
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetMeterEntry(UI32_T meter_index, const RULE_TYPE_TBParamEntry_T *meter_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetMeterRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set meter row status by it's index
 * INPUT:    meter_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetMeterRowStatus(UI32_T meter_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetMeterEntryByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get Meter entry by meter_index
 * INPUT:    meter_index
 * OUTPUT:   meter_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetMeterEntryByIndex(UI32_T meter_index, RULE_TYPE_TBParamEntry_T *meter_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get next meter entry by meter_index
 * INPUT:    meter_index
 * OUTPUT:   meter_index, meter_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextMeterEntry(UI32_T *meter_index, RULE_TYPE_TBParamEntry_T *meter_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetMeterRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get meter row status by it's index
 * INPUT:    action_index
 * OUTPUT:   row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetMeterRowStatus(UI32_T meter_index, UI32_T *row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Set_ActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Set action entry
 * INPUT:    policy_map_index, class_map_index, action_entry
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL / SAME_CFG)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Set_ActionEntry(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetUIActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Set action entry
 * INPUT:    policy_map_index, class_map_index, action_entry
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL / SAME_CFG)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetUIActionEntry(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_UI_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_ActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get action entry
 * INPUT:    policy_map_index, class_map_index, action_entry
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Get_ActionEntry(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUIActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get action entry
 * INPUT:    policy_map_index, class_map_index, action_entry
 * OUTPUT:   None
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetUIActionEntry(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_UI_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreateAction
 *------------------------------------------------------------------------------
 * PURPOSE:  create action on specified index
 * INPUT:    action_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreateAction(UI32_T action_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DestroyAction
 *------------------------------------------------------------------------------
 * PURPOSE:  destroy action by index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     rule mgr SHALL remove action from policy-map element first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DestroyAction(UI32_T action_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DupActionEntryWithNewActionBitmap
 *------------------------------------------------------------------------------
 * PURPOSE  : will duplicate action entry of action_index and
 *            change the data according to the new action_bitmap
 * INPUT    : action_index, action_bitmap
 * OUTPUT   : action_p
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DupActionEntryWithNewActionBitmap(UI32_T action_index, const UI8_T *action_bitmap, RULE_TYPE_Action_T *dst_action_p);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  set action entry by action_index
 * INPUT:    action_index, action_entry
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetActionEntry(UI32_T action_index, const RULE_TYPE_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetActionRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set action row status by it's index
 * INPUT:    action_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetActionRowStatus(UI32_T action_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetInProfileActionToDefault
 *------------------------------------------------------------------------------
 * PURPOSE:  set in-profile action to default value by action_index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetInProfileActionToDefault(UI32_T action_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetOutOfProfileActionToDefault
 *------------------------------------------------------------------------------
 * PURPOSE:  set out-of-profile action to default value by action_index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetOutOfProfileActionToDefault(UI32_T action_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetClassActionToDefault
 *------------------------------------------------------------------------------
 * PURPOSE:  set class action to default value by action_index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetClassActionToDefault(UI32_T action_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetOutOfProfileActionToNull
 *------------------------------------------------------------------------------
 * PURPOSE:  set out-of-profile action to null value by action_index
 * INPUT:    action_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetOutOfProfileActionToNull(UI32_T action_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetActionEntryByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get action entry by action_index
 * INPUT:    action_index
 * OUTPUT:   action_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetActionEntryByIndex(UI32_T action_index, RULE_TYPE_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetMibActionEntryByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get mib action entry by action_index
 * INPUT    : action_index
 * OUTPUT   : action_entry
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetMibActionEntryByIndex(UI32_T action_index, RULE_TYPE_MIB_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get next action entry by action_index
 * INPUT:    action_index
 * OUTPUT:   action_index, action_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextActionEntry(UI32_T *action_index, RULE_TYPE_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextMibActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : get next mib action entry by action_index
 * INPUT    : action_index
 * OUTPUT   : action_index, action_entry
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextMibActionEntry(UI32_T *action_index, RULE_TYPE_MIB_Action_T *action_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetActionRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get action row status by it's index
 * INPUT:    action_index
 * OUTPUT:   row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetActionRowStatus(UI32_T action_index, UI32_T *row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_ConvertToAction
 *------------------------------------------------------------------------------
 * PURPOSE  : translate src_action to dst_action
 * INPUT    : src_action
 * OUTPUT   : dst_action
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_ConvertToAction(const RULE_TYPE_UI_Action_T *src_action, RULE_TYPE_Action_T *dst_action);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_PolicyMapListByClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get policy_map_list
 * INPUT:    class_map_index
 * OUTPUT:   policy_map_list, policymap_nbr
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     if no policy-map reference to this class-map, return FAIL
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Get_PolicyMapListByClassMap(UI32_T class_map_index, UI8_T *policymaplist, UI32_T *policymap_nbr);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_PortListByPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get port_list by policy_map_list
 * INPUT:    policy_map_list
 * OUTPUT:   port_list, port_nbr
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     if no port bound to this policy-map, return FAIL
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_Get_PortListByPolicyMap(
    UI8_T *policymaplist,
    BOOL_T ingress_flag,
    UI8_T *port_list, UI32_T *port_nbr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextPort_BindPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Enumerate all interface which bind the specified policy-map
 * INPUT:    policy_map_index   - the policy-map index
 * OUTPUT:   uport_ifindex_p    - logic port number
 *           inout_profile_p    - direction
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextPort_BindPolicyMap(
    UI32_T  policy_map_index,
    UI32_T  *uport_ifindex_p,
    RULE_TYPE_InOutDirection_T *inout_profile_p
    );

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_PortListByClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : get port_list by class_map_index
 * INPUT    : class_map_index
 * OUTPUT   : port_list, port_nbr
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : return FAIL if there is no port bound to
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_Get_PortListByClassMap(
    UI32_T class_map_index,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI8_T *port_list,
    UI32_T *port_nbr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_PortListByClassMapList
 *------------------------------------------------------------------------------
 * PURPOSE  : get port_list by class_map_list
 * INPUT    : class_map_list
 * OUTPUT   : port_list, port_nbr
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : return FAIL if there is no port bound to
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Get_PortListByClassMapList(const UI8_T *class_map_list, UI8_T *port_list, UI32_T *port_nbr);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextPort_BindClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : Enumerate all interface which have bound the specified class-map.
 * INPUT    : class_map_index       - class-map index
 *            uport_ifindex_p       - uport ifindex; pass 0 to start.
 * OUTPUT   : uport_ifindex_p       - uport ifindex
 *            inout_profile_p       - direction
 *            policy_map_index_p    - policy-map index
 * RETURN   : RULE_TYPE_OK if get an interface; else if error or no more interface
 * NOTES    : Sequence of the enumerating are
 *            uport_inindex -> inout_profile -> policy_map_index
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextPort_BindClassMap(
    UI32_T  class_map_index,
    UI32_T  *uport_ifindex_p,
    RULE_TYPE_InOutDirection_T  *inout_profile_p,
    UI32_T  *policy_map_index_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPortPolicyMapIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get the policy-map index bound to a port by ifindex and ingress flag
 * INPUT:    ifindex, ingress_flag
 * OUTPUT:   pmap_index
 * RETURN:   Error code.
 * NOTE:     return RULE_TYPE_FAIL if no policy-map bound to that port.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetPortPolicyMapIndex(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *pmap_idx
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapNameByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get binding policy map's name of port.
 * INPUT:    uport_ifinde
 * OUTPUT:   policy_map_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapNameByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    char *policy_map_name
);


/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of policy map by it's index.
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapNameById(UI32_T policy_map_index, char *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of policy map by it's name.
 * INPUT:    policy_map_name
 * OUTPUT:   policy_map_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapIdByName(const char *policy_map_name, UI32_T *policy_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapIndexByElementIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get index of policy map by policy-map element index
 * INPUT:    policy_element_index
 * OUTPUT:   policy_map_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapIndexByElementIndex(UI32_T policy_element_index, UI32_T *policy_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPolicyMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy map name by it's index
 * INPUT:    policy_map_index, policy_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetPolicyMapNameById(UI32_T policy_map_index, const char *policy_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPolicyMapDescById
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy map description by it's index
 * INPUT:    policy_map_index, policy_map_desc
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetPolicyMapDescById(UI32_T policy_map_index, const char *policy_map_desc);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPolicyMapRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy map row status by it's index
 * INPUT:    policy_map_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetPolicyMapRowStatus(UI32_T policy_map_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_BindElement2PolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE  : bind policy-map element to policy-map
 * INPUT    : policy_map_index, policy_element_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_BindElement2PolicyMap(UI32_T policy_map_index, UI32_T policy_element_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_UnbindElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE  : unbind policy-map element from policy-map
 * INPUT    : policy_map_index, policy_element_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_UnbindElementFromPolicyMap(UI32_T policy_map_index, UI32_T policy_element_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapByIndex(UI32_T policy_map_index, RULE_TYPE_PolicyMap_T *policy_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DeletePolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete PolicyMap Element
 * INPUT:    policy_map_index, class_map_index
 * OUTPUT:   None.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DeletePolicyMapElement(UI32_T policy_map_index, UI32_T class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextElementFromPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next element of policy map
 * INPUT:    policy_map_index, element_entry->precedence
 * OUTPUT:   element_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: policy_map_index, element_entry->precedence
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextElementFromPolicyMap(UI32_T policy_map_index, RULE_TYPE_UI_PolicyElement_T *element_entry) ;

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreatePolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  create policy-map element on specified index
 * INPUT:    policy_element_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreatePolicyMapElement(UI32_T policy_element_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DestroyPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  destroy policy-map element by index
 * INPUT:    policy_element_index
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     rule mgr SHALL remove element from policy-map first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DestroyPolicyMapElement(UI32_T policy_element_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy-map element by index
 * INPUT:    policy_element_index, policy_element
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetPolicyMapElement(UI32_T policy_element_index, const RULE_TYPE_PolicyElement_T *policy_element);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPolicyMapElementRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy-map element row status by it's index
 * INPUT:    policy_element_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetPolicyMapElementRowStatus(UI32_T policy_element_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  get policy-map element by index
 * INPUT:    policy_element_index
 * OUTPUT:   policy_element
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapElement(UI32_T policy_element_index, RULE_TYPE_PolicyElement_T *policy_element);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapElementFromPolicyMapByClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  get policy-map element from specified policy-map by class-map index
 * INPUT:    policy_map_index, class_map_index
 * OUTPUT:   policy_element
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapElementFromPolicyMapByClassMap(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_PolicyElement_T *policy_element);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_GetPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE  : get policy-map element by policy_map_index and class_map_index
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : NULL -- failed
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetPolicyMapElementEntry(
    UI32_T policy_map_index,
    UI32_T class_map_index,
    RULE_TYPE_PolicyElement_T *entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapElementIndexByMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  get index of policy-map element by meter index
 * INPUT:    meter_index
 * OUTPUT:   policy_element_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapElementIndexByMeter(UI32_T meter_index, UI32_T *policy_element_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapElementIndexByAction
 *------------------------------------------------------------------------------
 * PURPOSE:  get index of policy-map element by action index
 * INPUT:    action_index
 * OUTPUT:   policy_element_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapElementIndexByAction(UI32_T action_index, UI32_T *policy_element_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapElementRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get policy map element row status by it's index
 * INPUT:    policy_element_index
 * OUTPUT:   row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetPolicyMapElementRowStatus(UI32_T policy_element_index, UI32_T *row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  get policy-map element by index
 * INPUT:    policy_element_index
 * OUTPUT:   policy_element_index, policy_element
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextPolicyMapElement(UI32_T *policy_element_index, RULE_TYPE_PolicyElement_T *policy_element);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextClassMap(UI32_T *class_map_index, RULE_TYPE_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextMibClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next mib class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextMibClassMap(UI32_T *class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map_index, class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextUIClassMap(UI32_T *class_map_index, RULE_TYPE_UI_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of class map by it's index.
 * INPUT:    class_map_id
 * OUTPUT:   class_map_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapNameById(UI32_T class_map_id, char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get class-map row status by it's index
 * INPUT:    class_map_index
 * OUTPUT:   row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapRowStatus(UI32_T class_map_index, UI32_T *row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreateClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  create class-map on specified index
 * INPUT:    class_map_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreateClassMap(UI32_T class_map_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DestroyClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : destroy class-map by index
 * INPUT    : class_map_index
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : 1, rule mgr SHALL remove class-map from policy-map element first
 *            2, didn't destroy its elements (MF or ACL)
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DestroyClassMap(UI32_T class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a class map entry by it's index
 * INPUT:    class_map_id, class_map
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetClassMap(UI32_T class_map_index, RULE_TYPE_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetUIClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a class map entry by it's index
 * INPUT:    class_map_id, class_map
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetUIClassMap(UI32_T class_map_index, const RULE_TYPE_UI_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetClassMapNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  set class map name by it's index
 * INPUT:    class_map_index, class_map_name
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetClassMapNameById(UI32_T class_map_index, const char *class_map_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetClassMapDescById
 *------------------------------------------------------------------------------
 * PURPOSE:  set class map description by it's index
 * INPUT:    class_map_index, class_map_desc
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetClassMapDescById(UI32_T class_map_index, const char *class_map_desc);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetClassMapRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set class map row status by index
 * INPUT:    class_map_index, is_active
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetClassMapRowStatus(UI32_T class_map_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_BindElement2ClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : bind class-map element to class-map
 * INPUT    : class_map_index, class_type, class_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : class_index MAY be an ACL index or a MF index, depends on class_type
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_BindElement2ClassMap(UI32_T class_map_index, RULE_TYPE_ClassType_T class_type, UI32_T class_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_UnbindElementFromClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : unbind class-map element from class-map
 * INPUT    : class_map_index, class_type, class_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : class_index MAY be an ACL index or a MF index, depends on class_type
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_UnbindElementFromClassMap(UI32_T class_map_index, RULE_TYPE_ClassType_T class_type, UI32_T class_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMapById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a class map
 * INPUT:    class_map_index
 * OUTPUT:   class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMapById(UI32_T class_map_index, RULE_TYPE_UI_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  get a class map by its index
 * INPUT:    class_map_index
 * OUTPUT:   class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetClassMap(UI32_T class_map_index, RULE_TYPE_ClassMap_T *class_map);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetMibClassMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a mib class map by its index
 * INPUT:    class_map_index
 * OUTPUT:   class_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Key: class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetMibClassMap(UI32_T class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map);

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetUIBundleGroup
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
RULE_OM_SetUIBundleGroup(
    UI32_T policy_map_index,
    RULE_TYPE_UI_BundleGroup_T *ui_bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUIBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a bundle group
 * INPUT:    policy_map_index    - policy-map name
 *
 * OUTPUT:   ui_bundle_group_p  - bundle group object
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetUIBundleGroup(
    UI32_T policy_map_index,
    RULE_TYPE_UI_BundleGroup_T *ui_bundle_group_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RebuildBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Rebuild all bundle group in specified policy-map
 * INPUT:    policy_map_index   - policy-map index
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_RebuildBundleGroup(
    UI32_T policy_map_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_IsClassMapInBundleGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Does the class map in bundle group by specified policy map
 * INPUT:    policy_map_index   - policy-map index
 *           class_map_index    - class-map index
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_IsClassMapInBundleGroup(
    UI32_T policy_map_index,
    UI32_T class_map_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_IsBundleRateSet
 *------------------------------------------------------------------------------
 * PURPOSE:  Does the policy map enable bundle rate control
 * INPUT:    policy_map_index   - policy-map index
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_IsBundleRateSet(
    UI32_T policy_map_index
);
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreateAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Create an ACL.
 * INPUT:    acl_name, acl_type
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreateAcl(const char *acl_name, RULE_TYPE_AclType_T acl_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreateAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  create acl on specified index
 * INPUT:    acl_index, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreateAclByIndex(UI32_T acl_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DestroyAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : destroy acl by index
 * INPUT    : acl_index
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : 1, rule mgr SHALL remove acl from class-map and unbind ports first
 *            2, didn't destroy its elements (ACEs)
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DestroyAcl(UI32_T acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAclTypeByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : set acl type by it's index
 * INPUT    : acl_index, acl_type
 * OUTPUT   : none.
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : 1, rule mgr SHALL remove acl from class-map and unbind ports first
 *            2, if acl type is changed, its ace list will be clear (unbind --
 *               all ACE(s) no longer belong to this ACL)
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetAclTypeByIndex(UI32_T acl_index, RULE_TYPE_AclType_T acl_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAclNameByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  set acl name by it's index
 * INPUT:    acl_index, acl_name
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetAclNameByIndex(UI32_T acl_index, const char *acl_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAclRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set acl row status by index
 * INPUT:    acl_index, is_active
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetAclRowStatus(UI32_T acl_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_BindElement2Acl
 *------------------------------------------------------------------------------
 * PURPOSE  : bind ace to acl
 * INPUT    : acl_index, ace_type, ace_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_BindElement2Acl(UI32_T acl_index, RULE_TYPE_AclType_T ace_type, UI32_T ace_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_UnbindElementFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : unbind ace to acl
 * INPUT    : acl_index, ace_type, ace_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_UnbindElementFromAcl(UI32_T acl_index, RULE_TYPE_AclType_T ace_type, UI32_T ace_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DelAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete an ACL.
 * INPUT:    acl_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DelAcl(UI32_T acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAclByIndex(UI32_T acl_index, RULE_TYPE_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUIAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetUIAclByIndex(UI32_T acl_index, RULE_TYPE_UI_AclEntry_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetMibAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get MIB ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetMibAclByIndex(UI32_T acl_index, RULE_TYPE_MIB_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_index, acl_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextAclByIndex(UI32_T *acl_index, RULE_TYPE_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextMibAclByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next MIB ACL by acl_index
 * INPUT:    acl_index
 * OUTPUT:   acl_index, acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextMibAclByIndex(UI32_T *acl_index, RULE_TYPE_MIB_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACL which (acl_type == acl_entry->acl_type)
 * INPUT:    acl_index, acl_entry->acl_type
 * OUTPUT:   acl_index, acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextAcl(UI32_T *acl_index, RULE_TYPE_Acl_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACL which (acl_type == acl_entry->acl_type)
 * INPUT:    acl_index, acl_entry->acl_type
 * OUTPUT:   acl_index, acl_entry.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextUIAcl(UI32_T *acl_index, RULE_TYPE_UI_AclEntry_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclNameById
 *------------------------------------------------------------------------------
 * PURPOSE:  Get name of ACL by it's index.
 * INPUT:    acl_index
 * OUTPUT:   acl_name.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAclNameById(UI32_T acl_index, char *acl_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclIdByName
 *------------------------------------------------------------------------------
 * PURPOSE:  Get index of ACL by it's name.
 * INPUT:    acl_name
 * OUTPUT:   acl_index.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAclIdByName(const char *acl_name, UI32_T *acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclIndexByAce
 *------------------------------------------------------------------------------
 * PURPOSE:  get acl index by ace_index
 * INPUT:    ace_index
 * OUTPUT:   acl_index
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAclIndexByAce(UI32_T ace_index, UI32_T *acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclTypeByAce
 *------------------------------------------------------------------------------
 * PURPOSE:  get acl type by ace_index
 * INPUT:    ace_index
 * OUTPUT:   acl_type
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAclTypeByAce(UI32_T ace_index, RULE_TYPE_AclType_T *acl_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAclRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get acl row status by it's index
 * INPUT:    acl_index
 * OUTPUT:   row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAclRowStatus(UI32_T acl_index, UI32_T *row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_AddAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Add a ACE to ACL.
 * INPUT:    acl_index, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_AddAce2Acl(UI32_T acl_index, RULE_TYPE_AclType_T acl_type, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNumberOfPermitAcesByAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get number of permit aces in an ACL
 * INPUT    : acl_index
 * OUTPUT   : permit_qty
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNumberOfPermitAcesByAclIndex (UI32_T acl_index, UI32_T *permit_qty);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DelAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Del a ACE to ACL.
 * INPUT:    acl_index, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DelAceFromAcl(UI32_T acl_index, RULE_TYPE_AclType_T acl_type, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPortAclIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  get port bounded ACL by acl type and ingress flag
 * INPUT:    ifindex, acl_type, ingress_flag
 * OUTPUT:   acl_index, time_range_name
 * RETURN:   Error code.
 * NOTE:     (acl_type, ingress_flag) pair e.g.
 *           (IP, ingress), (IP, egress), (MAC, ingress), (MAC, egress), ...
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetPortAclIndex(
    UI32_T ifindex,
    RULE_TYPE_AclType_T acl_type,
    BOOL_T ingress_flag,
    UI32_T *acl_index,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next binding ACL by port
 * INPUT:    uport_ifindex, acl_index, precedence
 * OUTPUT:   acl_index, acl_entry, precedence, time_range_name.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextAclByPort(
    UI32_T uport_ifindex,
    UI32_T *acl_index,
    RULE_TYPE_Acl_T *acl_entry,
    UI32_T *precedence,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUIAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  get binding ACL by port
 * INPUT:    uport_ifindex, acl_entry->acl->type
 * OUTPUT:   acl_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetUIAclByPort(UI32_T uport_ifindex, RULE_TYPE_UI_AclEntry_T *acl_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUIAclByTypeAndDirection
 *------------------------------------------------------------------------------
 * PURPOSE:  get binding ACL by type and direction
 * INPUT:    uport_ifindex, acl_entry->acl->type, ingress_flag
 * OUTPUT:   acl_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetUIAclByTypeAndDirection(UI32_T uport_ifindex, RULE_TYPE_UI_AclEntry_T *acl_entry, BOOL_T ingress_flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next binding ACL by port
 * INPUT:    uport_ifindex, acl_index, precedence
 * OUTPUT:   acl_index, precedence, acl_entry, time_range_name.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextUIAclByPort(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI32_T *precedence,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAclByPortAndAclType
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next binding ACL by port and acl_type
 * INPUT:    uport_ifindex, acl_index, acl_entry->acl_type
 * OUTPUT:   acl_index, acl_entry, time_range_index.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextAclByPortAndAclType(UI32_T uport_ifindex, UI32_T *acl_index, RULE_TYPE_UI_AclEntry_T *acl_entry, UI8_T *time_range_name);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIAclByPortAndBindingType
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next same binding type ACL by port and binding type.
 * INPUT:    precedence, uport_ifindex
 * OUTPUT:   uport_index, acl_entry, time_range_name
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextUIAclByPortAndBindingType(
    UI32_T precedence, 
    UI32_T *uport_ifindex, 
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI8_T *time_range_name);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_GetNextPort_BindAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Enumerate all interface which have bind the specified ACL
 * INPUT    : acl_index         - ACL index
 *            uport_ifindex_p   - uport ifindex; pass 0 to start.
 * OUTPUT   : uport_ifindex_p   - uport ifindex
 *            inout_profile_p   - direction
 * RETURN   : RULE_TYPE_OK if get an interface; else if error or no more interface
 * NOTES    : Sequence of the enumerating are
 *            uport_inindex -> inout_profile
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextPort_BindAcl(
    UI32_T  acl_index,
    UI32_T  *uport_ifindex_p,
    RULE_TYPE_InOutDirection_T *inout_profile_p
    );

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_GetNextPort_BindClsMapAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Enumerate all interface which have bind the specified ACL through
 *            class-map and policy-map.
 * INPUT    : acl_index             - ACL index
 *            uport_ifindex_p       - uport ifindex; pass 0 to start.
 * OUTPUT   : uport_ifindex_p       - uport ifindex
 *            inout_profile_p       - direction
 *            policy_map_index_p    - policy-map index
 *            class_map_index_p     - class-map index
 * RETURN   : RULE_TYPE_OK if get an interface; else if error or no more interface
 * NOTES    : Sequence of the enumerating are
 *            uport_inindex -> inout_profile -> policy_map_index -> class_map_index
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextPort_BindClsMapAcl(
    UI32_T  acl_index,
    UI32_T  *uport_ifindex_p,
    RULE_TYPE_InOutDirection_T  *inout_profile_p,
    UI32_T  *policy_map_index_p,
    UI32_T  *class_map_index_p
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_InitAce
 *------------------------------------------------------------------------------
 * PURPOSE  : for SNMP and WWW and CLI to make a default ACE
 * INPUT    : ace_entry->ace_type
 * OUTPUT   : ace_entry
 * RETURN   : Error code
 * NOTES    : this API is used to init the ace before use it, need to set ace_entry->ace_type
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_InitAce(RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_InitUIAce
 *------------------------------------------------------------------------------
 * PURPOSE  : for UI to make a default UI ACE entry
 * INPUT    : ace_entry_p->ace_type
 * OUTPUT   : ace_entry_p
 * RETURN   : Error code
 * NOTES    : this API is used to init the ace before use it, need to set ace_entry->ace_type
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_InitUIAce(
    RULE_TYPE_UI_Ace_Entry_T *ace_entry_p
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_SetAcePatternToDefaultByType
 *------------------------------------------------------------------------------
 * PURPOSE  : For UI to initial ace.u.{mac|ip|ipv6|arp} to default
 * INPUT    : ace_type
 * OUTPUT   : ace_entry_p
 * RETURN   : Error code
 * NOTES    : Initial the ace pattern by ace_type
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetAcePatternToDefaultByType(
    RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_AclType_T ace_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_ConvertUIAce2Ace
 *------------------------------------------------------------------------------
 * PURPOSE  : Convert RULE_TYPE_UI_Ace_Entry_T to RULE_TYPE_Ace_Entry_T
 * INPUT    : ui_ace_p
 * OUTPUT   : ace_p
 * RETURN   : RULE_TYPE_RETURN_TYPE_T
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_ConvertAce2UIAce(
    const RULE_TYPE_Ace_Entry_T *ace_p,
    RULE_TYPE_UI_Ace_Entry_T *ui_ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_ConvertAce2UIAce
 *------------------------------------------------------------------------------
 * PURPOSE  : Convert RULE_TYPE_Ace_Entry_T to RULE_TYPE_UI_Ace_Entry_T
 * INPUT    : ace_p
 * OUTPUT   : ui_ace_p
 * RETURN   : RULE_TYPE_RETURN_TYPE_T
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_ConvertUIAce2Ace(
    const RULE_TYPE_UI_Ace_Entry_T *ui_ace_p,
    RULE_TYPE_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_IsDefaultIpACE
 *------------------------------------------------------------------------------
 * PURPOSE  : Check if an ACE is default ACE
 * INPUT    : ip_ace
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTES    : This API associates with RULE_OM_LocalSetAceEntryToDefaultValue,
              if the latter is changed, this API should be changed.
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_OM_IsDefaultIpACE(const RULE_TYPE_IpAce_Entry_T *ip_ace);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_IsMatchAnyAnyAce
 *------------------------------------------------------------------------------
 * PURPOSE  : Check ACE whther is "permit any any" or "deny any any".
 * INPUT    : ace_index	- ACE index
 * OUTPUT   : None
 * RETURN   : TRUE / FALSE
 * NOTES    : None
 * NOTE:    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_OM_IsMatchAnyAnyAce(
    UI32_T ace_index
);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_ResetAceEntryByNewAceType
 *------------------------------------------------------------------------------
 * PURPOSE  : Reset unsupport fields of ace to default value according to ace_type
 * INPUT    : ace_entry, new_ace_type
 * OUTPUT   : none
 * RETURN   : Error code
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_ResetAceEntryByNewAceType(UI32_T ace_index, RULE_TYPE_AclType_T new_ace_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next UI ACE for ACL
 * INPUT:    acl_index, ace_index
 * OUTPUT:   ace_index, ace_p.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextUIAceByAcl(
    UI32_T acl_index,
    UI32_T *ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ui_ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUIAceByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ACE by its index
 * INPUT:    ace_index
 * OUTPUT:   ace_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T 
RULE_OM_GetUIAceByIndex(
    UI32_T ace_index, 
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIAceByAceTypeAndIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next UI ACE by its index and ace type
 * INPUT:    ace_index_p, ace_type (RULE_TYPE_IP_ACL, RULE_TYPE_MAC_ACL, RULE_TYPE_IPV6_ACL)
 * OUTPUT:   ace_index_p, ace_p
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextUIAceByAceTypeAndIndex(
    UI32_T ace_type, 
    UI32_T *ace_index_p, 
    RULE_TYPE_UI_Ace_Entry_T *ace_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE for ACL
 * INPUT:    acl_index, ace_index
 * OUTPUT:   ace_index, acl_type, ace_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextAceByAcl(
    UI32_T acl_index,
    RULE_TYPE_AclType_T *ace_type,
    UI32_T *ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetLastAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get last ACE for ACL
 * INPUT:    acl_index, ace_index
 * OUTPUT:   ace_index, acl_type, ace_entry.
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetLastAceByAcl(
    UI32_T acl_index,
    RULE_TYPE_AclType_T *ace_type,
    UI32_T *ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_CreateAce
 *------------------------------------------------------------------------------
 * PURPOSE:  create ace on specified index
 * INPUT:    ace_index, ace_tye, is_active
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_CreateAce(UI32_T ace_index, RULE_TYPE_AclType_T ace_type, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DestroyAce
 *------------------------------------------------------------------------------
 * PURPOSE  : destroy ace by index
 * INPUT    : ace_index
 * OUTPUT   : none
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : rule mgr SHALL remove ace from acl/class-map first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DestroyAce(UI32_T ace_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAceFieldByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  set ACE field by its index
 * INPUT:    ace_index, ace_entry, field_index (which leaf), owner (ACL/MF)
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetAceFieldByIndex(UI32_T ace_index, const RULE_TYPE_Ace_Entry_T *ace_entry, UI32_T field_index, UI32_T owner);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAceRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set ace row status by index
 * INPUT:    ace_index, is_active
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_SetAceRowStatus(UI32_T ace_index, BOOL_T is_active);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAceByIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ACE by its index
 * INPUT:    ace_index
 * OUTPUT:   ace_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAceByIndex(UI32_T ace_index, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextMibAceByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE by port
 * INPUT:    ifindex       - interface index
 *           inout_profile - direction
 *           acl_index_p   - user config ACL index
 *           ace_index_p   - hardware ACE index
 * OUTPUT:   ace_entry_p   - ACE entry
 * RETURN:   Error code.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextMibAceByPort(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index_p,
    UI32_T *ace_index_p,
    RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAceIndexFromAclByAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : search an ace match ace_entry in specified acl
 * INPUT    : acl_index, ace_entry
 * OUTPUT   : ace_index
 * RETURN   : Error code (OK / FAIL)
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetAceIndexFromAclByAceEntry(
    UI32_T acl_index,
    const RULE_TYPE_Ace_Entry_T *ace_entry,
    UI32_T *ace_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAceByAceTypeAndIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE by its index and ace type
 * INPUT:    ace_index, ace_type
 * OUTPUT:   ace_index, ace_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextAceByAceTypeAndIndex(UI32_T ace_type, UI32_T *ace_index, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetAceTypeAndRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  get ace type and row status by it's index
 * INPUT:    ace_index
 * OUTPUT:   ace_type, row_status
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetAceTypeAndRowStatus(UI32_T ace_index, RULE_TYPE_AclType_T *ace_type, UI32_T *row_status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAccessTypeByAceEntry
 *------------------------------------------------------------------------------
 * PURPOSE  : set access type to an ace_entry
 * INPUT    : ace_entry
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
void RULE_OM_SetAccessTypeByAceEntry(
    RULE_TYPE_Ace_Entry_T *ace_entry_p,
    RULE_TYPE_AceAction_T ace_access
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Record the ACL information on port
 * INPUT    : ifindex, direction, acl_index, time_range_index, time_range_name, counter
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_SetAcl(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    UI32_T time_range_index,
    UI8_T  *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T counter
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_ResetAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Reset the ACL information on port
 * INPUT    : ifindex, direction, acl_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
RULE_TYPE_RETURN_TYPE_T
RULE_OM_ResetAcl(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index
);

#if (SYS_CPNT_ACL_MIRROR == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_AddAclMirrorEntry
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
RULE_OM_AddAclMirrorEntry(
    UI32_T ifindex_dest,
    UI32_T acl_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RemoveAclMirrorEntry
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
RULE_OM_RemoveAclMirrorEntry(
    UI32_T ifindex_dest,
    UI32_T acl_index
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextAclMirrorEntry
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
RULE_OM_GetNextAclMirrorEntry(
    UI32_T *ifindex_dest,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry
);
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_InstanceIdByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get port entry
 * INPUT:    uport_ifindex
 * OUTPUT:   port_entry
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Get_PortEntry(UI32_T uport_ifindex, RULE_TYPE_PortEntry_T *port_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_PortEntryEx
 *------------------------------------------------------------------------------
 * PURPOSE:  Get port entry
 * INPUT:    uport_ifindex
 * OUTPUT:   port_entry
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_Get_PortEntryEx(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    RULE_TYPE_PortEntry_T *port_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetUIPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Get port entry by ifindex
 * INPUT:    ifindex
 *           inout_profile  - direction
 * OUTPUT:   port_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetUIPortEntry(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    RULE_TYPE_UI_PortEntry_T *port_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextUIPortEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next port entry by ifindex
 * INPUT:    ifindex
 * OUTPUT:   ifindex, port_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_GetNextUIPortEntry(
    UI32_T *ifindex,
    RULE_TYPE_InOutDirection_T *inout_profile,
    RULE_TYPE_UI_PortEntry_T *port_entry
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetPolicyMapInstance
 *------------------------------------------------------------------------------
 * PURPOSE:  Assign policy map instance
 * INPUT:    port_entry_p, policy_index, class_index, policy_inst_p
 * OUTPUT:   NONE
 * RETURN:   NONE
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
void
RULE_OM_SetPolicyMapInstance(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_inst_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetPolicyMapInstance
 *------------------------------------------------------------------------------
 * PURPOSE:  Get policy map instance
 * INPUT:    port_entry_p, policy_index, class_index
 * OUTPUT:   The pointer of policy map instance
 * RETURN:   NONE
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_OM_GetPolicyMapInstance(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_Get_AceEntryByID
 *------------------------------------------------------------------------------
 * PURPOSE:  Get ace entry by index
 * INPUT:    ace_index
 * OUTPUT:   ace_entry
 * RETURN:   Error code.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_Get_AceEntryByID(UI32_T ace_index, RULE_TYPE_Ace_Entry_T *ace_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_ClassifyClassMap
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to class-map
 * INPUT    : class_map
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6 because they use different selector
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_ClassifyClassMap(const RULE_TYPE_ClassMap_T *class_map, RULE_TYPE_AclType_T *class_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_ClassifyClassMapByIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to class-map index
 * INPUT    : class_map_index
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6 because they use different selector
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_ClassifyClassMapByIndex(UI32_T class_map_index, RULE_TYPE_AclType_T *class_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_ClassifyAcl
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to acl
 * INPUT    : acl_entry
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6-std/IPv6-ext because they use different selector
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_ClassifyAcl(const RULE_TYPE_Acl_T *acl_entry, RULE_TYPE_AclType_T *class_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_ClassifyAce
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to ace
 * INPUT    : ace_entry
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6-std/IPv6-ext because they use different selector
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_ClassifyAce(const RULE_TYPE_Ace_Entry_T *ace_entry, RULE_TYPE_AclType_T *class_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - RULE_OM_ClassifyMf
 *------------------------------------------------------------------------------
 * PURPOSE  : get class type according to mf
 * INPUT    : mf_entry
 * OUTPUT   : class_type
 * RETURN   : Error code (OK / FAIL)
 * NOTES    : class_type could be MAC/IP/IPv6-std/IPv6-ext because they use different selector
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_ClassifyMf(const RULE_TYPE_Ace_Entry_T *mf_entry, RULE_TYPE_AclType_T *class_type);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_OM_Is_AclType_Matched
 *------------------------------------------------------------------------------
 * PURPOSE  : Test if two acltypes are matched
 * INPUT    : src_type, dst_type
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_OM_Is_AclType_Matched(RULE_TYPE_AclType_T src_type, RULE_TYPE_AclType_T dst_type);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_AddCosReference
 *------------------------------------------------------------------------------
 * PURPOSE  : add cos reference if action reference to COS
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : keep the policy-map elements' information if their actions reference to COS
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_AddCosReference(UI32_T policy_map_index, UI32_T class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RemoveCosReference
 *------------------------------------------------------------------------------
 * PURPOSE  : remove cos reference if action reference to COS
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 *------------------------------------------------------------------------------
 */
void RULE_OM_RemoveCosReference(UI32_T policy_map_index, UI32_T class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextIpDscpReference
 *------------------------------------------------------------------------------
 * PURPOSE  : get next IP DSCP reference
 * INPUT    : ip_dscp, index
 * OUTPUT   : policy_map_index, class_map_index
 * RETURN   : error code (OK / FAIL)
 * NOTE     : *index = 0 implies get first entry
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextIpDscpReference(UI8_T ip_dscp, UI32_T *index, UI32_T *policy_map_index, UI32_T *class_map_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_GetNextIpPrecedenceReference
 *------------------------------------------------------------------------------
 * PURPOSE  : get next IP Precedence reference
 * INPUT    : ip_precedence, index
 * OUTPUT   : policy_map_index, class_map_index
 * RETURN   : error code (OK / FAIL)
 * NOTE     : *index = 0 implies get first entry
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetNextIpPrecedenceReference(UI8_T ip_precedence, UI32_T *index, UI32_T *policy_map_index, UI32_T *class_map_index);

/*------------------------------------------------------------------------------
 * FUNCTION NAME:  RULE_OM_SetDebugFlag
 *------------------------------------------------------------------------------
 * PURPOSE  : set backdoor debug flag
 * INPUT    : debug_flag
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
void RULE_OM_SetDebugFlag(UI32_T debug_flag);

/*------------------------------------------------------------------------------
 * FUNCTION NAME:  RULE_OM_GetDebugFlag
 *------------------------------------------------------------------------------
 * PURPOSE  : get backdoor debug flag
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : debug flag
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_GetDebugFlag();

#if (SYS_CPNT_DAI == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_ArpACLCheckForDai
 *------------------------------------------------------------------------------
 * PURPOSE:  for ARP INSPETCION (DAI)
 *            to check if match the existed arp rules with arp packets
 * INPUT:    arp_acl_name - check which arp acl
 *           arp_pkt_p    - compared arp packet
 * OUTPUT:   b_acl_drop   - is drop
 * RETURN:   RULE_TYPE_OK/RULE_TYPE_FAIL
 * NOTE:     RULE_TYPE_OK means match rule,
 *           RULE_TYPE_FAIL means no match rule.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_ArpACLCheckForDai(const char *arp_acl_name, RULE_TYPE_ArpPktFormat_T *arp_pkt_p, BOOL_T *b_acl_drop);
#endif /*end of #if (SYS_CPNT_DAI == TRUE)*/

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RebuildAggregateAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Rebuild aggregate ACL
 * INPUT  :  acl_index  - ACL index
 * OUTPUT :  none
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_RebuildAggregateAcl(UI32_T acl_index);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_SetUserAclIndexOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Set user config ACL index on port
 * INPUT  :  ifIndex    - interface index
 *           dirction   - traffic direction
 *           acl_index  - ACL index
 *           acl_type   - ACL type
 * OUTPUT :  none
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  This funciton used to set the user configured ACL index on port,
 *           then you can easily get the user ACL index(Not eggregate ACL index)
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_SetUserAclIndexOnPort(
    UI32_T ifIndex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RemoveUserAclIndexOnPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Set user config ACL index on port
 * INPUT  :  ifIndex    - interface index
 *           dirction   - traffic direction
 *           acl_index  - ACL index
 *           acl_type   - ACL type
 * OUTPUT :  none
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  This funciton used to remove the user configured ACL index on port.
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_OM_RemoveUserAclIndexOnPort(
    UI32_T ifIndex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_BuildMatchAllClassMapRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Build aggregation ACL for match-all class-map
 * INPUT  :  class_map_index    - index of class-map
 * OUTPUT :  acl_index_p        - index of aggregation ACL
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_BuildMatchAllClassMapRule(
    UI32_T class_map_index,
    UI32_T *acl_index_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RebuildMatchAllClassMapRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Rebuild aggregation ACL for match-all class-map
 * INPUT  :  class_map_index    - index of class-map
 * OUTPUT :  acl_index_p        - index of aggregation ACL
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  none
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_RebuildMatchAllClassMapRule(
    UI32_T class_map_index,
    UI32_T *acl_index_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_DelMatchAllClassMapRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy aggregation ACL of the match-all class-map
 * INPUT  :  class_map_index    - index of class-map
 * OUTPUT :  none
 * RETURN :  Error code (OK / FAIL)
 * NOTE   :  The aggregation ACL will be destroyed when it have no any
 *           referenced instances.
 *------------------------------------------------------------------------------
 */
UI32_T RULE_OM_DelMatchAllClassMapRule(
    UI32_T class_map_index
);

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_TakeSnapshot
 *------------------------------------------------------------------------------
 * PURPOSE:  Take a snapshot of rule OM
 * INPUT:    snapshot_p  - Snapshot
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
void RULE_OM_TakeSnapshot(
    RULE_OM_Snapshot_T *snapshot_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_OM_RevertSnapshot
 *------------------------------------------------------------------------------
 * PURPOSE:  Revert to snapshot
 * INPUT:    snapshot_p  - Snapshot
 * OUTPUT:   None
 * RETURN:   None
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
void RULE_OM_RevertSnapshot(
    RULE_OM_Snapshot_T *snapshot_p
);
#endif /* SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT */

#endif
