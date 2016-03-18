/*
 *  rule_mgr_utest_cases.c
 *  l4_mgr
 *
 *  Created by yehjunying on 2012/5/28.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

/*------------------------------------------------------------------------------
 * Help Functions
 *------------------------------------------------------------------------------
 */
void
RULE_MGR_UT_Proc_Create_Policy_Map(
    const char *policy_map_name)
{
    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_MGR_CreatePolicyMap(policy_map_name);
    assert(RULE_TYPE_OK == result);
}

void
RULE_MGR_UT_Proc_Create_Class_Map(
    const char *class_map_name,
    RULE_TYPE_MatchType_T class_map_match_type)
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T class_map_index;

    RULE_TYPE_ClassMap_T class_map;

    result = RULE_MGR_CreateClassMap(class_map_name);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_GetClassMap(class_map_index, &class_map);
    assert(RULE_TYPE_OK == result);

    class_map.class_map_match_type = class_map_match_type;

    result = RULE_MGR_SetClassMap(class_map_index, &class_map);
    assert(RULE_TYPE_OK == result);
}

void
RULE_MGR_UT_Proc_Create_ACL(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    UI32_T number_of_ace,
    RULE_CTRL_UT_COMPRESS_OPTION_T compress_option)
{
    UI32_T out_acl_index;

    RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name,
                                             acl_type,
                                             number_of_ace,
                                             compress_option,
                                             &out_acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    {
        RULE_TYPE_RETURN_TYPE_T result;

        result = RULE_OM_RebuildAggregateAcl(out_acl_index);
        assert(RULE_TYPE_OK == result);
    }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE
}

RULE_TYPE_RETURN_TYPE_T
RULE_MGR_UT_Proc_Delete_ACL(
    const char *acl_name)
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T acl_index;

    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    return RULE_MGR_DelAcl(acl_index);
}


void
RULE_MGR_UT_Proc_Create_Periodic_Time_Range(
    const char *time_range_name,
    UI32_T start_hour,
    UI32_T start_minute,
    UI32_T end_hour,
    UI32_T end_minute)
{
    TIME_RANGE_ERROR_TYPE_T result;

    result = TIME_RANGE_PMGR_CreateTimeRangeEntry((UI8_T *)time_range_name);
    assert(TIME_RANGE_ERROR_TYPE_NONE == result);

    {
        TIME_RANGE_TYPE_PERIODIC_TIME_T periodic_time;

        memset(&periodic_time, 0, sizeof(periodic_time));
        periodic_time.start_type_of_wk = TIME_RANGE_TYPE_WEEK_DAILY;
        periodic_time.end_type_of_wk   = TIME_RANGE_TYPE_WEEK_DAILY;
        periodic_time.start_hour       = start_hour;
        periodic_time.start_minute     = start_minute;
        periodic_time.end_hour         = end_hour;
        periodic_time.end_minute       = end_minute;

        result = TIME_RANGE_PMGR_SetTimeRangePeriodic((UI8_T *)time_range_name, &periodic_time);
        assert(TIME_RANGE_ERROR_TYPE_NONE == result);
    }
}

void
RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map(
    const char *class_map_name,
    RULE_TYPE_MfType_T mf_type,
    UI32_T mf_value)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T class_map_index;

    RULE_TYPE_UI_ClassMapElement_T class_map_element;

    result = RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index);
    assert(RULE_TYPE_OK == result);

    class_map_element.class_type = RULE_TYPE_CLASS_MF;
    class_map_element.element.mf_entry.mf_type = mf_type;
    class_map_element.element.mf_entry.mf_value = mf_value;

    result = RULE_MGR_AddElement2ClassMap(class_map_index, &class_map_element);
    assert(RULE_TYPE_OK == result);
}

void
RULE_MGR_UT_Proc_Add_ACL_To_Class_Map(
    const char *class_map_name,
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    UI32_T number_of_ace)
{
    RULE_TYPE_RETURN_TYPE_T result;

    RULE_CTRL_UT_COMPRESS_OPTION_T compress_option = RULE_CTRL_UT_COMPRESS_DEFAULT;

    UI32_T class_map_index;
    UI32_T out_acl_index;

    result = RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index);
    assert(RULE_TYPE_OK == result);

    RULE_CTRL_UT_Proc_Add_ACL_Into_Class_Map(class_map_index, acl_name, acl_type,
                                             number_of_ace, compress_option,
                                             &out_acl_index);
}

void
RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter(
    const char *policy_map_name,
    const char *class_map_name,
    UI32_T meter_cir,
    UI32_T meter_cbs,
    RULE_TYPE_UI_ConfirmActionType_T confirm_action_type,
    UI32_T confirm_action_value,
    RULE_TYPE_UI_ViolateActionType_T violate_action_type,
    UI32_T violate_action_value)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T policy_map_index;
    UI32_T class_map_index;

    result = RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_AddElement2PolicyMap(policy_map_index, class_map_index);
    assert(RULE_TYPE_OK == result);

    {
        RULE_TYPE_TBParamEntry_T meter;

        meter.meter_model = RULE_TYPE_METER_MODE_FLOW;
        meter.rate = meter_cir;
        meter.burst_size = meter_cbs;

        meter.row_status = VAL_diffServMeterStatus_active;

        result = RULE_MGR_SetElementMeterFromPolicyMap(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == result);
    }

    {
        RULE_TYPE_UI_Action_T action;

        action.in_action_type = IN_ACTION_INVALID;
        action.in_action_value= 8;

        action.out_action_type = OUT_ACTION_INVALID;
        action.out_action_value = 8;

        action.confirm_action_type = CONFIRM_ACTION_INVALID;
        action.confirm_action_value = 64;

        action.exceed_action_type = EXCEED_ACTION_INVALID;
        action.exceed_action_value = 64;

        action.violate_action_type = violate_action_type;
        action.violate_action_value = violate_action_value;

        action.class_action_type = CLASS_ACTION_INVALID;

        action.confirm_action_type = confirm_action_type;
        action.confirm_action_value = confirm_action_value;

        result = RULE_MGR_SetElementActionFromPolicyMap(policy_map_index, class_map_index, &action);
        assert(RULE_TYPE_OK == result);
    }
}

void
RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter(
    const char *policy_map_name,
    const char *class_map_name,
    UI32_T rate)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T policy_map_index;
    UI32_T class_map_index;

    result = RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_GetClassMapIdByName(class_map_name, &class_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_AddElement2PolicyMap(policy_map_index, class_map_index);
    assert(RULE_TYPE_OK == result);

    {
        RULE_TYPE_TBParamEntry_T meter;

        meter.meter_model = RULE_TYPE_METER_MODE_RATE;
        meter.rate = rate;

        meter.row_status = VAL_diffServMeterStatus_active;

        result = RULE_MGR_SetElementMeterFromPolicyMap(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == result);
    }

    {
        RULE_TYPE_UI_Action_T action;

        action.in_action_type = IN_ACTION_INVALID;
        action.in_action_value= 8;

        action.out_action_type = OUT_ACTION_INVALID;
        action.out_action_value = 8;

        action.confirm_action_type = CONFIRM_ACTION_INVALID;
        action.confirm_action_value = 64;

        action.exceed_action_type = EXCEED_ACTION_INVALID;
        action.exceed_action_value = 64;

        action.violate_action_type = VIOLATE_ACTION_DROP;
        action.violate_action_value = 64;

        action.class_action_type = CLASS_ACTION_INVALID;

        action.confirm_action_type = CONFIRM_ACTION_TRANSMIT;
        action.confirm_action_value = 64;

        result = RULE_MGR_SetElementActionFromPolicyMap(policy_map_index, class_map_index, &action);
        assert(RULE_TYPE_OK == result);
    }
}

RULE_TYPE_RETURN_TYPE_T
RULE_MGR_UT_Proc_Bind_ACL(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    const char *acl_name,
    const char *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T acl_index;
    RULE_TYPE_AclType_T port_acl_type;

    result = RULE_MGR_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    {
        RULE_TYPE_Acl_T *acl_entry_p;

        acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_index);
        assert(NULL != acl_entry_p);

        result = RULE_OM_LocalAclTypeToPortAclType(acl_entry_p->acl_type, &port_acl_type);
        assert(RULE_TYPE_OK == result);
    }

    result = RULE_MGR_BindPort2Acl(ifindex,
                                   acl_index,
                                   port_acl_type,
                                   RULE_TYPE_INBOUND == direction ? TRUE : FALSE,
                                   (UI8_T *)time_range_name,
                                   counter_enable);

    RULE_CTRL_UT_Proc_Validate_Rule();

    return result;
}

RULE_TYPE_RETURN_TYPE_T
RULE_MGR_UT_Proc_UnBind_ACL(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    const char *acl_name)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T acl_index;

    RULE_TYPE_AclType_T port_acl_type;

    result = RULE_MGR_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    {
        RULE_TYPE_Acl_T *acl_entry_p;

        acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_index);
        assert(NULL != acl_entry_p);

        result = RULE_OM_LocalAclTypeToPortAclType(acl_entry_p->acl_type, &port_acl_type);
        assert(RULE_TYPE_OK == result);
    }

    result = RULE_MGR_UnBindPortFromAcl(ifindex,
                                        acl_index,
                                        port_acl_type,
                                        RULE_TYPE_INBOUND == direction ? TRUE : FALSE);

    RULE_CTRL_UT_Proc_Validate_Rule();

    return result;
}

RULE_TYPE_RETURN_TYPE_T
RULE_MGR_UT_Proc_Bind_Policy_Map(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    const char *policy_map_name)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T policy_map_index;

    result = RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_BindPort2PolicyMap(ifindex, direction, policy_map_index);

    RULE_CTRL_UT_Proc_Validate_Rule();

    return result;
}

RULE_TYPE_RETURN_TYPE_T
RULE_MGR_UT_Proc_UnBind_Policy_Map(
    UI32_T ifindex,
    RULE_TYPE_InOutDirection_T direction,
    const char *policy_map_name)
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T policy_map_index;

    result = RULE_MGR_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_UnBindPortFromPolicyMap(ifindex, direction, policy_map_index);

    RULE_CTRL_UT_Proc_Validate_Rule();

    return result;
}

/*------------------------------------------------------------------------------
 * Test Functions
 *------------------------------------------------------------------------------
 */
int
RULE_MGR_UT_Setup()
{
    RULE_CTRL_UT_Setup();

    RULE_MGR_InitiateSystemResources();
    RULE_MGR_EnterMasterMode();

    return 0;
}

int
RULE_MGR_UT_Teardown()
{
    RULE_CTRL_UT_Teardown();
    return 0;
}

#pragma mark Basic ACL test items

int
RULE_MGR_UT_Apply_ACL()
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

    UI32_T i;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                             RULE_TYPE_INBOUND},
        {2,                                             RULE_TYPE_INBOUND},
        {3,                                             RULE_TYPE_INBOUND},
#if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_INBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_INBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_INBOUND},
#endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                             RULE_TYPE_OUTBOUND},
        {2,                                             RULE_TYPE_OUTBOUND},
        {3,                                             RULE_TYPE_OUTBOUND},
    #if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_OUTBOUND},
    #endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        {SYS_ADPT_CPU_1_IF_INDEX_NUMBER,                RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,         RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    for (i = 0; i < _countof(acls); ++ i)
    {
        UI32_T pi;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        RULE_MGR_UT_Proc_Create_ACL(acl_name, acls[i].acl_type, MAX_ACE, RULE_CTRL_UT_COMPRESS_DEFAULT);

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;
            const char time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

            result = RULE_MGR_UT_Proc_Bind_ACL(ifindex, direction, acl_name, time_range_name, counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

            result = RULE_MGR_UT_Proc_UnBind_ACL(ifindex, direction, acl_name);
            assert(RULE_TYPE_OK == result);
        }

        RULE_MGR_UT_Proc_Delete_ACL(acl_name);
    }

    return 0;
}

int
RULE_MGR_UT_Apply_ACL_With_Same_Time_Range()
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
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

    UI32_T i;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                             RULE_TYPE_INBOUND},
        {2,                                             RULE_TYPE_INBOUND},
        {3,                                             RULE_TYPE_INBOUND},
#if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_INBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_INBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_INBOUND},
#endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                             RULE_TYPE_OUTBOUND},
        {2,                                             RULE_TYPE_OUTBOUND},
        {3,                                             RULE_TYPE_OUTBOUND},
    #if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_OUTBOUND},
    #endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        {SYS_ADPT_CPU_1_IF_INDEX_NUMBER,                RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,         RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    RULE_MGR_UT_Proc_Create_Periodic_Time_Range("daily-8-to-12", 8, 0, 12, 0);

    for (i = 0; i < _countof(acls); ++ i)
    {
        UI32_T pi;
        UI32_T cur_hour;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        RULE_MGR_UT_Proc_Create_ACL(acl_name, acls[i].acl_type, MAX_ACE, RULE_CTRL_UT_COMPRESS_DEFAULT);

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;
            const char *time_range_name = "daily-8-to-12";
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            result = RULE_MGR_UT_Proc_Bind_ACL(ifindex, direction, acl_name, time_range_name, counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        for (cur_hour = 0; cur_hour < 24; ++ cur_hour)
        {
            int year, month, day, hour, minute, second;
            BOOL_T b_result;

            year    = 2001;
            month   = 1;
            day     = 1;
            hour    = cur_hour;
            minute  = 0;
            second  = 0;

            b_result = SYS_TIME_SetRealTimeClock(year, month, day, hour, minute, second);
            assert(TRUE == b_result);

            //FIXEME:
            //TIME_RANGE_MGR_HandleTimerEvent();
            //RULE_CTRL_ProcessTimeRangeStatusChange(_change_list, _status_list);

            RULE_CTRL_UT_Proc_Validate_Rule();
        }

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

            result = RULE_MGR_UT_Proc_UnBind_ACL(ifindex, direction, acl_name);
            assert(RULE_TYPE_OK == result);
        }

        RULE_MGR_UT_Proc_Delete_ACL(acl_name);
    }

    TIME_RANGE_PMGR_DeleteTimeRangeEntry((UI8_T*)"daily-8-to-12");
#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

int
RULE_MGR_UT_Apply_ACL_With_Diff_Time_Range()
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
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

    UI32_T i;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                             RULE_TYPE_INBOUND},
        {2,                                             RULE_TYPE_INBOUND},
        {3,                                             RULE_TYPE_INBOUND},
#if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_INBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_INBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_INBOUND},
#endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                             RULE_TYPE_OUTBOUND},
        {2,                                             RULE_TYPE_OUTBOUND},
        {3,                                             RULE_TYPE_OUTBOUND},
    #if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_OUTBOUND},
    #endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        {SYS_ADPT_CPU_1_IF_INDEX_NUMBER,                RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,         RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    RULE_MGR_UT_Proc_Create_Periodic_Time_Range("daily-8-to-12",   8, 0, 12, 0);
    RULE_MGR_UT_Proc_Create_Periodic_Time_Range("daily-13-to-15", 13, 0, 15, 0);

    for (i = 0; i < _countof(acls); ++ i)
    {
        UI32_T pi;
        UI32_T cur_hour;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        RULE_MGR_UT_Proc_Create_ACL(acl_name, acls[i].acl_type, MAX_ACE, RULE_CTRL_UT_COMPRESS_DEFAULT);

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;
            char *time_range_name;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            switch (pi % 3)
            {
                case 0:
                    time_range_name = NULL;
                    break;
                case 1:
                    time_range_name = "daily-8-to-12";
                    break;
                case 2:
                    time_range_name = "daily-13-to-15";
                    break;
                default:
                    assert(0);
                    break;
            }

            result = RULE_MGR_UT_Proc_Bind_ACL(ifindex, direction, acl_name, time_range_name, counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        for (cur_hour = 0; cur_hour < 24; ++ cur_hour)
        {
            int year, month, day, hour, minute, second;
            BOOL_T b_result;

            year    = 2001;
            month   = 1;
            day     = 1;

            hour    = cur_hour;

            minute  = 0;
            second  = 0;

            b_result = SYS_TIME_SetRealTimeClock(year, month, day, hour, minute, second);
            assert(TRUE == b_result);

            //RULE_CTRL_LocalTimeBasedACLProcessTimerEvent();
            TIME_RANGE_MGR_HandleTimerEvent();
            RULE_CTRL_UT_Proc_Validate_Rule();
        }

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

            result = RULE_MGR_UT_Proc_UnBind_ACL(ifindex, direction, acl_name);
            assert(RULE_TYPE_OK == result);
        }

        RULE_MGR_UT_Proc_Delete_ACL(acl_name);
    }

    TIME_RANGE_PMGR_DeleteTimeRangeEntry((UI8_T*)"daily-8-to-12");
    TIME_RANGE_PMGR_DeleteTimeRangeEntry((UI8_T*)"daily-13-to-15");
#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

int
RULE_MGR_UT_Apply_ACL_With_Counter()
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)
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

    UI32_T i;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                             RULE_TYPE_INBOUND},
        {2,                                             RULE_TYPE_INBOUND},
        {3,                                             RULE_TYPE_INBOUND},
#if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_INBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_INBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_INBOUND},
#endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                             RULE_TYPE_OUTBOUND},
        {2,                                             RULE_TYPE_OUTBOUND},
        {3,                                             RULE_TYPE_OUTBOUND},
    #if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_OUTBOUND},
    #endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        {SYS_ADPT_CPU_1_IF_INDEX_NUMBER,                RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,         RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    for (i = 0; i < _countof(acls); ++ i)
    {
        UI32_T pi;

        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        RULE_MGR_UT_Proc_Create_ACL(acl_name, acls[i].acl_type, MAX_ACE, RULE_CTRL_UT_COMPRESS_DEFAULT);

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;
            const char time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_ENABLE;

            result = RULE_MGR_UT_Proc_Bind_ACL(ifindex, direction, acl_name, time_range_name, counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

            result = RULE_MGR_UT_Proc_UnBind_ACL(ifindex, direction, acl_name);
            assert(RULE_TYPE_OK == result);
        }

        RULE_MGR_UT_Proc_Delete_ACL(acl_name);
    }
#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

#if 0 /* FIXME: Time range */
int
RULE_MGR_UT_Apply_ACL_With_Time_Range_And_Counter()
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE && SYS_CPNT_ACL_COUNTER == TRUE)
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

    UI32_T i;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                             RULE_TYPE_INBOUND},
        {2,                                             RULE_TYPE_INBOUND},
        {3,                                             RULE_TYPE_INBOUND},
#if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_INBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_INBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_INBOUND},
#endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                             RULE_TYPE_OUTBOUND},
        {2,                                             RULE_TYPE_OUTBOUND},
        {3,                                             RULE_TYPE_OUTBOUND},
    #if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_OUTBOUND},
    #endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        {SYS_ADPT_CPU_1_IF_INDEX_NUMBER,                RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,         RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    RULE_MGR_UT_Proc_Create_Periodic_Time_Range("daily-8-to-12",   8, 0, 12, 0);
    RULE_MGR_UT_Proc_Create_Periodic_Time_Range("daily-13-to-15", 13, 0, 15, 0);

    for (i = 0; i < _countof(acls); ++ i)
    {
        UI32_T pi;
        UI32_T cur_hour;

        char acl_name[30];
        UI32_T acl_index;

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        RULE_MGR_UT_Proc_Create_ACL(acl_name, acls[i].acl_type, MAX_ACE, RULE_CTRL_UT_COMPRESS_DEFAULT);

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;
            char *time_range_name;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_ENABLE;

            switch (pi % 3)
            {
                case 0:
                    time_range_name = NULL;
                    break;
                case 1:
                    time_range_name = "daily-8-to-12";
                    break;
                case 2:
                    time_range_name = "daily-13-to-15";
                    break;
                default:
                    assert(0);
                    break;
            }

            result = RULE_MGR_UT_Proc_Bind_ACL(ifindex, direction, acl_name, time_range_name, counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        for (cur_hour = 0; cur_hour < 24; ++ cur_hour)
        {
            int year, month, day, hour, minute, second;
            BOOL_T b_result;

            year    = 2001;
            month   = 1;
            day     = 1;

            hour    = cur_hour;

            minute  = 0;
            second  = 0;

            b_result = SYS_TIME_SetRealTimeClock(year, month, day, hour, minute, second);
            assert(TRUE == b_result);

            RULE_CTRL_LocalTimeBasedACLProcessTimerEvent();
            RULE_CTRL_UT_Proc_Validate_Rule();
        }

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

            result = RULE_MGR_UT_Proc_UnBind_ACL(ifindex, direction, acl_name);
            assert(RULE_TYPE_OK == result);
        }

        RULE_OM_DestroyAcl(acl_index);
    }
#endif // SYS_CPNT_ACL_COUNTER && SYS_CPNT_ACL_COUNTER

    return 0;
}
#endif /* 0 */

int
RULE_MGR_UT_Apply_All_Type_Of_ACLs_On_Port_At_The_Same_Time()
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
#if (PLATFORM_MAX_NBR_OF_ACL_PER_PORT > 1)
        {
            RULE_TYPE_IP_EXT_ACL
        },
        {
            RULE_TYPE_IPV6_EXT_ACL
        },
#endif // PLATFORM_MAX_NBR_OF_ACL_PER_PORT > 1
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i, j;

    UI32_T port_list[] =
    {
        1,
        2,
        3,
#if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,
#endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */
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

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

        for (j = 0; j < _countof(port_list); ++ j)
        {
            UI32_T ifindex = port_list[j];

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            for (interface.direction = MIN_DIRECTION; interface.direction <= MAX_DIRECTION; ++ interface.direction)
            {
                BOOL_T ingress_flag = RULE_TYPE_DIRECTION_TO_IS_INGRESS(interface.direction);

                UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

                RULE_TYPE_AclType_T port_acl_type;

                result = RULE_OM_LocalAclTypeToPortAclType(acls[i].acl_type, &port_acl_type);
                assert(RULE_TYPE_OK == result);

                result = RULE_MGR_BindPort2Acl(ifindex, acl_index, port_acl_type, ingress_flag, time_range_name, counter_enable);
                assert(RULE_TYPE_OK == result);

                RULE_CTRL_UT_Proc_Validate_Rule();
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
                BOOL_T ingress_flag = RULE_TYPE_DIRECTION_TO_IS_INGRESS(interface.direction);

                RULE_TYPE_AclType_T port_acl_type;

                result = RULE_OM_LocalAclTypeToPortAclType(acls[i].acl_type, &port_acl_type);
                assert(RULE_TYPE_OK == result);

                result = RULE_MGR_UnBindPortFromAcl(ifindex, acl_index, port_acl_type, ingress_flag);
                assert(RULE_TYPE_OK == result);

                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    return 0;
}

int
RULE_MGR_UT_Delete_ACL_Shall_Unbind_From_Port()
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

    UI32_T i;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                             RULE_TYPE_INBOUND},
        {2,                                             RULE_TYPE_INBOUND},
        {3,                                             RULE_TYPE_INBOUND},
#if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_INBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_INBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_INBOUND},
#endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                             RULE_TYPE_OUTBOUND},
        {2,                                             RULE_TYPE_OUTBOUND},
        {3,                                             RULE_TYPE_OUTBOUND},
    #if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_OUTBOUND},
    #endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        {SYS_ADPT_CPU_1_IF_INDEX_NUMBER,                RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,         RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    for (i = 0; i < _countof(acls); ++ i)
    {
        UI32_T pi;

        char acl_name[30];
//        UI32_T acl_index;

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        RULE_MGR_UT_Proc_Create_ACL(acl_name, acls[i].acl_type, MAX_ACE, RULE_CTRL_UT_COMPRESS_DEFAULT);

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;
            char time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

            result = RULE_MGR_UT_Proc_Bind_ACL(ifindex, direction, acl_name, time_range_name, counter_enable);
            assert(RULE_TYPE_OK == result);
        }

//        for (pi = 0; pi < _countof(port_list); ++ pi)
//        {
//            UI32_T ifindex = port_list[pi].ifindex;
//            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;
//
//            result = RULE_MGR_UT_Proc_UnBind_ACL(ifindex, direction, acl_name);
//            assert(RULE_TYPE_OK == result);
//        }

//        RULE_OM_DestroyAcl(acl_index);
//        result = RULE_MGR_DelAcl(acl_index);
//        result = RULE_MGR_UT_Proc_Delete_ACL(acl_name);
//        assert(RULE_TYPE_OK != result);
//
//        RULE_CTRL_UT_Proc_Validate_Rule();
//
//        for (pi = 0; pi < _countof(port_list); ++ pi)
//        {
//            UI32_T ifindex = port_list[pi].ifindex;
//            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;
//
//            result = RULE_MGR_UT_Proc_UnBind_ACL(ifindex, direction, acl_name);
//            assert(RULE_TYPE_OK == result);
//        }

        result = RULE_MGR_UT_Proc_Delete_ACL(acl_name);
        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();
    }

    return 0;
}

int
RULE_MGR_UT_Get_Next_Hardware_ACE()
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
#if (PLATFORM_MAX_NBR_OF_ACL_PER_PORT > 1)
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
#endif // PLATFORM_MAX_NBR_OF_ACL_PER_PORT > 1
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i, j;

    UI32_T port_list[] =
    {
        1,
        2,
        3,
#if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,
        SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,
#endif /* (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1) */
    };

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[30];
        UI32_T acl_index;

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acls[i].acl_type,
                                                 MAX_ACE, RULE_CTRL_UT_COMPRESS_ENABLE,
                                                 &acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

        for (j = 0; j < _countof(port_list); ++ j)
        {
            UI32_T ifindex = port_list[j];

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            for (interface.direction = MIN_DIRECTION; interface.direction <= MAX_DIRECTION; ++ interface.direction)
            {
                BOOL_T ingress_flag = RULE_TYPE_DIRECTION_TO_IS_INGRESS(interface.direction);

                UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

                RULE_TYPE_AclType_T port_acl_type;

                result = RULE_OM_LocalAclTypeToPortAclType(acls[i].acl_type, &port_acl_type);
                assert(RULE_TYPE_OK == result);

                result = RULE_MGR_BindPort2Acl(ifindex, acl_index, port_acl_type, ingress_flag, time_range_name, counter_enable);
                assert(RULE_TYPE_OK == result);

                RULE_CTRL_UT_Proc_Validate_Rule();

                {
                    RULE_TYPE_AclType_T ace_type;
                    UI32_T ace_index;
                    RULE_TYPE_Ace_Entry_T ace_entry;

                    UI32_T ace_count = 0;

                    ace_index = 0;
                    while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, interface.direction, acl_index,
                                                                            &ace_type, &ace_index, &ace_entry))
                    {
                        ace_count ++;
                    }

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
                    {
                        RULE_TYPE_Acl_T *acl_entry_p;
                        acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_index);
                        assert(NULL != acl_entry_p);

                        assert(TRUE == acl_entry_p->flag_aggregate_done);
                        acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_entry_p->aggregate_acl_index);
                        assert(NULL != acl_entry_p);
                        assert(acl_entry_p->ace_count == ace_count);
                    }
#else
                    {
                        RULE_TYPE_Acl_T *acl_entry_p;
                        acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_index);
                        assert(NULL != acl_entry_p);

                        assert(acl_entry_p->ace_count == ace_count);
                    }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

                }
            }
        }
    }

    return 0;
}

void
CLI_ACL_UT_Proc_Set_ACL_Counter(
    UI32_T ifindex,
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_InOutDirection_T direction
);

int
RULE_MGR_UT_Clear_Hardware_ACL_Counter_For_All()
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)
    enum
    {
        MAX_ACE  = 5,
    };

    struct
    {
        RULE_TYPE_AclType_T acl_type;
        char acl_name[30];
        UI32_T acl_index;
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL,
            {0},
            0,
            1,
            RULE_TYPE_INBOUND,
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            {0},
            0,
            2,
            RULE_TYPE_INBOUND,
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            3,
            RULE_TYPE_INBOUND,
        },
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {
            RULE_TYPE_MAC_ACL,
            {0},
            0,
            1,
            RULE_TYPE_OUTBOUND,
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            {0},
            0,
            2,
            RULE_TYPE_OUTBOUND,
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            3,
            RULE_TYPE_OUTBOUND,
        },
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,
            RULE_TYPE_INBOUND,
        },
#endif // SYS_CPNT_QOS_V2_ALL_PORTS

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            SYS_ADPT_CPU_1_IF_INDEX_NUMBER,
            RULE_TYPE_OUTBOUND,
        }
#endif // SYS_CPNT_QOS_V2_CPU_PORT
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ai;

    for (ai = 0; ai < _countof(acls); ++ ai)
    {
        snprintf(acls[ai].acl_name, sizeof(acls[ai].acl_name), "%lu", ai);
        acls[ai].acl_name[sizeof(acls[ai].acl_name)-1] = '\0';

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acls[ai].acl_name, acls[ai].acl_type,
                                                 MAX_ACE, RULE_CTRL_UT_COMPRESS_ENABLE,
                                                 &acls[ai].acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        result = RULE_OM_RebuildAggregateAcl(acls[ai].acl_index);
        assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

        {
            UI32_T ifindex = acls[ai].ifindex;

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            interface.direction = acls[ai].direction;

            {
                BOOL_T ingress_flag = RULE_TYPE_DIRECTION_TO_IS_INGRESS(interface.direction);

                UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_ENABLE;

                RULE_TYPE_AclType_T port_acl_type;

                result = RULE_OM_LocalAclTypeToPortAclType(acls[ai].acl_type, &port_acl_type);
                assert(RULE_TYPE_OK == result);

                result = RULE_MGR_BindPort2Acl(ifindex, acls[ai].acl_index, port_acl_type, ingress_flag, time_range_name, counter_enable);
                assert(RULE_TYPE_OK == result);

                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    //
    // Setup counter
    //
    for (ai = 0; ai < _countof(acls); ++ ai)
    {
        UI32_T ifindex = acls[ai].ifindex;
        RULE_TYPE_InOutDirection_T direction = acls[ai].direction;

        CLI_ACL_UT_Proc_Set_ACL_Counter(ifindex, acls[ai].acl_name, acls[ai].acl_type, direction);

        {
            RULE_TYPE_AclType_T ace_type;
            UI32_T ace_index;
            RULE_TYPE_Ace_Entry_T ace_entry;

            ace_index = 0;
            while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, direction, acls[ai].acl_index,
                                                                    &ace_type, &ace_index, &ace_entry))
            {
                assert(ace_entry.counter.total_packets != 0);
            }
        }
    }

    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        interface.type = RULE_TYPE_INTERFACE_UNSPEC;
        interface.direction = RULE_TYPE_UNSPEC_DIRECTION;
        result = RULE_MGR_ClearAclCounter(0xffffffff, 0xffffffff, &interface);
        assert(RULE_TYPE_OK == result);
    }

    for (ai = 0; ai < _countof(acls); ++ ai)
    {
        UI32_T ifindex = acls[ai].ifindex;
        RULE_TYPE_InOutDirection_T direction = acls[ai].direction;

        RULE_TYPE_AclType_T ace_type;
        UI32_T ace_index;
        RULE_TYPE_Ace_Entry_T ace_entry;

        ace_index = 0;
        while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, direction, acls[ai].acl_index,
                                                                &ace_type, &ace_index, &ace_entry))
        {
            assert(ace_entry.counter.total_packets == 0);
        }
    }

    //
    // Try use RULE_TYPE_BOTH_DIRECTION again
    //

    //
    // Setup counter
    //
    for (ai = 0; ai < _countof(acls); ++ ai)
    {
        UI32_T ifindex = acls[ai].ifindex;
        RULE_TYPE_InOutDirection_T direction = acls[ai].direction;

        CLI_ACL_UT_Proc_Set_ACL_Counter(ifindex, acls[ai].acl_name, acls[ai].acl_type, direction);

        {
            RULE_TYPE_AclType_T ace_type;
            UI32_T ace_index;
            RULE_TYPE_Ace_Entry_T ace_entry;

            ace_index = 0;
            while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, direction, acls[ai].acl_index,
                                                                    &ace_type, &ace_index, &ace_entry))
            {
                assert(ace_entry.counter.total_packets != 0);
            }
        }
    }

    {
        RULE_TYPE_INTERFACE_INFO_T interface;

        interface.type = RULE_TYPE_INTERFACE_UNSPEC;
        interface.direction = RULE_TYPE_BOTH_DIRECTION;
        result = RULE_MGR_ClearAclCounter(0xffffffff, 0xffffffff, &interface);
        assert(RULE_TYPE_OK == result);
    }

    for (ai = 0; ai < _countof(acls); ++ ai)
    {
        UI32_T ifindex = acls[ai].ifindex;
        RULE_TYPE_InOutDirection_T direction = acls[ai].direction;

        RULE_TYPE_AclType_T ace_type;
        UI32_T ace_index;
        RULE_TYPE_Ace_Entry_T ace_entry;

        ace_index = 0;
        while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, direction, acls[ai].acl_index,
                                                                &ace_type, &ace_index, &ace_entry))
        {
            assert(ace_entry.counter.total_packets == 0);
        }
    }

#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

int
RULE_MGR_UT_Clear_Hardware_ACL_Counter_By_Direction()
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)
    enum
    {
        MAX_ACE  = 5,
    };

    struct
    {
        RULE_TYPE_AclType_T acl_type;
        char acl_name[30];
        UI32_T acl_index;
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL,
            {0},
            0,
            1,
            RULE_TYPE_INBOUND,
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            {0},
            0,
            2,
            RULE_TYPE_INBOUND,
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            3,
            RULE_TYPE_INBOUND,
        },
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {
            RULE_TYPE_MAC_ACL,
            {0},
            0,
            1,
            RULE_TYPE_OUTBOUND,
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            {0},
            0,
            2,
            RULE_TYPE_OUTBOUND,
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            3,
            RULE_TYPE_OUTBOUND,
        },
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,
            RULE_TYPE_INBOUND,
        },
#endif // SYS_CPNT_QOS_V2_ALL_PORTS

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            SYS_ADPT_CPU_1_IF_INDEX_NUMBER,
            RULE_TYPE_OUTBOUND,
        }
#endif // SYS_CPNT_QOS_V2_CPU_PORT
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ai;

    for (ai = 0; ai < _countof(acls); ++ ai)
    {
        snprintf(acls[ai].acl_name, sizeof(acls[ai].acl_name), "%lu", ai);
        acls[ai].acl_name[sizeof(acls[ai].acl_name)-1] = '\0';

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acls[ai].acl_name, acls[ai].acl_type,
                                                 MAX_ACE, RULE_CTRL_UT_COMPRESS_ENABLE,
                                                 &acls[ai].acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        result = RULE_OM_RebuildAggregateAcl(acls[ai].acl_index);
        assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

        {
            UI32_T ifindex = acls[ai].ifindex;

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            interface.direction = acls[ai].direction;

            {
                BOOL_T ingress_flag = RULE_TYPE_DIRECTION_TO_IS_INGRESS(interface.direction);

                UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_ENABLE;

                RULE_TYPE_AclType_T port_acl_type;

                result = RULE_OM_LocalAclTypeToPortAclType(acls[ai].acl_type, &port_acl_type);
                assert(RULE_TYPE_OK == result);

                result = RULE_MGR_BindPort2Acl(ifindex, acls[ai].acl_index, port_acl_type, ingress_flag, time_range_name, counter_enable);
                assert(RULE_TYPE_OK == result);

                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    {
        UI32_T filter_direction;

        for (filter_direction = RULE_TYPE_INBOUND; filter_direction <= RULE_TYPE_UNSPEC_DIRECTION; ++ filter_direction)
        {
            for (ai = 0; ai < _countof(acls); ++ ai)
            {
                UI32_T ifindex = acls[ai].ifindex;
                RULE_TYPE_InOutDirection_T direction = acls[ai].direction;

                CLI_ACL_UT_Proc_Set_ACL_Counter(ifindex, acls[ai].acl_name, acls[ai].acl_type, direction);

                {
                    RULE_TYPE_AclType_T ace_type;
                    UI32_T ace_index;
                    RULE_TYPE_Ace_Entry_T ace_entry;

                    ace_index = 0;
                    while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, direction, acls[ai].acl_index,
                                                                            &ace_type, &ace_index, &ace_entry))
                    {
                        assert(ace_entry.counter.total_packets != 0);
                    }
                }
            }

            {
                RULE_TYPE_INTERFACE_INFO_T filter_interface;

                filter_interface.type = RULE_TYPE_INTERFACE_UNSPEC;
                filter_interface.direction = filter_direction;
                result = RULE_MGR_ClearAclCounter(0xffffffff, 0xffffffff, &filter_interface);
                assert(RULE_TYPE_OK == result);
            }

            for (ai = 0; ai < _countof(acls); ++ ai)
            {
                UI32_T ifindex = acls[ai].ifindex;
                RULE_TYPE_InOutDirection_T direction = acls[ai].direction;

                RULE_TYPE_AclType_T ace_type;
                UI32_T ace_index;
                RULE_TYPE_Ace_Entry_T ace_entry;

                ace_index = 0;
                while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, direction, acls[ai].acl_index,
                                                                        &ace_type, &ace_index, &ace_entry))
                {
                    if (RULE_TYPE_INBOUND == filter_direction ||
                        RULE_TYPE_OUTBOUND == filter_direction)
                    {
                        if (filter_direction == direction)
                        {
                            assert(ace_entry.counter.total_packets == 0);
                        }
                        else
                        {
                            assert(ace_entry.counter.total_packets != 0);
                        }
                    }
                    else if (RULE_TYPE_BOTH_DIRECTION == filter_direction ||
                             RULE_TYPE_UNSPEC_DIRECTION == filter_direction)
                    {
                        assert(ace_entry.counter.total_packets == 0);
                    }
                    else
                    {
                        assert(FALSE);
                    }
                }
            }

        }
    }

#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

int
RULE_MGR_UT_Clear_Hardware_ACL_Counter_By_Port()
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)
    enum
    {
        MAX_ACE  = 5,
    };

    struct
    {
        RULE_TYPE_AclType_T acl_type;
        char acl_name[30];
        UI32_T acl_index;
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL,
            {0},
            0,
            1,
            RULE_TYPE_INBOUND,
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            {0},
            0,
            2,
            RULE_TYPE_INBOUND,
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            3,
            RULE_TYPE_INBOUND,
        },
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {
            RULE_TYPE_MAC_ACL,
            {0},
            0,
            1,
            RULE_TYPE_OUTBOUND,
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            {0},
            0,
            2,
            RULE_TYPE_OUTBOUND,
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            3,
            RULE_TYPE_OUTBOUND,
        },
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,
            RULE_TYPE_INBOUND,
        },
#endif // SYS_CPNT_QOS_V2_ALL_PORTS

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            SYS_ADPT_CPU_1_IF_INDEX_NUMBER,
            RULE_TYPE_OUTBOUND,
        }
#endif // SYS_CPNT_QOS_V2_CPU_PORT
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ai;

    for (ai = 0; ai < _countof(acls); ++ ai)
    {
        snprintf(acls[ai].acl_name, sizeof(acls[ai].acl_name), "%lu", ai);
        acls[ai].acl_name[sizeof(acls[ai].acl_name)-1] = '\0';

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acls[ai].acl_name, acls[ai].acl_type,
                                                 MAX_ACE, RULE_CTRL_UT_COMPRESS_ENABLE,
                                                 &acls[ai].acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        result = RULE_OM_RebuildAggregateAcl(acls[ai].acl_index);
        assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

        {
            UI32_T ifindex = acls[ai].ifindex;

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            interface.direction = acls[ai].direction;

            {
                BOOL_T ingress_flag = RULE_TYPE_DIRECTION_TO_IS_INGRESS(interface.direction);

                UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_ENABLE;

                RULE_TYPE_AclType_T port_acl_type;

                result = RULE_OM_LocalAclTypeToPortAclType(acls[ai].acl_type, &port_acl_type);
                assert(RULE_TYPE_OK == result);

                result = RULE_MGR_BindPort2Acl(ifindex, acls[ai].acl_index, port_acl_type, ingress_flag, time_range_name, counter_enable);
                assert(RULE_TYPE_OK == result);

                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    {
        UI32_T filter_plist_idx;

        for (filter_plist_idx = 0; filter_plist_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ filter_plist_idx)
        {
            UI32_T filter_ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(filter_plist_idx);

            for (ai = 0; ai < _countof(acls); ++ ai)
            {
                UI32_T ifindex = acls[ai].ifindex;
                RULE_TYPE_InOutDirection_T direction = acls[ai].direction;

                CLI_ACL_UT_Proc_Set_ACL_Counter(ifindex, acls[ai].acl_name, acls[ai].acl_type, direction);

                {
                    RULE_TYPE_AclType_T ace_type;
                    UI32_T ace_index;
                    RULE_TYPE_Ace_Entry_T ace_entry;

                    ace_index = 0;
                    while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, direction, acls[ai].acl_index,
                                                                            &ace_type, &ace_index, &ace_entry))
                    {
                        assert(ace_entry.counter.total_packets != 0);
                    }
                }
            }

            {
                RULE_TYPE_INTERFACE_INFO_T filter_interface;

                RULE_CTRL_Ifindex_To_Interface(filter_ifindex, &filter_interface);
                filter_interface.direction = RULE_TYPE_UNSPEC_DIRECTION;
                result = RULE_MGR_ClearAclCounter(0xffffffff, 0xffffffff, &filter_interface);
                assert(RULE_TYPE_OK == result);
            }

            for (ai = 0; ai < _countof(acls); ++ ai)
            {
                UI32_T ifindex = acls[ai].ifindex;
                RULE_TYPE_InOutDirection_T direction = acls[ai].direction;

                RULE_TYPE_AclType_T ace_type;
                UI32_T ace_index;
                RULE_TYPE_Ace_Entry_T ace_entry;

                ace_index = 0;
                while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, direction, acls[ai].acl_index,
                                                                        &ace_type, &ace_index, &ace_entry))
                {
                    if (filter_ifindex == ifindex)
                    {
                        assert(ace_entry.counter.total_packets == 0);
                    }
                    else
                    {
                        assert(ace_entry.counter.total_packets != 0);
                    }
                }
            }

        }
    }

#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

int
RULE_MGR_UT_Clear_Hardware_ACL_Counter_By_ACL()
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)
    enum
    {
        MAX_ACE  = 5,
    };

    struct
    {
        RULE_TYPE_AclType_T acl_type;
        char acl_name[30];
        UI32_T acl_index;
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL,
            {0},
            0,
            1,
            RULE_TYPE_INBOUND,
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            {0},
            0,
            2,
            RULE_TYPE_INBOUND,
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            3,
            RULE_TYPE_INBOUND,
        },
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {
            RULE_TYPE_MAC_ACL,
            {0},
            0,
            1,
            RULE_TYPE_OUTBOUND,
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            {0},
            0,
            2,
            RULE_TYPE_OUTBOUND,
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            3,
            RULE_TYPE_OUTBOUND,
        },
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,
            RULE_TYPE_INBOUND,
        },
#endif // SYS_CPNT_QOS_V2_ALL_PORTS

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            SYS_ADPT_CPU_1_IF_INDEX_NUMBER,
            RULE_TYPE_OUTBOUND,
        }
#endif // SYS_CPNT_QOS_V2_CPU_PORT
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ai;

    for (ai = 0; ai < _countof(acls); ++ ai)
    {
        snprintf(acls[ai].acl_name, sizeof(acls[ai].acl_name), "%lu", ai);
        acls[ai].acl_name[sizeof(acls[ai].acl_name)-1] = '\0';

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acls[ai].acl_name, acls[ai].acl_type,
                                                 MAX_ACE, RULE_CTRL_UT_COMPRESS_ENABLE,
                                                 &acls[ai].acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        result = RULE_OM_RebuildAggregateAcl(acls[ai].acl_index);
        assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

        {
            UI32_T ifindex = acls[ai].ifindex;

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            interface.direction = acls[ai].direction;

            {
                BOOL_T ingress_flag = RULE_TYPE_DIRECTION_TO_IS_INGRESS(interface.direction);

                UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_ENABLE;

                RULE_TYPE_AclType_T port_acl_type;

                result = RULE_OM_LocalAclTypeToPortAclType(acls[ai].acl_type, &port_acl_type);
                assert(RULE_TYPE_OK == result);

                result = RULE_MGR_BindPort2Acl(ifindex, acls[ai].acl_index, port_acl_type, ingress_flag, time_range_name, counter_enable);
                assert(RULE_TYPE_OK == result);

                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    {
        UI32_T filter_ai;

        for (filter_ai = 0; filter_ai < _countof(acls); ++ filter_ai)
        {
            for (ai = 0; ai < _countof(acls); ++ ai)
            {
                UI32_T ifindex = acls[ai].ifindex;
                RULE_TYPE_InOutDirection_T direction = acls[ai].direction;

                CLI_ACL_UT_Proc_Set_ACL_Counter(ifindex, acls[ai].acl_name, acls[ai].acl_type, direction);

                {
                    RULE_TYPE_AclType_T ace_type;
                    UI32_T ace_index;
                    RULE_TYPE_Ace_Entry_T ace_entry;

                    ace_index = 0;
                    while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, direction, acls[ai].acl_index,
                                                                            &ace_type, &ace_index, &ace_entry))
                    {
                        assert(ace_entry.counter.total_packets != 0);
                    }
                }
            }

            {
                RULE_TYPE_INTERFACE_INFO_T filter_interface;

                filter_interface.type = RULE_TYPE_INTERFACE_UNSPEC;
                filter_interface.direction = RULE_TYPE_UNSPEC_DIRECTION;
                result = RULE_MGR_ClearAclCounter(acls[filter_ai].acl_index, 0xffffffff, &filter_interface);
                assert(RULE_TYPE_OK == result);
            }

            for (ai = 0; ai < _countof(acls); ++ ai)
            {
                UI32_T ifindex = acls[ai].ifindex;
                RULE_TYPE_InOutDirection_T direction = acls[ai].direction;

                RULE_TYPE_AclType_T ace_type;
                UI32_T ace_index;
                RULE_TYPE_Ace_Entry_T ace_entry;

                ace_index = 0;
                while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, direction, acls[ai].acl_index,
                                                                        &ace_type, &ace_index, &ace_entry))
                {
                    if (acls[filter_ai].acl_index == acls[ai].acl_index)
                    {
                        assert(ace_entry.counter.total_packets == 0);
                    }
                    else
                    {
                        assert(ace_entry.counter.total_packets != 0);
                    }
                }
            }

        }
    }

#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

int
RULE_MGR_UT_Clear_Hardware_ACL_Counter_By_ACE()
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)
    enum
    {
        MAX_ACE  = 5,
    };

    struct
    {
        RULE_TYPE_AclType_T acl_type;
        char acl_name[30];
        UI32_T acl_index;
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } acls[] =
    {
        {
            RULE_TYPE_MAC_ACL,
            {0},
            0,
            1,
            RULE_TYPE_INBOUND,
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            {0},
            0,
            2,
            RULE_TYPE_INBOUND,
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            3,
            RULE_TYPE_INBOUND,
        },
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {
            RULE_TYPE_MAC_ACL,
            {0},
            0,
            1,
            RULE_TYPE_OUTBOUND,
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            {0},
            0,
            2,
            RULE_TYPE_OUTBOUND,
        },
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            3,
            RULE_TYPE_OUTBOUND,
        },
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,
            RULE_TYPE_INBOUND,
        },
#endif // SYS_CPNT_QOS_V2_ALL_PORTS

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        {
            RULE_TYPE_IPV6_EXT_ACL,
            {0},
            0,
            SYS_ADPT_CPU_1_IF_INDEX_NUMBER,
            RULE_TYPE_OUTBOUND,
        }
#endif // SYS_CPNT_QOS_V2_CPU_PORT
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T ai;

    for (ai = 0; ai < _countof(acls); ++ ai)
    {
        snprintf(acls[ai].acl_name, sizeof(acls[ai].acl_name), "%lu", ai);
        acls[ai].acl_name[sizeof(acls[ai].acl_name)-1] = '\0';

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acls[ai].acl_name, acls[ai].acl_type,
                                                 MAX_ACE, RULE_CTRL_UT_COMPRESS_ENABLE,
                                                 &acls[ai].acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        result = RULE_OM_RebuildAggregateAcl(acls[ai].acl_index);
        assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

        {
            UI32_T ifindex = acls[ai].ifindex;

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            interface.direction = acls[ai].direction;

            {
                BOOL_T ingress_flag = RULE_TYPE_DIRECTION_TO_IS_INGRESS(interface.direction);

                UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_ENABLE;

                RULE_TYPE_AclType_T port_acl_type;

                result = RULE_OM_LocalAclTypeToPortAclType(acls[ai].acl_type, &port_acl_type);
                assert(RULE_TYPE_OK == result);

                result = RULE_MGR_BindPort2Acl(ifindex, acls[ai].acl_index, port_acl_type, ingress_flag, time_range_name, counter_enable);
                assert(RULE_TYPE_OK == result);

                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    {
        UI32_T filter_ai;

        for (filter_ai = 0; filter_ai < _countof(acls); ++ filter_ai)
        {
            UI32_T filter_ifindex = acls[filter_ai].ifindex;
            RULE_TYPE_InOutDirection_T filter_direction = acls[filter_ai].direction;

            RULE_TYPE_AclType_T filter_ace_type;
            UI32_T filter_ace_index;
            RULE_TYPE_Ace_Entry_T filter_ace_entry;

            filter_ace_index = 0;
            while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(filter_ifindex, filter_direction, acls[filter_ai].acl_index,
                                                                    &filter_ace_type, &filter_ace_index, &filter_ace_entry))
            {

                for (ai = 0; ai < _countof(acls); ++ ai)
                {
                    UI32_T ifindex = acls[ai].ifindex;
                    RULE_TYPE_InOutDirection_T direction = acls[ai].direction;

                    CLI_ACL_UT_Proc_Set_ACL_Counter(ifindex, acls[ai].acl_name, acls[ai].acl_type, direction);

                    {
                        RULE_TYPE_AclType_T ace_type;
                        UI32_T ace_index;
                        RULE_TYPE_Ace_Entry_T ace_entry;

                        ace_index = 0;
                        while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, direction, acls[ai].acl_index,
                                                                                &ace_type, &ace_index, &ace_entry))
                        {
                            assert(ace_entry.counter.total_packets != 0);
                        }
                    }
                }

                {
                    RULE_TYPE_INTERFACE_INFO_T filter_interface;

                    filter_interface.type = RULE_TYPE_INTERFACE_UNSPEC;
                    filter_interface.direction = RULE_TYPE_UNSPEC_DIRECTION;
                    result = RULE_MGR_ClearAclCounter(acls[filter_ai].acl_index, filter_ace_index, &filter_interface);
                    assert(RULE_TYPE_OK == result);
                }

                for (ai = 0; ai < _countof(acls); ++ ai)
                {
                    UI32_T ifindex = acls[ai].ifindex;
                    RULE_TYPE_InOutDirection_T direction = acls[ai].direction;

                    RULE_TYPE_AclType_T ace_type;
                    UI32_T ace_index;
                    RULE_TYPE_Ace_Entry_T ace_entry;

                    ace_index = 0;
                    while (RULE_TYPE_OK == RULE_MGR_GetNextHardwareAceByAcl(ifindex, direction, acls[ai].acl_index,
                                                                            &ace_type, &ace_index, &ace_entry))
                    {
                        if (acls[filter_ai].acl_index == acls[ai].acl_index &&
                            filter_ace_index == ace_index)
                        {
                            assert(ace_entry.counter.total_packets == 0);
                        }
                        else
                        {
                            assert(ace_entry.counter.total_packets != 0);
                        }
                    }
                }

            }

        }
    }

#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

int
RULE_MGR_UT_Defrag_Rule_Failure()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    enum
    {
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
        UI32_T ace_count;
        UI32_T bind_to_ifindex;
    } acls[] =
    {
        {
            RULE_TYPE_IP_EXT_ACL,
            SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR,
            1
        },
        {
            RULE_TYPE_IP_STD_ACL,
            SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR,
            2
        },
        {
            RULE_TYPE_IP_EXT_ACL,
            SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR,
            3
        },
    };

    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i;

    {
        UI32_T acl_index;

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace("m1",
                                                 RULE_TYPE_MAC_ACL,
                                                 1,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &acl_index);
        {
            BOOL_T ingress_flag = TRUE;

            UI32_T ifindex = 5;
            UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = TRUE;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace("ip1",
                                                     RULE_TYPE_IP_STD_ACL,
                                                     2,
                                                     RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                     &acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
            result = RULE_OM_RebuildAggregateAcl(acl_index);
            assert(RULE_TYPE_OK == result);
#endif

            result = RULE_MGR_BindPort2Acl(ifindex, acl_index, RULE_TYPE_IP_ACL, ingress_flag, time_range_name, counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        {
            BOOL_T ingress_flag = TRUE;

            UI32_T ifindex = 6;
            UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = TRUE;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace("ip2",
                                                     RULE_TYPE_IP_STD_ACL,
                                                     2,
                                                     RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                     &acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
            result = RULE_OM_RebuildAggregateAcl(acl_index);
            assert(RULE_TYPE_OK == result);
#endif

            result = RULE_MGR_BindPort2Acl(ifindex, acl_index, RULE_TYPE_IP_ACL, ingress_flag, time_range_name, counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        {
            BOOL_T ingress_flag = TRUE;

            UI32_T ifindex = 7;
            UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = TRUE;

            RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace("ip3",
                                                     RULE_TYPE_IP_STD_ACL,
                                                     1,
                                                     RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                     &acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
            result = RULE_OM_RebuildAggregateAcl(acl_index);
            assert(RULE_TYPE_OK == result);
#endif

            result = RULE_MGR_BindPort2Acl(ifindex, acl_index, RULE_TYPE_IP_ACL, ingress_flag, time_range_name, counter_enable);
            assert(RULE_TYPE_OK == result);
        }

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace("ipv6-std",
                                                 RULE_TYPE_IPV6_STD_ACL,
                                                 1,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &acl_index);
        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace("ipv6-ext",
                                                 RULE_TYPE_IPV6_EXT_ACL,
                                                 1,
                                                 RULE_CTRL_UT_COMPRESS_DEFAULT,
                                                 &acl_index);
    }

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[30];
        UI32_T acl_index;

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name,
                                                 acls[i].acl_type,
                                                 acls[i].ace_count,
                                                 RULE_CTRL_UT_COMPRESS_DISABLE,
                                                 &acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        result = RULE_OM_RebuildAggregateAcl(acl_index);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_Acl_T *acl_entry_p;
            RULE_TYPE_Acl_T *agg_acl_entry_p;

            acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_index);
            assert(NULL != acl_entry_p);

            agg_acl_entry_p = RULE_OM_LocalGetAclByIndex(acl_entry_p->aggregate_acl_index);
            assert(NULL != agg_acl_entry_p);
            assert(agg_acl_entry_p->ace_count == acls[i].ace_count);
        }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

        {
            UI32_T ifindex = acls[i].bind_to_ifindex;

            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
            interface.direction = RULE_TYPE_INBOUND;

            {
                BOOL_T ingress_flag = RULE_TYPE_DIRECTION_TO_IS_INGRESS(interface.direction);

                UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T counter_enable = FALSE;

                RULE_TYPE_AclType_T port_acl_type;

                result = RULE_OM_LocalAclTypeToPortAclType(acls[i].acl_type, &port_acl_type);
                assert(RULE_TYPE_OK == result);

                //
                // This function will trigger rule defrag. But all slice can not
                // satify the new requirement. So the action will fail.
                //
                result = RULE_MGR_BindPort2Acl(ifindex, acl_index, port_acl_type, ingress_flag, time_range_name, counter_enable);
                assert(RULE_TYPE_OK != result);

                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }
#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

#pragma mark Basic Policy-map test items
int
RULE_MGR_UT_Apply_Policy_With_Match_Any_Class_Map_With_MF()
{
    RULE_TYPE_RETURN_TYPE_T result;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        /* FIXME: c1 action not allow on egress
         */
        //{SYS_ADPT_CPU_1_IF_INDEX_NUMBER,        RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

        //
        // rule ctrl had not support 'all-ports' interface yet
        //
//#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
//        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER, RULE_TYPE_INBOUND},
//#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    UI32_T pi;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 2000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 3000);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_REMARK_DSCP_TRANSMIT, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    RULE_MGR_UT_Proc_Create_Class_Map("c2", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c2", RULE_TYPE_MF_DSCP, 42);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c2", RULE_TYPE_MF_DSCP, 43);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c2", RULE_TYPE_MF_DSCP, 44);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c2", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c2",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */


    RULE_MGR_UT_Proc_Create_Class_Map("c3", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c3", RULE_TYPE_MF_DSCP, 10);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c3", RULE_TYPE_MF_DSCP, 20);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c3", RULE_TYPE_MF_DSCP, 30);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c3", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c3",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    //RULE_OM_SetDebugFlag(7);
    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }

    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }

    return 0;
}

int
RULE_MGR_UT_Apply_Policy_With_Match_Any_Class_Map_With_ACL()
{
    enum
    {
        MAX_ACE_IN_ACL = 5,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        /* FIXME: c1 action not allow on egress
         */
        //{SYS_ADPT_CPU_1_IF_INDEX_NUMBER,        RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

        //
        // rule ctrl had not support 'all-ports' interface yet
        //
//#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
//        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER, RULE_TYPE_INBOUND},
//#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    UI32_T pi;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c1", "a1", RULE_TYPE_MAC_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c1", "a2", RULE_TYPE_MAC_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c1", "a3", RULE_TYPE_MAC_ACL, MAX_ACE_IN_ACL);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_REMARK_DSCP_TRANSMIT, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */



    RULE_MGR_UT_Proc_Create_Class_Map("c2", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c2", "a4", RULE_TYPE_IP_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c2", "a5", RULE_TYPE_IP_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c2", "a6", RULE_TYPE_IP_EXT_ACL, MAX_ACE_IN_ACL);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c2", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c2",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */


    RULE_MGR_UT_Proc_Create_Class_Map("c3", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c3", "a7", RULE_TYPE_IPV6_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c3", "a8", RULE_TYPE_IPV6_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c3", "a9", RULE_TYPE_IPV6_EXT_ACL, MAX_ACE_IN_ACL);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c3", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c3",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);

#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }

    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }

    return 0;
}

int
RULE_MGR_UT_Apply_Policy_With_Match_Any_Class_Map_With_MF_ACL_Mix()
{
    enum
    {
        MAX_ACE_IN_ACL = 5,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        /* FIXME: c1 action not allow on egress
         */
        //{SYS_ADPT_CPU_1_IF_INDEX_NUMBER,        RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

        //
        // rule ctrl had not support 'all-ports' interface yet
        //
//#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
//        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER, RULE_TYPE_INBOUND},
//#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    UI32_T pi;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c1", "a1", RULE_TYPE_MAC_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c1", "a2", RULE_TYPE_MAC_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c1", "a3", RULE_TYPE_MAC_ACL, MAX_ACE_IN_ACL);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 2000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 3000);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_REMARK_DSCP_TRANSMIT, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */


    RULE_MGR_UT_Proc_Create_Class_Map("c2", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c2", "a4", RULE_TYPE_IP_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c2", "a5", RULE_TYPE_IP_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c2", "a6", RULE_TYPE_IP_EXT_ACL, MAX_ACE_IN_ACL);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c2", RULE_TYPE_MF_DSCP, 42);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c2", RULE_TYPE_MF_DSCP, 43);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c2", RULE_TYPE_MF_DSCP, 44);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c2", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c2",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */


    RULE_MGR_UT_Proc_Create_Class_Map("c3", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c3", "a7", RULE_TYPE_IPV6_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c3", "a8", RULE_TYPE_IPV6_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c3", "a9", RULE_TYPE_IPV6_EXT_ACL, MAX_ACE_IN_ACL);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c3", RULE_TYPE_MF_DSCP_IPV6, 10);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c3", RULE_TYPE_MF_DSCP_IPV6, 20);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c3", RULE_TYPE_MF_DSCP_IPV6, 30);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c3", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c3",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */


    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }

    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }

    return 0;
}

int
RULE_MGR_UT_Apply_Policy_With_Match_All_Class_Map_With_MF()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        /* FIXME: c1 action not allow on egress
         */
        //{SYS_ADPT_CPU_1_IF_INDEX_NUMBER,        RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

        //
        // rule ctrl had not support 'all-ports' interface yet
        //
//#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
//        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER, RULE_TYPE_INBOUND},
//#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    UI32_T pi;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ALL);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_REMARK_DSCP_TRANSMIT, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    RULE_MGR_UT_Proc_Create_Class_Map("c2", RULE_TYPE_CLASS_MAP_MATCH_ALL);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c2", RULE_TYPE_MF_DSCP, 42);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c2", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c2",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */


    RULE_MGR_UT_Proc_Create_Class_Map("c3", RULE_TYPE_CLASS_MAP_MATCH_ALL);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c3", RULE_TYPE_MF_DSCP, 10);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c3", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c3",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }

    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }
#endif // SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL

    return 0;
}

int
RULE_MGR_UT_Apply_Policy_With_Match_All_Class_Map_With_ACL()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    enum
    {
        MAX_ACE_IN_ACL = 5,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        /* FIXME: c1 action not allow on egress
         */
        //{SYS_ADPT_CPU_1_IF_INDEX_NUMBER,        RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

        //
        // rule ctrl had not support 'all-ports' interface yet
        //
//#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
//        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER, RULE_TYPE_INBOUND},
//#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    UI32_T pi;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ALL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c1", "a1", RULE_TYPE_MAC_ACL, MAX_ACE_IN_ACL);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_REMARK_DSCP_TRANSMIT, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */


    RULE_MGR_UT_Proc_Create_Class_Map("c2", RULE_TYPE_CLASS_MAP_MATCH_ALL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c2", "a2", RULE_TYPE_IP_EXT_ACL, MAX_ACE_IN_ACL);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c2", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c2",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    RULE_MGR_UT_Proc_Create_Class_Map("c3", RULE_TYPE_CLASS_MAP_MATCH_ALL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c3", "a3", RULE_TYPE_IPV6_EXT_ACL, MAX_ACE_IN_ACL);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c3", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c3",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }

    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }
#endif // SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL

    return 0;
}

int
RULE_MGR_UT_Apply_Policy_With_Match_All_Class_Map_With_MF_ACL_Mix()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    enum
    {
        MAX_ACE_IN_ACL = 5,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        /* FIXME: c1 action not allow on egress
         */
        //{SYS_ADPT_CPU_1_IF_INDEX_NUMBER,        RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

        //
        // rule ctrl had not support 'all-ports' interface yet
        //
//#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
//        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER, RULE_TYPE_INBOUND},
//#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    UI32_T pi;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ALL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c1", "a1", RULE_TYPE_MAC_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_REMARK_DSCP_TRANSMIT, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */


    RULE_MGR_UT_Proc_Create_Class_Map("c2", RULE_TYPE_CLASS_MAP_MATCH_ALL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c2", "a2", RULE_TYPE_IP_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c2", RULE_TYPE_MF_DSCP, 42);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c2", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c2",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    RULE_MGR_UT_Proc_Create_Class_Map("c3", RULE_TYPE_CLASS_MAP_MATCH_ALL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c3", "a3", RULE_TYPE_IPV6_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c3", RULE_TYPE_MF_DSCP_IPV6, 10);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c3", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c3",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }

    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }
#endif // SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL

    return 0;
}

int
RULE_MGR_UT_Apply_Policy_With_Match_Any_All_Class_Map_Mix()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    enum
    {
        MAX_ACE_IN_ACL = 5,
    };

    RULE_TYPE_RETURN_TYPE_T result;

    struct port_t
    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;
    } port_list[] =
    {
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                     RULE_TYPE_INBOUND},
        {2,                                     RULE_TYPE_INBOUND},
        {3,                                     RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        /* FIXME: c1 action not allow on egress
         */
        //{SYS_ADPT_CPU_1_IF_INDEX_NUMBER,        RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

        //
        // rule ctrl had not support 'all-ports' interface yet
        //
//#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
//        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER, RULE_TYPE_INBOUND},
//#endif // SYS_CPNT_QOS_V2_ALL_PORTS
    };

    UI32_T pi;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c1", "a1", RULE_TYPE_MAC_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c1", "a2", RULE_TYPE_MAC_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c1", "a3", RULE_TYPE_MAC_ACL, MAX_ACE_IN_ACL);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 2000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 3000);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_REMARK_DSCP_TRANSMIT, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */


    RULE_MGR_UT_Proc_Create_Class_Map("c2", RULE_TYPE_CLASS_MAP_MATCH_ALL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c2", "a4", RULE_TYPE_IP_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c2", RULE_TYPE_MF_DSCP, 42);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c2", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c2",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    RULE_MGR_UT_Proc_Create_Class_Map("c3", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c3", "a5", RULE_TYPE_IPV6_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c3", "a6", RULE_TYPE_IPV6_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c3", "a7", RULE_TYPE_IPV6_EXT_ACL, MAX_ACE_IN_ACL);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c3", RULE_TYPE_MF_DSCP_IPV6, 10);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c3", RULE_TYPE_MF_DSCP_IPV6, 20);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c3", RULE_TYPE_MF_DSCP_IPV6, 30);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c3", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c3",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    RULE_MGR_UT_Proc_Create_Class_Map("c4", RULE_TYPE_CLASS_MAP_MATCH_ALL);
    RULE_MGR_UT_Proc_Add_ACL_To_Class_Map("c4", "a8", RULE_TYPE_IPV6_EXT_ACL, MAX_ACE_IN_ACL);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c4", RULE_TYPE_MF_DSCP_IPV6, 10);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c4", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c4",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 0);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }

    for (pi = 0; pi < _countof(port_list); ++ pi)
    {
        UI32_T ifindex = port_list[pi].ifindex;
        RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

        result = RULE_MGR_UT_Proc_Bind_Policy_Map(ifindex, direction, "p1");
        assert(RULE_TYPE_OK == result);
    }
#endif // SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL

    return 0;
}

#pragma mark MIB

int
RULE_MGR_UT_Set_Class_Map_Attach_Ctrl()
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T i;

    struct {
        RULE_TYPE_AclType_T ace_type;
        UI32_T ace_status;
        UI32_T element_type;
    } class_map_element_ar[] = {
        {RULE_TYPE_IPV6_ACL, VAL_diffServIpv6AceStatus_createAndGo, VAL_diffServClassMapAttachCtlElementIndexType_ipv6Ace},
        {RULE_TYPE_MAC_ACL, VAL_diffServMacAceStatus_createAndGo, VAL_diffServClassMapAttachCtlElementIndexType_macAce},
        {RULE_TYPE_IP_ACL, VAL_diffServIpAceStatus_createAndGo, VAL_diffServClassMapAttachCtlElementIndexType_ipAce}
    };

    for (i = 0; i < _countof(class_map_element_ar); ++i)
    {
        UI32_T index = i+1;

        result = RULE_MGR_SetAceRowStatus(index, class_map_element_ar[i].ace_type, class_map_element_ar[i].ace_status);
        assert(result == RULE_TYPE_OK);

        result = RULE_MGR_SetClassMapRowStatus(index, VAL_diffServClassMapStatus_createAndGo);
        assert(result == RULE_TYPE_OK);

        result = RULE_MGR_SetClassMapAttachCtrl(index,
                                                class_map_element_ar[i].element_type,
                                                index,
                                                VAL_diffServClassMapAttachCtlAction_attaching);
        assert(result == RULE_TYPE_OK);
    }

    {
        UI32_T index = i+1;
        UI32_T acl_index;

        RULE_MGR_UT_Proc_Create_ACL("m1", RULE_TYPE_MAC_ACL, 2, RULE_CTRL_UT_COMPRESS_DEFAULT);

        result = RULE_MGR_GetAclIdByName("m1", &acl_index);
        assert(result == RULE_TYPE_OK);

        result = RULE_MGR_SetClassMapRowStatus(index, VAL_diffServClassMapStatus_createAndGo);
        assert(result == RULE_TYPE_OK);

        result = RULE_MGR_SetClassMapAttachCtrl(index,
                                                VAL_diffServClassMapAttachCtlElementIndexType_acl,
                                                acl_index,
                                                VAL_diffServClassMapAttachCtlAction_attaching);
        assert(result == RULE_TYPE_OK);
    }

    return 0;
}

int
RULE_MGR_UT_Set_Policy_Map_Attach_Ctrl()
{
    const char *policy_map_name = "p1";
    UI32_T policy_map_index;

    struct class_map_s
    {
        const char *class_map_name;
        UI32_T policy_element_index;
        UI32_T class_map_index;
        UI32_T meter_index;
        UI32_T action_index;
    } policy_element_class[] =
    {
        {"c1", 0, 0, 0, 0},
        {"c2", 0, 0, 0, 0},
        {"c3", 0, 0, 0, 0},
        {"c4", 0, 0, 0, 0}
    };

    UI32_T i;
    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(policy_element_class); ++i)
    {
        UI32_T                          class_map_index;
        UI32_T                          class_index;

        RULE_TYPE_TBParamEntry_T        meter;
        RULE_TYPE_Action_T              action;

        RULE_TYPE_UI_ClassMapElement_T  class_element;

        //
        // class-map
        //
        result = RULE_OM_Create_ClassMap(policy_element_class[i].class_map_name,
                                         RULE_TYPE_CLASS_MAP_MATCH_ANY,
                                         &policy_element_class[i].class_map_index);
        assert(RULE_TYPE_OK == result);

        class_map_index = policy_element_class[i].class_map_index;

        //
        // Match MF
        //

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;
        class_element.element.mf_entry.mf_value = 1001;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // Match MF
        //

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;
        class_element.element.mf_entry.mf_value = 2001;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // meter
        //
        policy_element_class[i].meter_index = i + 1;

        result = RULE_MGR_SetMeterRowStatus(policy_element_class[i].meter_index, VAL_diffServPolicyMapStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        //
        // FIXME: The policy-map should not bind to port
        //
        memset(&meter, 0, sizeof(meter));


#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
        meter.meter_model   = RULE_TYPE_METER_MODE_RATE;
        meter.rate          = 5000;
#else
        meter.meter_model   = RULE_TYPE_METER_MODE_FLOW;
        meter.rate          = 5000;
        meter.burst_size    = 4000;
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

        result = RULE_MGR_SetMeterEntry(policy_element_class[i].meter_index, &meter);
        assert(RULE_TYPE_OK == result);

        //
        // action
        //
        policy_element_class[i].action_index = i + 1;
        result = RULE_MGR_SetActionRowStatus(policy_element_class[i].action_index, VAL_diffServPolicyMapStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        memset(&action, 0, sizeof(action));
        action.action_bitmap |= (RULE_TYPE_ACTION_YELLOW_DROP | RULE_TYPE_ACTION_RED_DROP);

        result = RULE_OM_SetActionEntry(policy_element_class[i].action_index, &action);
        assert(RULE_TYPE_OK == result);

        //
        // policy map element
        //
        policy_element_class[i].policy_element_index = i + 1;

        result = RULE_OM_CreatePolicyMapElement(policy_element_class[i].policy_element_index, TRUE);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_PolicyElement_T *policy_element_p;

            policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_element_class[i].policy_element_index);
            assert(NULL != policy_element_p);

            policy_element_p->class_map_index = policy_element_class[i].class_map_index;
            policy_element_p->meter_index = policy_element_class[i].meter_index;
            policy_element_p->action_index = policy_element_class[i].action_index;
            policy_element_p->row_status = VAL_diffServPolicyMapStatus_active;
        }
    }

    //
    // Bind to port
    //
    {
        enum
        {
            MIN_IFINDEX = 1,
            MAX_IFINDEX = 3,
            MIN_DIRECTION = RULE_TYPE_INBOUND,
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
            MAX_DIRECTION = RULE_TYPE_OUTBOUND
#else
            MAX_DIRECTION = RULE_TYPE_INBOUND
#endif
        };

        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;

        for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
        {
            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
            {
                result = RULE_CTRL_SetPolicyMap(ifindex, direction, policy_map_index, TRUE);
                assert(RULE_TYPE_OK == result);

                result = RULE_OM_BindPort2PolicyMap(ifindex, direction, policy_map_index);
                assert(RULE_TYPE_OK == result);
            }
        }
    }

    for (i = 0; i < _countof(policy_element_class); ++i)
    {
        result = RULE_MGR_SetPolicyMapAttachCtrl(policy_map_index, policy_element_class[i].policy_element_index, VAL_diffServPolicyMapAttachCtlAction_attaching);

        RULE_CTRL_UT_Proc_Validate_Rule();
        assert(RULE_TYPE_OK == result);
    }

    for (i = 0; i < _countof(policy_element_class); ++i)
    {
        result = RULE_MGR_SetPolicyMapAttachCtrl(policy_map_index, policy_element_class[i].policy_element_index, VAL_diffServPolicyMapAttachCtlAction_notAttaching);

        RULE_CTRL_UT_Proc_Validate_Rule();
        assert(RULE_TYPE_OK == result);
    }

    return 0;
}

int
RULE_MGR_UT_MIB_SetArpAce()
{
    UI32_T ace_index = 1;
    RULE_TYPE_AclType_T ace_type = RULE_TYPE_ARP_ACL;
    RULE_TYPE_UI_Ace_Entry_T ui_ace;
    UI8_T ip[] = {192,168,1,1};
    UI8_T ip_mask[] = {255,254,211,1};

    RULE_TYPE_RETURN_TYPE_T ret;

    ret = RULE_MGR_SetAceRowStatus(ace_index, ace_type, VAL_diffServArpAceStatus_createAndGo);
    assert(ret == RULE_TYPE_OK);

    ret = RULE_MGR_GetUIAceByIndex(ace_index, &ui_ace);
    assert(ret == RULE_TYPE_OK);

    SET_IPV4(ui_ace.arp.sender.ip.mask, ip_mask[0], ip_mask[1], ip_mask[2], ip_mask[3]);
    ret = RULE_MGR_SetUIAceFieldByIndex(ace_index, &ui_ace, LEAF_diffServArpAceSourceIpAddrBitmask);
    assert(ret == RULE_TYPE_OK);

    ret = RULE_MGR_GetUIAceByIndex(ace_index, &ui_ace);
    assert(ret == RULE_TYPE_OK);

    SET_IPV4(ui_ace.arp.sender.ip.addr, ip[0], ip[1], ip[2], ip[3]);
    ret = RULE_MGR_SetUIAceFieldByIndex(ace_index, &ui_ace, LEAF_diffServArpAceSourceIpAddr);
    assert(ret == RULE_TYPE_OK);

    ret = RULE_MGR_GetUIAceByIndex(ace_index, &ui_ace);
    assert(ret == RULE_TYPE_OK);

    SET_IPV4(ui_ace.arp.target.ip.mask, ip_mask[0], ip_mask[1], ip_mask[2], (ip_mask[3]+1));
    ret = RULE_MGR_SetUIAceFieldByIndex(ace_index, &ui_ace, LEAF_diffServArpAceDestIpAddrBitmask);
    assert(ret == RULE_TYPE_OK);

    ret = RULE_MGR_GetUIAceByIndex(ace_index, &ui_ace);
    assert(ret == RULE_TYPE_OK);

    SET_IPV4(ui_ace.arp.target.ip.addr, ip[0], ip[1], ip[2], (ip[3]+1));
    ret = RULE_MGR_SetUIAceFieldByIndex(ace_index, &ui_ace, LEAF_diffServArpAceDestIpAddr);
    assert(ret == RULE_TYPE_OK);

    ret = RULE_MGR_GetUIAceByIndex(ace_index, &ui_ace);
    assert(ret == RULE_TYPE_OK);

    assert(0 == memcmp(ui_ace.arp.sender.ip.mask, ip_mask, 4));
    assert(0 == memcmp(ui_ace.arp.sender.ip.addr, ip, 4));
    ip_mask[3] += 1;
    ip[3] += 1;
    assert(0 == memcmp(ui_ace.arp.target.ip.mask, ip_mask, 4));
    assert(0 == memcmp(ui_ace.arp.target.ip.addr, ip, 4));

    ret = RULE_MGR_SetAceRowStatus(ace_index, ace_type, VAL_diffServArpAceStatus_destroy);
    assert(ret == RULE_TYPE_OK);

    ret = RULE_MGR_GetUIAceByIndex(ace_index, &ui_ace);
    assert(ret != RULE_TYPE_OK);

    return 0;
}

//
// Assumption: The device have less resource, e.g., meter or qos remark entry,
//             then upper layer.
//
// This case was designed for Marvell platform that have inconsistent resource
// between device and upper layer.
//
// Test when failed to bind policy element to policy map, the undo will work well.
//
int
RULE_MGR_UT_Consistency_Check_For_Failed_To_Set_Policy_Map_Attach_Ctrl()
{
    const char *policy_map_name = "p1";
    UI32_T policy_map_index;

    struct class_map_s
    {
        UI32_T policy_element_index;
        UI32_T class_map_index;
        UI32_T meter_index;
        UI32_T action_index;
    } policy_element_class[ SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_NBR_OF_POLICY_MAP ];

    UI32_T i;
    RULE_TYPE_RETURN_TYPE_T result;

    memset(policy_element_class, 0, sizeof(policy_element_class));

    result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(policy_element_class); ++i)
    {
        UI32_T                          class_map_index;
        UI32_T                          class_index;

        RULE_TYPE_TBParamEntry_T        meter;
        RULE_TYPE_Action_T              action;

        RULE_TYPE_UI_ClassMapElement_T  class_element;

        char class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH];

        memset(class_map_name, 0, sizeof(class_map_name));

        sprintf(class_map_name, "c%lu", i);

        //
        // class-map
        //
        result = RULE_OM_Create_ClassMap(class_map_name, //policy_element_class[i].class_map_name,
                                         RULE_TYPE_CLASS_MAP_MATCH_ANY,
                                         &policy_element_class[i].class_map_index);
        assert(RULE_TYPE_OK == result);

        class_map_index = policy_element_class[i].class_map_index;

        //
        // Match MF
        //

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;
        class_element.element.mf_entry.mf_value = 1001;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // Match MF
        //

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;
        class_element.element.mf_entry.mf_value = 2001;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // meter
        //
        policy_element_class[i].meter_index = i + 1;

        result = RULE_MGR_SetMeterRowStatus(policy_element_class[i].meter_index,
                                            VAL_diffServPolicyMapStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        //
        // FIXME: The policy-map should not bind to port
        //
        memset(&meter, 0, sizeof(meter));

        meter.meter_model       = RULE_TYPE_METER_MODE_TRTCM_COLOR_BLIND;
        meter.rate              = 5000;
        meter.burst_size        = 4000;
        meter.peak_rate         = 10000;
        meter.peak_burst_size   = 20000;

        result = RULE_MGR_SetMeterEntry(policy_element_class[i].meter_index, &meter);
        assert(RULE_TYPE_OK == result);

        //
        // action
        //
        policy_element_class[i].action_index = i + 1;
        result = RULE_MGR_SetActionRowStatus(policy_element_class[i].action_index,
                                             VAL_diffServPolicyMapStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        memset(&action, 0, sizeof(action));
        action.action_bitmap |= (RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP | RULE_TYPE_ACTION_RED_PKT_NEW_DSCP);
        action.yellow_pkt_new_dscp = 42;
        action.red_pkt_new_dscp = 1;

        result = RULE_OM_SetActionEntry(policy_element_class[i].action_index, &action);
        assert(RULE_TYPE_OK == result);

        //
        // policy map element
        //
        policy_element_class[i].policy_element_index = i + 1;

        result = RULE_OM_CreatePolicyMapElement(policy_element_class[i].policy_element_index, TRUE);
        assert(RULE_TYPE_OK == result);

        {
            RULE_TYPE_PolicyElement_T *policy_element_p;

            policy_element_p = RULE_OM_LocalGetPolicyMapElementByIndex(policy_element_class[i].policy_element_index);
            assert(NULL != policy_element_p);

            policy_element_p->class_map_index = policy_element_class[i].class_map_index;
            policy_element_p->meter_index = policy_element_class[i].meter_index;
            policy_element_p->action_index = policy_element_class[i].action_index;
            policy_element_p->row_status = VAL_diffServPolicyMapStatus_active;
        }
    }

    //
    // Bind to port
    //
    {
        enum
        {
            MIN_IFINDEX = 1,
            MAX_IFINDEX = 5,
            MIN_DIRECTION = RULE_TYPE_INBOUND,
            MAX_DIRECTION = RULE_TYPE_INBOUND
        };

        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;

        for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
        {
            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
            {
                result = RULE_CTRL_SetPolicyMap(ifindex, direction, policy_map_index, TRUE);
                assert(RULE_TYPE_OK == result);

                result = RULE_OM_BindPort2PolicyMap(ifindex, direction, policy_map_index);
                assert(RULE_TYPE_OK == result);

                RULE_CTRL_UT_Proc_Validate_Rule();
            }
        }
    }

    for (i = 0; i < _countof(policy_element_class); ++ i)
    {
        result = RULE_MGR_SetPolicyMapAttachCtrl(policy_map_index,
                                                 policy_element_class[i].policy_element_index,
                                                 VAL_diffServPolicyMapAttachCtlAction_attaching);

        RULE_CTRL_UT_Proc_Validate_Rule();

        if (RULE_TYPE_OK != result)
        {
            //
            // Unbind all
            //
            UI32_T j;

            for (j = 0; j < i; ++ j)
            {
                result = RULE_MGR_SetPolicyMapAttachCtrl(policy_map_index,
                                                         policy_element_class[j].policy_element_index,
                                                         VAL_diffServPolicyMapAttachCtlAction_notAttaching);

                RULE_CTRL_UT_Proc_Validate_Rule();

                if (RULE_TYPE_OK != result)
                {
                    break;
                }
            }

            break;
        }
    }

    return 0;
}

int
RULE_MGR_UT_SetAccessGroupStatus_On_All_Port()
{
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_InOutDirection_T direction;
    RULE_TYPE_MIB_AccessGroupEntry_T entry;
    UI32_T acl_index;
    UI32_T ifindex;
    UI32_T mib_direction = VAL_diffServAccessGroupDirection_ingress;
    UI32_T mib_acl_type = VAL_diffServAccessGroupType_mac;
    UI32_T mib_counter_status;

// FIXEME: time range
#if 0 && (SYS_CPNT_TIME_BASED_ACL == TRUE)
    UI16_T t_index;
    char *time_range_name = "daily-8-to-12";

    RULE_MGR_UT_Proc_Create_Periodic_Time_Range(time_range_name, 8, 0, 12, 0);

    result = RULE_MGR_GetTimeRangeIdByName(time_range_name, &t_index);
    assert(result == RULE_TYPE_OK);
#endif /*(SYS_CPNT_TIME_BASED_ACL == TRUE) */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    mib_counter_status = VAL_diffServAccessGroupCounterStatus_enabled;
#else
    mib_counter_status = VAL_diffServAccessGroupCounterStatus_disabled;
#endif /* #if (SYS_CPNT_ACL_COUNTER == TRUE) */

    RULE_MGR_UT_Proc_Create_ACL("m1", RULE_TYPE_MAC_ACL, 2, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("m1", &acl_index);
    assert(result == RULE_TYPE_OK);

    ifindex = 0;
    while (TRUE == RULE_OM_LocalGetNextInterface(&ifindex, &direction))
    {
        if (TRUE != RULE_TYPE_IS_UPORT(ifindex))
        {
            /* FIXME: broadcom only support binding acl on uport
             */
            continue;
        }

        mib_direction = (direction == RULE_TYPE_INBOUND)
                        ? VAL_diffServAccessGroupDirection_ingress : VAL_diffServAccessGroupDirection_egress;

        entry.ifindex = ifindex;
        entry.direction = mib_direction;
        entry.type = mib_acl_type;

        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndGo);
        assert(RULE_TYPE_OK != result);

        /* destroy -> notReady
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
        assert(RULE_TYPE_OK == result);

// FIXME: time range
#if 0 && (SYS_CPNT_TIME_BASED_ACL == TRUE)
        result = RULE_MGR_SetAccessGroupTimeRangeName(ifindex, mib_direction, mib_acl_type, time_range_name);
        assert(RULE_TYPE_OK == result);
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
        result = RULE_MGR_SetAccessGroupCounterStatus(ifindex, mib_direction, mib_acl_type, mib_counter_status);
        assert(RULE_TYPE_OK == result);
#endif /* (SYS_CPNT_ACL_COUNTER == TRUE) */

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index = acl_index);
#if 0 //FIXME: time range
        assert(0 == strncmp(entry.time_range_name, time_range_name, strlen(time_range_name)));
#endif
        assert(entry.counter_status == mib_counter_status);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);

        /* notReady -> active
         */
        result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, 0);
        assert(RULE_TYPE_OK != result);

        result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_active);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index = acl_index);
#if 0 //FIXME: time range
        assert(0 == strncmp(entry.time_range_name, time_range_name, strlen(time_range_name)));
#endif /* 0 */
        assert(entry.counter_status == mib_counter_status);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_active);

        result = RULE_MGR_SetAccessGroupCounterStatus(ifindex, mib_direction,
                                                      mib_acl_type, VAL_diffServAccessGroupCounterStatus_disabled);
        assert(RULE_TYPE_OK != result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index = acl_index);
#if 0 //FIXME: time range
        assert(0 == strncmp(entry.time_range_name, time_range_name, strlen(time_range_name)));
#endif /* 0 */
        assert(entry.counter_status == mib_counter_status);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_active);

        /* active -> notInservice
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_notInService);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index = acl_index);
#if 0 //FIXME: time range
        assert(0 == strncmp(entry.time_range_name, time_range_name, strlen(time_range_name)));
#endif /* 0 */
        assert(entry.counter_status == mib_counter_status);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);

#if (SYS_CPNT_ACL_COUNTER == TRUE)
        mib_counter_status = VAL_diffServAccessGroupCounterStatus_enabled;
        result = RULE_MGR_SetAccessGroupCounterStatus(ifindex, mib_direction, mib_acl_type, mib_counter_status);
        assert(RULE_TYPE_OK == result);
#endif /* SYS_CPNT_ACL_COUNTER */

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index = acl_index);
#if 0 //FIXME: time range
        assert(0 == strncmp(entry.time_range_name, time_range_name, strlen(time_range_name)));
#endif /* 0 */
        assert(entry.counter_status == mib_counter_status);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);

        /* notReady -> active
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_active);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index = acl_index);
#if 0 //FIXME: time range
        assert(0 == strncmp(entry.time_range_name, time_range_name, strlen(time_range_name)));
#endif /* 0 */
        assert(entry.counter_status == mib_counter_status);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_active);

        /* active -> destroy
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_destroy);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result != RULE_TYPE_OK);

        /* destroy -> notReady
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
        assert(RULE_TYPE_OK == result);

//FIXME: time range
#if 0 && (SYS_CPNT_TIME_BASED_ACL == TRUE)
        result = RULE_MGR_SetAccessGroupTimeRangeName(ifindex, mib_direction, mib_acl_type, time_range_name);
        assert(RULE_TYPE_OK == result);
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
        result = RULE_MGR_SetAccessGroupCounterStatus(ifindex, mib_direction, mib_acl_type, mib_counter_status);
        assert(RULE_TYPE_OK == result);
#endif /* (SYS_CPNT_ACL_COUNTER == TRUE) */

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index = acl_index);
#if 0 //FIXME: time range
        assert(0 == strncmp(entry.time_range_name, time_range_name, strlen(time_range_name)));
#endif /* 0 */
        assert(entry.counter_status == mib_counter_status);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);

        /* notReady -> destroy
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_destroy);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result != RULE_TYPE_OK);
    }

    return 0;
}

int
RULE_MGR_UT_SetAccessGroupStatus_Active()
{
    typedef struct
    {
        UI32_T ifindex;
        UI32_T mib_direction;
        UI32_T mib_acl_type;
    } INPUT_T;

    INPUT_T input[] =
    {
        {1, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_mac},
        {2, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_ip},
        {3, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_ipv6},
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_mac},
        {2, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_ip},
        {3, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_ipv6},
#endif /* (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE) */
    };

    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T mac_acl_index, ip_acl_index, ipv6_acl_index;
    RULE_TYPE_MIB_AccessGroupEntry_T entry;

    UI32_T i;
    char time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};

#if 0 && (SYS_CPNT_TIME_BASED_ACL == TRUE)
    UI16_T t_index;
    char *time_range_name = "daily-8-to-12";

    RULE_MGR_UT_Proc_Create_Periodic_Time_Range(time_range_name, 8, 0, 12, 0);

    result = RULE_MGR_GetTimeRangeIdByName(time_range_name, &t_index);
    assert(result == RULE_TYPE_OK);
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

    RULE_MGR_UT_Proc_Create_ACL("m1", RULE_TYPE_MAC_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("m1", &mac_acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_ACL("ip_1", RULE_TYPE_IP_EXT_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("ip_1", &ip_acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_ACL("ipv6_1", RULE_TYPE_IPV6_EXT_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("ipv6_1", &ipv6_acl_index);
    assert(result == RULE_TYPE_OK);


    for (i = 0; i < _countof(input); ++i)
    {
        INPUT_T *input_p = &input[i];
        UI32_T ifindex = input_p->ifindex;
        UI32_T mib_direction = input_p->mib_direction;
        UI32_T mib_acl_type = input_p->mib_acl_type;
        UI32_T mib_counter_status = VAL_diffServAccessGroupCounterStatus_enabled;
        UI32_T acl_index = (input_p->mib_acl_type == VAL_diffServAccessGroupType_mac)
                           ? mac_acl_index
                           : ((input_p->mib_acl_type == VAL_diffServAccessGroupType_ip)
                              ? ip_acl_index : ipv6_acl_index);

        entry.ifindex = ifindex;
        entry.direction = mib_direction;
        entry.type = mib_acl_type;

        /* -- Entry notExist
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_active);
        assert(RULE_TYPE_OK != result);

        /* -- Entry notReady
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
        assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        result = RULE_MGR_SetAccessGroupTimeRangeName(ifindex, mib_direction, mib_acl_type, time_range_name);
        assert(RULE_TYPE_OK == result);
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
        result = RULE_MGR_SetAccessGroupCounterStatus(ifindex, mib_direction, mib_acl_type, mib_counter_status);
        assert(RULE_TYPE_OK == result);
#endif /* (SYS_CPNT_ACL_COUNTER == TRUE) */

        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_active);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index = acl_index);
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        assert(0 == strncmp(entry.time_range_name, time_range_name, strlen(time_range_name)));
#else
        //assert(0 == strlen(entry.time_range_name));
#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
        assert(entry.counter_status == mib_counter_status);
#else
        assert(entry.counter_status == VAL_diffServAccessGroupCounterStatus_disabled);
#endif /* (SYS_CPNT_ACL_COUNTER == TRUE) */

        assert(entry.row_status == VAL_diffServAccessGroupStatus_active);

        {
            RULE_TYPE_Acl_T acl_entry;
            UI8_T *filter_port_list;

            result = RULE_MGR_GetAclByIndex(acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

            filter_port_list = (mib_direction == VAL_diffServAccessGroupDirection_ingress)
                               ? &acl_entry.filtering_in_port_list[0]
                               : &acl_entry.filtering_out_port_list[0];

            assert(RULE_MGR_IS_BIT_ON(filter_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex)));
        }

        /* -- Entry active
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_active);
        assert(RULE_TYPE_OK == result);
    }

    return 0;
}

int
RULE_MGR_UT_SetAccessGroupStatus_NotInService()
{
    typedef struct
    {
        UI32_T ifindex;
        UI32_T mib_direction;
        UI32_T mib_acl_type;
    } INPUT_T;

    INPUT_T input[] =
    {
        {1, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_mac},
        {2, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_ip},
        {3, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_ipv6},
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_mac},
        {2, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_ip},
        {3, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_ipv6},
#endif /* (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE) */
    };

    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T mac_acl_index, ip_acl_index, ipv6_acl_index;
    RULE_TYPE_MIB_AccessGroupEntry_T entry;

    UI32_T i;


    RULE_MGR_UT_Proc_Create_ACL("m1", RULE_TYPE_MAC_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("m1", &mac_acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_ACL("ip_1", RULE_TYPE_IP_EXT_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("ip_1", &ip_acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_ACL("ipv6_1", RULE_TYPE_IPV6_EXT_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("ipv6_1", &ipv6_acl_index);
    assert(result == RULE_TYPE_OK);

    for (i = 0; i < _countof(input); ++i)
    {
        INPUT_T *input_p = &input[i];
        UI32_T ifindex = input_p->ifindex;
        UI32_T mib_direction = input_p->mib_direction;
        UI32_T mib_acl_type = input_p->mib_acl_type;
        UI32_T acl_index = (input_p->mib_acl_type == VAL_diffServAccessGroupType_mac)
                           ? mac_acl_index
                           : ((input_p->mib_acl_type == VAL_diffServAccessGroupType_ip)
                              ? ip_acl_index : ipv6_acl_index);

        entry.ifindex = ifindex;
        entry.direction = mib_direction;
        entry.type = mib_acl_type;

        /* -- Entry notExist
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_notInService);
        assert(RULE_TYPE_OK != result);

        /* -- Entry notReady
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_notInService);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index = acl_index);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);

        /* -- Entry active
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_notInService);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index = acl_index);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);
    }

    return 0;
}

int
RULE_MGR_UT_SetAccessGroupStatus_NotReady()
{
    typedef struct
    {
        UI32_T ifindex;
        UI32_T mib_direction;
        UI32_T mib_acl_type;
    } INPUT_T;

    INPUT_T input[] =
    {
        {1, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_mac},
        {2, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_ip},
        {3, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_ipv6},
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_mac},
        {2, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_ip},
        {3, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_ipv6},
#endif /* (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE) */
    };

    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T mac_acl_index, ip_acl_index, ipv6_acl_index;
    RULE_TYPE_MIB_AccessGroupEntry_T entry;

    UI32_T i;


    RULE_MGR_UT_Proc_Create_ACL("m1", RULE_TYPE_MAC_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("m1", &mac_acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_ACL("ip_1", RULE_TYPE_IP_EXT_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("ip_1", &ip_acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_ACL("ipv6_1", RULE_TYPE_IPV6_EXT_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("ipv6_1", &ipv6_acl_index);
    assert(result == RULE_TYPE_OK);

    for (i = 0; i < _countof(input); ++i)
    {
        INPUT_T *input_p = &input[i];
        UI32_T ifindex = input_p->ifindex;
        UI32_T mib_direction = input_p->mib_direction;
        UI32_T mib_acl_type = input_p->mib_acl_type;
        UI32_T acl_index = (input_p->mib_acl_type == VAL_diffServAccessGroupType_mac)
        ? mac_acl_index : ((input_p->mib_acl_type == VAL_diffServAccessGroupType_ip)
                           ? ip_acl_index : ipv6_acl_index);

        entry.ifindex = ifindex;
        entry.direction = mib_direction;
        entry.type = mib_acl_type;

        /* -- Entry notExist
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_notReady);
        assert(RULE_TYPE_OK != result);

        /* -- Entry notReady
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_notReady);
        assert(RULE_TYPE_OK == result);

        // entry(notReady) with notReady event will destroy entry
        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result != RULE_TYPE_OK);

        /* -- Entry active
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_active);
        assert(RULE_TYPE_OK == result);

        // entry(active) with notReady event will destroy entry
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_notReady);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result != RULE_TYPE_OK);

        {
            RULE_TYPE_Acl_T acl_entry;
            UI8_T *filter_port_list;

            result = RULE_MGR_GetAclByIndex(acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

            filter_port_list = (mib_direction == VAL_diffServAccessGroupDirection_ingress)
            ? &acl_entry.filtering_in_port_list[0]
            : &acl_entry.filtering_out_port_list[0];

            assert(RULE_MGR_IS_BIT_OFF(filter_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex)));
        }

    }

    return 0;
}

int
RULE_MGR_UT_SetAccessGroupStatus_CreateAndGo()
{
    typedef struct
    {
        UI32_T ifindex;
        UI32_T mib_direction;
        UI32_T mib_acl_type;
    } INPUT_T;

    INPUT_T input[] =
    {
        {1, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_mac},
        {2, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_ip},
        {3, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_ipv6},
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_mac},
        {2, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_ip},
        {3, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_ipv6},
#endif /* (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE) */
    };

    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T mac_acl_index, ip_acl_index, ipv6_acl_index;
    RULE_TYPE_MIB_AccessGroupEntry_T entry;

    UI32_T i;


    RULE_MGR_UT_Proc_Create_ACL("m1", RULE_TYPE_MAC_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("m1", &mac_acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_ACL("ip_1", RULE_TYPE_IP_EXT_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("ip_1", &ip_acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_ACL("ipv6_1", RULE_TYPE_IPV6_EXT_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("ipv6_1", &ipv6_acl_index);
    assert(result == RULE_TYPE_OK);

    for (i = 0; i < _countof(input); ++i)
    {
        INPUT_T *input_p = &input[i];
        UI32_T ifindex = input_p->ifindex;
        UI32_T mib_direction = input_p->mib_direction;
        UI32_T mib_acl_type = input_p->mib_acl_type;
        UI32_T acl_index = (input_p->mib_acl_type == VAL_diffServAccessGroupType_mac)
        ? mac_acl_index : ((input_p->mib_acl_type == VAL_diffServAccessGroupType_ip)
                           ? ip_acl_index : ipv6_acl_index);

        entry.ifindex = ifindex;
        entry.direction = mib_direction;
        entry.type = mib_acl_type;

        /* -- Entry notExist
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndGo);
        assert(RULE_TYPE_OK != result);

        /* -- Entry notReady
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndGo);
        assert(RULE_TYPE_OK == result);

        // entry(notReady) with createAndGo event will do nothing
        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index == acl_index);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);

        /* -- Entry active
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_active);
        assert(RULE_TYPE_OK == result);

        // entry(active) with createAndGo event will do nothing
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndGo);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index == acl_index);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_active);

        {
            RULE_TYPE_Acl_T acl_entry;
            UI8_T *filter_port_list;

            result = RULE_MGR_GetAclByIndex(acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

            filter_port_list = (mib_direction == VAL_diffServAccessGroupDirection_ingress)
            ? &acl_entry.filtering_in_port_list[0]
            : &acl_entry.filtering_out_port_list[0];

            assert(RULE_MGR_IS_BIT_ON(filter_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex)));
        }

    }

    return 0;
}

int
RULE_MGR_UT_SetAccessGroupStatus_CreateAndWait()
{
    typedef struct
    {
        UI32_T ifindex;
        UI32_T mib_direction;
        UI32_T mib_acl_type;
    } INPUT_T;

    INPUT_T input[] =
    {
        {1, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_mac},
        {2, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_ip},
        {3, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_ipv6},
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_mac},
        {2, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_ip},
        {3, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_ipv6},
#endif /* (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE) */
    };

    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T mac_acl_index, ip_acl_index, ipv6_acl_index;
    RULE_TYPE_MIB_AccessGroupEntry_T entry;

    UI32_T i;


    RULE_MGR_UT_Proc_Create_ACL("m1", RULE_TYPE_MAC_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("m1", &mac_acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_ACL("ip_1", RULE_TYPE_IP_EXT_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("ip_1", &ip_acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_ACL("ipv6_1", RULE_TYPE_IPV6_EXT_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("ipv6_1", &ipv6_acl_index);
    assert(result == RULE_TYPE_OK);

    for (i = 0; i < _countof(input); ++i)
    {
        INPUT_T *input_p = &input[i];
        UI32_T ifindex = input_p->ifindex;
        UI32_T mib_direction = input_p->mib_direction;
        UI32_T mib_acl_type = input_p->mib_acl_type;
        UI32_T acl_index = (input_p->mib_acl_type == VAL_diffServAccessGroupType_mac)
        ? mac_acl_index : ((input_p->mib_acl_type == VAL_diffServAccessGroupType_ip)
        ? ip_acl_index : ipv6_acl_index);

        entry.ifindex = ifindex;
        entry.direction = mib_direction;
        entry.type = mib_acl_type;

        /* -- Entry notExist
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index == 0);
        assert(entry.counter_status == VAL_diffServAccessGroupCounterStatus_disabled);
        //assert(0 == strlen(entry.time_range_name));
        assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);

        /* -- Entry notReady
         */
        result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        // entry(notReady) with createAndWait event will do nothing
        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index == acl_index);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);

        /* -- Entry active
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_active);
        assert(RULE_TYPE_OK == result);

        // entry(active) with createAndWait event will do nothing
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index == acl_index);
        assert(entry.row_status == VAL_diffServAccessGroupStatus_active);

        {
            RULE_TYPE_Acl_T acl_entry;
            UI8_T *filter_port_list;

            result = RULE_MGR_GetAclByIndex(acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

            filter_port_list = (mib_direction == VAL_diffServAccessGroupDirection_ingress)
            ? &acl_entry.filtering_in_port_list[0]
            : &acl_entry.filtering_out_port_list[0];

            assert(RULE_MGR_IS_BIT_ON(filter_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex)));
        }

    }

    return 0;
}

int
RULE_MGR_UT_SetAccessGroupStatus_Destory()
{
    typedef struct
    {
        UI32_T ifindex;
        UI32_T mib_direction;
        UI32_T mib_acl_type;
    } INPUT_T;

    INPUT_T input[] =
    {
        {1, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_mac},
        {2, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_ip},
        {3, VAL_diffServAccessGroupDirection_ingress, VAL_diffServAccessGroupType_ipv6},
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_mac},
        {2, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_ip},
        {3, VAL_diffServAccessGroupDirection_egress, VAL_diffServAccessGroupType_ipv6},
#endif /* (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE) */
    };

    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T mac_acl_index, ip_acl_index, ipv6_acl_index;
    RULE_TYPE_MIB_AccessGroupEntry_T entry;

    UI32_T i;


    RULE_MGR_UT_Proc_Create_ACL("m1", RULE_TYPE_MAC_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("m1", &mac_acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_ACL("ip_1", RULE_TYPE_IP_EXT_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("ip_1", &ip_acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_ACL("ipv6_1", RULE_TYPE_IPV6_EXT_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);

    result = RULE_MGR_GetAclIdByName("ipv6_1", &ipv6_acl_index);
    assert(result == RULE_TYPE_OK);

    for (i = 0; i < _countof(input); ++i)
    {
        INPUT_T *input_p = &input[i];
        UI32_T ifindex = input_p->ifindex;
        UI32_T mib_direction = input_p->mib_direction;
        UI32_T mib_acl_type = input_p->mib_acl_type;
        UI32_T acl_index = (input_p->mib_acl_type == VAL_diffServAccessGroupType_mac)
        ? mac_acl_index : ((input_p->mib_acl_type == VAL_diffServAccessGroupType_ip)
                           ? ip_acl_index : ipv6_acl_index);

        entry.ifindex = ifindex;
        entry.direction = mib_direction;
        entry.type = mib_acl_type;

        /* -- Entry notExist
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_destroy);
        assert(RULE_TYPE_OK != result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result != RULE_TYPE_OK);

        /* -- Entry notReady
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_destroy);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result != RULE_TYPE_OK);

        /* -- Entry active
         */
        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
        assert(RULE_TYPE_OK == result);
        result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_active);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_destroy);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result != RULE_TYPE_OK);

        {
            RULE_TYPE_Acl_T acl_entry;
            UI8_T *filter_port_list;

            result = RULE_MGR_GetAclByIndex(acl_index, &acl_entry);
            assert(RULE_TYPE_OK == result);

            filter_port_list = (mib_direction == VAL_diffServAccessGroupDirection_ingress)
            ? &acl_entry.filtering_in_port_list[0]
            : &acl_entry.filtering_out_port_list[0];

            assert(RULE_MGR_IS_BIT_OFF(filter_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex)));
        }

    }

    return 0;
}


int
RULE_MGR_UT_SetAccessGroupStatus_SetAclIndex()
{
    UI32_T ifindex = 2;
    UI32_T mib_direction = VAL_diffServAccessGroupDirection_ingress;
    UI32_T mib_acl_type = VAL_diffServAccessGroupType_mac;

    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T acl_index = 0, acl_index2 = 0;
    RULE_TYPE_MIB_AccessGroupEntry_T entry;

    entry.ifindex = ifindex;
    entry.direction = mib_direction;
    entry.type = mib_acl_type;

    result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
    assert(RULE_TYPE_OK != result);

    result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
    assert(RULE_TYPE_OK != result);

    RULE_MGR_UT_Proc_Create_ACL("m1", RULE_TYPE_MAC_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);
    result = RULE_MGR_GetAclIdByName("m1", &acl_index);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_GetAccessGroupEntry(&entry);
    assert(result == RULE_TYPE_OK);
    assert(entry.acl_index == acl_index);
    assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);

    result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_active);
    assert(RULE_TYPE_OK == result);

    {
        RULE_TYPE_Acl_T acl_entry;
        UI8_T *filter_port_list;

        result = RULE_MGR_GetAclByIndex(acl_index, &acl_entry);
        assert(RULE_TYPE_OK == result);

        filter_port_list = (mib_direction == VAL_diffServAccessGroupDirection_ingress)
                           ? &acl_entry.filtering_in_port_list[0]
                           : &acl_entry.filtering_out_port_list[0];

        assert(RULE_MGR_IS_BIT_ON(filter_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex)));
    }

    /* Set same acl_index
     */
    result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
    assert(RULE_TYPE_OK == result);

    /* Set other acl_index
     */
    RULE_MGR_UT_Proc_Create_ACL("m2", RULE_TYPE_MAC_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);
    result = RULE_MGR_GetAclIdByName("m2", &acl_index2);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index2);
    assert(RULE_TYPE_OK != result);

    result = RULE_MGR_GetAccessGroupEntry(&entry);
    assert(result == RULE_TYPE_OK);
    assert(entry.acl_index == acl_index);
    assert(entry.row_status == VAL_diffServAccessGroupStatus_active);

    {
        RULE_TYPE_Acl_T acl_entry;
        UI8_T *filter_port_list;

        result = RULE_MGR_GetAclByIndex(acl_index, &acl_entry);
        assert(RULE_TYPE_OK == result);

        filter_port_list = (mib_direction == VAL_diffServAccessGroupDirection_ingress)
                           ? &acl_entry.filtering_in_port_list[0]
                           : &acl_entry.filtering_out_port_list[0];

        assert(RULE_MGR_IS_BIT_ON(filter_port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex)));
    }

    return 0;
}

int
RULE_MGR_UT_SetAccessGroupStatus_SetTimeRangeName()
{
#if 0 && (SYS_CPNT_TIME_BASED_ACL == TRUE)
    UI32_T ifindex = 2;
    UI32_T mib_direction = VAL_diffServAccessGroupDirection_ingress;
    UI32_T mib_acl_type = VAL_diffServAccessGroupType_mac;

    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T acl_index;
    RULE_TYPE_MIB_AccessGroupEntry_T entry;

    UI16_T t_index;
    char *time_range_name = "daily-8-to-12";

    entry.ifindex = ifindex;
    entry.direction = mib_direction;
    entry.type = mib_acl_type;

    RULE_MGR_UT_Proc_Create_ACL("m1", RULE_TYPE_MAC_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);
    result = RULE_MGR_GetAclIdByName("m1", &acl_index);
    assert(result == RULE_TYPE_OK);

    RULE_MGR_UT_Proc_Create_Periodic_Time_Range(time_range_name, 8, 0, 12, 0);
    result = RULE_MGR_GetTimeRangeIdByName(time_range_name, &t_index);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAccessGroupTimeRangeName(ifindex, mib_direction, mib_acl_type, time_range_name);
    assert(result != RULE_TYPE_OK);

    result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
    assert(RULE_TYPE_OK == result);

    {
        char long_name[SYS_ADPT_MAX_LENGTH_OF_TIME_ACL_TABLE_NAME + 2] = {0};

        result = RULE_MGR_SetAccessGroupTimeRangeName(ifindex, mib_direction, mib_acl_type, "NotExistTimeRangeName");
        assert(result != RULE_TYPE_OK);

        memset(long_name, 50, sizeof(long_name) - 1);
        long_name[sizeof(long_name) - 1] = '\0';

        result = RULE_MGR_SetAccessGroupTimeRangeName(ifindex, mib_direction, mib_acl_type, long_name);
        assert(result != RULE_TYPE_OK);
    }

    result = RULE_MGR_SetAccessGroupTimeRangeName(ifindex, mib_direction, mib_acl_type, "");
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_GetAccessGroupEntry(&entry);
    assert(result == RULE_TYPE_OK);
    assert(entry.acl_index == acl_index);
    assert(entry.time_range_name[0] == '\0');
    assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);


    result = RULE_MGR_SetAccessGroupTimeRangeName(ifindex, mib_direction, mib_acl_type, time_range_name);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_GetAccessGroupEntry(&entry);
    assert(result == RULE_TYPE_OK);
    assert(entry.acl_index == acl_index);
    assert(0 == strncmp(entry.time_range_name, time_range_name, sizeof(entry.time_range_name)));
    assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);

    result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_active);
    assert(RULE_TYPE_OK == result);

    {
        char *time_range_name2 = "daily-8-to-10";

        RULE_MGR_UT_Proc_Create_Periodic_Time_Range(time_range_name2, 8, 0, 10, 0);
        result = RULE_MGR_GetTimeRangeIdByName(time_range_name2, &t_index);
        assert(result == RULE_TYPE_OK);

        result = RULE_MGR_SetAccessGroupTimeRangeName(ifindex, mib_direction, mib_acl_type, time_range_name2);
        assert(result != RULE_TYPE_OK);

        result = RULE_MGR_GetAccessGroupEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.acl_index == acl_index);
        assert(0 == strncmp(entry.time_range_name, time_range_name, sizeof(entry.time_range_name)));
        assert(entry.row_status == VAL_diffServAccessGroupStatus_active);
    }

    result = RULE_MGR_SetAccessGroupTimeRangeName(ifindex, mib_direction, mib_acl_type, "");
    assert(result != RULE_TYPE_OK);

    result = RULE_MGR_GetAccessGroupEntry(&entry);
    assert(result == RULE_TYPE_OK);
    assert(entry.acl_index == acl_index);
    assert(0 == strncmp(entry.time_range_name, time_range_name, sizeof(entry.time_range_name)));
    assert(entry.row_status == VAL_diffServAccessGroupStatus_active);

#endif /* (SYS_CPNT_TIME_BASED_ACL == TRUE) */
    return 0;
}

int
RULE_MGR_UT_SetAccessGroupStatus_SetCounterStatus()
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)
    UI32_T ifindex = 2;
    UI32_T mib_direction = VAL_diffServAccessGroupDirection_ingress;
    UI32_T mib_acl_type = VAL_diffServAccessGroupType_mac;

    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T acl_index;
    RULE_TYPE_MIB_AccessGroupEntry_T entry;

    entry.ifindex = ifindex;
    entry.direction = mib_direction;
    entry.type = mib_acl_type;

    RULE_MGR_UT_Proc_Create_ACL("m1", RULE_TYPE_MAC_ACL, 1, RULE_CTRL_UT_COMPRESS_DEFAULT);
    result = RULE_MGR_GetAclIdByName("m1", &acl_index);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAccessGroupCounterStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupCounterStatus_enabled);
    assert(result != RULE_TYPE_OK);

    result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_createAndWait);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_SetAccessGroupAclIndex(ifindex, mib_direction, mib_acl_type, acl_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_SetAccessGroupCounterStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupCounterStatus_disabled);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAccessGroupCounterStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupCounterStatus_enabled);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_GetAccessGroupEntry(&entry);
    assert(result == RULE_TYPE_OK);
    assert(entry.acl_index == acl_index);
    assert(entry.counter_status == VAL_diffServAccessGroupCounterStatus_enabled);
    assert(entry.row_status == VAL_diffServAccessGroupStatus_notReady);

    result = RULE_MGR_SetAccessGroupRowStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupStatus_active);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_SetAccessGroupCounterStatus(ifindex, mib_direction, mib_acl_type, VAL_diffServAccessGroupCounterStatus_disabled);
    assert(result != RULE_TYPE_OK);

    result = RULE_MGR_GetAccessGroupEntry(&entry);
    assert(result == RULE_TYPE_OK);
    assert(entry.acl_index == acl_index);
    assert(entry.counter_status == VAL_diffServAccessGroupCounterStatus_enabled);
    assert(entry.row_status == VAL_diffServAccessGroupStatus_active);

#endif /* (SYS_CPNT_ACL_COUNTER == TRUE) */
    return 0;
}

int
RULE_MGR_UT_SetPolicyMapPortStatus_On_All_Port()
{
    UI32_T policy_map_index;
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_InOutDirection_T direction;
    RULE_TYPE_MIB_PolicyMapPortEntry_T entry;
    UI32_T ifindex;
    UI32_T mib_direction = VAL_diffServPolicyMapPortDirection_ingress;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 2000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 3000);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_REMARK_DSCP_TRANSMIT, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    result = RULE_MGR_GetPolicyMapIdByName("p1", &policy_map_index);
    assert(RULE_TYPE_OK == result);

    ifindex = 0;
    while (TRUE == RULE_OM_LocalGetNextInterface(&ifindex, &direction))
    {
        if (RULE_TYPE_IS_ALL_PORTS(ifindex))
        {
            continue;
        }

        if (TRUE != RULE_TYPE_IS_UPORT(ifindex) ||
            TRUE != RULE_TYPE_IS_CPU_PORT(ifindex))
        {
            /* FIXME: broadcom only support binding policy_map on uport and cpu port
             */
            continue;
        }

        mib_direction = (direction == RULE_TYPE_INBOUND)
                        ? VAL_diffServPolicyMapPortDirection_ingress
                        : VAL_diffServPolicyMapPortDirection_egress;

        entry.ifindex = ifindex;
        entry.direction = mib_direction;

        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndGo);
        assert(RULE_TYPE_OK != result);

        /* destroy -> notReady
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(RULE_TYPE_OK == result);
        assert(entry.policy_map_index == policy_map_index);
        assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_notReady);

        /* notReady -> active
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_active);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(RULE_TYPE_OK == result);
        assert(entry.policy_map_index == policy_map_index);
        assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_active);

        /* active -> destory
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_destroy);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(RULE_TYPE_OK != result);

    }

    return 0;
}

int
RULE_MGR_UT_SetPolicyMapPortStatus_Active()
{
    UI32_T input_mib_direction[] =
    {
        VAL_diffServPolicyMapPortDirection_ingress,
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        VAL_diffServPolicyMapPortDirection_egress
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */
    };

    UI32_T i;
    UI32_T policy_map_index;
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_MIB_PolicyMapPortEntry_T entry;
    UI32_T ifindex = 2;
    UI32_T mib_direction = VAL_diffServPolicyMapPortDirection_ingress;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 2000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 3000);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    result = RULE_MGR_GetPolicyMapIdByName("p1", &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(input_mib_direction); ++i)
    {
        mib_direction = input_mib_direction[i];

        entry.ifindex = ifindex;
        entry.direction = mib_direction;

        /* -- Entry notExist
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_active);
        assert(RULE_TYPE_OK != result);

        /* -- Entry notReady
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_active);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.policy_map_index == policy_map_index);
        assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_active);

        {
            RULE_TYPE_PolicyMap_T policy_map;
            UI8_T *port_list;

            result = RULE_MGR_GetPolicyMap(policy_map_index, &policy_map);
            assert(result == RULE_TYPE_OK);

            port_list = (mib_direction == VAL_diffServPolicyMapPortPortStatus_active)
                        ? &policy_map.qos_port_list[0] : &policy_map.egress_qos_port_list[0];

            assert(RULE_MGR_IS_BIT_ON(port_list, RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex)));

        }

        /* -- Entry active
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_active);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.policy_map_index == policy_map_index);
        assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_active);
    }

    return 0;
}

int
RULE_MGR_UT_SetPolicyMapPortStatus_NotInService()
{
    UI32_T input_mib_direction[] =
    {
        VAL_diffServPolicyMapPortDirection_ingress,
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        VAL_diffServPolicyMapPortDirection_egress
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */
    };

    UI32_T i;
    UI32_T policy_map_index;
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_MIB_PolicyMapPortEntry_T entry;
    UI32_T ifindex = 2;
    UI32_T mib_direction = VAL_diffServPolicyMapPortDirection_ingress;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 2000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 3000);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    result = RULE_MGR_GetPolicyMapIdByName("p1", &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(input_mib_direction); ++i)
    {
        mib_direction = input_mib_direction[i];

        entry.ifindex = ifindex;
        entry.direction = mib_direction;

        /* -- Entry notExist
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_notInService);
        assert(RULE_TYPE_OK != result);

        /* -- Entry notReady
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_notInService);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.policy_map_index == policy_map_index);
        assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_notReady);

        /* -- Entry active
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_active);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_notInService);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.policy_map_index == policy_map_index);
        assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_notReady);
    }

    return 0;
}

int
RULE_MGR_UT_SetPolicyMapPortStatus_NotReady()
{
    UI32_T input_mib_direction[] =
    {
        VAL_diffServPolicyMapPortDirection_ingress,
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        VAL_diffServPolicyMapPortDirection_egress
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */
    };

    UI32_T i;
    UI32_T policy_map_index;
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_MIB_PolicyMapPortEntry_T entry;
    UI32_T ifindex = 2;
    UI32_T mib_direction = VAL_diffServPolicyMapPortDirection_ingress;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 2000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 3000);
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    result = RULE_MGR_GetPolicyMapIdByName("p1", &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(input_mib_direction); ++i)
    {
        mib_direction = input_mib_direction[i];

        entry.ifindex = ifindex;
        entry.direction = mib_direction;

        /* -- Entry notExist
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_notReady);
        assert(RULE_TYPE_OK != result);

        /* -- Entry notReady
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);
        assert(RULE_TYPE_OK == result);

        // entry(notReady) with event notReady will destroy this entry
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_notReady);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result != RULE_TYPE_OK);

        /* -- Entry active
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_active);
        assert(RULE_TYPE_OK == result);

        // entry(active) with event notReady will destroy this entry
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_notReady);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result != RULE_TYPE_OK);

    }

    return 0;
}

int
RULE_MGR_UT_SetPolicyMapPortStatus_CreateAndGo()
{
    UI32_T input_mib_direction[] =
    {
        VAL_diffServPolicyMapPortDirection_ingress,
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        VAL_diffServPolicyMapPortDirection_egress
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */
    };

    UI32_T i;
    UI32_T policy_map_index;
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_MIB_PolicyMapPortEntry_T entry;
    UI32_T ifindex = 2;
    UI32_T mib_direction = VAL_diffServPolicyMapPortDirection_ingress;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 2000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 3000);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    result = RULE_MGR_GetPolicyMapIdByName("p1", &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(input_mib_direction); ++i)
    {
        mib_direction = input_mib_direction[i];

        entry.ifindex = ifindex;
        entry.direction = mib_direction;

        /* -- Entry notExist
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndGo);
        assert(RULE_TYPE_OK != result);

        /* -- Entry notReady
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);
        assert(RULE_TYPE_OK == result);

        // entry(notReady) with event createAndGo will do nothing
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndGo);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.policy_map_index == policy_map_index);
        assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_notReady);

        /* -- Entry active
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_active);
        assert(RULE_TYPE_OK == result);

        // entry(active) with event createAndGo will do nothing
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndGo);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.policy_map_index == policy_map_index);
        assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_active);
    }

    return 0;
}

int
RULE_MGR_UT_SetPolicyMapPortStatus_CreateAndWait()
{
    UI32_T input_mib_direction[] =
    {
        VAL_diffServPolicyMapPortDirection_ingress,
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        VAL_diffServPolicyMapPortDirection_egress
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */
    };

    UI32_T i;
    UI32_T policy_map_index;
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_MIB_PolicyMapPortEntry_T entry;
    UI32_T ifindex = 2;
    UI32_T mib_direction = VAL_diffServPolicyMapPortDirection_ingress;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 2000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 3000);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    result = RULE_MGR_GetPolicyMapIdByName("p1", &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(input_mib_direction); ++i)
    {
        mib_direction = input_mib_direction[i];

        entry.ifindex = ifindex;
        entry.direction = mib_direction;

        /* -- Entry notExist
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.policy_map_index == 0);
        assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_notReady);

        /* -- Entry notReady
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);
        assert(RULE_TYPE_OK == result);

        // entry(notReady) with event createAndWait will do nothing
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.policy_map_index == policy_map_index);
        assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_notReady);

        /* -- Entry active
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_active);
        assert(RULE_TYPE_OK == result);

        // entry(active) with event createAndWait will do nothing
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result == RULE_TYPE_OK);
        assert(entry.policy_map_index == policy_map_index);
        assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_active);
    }

    return 0;
}

int
RULE_MGR_UT_SetPolicyMapPortStatus_Destory()
{
    UI32_T input_mib_direction[] =
    {
        VAL_diffServPolicyMapPortDirection_ingress,
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        VAL_diffServPolicyMapPortDirection_egress
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */
    };

    UI32_T i;
    UI32_T policy_map_index;
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_MIB_PolicyMapPortEntry_T entry;
    UI32_T ifindex = 2;
    UI32_T mib_direction = VAL_diffServPolicyMapPortDirection_ingress;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 2000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 3000);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    result = RULE_MGR_GetPolicyMapIdByName("p1", &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(input_mib_direction); ++i)
    {
        mib_direction = input_mib_direction[i];

        entry.ifindex = ifindex;
        entry.direction = mib_direction;

        /* -- Entry notExist
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_destroy);
        assert(RULE_TYPE_OK != result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result != RULE_TYPE_OK);

        /* -- Entry notReady
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_destroy);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result != RULE_TYPE_OK);

        /* -- Entry active
         */
        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_active);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_destroy);
        assert(RULE_TYPE_OK == result);

        result = RULE_MGR_GetPolicyMapPortEntry(&entry);
        assert(result != RULE_TYPE_OK);
    }
    return 0;
}

int
RULE_MGR_UT_SetPolicyMapPortStatus_SetPolicyMapIndex()
{
    UI32_T policy_map_index, policy_map_index2;
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_MIB_PolicyMapPortEntry_T entry;
    UI32_T ifindex = 2;
    UI32_T mib_direction = VAL_diffServPolicyMapPortDirection_ingress;

    entry.ifindex = ifindex;
    entry.direction = mib_direction;

    RULE_MGR_UT_Proc_Create_Policy_Map("p1");

    RULE_MGR_UT_Proc_Create_Class_Map("c1", RULE_TYPE_CLASS_MAP_MATCH_ANY);

    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 1000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 2000);
    RULE_MGR_UT_Proc_Add_MF_ACE_To_Class_Map("c1", RULE_TYPE_MF_VLAN, 3000);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

    result = RULE_MGR_GetPolicyMapIdByName("p1", &policy_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);
    assert(RULE_TYPE_OK != result);

    result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_createAndWait);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, 123);
    assert(RULE_TYPE_OK != result);

    result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_GetPolicyMapPortEntry(&entry);
    assert(result == RULE_TYPE_OK);
    assert(entry.policy_map_index == policy_map_index);
    assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_notReady);

    result = RULE_MGR_SetPolicyMapPortRowStatus(ifindex, mib_direction, VAL_diffServPolicyMapPortPortStatus_active);

    RULE_MGR_UT_Proc_Create_Policy_Map("p2");
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Rate_Meter("p1", "c1", 100000);
#else
    RULE_MGR_Proc_Add_Policy_Map_Element_With_Flow_Meter("p1", "c1",
                                                         100000, 100000,
                                                         CONFIRM_ACTION_TRANSMIT, 0,
                                                         VIOLATE_ACTION_DROP, 1);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */
    result = RULE_MGR_GetPolicyMapIdByName("p2", &policy_map_index2);

    result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, policy_map_index2);
    assert(RULE_TYPE_OK != result);

    result = RULE_MGR_GetPolicyMapPortEntry(&entry);
    assert(result == RULE_TYPE_OK);
    assert(entry.policy_map_index == policy_map_index);
    assert(entry.row_status == VAL_diffServPolicyMapPortPortStatus_active);
    assert(RULE_TYPE_OK == result);

    result = RULE_MGR_SetPolicyMapPortPolcyMapIndex(ifindex, mib_direction, 0);
    assert(RULE_TYPE_OK != result);

    return 0;
}

#pragma mark Sortware Bundle Rate Limitation

int
RULE_MGR_UT_Delete_PolicyElement_That_Bind_In_Bundle()
{
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    const char *policy_map_name = "p1";
    UI32_T policy_map_index;

    struct class_map_s
    {
        const char *class_map_name;
        UI32_T class_map_index;
        UI32_T meter_index;
        UI32_T action_index;
        BOOL_T add_into_bundle;
    } policy_element_class[] =
    {
        {"c1", 0, 0, 0, TRUE},
        {"c2", 0, 0, 0, TRUE},
        {"c3", 0, 0, 0, TRUE},
        {"c4", 0, 0, 0, FALSE},
    };

    RULE_TYPE_UI_BundleGroup_T ui_bundle_group;

    UI32_T i;
    UI32_T result;

    memset(&ui_bundle_group, 0, sizeof(ui_bundle_group));

    result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(policy_element_class); ++i)
    {
        UI32_T                          class_map_index;
        RULE_TYPE_UI_Action_T           ui_action;
        RULE_TYPE_TBParamEntry_T        meter;
        RULE_TYPE_PolicyElement_T       policy_element;

        //
        // class-map
        //
        result = RULE_OM_Create_ClassMap(policy_element_class[i].class_map_name,
                                         RULE_TYPE_CLASS_MAP_MATCH_ANY,
                                         &policy_element_class[i].class_map_index);
        assert(RULE_TYPE_OK == result);

        class_map_index = policy_element_class[i].class_map_index;

        result = RULE_MGR_AddElement2PolicyMap(policy_map_index, class_map_index);
        assert(RULE_TYPE_OK == result);

        //
        // meter
        //
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
        meter.meter_model   = RULE_TYPE_METER_MODE_RATE;
        meter.rate          = 5000;
#else
        meter.meter_model   = RULE_TYPE_METER_MODE_FLOW;
        meter.rate          = 5000;
        meter.burst_size    = 4000;
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

        result = RULE_MGR_SetElementMeterFromPolicyMap(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == result);

        //
        // action
        //
        ui_action.in_action_type        = IN_ACTION_INVALID;
        ui_action.out_action_type       = OUT_ACTION_INVALID;
        ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
        ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
        ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

        ui_action.class_action_type     = CLASS_ACTION_SET_PRIORITY;
        ui_action.class_action_value    = 1000;

        result = RULE_MGR_SetElementActionFromPolicyMap(policy_map_index, class_map_index, &ui_action);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetPolicyMapElementEntry(policy_map_index, class_map_index, &policy_element);
        assert(RULE_TYPE_OK == result);

        policy_element_class[i].class_map_index   = policy_element.class_map_index;
        policy_element_class[i].meter_index       = policy_element.meter_index;
        policy_element_class[i].action_index      = policy_element.action_index;

        if (policy_element_class[i].add_into_bundle)
        {
            strcpy(ui_bundle_group.class_map_name_list[ui_bundle_group.class_map_count ++], policy_element_class[i].class_map_name);
        }
    }

    ui_bundle_group.index = 1;
    ui_bundle_group.valid_status = TRUE;

    result = RULE_MGR_SetUIBundleGroup(policy_map_name, &ui_bundle_group);
    assert(RULE_TYPE_OK == result);

    //
    // start
    //
    for (i = 0; i < _countof(policy_element_class); ++i)
    {
        UI32_T                          class_map_index;

        class_map_index = policy_element_class[i].class_map_index;

        result = RULE_MGR_DelClassMap(class_map_index);

        if (policy_element_class[i].add_into_bundle)
        {
            assert(RULE_TYPE_OK != result);
        }
        // We can not test the success case due to the code doesn't allow delete a class-map
        // that bind into a policy-map.
        // So need to trace code manually.
//        else
//        {
//            assert(RULE_TYPE_OK == result);
//        }

        result = RULE_MGR_DelElementFromPolicyMap(policy_map_index, class_map_index);
        if (policy_element_class[i].add_into_bundle)
        {
            assert(RULE_TYPE_OK != result);
        }
        else
        {
            assert(RULE_TYPE_OK == result);
        }
    }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    return 0;
}

int
RULE_MGR_UT_Delete_PolicyElement_Meter_That_Bind_In_Bundle()
{
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    const char *policy_map_name = "p1";
    UI32_T policy_map_index;

    struct class_map_s
    {
        const char *class_map_name;
        UI32_T class_map_index;
        UI32_T meter_index;
        UI32_T action_index;
        BOOL_T add_into_bundle;
    } policy_element_class[] =
    {
        {"c1", 0, 0, 0, TRUE},
        {"c2", 0, 0, 0, TRUE},
        {"c3", 0, 0, 0, TRUE},
        {"c4", 0, 0, 0, FALSE},
    };

    RULE_TYPE_UI_BundleGroup_T ui_bundle_group;

    UI32_T i;
    UI32_T result;

    memset(&ui_bundle_group, 0, sizeof(ui_bundle_group));

    result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(policy_element_class); ++i)
    {
        UI32_T                          class_map_index;
        RULE_TYPE_UI_Action_T           ui_action;
        RULE_TYPE_TBParamEntry_T        meter;
        RULE_TYPE_PolicyElement_T       policy_element;

        //
        // class-map
        //
        result = RULE_OM_Create_ClassMap(policy_element_class[i].class_map_name,
                                         RULE_TYPE_CLASS_MAP_MATCH_ANY,
                                         &policy_element_class[i].class_map_index);
        assert(RULE_TYPE_OK == result);

        class_map_index = policy_element_class[i].class_map_index;

        result = RULE_MGR_AddElement2PolicyMap(policy_map_index, class_map_index);
        assert(RULE_TYPE_OK == result);

        //
        // meter
        //
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
        meter.meter_model   = RULE_TYPE_METER_MODE_RATE;
        meter.rate          = 5000;
#else
        meter.meter_model   = RULE_TYPE_METER_MODE_FLOW;
        meter.rate          = 5000;
        meter.burst_size    = 4000;
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

        result = RULE_MGR_SetElementMeterFromPolicyMap(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == result);

        //
        // action
        //
        ui_action.in_action_type        = IN_ACTION_INVALID;
        ui_action.out_action_type       = OUT_ACTION_INVALID;
        ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
        ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
        ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

        ui_action.class_action_type     = CLASS_ACTION_SET_PRIORITY;
        ui_action.class_action_value    = 1000;

        result = RULE_MGR_SetElementActionFromPolicyMap(policy_map_index, class_map_index, &ui_action);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetPolicyMapElementEntry(policy_map_index, class_map_index, &policy_element);
        assert(RULE_TYPE_OK == result);

        policy_element_class[i].class_map_index   = policy_element.class_map_index;
        policy_element_class[i].meter_index       = policy_element.meter_index;
        policy_element_class[i].action_index      = policy_element.action_index;

        if (policy_element_class[i].add_into_bundle)
        {
            strcpy(ui_bundle_group.class_map_name_list[ui_bundle_group.class_map_count ++], policy_element_class[i].class_map_name);
        }
    }

    ui_bundle_group.index = 1;
    ui_bundle_group.valid_status = TRUE;

    result = RULE_MGR_SetUIBundleGroup(policy_map_name, &ui_bundle_group);
    assert(RULE_TYPE_OK == result);

    //
    // start
    //
    for (i = 0; i < _countof(policy_element_class); ++i)
    {
        UI32_T                          class_map_index;
        RULE_TYPE_TBParamEntry_T        meter;

        class_map_index = policy_element_class[i].class_map_index;

        //
        // meter
        //
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
        meter.meter_model   = RULE_TYPE_METER_MODE_RATE;
        meter.rate          = 5000;
#else
        meter.meter_model   = RULE_TYPE_METER_MODE_FLOW;
        meter.rate          = 5000;
        meter.burst_size    = 4000;
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

        result = RULE_MGR_DelElementMeterFromPolicyMap(policy_map_index, class_map_index, &meter);

        if (policy_element_class[i].add_into_bundle)
        {
            assert(RULE_TYPE_OK != result);
        }
        else
        {
            assert(RULE_TYPE_OK == result);
        }
    }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    return 0;
}

int
RULE_MGR_UT_Delete_PolicyElement_Action_That_Bind_In_Bundle()
{
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    const char *policy_map_name = "p1";
    UI32_T policy_map_index;

    struct class_map_s
    {
        const char *class_map_name;
        UI32_T class_map_index;
        UI32_T meter_index;
        UI32_T action_index;
        BOOL_T add_into_bundle;
    } policy_element_class[] =
    {
        {"c1", 0, 0, 0, TRUE},
        {"c2", 0, 0, 0, TRUE},
        {"c3", 0, 0, 0, TRUE},
        {"c4", 0, 0, 0, FALSE},
    };

    RULE_TYPE_UI_BundleGroup_T ui_bundle_group;

    UI32_T i;
    UI32_T result;

    memset(&ui_bundle_group, 0, sizeof(ui_bundle_group));

    result = RULE_OM_Create_PolicyMap(policy_map_name, &policy_map_index);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(policy_element_class); ++i)
    {
        UI32_T                          class_map_index;
        UI32_T                          class_index;

        RULE_TYPE_UI_Action_T           ui_action;
        RULE_TYPE_TBParamEntry_T        meter;

        RULE_TYPE_UI_ClassMapElement_T  class_element;
        RULE_TYPE_PolicyElement_T       policy_element;

        //
        // class-map
        //
        result = RULE_OM_Create_ClassMap(policy_element_class[i].class_map_name,
                                         RULE_TYPE_CLASS_MAP_MATCH_ANY,
                                         &policy_element_class[i].class_map_index);
        assert(RULE_TYPE_OK == result);

        class_map_index = policy_element_class[i].class_map_index;

        result = RULE_MGR_AddElement2PolicyMap(policy_map_index, class_map_index);
        assert(RULE_TYPE_OK == result);

        //
        // Match MF
        //

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;
        class_element.element.mf_entry.mf_value = 1001;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // Match MF
        //

        class_element.class_type = RULE_TYPE_CLASS_MF;
        class_element.element.mf_entry.mf_type  = RULE_TYPE_MF_VLAN;
        class_element.element.mf_entry.mf_value = 2001;

        result = RULE_OM_Add_ClassMapElement_To_ClassMap(class_map_index, &class_element, &class_index);
        assert(RULE_TYPE_OK == result);

        //
        // meter
        //
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
        meter.meter_model   = RULE_TYPE_METER_MODE_RATE;
        meter.rate          = 5000;
#else
        meter.meter_model   = RULE_TYPE_METER_MODE_FLOW;
        meter.rate          = 5000;
        meter.burst_size    = 4000;
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

        result = RULE_MGR_SetElementMeterFromPolicyMap(policy_map_index, class_map_index, &meter);
        assert(RULE_TYPE_OK == result);

        //
        // action
        //
        memset(&ui_action, 0, sizeof(ui_action));

        ui_action.in_action_type        = IN_ACTION_INVALID;
        ui_action.out_action_type       = OUT_ACTION_INVALID;
        ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
        ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
        ui_action.violate_action_type   = VIOLATE_ACTION_DROP;

        ui_action.class_action_type     = CLASS_ACTION_SET_PRIORITY;
        ui_action.class_action_value    = 1000;

        result = RULE_MGR_SetElementActionFromPolicyMap(policy_map_index, class_map_index, &ui_action);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetPolicyMapElementEntry(policy_map_index, class_map_index, &policy_element);
        assert(RULE_TYPE_OK == result);

        policy_element_class[i].class_map_index   = policy_element.class_map_index;
        policy_element_class[i].meter_index       = policy_element.meter_index;
        policy_element_class[i].action_index      = policy_element.action_index;

        if (policy_element_class[i].add_into_bundle)
        {
            strcpy(ui_bundle_group.class_map_name_list[ui_bundle_group.class_map_count ++], policy_element_class[i].class_map_name);
        }
    }

    ui_bundle_group.index = 1;
    ui_bundle_group.valid_status = TRUE;

    result = RULE_MGR_SetUIBundleGroup(policy_map_name, &ui_bundle_group);
    assert(RULE_TYPE_OK == result);

    //
    // Bind to port
    //
    {
        enum
        {
            MIN_IFINDEX = 1,
            MAX_IFINDEX = 3,
            MIN_DIRECTION = RULE_TYPE_INBOUND,
            MAX_DIRECTION = RULE_TYPE_OUTBOUND
        };

        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T direction;

        for (ifindex = MIN_IFINDEX; ifindex <= MAX_IFINDEX; ++ ifindex)
        {
            for (direction = MIN_DIRECTION; direction <= MAX_DIRECTION; ++ direction)
            {
                result = RULE_CTRL_SetPolicyMap(ifindex, direction, policy_map_index, TRUE);
                assert(RULE_TYPE_OK == result);

                result = RULE_OM_BindPort2PolicyMap(ifindex, direction, policy_map_index);
                assert(RULE_TYPE_OK == result);
            }
        }
    }

    //
    // start
    //
    for (i = 0; i < _countof(policy_element_class); ++i)
    {
        UI32_T                          class_map_index;
        RULE_TYPE_UI_Action_T           ui_action;
        RULE_TYPE_TBParamEntry_T        meter;

        class_map_index = policy_element_class[i].class_map_index;

        //
        // meter
        //
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
        meter.meter_model   = RULE_TYPE_METER_MODE_RATE;
        meter.rate          = 5000;
#else
        meter.meter_model   = RULE_TYPE_METER_MODE_FLOW;
        meter.rate          = 5000;
        meter.burst_size    = 4000;
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE */

        result = RULE_MGR_DelElementMeterFromPolicyMap(policy_map_index, class_map_index, &meter);

        if (policy_element_class[i].add_into_bundle)
        {
            assert(RULE_TYPE_OK != result);
        }
        else
        {
            assert(RULE_TYPE_OK == result);
        }

        //
        // out-of-profile action
        //
        memset(&ui_action, 0, sizeof(ui_action));

        ui_action.in_action_type        = IN_ACTION_INVALID;
        ui_action.out_action_type       = OUT_ACTION_INVALID;
        ui_action.confirm_action_type   = CONFIRM_ACTION_TRANSMIT;
        ui_action.exceed_action_type    = EXCEED_ACTION_DROP;
        ui_action.violate_action_type   = VIOLATE_ACTION_DROP;
        ui_action.class_action_type     = CLASS_ACTION_INVALID;

        result = RULE_MGR_DelElementActionFromPolicyMap(policy_map_index, class_map_index, &ui_action);

        if (policy_element_class[i].add_into_bundle)
        {
            assert(RULE_TYPE_OK != result);
        }
        else
        {
            assert(RULE_TYPE_OK == result);
        }

        //
        // class action
        //
        memset(&ui_action, 0, sizeof(ui_action));

        ui_action.in_action_type        = IN_ACTION_INVALID;
        ui_action.out_action_type       = OUT_ACTION_INVALID;
        ui_action.confirm_action_type   = CONFIRM_ACTION_INVALID;
        ui_action.exceed_action_type    = EXCEED_ACTION_INVALID;
        ui_action.violate_action_type   = VIOLATE_ACTION_INVALID;
        ui_action.class_action_type     = CLASS_ACTION_SET_PRIORITY;

        result = RULE_MGR_DelElementActionFromPolicyMap(policy_map_index, class_map_index, &ui_action);
//        assert(RULE_TYPE_OK == result);

        RULE_CTRL_UT_Proc_Validate_Rule();
    }
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */
    return 0;
}

#pragma mark EPR

#if 0 /* FIXME: time range */
int
RULE_MGR_UT_Delete_Rule_With_Timerange()
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)

    UI32_T acl_index;
    char *acl_name = "m1";
    RULE_TYPE_Ace_Entry_T ace;
    unsigned short t_index;
    RULE_TYPE_RETURN_TYPE_T ret;

    RULE_MGR_CreateAcl(acl_name, RULE_TYPE_MAC_ACL);

    RULE_MGR_UT_Proc_Create_Periodic_Time_Range("daily-8-to-12", 8, 0, 12, 0);

    ace.ace_type = RULE_TYPE_MAC_ACL;
    RULE_MGR_InitAce(&ace);

    ret = RULE_MGR_GetTimeRangeIdByName("daily-8-to-12", &t_index);
    assert(ret == RULE_TYPE_OK);

    ace.time_range_index = t_index;

    ret = RULE_MGR_GetAclIdByName(acl_name, &acl_index);
    assert(ret == RULE_TYPE_OK);

    ret = RULE_MGR_SetAce2Acl(acl_index, RULE_TYPE_MAC_ACL, &ace);
    assert(ret == RULE_TYPE_OK);

    ret = RULE_MGR_DelAcl(acl_index);
    assert(ret == RULE_TYPE_OK);

    ret = RULE_MGR_DeleteTimeRange("daily-8-to-12");
    assert(ret == RULE_TYPE_OK);

#endif // SYS_CPNT_TIME_BASED_ACL

    return 0;
}
#endif /* 0 */

int
RULE_MGR_UT_MIB_AttachAceToACL()
{
    /* Exception when attach ip STD ACE to ACL
     */
    UI32_T ace_index = 1;
    UI32_T acl_index = 1;
    RULE_TYPE_RETURN_TYPE_T result;

    // IPv4 standard ========================================================
    /* add IP standard ace
     */
    result = RULE_MGR_SetAceRowStatus(ace_index, RULE_TYPE_IP_ACL, VAL_diffServIpAceStatus_createAndWait);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAceTypeByIndex(ace_index, RULE_TYPE_IP_STD_ACL);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAceRowStatus(ace_index, RULE_TYPE_IP_ACL, VAL_diffServIpAceStatus_active);
    assert(result == RULE_TYPE_OK);

    /* add IP ACL
     */
    result = RULE_MGR_SetAclRowStatus(acl_index, VAL_diffServAclStatus_createAndWait);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAclTypeByIndex(acl_index, VAL_diffServAclType_ipstandard);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAclRowStatus(acl_index, VAL_diffServAclStatus_active);
    assert(result == RULE_TYPE_OK);

    /* attach
     */
    result = RULE_MGR_SetAclAttachCtrl(acl_index,
                                       VAL_diffServAclAttachCtlAceType_ipAce,
                                       ace_index,
                                       VAL_diffServAclAttachCtlAction_attaching);
    assert(result == RULE_TYPE_OK);

    // IPv6 standard ========================================================
    /* add IPv6 standard ace
     */
    ace_index = 2;
    acl_index = 2;

    result = RULE_MGR_SetAceRowStatus(ace_index, RULE_TYPE_IPV6_ACL, VAL_diffServIpv6AceStatus_createAndWait);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAceTypeByIndex(ace_index, RULE_TYPE_IPV6_STD_ACL);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAceRowStatus(ace_index, RULE_TYPE_IPV6_ACL, VAL_diffServIpv6AceStatus_active);
    assert(result == RULE_TYPE_OK);

    /* add IPv6 ACL
     */
    result = RULE_MGR_SetAclRowStatus(acl_index, VAL_diffServAclStatus_createAndWait);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAclTypeByIndex(acl_index, VAL_diffServAclType_ipv6standard);
    assert(result == RULE_TYPE_OK);

    result = RULE_MGR_SetAclRowStatus(acl_index, VAL_diffServAclStatus_active);
    assert(result == RULE_TYPE_OK);

    /* attach
     */
    result = RULE_MGR_SetAclAttachCtrl(acl_index,
                                       VAL_diffServAclAttachCtlAceType_ipv6Ace,
                                       ace_index,
                                       VAL_diffServAclAttachCtlAction_attaching);
    assert(result == RULE_TYPE_OK);

    return 0;
}

int
RULE_MGR_UT_Get_Class_Map_Entry_With_Multi_Element_Type()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MIXED_MODE == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T class_map_index = 1;
    UI32_T i;
    UI8_T  class_type[MAXSIZE_diffServClassMapElementIndexTypeList] = {0};
    UI8_T  class_index_list[MAXSIZE_diffServClassMapElementIndexList] = {0};

    struct {
        RULE_TYPE_AclType_T ace_type;
        UI32_T ace_status;
        UI32_T element_type;
    } class_map_element_ar[] = {
        {RULE_TYPE_IPV6_ACL, VAL_diffServIpv6AceStatus_createAndGo, VAL_diffServClassMapAttachCtlElementIndexType_ipv6Ace},
        {RULE_TYPE_MAC_ACL, VAL_diffServMacAceStatus_createAndGo, VAL_diffServClassMapAttachCtlElementIndexType_macAce},
        {RULE_TYPE_IP_ACL, VAL_diffServIpAceStatus_createAndGo, VAL_diffServClassMapAttachCtlElementIndexType_ipAce}
    };

    result = RULE_MGR_SetClassMapRowStatus(class_map_index, VAL_diffServClassMapStatus_createAndGo);
    assert(result == RULE_TYPE_OK);

    for (i = 0; i < _countof(class_map_element_ar); ++i)
    {
        UI32_T index = i+1;

        result = RULE_MGR_SetAceRowStatus(index, class_map_element_ar[i].ace_type, class_map_element_ar[i].ace_status);
        assert(result == RULE_TYPE_OK);

        result = RULE_MGR_SetClassMapAttachCtrl(class_map_index,
                                                class_map_element_ar[i].element_type,
                                                index,
                                                VAL_diffServClassMapAttachCtlAction_attaching);
        assert(result == RULE_TYPE_OK);

        {
            UI16_T network_byte_order_ui16;
            const int max_byte_per_element = 2;

            network_byte_order_ui16 = L_STDLIB_Hton16 (class_map_element_ar[i].element_type);
            memcpy(&class_type[i * max_byte_per_element], &network_byte_order_ui16, sizeof(UI16_T));

            network_byte_order_ui16 = L_STDLIB_Hton16 (index);
            memcpy(&class_index_list[i * max_byte_per_element], &network_byte_order_ui16, sizeof(UI16_T));
        }
    }

    {
        RULE_TYPE_MIB_ClassMap_T class_map;

        memset(&class_map, 0, sizeof(class_map));

        result = RULE_MGR_GetMibClassMap(class_map_index, &class_map);
        assert(result == RULE_TYPE_OK);

        assert(0 == memcmp(class_map.class_type, class_type, sizeof(class_type)));
        assert(0 == memcmp(class_map.class_index_list, class_index_list, sizeof(class_index_list)));

    }

#endif /* (SYS_CPNT_QOS_V2_CLASS_MAP_MIXED_MODE == TRUE) */
    return 0;
}


int RULE_MGR_UT_Set_Default_Rule()
{
    /* FIXME: test this
     */

    /* Add default rule, same with L4_MGR_EnterMasterMode()
     */
#if (SYS_CPNT_OSPF6 == TRUE)
    if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_OSPF6, NULL))
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_OSPF6 failed");
#endif

#if (SYS_CPNT_DVMRP == TRUE)
    if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_DVMRP, NULL))
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_DVMRP failed");
#endif

    if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_L2_SLF, NULL))
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_L2_SLF failed");

    if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_ALL_HOST, NULL))
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_ALL_HOST failed");
    if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_ALL_ROUTER, NULL))
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_ALL_ROUTER failed");
    if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_MY_MAC_MY_IP, NULL))
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_MY_MAC_MY_IP failed");
    if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_IP_OPTION, NULL))
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_IP_OPTION failed");

    if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_BPDU, NULL))
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_BPDU failed");
    if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_L2CP, NULL))
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_L2CP failed");

#if (SYS_CPNT_IPV6 == TRUE)
    if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_IPV6_ALL_NODES, NULL))
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_IPV6_ALL_NODES failed");
    if (RULE_TYPE_FAIL == RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_IPV6_ALL_ROUTERS, NULL))
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_IPV6_ALL_ROUTERS failed");
#endif

#if (SYS_CPNT_ROUTING == TRUE)
    if (RULE_TYPE_OK != RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_MY_MAC_ARP, NULL))
    {
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_PacketType_MY_MAC_ARP failed");
    }
#endif /* SYS_CPNT_ROUTING */

#if (SYS_CPNT_CRAFT_PORT == TRUE) && (SYS_CPNT_CRAFT_PORT_MODE == SYS_CPNT_CRAFT_PORT_MODE_FRONT_PORT_CRAFT_PORT)
    if (RULE_TYPE_OK != RULE_MGR_TrapPacket2Cpu(TRUE, RULE_TYPE_Packettype_MY_MAC_CRAFT_PORT, NULL))
    {
        printf("\r\n[L4_MGR_EnterMasterMode] RULE_TYPE_Packettype_MY_MAC_CRAFT_PORT failed");
    }
#endif /* SYS_CPNT_CRAFT_PORT */


    // DEVRM_SetDebugFlags(DEVRM_DEBUG_FLAGS_BIT_ERR | DEVRM_DEBUG_FLAGS_BIT_LOG | DEVRM_DEBUG_FLAGS_BIT_TRACE);
    // _DEVRM_BackdoorMain();

    return 0;
}

#pragma mark Run test case
/*------------------------------------------------------------------------------
 * Entry Pointer
 *------------------------------------------------------------------------------
 */
int
RULE_MGR_UT_RunTestCases()
{
#define RULE_MGR_UT_TEST(func)                              \
    printf("\r\n>>== Test (%s)==\r\n", #func);              \
    UT_Test(RULE_MGR_UT_Setup, RULE_MGR_UT_Teardown, func); \
    printf("<<== End Test (%s)==\r\n", #func)

    /* priority test
     */
    if (1)
    {

    }

    if (l4_options.run_rule_mgr)
    {
        //
        // Basic ACL test items
        //
        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_ACL);

        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_ACL_With_Same_Time_Range);

        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_ACL_With_Diff_Time_Range);
#if 0 /* FIXME: Time range */
        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_ACL_With_Time_Range_And_Counter);
#endif /* 0 */
        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_ACL_With_Counter);

        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_All_Type_Of_ACLs_On_Port_At_The_Same_Time);

        RULE_MGR_UT_TEST(RULE_MGR_UT_Delete_ACL_Shall_Unbind_From_Port);

        RULE_MGR_UT_TEST(RULE_MGR_UT_Get_Next_Hardware_ACE);
        RULE_MGR_UT_TEST(RULE_MGR_UT_Clear_Hardware_ACL_Counter_For_All);
        RULE_MGR_UT_TEST(RULE_MGR_UT_Clear_Hardware_ACL_Counter_By_Direction);
        RULE_MGR_UT_TEST(RULE_MGR_UT_Clear_Hardware_ACL_Counter_By_Port);
        RULE_MGR_UT_TEST(RULE_MGR_UT_Clear_Hardware_ACL_Counter_By_ACL);
        RULE_MGR_UT_TEST(RULE_MGR_UT_Clear_Hardware_ACL_Counter_By_ACE);

        RULE_MGR_UT_TEST(RULE_MGR_UT_Defrag_Rule_Failure);

        //
        // Basic Policy-map test items
        //
        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_Policy_With_Match_Any_Class_Map_With_MF);
        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_Policy_With_Match_Any_Class_Map_With_ACL);
        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_Policy_With_Match_Any_Class_Map_With_MF_ACL_Mix);

        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_Policy_With_Match_All_Class_Map_With_MF);
        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_Policy_With_Match_All_Class_Map_With_ACL);
        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_Policy_With_Match_All_Class_Map_With_MF_ACL_Mix);

        RULE_MGR_UT_TEST(RULE_MGR_UT_Apply_Policy_With_Match_Any_All_Class_Map_Mix);

        //
        // MIB
        //
        RULE_MGR_UT_TEST(RULE_MGR_UT_Set_Class_Map_Attach_Ctrl);
        RULE_MGR_UT_TEST(RULE_MGR_UT_Set_Policy_Map_Attach_Ctrl);

        /* ARP ACL */
        RULE_MGR_UT_TEST(RULE_MGR_UT_MIB_SetArpAce);

#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_SRTCM_COLOR_BLIND)
        RULE_MGR_UT_TEST(RULE_MGR_UT_Consistency_Check_For_Failed_To_Set_Policy_Map_Attach_Ctrl);
#endif /* SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_SRTCM_COLOR_BLIND */


        RULE_MGR_UT_TEST(RULE_MGR_UT_SetAccessGroupStatus_On_All_Port);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetAccessGroupStatus_Active);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetAccessGroupStatus_NotInService);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetAccessGroupStatus_NotReady);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetAccessGroupStatus_CreateAndGo);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetAccessGroupStatus_CreateAndWait);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetAccessGroupStatus_Destory);

        RULE_MGR_UT_TEST(RULE_MGR_UT_SetAccessGroupStatus_SetAclIndex);
        // FIXME: time range
        //RULE_MGR_UT_TEST(RULE_MGR_UT_SetAccessGroupStatus_SetTimeRangeName);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetAccessGroupStatus_SetCounterStatus);

        RULE_MGR_UT_TEST(RULE_MGR_UT_SetPolicyMapPortStatus_On_All_Port);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetPolicyMapPortStatus_Active);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetPolicyMapPortStatus_NotInService);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetPolicyMapPortStatus_NotReady);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetPolicyMapPortStatus_CreateAndGo);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetPolicyMapPortStatus_CreateAndWait);
        RULE_MGR_UT_TEST(RULE_MGR_UT_SetPolicyMapPortStatus_Destory);

        RULE_MGR_UT_TEST(RULE_MGR_UT_SetPolicyMapPortStatus_SetPolicyMapIndex);

        //
        // Sortware Bundle Rate Limitation
        //
        RULE_MGR_UT_TEST(RULE_MGR_UT_Delete_PolicyElement_That_Bind_In_Bundle);
        RULE_MGR_UT_TEST(RULE_MGR_UT_Delete_PolicyElement_Meter_That_Bind_In_Bundle);
        RULE_MGR_UT_TEST(RULE_MGR_UT_Delete_PolicyElement_Action_That_Bind_In_Bundle);

        // EPR
#if 0 /* FIXME: time range */
        // ASF4628BBS5-FLF-EC-0048
        RULE_MGR_UT_TEST(RULE_MGR_UT_Delete_Rule_With_Timerange);
#endif /* 0 */
        RULE_MGR_UT_TEST(RULE_MGR_UT_MIB_AttachAceToACL);
        // ECS4210-52P-00540
        RULE_MGR_UT_TEST(RULE_MGR_UT_Get_Class_Map_Entry_With_Multi_Element_Type);
    }
    else
    {
        printf("%s %d: Skip all rule_mgr test items\r\n", __FUNCTION__, __LINE__);
    }

    return 0;

#undef RULE_MGR_UT_TEST
}
