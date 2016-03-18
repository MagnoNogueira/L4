/*
 *  l4_mgr_utest_cases.c
 *  l4_mgr
 *
 *  Created by yehjunying on 2011/7/10.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
/*------------------------------------------------------------------------------
 * Help Functions
 *------------------------------------------------------------------------------
 */


/*------------------------------------------------------------------------------
 * Test Functions
 *------------------------------------------------------------------------------
 */
int
L4_MGR_UT_Setup()
{
    RULE_CTRL_UT_Setup();

    L4_MGR_InitiateSystemResources();
    L4_MGR_EnterMasterMode();

    return 0;
}

int
L4_MGR_UT_Teardown()
{
    RULE_CTRL_UT_Teardown();
    return 0;
}

int
L4_MGR_UT_Get_Next_DiffServ_Acl_Hw_Counter_Entry()
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

    //
    // Sorted by precedence
    //
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

    UI32_T i, pi;

    //
    // Sorted by direction, interface type
    //
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
#endif /* SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1 */

#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)
        {RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER,         RULE_TYPE_INBOUND},
#endif // SYS_CPNT_QOS_V2_ALL_PORTS

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        {1,                                             RULE_TYPE_OUTBOUND},
        {2,                                             RULE_TYPE_OUTBOUND},
        {3,                                             RULE_TYPE_OUTBOUND},
#if (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1)
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 1,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 2,  RULE_TYPE_OUTBOUND},
        {SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP + 3,  RULE_TYPE_OUTBOUND},
#endif /* SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT > 1 */
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
        {SYS_ADPT_CPU_1_IF_INDEX_NUMBER,                RULE_TYPE_OUTBOUND},
#endif // SYS_CPNT_QOS_V2_CPU_PORT

    };

    for (i = 0; i < _countof(acls); ++ i)
    {
        char acl_name[30];

        snprintf(acl_name, sizeof(acl_name), "%lu", i);
        acl_name[sizeof(acl_name)-1] = '\0';

        RULE_MGR_UT_Proc_Create_ACL(acl_name, acls[i].acl_type, MAX_ACE, RULE_CTRL_UT_COMPRESS_DEFAULT);
        result = RULE_MGR_GetAclIdByName(acl_name, &acls[i].acl_index);
        assert(RULE_TYPE_OK == result);

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_ENABLE;

            RULE_MGR_UT_Proc_Bind_ACL(ifindex, direction, acl_name, NULL, counter_enable);
        }
    }

    {
        typedef struct
        {
            UI32_T ifindex;
            RULE_TYPE_InOutDirection_T direction;
            UI32_T acl_index;
            UI32_T ace_index;
        } index_key_t;

        RULE_TYPE_Ace_Entry_T test_ace_entry;

        index_key_t index_key;

        index_key.ifindex = 0;

        for (pi = 0; pi < _countof(port_list); ++ pi)
        {
            UI32_T ifindex = port_list[pi].ifindex;
            RULE_TYPE_InOutDirection_T direction = port_list[pi].direction;

            for (i = 0; i < _countof(acls); ++ i)
            {
                result = L4_MGR_QoS_GetNextDiffServAclHwCounterEntry(&index_key.ifindex, &index_key.direction,
                                                                     &index_key.acl_index,
                                                                     &index_key.ace_index,
                                                                     &test_ace_entry);
                assert(RULE_TYPE_OK == result);

                assert(index_key.ifindex == ifindex);
                assert(index_key.direction == direction);
                assert(index_key.acl_index == acls[i].acl_index);

                {
                    index_key_t tmp_index_key[2]; // [0]: current key, [1]: previous key

                    UI32_T ace_count = 1;

                    memcpy(&tmp_index_key[0], &index_key, sizeof(tmp_index_key[0]));
                    memcpy(&tmp_index_key[1], &index_key, sizeof(tmp_index_key[0]));

                    while (RULE_TYPE_OK == L4_MGR_QoS_GetNextDiffServAclHwCounterEntry(&tmp_index_key[0].ifindex, &tmp_index_key[0].direction,
                                                                                       &tmp_index_key[0].acl_index,
                                                                                       &tmp_index_key[0].ace_index,
                                                                                       &test_ace_entry))
                    {
                        if (tmp_index_key[0].ifindex != index_key.ifindex ||
                            tmp_index_key[0].direction != index_key.direction ||
                            tmp_index_key[0].acl_index != index_key.acl_index)
                        {
                            memcpy(&index_key, &tmp_index_key[1], sizeof(index_key));
                            break;
                        }

                        memcpy(&tmp_index_key[1], &tmp_index_key[0], sizeof(tmp_index_key[1]));
                        ++ ace_count;

                        {
                            RULE_TYPE_Ace_Entry_T tmp_ace_entry;
                            result = L4_MGR_QoS_GetDiffServAclHwCounterEntry(tmp_index_key[0].ifindex, tmp_index_key[0].direction,
                                                                             tmp_index_key[0].acl_index,
                                                                             tmp_index_key[0].ace_index,
                                                                             &tmp_ace_entry);
                            assert(RULE_TYPE_OK == result);

                            //
                            // Not compare all because the 'last_update_tick' will not same.
                            //
                            assert(test_ace_entry.ace_type == tmp_ace_entry.ace_type);
                            assert(0 == memcmp(&test_ace_entry.u, &tmp_ace_entry.u, sizeof(test_ace_entry.u)));
                        }
                    }

                    //
                    // Check ACE count
                    //
                    {
                        UI32_T tmp_acl_index = acls[i].acl_index;

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
                        {
                            RULE_TYPE_Acl_T *tmp_acl_entry_p;

                            tmp_acl_entry_p = RULE_OM_LocalGetAclByIndex(tmp_acl_index);
                            assert(NULL != tmp_acl_entry_p);

                            tmp_acl_index = tmp_acl_entry_p->aggregate_acl_index;
                        }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

                        {
                            RULE_TYPE_Acl_T *tmp_acl_entry_p;
                            tmp_acl_entry_p = RULE_OM_LocalGetAclByIndex(tmp_acl_index);
                            assert(NULL != tmp_acl_entry_p);

                            assert(tmp_acl_entry_p->ace_count == ace_count);
                        }
                    }
                }
            }
        }
    }

#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

static int
L4_MGR_UT_QoS_GetDiffServPortEntry()
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i = 1;

    UI32_T pl_idx;
    UI32_T ifindex;

    RULE_TYPE_PortEntry_T port_entry;

    memset(&port_entry, 0, sizeof(port_entry));

    {
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
        RULE_TYPE_AclType_T port_acl_type;

        char acl_name[20];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);

        result = L4_MGR_ACL_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalAclTypeToPortAclType(acl_type, &port_acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalGetAclIdByName(acl_name, &port_entry.bind_acl[port_acl_type].acl_index);
        assert(RULE_TYPE_OK == result);

        /* FIXME: Not support trunk port now,
         *        broadcom code will check is trunk on RULE_CTRL_SetAcl()
         */
        for (pl_idx = 0; pl_idx < SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER - 1; ++ pl_idx)
        {
            UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

            if (FALSE == SWCTRL_LogicalPortExisting(ifindex))
            {
                continue;
            }

            result = RULE_MGR_BindPort2Acl(ifindex,
                                           port_entry.bind_acl[port_acl_type].acl_index,
                                           port_acl_type,
                                           TRUE,
                                           time_range_name,
                                           counter_enable);

            assert(RULE_TYPE_OK == result);
        }
    }

    //
    // Verification
    //
    for (pl_idx = 0; pl_idx < SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER - 1; ++ pl_idx)
    {
        RULE_TYPE_UI_PortEntry_T ui_port_entry;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);
        if (FALSE == SWCTRL_LogicalPortExisting(ifindex))
        {
            result = L4_MGR_QoS_GetDiffServPortEntry(ifindex, RULE_TYPE_INBOUND, &ui_port_entry);
            assert(RULE_TYPE_OK != result);

            continue;
        }

        result = L4_MGR_QoS_GetDiffServPortEntry(ifindex, RULE_TYPE_INBOUND, &ui_port_entry);
        assert(RULE_TYPE_OK == result);

        assert(ui_port_entry.ingress_mac_acl_index == port_entry.bind_acl[RULE_TYPE_MAC_ACL].acl_index);
        assert(ui_port_entry.ingress_ip_acl_index == port_entry.bind_acl[RULE_TYPE_IP_ACL].acl_index);
        assert(ui_port_entry.ingress_ipv6_acl_index == port_entry.bind_acl[RULE_TYPE_IPV6_ACL].acl_index);
    }
    
    return 0;
}


int
L4_MGR_UT_QoS_GetNextDiffServPortEntry()
{
    UI32_T pl_idx, ifindex;
    UI32_T next_ifindex = 0;
    RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;
    RULE_TYPE_InOutDirection_T next_direction = RULE_TYPE_INBOUND;
    RULE_TYPE_RETURN_TYPE_T result;

    for (pl_idx = 0; pl_idx < RULE_TYPE_TOTAL_NBR_OF_PORT; ++ pl_idx)
    {
        RULE_TYPE_UI_PortEntry_T ui_port_entry;
        RULE_TYPE_UI_PortEntry_T next_port_entry;

        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        if ((ifindex <= SYS_ADPT_TOTAL_NBR_OF_LPORT)
            && (FALSE == SWCTRL_LogicalPortExisting(ifindex)))
        {
            /* not existed user port and trunk port
             */
            continue;
        }

        if (FALSE == RULE_OM_LocalIsValidIfindex(ifindex, direction))
        {
            continue;
        }

        result = L4_MGR_QoS_GetDiffServPortEntry(ifindex, direction, &ui_port_entry);
        assert(RULE_TYPE_OK == result);

        result = L4_MGR_QoS_GetNextDiffServPortEntry(&next_ifindex, &next_direction, &next_port_entry);
        assert(RULE_TYPE_OK == result);
        
        assert(ifindex == next_ifindex);
        assert(0 == memcmp(&ui_port_entry, &next_port_entry, sizeof(ui_port_entry)));
    }

    return 0;
}

static int
L4_MGR_UT_QoS_GetNextAccessGroupEntry()
{
    RULE_TYPE_RETURN_TYPE_T result;

    UI32_T i = 1;

    UI32_T pl_idx;
    UI32_T ifindex;

    RULE_TYPE_PortEntry_T port_entry;
    RULE_TYPE_MIB_AccessGroupEntry_T entry;
    RULE_TYPE_MIB_AccessGroupEntry_T next_entry;

    memset(&port_entry, 0, sizeof(port_entry));

    {
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
        RULE_TYPE_AclType_T port_acl_type;

        char acl_name[20];

        snprintf(acl_name, sizeof(acl_name), "a%lu", i);

        result = L4_MGR_ACL_CreateAcl(acl_name, acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalAclTypeToPortAclType(acl_type, &port_acl_type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_LocalGetAclIdByName(acl_name, &port_entry.bind_acl[port_acl_type].acl_index);
        assert(RULE_TYPE_OK == result);

        /* FIXME: Not support trunk port now,
         *        broadcom code will check is trunk on RULE_CTRL_SetAcl()
         */
        for (pl_idx = 0; pl_idx < SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER - 1; ++ pl_idx)
        {
            UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
            RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

            ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

            if (FALSE == SWCTRL_LogicalPortExisting(ifindex))
            {
                continue;
            }

            result = RULE_MGR_BindPort2Acl(ifindex,
                                           port_entry.bind_acl[port_acl_type].acl_index,
                                           port_acl_type,
                                           TRUE,
                                           time_range_name,
                                           counter_enable);

            assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
            result = RULE_MGR_BindPort2Acl(ifindex,
                                           port_entry.bind_acl[port_acl_type].acl_index,
                                           port_acl_type,
                                           FALSE,
                                           time_range_name,
                                           counter_enable);

            assert(RULE_TYPE_OK == result);
#endif /* (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE) */
        }
    }

    //
    // Verification
    //
    next_entry.ifindex = 0;

    for (pl_idx = 0; pl_idx < SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER - 1; ++ pl_idx)
    {
        ifindex = RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(pl_idx);

        memset(&entry, 0, sizeof(entry));
        entry.ifindex = ifindex;
        entry.type = VAL_diffServAccessGroupType_mac;
        entry.direction = VAL_diffServAccessGroupDirection_ingress;

        if (FALSE == SWCTRL_LogicalPortExisting(ifindex))
        {
            result = L4_MGR_QoS_GetAccessGroupEntry(&entry);
            assert(RULE_TYPE_OK != result);

            continue;
        }

        result = L4_MGR_QoS_GetAccessGroupEntry(&entry);
        assert(RULE_TYPE_OK == result);

        result = L4_MGR_QoS_GetNextAccessGroupEntry(&next_entry);
        assert(RULE_TYPE_OK == result);

        assert(0 == memcmp(&entry.ifindex, &next_entry.ifindex, sizeof(entry.ifindex)));
        assert(0 == memcmp(&entry.direction, &next_entry.direction, sizeof(entry.direction)));
        assert(0 == memcmp(&entry.type, &next_entry.type, sizeof(entry.type)));
        assert(0 == memcmp(&entry.acl_index, &next_entry.acl_index, sizeof(entry.acl_index)));
        //assert(0 == memcmp(&entry.time_range_name, &next_entry.time_range_name, sizeof(entry.time_range_name)));
        assert(0 == memcmp(&entry.counter_status, &next_entry.counter_status, sizeof(entry.counter_status)));
        assert(0 == memcmp(&entry.row_status, &next_entry.row_status, sizeof(entry.row_status)));
        // FIXME: Data structure alignmen issue
        // assert(0 == memcmp(&entry, &next_entry, sizeof(entry)));

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        entry.direction = VAL_diffServAccessGroupDirection_egress;
        result = L4_MGR_QoS_GetAccessGroupEntry(&entry);
        assert(RULE_TYPE_OK == result);

        result = L4_MGR_QoS_GetNextAccessGroupEntry(&next_entry);
        assert(RULE_TYPE_OK == result);

        // FIXME: Data structure alignmen issue
        // assert(0 == memcmp(&entry, &next_entry, sizeof(entry)));

        assert(0 == memcmp(&entry.ifindex, &next_entry.ifindex, sizeof(entry.ifindex)));
        assert(0 == memcmp(&entry.direction, &next_entry.direction, sizeof(entry.direction)));
        assert(0 == memcmp(&entry.type, &next_entry.type, sizeof(entry.type)));
        assert(0 == memcmp(&entry.acl_index, &next_entry.acl_index, sizeof(entry.acl_index)));
        //assert(0 == memcmp(&entry.time_range_name, &next_entry.time_range_name, sizeof(entry.time_range_name)));
        assert(0 == memcmp(&entry.counter_status, &next_entry.counter_status, sizeof(entry.counter_status)));
        assert(0 == memcmp(&entry.row_status, &next_entry.row_status, sizeof(entry.row_status)));


#endif /* (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE) */
    }
    
    return 0;
}


/*------------------------------------------------------------------------------
 * Entry Pointer
 *------------------------------------------------------------------------------
 */
int
L4_MGR_UT_RunTestCases()
{
#define L4_MGR_UT_TEST(func) \
    UT_Test(L4_MGR_UT_Setup, L4_MGR_UT_Teardown, func)

    L4_MGR_UT_TEST(L4_MGR_UT_Get_Next_DiffServ_Acl_Hw_Counter_Entry);

    L4_MGR_UT_TEST(L4_MGR_UT_QoS_GetDiffServPortEntry);
    L4_MGR_UT_TEST(L4_MGR_UT_QoS_GetNextDiffServPortEntry);
    L4_MGR_UT_TEST(L4_MGR_UT_QoS_GetNextAccessGroupEntry);

    return 0;

#undef L4_MGR_UT_TEST
}
