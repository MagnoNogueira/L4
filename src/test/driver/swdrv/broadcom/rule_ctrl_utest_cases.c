/*
 *  rule_ctrl_utest_cases.c
 *  rule_ctrl
 *
 *  Created by yehjunying on 2011/4/29.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "utest.h"

/*------------------------------------------------------------------------------
 * Help Functions
 *------------------------------------------------------------------------------
 */

RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_UT_Proc_Get_Next_Ifindex(
    UI32_T *ifindex_p)
{
    if (0 == *ifindex_p)
    {
        *ifindex_p = 1;
        return RULE_TYPE_OK;
    }

    if (*ifindex_p < SYS_ADPT_TOTAL_NBR_OF_LPORT)
    {
        (*ifindex_p) ++;
        return RULE_TYPE_OK;
    }

    if (SYS_ADPT_TOTAL_NBR_OF_LPORT == *ifindex_p)
    {
        *ifindex_p = SYS_ADPT_CPU_1_IF_INDEX_NUMBER;
        return RULE_TYPE_OK;
    }

    return RULE_TYPE_FAIL;
}

RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_UT_Proc_Ifindex_To_Interface(
    UI32_T ifindex,
    RULE_TYPE_INTERFACE_INFO_T *interface_p)
{
    return RULE_CTRL_Ifindex_To_Interface(ifindex, interface_p);
}

RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_UT_Proc_Interface_To_Ifindex(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    UI32_T *ifindex_p)
{
    return RULE_CTRL_Interface_To_Ifindex(interface_p, ifindex_p);
}

void
RULE_CTRL_UT_Proc_Set_Billing_Counter(
    UI32_T device_id,
    UI32_T dev_rule_id,
    UI32_T greenCntr,
    UI32_T yellowCntr,
    UI32_T redCntr)
{
    int rc;

    int counter_num = 1;
    uint64 val64 = greenCntr + yellowCntr + redCntr;

#if (SDK_VERSION >= 6)
    int stat_id;

    rc = bcm_field_entry_stat_get(device_id, dev_rule_id, &stat_id);
    assert(BCM_E_NONE == rc);

    rc = bcm_field_stat_set(device_id, stat_id, bcmFieldStatPackets, val64);
    assert(BCM_E_NONE == rc);
#else
    rc = bcm_field_counter_set(device_id, dev_rule_id, counter_num, val64);
    assert(BCM_E_NONE == rc);
#endif /* SDK_VERSION */

    {
        UI32_T val32;
        BOOL_T b_result;

        b_result = DEVRM_GetCounter(device_id, dev_rule_id, &val32);
        assert(TRUE == b_result);
        assert(((uint64)val32 & 0x00000000ffffffff) == val64);
    }
}

#pragma mark Generic Help Functions

/*------------------------------------------------------------------------------
 * Generic Generic Help Functions
 *------------------------------------------------------------------------------
 */
BOOL_T
RULE_CTRL_UT_Porc_ReturnFalse()
{
    return FALSE;
}

void
RULE_CTRL_UT_Proc_Validate_Rule();

void
RULE_CTRL_UT_Proc_Create_ACL(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    UI32_T *acl_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_OM_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name, acl_index);
    assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    result = RULE_CTRL_ReserveResourceOnCreateAcl(*acl_index);
    assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT
}

void
RULE_CTRL_UT_Proc_Delete_ACL(
    const char *acl_name)
{
    UI32_T acl_index;
    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_DelAcl(acl_index);
    assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    result = RULE_CTRL_ReleaseResourceOnDeleteAcl();
    assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT
}

typedef enum
{
    RULE_CTRL_UT_COMPRESS_DISABLE = 0,  // create ACL which can NOT be compressed
    RULE_CTRL_UT_COMPRESS_ENABLE,       // create ACE which can be compressed
    RULE_CTRL_UT_COMPRESS_DEFAULT = RULE_CTRL_UT_COMPRESS_ENABLE,    
} RULE_CTRL_UT_COMPRESS_OPTION_T;

void
RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    UI32_T number_of_ace,
    RULE_CTRL_UT_COMPRESS_OPTION_T compress,
    UI32_T *acl_index)
{
    UI32_T i;
    RULE_TYPE_Ace_Entry_T ace_entry;

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, acl_index);

    if (0 < number_of_ace)
    {
        assert(number_of_ace <= (255*255));

        for (i = 0; i < (number_of_ace - 1); ++ i)
        {
            UI8_T  a3, a4;

            UI8_T  ipv4_addr[SYS_TYPE_IPV4_ADDR_LEN];
            UI8_T  ipv4_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

            UI8_T  mac_addr[SYS_TYPE_MAC_ADDR_LEN];
            UI8_T  mac_addr_mask[SYS_TYPE_MAC_ADDR_LEN];

            char   ipv6_addr[L_INET_MAX_IPADDR_STR_LEN + 1];

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(result == RULE_TYPE_OK);

            if (RULE_CTRL_UT_COMPRESS_DISABLE == compress)
            {
                ace_entry.access = (i & 1) ?
                                   RULE_TYPE_ACE_PERMIT : RULE_TYPE_ACE_DENY;
            }

            a3 = (i + 1) / 255;
            a4 = (i + 1) % 255;

            SET_MAC(mac_addr, 0, 0, 0, 0, a3, a4);
            SET_MAC(mac_addr_mask, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

            SET_IPV4(ipv4_addr, 192, 168, a3, a4);
            SET_IPV4(ipv4_addr_mask, 255, 255, 255, 255);

            snprintf(ipv6_addr, sizeof(ipv6_addr), "%02x%02x::", (UI8_T)a3, (UI8_T)a4);
            ipv6_addr[ sizeof(ipv6_addr)-1 ] = '\0';

            switch (acl_type)
            {
                case RULE_TYPE_MAC_ACL:
                    RULE_OM_UT_Proc_SetAceDstMacAddress(&ace_entry, mac_addr, mac_addr_mask);
                    RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, mac_addr, mac_addr_mask);
                    break;

                case RULE_TYPE_IP_STD_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, ipv4_addr, ipv4_addr_mask);
                    break;

                case RULE_TYPE_IP_EXT_ACL:
                    RULE_OM_UT_Proc_SetAceDstIpAddress(&ace_entry, ipv4_addr, ipv4_addr_mask);
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, ipv4_addr, ipv4_addr_mask);
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, ipv6_addr);
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, ipv6_addr);
#endif

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceDstIpv6Address(&ace_entry, ipv6_addr);
#endif

//#if (SYS_CPNT_ACL_IPV6_EXT_FLOW_LABEL == TRUE)
//                    RULE_OM_UT_Proc_SetAceFlowLabel(&ace_entry, 1234);
//#endif
                    break;

                default:
                    assert(0);
                    break;
            }

            result = RULE_OM_AddAce2Acl(*acl_index, acl_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }

        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.access = RULE_TYPE_ACE_DENY;

        result = RULE_OM_AddAce2Acl(*acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);
    }
}

void
RULE_CTRL_UT_Proc_Add_Ace_To_Acl(
    UI32_T acl_index,
    RULE_TYPE_AclType_T acl_type,
    UI32_T number_of_ace,
    UI32_T pattern_offset,
    RULE_CTRL_UT_COMPRESS_OPTION_T compress)
{
    UI32_T i;
    RULE_TYPE_Ace_Entry_T ace_entry;
    RULE_TYPE_RETURN_TYPE_T result;

    if (0 < number_of_ace)
    {
        assert(number_of_ace <= (SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR));
        assert(number_of_ace + pattern_offset <= (255*255));

        for (i = 0; i < (number_of_ace); ++ i)
        {
            UI8_T  a3, a4;

            UI8_T  ipv4_addr[SYS_TYPE_IPV4_ADDR_LEN];
            UI8_T  ipv4_addr_mask[SYS_TYPE_IPV4_ADDR_LEN];

            UI8_T  mac_addr[SYS_TYPE_MAC_ADDR_LEN];
            UI8_T  mac_addr_mask[SYS_TYPE_MAC_ADDR_LEN];

            char   ipv6_addr[L_INET_MAX_IPADDR_STR_LEN + 1];

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(result == RULE_TYPE_OK);

            if (RULE_CTRL_UT_COMPRESS_DISABLE == compress)
            {
                ace_entry.access = (i & 1) ?
                RULE_TYPE_ACE_PERMIT : RULE_TYPE_ACE_DENY;
            }

            a3 = (i + 1 + pattern_offset) / 255;
            a4 = (i + 1 + pattern_offset) % 255;

            SET_MAC(mac_addr, 0, 0, a3, a4, 0, 0);
            SET_MAC(mac_addr_mask, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

            SET_IPV4(ipv4_addr, a3, a4, 1, 1);
            SET_IPV4(ipv4_addr_mask, 255, 255, 255, 255);

            snprintf(ipv6_addr, sizeof(ipv6_addr), "0:%02x%02x::", (UI8_T)a3, (UI8_T)a4);
            ipv6_addr[ sizeof(ipv6_addr)-1 ] = '\0';

            switch (acl_type)
            {
                case RULE_TYPE_MAC_ACL:
                    RULE_OM_UT_Proc_SetAceDstMacAddress(&ace_entry, mac_addr, mac_addr_mask);
                    RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, mac_addr, mac_addr_mask);
                    break;

                case RULE_TYPE_IP_STD_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, ipv4_addr, ipv4_addr_mask);
                    break;

                case RULE_TYPE_IP_EXT_ACL:
                    RULE_OM_UT_Proc_SetAceDstIpAddress(&ace_entry, ipv4_addr, ipv4_addr_mask);
                    RULE_OM_UT_Proc_SetAceSrcIpAddress(&ace_entry, ipv4_addr, ipv4_addr_mask);
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, ipv6_addr);
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceSrcIpv6Address(&ace_entry, ipv6_addr);
#endif

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                    RULE_OM_UT_Proc_SetAceDstIpv6Address(&ace_entry, ipv6_addr);
#endif
                    break;
                    
                default:
                    assert(0);
                    break;
            }
            
            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
            
            result = RULE_CTRL_SetAce(acl_index, &ace_entry, TRUE);
            assert(RULE_TYPE_OK == result);
        }
    }
}

void
RULE_CTRL_UT_Proc_Add_ACL_Into_Class_Map(
    UI32_T class_map_index,
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    UI32_T number_of_ace,
    RULE_CTRL_UT_COMPRESS_OPTION_T compress,
    UI32_T *acl_index_p)
{
    RULE_TYPE_RETURN_TYPE_T         result;

    UI32_T                          acl_index;

    RULE_TYPE_UI_ClassMapElement_T  class_element;
    UI32_T                          class_index;

    RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acl_type, number_of_ace, compress, &acl_index);

    if (NULL != acl_index_p)
    {
        *acl_index_p = acl_index;
    }

    memset(&class_element, 0, sizeof(class_element));

    class_element.class_type = RULE_TYPE_CLASS_ACL;
    strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
    class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

    result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
    assert(RULE_TYPE_OK == result);
    assert(acl_index == class_index);
}

void
RULE_CTRL_UT_Proc_Add_MF_Into_Class_Map(
    UI32_T class_map_index,
    RULE_TYPE_MfType_T mf_type,
    UI32_T mf_value,
    UI32_T *mf_index_p)
{
    RULE_TYPE_RETURN_TYPE_T         result;

    RULE_TYPE_UI_ClassMapElement_T  class_element;
    UI32_T                          class_index;

    memset(&class_element, 0, sizeof(class_element));

    class_element.class_type = RULE_TYPE_CLASS_MF;
    class_element.element.mf_entry.mf_type  = mf_type;
    class_element.element.mf_entry.mf_value = mf_value;

    result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
    assert(RULE_TYPE_OK == result);

    if (NULL != mf_index_p)
    {
        *mf_index_p = class_index;
    }
}

static RULE_TYPE_CLASS_INSTANCE_PTR_T
RULE_CTRL_UT_Proc_Get_QoS_Class_Instance(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_map_index,
    UI32_T class_map_index,
    RULE_TYPE_ClassType_T class_type,
    UI16_T class_id)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_INTERFACE_INFO_T interface;

    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

    RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;

    RULE_TYPE_CLASS_INSTANCE_PTR_T class_map_inst_p;

    RULE_CTRL_INSTANCE_TYPE_ID_T param;

    assert(RULE_TYPE_CLASS_ACL == class_type ||
           RULE_TYPE_CLASS_MF == class_type);

    result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
    assert(RULE_TYPE_OK == result);
    interface.direction = direction;

    if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
    ASSERT(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_POLICY_MAP;
    param.id = policy_map_index;

    policy_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) if_class_obj.inst,
                                                                                       RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                       &param);
    ASSERT(NULL != policy_map_inst_p);

    memset(&param, 0, sizeof(param));

    param.type = RULE_TYPE_INST_CLASS_MAP;
    param.id = class_map_index;

    class_map_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) policy_map_inst_p,
                                                                                      RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                                      &param);

    ASSERT(NULL != class_map_inst_p);

    param.type = (class_type == RULE_TYPE_CLASS_ACL) ? RULE_TYPE_INST_ACL : RULE_TYPE_INST_MF_ACE;
    param.id = class_id;

    return (RULE_TYPE_CLASS_INSTANCE_PTR_T) RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) class_map_inst_p,
                                                                           RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
                                                                           &param);
}

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
void RULE_CTRL_UT_Proc_ChangeGroupQualifySet(
    RULE_TYPE_FunctionType_T fun_type,
    RULE_TYPE_FunctionType_T remap_fun_type)
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T unit = 1;
    UI32_T device_id;

    FP_CONFIG_FunctionInfo_T    *fun_p;
    FP_CONFIG_GroupInfo_T       *grp_p;

    FP_CONFIG_FunctionInfo_T    *remap_fun_p;
    FP_CONFIG_GroupInfo_T       *remap_grp_p;

    fun_p = FP_CONFIG_get_function_info_by_type(fun_type);
    grp_p = FP_CONFIG_get_group_info_by_id(fun_p->group_id);

    remap_fun_p = FP_CONFIG_get_function_info_by_type(remap_fun_type);
    remap_grp_p = FP_CONFIG_get_group_info_by_id(remap_fun_p->group_id);

    result = RULE_CTRL_Priv_ChangeGroupQualifySet(fun_type, remap_fun_type);
    assert(RULE_TYPE_OK == result);

    for (device_id = 0; device_id < shmem_data_p->dev_info[unit-1].num_of_chips; ++device_id)
    {
        bcm_field_qset_t qset = {0};

        RULE_CTRL_ResourceInfo_T *res_p = RULE_CTRL_GetResourceInfo(unit,
                                                                    device_id,
                                                                    remap_grp_p->group_id);
        assert(res_p->total_rule == res_p->free_rule);

        bcm_field_group_get(device_id, remap_grp_p->group_id, &qset);

        assert(0 == memcmp(&qset, grp_p->w, sizeof(qset)));
    }
}
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

void
RULE_CTRL_UT_Proc_Create_Policy_Class_Map_With_Match_ACL(
    const char *policy_map_name,
    const char *class_map_name,
    const char *acl_name)
{
    UI32_T policy_map_index;
    UI32_T class_map_index;
    UI32_T acl_index;

    RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ANY;
    RULE_TYPE_AclType_T   acl_type = RULE_TYPE_IP_EXT_ACL;

    RULE_TYPE_Ace_Entry_T ace_entry;

    RULE_TYPE_UI_ClassMapElement_T  class_element;
    UI32_T                          class_index;

    RULE_TYPE_UI_Action_T           ui_action;
    RULE_TYPE_TBParamEntry_T        meter;

    UI32_T ui32_result;

    ui32_result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == ui32_result);

    ui32_result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
    assert(RULE_TYPE_OK == ui32_result);

    RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);


    ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
    RULE_OM_InitAce(&ace_entry);

    ace_entry.u.ip.aceSourceIpAddr = 0x01020304;
    ace_entry.u.ip.aceSourceIpAddrBitmask = 0xffffffff;

    ui32_result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
    assert(RULE_TYPE_OK == ui32_result);

    class_element.class_type = RULE_TYPE_CLASS_ACL;
    strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
    class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

    ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
    assert(RULE_TYPE_OK == ui32_result);

    memset(&ui_action, 0, sizeof(ui_action));

    ui_action.in_action_type        = IN_ACTION_INVALID;
    ui_action.out_action_type       = OUT_ACTION_INVALID;
    ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
    ui_action.class_action_type     = CLASS_ACTION_INVALID;
    ui_action.exceed_action_type = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
    ui_action.exceed_action_value = 32;
    ui_action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
    ui_action.violate_action_value = 63;

    meter.meter_model = RULE_TYPE_METER_MODE_SRTCM_COLOR_AWARE;
    meter.rate = 84;
    meter.burst_size = 4000;
    meter.peak_burst_size = 4000;

    ui32_result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
    assert(RULE_TYPE_OK == ui32_result);

    ui32_result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
    assert(RULE_TYPE_OK == ui32_result);

    ui32_result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
    assert(RULE_TYPE_OK == ui32_result);
}

void
RULE_CTRL_UT_Proc_Create_Policy_Class_Map_With_Match_ACL_2(
    const char *policy_map_name,
    RULE_TYPE_AclType_T acl_type)
{
    enum
    {
        MAX_ACE_IN_ACL = 5,
    };

    RULE_TYPE_MatchType_T           match_type = RULE_TYPE_CLASS_MAP_MATCH_ANY;

    RULE_TYPE_RETURN_TYPE_T         ui32_result;

    UI32_T                          policy_map_index;

    RULE_TYPE_UI_ClassMapElement_T  class_element;
    UI32_T                          class_index;

    RULE_TYPE_UI_Action_T           ui_action;
    RULE_TYPE_TBParamEntry_T        meter;

    ui32_result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == ui32_result);

    {
        const char                      *class_map_name = "c1";
        UI32_T                          class_map_index;

        ui32_result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == ui32_result);

        //
        // Match ACL
        //
        {
            const char                      *acl_name = "a1";
            UI32_T                          acl_index;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acl_type, MAX_ACE_IN_ACL, RULE_CTRL_UT_COMPRESS_DEFAULT, &acl_index);

            class_element.class_type = RULE_TYPE_CLASS_ACL;
            strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
            class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

            ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
            assert(RULE_TYPE_OK == ui32_result);
        }

        //
        // Match ACL
        //
        if (RULE_TYPE_CLASS_MAP_MATCH_ANY == match_type)
        {
            const char                      *acl_name = "a2";
            UI32_T                          acl_index;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name,
                                                     acl_type,
                                                     MAX_ACE_IN_ACL,
                                                     RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                     &acl_index);

            class_element.class_type = RULE_TYPE_CLASS_ACL;
            strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
            class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

            ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
            assert(RULE_TYPE_OK == ui32_result);
        }
    }

    {
        const char                      *class_map_name = "c2";
        UI32_T                          class_map_index;

        ui32_result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == ui32_result);

        //
        // Match ACL
        //
        {
            const char                      *acl_name = "a3";
            UI32_T                          acl_index;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name,
                                                     acl_type,
                                                     MAX_ACE_IN_ACL,
                                                     RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                     &acl_index);

            class_element.class_type = RULE_TYPE_CLASS_ACL;
            strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
            class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

            ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
            assert(RULE_TYPE_OK == ui32_result);
        }

        //
        // Match ACL
        //
        if (RULE_TYPE_CLASS_MAP_MATCH_ANY == match_type)
        {
            const char                      *acl_name = "a4";
            UI32_T                          acl_index;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acl_type, MAX_ACE_IN_ACL, RULE_CTRL_UT_COMPRESS_DEFAULT, &acl_index);

            class_element.class_type = RULE_TYPE_CLASS_ACL;
            strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
            class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

            ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
            assert(RULE_TYPE_OK == ui32_result);
        }

        //
        // Action, meter configuration
        //

        ui_action.in_action_type        = IN_ACTION_INVALID;
        ui_action.out_action_type       = OUT_ACTION_INVALID;
        ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
        ui_action.class_action_type     = CLASS_ACTION_INVALID;
        ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
        ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

        meter.meter_model = RULE_TYPE_METER_MODE_FLOW;
        meter.rate = 84;
        meter.burst_size = 4000;
        meter.peak_burst_size = 4000;

        ui32_result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
        assert(RULE_TYPE_OK == ui32_result);
        
        ui32_result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == ui32_result);
        
        ui32_result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
        assert(RULE_TYPE_OK == ui32_result);
    }
}

void
RULE_CTRL_UT_Proc_Create_Policy_Match_Any_Class_Map_With_Match_ACL_MF(
    const char *policy_map_name)
{
    enum
    {
        MAX_ACE_IN_ACL = 5,
    };

    RULE_TYPE_MatchType_T           match_type = RULE_TYPE_CLASS_MAP_MATCH_ANY;

    RULE_TYPE_RETURN_TYPE_T         ui32_result;

    UI32_T                          policy_map_index;

    RULE_TYPE_UI_ClassMapElement_T  class_element;
    UI32_T                          class_index;

    RULE_TYPE_UI_Action_T           ui_action;
    RULE_TYPE_TBParamEntry_T        meter;

    ui32_result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == ui32_result);

    {
        const char                      *class_map_name = "c1";
        UI32_T                          class_map_index;

        ui32_result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == ui32_result);

        //
        // Match ACL
        //
        {
            const char                      *acl_name = "a1";
            UI32_T                          acl_index;

            RULE_TYPE_AclType_T             acl_type = RULE_TYPE_IP_EXT_ACL;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acl_type, MAX_ACE_IN_ACL, RULE_CTRL_UT_COMPRESS_DEFAULT, &acl_index);

            class_element.class_type = RULE_TYPE_CLASS_ACL;
            strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
            class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

            ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
            assert(RULE_TYPE_OK == ui32_result);
        }

        //
        // Match MF
        //

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
        class_element.element.mf_entry.mf_value = 42;

        ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == ui32_result);

        //
        // Match ACL
        //
        if (RULE_TYPE_CLASS_MAP_MATCH_ANY == match_type)
        {
            const char                      *acl_name = "a2";
            UI32_T                          acl_index;

            RULE_TYPE_AclType_T             acl_type = RULE_TYPE_IP_EXT_ACL;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name,
                                                     acl_type,
                                                     MAX_ACE_IN_ACL,
                                                     RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                     &acl_index);

            class_element.class_type = RULE_TYPE_CLASS_ACL;
            strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
            class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

            ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
            assert(RULE_TYPE_OK == ui32_result);
        }

        //
        // Match MF
        //

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
        class_element.element.mf_entry.mf_value = 43;

        ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == ui32_result);

        //
        // Action, meter configuration
        //

        ui_action.in_action_type        = IN_ACTION_INVALID;
        ui_action.out_action_type       = OUT_ACTION_INVALID;
        ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
        ui_action.class_action_type     = CLASS_ACTION_INVALID;
        ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
        ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

        meter.meter_model = RULE_TYPE_METER_MODE_FLOW;
        meter.rate = 84;
        meter.burst_size = 4000;
        meter.peak_burst_size = 4000;

        ui32_result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
        assert(RULE_TYPE_OK == ui32_result);
    }

    {
        const char                      *class_map_name = "c2";
        UI32_T                          class_map_index;

        ui32_result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == ui32_result);

        //
        // Match ACL
        //
        {
            const char                      *acl_name = "a3";
            UI32_T                          acl_index;

            RULE_TYPE_AclType_T             acl_type = RULE_TYPE_IP_EXT_ACL;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name,
                                                     acl_type,
                                                     MAX_ACE_IN_ACL,
                                                     RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                     &acl_index);

            class_element.class_type = RULE_TYPE_CLASS_ACL;
            strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
            class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

            ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
            assert(RULE_TYPE_OK == ui32_result);
        }

        //
        // Match MF
        //

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
        class_element.element.mf_entry.mf_value = 44;

        ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == ui32_result);

        //
        // Match ACL
        //
        if (RULE_TYPE_CLASS_MAP_MATCH_ANY == match_type)
        {
            const char                      *acl_name = "a4";
            UI32_T                          acl_index;

            RULE_TYPE_AclType_T             acl_type = RULE_TYPE_IP_EXT_ACL;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acl_type, MAX_ACE_IN_ACL, RULE_CTRL_UT_COMPRESS_DEFAULT, &acl_index);

            class_element.class_type = RULE_TYPE_CLASS_ACL;
            strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
            class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

            ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
            assert(RULE_TYPE_OK == ui32_result);
        }

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
        class_element.element.mf_entry.mf_value = 45;

        ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == ui32_result);

        //
        // Action, meter configuration
        //

        ui_action.in_action_type        = IN_ACTION_INVALID;
        ui_action.out_action_type       = OUT_ACTION_INVALID;
        ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
        ui_action.class_action_type     = CLASS_ACTION_INVALID;
        ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
        ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

        meter.meter_model = RULE_TYPE_METER_MODE_FLOW;
        meter.rate = 84;
        meter.burst_size = 4000;
        meter.peak_burst_size = 4000;

        ui32_result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
        assert(RULE_TYPE_OK == ui32_result);
    }
}

void
RULE_CTRL_UT_Proc_Create_Policy_Match_All_Class_Map_With_Match_ACL_MF(
    const char *policy_map_name)
{
    enum
    {
        MAX_ACE_IN_ACL = 5,
    };

    RULE_TYPE_MatchType_T           match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;

    RULE_TYPE_RETURN_TYPE_T         ui32_result;

    UI32_T                          policy_map_index;

    RULE_TYPE_UI_ClassMapElement_T  class_element;
    UI32_T                          class_index;

    RULE_TYPE_UI_Action_T           ui_action;
    RULE_TYPE_TBParamEntry_T        meter;

    ui32_result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == ui32_result);

    {
        const char                      *class_map_name = "c1";
        UI32_T                          class_map_index;

        ui32_result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == ui32_result);

        //
        // Match ACL
        //
        {
            const char                      *acl_name = "a1";
            UI32_T                          acl_index;

            RULE_TYPE_AclType_T             acl_type = RULE_TYPE_IP_EXT_ACL;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acl_type, MAX_ACE_IN_ACL, RULE_CTRL_UT_COMPRESS_DISABLE, &acl_index);

            class_element.class_type = RULE_TYPE_CLASS_ACL;
            strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
            class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

            ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
            assert(RULE_TYPE_OK == ui32_result);
        }

        //
        // Match MF
        //

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
        class_element.element.mf_entry.mf_value = 42;

        ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == ui32_result);

        //
        // Action, meter configuration
        //

        ui_action.in_action_type        = IN_ACTION_INVALID;
        ui_action.out_action_type       = OUT_ACTION_INVALID;
        ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
        ui_action.class_action_type     = CLASS_ACTION_INVALID;
        ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
        ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

        meter.meter_model = RULE_TYPE_METER_MODE_FLOW;
        meter.rate = 84;
        meter.burst_size = 4000;
        meter.peak_burst_size = 4000;

        ui32_result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
        assert(RULE_TYPE_OK == ui32_result);
    }

    {
        const char                      *class_map_name = "c2";
        UI32_T                          class_map_index;

        ui32_result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == ui32_result);

        //
        // Match ACL
        //
        {
            const char                      *acl_name = "a3";
            UI32_T                          acl_index;

            RULE_TYPE_AclType_T             acl_type = RULE_TYPE_IP_EXT_ACL;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acl_type, MAX_ACE_IN_ACL, RULE_CTRL_UT_COMPRESS_DISABLE, &acl_index);

            class_element.class_type = RULE_TYPE_CLASS_ACL;
            strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
            class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

            ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
            assert(RULE_TYPE_OK == ui32_result);
        }

        //
        // Match MF
        //

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;
        class_element.element.mf_entry.mf_value = 1001;

        ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == ui32_result);

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_DSCP;
        class_element.element.mf_entry.mf_value = 42;

        ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == ui32_result);

        //
        // Action, meter configuration
        //

        ui_action.in_action_type        = IN_ACTION_INVALID;
        ui_action.out_action_type       = OUT_ACTION_INVALID;
        ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
        ui_action.class_action_type     = CLASS_ACTION_INVALID;
        ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
        ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

        meter.meter_model = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
        meter.rate = 84;
        meter.burst_size = 4000;
        meter.peak_burst_size = 4001;
        meter.peak_rate = 4001;

        ui32_result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
        assert(RULE_TYPE_OK == ui32_result);
    }
}

// policy-map p1
//   class c1, match a1
//   class c2, match a1
//   class c3, match a1
//   ...
void
RULE_CTRL_UT_Proc_Create_Policy_Map_With_Max_Class_Map_With_Match_Same_ACL(
    const char *policy_map_name)
{
    UI32_T policy_map_index;
    UI32_T class_map_index;
    UI32_T acl_index;

    RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ANY;
    RULE_TYPE_AclType_T   acl_type = RULE_TYPE_IP_EXT_ACL;

    RULE_TYPE_Ace_Entry_T ace_entry;

    UI32_T                          class_map_name_id;
    char                            class_map_name[ SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH+1 ];
    RULE_TYPE_UI_ClassMapElement_T  class_element;
    UI32_T                          class_index;

    const char                      *acl_name = "a1";

    RULE_TYPE_UI_Action_T           ui_action;
    RULE_TYPE_TBParamEntry_T        meter;

    UI32_T ui32_result;


    /* create policy map
     */
    ui32_result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == ui32_result);

    /* create ACL
     */
    RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

    ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
    RULE_OM_InitAce(&ace_entry);

    ace_entry.u.ip.aceSourceIpAddr = 0x01020304;
    ace_entry.u.ip.aceSourceIpAddrBitmask = 0xffffffff;

    ui32_result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
    assert(RULE_TYPE_OK == ui32_result);

    ace_entry.u.ip.aceSourceIpAddr = 0x05060708;
    ace_entry.u.ip.aceSourceIpAddrBitmask = 0xffffffff;

    ui32_result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
    assert(RULE_TYPE_OK == ui32_result);

    /* crate class map and bind an ACL
     */
    for (class_map_name_id = 1; ; ++class_map_name_id)
    {
        snprintf(class_map_name, sizeof(class_map_name), "c%lu", class_map_name_id);

        ui32_result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == ui32_result);

        class_element.class_type = RULE_TYPE_CLASS_ACL;
        strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
        class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

        ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == ui32_result);

        memset(&ui_action, 0, sizeof(ui_action));

        ui_action.in_action_type      = IN_ACTION_INVALID;
        ui_action.out_action_type     = OUT_ACTION_INVALID;
        ui_action.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
        ui_action.class_action_type   = CLASS_ACTION_INVALID;
        ui_action.exceed_action_type  = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
        ui_action.exceed_action_value = 32;
        ui_action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
        ui_action.violate_action_value = 63;


        meter.meter_model = RULE_TYPE_METER_MODE_SRTCM_COLOR_AWARE;
        meter.rate = 84;
        meter.burst_size = 4000;
        meter.peak_burst_size = 4000;

        ui32_result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
        if (RULE_TYPE_OK != ui32_result)
        {
            break;
        }

        ui32_result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
        assert(RULE_TYPE_OK == ui32_result);
    }

    assert(1 < class_map_name_id);
}

// policy-map p1
//   class c1, match a1
//   class c2, match a2
//   class c3, match a3
//   ...
void
RULE_CTRL_UT_Proc_Create_Policy_Map_With_Max_Class_Map_With_Match_Diff_ACL(
    const char *policy_map_name)
{
    UI32_T policy_map_index;
    UI32_T class_map_index;
    UI32_T acl_index;

    RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ANY;
    RULE_TYPE_AclType_T   acl_type = RULE_TYPE_IP_EXT_ACL;

    RULE_TYPE_Ace_Entry_T ace_entry;

    UI32_T                          class_map_name_id;

    char                            class_map_name[ SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH+1 ];
    RULE_TYPE_UI_ClassMapElement_T  class_element;
    UI32_T                          class_index;

    char                            acl_name[ SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH+1 ];

    RULE_TYPE_UI_Action_T           ui_action;
    RULE_TYPE_TBParamEntry_T        meter;

    UI32_T ui32_result;

    /* create policy map
     */
    ui32_result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == ui32_result);

    /* crate class map, ACL and bind together
     */
    for (class_map_name_id = 1; ; ++class_map_name_id)
    {
        /* create ACL
         */
        snprintf(acl_name, sizeof(acl_name), "a%lu", class_map_name_id);

        ui32_result = RULE_OM_CreateAcl(acl_name, acl_type);
        //assert(RULE_TYPE_OK == ui32_result);
        if (RULE_TYPE_OK != ui32_result)
        {
            break;
        }

        ui32_result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == ui32_result);

        ace_entry.ace_type = RULE_TYPE_IP_EXT_ACL;
        RULE_OM_InitAce(&ace_entry);

        ace_entry.u.ip.aceSourceIpAddr = 0x01020304;
        ace_entry.u.ip.aceSourceIpAddrBitmask = 0xffffffff;

        ui32_result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == ui32_result);


        ace_entry.u.ip.aceSourceIpAddr = 0x05060708;
        ace_entry.u.ip.aceSourceIpAddrBitmask = 0xffffffff;

        ui32_result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == ui32_result);


        /* Create class map
         */
        snprintf(class_map_name, sizeof(class_map_name), "c%lu", class_map_name_id);

        ui32_result = RULE_OM_Create_ClassMap(class_map_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == ui32_result);

        class_element.class_type = RULE_TYPE_CLASS_ACL;
        strncpy(class_element.element.acl_name, acl_name, sizeof(class_element.element.acl_name)-1);
        class_element.element.acl_name[ sizeof(class_element.element.acl_name)-1 ] = '\0';

        /* Add ACL into class map
         */
        ui32_result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == ui32_result);

        memset(&ui_action, 0, sizeof(ui_action));

        ui_action.in_action_type    = IN_ACTION_INVALID;
        ui_action.out_action_type   = OUT_ACTION_INVALID;
        ui_action.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
        ui_action.class_action_type  = CLASS_ACTION_INVALID;
        ui_action.exceed_action_type = EXCEED_ACTION_REMARK_DSCP_TRANSMIT;
        ui_action.exceed_action_value = 32;
        ui_action.violate_action_type = VIOLATE_ACTION_REMARK_DSCP_TRANSMIT;
        ui_action.violate_action_value = 63;


        meter.meter_model = RULE_TYPE_METER_MODE_SRTCM_COLOR_AWARE;
        meter.rate = 84;
        meter.burst_size = 4000;
        meter.peak_burst_size = 4000;

        /* Add class-map, meter, and action into policy-map
         */
        ui32_result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index, class_map_index);
        if (RULE_TYPE_OK != ui32_result)
        {
            break;
        }

        ui32_result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == ui32_result);

        ui32_result = RULE_OM_SetUIActionEntry(policy_map_index, class_map_index, &ui_action);
        assert(RULE_TYPE_OK == ui32_result);
    }

    assert(1 < class_map_name_id);
}

RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_UT_Proc_Bind_ACL(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    UI32_T time_range_index,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_CTRL_SetAcl(ifindex,
                              direction,
                              acl_index,
                              TRUE, time_range_index, counter_enable);

    if (RULE_TYPE_OK == result)
    {
        RULE_TYPE_RETURN_TYPE_T result_2;
        UI8_T time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
        if (time_range_index != RULE_TYPE_UNDEF_TIME_RANGE)
        {
            // FIXME: time range
            assert(0);
        }

        result_2 = RULE_OM_SetAcl(ifindex, direction, acl_index, time_range_index, time_range_name, counter_enable);
        assert(RULE_TYPE_OK == result_2);
    }

    RULE_CTRL_UT_Proc_Validate_Rule();

    return result;
}

RULE_TYPE_RETURN_TYPE_T
RULE_CTRL_UT_Proc_UnBind_ACL(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T acl_index,
    UI32_T time_range_index,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_CTRL_SetAcl(ifindex,
                              direction,
                              acl_index,
                              FALSE, time_range_index, counter_enable);

    if (RULE_TYPE_OK == result)
    {
        RULE_TYPE_RETURN_TYPE_T result_2;

        result_2 = RULE_OM_ResetAcl(ifindex, direction, acl_index);
        assert(RULE_TYPE_OK == result_2);
    }

    RULE_CTRL_UT_Proc_Validate_Rule();

    return result;
}

void
RULE_CTRL_UT_Proc_Bind_Policy_Map(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_map_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_CTRL_SetPolicyMap(ifindex, direction, policy_map_index, TRUE);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_BindPort2PolicyMap(ifindex, direction, policy_map_index);
    assert(RULE_TYPE_OK == result);
}

void
RULE_CTRL_UT_Proc_UnBind_Policy_Map(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    UI32_T policy_map_index)
{
    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_CTRL_SetPolicyMap(ifindex, direction, policy_map_index, FALSE);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_UnBindPort2PolicyMap(ifindex, direction, policy_map_index);
    assert(RULE_TYPE_OK == result);
}

typedef enum
{
    RULE_CTRL_UT_PROC_BIND_POLICY_CLASS_MAP_FLAG_ADD_RULE = TRUE,
    RULE_CTRL_UT_PROC_BIND_POLICY_CLASS_MAP_FLAG_DEL_RULE = FALSE,
    RULE_CTRL_UT_PROC_BIND_POLICY_CLASS_MAP_FLAG_TEST_RULE = 3,
} RULE_CTRL_UT_PROC_BIND_POLICY_CLASS_MAP_FLAG_T;

void
RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    RULE_CTRL_UT_PROC_BIND_POLICY_CLASS_MAP_FLAG_T flag,
    const char *policy_map_name)
{
    UI32_T policy_map_index;

    UI32_T ui32_result;

    ui32_result = RULE_OM_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == ui32_result);

    if (RULE_CTRL_UT_PROC_BIND_POLICY_CLASS_MAP_FLAG_ADD_RULE == flag)
    {
        RULE_CTRL_UT_Proc_Bind_Policy_Map(ifindex, direction, policy_map_index);

    }
    else if (RULE_CTRL_UT_PROC_BIND_POLICY_CLASS_MAP_FLAG_DEL_RULE == flag)
    {
        RULE_CTRL_UT_Proc_UnBind_Policy_Map(ifindex, direction, policy_map_index);
    }

    RULE_CTRL_UT_Proc_Validate_Rule();
}

/*------------------------------------------------------------------------------
 * Validation Functions
 *------------------------------------------------------------------------------
 */

void
RULE_CTRL_UT_Proc_Validate_FuncType_Remapped_Table()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)

    UI32_T i;

    for (i = 0; i < _countof(shmem_data_p->av_fun_type_tbl.av_vector); ++i)
    {
        RULE_CTRL_FunTypeFreeEntry_T *free_entry = &shmem_data_p->av_fun_type_tbl.av_vector[i];

        assert(NULL != free_entry);

        if (RULE_TYPE_FUNCTION_TYPE_MAX == free_entry->used_by_fun_type)
        {
            continue;
        }

        //
        // If an function type remapped entry exist => exist an ACL refer to at least
        //
        assert(0 != RULE_CTRL_Priv_GetFuncTypeRefCountFromAcl(free_entry->used_by_fun_type));

    }

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT
}

void
RULE_CTRL_UT_Proc_Validate_QoS_Meter(
    UI32_T ifindex,
    const RULE_TYPE_RULE_INSTANCE_T *ace_inst_p)
{
}

void
RULE_CTRL_UT_Proc_Validate_QoS_Action(
    UI32_T ifindex,
    const RULE_TYPE_RULE_INSTANCE_T *ace_inst_p)
{
}

void
RULE_CTRL_UT_Proc_Validate_QoS_Class_Instance_ACL(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    const RULE_TYPE_PolicyElement_T *policy_element_p,
    RULE_TYPE_ClassType_T class_type,
    UI32_T acl_index,
    const RULE_TYPE_CLASS_INSTANCE_T *acl_inst)
{
    UI32_T i;
    UI32_T ui32_result;

    UI32_T unit = STKTPLG_OM_IFINDEX_TO_UNIT(ifindex);
    UI32_T port = STKTPLG_OM_IFINDEX_TO_PORT(ifindex);

    UI32_T module_id, device_id, phy_port;
    UI32_T ace_count;
    UI32_T ace_index;

    RULE_TYPE_AclType_T ace_type;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T           acl_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;

    RULE_TYPE_Acl_T         acl_entry;
    RULE_TYPE_Ace_Entry_T   ace_entry;

    BOOL_T b_result;

    assert(RULE_TYPE_INBOUND == direction ||
           RULE_TYPE_OUTBOUND == direction);

    DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);

    assert(NULL != policy_element_p);
    assert(NULL != acl_inst);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst, &acl_inst_obj);
    acl_inst_obj.get_element_iterator(&acl_inst_obj, &ace_inst_iter);

    assert(RULE_TYPE_CLASS_ACL == class_type);
    assert(RULE_TYPE_INST_ACL == acl_inst->super.type);
    assert(acl_index == acl_inst->id);

    ui32_result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
    assert(RULE_TYPE_OK == ui32_result);

#if (SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL == TRUE)
    ace_count = acl_entry.ace_count;
#else
    ace_count = 0;

    for (ace_index = 0, i = 0;
         RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry);
         ++i)
    {
        if (RULE_TYPE_ACE_PERMIT == ace_entry.access)
        {
            ace_count ++;
        }
    }

    assert(acl_entry.ace_count == i);
#endif // SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL

    assert(ace_count == ace_inst_iter.count(&ace_inst_iter));

    ace_inst_iter.first(&ace_inst_iter);

    {
        RULE_TYPE_CLASS_INSTANCE_T *prev_ace_inst_p;

        prev_ace_inst_p = NULL;

        for (ace_index = 0, i = 0;
             RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry);
             ++i)
        {
            RULE_TYPE_CLASS_INSTANCE_T *ace_inst_p;
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;
            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

            RULE_TYPE_RULE_INSTANCE_T *rule_inst_p;
            DEVRM_VirtualGroup_T vir_grp;
            DEVRM_Rule_T rule;
            int rule_prio;

#if (SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL == TRUE)
            // Nothing to do
#else
            if (RULE_TYPE_ACE_PERMIT != ace_entry.access)
            {
                continue;
            }
#endif // SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL

            ace_inst_p = (RULE_TYPE_CLASS_INSTANCE_T *) ace_inst_iter.get_instance(&ace_inst_iter);

            assert(NULL != ace_inst_p);

            ace_inst_iter.next(&ace_inst_iter);

            //
            // Check the rule order in ACL and rule group is consistent
            //

            assert(ace_index == ace_inst_p->id);

            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(ace_inst_p, &ace_inst_obj);
            ace_inst_obj.get_element_iterator(&ace_inst_obj, &rule_inst_iter);

            rule_inst_iter.first(&rule_inst_iter);
            rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T) rule_inst_iter.get_instance(&rule_inst_iter);

            //
            // Check the ACE ID
            //

            assert(ace_index == rule_inst_p->ace_id);

            //
            // Check wether PCE is exist or not
            //
            b_result = DEVRM_UT_Proc_Get_Rule(device_id, rule_inst_p->dev_rule_info.rule_id,
                                                       &vir_grp, &rule, &rule_prio);

            assert(TRUE == b_result);
            assert(vir_grp.group_id == rule_inst_p->dev_rule_info.group_id);

            //
            // -= Duplicated code start
            //
            switch (acl_entry.acl_type)
            {
                case RULE_TYPE_MAC_ACL:
                    if (RULE_TYPE_INBOUND == direction)
                    {
                        assert(vir_grp.vir_group_id == RULE_TYPE_MAC_DIFFSERV);
                    }
                    else
                    {
                        assert(vir_grp.vir_group_id == RULE_TYPE_EGRESS_MAC_DIFFSERV);
                    }
                    break;

                case RULE_TYPE_IP_STD_ACL:
                case RULE_TYPE_IP_EXT_ACL:
                    if (RULE_TYPE_INBOUND == direction)
                    {
                        assert(vir_grp.vir_group_id == RULE_TYPE_IP_DIFFSERV);
                    }
                    else
                    {
                        assert(vir_grp.vir_group_id == RULE_TYPE_EGRESS_IP_DIFFSERV);
                    }
                    break;

                case RULE_TYPE_IPV6_STD_ACL:
                    if (RULE_TYPE_INBOUND == direction)
                    {
                        assert(vir_grp.vir_group_id == RULE_TYPE_INGRESS_IPV6_STD_DIFFSERV);
                    }
                    else
                    {
                        assert(vir_grp.vir_group_id == RULE_TYPE_EGRESS_IPV6_STD_DIFFSERV);
                    }
                    break;

                case RULE_TYPE_IPV6_EXT_ACL:
                    if (RULE_TYPE_INBOUND == direction)
                    {
                        assert(vir_grp.vir_group_id == RULE_TYPE_INGRESS_IPV6_EXT_DIFFSERV);
                    }
                    else
                    {
                        assert(vir_grp.vir_group_id == RULE_TYPE_EGRESS_IPV6_EXT_DIFFSERV);
                    }
                    break;

                case RULE_TYPE_IP_ACL:
                case RULE_TYPE_IPV6_ACL:
                default:
                    assert(0);
                    break;
            }
            //
            // -= Duplicated code end
            //

            {
                RULE_TYPE_FunctionType_T func_type = vir_grp.vir_group_id;

                FP_CONFIG_FunctionInfo_T *fp_func_info = FP_CONFIG_get_function_info_by_type(func_type);
                FP_CONFIG_GroupInfo_T *fp_group = FP_CONFIG_get_group_info_by_id(fp_func_info->group_id);

                assert(vir_grp.group_id == fp_group->group_id);
            }

//            assert(policy_element_p->meter_index == ace_inst_p->meter_id);

            //
            // Check all ACE in ACL must be sort by priority
            //
            if (NULL != prev_ace_inst_p)
            {
                RULE_TYPE_CLASS_INSTANCE_OBJECT_T prev_ace_inst_obj;
                RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T prev_rule_inst_iter;

                RULE_TYPE_RULE_INSTANCE_PTR_T prev_rule_inst_p;

                DEVRM_VirtualGroup_T prev_vir_grp;
                DEVRM_Rule_T         prev_rule;
                int                  prev_rule_prio;

                RULE_CTRL_IN_Priv_ConstructClassInstanceObject(prev_ace_inst_p, &prev_ace_inst_obj);
                ace_inst_obj.get_element_iterator(&prev_ace_inst_obj, &prev_rule_inst_iter);

                prev_rule_inst_iter.first(&prev_rule_inst_iter);
                prev_rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T) rule_inst_iter.get_instance(&prev_rule_inst_iter);

                b_result = DEVRM_UT_Proc_Get_Rule(device_id, prev_rule_inst_p->dev_rule_info.rule_id,
                                          &prev_vir_grp, &prev_rule, &prev_rule_prio);
                assert(TRUE == b_result);

                assert(vir_grp.group_id == prev_vir_grp.group_id);
                assert(vir_grp.vir_group_id == prev_vir_grp.vir_group_id);

                //
                // logical priority
                //
                assert(rule.rule_pri <= prev_rule.rule_pri);

                //
                // physical priority
                //
                assert(rule_prio < prev_rule_prio);
            }

            RULE_CTRL_UT_Proc_Validate_QoS_Meter(ifindex, rule_inst_p);

            if (0 != rule_inst_p->meter_id)
            {
                RULE_CTRL_UT_Proc_Validate_QoS_Action(ifindex, rule_inst_p);
            }

            prev_ace_inst_p = ace_inst_p;

        } // for (ace_index = 0;

        assert(acl_entry.ace_count == i);
    }
}

void
RULE_CTRL_UT_Proc_Validate_QoS_Class_Instance_MF(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    const RULE_TYPE_PolicyElement_T *policy_element_p,
    RULE_TYPE_ClassType_T class_type,
    UI32_T ace_index,
    const RULE_TYPE_CLASS_INSTANCE_T *mf_ace_inst)
{
    RULE_TYPE_Ace_Entry_T   ace_entry;

    UI32_T                  ui32_result;
    BOOL_T                  b_result;

    UI32_T unit = STKTPLG_OM_IFINDEX_TO_UNIT(ifindex);
    UI32_T port = STKTPLG_OM_IFINDEX_TO_PORT(ifindex);

    UI32_T module_id, device_id, phy_port;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T           mf_ace_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T mf_ace_inst_iter;

    assert(RULE_TYPE_INBOUND == direction ||
           RULE_TYPE_OUTBOUND == direction);

    DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);

    assert(NULL != policy_element_p);
    assert(NULL != mf_ace_inst);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(mf_ace_inst, &mf_ace_inst_obj);
    mf_ace_inst_obj.get_element_iterator(&mf_ace_inst_obj, &mf_ace_inst_iter);

    assert(RULE_TYPE_CLASS_MF == class_type);
    assert(RULE_TYPE_INST_MF_ACE == mf_ace_inst->super.type);
    assert(ace_index == mf_ace_inst->id);


    ui32_result = RULE_OM_GetAceByIndex(ace_index, &ace_entry);
    assert(RULE_TYPE_OK == ui32_result);

    assert(1 == mf_ace_inst_iter.count(&mf_ace_inst_iter));

    {
        RULE_TYPE_RULE_INSTANCE_T   *ace_inst_p;
        DEVRM_VirtualGroup_T        vir_grp;
        DEVRM_Rule_T                rule;
        int                         rule_prio;

        ace_inst_p = (RULE_TYPE_RULE_INSTANCE_T *) mf_ace_inst_iter.get_instance(&mf_ace_inst_iter);

        //
        // Check wether ACE index is consistent
        //
        assert(ace_index == ace_inst_p->ace_id);

        //
        // Check wether rule is exist or not
        //
        b_result = DEVRM_UT_Proc_Get_Rule(device_id, ace_inst_p->dev_rule_info.rule_id, &vir_grp, &rule, &rule_prio);
        assert(TRUE == b_result);

        assert(vir_grp.group_id == ace_inst_p->dev_rule_info.group_id);

        //
        // -= Duplicated code start
        //
        switch (ace_entry.ace_type)
        {
            case RULE_TYPE_MAC_ACL:
                if (RULE_TYPE_INBOUND == direction)
                {
                    assert(vir_grp.vir_group_id == RULE_TYPE_MAC_DIFFSERV);
                }
                else
                {
                    assert(vir_grp.vir_group_id == RULE_TYPE_EGRESS_MAC_DIFFSERV);
                }
                break;

            case RULE_TYPE_IP_STD_ACL:
            case RULE_TYPE_IP_EXT_ACL:
                if (RULE_TYPE_INBOUND == direction)
                {
                    assert(vir_grp.vir_group_id == RULE_TYPE_IP_DIFFSERV);
                }
                else
                {
                    assert(vir_grp.vir_group_id == RULE_TYPE_EGRESS_IP_DIFFSERV);
                }
                break;

            case RULE_TYPE_IPV6_STD_ACL:
                if (RULE_TYPE_INBOUND == direction)
                {
                    assert(vir_grp.vir_group_id == RULE_TYPE_INGRESS_IPV6_STD_DIFFSERV);
                }
                else
                {
                    assert(vir_grp.vir_group_id == RULE_TYPE_EGRESS_IPV6_STD_DIFFSERV);
                }
                break;

            case RULE_TYPE_IPV6_EXT_ACL:
                if (RULE_TYPE_INBOUND == direction)
                {
                    assert(vir_grp.vir_group_id == RULE_TYPE_INGRESS_IPV6_EXT_DIFFSERV);
                }
                else
                {
                    assert(vir_grp.vir_group_id == RULE_TYPE_EGRESS_IPV6_EXT_DIFFSERV);
                }
                break;

            case RULE_TYPE_IP_ACL:
            case RULE_TYPE_IPV6_ACL:
            default:
                assert(0);
                break;
        }
        //
        // -= Duplicated code end
        //

        {
            RULE_TYPE_FunctionType_T func_type = vir_grp.vir_group_id;

            FP_CONFIG_FunctionInfo_T *fp_func_info = FP_CONFIG_get_function_info_by_type(func_type);
            FP_CONFIG_GroupInfo_T *fp_group = FP_CONFIG_get_group_info_by_id(fp_func_info->group_id);

            assert(vir_grp.group_id == fp_group->group_id);
        }

        assert(policy_element_p->meter_index == ace_inst_p->meter_id);

        RULE_CTRL_UT_Proc_Validate_QoS_Meter(ifindex, ace_inst_p);

        if (0 != ace_inst_p->meter_id)
        {
            RULE_CTRL_UT_Proc_Validate_QoS_Action(ifindex, ace_inst_p);
        }
    }

}

void
RULE_CTRL_UT_Proc_Validate_QoS_Class_Instance(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    const RULE_TYPE_PolicyElement_T *policy_element_p,
    RULE_TYPE_ClassType_T class_type,
    UI32_T class_id,
    const RULE_TYPE_INSTANCE_T *inst)
{
    switch (class_type)
    {
        case RULE_TYPE_CLASS_ACL:
            RULE_CTRL_UT_Proc_Validate_QoS_Class_Instance_ACL(ifindex,
                                                              direction,
                                                              policy_element_p,
                                                              class_type,
                                                              class_id,
                                                              (const RULE_TYPE_CLASS_INSTANCE_T *)inst);
            break;

        case RULE_TYPE_CLASS_MF:
            RULE_CTRL_UT_Proc_Validate_QoS_Class_Instance_MF(ifindex,
                                                             direction,
                                                             policy_element_p,
                                                             class_type,
                                                             class_id,
                                                             (const RULE_TYPE_CLASS_INSTANCE_T *)inst);
            break;

        default:
            assert(0);
            break;
    }
}

void
RULE_CTRL_UT_Proc_Validate_QoS()
{
    UI32_T ifindex;

    //
    // Scan QoS rule by uport/direction
    //

    ifindex = 0;
    while (RULE_TYPE_OK == RULE_CTRL_UT_Proc_Get_Next_Ifindex(&ifindex))
    {
        RULE_TYPE_InOutDirection_T direction;

        for (direction = RULE_TYPE_INBOUND; direction <= RULE_TYPE_OUTBOUND; ++direction)
        {
            RULE_TYPE_CLASS_INSTANCE_PTR_T policy_map_inst_p;

            UI32_T  policy_map_index;

            RULE_TYPE_RETURN_TYPE_T result;

            RULE_TYPE_INTERFACE_INFO_T interface;

            BOOL_T  b_result;

            memset(&interface, 0, sizeof(interface));
            result = RULE_CTRL_UT_Proc_Ifindex_To_Interface(ifindex, &interface);
            assert(RULE_TYPE_OK == result);
            interface.direction  = direction;

            result = RULE_OM_GetPortPolicyMapIndex(ifindex, direction,
                                                             &policy_map_index);

            policy_map_inst_p = RULE_CTRL_Priv_GetPolicyMapInstance(&interface);

            assert((RULE_TYPE_OK == result && policy_map_inst_p && policy_map_index == policy_map_inst_p->policy_map_id) ||
                   (RULE_TYPE_OK != result && NULL == policy_map_inst_p));

            if (RULE_TYPE_OK == result)
            {
                RULE_TYPE_PortEntry_T   port_entry;
                RULE_TYPE_PolicyMap_T   policy_map_entry;

                UI16_T                  policy_index;

                result = RULE_OM_Get_PortEntryEx(ifindex,
                                                 direction,
                                                 &port_entry);
                assert(RULE_TYPE_OK == result);

                result = RULE_OM_GetPolicyMapByIndex(policy_map_index,
                                                     &policy_map_entry);
                assert(RULE_TYPE_OK == result);

                for (policy_index = 0;
                     policy_index < policy_map_entry.element_count;
                     ++policy_index)
                {
                    RULE_TYPE_PolicyElement_T   policy_element;
                    RULE_TYPE_ClassMap_T        class_map_entry;

                    UI32_T                      policy_element_index;
                    UI32_T                      class_index;

                    policy_element_index = policy_map_entry.policy_map_element_index_list[policy_index];

                    result = RULE_OM_GetPolicyMapElement(policy_element_index,
                                                         &policy_element);
                    assert(RULE_TYPE_OK == result);

                    result = RULE_OM_GetClassMap(policy_element.class_map_index,
                                                 &class_map_entry);
                    assert(RULE_TYPE_OK == result);

                    if (0 == class_map_entry.row_status)
                    {
                        continue;
                    }

                    assert(RULE_TYPE_CLASS_MAP_MATCH_ANY == class_map_entry.class_map_match_type);

                    if (RULE_TYPE_CLASS_MAP_MATCH_ANY == class_map_entry.class_map_match_type)
                    {
                        for (class_index = 0;
                             class_index < class_map_entry.class_element_count;
                             ++class_index)
                        {
                            RULE_TYPE_ClassType_T     class_type = RULE_TYPE_CLASS_MF;
                            UI32_T                    class_id   = 0;

                            RULE_TYPE_INSTANCE_T      *mf_or_acl_inst = NULL;

                            class_type = class_map_entry.class_type[ class_index ];
                            class_id   = class_map_entry.class_index_list[ class_index ];

                            mf_or_acl_inst = (RULE_TYPE_INSTANCE_PTR_T) RULE_CTRL_UT_Proc_Get_QoS_Class_Instance(ifindex,
                                                                                                                 direction,
                                                                                                                 policy_map_index,
                                                                                                                 policy_element.class_map_index,
                                                                                                                 class_type,
                                                                                                                 class_id);
                            assert(NULL != mf_or_acl_inst);

                            RULE_CTRL_UT_Proc_Validate_QoS_Class_Instance(ifindex,
                                                                          direction,
                                                                          &policy_element,
                                                                          class_type,
                                                                          class_id,
                                                                          mf_or_acl_inst);

                            //
                            // Check rule priority of MF/ACL in the different class-map
                            //
                            if (0 < class_index)
                            {
                                RULE_TYPE_INSTANCE_T        *prev_mf_or_acl_inst = NULL;

                                RULE_TYPE_CLASS_INSTANCE_OBJECT_T mf_or_acl_obj;
                                RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;

                                RULE_TYPE_CLASS_INSTANCE_OBJECT_T prev_mf_or_acl_obj;
                                RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T prev_ace_inst_iter;

                                RULE_TYPE_RULE_INSTANCE_T   *prev_class_rule_inst = NULL;
                                RULE_TYPE_RULE_INSTANCE_T   *rule_inst = NULL;

                                UI32_T prev_class_type = class_map_entry.class_type[ class_index -1 ];
                                UI32_T prev_class_id   = class_map_entry.class_index_list[ class_index -1 ];

                                prev_mf_or_acl_inst = (RULE_TYPE_INSTANCE_PTR_T) RULE_CTRL_UT_Proc_Get_QoS_Class_Instance(ifindex,
                                                                                                                          direction,
                                                                                                                          policy_map_index,
                                                                                                                          policy_element.class_map_index,
                                                                                                                          prev_class_type,
                                                                                                                          prev_class_id);
                                assert(NULL != prev_mf_or_acl_inst);

                                RULE_CTRL_IN_Priv_ConstructClassInstanceObject((RULE_TYPE_CLASS_INSTANCE_T *)mf_or_acl_inst,
                                                                               &mf_or_acl_obj);
                                mf_or_acl_obj.get_element_iterator(&mf_or_acl_obj, &ace_inst_iter);
                                rule_inst = (RULE_TYPE_RULE_INSTANCE_T *)ace_inst_iter.first(&ace_inst_iter);

                                RULE_CTRL_IN_Priv_ConstructClassInstanceObject((RULE_TYPE_CLASS_INSTANCE_T *)prev_mf_or_acl_inst,
                                                                               &prev_mf_or_acl_obj);
                                prev_mf_or_acl_obj.get_element_iterator(&prev_mf_or_acl_obj, &prev_ace_inst_iter);
                                prev_class_rule_inst = (RULE_TYPE_RULE_INSTANCE_T *)prev_ace_inst_iter.first(&prev_ace_inst_iter);

                                if (rule_inst && prev_class_rule_inst)
                                {
                                    DEVRM_VirtualGroup_T vir_grp;
                                    DEVRM_VirtualGroup_T prev_vir_grp;

                                    DEVRM_Rule_T rule;
                                    DEVRM_Rule_T prev_rule;

                                    int rule_prio;
                                    int prev_rule_prio;

                                    b_result = DEVRM_UT_Proc_Get_Rule(rule_inst->dev_rule_info.device,
                                                                      rule_inst->dev_rule_info.rule_id,
                                                       &vir_grp, &rule, &rule_prio);

                                    assert(TRUE == b_result);

                                    b_result = DEVRM_UT_Proc_Get_Rule(prev_class_rule_inst->dev_rule_info.device,
                                                                      prev_class_rule_inst->dev_rule_info.rule_id,
                                                                      &prev_vir_grp, &prev_rule, &prev_rule_prio);
                                    assert(TRUE == b_result);

                                    assert(rule_prio < prev_rule_prio);
                                }
                            }
                        } // class_index
                    } // RULE_TYPE_CLASS_MAP_MATCH_ANY

#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
                    else if (RULE_TYPE_CLASS_MAP_MATCH_ALL == class_map_entry.class_map_match_type)
                    {
                        assert(0);
                    }
#endif // SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL
                    else
                    {
                        assert(0);
                    }

                } // policy_index

            }
        } // direction
    } // ifindex
}

void RULE_CTRL_UT_Proc_Validate_ACL_Action(
    UI32_T ifindex,
    RULE_TYPE_RULE_INSTANCE_T *rule_inst_p)
{
}

void
RULE_CTRL_UT_Proc_Validate_ACLs()
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T ifindex;

    //
    // Scan ACL by uport/direction/acl_type
    //

    for (ifindex = 1; ifindex <= SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ifindex)
    {
        RULE_TYPE_InOutDirection_T direction;

        UI32_T unit = STKTPLG_OM_IFINDEX_TO_UNIT(ifindex);
        UI32_T port = STKTPLG_OM_IFINDEX_TO_PORT(ifindex);

        UI32_T module_id, device_id, phy_port;

        DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);

        for (direction = RULE_TYPE_INBOUND; direction <= RULE_TYPE_OUTBOUND; ++direction)
        {

            RULE_TYPE_AclType_T acl_type_ary[] = {
                RULE_TYPE_MAC_ACL,
                RULE_TYPE_IP_ACL,
                RULE_TYPE_IPV6_ACL
            };

            RULE_TYPE_AclType_T acl_type_idx;
            RULE_TYPE_INTERFACE_INFO_T interface;

            result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
            assert(RULE_TYPE_OK == result);

            interface.direction = direction;

            for (acl_type_idx = 0; acl_type_idx < _countof(acl_type_ary); ++acl_type_idx)
            {
                UI32_T                      acl_type = acl_type_ary[acl_type_idx];
                UI32_T                      acl_index;

                RULE_TYPE_COUNTER_ENABLE_T  counter_enable;

                UI16_T                      time_range_index;

                BOOL_T                      b_result;

                result = RULE_OM_GetPortAclIndex(ifindex, acl_type,
                                                 direction==RULE_TYPE_INBOUND?TRUE:FALSE,
                                                 &acl_index,
                                                 &time_range_index,
                                                 &counter_enable);

                if (RULE_TYPE_OK == result)
                {
                    UI32_T                      ace_index;
                    UI32_T                      i;

                    RULE_TYPE_AclType_T         ace_type;

                    RULE_TYPE_Acl_T             acl_entry;
                    RULE_TYPE_Ace_Entry_T       ace_entry;
                    RULE_TYPE_PortEntry_T       port_entry;

                    RULE_TYPE_CLASS_INSTANCE_T  *acl_inst_p;

                    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;
                    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_iter;

                    RULE_TYPE_FunctionType_T fun_type;
                    UI32_T  group_id;
                    int     fun_prio;

                    result = RULE_OM_Get_PortEntryEx(ifindex,
                                                     direction,
                                                     &port_entry);
                    assert(RULE_TYPE_OK == result);

                    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
                    assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
                    if (TRUE == acl_entry.flag_aggregate_done &&
                        0 != acl_entry.aggregate_acl_index)
                    {
                        acl_index = acl_entry.aggregate_acl_index;

                        result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
                        assert(RULE_TYPE_OK == result);
                    }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

                    b_result = RULE_CTRL_GetFunctionTypeForACLAndQoS(TRUE,
                                   RULE_TYPE_DIRECTION_TO_IS_INGRESS(direction),
                                                             acl_entry.acl_type,
                                                                     &fun_type);
                    assert(TRUE == b_result);

                    b_result = RULE_CTRL_GetFunctionInfoByFunctionType(fun_type,
                                                                       &group_id,
                                                                       &fun_prio);
                    assert(TRUE == b_result);

                    //
                    // Check ACL instance
                    //

                    acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                    assert(NULL != acl_inst_p);

                    assert(acl_inst_p->super.type == RULE_TYPE_INST_ACL);
                    assert(acl_inst_p->id == acl_index);
                    assert(0 < acl_inst_p->ref_count);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                    assert(time_range_index == acl_inst_p->time_range_index);
#else
                    assert(RULE_TYPE_UNDEF_TIME_RANGE == acl_inst_p->time_range_index);
#endif // SYS_CPNT_TIME_BASED_ACL

#if (SYS_CPNT_ACL_COUNTER == TRUE)
                    assert(counter_enable == acl_inst_p->counter_enable);
#else
                    assert(RULE_TYPE_COUNTER_DISABLE == acl_inst_p->counter_enable);
#endif // SYS_CPNT_ACL_COUNTER

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
                    assert(acl_inst_p->fun_type == fun_type);

                    if (acl_inst_p->fun_type != acl_inst_p->remap_fun_type)
                    {
                        RULE_CTRL_FunTypeFreeEntry_T *free_entry;

                        free_entry = RULE_CTRL_Priv_GetFunTypeFreeEntry(acl_inst_p->remap_fun_type);

                        assert(NULL != free_entry);
                        assert(free_entry->fun_type == acl_inst_p->remap_fun_type);
                        assert(free_entry->used_by_fun_type == acl_inst_p->fun_type);

                        fun_type = acl_inst_p->remap_fun_type;

                        b_result = RULE_CTRL_GetFunctionInfoByFunctionType(fun_type,
                                                                           &group_id,
                                                                           &fun_prio);
                        assert(TRUE == b_result);
                    }
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

                    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p,
                                                                   &acl_inst_obj);
                    acl_inst_obj.get_element_iterator(&acl_inst_obj, &rule_iter);

                    assert(rule_iter.count(&rule_iter) == acl_entry.ace_count);

                    for (ace_index = 0, i = 0;
                         RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry);
                         ++i, rule_iter.next(&rule_iter))
                    {
                        RULE_TYPE_RULE_INSTANCE_T *rule_inst_ptr;

                        DEVRM_Rule_T rule;
                        DEVRM_VirtualGroup_T vir_grp;
                        int rule_prio;

                        rule_inst_ptr = (RULE_TYPE_RULE_INSTANCE_T *) rule_iter.get_instance(&rule_iter);

                        assert(NULL != rule_inst_ptr);
                        assert(RULE_TYPE_INST_RULE == rule_inst_ptr->super.type);

                        assert(rule_inst_ptr->dev_rule_info.group_id == group_id);

                        //
                        // Check the rule order in ACL and rule group is consistent
                        //

                        assert(ace_index == rule_inst_ptr->ace_id);

                        //
                        // Check the port bitmap
                        //

                        if (RULE_TYPE_INTERFACE_UPORT == interface.type)
                        {
                            UI32_T _module_id;
                            UI32_T _device_id;
                            UI32_T _phy_port;

                            b_result = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(interface.uport.unit,
                                                                              interface.uport.port,
                                                                              &_module_id,
                                                                              &_device_id,
                                                                              &_phy_port);
                            assert(TRUE == b_result);

                            assert(BCM_PBMP_NOT_NULL(rule_inst_ptr->dev_rule_info.port_bmp));
                            assert(BCM_PBMP_MEMBER(rule_inst_ptr->dev_rule_info.port_bmp, _phy_port));
                        }

                        //
                        // Check time-range index
                        //

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
                        {
                            BOOL_T _is_active = RULE_CTRL_LocalCheckRuleInActiveByTimeRange(ace_entry.time_range_index,
                                                                                            acl_inst_p->time_range_index);

                            assert(rule_inst_ptr->active == _is_active);
                        }
#else
                        assert(rule_inst_ptr->active == 1);
#endif // SYS_CPNT_TIME_BASED_ACL

                        //
                        // Ignore if rule is not active
                        //

                        if (rule_inst_ptr->active == FALSE)
                        {
                            continue;
                        }

                        //
                        // Check the virtual group is correct
                        //

                        b_result = DEVRM_UT_Proc_Get_Rule(device_id,
                                           rule_inst_ptr->dev_rule_info.rule_id,
                                                   &vir_grp, &rule, &rule_prio);
                        assert(TRUE == b_result);
                        assert(vir_grp.group_id == rule_inst_ptr->dev_rule_info.group_id);

                        assert(vir_grp.vir_group_id == fun_type);
                        assert(vir_grp.pri_lvl == fun_prio);
                        assert(vir_grp.group_id == group_id);

                        //
                        // Check stage of group
                        //

                        {
                            bcm_field_qset_t qset = {0};

                            bcm_field_group_get(device_id, rule_inst_ptr->dev_rule_info.group_id, &qset);

                            if (RULE_TYPE_INBOUND == direction)
                            {
                                assert(0 == DEVRM_SHR_BITGET(qset.w, DEVRM_FIELD_QUALIFY_StageEgress));
                            }
                            else if (RULE_TYPE_OUTBOUND == direction)
                            {
                                assert(0 != DEVRM_SHR_BITGET(qset.w, DEVRM_FIELD_QUALIFY_StageEgress));
                            }
                        }

                        //
                        // Check all ACE in ACL must be sort by priority
                        //

                        if (TRUE == rule_iter.has_prev(&rule_iter))
                        {
                            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_iter_tmp = rule_iter;

                            RULE_TYPE_RULE_INSTANCE_T *prev_rule_inst_ptr;

                            DEVRM_VirtualGroup_T prev_vir_grp;
                            DEVRM_Rule_T    prev_rule;
                            int             prev_rule_prio;

                            prev_rule_inst_ptr = (RULE_TYPE_RULE_INSTANCE_T *) rule_iter_tmp.prev(&rule_iter_tmp);
                            assert(NULL != prev_rule_inst_ptr);

                            if (TRUE == prev_rule_inst_ptr->active)
                            {
                                b_result = DEVRM_UT_Proc_Get_Rule(device_id,
                                                    prev_rule_inst_ptr->dev_rule_info.rule_id,
                                        &prev_vir_grp, &prev_rule, &prev_rule_prio);
                                assert(TRUE == b_result);

                                assert(vir_grp.group_id == prev_vir_grp.group_id);
                                assert(vir_grp.vir_group_id == prev_vir_grp.vir_group_id);

                                //
                                // logical priority
                                //
                                assert(rule.rule_pri <= prev_rule.rule_pri);

                                //
                                // physical priority
                                //
                                assert(rule_prio < prev_rule_prio);
                            }

                        }

                        RULE_CTRL_UT_Proc_Validate_ACL_Action(ifindex, rule_inst_ptr/*, pce*/);
                    }

                    assert(acl_entry.ace_count == i);

                } // ui32_result == OK

            } // acl_type_idx

        } // direction

    } // ifindex
}

void
RULE_CTRL_UT_Proc_Validate_Leaks_Instance_Make_ACE(
    UI32_T ace_index,
    RULE_TYPE_RULE_INSTANCE_T *ace_inst,
    RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst_pool_buf,
    RULE_TYPE_RULE_INSTANCE_T *dup_rule_inst_pool_buf)
{
    UI32_T                                  rule_ary_index;
    RULE_TYPE_RULE_INSTANCE_T               *dup_rule_inst;

    RULE_TYPE_Ace_Entry_T                   ace_entry;
    UI32_T                                  ui32_result;

    assert(0 != ace_index);
    assert(NULL != ace_inst);
    assert(NULL != dup_class_inst_pool_buf);
    assert(NULL != dup_rule_inst_pool_buf);

    assert(RULE_TYPE_INST_RULE == ace_inst->super.type);
    assert(ace_inst->id == ace_index);

    ui32_result = RULE_OM_GetAceByIndex(ace_inst->id, &ace_entry);
    assert(RULE_TYPE_OK == ui32_result);

    rule_ary_index = RULE_CTRL_IN_IndexOfRuleInstance(ace_inst);
    dup_rule_inst = &dup_rule_inst_pool_buf[ rule_ary_index ];

    assert(0 == memcmp(dup_rule_inst, ace_inst, sizeof(*dup_rule_inst)));
    memset(dup_rule_inst, 0, sizeof(*dup_rule_inst));
}

void
RULE_CTRL_UT_Proc_Validate_Leaks_Instance_Make_MF_ACE(
    UI32_T ace_index,
    RULE_TYPE_CLASS_INSTANCE_T *mf_ace_inst,
    RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst_pool_buf,
    RULE_TYPE_RULE_INSTANCE_T *dup_rule_inst_pool_buf)
{
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T           mf_ace_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;

    UI32_T                                      class_ary_index;
    RULE_TYPE_CLASS_INSTANCE_T                  *dup_class_inst;

    assert(0 != ace_index);
    assert(NULL != mf_ace_inst);
    assert(NULL != dup_class_inst_pool_buf);
    assert(NULL != dup_rule_inst_pool_buf);

    assert(RULE_TYPE_INST_MF_ACE == mf_ace_inst->super.type);
    assert(mf_ace_inst->id == ace_index);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(mf_ace_inst, &mf_ace_inst_obj);
    mf_ace_inst_obj.get_element_iterator(&mf_ace_inst_obj, &ace_inst_iter);

    assert(1 == ace_inst_iter.count(&ace_inst_iter));

    RULE_CTRL_UT_Proc_Validate_Leaks_Instance_Make_ACE(mf_ace_inst->id,
                                                       (RULE_TYPE_RULE_INSTANCE_T*) ace_inst_iter.first(&ace_inst_iter),
                                                       dup_class_inst_pool_buf,
                                                       dup_rule_inst_pool_buf);

    class_ary_index = RULE_CTRL_IN_IndexOfClassInstance(mf_ace_inst);
    dup_class_inst = &dup_class_inst_pool_buf[ class_ary_index ];

    assert(0 == memcmp(dup_class_inst, mf_ace_inst, sizeof(*dup_class_inst)));
    memset(dup_class_inst, 0, sizeof(*dup_class_inst));
}

void
RULE_CTRL_UT_Proc_Validate_Leaks_Instance_Make_QoS_ACL(
    UI32_T acl_index,
    RULE_TYPE_CLASS_INSTANCE_T *acl_inst,
    RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst_pool_buf,
    RULE_TYPE_RULE_INSTANCE_T *dup_rule_inst_pool_buf)
{
    UI32_T                                      i;
    UI32_T                                      ace_index;
    RULE_TYPE_AclType_T                         ace_type;

    RULE_TYPE_Acl_T                             acl_entry;
    RULE_TYPE_Ace_Entry_T                       ace_entry;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T           acl_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;

    UI32_T                                      class_ary_index;
    RULE_TYPE_CLASS_INSTANCE_T                  *dup_class_inst;

    UI32_T                                      ui32_result;

    assert(0 != acl_index);
    assert(NULL != acl_inst);
    assert(NULL != dup_class_inst_pool_buf);
    assert(NULL != dup_rule_inst_pool_buf);

    assert(RULE_TYPE_INST_ACL == acl_inst->super.type);
    assert(acl_inst->id == acl_index);

    ui32_result = RULE_OM_GetAclByIndex(acl_inst->id, &acl_entry);
    assert(RULE_TYPE_OK == ui32_result);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst, &acl_inst_obj);
    acl_inst_obj.get_element_iterator(&acl_inst_obj, &ace_inst_iter);

    for (ace_index = 0, i = 0;
         RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry);
         ++i)
    {
        RULE_TYPE_RULE_INSTANCE_T   *ace_inst;

#if (SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL == TRUE)
        // Nothing to do
#else
        if (RULE_TYPE_ACE_PERMIT != ace_entry.access)
        {
            continue;
        }
#endif // SYS_CPNT_DIFFSERV_USE_DENY_ACE_IN_ACL

        ace_inst = (RULE_TYPE_RULE_INSTANCE_T *) ace_inst_iter.get_instance(&ace_inst_iter);

        assert(NULL != ace_inst);

        ace_inst_iter.next(&ace_inst_iter);

        RULE_CTRL_UT_Proc_Validate_Leaks_Instance_Make_ACE(ace_index, ace_inst,
                                                        dup_class_inst_pool_buf,
                                                        dup_rule_inst_pool_buf);
    }

    assert(acl_entry.ace_count == i);

    class_ary_index = RULE_CTRL_IN_IndexOfClassInstance(acl_inst);
    dup_class_inst = &dup_class_inst_pool_buf[ class_ary_index ];

    assert(0 == memcmp(dup_class_inst, acl_inst, sizeof(*dup_class_inst)));
    memset(dup_class_inst, 0, sizeof(*dup_class_inst));
}

void
RULE_CTRL_UT_Proc_Validate_Leaks_Instance_Make_ACL(
    UI32_T acl_index,
    RULE_TYPE_CLASS_INSTANCE_T *acl_inst,
    RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst_pool_buf,
    RULE_TYPE_RULE_INSTANCE_T *dup_rule_inst_pool_buf)
{
    UI32_T                                      i;
    UI32_T                                      ace_index;
    RULE_TYPE_AclType_T                         ace_type;

    RULE_TYPE_Acl_T                             acl_entry;
    RULE_TYPE_Ace_Entry_T                       ace_entry;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T           acl_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;

    UI32_T                                      class_ary_index;
    RULE_TYPE_CLASS_INSTANCE_T                  *dup_class_inst;

    UI32_T                                      ui32_result;

    assert(0 != acl_index);
    assert(NULL != acl_inst);
    assert(NULL != dup_class_inst_pool_buf);
    assert(NULL != dup_rule_inst_pool_buf);

    assert(RULE_TYPE_INST_ACL == acl_inst->super.type);
    assert(acl_inst->id == acl_index);

    ui32_result = RULE_OM_GetAclByIndex(acl_inst->id, &acl_entry);
    assert(RULE_TYPE_OK == ui32_result);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst, &acl_inst_obj);
    acl_inst_obj.get_element_iterator(&acl_inst_obj, &ace_inst_iter);

    for (ace_index = 0, i = 0;
         RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry);
         ++i)
    {
        RULE_TYPE_RULE_INSTANCE_T   *ace_inst;

        ace_inst = (RULE_TYPE_RULE_INSTANCE_T *) ace_inst_iter.get_instance(&ace_inst_iter);

        assert(NULL != ace_inst);

        ace_inst_iter.next(&ace_inst_iter);

        RULE_CTRL_UT_Proc_Validate_Leaks_Instance_Make_ACE(ace_index, ace_inst,
                                                        dup_class_inst_pool_buf,
                                                        dup_rule_inst_pool_buf);
    }

    assert(acl_entry.ace_count == i);

    class_ary_index = RULE_CTRL_IN_IndexOfClassInstance(acl_inst);
    dup_class_inst = &dup_class_inst_pool_buf[ class_ary_index ];

    assert(0 == memcmp(dup_class_inst, acl_inst, sizeof(*dup_class_inst)));
    memset(dup_class_inst, 0, sizeof(*dup_class_inst));
}

void
RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface_Make_ACE(
    const RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p,
    RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst_pool_buf,
    RULE_TYPE_RULE_INSTANCE_T *dup_rule_inst_pool_buf)
{
    UI32_T                                  rule_ary_index;
    RULE_TYPE_RULE_INSTANCE_T               *dup_rule_inst;

    assert(NULL != rule_inst_p);
    assert(NULL != dup_class_inst_pool_buf);
    assert(NULL != dup_rule_inst_pool_buf);

    assert(RULE_TYPE_INST_RULE == rule_inst_p->super.type);

    rule_ary_index = RULE_CTRL_IN_IndexOfRuleInstance(rule_inst_p);
    dup_rule_inst = &dup_rule_inst_pool_buf[ rule_ary_index ];

    //
    // The instance of ACL, ACE, and rule shall be count more than once by ACL
    // sharing case
    //
    if (0 != memcmp(dup_rule_inst, rule_inst_p, sizeof(*dup_rule_inst)))
    {
        RULE_TYPE_RULE_INSTANCE_T tmp_rule_inst;

        memset(&tmp_rule_inst, 0, sizeof(tmp_rule_inst));
        assert(0 == memcmp(&tmp_rule_inst, dup_rule_inst, sizeof(tmp_rule_inst)));

        return;
    }

    assert(0 == memcmp(dup_rule_inst, rule_inst_p, sizeof(*dup_rule_inst)));
    memset(dup_rule_inst, 0, sizeof(*dup_rule_inst));
}

void
RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface_Dispatch(
    RULE_TYPE_INSTANCE_PTR_T inst_p,
    RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst_pool_buf,
    RULE_TYPE_RULE_INSTANCE_T *dup_rule_inst_pool_buf
);

void
RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface_Make_Container(
    const RULE_TYPE_CONTAINER_PTR_T container_p,
    RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst_pool_buf,
    RULE_TYPE_RULE_INSTANCE_T *dup_rule_inst_pool_buf)
{
    RULE_TYPE_CONTAINER_OBJECT_T container_obj;
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T inst_iter;

    assert(NULL != container_p);
    assert(NULL != dup_class_inst_pool_buf);
    assert(NULL != dup_rule_inst_pool_buf);

    assert(RULE_CTRL_IS_CONTAINER_TYPE(container_p->super.type));

    RULE_CTRL_IN_Priv_ConstructContainerObject(container_p, &container_obj);

    container_obj.get_element_iterator(&container_obj, &inst_iter);

    for (inst_iter.first(&inst_iter);
         NULL != inst_iter.get_instance(&inst_iter);
         inst_iter.next(&inst_iter))
    {
        RULE_TYPE_INSTANCE_PTR_T inst_p;

        inst_p = inst_iter.get_instance(&inst_iter);

        RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface_Dispatch(inst_p,
                                                                         dup_class_inst_pool_buf,
                                                                         dup_rule_inst_pool_buf);
    }
}

void
RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface_Make_ACL(
    const RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p,
    RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst_pool_buf,
    RULE_TYPE_RULE_INSTANCE_T *dup_rule_inst_pool_buf)
{
    UI32_T class_ary_index;
    RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_inst_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T inst_iter;

    assert(NULL != class_inst_p);
    assert(NULL != dup_class_inst_pool_buf);
    assert(NULL != dup_rule_inst_pool_buf);

    assert(RULE_CTRL_IS_CLASS_INSTANCE_TYPE(class_inst_p->super.type) ||
           RULE_CTRL_IS_RULE_INSTANCE_TYPE(class_inst_p->super.type));

    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p,
                                                   &class_inst_obj);

    class_inst_obj.get_element_iterator(&class_inst_obj, &inst_iter);

    for (inst_iter.first(&inst_iter);
         NULL != inst_iter.get_instance(&inst_iter);
         inst_iter.next(&inst_iter))
    {
        RULE_TYPE_INSTANCE_PTR_T inst_p;

        inst_p = inst_iter.get_instance(&inst_iter);

        RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface_Dispatch(inst_p,
                                                                         dup_class_inst_pool_buf,
                                                                         dup_rule_inst_pool_buf);
    }

    class_ary_index = RULE_CTRL_IN_IndexOfClassInstance(class_inst_p);
    dup_class_inst = &dup_class_inst_pool_buf[ class_ary_index ];

    //
    // The instance of ACL, ACE, and rule shall be count more than once by ACL
    // sharing case
    //
    if (0 != memcmp(dup_class_inst, class_inst_p, sizeof(*dup_class_inst)))
    {
        RULE_TYPE_CLASS_INSTANCE_T tmp_class_inst;

        memset(&tmp_class_inst, 0, sizeof(tmp_class_inst));
        assert(0 == memcmp(&tmp_class_inst, dup_class_inst, sizeof(tmp_class_inst)));

        return;
    }

    assert(0 == memcmp(dup_class_inst, class_inst_p, sizeof(*dup_class_inst)));
    memset(dup_class_inst, 0, sizeof(*dup_class_inst));
}

void
RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface_Dispatch(
    RULE_TYPE_INSTANCE_PTR_T inst_p,
    RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst_pool_buf,
    RULE_TYPE_RULE_INSTANCE_T *dup_rule_inst_pool_buf)
{
    if (RULE_CTRL_IS_CONTAINER_TYPE(inst_p->type))
    {
        RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface_Make_Container((RULE_TYPE_CONTAINER_PTR_T) inst_p,
                                                                               dup_class_inst_pool_buf,
                                                                               dup_rule_inst_pool_buf);
    }
    else if (RULE_CTRL_IS_CLASS_INSTANCE_TYPE(inst_p->type))
    {
        RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface_Make_ACL((RULE_TYPE_CLASS_INSTANCE_PTR_T) inst_p,
                                                                         dup_class_inst_pool_buf,
                                                                         dup_rule_inst_pool_buf);
    }
    else if (RULE_CTRL_IS_RULE_INSTANCE_TYPE(inst_p->type))
    {
        RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface_Make_ACE((RULE_TYPE_RULE_INSTANCE_PTR_T) inst_p,
                                                                         dup_class_inst_pool_buf,
                                                                         dup_rule_inst_pool_buf);
    }
    else
    {
        assert(0);
    }
}

void
RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface(
    const RULE_TYPE_INTERFACE_INFO_T *interface_p,
    RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst_pool_buf,
    RULE_TYPE_RULE_INSTANCE_T *dup_rule_inst_pool_buf)
{
    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T agg_inst_iter;

    if_entry_p = RULE_CTRL_Priv_GetIfEntry(interface_p);
    assert(NULL != if_entry_p);

    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                              &if_class_obj);

    if_class_obj.get_element_iterator(&if_class_obj, &agg_inst_iter);
    for (agg_inst_iter.first(&agg_inst_iter);
         NULL != agg_inst_iter.get_instance(&agg_inst_iter);
         agg_inst_iter.next(&agg_inst_iter))
    {
        RULE_TYPE_INSTANCE_PTR_T agg_inst_p;

        agg_inst_p = agg_inst_iter.get_instance(&agg_inst_iter);

        assert(TRUE != RULE_CTRL_IS_RULE_INSTANCE_TYPE(agg_inst_p->type));

        RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface_Dispatch(agg_inst_p,
                                                                         dup_class_inst_pool_buf,
                                                                         dup_rule_inst_pool_buf);
    }

    //
    // Validate class instance of if entry
    //
    {
        UI32_T class_ary_index;
        RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst;

        class_ary_index = RULE_CTRL_IN_IndexOfClassInstance(if_class_obj.inst);
        dup_class_inst = &dup_class_inst_pool_buf[class_ary_index];

        assert(0 == memcmp(dup_class_inst, if_class_obj.inst, sizeof(*dup_class_inst)));
        memset(dup_class_inst, 0, sizeof(*dup_class_inst));
    }
}

void
RULE_CTRL_UT_Proc_Validate_Leaks_Instance()
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T i;

    UI32_T ifindex;

    RULE_TYPE_CLASS_INSTANCE_T *dup_class_inst_pool_buf = malloc(sizeof(shmem_data_p->_class_inst_pool_buf));
    RULE_TYPE_RULE_INSTANCE_T  *dup_rule_inst_pool_buf  = malloc(sizeof(shmem_data_p->_rule_inst_pool_buf));

    RULE_TYPE_Acl_T dup_acl_table[RULE_TYPE_MAX_NBRS_OF_ACL];
    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_hash[RULE_TYPE_MAX_NBRS_OF_ACL];

    assert(NULL != dup_class_inst_pool_buf);
    assert(NULL != dup_rule_inst_pool_buf);

    memcpy(dup_class_inst_pool_buf, shmem_data_p->_class_inst_pool_buf,
           sizeof(shmem_data_p->_class_inst_pool_buf));

    memcpy(dup_rule_inst_pool_buf, shmem_data_p->_rule_inst_pool_buf,
                                     sizeof(shmem_data_p->_rule_inst_pool_buf));

    memset(dup_acl_table, 0, sizeof(dup_acl_table));
    memset(acl_inst_hash, 0, sizeof(acl_inst_hash));

    for (i = 0; i < _countof(shmem_data_p->if_table); ++ i)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        result = RULE_CTRL_Priv_IfEntry_Index_To_Interface(i, &interface);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Leaks_Instance_CPU_Interface(&interface,
                                                                dup_class_inst_pool_buf,
                                                                dup_rule_inst_pool_buf);
    }

//    free(dup_class_inst_pool_buf);
//    free(dup_rule_inst_pool_buf);
//
//    return;

    ifindex = 0;
    for (ifindex = 1; ifindex < SYS_ADPT_TOTAL_NBR_OF_LPORT; ++ifindex)
    {
        RULE_TYPE_InOutDirection_T direction;

        UI32_T unit = STKTPLG_OM_IFINDEX_TO_UNIT(ifindex);
        UI32_T port = STKTPLG_OM_IFINDEX_TO_PORT(ifindex);

        UI32_T module_id, device_id, phy_port;

        DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);

        for (direction = RULE_TYPE_INBOUND; direction <= RULE_TYPE_OUTBOUND; ++direction)
        {
            //
            // ACL
            //

            RULE_TYPE_AclType_T acl_type_ary[] =
            {
                RULE_TYPE_MAC_ACL,
                RULE_TYPE_IP_ACL,
                RULE_TYPE_IPV6_ACL
            };

            RULE_TYPE_AclType_T acl_type_idx;
//            UI32_T              policy_map_index;

            RULE_TYPE_INTERFACE_INFO_T interface;

            memset(&interface, 0, sizeof(interface));

            RULE_CTRL_UT_Proc_Ifindex_To_Interface(ifindex, &interface);
            interface.direction  = direction;

            for (acl_type_idx = 0; acl_type_idx < _countof(acl_type_ary); ++acl_type_idx)
            {
                UI32_T                      acl_type = acl_type_ary[acl_type_idx];
                UI32_T                      acl_index;

                RULE_TYPE_COUNTER_ENABLE_T  counter_enable;

                UI8_T                       time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};

                result = RULE_OM_GetPortAclIndex(ifindex, acl_type,
                                                 direction==RULE_TYPE_INBOUND ? TRUE : FALSE,
                                                 &acl_index,
                                                 time_range_name,
                                                 &counter_enable);
                if (RULE_TYPE_OK == result)
                {
                    RULE_TYPE_Acl_T acl_entry;

                    RULE_TYPE_CLASS_INSTANCE_T  *acl_inst;

                    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
                    assert(RULE_TYPE_OK == result);

                    //
                    // Update port list
                    //
                    if (RULE_TYPE_INBOUND == direction)
                    {
                        RULE_CTRL_SET_BIT_ON( dup_acl_table[acl_index-1].filtering_in_port_list, (ifindex-1) );
                    }
                    else
                    {
                        RULE_CTRL_SET_BIT_ON( dup_acl_table[acl_index-1].filtering_out_port_list, (ifindex-1) );
                    }

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
                    if (TRUE == acl_entry.flag_aggregate_done &&
                        0 != acl_entry.aggregate_acl_index)
                    {
                        acl_index = acl_entry.aggregate_acl_index;

                        result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
                        assert(RULE_TYPE_OK == result);
                    }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

                    acl_inst = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                    assert(NULL != acl_inst);

                    //
                    // Just check the leakage here
                    // The verification about 'sharing' need to check on another place
                    //

                    if (RULE_TYPE_INBOUND == direction)
                    {
//                        #define HASH_GET(hash, key)                             \
//                        ({                                                      \
//                            UI32_T __i;                                         \
//                            for (__i = 0; __i < _countof(hash); ++ __i) {       \
//                                if (hash[__i] == key) {                         \
//                                    break;                                      \
//                                }                                               \
//                            }                                                   \
//                            (__i < _countof(hash)) ? 1 : 0;                     \
//                        })
//
//                        #define HASH_SET(hash, key)                             \
//                        ({                                                      \
//                            UI32_T __i;                                         \
//                            for (__i = 0; __i < _countof(hash); ++ __i) {       \
//                                if (hash[__i] == 0) {                           \
//                                    hash[__i] = key;                            \
//                                    break;                                      \
//                                }                                               \
//                            }                                                   \
//                            1;                                                  \
//                        })

                        RULE_CTRL_SET_BIT_ON( dup_acl_table[acl_index-1].filtering_in_port_list, (ifindex-1) );

//                        if (0 == HASH_GET(acl_inst_hash, acl_inst))
//                        {
////                            RULE_CTRL_UT_Proc_Validate_Leaks_Instance_Make_ACL(acl_index,
////                                                                               acl_inst,
////                                                                               dup_class_inst_pool_buf,
////                                                                               dup_rule_inst_pool_buf);
////                            HASH_SET(acl_inst_hash, acl_inst);
//                        }
//
////                        #undef HASH_GET
////                        #undef HASH_SET

                    }
                    else
                    {
                        RULE_CTRL_SET_BIT_ON( dup_acl_table[acl_index-1].filtering_out_port_list, (ifindex-1) );
//                        RULE_CTRL_UT_Proc_Validate_Leaks_Instance_Make_ACL(acl_index,
//                                                                           acl_inst,
//                                                                           dup_class_inst_pool_buf,
//                                                                           dup_rule_inst_pool_buf);
                    }
                }
            }

//            //
//            // QoS
//            //
//            result = RULE_OM_GetPortPolicyMapIndex(ifindex, direction,
//                                                   &policy_map_index);
//            if (RULE_TYPE_OK == result)
//            {
//                RULE_TYPE_CLASS_INSTANCE_PTR_T  policy_map_inst_p;
//                RULE_TYPE_PortEntry_T           port_entry;
//                RULE_TYPE_PolicyMap_T           policy_map_entry;
//
//                UI16_T                          policy_index;
//
//                result = RULE_OM_Get_PortEntryEx(ifindex,
//                                                 direction,
//                                                 &port_entry);
//                assert(RULE_TYPE_OK == result);
//
//                result = RULE_OM_GetPolicyMapByIndex(policy_map_index,
//                                                     &policy_map_entry);
//                assert(RULE_TYPE_OK == result);
//
//                policy_map_inst_p = RULE_CTRL_Priv_GetPolicyMapInstance(&interface);
//
//                //
//                // RULE_CTRL_UT_Proc_Validate_Leaks_Instance_Make_PolicyMap
//                //
//
//                assert(NULL != policy_map_inst_p);
//                assert(RULE_TYPE_INST_POLICY_MAP == policy_map_inst_p->super.type);
//                assert(policy_map_index == policy_map_inst_p->policy_map_id);
//
//                {
//                    RULE_TYPE_CLASS_INSTANCE_PTR_T  dup_class_inst;
//                    UI32_T                          class_ary_index;
//
//                    class_ary_index = RULE_CTRL_IN_IndexOfClassInstance(policy_map_inst_p);
//                    dup_class_inst = &dup_class_inst_pool_buf[ class_ary_index ];
//
//                    assert(0 == memcmp(dup_class_inst, policy_map_inst_p, sizeof(*dup_class_inst)));
//                    memset(dup_class_inst, 0, sizeof(*dup_class_inst));
//                }
//
//                for (policy_index = 0;
//                     policy_index < policy_map_entry.element_count;
//                     ++policy_index)
//                {
//                    RULE_TYPE_PolicyElement_T   policy_element;
//                    RULE_TYPE_ClassMap_T        class_map_entry;
//
//                    UI32_T                      policy_element_index;
//                    UI32_T                      class_index;
//
//                    policy_element_index = policy_map_entry.policy_map_element_index_list[policy_index];
//
//                    result = RULE_OM_GetPolicyMapElement(policy_element_index,
//                                                         &policy_element);
//                    assert(RULE_TYPE_OK == result);
//
//                    result = RULE_OM_GetClassMap(policy_element.class_map_index,
//                                                 &class_map_entry);
//                    assert(RULE_TYPE_OK == result);
//
//                    if (0 == class_map_entry.row_status)
//                    {
//                        continue;
//                    }
//
//                    assert(RULE_TYPE_CLASS_MAP_MATCH_ANY == class_map_entry.class_map_match_type);
//
//                    if (RULE_TYPE_CLASS_MAP_MATCH_ANY == class_map_entry.class_map_match_type)
//                    {
//                        for (class_index = 0;
//                             class_index < class_map_entry.class_element_count;
//                             ++class_index)
//                        {
//                            RULE_TYPE_CLASS_INSTANCE_T *rule_group_ptr;
//                            RULE_TYPE_ClassType_T     class_type;
//                            UI32_T                    class_id;
//
//                            class_type = class_map_entry.class_type[ class_index ];
//                            class_id   = class_map_entry.class_index_list[ class_index ];
//
//                            rule_group_ptr = RULE_CTRL_UT_Proc_Get_QoS_Class_Instance(ifindex,
//                                                                                      direction,
//                                                                                      policy_map_index,
//                                                                                      policy_element.class_map_index,
//                                                                                      class_type,
//                                                                                      class_id);
//                            assert(NULL != rule_group_ptr);
//
//                            if (RULE_TYPE_INST_ACL == rule_group_ptr->super.type)
//                            {
//                                RULE_CTRL_UT_Proc_Validate_Leaks_Instance_Make_QoS_ACL(
//                                                                       class_id,
//                                                                 rule_group_ptr,
//                                                        dup_class_inst_pool_buf,
//                                                        dup_rule_inst_pool_buf);
//                            }
//                            else if (RULE_TYPE_INST_MF_ACE == rule_group_ptr->super.type)
//                            {
//                                RULE_CTRL_UT_Proc_Validate_Leaks_Instance_Make_MF_ACE(
//                                                                       class_id,
//                                                                 rule_group_ptr,
//                                                        dup_class_inst_pool_buf,
//                                                        dup_rule_inst_pool_buf);
//                            }
//
//                        }
//                    }
//#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
//                    else if (RULE_TYPE_CLASS_MAP_MATCH_ALL == class_map_entry.class_map_match_type)
//                    {
//                        assert(0);
//                    }
//#endif // SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL
//                    else
//                    {
//                        assert(0);
//                    }
//
//                }
//            }

        } // for (direction
    } // for (ifindex

    //
    // ACL group and MF group
    //
    {
        UI32_T i;

        for (i = 0; i < _countof(shmem_data_p->_class_inst_pool_buf); ++i)
        {
            RULE_TYPE_CLASS_INSTANCE_T *class_inst = &dup_class_inst_pool_buf[i];
            assert(RULE_TYPE_NIL == class_inst->super.type);
        }

        for (i = 0; i < _countof(shmem_data_p->_rule_inst_pool_buf); ++i)
        {
            RULE_TYPE_RULE_INSTANCE_T *rule_inst = &dup_rule_inst_pool_buf[i];
            assert(RULE_TYPE_NIL == rule_inst->super.type);
        }
    }

    //
    // ACL table
    //
    {
        RULE_TYPE_RETURN_TYPE_T result;
        UI32_T acl_index;

        //
        // Verificate port list for user configured ACL
        //

//        for (acl_index = 1; acl_index <= _countof(dup_acl_table); ++acl_index)
        assert(_countof(dup_acl_table) == RULE_TYPE_MAX_NBRS_OF_ACL);

        for (acl_index = 1; acl_index <= SYS_ADPT_MAX_NBRS_OF_ACL; ++acl_index)
        {
            RULE_TYPE_Acl_T acl;
            int    int_result;

            result = RULE_OM_GetAclByIndex(acl_index, &acl);
            if (RULE_TYPE_OK != result)
            {
                memset(&acl, 0, sizeof(acl));
            }

            //
            // filtering_acl_instance and filtering_acl_instance_ref_count
            // are BCM only
            //

            int_result = memcmp(acl.filtering_in_port_list,
                                dup_acl_table[acl_index-1].filtering_in_port_list,
                                sizeof(acl.filtering_in_port_list));

            assert(0 == int_result);

            int_result = memcmp(acl.filtering_out_port_list,
                                dup_acl_table[acl_index-1].filtering_out_port_list,
                                sizeof(acl.filtering_out_port_list));

            assert(0 == int_result);

        }
    }

    free(dup_class_inst_pool_buf);
    free(dup_rule_inst_pool_buf);
}

void
RULE_CTRL_UT_Proc_Validate_Leaks_ResourceInfo_ACL_QoS()
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T i;

    UI32_T                      unit;
    RULE_CTRL_ResourceInfo_T    *dup_resource_info = malloc(sizeof(shmem_data_p->resource_info));

    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_hash[RULE_TYPE_MAX_NBRS_OF_ACL];

    assert(NULL != dup_resource_info);

    memcpy(dup_resource_info, shmem_data_p->resource_info, sizeof(shmem_data_p->resource_info));

    memset(acl_inst_hash, 0, sizeof(acl_inst_hash));

    for (i = 0; i < _countof(shmem_data_p->if_table); ++ i)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        result = RULE_CTRL_Priv_IfEntry_Index_To_Interface(i, &interface);
        assert(RULE_TYPE_OK == result);

        {

            //
            // ACL
            //

            RULE_TYPE_AclType_T acl_type_ary[] =
            {
                RULE_TYPE_MAC_ACL,
                RULE_TYPE_IP_ACL,
                RULE_TYPE_IPV6_ACL
            };

            RULE_TYPE_AclType_T acl_type_idx;
            UI32_T ifindex;

            UI32_T policy_map_index;
//            UI32_T ui32_result;

            //
            // NOT support craft port now
            //
            if (RULE_TYPE_INTERFACE_CRAFT_PORT == interface.type)
            {
                continue;
            }

            result = RULE_CTRL_Interface_To_Ifindex(&interface, &ifindex);
            assert(RULE_TYPE_OK == result);

            for (acl_type_idx=0; acl_type_idx < _countof(acl_type_ary); ++acl_type_idx)
            {
                UI32_T                      acl_type = acl_type_ary[acl_type_idx];
                UI32_T                      acl_index;

                RULE_TYPE_COUNTER_ENABLE_T  counter_enable;

                UI16_T                      time_range_index;

                result = RULE_OM_GetPortAclIndex(ifindex, acl_type,
                                                 interface.direction == RULE_TYPE_INBOUND ? TRUE : FALSE,
                                                 &acl_index,
                                                 &time_range_index,
                                                 &counter_enable);
                if (RULE_TYPE_OK == result)
                {

//                    UI32_T precedence;

                    RULE_TYPE_Acl_T acl_entry;
                    RULE_TYPE_PortEntry_T port_entry;

                    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
                    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;
//                    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

                    result = RULE_OM_Get_PortEntryEx(ifindex,
                                                     interface.direction,
                                                     &port_entry);
                    assert(RULE_TYPE_OK == result);

                    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
                    assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
                    if (TRUE == acl_entry.flag_aggregate_done &&
                        0 != acl_entry.aggregate_acl_index)
                    {
                        acl_index = acl_entry.aggregate_acl_index;

                        result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
                        assert(RULE_TYPE_OK == result);
                    }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

//                    switch (acl_entry.acl_type)
//                    {
//                        case RULE_TYPE_MAC_ACL:
//                            precedence = INGRESS_MAC_ACL_PRECEDENCE;
//                            break;
//
//                        case RULE_TYPE_IP_ACL:
//                        case RULE_TYPE_IP_STD_ACL:
//                        case RULE_TYPE_IP_EXT_ACL:
//                            precedence = INGRESS_IP_ACL_PRECEDENCE;
//                            break;
//
//                        case RULE_TYPE_IPV6_ACL:
//                        case RULE_TYPE_IPV6_STD_ACL:
//                        case RULE_TYPE_IPV6_EXT_ACL:
//                            precedence = INGRESS_IPV6_ACL_PRECEDENCE;
//                            break;
//
//                        default:
//                            assert(0);
//                            break;
//                    }

                    acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                    assert(NULL != acl_inst_p);

                    if (RULE_TYPE_INBOUND == interface.direction)
                    {
                        #define HASH_GET(hash, key)                             \
                        ({                                                      \
                            UI32_T __i;                                         \
                            for (__i = 0; __i < _countof(hash); ++ __i) {       \
                                if (hash[__i] == key) {                         \
                                    break;                                      \
                                }                                               \
                            }                                                   \
                            (__i < _countof(hash)) ? 1 : 0;                     \
                        })

                        #define HASH_SET(hash, key)                             \
                        ({                                                      \
                            UI32_T __i;                                         \
                            for (__i = 0; __i < _countof(hash); ++ __i) {       \
                                if (hash[__i] == 0) {                           \
                                    hash[__i] = key;                            \
                                    break;                                      \
                                }                                               \
                            }                                                   \
                            1;                                                  \
                        })

                        //
                        // Check ref count, make sure we don't count the same rule
                        //
                        if (0 == HASH_GET(acl_inst_hash, acl_inst_p))
                        {
                            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;

                            //
                            // -= Duplicated code start
                            //
                            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);
                            acl_inst_obj.get_element_iterator(&acl_inst_obj, &ace_inst_iter);

                            for (ace_inst_iter.first(&ace_inst_iter);
                                 NULL != ace_inst_iter.get_instance(&ace_inst_iter);
                                 ace_inst_iter.next(&ace_inst_iter))
                            {
                                RULE_TYPE_CLASS_INSTANCE_PTR_T ace_inst_p;
                                RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;

                                RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

                                ace_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T) ace_inst_iter.get_instance(&ace_inst_iter);

                                RULE_CTRL_IN_Priv_ConstructClassInstanceObject(ace_inst_p, &ace_inst_obj);

                                ace_inst_obj.get_element_iterator(&ace_inst_obj, &rule_inst_iter);

                                for (rule_inst_iter.first(&rule_inst_iter);
                                     NULL != rule_inst_iter.get_instance(&rule_inst_iter);
                                     rule_inst_iter.next(&rule_inst_iter))
                                {
                                    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;
                                    RULE_CTRL_ResourceInfo_T      *res_p;

                                    rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T) rule_inst_iter.get_instance(&rule_inst_iter);

                                    res_p = RULE_CTRL_GetResourceInfo(rule_inst_p->dev_rule_info.unit,
                                                                      rule_inst_p->dev_rule_info.device,
                                                                      rule_inst_p->dev_rule_info.group_id);

                                    res_p->free_rule ++;
                                    assert(res_p->free_rule <= res_p->total_rule);
                                }
                            }
                            //
                            // -= Duplicated code end
                            //

                            HASH_SET(acl_inst_hash, acl_inst_p);
                        }

                        #undef HASH_GET
                        #undef HASH_SET

                    }
                    else if (RULE_TYPE_OUTBOUND == interface.direction)
                    {
                        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;
                        
                        //
                        // -= Duplicated code start
                        //
                        RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);
                        acl_inst_obj.get_element_iterator(&acl_inst_obj, &ace_inst_iter);

                        for (ace_inst_iter.first(&ace_inst_iter);
                             NULL != ace_inst_iter.get_instance(&ace_inst_iter);
                             ace_inst_iter.next(&ace_inst_iter))
                        {
                            RULE_TYPE_CLASS_INSTANCE_PTR_T ace_inst_p;
                            RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;

                            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

                            ace_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T) ace_inst_iter.get_instance(&ace_inst_iter);

                            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(ace_inst_p, &ace_inst_obj);

                            ace_inst_obj.get_element_iterator(&ace_inst_obj, &rule_inst_iter);

                            for (rule_inst_iter.first(&rule_inst_iter);
                                 NULL != rule_inst_iter.get_instance(&rule_inst_iter);
                                 rule_inst_iter.next(&rule_inst_iter))
                            {
                                RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;
                                RULE_CTRL_ResourceInfo_T      *res_p;

                                rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T) rule_inst_iter.get_instance(&rule_inst_iter);

                                res_p = RULE_CTRL_GetResourceInfo(rule_inst_p->dev_rule_info.unit,
                                                                  rule_inst_p->dev_rule_info.device,
                                                                  rule_inst_p->dev_rule_info.group_id);
                                
                                res_p->free_rule ++;
                                assert(res_p->free_rule <= res_p->total_rule);
                            }
                        }
                        //
                        // -= Duplicated code end
                        //
                    }
                }

            } // for (acl_type_idx


            //
            // QoS
            //
            result = RULE_OM_GetPortPolicyMapIndex(ifindex, interface.direction,
                                                   &policy_map_index);
            if (RULE_TYPE_OK == result)
            {
                RULE_TYPE_PortEntry_T   port_entry;
                RULE_TYPE_PolicyMap_T   policy_map_entry;

                UI16_T                  policy_index;

                result = RULE_OM_Get_PortEntryEx(ifindex,
                                                 interface.direction,
                                                 &port_entry);
                assert(RULE_TYPE_OK == result);

                result = RULE_OM_GetPolicyMapByIndex(policy_map_index,
                                                     &policy_map_entry);
                assert(RULE_TYPE_OK == result);

                for (policy_index = 0;
                     policy_index < policy_map_entry.element_count;
                     ++policy_index)
                {
                    RULE_TYPE_PolicyElement_T   policy_element;
                    RULE_TYPE_ClassMap_T        class_map_entry;

                    UI32_T                      policy_element_index;
                    UI32_T                      class_index;

                    policy_element_index = policy_map_entry.policy_map_element_index_list[policy_index];

                    result = RULE_OM_GetPolicyMapElement(policy_element_index,
                                                              &policy_element);
                    assert(RULE_TYPE_OK == result);

                    result = RULE_OM_GetClassMap(policy_element.class_map_index,
                                                 &class_map_entry);
                    assert(RULE_TYPE_OK == result);

                    if (0 == class_map_entry.row_status)
                    {
                        continue;
                    }

                    for (class_index = 0;
                         class_index < class_map_entry.class_element_count;
                         ++class_index)
                    {
                        RULE_TYPE_ClassType_T     class_type;
                        UI32_T                    class_id;

                        class_type = class_map_entry.class_type[ class_index ];
                        class_id   = class_map_entry.class_index_list[ class_index ];

                        if (RULE_TYPE_CLASS_ACL == class_type)
                        {
                            RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
                            RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;
                            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;

                            acl_inst_p = RULE_CTRL_UT_Proc_Get_QoS_Class_Instance(ifindex,
                                                                                  interface.direction,
                                                                                  policy_map_index,
                                                                                  policy_element.class_map_index,
                                                                                  class_type,
                                                                                  class_id);
                            assert(NULL != acl_inst_p);

                            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);
                            acl_inst_obj.get_element_iterator(&acl_inst_obj, &ace_inst_iter);

                            for (ace_inst_iter.first(&ace_inst_iter);
                                 NULL != ace_inst_iter.get_instance(&ace_inst_iter);
                                 ace_inst_iter.next(&ace_inst_iter))
                            {
                                RULE_TYPE_CLASS_INSTANCE_PTR_T ace_inst_p;
                                RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;
                                RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

                                ace_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T) ace_inst_iter.get_instance(&ace_inst_iter);

                                RULE_CTRL_IN_Priv_ConstructClassInstanceObject(ace_inst_p, &ace_inst_obj);
                                ace_inst_obj.get_element_iterator(&ace_inst_obj, &rule_inst_iter);

                                //
                                // -= Duplicated Start
                                //
                                for (rule_inst_iter.first(&rule_inst_iter);
                                     NULL != rule_inst_iter.get_instance(&rule_inst_iter);
                                     rule_inst_iter.next(&rule_inst_iter))
                                {
                                    RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;
                                    RULE_CTRL_ResourceInfo_T      *res_p;

                                    rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T) rule_inst_iter.get_instance(&rule_inst_iter);

                                    res_p = RULE_CTRL_GetResourceInfo(rule_inst_p->dev_rule_info.unit,
                                                                      rule_inst_p->dev_rule_info.device,
                                                                      rule_inst_p->dev_rule_info.group_id);

                                    res_p->free_rule ++;
                                    assert(res_p->free_rule <= res_p->total_rule);
                                }
                                //
                                // -= Duplicated End
                                //

                            }

                        }
                        else if (RULE_TYPE_CLASS_MF == class_type)
                        {
                            RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p;
                            RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_inst_obj;
                            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

                            class_inst_p = RULE_CTRL_UT_Proc_Get_QoS_Class_Instance(ifindex,
                                                                                    interface.direction,
                                                                                    policy_map_index,
                                                                                    policy_element.class_map_index,
                                                                                    class_type,
                                                                                    class_id);
                            assert(NULL != class_inst_p);

                            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p, &class_inst_obj);
                            class_inst_obj.get_element_iterator(&class_inst_obj, &rule_inst_iter);

                            //
                            // -= Duplicated Start
                            //
                            for (rule_inst_iter.first(&rule_inst_iter);
                                 NULL != rule_inst_iter.get_instance(&rule_inst_iter);
                                 rule_inst_iter.next(&rule_inst_iter))
                            {
                                RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;
                                RULE_CTRL_ResourceInfo_T      *res_p;

                                rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T) rule_inst_iter.get_instance(&rule_inst_iter);

                                res_p = RULE_CTRL_GetResourceInfo(rule_inst_p->dev_rule_info.unit,
                                                                  rule_inst_p->dev_rule_info.device,
                                                                  rule_inst_p->dev_rule_info.group_id);

                                res_p->free_rule ++;
                                assert(res_p->free_rule <= res_p->total_rule);
                            }
                            //
                            // -= Duplicated End
                            //

                        }
                        else
                        {
                            assert(0);
                        }

                    }
                }
            } // QoS

        } // for (direction
    } // for (ifindex

    for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
    {
        UI32_T device_id;

        for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; ++device_id)
        {
            UI32_T fun_type_ary[] =
            {
                //
                // ACL
                //
                RULE_TYPE_INGRESS_MAC_ACL,
                RULE_TYPE_INGRESS_IP_ACL,
                RULE_TYPE_INGRESS_IPV6_STD_ACL,
                RULE_TYPE_INGRESS_IPV6_EXT_ACL,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
                RULE_TYPE_EGRESS_MAC_ACL,
                RULE_TYPE_EGRESS_IP_ACL,
                RULE_TYPE_EGRESS_IPV6_STD_ACL,
                RULE_TYPE_EGRESS_IPV6_EXT_ACL,
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

                //
                // QoS
                //
                RULE_TYPE_MAC_DIFFSERV,
                RULE_TYPE_IP_DIFFSERV,
                RULE_TYPE_INGRESS_IPV6_STD_DIFFSERV,
                RULE_TYPE_INGRESS_IPV6_EXT_DIFFSERV,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
                RULE_TYPE_EGRESS_MAC_DIFFSERV,
                RULE_TYPE_EGRESS_IP_DIFFSERV,
                RULE_TYPE_EGRESS_IPV6_STD_DIFFSERV,
                RULE_TYPE_EGRESS_IPV6_EXT_DIFFSERV,
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

            };
            UI32_T fun_type_ary_index;

            for (fun_type_ary_index = 0; fun_type_ary_index < _countof(fun_type_ary); ++fun_type_ary_index)
            {
                UI32_T  group_id;
                int     rule_pri;

                BOOL_T  b_result;

                RULE_CTRL_ResourceInfo_T *res_p;

                b_result = RULE_CTRL_GetFunctionInfoByFunctionType(fun_type_ary[ fun_type_ary_index ], &group_id, &rule_pri);
                assert(TRUE == b_result);

                res_p = RULE_CTRL_GetResourceInfo(unit, device_id, group_id);
                assert(NULL != res_p);

                assert(res_p->free_rule == res_p->total_rule);
            }
        }
    }

    memcpy(shmem_data_p->resource_info, dup_resource_info, sizeof(shmem_data_p->resource_info));
    free(dup_resource_info);
}

void
RULE_CTRL_UT_Proc_Validate_Group_Qualify_Set()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)

    UI32_T i;

    for (i = 0; i < _countof(shmem_data_p->av_fun_type_tbl.av_vector); ++i)
    {
        RULE_CTRL_FunTypeFreeEntry_T *free_entry = &shmem_data_p->av_fun_type_tbl.av_vector[i];

        if (RULE_TYPE_FUNCTION_TYPE_MAX != free_entry->used_by_fun_type)
        {
            FP_CONFIG_FunctionInfo_T    *fun_p;
            FP_CONFIG_GroupInfo_T       *grp_p;

            FP_CONFIG_FunctionInfo_T    *remap_fun_p;
            FP_CONFIG_GroupInfo_T       *remap_grp_p;

            UI32_T unit = 1;
            UI32_T device_id;

            fun_p = FP_CONFIG_get_function_info_by_type(free_entry->used_by_fun_type);
            grp_p = FP_CONFIG_get_group_info_by_id(fun_p->group_id);

            remap_fun_p = FP_CONFIG_get_function_info_by_type(free_entry->fun_type);
            remap_grp_p = FP_CONFIG_get_group_info_by_id(remap_fun_p->group_id);

            for (device_id = 0; device_id < shmem_data_p->dev_info[unit-1].num_of_chips; ++device_id)
            {
                bcm_field_qset_t remap_qset = {0};

                bcm_field_group_get(device_id, remap_grp_p->group_id, &remap_qset);

                //
                // MAC => {IP, IPv6 std}
                // The qset of IP and IPv6 std should be configured as MAC's
                //
                // At the same time, maybe exist a mapping like IP => {MAP}
                // So the check should not compare qset of MAC and IP. It should
                // check the source FP_CONFIG.
                //
                assert(0 == memcmp(&remap_qset.w, grp_p->w, sizeof(grp_p->w)));
            }
        }
    }

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

}

void
RULE_CTRL_UT_Proc_Validate_Group_Priority()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)

    UI32_T unit = 1;

    RULE_TYPE_FunctionType_T    fun_type_1;
    RULE_TYPE_FunctionType_T    fun_type_2;

    // Suppose: the priority will be
    // CPU If > protocol > Diffserv > ACL (MAC > IP > IPv6)
    //
    // group.id -> group.priority
    //
    //  for each function type ftype_1
    //      for each function type ftype_2
    //
    //          ftype_1 is a remapped function type of ftype_1_s
    //          ftype_2 is a remapped function type of ftype_2_s
    //
    //          if ftype_1_s exist
    //              ftype_1_priority = group(ftype_1_s).id + 1
    //              hw_group_priority(ftype_1) == ftype_1_priority
    //          else
    //              ftype_1_priority = group(ftype_1).id + 1
    //
    //          if ftype_2_s exist
    //              ftype_2_priority = group(ftype_2_s).id + 1
    //              hw_group_priority(ftype_2) == ftype_2_priority
    //          else
    //              ftype_2_priority = group(ftype_2).id + 1
    //
    //          if ftype_1_s == ftype_2_s
    //              ftype_1_priority == ftype_2_priority
    //          else if ftype_1_priority < ftype_2_priority
    //              hw_group_priority(ftype_1) < hw_group_priority(ftype_2) and v.v.
    //
    //

    for (fun_type_1 = 0; fun_type_1 < RULE_TYPE_FUNCTION_TYPE_MAX; ++fun_type_1)
    {
        RULE_CTRL_FunTypeFreeEntry_T    *free_entry_1_p;

        RULE_TYPE_FunctionType_T        fun_type_1_s = RULE_TYPE_FUNCTION_TYPE_MAX;

        FP_CONFIG_FunctionInfo_T        *fun_1_p = NULL;
        FP_CONFIG_GroupInfo_T           *grp_1_p = NULL;

        FP_CONFIG_FunctionInfo_T        *fun_1_s_p = NULL;
        FP_CONFIG_GroupInfo_T           *grp_1_s_p = NULL;

        int                             fun_type_priority_1 = -1;

        fun_1_p = FP_CONFIG_get_function_info_by_type(fun_type_1);

        if (NULL == fun_1_p)
        {
            continue;
        }

        grp_1_p = FP_CONFIG_get_group_info_by_id(fun_1_p->group_id);
        assert(NULL != grp_1_p);

        if (grp_1_p->selector_stage == DEVRM_STAGE_EFP)
        {
            continue;
        }

        free_entry_1_p = RULE_CTRL_Priv_GetFunTypeFreeEntry(fun_type_1);
        if (NULL != free_entry_1_p)
        {
            fun_type_1_s = free_entry_1_p->used_by_fun_type;
        }

        if (RULE_TYPE_FUNCTION_TYPE_MAX != fun_type_1_s)
        {
            fun_1_s_p = FP_CONFIG_get_function_info_by_type(fun_type_1_s);
            assert(NULL != fun_1_s_p);
            grp_1_s_p = FP_CONFIG_get_group_info_by_id(fun_1_s_p->group_id);
            assert(NULL != grp_1_p);

            fun_type_priority_1 = grp_1_s_p->group_id + 1;
        }
        else
        {
            fun_type_priority_1 = grp_1_p->group_id + 1;
        }

        for (fun_type_2 = 0; fun_type_2 < RULE_TYPE_FUNCTION_TYPE_MAX; ++fun_type_2)
        {
            RULE_CTRL_FunTypeFreeEntry_T    *free_entry_2_p;

            RULE_TYPE_FunctionType_T        fun_type_2_s = RULE_TYPE_FUNCTION_TYPE_MAX;

            FP_CONFIG_FunctionInfo_T        *fun_2_p = NULL;
            FP_CONFIG_GroupInfo_T           *grp_2_p = NULL;

            FP_CONFIG_FunctionInfo_T        *fun_2_s_p = NULL;
            FP_CONFIG_GroupInfo_T           *grp_2_s_p = NULL;

            int                             fun_type_priority_2 = -1;

            UI32_T device_id;

            fun_2_p = FP_CONFIG_get_function_info_by_type(fun_type_2);

            if (NULL == fun_2_p)
            {
                continue;
            }

            grp_2_p = FP_CONFIG_get_group_info_by_id(fun_2_p->group_id);
            assert(NULL != grp_2_p);

            if (grp_2_p->selector_stage == DEVRM_STAGE_EFP)
            {
                continue;
            }

            free_entry_2_p = RULE_CTRL_Priv_GetFunTypeFreeEntry(fun_type_2);
            if (NULL != free_entry_2_p)
            {
                fun_type_2_s = free_entry_2_p->used_by_fun_type;
            }

            if (RULE_TYPE_FUNCTION_TYPE_MAX != fun_type_2_s)
            {
                fun_2_s_p = FP_CONFIG_get_function_info_by_type(fun_type_2_s);
                assert(NULL != fun_2_s_p);
                grp_2_s_p = FP_CONFIG_get_group_info_by_id(fun_2_s_p->group_id);
                assert(NULL != grp_2_p);

                fun_type_priority_2 = grp_2_s_p->group_id + 1;
            }
            else
            {
                fun_type_priority_2 = grp_2_p->group_id + 1;
            }


            for (device_id = 0; device_id < shmem_data_p->dev_info[unit-1].num_of_chips; ++device_id)
            {
                int hw_priority_1 = 0;
                int hw_priority_2 = 0;

                bcm_field_group_priority_get(device_id, grp_1_p->group_id, &hw_priority_1);
                bcm_field_group_priority_get(device_id, grp_2_p->group_id, &hw_priority_2);

                if (RULE_TYPE_FUNCTION_TYPE_MAX != fun_type_1_s)
                {
                    assert(fun_type_priority_1 == hw_priority_1);
                }

                if (RULE_TYPE_FUNCTION_TYPE_MAX != fun_type_2_s)
                {
                    assert(fun_type_priority_2 == hw_priority_2);
                }

                if ((RULE_TYPE_FUNCTION_TYPE_MAX != fun_type_1_s) &&
                    (fun_type_1_s == fun_type_2_s))
                {
                    assert(fun_type_priority_1 == fun_type_priority_2);
                }
                else if (fun_type_priority_1 < fun_type_priority_2)
                {
                    assert(hw_priority_1 < hw_priority_2);
                }
                else
                {
                    assert(hw_priority_2 <= hw_priority_2);
                }
            }
        }
    } // for (fun_type

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT
}

void
RULE_CTRL_UT_Proc_Validate_Group()
{
    RULE_CTRL_UT_Proc_Validate_Group_Qualify_Set();
    RULE_CTRL_UT_Proc_Validate_Group_Priority();
}

void
RULE_CTRL_UT_Proc_Validate_Leaks()
{
    RULE_CTRL_UT_Proc_Validate_Leaks_Instance();

    if (0)
    {
        RULE_CTRL_UT_Proc_Validate_Leaks_ResourceInfo_ACL_QoS();
    }
}

void
RULE_CTRL_UT_Proc_Validate_Rule()
{
    RULE_CTRL_UT_Proc_Validate_Group();

    if (0)
    {
        RULE_CTRL_UT_Proc_Validate_QoS();
        RULE_CTRL_UT_Proc_Validate_ACLs();
    }

    RULE_CTRL_UT_Proc_Validate_Leaks();

    DEVRM_UT_Proc_Validate();
}

/*------------------------------------------------------------------------------
 * Test Functions
 *------------------------------------------------------------------------------
 */

int
RULE_CTRL_UT_Setup()
{
    DEVRM_PMGR_FieldInit();

    RULE_OM_InitiateSystemResources();
    RULE_CTRL_InitiateSystemResources();

    RULE_CTRL_EnterTransitionMode();
    RULE_CTRL_EnterMasterMode();

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    SYS_TIME_AttachSystemResources();

    SYS_TIME_InitiateSystemResources();
    SYS_TIME_EnterMasterMode();
#endif // SYS_CPNT_TIME_BASED_ACL

    return 0;
}

int
RULE_CTRL_UT_Teardown()
{
    return 0;
}

int
RULE_CTRL_UT_Teardown_With_Validating_Rule()
{
    RULE_CTRL_UT_Proc_Validate_Rule();

    return RULE_CTRL_UT_Teardown();
}

/*------------------------------------------------------------------------------
 * Core Functions Test
 *------------------------------------------------------------------------------
 */

int
RULE_CTRL_UT_SelfTest_Create_Acl_And_Add_Ace()
{
    enum
    {
        MAX_ACE_IN_ACL = 5,
    };

    RULE_TYPE_AclType_T acl_type_ar[] =
    {
        RULE_TYPE_MAC_ACL,
        RULE_TYPE_IP_STD_ACL,
        RULE_TYPE_IP_EXT_ACL,
        RULE_TYPE_IPV6_STD_ACL,
        RULE_TYPE_IPV6_EXT_ACL,
    };

    const char *acl_name = "a1";

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;

    for (i = 0; i < _countof(acl_type_ar); ++ i)
    {
        RULE_CTRL_UT_COMPRESS_OPTION_T compress;

        for (compress = RULE_CTRL_UT_COMPRESS_DISABLE; compress <= RULE_CTRL_UT_COMPRESS_ENABLE; ++ compress)
        {
            RULE_TYPE_AclType_T acl_type = acl_type_ar[i];
            UI32_T acl_index;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name,
                                                     acl_type,
                                                     MAX_ACE_IN_ACL,
                                                     compress,
                                                     &acl_index);

            assert(MAX_ACE_IN_ACL == RULE_OM_LocalGetAclByIndex(acl_index)->ace_count);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
            {
                UI32_T agg_acl_index;
                RULE_TYPE_Acl_T *agg_acl_p;

                result = RULE_OM_RebuildAggregateAcl(acl_index);
                assert(RULE_TYPE_OK == result);

                agg_acl_index = RULE_OM_LocalGetAclByIndex(acl_index)->aggregate_acl_index;

                agg_acl_p = RULE_OM_LocalGetAclByIndex(agg_acl_index);

                if (RULE_CTRL_UT_COMPRESS_DISABLE == compress)
                {
                    assert(MAX_ACE_IN_ACL == agg_acl_p->ace_count);
                }
                else if (RULE_CTRL_UT_COMPRESS_ENABLE == compress)
                {
                    assert(agg_acl_p->ace_count < MAX_ACE_IN_ACL);
                }
            }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE
            
            RULE_OM_DelAcl(acl_index);
        }
    }

    return 0;
}

int
RULE_CTRL_UT_SelfTest_FP_Config_Initial()
{
    FP_CONFIG_FunctionInfo_T *fun_info_p;
    FP_CONFIG_GroupInfo_T    *grp_info_p;
    RULE_CTRL_ResourceInfo_T *res_p;
    UI32_T unit, device_id;
    UI32_T index;

    for (index = 0; index < FP_CONFIG_NumberOfGroup(); ++index)
    {
        _field_group_t *grp_p = NULL;
        bcm_field_qset_t qset;

        UI32_T max_rule_num;

        grp_info_p = FP_CONFIG_get_group_info_by_id(index);
        assert(NULL != grp_info_p);

        assert(grp_info_p->is_created == TRUE);

        memset(&qset, 0, sizeof(qset));
        memcpy(qset.w, grp_info_p->w, sizeof(qset.w));

        max_rule_num = grp_info_p->selector_count * SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR;
        if (grp_info_p->group_mode == DEVRM_GROUP_MODE_DOUBLE)
        {
            max_rule_num = max_rule_num / 2;
        }

        for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
        {
            grp_p = _bcm_esw_field_group_get(0, grp_info_p->group_id);

            assert (NULL != grp_p);
            assert (1 == grp_p->valid);
            assert(0 == memcmp(grp_p->qset.w, qset.w, sizeof(grp_p->qset.w)));

            for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; device_id++)
            {
                res_p = RULE_CTRL_GetResourceInfo(unit, device_id, grp_info_p->group_id);

                assert(res_p->total_rule == max_rule_num);
            }
        }
    }

    for (index = 0; index < FP_CONFIG_NumberOfFunctionType(); ++index)
    {
        RULE_CTRL_RuleQuota_T       *rule_quota_p;

        fun_info_p = FP_CONFIG_get_function_info_by_type(index);
        if (fun_info_p ==  NULL)
        {
            continue;
        }

        grp_info_p = FP_CONFIG_get_group_info_by_id(fun_info_p->group_id);
        assert(NULL != grp_info_p);
        assert(grp_info_p->is_created == TRUE);

        for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
        {
            for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; device_id++)
            {
                res_p = RULE_CTRL_GetResourceInfo(unit, device_id, grp_info_p->group_id);
                rule_quota_p = RULE_CTRL_GetRuleQuota(unit, device_id, index);

                assert(rule_quota_p->total_rule == fun_info_p->rule_quota);
                assert(rule_quota_p->total_rule <= res_p->total_rule);
            }
        }

    }

    return 0;
}

int
RULE_CTRL_UT_CPU_IPSG6_Deny_All_IP6_Packet()
{
#if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE)
    BOOL_T enable;
    UI32_T unit;
    UI32_T port;
    RULE_TYPE_INTERFACE_INFO_T interface;
    BOOL_T b_result;

    enable = TRUE;
    unit = 1;
    port = 2;

    interface.type = RULE_TYPE_INTERFACE_UPORT;
    interface.uport.unit = unit;
    interface.uport.port = port;
    b_result = RULE_CTRL_IPV6SG_DenyAnyIp6Packet(enable, &interface);
    assert(TRUE == b_result);

    {
        DEVRM_VirtualGroup_T        vir_grp;
        DEVRM_Rule_T                rule;
        int                         rule_prio;

        RULE_CTRL_PARAM             param;
        RULE_CTRL_IPV6SGDenyAnyIPRuleStorage_T *rs_p;
        UI32_T device_id;

        param.unit = interface.uport.unit;
        param.port = interface.uport.port;

        rs_p = RULE_CTRL_IPV6SG_GETOM_DenyAnyIp6Packet(&param);
        assert(NULL != rs_p);
        assert(TRUE == rs_p->rule_storage.is_enable);

        {
            UI32_T module_id;
            UI32_T phy_port;
            RULE_CTRL_IPV6SGDenyAnyIPRuleStorage_T *rs_2_p;

            b_result = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(param.unit,
                                                              param.port,
                                                              &module_id,
                                                              &device_id,
                                                              &phy_port);
            assert(TRUE == b_result);

            rs_2_p = &shmem_data_p->ipv6sg_deny_any_ip6[param.unit-1][device_id];

            assert(rs_p == rs_2_p);
        }

        b_result = DEVRM_UT_Proc_Get_Rule(device_id, rs_p->rule_storage.rule_id, &vir_grp, &rule, &rule_prio);
        assert(TRUE == b_result);
        assert(RULE_TYPE_IPV6_SG_DENY_ALL == vir_grp.vir_group_id);
    }

    /* remove
     */
    enable = FALSE;
    unit = 1;
    port = 2;

    interface.type = RULE_TYPE_INTERFACE_UPORT;
    interface.uport.unit = unit;
    interface.uport.port = port;
    b_result = RULE_CTRL_IPV6SG_DenyAnyIp6Packet(enable, &interface);
    assert(TRUE == b_result);

    {
        DEVRM_VirtualGroup_T        vir_grp;
        DEVRM_Rule_T                rule;
        int                         rule_prio;

        RULE_CTRL_PARAM             param;
        RULE_CTRL_IPV6SGDenyAnyIPRuleStorage_T *rs_p;
        UI32_T device_id;

        param.unit = interface.uport.unit;
        param.port = interface.uport.port;

        rs_p = RULE_CTRL_IPV6SG_GETOM_DenyAnyIp6Packet(&param);
        assert(NULL != rs_p);
        assert(FALSE == rs_p->rule_storage.is_enable);

        {
            UI32_T module_id;
            UI32_T phy_port;
            RULE_CTRL_IPV6SGDenyAnyIPRuleStorage_T *rs_2_p;

            b_result = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(param.unit,
                                                              param.port,
                                                              &module_id,
                                                              &device_id,
                                                              &phy_port);
            assert(TRUE == b_result);

            rs_2_p = &shmem_data_p->ipv6sg_deny_any_ip6[param.unit-1][device_id];

            assert(rs_p == rs_2_p);
        }

        b_result = DEVRM_UT_Proc_Get_Rule(device_id, rs_p->rule_storage.rule_id, &vir_grp, &rule, &rule_prio);
        assert(FALSE == b_result);
    }
#endif /* #if (SYS_CPNT_IPV6_SOURCE_GUARD == TRUE) */

    return 0;
}

static BOOL_T
RULE_CTRL_UT_Proc_GetRuleByTagName(
    char *tag,
    RULE_CTRL_PARAM *param_p)
{
    RULE_CTRL_FindRuleLookupTable_T find;
    RULE_CTRL_Filter_T filter;

    RULE_CTRL_InitFilter(&filter);

    memset(param_p, 0, sizeof(*param_p));

    memcpy(filter.tag, tag, strlen(tag));

    memset(&find, 0, sizeof(find));

    find.flag = RULE_CTRL_FIND_NEXT_EXISTED_RULE_ENTRY;
    find.unit = RULE_CTRL_INVALID;
    find.device_id  = RULE_CTRL_INVALID;
    find.group_id   = RULE_CTRL_INVALID;
    find.rule_id    = RULE_CTRL_INVALID;

    while (TRUE == RULE_CTRL_GetNextRuleEntryInLookupTable(&find))
    {
        RULE_CTRL_RuleLookupEntry_T *rl_p = find.existed_p;
        RULE_CTRL_PARAM param;

        if (FALSE == RULE_CTRL_GetParamFromLookupEntry(rl_p, &param))
        {
            continue;
        }

        /* filter by tags
         */
        assert(filter.tag[0] != '\0');

        {
            char dup_tag[RULE_CTRL_FILTER_MAX_NAME_STR_LEN + 1] = {0};
            char *p;
            char *tag;
            BOOL_T tag_matched = TRUE;

            strncpy(dup_tag, filter.tag, RULE_CTRL_FILTER_MAX_NAME_STR_LEN);

            tag = &dup_tag[0];

            do
            {
                p = strchr(tag, ';');
                if (p)
                {
                    *p = '\0';
                }

                if (FALSE == RULE_CTRL_IsTagMatch(&param, tag))
                {
                    tag_matched = FALSE;
                    break;
                }

                /* last tag
                 */
                if (NULL == p)
                {
                    break;
                }

                ++p;
                tag = p;

            } while (tag[0] != '\0');

            if (FALSE == tag_matched)
            {
                continue;
            }

        }

        /* rule field in device
         */
        memcpy(param_p, &param, sizeof(*param_p));
        return TRUE;
    }
    

    return FALSE;
}

static void
RULE_CTRL_UT_Validate_Permit_EAPoL_Packet()
{
    BOOL_T b_result;
    RULE_CTRL_PARAM param;
    DEVRM_AceEntry_T *ace_entry_p;
    UI8_T eapol_mac_addr[] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x03};
    UI8_T eapol_mac_mask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    b_result = RULE_CTRL_UT_Proc_GetRuleByTagName("1x-permit-eapol", &param);
    assert(TRUE == b_result);

    ace_entry_p = &param.ace_entry;

    assert(DEVRM_SHR_BITGET(ace_entry_p->w ,DEVRM_FIELD_QUALIFY_DstMac));
    assert(0 == memcmp(ace_entry_p->dstmac_data, eapol_mac_addr, sizeof(ace_entry_p->dstmac_data)));
    assert(0 == memcmp(ace_entry_p->dstmac_mask, eapol_mac_mask, sizeof(ace_entry_p->dstmac_mask)));

    assert(DEVRM_SHR_BITGET(ace_entry_p->w ,DEVRM_FIELD_QUALIFY_EtherType));
    assert(RULE_CTRL_ETHER_TYPE_EAP == ace_entry_p->ethertype_data);
    assert(0xffff == ace_entry_p->ethertype_mask);

    assert(param.action_entries[0].action == bcmFieldActionDropCancel);
    assert(param.action_entries[0].next_action == NULL);

    return ;
}


int
RULE_CTRL_UT_Set_All_Global_And_Dflt_Rule()
{
//    RULE_TYPE_RETURN_TYPE_T result;
//
//    UI32_T unit = 1;
//    UI32_T port = 3;

    RULE_TYPE_CpuRuleInfo_T info;

    BOOL_T is_enable;
    BOOL_T b_result;

    is_enable = TRUE;

again:

    info.common.flood = FALSE;
    info.common.to_cpu = TRUE;

    b_result = RULE_CTRL_TrapPacket2Cpu(is_enable, RULE_TYPE_PacketType_RIP, &info);
    assert(TRUE == b_result);

#if (SYS_CPNT_UDLD == TRUE)
    b_result = RULE_CTRL_TrapPacket2Cpu(is_enable, RULE_TYPE_PacketType_ORG_SPECIFIC2, &info);
    assert(TRUE == b_result);
#endif // SYS_CPNT_UDLD

#if (SYS_CPNT_DOT1X_EAPOL_PASS_THROUGH_BY_RULE == TRUE)
    b_result = RULE_CTRL_1X_SetEapolPassThrough(is_enable);
    assert(TRUE == b_result);
    if (is_enable)
    {
        RULE_CTRL_UT_Validate_Permit_EAPoL_Packet();
    }

//    {
//        RULE_CTRL_Filter_T filter;
//        RULE_CTRL_InitFilter(&filter);
//          memcpy(filter.tag, "1x-permit-eapol", strlen("1x-permit-eapol"));
//          RULE_CTRL_DumpRule(&filter, printf);
//
//    }

#endif /* SYS_CPNT_DOT1X_EAPOL_PASS_THROUGH_BY_RULE */

    if (TRUE == is_enable)
    {
        is_enable = FALSE;
        goto again;
    }

    return 0;
}

int
RULE_CTRL_UT_Apply_ACL()
{
    enum
    {
        MAX_ACE  = 5,

        MIN_DIRECTION = RULE_TYPE_INBOUND,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION = RULE_TYPE_INBOUND,
#endif
    };

    struct
    {
        RULE_TYPE_AclType_T acl_type;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL
        },
        {
            RULE_TYPE_IP_STD_ACL
        },
        {
            RULE_TYPE_IP_EXT_ACL
        },
        {
            RULE_TYPE_IPV6_STD_ACL
        },
        {
            RULE_TYPE_IPV6_EXT_ACL
        },
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i, j;

    UI32_T port_list[] =
    {
        1,
        2,
        3,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,
    };

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[30];
        UI32_T acl_index;

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acls[i].acl_type,
                                                 MAX_ACE, RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &acl_index);

        for (j = 0; j < _countof(port_list); ++ j)
        {
            UI32_T ifindex = port_list[j];

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            for (interface.direction = MIN_DIRECTION; interface.direction <= MAX_DIRECTION; ++ interface.direction)
            {
                UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

                result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, interface.direction,
                                                    acl_index,
                                                    time_range_index,
                                                    counter_enable);
                assert(RULE_TYPE_OK == result);
            }
        }

        for (j = 0; j < _countof(port_list); ++ j)
        {
            UI32_T ifindex = port_list[j];

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            for (interface.direction = MIN_DIRECTION; interface.direction <= MAX_DIRECTION; ++ interface.direction)
            {
                UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

                result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex, interface.direction,
                                                      acl_index,
                                                      time_range_index,
                                                      counter_enable);
                assert(RULE_TYPE_OK == result);
            }
        }

        RULE_OM_DestroyAcl(acl_index);
    }

    return 0;
}

int
RULE_CTRL_UT_Apply_ACL_With_Counter_Enabled()
{
    enum
    {
        MAX_ACE  = 5,

        MIN_DIRECTION = RULE_TYPE_INBOUND,

//#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
//        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
//#else
        MAX_DIRECTION = RULE_TYPE_INBOUND,
//#endif
    };

    struct
    {
        RULE_TYPE_AclType_T acl_type;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL
        },
//        {
//            RULE_TYPE_IP_EXT_ACL
//        },
//        {
//            RULE_TYPE_IPV6_EXT_ACL
//        },
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i, j;

    UI32_T port_list[] =
    {
        1,
        2,
        3,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,
    };

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[30];
        UI32_T acl_index;

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acls[i].acl_type,
                                                 MAX_ACE, RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &acl_index);

        for (j = 0; j < _countof(port_list); ++ j)
        {
            UI32_T ifindex = port_list[j];

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            for (interface.direction = MIN_DIRECTION; interface.direction <= MAX_DIRECTION; ++ interface.direction)
            {
                UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = TRUE;

                result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, interface.direction,
                                                    acl_index,
                                                    time_range_index,
                                                    counter_enable);
                assert(RULE_TYPE_OK == result);

                //
                //
                //
                {
                    RULE_TYPE_AclType_T ace_type;
                    UI32_T ace_index;
                    RULE_TYPE_Ace_Entry_T ace_entry;

                    ace_index = 0;
                    while (RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry))
                    {
                        UI32_T cx;
                        RULE_TYPE_RULE_INSTANCE_T rule_inst;

                        for (cx = 0; cx < _countof(rule_inst.counter); ++ cx)
                        {
                            result = RULE_CTRL_GetAclRuleCounter(ifindex, interface.direction,
                                                                 acl_index,
                                                                 ace_index, &ace_entry);
                            assert(RULE_TYPE_OK == result);
                        }
                    }

                    ace_index = 0;
                    while (RULE_TYPE_OK == RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry))
                    {
                        result = RULE_CTRL_ClearAclRuleCounter(ifindex, interface.direction,
                                                               acl_index,
                                                               ace_index);
                        assert(RULE_TYPE_OK == result);
                    }
                }

            }
        }
    }
    
    return 0;
}

int
RULE_CTRL_UT_Apply_All_Type_Of_ACLs_On_Port_At_The_Same_Time()
{
    enum
    {
        MAX_ACE  = 5,

        MIN_DIRECTION = RULE_TYPE_INBOUND,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION = RULE_TYPE_INBOUND,
#endif
    };

    struct
    {
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL
        },
        {
            RULE_TYPE_IP_EXT_ACL
        },
        {
            RULE_TYPE_IPV6_EXT_ACL
        },
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i, j;

    UI32_T port_list[] =
    {
        1,
        2,
        3,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,
    };

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[30];
        UI32_T acl_index;

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acls[i].acl_type,
                                                 MAX_ACE, RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &acl_index);
        acls[i].acl_index = acl_index;

        for (j = 0; j < _countof(port_list); ++ j)
        {
            UI32_T ifindex = port_list[j];

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            for (interface.direction = MIN_DIRECTION; interface.direction <= MAX_DIRECTION; ++ interface.direction)
            {
                UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

                result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, interface.direction,
                                                    acl_index,
                                                    time_range_index,
                                                    counter_enable);
                assert(RULE_TYPE_OK == result);
            }
        }
    }

    for (i = 0; i < _countof(acls); ++ i)
    {
        UI32_T acl_index = acls[i].acl_index;

        for (j = 0; j < _countof(port_list); ++ j)
        {
            UI32_T ifindex = port_list[j];

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            for (interface.direction = MIN_DIRECTION; interface.direction <= MAX_DIRECTION; ++ interface.direction)
            {
                UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

                result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex, interface.direction,
                                                      acl_index,
                                                      time_range_index,
                                                      counter_enable);
                assert(RULE_TYPE_OK == result);
            }
        }
    }

    return 0;
}

#if 0 // FIXME: time range
int
RULE_CTRL_UT_Cant_Share_ACL_Due_To_Time_Range_Enabled()
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)    
    enum
    {
        MAX_ACE  = 5,

        MIN_IFINDEX = 1,
        MAX_IFINDEX = 5,
        
        MIN_DIRECTION = RULE_TYPE_INBOUND,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION = RULE_TYPE_INBOUND,
#endif
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ifindex;

    RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
    const char *acl_name = "a1";
    UI32_T acl_index;

    UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;

    RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acl_type,
                                             MAX_ACE, RULE_CTRL_UT_COMPRESS_DEFAULT,
                                             &acl_index);

    {
        const char *time_range_name = "time-range";

        result = RULE_OM_CreateTimeRangeEntry(time_range_name, &time_range_index);
        assert(RULE_TYPE_OK == result);
    }

    for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

        for (interface.direction = MIN_DIRECTION; interface.direction <= MAX_DIRECTION; ++ interface.direction)
        {
            UI16_T _tm_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

            switch (ifindex % 5) {
                case 3:
                    _tm_index = time_range_index;
                    break;

                case 4:
                    _tm_index = time_range_index;
                    break;

                default:
                    break;
            }

            result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, interface.direction,
                                                acl_index,
                                                time_range_index,
                                                counter_enable);
            assert(RULE_TYPE_OK == result);
        }
    }

    for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

        for (interface.direction = MIN_DIRECTION; interface.direction <= MAX_DIRECTION; ++ interface.direction)
        {
            UI16_T _tm_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

            result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex, interface.direction,
                                                  acl_index,
                                                  _tm_index,
                                                  counter_enable);
            assert(RULE_TYPE_OK == result);
        }
    }
#endif // SYS_CPNT_TIME_BASED_ACL
    
    return 0;
}
#endif /* 0 */

int
RULE_CTRL_UT_Capacity_Test_Max_ACE_In_One_ACL_Ideal()
{
    enum
    {
        ACL_TYPE = RULE_TYPE_MAC_ACL,
        ACE_TYPE = RULE_TYPE_MAC_ACL,
        MIN_DIRECTION = RULE_TYPE_INBOUND,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION = RULE_TYPE_INBOUND,
#endif
    };

    const char                  *acl_name = "a1";
    RULE_TYPE_AclType_T         acl_type = ACL_TYPE;

    UI32_T                      MAX_ACE;
    UI32_T                      ace_count;
    UI32_T                      acl_index;

    RULE_TYPE_TCAMUtilization_T tcam;

    RULE_TYPE_RETURN_TYPE_T     result;

    RULE_CTRL_GetTCAMUtilization(&tcam);

    MAX_ACE = tcam.pce_free < SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR ? tcam.pce_free : SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR;

    RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acl_type,
                                             SYS_ADPT_MAX_NBRS_OF_ACE,
                                             RULE_CTRL_UT_COMPRESS_ENABLE,
                                             &acl_index);

    assert(SYS_ADPT_MAX_NBRS_OF_ACE == RULE_OM_LocalGetAclByIndex(acl_index)->ace_count);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    result = RULE_OM_RebuildAggregateAcl(acl_index);
    assert(RULE_TYPE_OK == result);

    acl_index = RULE_OM_LocalGetAclByIndex(acl_index)->aggregate_acl_index;

    //
    // update the final count by compression
    //
    {
        ace_count = RULE_OM_LocalGetAclByIndex(acl_index)->ace_count;
        assert(ace_count <=  SYS_ADPT_MAX_NBRS_OF_ACE);
    }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        UI32_T ifindex = 2;

        UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

        memset(&interface, 0, sizeof(interface));
        result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

        for (interface.direction = MIN_DIRECTION; interface.direction <= MAX_DIRECTION; ++ interface.direction)
        {
            result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, interface.direction,
                                                acl_index,
                                                time_range_index,
                                                counter_enable);

            if (tcam.pce_free < RULE_OM_LocalGetAclByIndex(acl_index)->ace_count)
            {
                assert(RULE_TYPE_OK != result);
            }
            else
            {
                assert(RULE_TYPE_OK == result);

                {
                    RULE_TYPE_CLASS_INSTANCE_PTR_T              class_inst_p;
                    RULE_TYPE_CLASS_INSTANCE_OBJECT_T           class_inst_obj;
                    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

                    class_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                    assert(NULL != class_inst_p);

                    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p, &class_inst_obj);
                    class_inst_obj.get_element_iterator(&class_inst_obj, &rule_inst_iter);

                    assert(acl_index == class_inst_p->acl_id);

                    assert(RULE_TYPE_INST_ACL == class_inst_p->super.type);
                    assert(ace_count == rule_inst_iter.count(&rule_inst_iter));
                }
            }
        }
    }

    return 0;
}

int
RULE_CTRL_UT_Capacity_Test_Max_ACE_In_One_ACL_Worst()
{
    enum
    {
        ACL_TYPE = RULE_TYPE_MAC_ACL,
        ACE_TYPE = RULE_TYPE_MAC_ACL,
    };

    const char                  *acl_name = "a1";
    RULE_TYPE_AclType_T         acl_type = ACL_TYPE;
    RULE_TYPE_AclType_T         ace_type = ACE_TYPE;

    RULE_TYPE_Ace_Entry_T       ace_entry;

    UI32_T                      MAX_ACE;
    UI32_T                      ace_count;
    UI32_T                      acl_index;

    RULE_TYPE_TCAMUtilization_T tcam;

    RULE_TYPE_RETURN_TYPE_T     result;

    RULE_CTRL_GetTCAMUtilization(&tcam);

    MAX_ACE = tcam.pce_free < SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR ? tcam.pce_free : SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR;

    RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

    ace_count = 0;

    while (TRUE)
    {
        UI8_T src_mac[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T src_mask[SYS_ADPT_MAC_ADDR_LEN];

        int i, j;

        ace_entry.ace_type = ace_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.access = (ace_count%2) ?
                           RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;

        memset(src_mac, 0, sizeof(src_mac));
        memset(src_mask, 0xff, sizeof(src_mask));

        assert(SYS_ADPT_MAX_NBRS_OF_ACE < (255*255));

        i = (ace_count+1) / 255;
        j = (ace_count+1) % 255;

        src_mac[ 4 ] = i;
        src_mac[ 5 ] = j;

        RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac, src_mask);

        result = RULE_OM_AddAce2Acl(acl_index, ace_type, &ace_entry);
        if (result != RULE_TYPE_OK)
        {
            break;
        }

        ace_count ++;
    }

    assert(SYS_ADPT_MAX_NBRS_OF_ACE == ace_count);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    result = RULE_OM_RebuildAggregateAcl(acl_index);
    assert(RULE_TYPE_OK == result);

    acl_index = RULE_OM_LocalGetAclByIndex(acl_index)->aggregate_acl_index;

    ace_count = RULE_OM_LocalGetAclByIndex(acl_index)->ace_count;

    assert(SYS_ADPT_MAX_NBRS_OF_ACE == ace_count);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        UI32_T ifindex = 2;
        RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

        UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

        memset(&interface, 0, sizeof(interface));
        result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
        assert(RULE_TYPE_OK == result);
        interface.direction = direction;

        result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                            acl_index,
                                            time_range_index,
                                            counter_enable);

        if (MAX_ACE < RULE_OM_LocalGetAclByIndex(acl_index)->ace_count)
        {
            assert(RULE_TYPE_OK != result);
        }
        else
        {
            assert(RULE_TYPE_OK == result);

            {
                RULE_TYPE_CLASS_INSTANCE_PTR_T              class_inst_p;
                RULE_TYPE_CLASS_INSTANCE_OBJECT_T           class_inst_obj;
                RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

                class_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                assert(NULL != class_inst_p);

                assert(acl_index == class_inst_p->acl_id);
                assert(RULE_TYPE_INST_ACL == class_inst_p->super.type);

                RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p, &class_inst_obj);
                class_inst_obj.get_element_iterator(&class_inst_obj, &rule_inst_iter);

                assert(ace_count == rule_inst_iter.count(&rule_inst_iter));
            }
        }
    }

    return 0;
}

int
RULE_CTRL_UT_Capacity_Test_Max_ACE_In_One_ACL_Normal()
{
    enum
    {
        ACL_TYPE = RULE_TYPE_MAC_ACL,
        ACE_TYPE = RULE_TYPE_MAC_ACL,
        MAX_UNIT = 1,
        MAX_PORT = 12,
        MAX_ACE  = SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR,
        RULE_SIZE = 2,
    };

    const char              *acl_name = "a1";
    RULE_TYPE_AclType_T     acl_type = ACL_TYPE;
    RULE_TYPE_AclType_T     ace_type = ACE_TYPE;

    RULE_TYPE_Ace_Entry_T   ace_entry;

    UI32_T                  ace_count;
    UI32_T                  acl_index;

    UI32_T                  unit;
    UI32_T                  port;

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

    ace_count = 0;

    while (TRUE)
    {
        UI8_T src_mac[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T src_mask[SYS_ADPT_MAC_ADDR_LEN];

        int i, j;

        ace_entry.ace_type = ace_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.access = (ace_count%2) ?
                           RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;

        memset(src_mac, 0, sizeof(src_mac));
        memset(src_mask, 0xff, sizeof(src_mask));

        assert(SYS_ADPT_MAX_NBRS_OF_ACE < (255*255));

        i = (ace_count+1) / 255;
        j = (ace_count+1) % 255;

        src_mac[ 4 ] = i;
        src_mac[ 5 ] = j;

        RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac, src_mask);

        result = RULE_OM_AddAce2Acl(acl_index, ace_type, &ace_entry);
        if (result != RULE_TYPE_OK)
        {
            break;
        }

        ace_count ++;

        if (MAX_ACE == ace_count)
        {
            break;
        }
    }

    assert(MAX_ACE == ace_count);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    result = RULE_OM_RebuildAggregateAcl(acl_index);
    assert(RULE_TYPE_OK == result);

    acl_index = RULE_OM_LocalGetAclByIndex(acl_index)->aggregate_acl_index;

    ace_count = RULE_OM_LocalGetAclByIndex(acl_index)->ace_count;

    assert(MAX_ACE == ace_count);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++ port)
        {
            UI32_T ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);
            RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

            RULE_TYPE_INTERFACE_INFO_T interface;

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

            memset(&interface, 0, sizeof(interface));
            result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
            assert(RULE_TYPE_OK == result);
            interface.direction = direction;

            result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                acl_index,
                                                time_range_index,
                                                counter_enable);
            assert(RULE_TYPE_OK == result);

            {
                RULE_TYPE_CLASS_INSTANCE_PTR_T              class_inst_p;
                RULE_TYPE_CLASS_INSTANCE_OBJECT_T           class_inst_obj;
                RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

                class_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                assert(NULL != class_inst_p);

                assert(acl_index == class_inst_p->acl_id);
                assert(RULE_TYPE_INST_ACL == class_inst_p->super.type);

                RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p, &class_inst_obj);
                class_inst_obj.get_element_iterator(&class_inst_obj, &rule_inst_iter);

                assert(ace_count == rule_inst_iter.count(&rule_inst_iter));
            }

            RULE_CTRL_UT_Proc_Validate_Rule();

        }
    }

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++ port)
        {
            UI32_T ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);
            RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

            result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex, direction,
                                                  acl_index,
                                                  time_range_index,
                                                  counter_enable);
            assert(RULE_TYPE_OK == result);
        }
    }

    return 0;
}

int
RULE_CTRL_UT_Capacity_Test_Max_ACE_In_One_ACL_Modify_On_Fly()
{
    enum
    {
        ACL_TYPE  = RULE_TYPE_MAC_ACL,
        ACE_TYPE  = RULE_TYPE_MAC_ACL,
        MIN_IFINDEX = 1,
        MAX_IFINDEX = 10,
        DIRECTION = RULE_TYPE_INBOUND,
        MAX_ACE   = SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR,
    };

    const char              *acl_name = "a1";
    RULE_TYPE_AclType_T     acl_type = ACL_TYPE;
    RULE_TYPE_AclType_T     ace_type = ACE_TYPE;

    RULE_TYPE_Ace_Entry_T   ace_entry;

    UI32_T                  ace_count;
    UI32_T                  acl_index;

    UI32_T                  ifindex;

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

    ace_count = 0;

    {
        UI8_T src_mac[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T src_mask[SYS_ADPT_MAC_ADDR_LEN];

        int i, j;

        ace_entry.ace_type = ace_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.access = (ace_count%2) ?
                           RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;

        memset(src_mac, 0, sizeof(src_mac));
        memset(src_mask, 0xff, sizeof(src_mask));

        assert(SYS_ADPT_MAX_NBRS_OF_ACE < (255*255));

        i = (ace_count+1) / 255;
        j = (ace_count+1) % 255;

        src_mac[ 4 ] = i;
        src_mac[ 5 ] = j;

        RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac, src_mask);

        result = RULE_OM_AddAce2Acl(acl_index, ace_type, &ace_entry);

        assert(RULE_TYPE_OK == result);
        ace_count ++;
    }

    assert(1 == ace_count);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    result = RULE_OM_RebuildAggregateAcl(acl_index);
    assert(RULE_TYPE_OK == result);

    acl_index = RULE_OM_LocalGetAclByIndex(acl_index)->aggregate_acl_index;

    ace_count = RULE_OM_LocalGetAclByIndex(acl_index)->ace_count;

    assert(1 == ace_count);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

    for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

        result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
        assert(RULE_TYPE_OK == result);
        interface.direction = DIRECTION;

        result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, DIRECTION,
                                            acl_index,
                                            time_range_index,
                                            counter_enable);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_CLASS_INSTANCE_PTR_T              class_inst_p;
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T           class_inst_obj;
            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T rule_inst_iter;

            class_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
            assert(NULL != class_inst_p);

            assert(acl_index == class_inst_p->acl_id);
            assert(RULE_TYPE_INST_ACL == class_inst_p->super.type);

            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(class_inst_p, &class_inst_obj);
            class_inst_obj.get_element_iterator(&class_inst_obj, &rule_inst_iter);

            assert(ace_count == rule_inst_iter.count(&rule_inst_iter));
        }

        RULE_CTRL_UT_Proc_Validate_Rule();
    }

    //
    // Add on Fly
    //
    for (ace_count = 1; ace_count <= MAX_ACE; ++ace_count)
    {
        BOOL_T is_add = TRUE;

        UI8_T src_mac[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T src_mask[SYS_ADPT_MAC_ADDR_LEN];

        int i, j;

        //
        //
        ace_entry.ace_type = ace_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.access = (ace_count%2) ?
                           RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;

        memset(src_mac, 0, sizeof(src_mac));
        memset(src_mask, 0xff, sizeof(src_mask));

        assert(SYS_ADPT_MAX_NBRS_OF_ACE < (255*255));

        i = (ace_count+1) / 255;
        j = (ace_count+1) % 255;

        src_mac[ 4 ] = i;
        src_mac[ 5 ] = j;

        RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac, src_mask);

        result = RULE_OM_AddAce2Acl(acl_index, ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        //
        //

        result = RULE_CTRL_SetAce(acl_index, &ace_entry, is_add);

        if (ace_count == MAX_ACE)
        {
            assert(RULE_TYPE_OK != result);

            result = RULE_OM_DelAceFromAcl(acl_index, ace_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }
        else
        {
            assert(RULE_TYPE_OK == result);
        }

        RULE_CTRL_UT_Proc_Validate_Rule();
    }

    //
    // Del on Fly
    //
    for (ace_count = 1; ace_count <= (MAX_ACE-1); ++ace_count)
    {
        BOOL_T is_add = FALSE;

        UI8_T src_mac[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T src_mask[SYS_ADPT_MAC_ADDR_LEN];

        int i, j;

        //
        //
        ace_entry.ace_type = ace_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.access = (ace_count%2) ?
                           RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;

        memset(src_mac, 0, sizeof(src_mac));
        memset(src_mask, 0xff, sizeof(src_mask));

        assert(SYS_ADPT_MAX_NBRS_OF_ACE < (255*255));

        i = (ace_count+1) / 255;
        j = (ace_count+1) % 255;

        src_mac[ 4 ] = i;
        src_mac[ 5 ] = j;

        RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac, src_mask);

        //
        //

        result = RULE_CTRL_SetAce(acl_index, &ace_entry, is_add);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_DelAceFromAcl(acl_index, ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();
    }

    //
    // Unbind all ACL
    //

    for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
    {
        UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

        result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex, DIRECTION,
                                              acl_index,
                                              time_range_index,
                                              counter_enable);
        assert(RULE_TYPE_OK == result);
    }

    return 0;
}

int
RULE_CTRL_UT_Capacity_Test_Policy_Map_With_Max_ACE_In_One_ACL()
{
#define __rc_min(a,b) ( ((a) < (b)) ? (a) : (b) )

    enum
    {
        MAX_UNIT            = 1,
        MAX_PORT            = 28,
        MAX_DIRECTION       = RULE_TYPE_INBOUND,

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,
        MAX_POLICY_ELEMENT  = 1,
        MATCH_TYPE          = RULE_TYPE_CLASS_MAP_MATCH_ANY,
    };


    UI32_T                  MAX_ACE;
    UI32_T                  MAX_PERMIT_ACE;

    UI32_T                  unit;
    UI32_T                  port;
    UI32_T                  direction;

    const char              *acl_name = "a1";
    RULE_TYPE_AclType_T     acl_type = ACL_TYPE;
    RULE_TYPE_AclType_T     ace_type = ACE_TYPE;

    const char              *policy_name = "p1";
    UI32_T                  policy_map_index;
    UI32_T                  class_id;

    UI32_T                  ace_count;
    RULE_TYPE_Ace_Entry_T   ace_entry;

    UI32_T                  acl_index;

    UI32_T                  result;

    //
    // If the ACL is used to bind with a policy-map,
    // all ACE in the ACL MUST not be compress due to
    // metering. So we must make sure all rule of ACL
    // can be save into class instance. Then it should
    // success to bind policy into a port.
    //
    MAX_ACE         = __rc_min(SYS_ADPT_MAX_NBRS_OF_ACE, 2);
    MAX_PERMIT_ACE  = MAX_ACE;


    //
    // ACL
    //
    RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

    for (ace_count = 0; ace_count < MAX_ACE; ++ace_count)
    {
        UI8_T src_mac[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T src_mask[SYS_ADPT_MAC_ADDR_LEN];

        int i, j, k;

        ace_entry.ace_type = ace_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.access = RULE_TYPE_ACE_PERMIT;

        memset(src_mac, 0, sizeof(src_mac));
        memset(src_mask, 0xff, sizeof(src_mask));

        assert(SYS_ADPT_MAX_NBRS_OF_ACE < (255*255));

        i = (ace_count+1) / 255;
        j = (ace_count+1) % 255;

        src_mac[ 4 ] = i;
        src_mac[ 5 ] = j;

        //
        // Make sure each rule in ACL will not be compressed
        //
        k = (ace_count) % 3;

        src_mac[ k ] = (k+1);

        RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac, src_mask);

        result = RULE_OM_AddAce2Acl(acl_index, ace_type, &ace_entry);
        assert(result == RULE_TYPE_OK);
    }

    //
    // Policy-map
    //
    result = RULE_OM_Create_PolicyMap(policy_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (class_id = 1; class_id <= MAX_POLICY_ELEMENT; ++class_id)
    {
        char class_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        RULE_TYPE_MatchType_T match_type = MATCH_TYPE;

        UI32_T class_map_index;
        UI32_T class_index;

        RULE_TYPE_UI_ClassMapElement_T class_element;

        snprintf(class_name, sizeof(class_name), "c%lu", class_id);

        result = RULE_OM_Create_ClassMap(class_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);


        class_element.class_type = RULE_TYPE_CLASS_ACL;
        strncpy(class_element.element.acl_name, acl_name,
                sizeof(class_element.element.acl_name)-1);
        class_element.element.acl_name[sizeof(class_element.element.acl_name)-1] = '\0';


        //
        // ACL
        //
        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index,
                                                         &class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index,
                                                           class_map_index);
        assert(RULE_TYPE_OK == result);
    }

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++port)
        {
            UI32_T ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                RULE_CTRL_UT_Proc_Bind_Policy_Map(ifindex, direction, policy_map_index);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++port)
        {
            UI32_T ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                RULE_CTRL_UT_Proc_UnBind_Policy_Map(ifindex, direction, policy_map_index);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    return 0;

#undef __rc_min
}

int
RULE_CTRL_UT_Set_QoS_With_MF_IPv6_Ext_ACL()
{
    enum
    {
        MAX_UNIT            = 1,
        MAX_PORT            = 2,

        MIN_DIRECTION       = RULE_TYPE_OUTBOUND,
        MAX_DIRECTION       = RULE_TYPE_OUTBOUND,

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,

        MAX_POLICY_ELEMENT  = 5,
        MATCH_TYPE          = RULE_TYPE_CLASS_MAP_MATCH_ANY,
    };

    UI32_T                  unit;
    UI32_T                  port;
    UI32_T                  direction;

    const char              *policy_map_name = "p1";
    UI32_T                  policy_map_index;

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UT_Proc_Create_Policy_Class_Map_With_Match_ACL_2(policy_map_name, RULE_TYPE_IPV6_EXT_ACL);
    result = RULE_OM_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++port)
        {
            UI32_T ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++direction)
            {
                RULE_CTRL_UT_Proc_Bind_Policy_Map(ifindex, direction, policy_map_index);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++port)
        {
            UI32_T ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++direction)
            {
                RULE_CTRL_UT_Proc_UnBind_Policy_Map(ifindex, direction, policy_map_index);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    return 0;
}

int
RULE_CTRL_UT_Set_QoS_With_MF_ACL_Mixed()
{
    enum
    {
        MAX_UNIT            = 1,
        MAX_PORT            = 2,
        MAX_DIRECTION       = RULE_TYPE_INBOUND,

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,

        MAX_POLICY_ELEMENT  = 5,
        MATCH_TYPE          = RULE_TYPE_CLASS_MAP_MATCH_ANY,
    };

    UI32_T                  unit;
    UI32_T                  port;
    UI32_T                  direction;

    const char              *policy_name = "p1";
    UI32_T                  policy_map_index;
    UI32_T                  class_id;

    RULE_TYPE_RETURN_TYPE_T result;

    //
    // Policy-map
    //
    result = RULE_OM_Create_PolicyMap(policy_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (class_id = 1; class_id <= MAX_POLICY_ELEMENT; ++class_id)
    {
        char acl_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        char class_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        RULE_TYPE_MatchType_T match_type = MATCH_TYPE;

        UI32_T class_map_index;
        UI32_T class_index;

        UI32_T  acl_index;

        RULE_TYPE_Ace_Entry_T   ace_entry;

        RULE_TYPE_UI_ClassMapElement_T class_element;

        snprintf(class_name, sizeof(class_name), "c%lu", class_id);

        result = RULE_OM_Create_ClassMap(class_name, match_type, &class_map_index);
        assert(RULE_TYPE_OK == result);

        //
        // Add MF into class-map
        //
        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
        class_element.element.mf_entry.mf_value = 99;
        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index,
                                                         &class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
        class_element.element.mf_entry.mf_value = 100;
        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index,
                                                         &class_element, &class_index);
        assert(RULE_TYPE_OK == result);


        //
        // Create ACL
        //
        snprintf(acl_name, sizeof(acl_name), "a%lu", class_id);
        RULE_CTRL_UT_Proc_Create_ACL(acl_name, ACL_TYPE, &acl_index);

        //
        // Add ACE
        //
        ace_entry.ace_type = ACE_TYPE;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.u.mac.aceSourceMacAddr[5] = 0x01;
        memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.u.mac.aceSourceMacAddr[5] = 0x02;
        memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.u.mac.aceSourceMacAddr[5] = 0x04;
        memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        //
        // Add ACL into class-map
        //

        class_element.class_type = RULE_TYPE_CLASS_ACL;
        strncpy(class_element.element.acl_name, acl_name,
                sizeof(class_element.element.acl_name)-1);
        class_element.element.acl_name[sizeof(class_element.element.acl_name)-1] = '\0';

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index,
                                                         &class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // Add MF into class-map
        //
        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
        class_element.element.mf_entry.mf_value = 199;
        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index,
                                                         &class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type = RULE_TYPE_MF_VLAN;
        class_element.element.mf_entry.mf_value = 200;
        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index,
                                                         &class_element, &class_index);
        assert(RULE_TYPE_OK == result);


        //
        // Create ACL
        //
        snprintf(acl_name, sizeof(acl_name), "A%lu", class_id);
        RULE_CTRL_UT_Proc_Create_ACL(acl_name, ACL_TYPE, &acl_index);

        //
        // Add ACE
        //
        ace_entry.ace_type = ACE_TYPE;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.u.mac.aceSourceMacAddr[5] = 0x10;
        memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.u.mac.aceSourceMacAddr[5] = 0x20;
        memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.u.mac.aceSourceMacAddr[5] = 0x40;
        memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        //
        // Add ACL into class-map
        //

        class_element.class_type = RULE_TYPE_CLASS_ACL;
        strncpy(class_element.element.acl_name, acl_name,
                sizeof(class_element.element.acl_name)-1);
        class_element.element.acl_name[sizeof(class_element.element.acl_name)-1] = '\0';

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index,
                                                         &class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // Add class-map into policy-map
        //
        result = RULE_OM_Add_PolicyMapElement_To_PolicyMap(policy_map_index,
                                                           class_map_index);
        assert(RULE_TYPE_OK == result);
    }

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++port)
        {
            UI32_T ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                RULE_CTRL_UT_Proc_Bind_Policy_Map(ifindex, direction, policy_map_index);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++port)
        {
            UI32_T ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                RULE_CTRL_UT_Proc_UnBind_Policy_Map(ifindex, direction, policy_map_index);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    return 0;
}

int
RULE_CTRL_UT_Set_QoS_With_Match_Any_ACL_MF_Rule()
{
    enum
    {
        MAX_UNIT        = 1,
        MAX_PORT        = 5,

        MIN_DIRECTION   = RULE_TYPE_INBOUND,
        
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION   = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION   = RULE_TYPE_INBOUND,
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

    };

    const char                  *policy_map_name = "p1";

    UI32_T                      unit;
    UI32_T                      port;

    UI32_T                      ifindex;

    RULE_TYPE_InOutDirection_T  direction;
    BOOL_T                      is_add;

    RULE_CTRL_UT_Proc_Create_Policy_Match_Any_Class_Map_With_Match_ACL_MF(policy_map_name);
    RULE_CTRL_UT_Proc_Validate_Rule();

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
    //{{{
    ifindex = 2;
    direction = RULE_TYPE_OUTBOUND;

    is_add = TRUE;

    RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex, direction, is_add, policy_map_name);
    RULE_CTRL_UT_Proc_Validate_Rule();

    is_add = FALSE;

    RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex, direction, is_add, policy_map_name);
    RULE_CTRL_UT_Proc_Validate_Rule();
    //}}}
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

    ifindex = 2;
    direction = RULE_TYPE_INBOUND;

    is_add = TRUE;

    RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex, direction, is_add, policy_map_name);
    RULE_CTRL_UT_Proc_Validate_Rule();

    is_add = FALSE;

    RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex, direction, is_add, policy_map_name);
    RULE_CTRL_UT_Proc_Validate_Rule();

    ifindex = 2;
    direction = RULE_TYPE_INBOUND;

    is_add = TRUE;

    RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex, direction, is_add, policy_map_name);
    RULE_CTRL_UT_Proc_Validate_Rule();

    is_add = FALSE;

    RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex, direction, is_add, policy_map_name);
    RULE_CTRL_UT_Proc_Validate_Rule();

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++port)
        {
            ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++direction)
            {
                is_add = TRUE;
                RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex,
                                                          direction,
                                                          is_add,
                                                          policy_map_name);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++port)
        {
            ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++direction)
            {
                is_add = FALSE;
                RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex,
                                                          direction,
                                                          is_add,
                                                          policy_map_name);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    return 0;
}

int
RULE_CTRL_UT_Set_QoS_With_Match_All_ACL_MF_Rule()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    enum
    {
        MAX_UNIT        = 1,
        MAX_PORT        = SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT_ON_BOARD,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION   = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION   = RULE_TYPE_INBOUND,
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

    };

    const char                  *policy_map_name = "p1";

    UI32_T                      unit;
    UI32_T                      port;

    UI32_T                      ifindex;

    RULE_TYPE_InOutDirection_T  direction;

    BOOL_T                      is_add;

    RULE_CTRL_UT_Proc_Create_Policy_Match_All_Class_Map_With_Match_ACL_MF(policy_map_name);

    ifindex = 2;
    direction = RULE_TYPE_INBOUND;

    is_add = TRUE;

    RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex, direction, is_add, policy_map_name);
    RULE_CTRL_UT_Proc_Validate_Rule();

    is_add = FALSE;

    RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex, direction, is_add, policy_map_name);
    RULE_CTRL_UT_Proc_Validate_Rule();

    ifindex = 2;
    direction = RULE_TYPE_INBOUND;

    is_add = TRUE;

    RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex, direction, is_add, policy_map_name);
    RULE_CTRL_UT_Proc_Validate_Rule();

    is_add = FALSE;

    RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex, direction, is_add, policy_map_name);
    RULE_CTRL_UT_Proc_Validate_Rule();

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++port)
        {
            ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                is_add = TRUE;
                RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex,
                                                          direction,
                                                          is_add,
                                                          policy_map_name);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++port)
        {
            ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                is_add = FALSE;
                RULE_CTRL_UT_Proc_Bind_Policy_Map_To_Port(ifindex,
                                                          direction,
                                                          is_add,
                                                          policy_map_name);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }
#endif /* #if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE) */

    return 0;
}

int
RULE_CTRL_UT_Set_QoS_With_Modifying_Match_Cond_Of_Class_Map_On_Fly()
{
    enum
    {
        MIN_IFINDEX         = 1,
        MAX_IFINDEX         = 3,

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION       = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION       = RULE_TYPE_INBOUND,
#endif

        MAX_POLICY_ELEMENT  = 5,
        MAX_ACE_IN_ACL      = 5,        
    };

    RULE_TYPE_RETURN_TYPE_T result;

    const char *policy_map_name = "p1";
    UI32_T policy_map_index;

    UI32_T ifindex;
    RULE_TYPE_InOutDirection_T direction;

    //
    // Bind Policy-map and ACL on port
    //
    {
        RULE_CTRL_UT_Proc_Create_Policy_Match_Any_Class_Map_With_Match_ACL_MF(policy_map_name);

        result = RULE_OM_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);

        for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
        {
            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                RULE_CTRL_UT_Proc_Bind_Policy_Map(ifindex, direction, policy_map_index);

                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    //
    // Create new class-map
    //
    {
        const char                      *class_map_name = "c1";
        UI32_T                          class_map_index;
        UI32_T                          class_index;

        result = RULE_OM_GetClassMapIndexByName(class_map_name, &class_map_index);
        assert(RULE_TYPE_OK == result);

        //
        // Match ACL
        //
        RULE_CTRL_UT_Proc_Add_ACL_Into_Class_Map(class_map_index,
                                                 "a-new-1",
                                                 RULE_TYPE_IP_EXT_ACL,
                                                 MAX_ACE_IN_ACL,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &class_index);

        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_ACL, class_index);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();

        result = RULE_CTRL_RemoveClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_ACL, class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_ACL, class_index);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();

        //
        // Match MF
        //
        RULE_CTRL_UT_Proc_Add_MF_Into_Class_Map(class_map_index, RULE_TYPE_MF_DSCP, 1, &class_index);
//        RULE_CTRL_UT_Proc_Add_MF_Into_Class_Map(class_map_index, RULE_TYPE_MF_VLAN, 5001, &class_index);

        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();

        result = RULE_CTRL_RemoveClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();

        //
        // Match MF
        //
        RULE_CTRL_UT_Proc_Add_MF_Into_Class_Map(class_map_index, RULE_TYPE_MF_DSCP, 2, &class_index);
//        RULE_CTRL_UT_Proc_Add_MF_Into_Class_Map(class_map_index, RULE_TYPE_MF_VLAN, 6001, &class_index);

        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();

        result = RULE_CTRL_RemoveClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();

        //
        // Match MF
        //
        RULE_CTRL_UT_Proc_Add_MF_Into_Class_Map(class_map_index, RULE_TYPE_MF_DSCP, 3, &class_index);        
//        RULE_CTRL_UT_Proc_Add_MF_Into_Class_Map(class_map_index, RULE_TYPE_MF_VLAN, 7001, &class_index);

        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();

        result = RULE_CTRL_RemoveClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();

        //
        // Match ACL
        //
        RULE_CTRL_UT_Proc_Add_ACL_Into_Class_Map(class_map_index,
                                                 "a-new-2",
                                                 RULE_TYPE_IP_EXT_ACL,
                                                 MAX_ACE_IN_ACL,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &class_index);

        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_ACL, class_index);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();

        result = RULE_CTRL_RemoveClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_ACL, class_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_ACL, class_index);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();

//        //
//        // Match MF
//        //
//        RULE_CTRL_UT_Proc_Add_MF_Into_Class_Map(class_map_index, RULE_TYPE_MF_DSCP, 43, &class_index);
//
//        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);
//        assert(RULE_TYPE_OK == result);
//
//        RULE_CTRL_UT_Proc_Validate_Rule();
//
//        result = RULE_CTRL_RemoveClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);
//        assert(RULE_TYPE_OK == result);
//
//        result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);
//        assert(RULE_TYPE_OK == result);
//
//        RULE_CTRL_UT_Proc_Validate_Rule();
    }

    return 0;
}

int
RULE_CTRL_UT_Set_QoS_With_Modifying_Match_Cond_Of_Class_Map_On_Fly_Undo()
{
    enum
    {
        MIN_IFINDEX         = 1,
        MAX_IFINDEX         = 3,

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION       = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION       = RULE_TYPE_INBOUND,
#endif

        MAX_POLICY_ELEMENT  = 5,
        MAX_ACE_IN_ACL      = SYS_ADPT_MAX_NBRS_OF_ACE / 2,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    const char *policy_map_name = "p1";
    UI32_T policy_map_index;

    UI32_T ifindex;
    RULE_TYPE_InOutDirection_T direction;

    //
    // Bind Policy-map and ACL on port
    //
    {
        RULE_CTRL_UT_Proc_Create_Policy_Match_Any_Class_Map_With_Match_ACL_MF(policy_map_name);

        result = RULE_OM_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);

        for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
        {
            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                RULE_CTRL_UT_Proc_Bind_Policy_Map(ifindex, direction, policy_map_index);

                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    //
    // Create new class-map
    //
    {
        const char                      *class_map_name = "c1";
        UI32_T                          class_map_index;
        UI32_T                          class_index;
        UI32_T                          i;

        result = RULE_OM_GetClassMapIndexByName(class_map_name, &class_map_index);
        assert(RULE_TYPE_OK == result);

        for (i = 0; ; ++ i)
        {
            char acl_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH];

            sprintf(acl_name, "a-new-%lu", i);

            //
            // Match ACL
            //
            RULE_CTRL_UT_Proc_Add_ACL_Into_Class_Map(class_map_index,
                                                     acl_name,
                                                     RULE_TYPE_IP_EXT_ACL,
                                                     MAX_ACE_IN_ACL,
                                                     RULE_CTRL_UT_COMPRESS_DISABLE,
                                                     &class_index);

            result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_ACL, class_index);

            if (RULE_TYPE_OK != result)
            {
                result = RULE_OM_RemoveClassMapElementByAclIndex(class_map_index, class_index);
                assert(RULE_TYPE_OK == result);
                
                RULE_CTRL_UT_Proc_Validate_Rule();
                break;
            }

            //
            // Match MF
            //
//            RULE_CTRL_UT_Proc_Add_MF_Into_Class_Map(class_map_index, RULE_TYPE_MF_VLAN, 5000 + i, &class_index);
            RULE_CTRL_UT_Proc_Add_MF_Into_Class_Map(class_map_index, RULE_TYPE_MF_DSCP, i, &class_index);            

            result = RULE_CTRL_AddClassMapElementConfig(class_map_index, RULE_TYPE_CLASS_MF, class_index);

            if (RULE_TYPE_OK != result)
            {
                result = RULE_OM_RemoveClassMapElementByMfIndex(class_map_index, class_index);
                assert(RULE_TYPE_OK == result);

                RULE_CTRL_UT_Proc_Validate_Rule();
                break;
            }

        }
    }
    
    return 0;
}

int
RULE_CTRL_UT_Set_QoS_With_Modifying_ACE_On_Fly()
{
    enum
    {
        MIN_IFINDEX         = 1,
        MAX_IFINDEX         = 3,

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION       = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION       = RULE_TYPE_INBOUND,
#endif

        MAX_POLICY_ELEMENT  = 5,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    const char *acl_name = "a2";
    UI32_T acl_index;

    UI32_T ifindex;
    RULE_TYPE_InOutDirection_T direction;

    //
    // Bind Policy-map and ACL on port
    //
    {
        const char *policy_map_name = "p1";
        UI32_T policy_map_index;

        RULE_CTRL_UT_Proc_Create_Policy_Match_Any_Class_Map_With_Match_ACL_MF(policy_map_name);
        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);

        result = RULE_OM_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);

        for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
        {
            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_ENABLE;

                RULE_CTRL_UT_Proc_Bind_Policy_Map(ifindex, direction, policy_map_index);

                if (RULE_TYPE_INBOUND == direction)
                {
                    result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                        acl_index,
                                                        time_range_index,
                                                        counter_enable);
                    assert(RULE_TYPE_OK == result);
                }

                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    //
    // Add on Fly
    //
    {
        RULE_TYPE_Acl_T acl_entry;
        RULE_TYPE_Ace_Entry_T ace_entry;

        BOOL_T is_add;

        result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.ace_type = acl_entry.acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.access = RULE_TYPE_ACE_PERMIT;

        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        //
        //

        is_add = TRUE;
        result = RULE_CTRL_SetAce(acl_index, &ace_entry, is_add);

        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();

        is_add = FALSE;
        result = RULE_CTRL_SetAce(acl_index, &ace_entry, is_add);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_DelAceFromAcl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();
    }

    return 0;
}

int
RULE_CTRL_UT_Set_QoS_With_Modifying_Meter_On_Fly()
{
    enum
    {
        MAX_UNIT            = 1,
        MAX_PORT            = 3,

        MIN_IFINDEX         = 1,
        MAX_IFINDEX         = 3,

        MIN_DIRECTION       = RULE_TYPE_INBOUND,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION       = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION       = RULE_TYPE_INBOUND,
#endif

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,

        MAX_POLICY_ELEMENT  = 5,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ifindex;
    RULE_TYPE_InOutDirection_T direction;

    {
        const char *policy_map_name = "p1";
        UI32_T policy_map_index;

        RULE_CTRL_UT_Proc_Create_Policy_Match_Any_Class_Map_With_Match_ACL_MF(policy_map_name);

        result = RULE_OM_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);

        for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
        {
            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
            {
                RULE_CTRL_UT_Proc_Bind_Policy_Map(ifindex, direction, policy_map_index);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }

        //
        // Update meter config
        //
        {
            const char *class_map_name = "c2";
            UI32_T class_map_index;
            RULE_TYPE_TBParamEntry_T meter_entry;

            result = RULE_OM_GetClassMapIndexByName(class_map_name, &class_map_index);
            assert(RULE_TYPE_OK == result);

            memset(&meter_entry, 0, sizeof(meter_entry));
            meter_entry.meter_model = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
            meter_entry.rate = 4000;
            meter_entry.burst_size = 5000;
            meter_entry.peak_rate = 6000;
            meter_entry.peak_burst_size = 7000;

            result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter_entry);
            assert(RULE_TYPE_OK == result);

            result = RULE_CTRL_UpdateMeterConfig(policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);

            RULE_CTRL_UT_Proc_Validate_Rule();

            memset(&meter_entry, 0, sizeof(meter_entry));
            meter_entry.meter_model = RULE_TYPE_METER_MODE_TRTCM_COLOR_BLIND;
            meter_entry.rate = 4100;
            meter_entry.burst_size = 5100;
            meter_entry.peak_rate = 6100;
            meter_entry.peak_burst_size = 7100;

            result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter_entry);
            assert(RULE_TYPE_OK == result);

            result = RULE_CTRL_UpdateMeterConfig(policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);

            RULE_CTRL_UT_Proc_Validate_Rule();

            memset(&meter_entry, 0, sizeof(meter_entry));
            meter_entry.meter_model = RULE_TYPE_METER_MODE_SRTCM_COLOR_AWARE;
            meter_entry.rate = 4200;
            meter_entry.burst_size = 5200;
            meter_entry.peak_rate = 6200;
            meter_entry.peak_burst_size = 7200;

            result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter_entry);
            assert(RULE_TYPE_OK == result);

            result = RULE_CTRL_UpdateMeterConfig(policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);

            RULE_CTRL_UT_Proc_Validate_Rule();

            memset(&meter_entry, 0, sizeof(meter_entry));
            meter_entry.meter_model = RULE_TYPE_METER_MODE_SRTCM_COLOR_BLIND;
            meter_entry.rate = 4300;
            meter_entry.burst_size = 5300;
            meter_entry.peak_rate = 6300;
            meter_entry.peak_burst_size = 7300;

            result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter_entry);
            assert(RULE_TYPE_OK == result);

            result = RULE_CTRL_UpdateMeterConfig(policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);

            RULE_CTRL_UT_Proc_Validate_Rule();

            memset(&meter_entry, 0, sizeof(meter_entry));
            meter_entry.meter_model = RULE_TYPE_METER_MODE_FLOW;
            meter_entry.rate = 4400;
            meter_entry.burst_size = 5400;

            result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter_entry);
            assert(RULE_TYPE_OK == result);

            result = RULE_CTRL_UpdateMeterConfig(policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);

            RULE_CTRL_UT_Proc_Validate_Rule();

            memset(&meter_entry, 0, sizeof(meter_entry));
            meter_entry.meter_model = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
            meter_entry.rate = 4500;
            meter_entry.burst_size = 5500;
            meter_entry.peak_rate = 6500;
            meter_entry.peak_burst_size = 7500;

            result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter_entry);
            assert(RULE_TYPE_OK == result);

            result = RULE_CTRL_UpdateMeterConfig(policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);

            RULE_CTRL_UT_Proc_Validate_Rule();
        }

        //
        // Remove meter config
        //
        {
            const char *class_map_name = "c2";
            UI32_T class_map_index;

            result = RULE_OM_GetClassMapIndexByName(class_map_name, &class_map_index);
            assert(RULE_TYPE_OK == result);

            result = RULE_OM_Remove_MeterEntry(policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);

            result = RULE_CTRL_RemoveMeterConfig(policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);

            RULE_CTRL_UT_Proc_Validate_Rule();
        }

        //
        // Add meter config
        //
        {
            const char *class_map_name = "c2";
            UI32_T class_map_index;
            RULE_TYPE_TBParamEntry_T meter_entry;

            result = RULE_OM_GetClassMapIndexByName(class_map_name, &class_map_index);
            assert(RULE_TYPE_OK == result);

            memset(&meter_entry, 0, sizeof(meter_entry));
            meter_entry.meter_model = RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE;
            meter_entry.rate = 4000;
            meter_entry.burst_size = 5000;
            meter_entry.peak_rate = 6000;
            meter_entry.peak_burst_size = 7000;

            result = RULE_OM_Set_MeterEntry(policy_map_index, class_map_index, &meter_entry);
            assert(RULE_TYPE_OK == result);

            result = RULE_CTRL_AddMeterConfig(policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);

            RULE_CTRL_UT_Proc_Validate_Rule();
        }
    }

    return 0;
}

int RULE_CTRL_UT_Set_QoS_With_Modifying_Action_On_Fly()
{
    enum
    {
        MIN_IFINDEX         = 1,
        MAX_IFINDEX         = 3,

        MIN_DIRECTION       = RULE_TYPE_INBOUND,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION       = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION       = RULE_TYPE_INBOUND,
#endif

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ifindex;
    RULE_TYPE_InOutDirection_T direction;

    {
        const char *policy_map_name = "p1";
        UI32_T policy_map_index;

        RULE_CTRL_UT_Proc_Create_Policy_Match_Any_Class_Map_With_Match_ACL_MF(policy_map_name);

        result = RULE_OM_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);

        for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
        {
            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                RULE_CTRL_UT_Proc_Bind_Policy_Map(ifindex, direction, policy_map_index);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }

        //
        // Update action config
        //
        {
            const char *class_map_name = "c2";
            UI32_T class_map_index;

            result = RULE_OM_GetClassMapIndexByName(class_map_name, &class_map_index);
            assert(RULE_TYPE_OK == result);

            result = RULE_CTRL_UpdateActionConfig(policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);

            RULE_CTRL_UT_Proc_Validate_Rule();
        }

        {
            const char *class_map_name = "c1";
            UI32_T class_map_index;

            result = RULE_OM_GetClassMapIndexByName(class_map_name, &class_map_index);
            assert(RULE_TYPE_OK == result);

            result = RULE_CTRL_UpdateActionConfig(policy_map_index, class_map_index);
            assert(RULE_TYPE_OK == result);

            RULE_CTRL_UT_Proc_Validate_Rule();
        }
    }

    return 0;
}


int RULE_CTRL_UT_Set_QoS_Rule_AllocateRule_Fail_Case()
{
    enum
    {
        MIN_IFINDEX         = 1,
        MAX_IFINDEX         = 3,

        MIN_DIRECTION       = RULE_TYPE_INBOUND,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION       = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION       = RULE_TYPE_INBOUND,
#endif

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,
    };

    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_InOutDirection_T direction;
    RULE_CTRL_OpTable_T test_optable;
    UI32_T ifindex;

    memcpy(&test_optable, &rule_ctrl_dflt_optable, sizeof(test_optable));

    test_optable.fn_allocate_rule = (void *) RULE_CTRL_UT_Porc_ReturnFalse;
    rule_ctrl_optable_ptr = &test_optable;

    {
        const char *policy_map_name = "p1";
        UI32_T policy_map_index;

        RULE_CTRL_UT_Proc_Create_Policy_Match_Any_Class_Map_With_Match_ACL_MF(policy_map_name);

        result = RULE_OM_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);

        for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
        {
            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                result = RULE_CTRL_SetPolicyMap(ifindex, direction, policy_map_index, TRUE);
                assert(RULE_TYPE_OK != result);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    rule_ctrl_optable_ptr = &rule_ctrl_dflt_optable;

    return 0;
}

int RULE_CTRL_UT_Set_QoS_Rule_SetRule_Fail_Case()
{
    enum
    {
        MIN_IFINDEX         = 1,
        MAX_IFINDEX         = 3,

        MIN_DIRECTION       = RULE_TYPE_INBOUND,

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION       = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION       = RULE_TYPE_INBOUND,
#endif

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,
    };

    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_InOutDirection_T direction;
    RULE_CTRL_OpTable_T test_optable;
    UI32_T ifindex;

    memcpy(&test_optable, &rule_ctrl_dflt_optable, sizeof(test_optable));

    test_optable.fn_set_rule = (void *) RULE_CTRL_UT_Porc_ReturnFalse;
    rule_ctrl_optable_ptr = &test_optable;

    {
        const char *policy_map_name = "p1";
        UI32_T policy_map_index;

        RULE_CTRL_UT_Proc_Create_Policy_Match_Any_Class_Map_With_Match_ACL_MF(policy_map_name);

        result = RULE_OM_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);

        for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
        {
            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                result = RULE_CTRL_SetPolicyMap(ifindex, direction, policy_map_index, TRUE);
                assert(RULE_TYPE_OK != result);
                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }
    
    rule_ctrl_optable_ptr = &rule_ctrl_dflt_optable;
    
    return 0;
}

BOOL_T
RULE_CTRL_UT_Proc_Is_Active_Rule_Instance(
    const RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    if (RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type))
    {
        RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;

        rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T)in;

        if (TRUE == rule_inst_p->active)
        {
            return TRUE;
        }
    }

    return FALSE;
}

#if 0
int
RULE_CTRL_UT_Time_Based_ACL_Apply_ACE()
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    enum
    {
        MAX_UNIT            = 1,
        MAX_PORT            = 1,
        MAX_DIRECTION       = RULE_TYPE_INBOUND,

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,

        MAX_POLICY_ELEMENT  = 5,
        MATCH_TYPE          = RULE_TYPE_CLASS_MAP_MATCH_ANY,
    };

    UI32_T                  unit;
    UI32_T                  port;
    UI32_T                  direction;

    const char *time_range_name = "t1";
    UI32_T time_range_index;

    RULE_TYPE_TIME_ACL_PERIODIC_TIME_T periodic;

    const char *acl_name = "a1";
    UI32_T acl_index;

    RULE_TYPE_Ace_Entry_T ace_entry;
    UI32_T active_ace_count = 0;

    RULE_TYPE_RETURN_TYPE_T result;
    BOOL_T                  b_result;

    //
    // Create Time-Range
    //
    result = RULE_OM_CreateTimeRangeEntry(time_range_name, &time_range_index);
    assert(RULE_TYPE_OK == result);

    memset(&periodic, 0, sizeof(periodic));

    periodic.start_type_of_wk   = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
    periodic.end_type_of_wk     = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
    periodic.start_hour         = 8;
    periodic.end_hour           = 12;

    result = RULE_OM_SetTimeRangePeriodic(time_range_index, &periodic);
    assert(RULE_TYPE_OK == result);

    //
    // Create ACL
    //
    RULE_CTRL_UT_Proc_Create_ACL(acl_name, ACL_TYPE, &acl_index);

    //
    // One
    ace_entry.ace_type = ACE_TYPE;
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x01;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    active_ace_count ++;

    //
    // Two
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x02;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    ace_entry.time_range_index = time_range_index;

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    //
    // Three
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x03;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    ace_entry.time_range_index = time_range_index;

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    //
    // Four
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x04;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    active_ace_count ++;

    //
    // Five
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x05;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    ace_entry.time_range_index = time_range_index;

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    //
    // Set time out of time-range
    //
    {
        int year, month, day, hour, minute, second;

        year    = 2001;
        month   = 1;
        day     = 1;

        assert(0 < periodic.start_hour);
        hour    = periodic.start_hour - 1;

        minute  = 0;
        second  = 0;

        b_result = SYS_TIME_SetRealTimeClock(year, month, day, hour, minute, second);
        assert(TRUE == b_result);
    }

    //
    // Update OM
    //
    RULE_CTRL_LocalTimeBasedACLProcessTimerEvent();

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++ port)
        {
            UI32_T ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                RULE_TYPE_INTERFACE_INFO_T interface;

                UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

                memset(&interface, 0, sizeof(interface));
                result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                assert(RULE_TYPE_OK == result);
                interface.direction = direction;

                result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                    acl_index,
                                                    time_range_index,
                                                    counter_enable);
                assert(RULE_TYPE_OK == result);

                {
                    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;

                    acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                    assert(NULL != acl_inst_p);

                    assert(active_ace_count == RULE_CTRL_IN_Priv_CountIf((RULE_TYPE_INSTANCE_PTR_T) acl_inst_p,
                                                                         RULE_CTRL_UT_Proc_Is_Active_Rule_Instance,
                                                                         NULL));
                }
            }
        }
    }

    //
    // Set time in time-range
    //
    {
        int year, month, day, hour, minute, second;

        year    = 2001;
        month   = 1;
        day     = 1;

        hour    = periodic.start_hour;

        minute  = 0;
        second  = 0;

        b_result = SYS_TIME_SetRealTimeClock(year, month, day, hour, minute, second);
        assert(TRUE == b_result);
    }

    RULE_CTRL_LocalTimeBasedACLProcessTimerEvent();
    RULE_CTRL_UT_Proc_Validate_Rule();


#endif // SYS_CPNT_TIME_BASED_ACL

    return 0;
}

int
RULE_CTRL_UT_Time_Based_ACL_Remove_ACE()
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    enum
    {
        MAX_UNIT            = 1,
        MAX_PORT            = 1,
        MAX_DIRECTION       = RULE_TYPE_INBOUND,

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,

        MAX_POLICY_ELEMENT  = 5,
        MATCH_TYPE          = RULE_TYPE_CLASS_MAP_MATCH_ANY,
    };

    UI32_T                  unit;
    UI32_T                  port;
    UI32_T                  direction;

    const char *time_range_name = "t1";
    UI32_T time_range_index;

    RULE_TYPE_TIME_ACL_PERIODIC_TIME_T periodic;

    const char *acl_name = "a1";
    UI32_T acl_index;

    RULE_TYPE_Ace_Entry_T ace_entry;
    UI32_T active_ace_count = 0;

    RULE_TYPE_RETURN_TYPE_T result;
    BOOL_T                  b_result;

    //
    // Create Time-Range
    //
    result = RULE_OM_CreateTimeRangeEntry(time_range_name, &time_range_index);
    assert(RULE_TYPE_OK == result);

    memset(&periodic, 0, sizeof(periodic));

    periodic.start_type_of_wk   = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
    periodic.end_type_of_wk     = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
    periodic.start_hour         = 8;
    periodic.end_hour           = 12;

    result = RULE_OM_SetTimeRangePeriodic(time_range_index, &periodic);
    assert(RULE_TYPE_OK == result);

    //
    // Create ACL
    //
    RULE_CTRL_UT_Proc_Create_ACL(acl_name, ACL_TYPE, &acl_index);

    //
    // One
    ace_entry.ace_type = ACE_TYPE;
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x01;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    active_ace_count ++;

    //
    // Two
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x02;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    ace_entry.time_range_index = time_range_index;

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    //
    // Three
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x03;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    ace_entry.time_range_index = time_range_index;

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    //
    // Four
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x04;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    active_ace_count ++;

    //
    // Five
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x05;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    ace_entry.time_range_index = time_range_index;

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    //
    // Set time in time-range
    //
    {
        int year, month, day, hour, minute, second;

        year    = 2001;
        month   = 1;
        day     = 1;
        hour    = periodic.start_hour;
        minute  = 0;
        second  = 0;

        b_result = SYS_TIME_SetRealTimeClock(year, month, day, hour, minute, second);
        assert(TRUE == b_result);
    }

    //
    // Update OM
    //
    RULE_CTRL_LocalTimeBasedACLProcessTimerEvent();

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++ port)
        {
            UI32_T ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                RULE_TYPE_INTERFACE_INFO_T interface;

                UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

                memset(&interface, 0, sizeof(interface));
                result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                assert(RULE_TYPE_OK == result);
                interface.direction = direction;

                result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                    acl_index,
                                                    time_range_index,
                                                    counter_enable);
                assert(RULE_TYPE_OK == result);

                {
                    RULE_TYPE_CLASS_INSTANCE_PTR_T              acl_inst_p;
                    RULE_TYPE_CLASS_INSTANCE_OBJECT_T           acl_inst_obj;
                    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;

                    RULE_TYPE_Acl_T acl_entry;

                    acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                    assert(NULL != acl_inst_p);

                    assert(acl_index == acl_inst_p->acl_id);
                    assert(RULE_TYPE_INST_ACL == acl_inst_p->super.type);

                    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);
                    acl_inst_obj.get_element_iterator(&acl_inst_obj, &ace_inst_iter);

                    result = RULE_OM_GetAclByIndex(acl_inst_p->acl_id, &acl_entry);
                    assert(RULE_TYPE_OK == result);

                    assert(acl_entry.ace_count == RULE_CTRL_IN_Priv_CountIf((RULE_TYPE_INSTANCE_PTR_T)acl_inst_p,
                                                                            RULE_CTRL_UT_Proc_Is_Active_Rule_Instance,
                                                                            NULL));
                }
            }
        }
    }

    //
    // Set time out of time-range
    //
    {
        int year, month, day, hour, minute, second;

        year    = 2001;
        month   = 1;
        day     = 1;

        assert(0 < periodic.start_hour);
        hour    = periodic.start_hour - 1;

        minute  = 0;
        second  = 0;

        b_result = SYS_TIME_SetRealTimeClock(year, month, day, hour, minute, second);
        assert(TRUE == b_result);
    }

    RULE_CTRL_LocalTimeBasedACLProcessTimerEvent();

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++ port)
        {
            UI32_T ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = RULE_TYPE_INBOUND; direction <= MAX_DIRECTION; ++direction)
            {
                RULE_TYPE_INTERFACE_INFO_T interface;

                RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;

                memset(&interface, 0, sizeof(interface));
                result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                assert(RULE_TYPE_OK == result);
                interface.direction = direction;

                acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);

                assert(NULL != acl_inst_p);

                assert(active_ace_count == RULE_CTRL_IN_Priv_CountIf((RULE_TYPE_INSTANCE_PTR_T)acl_inst_p,
                                                                     RULE_CTRL_UT_Proc_Is_Active_Rule_Instance,
                                                                     NULL));
            }
        }
    }

    RULE_CTRL_UT_Proc_Validate_Rule();


#endif // SYS_CPNT_TIME_BASED_ACL

    return 0;
}

int
RULE_CTRL_UT_Apply_ACL_With_Different_Time_Range()
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    enum
    {
        MIN_DIRECTION       = RULE_TYPE_INBOUND,
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION   = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION   = RULE_TYPE_INBOUND,
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

        ACL_TYPE            = RULE_TYPE_MAC_ACL,
        ACE_TYPE            = RULE_TYPE_MAC_ACL,
    };

    UI32_T                  direction;

    typedef struct
    {
        UI16_T index;
        UI32_T start_hour;
        UI32_T end_hour;
    } time_range_cfg_t;

    time_range_cfg_t acl_time_range[] =
    {
        {
            0,
            8,
            12
        },
        {
            0,
            11,
            13
        },
        {
            0,
            14,
            16
        },
    };

    time_range_cfg_t ace_time_range =
    {
        0,
        7,
        13
    };

    UI32_T port_list[] =
    {
        1,
        2,
        3,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,
    };

    const char *acl_name = "a1";
    UI32_T acl_index;

    UI32_T i;

    RULE_TYPE_Ace_Entry_T ace_entry;
    UI32_T active_ace_count = 0;

    RULE_TYPE_RETURN_TYPE_T result;
    BOOL_T                  b_result;

    //
    // Create Time-Range
    //
    {
        char time_range_name[30];
        RULE_TYPE_TIME_ACL_PERIODIC_TIME_T periodic;

        snprintf(time_range_name, sizeof(time_range_name), "t%lu", i);

        result = RULE_OM_CreateTimeRangeEntry(time_range_name, &ace_time_range.index);
        assert(RULE_TYPE_OK == result);

        memset(&periodic, 0, sizeof(periodic));

        periodic.start_type_of_wk   = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
        periodic.end_type_of_wk     = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
        periodic.start_hour         = ace_time_range.start_hour;
        periodic.end_hour           = ace_time_range.end_hour;

        result = RULE_OM_SetTimeRangePeriodic(ace_time_range.index, &periodic);
        assert(RULE_TYPE_OK == result);
    }

    for (i = 0; i < _countof(acl_time_range); ++ i)
    {
        RULE_TYPE_TIME_ACL_PERIODIC_TIME_T periodic;
        char time_range_name[30];

        snprintf(time_range_name, sizeof(time_range_name), "tt%lu", i);

        result = RULE_OM_CreateTimeRangeEntry(time_range_name, &acl_time_range[i].index);
        assert(RULE_TYPE_OK == result);

        memset(&periodic, 0, sizeof(periodic));

        periodic.start_type_of_wk   = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
        periodic.end_type_of_wk     = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
        periodic.start_hour         = acl_time_range[i].start_hour;
        periodic.end_hour           = acl_time_range[i].end_hour;
    }

    //
    // Create ACL
    //
//    result = RULE_OM_CreateAcl(acl_name, ACL_TYPE);
//    assert(RULE_TYPE_OK == result);
//
//    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
//    assert(RULE_TYPE_OK == result);

    RULE_CTRL_UT_Proc_Create_ACL(acl_name, ACL_TYPE, &acl_index);

    //
    // One
    ace_entry.ace_type = ACE_TYPE;
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x01;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    active_ace_count ++;

    //
    // Two
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x02;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    ace_entry.time_range_index = ace_time_range.index;

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    //
    // Three
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x03;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    ace_entry.time_range_index = ace_time_range.index;

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    //
    // Four
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x04;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    active_ace_count ++;

    //
    // Five
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.u.mac.aceSourceMacAddr[5] = 0x05;
    memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));

    ace_entry.time_range_index = ace_time_range.index;

    result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    //
    // Set time in time-range
    //
    {
        int year, month, day, hour, minute, second;

        year    = 2001;
        month   = 1;
        day     = 1;
        hour    = 0;
        minute  = 0;
        second  = 0;

        b_result = SYS_TIME_SetRealTimeClock(year, month, day, hour, minute, second);
        assert(TRUE == b_result);
    }

    //
    // Update OM
    //
    RULE_CTRL_LocalTimeBasedACLProcessTimerEvent();

    for (i = 0; i < _countof(port_list); ++ i)
    {
        UI32_T ifindex = port_list[i];

        for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
        {
            RULE_TYPE_INTERFACE_INFO_T interface;

            UI32_T time_range_index = acl_time_range[ i % _countof(acl_time_range) ].index;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

            memset(&interface, 0, sizeof(interface));
            result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
            assert(RULE_TYPE_OK == result);
            interface.direction = direction;

            result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                acl_index,
                                                time_range_index,
                                                counter_enable);
            assert(RULE_TYPE_OK == result);
        }
    }

    for (i = 0; i < 24; ++ i)
    {
        {
            int year, month, day, hour, minute, second;

            year    = 2001;
            month   = 1;
            day     = 1;

            hour    = i;

            minute  = 0;
            second  = 0;

            b_result = SYS_TIME_SetRealTimeClock(year, month, day, hour, minute, second);
            assert(TRUE == b_result);
        }

        RULE_CTRL_LocalTimeBasedACLProcessTimerEvent();
        RULE_CTRL_UT_Proc_Validate_Rule();
    }

    RULE_CTRL_UT_Proc_Validate_Rule();

#endif // SYS_CPNT_TIME_BASED_ACL

    return 0;
}

int
RULE_CTRL_UT_Apply_All_Type_Of_ACLs_With_Different_Time_Range()
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    enum
    {
        MIN_DIRECTION       = RULE_TYPE_INBOUND,
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        MAX_DIRECTION   = RULE_TYPE_OUTBOUND,
#else
        MAX_DIRECTION   = RULE_TYPE_INBOUND,
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT
    };

    UI32_T                  direction;

    typedef struct
    {
        UI16_T index;
        UI32_T start_hour;
        UI32_T end_hour;
    } time_range_cfg_t;

    time_range_cfg_t acl_time_range[] =
    {
        {
            0,
            8,
            12
        },
        {
            0,
            11,
            13
        },
        {
            0,
            14,
            16
        },
    };

    time_range_cfg_t ace_time_range =
    {
        0,
        7,
        13
    };

    struct
    {
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL
        },
        {
            RULE_TYPE_IP_EXT_ACL
        },
        {
            RULE_TYPE_IPV6_EXT_ACL
        },
    };

    UI32_T port_list[] =
    {
        1,
        2,
        3,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,
    };

    UI32_T i, j;

    RULE_TYPE_Ace_Entry_T ace_entry;
    UI32_T active_ace_count = 0;

    RULE_TYPE_RETURN_TYPE_T result;
    BOOL_T                  b_result;

    //
    // Create Time-Range
    //
    {
        char time_range_name[30];
        RULE_TYPE_TIME_ACL_PERIODIC_TIME_T periodic;

        snprintf(time_range_name, sizeof(time_range_name), "t%lu", i);

        result = RULE_OM_CreateTimeRangeEntry(time_range_name, &ace_time_range.index);
        assert(RULE_TYPE_OK == result);

        memset(&periodic, 0, sizeof(periodic));

        periodic.start_type_of_wk   = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
        periodic.end_type_of_wk     = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
        periodic.start_hour         = ace_time_range.start_hour;
        periodic.end_hour           = ace_time_range.end_hour;

        result = RULE_OM_SetTimeRangePeriodic(ace_time_range.index, &periodic);
        assert(RULE_TYPE_OK == result);
    }

    for (i = 0; i < _countof(acl_time_range); ++ i)
    {
        RULE_TYPE_TIME_ACL_PERIODIC_TIME_T periodic;
        char time_range_name[30];

        snprintf(time_range_name, sizeof(time_range_name), "tt%lu", i);

        result = RULE_OM_CreateTimeRangeEntry(time_range_name, &acl_time_range[i].index);
        assert(RULE_TYPE_OK == result);

        memset(&periodic, 0, sizeof(periodic));

        periodic.start_type_of_wk   = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
        periodic.end_type_of_wk     = RULE_TYPE_TIME_RANGE_TYPE_OF_WK_DAILY;
        periodic.start_hour         = acl_time_range[i].start_hour;
        periodic.end_hour           = acl_time_range[i].end_hour;
    }

    for (i = 0; i < _countof(acls); ++ i)
    {
        RULE_TYPE_AclType_T acl_type = acls[i].acl_type;
        
        char acl_name[30];
        UI32_T acl_index;

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        //
        // Create ACL
        //
        RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

        acls[i].acl_index = acl_index;

        //
        // One
        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        switch (ace_entry.ace_type)
        {
            case RULE_TYPE_MAC_ACL:
                ace_entry.u.mac.aceSourceMacAddr[5] = 0x01;
                memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));
                break;

            case RULE_TYPE_IP_STD_ACL:
            case RULE_TYPE_IP_EXT_ACL:
                ace_entry.u.ip.aceSourceIpAddr = 0x00000001;
                ace_entry.u.ip.aceSourceIpAddrBitmask = 0xffffffff;
                break;

            case RULE_TYPE_IPV6_STD_ACL:
                ace_entry.u.ipv6.aceSourceIpAddr[15] = 0x01;
                //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceSourceIpAddrBitmask);
                break;

            case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                ace_entry.u.ipv6.aceSourceIpAddr[15] = 0x01;
                //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceSourceIpAddrBitmask);
#endif

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                ace_entry.u.ipv6.aceDestIpAddr[15] = 0x01;
                //ace_entry.u.ipv6.aceDestIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceDestIpAddrBitmask);
#endif

                break;

            default:
                assert(0);
                break;
        }

        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        active_ace_count ++;

        //
        // Two
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        switch (ace_entry.ace_type)
        {
            case RULE_TYPE_MAC_ACL:
                ace_entry.u.mac.aceSourceMacAddr[5] = 0x02;
                memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));
                break;

            case RULE_TYPE_IP_STD_ACL:
            case RULE_TYPE_IP_EXT_ACL:
                ace_entry.u.ip.aceSourceIpAddr = 0x00000002;
                ace_entry.u.ip.aceSourceIpAddrBitmask = 0xffffffff;
                break;

            case RULE_TYPE_IPV6_STD_ACL:
                ace_entry.u.ipv6.aceSourceIpAddr[15] = 0x02;
                //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceSourceIpAddrBitmask);
                break;

            case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                ace_entry.u.ipv6.aceSourceIpAddr[15] = 0x02;
                //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceSourceIpAddrBitmask);
#endif

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                ace_entry.u.ipv6.aceDestIpAddr[15] = 0x02;
                //ace_entry.u.ipv6.aceDestIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceDestIpAddrBitmask);

#endif
                break;

            default:
                assert(0);
                break;
        }

        ace_entry.time_range_index = ace_time_range.index;

        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        //
        // Three
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        switch (ace_entry.ace_type)
        {
            case RULE_TYPE_MAC_ACL:
                ace_entry.u.mac.aceSourceMacAddr[5] = 0x03;
                memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));
                break;

            case RULE_TYPE_IP_STD_ACL:
            case RULE_TYPE_IP_EXT_ACL:
                ace_entry.u.ip.aceSourceIpAddr = 0x00000003;
                ace_entry.u.ip.aceSourceIpAddrBitmask = 0xffffffff;
                break;

            case RULE_TYPE_IPV6_STD_ACL:
                ace_entry.u.ipv6.aceSourceIpAddr[15] = 0x03;
                //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceSourceIpAddrBitmask);

                break;

            case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                ace_entry.u.ipv6.aceSourceIpAddr[15] = 0x03;
                //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceSourceIpAddrBitmask);
#endif

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                ace_entry.u.ipv6.aceDestIpAddr[15] = 0x03;
                //ace_entry.u.ipv6.aceDestIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceDestIpAddrBitmask);

#endif
                break;

            default:
                assert(0);
                break;
        }

        ace_entry.time_range_index = ace_time_range.index;

        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        //
        // Four
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        switch (ace_entry.ace_type)
        {
            case RULE_TYPE_MAC_ACL:
                ace_entry.u.mac.aceSourceMacAddr[5] = 0x04;
                memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));
                break;

            case RULE_TYPE_IP_STD_ACL:
            case RULE_TYPE_IP_EXT_ACL:
                ace_entry.u.ip.aceSourceIpAddr = 0x00000004;
                ace_entry.u.ip.aceSourceIpAddrBitmask = 0xffffffff;
                break;

            case RULE_TYPE_IPV6_STD_ACL:
                ace_entry.u.ipv6.aceSourceIpAddr[15] = 0x04;
                //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceSourceIpAddrBitmask);

                break;

            case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                ace_entry.u.ipv6.aceSourceIpAddr[15] = 0x04;
                //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceSourceIpAddrBitmask);

#endif

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                ace_entry.u.ipv6.aceDestIpAddr[15] = 0x04;
                //ace_entry.u.ipv6.aceDestIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceDestIpAddrBitmask);

#endif
                break;

            default:
                assert(0);
                break;
        }

        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        active_ace_count ++;

        //
        // Five
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        switch (ace_entry.ace_type)
        {
            case RULE_TYPE_MAC_ACL:
                ace_entry.u.mac.aceSourceMacAddr[5] = 0x05;
                memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, sizeof(ace_entry.u.mac.aceSourceMacAddrBitmask));
                break;

            case RULE_TYPE_IP_STD_ACL:
            case RULE_TYPE_IP_EXT_ACL:
                ace_entry.u.ip.aceSourceIpAddr = 0x00000005;
                ace_entry.u.ip.aceSourceIpAddrBitmask = 0xffffffff;
                break;

            case RULE_TYPE_IPV6_STD_ACL:
                ace_entry.u.ipv6.aceSourceIpAddr[15] = 0x05;
                //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceSourceIpAddrBitmask);
                break;

            case RULE_TYPE_IPV6_EXT_ACL:
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
                ace_entry.u.ipv6.aceSourceIpAddr[15] = 0x05;
                //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceSourceIpAddrBitmask);
#endif

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
                ace_entry.u.ipv6.aceDestIpAddr[15] = 0x05;
                //ace_entry.u.ipv6.aceDestIpAddrPrefixLen = 8;
                RULE_OM_LocalConvertIpv6Prefix2Mask(8,
                                                    ace_entry.u.ipv6.aceDestIpAddrBitmask);
#endif
                break;

            default:
                assert(0);
                break;
        }

        ace_entry.time_range_index = ace_time_range.index;
        
        result = RULE_OM_AddAce2Acl(acl_index, ace_entry.ace_type, &ace_entry);
        assert(RULE_TYPE_OK == result);
    }

    //
    // Set time in time-range
    //
    {
        int year, month, day, hour, minute, second;

        year    = 2001;
        month   = 1;
        day     = 1;
        hour    = 0;
        minute  = 0;
        second  = 0;

        b_result = SYS_TIME_SetRealTimeClock(year, month, day, hour, minute, second);
        assert(TRUE == b_result);
    }

    //
    // Update OM
    //
    RULE_CTRL_LocalTimeBasedACLProcessTimerEvent();

    for (i = 0; i < _countof(acls); ++ i)
    {
        UI32_T acl_index = acls[i].acl_index;

        for (j = 0; j < _countof(port_list); ++ j)
        {
            UI32_T ifindex = port_list[j];

            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
            {
                UI32_T time_range_index = acl_time_range[ j % _countof(acl_time_range) ].index;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

                result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                    acl_index,
                                                    time_range_index,
                                                    counter_enable);
                assert(RULE_TYPE_OK == result);
            }
        }
    }

    for (i = 0; i < 24; ++ i)
    {
        {
            int year, month, day, hour, minute, second;

            year    = 2001;
            month   = 1;
            day     = 1;

            hour    = i;

            minute  = 0;
            second  = 0;

            b_result = SYS_TIME_SetRealTimeClock(year, month, day, hour, minute, second);
            assert(TRUE == b_result);
        }

        RULE_CTRL_LocalTimeBasedACLProcessTimerEvent();
        RULE_CTRL_UT_Proc_Validate_Rule();
    }

    //
    // Unbind all
    //

    for (i = 0; i < _countof(acls); ++ i)
    {
        UI32_T acl_index = acls[i].acl_index;

        for (j = 0; j < _countof(port_list); ++ j)
        {
            UI32_T ifindex = port_list[j];

            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
            {
                UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

                result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex, direction,
                                                      acl_index,
                                                      time_range_index,
                                                      counter_enable);
                assert(RULE_TYPE_OK == result);

            }
        }
    }

#endif // SYS_CPNT_TIME_BASED_ACL

    return 0;
}

#endif /* 0 */

int
RULE_CTRL_UT_Hot_Removal()
{
    enum
    {
        MAX_UNIT        = 1,
        MAX_PORT        = SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
        MAX_ACE         = 3,
        MIN_DIRECTION   = RULE_TYPE_INBOUND,

//#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
//        MAX_DIRECTION   = RULE_TYPE_OUTBOUND,
//#else
        MAX_DIRECTION   = RULE_TYPE_INBOUND,
//#endif // SYS_CPNT_QOS_V2_EGRESS_PORT
        MAX_RETRY_COUNT = 10,
    };

    const char *acl_name = "aa";
    const char *policy_map_name = "p1";
    const char *class_map_name = "c1";
    const char *acl_name_2 = "a1";

    UI32_T unit;
    UI32_T port;

    UI32_T acl_type = RULE_TYPE_IP_STD_ACL;
    UI32_T acl_index;
    UI32_T policy_map_index;

    UI32_T start_lport;
    UI32_T number_of_port;

    int    retry_count = MAX_RETRY_COUNT;

    RULE_TYPE_InOutDirection_T  direction;

    RULE_TYPE_RETURN_TYPE_T result;

//#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
//    MAX_DIRECTION = RULE_TYPE_OUTBOUND;
//#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

//    printf("%s, Something can not handle will when hotremoval !!!\r\n", __FUNCTION__);
//    return 0;

    RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acl_type, MAX_ACE,
                                             RULE_CTRL_UT_COMPRESS_DEFAULT,
                                             &acl_index);

    RULE_CTRL_UT_Proc_Create_Policy_Class_Map_With_Match_ACL(policy_map_name, class_map_name, acl_name_2);

    result = RULE_OM_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

retry:

    for (unit = 1; unit <= MAX_UNIT; ++unit)
    {
        for (port = 1; port <= MAX_PORT; ++port)
        {
            UI32_T ifindex = STKTPLG_OM_UPORT_TO_IFINDEX(unit, port);

            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++direction)
            {
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
                static BOOL_T show_warnning = TRUE;

                if (show_warnning)
                {
                    printf("%s, When SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT is enable, this item can not be pass\r\n", __FUNCTION__);
                    show_warnning = FALSE;
                }
#else
                result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                    acl_index,
                                                    RULE_TYPE_UNDEF_TIME_RANGE,
                                                    RULE_TYPE_COUNTER_DISABLE);
                assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

                RULE_CTRL_UT_Proc_Bind_Policy_Map(ifindex, direction, policy_map_index);
            }
        }
    }

    //
    //
    // reset device 0 on unit 1
    //
    //
    bcm_esw_field_init(0);
    DEVRM_Initial();

    start_lport = 1;
    number_of_port = MAX_PORT + 1;

    RULE_OM_HandleHotRemoval(start_lport, number_of_port);
    RULE_CTRL_HandleHotRemoval(start_lport, number_of_port);

    RULE_CTRL_UT_Proc_Validate_Rule();

    if (0 < retry_count--)
    {
        FP_CONFIG_Init();
        RULE_CTRL_EnterMasterMode();
        goto retry;
    }

    return 0;
}

int
RULE_CTRL_UT_Sort_Function_Type_List_By_Rule_Count()
{
    struct
    {
        RULE_TYPE_FunctionType_T    fun_type;
        UI32_T                      free_rule;
    } resource_information[] =
    {
        {RULE_TYPE_INGRESS_IP_ACL,          128},
        {RULE_TYPE_INGRESS_MAC_ACL,         100},
        {RULE_TYPE_INGRESS_IPV6_STD_ACL,    50},
        {RULE_TYPE_INGRESS_IPV6_EXT_ACL,    50},
    };

    RULE_TYPE_FunctionType_T fun_type_list[ _countof(resource_information) ];

    UI32_T i;

    UI32_T unit = 1;
    UI32_T device_id = 0;

    UI32_T group_id;
    int rule_pri;

    RULE_CTRL_ResourceInfo_T    *res_p;

    BOOL_T retry = TRUE;

    BOOL_T b_result;

again:
    for (i = 0; i < _countof(resource_information); ++i)
    {
        b_result = RULE_CTRL_GetFunctionInfoByFunctionType(resource_information[i].fun_type,
                                                           &group_id, &rule_pri);
        assert(TRUE == b_result);

        res_p = RULE_CTRL_GetResourceInfo(unit, device_id, group_id);
        assert(NULL != res_p);

        res_p->free_rule = resource_information[i].free_rule;
    }

    for (i = 0; i < _countof(resource_information); ++i)
    {
        fun_type_list[i] = resource_information[i].fun_type;
    }

    RULE_CTRL_Priv_SortByRuleCount(fun_type_list, _countof(fun_type_list), unit, device_id);

    for (i = 0; i < _countof(resource_information); ++i)
    {
        UI32_T j;

        b_result = RULE_CTRL_GetFunctionInfoByFunctionType(fun_type_list[i],
                                                           &group_id, &rule_pri);
        assert(TRUE == b_result);

        res_p = RULE_CTRL_GetResourceInfo(unit, device_id, group_id);
        assert(NULL != res_p);

        for (j=i+1; j < _countof(resource_information); ++j)
        {
            UI32_T tmp_group_id;
            int    tmp_rule_pri;

            RULE_CTRL_ResourceInfo_T    *tmp_res_p;

            b_result = RULE_CTRL_GetFunctionInfoByFunctionType(fun_type_list[j],
                                                               &tmp_group_id, &tmp_rule_pri);
            assert(TRUE == b_result);

            tmp_res_p = RULE_CTRL_GetResourceInfo(unit, device_id, tmp_group_id);
            assert(NULL != tmp_res_p);

            assert(res_p->free_rule <= tmp_res_p->free_rule);
        }
    }

    if (TRUE == retry)
    {
        resource_information[0].free_rule = 10;
        resource_information[1].free_rule = 20;
        resource_information[2].free_rule = 30;
        resource_information[3].free_rule = 40;

        retry = FALSE;

        goto again;
    }


    return 0;
}

int
RULE_CTRL_UT_Find_Best_Fit_Function_Type()
{
    struct
    {
        RULE_TYPE_FunctionType_T    fun_type;
        UI32_T                      free_rule;
    } resource_information[] =
    {
        {RULE_TYPE_INGRESS_IP_ACL,          128},
        {RULE_TYPE_INGRESS_MAC_ACL,         100},
        {RULE_TYPE_INGRESS_IPV6_STD_ACL,    50},
        {RULE_TYPE_INGRESS_IPV6_EXT_ACL,    50},
    };

    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_STD_ACL;

    const char *a30_name = "a30";
    UI32_T a30_index;

    const char *a50_name = "a50";
    UI32_T a50_index;

    const char *a70_name = "a70";
    UI32_T a70_index;

    const char *a100_name = "a100";
    UI32_T a100_index;

    const char *a130_name = "a130";
    UI32_T a130_index;

    UI32_T i;

    UI32_T unit = 1;
    UI32_T device_id = 0;
    RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

    RULE_TYPE_Acl_T acl_entry;

    RULE_TYPE_FunctionType_T out_fun_type;

    UI32_T group_id;
    int rule_pri;

    RULE_CTRL_ResourceInfo_T    *res_p;

    RULE_TYPE_RETURN_TYPE_T result;
    BOOL_T b_result;

    RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(a30_name, acl_type, 30, RULE_CTRL_UT_COMPRESS_DEFAULT, &a30_index);
    RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(a50_name, acl_type, 50, RULE_CTRL_UT_COMPRESS_DEFAULT, &a50_index);
    RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(a70_name, acl_type, 70, RULE_CTRL_UT_COMPRESS_DEFAULT, &a70_index);
    RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(a100_name, acl_type, 100, RULE_CTRL_UT_COMPRESS_DEFAULT, &a100_index);
    RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(a130_name, acl_type, 130, RULE_CTRL_UT_COMPRESS_DEFAULT, &a130_index);

    for (i = 0; i < _countof(resource_information); ++i)
    {
        b_result = RULE_CTRL_GetFunctionInfoByFunctionType(resource_information[i].fun_type,
                                                           &group_id, &rule_pri);
        assert(TRUE == b_result);

        res_p = RULE_CTRL_GetResourceInfo(unit, device_id, group_id);
        assert(NULL != res_p);

        res_p->free_rule = resource_information[i].free_rule;
    }

    ////////////////////////////////////////////////////////////////////////////

    result = RULE_OM_GetAclByIndex(a30_index, &acl_entry);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_FindBestFitFunctionType(unit, device_id, direction,
                                                    &acl_entry, &out_fun_type);
    assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    assert(RULE_TYPE_INGRESS_IPV6_STD_ACL == out_fun_type);
#else
    assert(RULE_TYPE_INGRESS_IP_ACL == out_fun_type);
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    ////////////////////////////////////////////////////////////////////////////

    result = RULE_OM_GetAclByIndex(a50_index, &acl_entry);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_FindBestFitFunctionType(unit, device_id, direction,
                                                    &acl_entry, &out_fun_type);
    assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    assert(RULE_TYPE_INGRESS_IPV6_STD_ACL == out_fun_type);
#else
    assert(RULE_TYPE_INGRESS_IP_ACL == out_fun_type);
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    ////////////////////////////////////////////////////////////////////////////

    result = RULE_OM_GetAclByIndex(a70_index, &acl_entry);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_FindBestFitFunctionType(unit, device_id, direction,
                                                    &acl_entry, &out_fun_type);
    assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    assert(RULE_TYPE_INGRESS_MAC_ACL == out_fun_type);
#else
    assert(RULE_TYPE_INGRESS_IP_ACL == out_fun_type);
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    ////////////////////////////////////////////////////////////////////////////

    result = RULE_OM_GetAclByIndex(a100_index, &acl_entry);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_FindBestFitFunctionType(unit, device_id, direction,
                                                    &acl_entry, &out_fun_type);
    assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    assert(RULE_TYPE_INGRESS_MAC_ACL == out_fun_type);
#else
    assert(RULE_TYPE_INGRESS_IP_ACL == out_fun_type);
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    ////////////////////////////////////////////////////////////////////////////

    result = RULE_OM_GetAclByIndex(a130_index, &acl_entry);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_FindBestFitFunctionType(unit, device_id, direction,
                                                    &acl_entry, &out_fun_type);
    assert(RULE_TYPE_OK != result);


    return 0;
}

int
RULE_CTRL_UT_Get_Next_Port()
{
    UI32_T port_list[] =
    {
        1,
        1 + 1,
        1 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
        1 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
    };

    UI8_T port_bitmap[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST] = {0};

    UI32_T i;
    UI32_T ifindex;

    for (i = 0; i < _countof(port_list); ++i)
    {
        RULE_CTRL_SET_BIT_ON(port_bitmap, (port_list[i]-1));
    }

    i = 0;
    ifindex = 0;
    while (TRUE == RULE_CTRL_Priv_GetNextPort(port_bitmap, &ifindex))
    {
        assert(ifindex == port_list[i ++]);
    }

    assert(_countof(port_list) == i);

    return 0;
}

int
RULE_CTRL_UT_Capacity_Test_For_ACL_List()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T *acl_index_list;
    UI32_T written_acl_count;

    UI32_T i;

    acl_index_list = calloc(SYS_ADPT_MAX_NBRS_OF_ACL, sizeof(UI32_T));
    assert (NULL != acl_index_list);

    written_acl_count = 0;

    for (i = 0; i < SYS_ADPT_MAX_NBRS_OF_ACL; ++ i)
    {
        UI32_T acl_index;

        char acl_name[10];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name,
                                                 RULE_TYPE_IP_STD_ACL,
                                                 1,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &acl_index);

        result = RULE_CTRL_Priv_LST_Set(acl_index_list, SYS_ADPT_MAX_NBRS_OF_ACL, &written_acl_count, acl_index);
        assert(RULE_TYPE_OK == result);
    }

    assert(written_acl_count == SYS_ADPT_MAX_NBRS_OF_ACL);

    free(acl_index_list);
    acl_index_list = NULL;
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

int
RULE_CTRL_UT_Capacity_And_Order_Test_For_ACL_List()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T *acl_index_list;
    UI32_T written_acl_count;
    UI32_T first_acl_index;

    UI32_T i;

    acl_index_list = calloc(SYS_ADPT_MAX_NBRS_OF_ACL, sizeof(UI32_T));
    assert (NULL != acl_index_list);

    written_acl_count = 0;

    for (i = 0; i < SYS_ADPT_MAX_NBRS_OF_ACL; ++ i)
    {
        UI32_T acl_index;

        char acl_name[10];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name,
                                                 RULE_TYPE_IP_STD_ACL,
                                                 (i < SYS_ADPT_MAX_NBRS_OF_ACL - 1) ? 1 : 2,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &acl_index);
        if (i == SYS_ADPT_MAX_NBRS_OF_ACL - 1)
        {
            first_acl_index = acl_index;
        }

        result = RULE_CTRL_Priv_LST_Set(acl_index_list, SYS_ADPT_MAX_NBRS_OF_ACL, &written_acl_count, acl_index);
        assert(RULE_TYPE_OK == result);
    }

    assert(written_acl_count == SYS_ADPT_MAX_NBRS_OF_ACL);
    assert(acl_index_list[0] == first_acl_index);

    free(acl_index_list);    
    acl_index_list = NULL;
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

int
RULE_CTRL_UT_Get_Acl_Id_List_Using_Identical_Function_Type_Same_ACE_Count()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)

    enum
    {
        MIN_IFINDEX = 1,
        MAX_IFINDEX = 3,
        DIRECTION = RULE_TYPE_INBOUND,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ifindex;
    RULE_TYPE_InOutDirection_T direction = DIRECTION;

    UI32_T g_acl_index = 0;

    for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
    {
        UI32_T acl_index;

        char acl_name[10];

        snprintf(acl_name, sizeof(acl_name), "a%lu", ifindex);

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name,
                                                 RULE_TYPE_IP_STD_ACL,
                                                 2,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &acl_index);

        if (0 == g_acl_index)
        {
            g_acl_index = acl_index;
        }

        {
            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = TRUE;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
            {
                RULE_TYPE_Acl_T *acl_entry_p;

                result = RULE_OM_RebuildAggregateAcl(acl_index);
                assert(RULE_TYPE_OK == result);

                acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_index);
                assert(NULL != acl_entry_p);

                acl_index = acl_entry_p->aggregate_acl_index;
            }
#endif
            result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                acl_index,
                                                time_range_index,
                                                counter_enable);
            assert(RULE_TYPE_OK == result);
        }
    }

    {
        UI32_T *acl_index_list;
        UI32_T written_acl_count;

        acl_index_list = calloc(SYS_ADPT_MAX_NBRS_OF_ACL, sizeof(UI32_T));
        assert (NULL != acl_index_list);

        result = RULE_CTRL_Priv_GetAclIdListUseSameFunctionType(direction, g_acl_index,
                  acl_index_list, SYS_ADPT_MAX_NBRS_OF_ACL, &written_acl_count);

        assert(written_acl_count ==(MAX_IFINDEX - MIN_IFINDEX + 1));

        free(acl_index_list);
        acl_index_list = NULL;

    }
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT
    return 0;
}

int RULE_CTRL_UT_Get_Acl_Id_List_Using_Identical_Function_Type()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    RULE_TYPE_RETURN_TYPE_T     result;

    UI32_T                      ifindex;
    RULE_TYPE_InOutDirection_T  direction;

    UI16_T                      time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
    RULE_TYPE_COUNTER_ENABLE_T  counter_enable = RULE_TYPE_COUNTER_DISABLE;

    ifindex = 1;
    direction = RULE_TYPE_INBOUND;

    {
        UI32_T acl_index;
        RULE_TYPE_AclType_T acl_type;
        char acl_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        RULE_TYPE_Ace_Entry_T ace_entry;

        strcpy(acl_name, "m-1");

        acl_type = RULE_TYPE_MAC_ACL;
        RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.access = RULE_TYPE_ACE_PERMIT;
        SET_MAC(ace_entry.u.mac.aceSourceMacAddr, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06);
        SET_MAC(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                            acl_index,
                                            time_range_index,
                                            counter_enable);
        assert(RULE_TYPE_OK == result);

        //
        //
        //
        ifindex ++;

        strcpy(acl_name, "m-3");

        acl_type = RULE_TYPE_MAC_ACL;
        RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.access = RULE_TYPE_ACE_PERMIT;
        SET_MAC(ace_entry.u.mac.aceSourceMacAddr, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06);
        SET_MAC(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.access = RULE_TYPE_ACE_DENY;
        SET_MAC(ace_entry.u.mac.aceSourceMacAddr, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07);
        SET_MAC(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.access = RULE_TYPE_ACE_PERMIT;
        SET_MAC(ace_entry.u.mac.aceSourceMacAddr, 0x01, 0x02, 0x03, 0x04, 0x05, 0x08);
        SET_MAC(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                            acl_index,
                                            time_range_index,
                                            counter_enable);
        assert(RULE_TYPE_OK == result);

        //
        //
        //
        ifindex ++;

        strcpy(acl_name, "i4s-1");

        acl_type = RULE_TYPE_IP_STD_ACL;
        RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.access = RULE_TYPE_ACE_PERMIT;
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddr, 192, 168, 1, 1);
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                            acl_index,
                                            time_range_index,
                                            counter_enable);
        assert(RULE_TYPE_OK == result);

        //
        //
        //
        ifindex ++;

        strcpy(acl_name, "i4e-2");

        acl_type = RULE_TYPE_IP_EXT_ACL;
        RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.access = RULE_TYPE_ACE_PERMIT;
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddr, 192, 168, 1, 1);
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.access = RULE_TYPE_ACE_DENY;
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddr, 192, 168, 1, 2);
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                            acl_index,
                                            time_range_index,
                                            counter_enable);
        assert(RULE_TYPE_OK == result);

        //
        //
        //
        ifindex ++;

        strcpy(acl_name, "i4s-3");

        acl_type = RULE_TYPE_IP_EXT_ACL;
        RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

        ace_entry.ace_type = acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.access = RULE_TYPE_ACE_PERMIT;
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddr, 192, 168, 1, 1);
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.access = RULE_TYPE_ACE_DENY;
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddr, 192, 168, 1, 2);
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        ace_entry.access = RULE_TYPE_ACE_DENY;
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddr, 192, 168, 1, 3);
        SET_IPV4(ace_entry.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

        result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                            acl_index,
                                            time_range_index,
                                            counter_enable);
        assert(RULE_TYPE_OK == result);

        ifindex ++;

    }

    {
        UI32_T acl_id;
        UI32_T acl_index_2;

        UI32_T *acl_index_list;
        UI32_T written_acl_count;
        UI32_T i;

        acl_index_list = calloc(SYS_ADPT_MAX_NBRS_OF_ACL, sizeof(UI32_T));
        assert (NULL != acl_index_list);

        result = RULE_OM_GetAclIdByName("m-1", &acl_id);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_Priv_GetAclIdListUseSameFunctionType(direction, acl_id,
                  acl_index_list, SYS_ADPT_MAX_NBRS_OF_ACL, &written_acl_count);

        ////////////////////////////////////////////////////////////////////////

        result = RULE_OM_GetAclIdByName("m-3", &acl_index_2);
        assert(RULE_TYPE_OK == result);

        i = 0;
        assert(acl_index_list[i++] == acl_index_2);

        ////////////////////////////////////////////////////////////////////////

        result = RULE_OM_GetAclIdByName("m-1", &acl_index_2);
        assert(RULE_TYPE_OK == result);

        assert(acl_index_list[i++] == acl_index_2);

        ////////////////////////////////////////////////////////////////////////
        assert(acl_index_list[i] == 0);
        assert(written_acl_count == 2);

        free(acl_index_list);
        acl_index_list = NULL;
    }

    {
        UI32_T acl_id;
        UI32_T acl_index_2;

        UI32_T *acl_index_list;
        UI32_T written_acl_count;
        UI32_T i;

        acl_index_list = calloc(SYS_ADPT_MAX_NBRS_OF_ACL, sizeof(UI32_T));
        assert (NULL != acl_index_list);

        result = RULE_OM_GetAclIdByName("m-3", &acl_id);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_Priv_GetAclIdListUseSameFunctionType(direction, acl_id,
                  acl_index_list, SYS_ADPT_MAX_NBRS_OF_ACL, &written_acl_count);

        ////////////////////////////////////////////////////////////////////////

        i = 0;

        result = RULE_OM_GetAclIdByName("m-3", &acl_index_2);
        assert(RULE_TYPE_OK == result);

        assert(acl_index_list[i++] == acl_index_2);

        ////////////////////////////////////////////////////////////////////////

        result = RULE_OM_GetAclIdByName("m-1", &acl_index_2);
        assert(RULE_TYPE_OK == result);

        assert(acl_index_list[i++] == acl_index_2);

        ////////////////////////////////////////////////////////////////////////
        assert(acl_index_list[i++] == 0);
        assert(written_acl_count == 2);

        free(acl_index_list);
        acl_index_list = NULL;
    }

    {
        UI32_T acl_id;
        UI32_T acl_index_2;

        UI32_T *acl_index_list;
        UI32_T written_acl_count;
        UI32_T i;

        acl_index_list = calloc(SYS_ADPT_MAX_NBRS_OF_ACL, sizeof(UI32_T));
        assert (NULL != acl_index_list);

        result = RULE_OM_GetAclIdByName("i4e-2", &acl_id);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_Priv_GetAclIdListUseSameFunctionType(direction, acl_id,
                  acl_index_list, SYS_ADPT_MAX_NBRS_OF_ACL, &written_acl_count);

        ////////////////////////////////////////////////////////////////////////

        i = 0;

        result = RULE_OM_GetAclIdByName("i4s-3", &acl_index_2);
        assert(RULE_TYPE_OK == result);

        assert(acl_index_list[i++] == acl_index_2);

        ////////////////////////////////////////////////////////////////////////

        result = RULE_OM_GetAclIdByName("i4e-2", &acl_index_2);
        assert(RULE_TYPE_OK == result);

        assert(acl_index_list[i++] == acl_index_2);

        ////////////////////////////////////////////////////////////////////////

        result = RULE_OM_GetAclIdByName("i4s-1", &acl_index_2);
        assert(RULE_TYPE_OK == result);

        assert(acl_index_list[i++] == acl_index_2);

        ////////////////////////////////////////////////////////////////////////
        assert(acl_index_list[i++] == 0);


        free(acl_index_list);
        acl_index_list = NULL;
    }

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

int
RULE_CTRL_UT_Defrag_Rule()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    enum
    {
        TOTAL_NUMBER_OF_PORT = 4
    };

    struct
    {
        char *acl_name;
        RULE_TYPE_AclType_T acl_type;
        UI32_T ace_count;

        UI32_T port_list[TOTAL_NUMBER_OF_PORT];

        RULE_TYPE_FunctionType_T before_fun_type;
        RULE_TYPE_FunctionType_T after_fun_type;

        UI32_T acl_index;
    } acl_profiles[] =
    {
        {
            .acl_name = "a28",
            .acl_type = RULE_TYPE_IP_STD_ACL,
            .ace_count = 28,
            .port_list =
            {
                1,
                1 + 1,
                1 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                1 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
            },
            .before_fun_type = RULE_TYPE_INGRESS_MAC_ACL,
            .after_fun_type = RULE_TYPE_INGRESS_IP_ACL,
        },

        {
            .acl_name = "a30",
            .acl_type = RULE_TYPE_IP_STD_ACL,
            .ace_count = 30,
            .port_list =
            {
                3,
                3 + 1,
                3 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                3 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
            },
            .before_fun_type = RULE_TYPE_INGRESS_MAC_ACL,
            .after_fun_type = RULE_TYPE_INGRESS_MAC_ACL,
        },

        {
            .acl_name = "a40",
            .acl_type = RULE_TYPE_IP_EXT_ACL,
            .ace_count = 40,
            .port_list =
            {
                5,
                5 + 1,
                5 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                5 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
            },
            .before_fun_type = RULE_TYPE_INGRESS_MAC_ACL,
            .after_fun_type = RULE_TYPE_INGRESS_MAC_ACL,
        },

        {
            .acl_name = "a50",
            .acl_type = RULE_TYPE_IP_STD_ACL,
            .ace_count = 50,
            .port_list =
            {
                7,
                7 + 1,
                7 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                7 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
            },
            .before_fun_type = RULE_TYPE_INGRESS_IP_ACL,
            .after_fun_type = RULE_TYPE_INGRESS_MAC_ACL,
        },
    };

    UI32_T i, j;

    RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

    RULE_TYPE_RETURN_TYPE_T result;

    for (i = 0; i < _countof(acl_profiles); ++i)
    {
        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_profiles[i].acl_name,
                                                 acl_profiles[i].acl_type,
                                                 acl_profiles[i].ace_count,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &acl_profiles[i].acl_index);

        for (j = 0; j < _countof(acl_profiles[i].port_list); ++j)
        {
            UI32_T ifindex = acl_profiles[i].port_list[j];

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                acl_profiles[i].acl_index,
                                                time_range_index,
                                                counter_enable);
            assert(RULE_TYPE_OK == result);

            {
                RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p;

                RULE_TYPE_INTERFACE_INFO_T interface;

                RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                interface.direction = direction;

                class_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_profiles[i].acl_index);
                assert(NULL != class_inst_p);
                assert(RULE_TYPE_INGRESS_IP_ACL == class_inst_p->fun_type);
                assert(acl_profiles[i].before_fun_type == class_inst_p->remap_fun_type);
            }
        }
    }

    {
        result = RULE_CTRL_Priv_DefragRule(direction,
                                           acl_profiles[0].acl_index,
                                           NULL,
                                           NULL,
                                           NULL);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();

        for (i = 0; i < _countof(acl_profiles); ++i)
        {
            for (j = 0; j < _countof(acl_profiles[i].port_list); ++j)
            {
                UI32_T ifindex = acl_profiles[i].port_list[j];

                {
                    RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p;

                    RULE_TYPE_INTERFACE_INFO_T interface;

                    RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                    interface.direction = direction;

                    class_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_profiles[i].acl_index);
                    assert(NULL != class_inst_p);
                    assert(RULE_TYPE_INGRESS_IP_ACL == class_inst_p->fun_type);
                    assert(acl_profiles[i].after_fun_type == class_inst_p->remap_fun_type);
                }
            }
        }
    }

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

int
RULE_CTRL_UT_Defrag_Rule_With_Add_ACL()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    enum
    {
        TOTAL_NUMBER_OF_PORT = 4
    };

    struct
    {
        char *acl_name;
        RULE_TYPE_AclType_T acl_type;
        UI32_T ace_count;

        UI32_T port_list[TOTAL_NUMBER_OF_PORT];

        RULE_TYPE_FunctionType_T before_fun_type;
        RULE_TYPE_FunctionType_T after_fun_type;

        UI32_T acl_index;
    } acl_profiles[] =
    {
        {
            .acl_name = "a30",
            .acl_type = RULE_TYPE_IP_STD_ACL,
            .ace_count = 30,
            .port_list =
            {
                1,
                1 + 1,
                1 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                1 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
            },
            .before_fun_type = RULE_TYPE_INGRESS_IPV6_STD_ACL,
            .after_fun_type = RULE_TYPE_INGRESS_IPV6_STD_ACL,
        },

        {
            .acl_name = "a40",
            .acl_type = RULE_TYPE_IP_STD_ACL,
            .ace_count = 40,
            .port_list =
            {
                3,
                3 + 1,
                3 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                3 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
            },
            .before_fun_type = RULE_TYPE_INGRESS_IPV6_STD_ACL,
            .after_fun_type = RULE_TYPE_INGRESS_IPV6_STD_ACL,
        },

        {
            .acl_name = "a58",
            .acl_type = RULE_TYPE_IP_EXT_ACL,
            .ace_count = 58,
            .port_list =
            {
                5,
                5 + 1,
                5 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                5 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
            },
            .before_fun_type = RULE_TYPE_INGRESS_IPV6_STD_ACL,
            .after_fun_type = RULE_TYPE_INGRESS_IPV6_STD_ACL,
        },
    };

    UI32_T i, j;

    RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

    RULE_TYPE_RETURN_TYPE_T result;

    //
    // Create other type of ACL to limit the testing ACL can use two function
    // type
    //

    {
        const char *acl_ip6_std_name = "ip6-std";
        const char *acl_ip6_ext_name = "ip6-ext";
        UI32_T acl_index;

        // Use RULE_TYPE_INGRESS_MAC_ACL
        RULE_CTRL_UT_Proc_Create_ACL(acl_ip6_std_name, RULE_TYPE_IPV6_STD_ACL, &acl_index);

        // Use RULE_TYPE_INGRESS_IP_ACL
        RULE_CTRL_UT_Proc_Create_ACL(acl_ip6_ext_name, RULE_TYPE_IPV6_EXT_ACL, &acl_index);
    }


    for (i = 0; i < _countof(acl_profiles); ++i)
    {
        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_profiles[i].acl_name,
                                                 acl_profiles[i].acl_type,
                                                 acl_profiles[i].ace_count,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &acl_profiles[i].acl_index);

        for (j = 0; j < _countof(acl_profiles[i].port_list); ++j)
        {
            UI32_T ifindex = acl_profiles[i].port_list[j];

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                acl_profiles[i].acl_index,
                                                time_range_index,
                                                counter_enable);
            assert(RULE_TYPE_OK == result);

            assert(RULE_TYPE_IS_UPORT(ifindex));

            {
                RULE_TYPE_CLASS_INSTANCE_PTR_T              class_inst_p;

                RULE_TYPE_INTERFACE_INFO_T interface;

                RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                interface.direction = direction;

                class_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_profiles[i].acl_index);
                assert(NULL != class_inst_p);

                assert(RULE_TYPE_INGRESS_IP_ACL == class_inst_p->fun_type);
                assert(acl_profiles[i].before_fun_type == class_inst_p->remap_fun_type);

            }

            RULE_CTRL_UT_Proc_Validate_Rule();
        }
    }

    {
        const char *new_acl_name = "a35";
        RULE_TYPE_AclType_T new_acl_type = RULE_TYPE_IP_EXT_ACL;
        UI32_T new_acl_index;
        UI32_T ifindex = 7;
        UI32_T unit = STKTPLG_OM_IFINDEX_TO_UNIT(ifindex);
        UI32_T port = STKTPLG_OM_IFINDEX_TO_PORT(ifindex);
        BOOL_T is_ingress = RULE_TYPE_DIRECTION_TO_IS_INGRESS(direction);
        UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(new_acl_name, new_acl_type, 35,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                                &new_acl_index);

        {
            RULE_CTRL_WRAP_FN_PARAM_T cookie;

            cookie.is_ingress = is_ingress;
            cookie.unit = unit;
            cookie.port = port;
            cookie.acl_id = new_acl_index;
            cookie.time_range_index = time_range_index;
            cookie.counter_enable = counter_enable;

            result = RULE_CTRL_Priv_DefragRule(direction, new_acl_index,
                                               RULE_CTRL_Priv_Wrap_SetACL2DEV,
                                               NULL,
                                               &cookie);
            assert(RULE_TYPE_OK == result);

            result = RULE_OM_SetAcl(ifindex, direction, new_acl_index,
                                              time_range_index, counter_enable);
            assert(RULE_TYPE_OK == result);

            RULE_CTRL_UT_Proc_Validate_Rule();
        }

        {
            RULE_TYPE_CLASS_INSTANCE_PTR_T              class_inst_p;

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
            interface.direction = direction;

            class_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, new_acl_index);
            assert(NULL != class_inst_p);
            assert(RULE_TYPE_INGRESS_IP_ACL == class_inst_p->fun_type);
            assert(RULE_TYPE_INGRESS_IPV6_EXT_ACL == class_inst_p->remap_fun_type);
        }

        ////////////////////////////////////////////////////////////////////////
        //
        // If the ACL ID is already in ACL ID list
        //
        ////////////////////////////////////////////////////////////////////////

        ifindex += SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP;
        port += SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP;

        {
            RULE_CTRL_WRAP_FN_PARAM_T cookie;

            cookie.is_ingress = is_ingress;
            cookie.unit = unit;
            cookie.port = port;
            cookie.acl_id = new_acl_index;
            cookie.time_range_index = time_range_index;
            cookie.counter_enable = counter_enable;

            result = RULE_CTRL_Priv_DefragRule(direction, new_acl_index,
                                               RULE_CTRL_Priv_Wrap_SetACL2DEV,
                                               NULL,
                                               &cookie);
            assert(RULE_TYPE_OK == result);

            result = RULE_OM_SetAcl(ifindex, direction, new_acl_index,
                                              time_range_index, counter_enable);            

            RULE_CTRL_UT_Proc_Validate_Rule();
        }

        {
            RULE_TYPE_CLASS_INSTANCE_PTR_T              class_inst_p;

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
            interface.direction = direction;

            class_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, new_acl_index);
            assert(NULL != class_inst_p);
            assert(RULE_TYPE_INGRESS_IP_ACL == class_inst_p->fun_type);
            assert(RULE_TYPE_INGRESS_IPV6_EXT_ACL == class_inst_p->remap_fun_type);
        }

        ////////////////////////////////////////////////////////////////////////
    }

    {
        const char *new_acl_name = "a100";
        RULE_TYPE_AclType_T new_acl_type = RULE_TYPE_IP_EXT_ACL;
        UI32_T new_acl_index;
        UI32_T ifindex = 8;
        UI32_T unit = STKTPLG_OM_IFINDEX_TO_UNIT(ifindex);
        UI32_T port = STKTPLG_OM_IFINDEX_TO_PORT(ifindex);
        BOOL_T is_ingress = RULE_TYPE_DIRECTION_TO_IS_INGRESS(direction);
        UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(new_acl_name, new_acl_type, 100,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                                &new_acl_index);

        {
            RULE_CTRL_WRAP_FN_PARAM_T cookie;

            cookie.is_ingress = is_ingress;
            cookie.unit = unit;
            cookie.port = port;
            cookie.acl_id = new_acl_index;
            cookie.time_range_index = time_range_index;
            cookie.counter_enable = counter_enable;

            result = RULE_CTRL_Priv_DefragRule(direction, new_acl_index,
                                               RULE_CTRL_Priv_Wrap_SetACL2DEV,
                                               NULL,
                                               &cookie);
            assert(RULE_TYPE_OK != result);

            RULE_CTRL_UT_Proc_Validate_Rule();
        }
    }

    for (i = 0; i < _countof(acl_profiles); ++i)
    {
        for (j = 0; j < _countof(acl_profiles[i].port_list); ++j)
        {
            UI32_T ifindex = acl_profiles[i].port_list[j];

            {
                RULE_TYPE_CLASS_INSTANCE_PTR_T              class_inst_p;

                RULE_TYPE_INTERFACE_INFO_T interface;

                RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                interface.direction = direction;

                class_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_profiles[i].acl_index);
                assert(NULL != class_inst_p);
                assert(RULE_TYPE_INGRESS_IP_ACL == class_inst_p->fun_type);
                assert(acl_profiles[i].after_fun_type == class_inst_p->remap_fun_type);
            }
        }
    }

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

int
RULE_CTRL_UT_Defrag_Rule_With_Add_ACE_On_Fly()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    enum
    {
        TOTAL_NUMBER_OF_PORT = 4
    };

    struct
    {
        char *acl_name;
        RULE_TYPE_AclType_T acl_type;
        UI32_T ace_count;

        UI32_T port_list[TOTAL_NUMBER_OF_PORT];

        RULE_TYPE_FunctionType_T before_fun_type;
        RULE_TYPE_FunctionType_T after_fun_type;

        UI32_T acl_index;
    } acl_profiles[] =
    {
        {
            .acl_name = "a30",
            .acl_type = RULE_TYPE_IP_STD_ACL,
            .ace_count = 30,
            .port_list =
            {
                1,
                1 + 1,
                1 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                1 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
            },
            .before_fun_type = RULE_TYPE_INGRESS_MAC_ACL,
            .after_fun_type = RULE_TYPE_INGRESS_IP_ACL,
        },

        {
            .acl_name = "a40",
            .acl_type = RULE_TYPE_IP_STD_ACL,
            .ace_count = 40,
            .port_list =
            {
                3,
                3 + 1,
                3 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                3 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
            },
            .before_fun_type = RULE_TYPE_INGRESS_MAC_ACL,
            .after_fun_type = RULE_TYPE_INGRESS_MAC_ACL,
        },

        {
            .acl_name = "a58",
            .acl_type = RULE_TYPE_IP_EXT_ACL,
            .ace_count = 58,
            .port_list =
            {
                5,
                5 + 1,
                5 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                5 + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
            },
            .before_fun_type = RULE_TYPE_INGRESS_MAC_ACL,
            .after_fun_type = RULE_TYPE_INGRESS_MAC_ACL,
        },
    };

    UI32_T i, j;

    RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

    RULE_TYPE_RETURN_TYPE_T result;
    BOOL_T b_result;

    for (i = 0; i < _countof(acl_profiles); ++i)
    {
        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_profiles[i].acl_name,
                                                 acl_profiles[i].acl_type,
                                                 acl_profiles[i].ace_count,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &acl_profiles[i].acl_index);

        for (j = 0; j < _countof(acl_profiles[i].port_list); ++j)
        {
            UI32_T ifindex = acl_profiles[i].port_list[j];

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                acl_profiles[i].acl_index,
                                                time_range_index,
                                                counter_enable);
            assert(RULE_TYPE_OK == result);

            {
                RULE_TYPE_CLASS_INSTANCE_PTR_T              class_inst_p;

                RULE_TYPE_INTERFACE_INFO_T interface;

                RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                interface.direction = direction;

                class_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_profiles[i].acl_index);
                assert(NULL != class_inst_p);
                assert(RULE_TYPE_INGRESS_IP_ACL == class_inst_p->fun_type);
                assert(acl_profiles[i].before_fun_type == class_inst_p->remap_fun_type);
            }
        }
    }

    for (i = 0; i < _countof(acl_profiles); ++i)
    {
        RULE_TYPE_Acl_T acl_entry;
        RULE_TYPE_Ace_Entry_T ace_entry;

        ace_entry.ace_type = acl_profiles[i].acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_AddAce2Acl(acl_profiles[i].acl_index, acl_profiles[i].acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclByIndex(acl_profiles[i].acl_index, &acl_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_CTRL_LocalAddAce2DEV(acl_profiles[i].acl_index, &acl_entry, &ace_entry);
        assert(RULE_TYPE_INSUFFICIENT_RESOURCE == result);

        result = RULE_OM_DelAceFromAcl(acl_profiles[i].acl_index, acl_profiles[i].acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);
    }

    //
    // Add ACE into the ACL which have 30 ACE.
    // It will cause rule defragment
    //
    for (i = 0; i < _countof(acl_profiles); ++i)
    {
        RULE_TYPE_Acl_T acl_entry;
        RULE_TYPE_Ace_Entry_T ace_entry;
        BOOL_T is_add = TRUE;

        ace_entry.ace_type = acl_profiles[i].acl_type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_AddAce2Acl(acl_profiles[i].acl_index, acl_profiles[i].acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclByIndex(acl_profiles[i].acl_index, &acl_entry);
        assert(RULE_TYPE_OK == result);

        if (0 == i)
        {
            result = RULE_CTRL_LocalAddAce2DEV(acl_profiles[i].acl_index, &acl_entry, &ace_entry);

            assert(RULE_TYPE_INSUFFICIENT_RESOURCE == result);

            b_result = RULE_CTRL_SetAce(acl_profiles[i].acl_index, &ace_entry, is_add);
            assert(TRUE == b_result);
        }
        else
        {
            b_result = RULE_CTRL_SetAce(acl_profiles[i].acl_index, &ace_entry, is_add);
            assert(TRUE == b_result);
        }
    }

    for (i = 0; i < _countof(acl_profiles); ++i)
    {
        for (j = 0; j < _countof(acl_profiles[i].port_list); ++j)
        {
            UI32_T ifindex = acl_profiles[i].port_list[j];

            {
                RULE_TYPE_CLASS_INSTANCE_PTR_T              class_inst_p;

                RULE_TYPE_INTERFACE_INFO_T interface;

                RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                interface.direction = direction;

                class_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_profiles[i].acl_index);
                assert(NULL != class_inst_p);
                assert(RULE_TYPE_INGRESS_IP_ACL == class_inst_p->fun_type);
                assert(acl_profiles[i].after_fun_type == class_inst_p->remap_fun_type);
            }
        }
    }

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

int
RULE_CTRL_UT_Get_Available_Function_Type()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    const char *acl_name_mac = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
    RULE_TYPE_Acl_T acl_entry_mac;
    RULE_TYPE_InOutDirection_T  direction = RULE_TYPE_INBOUND;

    const char *acl_name_2 = "a2";
    const char *acl_name_3 = "a3";

    UI32_T acl_index_mac;
    UI32_T acl_index_2;
    UI32_T acl_index_3;

    RULE_TYPE_FunctionType_T fun_stats[ RULE_TYPE_FUNCTION_TYPE_MAX ];
    UI32_T number_of_entries_written;

    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_OM_CreateAcl(acl_name_mac, acl_type);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_mac, &acl_index_mac);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclByIndex(acl_index_mac, &acl_entry_mac);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_mac,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(4 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_MAC_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IP_ACL);
    assert(fun_stats[2] == RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(fun_stats[3] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    result = RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                RULE_TYPE_INGRESS_MAC_ACL);
    assert(RULE_TYPE_OK == result);

    ////////////////////////////////////////////////////////////////////////////
    // Create an IP Std ACL
    //
    result = RULE_OM_CreateAcl(acl_name_2, RULE_TYPE_IP_STD_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_2, &acl_index_2);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                RULE_TYPE_INGRESS_IP_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_mac,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(3 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_MAC_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(fun_stats[2] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    result = RULE_OM_DelAcl(acl_index_2);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_DisableFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                 RULE_TYPE_INGRESS_IP_ACL);
    assert(RULE_TYPE_OK == result);

    ////////////////////////////////////////////////////////////////////////////
    // Create an IP Ext ACL
    //
    result = RULE_OM_CreateAcl(acl_name_2, RULE_TYPE_IP_EXT_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_2, &acl_index_2);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                RULE_TYPE_INGRESS_IP_ACL);
    assert(RULE_TYPE_OK == result);


    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_mac,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(3 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_MAC_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(fun_stats[2] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    result = RULE_OM_DelAcl(acl_index_2);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_DisableFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                 RULE_TYPE_INGRESS_IP_ACL);
    assert(RULE_TYPE_OK == result);

    ////////////////////////////////////////////////////////////////////////////
    // Create IPv6 Std ACL
    //
    result = RULE_OM_CreateAcl(acl_name_2, RULE_TYPE_IPV6_STD_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_2, &acl_index_2);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_STD_ACL,
                                                RULE_TYPE_INGRESS_IPV6_EXT_ACL);
    assert(RULE_TYPE_OK == result);


    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_mac,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(3 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_MAC_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IP_ACL);
    assert(fun_stats[2] == RULE_TYPE_INGRESS_IPV6_STD_ACL);

    result = RULE_OM_DelAcl(acl_index_2);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_DisableFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_STD_ACL,
                                                 RULE_TYPE_INGRESS_IPV6_EXT_ACL);
    assert(RULE_TYPE_OK == result);

    ////////////////////////////////////////////////////////////////////////////
    // Create IPv6 Ext ACL
    //
    result = RULE_OM_CreateAcl(acl_name_2, RULE_TYPE_IPV6_EXT_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_2, &acl_index_2);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_EXT_ACL,
                                                RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_mac,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(3 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_MAC_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IP_ACL);
    assert(fun_stats[2] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    result = RULE_OM_DelAcl(acl_index_2);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_DisableFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_EXT_ACL,
                                                 RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(RULE_TYPE_OK == result);

    ////////////////////////////////////////////////////////////////////////////
    // Create IP Std + IPv6 Std
    //
    result = RULE_OM_CreateAcl(acl_name_2, RULE_TYPE_IP_STD_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_2, &acl_index_2);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                RULE_TYPE_INGRESS_IP_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_CreateAcl(acl_name_3, RULE_TYPE_IPV6_STD_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_3, &acl_index_3);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_STD_ACL,
                                                RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(RULE_TYPE_OK == result);


    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_mac,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(2 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_MAC_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    result = RULE_OM_DelAcl(acl_index_2);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_DisableFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                 RULE_TYPE_INGRESS_IP_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_DelAcl(acl_index_3);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_DisableFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_STD_ACL,
                                                 RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(RULE_TYPE_OK == result);

    ////////////////////////////////////////////////////////////////////////////
    // IP Std + IP Ext
    //
    result = RULE_OM_CreateAcl(acl_name_2, RULE_TYPE_IP_STD_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_2, &acl_index_2);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                RULE_TYPE_INGRESS_IP_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_CreateAcl(acl_name_3, RULE_TYPE_IP_EXT_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_3, &acl_index_3);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_mac,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(3 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_MAC_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(fun_stats[2] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    result = RULE_OM_DelAcl(acl_index_2);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_DelAcl(acl_index_3);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_DisableFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                 RULE_TYPE_INGRESS_IP_ACL);
    assert(RULE_TYPE_OK == result);

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT
    return 0;
}

//
// ACL         Fun Type
// -------     --------
// MAC ACL --> MAC, IP
// IP Std  --> IP6 Std
// IP Ext  --> IP6 Std
// IP6 Std --> IP6 Ext
//
int
RULE_CTRL_UT_Get_Available_Function_Type_With_FunctionRemap()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    RULE_TYPE_Acl_T acl_entry_mac;
    RULE_TYPE_Acl_T acl_entry_ip_std;
    RULE_TYPE_Acl_T acl_entry_ip_ext;
    RULE_TYPE_Acl_T acl_entry_ip6_std;

    const char *acl_name_mac        = "mac";
    const char *acl_name_ip_std     = "ip-std";
    const char *acl_name_ip_ext     = "ip-ext";
    const char *acl_name_ip6_std    = "ip6-std";

    UI32_T acl_index_mac;
    UI32_T acl_index_ip_std;
    UI32_T acl_index_ip_ext;
    UI32_T acl_index_ip6_std;

    RULE_TYPE_FunctionType_T fun_stats[ RULE_TYPE_FUNCTION_TYPE_MAX ];
    RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;
    UI32_T number_of_entries_written;

    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_OM_CreateAcl(acl_name_mac, RULE_TYPE_MAC_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_mac, &acl_index_mac);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclByIndex(acl_index_mac, &acl_entry_mac);
    assert(RULE_TYPE_OK == result);

    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_mac,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(4 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_MAC_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IP_ACL);
    assert(fun_stats[2] == RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(fun_stats[3] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    //
    // Remap MAC to IP
    //
    RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                       RULE_TYPE_INGRESS_MAC_ACL);

    RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                       RULE_TYPE_INGRESS_IP_ACL);

    RULE_CTRL_Priv_ChangeGroupQualifySet(RULE_TYPE_INGRESS_MAC_ACL,
                                         RULE_TYPE_INGRESS_IP_ACL);
    RULE_CTRL_Priv_ChangeGroupPriority(RULE_TYPE_INGRESS_MAC_ACL,
                                       RULE_TYPE_INGRESS_IP_ACL);

    RULE_CTRL_UT_Proc_Validate_FuncType_Remapped_Table();
    RULE_CTRL_UT_Proc_Validate_Group_Qualify_Set();
    RULE_CTRL_UT_Proc_Validate_Group_Priority();

    ////////////////////////////////////////////////////////////////////////////
    // Create IP Std and Query
    //
    result = RULE_OM_CreateAcl(acl_name_ip_std, RULE_TYPE_IP_STD_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_ip_std, &acl_index_ip_std);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclByIndex(acl_index_ip_std, &acl_entry_ip_std);
    assert(RULE_TYPE_OK == result);

    //
    // Query for MAC ACL
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_mac,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(4 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_MAC_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IP_ACL);
    assert(fun_stats[2] == RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(fun_stats[3] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    //
    // Query for IP ACL. It can use IPv6 std/ext only.
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_ip_std,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(2 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    //
    // Remap IP to IP6 Std
    //
    RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                       RULE_TYPE_INGRESS_IPV6_STD_ACL);

    RULE_CTRL_Priv_ChangeGroupQualifySet(RULE_TYPE_INGRESS_IP_ACL,
                                         RULE_TYPE_INGRESS_IPV6_STD_ACL);

    RULE_CTRL_Priv_ChangeGroupPriority(RULE_TYPE_INGRESS_IP_ACL,
                                       RULE_TYPE_INGRESS_IPV6_STD_ACL);

    RULE_CTRL_UT_Proc_Validate_FuncType_Remapped_Table();
    RULE_CTRL_UT_Proc_Validate_Group_Qualify_Set();
    RULE_CTRL_UT_Proc_Validate_Group_Priority();

    ////////////////////////////////////////////////////////////////////////////
    // Create IP Ext
    //
    result = RULE_OM_CreateAcl(acl_name_ip_ext, RULE_TYPE_IP_EXT_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_ip_ext, &acl_index_ip_ext);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclByIndex(acl_index_ip_ext, &acl_entry_ip_ext);
    assert(RULE_TYPE_OK == result);

    //
    // Query for MAC ACL
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_mac,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(3 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_MAC_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IP_ACL);
    assert(fun_stats[2] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    //
    // Query for IP Std
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_ip_std,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(2 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    //
    // Query for IP Ext
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_ip_ext,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(2 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    ////////////////////////////////////////////////////////////////////////////
    // Create IP6 Std
    //
    result = RULE_OM_CreateAcl(acl_name_ip6_std, RULE_TYPE_IPV6_STD_ACL);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name_ip6_std, &acl_index_ip6_std);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclByIndex(acl_index_ip6_std, &acl_entry_ip6_std);
    assert(RULE_TYPE_OK == result);

    //
    // Query for MAC ACL
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_mac,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(3 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_MAC_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IP_ACL);
    assert(fun_stats[2] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    //
    // Query for IP Std
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_ip_std,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(2 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    //
    // Query for IP Ext
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_ip_ext,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(2 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_IPV6_STD_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    //
    // Query for IP6 Std
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_ip6_std,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(1 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    //
    // Remap IP6 Std to IP6 Ext
    //
    RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_STD_ACL,
                                       RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    RULE_CTRL_Priv_ChangeGroupQualifySet(RULE_TYPE_INGRESS_IPV6_STD_ACL,
                                         RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    RULE_CTRL_Priv_ChangeGroupPriority(RULE_TYPE_INGRESS_IPV6_STD_ACL,
                                       RULE_TYPE_INGRESS_IPV6_EXT_ACL);

    RULE_CTRL_UT_Proc_Validate_FuncType_Remapped_Table();
    RULE_CTRL_UT_Proc_Validate_Group_Qualify_Set();
    RULE_CTRL_UT_Proc_Validate_Group_Priority();

    //
    // Query for MAC ACL
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_mac,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(2 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_MAC_ACL);
    assert(fun_stats[1] == RULE_TYPE_INGRESS_IP_ACL);

    //
    // Query for IP Std
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_ip_std,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(1 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_IPV6_STD_ACL);

    //
    // Query for IP Ext
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_ip_ext,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(1 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_IPV6_STD_ACL);

    //
    // Query for IP6 Std
    //
    result = RULE_CTRL_Priv_GetAvailableFunctionTypeList(direction, &acl_entry_ip6_std,
                                                     fun_stats, _countof(fun_stats),
                                                     &number_of_entries_written);
    assert(RULE_TYPE_OK == result);

    assert(1 == number_of_entries_written);
    assert(fun_stats[0] == RULE_TYPE_INGRESS_IPV6_EXT_ACL);

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

int
RULE_CTRL_UT_Change_Group_Attributes()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)

    RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_EXT_ACL,
                                       RULE_TYPE_INGRESS_IPV6_STD_ACL);
    RULE_CTRL_UT_Proc_ChangeGroupQualifySet(RULE_TYPE_INGRESS_IPV6_EXT_ACL,
                                            RULE_TYPE_INGRESS_IPV6_STD_ACL);
    RULE_CTRL_Priv_ChangeGroupPriority(RULE_TYPE_INGRESS_IPV6_EXT_ACL,
                                       RULE_TYPE_INGRESS_IPV6_STD_ACL);

    RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_EXT_ACL,
                                       RULE_TYPE_INGRESS_IP_ACL);
    RULE_CTRL_UT_Proc_ChangeGroupQualifySet(RULE_TYPE_INGRESS_IPV6_EXT_ACL,
                                            RULE_TYPE_INGRESS_IP_ACL);
    RULE_CTRL_Priv_ChangeGroupPriority(RULE_TYPE_INGRESS_IPV6_EXT_ACL,
                                       RULE_TYPE_INGRESS_IP_ACL);

    RULE_CTRL_Priv_EnableFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_EXT_ACL,
                                       RULE_TYPE_INGRESS_MAC_ACL);
    RULE_CTRL_UT_Proc_ChangeGroupQualifySet(RULE_TYPE_INGRESS_IPV6_EXT_ACL,
                                            RULE_TYPE_INGRESS_MAC_ACL);
    RULE_CTRL_Priv_ChangeGroupPriority(RULE_TYPE_INGRESS_IPV6_EXT_ACL,
                                       RULE_TYPE_INGRESS_MAC_ACL);

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

int
RULE_CTRL_UT_Recycle_Group()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)

    const char *acl_name_mac_1  = "mac 1";
    const char *acl_name_mac_2  = "mac 2";
    const char *acl_name_ip_std = "ip std";

    UI32_T acl_index_mac_1;
    UI32_T acl_index_mac_2;
    UI32_T acl_index_ip_std;

    ////////////////////////////////////////////////////////////////////////////
    // MAC
    //
    RULE_CTRL_UT_Proc_Create_ACL(acl_name_mac_1, RULE_TYPE_MAC_ACL, &acl_index_mac_1);

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                         RULE_TYPE_INGRESS_MAC_ACL));

    RULE_CTRL_UT_Proc_Create_ACL(acl_name_mac_2, RULE_TYPE_MAC_ACL, &acl_index_mac_2);

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                         RULE_TYPE_INGRESS_MAC_ACL));

    RULE_CTRL_UT_Proc_Delete_ACL(acl_name_mac_1);

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                         RULE_TYPE_INGRESS_MAC_ACL));

    RULE_CTRL_UT_Proc_Delete_ACL(acl_name_mac_2);

    assert(FALSE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                          RULE_TYPE_INGRESS_MAC_ACL));


    ////////////////////////////////////////////////////////////////////////////
    // IP
    //
    RULE_CTRL_UT_Proc_Create_ACL(acl_name_ip_std, RULE_TYPE_IP_STD_ACL, &acl_index_ip_std);

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                         RULE_TYPE_INGRESS_MAC_ACL));

    RULE_CTRL_UT_Proc_Create_ACL(acl_name_mac_1, RULE_TYPE_MAC_ACL, &acl_index_mac_1);

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                         RULE_TYPE_INGRESS_IP_ACL));

    RULE_CTRL_UT_Proc_Create_ACL(acl_name_mac_2, RULE_TYPE_MAC_ACL, &acl_index_mac_2);

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                         RULE_TYPE_INGRESS_MAC_ACL));

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                         RULE_TYPE_INGRESS_IP_ACL));

    RULE_CTRL_UT_Proc_Delete_ACL(acl_name_mac_1);

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                         RULE_TYPE_INGRESS_MAC_ACL));

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                         RULE_TYPE_INGRESS_IP_ACL));

    RULE_CTRL_UT_Proc_Delete_ACL(acl_name_mac_2);

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                         RULE_TYPE_INGRESS_MAC_ACL));

    assert(FALSE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                         RULE_TYPE_INGRESS_IP_ACL));

    RULE_CTRL_UT_Proc_Delete_ACL(acl_name_ip_std);

    assert(FALSE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                         RULE_TYPE_INGRESS_MAC_ACL));

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

int
RULE_CTRL_UT_Dynamic_Selector_Assignment_By_MAC_ACL()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)

    const char *acl_name_fmt = "a%lu";

    UI32_T round;
    UI32_T port_cnt;

    for (round = 1, port_cnt = 1; ; ++round)
    {
        char acl_name[ SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1 ];
        UI32_T acl_index;
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;

        UI32_T ace_count;

        RULE_TYPE_RETURN_TYPE_T result;

        //
        // Create ACL
        //

        sprintf(acl_name, acl_name_fmt, round);

        RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

        //
        // Add Max number of ACE into ACL
        //

        for (ace_count = 0; ace_count < SYS_ADPT_MAX_NBRS_OF_MAC_ACE ;++ace_count)
        {
            RULE_TYPE_Ace_Entry_T ace_entry;

            UI8_T src_mac[SYS_ADPT_MAC_ADDR_LEN];
            UI8_T src_mask[SYS_ADPT_MAC_ADDR_LEN];

            int i, j;

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(result == RULE_TYPE_OK);

            ace_entry.access = (ace_count%2) ?
                               RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;

            memset(src_mac, 0, sizeof(src_mac));
            memset(src_mask, 0xff, sizeof(src_mask));

            assert(SYS_ADPT_MAX_NBRS_OF_ACE < (255*255));

            i = (ace_count+1) / 255;
            j = (ace_count+1) % 255;

            src_mac[ 4 ] = i;
            src_mac[ 5 ] = j;

            RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac, src_mask);

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
            if (result != RULE_TYPE_OK)
            {
                break;
            }
        }

        assert(0 < ace_count);

        //
        // Bind ACL to port
        //
        // acl 1 bind to port 1, 2, 28+1, 28+2
        // acl 2 --> failed if dynamic selector assignment is not enalbed
        //
        // acl 2 bind to port 3, 4, 28+3, 28+4 (dynamic selector assignment is enabled)
        // acl 3 bind to port 5, 6, 28+5, 28+6 (dynamic selector assignment is enabled)
        // acl 4 bind to port 7, 8, 28+7, 28+8 (dynamic selector assignment is enabled)
        // acl 5 --> failed
        //
        {
            UI32_T port_list[] =
                {
                    port_cnt,
                    port_cnt + 1,
                    port_cnt + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                    port_cnt + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
                };
            UI32_T port_list_idx;

            port_cnt += 2;

            for (port_list_idx = 0; port_list_idx < _countof(port_list); ++port_list_idx)
            {
                UI32_T ifindex;
                RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

                BOOL_T is_ingress = TRUE;
                UI32_T unit;
                UI32_T port;

                BOOL_T is_add = TRUE;
                UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

                unit = 1;
                port = port_list[ port_list_idx ];

                ifindex = port_list[ port_list_idx ];

                result = RULE_CTRL_SetAcl(ifindex,
                                          direction,
                                          acl_index,
                                          is_add, time_range_index,
                                          counter_enable);

                {
                    switch (round)
                    {
                        case 1:
                            assert(RULE_TYPE_OK == result);

                            result = RULE_OM_SetAcl(ifindex, direction, acl_index, time_range_index, counter_enable);
                            assert(RULE_TYPE_OK == result);

                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_MAC_ACL));
                            break;

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
                        case 2:
                        {
                            RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
                            RULE_TYPE_INTERFACE_INFO_T interface;

                            assert(RULE_TYPE_OK == result);

                            result = RULE_OM_SetAcl(ifindex, direction, acl_index, time_range_index, counter_enable);
                            assert(RULE_TYPE_OK == result);

                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_MAC_ACL));

                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_IP_ACL));

                            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                            interface.direction = RULE_TYPE_IS_INGRESS_TO_DIRECTION(is_ingress);

                            acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                            assert(NULL != acl_inst_p);
                            assert(RULE_TYPE_INGRESS_MAC_ACL == acl_inst_p->fun_type);
                            assert(RULE_TYPE_INGRESS_IP_ACL == acl_inst_p->remap_fun_type);
                            break;
                        }
                        case 3:
                        {
                            RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
                            RULE_TYPE_INTERFACE_INFO_T interface;

                            assert(RULE_TYPE_OK == result);

                            result = RULE_OM_SetAcl(ifindex, direction, acl_index, time_range_index, counter_enable);
                            assert(RULE_TYPE_OK == result);

                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_MAC_ACL));

                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_IPV6_STD_ACL));
                            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                            interface.direction = RULE_TYPE_IS_INGRESS_TO_DIRECTION(is_ingress);

                            acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                            assert(NULL != acl_inst_p);
                            assert(RULE_TYPE_INGRESS_MAC_ACL == acl_inst_p->fun_type);
                            assert(RULE_TYPE_INGRESS_IPV6_STD_ACL == acl_inst_p->remap_fun_type);
                            break;
                        }
                        case 4:
                        {
                            RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
                            RULE_TYPE_INTERFACE_INFO_T interface;

                            assert(RULE_TYPE_OK == result);

                            result = RULE_OM_SetAcl(ifindex, direction, acl_index, time_range_index, counter_enable);
                            assert(RULE_TYPE_OK == result);

                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_MAC_ACL));

                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_IPV6_EXT_ACL));
                            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                            interface.direction = RULE_TYPE_IS_INGRESS_TO_DIRECTION(is_ingress);

                            acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                            assert(NULL != acl_inst_p);
                            assert(RULE_TYPE_INGRESS_MAC_ACL == acl_inst_p->fun_type);
                            assert(RULE_TYPE_INGRESS_IPV6_EXT_ACL == acl_inst_p->remap_fun_type);
                            break;
                        }
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

                        default:
                            assert(RULE_TYPE_OK != result);

                            result = RULE_OM_DelAcl(acl_index);
                            assert(RULE_TYPE_OK == result);

                            goto end_bind;
                    }
                }

            } // for (port_list_idx

        }
    } // for (round

end_bind:

    RULE_CTRL_UT_Proc_Validate_Rule();

    for (round = 1, port_cnt = 1; ; ++round)
    {
        char acl_name[ SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1 ];
        UI32_T acl_index;

        RULE_TYPE_RETURN_TYPE_T result;

        sprintf(acl_name, acl_name_fmt, round);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);

        switch (round)
        {
            case 1:
                assert(RULE_TYPE_OK == result);
                break;

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
            case 2:
            case 3:
            case 4:
                assert(RULE_TYPE_OK == result);
                break;
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

            default:
                assert(RULE_TYPE_OK != result);
                goto end_unbind;
        }

        {
            UI32_T port_list[] =
            {
                port_cnt,
                port_cnt + 1,
                port_cnt + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                port_cnt + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
            };
            UI32_T port_list_idx;

            port_cnt += 2;

            for (port_list_idx = 0; port_list_idx < _countof(port_list); ++port_list_idx)
            {
                UI32_T ifindex = port_list[ port_list_idx ];
                RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

                UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

                result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex, direction,
                                                      acl_index,
                                                      time_range_index,
                                                      counter_enable);
                assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
                switch (round)
                {
                    case 1:

                        if ( port_list_idx == (_countof(port_list)-1) )
                        {
                            assert(FALSE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                  RULE_TYPE_INGRESS_MAC_ACL));

                            RULE_CTRL_UT_Proc_Delete_ACL(acl_name);
                        }
                        else
                        {
                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_MAC_ACL));
                        }

                        assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                             RULE_TYPE_INGRESS_IP_ACL));

                        assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                             RULE_TYPE_INGRESS_IPV6_STD_ACL));

                        assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                             RULE_TYPE_INGRESS_IPV6_EXT_ACL));
                        break;

                    case 2:

                        /* When the last rule be removed. It should restore to original mapping
                         */
                        if ( port_list_idx == (_countof(port_list)-1) )
                        {
                            assert(FALSE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                  RULE_TYPE_INGRESS_IP_ACL));

                            RULE_CTRL_UT_Proc_Delete_ACL(acl_name);

                        }
                        else
                        {
                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_IP_ACL));
                        }

                        assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                             RULE_TYPE_INGRESS_IPV6_STD_ACL));

                        assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                             RULE_TYPE_INGRESS_IPV6_EXT_ACL));
                        break;

                    case 3:

                        if ( port_list_idx == (_countof(port_list)-1) )
                        {
                            assert(FALSE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                  RULE_TYPE_INGRESS_IPV6_STD_ACL));

                            RULE_CTRL_UT_Proc_Delete_ACL(acl_name);
                        }
                        else
                        {
                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_IPV6_STD_ACL));
                        }

                        assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                             RULE_TYPE_INGRESS_IPV6_EXT_ACL));
                        break;

                    case 4:

                        if ( port_list_idx == (_countof(port_list)-1) )
                        {
                            RULE_CTRL_UT_Proc_Delete_ACL(acl_name);

                            assert(FALSE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_IPV6_EXT_ACL));
                        }
                        else
                        {
                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_IPV6_EXT_ACL));
                        }

                        break;

                    default:
                        break;
                }
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

            } // for (port_list_idx
        }
    } // for (round

end_unbind:

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

int
RULE_CTRL_UT_Dynamic_Selector_Assignment_Mixed_ACL()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)

    const char *acl_name_fmt = "a%lu";

    UI32_T round;
    UI32_T port_cnt;

    for (round = 1, port_cnt = 1; ; ++round)
    {
        char acl_name[ SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1 ];
        UI32_T acl_index;
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;

        UI32_T ace_count;

        RULE_TYPE_RETURN_TYPE_T result;

        //
        // Create ACL
        //

        sprintf(acl_name, acl_name_fmt, round);

        RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);

        //
        // Add Max number of ACE into ACL
        //

        for (ace_count = 0; ace_count < SYS_ADPT_MAX_NBRS_OF_MAC_ACE ;++ace_count)
        {
            RULE_TYPE_Ace_Entry_T ace_entry;

            UI8_T src_mac[SYS_ADPT_MAC_ADDR_LEN];
            UI8_T src_mask[SYS_ADPT_MAC_ADDR_LEN];

            int i, j;

            ace_entry.ace_type = acl_type;
            result = RULE_OM_InitAce(&ace_entry);
            assert(result == RULE_TYPE_OK);

            ace_entry.access = (ace_count%2) ?
                               RULE_TYPE_ACE_DENY : RULE_TYPE_ACE_PERMIT;

            memset(src_mac, 0, sizeof(src_mac));
            memset(src_mask, 0xff, sizeof(src_mask));

            assert(SYS_ADPT_MAX_NBRS_OF_ACE < (255*255));

            i = (ace_count+1) / 255;
            j = (ace_count+1) % 255;

            src_mac[ 4 ] = i;
            src_mac[ 5 ] = j;

            RULE_OM_UT_Proc_SetAceSrcMacAddress(&ace_entry, src_mac, src_mask);

            result = RULE_OM_AddAce2Acl(acl_index, acl_type, &ace_entry);
            if (result != RULE_TYPE_OK)
            {
                break;
            }
        }

        assert(0 < ace_count);

        //
        // Bind ACL to port
        //
        // acl 1 bind to port 1, 2, 28+1, 28+2
        // acl 2 --> failed if dynamic selector assignment is not enalbed
        //
        // acl 2 bind to port 3, 4, 28+3, 28+4 (dynamic selector assignment is enabled)
        // acl 3 bind to port 5, 6, 28+5, 28+6 (dynamic selector assignment is enabled)
        // acl 4 bind to port 7, 8, 28+7, 28+8 (dynamic selector assignment is enabled)
        // acl 5 --> failed
        //
        {
            UI32_T port_list[] =
            {
                port_cnt,
                port_cnt + 1,
                port_cnt + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP,
                port_cnt + SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1
            };
            UI32_T port_list_idx;

            port_cnt += 2;

            for (port_list_idx = 0; port_list_idx < _countof(port_list); ++port_list_idx)
            {
                UI32_T ifindex = port_list[ port_list_idx ];
                RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

                BOOL_T is_add = TRUE;
                UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

                result = RULE_CTRL_SetAcl(ifindex,
                                          direction,
                                          acl_index,
                                          is_add, time_range_index,
                                          counter_enable);

                {
                    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
                    RULE_TYPE_INTERFACE_INFO_T interface;

                    RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
                    interface.direction = direction;

                    switch (round)
                    {
                        case 1:
                            assert(RULE_TYPE_OK == result);

                            result = RULE_OM_SetAcl(ifindex, direction, acl_index, time_range_index, counter_enable);
                            assert(RULE_TYPE_OK == result);

                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_MAC_ACL));
                            acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                            assert(NULL != acl_inst_p);
                            assert(RULE_TYPE_INGRESS_MAC_ACL == acl_inst_p->fun_type);
                            assert(RULE_TYPE_INGRESS_MAC_ACL == acl_inst_p->remap_fun_type);
                            break;

#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
                        case 2:
                            assert(RULE_TYPE_OK == result);

                            result = RULE_OM_SetAcl(ifindex, direction, acl_index, time_range_index, counter_enable);
                            assert(RULE_TYPE_OK == result);

                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_IP_ACL));
                            acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                            assert(NULL != acl_inst_p);
                            assert(RULE_TYPE_INGRESS_MAC_ACL == acl_inst_p->fun_type);
                            assert(RULE_TYPE_INGRESS_IP_ACL == acl_inst_p->remap_fun_type);
                            break;

                        case 3:
                            assert(RULE_TYPE_OK == result);

                            result = RULE_OM_SetAcl(ifindex, direction, acl_index, time_range_index, counter_enable);
                            assert(RULE_TYPE_OK == result);

                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_IPV6_STD_ACL));
                            acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                            assert(NULL != acl_inst_p);
                            assert(RULE_TYPE_INGRESS_MAC_ACL == acl_inst_p->fun_type);
                            assert(RULE_TYPE_INGRESS_IPV6_STD_ACL == acl_inst_p->remap_fun_type);
                            break;

                        case 4:
                            assert(RULE_TYPE_OK == result);

                            result = RULE_OM_SetAcl(ifindex, direction, acl_index, time_range_index, counter_enable);
                            assert(RULE_TYPE_OK == result);

                            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                                 RULE_TYPE_INGRESS_IPV6_EXT_ACL));
                            acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
                            assert(NULL != acl_inst_p);
                            assert(RULE_TYPE_INGRESS_MAC_ACL == acl_inst_p->fun_type);
                            assert(RULE_TYPE_INGRESS_IPV6_EXT_ACL == acl_inst_p->remap_fun_type);
                            break;
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

                        default:
                            assert(RULE_TYPE_OK != result);
                            RULE_CTRL_UT_Proc_Delete_ACL(acl_name);

                            goto end_bind;
                    }
                }

            } // for (port_list_idx

        }
    } // for (round

end_bind:

    RULE_CTRL_UT_Proc_Validate_Rule();

    ////////////////////////////////////////////////////////////////////////////
    //
    // 1. Unbind ACL that used the IPv6 Ext group
    // 2. Create IP ACL
    // 3. Bind IP ACL and then unbind it
    // 4. Create IPv6 Ext --> This step should fail
    // 5. Do step 3
    //
    // 6. Unbind ACL that used the IPv6 Std group
    // 7. Create IPv6 Std and bind it to port
    //

    {
        char acl_name[ SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1 ];
        UI32_T acl_index;

        RULE_TYPE_RETURN_TYPE_T result;

        UI32_T port_list[] =
            {
                7,
                8,
                SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 7,
                SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 8
            };

        UI32_T port_list_idx;

        round = 4;

        sprintf(acl_name, acl_name_fmt, round);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);


        for (port_list_idx = 0; port_list_idx < _countof(port_list); ++port_list_idx)
        {
            UI32_T ifindex = port_list[ port_list_idx ];
            RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex, direction,
                                                  acl_index,
                                                  time_range_index,
                                                  counter_enable);
            assert(RULE_TYPE_OK == result);

            if ( port_list_idx == (_countof(port_list)-1) )
            {
                assert(FALSE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                      RULE_TYPE_INGRESS_IPV6_EXT_ACL));
            }
            else
            {
                assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                     RULE_TYPE_INGRESS_IPV6_EXT_ACL));
            }

            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                 RULE_TYPE_INGRESS_IP_ACL));

            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                 RULE_TYPE_INGRESS_IPV6_STD_ACL));

        }
    }

    {
        const char *acl_name_ip_std = "ip-std";
        const char *acl_name_ip_ext = "ip-ext";
        UI32_T acl_index_ip_std;
        UI32_T acl_index_ip_ext;

        UI32_T port_list[] =
        {
            7,
            8,
        };

        UI32_T port_list_idx;

        UI32_T retry_count = 2;

        RULE_TYPE_RETURN_TYPE_T result;

        //
        // Create IP Std
        //
        RULE_CTRL_UT_Proc_Create_ACL(acl_name_ip_std, RULE_TYPE_IP_STD_ACL, &acl_index_ip_std);

        assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                             RULE_TYPE_INGRESS_IPV6_EXT_ACL));

        //
        // Create IP Ext
        //
        RULE_CTRL_UT_Proc_Create_ACL(acl_name_ip_ext, RULE_TYPE_IP_EXT_ACL, &acl_index_ip_ext);

    step3:

        port_list[0] = 7;
        port_list[1] = 8;

        for (port_list_idx = 0; port_list_idx < _countof(port_list); ++port_list_idx)
        {
            UI32_T ifindex = port_list[ port_list_idx ];
            RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                acl_index_ip_std,
                                                time_range_index,
                                                counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        port_list[0] = SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 7;
        port_list[1] = SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 8;

        for (port_list_idx = 0; port_list_idx < _countof(port_list); ++port_list_idx)
        {
            UI32_T ifindex = port_list[ port_list_idx ];
            RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                acl_index_ip_ext,
                                                time_range_index,
                                                counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        //
        // unbind
        //
        port_list[0] = 7;
        port_list[1] = 8;

        for (port_list_idx = 0; port_list_idx < _countof(port_list); ++port_list_idx)
        {
            UI32_T ifindex = port_list[ port_list_idx ];
            RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex, direction,
                                                  acl_index_ip_std,
                                                  time_range_index,
                                                  counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        port_list[0] = SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 7;
        port_list[1] = SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 8;

        for (port_list_idx = 0; port_list_idx < _countof(port_list); ++port_list_idx)
        {
            UI32_T ifindex = port_list[ port_list_idx ];
            RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex, direction,
                                                  acl_index_ip_ext,
                                                  time_range_index,
                                                  counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                             RULE_TYPE_INGRESS_IPV6_EXT_ACL));

        {
            const char *acl_name_ip6_ext = "ip6-ext";
            UI32_T acl_index_ip6_ext;

            result = RULE_OM_CreateAcl(acl_name_ip6_ext, RULE_TYPE_IPV6_EXT_ACL);
            assert(RULE_TYPE_OK == result);

            result = RULE_OM_GetAclIdByName(acl_name_ip6_ext, &acl_index_ip6_ext);
            assert(RULE_TYPE_OK == result);

            result = RULE_CTRL_ReserveResourceOnCreateAcl(acl_index_ip6_ext);
            assert(RULE_TYPE_OK != result);

            result = RULE_OM_DelAcl(acl_index_ip6_ext);
            assert(RULE_TYPE_OK == result);
        }

        if (retry_count -- > 0)
        {
            goto step3;
        }
    }

    {
        char acl_name[ SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1 ];
        UI32_T acl_index;

        RULE_TYPE_RETURN_TYPE_T result;

        UI32_T port_list[] =
        {
            5,
            6,
            SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 5,
            SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 6
        };

        UI32_T port_list_idx;

        round = 3;

        sprintf(acl_name, acl_name_fmt, round);

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);

        for (port_list_idx = 0; port_list_idx < _countof(port_list); ++port_list_idx)
        {
            UI32_T ifindex = port_list[ port_list_idx ];
            RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex, direction,
                                                  acl_index,
                                                  time_range_index,
                                                  counter_enable);
            assert(RULE_TYPE_OK == result);

            if ( port_list_idx == (_countof(port_list)-1) )
            {
                assert(FALSE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                      RULE_TYPE_INGRESS_IPV6_STD_ACL));
            }
            else
            {
                assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                     RULE_TYPE_INGRESS_IPV6_STD_ACL));
            }

            assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                                 RULE_TYPE_INGRESS_IP_ACL));

            RULE_CTRL_UT_Proc_Validate_Rule();

        }
    }

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                         RULE_TYPE_INGRESS_IPV6_EXT_ACL));

    {
        const char *acl_name_ip6_std = "ip6-std";
        UI32_T acl_index_ip6_std;

        RULE_TYPE_RETURN_TYPE_T result;

        UI32_T port_list[] =
        {
            5,
            6,
            SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 5,
            SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 6
        };

        UI32_T port_list_idx;

        UI32_T retry_count = 2;

        //
        // Create IP Std
        //
        RULE_CTRL_UT_Proc_Create_ACL(acl_name_ip6_std, RULE_TYPE_IPV6_STD_ACL, &acl_index_ip6_std);

        assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_STD_ACL,
                                                             RULE_TYPE_INGRESS_IPV6_STD_ACL));

    step6:

        for (port_list_idx = 0; port_list_idx < _countof(port_list); ++port_list_idx)
        {
            UI32_T ifindex = port_list[ port_list_idx ];
            RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex, direction,
                                                acl_index_ip6_std,
                                                time_range_index,
                                                counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        //
        // unbind
        //
        for (port_list_idx = 0; port_list_idx < _countof(port_list); ++port_list_idx)
        {
            UI32_T ifindex = port_list[ port_list_idx ];
            RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

            UI32_T time_range_index = RULE_TYPE_UNDEF_TIME_RANGE;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex, direction,
                                                  acl_index_ip6_std,
                                                  time_range_index,
                                                  counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        if (retry_count -- > 0)
        {
            goto step6;
        }
    }

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                         RULE_TYPE_INGRESS_MAC_ACL));

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_MAC_ACL,
                                                         RULE_TYPE_INGRESS_IP_ACL));

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_IP_ACL,
                                                         RULE_TYPE_INGRESS_IPV6_EXT_ACL));

    assert(TRUE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_STD_ACL,
                                                         RULE_TYPE_INGRESS_IPV6_STD_ACL));

    {
        const char *acl_name_ip6_std = "ip6-std";

        RULE_CTRL_UT_Proc_Delete_ACL(acl_name_ip6_std);

        assert(FALSE == RULE_CTRL_Priv_IsEnabledFuncTypeRemap(RULE_TYPE_INGRESS_IPV6_STD_ACL,
                                                             RULE_TYPE_INGRESS_IPV6_STD_ACL));

    }

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

int
RULE_CTRL_UT_Get_TCAM_Entry()
{
    RULE_TYPE_TCAM_ENTRY_T tcam_entry;
    UI32_T ifindex = 2;

    UI32_T unit = STKTPLG_OM_IFINDEX_TO_UNIT(ifindex);
    UI32_T port = STKTPLG_OM_IFINDEX_TO_PORT(ifindex);

    UI32_T pool_id;
    UI32_T module_id, device_id, phy_port;

    RULE_TYPE_RETURN_TYPE_T result;

    DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);

    for (pool_id = 0; pool_id < FP_CONFIG_NumberOfGroup(); ++pool_id)
    {
        tcam_entry.unit = unit;
        tcam_entry.device_id = device_id;
        tcam_entry.pool_id = pool_id;

        result = RULE_CTRL_GetTcamEntry(&tcam_entry);
        assert(RULE_TYPE_OK == result);

        if (RULE_TYPE_IS_BIT_ON(tcam_entry.cap_bitmap.bits, RULE_TYPE_TCAM_CAP_MAC_ACL))
        {
            const char                  *acl_name = "a1";
            RULE_TYPE_AclType_T         acl_type = RULE_TYPE_MAC_ACL;
            UI32_T                      max_ace_num;
            UI32_T                      acl_index;
            RULE_TYPE_TCAM_ENTRY_T      expect_tcam_entry;

            memcpy(&expect_tcam_entry, &tcam_entry, sizeof(expect_tcam_entry));

            max_ace_num = tcam_entry.free;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name,
                                                     acl_type,
                                                     max_ace_num,
                                                     RULE_CTRL_UT_COMPRESS_DISABLE,
                                                     &acl_index);

            result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex,
                                                RULE_TYPE_INBOUND,
                                                acl_index,
                                                RULE_TYPE_UNDEF_TIME_RANGE,
                                                RULE_TYPE_COUNTER_DISABLE);
            assert(RULE_TYPE_OK == result);

            expect_tcam_entry.free -= max_ace_num;
            expect_tcam_entry.used += max_ace_num;
            assert((expect_tcam_entry.free + expect_tcam_entry.used) == expect_tcam_entry.total);


            result = RULE_CTRL_GetTcamEntry(&tcam_entry);
            assert(RULE_TYPE_OK == result);

            assert(0 == memcmp(&tcam_entry, &expect_tcam_entry, sizeof(tcam_entry)));

            /* Unbind
             */
            result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex,
                                                  RULE_TYPE_INBOUND,
                                                  acl_index,
                                                  RULE_TYPE_UNDEF_TIME_RANGE,
                                                  RULE_TYPE_COUNTER_DISABLE);
            assert(RULE_TYPE_OK == result);

            expect_tcam_entry.free += max_ace_num;
            expect_tcam_entry.used -= max_ace_num;
            assert((expect_tcam_entry.free + expect_tcam_entry.used) == expect_tcam_entry.total);


            result = RULE_CTRL_GetTcamEntry(&tcam_entry);
            assert(RULE_TYPE_OK == result);
            
            assert(0 == memcmp(&tcam_entry, &expect_tcam_entry, sizeof(tcam_entry)));
        }
    }
    
    return 0;
}


int
RULE_CTRL_UT_Get_TCAM_Entry_AddAce()
{
    RULE_TYPE_TCAM_ENTRY_T tcam_entry;
    UI32_T ifindex = 2;
    UI32_T unit = STKTPLG_OM_IFINDEX_TO_UNIT(ifindex);
    UI32_T port = STKTPLG_OM_IFINDEX_TO_PORT(ifindex);
    UI32_T module_id, device_id, phy_port;
    UI32_T pool_id;
    RULE_TYPE_RETURN_TYPE_T result;
    int cnt, repeat = 2;

    DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);

    for (cnt = 0; cnt < repeat; ++cnt)
    {
        for (pool_id = 0; pool_id < FP_CONFIG_NumberOfGroup(); ++pool_id)
        {
            RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAX_NBR_OF_ACL_TYPE;

            tcam_entry.unit = unit;
            tcam_entry.device_id = device_id;
            tcam_entry.pool_id = pool_id;

            result = RULE_CTRL_GetTcamEntry(&tcam_entry);
            assert(RULE_TYPE_OK == result);

            if (RULE_TYPE_IS_BIT_ON(tcam_entry.cap_bitmap.bits, RULE_TYPE_TCAM_CAP_MAC_ACL))
            {
                acl_type = RULE_TYPE_MAC_ACL;
            }
            else if (RULE_TYPE_IS_BIT_ON(tcam_entry.cap_bitmap.bits, RULE_TYPE_TCAM_CAP_IPV4_ACL))
            {
                acl_type = RULE_TYPE_IP_EXT_ACL;
            }
            else if (RULE_TYPE_IS_BIT_ON(tcam_entry.cap_bitmap.bits, RULE_TYPE_TCAM_CAP_IPV6_EXT_ACL))
            {
                acl_type = RULE_TYPE_IPV6_EXT_ACL;
            }
            else if (RULE_TYPE_IS_BIT_ON(tcam_entry.cap_bitmap.bits, RULE_TYPE_TCAM_CAP_IPV6_STD_ACL))
            {
                acl_type = RULE_TYPE_IPV6_STD_ACL;
            }

            if (acl_type != RULE_TYPE_MAX_NBR_OF_ACL_TYPE)
            {
                const char                  *acl_name = "a1";
                UI32_T                      max_ace_num;
                UI32_T                      acl_index;
                RULE_TYPE_TCAM_ENTRY_T      expect_tcam_entry;

                memcpy(&expect_tcam_entry, &tcam_entry, sizeof(expect_tcam_entry));
                assert(expect_tcam_entry.free == expect_tcam_entry.total);
                assert(expect_tcam_entry.used == 0);

                max_ace_num = 1;

                RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name,
                                                         acl_type,
                                                         max_ace_num,
                                                         RULE_CTRL_UT_COMPRESS_DISABLE,
                                                         &acl_index);

                result = RULE_CTRL_UT_Proc_Bind_ACL(ifindex,
                                                    RULE_TYPE_INBOUND,
                                                    acl_index,
                                                    RULE_TYPE_UNDEF_TIME_RANGE,
                                                    RULE_TYPE_COUNTER_DISABLE);
                assert(RULE_TYPE_OK == result);

                expect_tcam_entry.free -= max_ace_num;
                expect_tcam_entry.used += max_ace_num;
                assert((expect_tcam_entry.free + expect_tcam_entry.used) == expect_tcam_entry.total);

                result = RULE_CTRL_GetTcamEntry(&tcam_entry);
                assert(RULE_TYPE_OK == result);
                assert(0 == memcmp(&tcam_entry, &expect_tcam_entry, sizeof(tcam_entry)));

                /* Add new ace to acl
                 */
                {
                    int i;
                    for (i = max_ace_num; i < expect_tcam_entry.total; ++i)
                    {
                        RULE_CTRL_UT_Proc_Add_Ace_To_Acl(acl_index, acl_type, 1, i, RULE_CTRL_UT_COMPRESS_DISABLE);
                        max_ace_num++;

                        expect_tcam_entry.free--;
                        expect_tcam_entry.used++;
                        assert((expect_tcam_entry.free + expect_tcam_entry.used) == expect_tcam_entry.total);

                        result = RULE_CTRL_GetTcamEntry(&tcam_entry);
                        assert(RULE_TYPE_OK == result);
                        assert(0 == memcmp(&tcam_entry, &expect_tcam_entry, sizeof(tcam_entry)));
                    }
                }

                /* Unbind
                 */
                result = RULE_CTRL_UT_Proc_UnBind_ACL(ifindex,
                                                      RULE_TYPE_INBOUND,
                                                      acl_index,
                                                      RULE_TYPE_UNDEF_TIME_RANGE,
                                                      RULE_TYPE_COUNTER_DISABLE);
                assert(RULE_TYPE_OK == result);

                expect_tcam_entry.free += max_ace_num;
                expect_tcam_entry.used -= max_ace_num;
                assert((expect_tcam_entry.free + expect_tcam_entry.used) == expect_tcam_entry.total);

                result = RULE_CTRL_GetTcamEntry(&tcam_entry);
                assert(RULE_TYPE_OK == result);
                assert(0 == memcmp(&tcam_entry, &expect_tcam_entry, sizeof(tcam_entry)));
                
                RULE_CTRL_UT_Proc_Delete_ACL(acl_name);
            } /* In ACL pool */
        } /* for pool */
    } /* repeat */
    
    return 0;
}


int
RULE_CTRL_UT_Get_TCAM_Entry_ForIPSG()
{
    RULE_TYPE_TCAM_ENTRY_T tcam_entry;
    UI32_T ifindex = 2;

    UI32_T unit = STKTPLG_OM_IFINDEX_TO_UNIT(ifindex);
    UI32_T port = STKTPLG_OM_IFINDEX_TO_PORT(ifindex);

    UI32_T pool_id;
    UI32_T module_id, device_id, phy_port;

    RULE_TYPE_RETURN_TYPE_T result;

    DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);

    for (pool_id = 0; pool_id < FP_CONFIG_NumberOfGroup(); ++pool_id)
    {
        tcam_entry.unit = unit;
        tcam_entry.device_id = device_id;
        tcam_entry.pool_id = pool_id;

        result = RULE_CTRL_GetTcamEntry(&tcam_entry);
        assert(RULE_TYPE_OK == result);

        if (RULE_TYPE_IS_BIT_ON(tcam_entry.cap_bitmap.bits, RULE_TYPE_TCAM_CAP_IP_SOURCE_GUARD))
        {
            int                     ipsg_i;
            UI8_T                   src_ipv4_addr[4] = {0};
            UI8_T                   src_mac_addr[6] = {0};
            UI32_T                  max_ace_num;
            BOOL_T                  b_result;
            UI32_T                  temp_port;
            RULE_TYPE_TCAM_ENTRY_T  expect_tcam_entry;

            memcpy(&expect_tcam_entry, &tcam_entry, sizeof(expect_tcam_entry));
            max_ace_num = tcam_entry.free;
            assert(max_ace_num != 0);

            for (ipsg_i = 0; ipsg_i < max_ace_num; ipsg_i++)
            {
                src_ipv4_addr[3] = ipsg_i+1;
                src_mac_addr[5] = ipsg_i+1;
                temp_port = port + (ipsg_i / SYS_ADPT_DHCPSNP_MAX_NBR_OF_CLIENT_PER_PORT);

                b_result =  RULE_CTRL_IPSG_PermitIpPacket(TRUE, unit, temp_port, 1,
                                                          src_ipv4_addr,
                                                          src_mac_addr);
                assert(b_result == TRUE);

                expect_tcam_entry.free -= 1;
                expect_tcam_entry.used += 1;
                assert((expect_tcam_entry.free + expect_tcam_entry.used) == expect_tcam_entry.total);

                result = RULE_CTRL_GetTcamEntry(&tcam_entry);
                assert(RULE_TYPE_OK == result);

                assert(0 == memcmp(&tcam_entry, &expect_tcam_entry, sizeof(tcam_entry)));
            }

            for (ipsg_i = 0; ipsg_i < max_ace_num; ipsg_i++)
            {
                src_ipv4_addr[3] = ipsg_i+1;
                src_mac_addr[5] = ipsg_i+1;
                temp_port = port + (ipsg_i / SYS_ADPT_DHCPSNP_MAX_NBR_OF_CLIENT_PER_PORT);

                b_result =  RULE_CTRL_IPSG_PermitIpPacket(FALSE, unit, temp_port, 1,
                                                          src_ipv4_addr,
                                                          src_mac_addr);
                assert(b_result == TRUE);

                expect_tcam_entry.free += 1;
                expect_tcam_entry.used -= 1;
                assert((expect_tcam_entry.free + expect_tcam_entry.used) == expect_tcam_entry.total);

                result = RULE_CTRL_GetTcamEntry(&tcam_entry);
                assert(RULE_TYPE_OK == result);

                assert(0 == memcmp(&tcam_entry, &expect_tcam_entry, sizeof(tcam_entry)));
            }
        }
    }

    return 0;
}

int
RULE_CTRL_UT_SetTcpUdpPortCosMap()
{
    UI32_T map_index;
    UI32_T unit, port;
    UI16_T dest_port;
    UI8_T  phb, color;
    BOOL_T is_tcp, is_update = TRUE;

    BOOL_T ret;
    RULE_CTRL_L4PortDscpRuleStorage_T *rs_p;
    UI32_T  module_id, device_id, phy_port;

    //RULE_OM_SetDebugFlag(7);

    for (map_index = 0; map_index < SYS_ADPT_MAX_NBRS_OF_TCPUDP_ACE; ++map_index)
    {
        /* add -> remove -> add
         */
        is_tcp = map_index % 2;
        dest_port = map_index + 1;
        phb = map_index % 8;
        color = 1;

        /* Add
         */
        for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
        {
            for (port = 1; port < SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD; ++port)
            {
                ret = RULE_CTRL_SetTcpUdpPortCosMap(unit, port, is_tcp, dest_port, phb, color, is_update);
                assert(ret == TRUE);
            }
        }

        for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
        {
            for (port = 1; port < SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD; ++port)
            {
                ret = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);
                assert(ret == TRUE);

                rs_p = &shmem_data_p->l4_port_internal_pri[unit - 1][device_id][map_index];
                assert(TRUE == rs_p->spr_rs.rule_storage.is_enable);
                assert(BCM_PBMP_MEMBER(rs_p->spr_rs.pbmp, phy_port));
            }
        }

        /* Remove
         */
        for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
        {
            for (port = 1; port < SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD; ++port)
            {
                ret = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);
                assert(ret == TRUE);

                ret = RULE_CTRL_DeleteTcpUdpPortCosMap(unit, port, is_tcp, dest_port, phb, color);
                assert(ret == TRUE);

                rs_p = &shmem_data_p->l4_port_internal_pri[unit - 1][device_id][map_index];
                if (TRUE == rs_p->spr_rs.rule_storage.is_enable)
                {
                    assert(FALSE == BCM_PBMP_MEMBER(rs_p->spr_rs.pbmp, phy_port));
                }
            }
        }

        for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
        {
            for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; ++device_id)
            {
                rs_p = &shmem_data_p->l4_port_internal_pri[unit - 1][device_id][map_index];
                assert(FALSE == rs_p->spr_rs.rule_storage.is_enable);
            }
        }

        /* Add
         */
        for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
        {
            for (port = 1; port < SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD; ++port)
            {
                ret = RULE_CTRL_SetTcpUdpPortCosMap(unit, port, is_tcp, dest_port, phb, color, is_update);
                assert(ret == TRUE);
            }
        }

        for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
        {
            for (port = 1; port < SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD; ++port)
            {
                ret = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port);
                assert(ret == TRUE);

                rs_p = &shmem_data_p->l4_port_internal_pri[unit - 1][device_id][map_index];
                assert(TRUE == rs_p->spr_rs.rule_storage.is_enable);
                assert(BCM_PBMP_MEMBER(rs_p->spr_rs.pbmp, phy_port));
                assert(rs_p->is_tcp == is_tcp);
                assert(rs_p->dst_port == dest_port);
                assert(rs_p->phb == phb);
                assert(rs_p->color == color);
            }
        }
    }

    /* Map entry number = SYS_ADPT_MAX_NBRS_OF_TCPUDP_ACE,
     * shall not be able to add new one.
     */
    for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
    {
        for (port = 1; port < SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD; ++port)
        {
            dest_port++;

            ret = RULE_CTRL_SetTcpUdpPortCosMap(unit, port, is_tcp, dest_port, phb, color, is_update);
            assert(ret == FALSE);
        }
    }

    /* Remove all
     */
    for (map_index = 0; map_index < SYS_ADPT_MAX_NBRS_OF_TCPUDP_ACE; ++map_index)
    {
        is_tcp = map_index % 2;
        dest_port = map_index + 1;
        phb = map_index % 8;
        color = 1;

        for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
        {
            for (port = 1; port < SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD; ++port)
            {
                ret = RULE_CTRL_DeleteTcpUdpPortCosMap(unit, port, is_tcp, dest_port, phb, color);
                assert(ret == TRUE);
            }
        }

        for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; ++unit)
        {
            for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; ++device_id)
            {
                rs_p = &shmem_data_p->l4_port_internal_pri[unit - 1][device_id][map_index];
                assert(FALSE == rs_p->spr_rs.rule_storage.is_enable);
            }
        }
    }

    //RULE_OM_SetDebugFlag(0);

    return 0;
}

int RULE_CTRL_UT_TrapArpPacketToCpuWitherRate()
{
#if (SYS_CPNT_DAI_RATE_LIMIT_BY_RULE == TRUE)
    BOOL_T enable;
    UI32_T unit = 1, port = 1, rate = 1;
    UI32_T counter;
    RULE_TYPE_INTERFACE_INFO_T interface;
    RULE_TYPE_RETURN_TYPE_T    ret;
    RULE_CTRL_RS_ARP_T *rs_p;

    memset(&interface, 0, sizeof(interface));
    interface.type = RULE_TYPE_INTERFACE_UPORT;
    interface.uport.unit = unit;
    interface.uport.port = port;

    ret = RULE_CTRL_DAI_TrapArpPacketToCpuWithRate(TRUE, &interface, rate);
    assert(ret == RULE_TYPE_OK);

    rs_p = &rule_ctrl_shmem_data_p->arp[unit-1][port-1];

    assert(rs_p->rule_storage.is_enable == TRUE);
    assert(rs_p->meter_rate == rate);

    ret = RULE_CTRL_DAI_GetArpPacketCounter(&interface, NULL, FALSE, &counter);
    assert(ret == RULE_TYPE_OK);
    assert(counter == 0);

    {
        UI32_T module_id, device_id, phy_port;
        const UI32_T expect_counter = 100;
        BOOL_T b_result;

        b_result = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(interface.uport.unit,
                                                          interface.uport.port,
                                                          &module_id,
                                                          &device_id,
                                                          &phy_port);
        assert(b_result == TRUE);

        RULE_CTRL_UT_Proc_Set_Billing_Counter(device_id, rs_p->rule_storage.rule_id, expect_counter, 0, 0);

        ret = RULE_CTRL_DAI_GetArpPacketCounter(&interface, NULL, TRUE, &counter);
        assert(ret == RULE_TYPE_OK);
        assert(counter == expect_counter);

        ret = RULE_CTRL_DAI_GetArpPacketCounter(&interface, NULL, FALSE, &counter);
        assert(ret == RULE_TYPE_OK);
        assert(counter == 0);
    }

    ret = RULE_CTRL_DAI_TrapArpPacketToCpuWithRate(FALSE, &interface, rate);
    assert(ret == RULE_TYPE_OK);
    assert(rs_p->rule_storage.is_enable == FALSE);
#endif /* SYS_CPNT_DAI_RATE_LIMIT_BY_RULE */
    return 0;
}

int RULE_CTRL_UT_TrapArpPacketToCpuWitherRate_trunk()
{
#if (SYS_CPNT_DAI_RATE_LIMIT_BY_RULE == TRUE)
    BOOL_T enable;
    UI32_T unit = 1, port = 1, max_trunk_member_port = 7, rate = 1;
    UI32_T counter;
    RULE_TYPE_INTERFACE_INFO_T interface;
    RULE_TYPE_RETURN_TYPE_T    ret;
    RULE_CTRL_RS_ARP_T *rs_p;
    RULE_TYPE_TRUNK_MEMBER_T trunk_mem;

    memset(&trunk_mem, 0, sizeof(trunk_mem));
    for (port  = 1; port <= max_trunk_member_port; ++port)
    {
        memset(&interface, 0, sizeof(interface));
        interface.type = RULE_TYPE_INTERFACE_UPORT;
        interface.uport.unit = unit;
        interface.uport.port = port;

        RULE_TYPE_SET_BIT_ON(trunk_mem.port_list[unit-1], port - 1);

        ret = RULE_CTRL_DAI_TrapArpPacketToCpuWithRate(TRUE, &interface, rate);
        assert(ret == RULE_TYPE_OK);

        rs_p = &rule_ctrl_shmem_data_p->arp[unit-1][port-1];

        assert(rs_p->rule_storage.is_enable == TRUE);
        assert(rs_p->meter_rate == rate);
    }

    memset(&interface, 0, sizeof(interface));
    interface.type = RULE_TYPE_INTERFACE_TRUNK;
    interface.trunk_id = 1;

    ret = RULE_CTRL_DAI_GetArpPacketCounter(&interface, &trunk_mem, FALSE, &counter);
    assert(ret == RULE_TYPE_OK);
    assert(counter == 0);

    {
        UI32_T module_id, device_id, phy_port;
        const UI32_T expect_counter = 10;
        BOOL_T b_result;

        for (port  = 1; port <= max_trunk_member_port; ++port)
        {

            b_result = DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit,
                                                              port,
                                                              &module_id,
                                                              &device_id,
                                                              &phy_port);
            assert(b_result == TRUE);

            rs_p = &rule_ctrl_shmem_data_p->arp[unit-1][port-1];

            RULE_CTRL_UT_Proc_Set_Billing_Counter(device_id, rs_p->rule_storage.rule_id, expect_counter, 0, 0);
        }

        memset(&interface, 0, sizeof(interface));
        interface.type = RULE_TYPE_INTERFACE_TRUNK;
        interface.trunk_id = 1;

        ret = RULE_CTRL_DAI_GetArpPacketCounter(&interface, &trunk_mem, TRUE, &counter);
        assert(ret == RULE_TYPE_OK);
        assert(counter == (expect_counter * max_trunk_member_port));

        ret = RULE_CTRL_DAI_GetArpPacketCounter(&interface, &trunk_mem, FALSE, &counter);
        assert(ret == RULE_TYPE_OK);
        assert(counter == 0);
    }

    for (port  = 1; port <= max_trunk_member_port; ++port)
    {
        memset(&interface, 0, sizeof(interface));
        interface.type = RULE_TYPE_INTERFACE_UPORT;
        interface.uport.unit = unit;
        interface.uport.port = port;

        ret = RULE_CTRL_DAI_TrapArpPacketToCpuWithRate(FALSE, &interface, rate);
        assert(ret == RULE_TYPE_OK);

        rs_p = &rule_ctrl_shmem_data_p->arp[unit-1][port-1];

        assert(rs_p->rule_storage.is_enable == FALSE);
    }

#endif /* SYS_CPNT_DAI_RATE_LIMIT_BY_RULE */
    return 0;
}

#pragma mark -
#pragma mark Test Cases for Class/Rule Instance
int
RULE_CTRL_UT_Create_Class_Instance()
{
    enum
    {
        AV_CLASS_INST_NUM = (_countof(shmem_data_p->_class_inst_pool_buf)
                             - _countof(shmem_data_p->if_table)),
    };

    RULE_TYPE_SHM_POINTER_T ptr;
    UI32_T cnt = 0;

    while (1)
    {
        ptr = RULE_CTRL_IN_Priv_CreateClassInstance(RULE_TYPE_INST_POLICY_MAP);
        if (RULE_TYPE_NIL == ptr.type)
        {
            break;
        }

        ++ cnt;
    }

    assert(AV_CLASS_INST_NUM == cnt);

    return 0;
}

int
RULE_CTRL_UT_Create_Class_Instance_2()
{
    enum
    {
        AV_CLASS_INST_NUM = (_countof(shmem_data_p->_class_inst_pool_buf)
                             - _countof(shmem_data_p->if_table)),
    };

    RULE_TYPE_SHM_POINTER_T ptr;
    UI32_T cnt = 0;

    while (1)
    {
        ptr = RULE_CTRL_IN_Priv_CreateClassInstance(RULE_TYPE_INST_ACL);
        if (RULE_TYPE_NIL == ptr.type)
        {
            break;
        }

        ++ cnt;
    }

    assert(AV_CLASS_INST_NUM == cnt);

    return 0;
}

int
RULE_CTRL_UT_Create_Class_Instance_Temp()
{
    enum
    {
        AV_CLASS_INST_NUM = (_countof(shmem_data_p->_class_inst_pool_buf)
                             - _countof(shmem_data_p->if_table)),
    };

    RULE_TYPE_SHM_POINTER_T ptr;
    UI32_T cnt = 0;

    while (1)
    {
        ptr = RULE_CTRL_IN_Priv_CreateClassInstance(RULE_TYPE_INST_MF_ACE);
        if (RULE_TYPE_NIL == ptr.type)
        {
            break;
        }

        ++ cnt;
    }

    assert(AV_CLASS_INST_NUM == cnt);

    return 0;
}

int
RULE_CTRL_UT_Create_Rule_Instance()
{
    RULE_TYPE_SHM_POINTER_T ptr;
    UI32_T cnt = 0;

    while (1)
    {
        ptr = RULE_CTRL_IN_Priv_CreateRuleInstance(RULE_TYPE_INST_RULE);
        if (RULE_TYPE_NIL == ptr.type)
        {
            break;
        }

        ++ cnt;
    }

    assert(_countof(shmem_data_p->_rule_inst_pool_buf) == cnt);

    return 0;
}

int
RULE_CTRL_UT_Create_Container()
{
    RULE_TYPE_SHM_POINTER_T ptr;
    UI32_T cnt = 0;

    while (1)
    {
        ptr = RULE_CTRL_IN_Priv_CreateContainer();
        if (RULE_TYPE_NIL == ptr.type)
        {
            break;
        }

        ++ cnt;
    }

    assert(_countof(shmem_data_p->_container_pool_buf) == cnt);

    return 0;
}

int
RULE_CTRL_UT_Add_Class_Instance_Into_Class_Instance()
{
    enum
    {
        AV_CLASS_INST_NUM = (_countof(shmem_data_p->_class_inst_pool_buf)
                             - _countof(shmem_data_p->if_table)),
    };
    
    const UI32_T    ACL_ID_BASE = 100;

    RULE_TYPE_SHM_POINTER_T  class_ptr;
    RULE_TYPE_SHM_POINTER_T  acl_ptr;
    RULE_TYPE_SHM_POINTER_T  ptr;

    RULE_TYPE_INSTANCE_T *class_map;
    RULE_TYPE_CLASS_INSTANCE_T *acl;

    UI16_T          acl_id;

    class_ptr = RULE_CTRL_IN_Priv_CreateClassInstance(RULE_TYPE_INST_CLASS_MAP);
    assert(RULE_TYPE_NIL != class_ptr.type);

    acl_id = ACL_ID_BASE;

    while (1)
    {
        acl_ptr = RULE_CTRL_IN_Priv_CreateClassInstance(RULE_TYPE_INST_ACL);
        if (RULE_TYPE_NIL == acl_ptr.type)
        {
            break;
        }

        acl = RULE_CTRL_IN_Ptr2Instance(acl_ptr);
        acl->acl_id = acl_id ++;

        RULE_CTRL_IN_LL_Priv_InsertEnd(class_ptr, acl_ptr);
    }

    assert(ACL_ID_BASE + (AV_CLASS_INST_NUM-1) == acl_id);

    acl_id = ACL_ID_BASE;

    class_map = RULE_CTRL_IN_Ptr2Instance(class_ptr);
    ptr = class_map->links.first_node;
    while (RULE_TYPE_NIL != ptr.type)
    {
        acl = RULE_CTRL_IN_Ptr2Instance(ptr);

        assert(acl->acl_id == acl_id++);

        ptr = acl->super.links.next;
    }

    assert(ACL_ID_BASE + (AV_CLASS_INST_NUM-1) == acl_id);

    return 0;
}

int
RULE_CTRL_UT_Add_Rule_Instance_Into_Class_Instance()
{
    RULE_TYPE_SHM_POINTER_T  class_ptr;
    RULE_TYPE_SHM_POINTER_T  rule_ptr;
    RULE_TYPE_SHM_POINTER_T  ptr;

    RULE_TYPE_INSTANCE_T *class;
    RULE_TYPE_RULE_INSTANCE_T *rule;

    UI16_T          ace_id;

    class_ptr = RULE_CTRL_IN_Priv_CreateClassInstance(RULE_TYPE_INST_CLASS_MAP);
    assert(RULE_TYPE_NIL != class_ptr.type);

    ace_id = 100;

    while (1)
    {
        rule_ptr = RULE_CTRL_IN_Priv_CreateRuleInstance(RULE_TYPE_INST_RULE);
        if (RULE_TYPE_NIL == rule_ptr.type)
        {
            break;
        }

        rule = RULE_CTRL_IN_Ptr2Instance(rule_ptr);
        rule->ace_id = ace_id ++;

        RULE_CTRL_IN_LL_Priv_InsertEnd(class_ptr, rule_ptr);
    }

    assert(100 + _countof(shmem_data_p->_rule_inst_pool_buf) == ace_id);

    ace_id = 100;

    class = RULE_CTRL_IN_Ptr2Instance(class_ptr);
    ptr = class->links.first_node;
    while (RULE_TYPE_NIL != ptr.type)
    {
        rule = RULE_CTRL_IN_Ptr2Instance(ptr);

        assert(rule->ace_id == ace_id++);

        ptr = rule->super.links.next;
    }

    assert(100 + _countof(shmem_data_p->_rule_inst_pool_buf) == ace_id);

    return 0;
}

int
RULE_CTRL_UT_Create_Class_Instance_Object()
{
    enum
    {
        AV_CLASS_INST_NUM = (_countof(shmem_data_p->_class_inst_pool_buf)
                             - _countof(shmem_data_p->if_table)),
    };

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T policy_map;
    RULE_TYPE_RETURN_TYPE_T ret;
    UI32_T cnt = 0;

    while (1)
    {
        ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_POLICY_MAP, &policy_map);
        if (RULE_TYPE_OK != ret)
        {
            break;
        }

        policy_map.inst->policy_map_id = cnt++;
    }

    assert(AV_CLASS_INST_NUM == cnt);

    return 0;
}

int
RULE_CTRL_UT_Create_Rule_Instance_Object()
{
    RULE_TYPE_RULE_INSTANCE_OBJECT_T ace;
    RULE_TYPE_RETURN_TYPE_T ret;
    UI32_T cnt = 0;

    while (1)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
        if (RULE_TYPE_OK != ret)
        {
            break;
        }

        ace.inst->ace_id = cnt++;
    }

    assert(_countof(shmem_data_p->_rule_inst_pool_buf) == cnt);

    return 0;
}

int
RULE_CTRL_UT_Create_Class_Rule_Instance_Object()
{
    enum
    {
        AV_CLASS_INST_NUM = (_countof(shmem_data_p->_class_inst_pool_buf)
                             - _countof(shmem_data_p->if_table)),
    };

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_map;
    RULE_TYPE_RULE_INSTANCE_OBJECT_T mf_ace;
    RULE_TYPE_RETURN_TYPE_T ret;

    UI32_T cnt = 0;

    while (1)
    {
        ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP, &class_map);
        if (RULE_TYPE_OK != ret)
        {
            break;
        }

        class_map.inst->class_map_id = cnt++;
    }

    assert(AV_CLASS_INST_NUM == cnt);

    cnt = 0;
    while (1)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &mf_ace);
        if (RULE_TYPE_OK != ret)
        {
            break;
        }

        mf_ace.inst->ace_id = cnt++;
    }

    assert(_countof(shmem_data_p->_rule_inst_pool_buf) == cnt);

    return 0;
}

int
RULE_CTRL_UT_Append_ACE_Into_ACL()
{
    const UI32_T ACL_ID_BASE = 100;
    const UI32_T ACE_ID_BASE = 200;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl;
    RULE_TYPE_RULE_INSTANCE_OBJECT_T ace;

    RULE_TYPE_SHM_POINTER_T ptr;
    RULE_TYPE_RULE_INSTANCE_T *ace_in;

    RULE_TYPE_RETURN_TYPE_T ret;
    UI32_T acl_id = ACL_ID_BASE;
    UI32_T ace_id = ACE_ID_BASE;

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id;
    assert(acl.inst->acl_id == acl_id);

    while (1)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
        if (RULE_TYPE_OK != ret)
        {
            break;
        }

        ace.inst->id = ace_id ++;
        acl.add_rule_obj(&acl, &ace);
    }

    ace_id = ACE_ID_BASE;

    ptr = acl.inst->super.links.first_node;
    while (RULE_TYPE_NIL != ptr.type)
    {
        ace_in = RULE_CTRL_IN_Ptr2Instance(ptr);

        assert(ace_in->ace_id == ace_id++);

        ptr = ace_in->super.links.next;
    }

    return 0;
}

int
RULE_CTRL_UT_Append_MF_ACE_ACL_Into_Class_Map()
{
    const UI32_T CLASS_MAP_ID_BASE = 50;
    const UI32_T ACL_ID_BASE = 100;
    const UI32_T ACE_ID_BASE = 200;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T    class_map;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T    acl_1;
    RULE_TYPE_RULE_INSTANCE_OBJECT_T     mf_ace_2;
    RULE_TYPE_RULE_INSTANCE_OBJECT_T     mf_ace_3;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T    acl_4;
    RULE_TYPE_RULE_INSTANCE_OBJECT_T     ace;

    UI32_T class_map_id = CLASS_MAP_ID_BASE;
    UI32_T acl_id = ACL_ID_BASE;
    UI32_T ace_id = ACE_ID_BASE;

    RULE_TYPE_RETURN_TYPE_T ret;

    printf("%s no run\r\n", __FUNCTION__);
    return 0;

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP, &class_map);
    assert(RULE_TYPE_OK == ret);

    class_map.inst->id = class_map_id;

    ////////////////////////////////////////////////////////////////////////////
    // acl 1

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl_1);
    assert(RULE_TYPE_OK == ret);

    acl_1.inst->id = acl_id++;

    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
    assert(RULE_TYPE_OK == ret);

    ace.inst->id = ace_id++;

    acl_1.add_rule_obj(&acl_1, &ace);

    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
    assert(RULE_TYPE_OK == ret);

    ace.inst->id = ace_id++;

    acl_1.add_rule_obj(&acl_1, &ace);

    ////////////////////////////////////////////////////////////////////////////
    // mf ace 2

    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace_2);
    assert(RULE_TYPE_OK == ret);

    mf_ace_2.inst->id = ace_id++;

    ////////////////////////////////////////////////////////////////////////////
    // mf ace 3

    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace_3);
    assert(RULE_TYPE_OK == ret);

    mf_ace_3.inst->id = ace_id++;

    ////////////////////////////////////////////////////////////////////////////
    // acl 4

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl_4);
    assert(RULE_TYPE_OK == ret);

    acl_4.inst->id = acl_id++;

    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
    assert(RULE_TYPE_OK == ret);

    ace.inst->id = ace_id++;

    acl_4.add_rule_obj(&acl_4, &ace);

    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
    assert(RULE_TYPE_OK == ret);

    ace.inst->id = ace_id++;

    acl_4.add_rule_obj(&acl_4, &ace);

    ////////////////////////////////////////////////////////////////////////////
    // add all into class map

    class_map.add_class_obj(&class_map, &acl_1);
    class_map.add_rule_obj(&class_map, &mf_ace_2);
    class_map.add_rule_obj(&class_map, &mf_ace_3);
    class_map.add_class_obj(&class_map, &acl_4);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    {
        RULE_TYPE_SHM_POINTER_T ptr;
        RULE_TYPE_INSTANCE_T *in;
        RULE_TYPE_CLASS_INSTANCE_T *class_in;
        RULE_TYPE_RULE_INSTANCE_T *rule_in;

        UI32_T no = 1;

        ptr = class_map.inst->super.links.first_node;
        while (RULE_TYPE_NIL != ptr.type)
        {
            in = RULE_CTRL_IN_Ptr2Instance(ptr);

            switch (no)
            {
                case 1:
                    assert(in->type == RULE_TYPE_INST_ACL);

                    class_in = (RULE_TYPE_CLASS_INSTANCE_T *) in;
                    assert(class_in->id == ACL_ID_BASE);

                    {
                        RULE_TYPE_SHM_POINTER_T ace_ptr = class_in->super.links.first_node;
                        RULE_TYPE_RULE_INSTANCE_T *ace_in;

                        assert(RULE_TYPE_NIL != ace_ptr.type);
                        ace_in = RULE_CTRL_IN_Ptr2Instance(ace_ptr);
                        assert(ace_in->id == ACE_ID_BASE);

                        ace_ptr = ace_in->super.links.next;
                        assert(RULE_TYPE_NIL != ace_ptr.type);
                        ace_in = RULE_CTRL_IN_Ptr2Instance(ace_ptr);
                        assert(ace_in->id == ACE_ID_BASE+1);

                        ace_ptr = ace_in->super.links.next;
                        assert(RULE_TYPE_NIL == ace_ptr.type);
                    }
                    break;

                case 2:
                    assert(in->type == RULE_TYPE_INST_MF_ACE);

                    rule_in = (RULE_TYPE_RULE_INSTANCE_T *) in;
                    assert(rule_in->id == ACE_ID_BASE+2);
                    break;

                case 3:
                    assert(in->type == RULE_TYPE_INST_MF_ACE);

                    rule_in = (RULE_TYPE_RULE_INSTANCE_T *) in;
                    assert(rule_in->id == ACE_ID_BASE+3);
                    break;

                case 4:
                    assert(in->type == RULE_TYPE_INST_ACL);

                    class_in = (RULE_TYPE_CLASS_INSTANCE_T *) in;
                    assert(class_in->id == ACL_ID_BASE+1);

                    {
                        RULE_TYPE_SHM_POINTER_T ace_ptr = class_in->super.links.first_node;
                        RULE_TYPE_RULE_INSTANCE_T *ace_in;

                        assert(RULE_TYPE_NIL != ace_ptr.type);
                        ace_in = RULE_CTRL_IN_Ptr2Instance(ace_ptr);
                        assert(ace_in->id == ACE_ID_BASE+4);

                        ace_ptr = ace_in->super.links.next;
                        assert(RULE_TYPE_NIL != ace_ptr.type);
                        ace_in = RULE_CTRL_IN_Ptr2Instance(ace_ptr);
                        assert(ace_in->id == ACE_ID_BASE+5);

                        ace_ptr = ace_in->super.links.next;
                        assert(RULE_TYPE_NIL == ace_ptr.type);
                    }
                    break;

                default:
                    assert(0);
                    break;
            }

            ptr = in->links.next;
            ++no;
        }

        assert(5 == no);
    }

    return 0;
}

int
RULE_CTRL_UT_Append_All_Into_Policy_Map()
{
    //
    // access-list a1
    //      permit 1.1.1.1
    //      permit 1.1.1.2
    //      deny   1.1.1.3
    // access-list a2
    //      permit 1.1.1.4
    //      permit 1.1.1.5
    //      deny   1.1.1.6
    //
    // class-map c1
    //      match vlan 1
    //      match access-list a1
    //      match vlan 2
    //      match access-list a2
    //
    //  class-map c2
    //      match access-list a1
    //      match access-list a2
    //      match vlan 1
    //      match vlan 2
    //
    //  class-map c3
    //      match vlan 1
    //      match vlan 2
    //
    //  class-map c4
    //      match access-list a1
    //
    //  policy-map p1
    //      class c1
    //      class c2
    //      class c3
    //      class c4
    //
    const UI32_T POLICY_MAP_ID_BASE = 20;
    const UI32_T CLASS_MAP_ID_BASE  = 50;
    const UI32_T ACL_ID_BASE = 100;
    const UI32_T ACE_ID_BASE = 200;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T    policy_map;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T    class_map;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T    acl;
    RULE_TYPE_RULE_INSTANCE_OBJECT_T     mf_ace;

    UI32_T policy_map_id = POLICY_MAP_ID_BASE;
    UI32_T class_map_id = CLASS_MAP_ID_BASE;
    UI32_T acl_id = ACL_ID_BASE;
    UI32_T ace_id = ACE_ID_BASE;

    RULE_TYPE_RETURN_TYPE_T ret;

    printf("%s no run\r\n", __FUNCTION__);
    return 0;

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_POLICY_MAP, &policy_map);
    assert(RULE_TYPE_OK == ret);

    policy_map.inst->id = policy_map_id++;

    ////////////////////////////////////////////////////////////////////////////
    // class-map c1

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP, &class_map);
    assert(RULE_TYPE_OK == ret);

    class_map.inst->id = class_map_id++;

    policy_map.add_class_obj(&policy_map, &class_map);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace);
    assert(RULE_TYPE_OK == ret);

    mf_ace.inst->id = ace_id++;

    class_map.add_rule_obj(&class_map, &mf_ace);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id++;

    class_map.add_class_obj(&class_map, &acl);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace);
    assert(RULE_TYPE_OK == ret);

    mf_ace.inst->id = ace_id++;

    class_map.add_rule_obj(&class_map, &mf_ace);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id++;

    class_map.add_class_obj(&class_map, &acl);

    ////////////////////////////////////////////////////////////////////////////
    // class-map c2

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP, &class_map);
    assert(RULE_TYPE_OK == ret);

    class_map.inst->id = class_map_id++;

    policy_map.add_class_obj(&policy_map, &class_map);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id++;

    class_map.add_class_obj(&class_map, &acl);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id++;

    class_map.add_class_obj(&class_map, &acl);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace);
    assert(RULE_TYPE_OK == ret);

    mf_ace.inst->id = ace_id++;

    class_map.add_rule_obj(&class_map, &mf_ace);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace);
    assert(RULE_TYPE_OK == ret);

    mf_ace.inst->id = ace_id++;

    class_map.add_rule_obj(&class_map, &mf_ace);

    ////////////////////////////////////////////////////////////////////////////
    // class-map c3

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP, &class_map);
    assert(RULE_TYPE_OK == ret);

    class_map.inst->id = class_map_id++;

    policy_map.add_class_obj(&policy_map, &class_map);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace);
    assert(RULE_TYPE_OK == ret);

    mf_ace.inst->id = ace_id++;

    class_map.add_rule_obj(&class_map, &mf_ace);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace);
    assert(RULE_TYPE_OK == ret);

    mf_ace.inst->id = ace_id++;

    class_map.add_rule_obj(&class_map, &mf_ace);

    ////////////////////////////////////////////////////////////////////////////
    // class-map c4

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP, &class_map);
    assert(RULE_TYPE_OK == ret);

    class_map.inst->id = class_map_id++;

    policy_map.add_class_obj(&policy_map, &class_map);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id++;

    class_map.add_class_obj(&class_map, &acl);


    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    {
        RULE_TYPE_SHM_POINTER_T ptr;
        RULE_TYPE_INSTANCE_T *in;
        RULE_TYPE_CLASS_INSTANCE_T *class_in;

        UI32_T no = 1;

        ptr = policy_map.inst->super.links.first_node;
        while (RULE_TYPE_NIL != ptr.type)
        {
            in = RULE_CTRL_IN_Ptr2Instance(ptr);

            switch (no)
            {
                case 1:
                    assert(in->type == RULE_TYPE_INST_CLASS_MAP);

                    class_in = (RULE_TYPE_CLASS_INSTANCE_T *) in;
                    assert(class_in->id == CLASS_MAP_ID_BASE);

                    {
                        RULE_TYPE_SHM_POINTER_T class_elem_ptr = class_in->super.links.first_node;
                        RULE_TYPE_INSTANCE_T *class_elem_in;
                        RULE_TYPE_RULE_INSTANCE_T *mf_in;
                        RULE_TYPE_CLASS_INSTANCE_T *acl_in;

                        assert(RULE_TYPE_NIL != class_elem_ptr.type);
                        class_elem_in = RULE_CTRL_IN_Ptr2Instance(class_elem_ptr);
                        assert(RULE_TYPE_INST_MF_ACE == class_elem_in->type);

                        mf_in = (RULE_TYPE_RULE_INSTANCE_T *)class_elem_in;
                        assert(mf_in->id == ACE_ID_BASE+0);

                        class_elem_ptr = class_elem_in->links.next;
                        assert(RULE_TYPE_NIL != class_elem_ptr.type);
                        class_elem_in = RULE_CTRL_IN_Ptr2Instance(class_elem_ptr);
                        assert(RULE_TYPE_INST_ACL == class_elem_in->type);

                        acl_in = (RULE_TYPE_CLASS_INSTANCE_T *)class_elem_in;
                        assert(acl_in->id == ACL_ID_BASE+0);

                        class_elem_ptr = class_elem_in->links.next;
                        assert(RULE_TYPE_NIL != class_elem_ptr.type);
                        class_elem_in = RULE_CTRL_IN_Ptr2Instance(class_elem_ptr);
                        assert(RULE_TYPE_INST_MF_ACE == class_elem_in->type);

                        mf_in = (RULE_TYPE_RULE_INSTANCE_T *)class_elem_in;
                        assert(mf_in->id == ACE_ID_BASE+1);

                        class_elem_ptr = class_elem_in->links.next;
                        assert(RULE_TYPE_NIL != class_elem_ptr.type);
                        class_elem_in = RULE_CTRL_IN_Ptr2Instance(class_elem_ptr);
                        assert(RULE_TYPE_INST_ACL == class_elem_in->type);

                        acl_in = (RULE_TYPE_CLASS_INSTANCE_T *)class_elem_in;
                        assert(acl_in->id == ACL_ID_BASE+1);

                        class_elem_ptr = class_elem_in->links.next;
                        assert(RULE_TYPE_NIL == class_elem_ptr.type);
                    }

                    break;

                case 2:
                    assert(in->type == RULE_TYPE_INST_CLASS_MAP);

                    class_in = (RULE_TYPE_CLASS_INSTANCE_T *) in;
                    assert(class_in->id == CLASS_MAP_ID_BASE+1);

                    {
                        RULE_TYPE_SHM_POINTER_T class_elem_ptr = class_in->super.links.first_node;
                        RULE_TYPE_INSTANCE_T *class_elem_in;
                        RULE_TYPE_RULE_INSTANCE_T *mf_in;
                        RULE_TYPE_CLASS_INSTANCE_T *acl_in;

                        assert(RULE_TYPE_NIL != class_elem_ptr.type);
                        class_elem_in = RULE_CTRL_IN_Ptr2Instance(class_elem_ptr);
                        assert(RULE_TYPE_INST_ACL == class_elem_in->type);

                        acl_in = (RULE_TYPE_CLASS_INSTANCE_T *)class_elem_in;
                        assert(acl_in->id == ACL_ID_BASE+2);

                        class_elem_ptr = class_elem_in->links.next;
                        assert(RULE_TYPE_NIL != class_elem_ptr.type);
                        class_elem_in = RULE_CTRL_IN_Ptr2Instance(class_elem_ptr);
                        assert(RULE_TYPE_INST_ACL == class_elem_in->type);

                        acl_in = (RULE_TYPE_CLASS_INSTANCE_T *)class_elem_in;
                        assert(acl_in->id == ACL_ID_BASE+3);

                        class_elem_ptr = class_elem_in->links.next;
                        assert(RULE_TYPE_NIL != class_elem_ptr.type);
                        class_elem_in = RULE_CTRL_IN_Ptr2Instance(class_elem_ptr);
                        assert(RULE_TYPE_INST_MF_ACE == class_elem_in->type);

                        mf_in = (RULE_TYPE_RULE_INSTANCE_T *)class_elem_in;
                        assert(mf_in->id == ACE_ID_BASE+2);

                        class_elem_ptr = class_elem_in->links.next;
                        assert(RULE_TYPE_NIL != class_elem_ptr.type);
                        class_elem_in = RULE_CTRL_IN_Ptr2Instance(class_elem_ptr);
                        assert(RULE_TYPE_INST_MF_ACE == class_elem_in->type);

                        mf_in = (RULE_TYPE_RULE_INSTANCE_T *)class_elem_in;
                        assert(mf_in->id == ACE_ID_BASE+3);

                        class_elem_ptr = class_elem_in->links.next;
                        assert(RULE_TYPE_NIL == class_elem_ptr.type);
                    }
                    break;

                case 3:
                    assert(in->type == RULE_TYPE_INST_CLASS_MAP);

                    class_in = (RULE_TYPE_CLASS_INSTANCE_T *) in;
                    assert(class_in->id == CLASS_MAP_ID_BASE+2);

                    {
                        RULE_TYPE_SHM_POINTER_T class_elem_ptr = class_in->super.links.first_node;
                        RULE_TYPE_INSTANCE_T *class_elem_in;
                        RULE_TYPE_RULE_INSTANCE_T *mf_in;

                        assert(RULE_TYPE_NIL != class_elem_ptr.type);
                        class_elem_in = RULE_CTRL_IN_Ptr2Instance(class_elem_ptr);
                        assert(RULE_TYPE_INST_MF_ACE == class_elem_in->type);

                        mf_in = (RULE_TYPE_RULE_INSTANCE_T *)class_elem_in;
                        assert(mf_in->id == ACE_ID_BASE+4);

                        class_elem_ptr = class_elem_in->links.next;
                        assert(RULE_TYPE_NIL != class_elem_ptr.type);
                        class_elem_in = RULE_CTRL_IN_Ptr2Instance(class_elem_ptr);
                        assert(RULE_TYPE_INST_MF_ACE == class_elem_in->type);

                        mf_in = (RULE_TYPE_RULE_INSTANCE_T *)class_elem_in;
                        assert(mf_in->id == ACE_ID_BASE+5);

                        class_elem_ptr = class_elem_in->links.next;
                        assert(RULE_TYPE_NIL == class_elem_ptr.type);
                    }
                    break;

                case 4:
                    assert(in->type == RULE_TYPE_INST_CLASS_MAP);

                    class_in = (RULE_TYPE_CLASS_INSTANCE_T *) in;
                    assert(class_in->id == CLASS_MAP_ID_BASE+3);

                    {
                        RULE_TYPE_SHM_POINTER_T class_elem_ptr = class_in->super.links.first_node;
                        RULE_TYPE_INSTANCE_T *class_elem_in;
                        RULE_TYPE_CLASS_INSTANCE_T *acl_in;

                        assert(RULE_TYPE_NIL != class_elem_ptr.type);
                        class_elem_in = RULE_CTRL_IN_Ptr2Instance(class_elem_ptr);
                        assert(RULE_TYPE_INST_ACL == class_elem_in->type);

                        acl_in = (RULE_TYPE_CLASS_INSTANCE_T *)class_elem_in;
                        assert(acl_in->id == ACL_ID_BASE+4);

                        class_elem_ptr = class_elem_in->links.next;
                        assert(RULE_TYPE_NIL == class_elem_ptr.type);
                    }
                    break;

                default:
                    assert(0);
                    break;
            }

            ptr = in->links.next;
            ++no;
        }

        assert(5 == no);
    }

    return 0;
}

int
RULE_CTRL_UT_Append_All_Into_Policy_Map_Use_Iterator()
{
    //
    // access-list a1
    //      permit 1.1.1.1
    //      permit 1.1.1.2
    //      deny   1.1.1.3
    // access-list a2
    //      permit 1.1.1.4
    //      permit 1.1.1.5
    //      deny   1.1.1.6
    //
    // class-map c1
    //      match vlan 1
    //      match access-list a1
    //      match vlan 2
    //      match access-list a2
    //
    //  class-map c2
    //      match access-list a1
    //      match access-list a2
    //      match vlan 1
    //      match vlan 2
    //
    //  class-map c3
    //      match vlan 1
    //      match vlan 2
    //
    //  class-map c4
    //      match access-list a1
    //
    //  policy-map p1
    //      class c1
    //      class c2
    //      class c3
    //      class c4
    //
    const UI32_T POLICY_MAP_ID_BASE = 20;
    const UI32_T CLASS_MAP_ID_BASE  = 50;
    const UI32_T ACL_ID_BASE = 100;
    const UI32_T ACE_ID_BASE = 200;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T    policy_map;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T    class_map;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T    acl;
    RULE_TYPE_RULE_INSTANCE_OBJECT_T     mf_ace;
    RULE_TYPE_RULE_INSTANCE_OBJECT_T     ace;

    UI32_T policy_map_id = POLICY_MAP_ID_BASE;
    UI32_T class_map_id = CLASS_MAP_ID_BASE;
    UI32_T acl_id = ACL_ID_BASE;
    UI32_T ace_id = ACE_ID_BASE;
    UI32_T i;

    RULE_TYPE_RETURN_TYPE_T ret;

    printf("%s no run\r\n", __FUNCTION__);
    return 0;

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_POLICY_MAP, &policy_map);
    assert(RULE_TYPE_OK == ret);

    policy_map.inst->id = policy_map_id++;

    ////////////////////////////////////////////////////////////////////////////
    // class-map c1

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP, &class_map);
    assert(RULE_TYPE_OK == ret);

    class_map.inst->id = class_map_id++;

    policy_map.add_class_obj(&policy_map, &class_map);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace);
    assert(RULE_TYPE_OK == ret);

    mf_ace.inst->id = ace_id++;

    class_map.add_rule_obj(&class_map, &mf_ace);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id++;

    class_map.add_class_obj(&class_map, &acl);

    for (i = 0; i < 3; i++)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
        assert(RULE_TYPE_OK == ret);

        ace.inst->id = ace_id++;
        acl.add_rule_obj(&acl, &ace);
    }

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace);
    assert(RULE_TYPE_OK == ret);

    mf_ace.inst->id = ace_id++;

    class_map.add_rule_obj(&class_map, &mf_ace);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id++;

    class_map.add_class_obj(&class_map, &acl);

    for (i = 0; i < 3; i++)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
        assert(RULE_TYPE_OK == ret);

        ace.inst->id = ace_id++;
        acl.add_rule_obj(&acl, &ace);
    }

    ////////////////////////////////////////////////////////////////////////////
    // class-map c2

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP, &class_map);
    assert(RULE_TYPE_OK == ret);

    class_map.inst->id = class_map_id++;

    policy_map.add_class_obj(&policy_map, &class_map);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id++;

    class_map.add_class_obj(&class_map, &acl);

    for (i = 0; i < 3; i++)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
        assert(RULE_TYPE_OK == ret);

        ace.inst->id = ace_id++;
        acl.add_rule_obj(&acl, &ace);
    }

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id++;

    class_map.add_class_obj(&class_map, &acl);

    for (i = 0; i < 3; i++)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
        assert(RULE_TYPE_OK == ret);

        ace.inst->id = ace_id++;
        acl.add_rule_obj(&acl, &ace);
    }

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace);
    assert(RULE_TYPE_OK == ret);

    mf_ace.inst->id = ace_id++;

    class_map.add_rule_obj(&class_map, &mf_ace);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace);
    assert(RULE_TYPE_OK == ret);

    mf_ace.inst->id = ace_id++;

    class_map.add_rule_obj(&class_map, &mf_ace);

    ////////////////////////////////////////////////////////////////////////////
    // class-map c3

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP, &class_map);
    assert(RULE_TYPE_OK == ret);

    class_map.inst->id = class_map_id++;

    policy_map.add_class_obj(&policy_map, &class_map);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace);
    assert(RULE_TYPE_OK == ret);

    mf_ace.inst->id = ace_id++;

    class_map.add_rule_obj(&class_map, &mf_ace);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_MF_ACE, &mf_ace);
    assert(RULE_TYPE_OK == ret);

    mf_ace.inst->id = ace_id++;

    class_map.add_rule_obj(&class_map, &mf_ace);

    ////////////////////////////////////////////////////////////////////////////
    // class-map c4

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_CLASS_MAP, &class_map);
    assert(RULE_TYPE_OK == ret);

    class_map.inst->id = class_map_id++;

    policy_map.add_class_obj(&policy_map, &class_map);

    //===========================================
    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id++;

    class_map.add_class_obj(&class_map, &acl);

    for (i = 0; i < 3; i++)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
        assert(RULE_TYPE_OK == ret);

        ace.inst->id = ace_id++;
        acl.add_rule_obj(&acl, &ace);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    //
    // Reset the temporal variables
    //
    memset(&class_map, 0, sizeof(class_map));
    memset(&acl, 0, sizeof(acl));
    memset(&mf_ace, 0, sizeof(mf_ace));
    memset(&ace, 0, sizeof(ace));

    //
    // Use eof + while
    //
    {
        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T policy_elem_iter;
        UI32_T no = 1;

        policy_map.get_element_iterator(&policy_map, &policy_elem_iter);

        while (TRUE != policy_elem_iter.eof(&policy_elem_iter))
        {
            ++ no;
            policy_elem_iter.next(&policy_elem_iter);
        }

        assert(5 == no);
    }

    //
    // Use eof + for
    //
    {
        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T policy_elem_iter;
        UI32_T no = 1;

        for (policy_map.get_element_iterator(&policy_map, &policy_elem_iter);
             TRUE != policy_elem_iter.eof(&policy_elem_iter);
             policy_elem_iter.next(&policy_elem_iter))
        {
            ++ no;
        }

        assert(5 == no);
    }

    policy_map_id = POLICY_MAP_ID_BASE;
    class_map_id = CLASS_MAP_ID_BASE;
    acl_id = ACL_ID_BASE;
    ace_id = ACE_ID_BASE;

    {
        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T policy_elem_iter;

        UI32_T no = 1;

        policy_map.get_element_iterator(&policy_map, &policy_elem_iter);

        for (;
             TRUE != policy_elem_iter.eof(&policy_elem_iter);
             policy_elem_iter.next(&policy_elem_iter))
        {
            RULE_TYPE_INSTANCE_T *policy_elem_in = policy_elem_iter.get_instance(&policy_elem_iter);

            switch (no)
            {
                case 1:
                    assert(policy_elem_in->type == RULE_TYPE_INST_CLASS_MAP);

                    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(policy_elem_iter.get_ptr(&policy_elem_iter),
                                                                              &class_map);

                {
                    RULE_TYPE_CLASS_INSTANCE_OBJECT_T from_inst_class_map;

                    RULE_CTRL_IN_Priv_ConstructClassInstanceObject((RULE_TYPE_CLASS_INSTANCE_T *) policy_elem_iter.get_instance(&policy_elem_iter),
                                                                   &from_inst_class_map);

                    assert(0 == memcmp(&class_map, &from_inst_class_map, sizeof(class_map)));
                }

                    assert(class_map.inst->id == class_map_id);
                    class_map_id++;

                {
                    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T class_elem_iter;
                    RULE_TYPE_RULE_INSTANCE_T *mf_in;
                    RULE_TYPE_CLASS_INSTANCE_T *acl_in;

                    class_map.get_element_iterator(&class_map, &class_elem_iter);
                    assert(FALSE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL != class_elem_iter.get_instance(&class_elem_iter));
                    assert(RULE_TYPE_INST_MF_ACE == class_elem_iter.get_instance(&class_elem_iter)->type);
                    mf_in = (RULE_TYPE_RULE_INSTANCE_T *) class_elem_iter.get_instance(&class_elem_iter);
                    assert(mf_in->id == ace_id);
                    ace_id++;

                    class_elem_iter.next(&class_elem_iter);
                    assert(FALSE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL != class_elem_iter.get_instance(&class_elem_iter));
                    assert(RULE_TYPE_INST_ACL == class_elem_iter.get_instance(&class_elem_iter)->type);
                    acl_in = (RULE_TYPE_CLASS_INSTANCE_T *) class_elem_iter.get_instance(&class_elem_iter);
                    assert(acl_in->id == acl_id);
                    acl_id++;

                    {
                        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T acl_elem_iter;

                        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(class_elem_iter.get_ptr(&class_elem_iter),
                                                                 &acl);

                        acl.get_element_iterator(&acl, &acl_elem_iter);

                        for (i = 0; i < 3; i++)
                        {
                            RULE_TYPE_RULE_INSTANCE_T *ace_in = (RULE_TYPE_RULE_INSTANCE_T *) acl_elem_iter.get_instance(&acl_elem_iter);

                            assert(NULL != ace_in);
                            assert(TRUE != acl_elem_iter.eof(&acl_elem_iter));

                            assert(ace_in->id == ace_id);
                            ace_id++;

                            acl_elem_iter.next(&acl_elem_iter);
                        }

                        assert(RULE_TYPE_NIL == acl_elem_iter.get_ptr(&acl_elem_iter).type);
                        assert(NULL == acl_elem_iter.get_instance(&acl_elem_iter));
                        assert(TRUE == acl_elem_iter.eof(&acl_elem_iter));
                    }

                    class_elem_iter.next(&class_elem_iter);
                    assert(FALSE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL != class_elem_iter.get_instance(&class_elem_iter));
                    assert(RULE_TYPE_INST_MF_ACE == class_elem_iter.get_instance(&class_elem_iter)->type);
                    mf_in = (RULE_TYPE_RULE_INSTANCE_T *) class_elem_iter.get_instance(&class_elem_iter);
                    assert(mf_in->id == ace_id);
                    ace_id++;

                    class_elem_iter.next(&class_elem_iter);
                    assert(FALSE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL != class_elem_iter.get_instance(&class_elem_iter));
                    assert(RULE_TYPE_INST_ACL == class_elem_iter.get_instance(&class_elem_iter)->type);
                    acl_in = (RULE_TYPE_CLASS_INSTANCE_T *) class_elem_iter.get_instance(&class_elem_iter);
                    assert(acl_in->id == acl_id);
                    acl_id++;

                    {
                        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T acl_elem_iter;

                        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(class_elem_iter.get_ptr(&class_elem_iter),
                                                                 &acl);

                        acl.get_element_iterator(&acl, &acl_elem_iter);

                        for (i = 0; i < 3; i++)
                        {
                            RULE_TYPE_RULE_INSTANCE_T *ace_in = (RULE_TYPE_RULE_INSTANCE_T *) acl_elem_iter.get_instance(&acl_elem_iter);

                            assert(NULL != ace_in);
                            assert(TRUE != acl_elem_iter.eof(&acl_elem_iter));

                            assert(ace_in->id == ace_id);
                            ace_id++;

                            acl_elem_iter.next(&acl_elem_iter);
                        }

                        assert(RULE_TYPE_NIL == acl_elem_iter.get_ptr(&acl_elem_iter).type);
                        assert(NULL == acl_elem_iter.get_instance(&acl_elem_iter));
                        assert(TRUE == acl_elem_iter.eof(&acl_elem_iter));
                    }

                    class_elem_iter.next(&class_elem_iter);
                    assert(TRUE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL == class_elem_iter.get_instance(&class_elem_iter));
                }

                    break;

                case 2:
                    assert(policy_elem_in->type == RULE_TYPE_INST_CLASS_MAP);

                    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(policy_elem_iter.get_ptr(&policy_elem_iter),
                                                             &class_map);

                    assert(class_map.inst->id == class_map_id);
                    class_map_id++;

                {
                    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T class_elem_iter;
                    RULE_TYPE_RULE_INSTANCE_T *mf_in;
                    RULE_TYPE_CLASS_INSTANCE_T *acl_in;

                    class_map.get_element_iterator(&class_map, &class_elem_iter);
                    assert(FALSE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL != class_elem_iter.get_instance(&class_elem_iter));
                    assert(RULE_TYPE_INST_ACL == class_elem_iter.get_instance(&class_elem_iter)->type);
                    acl_in = (RULE_TYPE_CLASS_INSTANCE_T *) class_elem_iter.get_instance(&class_elem_iter);
                    assert(acl_in->id == acl_id);
                    acl_id++;

                    {
                        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T acl_elem_iter;

                        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(class_elem_iter.get_ptr(&class_elem_iter),
                                                                 &acl);

                        acl.get_element_iterator(&acl, &acl_elem_iter);

                        for (i = 0; i < 3; i++)
                        {
                            RULE_TYPE_RULE_INSTANCE_T *ace_in = (RULE_TYPE_RULE_INSTANCE_T *) acl_elem_iter.get_instance(&acl_elem_iter);

                            assert(NULL != ace_in);
                            assert(TRUE != acl_elem_iter.eof(&acl_elem_iter));

                            assert(ace_in->id == ace_id);
                            ace_id++;

                            acl_elem_iter.next(&acl_elem_iter);
                        }

                        assert(RULE_TYPE_NIL == acl_elem_iter.get_ptr(&acl_elem_iter).type);
                        assert(NULL == acl_elem_iter.get_instance(&acl_elem_iter));
                        assert(TRUE == acl_elem_iter.eof(&acl_elem_iter));
                    }

                    class_elem_iter.next(&class_elem_iter);
                    assert(FALSE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL != class_elem_iter.get_instance(&class_elem_iter));
                    assert(RULE_TYPE_INST_ACL == class_elem_iter.get_instance(&class_elem_iter)->type);
                    acl_in = (RULE_TYPE_CLASS_INSTANCE_T *) class_elem_iter.get_instance(&class_elem_iter);
                    assert(acl_in->id == acl_id);
                    acl_id++;

                    {
                        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T acl_elem_iter;

                        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(class_elem_iter.get_ptr(&class_elem_iter),
                                                                 &acl);

                        acl.get_element_iterator(&acl, &acl_elem_iter);

                        for (i = 0; i < 3; i++)
                        {
                            RULE_TYPE_RULE_INSTANCE_T *ace_in = (RULE_TYPE_RULE_INSTANCE_T *) acl_elem_iter.get_instance(&acl_elem_iter);

                            assert(NULL != ace_in);
                            assert(TRUE != acl_elem_iter.eof(&acl_elem_iter));

                            assert(ace_in->id == ace_id);
                            ace_id++;

                            acl_elem_iter.next(&acl_elem_iter);
                        }

                        assert(RULE_TYPE_NIL == acl_elem_iter.get_ptr(&acl_elem_iter).type);
                        assert(NULL == acl_elem_iter.get_instance(&acl_elem_iter));
                        assert(TRUE == acl_elem_iter.eof(&acl_elem_iter));
                    }

                    class_elem_iter.next(&class_elem_iter);
                    assert(FALSE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL != class_elem_iter.get_instance(&class_elem_iter));
                    assert(RULE_TYPE_INST_MF_ACE == class_elem_iter.get_instance(&class_elem_iter)->type);
                    mf_in = (RULE_TYPE_RULE_INSTANCE_T *) class_elem_iter.get_instance(&class_elem_iter);
                    assert(mf_in->id == ace_id);
                    ace_id++;

                    class_elem_iter.next(&class_elem_iter);
                    assert(FALSE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL != class_elem_iter.get_instance(&class_elem_iter));
                    assert(RULE_TYPE_INST_MF_ACE == class_elem_iter.get_instance(&class_elem_iter)->type);
                    mf_in = (RULE_TYPE_RULE_INSTANCE_T *) class_elem_iter.get_instance(&class_elem_iter);
                    assert(mf_in->id == ace_id);
                    ace_id++;

                    class_elem_iter.next(&class_elem_iter);
                    assert(TRUE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL == class_elem_iter.get_instance(&class_elem_iter));
                }

                    break;

                case 3:
                    assert(policy_elem_in->type == RULE_TYPE_INST_CLASS_MAP);

                    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(policy_elem_iter.get_ptr(&policy_elem_iter),
                                                             &class_map);

                    assert(class_map.inst->id == class_map_id);
                    class_map_id++;

                {
                    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T class_elem_iter;
                    RULE_TYPE_RULE_INSTANCE_T *mf_in;

                    class_map.get_element_iterator(&class_map, &class_elem_iter);
                    assert(FALSE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL != class_elem_iter.get_instance(&class_elem_iter));
                    assert(RULE_TYPE_INST_MF_ACE == class_elem_iter.get_instance(&class_elem_iter)->type);
                    mf_in = (RULE_TYPE_RULE_INSTANCE_T *) class_elem_iter.get_instance(&class_elem_iter);
                    assert(mf_in->id == ace_id);
                    ace_id++;

                    class_elem_iter.next(&class_elem_iter);
                    assert(FALSE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL != class_elem_iter.get_instance(&class_elem_iter));
                    assert(RULE_TYPE_INST_MF_ACE == class_elem_iter.get_instance(&class_elem_iter)->type);
                    mf_in = (RULE_TYPE_RULE_INSTANCE_T *) class_elem_iter.get_instance(&class_elem_iter);
                    assert(mf_in->id == ace_id);
                    ace_id++;

                    class_elem_iter.next(&class_elem_iter);
                    assert(TRUE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL == class_elem_iter.get_instance(&class_elem_iter));
                }

                    break;

                case 4:
                    assert(policy_elem_in->type == RULE_TYPE_INST_CLASS_MAP);

                    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(policy_elem_iter.get_ptr(&policy_elem_iter),
                                                             &class_map);

                    assert(class_map.inst->id == class_map_id);
                    class_map_id++;

                {
                    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T class_elem_iter;
                    RULE_TYPE_CLASS_INSTANCE_T *acl_in;

                    class_map.get_element_iterator(&class_map, &class_elem_iter);
                    assert(FALSE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL != class_elem_iter.get_instance(&class_elem_iter));
                    assert(RULE_TYPE_INST_ACL == class_elem_iter.get_instance(&class_elem_iter)->type);
                    acl_in = (RULE_TYPE_CLASS_INSTANCE_T *) class_elem_iter.get_instance(&class_elem_iter);
                    assert(acl_in->id == acl_id);
                    acl_id++;

                    {
                        RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T acl_elem_iter;

                        RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(class_elem_iter.get_ptr(&class_elem_iter),
                                                                 &acl);

                        acl.get_element_iterator(&acl, &acl_elem_iter);

                        for (i = 0; i < 3; i++)
                        {
                            RULE_TYPE_RULE_INSTANCE_T *ace_in = (RULE_TYPE_RULE_INSTANCE_T *) acl_elem_iter.get_instance(&acl_elem_iter);

                            assert(NULL != ace_in);
                            assert(TRUE != acl_elem_iter.eof(&acl_elem_iter));

                            assert(ace_in->id == ace_id);
                            ace_id++;

                            acl_elem_iter.next(&acl_elem_iter);
                        }

                        assert(RULE_TYPE_NIL == acl_elem_iter.get_ptr(&acl_elem_iter).type);
                        assert(NULL == acl_elem_iter.get_instance(&acl_elem_iter));
                        assert(TRUE == acl_elem_iter.eof(&acl_elem_iter));
                    }

                    class_elem_iter.next(&class_elem_iter);
                    assert(TRUE == class_elem_iter.eof(&class_elem_iter));

                    assert(NULL == class_elem_iter.get_instance(&class_elem_iter));
                }

                    break;

                default:
                    assert(0);
                    break;
            }

            memset(&class_map, 0, sizeof(class_map));
            ++no;
        }

        assert(5 == no);
    }

    return 0;
}

int
RULE_CTRL_UT_Add_Del_Rule_Instance_Object()
{
    const UI32_T ACL_ID_BASE = 100;
    const UI32_T ACE_ID_BASE = 200;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T            acl;
    RULE_TYPE_RULE_INSTANCE_OBJECT_T             ace;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T  ace_iter;

    UI32_T acl_id = ACL_ID_BASE;
    UI32_T ace_id = ACE_ID_BASE;
    UI32_T i;
    UI32_T retry_cnt = 5;

    RULE_TYPE_RETURN_TYPE_T ret;

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id++;

retry:

    ace_id = ACE_ID_BASE;
    i = 0;

    while (1)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
        if (RULE_TYPE_OK != ret)
        {
            break;
        }

        ace.inst->id = ace_id++;
        acl.add_rule_obj(&acl, &ace);

        i++;
    }

    assert(i == (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                 SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                 SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                 SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR));

    ace_id = ACE_ID_BASE;
    i = 0;

    acl.get_element_iterator(&acl, &ace_iter);

    //
    // count don't change any status of current object
    //
    assert((SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
            SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
            SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
            SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR) == ace_iter.count(&ace_iter));

    for (; TRUE != ace_iter.eof(&ace_iter); ace_iter.next(&ace_iter))
    {
        i ++;
    }

    assert(i == (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                 SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                 SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                 SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR));

    assert(i == ace_iter.count(&ace_iter));

    ////////////////////////////////////////////////////////////////////////////
    // get-next

    ace_id = ACE_ID_BASE;
    i = 0;

    ace_iter.first(&ace_iter);

    while ( NULL != ace_iter.get_instance(&ace_iter) )
    {
        i ++;

        if (TRUE != ace_iter.has_next(&ace_iter))
        {
            break;
        }

        ace_iter.next(&ace_iter);
    }

    assert(i == (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                 SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                 SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                 SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR));

    ////////////////////////////////////////////////////////////////////////////
    // get-next

    ace_id = ACE_ID_BASE;
    i = 0;

    for (ace_iter.first(&ace_iter);
         NULL != ace_iter.get_instance(&ace_iter);
         ace_iter.next(&ace_iter))
    {
        RULE_TYPE_RULE_INSTANCE_T *ace_in = (RULE_TYPE_RULE_INSTANCE_T *) ace_iter.get_instance(&ace_iter);

        assert(ace_in->id == ace_id ++);
        i ++;
    }

    assert(i == (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                 SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                 SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                 SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR));

    ////////////////////////////////////////////////////////////////////////////
    // get-prev

    ace_id = (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
              SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
              SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
              SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR) + ACE_ID_BASE;
    i = 0;

    for (ace_iter.last(&ace_iter);
         NULL != ace_iter.get_instance(&ace_iter);
         ace_iter.prev(&ace_iter))
    {
        RULE_TYPE_RULE_INSTANCE_T *ace_in = (RULE_TYPE_RULE_INSTANCE_T *) ace_iter.get_instance(&ace_iter);

        assert(ace_in->id == --ace_id);

        i ++;
    }

    assert(i == (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                 SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                 SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                 SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR));

    ////////////////////////////////////////////////////////////////////////////
    // destroy all

    ace_id = (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
              SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
              SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
              SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR) + ACE_ID_BASE;
    i = 0;

    for (ace_iter.last(&ace_iter);
         NULL != ace_iter.get_instance(&ace_iter);
         ace_iter.last(&ace_iter))
    {
        RULE_CTRL_IN_Priv_ConstructRuleInstanceObjectFromPointer(ace_iter.get_ptr(&ace_iter),
                                                &ace);

        assert(ace.inst->id == --ace_id);

        acl.remove_rule_obj(&acl, &ace);
        RULE_CTRL_IN_Priv_DestroyRuleInstance(ace_iter.get_ptr(&ace_iter));

        i ++;
    }

    assert(i == (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                 SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                 SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                 SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR));

    if (0 < retry_cnt)
    {
        retry_cnt --;
        goto retry;
    }

    return 0;
}

int
RULE_CTRL_UT_Remove_Class_Instance_Object()
{
    const UI32_T ACL_ID_BASE = 100;
    const UI32_T ACE_ID_BASE = 200;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T            acl;
    RULE_TYPE_RULE_INSTANCE_OBJECT_T             ace;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T  ace_iter;

    UI32_T acl_id = ACL_ID_BASE;
    UI32_T ace_id = ACE_ID_BASE;
    UI32_T i;
    UI32_T retry_cnt = /*RULE_CTRL_MAX_NBR_OF_PCE_PER_UNIT +*/ 5;

    RULE_TYPE_RETURN_TYPE_T ret;

retry:

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
    assert(RULE_TYPE_OK == ret);

    acl.inst->id = acl_id++;

    ace_id = ACE_ID_BASE;
    i = 0;

    while (1)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
        if (RULE_TYPE_OK != ret)
        {
            break;
        }

        ace.inst->id = ace_id++;
        acl.add_rule_obj(&acl, &ace);

        i++;
    }

    assert(i == (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                 SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                 SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                 SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR));

    ////////////////////////////////////////////////////////////////////////////
    // test

    ace_id = ACE_ID_BASE;
    i = 0;

    acl.get_element_iterator(&acl, &ace_iter);

    for (ace_iter.first(&ace_iter);
         NULL != ace_iter.get_instance(&ace_iter);
         ace_iter.next(&ace_iter))
    {
        RULE_TYPE_RULE_INSTANCE_T *ace_in = (RULE_TYPE_RULE_INSTANCE_T *) ace_iter.get_instance(&ace_iter);

        assert(ace_in->id == ace_id ++);
        i ++;
    }

    assert(i == (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                 SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                 SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                 SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR));

    ////////////////////////////////////////////////////////////////////////////
    // destroy all

    ace_id = (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
              SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
              SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
              SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR) + ACE_ID_BASE;
    i = 0;

    for (ace_iter.last(&ace_iter);
         NULL != ace_iter.get_instance(&ace_iter);
         ace_iter.last(&ace_iter))
    {
        RULE_CTRL_IN_Priv_ConstructRuleInstanceObjectFromPointer(ace_iter.get_ptr(&ace_iter),
                                                &ace);

        assert(ace.inst->id == --ace_id);

        acl.remove_rule_obj(&acl, &ace);
        RULE_CTRL_IN_Priv_DestroyRuleInstance(ace_iter.get_ptr(&ace_iter));

        i ++;
    }

    assert(i == (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                 SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                 SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                 SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR));

    RULE_CTRL_IN_Priv_DestroyClassInstance(acl.ptr);

    if (0 < retry_cnt)
    {
        retry_cnt --;
        goto retry;
    }

    return 0;
}

int
RULE_CTRL_UT_Remove_Class_Instance_Object_Recursively()
{
    enum
    {
        MAX_ACE         = 10,
        MAX_CHILD_ACL   = 10,
    };

    const UI32_T ACL_ID_BASE = 100;
    const UI32_T ACE_ID_BASE = 200;

    UI32_T acl_id = ACL_ID_BASE;
    UI32_T ace_id = ACE_ID_BASE;
    UI32_T i;
    UI32_T child_acl_index;
    UI32_T retry_cnt = 5;

    RULE_TYPE_RETURN_TYPE_T ret;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T            root;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T            child_acl[MAX_CHILD_ACL];

    RULE_TYPE_RULE_INSTANCE_OBJECT_T             ace;

retry:

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &root);
    assert(RULE_TYPE_OK == ret);

    root.inst->id = acl_id++;

    ace_id = ACE_ID_BASE;

    for (i = 0; i < MAX_ACE; ++i)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
        assert(RULE_TYPE_OK == ret);

        ace.inst->id = ace_id++;
        root.add_rule_obj(&root, &ace);
    }

    for (child_acl_index = 0; child_acl_index < MAX_CHILD_ACL; ++child_acl_index)
    {
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T   *child = &child_acl[child_acl_index];

        ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, child);
        assert(RULE_TYPE_OK == ret);

        child->inst->id = acl_id++;
        root.add_class_obj(&root, child);

        for (i = 0; i < MAX_ACE; ++i)
        {
            ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
            assert(RULE_TYPE_OK == ret);

            ace.inst->id = ace_id++;
            child->add_rule_obj(child, &ace);
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    // destroy all

    RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&root);

    for (i = 0; i < _countof(shmem_data_p->_class_inst_pool_buf); ++i)
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p;

        class_inst_p = &shmem_data_p->_class_inst_pool_buf[i];
        assert(RULE_TYPE_NIL == class_inst_p->super.type);
    }

    for (i = 0; i < _countof(shmem_data_p->_rule_inst_pool_buf); ++i)
    {
        RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;

        rule_inst_p = &shmem_data_p->_rule_inst_pool_buf[i];
        assert(RULE_TYPE_NIL == rule_inst_p->super.type);
    }

    if (0 < retry_cnt)
    {
        retry_cnt --;
        goto retry;
    }

    return 0;
}

int
RULE_CTRL_UT_Remove_Class_Instance_Object_From_Middle_Node_Recursively()
{
    enum
    {
        MAX_ACE         = 3,
        MAX_CHILD_ACL   = 3,
    };

    const UI32_T ACL_ID_BASE = 100;
    const UI32_T ACE_ID_BASE = 200;

    UI32_T acl_id = ACL_ID_BASE;
    UI32_T ace_id = ACE_ID_BASE;
    UI32_T i;
    UI32_T child_acl_index;
    UI32_T retry_cnt = 5;

    RULE_TYPE_RETURN_TYPE_T ret;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T           root;
    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T iter;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T           child_acl[MAX_CHILD_ACL];
    RULE_TYPE_RULE_INSTANCE_OBJECT_T            ace;

    ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &root);
    assert(RULE_TYPE_OK == ret);

    root.get_element_iterator(&root, &iter);

    root.inst->id = acl_id++;

retry:

    ace_id = ACE_ID_BASE;

    for (i = 0; i < MAX_ACE; ++i)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
        assert(RULE_TYPE_OK == ret);

        ace.inst->id = ace_id++;
        root.add_rule_obj(&root, &ace);
    }

    for (child_acl_index = 0; child_acl_index < MAX_CHILD_ACL; ++child_acl_index)
    {
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T   *child = &child_acl[child_acl_index];

        ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, child);
        assert(RULE_TYPE_OK == ret);

        child->inst->id = acl_id++;
        root.add_class_obj(&root, child);

        for (i = 0; i < MAX_ACE; ++i)
        {
            ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
            assert(RULE_TYPE_OK == ret);

            ace.inst->id = ace_id++;
            child->add_rule_obj(child, &ace);
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    // destroy all child

    for (iter.first(&iter); NULL != iter.get_instance(&iter); iter.first(&iter))
    {
        RULE_TYPE_INSTANCE_T    *child_in;

        child_in = (RULE_TYPE_INSTANCE_T *)iter.get_instance(&iter);

        //
        // Destory from middle was not safe !!
        // Remove it self.
        //
        //

        if (RULE_TYPE_INST_ACL == child_in->type)
        {
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T   child;

            RULE_CTRL_IN_Priv_ConstructClassInstanceObject((RULE_TYPE_CLASS_INSTANCE_T *)child_in,
                                                           &child);

            root.remove_class_obj(&root, &child);

            RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&child);
        }
        else if (RULE_TYPE_INST_RULE == child_in->type)
        {
            RULE_TYPE_RULE_INSTANCE_OBJECT_T   child;

            RULE_CTRL_IN_Priv_ConstructRuleInstanceObject((RULE_TYPE_RULE_INSTANCE_T *)child_in,
                                                          &child);

            root.remove_rule_obj(&root, &child);

            RULE_CTRL_IN_Priv_DestroyRuleInstanceObject(&child);
        }

    }

    assert(0 == iter.count(&iter));

    {
        UI32_T root_count = 0;
        for (i = 0; i < _countof(shmem_data_p->_class_inst_pool_buf); ++i)
        {
            RULE_TYPE_CLASS_INSTANCE_PTR_T class_inst_p;

            class_inst_p = &shmem_data_p->_class_inst_pool_buf[i];

            if (ACL_ID_BASE == class_inst_p->id)
            {
                assert(RULE_TYPE_INST_ACL == class_inst_p->super.type);

                root_count ++;
                continue;
            }

            assert(RULE_TYPE_NIL == class_inst_p->super.type);
        }

        assert(1 == root_count);
    }

    for (i = 0; i < _countof(shmem_data_p->_rule_inst_pool_buf); ++i)
    {
        RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;

        rule_inst_p = &shmem_data_p->_rule_inst_pool_buf[i];
        assert(RULE_TYPE_NIL == rule_inst_p->super.type);
    }

    if (0 < retry_cnt)
    {
        retry_cnt --;
        goto retry;
    }

    return 0;
}

int RULE_CTRL_UT_IndexOf_AtIndex()
{
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T            acl;
    RULE_TYPE_RULE_INSTANCE_OBJECT_T             ace;

    UI32_T i;

    RULE_TYPE_RETURN_TYPE_T ret;

    //
    // AtIndex
    //
    for (i = 0; i < _countof(shmem_data_p->_class_inst_pool_buf); ++i)
    {
        RULE_TYPE_CLASS_INSTANCE_T *in;

        in = RULE_CTRL_IN_AtIndex(RULE_TYPE_INST_ACL, i);

        assert(RULE_TYPE_NIL == in->super.type);
    }

    assert(0 == RULE_CTRL_IN_AtIndex(RULE_TYPE_INST_ACL, _countof(shmem_data_p->_class_inst_pool_buf)));
    assert(0 == RULE_CTRL_IN_AtIndex(RULE_TYPE_INST_ACL, _countof(shmem_data_p->_class_inst_pool_buf)+1));


    for (i = 0; i < (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                     SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                     SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                     SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR); ++i)
    {
        RULE_TYPE_RULE_INSTANCE_T *in;

        in = RULE_CTRL_IN_AtIndex(RULE_TYPE_INST_RULE, i);

        assert(RULE_TYPE_NIL == in->super.type);
    }

    assert(0 == RULE_CTRL_IN_AtIndex(RULE_TYPE_INST_RULE, (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                                                          SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                                                          SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                                                          SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR)));
    assert(0 == RULE_CTRL_IN_AtIndex(RULE_TYPE_INST_RULE, (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                                                          SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                                                          SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                                                          SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR)+1));

    //
    // IndexOf
    //
    i = 0;
    while (1)
    {
        ret = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
        if (RULE_TYPE_OK != ret)
        {
            break;
        }

        assert(i == RULE_CTRL_IN_IndexOfClassInstance(acl.inst));
        assert(RULE_CTRL_IN_AtIndex(acl.inst->super.type, i) == acl.inst);

        i++;
    }

    assert(i == _countof(shmem_data_p->_class_inst_pool_buf));
    i = 0;

    while (1)
    {
        ret = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &ace);
        if (RULE_TYPE_OK != ret)
        {
            break;
        }

        assert(i == RULE_CTRL_IN_IndexOfRuleInstance(ace.inst));

        i++;
    }

    assert(i == (SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *
                 SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT *
                 SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP *
                 SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR));

    return 0;
}

int
RULE_CTRL_UT_Add_Instance_Into_Class_Instance_Will_Happen_Mistake()
{
    enum
    {
        AV_CLASS_INST_NUM = (_countof(shmem_data_p->_class_inst_pool_buf)
                             - _countof(shmem_data_p->if_table)),
    };

    RULE_TYPE_SHM_POINTER_T  class_1_ptr;
    RULE_TYPE_SHM_POINTER_T  class_2_ptr;
    RULE_TYPE_SHM_POINTER_T  acl_1_ptr;
    RULE_TYPE_SHM_POINTER_T  acl_2_ptr;
    RULE_TYPE_SHM_POINTER_T  ptr;

    RULE_TYPE_INSTANCE_T *class_map;
    RULE_TYPE_CLASS_INSTANCE_T *acl;

    class_1_ptr = RULE_CTRL_IN_Priv_CreateClassInstance(RULE_TYPE_INST_CLASS_MAP);
    assert(RULE_TYPE_NIL != class_1_ptr.type);

    class_2_ptr = RULE_CTRL_IN_Priv_CreateClassInstance(RULE_TYPE_INST_CLASS_MAP);
    assert(RULE_TYPE_NIL != class_2_ptr.type);

    acl_1_ptr = RULE_CTRL_IN_Priv_CreateClassInstance(RULE_TYPE_INST_ACL);
    assert(RULE_TYPE_NIL != acl_1_ptr.type);

    acl = RULE_CTRL_IN_Ptr2Instance(acl_1_ptr);
    acl->id = 1;

    acl_2_ptr = RULE_CTRL_IN_Priv_CreateClassInstance(RULE_TYPE_INST_ACL);
    assert(RULE_TYPE_NIL != acl_1_ptr.type);

    acl = RULE_CTRL_IN_Ptr2Instance(acl_2_ptr);
    acl->id = 2;

    RULE_CTRL_IN_LL_Priv_InsertEnd(class_1_ptr, acl_1_ptr);
    RULE_CTRL_IN_LL_Priv_InsertEnd(class_2_ptr, acl_1_ptr);

    //
    // class1 --> acl1
    // class2 --> acl1
    //
    // Add acl1 into class1
    // class1 --> acl1 --> acl2
    //
    // But class2 also be changed
    // class2 --> acl1 --> acl2
    //

    RULE_CTRL_IN_LL_Priv_InsertEnd(class_1_ptr, acl_2_ptr);

    class_map = RULE_CTRL_IN_Ptr2Instance(class_2_ptr);
    ptr = class_map->links.first_node;

    acl = RULE_CTRL_IN_Ptr2Instance(ptr);
    assert(1 == acl->id);

    ptr = acl->super.links.next;

    acl = RULE_CTRL_IN_Ptr2Instance(ptr);
    assert(2 == acl->id);
    
    return 0;
}

int
RULE_CTRL_UT_Add_Instance_Into_Container()
{
    enum
    {
        AV_CLASS_INST_NUM = (_countof(shmem_data_p->_class_inst_pool_buf)
                             - _countof(shmem_data_p->if_table)),
    };

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_SHM_POINTER_T  class_1_ptr;
    RULE_TYPE_SHM_POINTER_T  class_2_ptr;
    RULE_TYPE_SHM_POINTER_T  ptr;

    RULE_TYPE_CONTAINER_OBJECT_T class_1_obj;
    RULE_TYPE_CONTAINER_OBJECT_T class_2_obj;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_1_obj;
    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_2_obj;

    RULE_TYPE_INSTANCE_T *class_map;

    class_1_ptr = RULE_CTRL_IN_Priv_CreateContainer();
    assert(RULE_TYPE_NIL != class_1_ptr.type);

    RULE_CTRL_IN_Priv_ConstructContainerObjectFromPointer(class_1_ptr, &class_1_obj);

    class_2_ptr = RULE_CTRL_IN_Priv_CreateContainer();
    assert(RULE_TYPE_NIL != class_2_ptr.type);

    RULE_CTRL_IN_Priv_ConstructContainerObjectFromPointer(class_2_ptr, &class_2_obj);

    //

    result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl_1_obj);
    assert(RULE_TYPE_OK == result);

    acl_1_obj.inst->id = 1;

    result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl_2_obj);
    assert(RULE_TYPE_OK == result);

    acl_2_obj.inst->id = 2;

    class_1_obj.add_class_obj(&class_1_obj, &acl_1_obj);
    class_2_obj.add_class_obj(&class_2_obj, &acl_1_obj);

    //
    // class1 --> acl1
    // class2 --> acl1
    //
    // Add acl1 into class1
    // class1 --> acl1 --> acl2
    //
    // But class2 also be changed
    // class2 --> acl1 --> acl2
    //

    class_1_obj.add_class_obj(&class_1_obj, &acl_2_obj);

    class_map = RULE_CTRL_IN_Ptr2Instance(class_2_ptr);
    ptr = class_map->links.first_node;
    assert(RULE_TYPE_NIL == ptr.type);

    {
        RULE_TYPE_CONTAINER_PTR_T container = (RULE_TYPE_CONTAINER_PTR_T) class_map;

        assert(0 == memcmp(&container->array[0].ptr, &acl_1_obj.ptr, sizeof(container->array[0].ptr)));
        assert(RULE_TYPE_NIL == container->array[1].ptr.type);

        assert(RULE_TYPE_NIL == acl_1_obj.inst->super.links.next.type);
        ASSERT(RULE_TYPE_NIL == acl_1_obj.inst->super.links.first_node.type);
    }

    return 0;
}

int
RULE_CTRL_UT_Container_Capability_Test()
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_CONTAINER_OBJECT_T container_obj;

    UI32_T NUM_OF_INSANCE = _countof(container_obj.inst->array);

    RULE_TYPE_RULE_INSTANCE_PTR_T rule_ptrs[NUM_OF_INSANCE + 1];

    UI32_T i;
    int retry = 2;

try_again:

    result = RULE_CTRL_IN_Priv_CreateContainerObject(&container_obj);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(rule_ptrs); ++ i)
    {
        RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_obj;

        result = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &rule_obj);
        assert(RULE_TYPE_OK == result);

        result = container_obj.add_rule_obj(&container_obj, &rule_obj);
        assert((i <  NUM_OF_INSANCE && RULE_TYPE_OK == result) ||
               (i >=  NUM_OF_INSANCE && RULE_TYPE_OK != result));

        if (RULE_TYPE_OK == result)
        {
            rule_ptrs[i] = rule_obj.inst;

            assert(RULE_CTRL_IN_Ptr2Instance(container_obj.inst->array[i].ptr) == rule_ptrs[i]);
        }
        else
        {
            RULE_CTRL_IN_Priv_DestroyRuleInstanceObject(&rule_obj);

            rule_ptrs[i] = NULL;
        }
    }

    //
    // Re-set again, all success due to the rule instance always exist
    //
    for (i = 0; i < _countof(rule_ptrs); ++ i)
    {
        if (NULL != rule_ptrs[i])
        {
            RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_obj;

            RULE_CTRL_IN_Priv_ConstructRuleInstanceObject(rule_ptrs[i], &rule_obj);

            result = container_obj.add_rule_obj(&container_obj, &rule_obj);
            assert(RULE_TYPE_OK == result);
        }
    }

    //
    // The add shall be 'append' at tail
    //
    // 0, 1, ... max
    // => 1, 2, ... max, 0
    //
    {
        RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_obj;
        RULE_TYPE_RULE_INSTANCE_PTR_T temp = rule_ptrs[0];

        for (i = 0; i < _countof(rule_ptrs); ++ i)
        {
            if (NULL != rule_ptrs[i])
            {
                RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_obj;

                RULE_CTRL_IN_Priv_ConstructRuleInstanceObject(rule_ptrs[i], &rule_obj);

                assert(RULE_CTRL_IN_Ptr2Instance(container_obj.inst->array[i].ptr) == rule_ptrs[i]);
            }
        }

        RULE_CTRL_IN_Priv_ConstructRuleInstanceObject(rule_ptrs[0], &rule_obj);

        result = container_obj.remove_rule_obj(&container_obj, &rule_obj);
        assert(RULE_TYPE_OK == result);

        result = container_obj.add_rule_obj(&container_obj, &rule_obj);

        memmove(&rule_ptrs[0],
                &rule_ptrs[1],
                (_countof(rule_ptrs) - 1) * sizeof(rule_ptrs[0]));
        rule_ptrs[NUM_OF_INSANCE - 1] = temp;

        for (i = 0; i < _countof(rule_ptrs); ++ i)
        {
            if (NULL != rule_ptrs[i])
            {
                RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_obj;

                RULE_CTRL_IN_Priv_ConstructRuleInstanceObject(rule_ptrs[i], &rule_obj);

                assert(RULE_CTRL_IN_Ptr2Instance(container_obj.inst->array[i].ptr) == rule_ptrs[i]);
            }
        }
    }

    //
    // remove all
    //
    for (i = 0; i < _countof(rule_ptrs); ++ i)
    {
        if (NULL != rule_ptrs[i])
        {
            RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_obj;

            RULE_CTRL_IN_Priv_ConstructRuleInstanceObject(rule_ptrs[i], &rule_obj);

            result = container_obj.remove_rule_obj(&container_obj, &rule_obj);
            assert(RULE_TYPE_OK == result);

            rule_ptrs[i] = NULL;
        }
    }

    //
    // The destroy function shall remove all child object
    //
    for (i = 0; i < _countof(rule_ptrs); ++ i)
    {
        RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_obj;

        result = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &rule_obj);
        assert(RULE_TYPE_OK == result);

        result = container_obj.add_rule_obj(&container_obj, &rule_obj);
        assert((i <  NUM_OF_INSANCE && RULE_TYPE_OK == result) ||
               (i >=  NUM_OF_INSANCE && RULE_TYPE_OK != result));

        if (RULE_TYPE_OK == result)
        {
            rule_ptrs[i] = rule_obj.inst;

            assert(RULE_CTRL_IN_Ptr2Instance(container_obj.inst->array[i].ptr) == rule_ptrs[i]);
        }
        else
        {
            RULE_CTRL_IN_Priv_DestroyRuleInstanceObject(&rule_obj);

            rule_ptrs[i] = NULL;
        }
    }

    RULE_CTRL_IN_Priv_DestroyContainerObject(&container_obj);

    memset(rule_ptrs, 0, sizeof(rule_ptrs));

    if (0 < -- retry)
    {
        goto try_again;
    }

    return 0;
}

int
RULE_CTRL_UT_Container_Iterator()
{
    const UI32_T    ACL_ID_BASE = 100;

    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_CONTAINER_OBJECT_T container;
    RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T iter;

    UI16_T acl_id;

    result = RULE_CTRL_IN_Priv_CreateContainerObject(&container);
    assert(RULE_TYPE_OK == result);

    acl_id = ACL_ID_BASE;

    while (1)
    {
        RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl;

        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl);
        assert(RULE_TYPE_OK == result);

        acl.inst->id = acl_id ++;

        result = container.add_class_obj(&container, &acl);
        if (RULE_TYPE_OK != result)
        {
            acl_id --;
            RULE_CTRL_IN_Priv_DestroyClassInstanceObject(&acl);
            break;
        }
    }

    assert(ACL_ID_BASE + _countof(container.inst->array) == acl_id);

    acl_id = ACL_ID_BASE;

    container.get_element_iterator(&container, &iter);

    //
    // Forward
    //
    for (iter.first(&iter); NULL != iter.get_instance(&iter); iter.next(&iter))
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T in;

        in = (RULE_TYPE_CLASS_INSTANCE_PTR_T) iter.get_instance(&iter);
        assert(in->id == acl_id ++);

        assert(TRUE != iter.eof(&iter));
    }

    assert(TRUE == iter.eof(&iter));

    assert(ACL_ID_BASE + _countof(container.inst->array) == acl_id);

    //
    // Backward
    //
    for (iter.last(&iter); NULL != iter.get_instance(&iter); iter.prev(&iter))
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T in;

        in = (RULE_TYPE_CLASS_INSTANCE_PTR_T) iter.get_instance(&iter);
        assert(in->id == -- acl_id);

        assert(TRUE != iter.eof(&iter));
    }
    
    assert(TRUE == iter.eof(&iter));

    assert(ACL_ID_BASE == acl_id);

    //
    // Has Previous
    //
    for (iter.first(&iter); NULL != iter.get_instance(&iter); iter.next(&iter))
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T in;

        in = (RULE_TYPE_CLASS_INSTANCE_PTR_T) iter.get_instance(&iter);
        assert(in->id == acl_id ++);

        if (ACL_ID_BASE < in->id)
        {
            assert(TRUE == iter.has_prev(&iter));
        }

        assert(TRUE != iter.eof(&iter));
    }

    assert(FALSE == iter.has_next(&iter));
    assert(TRUE == iter.eof(&iter));

    assert(ACL_ID_BASE + _countof(container.inst->array) == acl_id);

    //
    // Has Next
    //
    for (iter.last(&iter); NULL != iter.get_instance(&iter); iter.prev(&iter))
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T in;

        in = (RULE_TYPE_CLASS_INSTANCE_PTR_T) iter.get_instance(&iter);
        assert(in->id == -- acl_id);

        if (in->id < (ACL_ID_BASE + _countof(container.inst->array) - 1))
        {
            assert(TRUE == iter.has_next(&iter));
        }

        assert(TRUE != iter.eof(&iter));
    }

    assert(FALSE == iter.has_prev(&iter));
    assert(TRUE == iter.eof(&iter));

    assert(ACL_ID_BASE == acl_id);

    assert(_countof(container.inst->array) == iter.count(&iter));

    //
    // Clear all
    //
    for (iter.last(&iter); NULL != iter.get_instance(&iter); iter.prev(&iter))
    {
        RULE_TYPE_CLASS_INSTANCE_PTR_T in;

        in = (RULE_TYPE_CLASS_INSTANCE_PTR_T) iter.get_instance(&iter);

        {
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T obj;

            RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(iter.get_ptr(&iter), &obj);

            assert(obj.inst == in);

            container.remove_class_obj(&container, &obj);
        }

        RULE_CTRL_IN_Priv_DestroyClassInstance(iter.get_ptr(&iter));
    }

    //
    // At first
    //
    iter.first(&iter);
    assert(NULL == iter.get_instance(&iter));
    assert(TRUE != iter.has_next(&iter));
    assert(TRUE != iter.has_prev(&iter));
    assert(TRUE == iter.eof(&iter));

    iter.prev(&iter);
    assert(NULL == iter.get_instance(&iter));
    assert(TRUE != iter.has_next(&iter));
    assert(TRUE != iter.has_prev(&iter));
    assert(TRUE == iter.eof(&iter));

    iter.next(&iter);
    assert(NULL == iter.get_instance(&iter));
    assert(TRUE != iter.has_next(&iter));
    assert(TRUE != iter.has_prev(&iter));
    assert(TRUE == iter.eof(&iter));

    //
    // At last
    //
    iter.last(&iter);
    assert(NULL == iter.get_instance(&iter));
    assert(TRUE != iter.has_next(&iter));
    assert(TRUE != iter.has_prev(&iter));
    assert(TRUE == iter.eof(&iter));

    iter.prev(&iter);
    assert(NULL == iter.get_instance(&iter));
    assert(TRUE != iter.has_next(&iter));
    assert(TRUE != iter.has_prev(&iter));
    assert(TRUE == iter.eof(&iter));

    iter.next(&iter);
    assert(NULL == iter.get_instance(&iter));
    assert(TRUE != iter.has_next(&iter));
    assert(TRUE != iter.has_prev(&iter));
    assert(TRUE == iter.eof(&iter));    

    assert(0 == iter.count(&iter));

    return 0;
}

BOOL_T
RULE_CTRL_UT_Proc_Count_All(
    RULE_TYPE_INSTANCE_PTR_T in,
    void* cookie)
{
    return TRUE;
}

BOOL_T
RULE_CTRL_Priv_IsMatchId(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
    UI32_T id;

    RULE_CTRL_INSTANCE_TYPE_ID_PTR_T param_p;

    param_p = (RULE_CTRL_INSTANCE_TYPE_ID_PTR_T)cookie;

    if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(in->type))
    {
        id = ((RULE_TYPE_CONTAINER_PTR_T) in)->id;
    }
    else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type))
    {
        id = ((RULE_TYPE_CLASS_INSTANCE_PTR_T) in)->id;
    }
    else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type))
    {
        id = ((RULE_TYPE_RULE_INSTANCE_PTR_T) in)->id;
    }
    else
    {
        assert(0);
    }

    return id == param_p->id ? TRUE : FALSE;
}

int
RULE_CTRL_UT_Container_FindIf()
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_entry[2];

    struct xx
    {
        RULE_TYPE_INSTANCE_TYPE_T type;
    } inst_of_if_entry[] =
    {
        {
            RULE_TYPE_INST_CPU_IF,
        },
        {
            RULE_TYPE_INST_CPU_IF,
        },
        {
            RULE_TYPE_INST_POLICY_MAP,
        },
        {
            RULE_TYPE_INST_CONTAINER,
        },
        {
            RULE_TYPE_INST_RULE,
        },
        {
            RULE_TYPE_INST_RULE,
        },
    };

    struct yy
    {
        RULE_TYPE_INSTANCE_TYPE_T type;
    } inst_of_class_inst[] =
    {
        {
            RULE_TYPE_INST_CLASS_MAP,
        },
        {
            RULE_TYPE_INST_CLASS_MAP,
        },
        {
            RULE_TYPE_INST_RULE,
        },
        {
            RULE_TYPE_INST_RULE,
        },
        {
            RULE_TYPE_INST_CLASS_MAP,
        },
        {
            RULE_TYPE_INST_RULE,
        },
    };

    struct zz
    {
        RULE_TYPE_INSTANCE_TYPE_T type;
    } inst_of_container[] =
    {
        {
            RULE_TYPE_INST_ACL,
        },
        {
            RULE_TYPE_INST_ACL,
        },
        {
            RULE_TYPE_INST_RULE,
        },
        {
            RULE_TYPE_INST_RULE,
        },
        {
            RULE_TYPE_INST_ACL,
        },
        {
            RULE_TYPE_INST_RULE,
        },
    };

    struct hhh
    {
        RULE_TYPE_INSTANCE_TYPE_T type;
        UI32_T id;
    } list[100];

    UI32_T i;
    UI32_T sp = 0;
    UI32_T id = 1;

#define PUSH(_type, _id) ({ list[sp].type = _type; list[sp].id = _id; ++ sp; })

    memset(list, 0, sizeof(list));

    for (i = 0; i < _countof(if_entry); ++ i)
    {
        UI32_T j;

        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &if_entry[i]);
        assert(RULE_TYPE_OK == result);

        if_entry[i].inst->id = id;

        PUSH(RULE_TYPE_INST_ACL, id); ++ id;

        for (j = 0; j < _countof(inst_of_if_entry); ++ j)
        {
            RULE_TYPE_INSTANCE_TYPE_T type = inst_of_if_entry[j].type;

            if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(type))
            {
                RULE_TYPE_CONTAINER_OBJECT_T obj;
                UI32_T k;

                result = RULE_CTRL_IN_Priv_CreateContainerObject(&obj);
                assert(RULE_TYPE_OK == result);

                if_entry[i].add(&if_entry[i], (RULE_TYPE_INSTANCE_PTR_T) obj.inst);

                obj.inst->id = id;

                PUSH(type, id); ++ id;

                for (k = 0; k < _countof(inst_of_container); ++ k)
                {
                    RULE_TYPE_INSTANCE_TYPE_T _type = inst_of_container[k].type;
                    if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(_type))
                    {
                        RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_obj;

                        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(_type,
                                                                             &class_obj);
                        obj.add_class_obj(&obj, &class_obj);

                        class_obj.inst->id = id;

                        PUSH(_type, id); ++ id;
                    }
                    else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(_type))
                    {
                        RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_obj;

                        result = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(_type,
                                                                             &rule_obj);
                        obj.add_rule_obj(&obj, &rule_obj);

                        rule_obj.inst->id = id;

                        PUSH(_type, id); ++ id;
                    }
                    else
                    {
                        assert(0);
                    }
                }
            }
            else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(type))
            {
                RULE_TYPE_CLASS_INSTANCE_OBJECT_T obj;
                UI32_T k;

                result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(type, &obj);
                assert(RULE_TYPE_OK == result);

                if_entry[i].add(&if_entry[i], (RULE_TYPE_INSTANCE_PTR_T) obj.inst);

                obj.inst->id = id;

                PUSH(type, id); ++ id;

                for (k = 0; k < _countof(inst_of_class_inst); ++ k)
                {
                    RULE_TYPE_INSTANCE_TYPE_T _type = inst_of_class_inst[k].type;
                    if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(_type))
                    {
                        RULE_TYPE_CLASS_INSTANCE_OBJECT_T class_obj;

                        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(_type,
                                                                             &class_obj);
                        obj.add_class_obj(&obj, &class_obj);

                        class_obj.inst->id = id;

                        PUSH(_type, id); ++ id;
                    }
                    else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(_type))
                    {
                        RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_obj;

                        result = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(_type,
                                                                            &rule_obj);
                        obj.add_rule_obj(&obj, &rule_obj);

                        rule_obj.inst->id = id;

                        PUSH(_type, id); ++ id;
                    }
                    else
                    {
                        assert(0);
                    }
                }
            }
            else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(type))
            {
                RULE_TYPE_RULE_INSTANCE_OBJECT_T obj;

                result = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(type, &obj);
                assert(RULE_TYPE_OK == result);

                if_entry[i].add(&if_entry[i], (RULE_TYPE_INSTANCE_PTR_T) obj.inst);

                obj.inst->id = id;

                PUSH(type, id); ++ id;
            }
            else
            {
                assert(0);
            }
        }
    }

    //
    // CountIf
    //
    {
        UI32_T j;
        UI32_T count = 0;

        for (j = 0; j < _countof(if_entry); ++ j)
        {
            count += RULE_CTRL_IN_Priv_CountIf((RULE_TYPE_INSTANCE_PTR_T) if_entry[j].inst,
                                               RULE_CTRL_UT_Proc_Count_All,
                                               NULL);
        }

        assert(count == sp);
    }

    //
    // FindIf
    //
    for (i = 0; i < sp; ++ i)
    {
        RULE_TYPE_INSTANCE_PTR_T in;
        RULE_TYPE_INSTANCE_PTR_T in_2;
        RULE_CTRL_INSTANCE_TYPE_ID_T find_param;

        memset(&find_param, 0, sizeof(find_param));

        find_param.id = list[i].id;

        in = RULE_CTRL_IN_Priv_FindIf((RULE_TYPE_INSTANCE_PTR_T) if_entry[0].inst,
                                      RULE_CTRL_Priv_IsMatchId,
                                      &find_param);
        in_2 = RULE_CTRL_IN_Priv_FindIf((RULE_TYPE_INSTANCE_PTR_T) if_entry[1].inst,
                                      RULE_CTRL_Priv_IsMatchId,
                                      &find_param);

        assert((NULL != in && NULL == in_2) ||
               (NULL == in && NULL != in_2));

        if (NULL != in)
        {
            assert(in->type == list[i].type);
        }

        if (NULL != in_2)
        {
            assert(in_2->type == list[i].type);
        }
    }

#undef PUSH

    return 0;
}

int
RULE_CTRL_UT_Container_Get_First_Child_And_Sibling_If()
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_TYPE_CONTAINER_OBJECT_T container;

    struct zz
    {
        RULE_TYPE_INSTANCE_TYPE_T type;
    } inst_of_container[] =
    {
        {
            RULE_TYPE_INST_ACL,
        },
        {
            RULE_TYPE_INST_ACL,
        },
        {
            RULE_TYPE_INST_ACL,
        },
        {
            RULE_TYPE_INST_CONTAINER,
        },
        {
            RULE_TYPE_INST_CONTAINER,
        },
        {
            RULE_TYPE_INST_CLASS_MAP,
        },
        {
            RULE_TYPE_INST_CLASS_MAP,
        },
        {
            RULE_TYPE_INST_MF_ACE,
        },
        {
            RULE_TYPE_INST_MF_ACE,
        },
    };

    struct hhh
    {
        RULE_TYPE_INSTANCE_TYPE_T type;
        UI32_T id;
    } list[100];

    UI32_T i;
    UI32_T sp = 0;
    UI32_T id = 1;

#define PUSH(_type, _id) ({ list[sp].type = _type; list[sp].id = _id; ++ sp; })

    memset(list, 0, sizeof(list));

    result = RULE_CTRL_IN_Priv_CreateContainerObject(&container);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(inst_of_container); ++ i)
    {
        RULE_TYPE_INSTANCE_TYPE_T type = inst_of_container[i].type;

        if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(type))
        {
            RULE_TYPE_CONTAINER_OBJECT_T obj;

            result = RULE_CTRL_IN_Priv_CreateContainerObject(&obj);
            assert(RULE_TYPE_OK == result);

            container.add(&container, (RULE_TYPE_INSTANCE_PTR_T) obj.inst);

            obj.inst->id = id;

            PUSH(type, id); ++ id;
        }
        else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(type))
        {
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T obj;

            result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(type, &obj);

            container.add(&container, (RULE_TYPE_INSTANCE_PTR_T) obj.inst);

            obj.inst->id = id;

            PUSH(type, id); ++ id;
        }
        else
        {
            assert(0);
        }
    }

    //
    // Walk by getfrist and getnext
    //
    {
        RULE_TYPE_INSTANCE_PTR_T in;

        RULE_CTRL_INSTANCE_TYPE_ID_T find_param;

        i = 0;

        memset(&find_param, 0, sizeof(find_param));

        find_param.id = list[i].id;

        in = RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) container.inst,
                                            RULE_CTRL_Priv_IsMatchId,
                                            &find_param);
        while (NULL != in)
        {
            UI32_T id;

            assert(i < sp);

            assert(list[i].type == in->type);

            if (TRUE == RULE_CTRL_IS_CONTAINER_TYPE(in->type))
            {
                id = ((RULE_TYPE_CONTAINER_PTR_T) in)->id;
            }
            else if (TRUE == RULE_CTRL_IS_CLASS_INSTANCE_TYPE(in->type))
            {
                id = ((RULE_TYPE_CLASS_INSTANCE_PTR_T) in)->id;
            }
            else if (TRUE == RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type))
            {
                id = ((RULE_TYPE_RULE_INSTANCE_PTR_T) in)->id;
            }

            assert(list[i].id == id);

            //
            // The next
            //
            i ++;

            find_param.id = list[i].id;

            in = RULE_CTRL_Priv_GetNextSiblingIf(in, RULE_CTRL_Priv_IsMatchId, &find_param);
        }

        assert(i == sp);
    }

    //
    // getfirst
    //
    for (i = 2; i < sp; i += 2)
    {
        RULE_TYPE_INSTANCE_PTR_T in;

        RULE_CTRL_INSTANCE_TYPE_ID_T find_param;

        memset(&find_param, 0, sizeof(find_param));

        find_param.id = list[i].id;

        in = RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) container.inst,
                                            RULE_CTRL_Priv_IsMatchId,
                                            &find_param);
        assert(NULL != in);
        assert(in->type == list[i].type);
    }

    //
    // getnext
    //
    {
        RULE_TYPE_INSTANCE_PTR_T in;

        RULE_CTRL_INSTANCE_TYPE_ID_T find_param;

        memset(&find_param, 0, sizeof(find_param));

        i = 2;

        find_param.id = list[i].id;

        in = RULE_CTRL_Priv_GetFirstChildIf((RULE_TYPE_INSTANCE_PTR_T) container.inst,
                                            RULE_CTRL_Priv_IsMatchId,
                                            &find_param);
        assert(NULL != in);
        assert(in->type == list[i].type);

        for (i += 2; i < sp; i += 2)
        {
            find_param.id = list[i].id;

            in = RULE_CTRL_Priv_GetNextSiblingIf((RULE_TYPE_INSTANCE_PTR_T) in,
                                                 RULE_CTRL_Priv_IsMatchId,
                                                 &find_param);
            assert(NULL != in);
            assert(in->type == list[i].type);
        }
    }

#undef PUSH

    return 0;
}

int
RULE_CTRL_UT_Add_ACL_Instance_Into_IfEntry_As_Real_Case()
{
    enum
    {
        MIN_IFINDEX = 1,
        MAX_IFINDEX = 3,
        MIN_DIRECTION = RULE_TYPE_INBOUND,
        MAX_DIRECTION = RULE_TYPE_OUTBOUND,

        MAX_NUM_OF_ACE_IN_ACL = 3,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ifindex;

    for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
        assert(RULE_TYPE_OK == result);

        for (interface.direction = MIN_DIRECTION; interface.direction <= MAX_DIRECTION; ++ interface.direction)
        {
            RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;

            RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;

            RULE_TYPE_CONTAINER_PTR_T acl_container_p;
            RULE_TYPE_CONTAINER_OBJECT_T acl_container_obj;

            if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
            assert(NULL != if_entry_p);

            RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
                                                                      &if_class_obj);

            result = RULE_CTRL_IN_Priv_CreateContainerObject(&acl_container_obj);
            assert(RULE_TYPE_OK == result);

            if_class_obj.add(&if_class_obj, (RULE_TYPE_INSTANCE_PTR_T) acl_container_obj.inst);

            acl_container_p = acl_container_obj.inst;

            acl_container_p->id = RULE_TYPE_INST_ACL;
            
            RULE_CTRL_UT_Proc_Validate_Leaks_Instance();

            //
            // Add ACL into container
            //
            {
                RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

                result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACL, &acl_inst_obj);
                assert(RULE_TYPE_OK == result);

                result = acl_container_obj.add(&acl_container_obj, (RULE_TYPE_INSTANCE_PTR_T) acl_inst_obj.inst);
                assert(RULE_TYPE_OK == result);

                //
                // Add ACE and rule
                //
                {
                    UI32_T ax;

                    for (ax = 0; ax < MAX_NUM_OF_ACE_IN_ACL; ++ ax)
                    {
                        RULE_TYPE_CLASS_INSTANCE_OBJECT_T ace_inst_obj;

                        RULE_TYPE_RULE_INSTANCE_OBJECT_T rule_inst_obj;

                        result = RULE_CTRL_IN_Priv_CreateClassInstanceObject(RULE_TYPE_INST_ACE, &ace_inst_obj);
                        assert(RULE_TYPE_OK == result);

                        acl_inst_obj.add_class_obj(&acl_inst_obj, &ace_inst_obj);

                        RULE_CTRL_UT_Proc_Validate_Leaks_Instance();

                        result = RULE_CTRL_IN_Priv_CreateRuleInstanceObject(RULE_TYPE_INST_RULE, &rule_inst_obj);
                        assert(RULE_TYPE_OK == result);

                        ace_inst_obj.add_rule_obj(&ace_inst_obj, &rule_inst_obj);
                        
                        RULE_CTRL_UT_Proc_Validate_Leaks_Instance();                    
                    }
                }
            }
            
            RULE_CTRL_UT_Proc_Validate_Leaks_Instance();
        }
    }
    
    return 0;
}

#pragma mark End of Test Cases for Class/Rule Instance
#pragma mark -

void
RULE_CTRL_UT_Test_Instance()
{
    //
    // Only Test the Instance on OM
    //
#define RULE_CTRL_UT_TEST(func) \
    UT_Test(RULE_CTRL_UT_Setup, RULE_CTRL_UT_Teardown, func)

    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Create_Class_Instance);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Create_Class_Instance_2);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Create_Class_Instance_Temp);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Create_Rule_Instance);

    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Create_Container);

    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Add_Class_Instance_Into_Class_Instance);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Add_Rule_Instance_Into_Class_Instance);

    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Create_Class_Instance_Object);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Create_Rule_Instance_Object);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Create_Class_Rule_Instance_Object);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Append_ACE_Into_ACL);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Append_MF_ACE_ACL_Into_Class_Map);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Append_All_Into_Policy_Map);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Append_All_Into_Policy_Map_Use_Iterator);

    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Add_Del_Rule_Instance_Object);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Remove_Class_Instance_Object);

    if (0)
    {
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Remove_Class_Instance_Object_Recursively);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Remove_Class_Instance_Object_From_Middle_Node_Recursively);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_IndexOf_AtIndex);
    }

    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Add_Instance_Into_Class_Instance_Will_Happen_Mistake);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Add_Instance_Into_Container);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Container_Capability_Test);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Container_Iterator);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Container_FindIf);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Container_Get_First_Child_And_Sibling_If);
    RULE_CTRL_UT_TEST(RULE_CTRL_UT_Add_ACL_Instance_Into_IfEntry_As_Real_Case);    

#undef RULE_CTRL_UT_TEST
}

int
RULE_CTRL_UT_RunTestCaese()
{
#define RULE_CTRL_UT_TEST(func) \
    UT_Test(RULE_CTRL_UT_Setup, RULE_CTRL_UT_Teardown_With_Validating_Rule, func)

    //
    // The self test
    //
    {
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_SelfTest_Create_Acl_And_Add_Ace);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_SelfTest_FP_Config_Initial);
    }

    /* priority test (For coding)
     */
    if (1)
    {
    }

    if (l4_options.run_rule_ctrl)
    {
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Set_All_Global_And_Dflt_Rule);

        //
        // Test for ACL/QoS instance functons
        //
        RULE_CTRL_UT_Test_Instance();

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Apply_ACL);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Apply_ACL_With_Counter_Enabled);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Apply_All_Type_Of_ACLs_On_Port_At_The_Same_Time);
#if 0 // FIXME: Add time range test case
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Cant_Share_ACL_Due_To_Time_Range_Enabled);
#endif /* 0 */

        //
        // Test failed at SYS_CPNT_ACL_AUTO_COMPRESS_ACE was FALSE
        //
#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Capacity_Test_Max_ACE_In_One_ACL_Ideal);
#endif

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Capacity_Test_Max_ACE_In_One_ACL_Worst);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Capacity_Test_Max_ACE_In_One_ACL_Normal);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Capacity_Test_Max_ACE_In_One_ACL_Modify_On_Fly);

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Capacity_Test_Policy_Map_With_Max_ACE_In_One_ACL);

        if (0)
        {
            RULE_CTRL_UT_TEST(RULE_CTRL_UT_Set_QoS_With_MF_IPv6_Ext_ACL);
        }
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Set_QoS_With_MF_ACL_Mixed);

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Set_QoS_With_Match_Any_ACL_MF_Rule);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Set_QoS_With_Match_All_ACL_MF_Rule);

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Set_QoS_With_Modifying_Match_Cond_Of_Class_Map_On_Fly);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Set_QoS_With_Modifying_Match_Cond_Of_Class_Map_On_Fly_Undo);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Set_QoS_With_Modifying_ACE_On_Fly);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Set_QoS_With_Modifying_Meter_On_Fly);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Set_QoS_With_Modifying_Action_On_Fly);

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Set_QoS_Rule_AllocateRule_Fail_Case);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Set_QoS_Rule_SetRule_Fail_Case);

#if 0 // FIXME: Add time range test case
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Time_Based_ACL_Apply_ACE);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Time_Based_ACL_Remove_ACE);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Apply_ACL_With_Different_Time_Range);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Apply_All_Type_Of_ACLs_With_Different_Time_Range);
#endif /* 0 */

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Hot_Removal);

        //
        // These two cases will fail to validate resource info array. Skip the check.
        //
//        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Sort_Function_Type_List_By_Rule_Count);
        UT_Test(RULE_CTRL_UT_Setup, NULL, RULE_CTRL_UT_Sort_Function_Type_List_By_Rule_Count);

//        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Find_Best_Fit_Function_Type);
        UT_Test(RULE_CTRL_UT_Setup, NULL, RULE_CTRL_UT_Find_Best_Fit_Function_Type);

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Get_Next_Port);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Capacity_Test_For_ACL_List);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Capacity_And_Order_Test_For_ACL_List);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Get_Acl_Id_List_Using_Identical_Function_Type_Same_ACE_Count);        
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Get_Acl_Id_List_Using_Identical_Function_Type);

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Defrag_Rule);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Defrag_Rule_With_Add_ACL);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Defrag_Rule_With_Add_ACE_On_Fly);

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Get_Available_Function_Type);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Get_Available_Function_Type_With_FunctionRemap);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Change_Group_Attributes);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Recycle_Group);
        
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Dynamic_Selector_Assignment_By_MAC_ACL);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Dynamic_Selector_Assignment_Mixed_ACL);

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Get_TCAM_Entry);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Get_TCAM_Entry_AddAce);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_Get_TCAM_Entry_ForIPSG);

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_SetTcpUdpPortCosMap);

        RULE_CTRL_UT_TEST(RULE_CTRL_UT_TrapArpPacketToCpuWitherRate);
        RULE_CTRL_UT_TEST(RULE_CTRL_UT_TrapArpPacketToCpuWitherRate_trunk);
    }
    else
    {
        printf("%s (%d): Skip all rule_ctrl test cases\r\n", __FUNCTION__, __LINE__);
    }

    return 0;

#undef RULE_CTRL_UT_TEST
}


