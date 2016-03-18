/* FUNCTION NAME: RULE_MGR.C
* PURPOSE:
*   manage/save DiffServ, ACL database(OM) and hardware resource(FP)
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
/* INCLUDE FILE DECLARATIONS
 */
 #include <string.h>
#include "sys_type.h"
#include "sys_adpt.h"
#include "sys_cpnt.h"
#include "l_rstatus.h"
#include "rule_type.h"
#include "rule_om.h"
#include "rule_mgr.h"
#include "rule_ctrl.h"
#include "swctrl.h"
#include "leaf_es3626a.h"
#include "stktplg_pom.h"

#if (SYS_CPNT_TIME_BASED_ACL== TRUE)
#include "time_range_type.h"
#include "time_range_pmgr.h"
#endif

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */
#define RULE_MGR_IS_BAD_POLICY_MAP_IDX(idx)         ((0 >= idx) || (idx > SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP))
#define RULE_MGR_IS_VALID_POLICY_MAP_IDX(idx)       ((0 < idx) && (idx <= SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP))

#define RULE_MGR_IS_ENTRY_ACTIVE(row_status)         (VAL_diffServPolicyMapStatus_active == row_status)
#define RULE_MGR_IS_ENTRY_DESTROY(row_status)        (0 == row_status)

#define RULE_MGR_IS_IP_ACL(acl_type)                ((acl_type == RULE_TYPE_IP_EXT_ACL) || \
                                                     (acl_type == RULE_TYPE_IP_STD_ACL))

#define RULE_MGR_IS_MAC_ACL(acl_type)               (acl_type == RULE_TYPE_MAC_ACL)

#define RULE_MGR_IS_IPV6_ACL(acl_type)              ((acl_type == RULE_TYPE_IPV6_EXT_ACL) || \
                                                     (acl_type == RULE_TYPE_IPV6_STD_ACL))

#define RULE_MGR_CVRT_ACL_TYPE(acl_type)            \
    ((acl_type == RULE_TYPE_IP_STD_ACL || acl_type == RULE_TYPE_IP_EXT_ACL)?RULE_TYPE_IP_ACL: \
     (acl_type == RULE_TYPE_IPV6_STD_ACL || acl_type == RULE_TYPE_IPV6_EXT_ACL)?RULE_TYPE_IPV6_ACL: \
     acl_type)

#define RULE_MGR_ACL_TYPE_STR(acl_type)                     \
    (acl_type == RULE_TYPE_IP_ACL)      ? "IP_ACL"      :   \
    (acl_type == RULE_TYPE_IP_STD_ACL)  ? "IP_STD_ACL"  :   \
    (acl_type == RULE_TYPE_IP_EXT_ACL)  ? "IP_EXT_ACL"  :   \
    (acl_type == RULE_TYPE_IPV6_ACL)    ? "IP6_ACL"     :   \
    (acl_type == RULE_TYPE_IPV6_STD_ACL)? "IP6_STD_ACL" :   \
    (acl_type == RULE_TYPE_IPV6_EXT_ACL)? "IP6_EXT_ACL" :   \
    (acl_type == RULE_TYPE_MAC_ACL)     ? "MAC_ACL"     :   \
                                          "Other"

/* whether bno-th bit in bmp is 1 or 0 (bmp is an UI8_T array, bno is a zero-based bit index) */
#define RULE_MGR_IS_BIT_ON(bmp, bno)                (bmp[bno/8] & (1 << (7 - bno%8)))
#define RULE_MGR_IS_BIT_OFF(bmp, bno)               (!RULE_MGR_IS_BIT_ON(bmp, bno))
#define RULE_MGR_SET_BIT_ON(bmp, bno)               {bmp[bno/8] |= (1 << (7 - bno%8));}

#define RULE_MGR_IS_DEBUG_ERROR_ON(flag)            (flag & RULE_OM_DEBUG_MGR_ERR)

#if (1400 <= _MSC_VER /* VC2005 */)
#define RULE_MGR_DEBUG_MSG(fmt, ...)                            \
    {                                                           \
        UI32_T debug_flag = RULE_OM_GetDebugFlag();             \
        if (RULE_MGR_IS_DEBUG_ERROR_ON(debug_flag))             \
        {                                                       \
            printf("%s:%d ", __FUNCTION__, __LINE__);           \
            printf(fmt, __VA_ARGS__);                           \
            printf("\r\n");                                     \
        }                                                       \
    }
#else
#define RULE_MGR_DEBUG_MSG(fmt, args...)                        \
    {                                                           \
        UI32_T debug_flag = RULE_OM_GetDebugFlag();             \
        if (RULE_MGR_IS_DEBUG_ERROR_ON(debug_flag))             \
        {                                                       \
            printf("%s:%d ", __FUNCTION__, __LINE__);           \
            printf(fmt, ##args);                                \
            printf("\r\n");                                     \
        }                                                       \
    }
#endif

#if (1400 <= _MSC_VER /* VC2005 */)
#define LOG(fmt, ...)      RULE_MGR_DEBUG_MSG(fmt, __VA_ARGS__)
#else
#define LOG(fmt, args...)  RULE_MGR_DEBUG_MSG(fmt, ##args)
#endif

#ifndef _countof
#define _countof(ary)           (sizeof(ary)/sizeof(*ary))
#endif

#ifndef ASSERT
#define ASSERT(eq)
#endif

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */
#if 0
static UI32_T RULE_MGR_LocalCheckMeterWithPortRate(UI32_T policy_map_index, RULE_TYPE_TBParamEntry_T *meter_entry);
#endif

static UI32_T
RULE_MGR_LocalAddChipPolicyMapConfig(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index
);

static UI32_T
RULE_MGR_LocalRemoveChipPolicyMapConfig(
    UI32_T uport_ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index
);

static UI32_T RULE_MGR_LocalRemoveChipAclElementConfig(UI32_T acl_index, UI32_T ace_index);

static UI32_T
RULE_MGR_LocalUpdateChipCompressAclConfig(
    UI32_T agg_acl_index
);

static UI32_T
RULE_MGR_LocalNotify(
    RULE_TYPE_NOTIFY_MESSAGE_T message,
    void *param1,
    void *param2
);

static UI32_T RULE_MGR_LocalSetPolicyMapNotReady(UI32_T policy_map_index);
static UI32_T RULE_MGR_LocalSetPolicyMapElementNotReady(UI32_T policy_element_index);
static UI32_T RULE_MGR_LocalSetClassMapNotReady(UI32_T class_map_index);
static UI32_T RULE_MGR_LocalSetMeterNotReady(UI32_T meter_index);
static UI32_T RULE_MGR_LocalSetActionNotReady(UI32_T action_index);

static UI32_T
RULE_MGR_LocalSetPolicyMapPortActive2NotReady(
    UI32_T ifindex,
    UI32_T direction
);

static UI32_T
RULE_MGR_LocalSetPolicyMapPortNotReady2Active(
    UI32_T ifindex,
    UI32_T direction
);

static UI32_T
RULE_MGR_LocalSetAccessGroupActive2NotReady(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type
);

static RULE_TYPE_RETURN_TYPE_T
RULE_MGR_LocalSetAccessGroupNotReady2Active(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type
);

static UI32_T RULE_MGR_LocalSetAclNotReady(UI32_T acl_index);
static UI32_T RULE_MGR_LocalSetAceNotReady(UI32_T ace_index);

static BOOL_T RULE_MGR_LocalPolicyMapRStatusCallBack(void *rec);
static BOOL_T RULE_MGR_LocalPolicyMapElementRStatusCallBack(void *rec);
static BOOL_T RULE_MGR_LocalClassMapRStatusCallBack(void *rec);
static BOOL_T RULE_MGR_LocalActionRStatusCallBack(void *rec);
static BOOL_T RULE_MGR_LocalMeterRStatusCallBack(void *rec);
static BOOL_T RULE_MGR_LocalPolicyMapPortRStatusCallBack(void *rec);
static BOOL_T RULE_MGR_LocalAccessGroupRStatusCallBack(void *rec);
static BOOL_T RULE_MGR_LocalAclRStatusCallBack(void *rec);
static BOOL_T RULE_MGR_LocalAceRStatusCallBack(void *rec);
static UI32_T RULE_MGR_LocalCheckInputAclType(UI32_T acl_entry_acl_type, UI32_T input_acl_type);

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
BOOL_T RULE_MGR_InitiateSystemResources()
{
    BOOL_T  ret = TRUE;

    ret &= RULE_OM_Initialize();

    return ret;
}

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
void RULE_MGR_Create_InterCSC_Relation()
{
    return ;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_EnterMasterMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the rule mgr enter the master mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void RULE_MGR_EnterMasterMode()
{
}

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
void RULE_MGR_EnterSlaveMode()
{
}

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
void RULE_MGR_SetTransitionMode()
{
}

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
void RULE_MGR_EnterTransitionMode()
{
    RULE_OM_Initialize();
    RULE_CTRL_FieldInit();
    return;
}

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
BOOL_T RULE_MGR_ProcessEventTimer()
{
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    RULE_CTRL_ProcessTimerEvent();
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */
    return TRUE;
}

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
BOOL_T RULE_MGR_ProcessTimeRangeStatusChange(UI8_T *isChanged_list, UI8_T *status_list)
{
    RULE_CTRL_ProcessTimeRangeStatusChange(isChanged_list, status_list);
    return TRUE;
}
#endif

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
void RULE_MGR_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port)
{
    RULE_CTRL_HandleHotInsertion(starting_port_ifindex, number_of_port);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_HandleHotRemoval
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 *                                     module
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is removed at a time.
 *------------------------------------------------------------------------------
 */
void RULE_MGR_HandleHotRemoval(UI32_T starting_port_ifindex,UI32_T number_of_port)
{
    RULE_CTRL_HandleHotRemoval(starting_port_ifindex, number_of_port);
    RULE_OM_HandleHotRemoval(starting_port_ifindex, number_of_port);
}

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
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index)
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T unit, port, trunk, row_status;
    UI32_T bind_policy_map_index, bind_row_status;

    LOG("ifindex(%lu), direction(%s), policy_map_index(%lu)",
        ifindex,
        (inout_profile == RULE_TYPE_INBOUND) ? "In" : "Out",
        policy_map_index);

    if (RULE_TYPE_OK != RULE_TYPE_VALIDATE_IFINDEX(ifindex))
    {
        LOG("<Error> Invalid parameter. ifindex(%lu)", ifindex);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_IS_UPORT(ifindex))
    {
        if (SWCTRL_LPORT_NORMAL_PORT != SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk))
        {
            LOG("<Error> Invalid parameter. ifindex(%lu)", ifindex);
            return RULE_TYPE_FAIL;
        }
    }

    /* check if policy-map is active
     */
    if ((RULE_TYPE_OK != RULE_OM_GetPolicyMapRowStatus (policy_map_index, &row_status)) ||
        (FALSE == RULE_MGR_IS_ENTRY_ACTIVE(row_status)))
        return RULE_TYPE_FAIL;

    /* Set row status to notReady
     */
    result = RULE_OM_GetPortBindedPolicyMap(ifindex, inout_profile, &bind_policy_map_index, &bind_row_status);
    if(RULE_TYPE_OK == result)
    {
        if (TRUE == RULE_MGR_IS_ENTRY_ACTIVE(bind_row_status))
        {
            if (bind_policy_map_index == policy_map_index)
            {
                return RULE_TYPE_OK;
            }

            result = RULE_MGR_LocalSetPolicyMapPortActive2NotReady(ifindex, inout_profile);
            if (result != RULE_TYPE_OK)
            {
                return result;
            }
        }
    }
    else
    {
        result = RULE_OM_SetPortEntryPolicyMapRowStatus(ifindex, inout_profile,
                                                        VAL_diffServPolicyMapPortPortStatus_notReady);
        if (result != RULE_TYPE_OK)
        {
            ASSERT(0);
            return result;
        }
    }

    result = RULE_OM_SetPortEntryPolicyMapIndex(ifindex, inout_profile, policy_map_index);
    ASSERT(result == RULE_TYPE_OK);

    result = RULE_MGR_LocalSetPolicyMapPortNotReady2Active(ifindex, inout_profile);
    if (result != RULE_TYPE_OK)
    {
        LOG("Fail to bind policy map on port");
        RULE_OM_SetPortEntryPolicyMapIndex(ifindex, inout_profile, bind_policy_map_index);

        if (TRUE == RULE_MGR_IS_ENTRY_ACTIVE(bind_row_status))
        {
            RULE_MGR_LocalSetPolicyMapPortNotReady2Active(ifindex, inout_profile);
        }

        if (TRUE == RULE_MGR_IS_ENTRY_DESTROY(bind_row_status))
        {
            RULE_OM_SetPortEntryPolicyMapRowStatus(ifindex, inout_profile,
                                                   VAL_diffServPolicyMapStatus_destroy);
        }

        return result;
    }

    return RULE_TYPE_OK;
}

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
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T policy_map_index)
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T old_policy_map_index, old_row_status;

    if (RULE_TYPE_OK != RULE_TYPE_VALIDATE_IFINDEX(ifindex))
    {
        LOG("<Error> Invalid parameter. ifindex(%lu)", ifindex);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_IS_UPORT(ifindex))
    {
        UI32_T unit, port, trunk;

        if (SWCTRL_LPORT_NORMAL_PORT != SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk))
        {
            LOG("<Error> Invalid parameter. ifindex(%lu)", ifindex);
            return RULE_TYPE_FAIL;
        }
    }

    result = RULE_OM_GetPortBindedPolicyMap(ifindex,
                                            inout_profile,
                                            &old_policy_map_index,
                                            &old_row_status);
    if (result != RULE_TYPE_OK)
    {
        return RULE_TYPE_OK;
    }

    if (old_policy_map_index != policy_map_index)
    {
        return RULE_TYPE_OK;
    }

    result = RULE_MGR_LocalSetPolicyMapPortActive2NotReady(ifindex, inout_profile);
    if (result != RULE_TYPE_OK)
    {
        return result;
    }

    result = RULE_OM_SetPortEntryPolicyMapRowStatus(ifindex, inout_profile,
                                                    VAL_diffServPolicyMapPortPortStatus_destroy);
    ASSERT(result == RULE_TYPE_OK);

    return RULE_TYPE_OK;
}

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
// FIXIT: RULE_MGR_BindPort2DynamicPolicyMap
UI32_T RULE_MGR_BindPort2DynamicPolicyMap(UI32_T uport_ifindex, UI32_T policy_map_index)
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T row_status, old_pmap_idx;
    RULE_TYPE_InOutDirection_T inout_profile = RULE_TYPE_INBOUND;

//    //
//    // FIXME: Use function instead :-(
//    //
//    if (SYS_ADPT_CPU_1_IF_INDEX_NUMBER == uport_ifindex)
//    {
//    }
//    else
//    {
//        if (SWCTRL_LPORT_UNKNOWN_PORT == SWCTRL_LogicalPortToUserPort(uport_ifindex, &unit, &port, &trunk))
//            return RULE_TYPE_FAIL;
//    }

    /* check if policy-map is active
     */
    if ((RULE_TYPE_OK != RULE_OM_GetPolicyMapRowStatus (policy_map_index, &row_status)) ||
        (FALSE == RULE_MGR_IS_ENTRY_ACTIVE(row_status)))
        return RULE_TYPE_FAIL;

    /* check if this port has a policy-map already
     */
    if (RULE_TYPE_OK == RULE_OM_GetPortPolicyMapIndex(uport_ifindex, inout_profile, &old_pmap_idx))
    {
        if (old_pmap_idx != policy_map_index)
        {
            /* bind to another policy-map, remove the old one first. */
            if (RULE_TYPE_OK != RULE_MGR_UnBindPortFromPolicyMap(uport_ifindex, inout_profile, old_pmap_idx))
                return RULE_TYPE_FAIL;
        }
        else
        {
            /* if the binded dynamic policy-map is the same as user configured,
             * just set the flag to OM.
             */
            if (RULE_OM_IsBindDynamicPolicyMapOnPort(uport_ifindex, inout_profile) == FALSE)
            {
                RULE_OM_SetUserCfgPolicyMapIndexOnPort(uport_ifindex, inout_profile, old_pmap_idx);
                RULE_OM_SetBindDynamicPolicyMapStatusOnPort(uport_ifindex, inout_profile, TRUE);
            }
            return RULE_TYPE_OK;
        }
    }
    else
    {
        old_pmap_idx = 0;
    }

    result = RULE_MGR_LocalAddChipPolicyMapConfig(uport_ifindex, inout_profile, policy_map_index);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    if (RULE_OM_IsBindDynamicPolicyMapOnPort(uport_ifindex, inout_profile) == FALSE)
    {
        RULE_OM_SetUserCfgPolicyMapIndexOnPort(uport_ifindex, inout_profile, old_pmap_idx);
        RULE_OM_SetBindDynamicPolicyMapStatusOnPort(uport_ifindex, inout_profile, TRUE);
    }

    result = RULE_OM_BindPort2PolicyMap(uport_ifindex, inout_profile, policy_map_index);
    ASSERT(RULE_TYPE_OK == result);

    return result;
}

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
// FIXIT: RULE_MGR_UnBindPortFromDynamicPolicyMap
UI32_T RULE_MGR_UnBindPortFromDynamicPolicyMap(UI32_T uport_ifindex)
{
    UI32_T unit, port, trunk, row_status;
    RULE_TYPE_InOutDirection_T inout_profile = RULE_TYPE_INBOUND;
    RULE_TYPE_PortEntry_T port_entry;

    if (SWCTRL_LPORT_UNKNOWN_PORT == SWCTRL_LogicalPortToUserPort(uport_ifindex, &unit, &port, &trunk))
        return RULE_TYPE_FAIL;

    /* check the binded policy-map is dynamic
     */
    /* FIXIT: RULE_MGR_UnBindPortFromDynamicPolicyMap */
    if (RULE_TYPE_OK != RULE_OM_Get_PortEntryEx(uport_ifindex, inout_profile, &port_entry))
        return RULE_TYPE_FAIL;

    if (FALSE == port_entry.bind_dynamic_policy_map)
        return RULE_TYPE_OK;

    /* if the binded dynamic policy-map is the same as user configured,
     * just clear the flag from OM.
     */
    if (RULE_MGR_IS_ENTRY_ACTIVE(port_entry.backup_policy_map_row_status) &&
        port_entry.policy_map_index == port_entry.backup_policy_map_index)
    {
        RULE_OM_SetUserCfgPolicyMapIndexOnPort(uport_ifindex, inout_profile, 0);
        RULE_OM_SetBindDynamicPolicyMapStatusOnPort(uport_ifindex, inout_profile, FALSE);
        return RULE_TYPE_OK;
    }

    if (RULE_TYPE_OK != RULE_MGR_LocalRemoveChipPolicyMapConfig(uport_ifindex, inout_profile, port_entry.policy_map_index))
    {
        return RULE_TYPE_FAIL;
    }

    /* free resource
     */
    if (RULE_TYPE_OK != RULE_OM_UnBindPort2PolicyMap(uport_ifindex, inout_profile, port_entry.policy_map_index))
    {
        return RULE_TYPE_FAIL;
    }

    RULE_OM_SetBindDynamicPolicyMapStatusOnPort(uport_ifindex, inout_profile, FALSE);

    if ((RULE_TYPE_OK != RULE_OM_GetPolicyMapRowStatus (port_entry.backup_policy_map_index, &row_status)) ||
        (FALSE == RULE_MGR_IS_ENTRY_ACTIVE(row_status)))
    {
        return RULE_TYPE_OK;
    }

    return RULE_MGR_BindPort2PolicyMap(uport_ifindex, inout_profile, port_entry.backup_policy_map_index);
}
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
    char *policy_map_name)
{
    return RULE_OM_GetPolicyMapNameByPort(uport_ifindex, inout_profile, policy_map_name);
}

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
UI32_T RULE_MGR_GetPolicyMapNameById(UI32_T policy_map_id, char *policy_map_name)
{
    return RULE_OM_GetPolicyMapNameById(policy_map_id, policy_map_name);
}

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
UI32_T RULE_MGR_GetPolicyMapIdByName(const char *policy_map_name, UI32_T *policy_map_id)
{
    return RULE_OM_GetPolicyMapIdByName(policy_map_name, policy_map_id);
}

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
UI32_T RULE_MGR_CreatePolicyMap(const char *policy_map_name)
{
    UI32_T index=0;
    UI32_T ret = 0;

    ret = RULE_OM_Create_PolicyMap(policy_map_name, &index);
    return ret;
}

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
UI32_T RULE_MGR_SetPolicyMapNameById(UI32_T policy_map_index, const char *policy_map_name)
{
    return RULE_OM_SetPolicyMapNameById(policy_map_index, policy_map_name);
}

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
UI32_T RULE_MGR_SetPolicyMapDescById(UI32_T policy_map_index, const char *policy_map_desc)
{
    return RULE_OM_SetPolicyMapDescById(policy_map_index, policy_map_desc);
}

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
UI32_T RULE_MGR_SetPolicyMapRowStatus(UI32_T policy_map_index, UI32_T row_status)
{
    UI32_T      ret, org_status;

    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapRowStatus(policy_map_index, &org_status))
        org_status = L_RSTATUS_NOT_EXIST;
    else if( org_status == VAL_diffServPolicyMapStatus_active && row_status == VAL_diffServPolicyMapStatus_notInService)
        return RULE_TYPE_FAIL;

    switch (L_RSTATUS_Fsm(row_status, &org_status, RULE_MGR_LocalPolicyMapRStatusCallBack, &policy_map_index))
    {
        case L_RSTATUS_NOTEXIST_2_NOTEXIST:
            return RULE_TYPE_FAIL;

        case L_RSTATUS_NOTEXIST_2_NOTREADY:
            ret = RULE_OM_CreatePolicyMap(policy_map_index, FALSE);
            break;

        case L_RSTATUS_NOTEXIST_2_ACTIVE:
            ret = RULE_OM_CreatePolicyMap(policy_map_index, TRUE);
            break;

        case L_RSTATUS_NOTREADY_2_NOTEXIST:
            ret = RULE_OM_Delete_PolicyMap(policy_map_index);
            break;

        case L_RSTATUS_ACTIVE_2_NOTEXIST:
            /* let it to be noReady first then delete it */
            if (RULE_TYPE_OK != RULE_MGR_LocalSetPolicyMapNotReady(policy_map_index))
                return RULE_TYPE_FAIL;

            ret = RULE_OM_Delete_PolicyMap(policy_map_index);
            break;

        case L_RSTATUS_NOTREADY_2_NOTREADY: /* do nothing */
        case L_RSTATUS_ACTIVE_2_ACTIVE:
            ret = RULE_TYPE_OK;
            break;

        case L_RSTATUS_NOTREADY_2_ACTIVE:
            ret = RULE_OM_SetPolicyMapRowStatus(policy_map_index, TRUE);
            break;

        case L_RSTATUS_ACTIVE_2_NOTREADY:
            ret = RULE_MGR_LocalSetPolicyMapNotReady(policy_map_index);
            break;

        case L_RSTATUS_TRANSITION_STATE_ERROR:
        default:
            return RULE_TYPE_FAIL;
    }

    return ret;
}

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
UI32_T RULE_MGR_SetPolicyMapAttachCtrl(UI32_T policy_map_index, UI32_T policy_element_index, UI32_T attach_action)
{
    RULE_TYPE_RETURN_TYPE_T     result;

    RULE_TYPE_PolicyElement_T   policy_element;

    result = RULE_OM_GetPolicyMapElement(policy_element_index, &policy_element);
    if (RULE_TYPE_OK != result)
        return result;

    switch (attach_action)
    {
        case VAL_diffServPolicyMapAttachCtlAction_attaching:
            result = RULE_OM_BindElement2PolicyMap(policy_map_index, policy_element_index);
            if (RULE_TYPE_OK != result)
            {
                return result;
            }

            result = RULE_CTRL_AddClassMapConfig(policy_map_index, policy_element.class_map_index);
            if (RULE_TYPE_OK != result)
            {
                RULE_TYPE_RETURN_TYPE_T temp_result;

                temp_result = RULE_CTRL_RemoveClassMapConfig(policy_map_index, policy_element.class_map_index, TRUE);
                ASSERT(RULE_TYPE_OK == temp_result);

                temp_result = RULE_OM_UnbindElementFromPolicyMap(policy_map_index, policy_element_index);
                ASSERT(RULE_TYPE_OK == temp_result);

                return result;
            }

            break;

        case VAL_diffServPolicyMapAttachCtlAction_notAttaching:
            result = RULE_CTRL_RemoveClassMapConfig(policy_map_index, policy_element.class_map_index, TRUE);

            if (RULE_TYPE_OK != result)
            {
                RULE_TYPE_RETURN_TYPE_T temp_result;

                temp_result = RULE_CTRL_AddClassMapConfig(policy_map_index, policy_element.class_map_index);
                ASSERT(RULE_TYPE_OK == temp_result);

                return result;
            }

            result = RULE_OM_UnbindElementFromPolicyMap(policy_map_index, policy_element_index);
            if (RULE_TYPE_OK != result)
            {
                return result;
            }

            /* policy-map MAY become empty here but it is allowable */
            break;

        case VAL_diffServPolicyMapAttachCtlAction_notAction:
            return RULE_TYPE_OK; /* do nothing */

        default:
            return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_DelPolicyMapById
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete a policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Should free all binding resource
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_DelPolicyMapById(UI32_T policy_map_index)
{
    RULE_TYPE_PolicyMap_T   policy_map;

    if(RULE_TYPE_OK != RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map))
        return RULE_TYPE_FAIL;

    /*  2008-12-30, Jinfeng Chen:
        if policy map has been bound to interface, it can't be deleted.
     */
    if(0 != policy_map.port_count)
        return RULE_TYPE_FAIL;

#if 0
    if (RULE_TYPE_OK != RULE_MGR_LocalSetPolicyMapNotReady(policy_map_index))
        return RULE_TYPE_FAIL;
#endif

    return RULE_OM_Delete_PolicyMap(policy_map_index);
}

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
UI32_T RULE_MGR_GetPolicyMap(UI32_T policy_map_index, RULE_TYPE_PolicyMap_T *policy_map)
{
    return RULE_OM_GetPolicyMap(policy_map_index, policy_map);
}

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
UI32_T RULE_MGR_GetMibPolicyMap(UI32_T policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map)
{
    return RULE_OM_GetMibPolicyMap(policy_map_index, policy_map);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextPolicyMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next policy map entry by it's index
 * INPUT:    policy_map_index
 * OUTPUT:   policy_map.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Use *policy_map_index=0 to get first
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextPolicyMap(UI32_T *policy_map_index, RULE_TYPE_PolicyMap_T *policy_map)
{
    return RULE_OM_GetNextPolicyMap(policy_map_index, policy_map);
}

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
UI32_T RULE_MGR_GetNextMibPolicyMap(UI32_T *policy_map_index, RULE_TYPE_MIB_PolicyMap_T *policy_map)
{
    return RULE_OM_GetNextMibPolicyMap(policy_map_index, policy_map);
}

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
UI32_T RULE_MGR_GetNextUIPolicyMap(UI32_T *policy_map_index, RULE_TYPE_UI_PolicyMap_T *policy_map)
{
    return RULE_OM_GetNextUIPolicyMap(policy_map_index, policy_map);
}

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
UI32_T RULE_MGR_AddElement2PolicyMap(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_PolicyMap_T   policy_map;

    if(RULE_TYPE_OK != RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map))
        return RULE_TYPE_INVALID_PARAMETER;

    /* update om */
    result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
    if (RULE_TYPE_OK != result)
    {
        if (RULE_TYPE_EXISTED == result)
            return RULE_TYPE_OK;

        return RULE_TYPE_FAIL;
    }

    if (0 != policy_map.port_count)
    {
        result = RULE_CTRL_AddClassMapConfig(policy_map_index, class_map_index);
        if (RULE_TYPE_OK != result)
        {
            RULE_TYPE_RETURN_TYPE_T temp_result;

            temp_result = RULE_CTRL_RemoveClassMapConfig(policy_map_index, class_map_index, TRUE);
            ASSERT(RULE_TYPE_OK == temp_result);

            /* undo om setting */
            temp_result = RULE_OM_DeletePolicyMapElement(policy_map_index, class_map_index);
            ASSERT(RULE_TYPE_OK == temp_result);

            return result;
        }
    }

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_SetElementMeterFromPolicyMap(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_TBParamEntry_T *meter_entry)
{
    RULE_TYPE_RETURN_TYPE_T     result;

    RULE_TYPE_PolicyMap_T       policy_map;
    RULE_TYPE_PolicyElement_T   policy_element;
    RULE_TYPE_TBParamEntry_T    org_meter;
    BOOL_T meter_flag;

    ASSERT(NULL != meter_entry);

    if(RULE_TYPE_OK != RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map))
        return RULE_TYPE_INVALID_PARAMETER;

    if ( (meter_entry->meter_model == RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE)
		|| (meter_entry->meter_model == RULE_TYPE_METER_MODE_TRTCM_COLOR_BLIND) )
    {
        if ( (meter_entry->peak_rate < meter_entry->rate)
		|| (meter_entry->peak_burst_size < meter_entry->burst_size) )
        {
           return RULE_TYPE_FAIL;
        }
    }

    if ( (meter_entry->meter_model == RULE_TYPE_METER_MODE_SRTCM_COLOR_AWARE)
		|| (meter_entry->meter_model == RULE_TYPE_METER_MODE_SRTCM_COLOR_BLIND) )
    {
        if ( meter_entry->peak_burst_size < meter_entry->burst_size)
        {
            return RULE_TYPE_FAIL;
        }
    }

    result = RULE_OM_GetPolicyMapElementFromPolicyMapByClassMap(policy_map_index, class_map_index, &policy_element);
    if (RULE_TYPE_OK != result)
        return result;

    meter_flag = (RULE_TYPE_OK == RULE_OM_GetMeterEntryByIndex(policy_element.meter_index, &org_meter));

    /* create or update meter */
    result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, meter_entry);
    if (RULE_TYPE_OK != result)
    {
        if (RULE_TYPE_SAME_CFG == result)
            return RULE_TYPE_OK; /* meter is the same, nothing happen */

        return result;
    }

    if (0 != policy_map.port_count)
    {
        if (TRUE == meter_flag)
        {
            result = RULE_CTRL_UpdateMeterConfig(policy_map_index, class_map_index);
            if (RULE_TYPE_OK != result)
            {
                /* undo om setting */
                RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &org_meter);
                return result;
            }
        }
        else
        {
            result = RULE_CTRL_AddMeterConfig(policy_map_index, class_map_index);
            if (RULE_TYPE_OK != result)
            {
                /* undo om setting */
                RULE_OM_Remove_MeterEntry(policy_map_index, class_map_index);
                return result;
            }
        }
    }

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_SetElementActionFromPolicyMap(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_UI_Action_T *action_entry)
{
    RULE_TYPE_RETURN_TYPE_T     result;

    RULE_TYPE_PolicyElement_T   policy_element;
    RULE_TYPE_PolicyMap_T       policy_map;
    RULE_TYPE_Action_T          org_action;

    ASSERT(NULL != action_entry);

    if(RULE_TYPE_OK != RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map))
        return RULE_TYPE_INVALID_PARAMETER;

    result = RULE_OM_GetPolicyMapElementFromPolicyMapByClassMap(policy_map_index, class_map_index, &policy_element);
    if (RULE_TYPE_OK != result)
        return result;

    /* an active policy-map element MUST has an action */
    result = RULE_OM_GetActionEntryByIndex(policy_element.action_index, &org_action);
    if (RULE_TYPE_OK != result)
        return result;

    /* create or update action */
    result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, action_entry);
    if (RULE_TYPE_OK != result)
    {
        if (RULE_TYPE_SAME_CFG == result)
            return RULE_TYPE_OK; /* action is the same, nothing happen */

        return result;
    }

    if (0 != policy_map.port_count)
    {
        result = RULE_CTRL_UpdateActionConfig(policy_map_index, class_map_index);
        if (RULE_TYPE_OK != result)
        {
            /* undo om setting */
            RULE_OM_SetActionEntry(policy_element.action_index, &org_action);
            return result;
        }
    }

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_DelElementMeterFromPolicyMap(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_TBParamEntry_T *meter_entry)
{
    RULE_TYPE_RETURN_TYPE_T     result;

    RULE_TYPE_PolicyElement_T   policy_element;
    RULE_TYPE_TBParamEntry_T    org_meter_entry;

    ASSERT(NULL != meter_entry);

    if (NULL == meter_entry)
        return RULE_TYPE_INVALID_PARAMETER;

    if ((RULE_TYPE_OK != RULE_OM_GetPolicyMapElementFromPolicyMapByClassMap(policy_map_index, class_map_index, &policy_element)) ||
        (RULE_TYPE_OK != RULE_OM_GetMeterEntryByIndex(policy_element.meter_index, &org_meter_entry)))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    /* compare meter */
    if ((org_meter_entry.rate != meter_entry->rate) || (org_meter_entry.burst_size != meter_entry->burst_size))
        return RULE_TYPE_FAIL; /* meter_entry is different from OM */

    result = RULE_MGR_LocalNotify(RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_METER_DELETING, &class_map_index, &policy_map_index);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    result = RULE_OM_Remove_MeterEntry(policy_map_index, class_map_index);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    /* free hardware resource */
    result = RULE_CTRL_RemoveMeterConfig(policy_map_index, class_map_index);
    if (RULE_TYPE_OK != result)
    {
        RULE_TYPE_RETURN_TYPE_T temp_result;

        temp_result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, meter_entry);
        ASSERT(RULE_TYPE_OK == temp_result);

        temp_result = RULE_CTRL_AddMeterConfig(policy_map_index, class_map_index);
        ASSERT(RULE_TYPE_OK == temp_result);
        return result;
    }

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_DelElementActionFromPolicyMap(UI32_T policy_map_index, UI32_T class_map_index, RULE_TYPE_UI_Action_T *action_entry)
{
    RULE_TYPE_RETURN_TYPE_T     result;

    RULE_TYPE_PolicyElement_T   policy_element;
    RULE_TYPE_UI_Action_T       org_action;

    ASSERT(NULL != action_entry);

    if ((NULL == action_entry) ||
        (RULE_TYPE_OK != RULE_OM_GetPolicyMapElementFromPolicyMapByClassMap(policy_map_index, class_map_index, &policy_element)) ||
        (RULE_TYPE_OK != RULE_OM_GetUIActionEntry(policy_map_index, class_map_index, &org_action)))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (IN_ACTION_INVALID != action_entry->in_action_type)
    {
        if (org_action.in_action_value != action_entry->in_action_value)
            return RULE_TYPE_FAIL; /* not match with original setting */

        result = RULE_MGR_LocalNotify(RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_IN_ACTION_DELETING,
                                      &class_map_index,
                                      &policy_map_index);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }

        result = RULE_OM_SetInProfileActionToDefault(policy_element.action_index);
        if (RULE_TYPE_OK != result)
            return result;
    }

    if((VIOLATE_ACTION_INVALID != action_entry->violate_action_type) &&
       (CONFIRM_ACTION_INVALID != action_entry->confirm_action_type))
    {
        if((org_action.violate_action_value != action_entry->violate_action_value) &&
           (org_action.confirm_action_value != action_entry->confirm_action_value) &&
           (org_action.exceed_action_value != action_entry->exceed_action_value))
            return RULE_TYPE_FAIL; /* not match with original setting */

        result = RULE_MGR_LocalNotify(RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_OUT_OF_PROFILE_ACTION_DELETING,
                                      &class_map_index,
                                      &policy_map_index);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }

        /* when meter is deleted, out-of-profile action can be "NULL" */
        result = RULE_OM_SetOutOfProfileActionToDefault(policy_element.action_index);
        if (RULE_TYPE_OK != result)
            return result;
    }

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    if (CLASS_ACTION_INVALID != action_entry->class_action_type)
    {
        result = RULE_MGR_LocalNotify(RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_CLASS_ACTION_DELETING,
                                      &class_map_index,
                                      &policy_map_index);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }

        result = RULE_OM_SetClassActionToDefault(policy_element.action_index);
        if (RULE_TYPE_OK != result)
            return result;
    }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    result = RULE_CTRL_UpdateActionConfig(policy_map_index, class_map_index);
    if (RULE_TYPE_OK != result)
    {
        RULE_TYPE_RETURN_TYPE_T temp_result;

        /* undo om setting */
        temp_result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &org_action);
        ASSERT(RULE_TYPE_OK == temp_result);

        temp_result = RULE_CTRL_UpdateActionConfig(policy_map_index, class_map_index);
        ASSERT(RULE_TYPE_OK == temp_result);

        return result;
    }

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_DelElementFromPolicyMap(UI32_T policy_map_index, UI32_T class_map_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_PolicyMap_T   policy_map;

    result = RULE_MGR_LocalNotify(RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_CLASS_DELETING,
                                  &class_map_index, &policy_map_index);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    result = RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map);
    if(RULE_TYPE_OK != result)
        return result;

    if(0 != policy_map.port_count)
    {
        result = RULE_CTRL_RemoveClassMapConfig(policy_map_index, class_map_index, TRUE);
        if (RULE_TYPE_OK != result)
        {
            RULE_TYPE_RETURN_TYPE_T temp_result;

            temp_result = RULE_CTRL_AddClassMapConfig(policy_map_index, class_map_index);
            ASSERT(RULE_TYPE_OK == temp_result);

            return result;
        }
    }

    result = RULE_OM_DeletePolicyMapElement(policy_map_index, class_map_index);
    ASSERT(RULE_TYPE_OK == result);

    return result;
}

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
UI32_T RULE_MGR_GetNextElementFromPolicyMap(UI32_T policy_map_index, RULE_TYPE_UI_PolicyElement_T *element_entry)
{
    return RULE_OM_GetNextElementFromPolicyMap(policy_map_index, element_entry);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetPolicyMapElement
 *------------------------------------------------------------------------------
 * PURPOSE:  set policy-map element by index
 * INPUT:    policy_element_index, policy_element
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     none(mib only)
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetPolicyMapElement(UI32_T policy_element_index, const RULE_TYPE_PolicyElement_T *policy_element)
{
    RULE_TYPE_RETURN_TYPE_T     result;

    UI32_T                      policy_map_index;
    UI32_T                      debug_flag = RULE_OM_GetDebugFlag();
    RULE_TYPE_PolicyElement_T   org_element;
    BOOL_T                      config_chip_flag = FALSE;

    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapElement(policy_element_index, &org_element))
        return RULE_TYPE_FAIL;

    /* NOTE ...
     * NOT SUPPORT TO CHANGE THE POLICY MAP ELEMENT IF THE POLICY MAP ALWAYS BIND TO PORT.
     * SO THAT THE config_chip_flag WILL ALWAYS BE FALSE.
     * ALL CODE ABOUT THE POLICY MAP WHETHER BIND TO PORT HAVE CONSIDERED AS INGRESS ONLY.
     */
    result = RULE_TYPE_FAIL;
    config_chip_flag = FALSE;

    if (TRUE == RULE_MGR_IS_ENTRY_ACTIVE(org_element.row_status))
        return RULE_TYPE_FAIL;

    if (RULE_TYPE_OK == RULE_OM_GetPolicyMapIndexByElementIndex(policy_element_index, &policy_map_index))
    {
        RULE_TYPE_PolicyMap_T policy_map;

        if (RULE_TYPE_OK != RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map))
        {
            return RULE_TYPE_FAIL;
        }

        if (0 < policy_map.port_count)
        {
            config_chip_flag = TRUE;
        }
    }

    if (TRUE == config_chip_flag)
    {
        UI32_T  row_status;

        /* check all row_status MUST be active */

        /* check meter_index, remove meter if meter_index is 0 */
        if (0 != policy_element->meter_index)
        {
            if ((RULE_TYPE_OK != RULE_OM_GetMeterRowStatus(policy_element->meter_index, &row_status)) ||
                (FALSE == RULE_MGR_IS_ENTRY_ACTIVE(row_status)))
            {
                if (RULE_MGR_IS_DEBUG_ERROR_ON(debug_flag))
                    printf("\r\n[RULE_MGR_SetPolicyMapElement] check new meter_index failed.");

                return RULE_TYPE_FAIL;
            }
        }

        /* check action_index and class_map_index */
        if ((RULE_TYPE_OK != RULE_OM_GetClassMapRowStatus(policy_element->class_map_index, &row_status)) ||
            (FALSE == RULE_MGR_IS_ENTRY_ACTIVE(row_status)) ||
            (RULE_TYPE_OK != RULE_OM_GetActionRowStatus(policy_element->action_index, &row_status)) ||
            (FALSE == RULE_MGR_IS_ENTRY_ACTIVE(row_status)))
        {
            if (RULE_MGR_IS_DEBUG_ERROR_ON(debug_flag))
                printf("\r\n[RULE_MGR_SetPolicyMapElement] check new action_index or class_map_index failed.");

            return RULE_TYPE_FAIL;
        }

        /* if class_map_index has been modified */
        if (policy_element->class_map_index != org_element.class_map_index)
        {
            /* unbind original class-map */
            result = RULE_CTRL_RemoveClassMapConfig(policy_map_index, org_element.class_map_index, TRUE);
            if (RULE_TYPE_OK != result)
                return result;
        }
    }

    if (RULE_TYPE_OK != RULE_OM_SetPolicyMapElement(policy_element_index, policy_element))
    {
        /* restore chip config */
        if (TRUE == config_chip_flag)
            RULE_CTRL_AddClassMapConfig(policy_map_index, org_element.class_map_index);

        return RULE_TYPE_FAIL;
    }

    if (FALSE == config_chip_flag)
        return RULE_TYPE_OK; /* no need to config chip */

    /* if class_map_index has been modified
     * will install new rules to chip with new meter and action (if have)
     */
    if (policy_element->class_map_index != org_element.class_map_index)
    {
        result = RULE_CTRL_AddClassMapConfig(policy_map_index, policy_element->class_map_index);
        if (RULE_TYPE_OK != result)
        {
            /* OM has been updated to new, but rules on chip are all gone.
             */
            if (RULE_MGR_IS_DEBUG_ERROR_ON(debug_flag))
                printf("\r\n[RULE_MGR_SetPolicyMapElement] RULE_CTRL_AddClassMapConfig for new class-map failed.");

            return result;
        }

        return RULE_TYPE_OK;
    }


    /* start to update new settings of action or meter */
    config_chip_flag = TRUE;

    /* if action_index has been modified
     * will reinstall rules with new meter (if have)
     */
    if (policy_element->action_index != org_element.action_index)
    {
        result = RULE_CTRL_UpdateActionConfig(policy_map_index, policy_element->class_map_index);
        config_chip_flag = (RULE_TYPE_OK == result) ? TRUE : FALSE;

        if ((FALSE == config_chip_flag) &&
            (RULE_MGR_IS_DEBUG_ERROR_ON(debug_flag)))
            printf("\r\n[RULE_MGR_SetPolicyMapElement] RULE_CTRL_UpdateActionConfig for new action failed.");
    }

    /* if meter_index has been modified
     * will reinstall rules with new meter
     */
    if ((TRUE == config_chip_flag) &&
        (policy_element->meter_index != org_element.meter_index))
    {
        if (0 == org_element.meter_index)
        {
            /* no meter created before */
            result = RULE_CTRL_AddMeterConfig(policy_map_index, policy_element->class_map_index);
            config_chip_flag = (RULE_TYPE_OK == result) ? TRUE : FALSE;

            if ((FALSE == config_chip_flag) &&
                (RULE_MGR_IS_DEBUG_ERROR_ON(debug_flag)))
                printf("\r\n[RULE_MGR_SetPolicyMapElement] RULE_CTRL_AddMeterConfig for new meter failed.");
        }
        else
        {
            /* has meter already */
            if (0 == policy_element->meter_index)
            {
                /* remove meter */
                result = RULE_CTRL_RemoveMeterConfig(policy_map_index, policy_element->class_map_index);
                config_chip_flag = (RULE_TYPE_OK == result) ? TRUE : FALSE;

                if ((FALSE == config_chip_flag) &&
                    (RULE_MGR_IS_DEBUG_ERROR_ON(debug_flag)))
                    printf("\r\n[RULE_MGR_SetPolicyMapElement] RULE_CTRL_RemoveMeterConfig failed.");
            }
            else
            {
                /* change to new meter */
                result = RULE_CTRL_UpdateMeterConfig(policy_map_index, policy_element->class_map_index);
                config_chip_flag = (RULE_TYPE_OK == result) ? TRUE : FALSE;

                if ((FALSE == config_chip_flag) &&
                    (RULE_MGR_IS_DEBUG_ERROR_ON(debug_flag)))
                    printf("\r\n[RULE_MGR_SetPolicyMapElement] RULE_CTRL_UpdateMeterConfig for new meter failed.");
            }
        }
    }

    /* something wrong happened above */
    if (FALSE == config_chip_flag)
    {
        /* if config chip fail, undo om setting */
        RULE_OM_SetPolicyMapElement(policy_element_index, &org_element);
        return result;
    }

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_SetPolicyMapElementRowStatus(UI32_T policy_element_index, UI32_T row_status)
{
    UI32_T      ret, org_status;

    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapElementRowStatus(policy_element_index, &org_status))
        org_status = L_RSTATUS_NOT_EXIST;
    else if( org_status == VAL_diffServPolicyMapStatus_active && row_status == VAL_diffServPolicyMapStatus_notInService)
        return RULE_TYPE_FAIL;

    switch (L_RSTATUS_Fsm(row_status, &org_status, RULE_MGR_LocalPolicyMapElementRStatusCallBack, &policy_element_index))
    {
        case L_RSTATUS_NOTEXIST_2_NOTEXIST:
            return RULE_TYPE_FAIL;

        case L_RSTATUS_NOTEXIST_2_NOTREADY:
            ret = RULE_OM_CreatePolicyMapElement(policy_element_index, FALSE);
            break;

        case L_RSTATUS_NOTEXIST_2_ACTIVE:
            ret = RULE_OM_CreatePolicyMapElement(policy_element_index, TRUE);
            break;

        case L_RSTATUS_NOTREADY_2_NOTEXIST:
            ret = RULE_OM_DestroyPolicyMapElement(policy_element_index);
            break;

        case L_RSTATUS_ACTIVE_2_NOTEXIST:
            /* let it to be noReady first then delete it */
            if (RULE_TYPE_OK != RULE_MGR_LocalSetPolicyMapElementNotReady(policy_element_index))
                return RULE_TYPE_FAIL;

            ret = RULE_OM_DestroyPolicyMapElement(policy_element_index);
            break;

        case L_RSTATUS_NOTREADY_2_NOTREADY: /* do nothing */
        case L_RSTATUS_ACTIVE_2_ACTIVE:
            ret = RULE_TYPE_OK;
            break;

        case L_RSTATUS_NOTREADY_2_ACTIVE:
            ret = RULE_OM_SetPolicyMapElementRowStatus(policy_element_index, TRUE);
            break;

        case L_RSTATUS_ACTIVE_2_NOTREADY:
            ret = RULE_MGR_LocalSetPolicyMapElementNotReady(policy_element_index);
            break;

        case L_RSTATUS_TRANSITION_STATE_ERROR:
        default:
            return RULE_TYPE_FAIL;
    }

    return ret;
}

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
UI32_T RULE_MGR_GetPolicyMapElement(UI32_T policy_element_index, RULE_TYPE_PolicyElement_T *policy_element)
{
    return RULE_OM_GetPolicyMapElement(policy_element_index, policy_element);
}

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
UI32_T RULE_MGR_GetNextPolicyMapElement(UI32_T *policy_element_index, RULE_TYPE_PolicyElement_T *policy_element)
{
    return RULE_OM_GetNextPolicyMapElement(policy_element_index, policy_element);
}

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
UI32_T RULE_MGR_SetMeterEntry(UI32_T meter_index, const RULE_TYPE_TBParamEntry_T *meter_entry)
{
    RULE_TYPE_RETURN_TYPE_T     result;

    UI32_T                      policy_element_index, policy_map_index;
    RULE_TYPE_PolicyElement_T   policy_element;
    RULE_TYPE_TBParamEntry_T    org_meter;

    /* keep original meter for rollback */
    if (RULE_TYPE_OK != RULE_OM_GetMeterEntryByIndex(meter_index, &org_meter))
        return RULE_TYPE_FAIL;
    if (TRUE == RULE_MGR_IS_ENTRY_ACTIVE(org_meter.row_status))
        return RULE_TYPE_FAIL;
    /* no change */
    if (0 == memcmp (&org_meter, meter_entry, sizeof (RULE_TYPE_TBParamEntry_T)))
        return RULE_TYPE_OK;

    /* update om */
    if (RULE_TYPE_OK != RULE_OM_SetMeterEntry(meter_index, meter_entry))
        return RULE_TYPE_FAIL;

    /* if this meter doesn't belong to any one policy-map element, no need to config chip */
    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapElementIndexByMeter(meter_index, &policy_element_index))
        return RULE_TYPE_OK;

    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapElement(policy_element_index, &policy_element))
        return RULE_TYPE_FAIL; /* should not go here */

    /* if this element doesn't belong to any policy-map, no need to config chip */
    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapIndexByElementIndex(policy_element_index, &policy_map_index))
        return RULE_TYPE_OK;

    /* config chip */
    result = RULE_CTRL_UpdateMeterConfig(policy_map_index, policy_element.class_map_index);
    if (RULE_TYPE_OK != result)
    {
        /* undo om */
        RULE_OM_SetMeterEntry(meter_index, &org_meter);
        return result;
    }

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_SetMeterRowStatus(UI32_T meter_index, UI32_T row_status)
{
    UI32_T      ret, org_status;

    if (RULE_TYPE_OK != RULE_OM_GetMeterRowStatus(meter_index, &org_status))
        org_status = L_RSTATUS_NOT_EXIST;
    else if( org_status == VAL_diffServPolicyMapStatus_active && row_status == VAL_diffServPolicyMapStatus_notInService)
        return RULE_TYPE_FAIL;

    switch (L_RSTATUS_Fsm(row_status, &org_status, RULE_MGR_LocalMeterRStatusCallBack, &meter_index))
    {
        case L_RSTATUS_NOTEXIST_2_NOTEXIST:
            return RULE_TYPE_FAIL;

        case L_RSTATUS_NOTEXIST_2_NOTREADY:
            ret = RULE_OM_CreateMeter(meter_index, FALSE);
            break;

        case L_RSTATUS_NOTEXIST_2_ACTIVE:
            ret = RULE_OM_CreateMeter(meter_index, TRUE);
            break;

        case L_RSTATUS_NOTREADY_2_NOTEXIST:
            ret = RULE_OM_DestroyMeter(meter_index);
            break;

        case L_RSTATUS_ACTIVE_2_NOTEXIST:
            /* let it to be noReady first then delete it */
            if (RULE_TYPE_OK != RULE_MGR_LocalSetMeterNotReady(meter_index))
                return RULE_TYPE_FAIL;

            ret = RULE_OM_DestroyMeter(meter_index);
            break;

        case L_RSTATUS_NOTREADY_2_NOTREADY: /* do nothing */
        case L_RSTATUS_ACTIVE_2_ACTIVE:
            ret = RULE_TYPE_OK;
            break;

        case L_RSTATUS_NOTREADY_2_ACTIVE:
            ret = RULE_OM_SetMeterRowStatus(meter_index, TRUE);
            break;

        case L_RSTATUS_ACTIVE_2_NOTREADY:
            ret = RULE_MGR_LocalSetMeterNotReady(meter_index);
            break;

        case L_RSTATUS_TRANSITION_STATE_ERROR:
        default:
            return RULE_TYPE_FAIL;
    }

    return ret;
}

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
UI32_T RULE_MGR_GetMeterEntryByIndex(UI32_T meter_index, RULE_TYPE_TBParamEntry_T *meter_entry)
{
    return RULE_OM_GetMeterEntryByIndex(meter_index, meter_entry);
}

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
UI32_T RULE_MGR_GetNextMeterEntry(UI32_T *meter_index, RULE_TYPE_TBParamEntry_T *meter_entry)
{
    return RULE_OM_GetNextMeterEntry(meter_index, meter_entry);
}

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
UI32_T RULE_MGR_SetActionEntry(UI32_T action_index, const RULE_TYPE_Action_T *action_entry)
{
    RULE_TYPE_RETURN_TYPE_T     result;

    UI32_T                      policy_element_index, policy_map_index;
    RULE_TYPE_PolicyElement_T   policy_element;
    RULE_TYPE_Action_T          org_action;

    /* keep original action for rollback */
    if (RULE_TYPE_OK != RULE_OM_GetActionEntryByIndex(action_index, &org_action))
        return RULE_TYPE_FAIL;

    /* no change */
    if (0 == memcmp (&org_action, &action_entry, sizeof (RULE_TYPE_Action_T)))
        return RULE_TYPE_OK;

    /* update om */
    if (RULE_TYPE_OK != RULE_OM_SetActionEntry(action_index, action_entry))
        return RULE_TYPE_FAIL;

    /* if this action doesn't belong to any one policy-map element, no need to config chip */
    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapElementIndexByAction(action_index, &policy_element_index))
        return RULE_TYPE_OK;

    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapElement(policy_element_index, &policy_element))
        return RULE_TYPE_FAIL; /* should not go here */

    /* if this element doesn't belong to any policy-map, no need to config chip */
    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapIndexByElementIndex(policy_element_index, &policy_map_index))
        return RULE_TYPE_OK;

    /* config chip */
    result = RULE_CTRL_UpdateActionConfig(policy_map_index, policy_element.class_map_index);
    if (RULE_TYPE_OK != result)
    {
        /* undo om setting */
        RULE_OM_SetActionEntry(action_index, &org_action);
        return result;
    }

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_SetActionRowStatus(UI32_T action_index, UI32_T row_status)
{
    UI32_T      ret, org_status;

    if (RULE_TYPE_OK != RULE_OM_GetActionRowStatus(action_index, &org_status))
        org_status = L_RSTATUS_NOT_EXIST;
    else if( org_status == VAL_diffServPolicyMapStatus_active && row_status == VAL_diffServPolicyMapStatus_notInService)
        return RULE_TYPE_FAIL;

    switch (L_RSTATUS_Fsm(row_status, &org_status, RULE_MGR_LocalActionRStatusCallBack, &action_index))
    {
        case L_RSTATUS_NOTEXIST_2_NOTEXIST:
            return RULE_TYPE_FAIL;

        case L_RSTATUS_NOTEXIST_2_NOTREADY:
            ret = RULE_OM_CreateAction(action_index, FALSE);
            break;

        case L_RSTATUS_NOTEXIST_2_ACTIVE:
            ret = RULE_OM_CreateAction(action_index, TRUE);
            break;

        case L_RSTATUS_NOTREADY_2_NOTEXIST:
            ret = RULE_OM_DestroyAction(action_index);
            break;

        case L_RSTATUS_ACTIVE_2_NOTEXIST:
            /* let it to be noReady first then delete it */
            if (RULE_TYPE_OK != RULE_MGR_LocalSetActionNotReady(action_index))
                return RULE_TYPE_FAIL;

            ret = RULE_OM_DestroyAction(action_index);
            break;

        case L_RSTATUS_NOTREADY_2_NOTREADY: /* do nothing */
        case L_RSTATUS_ACTIVE_2_ACTIVE:
            ret = RULE_TYPE_OK;
            break;

        case L_RSTATUS_NOTREADY_2_ACTIVE:
            ret = RULE_OM_SetActionRowStatus(action_index, TRUE);
            break;

        case L_RSTATUS_ACTIVE_2_NOTREADY:
            ret = RULE_MGR_LocalSetActionNotReady(action_index);
            break;

        case L_RSTATUS_TRANSITION_STATE_ERROR:
        default:
            return RULE_TYPE_FAIL;
    }

    return ret;
}

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
UI32_T RULE_MGR_GetActionEntryByIndex(UI32_T action_index, RULE_TYPE_Action_T *action_entry)
{
    return RULE_OM_GetActionEntryByIndex(action_index, action_entry);
}

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
UI32_T RULE_MGR_GetMibActionEntryByIndex(UI32_T action_index, RULE_TYPE_MIB_Action_T *action_entry)
{
    return RULE_OM_GetMibActionEntryByIndex(action_index, action_entry);
}

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
UI32_T RULE_MGR_GetNextActionEntry(UI32_T *action_index, RULE_TYPE_Action_T *action_entry)
{
    return RULE_OM_GetNextActionEntry(action_index, action_entry);
}

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
UI32_T RULE_MGR_GetNextMibActionEntry(UI32_T *action_index, RULE_TYPE_MIB_Action_T *action_entry)
{
    return RULE_OM_GetNextMibActionEntry(action_index, action_entry);
}

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
UI32_T RULE_MGR_SetActionBitmap(UI32_T action_index, const UI8_T *action_bitmap)
{
    RULE_TYPE_Action_T          new_action;

    /* get new action */
    if (RULE_TYPE_OK != RULE_OM_DupActionEntryWithNewActionBitmap (action_index, action_bitmap, &new_action))
    {
        if (RULE_MGR_IS_DEBUG_ERROR_ON(RULE_OM_GetDebugFlag()))
            printf("\r\n[RULE_MGR_SetActionBitmap] RULE_OM_DupActionEntryWithNewActionBitmap failed.");

        return RULE_TYPE_FAIL;
    }
    return RULE_MGR_SetActionEntry(action_index, &new_action);
}

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
    RULE_TYPE_UI_BundleGroup_T *ui_bundle_group_p)
{
    UI32_T ret;
    UI32_T policy_map_index;

    ret = RULE_OM_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
    if (RULE_TYPE_OK != ret)
    {
        return ret;
    }

    if (FALSE == ui_bundle_group_p->valid_status)
    {
        RULE_CTRL_DeleteBundle(policy_map_index, ui_bundle_group_p->index);
    }

    return RULE_OM_SetUIBundleGroup(policy_map_index, ui_bundle_group_p);
}

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
    RULE_TYPE_UI_BundleGroup_T *ui_bundle_group_p)
{
    if (ui_bundle_group_p->index < 1 ||
        SYS_ADPT_DIFFSERV_MAX_BUNDLE_GROUP_OF_POLICY_MAP < ui_bundle_group_p->index)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    return RULE_OM_GetUIBundleGroup(policy_map_index, ui_bundle_group_p);
}

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
    RULE_TYPE_UI_BundleGroup_T *ui_bundle_group_p)
{
    RULE_TYPE_PolicyMap_T policy_map;

    if (*policy_map_index_p == 0)
    {
        if (RULE_TYPE_OK != RULE_OM_GetNextPolicyMap(policy_map_index_p, &policy_map))
        {
            return RULE_TYPE_POLICY_MAP_NONEXISTED;
        }

        ui_bundle_group_p->index = 1;
    }
    else if (ui_bundle_group_p->index == 0)
    {
        ui_bundle_group_p->index = 1;
    }
    else
    {
        ui_bundle_group_p->index++;
    }

    do
    {
        for (;ui_bundle_group_p->index <= SYS_ADPT_DIFFSERV_MAX_BUNDLE_GROUP_OF_POLICY_MAP; ++(ui_bundle_group_p->index))
        {
            if (RULE_TYPE_OK == RULE_OM_GetUIBundleGroup(*policy_map_index_p, ui_bundle_group_p))
            {
                return RULE_TYPE_OK;
            }
        }

        ui_bundle_group_p->index = 1;
    }
    while(RULE_TYPE_OK == RULE_OM_GetNextPolicyMap(policy_map_index_p, &policy_map));

    return RULE_TYPE_FAIL;
}

#endif /* #if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE) */

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
UI32_T RULE_MGR_GetClassMapNameById(UI32_T class_map_id, char *class_map_name)
{
    return RULE_OM_GetClassMapNameById(class_map_id, class_map_name);
}

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
UI32_T RULE_MGR_GetClassMapIdByName(const char *class_map_name, UI32_T *class_map_id)
{
    return RULE_OM_GetClassMapIndexByName(class_map_name, class_map_id);
}

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
UI32_T RULE_MGR_CreateClassMap(const char *class_map_name)
{
    UI32_T  class_map_index;

    return RULE_OM_Create_ClassMap(class_map_name,
                                   RULE_TYPE_CLASS_MAP_MATCH_ANY,
                                   &class_map_index);
}

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
UI32_T RULE_MGR_SetClassMap(UI32_T class_map_index, RULE_TYPE_ClassMap_T *class_map)
{
    return RULE_OM_SetClassMap(class_map_index, class_map);
}

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
UI32_T RULE_MGR_SetUIClassMap(UI32_T class_map_index, const RULE_TYPE_UI_ClassMap_T *class_map)
{
    return RULE_OM_SetUIClassMap(class_map_index, class_map);
}

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
UI32_T RULE_MGR_SetClassMapNameById(UI32_T class_map_index, const char *class_map_name)
{
    return RULE_OM_SetClassMapNameById(class_map_index, class_map_name);
}

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
UI32_T RULE_MGR_SetClassMapDescById(UI32_T class_map_index, const char *class_map_desc)
{
    return RULE_OM_SetClassMapDescById(class_map_index, class_map_desc);
}

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
UI32_T RULE_MGR_SetClassMapRowStatus(UI32_T class_map_index, UI32_T row_status)
{
    UI32_T      ret, org_status;

    if (RULE_TYPE_OK != RULE_OM_GetClassMapRowStatus(class_map_index, &org_status))
        org_status = L_RSTATUS_NOT_EXIST;
    else if( org_status == VAL_diffServPolicyMapStatus_active && row_status == VAL_diffServPolicyMapStatus_notInService)
        return RULE_TYPE_FAIL;

    switch (L_RSTATUS_Fsm(row_status, &org_status, RULE_MGR_LocalClassMapRStatusCallBack, &class_map_index))
    {
        case L_RSTATUS_NOTEXIST_2_NOTEXIST:
            return RULE_TYPE_FAIL;

        case L_RSTATUS_NOTEXIST_2_NOTREADY:
            ret = RULE_OM_CreateClassMap(class_map_index, FALSE);
            break;

        case L_RSTATUS_NOTEXIST_2_ACTIVE:
            ret = RULE_OM_CreateClassMap(class_map_index, TRUE);
            break;

        case L_RSTATUS_NOTREADY_2_NOTEXIST:
            ret = RULE_OM_DestroyClassMap(class_map_index);
            break;

        case L_RSTATUS_ACTIVE_2_NOTEXIST:
            /* let it to be noReady first then delete it */
            if (RULE_TYPE_OK != RULE_MGR_LocalSetClassMapNotReady(class_map_index))
                return RULE_TYPE_FAIL;

            ret = RULE_OM_DestroyClassMap(class_map_index);
            break;

        case L_RSTATUS_NOTREADY_2_NOTREADY: /* do nothing */
        case L_RSTATUS_ACTIVE_2_ACTIVE:
            ret = RULE_TYPE_OK;
            break;

        case L_RSTATUS_NOTREADY_2_ACTIVE:
            ret = RULE_OM_SetClassMapRowStatus(class_map_index, TRUE);
            break;

        case L_RSTATUS_ACTIVE_2_NOTREADY:
            ret = RULE_MGR_LocalSetClassMapNotReady(class_map_index);
            break;

        case L_RSTATUS_TRANSITION_STATE_ERROR:
        default:
            return RULE_TYPE_FAIL;
    }

    return ret;
}

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
UI32_T RULE_MGR_SetClassMapAttachCtrl(UI32_T class_map_index, UI32_T index_type, UI32_T class_index, UI32_T attach_action)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_ClassType_T   class_type;

    /* convert mib index type to MF or ACL */
    if (VAL_diffServClassMapAttachCtlElementIndexType_acl == index_type)
    {
        class_type = RULE_TYPE_CLASS_ACL;
    }
    else
    {
        UI32_T                  row_status; /* useless */
        RULE_TYPE_AclType_T     ace_type;   /* check with index_type */

        if (RULE_TYPE_OK != RULE_OM_GetAceTypeAndRowStatus(class_index, &ace_type, &row_status))
            return RULE_TYPE_FAIL;

        /* must make sure the type of ace match parameter: index_type */
        switch (index_type)
        {
            case VAL_diffServClassMapAttachCtlElementIndexType_macAce:
                if (RULE_TYPE_MAC_ACL != ace_type)
                    return RULE_TYPE_FAIL;
                break;
            case VAL_diffServClassMapAttachCtlElementIndexType_ipAce:
                if ((RULE_TYPE_IP_STD_ACL != ace_type) &&
                    (RULE_TYPE_IP_EXT_ACL != ace_type))
                {
                    return RULE_TYPE_FAIL;
                }
                break;
            case VAL_diffServClassMapAttachCtlElementIndexType_ipv6Ace:
                if ((RULE_TYPE_IPV6_STD_ACL != ace_type) &&
                    (RULE_TYPE_IPV6_EXT_ACL != ace_type))
                {
                    return RULE_TYPE_FAIL;
                }
                break;
            default:
                return RULE_TYPE_FAIL;
        }

        class_type = RULE_TYPE_CLASS_MF;
    }

    switch (attach_action)
    {
        case VAL_diffServClassMapAttachCtlAction_attaching:
            if (RULE_TYPE_OK != RULE_OM_BindElement2ClassMap(class_map_index, class_type, class_index))
                return RULE_TYPE_FAIL;

            result = RULE_CTRL_AddClassMapElementConfig(class_map_index, class_type, class_index);
            if (RULE_TYPE_OK != result)
            {
                /* undo om setting */
                RULE_OM_UnbindElementFromClassMap(class_map_index, class_type, class_index);
                return result;
            }
            break;

        case VAL_diffServClassMapAttachCtlAction_notAttaching:
            if (RULE_TYPE_OK != RULE_OM_UnbindElementFromClassMap(class_map_index, class_type, class_index))
                return RULE_TYPE_FAIL;

            result = RULE_CTRL_RemoveClassMapElementConfig(class_map_index, class_type, class_index);
            if (RULE_TYPE_OK != result)
                return result;
            break;

        case VAL_diffServClassMapAttachCtlAction_notAction:
            return RULE_TYPE_OK; /* do nothing */

        default:
            return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_DelClassMap(UI32_T class_map_index)
{
    UI8_T                   policy_map_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_POLICY_MAP_LIST];
    UI32_T                  policy_map_nbr = 0;

    if (RULE_TYPE_OK != RULE_MGR_LocalNotify(RULE_TYPE_NOTIFY_CLASS_MAP_DELETING, &class_map_index, NULL))
    {
        return RULE_TYPE_FAIL;
    }

    /*  2008-12-29, Jinfeng Chen:
        Unallow to modify class map if it is referenced by policy map.
     */
    memset(policy_map_list, 0, sizeof(policy_map_list));
    if (RULE_TYPE_OK == RULE_OM_Get_PolicyMapListByClassMap(class_map_index, policy_map_list, &policy_map_nbr))
        return RULE_TYPE_FAIL;

#if 0
    if (RULE_TYPE_OK != RULE_MGR_LocalSetClassMapNotReady(class_map_index))
        return RULE_TYPE_FAIL;
#endif

    /* delete specific class-map from class-map table */
    return RULE_OM_Delete_ClassMap(class_map_index);
}

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
UI32_T RULE_MGR_GetClassMapById(UI32_T class_map_index, RULE_TYPE_UI_ClassMap_T *class_map)
{
    return RULE_OM_GetClassMapById(class_map_index, class_map);
}

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
UI32_T RULE_MGR_GetClassMap(UI32_T class_map_index, RULE_TYPE_ClassMap_T *class_map)
{
    return RULE_OM_GetClassMap(class_map_index, class_map);
}

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
UI32_T RULE_MGR_GetMibClassMap(UI32_T class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map)
{
    return RULE_OM_GetMibClassMap(class_map_index, class_map);
}

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
UI32_T RULE_MGR_GetNextClassMap(UI32_T *class_map_index, RULE_TYPE_ClassMap_T *class_map)
{
    return RULE_OM_GetNextClassMap(class_map_index, class_map);
}

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
UI32_T RULE_MGR_GetNextMibClassMap(UI32_T *class_map_index, RULE_TYPE_MIB_ClassMap_T *class_map)
{
    return RULE_OM_GetNextMibClassMap(class_map_index, class_map);
}

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
UI32_T RULE_MGR_GetNextUIClassMap(UI32_T *class_map_index, RULE_TYPE_UI_ClassMap_T *class_map)
{
    return RULE_OM_GetNextUIClassMap(class_map_index, class_map);
}

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
UI32_T RULE_MGR_AddElement2ClassMap(UI32_T class_map_index, RULE_TYPE_UI_ClassMapElement_T *element_entry)
{
    UI32_T      result, class_index = 0;

    result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, element_entry, &class_index);
    if (RULE_TYPE_OK != result)
        return result;

    result = RULE_CTRL_AddClassMapElementConfig(class_map_index, element_entry->class_type, class_index);
    if (RULE_TYPE_OK != result)
    {
        /* undo om setting */
        RULE_OM_Remove_ClassMapElement_From_ClassMap(class_map_index, element_entry);
        return result;
    }

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_DelElementFromClassMap(UI32_T class_map_index, RULE_TYPE_UI_ClassMapElement_T *element_entry)
{
    UI32_T      class_index = 0;
    UI32_T      result;

    if (NULL == element_entry)
        return RULE_TYPE_FAIL;

    if (RULE_TYPE_OK != RULE_OM_GetClassMapElementClassIndex(class_map_index, element_entry, &class_index))
        return RULE_TYPE_FAIL;

    result = RULE_CTRL_RemoveClassMapElementConfig(class_map_index, element_entry->class_type, class_index);
    if (RULE_TYPE_OK != result)
        return result;

    result = RULE_OM_Remove_ClassMapElement_From_ClassMap(class_map_index, element_entry);
    if (RULE_TYPE_OK != result)
        return result;

    return RULE_TYPE_OK ;
}

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
UI32_T RULE_MGR_GetNextElementFromClassMap(UI32_T class_map_index, UI32_T *element_index, RULE_TYPE_UI_ClassMapElement_T *element_entry)
{
    if (NULL == element_index)
        return RULE_TYPE_FAIL;

    *element_index += 1;

    return RULE_OM_GetClassMapElement(class_map_index, *element_index, element_entry);
}

/* diffServAccessGroupTable
 */
static BOOL_T
RULE_MGR_LocalConvertAccessGroupDirection2InOutDirection(
    UI32_T                      mib_direction,
    RULE_TYPE_InOutDirection_T  *direction_p)
{
    if (mib_direction != VAL_diffServAccessGroupDirection_ingress &&
        mib_direction != VAL_diffServAccessGroupDirection_egress)
    {
        ASSERT(0);
        return FALSE;
    }

    *direction_p = (mib_direction == VAL_diffServAccessGroupDirection_ingress)
                   ? RULE_TYPE_INBOUND : RULE_TYPE_OUTBOUND;

    return TRUE;
}

static BOOL_T
RULE_MGR_LocalConvertAccessGroupTypeToAclType(
    UI32_T               mib_acl_type,
    RULE_TYPE_AclType_T  *acl_type_p)
{
    switch (mib_acl_type)
    {
        case VAL_diffServAccessGroupType_mac:
            *acl_type_p = RULE_TYPE_MAC_ACL;
            break;
        case VAL_diffServAccessGroupType_ip:
            *acl_type_p = RULE_TYPE_IP_ACL;
            break;
        case VAL_diffServAccessGroupType_ipv6:
            *acl_type_p = RULE_TYPE_IPV6_ACL;
            break;
        default:
            ASSERT(0);
            return FALSE;
    }

    return TRUE;
}

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
    UI32_T acl_index)
{
    RULE_TYPE_Acl_T            acl_entry;
    RULE_TYPE_AclMemento_T     bind_acl;
    RULE_TYPE_InOutDirection_T direction;
    RULE_TYPE_AclType_T        acl_type;

    if (FALSE == SWCTRL_LogicalPortExisting(ifindex) &&
        FALSE == RULE_TYPE_IS_CPU_PORT(ifindex) &&
        FALSE == RULE_TYPE_IS_ALL_PORTS(ifindex))
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_MGR_LocalConvertAccessGroupDirection2InOutDirection(mib_direction, &direction))
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_MGR_LocalConvertAccessGroupTypeToAclType(mib_acl_type, &acl_type))
    {
        return RULE_TYPE_FAIL;
    }

    if (RULE_TYPE_OK != RULE_OM_GetAclByIndex(acl_index, &acl_entry) ||
        FALSE == RULE_MGR_IS_ENTRY_ACTIVE(acl_entry.row_status))
    {
        return RULE_TYPE_FAIL;
    }

    if(RULE_TYPE_OK != RULE_OM_GetPortBindedAcl(ifindex, direction, acl_type, &bind_acl))
    {
        return RULE_TYPE_FAIL;
    }

    if (bind_acl.acl_index == acl_index)
    {
        return RULE_TYPE_OK;
    }

    if (RULE_MGR_IS_ENTRY_ACTIVE(bind_acl.row_status))
    {
        return RULE_TYPE_FAIL;
    }

    return RULE_OM_SetPortEntryAclIndex(ifindex, direction, acl_type, acl_index);
}

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
    char   *time_range_name)
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    RULE_TYPE_AclMemento_T     bind_acl;
    RULE_TYPE_InOutDirection_T direction;
    RULE_TYPE_AclType_T        acl_type;

    if (FALSE == SWCTRL_LogicalPortExisting(ifindex) &&
        FALSE == RULE_TYPE_IS_CPU_PORT(ifindex) &&
        FALSE == RULE_TYPE_IS_ALL_PORTS(ifindex))
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_MGR_LocalConvertAccessGroupDirection2InOutDirection(mib_direction, &direction))
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_MGR_LocalConvertAccessGroupTypeToAclType(mib_acl_type, &acl_type))
    {
        return RULE_TYPE_FAIL;
    }

    if (time_range_name[0] != '\0' &&
        TIME_RANGE_ERROR_TYPE_NONE != TIME_RANGE_PMGR_IsTimeRangeEntryValidByName((UI8_T *)time_range_name))
    {
        return RULE_TYPE_FAIL;
    }

    if(RULE_TYPE_OK != RULE_OM_GetPortBindedAcl(ifindex, direction, acl_type, &bind_acl))
    {
        return RULE_TYPE_FAIL;
    }

    if (strcmp((char *)bind_acl.time_range_name,time_range_name) == 0)
    {
        return RULE_TYPE_OK;
    }

    if (RULE_MGR_IS_ENTRY_ACTIVE(bind_acl.row_status))
    {
        return RULE_TYPE_FAIL;
    }

    return RULE_OM_SetPortEntryAclTimeRange(ifindex, direction, acl_type, (UI8_T *)time_range_name, RULE_TYPE_UNDEF_TIME_RANGE);
#else
    return RULE_TYPE_FAIL;
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */
}

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
    UI32_T counter_status)
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)
    RULE_TYPE_AclMemento_T     bind_acl;
    RULE_TYPE_InOutDirection_T direction;
    RULE_TYPE_AclType_T        acl_type;
    RULE_TYPE_COUNTER_ENABLE_T counter_enable;

    if (FALSE == SWCTRL_LogicalPortExisting(ifindex) &&
        FALSE == RULE_TYPE_IS_CPU_PORT(ifindex) &&
        FALSE == RULE_TYPE_IS_ALL_PORTS(ifindex))
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_MGR_LocalConvertAccessGroupDirection2InOutDirection(mib_direction, &direction))
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_MGR_LocalConvertAccessGroupTypeToAclType(mib_acl_type, &acl_type))
    {
        return RULE_TYPE_FAIL;
    }

    if (counter_status != VAL_diffServAccessGroupCounterStatus_enabled &&
        counter_status != VAL_diffServAccessGroupCounterStatus_disabled)
    {
        ASSERT(0);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    counter_enable = (counter_status == VAL_diffServAccessGroupCounterStatus_enabled)
                     ? RULE_TYPE_COUNTER_ENABLE : RULE_TYPE_COUNTER_DISABLE;

    if(RULE_TYPE_OK != RULE_OM_GetPortBindedAcl(ifindex, direction, acl_type, &bind_acl))
    {
        return RULE_TYPE_FAIL;
    }

    if (bind_acl.counter_enable == counter_enable)
    {
        return RULE_TYPE_OK;
    }

    if (RULE_MGR_IS_ENTRY_ACTIVE(bind_acl.row_status))
    {
        return RULE_TYPE_FAIL;
    }

    return RULE_OM_SetPortEntryAclCounterEnable(ifindex, direction, acl_type, counter_enable);
#else
    return RULE_TYPE_FAIL;
#endif /* (SYS_CPNT_ACL_COUNTER == TRUE) */
}

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
    UI32_T row_status)
{
    RULE_TYPE_AclMemento_T     bind_acl;
    RULE_TYPE_InOutDirection_T direction;
    RULE_TYPE_AclType_T        acl_type;
    UI32_T                     org_status;
    BOOL_T                     is_ingress;
    UI32_T                     ret;

    if (FALSE == SWCTRL_LogicalPortExisting(ifindex) &&
        FALSE == RULE_TYPE_IS_CPU_PORT(ifindex) &&
        FALSE == RULE_TYPE_IS_ALL_PORTS(ifindex))
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_MGR_LocalConvertAccessGroupDirection2InOutDirection(mib_direction, &direction))
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_MGR_LocalConvertAccessGroupTypeToAclType(mib_acl_type, &acl_type))
    {
        return RULE_TYPE_FAIL;
    }

    if(RULE_TYPE_OK != RULE_OM_GetPortBindedAcl(ifindex, direction, acl_type, &bind_acl))
    {
        org_status = L_RSTATUS_NOT_EXIST;
    }
    else
    {
        org_status = bind_acl.row_status;
    }

    is_ingress = (direction == RULE_TYPE_INBOUND) ? TRUE : FALSE;

    switch (L_RSTATUS_Fsm(row_status, &org_status, RULE_MGR_LocalAccessGroupRStatusCallBack, &bind_acl))
    {
        case L_RSTATUS_NOTEXIST_2_NOTEXIST:
            return RULE_TYPE_FAIL;

        case L_RSTATUS_NOTEXIST_2_NOTREADY:
            return  RULE_OM_SetPortEntryAclRowStatus(ifindex, direction, acl_type,
                                                     VAL_diffServAccessGroupStatus_notReady);
        case L_RSTATUS_NOTEXIST_2_ACTIVE:
            return RULE_TYPE_FAIL;

        case L_RSTATUS_NOTREADY_2_NOTEXIST:
            return  RULE_OM_SetPortEntryAclRowStatus(ifindex, direction, acl_type,
                                                     VAL_diffServAccessGroupStatus_destroy);

        case L_RSTATUS_ACTIVE_2_NOTEXIST:
            return RULE_MGR_UnBindPortFromAcl(ifindex,
                                              bind_acl.acl_index,
                                              acl_type,
                                              is_ingress);

        case L_RSTATUS_NOTREADY_2_NOTREADY: /* do nothing */
        case L_RSTATUS_ACTIVE_2_ACTIVE:
            return RULE_TYPE_OK;

        case L_RSTATUS_NOTREADY_2_ACTIVE:
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
            if (strlen((char *)bind_acl.time_range_name) != 0)
            {
                TIME_RANGE_PMGR_RequestTimeRangeEntry(bind_acl.time_range_name, TRUE, &bind_acl.time_range_index);
	         RULE_OM_SetPortEntryAclTimeRange(ifindex, direction, acl_type, bind_acl.time_range_name, bind_acl.time_range_index);
            }
#endif
            ret = RULE_MGR_LocalSetAccessGroupNotReady2Active(ifindex, direction, acl_type);
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
            if (ret != RULE_TYPE_OK)
            {
                TIME_RANGE_PMGR_RequestTimeRangeEntry(bind_acl.time_range_name, FALSE, &bind_acl.time_range_index);
            }
#endif
            return ret;

        case L_RSTATUS_ACTIVE_2_NOTREADY:
            ret = RULE_MGR_LocalSetAccessGroupActive2NotReady(ifindex, direction, acl_type);
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
            if ((ret == RULE_TYPE_OK) && (bind_acl.time_range_index != RULE_TYPE_UNDEF_TIME_RANGE))
            {
                TIME_RANGE_PMGR_RequestTimeRangeEntry(bind_acl.time_range_name, FALSE, &bind_acl.time_range_index);
            }
#endif
            return ret;

        case L_RSTATUS_TRANSITION_STATE_ERROR:
        default:
            ASSERT(0);
            return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_FAIL;
}

static RULE_TYPE_RETURN_TYPE_T
RULE_MGR_LocalConvertAclMem2MibAccessGroupEntry(
    const RULE_TYPE_AclMemento_T *bind_acl_p,
    RULE_TYPE_MIB_AccessGroupEntry_T *entry_p)
{
    entry_p->acl_index = bind_acl_p->acl_index;
    entry_p->counter_status = (bind_acl_p->counter_enable == RULE_TYPE_COUNTER_ENABLE)
                              ? VAL_diffServAccessGroupCounterStatus_enabled
                              : VAL_diffServAccessGroupCounterStatus_disabled;

    ASSERT(bind_acl_p->row_status == VAL_diffServAccessGroupStatus_notReady ||
           bind_acl_p->row_status == VAL_diffServAccessGroupStatus_active);
    entry_p->row_status = bind_acl_p->row_status;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    strncpy(entry_p->time_range_name, bind_acl_p->time_range_name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
    entry_p->time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH] = '\0';
#endif /* SYS_CPNT_TIME_BASED_ACL */

    return RULE_TYPE_OK;
}

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
    RULE_TYPE_MIB_AccessGroupEntry_T *entry_p)
{
    RULE_TYPE_InOutDirection_T direction;
    RULE_TYPE_AclType_T        acl_type;
    RULE_TYPE_RETURN_TYPE_T    result;
    RULE_TYPE_AclMemento_T     bind_acl;

    ASSERT(entry_p != NULL);

    if (FALSE == SWCTRL_LogicalPortExisting(entry_p->ifindex) &&
        FALSE == RULE_TYPE_IS_CPU_PORT(entry_p->ifindex) &&
        FALSE == RULE_TYPE_IS_ALL_PORTS(entry_p->ifindex))
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_MGR_LocalConvertAccessGroupDirection2InOutDirection(entry_p->direction, &direction))
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_MGR_LocalConvertAccessGroupTypeToAclType(entry_p->type, &acl_type))
    {
        return RULE_TYPE_FAIL;
    }

    result = RULE_OM_GetPortBindedAcl(entry_p->ifindex, direction, acl_type, &bind_acl);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    return RULE_MGR_LocalConvertAclMem2MibAccessGroupEntry(&bind_acl, entry_p);
}

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
    RULE_TYPE_MIB_AccessGroupEntry_T *entry_p)
{
    RULE_TYPE_RETURN_TYPE_T    result;
    UI32_T port_list_index, direction, type;

    ASSERT(entry_p != NULL);

    port_list_index = 0;
    direction = VAL_diffServAccessGroupDirection_ingress;
    type = VAL_diffServAccessGroupType_mac;

    /* Get next key
     */
    if (entry_p->ifindex != 0)
    {
        port_list_index = RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(entry_p->ifindex);

        if (entry_p->direction != RULE_TYPE_UNSPEC_DIRECTION)
        {
            direction =  entry_p->direction;

            if (entry_p->type != 0)
            {
                type = entry_p->type + 1;
            }
        }
    }

    for (; port_list_index < RULE_TYPE_TOTAL_NBR_OF_PORT; ++port_list_index)
    {
        for (; direction <= VAL_diffServAccessGroupDirection_egress; ++direction)
        {
            for (; type <= VAL_diffServAccessGroupType_ipv6; ++type)
            {
                entry_p->ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(port_list_index);
                entry_p->direction = direction;
                entry_p->type = type;

                result = RULE_MGR_GetAccessGroupEntry(entry_p);
                if (RULE_TYPE_OK == result)
                {
                    if (TRUE == SWCTRL_LogicalPortExisting(entry_p->ifindex) ||
                        TRUE == RULE_TYPE_IS_ALL_PORTS(entry_p->ifindex) ||
                        TRUE == RULE_TYPE_IS_CPU_PORT(entry_p->ifindex))
                    {
                        return RULE_TYPE_OK;
                    }
                }
            }

            type = VAL_diffServAccessGroupType_mac;
        }

        direction = VAL_diffServAccessGroupDirection_ingress;
    }

    return RULE_TYPE_FAIL;
}

/* diffservPolicyMapPortTable
 */
static BOOL_T
RULE_MGR_LocalConvertPolicyMapPortMibDirect2InOutDirect(
    UI32_T mib_direction,
    RULE_TYPE_InOutDirection_T *direction_p)
{
    if (mib_direction != VAL_diffServPolicyMapPortDirection_ingress &&
        mib_direction != VAL_diffServPolicyMapPortDirection_egress)
    {
        ASSERT(0);
        return FALSE;
    }

    *direction_p = (mib_direction == VAL_diffServPolicyMapPortDirection_ingress)
                   ? RULE_TYPE_INBOUND : RULE_TYPE_OUTBOUND;
    return TRUE;
}

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
   UI32_T policy_map_index)
{
    RULE_TYPE_PolicyMap_T   policy_map;
    RULE_TYPE_InOutDirection_T direction;
    UI32_T old_policy_map_index, row_status;

    if (FALSE == SWCTRL_LogicalPortExisting(ifindex) &&
        FALSE == RULE_TYPE_IS_CPU_PORT(ifindex) &&
        FALSE == RULE_TYPE_IS_ALL_PORTS(ifindex))
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_MGR_LocalConvertPolicyMapPortMibDirect2InOutDirect(mib_direction, &direction))
    {
        return RULE_TYPE_FAIL;
    }

    if(RULE_TYPE_OK != RULE_OM_GetPortBindedPolicyMap(ifindex, direction, &old_policy_map_index, &row_status))
    {
        return RULE_TYPE_FAIL;
    }

    if (old_policy_map_index == policy_map_index)
    {
        return RULE_TYPE_OK;
    }

    if(RULE_TYPE_OK != RULE_OM_GetPolicyMapByIndex(policy_map_index, &policy_map))
    {
        return RULE_TYPE_FAIL;
    }

    if (old_policy_map_index == policy_map_index)
    {
        return RULE_TYPE_OK;
    }

    if (RULE_MGR_IS_ENTRY_ACTIVE(row_status))
    {
        return RULE_TYPE_FAIL;
    }

    return RULE_OM_SetPortEntryPolicyMapIndex(ifindex, direction, policy_map_index);
}

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
   UI32_T row_status)
{
    RULE_TYPE_AclMemento_T     bind_acl;
    RULE_TYPE_InOutDirection_T direction;
    UI32_T                     org_status;
    UI32_T                     policy_map_index;

    if (FALSE == SWCTRL_LogicalPortExisting(ifindex) &&
        FALSE == RULE_TYPE_IS_CPU_PORT(ifindex) &&
        FALSE == RULE_TYPE_IS_ALL_PORTS(ifindex))
    {
        return RULE_TYPE_FAIL;
    }

    if (FALSE == RULE_MGR_LocalConvertPolicyMapPortMibDirect2InOutDirect(mib_direction, &direction))
    {
        return RULE_TYPE_FAIL;
    }

    if(RULE_TYPE_OK != RULE_OM_GetPortBindedPolicyMap(ifindex, direction, &policy_map_index, &org_status))
    {
        org_status = L_RSTATUS_NOT_EXIST;
    }

    switch (L_RSTATUS_Fsm(row_status, &org_status, RULE_MGR_LocalAccessGroupRStatusCallBack, &bind_acl))
    {
        case L_RSTATUS_NOTEXIST_2_NOTEXIST:
            return RULE_TYPE_FAIL;

        case L_RSTATUS_NOTEXIST_2_NOTREADY:
            return  RULE_OM_SetPortEntryPolicyMapRowStatus(ifindex, direction,
                                                           VAL_diffServPolicyMapPortPortStatus_notReady);

        case L_RSTATUS_NOTEXIST_2_ACTIVE:
            return RULE_TYPE_FAIL;

        case L_RSTATUS_NOTREADY_2_NOTEXIST:
            return  RULE_OM_SetPortEntryPolicyMapRowStatus(ifindex, direction,
                                                           VAL_diffServPolicyMapPortPortStatus_destroy);

        case L_RSTATUS_ACTIVE_2_NOTEXIST:
            return RULE_MGR_UnBindPortFromPolicyMap(ifindex, direction, policy_map_index);

        case L_RSTATUS_NOTREADY_2_NOTREADY: /* do nothing */
        case L_RSTATUS_ACTIVE_2_ACTIVE:
            return RULE_TYPE_OK;

        case L_RSTATUS_NOTREADY_2_ACTIVE:
            return RULE_MGR_LocalSetPolicyMapPortNotReady2Active(ifindex, direction);

        case L_RSTATUS_ACTIVE_2_NOTREADY:
            return RULE_MGR_LocalSetPolicyMapPortActive2NotReady(ifindex, direction);

        case L_RSTATUS_TRANSITION_STATE_ERROR:
        default:
            ASSERT(0);
            return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_FAIL;
}

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
    RULE_TYPE_MIB_PolicyMapPortEntry_T *entry_p)
{
    RULE_TYPE_InOutDirection_T direction;

    ASSERT(entry_p != NULL);

    if (FALSE == RULE_MGR_LocalConvertPolicyMapPortMibDirect2InOutDirect(entry_p->direction, &direction))
    {
        return RULE_TYPE_FAIL;
    }

    if (TRUE == RULE_TYPE_IS_ALL_PORTS(entry_p->ifindex))
    {
        return RULE_TYPE_FAIL;
    }

    return RULE_OM_GetPortBindedPolicyMap(entry_p->ifindex,
                                          direction,
                                          &entry_p->policy_map_index,
                                          &entry_p->row_status);
}

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
    RULE_TYPE_MIB_PolicyMapPortEntry_T *entry_p)
{
    RULE_TYPE_RETURN_TYPE_T    result;
    UI32_T port_list_index, ifindex, direction;

    ASSERT(entry_p != NULL);

    port_list_index = 0;
    direction = VAL_diffServPolicyMapPortDirection_ingress;

    if (entry_p->ifindex != 0)
    {
        port_list_index = RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(entry_p->ifindex);
        if (entry_p->direction != RULE_TYPE_UNSPEC_DIRECTION)
        {
            direction = entry_p->direction + 1;
        }
    }

    for (; port_list_index < RULE_TYPE_TOTAL_NBR_OF_PORT; ++port_list_index)
    {
        for (; direction <= VAL_diffServPolicyMapPortDirection_egress; ++direction)
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(port_list_index);
            entry_p->ifindex = ifindex;
            entry_p->direction = direction;

            result = RULE_MGR_GetPolicyMapPortEntry(entry_p);
            if (RULE_TYPE_OK == result)
            {
                return result;
            }
        }

        direction = VAL_diffServPolicyMapPortDirection_ingress;
    }

    return RULE_TYPE_FAIL;
}

/* ACL port table*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_BindPort2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Bind a port to ACL
 * INPUT:    uport_ifindex, acl_index, acl_type, ingress_flag
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     ingress_flag = FALSE implies egress
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_BindPort2Acl(
    UI32_T ifindex,
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type,
    BOOL_T ingress_flag,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable)
{
    RULE_TYPE_RETURN_TYPE_T     result;
    RULE_TYPE_Acl_T             acl_entry;
    RULE_TYPE_AclMemento_T      bind_acl;
    RULE_TYPE_InOutDirection_T  direction;
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
#endif

    LOG("ifindex = %lu, aclId = %lu, aclType = %s, ingress = %s, timeRangename = %s, counter = %s",
        ifindex,
        acl_index,
        RULE_MGR_ACL_TYPE_STR(acl_type),
        (ingress_flag==TRUE)?"In":"Out",
        (time_range_name==NULL)?"none":(char *)time_range_name,
        RULE_TYPE_COUNTER_ENABLE_STR(counter_enable));

    acl_type = RULE_MGR_CVRT_ACL_TYPE(acl_type);
    direction = RULE_TYPE_IS_INGRESS_TO_DIRECTION(ingress_flag);

    if (RULE_TYPE_OK != RULE_TYPE_VALIDATE_IFINDEX(ifindex))
    {
        LOG("<Error> Invalid parameter. ifindex(%lu)", ifindex);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_IS_UPORT(ifindex))
    {
        UI32_T unit, port, trunk;

        if (SWCTRL_LPORT_NORMAL_PORT != SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk))
        {
            LOG("<Error> Invalid parameter. ifindex(%lu)", ifindex);
            return RULE_TYPE_FAIL;
        }
    }

    /* check if acl is active */
    if ((RULE_TYPE_OK != RULE_OM_GetAclByIndex(acl_index, &acl_entry)) ||
        (FALSE == RULE_MGR_IS_ENTRY_ACTIVE(acl_entry.row_status)) ||
        (RULE_TYPE_OK != RULE_MGR_LocalCheckInputAclType(acl_entry.acl_type, acl_type)))
    {
        RULE_MGR_DEBUG_MSG("Inactive ACL or wrong ACL type: row_status(%lu), acl_type(%d)\n",
                           acl_entry.row_status,
                           acl_entry.acl_type);
        return RULE_TYPE_FAIL;
    }

    /*check time-range*/
    /*before RULE_CTRL related API, it should request time range first to get time-range index and protect the time-range index*/
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    if (time_range_name != NULL && strlen((char *)time_range_name) != 0)
    {
        if (TIME_RANGE_ERROR_TYPE_NONE != TIME_RANGE_PMGR_RequestTimeRangeEntry(time_range_name, TRUE, &time_range_index))
        {
            return RULE_TYPE_TIME_RANGE_NONEXISTED;
        }
    }
#endif

    /* Change row status to notReady
     */
    memset(&bind_acl, 0, sizeof(bind_acl));
    if(RULE_TYPE_OK == RULE_OM_GetPortBindedAcl(ifindex, direction, acl_type, &bind_acl))
    {
        if (TRUE == RULE_MGR_IS_ENTRY_ACTIVE(bind_acl.row_status))
        {
            if (bind_acl.acl_index == acl_index
                &&bind_acl.counter_enable == counter_enable
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                &&bind_acl.time_range_index, time_range_index == 0
#endif
             )
            {
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                /*bind the same ACL, no need to request two times*/
                if (time_range_index != RULE_TYPE_UNDEF_TIME_RANGE)
                {
                    TIME_RANGE_PMGR_RequestTimeRangeEntry(time_range_name, FALSE, &time_range_index);
                }
#endif
                return RULE_TYPE_OK;
            }

            result = RULE_MGR_LocalSetAccessGroupActive2NotReady(ifindex, direction, acl_type);
            if (result != RULE_TYPE_OK)
            {
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                /*Fail, unbind new time-range*/
                if (time_range_index != RULE_TYPE_UNDEF_TIME_RANGE)
                {
                    TIME_RANGE_PMGR_RequestTimeRangeEntry(time_range_name, FALSE, &time_range_index);
                }
#endif
                return result;
            }
        }
    }
    else
    {
        /* Destroy
         */
        result = RULE_OM_SetPortEntryAclRowStatus(ifindex, direction, acl_type,
                                                  VAL_diffServAccessGroupStatus_notReady);
        if (result != RULE_TYPE_OK)
        {
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
            /*Fail, unbind new time-range*/
            if (time_range_index != RULE_TYPE_UNDEF_TIME_RANGE)
            {
                TIME_RANGE_PMGR_RequestTimeRangeEntry(time_range_name, FALSE, &time_range_index);
            }
#endif
            return result;
        }
    }

    result = RULE_OM_SetPortEntryAclIndex(ifindex, direction, acl_type, acl_index);
    ASSERT(RULE_TYPE_OK== result);
    result = RULE_OM_SetPortEntryAclCounterEnable(ifindex, direction, acl_type, counter_enable);
    ASSERT(RULE_TYPE_OK== result);
#if (SYS_CPNT_TIME_RANGE == TRUE)
    if (time_range_index != RULE_TYPE_UNDEF_TIME_RANGE)
    {
        result = RULE_OM_SetPortEntryAclTimeRange(ifindex, direction, acl_type, time_range_name, time_range_index);
    }
    ASSERT(RULE_TYPE_OK== result);
#endif

    result = RULE_MGR_LocalSetAccessGroupNotReady2Active(ifindex, direction, acl_type);
    if (result != RULE_TYPE_OK)
    {
        LOG("Fail to bind port to ACL");

        RULE_OM_SetPortEntryAclIndex(ifindex, direction, acl_type, bind_acl.acl_index);
        RULE_OM_SetPortEntryAclCounterEnable(ifindex, direction, acl_type, bind_acl.counter_enable);
#if (SYS_CPNT_TIME_RANGE == TRUE)
        RULE_OM_SetPortEntryAclTimeRange(ifindex, direction, acl_type, bind_acl.time_range_name, bind_acl.time_range_index);
#endif

        if (RULE_MGR_IS_ENTRY_ACTIVE(bind_acl.row_status))
        {
            LOG("Rollback old binded ACL");

            RULE_MGR_LocalSetAccessGroupNotReady2Active(ifindex, direction, acl_type);
        }

        if (RULE_MGR_IS_ENTRY_DESTROY(bind_acl.row_status))
        {
            RULE_OM_SetPortEntryAclRowStatus(ifindex, direction, acl_type,
                                             VAL_diffServAccessGroupStatus_destroy);
        }

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        /*Fail, unbind new time-range*/
        if (time_range_index != RULE_TYPE_UNDEF_TIME_RANGE)
        {
            TIME_RANGE_PMGR_RequestTimeRangeEntry(time_range_name, FALSE, &time_range_index);
        }
#endif

        return result;
    }

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    /*unbind old time-range*/
    if (bind_acl.time_range_index != RULE_TYPE_UNDEF_TIME_RANGE)
    {
	 TIME_RANGE_PMGR_RequestTimeRangeEntry(bind_acl.time_range_name, FALSE, &bind_acl.time_range_index);
    }
#endif

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_UnBindPortFromAcl(UI32_T ifindex, UI32_T acl_index, RULE_TYPE_AclType_T acl_type, BOOL_T ingress_flag)
{
    RULE_TYPE_RETURN_TYPE_T     result;
    RULE_TYPE_Acl_T             acl_entry;
    UI32_T                      phy_acl_index;
    RULE_TYPE_InOutDirection_T  direction;
    RULE_TYPE_AclMemento_T      bind_acl;

    LOG("ifindex = %lu, aclId = %lu, aclType = %s, ingress = %s",
        ifindex,
        acl_index,
        RULE_MGR_ACL_TYPE_STR(acl_type),
        (ingress_flag==TRUE)?"In":"Out");

    acl_type = RULE_MGR_CVRT_ACL_TYPE(acl_type);
    direction = RULE_TYPE_IS_INGRESS_TO_DIRECTION(ingress_flag);

    if (RULE_TYPE_OK != RULE_TYPE_VALIDATE_IFINDEX(ifindex))
    {
        LOG("<Error> Invalid parameter. ifindex(%lu)", ifindex);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (RULE_TYPE_IS_UPORT(ifindex))
    {
        UI32_T unit, port, trunk;

        if (SWCTRL_LPORT_NORMAL_PORT != SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk))
        {
            LOG("<Error> Invalid parameter. ifindex(%lu)", ifindex);
            return RULE_TYPE_FAIL;
        }
    }

    if(RULE_TYPE_OK != RULE_OM_GetPortBindedAcl(ifindex, direction, acl_type, &bind_acl) ||
       TRUE != RULE_MGR_IS_ENTRY_ACTIVE(bind_acl.row_status))
    {
        LOG("Not bind yet");
        return RULE_TYPE_OK;
    }

    if (acl_index != bind_acl.acl_index)
    {
        LOG("ACL(%lu) is not same with binded ACL(%lu)", acl_index, bind_acl.acl_index)
        return RULE_TYPE_OK;
    }

    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    phy_acl_index = acl_index;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if (FALSE == acl_entry.flag_hide)
    {
        phy_acl_index = acl_entry.aggregate_acl_index;
        ASSERT(0 != phy_acl_index);
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    result = RULE_MGR_LocalSetAccessGroupActive2NotReady(ifindex, direction, acl_type);
    if (result != RULE_TYPE_OK)
    {
        LOG("Fail to unbind ACL(%lu) on ifindex(%lu)", acl_index, ifindex);
        return RULE_TYPE_FAIL;
    }

    result = RULE_OM_SetPortEntryAclRowStatus(ifindex, direction, acl_type,
                                              VAL_diffServAccessGroupStatus_destroy);
    ASSERT(RULE_TYPE_OK == result);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    if (bind_acl.time_range_index != TIME_RANGE_TYPE_UNDEF_TIME_RANGE)
    {
        TIME_RANGE_PMGR_RequestTimeRangeEntry(bind_acl.time_range_name, FALSE, &bind_acl.time_range_index);
    }
#endif

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_BindPort2DynamicAcl(UI32_T uport_ifindex, UI32_T acl_index, RULE_TYPE_AclType_T acl_type, BOOL_T ingress_flag)
{
    RULE_TYPE_Acl_T             acl_entry;
    RULE_TYPE_Acl_T             old_acl_entry;
    UI32_T                      unit, port, trunk;
    UI32_T                      old_acl_index = 0;
    SWCTRL_Lport_Type_T         uport_type;
    UI32_T                      rc;
    UI32_T                      old_precedence;
    RULE_TYPE_COUNTER_ENABLE_T  old_counter_enable;
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    UI8_T                        old_time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
#else
    UI8_T                        *old_time_range_name = NULL;
#endif
    RULE_TYPE_AclMemento_T      acl_mem;

    RULE_MGR_DEBUG_MSG("RULE_MGR_BindPort2DynamicAcl(%lu, %lu, %s, %s)\n",
                       uport_ifindex,
                       acl_index,
                       RULE_MGR_ACL_TYPE_STR(acl_type),
                       (ingress_flag==TRUE)?"In":"Out");

    uport_type = SWCTRL_LogicalPortToUserPort(uport_ifindex, &unit, &port, &trunk);

    if ( (SWCTRL_LPORT_UNKNOWN_PORT == uport_type) || (SWCTRL_LPORT_TRUNK_PORT_MEMBER == uport_type) )
    {
        RULE_MGR_DEBUG_MSG("Invalid port number: uport_ifindex(%lu), uport_type(%d)\n",
                           uport_ifindex,
                           uport_type);
        return RULE_TYPE_FAIL;
    }

    /* check if acl is active */
    if ((RULE_TYPE_OK != RULE_OM_GetAclByIndex(acl_index, &acl_entry)) ||
        (FALSE == RULE_MGR_IS_ENTRY_ACTIVE(acl_entry.row_status)) ||
        (RULE_TYPE_OK != RULE_MGR_LocalCheckInputAclType(acl_entry.acl_type, acl_type)))
    {
        RULE_MGR_DEBUG_MSG("Inactive ACL or wrong ACL type: row_status(%lu), acl_type(%d)\n",
                           acl_entry.row_status,
                           acl_entry.acl_type);
        return RULE_TYPE_FAIL;
    }

    /* If this port always bind user configured ACL,
     * then backup and unbind all.
     */
    if (!RULE_OM_IsBindDyanmicAclOnPort(uport_ifindex, ingress_flag?RULE_TYPE_INBOUND:RULE_TYPE_OUTBOUND))
    {
        RULE_MGR_DEBUG_MSG("No bind dynamic ACL on this port(%lu)\n",
                           uport_ifindex);

        old_acl_index   = 0;
        old_precedence  = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextAclByPort(uport_ifindex,
                                                        &old_acl_index,
                                                        &old_acl_entry,
                                                        &old_precedence,
                                                        old_time_range_name,
                                                        &old_counter_enable))
        {
            RULE_MGR_UnBindPortFromAcl(uport_ifindex,
                                       old_acl_index,
                                       RULE_MGR_CVRT_ACL_TYPE(old_acl_entry.acl_type),
                                       ingress_flag);

            memset(&acl_mem, 0, sizeof(acl_mem));
            acl_mem.acl_index        = old_acl_index;
            acl_mem.precedence       = old_precedence;
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
            strncpy((char *)acl_mem.time_range_name, (char *)old_time_range_name,
                        SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
#endif
            acl_mem.counter_enable   = old_counter_enable;
            acl_mem.row_status       = VAL_diffServAccessGroupStatus_active;
            RULE_OM_SetAclMementoOnPort(uport_ifindex,
                                        ingress_flag?RULE_TYPE_INBOUND:RULE_TYPE_OUTBOUND,
                                        RULE_MGR_CVRT_ACL_TYPE(old_acl_entry.acl_type),
                                        &acl_mem);
        }
    }

    /* clear dynamic flag for RULE_MGR_BindPort2Acl to bind ACL to DEV
     */
    RULE_OM_SetBindDynamicAclStatusOnPort(uport_ifindex, FALSE);
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    old_time_range_name[0] = '\0';
#endif
    rc = RULE_MGR_BindPort2Acl(uport_ifindex, acl_index, acl_type, ingress_flag,
                               old_time_range_name,
                               RULE_TYPE_COUNTER_DISABLE);
    RULE_OM_SetBindDynamicAclStatusOnPort(uport_ifindex, TRUE);

    RULE_MGR_DEBUG_MSG("RULE_MGR_BindPort2DynamicAcl return %s\n",
                       (rc == RULE_TYPE_OK)?"successed":"failed");

    return rc;
}

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
    BOOL_T ingress_flag)
{
    UI32_T                  unit, port, trunk;
    SWCTRL_Lport_Type_T     uport_type;
    UI32_T                  rc;
    BOOL_T                  set_to_user_cfg = TRUE;
    RULE_TYPE_AclMemento_T  acl_mem;

    RULE_MGR_DEBUG_MSG("RULE_MGR_UnBindPortFromDynamicAcl(%lu, %lu, %s, %s)\n",
                       uport_ifindex,
                       acl_index,
                       RULE_MGR_ACL_TYPE_STR(acl_type),
                       (ingress_flag==TRUE)?"In":"Out");

    uport_type = SWCTRL_LogicalPortToUserPort(uport_ifindex, &unit, &port, &trunk);

    if ( (SWCTRL_LPORT_UNKNOWN_PORT == uport_type) || (SWCTRL_LPORT_TRUNK_PORT_MEMBER == uport_type) )
    {
        RULE_MGR_DEBUG_MSG("Invalid port number: uport_ifindex(%lu), uport_type(%d)\n",
                           uport_ifindex,
                           uport_type);
        return RULE_TYPE_FAIL;
    }

    if (!RULE_OM_IsBindDyanmicAclOnPort(uport_ifindex, ingress_flag?RULE_TYPE_INBOUND:RULE_TYPE_OUTBOUND))
    {
        RULE_MGR_DEBUG_MSG("No any dynamic ACL on this port(%lu)\n",
                           uport_ifindex);
        return RULE_TYPE_OK;
    }

    /* clear dynamic flag for RULE_MGR_UnBindPortFromAcl to unbind ACL from DEV
     */
    RULE_OM_SetBindDynamicAclStatusOnPort(uport_ifindex, FALSE);
    RULE_MGR_UnBindPortFromAcl(uport_ifindex, acl_index, acl_type, ingress_flag);

    /* check whether have any (dynamic) ACL bind on this port
     */
    {
        UI32_T                      temp_index      = 0;
        RULE_TYPE_Acl_T             temp_acl_entry;
        UI32_T                      temp_precedence = 0;
        RULE_TYPE_COUNTER_ENABLE_T  temp_counter_enable;

        if (RULE_TYPE_OK == RULE_OM_GetNextAclByPort(uport_ifindex,
                                                     &temp_index,
                                                     &temp_acl_entry,
                                                     &temp_precedence,
                                                     NULL,
                                                     &temp_counter_enable))
        {
            set_to_user_cfg = FALSE;
        }
    }

    rc = RULE_TYPE_OK;
    if (set_to_user_cfg)
    {
        RULE_TYPE_AclType_T i;

        RULE_MGR_DEBUG_MSG("No any dynamic ACL on this port(%lu). Restore to user configured\n",
                           uport_ifindex);

        for (i=0; i<RULE_TYPE_MAX_NBR_OF_ACL_TYPE; ++i)
        {
            RULE_OM_GetAclMementoOnPort(uport_ifindex,
                                        ingress_flag?RULE_TYPE_INBOUND:RULE_TYPE_OUTBOUND,
                                        i,
                                        &acl_mem);

            if (acl_mem.acl_index != 0 &&
                TRUE == RULE_MGR_IS_ENTRY_ACTIVE(acl_mem.row_status))
            {
                RULE_TYPE_Acl_T     old_acl_entry;

                if (RULE_TYPE_OK == RULE_OM_GetAclByIndex(acl_mem.acl_index, &old_acl_entry))
                {
                    rc = RULE_MGR_BindPort2Acl(uport_ifindex,
                                               acl_mem.acl_index,
                                               RULE_MGR_CVRT_ACL_TYPE(old_acl_entry.acl_type),
                                               ingress_flag,
                                               acl_mem.time_range_name,
                                               acl_mem.counter_enable);
                }

                RULE_OM_SetAclMementoOnPort(uport_ifindex,
                                            ingress_flag?RULE_TYPE_INBOUND:RULE_TYPE_OUTBOUND,
                                            RULE_MGR_CVRT_ACL_TYPE(old_acl_entry.acl_type),
                                            NULL);
            }
        }

    }
    else
    {
        RULE_OM_SetBindDynamicAclStatusOnPort(uport_ifindex, TRUE);
    }

    RULE_MGR_DEBUG_MSG("RULE_MGR_UnBindPortFromDynamicAcl return %s\n",
                       (rc == RULE_TYPE_OK)?"successed":"failed");
    return rc;
}
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
UI32_T RULE_MGR_GetAclByTypeAndDirection(UI32_T uport_ifindex, RULE_TYPE_UI_AclEntry_T *acl_entry, BOOL_T ingress_flag)
{
    return RULE_OM_GetUIAclByTypeAndDirection(uport_ifindex, acl_entry, ingress_flag);
}

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
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable)
{
    return RULE_OM_GetNextUIAclByPort(uport_ifindex,
                                      inout_profile,
                                      acl_index,
                                      acl_entry,
                                      precedence,
                                      time_range_name,
                                      counter_enable);
}

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
    UI8_T *time_range_name)
{
    return RULE_OM_GetNextUIAclByPortAndBindingType(precedence, uport_ifindex, acl_entry, time_range_name);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextRunningAclByPort
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next running binding ACL by port
 * INPUT:    uport_ifindex, acl_index, precedence
 * OUTPUT:   uport_ifindex, acl_index, acl_entry, precedence, p_time_range_index.
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
RULE_MGR_GetNextRunningAclByPort(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T *acl_index,
    RULE_TYPE_UI_AclEntry_T *acl_entry,
    UI32_T *precedence,
    UI8_T *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable)
{
    if (RULE_TYPE_OK == RULE_OM_GetNextUIAclByPort(ifindex, inout_profile,
                                                   acl_index, acl_entry, precedence,
                                                   time_range_name,
                                                   counter_enable))
    {
        return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    }
}

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
UI32_T RULE_MGR_GetAclNameById(UI32_T acl_index, char *acl_name)
{
    return RULE_OM_GetAclNameById(acl_index, acl_name);
}

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
UI32_T RULE_MGR_GetAclIdByName(const char *acl_name, UI32_T *acl_index)
{
    return RULE_OM_GetAclIdByName(acl_name, acl_index);
}

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
UI32_T RULE_MGR_CreateAcl(const char *acl_name, RULE_TYPE_AclType_T acl_type)
{
    RULE_TYPE_RETURN_TYPE_T  ret;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    UI32_T  acl_index;
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE || SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT */

    ret = RULE_OM_CreateAcl(acl_name, acl_type);

    if (RULE_TYPE_OK != ret)
    {
        return ret;
    }

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    RULE_OM_GetAclIdByName(acl_name, &acl_index);
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE || SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT */

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)

    ret = RULE_OM_RebuildAggregateAcl(acl_index);
    if (RULE_TYPE_OK != ret)
    {
        RULE_OM_DelAcl(acl_index);
        return ret;
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    ret = RULE_CTRL_ReserveResourceOnCreateAcl(acl_index);
    if (RULE_TYPE_OK != ret)
    {
        RULE_OM_DelAcl(acl_index);
        return ret;
    }
#endif /* SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT */

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_SetAclTypeByIndex(UI32_T acl_index, UI32_T mib_acl_type)
{
    RULE_TYPE_AclType_T     core_layer_acl_type;
    RULE_TYPE_Acl_T         acl_entry;

    /* convert mib acl type to RULE_TYPE_AclType_T */
    switch (mib_acl_type)
    {
        case VAL_diffServAclType_mac:
            core_layer_acl_type = RULE_TYPE_MAC_ACL;
            break;
        case VAL_diffServAclType_ipstandard:
            core_layer_acl_type = RULE_TYPE_IP_STD_ACL;
            break;
        case VAL_diffServAclType_ipextended:
            core_layer_acl_type = RULE_TYPE_IP_EXT_ACL;
            break;
        case VAL_diffServAclType_ipv6standard:
            core_layer_acl_type = RULE_TYPE_IPV6_STD_ACL;
            break;
        case VAL_diffServAclType_ipv6extended:
            core_layer_acl_type = RULE_TYPE_IPV6_EXT_ACL;
            break;
#if (SYS_CPNT_DAI == TRUE)
        case VAL_diffServAclType_arp:
            core_layer_acl_type = RULE_TYPE_ARP_ACL;
            break;
#endif

        default:
            return RULE_TYPE_FAIL;
    }

    if (RULE_TYPE_OK != RULE_OM_GetAclByIndex(acl_index, &acl_entry))
        return RULE_TYPE_FAIL;

    if (core_layer_acl_type == acl_entry.acl_type)
        return RULE_TYPE_OK; /* no change do nothing */

    if (TRUE == RULE_MGR_IS_ENTRY_ACTIVE(acl_entry.row_status))
    {
        /* if an ACL is active, it may bind port or attach to class-map already.
         * hence use RULE_MGR_LocalSetAclNotReady() to unbind and unattach
         */
        if ((RULE_TYPE_OK != RULE_MGR_LocalSetAclNotReady(acl_index)) ||
            (RULE_TYPE_OK != RULE_OM_SetAclRowStatus(acl_index, TRUE)))
        {
            return RULE_TYPE_FAIL;
        }
    }

    /* change acl type (and remove all ace entries) */
    if (RULE_TYPE_OK != RULE_OM_SetAclTypeByIndex(acl_index, core_layer_acl_type))
        return RULE_TYPE_FAIL;

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_SetAclNameByIndex(UI32_T acl_index, const char *acl_name)
{
    return RULE_OM_SetAclNameByIndex(acl_index, acl_name);
}

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
UI32_T RULE_MGR_SetAclRowStatus(UI32_T acl_index, UI32_T row_status)
{
    UI32_T      ret, org_status;

    if (RULE_TYPE_OK != RULE_OM_GetAclRowStatus(acl_index, &org_status))
        org_status = L_RSTATUS_NOT_EXIST;

    switch (L_RSTATUS_Fsm(row_status, &org_status, RULE_MGR_LocalAclRStatusCallBack, &acl_index))
    {
        case L_RSTATUS_NOTEXIST_2_NOTEXIST:
            return RULE_TYPE_FAIL;

        case L_RSTATUS_NOTEXIST_2_NOTREADY:
            ret = RULE_OM_CreateAclByIndex(acl_index, FALSE);
            break;

        case L_RSTATUS_NOTEXIST_2_ACTIVE:
            ret = RULE_OM_CreateAclByIndex(acl_index, TRUE);
            break;

        case L_RSTATUS_NOTREADY_2_NOTEXIST:
            ret = RULE_OM_DestroyAcl(acl_index);
            break;

        case L_RSTATUS_ACTIVE_2_NOTEXIST:
            /* let it to be noReady first then delete it */
            if (RULE_TYPE_OK != RULE_MGR_LocalSetAclNotReady(acl_index))
                return RULE_TYPE_FAIL;

            ret = RULE_OM_DestroyAcl(acl_index);
            break;

        case L_RSTATUS_NOTREADY_2_NOTREADY: /* do nothing */
        case L_RSTATUS_ACTIVE_2_ACTIVE:
            ret = RULE_TYPE_OK;
            break;

        case L_RSTATUS_NOTREADY_2_ACTIVE:
            ret = RULE_OM_SetAclRowStatus(acl_index, TRUE);
            break;

        case L_RSTATUS_ACTIVE_2_NOTREADY:
            ret = RULE_MGR_LocalSetAclNotReady(acl_index);
            break;

        case L_RSTATUS_TRANSITION_STATE_ERROR:
        default:
            return RULE_TYPE_FAIL;
    }

    return ret;
}

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
UI32_T RULE_MGR_SetAclAttachCtrl(UI32_T mib_acl_index, UI32_T mib_ace_type, UI32_T mib_ace_index, UI32_T mib_attach_action)
{
    RULE_TYPE_AclType_T core_ace_type;

    switch (mib_ace_type)
    {
    case VAL_diffServAclAttachCtlAceType_macAce:
        core_ace_type = RULE_TYPE_MAC_ACL;
        break;

    case VAL_diffServAclAttachCtlAceType_ipAce:
        core_ace_type = RULE_TYPE_IP_ACL;
        break;

    case VAL_diffServAclAttachCtlAceType_ipv6Ace:
        core_ace_type = RULE_TYPE_IPV6_ACL;
        break;
#if (SYS_CPNT_DAI == TRUE)
    case VAL_diffServAclAttachCtlAceType_arpAce:
        core_ace_type = RULE_TYPE_ARP_ACL;
        break;
#endif
    default:
        return RULE_TYPE_FAIL;
    }

    switch (mib_attach_action)
    {
        case VAL_diffServAclAttachCtlAction_attaching:
            if (RULE_TYPE_OK != RULE_MGR_BindAce2Acl(mib_acl_index, mib_ace_index))
            {
                return RULE_TYPE_FAIL;
            }

            break;

        case VAL_diffServAclAttachCtlAction_notAttaching:
            if (RULE_TYPE_OK != RULE_MGR_UnbindAceFromAcl(mib_acl_index, core_ace_type, mib_ace_index))
            {
                return RULE_TYPE_FAIL;
            }
            break;

        /* since no action to do, return OK */
        case VAL_diffServAclAttachCtlAction_notAction:
            return RULE_TYPE_OK;

        default:
            return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_DelAcl(UI32_T acl_index)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_Acl_T acl_entry;
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    RULE_TYPE_AclType_T   acl_type;
    RULE_TYPE_Ace_Entry_T ace_entry;
    UI32_T ace_index;
#endif /* SYS_CPNT_TIME_BASED_ACL */

    if(RULE_TYPE_FAIL == RULE_OM_GetAclByIndex(acl_index, &acl_entry))
        return RULE_TYPE_FAIL;

    /*  2008-12-30, Jinfeng Chen:
        If ACL is referenced by class map, it can't be deleted.
     */
    if(acl_entry.class_map_count > 0)
        return RULE_TYPE_FAIL;

    result = RULE_MGR_LocalSetAclNotReady(acl_index);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    /* If user deletes ACL, we have to go through the ACEs belonging to this ACL,
     * and unbind these ACEs from time_range entries.
     */
    memset(&ace_entry, 0, sizeof(ace_entry));
    ace_index = 0;

    while (RULE_TYPE_OK == RULE_MGR_GetNextAceByAcl(acl_index, &acl_type, &ace_index, &ace_entry))
    {
        if (ace_entry.time_range_index != RULE_TYPE_UNDEF_TIME_RANGE)
            TIME_RANGE_PMGR_RequestTimeRangeEntry(ace_entry.time_range_name, FALSE, &ace_entry.time_range_index);
    }
#endif

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if (acl_entry.flag_aggregate_done == TRUE)
    {
        result = RULE_OM_DelAcl(acl_entry.aggregate_acl_index);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }

        acl_entry.flag_aggregate_done = FALSE;
        acl_entry.aggregate_acl_index = 0;
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    result = RULE_OM_DelAcl(acl_index);

    if (RULE_TYPE_OK != result)
    {
        return result;
    }

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    RULE_CTRL_ReleaseResourceOnDeleteAcl();
#endif /* SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT */

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_GetAclByIndex(UI32_T acl_index, RULE_TYPE_Acl_T *acl_entry)
{
    return RULE_OM_GetAclByIndex(acl_index, acl_entry);
}

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
UI32_T RULE_MGR_GetMibAclByIndex(UI32_T acl_index, RULE_TYPE_MIB_Acl_T *acl_entry)
{
    return RULE_OM_GetMibAclByIndex(acl_index, acl_entry);
}

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
UI32_T RULE_MGR_GetNextAclByIndex(UI32_T *acl_index, RULE_TYPE_Acl_T *acl_entry)
{
    return RULE_OM_GetNextAclByIndex(acl_index, acl_entry);
}

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
UI32_T RULE_MGR_GetNextMibAclByIndex(UI32_T *acl_index, RULE_TYPE_MIB_Acl_T *acl_entry)
{
    return RULE_OM_GetNextMibAclByIndex(acl_index, acl_entry);
}

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
UI32_T RULE_MGR_InitAce(RULE_TYPE_Ace_Entry_T *ace_entry)
{
    return RULE_OM_InitAce(ace_entry);
}

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
    RULE_TYPE_UI_Ace_Entry_T *ace_entry_p)
{
    return RULE_OM_InitUIAce(ace_entry_p);
}


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
    RULE_TYPE_AclType_T ace_type)
{
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
    return RULE_OM_SetAcePatternToDefaultByType(ace_entry_p, ace_type);
#else
    return RULE_TYPE_OK;
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
}

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
UI32_T RULE_MGR_GetNextAcl(UI32_T *acl_index, RULE_TYPE_UI_AclEntry_T *acl_entry)
{
    return RULE_OM_GetNextUIAcl(acl_index, acl_entry);
}

/* ACL ACE*/
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
    RULE_TYPE_UI_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_Ace_Entry_T temp_ace_entry;

    if (RULE_TYPE_OK != RULE_OM_ConvertUIAce2Ace(ace_entry, &temp_ace_entry))
    {
        RULE_MGR_DEBUG_MSG("Convert UI ACE to RULE_TYPE_Ace_Entry_T fail");
        return RULE_TYPE_FAIL;
    }

    return RULE_MGR_SetAce2Acl(acl_index, acl_type, &temp_ace_entry);
}

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
    RULE_TYPE_UI_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_Ace_Entry_T temp_ace_entry;

    if (RULE_TYPE_OK != RULE_OM_ConvertUIAce2Ace(ace_entry, &temp_ace_entry))
    {
        RULE_MGR_DEBUG_MSG("Convert UI ACE to RULE_TYPE_Ace_Entry_T fail");
        return RULE_TYPE_FAIL;
    }

    return RULE_MGR_DelAceFromAcl(acl_index, acl_type, &temp_ace_entry);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextUIAceByAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next UI ACE from ACL.
 * INPUT:    acl_index, ace_index
 * OUTPUT:   ace_p
 * RETURN:   Error code.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
UI32_T
RULE_MGR_GetNextUIAceByAcl(
    UI32_T acl_index,
    UI32_T *ace_index_p,
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    return RULE_OM_GetNextUIAceByAcl(acl_index, ace_index_p, ace_p);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetUIAceByIndex
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
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    return RULE_OM_GetUIAceByIndex(ace_index, ace_p);
}

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
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    return RULE_OM_GetNextUIAceByAceTypeAndIndex(ace_type, ace_index_p, ace_p);
}

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
    RULE_TYPE_UI_Ace_Entry_T *ace_p)
{
    UI32_T result;
    RULE_TYPE_Ace_Entry_T ace_entry;

    result = RULE_MGR_GetNextHardwareAceByAcl(uport_ifindex, inout_profile,
                 acl_index, acl_type, ace_index, &ace_entry);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    return RULE_OM_ConvertAce2UIAce(&ace_entry, ace_p);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAce2Acl
 *------------------------------------------------------------------------------
 * PURPOSE:  Set an ACE to ACL.
 * INPUT:    acl_index, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetAce2Acl(UI32_T acl_index, RULE_TYPE_AclType_T acl_type, RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_Acl_T         acl_entry;

    if(RULE_TYPE_FAIL == RULE_OM_GetAclByIndex(acl_index, &acl_entry))
        return RULE_TYPE_FAIL;

    /* add to om first */
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    if (strlen((char *)ace_entry->time_range_name) != 0)
    {
        if (TIME_RANGE_ERROR_TYPE_NONE != TIME_RANGE_PMGR_RequestTimeRangeEntry(ace_entry->time_range_name, TRUE, &(ace_entry->time_range_index)))
            return RULE_TYPE_TIME_RANGE_NONEXISTED;
    }
    else
    {
        ace_entry->time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
    }
#endif
    if (RULE_TYPE_OK != RULE_OM_AddAce2Acl(acl_index, acl_type, ace_entry))
    {
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        if (ace_entry->time_range_index != RULE_TYPE_UNDEF_TIME_RANGE)
        {
            TIME_RANGE_PMGR_RequestTimeRangeEntry(ace_entry->time_range_name, FALSE, &(ace_entry->time_range_index));
        }
#endif
        return RULE_TYPE_FAIL;
    }

#if (SYS_CPNT_DAI == TRUE)
    /* don't need to set the arp ace to the chip
     */
    if (RULE_TYPE_ARP_ACL == acl_type)
    {
        /*arp ace doesn't support time-range*/
        return RULE_TYPE_OK;
    }
#endif /* SYS_CPNT_DAI */

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if (RULE_TYPE_OK != RULE_MGR_LocalUpdateChipCompressAclConfig(acl_index))
    {
        /*recovery*/
        RULE_OM_DelAceFromAcl(acl_index, acl_type, ace_entry);
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        if (ace_entry->time_range_index != RULE_TYPE_UNDEF_TIME_RANGE)
        {
            TIME_RANGE_PMGR_RequestTimeRangeEntry(ace_entry->time_range_name, FALSE, &(ace_entry->time_range_index));
        }
#endif

        result = RULE_MGR_LocalUpdateChipCompressAclConfig(acl_index);

        if (RULE_TYPE_OK != result)
        {
            ASSERT(0);
            RULE_MGR_DEBUG_MSG("UNDO failed. aclIndex=%lu", acl_index);
        }
        return RULE_TYPE_FAIL;
    }
#else
    /* RULE_CTRL will check if we need to config the new ace on the chip or not (QoS/ACL),
       so it's not necessary to check here.
     */
    result = RULE_CTRL_SetAce(acl_index, ace_entry, TRUE);
    if (RULE_TYPE_OK != result)
    {
        /*recovery*/
        RULE_OM_DelAceFromAcl(acl_index, acl_type, ace_entry);
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        if (ace_entry->time_range_index != RULE_TYPE_UNDEF_TIME_RANGE)
        {
            TIME_RANGE_PMGR_RequestTimeRangeEntry(ace_entry->time_range_name, FALSE, &(ace_entry->time_range_index));
        }
#endif
        return result;
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_DelAceFromAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Delete an ACE from ACL.
 * INPUT:    acl_index, acl_type, ace_entry
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     Now the no CLI doesn't include time-range
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_DelAceFromAcl(UI32_T acl_index, RULE_TYPE_AclType_T acl_type, RULE_TYPE_Ace_Entry_T *ace_entry)
{
    /* RULE_CTRL will check if we need to config the new ace on the chip or not (QoS/ACL),
       so it's not necessary to check here.
     */

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_Acl_T acl_entry;
    UI32_T ace_index = 0;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE != TRUE)
    UI32_T i;
    UI32_T ifindex;
    UI8_T in_port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
    UI8_T out_port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
#endif /* #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE != TRUE) */

    if (RULE_TYPE_FAIL == RULE_OM_GetAclByIndex(acl_index, &acl_entry))
        return RULE_TYPE_FAIL;

    if (RULE_TYPE_OK != RULE_OM_GetAceIndexFromAclByAceEntry(acl_index, ace_entry, &ace_index))
        return RULE_TYPE_FAIL;

 #if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    /*before delete, update ace_entry for time_range_name*/
    RULE_OM_GetAceByIndex(ace_index, ace_entry);
 #endif

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if (RULE_TYPE_FAIL == RULE_OM_DelAceFromAcl(acl_index, acl_type, ace_entry))
        return RULE_TYPE_FAIL;

    if (RULE_TYPE_OK != RULE_MGR_LocalUpdateChipCompressAclConfig(acl_index))
    {
        RULE_OM_AddAce2Acl(acl_index, acl_type, ace_entry);

        result = RULE_MGR_LocalUpdateChipCompressAclConfig(acl_index);

        if (RULE_TYPE_OK != result)
        {
            ASSERT(0);
            RULE_MGR_DEBUG_MSG("Undo failed. aclIndex=%lu", acl_index);
        }

        return RULE_TYPE_FAIL;
    }
#else
    memcpy(in_port_list, acl_entry.filtering_in_port_list, sizeof(in_port_list));
    memcpy(out_port_list, acl_entry.filtering_out_port_list, sizeof(out_port_list));

    for (i = 0; i < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ i)
    {
        if (RULE_MGR_IS_BIT_ON(in_port_list, i))
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(i);

            result = RULE_CTRL_SetAcl(ifindex, RULE_TYPE_INBOUND, acl_index, FALSE, RULE_TYPE_UNDEF_TIME_RANGE, RULE_TYPE_COUNTER_DISABLE);
            if(RULE_TYPE_OK != result)
                return result;
        }
    }

    for (i = 0; i < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ i)
    {
        if (RULE_MGR_IS_BIT_ON(out_port_list, i))
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(i);

            result = RULE_CTRL_SetAcl(ifindex, RULE_TYPE_OUTBOUND, acl_index, FALSE, RULE_TYPE_UNDEF_TIME_RANGE, RULE_TYPE_COUNTER_DISABLE);
            if(RULE_TYPE_OK != result)
                return result;
        }
    }

#if 0
    result = RULE_CTRL_SetAce(acl_index, ace_entry, FALSE);
    if (RULE_TYPE_OK != result)
    {
    	return result;
    	/* 2008-06-26, Jinfeng.Chen:
    		below is previous comment, but why ?
    	 */
        /* go ahead to delete om ace */
    }
#endif

    if(RULE_TYPE_FAIL == RULE_OM_DelAceFromAcl(acl_index, acl_type, ace_entry))
        return RULE_TYPE_FAIL;

    for (i = 0; i < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ i)
    {
        if (RULE_MGR_IS_BIT_ON(in_port_list, i))
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(i);

            //
            // FIXME: When reset ACL the time-range will miss
            //
            result = RULE_CTRL_SetAcl(ifindex, RULE_TYPE_INBOUND, acl_index, TRUE, RULE_TYPE_UNDEF_TIME_RANGE, RULE_TYPE_COUNTER_DISABLE);
            if(RULE_TYPE_OK == result)
                return result;
        }
    }

    for (i = 0; i < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ i)
    {
        if (RULE_MGR_IS_BIT_ON(out_port_list, i))
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(i);

            //
            // FIXME: When reset ACL the time-range will miss
            //
            result = RULE_CTRL_SetAcl(ifindex, RULE_TYPE_OUTBOUND, acl_index, TRUE, RULE_TYPE_UNDEF_TIME_RANGE, RULE_TYPE_COUNTER_DISABLE);
            if(RULE_TYPE_OK != result)
                return result;
        }
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

/* unbind time-range in the end to protect the time-range index not be modified
  * time-range index is used in rule-ctrl */
#if (SYS_CPNT_TIME_BASED_ACL== TRUE)
    if (ace_entry->time_range_index != RULE_TYPE_UNDEF_TIME_RANGE)
    {
        TIME_RANGE_PMGR_RequestTimeRangeEntry(ace_entry->time_range_name,
                                              FALSE, &ace_entry->time_range_index);
    }
#endif

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_BindAce2Acl(UI32_T acl_index, UI32_T ace_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_Acl_T         acl_entry;
    RULE_TYPE_Ace_Entry_T   ace_entry;

    if(RULE_TYPE_FAIL == RULE_OM_GetAclByIndex(acl_index, &acl_entry))
        return RULE_TYPE_FAIL;

    if(RULE_TYPE_FAIL == RULE_OM_GetAceByIndex(ace_index, &ace_entry))
        return RULE_TYPE_FAIL;

    /* add to om first */
    if (RULE_TYPE_OK != RULE_OM_BindElement2Acl(acl_index, acl_entry.acl_type, ace_index))
        return RULE_TYPE_FAIL;
#if (SYS_CPNT_DAI == TRUE)
    /* don't need to set the arp ace to the chip
     */
    if (RULE_TYPE_ARP_ACL == acl_entry.acl_type)
    {
        return RULE_TYPE_OK;
    }
#endif /* SYS_CPNT_DAI */

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if (RULE_TYPE_OK != RULE_MGR_LocalUpdateChipCompressAclConfig(acl_index))
    {
        RULE_OM_UnbindElementFromAcl(acl_index, acl_entry.acl_type, ace_index);

        result = RULE_MGR_LocalUpdateChipCompressAclConfig(acl_index);

        if (RULE_TYPE_OK != result)
        {
            ASSERT(0);
            RULE_MGR_DEBUG_MSG("UNDO failed. aclIndex=%lu", acl_index);
        }

        return RULE_TYPE_FAIL;
    }
#else
    /* RULE_CTRL will check if we need to config the new ace on the chip or not (QoS/ACL),
       so it's not necessary to check here.
     */
    result = RULE_CTRL_SetAce(acl_index, &ace_entry, TRUE);
    if (RULE_TYPE_OK != result)
    {
        RULE_OM_UnbindElementFromAcl(acl_index, acl_entry.acl_type, ace_index);
        return result;
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_UnbindAceFromAcl(UI32_T acl_index, RULE_TYPE_AclType_T acl_type, UI32_T ace_index)
{
    /* RULE_CTRL will check if we need to config the new ace on the chip or not (QoS/ACL),
       so it's not necessary to check here.
     */
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_Acl_T acl_entry;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE != TRUE)
    UI32_T i;
    UI32_T ifindex;
    UI8_T in_port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
    UI8_T out_port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
#endif /* #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE != TRUE) */

    if (RULE_TYPE_FAIL == RULE_OM_GetAclByIndex(acl_index, &acl_entry))
        return RULE_TYPE_FAIL;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if (RULE_TYPE_FAIL == RULE_OM_UnbindElementFromAcl(acl_index, acl_type, ace_index))
        return RULE_TYPE_FAIL;

    if (RULE_TYPE_OK != RULE_MGR_LocalUpdateChipCompressAclConfig(acl_index))
    {
        RULE_OM_BindElement2Acl(acl_index, acl_type, ace_index);

        result = RULE_MGR_LocalUpdateChipCompressAclConfig(acl_index);

        if (RULE_TYPE_OK != result)
        {
            ASSERT(0);
            RULE_MGR_DEBUG_MSG("Undo failed. aclIndex=%lu", acl_index);
        }

        return RULE_TYPE_FAIL;
    }
#else
    memcpy(in_port_list, acl_entry.filtering_in_port_list, sizeof(in_port_list));
    memcpy(out_port_list, acl_entry.filtering_out_port_list, sizeof(out_port_list));

    for (i = 0; i < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ i)
    {
        if (RULE_MGR_IS_BIT_ON(in_port_list, i))
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(i);

            result = RULE_CTRL_SetAcl(ifindex, RULE_TYPE_INBOUND, acl_index, FALSE, RULE_TYPE_UNDEF_TIME_RANGE, RULE_TYPE_COUNTER_DISABLE);
            if (RULE_TYPE_OK != result)
                return result;
        }
    }

    for (i = 0; i < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ i)
    {
        if (RULE_MGR_IS_BIT_ON(out_port_list, i))
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(i);

            result = RULE_CTRL_SetAcl(ifindex, RULE_TYPE_OUTBOUND, acl_index, FALSE, RULE_TYPE_UNDEF_TIME_RANGE, RULE_TYPE_COUNTER_DISABLE);
            if(RULE_TYPE_OK != result)
                return result;
        }
    }

    result = RULE_OM_UnbindElementFromAcl(acl_index, acl_type, ace_index);
    if(RULE_TYPE_OK != result)
        return result;

    for (i = 0; i < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ i)
    {
        if (RULE_MGR_IS_BIT_ON(in_port_list, i))
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(i);

            //
            // FIXME: When reset ACL the time-range will miss
            //
            result = RULE_CTRL_SetAcl(ifindex, RULE_TYPE_INBOUND, acl_index, TRUE, RULE_TYPE_UNDEF_TIME_RANGE, RULE_TYPE_COUNTER_DISABLE);
            if(RULE_TYPE_OK != result)
                return result;
        }
    }

    for (i = 0; i < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ i)
    {
        if (RULE_MGR_IS_BIT_ON(out_port_list, i))
        {
            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(i);

            //
            // FIXME: When reset ACL the time-range will miss
            //
            result = RULE_CTRL_SetAcl(ifindex, RULE_TYPE_OUTBOUND, acl_index, TRUE, RULE_TYPE_UNDEF_TIME_RANGE, RULE_TYPE_COUNTER_DISABLE);
            if(RULE_TYPE_OK != result)
                return result;
        }
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_GetNextAceByAcl(UI32_T acl_index, RULE_TYPE_AclType_T *acl_type, UI32_T *ace_index, RULE_TYPE_Ace_Entry_T *ace_entry)
{
    return RULE_OM_GetNextAceByAcl(acl_index, acl_type, ace_index, ace_entry);
}

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
    RULE_TYPE_Ace_Entry_T *ace_entry_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    ASSERT(NULL != ace_entry_p);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    {
        RULE_TYPE_Acl_T acl_entry;

        result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }

        if (TRUE == acl_entry.flag_aggregate_done)
        {
            acl_index = acl_entry.aggregate_acl_index;
        }
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    result = RULE_OM_GetAceByIndex(ace_index, ace_entry_p);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

#if (SYS_CPNT_ACL_MIRROR == TRUE)
    if (RULE_TYPE_INBOUND == inout_profile)
    {
        RULE_TYPE_AceAction_T ace_access;
        RULE_TYPE_Acl_T acl_entry;

        if (RULE_TYPE_OK == RULE_OM_GetAclByIndex(acl_index, &acl_entry) &&
            RULE_TYPE_MIRROR_ENABLE == acl_entry.mirror.enable)
        {
            ace_access = ace_entry_p->access;
            if (RULE_TYPE_ACE_PERMIT == ace_access)
            {
                ace_entry_p->access = RULE_TYPE_ACE_MIRROR_TO_ANALYZER_PORT;
            }
        }
    }
#endif /* SYS_CPNT_ACL_MIRROR */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    RULE_CTRL_GetAclRuleCounter(ifindex, inout_profile, acl_index, ace_index,
                                                                     ace_entry_p);
#endif /* SYS_CPNT_ACL_COUNTER */

    return RULE_TYPE_OK;
}

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
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T inout_profile,
    UI32_T acl_index,
    RULE_TYPE_AclType_T *acl_type,
    UI32_T *ace_index,
    RULE_TYPE_Ace_Entry_T *ace_entry)
{
    RULE_TYPE_RETURN_TYPE_T result;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    {
        RULE_TYPE_Acl_T acl_entry;

        result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
        if (RULE_TYPE_OK != result)
        {
            return result;
        }

        if (TRUE == acl_entry.flag_aggregate_done)
        {
            acl_index = acl_entry.aggregate_acl_index;
        }
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    result = RULE_OM_GetNextAceByAcl(acl_index, acl_type, ace_index, ace_entry);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

#if (SYS_CPNT_ACL_MIRROR == TRUE)
    if (RULE_TYPE_INBOUND == inout_profile)
    {
        RULE_TYPE_AceAction_T ace_access;
        RULE_TYPE_Acl_T acl_entry;

        if (RULE_TYPE_OK == RULE_OM_GetAclByIndex(acl_index, &acl_entry) &&
            RULE_TYPE_MIRROR_ENABLE == acl_entry.mirror.enable)
        {
            ace_access = ace_entry->access;
            if (RULE_TYPE_ACE_PERMIT == ace_access)
            {
                ace_entry->access = RULE_TYPE_ACE_MIRROR_TO_ANALYZER_PORT;
            }
        }
    }
#endif /* SYS_CPNT_ACL_MIRROR */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    RULE_CTRL_GetAclRuleCounter(ifindex, inout_profile, acl_index, *ace_index,
                                                                     ace_entry);
#endif /* SYS_CPNT_ACL_COUNTER */

    return RULE_TYPE_OK;
}

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
    RULE_TYPE_COUNTER_ENABLE_T *counter_enable_p)
{
   return  RULE_OM_GetNextMibAceByPort(
               ifindex,
               inout_profile,
               acl_index_p,
               ace_index_p,
               ace_entry_p,
               counter_enable_p);
}

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
    RULE_TYPE_CounterRec_T *counter)
{
    UI32_T ret;
    RULE_TYPE_PolicyElement_T entry;
    RULE_TYPE_Action_T action_entry;
    UI32_T receive_packets = 0;
    UI32_T drop_packets = 0;

    ret = RULE_OM_GetPolicyMapElementEntry(policy_id, class_id, &entry);
    if (ret != RULE_TYPE_OK)
    {
        return ret;
    }

    ret = RULE_OM_GetActionEntryByIndex(entry.action_index, &action_entry);
    if (ret != RULE_TYPE_OK)
    {
        return ret;
    }

    ret = RULE_CTRL_GetQosCounter(ifindex, direction, policy_id, class_id, counter);
    if (ret != RULE_TYPE_OK)
    {
        return ret;
    }

    if (entry.meter_index == 0)
    {
        /* not bind meter, all are received packets */
        receive_packets = counter->green_octets + counter->yellow_octets + counter->red_octets;
    }
    else
    {
        receive_packets = counter->green_packets;
        if (action_entry.action_bitmap & RULE_TYPE_ACTION_YELLOW_DROP)
        {
            drop_packets += counter->yellow_packets;
        }
        else
        {
            receive_packets += counter->yellow_packets;
        }

        if (action_entry.action_bitmap & RULE_TYPE_ACTION_RED_DROP)
        {
            drop_packets += counter->red_packets;
        }
        else
        {
            receive_packets += counter->red_packets;
        }
    }

    memset(counter, 0, sizeof(RULE_TYPE_CounterRec_T));
    counter->receive_packets = receive_packets;
    counter->drop_packets = drop_packets;

    return RULE_TYPE_OK;
}

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
    UI32_T class_id)
{
    return RULE_CTRL_ClearQosCounter(ifindex, direction, policy_id, class_id);
}
#endif /*#if (SYS_CPNT_QOS_V2_COUNTER == TRUE)*/

#if (SYS_CPNT_ACL_COUNTER == TRUE)

typedef struct
{
    UI32_T acl_index;
    UI32_T ace_index;
} RULE_MGR_CLEAR_COUNTER_T;

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_LocalClearAclCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Clear ACL counter
 * INPUT:    interface  - The interface.
 *                        The type of this parameter should be
 *                        RULE_TYPE_INTERFACE_UPORT
 *           cookie     - Points to a RULE_MGR_CLEAR_COUNTER_T object.
 * OUTPUT:   None
 * RETURN:   Error code.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_MGR_LocalClearAclCounter(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    void *cookie)
{
    RULE_MGR_CLEAR_COUNTER_T *cc = (RULE_MGR_CLEAR_COUNTER_T *) cookie;
    RULE_TYPE_RETURN_TYPE_T result;

    ASSERT(NULL != cc);

    result = RULE_CTRL_ClearAclRuleCounter(ifindex,
                                           direction,
                                           cc->acl_index, cc->ace_index);

    if (RULE_TYPE_OK != result)
    {
        LOG("<Error> Failed to clear counters. ifindex=%lu, aclId=%lu, aceId=%lu",
            ifindex, cc->acl_index, cc->ace_index);
    }

    return result;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_LocalEachPort
 *------------------------------------------------------------------------------
 * PURPOSE:  For each normal port do fn
 * INPUT:    interface  - The interface.
 *                        The type of this parameter can be one of the following
 *                        values,
 *                        RULE_TYPE_INTERFACE_LPORT     - One port
 *                        RULE_TYPE_INTERFACE_UPORT     - One port
 *                        RULE_TYPE_INTERFACE_ALL_UNIT  - One global port
 *                        RULE_TYPE_INTERFACE_UNSPEC    - All port (all lport,
 *                                                        global port)
 *           fn         - The callback function.
 *           cookie     - User data.
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_MGR_LocalEachPort(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    RULE_TYPE_RETURN_TYPE_T (*fn)(UI32_T ifindex, RULE_TYPE_InOutDirection_T direction, void *),
    void *cookie)
{
    UI32_T i;

    ASSERT(NULL != interface_p);

    if (RULE_TYPE_INTERFACE_MAX < interface_p->type)
    {
        LOG("Invalid parameter: interface.type=%d", interface_p->type);
        return RULE_TYPE_INVALID_PARAMETER;
    }

    for (i = 0; i < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ i)
    {
        UI32_T ifindex;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(i);

        if (RULE_TYPE_INTERFACE_UPORT == interface_p->type)
        {
            if (TRUE != RULE_TYPE_IS_UPORT(ifindex))
            {
                continue;
            }

            if (ifindex != STKTPLG_OM_UPORT_TO_IFINDEX(interface_p->uport.unit,
                                                       interface_p->uport.port))
            {
                continue;
            }

            {
                UI32_T unit, port, trunk;
                SWCTRL_Lport_Type_T lport_type;

                lport_type = SWCTRL_LogicalPortToUserPort(ifindex, &unit, &port, &trunk);
                if (SWCTRL_LPORT_NORMAL_PORT != lport_type)
                {
                    continue;
                }
            }
        }
        else if (RULE_TYPE_INTERFACE_TRUNK == interface_p->type)
        {
            if (TRUE != RULE_TYPE_IS_TRUNK(ifindex))
            {
                continue;
            }

            if (ifindex != STKTPLG_OM_TRUNKID_TO_IFINDEX(interface_p->trunk_id))
            {
                continue;
            }
        }
        else if (RULE_TYPE_INTERFACE_ALL_UNIT == interface_p->type)
        {
            if (TRUE != RULE_TYPE_IS_ALL_PORTS(ifindex))
            {
                continue;
            }
        }
        else if (RULE_TYPE_INTERFACE_CPU_PORT == interface_p->type)
        {
            if (TRUE != RULE_TYPE_IS_CPU_PORT(ifindex))
            {
                continue;
            }
        }

        if (RULE_TYPE_BOTH_DIRECTION == interface_p->direction ||
            RULE_TYPE_UNSPEC_DIRECTION == interface_p->direction)
        {
            fn(ifindex, RULE_TYPE_INBOUND, cookie);

            fn(ifindex, RULE_TYPE_OUTBOUND, cookie);
        }
        else
        {
            fn(ifindex, interface_p->direction, cookie);
        }
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_LocalGetPhyAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get physical ACL information
 * INPUT:    acl_index     - ACL ID
 * OUTPUT:   phy_acl_index - Physical ACL ID written on chip
 *           phy_acl_entry - Points to an ACL object.
 * RETURN:   Error code.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_MGR_LocalGetPhyAcl(
    UI32_T acl_index,
    UI32_T *phy_acl_index)
{
    RULE_TYPE_RETURN_TYPE_T ret;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    RULE_TYPE_Acl_T acl_entry;

    ret = RULE_OM_GetAclByIndex(acl_index, &acl_entry);

    if (RULE_TYPE_OK != ret)
    {
        return ret;
    }

    if (TRUE == acl_entry.flag_hide)
    {
        return RULE_TYPE_FAIL;
    }

    *phy_acl_index = acl_entry.aggregate_acl_index;
#else
    ASSERT(NULL != phy_acl_index);

    *phy_acl_index = acl_index;
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    return RULE_TYPE_OK;
}

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
 *                        The type of this parameter can be one of the following
 *                        values,
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
    const RULE_TYPE_INTERFACE_INFO_T *interface)
{
#define IS_MATCHED(param_idx, om_idx)    (0xffffffff == param_idx || om_idx == param_idx)

    UI32_T w_acl_index;
    UI32_T w_ace_index;

    RULE_TYPE_RETURN_TYPE_T     ret = RULE_TYPE_OK;

    RULE_TYPE_AclType_T         w_ace_type;

    RULE_TYPE_Acl_T             acl_entry;
    RULE_TYPE_Ace_Entry_T       w_ace_entry;

    RULE_MGR_CLEAR_COUNTER_T cc = {0};

    ASSERT(NULL != interface);

    if (0xffffffff == acl_index)
    {
        ace_index = 0xffffffff;
    }

    w_acl_index = 0;
    while (RULE_TYPE_OK == RULE_OM_GetNextAclByIndex(&w_acl_index, &acl_entry))
    {
        UI32_T phy_acl_index;

        if (!IS_MATCHED(acl_index, w_acl_index))
        {
            continue;
        }

        if (RULE_TYPE_OK != RULE_MGR_LocalGetPhyAcl(w_acl_index, &phy_acl_index))
        {
            continue;
        }

        for (w_ace_index = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(phy_acl_index, &w_ace_type, &w_ace_index, &w_ace_entry); )
        {
            if (!IS_MATCHED(ace_index, w_ace_index))
            {
                continue;
            }

            cc.acl_index = phy_acl_index;
            cc.ace_index = w_ace_index;

            ret = RULE_MGR_LocalEachPort(interface, RULE_MGR_LocalClearAclCounter, &cc);

            if (0xffffffff != ace_index)
            {
                break;
            }
        }

        if (0xffffffff != acl_index)
        {
            break;
        }
    }

    return RULE_TYPE_OK;

#undef IS_MATCHED
}
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
UI32_T RULE_MGR_SetAceTypeByIndex(UI32_T ace_index, RULE_TYPE_AclType_T ace_type)
{
    RULE_TYPE_Ace_Entry_T   org_ace;

    if (RULE_TYPE_OK != RULE_OM_GetAceByIndex(ace_index, &org_ace))
    {
        return RULE_TYPE_FAIL;
    }

    /* change ace_type via SNMP, squid */

    if (org_ace.ace_type == ace_type)
        return RULE_TYPE_OK;

    switch (ace_type)
    {
    case RULE_TYPE_IP_STD_ACL:
    case RULE_TYPE_IP_EXT_ACL:

        if (FALSE == RULE_MGR_IS_IP_ACL(org_ace.ace_type))
            return RULE_TYPE_FAIL;
        break;

    case RULE_TYPE_IPV6_STD_ACL:
    case RULE_TYPE_IPV6_EXT_ACL:

        if (FALSE == RULE_MGR_IS_IPV6_ACL(org_ace.ace_type))
            return RULE_TYPE_FAIL;
        break;

    default:
        return RULE_TYPE_FAIL;
    }

    if (RULE_TYPE_FAIL == RULE_MGR_LocalSetAceNotReady (ace_index))
        return RULE_TYPE_FAIL;

    if (RULE_TYPE_FAIL == RULE_OM_ResetAceEntryByNewAceType (ace_index, ace_type))
        return RULE_TYPE_FAIL;

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_RequestTimeRangeReference
 *------------------------------------------------------------------------------
 * PURPOSE:  Request for adding/deleting time range entry reference.
 * INPUT:    time_range_name - Time range name
 *           add_reference   - TRUE:  Adding time range entry reference
 *                             FALSE: Deleting the time range entry reference.
 * OUTPUT:   time_range_index_p
 * RETURN:   TRUE / FALSE
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static BOOL_T
RULE_MGR_RequestTimeRangeReference(
    BOOL_T add_reference,
    UI8_T *time_range_name,
    UI32_T *time_range_index_p)
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    if (time_range_name[0] != '\0')
    {
        if (TIME_RANGE_ERROR_TYPE_NONE != TIME_RANGE_PMGR_RequestTimeRangeEntry(time_range_name,
                                                                                add_reference,
                                                                                time_range_index_p))
        {
            return FALSE;
        }
    }
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */
    return TRUE;
}

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
UI32_T RULE_MGR_SetAceFieldByIndex(UI32_T ace_index, RULE_TYPE_Ace_Entry_T *ace_entry, UI32_T field_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T                  acl_index, class_map_index;
    RULE_TYPE_Ace_Entry_T   org_ace;
    BOOL_T is_modified_time_range;

    is_modified_time_range = ((RULE_MGR_IS_MAC_ACL(ace_entry->ace_type) && field_index == LEAF_diffServMacAceTimeRange) ||
                              (RULE_MGR_IS_IP_ACL(ace_entry->ace_type) && field_index == LEAF_diffServIpAceTimeRange)   ||
                              (RULE_MGR_IS_IPV6_ACL(ace_entry->ace_type) && field_index == LEAF_diffServIpv6AceTimeRange));

    if ((NULL == ace_entry) ||
        (RULE_TYPE_OK != RULE_OM_GetAceByIndex(ace_index, &org_ace)))
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (ace_entry->ace_type != org_ace.ace_type)
        return RULE_TYPE_FAIL; /* do not allow to change ace type */

    if (RULE_TYPE_OK == RULE_OM_GetAclIndexByAce(ace_index, &acl_index)) /* this ace is own by ACL */
    {
        result = RULE_CTRL_SetAce(acl_index, &org_ace, FALSE); /* delete original rule */
        if (RULE_TYPE_OK != result)
            return result;

        if (is_modified_time_range &&
            TRUE != RULE_MGR_RequestTimeRangeReference(TRUE,
                                                       ace_entry->time_range_name,
                                                       &(ace_entry->time_range_index)))
                {
                    return RULE_TYPE_FAIL;
                }

        result = RULE_OM_SetAceFieldByIndex(ace_index, ace_entry, field_index, RULE_TYPE_ACE_OWNED_BY_ACL);
        if (RULE_TYPE_OK != result)
        {
            /* restore chip config */
            RULE_CTRL_SetAce(acl_index, &org_ace, TRUE);

            if (is_modified_time_range)
                {
                RULE_MGR_RequestTimeRangeReference(FALSE, ace_entry->time_range_name, &(ace_entry->time_range_index));
            }

            return result;
        }

        result = RULE_CTRL_SetAce(acl_index, (RULE_TYPE_Ace_Entry_T *)ace_entry, TRUE);
        if (RULE_TYPE_OK != result)
        {
            /* undo om setting */
            RULE_OM_SetAceFieldByIndex(ace_index, &org_ace, field_index, RULE_TYPE_ACE_OWNED_BY_ACL);

            if (is_modified_time_range)
                {
                RULE_MGR_RequestTimeRangeReference(FALSE, ace_entry->time_range_name, &(ace_entry->time_range_index));
            }
            return result;
        }

        if (is_modified_time_range)
                {
            RULE_MGR_RequestTimeRangeReference(FALSE, org_ace.time_range_name, &(org_ace.time_range_index));
            }
        }
    else if (RULE_TYPE_OK == RULE_OM_GetClassMapIndexByMf(ace_index, &class_map_index)) /* this ace is own by ACL */
    {
        /* delete original rule */
        result = RULE_CTRL_RemoveClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, ace_index);
        if (RULE_TYPE_OK != result)
            return result;

        result = RULE_OM_SetAceFieldByIndex(ace_index, ace_entry, field_index, RULE_TYPE_ACE_OWNED_BY_CLASS_MAP);
        if (RULE_TYPE_OK != result)
        {
            /* restore chip config */
            RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, ace_index);
            return result;
        }

        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, ace_index);
        if (RULE_TYPE_OK != result)
        {
            /* undo om setting */
            RULE_OM_SetAceFieldByIndex(ace_index, &org_ace, field_index, RULE_TYPE_ACE_OWNED_BY_CLASS_MAP);
            return result;
        }
    }
    else
    {
        if (is_modified_time_range &&
            TRUE != RULE_MGR_RequestTimeRangeReference(TRUE,
                                                       ace_entry->time_range_name,
                                                       &(ace_entry->time_range_index)))
                {
                    return RULE_TYPE_FAIL;
                }

        result = RULE_OM_SetAceFieldByIndex(ace_index, ace_entry, field_index, RULE_TYPE_ACE_OWNED_BY_ACL);
        if (RULE_TYPE_OK != result)
        {
            if (is_modified_time_range)
                {
                RULE_MGR_RequestTimeRangeReference(FALSE, ace_entry->time_range_name, &(ace_entry->time_range_index));
            }

            return result;
    }

        if (is_modified_time_range)
            {
            RULE_MGR_RequestTimeRangeReference(FALSE, org_ace.time_range_name, &(org_ace.time_range_index));
        }
    }

    return RULE_TYPE_OK;
}

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
    UI32_T field_index)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_Ace_Entry_T  user_ace;

    if (NULL == ace_entry_p)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    result = RULE_OM_ConvertUIAce2Ace(ace_entry_p, &user_ace);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    return RULE_MGR_SetAceFieldByIndex(ace_index,&user_ace, field_index);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_SetAceRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  set ace row status by index
 * INPUT:    acl_index, ace_type, row_status
 * OUTPUT:   NONE.
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     ace_index and ace_type MUST ALL MATCH if not create
 *           create a specified ace (index + type) if row status is createAndGo
 *           or createAndWait
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_SetAceRowStatus(UI32_T ace_index, RULE_TYPE_AclType_T ace_type, UI32_T row_status)
{
    UI32_T                  ret, org_status = L_RSTATUS_NOT_EXIST;
    RULE_TYPE_AclType_T     org_type;
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    RULE_TYPE_Ace_Entry_T   ace_entry;

    if (RULE_TYPE_OK != RULE_OM_GetAceByIndex(ace_index, &ace_entry))
    {
        org_status = L_RSTATUS_NOT_EXIST;
    }
#endif

    if (RULE_TYPE_OK == RULE_OM_GetAceTypeAndRowStatus(ace_index, &org_type, &org_status))
    {
        if (FALSE == RULE_OM_Is_AclType_Matched (ace_type, org_type))
            return RULE_TYPE_FAIL; /* ace found but ace_type doesn't match */
    }

    switch (L_RSTATUS_Fsm(row_status, &org_status, RULE_MGR_LocalAceRStatusCallBack, &ace_index))
    {
        case L_RSTATUS_NOTEXIST_2_NOTEXIST:
            return RULE_TYPE_FAIL;

        case L_RSTATUS_NOTEXIST_2_NOTREADY:
            ret = RULE_OM_CreateAce(ace_index, ace_type, FALSE);
            break;

        case L_RSTATUS_NOTEXIST_2_ACTIVE:
            ret = RULE_OM_CreateAce(ace_index, ace_type, TRUE);
            break;

        case L_RSTATUS_NOTREADY_2_NOTEXIST:
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
            if (strlen((char *)ace_entry.time_range_name) != 0)
                TIME_RANGE_PMGR_RequestTimeRangeEntry(ace_entry.time_range_name, FALSE, &ace_entry.time_range_index);
#endif
            ret = RULE_OM_DestroyAce(ace_index);
            break;

        case L_RSTATUS_ACTIVE_2_NOTEXIST:
            /* let it to be noReady first then delete it */
            if (RULE_TYPE_OK != RULE_MGR_LocalSetAceNotReady(ace_index))
                return RULE_TYPE_FAIL;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
            if (strlen((char *)ace_entry.time_range_name) != 0)
                TIME_RANGE_PMGR_RequestTimeRangeEntry(ace_entry.time_range_name, FALSE, &ace_entry.time_range_index);
#endif
            ret = RULE_OM_DestroyAce(ace_index);
            break;

        case L_RSTATUS_NOTREADY_2_NOTREADY: /* do nothing */
        case L_RSTATUS_ACTIVE_2_ACTIVE:
            ret = RULE_TYPE_OK;
            break;

        case L_RSTATUS_NOTREADY_2_ACTIVE:
            ret = RULE_OM_SetAceRowStatus(ace_index, TRUE);
            break;

        case L_RSTATUS_ACTIVE_2_NOTREADY:
            ret = RULE_MGR_LocalSetAceNotReady(ace_index);
            break;

        case L_RSTATUS_TRANSITION_STATE_ERROR:
        default:
            return RULE_TYPE_FAIL;
    }

    return ret;
}

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
UI32_T RULE_MGR_GetAceByIndex(UI32_T ace_index, RULE_TYPE_Ace_Entry_T *ace_entry)
{
    return RULE_OM_GetAceByIndex(ace_index, ace_entry);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_GetNextAceByAceTypeAndIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next ACE by its index and ace type
 * INPUT:    ace_index, ace_type (RULE_TYPE_IP_ACL, RULE_TYPE_MAC_ACL, RULE_TYPE_IPV6_ACL)
 * OUTPUT:   ace_index, ace_entry
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_GetNextAceByAceTypeAndIndex(UI32_T ace_type, UI32_T *ace_index, RULE_TYPE_Ace_Entry_T *ace_entry)
{
    return RULE_OM_GetNextAceByAceTypeAndIndex(ace_type, ace_index, ace_entry);
}

#if (SYS_CPNT_ACL_MIRROR == TRUE)
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
    UI32_T acl_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T w_acl_index;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    RULE_TYPE_Acl_T acl_entry;
#endif

    if (FALSE == SWCTRL_SetAclMirrorDestPort(ifindex_dest, TRUE))
    {
        return RULE_TYPE_E_DEST_PORT;
    }

    result = RULE_OM_AddAclMirrorEntry(ifindex_dest, acl_index);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    w_acl_index = acl_index;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)

    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    if (TRUE == acl_entry.flag_aggregate_done)
    {
        w_acl_index = acl_entry.aggregate_acl_index;
    }
    else
    {
        ASSERT(0);
        return RULE_TYPE_FAIL;
    }

#endif /* #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) */

    result = RULE_CTRL_SetAclMirror(w_acl_index);
    if (RULE_TYPE_OK != result)
    {
        RULE_OM_RemoveAclMirrorEntry(ifindex_dest, acl_index);
    }

    return result;
}

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
    UI32_T acl_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T                  w_acl_index;

    UI32_T                  tmp_ifindex;
    UI32_T                  tmp_acl_index;
    RULE_TYPE_UI_AclEntry_T tmp_acl_entry;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    RULE_TYPE_Acl_T         acl_entry;
#endif

    result = RULE_OM_RemoveAclMirrorEntry(ifindex_dest, acl_index);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    w_acl_index = acl_index;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    if (TRUE == acl_entry.flag_aggregate_done)
    {
        w_acl_index = acl_entry.aggregate_acl_index;
    }
    else
    {
        ASSERT(0);
        return RULE_TYPE_FAIL;
    }
#endif /* #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) */

    result = RULE_CTRL_SetAclMirror(w_acl_index);
    if (RULE_TYPE_OK != result)
    {
        RULE_OM_AddAclMirrorEntry(ifindex_dest, acl_index);
    }

    if (RULE_TYPE_OK == result)
    {
        tmp_ifindex = 0;

        if (RULE_TYPE_OK != RULE_OM_GetNextAclMirrorEntry(&tmp_ifindex,
                                                &tmp_acl_index, &tmp_acl_entry))
        {
            if (FALSE == SWCTRL_SetAclMirrorDestPort(ifindex_dest, FALSE))
            {
                result = RULE_TYPE_E_DEST_PORT;
            }
        }
    }

    return result;
}

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
    RULE_TYPE_UI_AclEntry_T *acl_entry)
{
    UI32_T ret;

    ret = RULE_OM_GetNextAclMirrorEntry(ifindex_dest, acl_index, acl_entry);

    return ret;
}
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

/* FIXME: Remove this function and relate func in L4_PMGR ~ RULE_CTRL.
 *        Replaced by RULE_MGR_GetTcamEntry.
 */
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
BOOL_T RULE_MGR_GetTCAMUtilization(RULE_TYPE_TCAMUtilization_T *tcam_util_p)
{
    return RULE_CTRL_GetTCAMUtilization(tcam_util_p);
}

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
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p)
{
    return RULE_CTRL_GetTcamEntry(tcam_entry_p);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_GetNextTcamEntry
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
RULE_TYPE_RETURN_TYPE_T
RULE_MGR_GetNextTcamEntry(
    RULE_TYPE_TCAM_ENTRY_T *tcam_entry_p)
{
    return RULE_CTRL_GetNextTcamEntry(tcam_entry_p);
}

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
    RULE_TYPE_TCAM_CAP_BITMAP_T *bit_map_p)
{
    RULE_TYPE_RETURN_TYPE_T result;

    ASSERT(bit_map_p != NULL);

    result = RULE_CTRL_GetSupportedTcamCapabilityBitMap(bit_map_p);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    RULE_TYPE_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_ALL);
    RULE_TYPE_SET_BIT_ON(bit_map_p->bits, RULE_TYPE_TCAM_CAP_RESERVED);

    return RULE_TYPE_OK;
}

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
    RULE_TYPE_UI_PortEntry_T *port_entry)
{
    if (TRUE == SWCTRL_LogicalPortExisting(ifindex) ||
        TRUE == RULE_TYPE_IS_CPU_PORT(ifindex) ||
        TRUE == RULE_TYPE_IS_ALL_PORTS(ifindex))
{
    return RULE_OM_GetUIPortEntry(ifindex, inout_profile, port_entry);
}

    return RULE_TYPE_INVALID_PARAMETER;
}

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
    RULE_TYPE_UI_PortEntry_T *port_entry_p)
{
    while (RULE_TYPE_OK == RULE_OM_GetNextUIPortEntry(ifindex_p, inout_profile_p, port_entry_p))
    {
        if (*inout_profile_p == RULE_TYPE_OUTBOUND)
        {
            /* MIB diffServPortEntry only need show ingress
             */
            continue;
        }

        if (TRUE == SWCTRL_LogicalPortExisting(*ifindex_p))
        {
            return RULE_TYPE_OK;
        }

        if (RULE_TYPE_IS_ALL_PORTS(*ifindex_p) ||
            RULE_TYPE_IS_CPU_PORT(*ifindex_p))
{
            return RULE_TYPE_OK;
        }
    }

    return RULE_TYPE_FAIL;
}

// ???: Remove ???
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
UI32_T RULE_MGR_TrapPacket2Cpu(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info)
{
    if (FALSE == RULE_CTRL_TrapPacket2Cpu(enable_flag, packet_type, rule_info))
        return RULE_TYPE_FAIL;

    return RULE_TYPE_OK;
}

// ???: Remove ???
/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_MirrorPacket2Cpu
 *------------------------------------------------------------------------------
 * PURPOSE:  copy packet to CPU or not
 * INPUT:    enable_flag, packet_type, rule_info
 * OUTPUT:   none
 * RETURN:   Error code (OK / FAIL)
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
UI32_T RULE_MGR_MirrorPacket2Cpu(BOOL_T enable_flag, RULE_TYPE_PacketType_T packet_type, RULE_TYPE_CpuRuleInfo_T *rule_info)
{
    if (FALSE == RULE_CTRL_MirrorPacket2Cpu(enable_flag, packet_type, rule_info))
        return RULE_TYPE_FAIL;

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_SetTcpPortCosMap(UI32_T unit, UI32_T port, UI16_T tcp_port, UI8_T cos_value, UI8_T mapping_index)
{
#if (SYS_CPNT_COS_ING_IP_PORT_TO_COS == TRUE)
    if (FALSE == RULE_CTRL_SetL4PortCosMap(unit, port, tcp_port, cos_value))
        return RULE_TYPE_FAIL;
#endif /* #if (SYS_CPNT_COS_ING_IP_PORT_TO_COS == TRUE) */
    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_DeleteTcpPortCosMap(UI32_T unit, UI32_T port, UI16_T tcp_port, UI8_T cos_value, UI8_T mapping_index)
{
#if (SYS_CPNT_COS_ING_IP_PORT_TO_COS == TRUE)
    if (FALSE == RULE_CTRL_DelL4PortCosMap(unit, port, tcp_port, cos_value))
        return RULE_TYPE_FAIL;
#endif /* #if (SYS_CPNT_COS_ING_IP_PORT_TO_COS == TRUE) */
    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_NotifyIpDscp2CosChangeByLport(UI32_T lport, UI8_T ip_dscp)
{
    if (FALSE == RULE_CTRL_NotifyIpDscp2CosChangeByLport(lport, ip_dscp))
        return RULE_TYPE_FAIL;

    return RULE_TYPE_OK;
}

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
UI32_T RULE_MGR_NotifyIpPrecedence2CosChangeByLport(UI32_T lport, UI8_T ip_precedence)
{
    if (FALSE == RULE_CTRL_NotifyIpPrecedence2CosChangeByLport(lport, ip_precedence))
        return RULE_TYPE_FAIL;

    return RULE_TYPE_OK;
}

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
BOOL_T RULE_MGR_SetRedirectHTTPClientPackets(UI32_T unit, UI32_T port, BOOL_T is_enable)
{
    RULE_CTRL_UnitPort_T unit_port;
    unit_port.unit = unit;
    unit_port.port = port;
    if (FALSE == RULE_CTRL_Service(is_enable, RULE_CTRL_WEBAUTH_REDIR_HTTP, &unit_port))
        return FALSE;

    if (FALSE == RULE_CTRL_Service(is_enable, RULE_CTRL_WEBAUTH_PERMIT_DHCPC, &unit_port))
        return FALSE;

    if (FALSE == RULE_CTRL_Service(is_enable, RULE_CTRL_WEBAUTH_PERMIT_DNS, &unit_port))
        return FALSE;

    return TRUE;
}

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
    UI32_T unit, UI32_T port, UI32_T sip, BOOL_T is_enable)
{
    RULE_CTRL_UnitPortSip_T unit_port_sip;
    unit_port_sip.unit = unit;
    unit_port_sip.port = port;
    unit_port_sip.sip = sip;
    return RULE_CTRL_Service(is_enable, RULE_CTRL_WEBAUTH_PERMIT_SIP, &unit_port_sip);
}

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
    UI32_T unit, UI32_T port, BOOL_T is_enable)
{
    RULE_CTRL_UnitPort_T unit_port;
    unit_port.unit = unit;
    unit_port.port = port;
    return RULE_CTRL_Service(is_enable, RULE_CTRL_WEBAUTH_DENY_IP, &unit_port);
}
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
BOOL_T RULE_MGR_TrapLinkLocalToCPU(BOOL_T is_enable, UI8_T link_local_addr[SYS_ADPT_IPV6_ADDR_LEN], UI32_T vlan_id)
{
    RULE_TYPE_CpuRuleInfo_T info;
    BOOL_T rc;

    memcpy(info.linklocal.addr, link_local_addr, sizeof(info.linklocal.addr));
    info.linklocal.vlan_id = vlan_id;

    rc = RULE_CTRL_TrapPacket2Cpu(is_enable, RULE_TYPE_PacketType_LinkLocal, &info);

    return rc;

}
#endif /* #if (SYS_CPNT_IPV6 == TRUE) */



/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalAddChipPolicyMapConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : add class-map config on chip for every ports bound with specific
 *            policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : 1, if there is nothing to config, return OK (do nothing)
 *            2, this func used when a policy-map already config to chip and
 *               dynamically add a class-map
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_MGR_LocalAddChipPolicyMapConfig(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_map_index)
{
    RULE_TYPE_RETURN_TYPE_T     result;

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    result = RULE_OM_RebuildBundleGroup(policy_map_index);
    if (RULE_TYPE_OK != result)
    {
        LOG("Failed to rebuild bundle group");
        return result;
    }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    result = RULE_CTRL_SetPolicyMap(ifindex, direction, policy_map_index, TRUE);
    return result;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalRemoveChipPolicyMapConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : remove class-map config on chip for every ports bound with
 *            specific policy-map and class-map
 * INPUT    : policy_map_index, class_map_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : if there is nothing to free, return OK (do nothing)
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_MGR_LocalRemoveChipPolicyMapConfig(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_map_index)
{
    return RULE_CTRL_SetPolicyMap(ifindex, direction, policy_map_index, FALSE);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalRemoveChipAclElementConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : remove ace config on chip for every ports bound with specific acl
 * INPUT    : acl_index, ace_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : if there is nothing to free, return OK (do nothing)
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_MGR_LocalRemoveChipAclElementConfig(UI32_T acl_index, UI32_T ace_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_Ace_Entry_T   ace_entry;

    result = RULE_OM_GetAceByIndex(ace_index, &ace_entry);
    if (RULE_TYPE_OK != result)
        return result;

    /* free resource */
    result = RULE_CTRL_SetAce(acl_index, &ace_entry, FALSE);
    if (RULE_TYPE_OK != result)
        return result;

    return RULE_TYPE_OK;
}

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalUpdateChipCompressAclConfig
 *------------------------------------------------------------------------------
 * PURPOSE  : Updates rule into chip if ACE changed in ACL
 * INPUT    : acl_index - ACL index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : If this function failed, all rule will be removed from chip.
 *            Then, the CALLER must undo all rule into chip.
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_MGR_LocalUpdateChipCompressAclConfig(
    UI32_T acl_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T              agg_ace_index;
    UI32_T              end_ace_index;
    RULE_TYPE_AclType_T agg_ace_type;
    RULE_TYPE_Ace_Entry_T agg_ace_entry;

    RULE_TYPE_Acl_T     acl_entry;

    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    /* The corresponding ACL of acl_index MUST NOT be working ACL
     */
    ASSERT(FALSE == acl_entry.flag_hide);

    if (TRUE == acl_entry.flag_aggregate_done &&
        0 != acl_entry.aggregate_acl_index)
    {
        agg_ace_index = 0;
        while (RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_entry.aggregate_acl_index,
                                                       &agg_ace_type,
                                                       &agg_ace_index,
                                                       &agg_ace_entry))
        {
            RULE_CTRL_SetAce(acl_entry.aggregate_acl_index, &agg_ace_entry, FALSE);
        }
    }

    result = RULE_OM_RebuildAggregateAcl(acl_index);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    /* Update ACL entry. Because the context have changed after rebuild
     * aggregate ACL.
     */
    RULE_OM_GetAclByIndex(acl_index, &acl_entry);

    agg_ace_index = 0;
    while (RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_entry.aggregate_acl_index,
                                                   &agg_ace_type,
                                                   &agg_ace_index,
                                                   &agg_ace_entry))
    {
        result = RULE_CTRL_SetAce(acl_entry.aggregate_acl_index, &agg_ace_entry, TRUE);
        if (RULE_TYPE_OK != result)
        {
            end_ace_index = agg_ace_index;
            goto UpdateChipCompressAclConfig_Restore;
        }
    }

    return RULE_TYPE_OK;

UpdateChipCompressAclConfig_Restore:
    agg_ace_index = 0;
    while (RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_entry.aggregate_acl_index,
                                                   &agg_ace_type,
                                                   &agg_ace_index,
                                                   &agg_ace_entry))
    {
        if (agg_ace_index == end_ace_index)
        {
            break;
        }

        RULE_CTRL_SetAce(acl_entry.aggregate_acl_index, &agg_ace_entry, FALSE);
    }

    return result;
}
#endif /* #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - RULE_MGR_LocalNotify
 *------------------------------------------------------------------------------
 * PURPOSE:  Nofify event
 * INPUT:    message            - message
 *           param1             - A pointer to the identifier of the object
 *                                sending the message. Exactly value depend on
 *                                message.
 *           param2             - A pointer to extra information. Exactly value
 *                                depend on message.
 * OUTPUT:   None
 * RETURN:   RULE_TYPE_OK; Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_MGR_LocalNotify(
    RULE_TYPE_NOTIFY_MESSAGE_T message,
    void *param1,
    void *param2)
{
    switch (message)
    {
        case RULE_TYPE_NOTIFY_CLASS_MAP_DELETING:
        {
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
            UI32_T class_map_index = *(UI32_T *)param1;
            UI32_T policy_map_index;

            policy_map_index = 0;

            while (RULE_TYPE_OK == RULE_OM_GetNextPolicyMapByClassMapIndex(class_map_index, &policy_map_index))
            {
                if (RULE_TYPE_OK == RULE_OM_IsClassMapInBundleGroup(policy_map_index, class_map_index))
                {
                    return RULE_TYPE_FAIL;
                }
            }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */
            break;
        }

        case RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_CLASS_DELETING:
        {
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
            UI32_T class_map_index = *(UI32_T *)param1;
            UI32_T policy_map_index = *(UI32_T *)param2;

            if (RULE_TYPE_OK == RULE_OM_IsClassMapInBundleGroup(policy_map_index, class_map_index))
            {
                return RULE_TYPE_FAIL;
            }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

            break;
        }

        case RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_METER_DELETING:
        {
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
            UI32_T class_map_index = *(UI32_T *)param1;
            UI32_T policy_map_index = *(UI32_T *)param2;

            if (RULE_TYPE_OK == RULE_OM_IsClassMapInBundleGroup(policy_map_index, class_map_index))
            {
                return RULE_TYPE_FAIL;
            }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

            break;
        }

        case RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_IN_ACTION_DELETING:
            break;

        case RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_OUT_OF_PROFILE_ACTION_DELETING:
        {
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
            UI32_T class_map_index = *(UI32_T *)param1;
            UI32_T policy_map_index = *(UI32_T *)param2;

            if (RULE_TYPE_OK == RULE_OM_IsClassMapInBundleGroup(policy_map_index, class_map_index))
            {
                return RULE_TYPE_FAIL;
            }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

            break;
        }

        case RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_CLASS_ACTION_DELETING:
            break;


        default:
            break;
    }
    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalSetPolicyMapNotReady
 *------------------------------------------------------------------------------
 * PURPOSE  : unbind port and free hardware resource
 * INPUT    : policy_map_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : if there is nothing to free, return OK (do nothing)
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_MGR_LocalSetPolicyMapNotReady(UI32_T policy_map_index)
{
    UI8_T   policy_map_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_POLICY_MAP_LIST];
    UI8_T   port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
    UI32_T  port_nbr;

    /* MUST check policy_map_index to avoid memory corrupt in RULE_MGR_SET_BIT_ON() */
    if (RULE_MGR_IS_BAD_POLICY_MAP_IDX(policy_map_index))
        return RULE_TYPE_FAIL;

    memset(policy_map_list, 0, sizeof(policy_map_list));
    memset(port_list, 0, sizeof(port_list));

    RULE_MGR_SET_BIT_ON(policy_map_list, (policy_map_index - 1)); /* to zero-based index */

    /* should we remove rule from chip if set status of a policy-map to notReady ?
     */
    if (RULE_TYPE_OK != RULE_OM_Get_PortListByPolicyMap(policy_map_list, TRUE, port_list, &port_nbr) &&
        RULE_TYPE_OK != RULE_OM_Get_PortListByPolicyMap(policy_map_list, FALSE, port_list, &port_nbr))
    {
        return RULE_OM_SetPolicyMapRowStatus(policy_map_index, FALSE); /* nothing to free */
    }
    else
        return RULE_TYPE_FAIL;/* If policy-map is bound to port, retrun fail */
#if 0
    /*should free all resource of those port list*/
    for (index = 0, match_counter = 0; (match_counter < port_nbr) && (SYS_ADPT_TOTAL_NBR_OF_LPORT > index); ++index)
    {
        if (RULE_MGR_IS_BIT_OFF(port_list, index))
            continue;

        ++match_counter;
        ok_flag &= (RULE_TYPE_OK == RULE_MGR_LocalRemoveChipPolicyMapConfig(index + 1, policy_map_index));
        ok_flag &= (RULE_TYPE_OK == RULE_OM_UnBindPort2PolicyMap(index + 1, policy_map_index));
    }

    ok_flag &= (RULE_TYPE_OK == RULE_OM_SetPolicyMapRowStatus(policy_map_index, FALSE));

    return (TRUE == ok_flag) ? RULE_TYPE_OK : RULE_TYPE_FAIL;
#endif
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalSetPolicyMapElementNotReady
 *------------------------------------------------------------------------------
 * PURPOSE  : break relationship with policy-map and free hardware resource
 * INPUT    : policy_element_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : if there is no policy-map own it, return OK (do nothing)
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_MGR_LocalSetPolicyMapElementNotReady(UI32_T policy_element_index)
{
    UI32_T  policy_map_index;

    /* if this element doesn't belong to any one policy-map, nothing to do */
    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapIndexByElementIndex(policy_element_index, &policy_map_index))
        return RULE_OM_SetPolicyMapElementRowStatus(policy_element_index, FALSE);
    else
        return RULE_TYPE_FAIL;/* If belongs to any policy-map, return fail. */
#if 0
    ret = RULE_MGR_SetPolicyMapAttachCtrl(policy_map_index, policy_element_index,
            VAL_diffServPolicyMapAttachCtlAction_notAttaching); /* be careful with semaphore */

    if (RULE_TYPE_OK != ret)
        return RULE_TYPE_FAIL;

    ret = RULE_OM_SetPolicyMapElementRowStatus(policy_element_index, FALSE);
    return ret;
#endif
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalSetClassMapNotReady
 *------------------------------------------------------------------------------
 * PURPOSE  : break relationship with policy-map and free hardware resource
 * INPUT    : class_map_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : if there is no policy-map own it, return OK (do nothing)
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_MGR_LocalSetClassMapNotReady(UI32_T class_map_index)
{
    UI32_T  policy_map_nbr = 0;
    UI8_T   policy_map_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_POLICY_MAP_LIST];

    memset(policy_map_list, 0, sizeof(policy_map_list));

    /* if this class-map belongs to any one policy-map, MUST free hardware resource */
    if (RULE_TYPE_OK == RULE_OM_Get_PolicyMapListByClassMap(class_map_index, policy_map_list, &policy_map_nbr))
    {
#if 0/* If belongs to any policy-map, return fail. */
        for (index = 0, hit_counter = 0; SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP > index; ++index)
        {
            RULE_TYPE_RETURN_TYPE_T result;

            if (hit_counter >= policy_map_nbr) /* no more policy-map in remainder bits */
                break;

            if (RULE_MGR_IS_BIT_OFF(policy_map_list, index))
                continue;

            ++hit_counter;

            result = RULE_CTRL_RemoveClassMapConfig(index + 1, class_map_index, TRUE);
            if (RULE_TYPE_OK != result)
            {
                if (RULE_MGR_IS_DEBUG_ERROR_ON(debug_flag))
                    printf("\r\n[RULE_MGR_LocalSetClassMapNotReady] remove class-map failed");
                return result;
            }

            /* specified class-map had been associated with policy-map
             * need to remove it from those policy-map here
             */
            if (RULE_TYPE_OK != RULE_OM_DeletePolicyMapElement(index + 1, class_map_index)) /* must tanslate index to 1-based */
            {
                if (RULE_MGR_IS_DEBUG_ERROR_ON(debug_flag))
                    printf("\r\n[RULE_MGR_LocalSetClassMapNotReady] delete element failed");
                return RULE_TYPE_FAIL;
            }
        }
#else
        return RULE_TYPE_FAIL;
#endif
    }

    /* update om */
    if (RULE_TYPE_OK != RULE_OM_SetClassMapRowStatus(class_map_index, FALSE))
    {
        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalSetMeterNotReady
 *------------------------------------------------------------------------------
 * PURPOSE  : break relationship with policy-map element and free hardware resource
 * INPUT    : meter_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : if there is no policy-map element own it, return OK (do nothing)
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_MGR_LocalSetMeterNotReady(UI32_T meter_index)
{
    UI32_T                      policy_element_index;

    /* if this meter doesn't belong to any one policy-map element, do nothing */
    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapElementIndexByMeter(meter_index, &policy_element_index))
        return RULE_OM_SetMeterRowStatus(meter_index, FALSE);
    else
        return RULE_TYPE_FAIL;
#if 0
    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapElement(policy_element_index, &policy_element))
        return RULE_TYPE_FAIL; /* should not go here */

    /* if this element belongs to one policy-map, MUST free hardware resource */
    if (RULE_TYPE_OK == RULE_OM_GetPolicyMapIndexByElementIndex(policy_element_index, &policy_map_index))
    {
        RULE_TYPE_RETURN_TYPE_T     result;

        result = RULE_CTRL_RemoveMeterConfig(policy_map_index, policy_element.class_map_index);

        if (RULE_TYPE_OK != result)
            return result;
    }

    /* set out action to NULL while no meter
     * And didn't check action_index so do not care return value
     */
    ret = RULE_OM_SetOutOfProfileActionToNull(policy_element.action_index);

    policy_element.meter_index = 0; /* break relationship */

    /* update om */
    if ((RULE_TYPE_OK != RULE_OM_SetPolicyMapElement(policy_element_index, &policy_element)) ||
        (RULE_TYPE_OK != RULE_OM_SetMeterRowStatus(meter_index, FALSE)))
    {
        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
#endif
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalSetActionNotReady
 *------------------------------------------------------------------------------
 * PURPOSE  : set row status to notReady
 * INPUT    : action_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : if there is any one policy-map element own it, return FAIL (not allow to set)
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_MGR_LocalSetActionNotReady(UI32_T action_index)
{
    UI32_T                      policy_element_index;

    /* if this action belong to one policy-map element, do not allow to do it */
    if (RULE_TYPE_OK == RULE_OM_GetPolicyMapElementIndexByAction(action_index, &policy_element_index))
        return RULE_TYPE_FAIL;

    return RULE_OM_SetActionRowStatus(action_index, FALSE);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalSetAclNotReady
 *------------------------------------------------------------------------------
 * PURPOSE  : break relationship with class-map/ports and free hardware resource
 * INPUT    : acl_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : if there is no class-map/port associated with it, return OK (do nothing)
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_MGR_LocalSetAclNotReady(UI32_T acl_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_InOutDirection_T direction;

    UI32_T fail_bitmap = 0;
    RULE_TYPE_Acl_T acl_entry;

    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    for (direction = RULE_TYPE_INBOUND; direction <= RULE_TYPE_OUTBOUND; ++ direction)
    {
        UI32_T plist_idx;

        BOOL_T is_ingress = RULE_TYPE_DIRECTION_TO_IS_INGRESS(direction);

        UI8_T *plist;
        UI32_T count_of_plist;

        if (RULE_TYPE_INBOUND == direction)
        {
            plist = acl_entry.filtering_in_port_list;
            count_of_plist = _countof(acl_entry.filtering_in_port_list) * 8;
        }
        else
        {
            plist = acl_entry.filtering_out_port_list;
            count_of_plist = _countof(acl_entry.filtering_out_port_list) * 8;
        }

        for (plist_idx = 0; plist_idx < count_of_plist; ++ plist_idx)
        {
            if (RULE_MGR_IS_BIT_ON(plist, plist_idx))
            {
                UI32_T ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(plist_idx);
                RULE_TYPE_InOutDirection_T direction = RULE_TYPE_IS_INGRESS_TO_DIRECTION(is_ingress);
                UI32_T port_acl_index;
                UI32_T port_time_range_index;
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                UI8_T port_time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
#else
                UI8_T *port_time_range_name = NULL;
#endif
                RULE_TYPE_COUNTER_ENABLE_T port_counter_enable;

                result = RULE_OM_GetPortAclIndex(ifindex, acl_entry.acl_type,
                                                 is_ingress,
                                                 &port_acl_index,
                                                 port_time_range_name,
                                                 &port_counter_enable);
                ASSERT(RULE_TYPE_OK == result);
                ASSERT(acl_index == port_acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
                ASSERT(TRUE == acl_entry.flag_aggregate_done);
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

                result = RULE_CTRL_SetAcl(ifindex,
                                          direction,
#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
                                          acl_entry.aggregate_acl_index,
#else
                                          acl_index,
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */
                                          FALSE,
                                          RULE_TYPE_UNDEF_TIME_RANGE,
                                          port_counter_enable);
                if (RULE_TYPE_OK != result)
                {
                    fail_bitmap |= 0x02; /* even fail, go ahead */
                }

                result = RULE_OM_ResetAcl(ifindex, direction, acl_index);
                ASSERT(RULE_TYPE_OK == result);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                TIME_RANGE_PMGR_RequestTimeRangeEntry(port_time_range_name, FALSE, &port_time_range_index);
#endif
            }
        }
    }

    if (0 < acl_entry.class_map_count) /* if this acl at least associated with one class-map, need to find them out */
    {
        UI32_T  class_map_bit_cnt, class_map_match_cnt;

        /* per class-map check */
        for ( class_map_bit_cnt = 0, class_map_match_cnt = 0;
              (class_map_match_cnt < acl_entry.class_map_count) &&
              (SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP > class_map_bit_cnt);
              ++class_map_bit_cnt )
        {
            RULE_TYPE_RETURN_TYPE_T result;

            if (RULE_MGR_IS_BIT_OFF(acl_entry.class_map_bit_list, class_map_bit_cnt))
                continue;

            ++class_map_match_cnt;

            result = RULE_CTRL_RemoveClassMapElementConfig (class_map_bit_cnt +1, RULE_TYPE_CLASS_ACL, acl_index);
            /* free resource */
            if (RULE_TYPE_OK != result)
                fail_bitmap |= 0x04; /* even fail, go ahead */

            /* update om */
            if (RULE_TYPE_OK != RULE_OM_RemoveClassMapElementByAclIndex(class_map_bit_cnt + 1, acl_index)) /* to 1-based */
                fail_bitmap |= 0x08; /* even fail, go ahead */
        }
    }

    /* update om */
    if (RULE_TYPE_OK != RULE_OM_SetAclRowStatus(acl_index, FALSE))
        fail_bitmap |= 0x10; /* even fail, go ahead */

    if (0 != fail_bitmap)
    {
        if (RULE_MGR_IS_DEBUG_ERROR_ON(RULE_OM_GetDebugFlag()))
            printf ("\r\n [RULE_MGR_LocalSetAclNotReady] fail_bitmap -> %lX.", fail_bitmap);

        return RULE_TYPE_FAIL;
    }

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalSetPolicyMapPortActive2NotReady
 *------------------------------------------------------------------------------
 * PURPOSE  : Set policy map port entry row status Active to notReady
 * INPUT    : ifindex, direction
 * OUTPUT   : None
 * RETURN   : error code (OK / FAIL)
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_MGR_LocalSetPolicyMapPortActive2NotReady(
    UI32_T ifindex,
    UI32_T direction)
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T old_policy_map_index, old_row_status;

    result = RULE_OM_GetPortBindedPolicyMap(ifindex,
                                            direction,
                                            &old_policy_map_index,
                                            &old_row_status);
    if(RULE_TYPE_OK != result ||
       TRUE != RULE_MGR_IS_ENTRY_ACTIVE(old_row_status))
    {
        LOG("Policy map port row status is not Active");
        return RULE_TYPE_FAIL;
    }

    if (TRUE == RULE_OM_IsBindDynamicPolicyMapOnPort(ifindex, direction))
    {
        return RULE_OM_SetPortEntryPolicyMapRowStatus(ifindex, direction,
                                                      VAL_diffServPolicyMapStatus_notReady);
    }

    if (RULE_TYPE_OK != RULE_MGR_LocalRemoveChipPolicyMapConfig(ifindex, direction, old_policy_map_index))
    {
        return RULE_TYPE_FAIL;
    }

    result = RULE_OM_UnBindPort2PolicyMap(ifindex, direction, old_policy_map_index);
    ASSERT(RULE_TYPE_OK == result);

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalSetPolicyMapPortNotReady2Active
 *------------------------------------------------------------------------------
 * PURPOSE  : Set policy map port entry row status notReady to Active
 * INPUT    : ifindex, direction
 * OUTPUT   : None
 * RETURN   : error code (OK / FAIL)
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_MGR_LocalSetPolicyMapPortNotReady2Active(
    UI32_T ifindex,
    UI32_T direction)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_PolicyMap_T     policy_map;
    RULE_TYPE_PolicyElement_T policy_element;
    UI32_T  row_status;
    UI32_T  policy_map_index, port_row_status;
    UI16_T  i;

    result = RULE_OM_GetPortBindedPolicyMap(ifindex, direction, &policy_map_index, &port_row_status);
    if(RULE_TYPE_OK != result ||
       TRUE == RULE_MGR_IS_ENTRY_ACTIVE(port_row_status))
    {
        LOG("Policy map port row status is not NotReady");
        return RULE_TYPE_FAIL;
    }

    /* check if policy-map is not ready
     */
    if ((RULE_TYPE_OK != RULE_OM_GetPolicyMapRowStatus (policy_map_index, &row_status)) ||
        (FALSE == RULE_MGR_IS_ENTRY_ACTIVE(row_status)))
        return RULE_TYPE_FAIL;

    /* can't bind an empty policy
     */
    if (RULE_TYPE_OK != RULE_OM_GetPolicyMap(policy_map_index, &policy_map))
    {
        return RULE_TYPE_FAIL;
    }

    if (policy_map.element_count == 0)
    {
        return RULE_TYPE_FAIL;
    }

    for (i = 0; i < policy_map.element_count; i++)
    {
        if (RULE_TYPE_OK == RULE_OM_GetPolicyMapElement(policy_map.policy_map_element_index_list[i], &policy_element))
        {
            if(policy_element.class_map_index == 0 || policy_element.action_index == 0)
            {
                return RULE_TYPE_FAIL;
            }
        }
        else
        {
            return RULE_TYPE_FAIL;
        }
    }

    if (TRUE == RULE_OM_IsBindDynamicPolicyMapOnPort(ifindex, direction))
    {
        result = RULE_OM_SetPortEntryPolicyMapRowStatus(ifindex, direction,
                                                        VAL_diffServPolicyMapPortPortStatus_active);
        return result;
    }

    result = RULE_MGR_LocalAddChipPolicyMapConfig(ifindex, direction, policy_map_index);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    result = RULE_OM_BindPort2PolicyMap(ifindex, direction, policy_map_index);
    ASSERT(RULE_TYPE_OK == result);

    return result;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalSetAccessGroupNotReady2Active
 *------------------------------------------------------------------------------
 * PURPOSE  : Set access group entry row status notReady to Active
 * INPUT    : ifindex, direction, acl_type
 * OUTPUT   : None
 * RETURN   : error code (OK / FAIL)
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static RULE_TYPE_RETURN_TYPE_T
RULE_MGR_LocalSetAccessGroupNotReady2Active(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type)
{
    RULE_TYPE_RETURN_TYPE_T    result;
    RULE_TYPE_Acl_T            acl_entry;
    RULE_TYPE_AclMemento_T     bind_acl;
    UI32_T                     acl_index;

    LOG("ifindex = %lu, direction = %s, aclType = %s",
        ifindex,
        (direction == RULE_TYPE_INBOUND) ? "In" : "Out",
        RULE_MGR_ACL_TYPE_STR(acl_type));

    memset(&bind_acl, 0, sizeof(bind_acl));

    result = RULE_OM_GetPortBindedAcl(ifindex, direction, acl_type, &bind_acl);
    if(RULE_TYPE_OK != result ||
       TRUE == RULE_MGR_IS_ENTRY_ACTIVE(bind_acl.row_status))
    {
        LOG("Access group entry row status is not NotReady");
        return  RULE_TYPE_FAIL;
    }

    /* check if acl is active
     */
    if ((RULE_TYPE_OK != RULE_OM_GetAclByIndex(bind_acl.acl_index, &acl_entry)) ||
        (FALSE == RULE_MGR_IS_ENTRY_ACTIVE(acl_entry.row_status)) ||
        (RULE_TYPE_OK != RULE_MGR_LocalCheckInputAclType(acl_entry.acl_type, acl_type)))
    {
        LOG("Inactive ACL or wrong ACL type: row_status(%lu), acl_type(%d)",
            acl_entry.row_status, acl_entry.acl_type);
        return RULE_TYPE_FAIL;
    }

    acl_index = bind_acl.acl_index;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if (TRUE == acl_entry.flag_aggregate_done)
    {
        acl_index = acl_entry.aggregate_acl_index;
    }
    else
    {
        return RULE_TYPE_FAIL;
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    if (RULE_OM_IsBindDyanmicAclOnPort(ifindex, direction))
    {
        result = RULE_OM_SetPortEntryAclRowStatus(ifindex, direction, acl_type,
                                                  VAL_diffServAccessGroupStatus_active);
        if (result != RULE_TYPE_OK)
        {
            ASSERT(0);
            return result;
        }
        return RULE_TYPE_OK;
    }

    result = RULE_CTRL_SetAcl(ifindex,
                              direction,
                              acl_index,
                              TRUE,
                              bind_acl.time_range_index,
                              bind_acl.counter_enable);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    result = RULE_OM_SetAcl(ifindex,
                            direction,
                            bind_acl.acl_index,
                            bind_acl.time_range_index,
                            bind_acl.time_range_name,
                            bind_acl.counter_enable);
    ASSERT(RULE_TYPE_OK == result);

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalSetAccessGroupActive2NotReady
 *------------------------------------------------------------------------------
 * PURPOSE  : Set access group entry row status active to notReady
 * INPUT    : ifindex, direction, acl_type
 * OUTPUT   : None
 * RETURN   : error code (OK / FAIL)
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static UI32_T
RULE_MGR_LocalSetAccessGroupActive2NotReady (
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_TYPE_AclType_T acl_type)
{
    RULE_TYPE_RETURN_TYPE_T     result;
    RULE_TYPE_Acl_T             acl_entry;
    UI32_T                      phy_acl_index;
    RULE_TYPE_AclMemento_T      bind_acl;

    LOG("ifindex = %lu, direction = %s, aclType = %s",
        ifindex,
        (direction == RULE_TYPE_INBOUND) ? "In" : "Out",
        RULE_MGR_ACL_TYPE_STR(acl_type));

    if(RULE_TYPE_OK != RULE_OM_GetPortBindedAcl(ifindex, direction, acl_type, &bind_acl) ||
       TRUE != RULE_MGR_IS_ENTRY_ACTIVE(bind_acl.row_status))
    {
        LOG("Not bind yet");
        return RULE_TYPE_FAIL;
    }

    result = RULE_OM_GetAclByIndex(bind_acl.acl_index, &acl_entry);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    phy_acl_index = bind_acl.acl_index;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    if (FALSE == acl_entry.flag_hide)
    {
        phy_acl_index = acl_entry.aggregate_acl_index;
        ASSERT(0 != phy_acl_index);
    }
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

    if (RULE_OM_IsBindDyanmicAclOnPort(ifindex, direction))
    {
        RULE_MGR_DEBUG_MSG("Have bind dynamic ACL on this port(%lu): clear ACL memento acl_type(%d)\n",
                           ifindex,
                           acl_type);

        result = RULE_OM_SetPortEntryAclRowStatus(ifindex, direction, acl_type,
                                                  VAL_diffServAccessGroupStatus_notReady);
        if (result != RULE_TYPE_OK)
        {
            ASSERT(0);
            return result;

        }
        return RULE_TYPE_OK;
    }

    result = RULE_CTRL_SetAcl(ifindex,
                              direction,
                              phy_acl_index,
                              FALSE,
                              RULE_TYPE_UNDEF_TIME_RANGE,
                              RULE_TYPE_COUNTER_DISABLE);
    if (RULE_TYPE_OK != result)
    {
        return result;
    }

    result = RULE_OM_ResetAcl(ifindex, direction, bind_acl.acl_index);
    ASSERT(RULE_TYPE_OK == result);

    return RULE_TYPE_OK;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalSetAceNotReady
 *------------------------------------------------------------------------------
 * PURPOSE  : break relationship with acl/class-map and free hardware resource
 * INPUT    : ace_index
 * OUTPUT   : none
 * RETURN   : error code (OK / FAIL)
 * NOTE     : if there is no acl/class-map associated with it, return OK (do nothing)
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_MGR_LocalSetAceNotReady(UI32_T ace_index)
{
    UI32_T                  acl_index, class_map_index;
    BOOL_T                  ok_flag = TRUE;

    if (RULE_TYPE_OK == RULE_OM_GetAclIndexByAce(ace_index, &acl_index)) /* this ace is own by ACL */
    {
        RULE_TYPE_Ace_Entry_T   ace_entry;

        if (RULE_TYPE_OK != RULE_OM_GetAceByIndex(ace_index, &ace_entry))
            return RULE_TYPE_FAIL; /* should not go here */

// FIXME: fix here !!

        /* free resource */
        ok_flag &= (RULE_TYPE_OK == RULE_CTRL_SetAce(acl_index, &ace_entry, FALSE));

        /* remove ace from acl */
        ok_flag &= (RULE_TYPE_OK == RULE_OM_UnbindElementFromAcl(acl_index, ace_entry.ace_type, ace_index));
    }
    else if (RULE_TYPE_OK == RULE_OM_GetClassMapIndexByMf(ace_index, &class_map_index)) /* this ace is own by Class-MAP */
    {
        /* free resource */
        ok_flag &= (RULE_TYPE_OK == RULE_CTRL_RemoveClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, ace_index));

        /* remove ace from class-map */
        ok_flag &= (RULE_TYPE_OK == RULE_OM_RemoveClassMapElementByMfIndex(class_map_index, ace_index));
    }

    /* update om */
    ok_flag &= (RULE_TYPE_OK == RULE_OM_SetAceRowStatus(ace_index, FALSE));

    return (TRUE == ok_flag) ? RULE_TYPE_OK : RULE_TYPE_FAIL;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalPolicyMapRStatusCallBack
 *------------------------------------------------------------------------------
 * PURPOSE  : callback func for L_RSTATUS_Fsm
 * INPUT    : rec
 * OUTPUT   : None
 * RETURN   : TRUE - Active; FALSE - Not ready
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_MGR_LocalPolicyMapRStatusCallBack(void *rec)
{
    UI32_T                  *policy_map_index_p;
    RULE_TYPE_PolicyMap_T   policy_map;

    policy_map_index_p = (UI32_T*)rec;

    if (RULE_TYPE_OK != RULE_OM_GetPolicyMap(*policy_map_index_p, &policy_map))
    {
        /* if policy-map does not exist, implies create
         * om will give it a default name
         */
        return TRUE;
    }

    if ('\0' == policy_map.name[0]) /* do not allow null name */
        return FALSE;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalPolicyMapElementRStatusCallBack
 *------------------------------------------------------------------------------
 * PURPOSE  : callback func for L_RSTATUS_Fsm
 * INPUT    : rec
 * OUTPUT   : None
 * RETURN   : TRUE - Active; FALSE - Not ready
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_MGR_LocalPolicyMapElementRStatusCallBack(void *rec)
{
    UI32_T                      *policy_element_index_p;
    RULE_TYPE_PolicyElement_T   policy_element;
    RULE_TYPE_ClassMap_T        class_map;
    RULE_TYPE_Action_T          action_entry;

    policy_element_index_p = (UI32_T*)rec;

    if (RULE_TYPE_OK != RULE_OM_GetPolicyMapElement(*policy_element_index_p, &policy_element))
    {
        /* if policy-map element does not exist, implies create
           the default values are zero so can't active
         */
        return FALSE;
    }

    if ((RULE_TYPE_OK != RULE_OM_GetClassMap(policy_element.class_map_index, &class_map)) ||
        (RULE_TYPE_OK != RULE_OM_GetActionEntryByIndex(policy_element.action_index, &action_entry)))
    {
        return FALSE; /* an active entry MUST have class-map and action */
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalClassMapRStatusCallBack
 *------------------------------------------------------------------------------
 * PURPOSE  : callback func for L_RSTATUS_Fsm
 * INPUT    : rec
 * OUTPUT   : None
 * RETURN   : TRUE - Active; FALSE - Not ready
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_MGR_LocalClassMapRStatusCallBack(void *rec)
{
    UI32_T                      *class_map_index_p;
    RULE_TYPE_ClassMap_T        class_map;

    class_map_index_p = (UI32_T*)rec;

    if (RULE_TYPE_OK != RULE_OM_GetClassMap(*class_map_index_p, &class_map))
    {
        /* if class-map does not exist, implies create
         * om will give it a default name
         */
        return TRUE;
    }

    if ('\0' == class_map.class_map_name[0]) /* do not allow null name */
        return FALSE;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalActionRStatusCallBack
 *------------------------------------------------------------------------------
 * PURPOSE  : callback func for L_RSTATUS_Fsm
 * INPUT    : rec
 * OUTPUT   : None
 * RETURN   : TRUE - Active; FALSE - Not ready
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_MGR_LocalActionRStatusCallBack(void *rec)
{
    UI32_T                      *action_index_p;
    RULE_TYPE_Action_T          action_entry;

    action_index_p = (UI32_T*)rec;

    if (RULE_TYPE_OK != RULE_OM_GetActionEntryByIndex(*action_index_p, &action_entry))
    {
        /* if action does not exist, implies create
         * om will give it a default action
         */
        return TRUE;
    }
#if 0 /* coding here */
    /* need to check action entry check valid or not*/
    UI32_T index;
    UI32_T action_bitmap;

    UI32_T pkt_new_pri;             /*RULE_TYPE_ACTION_PKT_NEW_PRI*/
    UI32_T pkt_new_tos;             /*RULE_TYPE_ACTION_PKT_NEW_TOS*/
    UI32_T pkt_new_dscp;            /*RULE_TYPE_ACTION_PKT_NEW_DSCP*/
    UI32_T red_pkt_new_dscp;        /*RULE_TYPE_ACTION_RED_PKT_NEW_DSCP*/
    UI32_T yellow_pkt_new_dscp;     /*RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP*/
    UI32_T drop_precedence;         /*RULE_TYPE_ACTION_DROP_PRECEDENCE*/
    UI32_T red_drop_precedence;     /*RULE_TYPE_ACTION_RED_DROP_PRECEDENCE*/
    UI32_T yellow_drop_precedence;  /*RULE_TYPE_ACTION_YELLOW_DROP_PRECEDENCE*/
#endif

    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalMeterRStatusCallBack
 *------------------------------------------------------------------------------
 * PURPOSE  : callback func for L_RSTATUS_Fsm
 * INPUT    : rec
 * OUTPUT   : None
 * RETURN   : TRUE - Active; FALSE - Not ready
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_MGR_LocalMeterRStatusCallBack(void *rec)
{
    UI32_T                      *meter_index_p;
    RULE_TYPE_TBParamEntry_T    meter_entry;

    meter_index_p = (UI32_T*)rec;

    if (RULE_TYPE_OK != RULE_OM_GetMeterEntryByIndex(*meter_index_p, &meter_entry))
    {
        /* if meter does not exist, implies create
         * om will give it a default meter
         */
        return TRUE;
    }

#if 0 /* coding here check valid or not*/
    /* need to check meter entry */
    UI16_T  ref_count;              /* how many entries are using it */
    UI32_T  meter_model;            /* meter model */
    UI32_T  rate;                   /* average rate in kbps */
    UI32_T  burst_size;             /* maxi. # of bytes in a single Tx burst */
    UI32_T  interval;               /* microseconds, time interval used with the token bucket */
#endif

    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalPolicyMapPortRStatusCallBack
 *------------------------------------------------------------------------------
 * PURPOSE  : callback func for L_RSTATUS_Fsm
 * INPUT    : rec
 * OUTPUT   : None
 * RETURN   : TRUE - Active; FALSE - Not ready
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_MGR_LocalPolicyMapPortRStatusCallBack(void *rec)
{
    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalAccessGroupRStatusCallBack
 *------------------------------------------------------------------------------
 * PURPOSE  : callback func for L_RSTATUS_Fsm
 * INPUT    : rec
 * OUTPUT   : None
 * RETURN   : TRUE - Active; FALSE - Not ready
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_MGR_LocalAccessGroupRStatusCallBack(void *rec)
{
    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalAclRStatusCallBack
 *------------------------------------------------------------------------------
 * PURPOSE  : callback func for L_RSTATUS_Fsm
 * INPUT    : rec
 * OUTPUT   : None
 * RETURN   : TRUE - Active; FALSE - Not ready
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_MGR_LocalAclRStatusCallBack(void *rec)
{
    UI32_T                      *acl_index_p;
    RULE_TYPE_Acl_T             acl_entry;

    acl_index_p = (UI32_T*)rec;

    if (RULE_TYPE_OK != RULE_OM_GetAclByIndex(*acl_index_p, &acl_entry))
    {
        /* if acl does not exist, implies create
         * om will give it default vlaues (name, type ...)
         */
        return TRUE;
    }

    if ('\0' == acl_entry.acl_name[0]) /* do not allow null name */
        return FALSE;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalAceRStatusCallBack
 *------------------------------------------------------------------------------
 * PURPOSE  : callback func for L_RSTATUS_Fsm
 * INPUT    : rec
 * OUTPUT   : None
 * RETURN   : TRUE - Active; FALSE - Not ready
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T RULE_MGR_LocalAceRStatusCallBack(void *rec)
{
    return TRUE;
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_LocalCheckInputAclType
 *------------------------------------------------------------------------------
 * PURPOSE  : Check the input acl type is matching with type of entry
 * INPUT    : acl_entry_acl_type, input_acl_type
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static UI32_T RULE_MGR_LocalCheckInputAclType(UI32_T acl_entry_acl_type, UI32_T input_acl_type)
{
	switch(acl_entry_acl_type)
	{
    	case RULE_TYPE_IP_STD_ACL:
    	case RULE_TYPE_IP_EXT_ACL:
        	if (RULE_TYPE_IP_ACL != input_acl_type)
            	return RULE_TYPE_FAIL; /* this acl can't be bound as this type */
        	break;
    	case RULE_TYPE_MAC_ACL:
        	if (RULE_TYPE_MAC_ACL != input_acl_type)
            	return RULE_TYPE_FAIL; /* this acl can't be bound as this type */
        	break;
    	case RULE_TYPE_IPV6_STD_ACL:
    	case RULE_TYPE_IPV6_EXT_ACL:
        	if (RULE_TYPE_IPV6_ACL != input_acl_type)
            	return RULE_TYPE_FAIL; /* this acl can't be bound as this type */
        	break;
    	case RULE_TYPE_IP_ACL:
    	case RULE_TYPE_MF:
    	case RULE_TYPE_IPV6_ACL:
    	default:
        	return RULE_TYPE_FAIL; /* should not go here */
	}
	return RULE_TYPE_OK;
}

/*  IP SRC GRD
 */
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
BOOL_T RULE_MGR_SetSrcIpFilterByUport(UI32_T unit, UI32_T port, UI32_T sip, UI32_T vid, BOOL_T is_enable)
{
    RULE_CTRL_UnitPortVidSip_T param;
    param.unit  = unit;
    param.port  = port;
    param.vid   = vid;
    param.sip   = sip;
    return RULE_CTRL_Service(is_enable, RULE_CTRL_IP_SOURCE_GUARD_PERMIT_VID_SIP, &param);
}

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
    UI32_T sip,  UI32_T vid,        BOOL_T is_enable)
{
    RULE_CTRL_UnitPortSaVidSip_T param;
    param.unit  = unit;
    param.port  = port;
    memcpy(param.sa, src_mac, sizeof(param.sa));
    param.vid   = vid;
    param.sip   = sip;
    return RULE_CTRL_Service(is_enable, RULE_CTRL_IP_SOURCE_GUARD_PERMIT_SA_VID_SIP, &param);
}

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
BOOL_T RULE_MGR_SetIpPktActionByUport(UI32_T unit, UI32_T port, BOOL_T is_enable)
{
    RULE_CTRL_UnitPort_T unit_port;
    unit_port.unit = unit;
    unit_port.port = port;
    return RULE_CTRL_Service(is_enable, RULE_CTRL_IP_SOURCE_GUARD_DENY_IP, &unit_port);
}
#endif /* #if (SYS_CPNT_IP_SOURCE_GUARD == TRUE) */

#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_SetSrcIpv6FilterByUport
 *------------------------------------------------------------------------------
 * PURPOSE : for IPv6 Source Guard
 *            config chip to permit switching traffic which contain
 *            specified Source IPv6 and VID
 *   INPUT : unit - unit
 *           port - port
 *            sip - source ipv6
 *            vid - vid
 *      is_enable - TRUE   set rule to chip
 *                  FALSE  delete rule from chip
 *  OUTPUT : None
 *  RETURN : TRUE/FALSE
 *    NOTE : 1. if chip is already configured, update/delete config from chip
 *              if chip does not be configured, do creation/nothing on chip
 *------------------------------------------------------------------------------
 */
BOOL_T RULE_MGR_SetSrcIpv6FilterByUport(
    UI32_T  unit,
    UI32_T  port,
    UI8_T   sip6[SYS_TYPE_IPV6_ADDR_LEN],
    UI32_T  vid,
    BOOL_T  is_enable)
{
    RULE_CTRL_UnitPortVidSip6_T param;
    param.unit  = unit;
    param.port  = port;
    param.vid   = vid;
    memcpy(param.sip6, sip6, sizeof(param.sip6));
    return RULE_CTRL_Service(is_enable, RULE_CTRL_IPV6_SOURCE_GUARD_PERMIT_VID_SIP6, &param);
}

/*------------------------------------------------------------------------------
 * FUNCTION NAME - RULE_MGR_SetIpv6PktActionByUport
 *------------------------------------------------------------------------------
 * PURPOSE : for IPv6 Source Guard
 *            config chip to drop IPv6 packets
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
BOOL_T RULE_MGR_SetIpv6PktActionByUport(
    UI32_T  unit,
    UI32_T  port,
    BOOL_T  is_enable)
{
    RULE_CTRL_UnitPort_T unit_port;
    unit_port.unit = unit;
    unit_port.port = port;
    return RULE_CTRL_Service(is_enable, RULE_CTRL_IPV6_SOURCE_GUARD_DENY_IP6, &unit_port);
}

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
    BOOL_T  is_enable)
{
    return RULE_CTRL_IPV6SG_PermitIp6LinkLocal(is_enable);
}

#endif /* #if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE) */


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
UI32_T RULE_MGR_AddTrapArp2Cpu(BOOL_T enable_flag)
{
    UI32_T  ret;
    RULE_TYPE_CpuRuleInfo_T rule_info;

    RULE_MGR_DEBUG_MSG("\r\n[RULE_MGR_AddTrapArp2Cpu]");

    memset(&rule_info, 0, sizeof(rule_info));
    rule_info.arp.all_arp_to_cpu = enable_flag;
    ret = RULE_MGR_TrapPacket2Cpu(enable_flag, RULE_TYPE_PacketType_ARP_REQUEST, &rule_info);
    ret += RULE_MGR_TrapPacket2Cpu(enable_flag, RULE_TYPE_PacketType_ARP_REPLY, &rule_info);
    if (ret == 2)
    {
        return RULE_TYPE_OK;
    }

    RULE_MGR_DEBUG_MSG(" failed.\r\n");
    return RULE_TYPE_FAIL;
}

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
UI32_T RULE_MGR_ArpACLCheckForDai(const char *arp_acl_name, RULE_TYPE_ArpPktFormat_T *arp_pkt_p, BOOL_T *b_acl_drop)
{
    UI32_T  ret=0;
    UI32_T  debug_flag = RULE_OM_GetDebugFlag();

    ret = RULE_OM_ArpACLCheckForDai(arp_acl_name, arp_pkt_p, b_acl_drop);

    if (RULE_MGR_IS_DEBUG_ERROR_ON(debug_flag))
    {
        if (ret == RULE_TYPE_OK)
        {
            printf("\r\n RULE_MGR_ArpACLCheckForDai : ret=OK, drop=%s", (TRUE==*b_acl_drop) ? "TRUE":"FALSE" );
        }
        else
        {
            printf("\r\n RULE_MGR_ArpACLCheckForDai : ret=FAILED(not match!!)");

        }
    }
    return ret;
}

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
    UI32_T rate)
{
    return RULE_CTRL_DAI_TrapArpPacketToCpuWithRate(enable, interface_p, rate);
}

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
    UI32_T *total_cnt_p)
{
    return RULE_CTRL_DAI_GetArpPacketCounter(interface_p, trunk_member_p, reset, total_cnt_p);
}
#endif /* SYS_CPNT_DAI_RATE_LIMIT_BY_RULE */
#endif /*end of #if (SYS_CPNT_DAI == TRUE)*/


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
    const RULE_TYPE_PBR_ACTION_T *action_p)
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_Acl_T acl_entry;

    if (vlan < 1 || SYS_ADPT_MAX_VLAN_ID < vlan)
    {
        return RULE_TYPE_INVALID_PARAMETER;
    }

    if (is_add == TRUE)
    {
        if (action_p == NULL)
        {
            return RULE_TYPE_INVALID_PARAMETER;
        }

        result = RULE_OM_GetAclByIndex(acl_id, &acl_entry);
        if (result != RULE_TYPE_OK ||
            RULE_MGR_IS_IP_ACL(acl_entry.acl_type) != TRUE)
        {
            return RULE_TYPE_INVALID_PARAMETER;
        }
    }

    return RULE_CTRL_SetPbrAcl(is_add, vlan, seq_num, acl_id, action_p);
}
#endif /* SYS_CPNT_PBR == TRUE */
