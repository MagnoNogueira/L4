/*
 *  cli_api_acl_qosv2_test_cases.c
 *  l4_mgr
 *
 *  Created by yehjunying on 2011/6/7.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

/*------------------------------------------------------------------------------
 * Help Functions
 *------------------------------------------------------------------------------
 */
void
CLI_ACL_UT_Proc_CreateOneAcl(
	const char *acl_name,
    RULE_TYPE_AclType_T acl_type)
{
    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T ace_count = 5;
//    RULE_TYPE_Ace_Entry_T ace_entry;
    UI32_T acl_index;

//    UI32_T inc = 0;

    assert(NULL != acl_name);

    RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(acl_name, acl_type, ace_count, RULE_CTRL_UT_COMPRESS_DEFAULT, &acl_index);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    result = RULE_OM_RebuildAggregateAcl(acl_index);
    assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

//	result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
//    assert(RULE_TYPE_OK == result);
//
//    result = L4_PMGR_ACL_GetAclIdByName(acl_name, &acl_index);
//    assert(RULE_TYPE_OK == result);
//
//    while (ace_count -- > 0)
//    {
//        ace_entry.ace_type = acl_type;
//        result = L4_PMGR_ACL_InitAce(&ace_entry);
//
//        assert(RULE_TYPE_OK == result);
//
//        switch (acl_type)
//        {
//            case RULE_TYPE_MAC_ACL:
//                ace_entry.u.mac.aceSourceMacAddr[5] = (UI8_T)inc++;
//                memset(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, 6);
//                break;
//
//            default:
//                assert(0);
//                break;
//        }
//
//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
//
//        assert(RULE_TYPE_OK == result);
//    }
//
//
//    ace_entry.ace_type = acl_type;
//    result = L4_PMGR_ACL_InitAce(&ace_entry);
//
//    assert(RULE_TYPE_OK == result);
//
//    RULE_OM_SetAccessTypeToAceEntry(&ace_entry, RULE_TYPE_ACE_DENY);
//    result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
//
//    assert(RULE_TYPE_OK == result);
//
}

void
CLI_ACL_UT_Proc_BindACLOnPort(
    UI32_T ifindex,
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_InOutDirection_T inout_profile,
    const char *time_range_name,
    RULE_TYPE_COUNTER_ENABLE_T counter_enable)
{
    UI32_T result;

    switch (acl_type)
    {
        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            acl_type = RULE_TYPE_IP_ACL;
            break;

        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            acl_type = RULE_TYPE_IPV6_ACL;
            break;

        default:
            break;
    }

    result = L4_PMGR_ACL_BindPort2Acl(ifindex, acl_name, acl_type,
                                      inout_profile== RULE_TYPE_INBOUND ? TRUE : FALSE,
                                      (char *)time_range_name,
                                      counter_enable);

    assert(RULE_TYPE_OK == result);
}

void
CLI_ACL_UT_Proc_Set_Counter(
    RULE_TYPE_INSTANCE_PTR_T in,
    void *cookie)
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)
    if (RULE_CTRL_IS_RULE_INSTANCE_TYPE(in->type))
    {
        RULE_TYPE_RULE_INSTANCE_PTR_T rule_inst_p;
        UI32_T greenCntr, yellowCntr, redCntr;

        rule_inst_p = (RULE_TYPE_RULE_INSTANCE_PTR_T)in;

        greenCntr   = 111; //rand() % 1000;
        yellowCntr  = 222; //rand() % 1000;
        redCntr     = 333; //rand() % 1000;

//        printf("set device = %hu, rule id = %lu\r\n",
//               rule_inst_p->dev_rule_info.device,
//               rule_inst_p->dev_rule_info.rule_id);

        RULE_CTRL_UT_Proc_Set_Billing_Counter(rule_inst_p->dev_rule_info.device,
                                              rule_inst_p->dev_rule_info.rule_id,
                                              greenCntr, yellowCntr, redCntr);
    }
#endif // SYS_CPNT_ACL_COUNTER
}

void
CLI_ACL_UT_Proc_Set_ACL_Counter(
    UI32_T ifindex,
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    RULE_TYPE_InOutDirection_T direction)
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_INTERFACE_INFO_T interface;

//    RULE_CTRL_IF_ENTRY_PTR_T if_entry_p;
//    RULE_TYPE_CLASS_INSTANCE_OBJECT_T if_class_obj;
//    RULE_CTRL_INSTANCE_TYPE_ID_T param;
    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;

    UI32_T acl_index;

    RULE_TYPE_Acl_T acl_entry;

    result = RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
    assert(RULE_TYPE_OK == result);

    interface.direction = direction;

    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
    assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    acl_index = acl_entry.aggregate_acl_index;

    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
    assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

//    if_entry_p = RULE_CTRL_Priv_GetIfEntry(&interface);
//    ASSERT(NULL != if_entry_p);
//
//    RULE_CTRL_IN_Priv_ConstructClassInstanceObjectFromPointer(if_entry_p->class_inst_ptr,
//                                                              &if_class_obj);
//
//    memset(&param, 0, sizeof(param));
//
//    param.type = RULE_TYPE_INST_ACL;
//    param.id = acl_index;
//
//    acl_inst_p = (RULE_TYPE_CLASS_INSTANCE_PTR_T)RULE_CTRL_Priv_GetFirstChildIf(if_class_obj.inst,
//                                                                                RULE_CTRL_Priv_IsClassInstanceMatchTypeId,
//                                                                                &param);
//    assert(NULL != acl_inst_p);
    acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, acl_index);
    assert(NULL != acl_inst_p);

    RULE_CTRL_IN_Priv_Walk((RULE_TYPE_INSTANCE_PTR_T)acl_inst_p, CLI_ACL_UT_Proc_Set_Counter, NULL);

#endif // SYS_CPNT_ACL_COUNTER
}

void
CLI_ACL_UT_Proc_Validate_ACLs_Mirror()
{
#if (SYS_CPNT_ACL_MIRROR == TRUE)

    UI32_T                      uport_ifindex;
    RULE_TYPE_InOutDirection_T  inout_profile;

    UI32_T                      acl_index;

    RULE_TYPE_UI_AclEntry_T     ui_acl_entry;
    UI32_T                      precedence;

    RULE_TYPE_COUNTER_ENABLE_T  counter_enable;
    char                        time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];


    for (uport_ifindex = 1; uport_ifindex <= SYS_ADPT_TOTAL_NBR_OF_LPORT;
                                                                ++uport_ifindex)
    {
        for (inout_profile = RULE_TYPE_INBOUND; inout_profile <= RULE_TYPE_OUTBOUND;
                                                                ++inout_profile)
        {
            acl_index = 0;
            precedence = 0;

            while (RULE_TYPE_OK == L4_PMGR_ACL_GetNextAclByPort(uport_ifindex,
                                                                inout_profile,
                                                                &acl_index,
                                                                &ui_acl_entry,
                                                                &precedence,
                                                                time_range_name,
                                                                &counter_enable) )
            {
                UI32_T                  ace_index;
                RULE_TYPE_Ace_Entry_T   ace_entry;
                RULE_TYPE_AceAction_T   ace_access;

                UI32_T                  hw_ace_index;
                RULE_TYPE_Ace_Entry_T   hw_ace_entry;
                RULE_TYPE_AceAction_T   hw_ace_access;

                RULE_TYPE_Acl_T         acl_entry;

                UI32_T                  result;

                ace_index = 0;
                hw_ace_index = 0;

                result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
                assert(RULE_TYPE_OK == result);

                while (RULE_TYPE_OK == L4_PMGR_ACL_GetNextAceByAcl(acl_index,
                                                                   &ace_index,
                                                                   &ace_entry))
                {
                    result = L4_PMGR_ACL_GetNextHardwareAceByAcl(uport_ifindex,
                                                                 inout_profile,
                                                                 acl_index,
                                                                 &hw_ace_index,
                                                                 &hw_ace_entry);
                    assert(RULE_TYPE_OK == result);
                    ace_access = ace_entry.access;
                    hw_ace_access = hw_ace_entry.access;

                    if (RULE_TYPE_INBOUND == inout_profile &&
                        RULE_TYPE_MIRROR_ENABLE == acl_entry.mirror.enable &&
                        ace_access == RULE_TYPE_ACE_PERMIT)
                    {
                        assert (hw_ace_access == RULE_TYPE_ACE_MIRROR_TO_ANALYZER_PORT);
                    }
                    else
                    {
                        assert(ace_access == hw_ace_access);
                    }

                }

                result = L4_PMGR_ACL_GetNextHardwareAceByAcl(uport_ifindex,
                                                             inout_profile,
                                                             acl_index,
                                                             &hw_ace_index,
                                                             &hw_ace_entry);
                assert(RULE_TYPE_OK != result);

            } // acl_index
        } // inout_profile
    } // uport_ifindex

#endif // SYS_CPNT_ACL_MIRROR
}

/*------------------------------------------------------------------------------
 * Test Functions
 *------------------------------------------------------------------------------
 */

int
CLI_ACL_UT_Setup()
{
    RULE_CTRL_UT_Setup();

    L4_MGR_InitiateSystemResources();
    L4_MGR_EnterMasterMode();

    return 0;
}

int
CLI_ACL_UT_Teardown()
{
    RULE_CTRL_UT_Teardown();
    return 0;
}

int
CLI_ACL_UT_String_Library()
{
    const char STRING[] = "1234567890";

    enum
    {
        MAX_BUFF_SIZE = 20,
        STRING_LENGTH = sizeof(STRING)-1
    };

    char buff[MAX_BUFF_SIZE] = {0};

    CLI_ACL_STRING_T string;

    CLI_ACL_STRING_RETURN_T result;
    int i_result;

    CLI_ACL_STRING_InitString(buff, sizeof(buff), &string);

    assert(buff == string.buff);
    assert(MAX_BUFF_SIZE == string.capacity);
    assert(0 == string.used);
    assert(string.buff == string.cur);


    //
    // Shift Test
    //
    {
        strcpy(buff, STRING);

        CLI_ACL_STRING_InitString(buff, sizeof(buff), &string);

        assert(buff == string.buff);
        assert(MAX_BUFF_SIZE == string.capacity);
        assert(STRING_LENGTH == string.used);
        assert('\0' == *string.cur);


        CLI_ACL_STRING_StrShift(&string, 1);

        assert(buff == string.buff);
        assert(MAX_BUFF_SIZE == string.capacity);
        assert(STRING_LENGTH-1 == string.used);
        assert('\0' == *string.cur);

        i_result = memcmp(string.buff, &STRING[1], STRING_LENGTH-1);
        assert(0 == i_result);

        CLI_ACL_STRING_StrShift(&string, STRING_LENGTH);
        assert(buff == string.buff);
        assert(MAX_BUFF_SIZE == string.capacity);
        assert(0 == string.used);
        assert('\0' == *string.cur);

        assert('\0' == string.buff[0]);
        assert(string.buff == string.cur);
    }

    //
    // Concatenate Test
    //
    {
        buff[0] = '\0';

        CLI_ACL_STRING_InitString(buff, sizeof(buff), &string);

        assert(buff == string.buff);
        assert(MAX_BUFF_SIZE == string.capacity);
        assert(0 == string.used);
        assert(string.buff == string.cur);

        CLI_ACL_STRING_StrCat(&string, STRING);

        assert(buff == string.buff);
        assert(MAX_BUFF_SIZE == string.capacity);
        assert(STRING_LENGTH == string.used);
        assert('\0' == *string.cur);

        i_result = strcmp(buff, STRING);
        assert(0 == i_result);


        //
        // off-1
        //
        {
            char buff_2[sizeof(buff)];

            memset(buff, 0, sizeof(buff));
            memset(buff, 'b', sizeof(buff)-1);

            CLI_ACL_STRING_InitString(buff, sizeof(buff), &string);

            assert((sizeof(buff)-1) == string.used);
            assert(string.buff+(sizeof(buff)-1) == string.cur);
            assert('\0' == *string.cur);

            memcpy(buff_2, buff, sizeof(buff));

            result = CLI_ACL_STRING_StrCat(&string, "c");

            assert(CLI_ACL_STRING_E_INSUFFICIENT_BUFFER == result);

            assert((sizeof(buff)-1) == string.used);
            assert(string.buff+(sizeof(buff)-1) == string.cur);
            assert('\0' == *string.cur);

            i_result = strcmp(buff_2, buff);
            assert(0 == i_result);
        }

        //
        // off-2
        //
        {
            char buff_2[sizeof(buff)];

            memset(buff, 0, sizeof(buff));
            memset(buff, 'b', sizeof(buff)-2);

            CLI_ACL_STRING_InitString(buff, sizeof(buff), &string);

            assert((sizeof(buff)-2) == string.used);
            assert(string.buff+(sizeof(buff)-2) == string.cur);
            assert('\0' == *string.cur);

            memcpy(buff_2, buff, sizeof(buff));

            result = CLI_ACL_STRING_StrCat(&string, "cc");

            assert(CLI_ACL_STRING_E_INSUFFICIENT_BUFFER == result);

            assert((sizeof(buff)-2) == string.used);
            assert(string.buff+(sizeof(buff)-2) == string.cur);
            assert('\0' == *string.cur);


            i_result = strcmp(buff_2, buff);
            assert(0 == i_result);


            buff_2[sizeof(buff_2)-2] = 'c';

            result = CLI_ACL_STRING_StrCat(&string, "c");
            assert(CLI_ACL_STRING_OK == result);

            i_result = strcmp(buff_2, buff);
            assert(0 == i_result);
        }
    }

    //
    // Concatenate At Head
    //
    {
        const char NEW_STRING[] = "abcd";

        enum
        {
            NEW_STRING_LEN = sizeof(NEW_STRING)-1
        };

        buff[0] = '\0';

        CLI_ACL_STRING_InitString(buff, sizeof(buff), &string);

        assert(buff == string.buff);
        assert(MAX_BUFF_SIZE == string.capacity);
        assert(0 == string.used);
        assert(string.buff == string.cur);

        CLI_ACL_STRING_StrCat(&string, STRING);

        assert(buff == string.buff);
        assert(MAX_BUFF_SIZE == string.capacity);
        assert(STRING_LENGTH == string.used);
        assert('\0' == *string.cur);

        i_result = strcmp(buff, STRING);
        assert(0 == i_result);

        {
            char buff_2[sizeof(buff)];

            memset(buff_2, 0, sizeof(buff_2));
            memcpy(buff_2, NEW_STRING, NEW_STRING_LEN);
            memcpy(&buff_2[NEW_STRING_LEN], buff, strlen(buff));

            result = CLI_ACL_STRING_StrCatAtHead(&string, NEW_STRING);
            assert(CLI_ACL_STRING_OK == result);

            i_result = strcmp(buff_2, buff);
            assert(0 == i_result);

            assert(STRING_LENGTH + NEW_STRING_LEN == string.used);
            assert('\0' == *string.cur);
            assert(string.buff + string.used == string.cur);
        }
    }


    return 0;
}

int
CLI_ACL_UT_GetMacAddr()
{
    typedef struct
    {
        char    *arg[256];
        UI32_T  addr_cmd_length;
        UI8_T   addr[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T   bitmask[SYS_ADPT_MAC_ADDR_LEN];
    } CMD_T;
    
    
    CMD_T  cmd[] =
    {
        {
            "any",
            .addr_cmd_length = 1,
            .addr = {0},
            .bitmask = {0},
        },
        {
            "host",
            "000000000001",
            .addr_cmd_length = 2,
            .addr    = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
            .bitmask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
            
        },
        {
            "000000000002",
            "00000000000f",
            .addr_cmd_length = 2,
            .addr    = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
            .bitmask = {0x00, 0x00, 0x00, 0x00, 0x00, 0x0f}
        }
    };
    
    
    unsigned int i;
    
    for (i = 0; i  < _countof(cmd); i++)
    {
        CMD_T *cmd_p = &cmd[i];
        UI8_T addr[SYS_ADPT_MAC_ADDR_LEN] = {0};
        UI8_T bitmask[SYS_ADPT_MAC_ADDR_LEN] = {0};
        UI32_T position = 0;
        UI32_T result;
        
        result = CLI_ACL_PrivGetMacAddr(cmd_p->arg, &position, addr, bitmask);
        assert(result == CLI_NO_ERROR);
        assert(position == cmd_p->addr_cmd_length);
        assert(0 == (memcmp(addr, cmd_p->addr, sizeof(addr))));
        assert(0 == (memcmp(bitmask, cmd_p->bitmask, sizeof(bitmask))));
    }
    
    return 0;
}

int
CLI_ACL_UT_GetIpAddr()
{
    typedef struct
    {
        char *arg[256];
        UI32_T  addr_start_position;
        UI32_T  addr_cmd_length;
        UI8_T  addr[SYS_ADPT_IPV4_ADDR_LEN];
        UI8_T  bitmask[SYS_ADPT_IPV4_ADDR_LEN];
    } CMD_T;
    
    
    CMD_T  cmd[] =
    {
        {
            "any",
            .addr_start_position = 0,
            .addr_cmd_length = 1,
            .addr = 0,
            .bitmask = 0,
        },
        {
            "host",
            "192.168.0.1",
            .addr_start_position = 0,
            .addr_cmd_length = 2,
            .addr = {192,168,0,1},
            .bitmask = {255,255,255,255}
        },
        {
            "192.168.0.3",
            "255.255.255.255",
            .addr_start_position = 0,
            .addr_cmd_length = 2,
            .addr = {192,168,0,3},
            .bitmask = {255,255,255,255}
        },
        {
            "192.168.0.3",
            "255.255.255.255",
            "time-range",
            "t1",
            .addr_start_position = 0,
            .addr_cmd_length = 2,
            .addr = {192,168,0,3},
            .bitmask ={255,255,255,255}
        },
        {
            "TCP",
            "192.168.0.3",
            "255.255.255.255",
            "time-range",
            "t1",
            .addr_start_position = 1,
            .addr_cmd_length = 2,
            .addr = {192,168,0,3},
            .bitmask = {255,255,255,255}
        },
        
    };
    
    unsigned int i;
    
    for (i = 0; i  < _countof(cmd); i++)
    {
        CMD_T *cmd_p = &cmd[i];
        UI32_T position = cmd_p->addr_start_position;
        UI8_T addr[SYS_ADPT_IPV4_ADDR_LEN] = {0};
        UI8_T bitmask[SYS_ADPT_IPV4_ADDR_LEN] = {0};
        UI32_T result;
    
        result = CLI_ACL_PrivGetIpAddr(cmd_p->arg, &position, addr, bitmask);
        assert(result == CLI_NO_ERROR);
        assert((position - cmd_p->addr_start_position) == cmd_p->addr_cmd_length);
        assert(0 == memcmp(addr, cmd_p->addr, sizeof(addr)));
        assert(0 == memcmp(bitmask, cmd_p->bitmask, sizeof(bitmask)));
    }
    
    return 0;
}

static void
CLI_ACL_UT_FillMac_Success()
{
    typedef struct
    {
        char *arg[256];
        UI8_T  macDAddr[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T  macDABitmask[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T  macSAddr[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T  macSABitmask[SYS_ADPT_MAC_ADDR_LEN];
        UI32_T vidOp;
        UI32_T vid;
        UI32_T vidMask;
        UI32_T ethertypeOp;
        UI32_T ethertype;
        UI32_T ethertypeMask;
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
        UI32_T sIp;
        UI32_T sIpMask;
        UI32_T dIP;
        UI32_T dIPMask;
        UI32_T l4_sPortOp;
        UI32_T l4_sPort;
        UI32_T l4_sPortMask;
        UI32_T l4_dPortOp;
        UI32_T l4_dPort;
        UI32_T l4_dPortMask;
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
    } CMD_T;
    
    CMD_T cmd[] =
    {
        {
            "any",
            "any",
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
            "ip",
            "host",
            "192.168.0.1",
            "any",
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
            .macDAddr = {0},
            .macDABitmask = {0},
            .macSAddr = {0},
            .macSABitmask = {0},
            .vidOp = VAL_aclMacAceVidOp_noOperator,
            .vid = 0,
            .vidMask = 0,
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
            .ethertypeOp = VAL_diffServMacAceEtherTypeOp_equal,
            .ethertype = 0x0800,
            .ethertypeMask = 0xffff,
            .sIp = 0xc0a80001,
            .sIpMask = 0xffffffff,
            .dIP = 0,
            .dIPMask = 0,
            .l4_sPortOp = VAL_diffServIpAceSourcePortOp_noOperator,
            .l4_sPort = 0,
            .l4_sPortMask = 0,
            .l4_dPortOp = VAL_aclIpAceDestPortOp_noOperator,
            .l4_dPort = 0,
            .l4_dPortMask = 0,
#else 
            .ethertypeOp = VAL_diffServMacAceEtherTypeOp_noOperator,
            .ethertype = 0,
            .ethertypeMask = 0,
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
        },
        {
            "any",
            "any",
            "vid",
            "1",
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
            "ip",
            "any",
            "any",
            "l4-source-port",
            "1",
            "l4-destination-port",
            "2",
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
            .macDAddr = {0},
            .macDABitmask = {0},
            .macSAddr = {0},
            .macSABitmask = {0},
            .vidOp = VAL_aclMacAceVidOp_equal,
            .vid = 1,
            .vidMask = MAX_aclMacAceVidBitmask,
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
            .ethertypeOp = VAL_diffServMacAceEtherTypeOp_equal,
            .ethertype = 0x0800,
            .ethertypeMask = 0xffff,
            .sIp = 0,
            .sIpMask = 0,
            .dIP = 0,
            .dIPMask = 0,
            .l4_sPortOp = VAL_diffServIpAceSourcePortOp_equal,
            .l4_sPort = 1,
            .l4_sPortMask = 65535,
            .l4_dPortOp = VAL_aclIpAceDestPortOp_equal,
            .l4_dPort = 2,
            .l4_dPortMask = 65535,
#else 
            .ethertypeOp = VAL_diffServMacAceEtherTypeOp_noOperator,
            .ethertype = 0,
            .ethertypeMask = 0,
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
        },
        {
            "any",
            "any",
            "ethertype",
            "0800",
            "fff0",
            .macDAddr = {0},
            .macDABitmask = {0},
            .macSAddr = {0},
            .macSABitmask = {0},
            .vidOp = VAL_aclMacAceVidOp_noOperator,
            .vid = 0,
            .vidMask = 0,
            .ethertypeOp = VAL_diffServMacAceEtherTypeOp_equal,
            .ethertype = 0x0800,
            .ethertypeMask = 0xfff0,
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
            .sIp = 0,
            .sIpMask = 0,
            .dIP = 0,
            .dIPMask = 0,
            .l4_sPortOp = VAL_diffServIpAceSourcePortOp_noOperator,
            .l4_sPort = 0,
            .l4_sPortMask = 0,
            .l4_dPortOp = VAL_aclIpAceDestPortOp_noOperator,
            .l4_dPort = 0,
            .l4_dPortMask = 0
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
        },
    };
    
    unsigned int i;

    for (i = 0; i  < _countof(cmd); i++)
    {
        CMD_T *cmd_p = &cmd[i];
        UI32_T result;
        RULE_TYPE_UI_Ace_Entry_T ui_ace_entry;
        RULE_TYPE_RETURN_TYPE_T rule_result;
        
        ui_ace_entry.ace_type = RULE_TYPE_MAC_ACL;

        rule_result = L4_MGR_ACL_InitUIAce(&ui_ace_entry);
        assert(rule_result == RULE_TYPE_OK);
        
        result = CLI_ACL_FillMAC(&ui_ace_entry, cmd_p->arg);
        assert(result == CLI_NO_ERROR);

        assert(0 == memcmp(cmd_p->macSAddr, ui_ace_entry.ether.sa.addr,
                           sizeof(cmd_p->macSAddr)));
        assert(0 == memcmp(cmd_p->macSABitmask, ui_ace_entry.ether.sa.mask,
                           sizeof(cmd_p->macSABitmask)));
        assert(0 == memcmp(cmd_p->macDAddr, ui_ace_entry.ether.da.addr,
                           sizeof(cmd_p->macDAddr)));
        assert(0 == memcmp(cmd_p->macDABitmask, ui_ace_entry.ether.da.mask,
                           sizeof(cmd_p->macDABitmask)));

        assert(cmd_p->ethertypeOp == ui_ace_entry.ether.ethertype.op);
        assert(cmd_p->ethertypeMask == ui_ace_entry.ether.ethertype.u.s.mask);
        assert(cmd_p->ethertype == ui_ace_entry.ether.ethertype.u.s.data);
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
        assert(cmd_p->vidOp == ui_ace_entry.ether.vid.op);
        assert(cmd_p->vid == ui_ace_entry.ether.vid.u.s.data);
        assert(cmd_p->vidMask == ui_ace_entry.ether.vid.u.s.mask);
        assert(cmd_p->l4_sPortOp  == ui_ace_entry.l4_common.sport.op);
        assert(cmd_p->l4_sPort  == ui_ace_entry.l4_common.sport.u.s.data);
        assert(cmd_p->l4_sPortMask  == ui_ace_entry.l4_common.sport.u.s.mask);
        assert(cmd_p->l4_dPortOp == ui_ace_entry.l4_common.dport.op);
        assert(cmd_p->l4_dPort == ui_ace_entry.l4_common.dport.u.s.data);
        assert(cmd_p->l4_dPortMask == ui_ace_entry.l4_common.dport.u.s.mask);
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
    }
    
}

static void
CLI_ACL_UT_FillMac_Fail()
{
    typedef struct
    {
        char *arg[256];
        UI8_T  macDAddr[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T  macDABitmask[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T  macSAddr[SYS_ADPT_MAC_ADDR_LEN];
        UI8_T  macSABitmask[SYS_ADPT_MAC_ADDR_LEN];
        UI32_T vidOp;
        UI32_T vid;
        UI32_T vidMask;
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
        UI32_T sIp;
        UI32_T sIpMask;
        UI32_T dIP;
        UI32_T dIPMask;
        UI32_T l4_sPortOp;
        UI32_T l4_sPort;
        UI32_T l4_sPortMask;
        UI32_T l4_dPortOp;
        UI32_T l4_dPort;
        UI32_T l4_dPortMask;
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
    } CMD_T;
    
    CMD_T cmd[] =
    {
        {
            "any",
            "vid",
            "1"
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
        },
        {
            "any",
            "any",
            "vid",
            "1",
            "ip",
            "any",
            "any",
            "l4-source-port",
            "1",
            "l4-destination-port",
            "2",
            "ethertype",
            "0800"
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
        }

    };
    
    unsigned int i;
    
    for (i = 0; i  < _countof(cmd); i++)
    {
        CMD_T *cmd_p = &cmd[i];
        UI32_T result;
        RULE_TYPE_UI_Ace_Entry_T ui_ace_entry;
        RULE_TYPE_RETURN_TYPE_T rule_result;
        
        ui_ace_entry.ace_type = RULE_TYPE_MAC_ACL;
        rule_result = L4_MGR_ACL_InitUIAce(&ui_ace_entry);
        assert(rule_result == RULE_TYPE_OK);
        
        result = CLI_ACL_FillMAC(&ui_ace_entry, cmd_p->arg);
        assert(result != CLI_NO_ERROR);
    }
}

int
CLI_ACL_UT_FillMac()
{

    CLI_ACL_UT_FillMac_Success();
    CLI_ACL_UT_FillMac_Fail();
    return 0;
}

int
CLI_ACL_UT_FillAclStandar()
{
    typedef struct
    {
        char *arg[256];
        UI8_T ip[SYS_ADPT_IPV4_ADDR_LEN];
        UI8_T mask[SYS_ADPT_IPV4_ADDR_LEN];
    } CMD_T;
    
    unsigned int i;
    
    CMD_T cmd[] =
    {
        {
            "any",
            .ip = {0,0,0,0},
            .mask = {0,0,0,0}
        },
        {
            "host",
            "192.168.1.1",
            .ip = {192,168,1,1},
            .mask = {255,255,255,255}
        },
        {
            "192.168.1.1",
            "255.255.255.0",
            .ip = {192,168,1,1},
            .mask = {255,255,255,0}
        }
    };
    
    for (i = 0; i < _countof(cmd); i++)
    {
        UI32_T result;
        CMD_T *cmd_p = &cmd[i];
        
        RULE_TYPE_UI_Ace_Entry_T ui_ace;
        
        ui_ace.ace_type = RULE_TYPE_IP_STD_ACL;
        
        L4_PMGR_ACL_InitUIAce(&ui_ace);
        
        result = CLI_ACL_FillStandard(&ui_ace, cmd_p->arg);
        assert(result == CLI_NO_ERROR);
        
        assert(0 == memcmp(ui_ace.ipv4.sip.addr, cmd_p->ip, SYS_ADPT_IPV4_ADDR_LEN));
        assert(0 == memcmp(ui_ace.ipv4.sip.mask, cmd_p->mask, SYS_ADPT_IPV4_ADDR_LEN));
    }
    
    return 0;
}

int
CLI_ACL_UT_FillAclExtend()
{
    typedef struct
    {
        char *arg[256];
        RULE_TYPE_UI_Ace_Entry_T ui_ace;
    } CMD_T;
    
    unsigned int i;
    
    CMD_T cmd[] =
    {
        {
            "any",
            "any",
            .ui_ace =
            {
                .ipv4 =
                {
                    .sip.addr = {0,0,0,0},
                    .sip.mask = {0,0,0,0},
                    .dip.addr = {0,0,0,0},
                    .dip.mask = {0,0,0,0},
                    .protocol.op = RULE_TYPE_IPV4_PROTOCOL_OP_NO_OPERATION,
                    .protocol.u.s.data = 0,
                    .protocol.u.s.mask = 0,
                    .dscp.op = RULE_TYPE_IPV4_DSCP_OP_NO_OPERATION,
                },
                .l4_common =
                {
                    .sport.op = VAL_diffServIpAceSourcePortOp_noOperator,
                    .sport.u.s.data = 0,
                    .sport.u.s.mask = 0,
                    .dport.op =VAL_diffServIpAceSourcePortOp_noOperator,
                    .dport.u.s.data = 0,
                    .dport.u.s.mask = 0,
                },
                .icmp.icmp_type =
                {
                    .op = RULE_TYPE_ICMP_ICMPTYPE_OP_NO_OPERATION,
                    .u.s.data = 0,
                    .u.s.mask = 0,
                },
                .tcp.flags =
                {
                    .data.u.code = RULE_TYPE_UNDEF_IP_CONTROL_CODE,
                    .mask.u.code = 0,
                }
            }
        },
        {
            "host",
            "192.168.1.1",
            "any",
            .ui_ace =
            {
                .ipv4 =
                {
                    .sip.addr = {192,168,1,1},
                    .sip.mask = {255,255,255,255},
                    .dip.addr = {0,0,0,0},
                    .dip.mask = {0,0,0,0},
                    .protocol.op = RULE_TYPE_IPV4_PROTOCOL_OP_NO_OPERATION,
                    .protocol.u.s.data = 0,
                    .protocol.u.s.mask = 0,
                    .dscp.op = RULE_TYPE_IPV4_DSCP_OP_NO_OPERATION,
                },
                .l4_common =
                {
                    .sport.op = VAL_diffServIpAceSourcePortOp_noOperator,
                    .sport.u.s.data = 0,
                    .sport.u.s.mask = 0,
                    .dport.op =VAL_diffServIpAceSourcePortOp_noOperator,
                    .dport.u.s.data = 0,
                    .dport.u.s.mask = 0,
                },
                .icmp.icmp_type =
                {
                    .op = RULE_TYPE_ICMP_ICMPTYPE_OP_NO_OPERATION,
                    .u.s.data = 0,
                    .u.s.mask = 0,
                },
                .tcp.flags =
                {
                    .data.u.code = RULE_TYPE_UNDEF_IP_CONTROL_CODE,
                    .mask.u.code = 0,
                }
            }
        },
        {
            "123",
            "192.168.1.1",
            "255.255.255.0",
            "any",
            "precedence",
            "7",
            "source-port",
            "1",
            "1",
            "destination-port",
            "2",
            "2",
            .ui_ace =
            {
                .ipv4 =
                {
                    .sip.addr = {192,168,1,1},
                    .sip.mask = {255,255,255,0},
                    .dip.addr = {0,0,0,0},
                    .dip.mask = {0,0,0,0},
                    .protocol.op = RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL,
                    .protocol.u.s.data = 123,
                    .protocol.u.s.mask = 255,
                    .dscp.op = RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE,
                    .dscp.u.tos.precedence = 7,
                },
                .l4_common =
                {
                    .sport.op = VAL_diffServIpAceSourcePortOp_equal,
                    .sport.u.s.data = 1,
                    .sport.u.s.mask = 1,
                    .dport.op = VAL_diffServIpAceSourcePortOp_equal,
                    .dport.u.s.data = 2,
                    .dport.u.s.mask = 2,
                },
                .icmp.icmp_type =
                {
                    .op = RULE_TYPE_ICMP_ICMPTYPE_OP_NO_OPERATION,
                    .u.s.data = 0,
                    .u.s.mask = 0,
                },
                .tcp.flags =
                {
                    .data.u.code = RULE_TYPE_UNDEF_IP_CONTROL_CODE,
                    .mask.u.code = 0,
                }
            }
        }
    };
    
    for (i = 0; i < _countof(cmd); i++)
    {
        UI32_T result;
        CMD_T *cmd_p = &cmd[i];
        
        RULE_TYPE_UI_Ace_Entry_T ui_ace;
        
        ui_ace.ace_type = RULE_TYPE_IP_EXT_ACL;
        
        L4_PMGR_ACL_InitUIAce(&ui_ace);
        
        result = CLI_ACL_FillExtended(&ui_ace, cmd_p->arg);
        assert(result == CLI_NO_ERROR);
        
        assert(0 == memcmp(&ui_ace.ipv4.sip,
                           &cmd_p->ui_ace.ipv4.sip,
                           sizeof(ui_ace.ipv4.sip)));
        assert(0 == memcmp(&ui_ace.ipv4.dip,
                           &cmd_p->ui_ace.ipv4.dip,
                           sizeof(ui_ace.ipv4.dip)));
        
        assert(ui_ace.ipv4.dscp.op == cmd_p->ui_ace.ipv4.dscp.op);
        switch (cmd_p->ui_ace.ipv4.dscp.op)
        {
            case RULE_TYPE_IPV4_DSCP_OP_DSCP:
                assert(ui_ace.ipv4.dscp.u.ds == cmd_p->ui_ace.ipv4.dscp.u.ds);
                break;
            case RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE:
                assert(ui_ace.ipv4.dscp.u.tos.precedence == cmd_p->ui_ace.ipv4.dscp.u.tos.precedence);
                break;
            case RULE_TYPE_IPV4_DSCP_OP_TOS:
                assert(ui_ace.ipv4.dscp.u.tos.tos == cmd_p->ui_ace.ipv4.dscp.u.tos.tos);
                break;
            default:
                break;
        }

        assert(0 == memcmp(&ui_ace.icmp,
                           &cmd_p->ui_ace.icmp,
                           sizeof(ui_ace.icmp)));

        assert(ui_ace.tcp.flags.mask.u.code == cmd_p->ui_ace.tcp.flags.mask.u.code);
        if (cmd_p->ui_ace.tcp.flags.mask.u.code != 0)
        {
            assert(ui_ace.tcp.flags.data.u.code == cmd_p->ui_ace.tcp.flags.data.u.code);
        }
        
        assert(0 == memcmp(&ui_ace.l4_common.sport,
                           &cmd_p->ui_ace.l4_common.sport,
                           sizeof(ui_ace.l4_common.sport)));

        assert(0 == memcmp(&ui_ace.l4_common.dport,
                           &cmd_p->ui_ace.l4_common.dport,
                           sizeof(ui_ace.l4_common.dport)));        
    }
    
    return 0;
}

int
CLI_ACL_UT_SET_ACE_MAC()
{
    typedef struct
    {
        char *arg[256];
        UI32_T action;
        UI32_T cmd_index;
        UI32_T cmd_index_no;
        //RULE_TYPE_UI_Ace_Entry_T ui_ace;
    } CMD_T;
    
    CMD_T cmd[] =
    {
        {
            "any",
            "any",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "host",
            "000000000001",
            "vid",
            "1",
            "1",
            "ethertype",
            "11",
            "11",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
        {
            "any",
            "host",
            "000000000001",
            "vid",
            "1",
            "1",
            "ethertype",
            "800",
            "ffff",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
        {
            "any",
            "host",
            "000000000001",
            "vid",
            "1",
            "ip",
            "any",
            "host",
            "192.168.1.1",
            "protocol",
            "17",
            "l4-source-port",
            "1",
            "l4-destination-port",
            "2",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
        {
            "any",
            "host",
            "000000000001",
            "vid",
            "1",
            "ipv6",
            "any",
            "0::1/1",
            "protocol",
            "16",
            "l4-source-port",
            "3",
            "l4-destination-port",
            "4",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
    };
    
    CLI_TASK_WorkingArea_T ctrl;
    unsigned long i;
    char *acl_name = "t1";
    RULE_TYPE_RETURN_TYPE_T result;
    
    strncpy(ctrl.CMenu.acl_name, acl_name, 3);
    ctrl.CMenu.acl_type = RULE_TYPE_MAC_ACL;
    
    result = L4_MGR_ACL_CreateAcl(ctrl.CMenu.acl_name, ctrl.CMenu.acl_type);
    assert(RULE_TYPE_OK == result);
    
    for (i = 0; i < _countof(cmd); ++i)
    {
        CMD_T * cmd_p = &cmd[i];
        UI32_T cli_result;
        char * show_arg[256] =
        {
            "t1"
        };
        
        cli_result = CLI_API_SET_ACL_MAC(cmd_p->action, cmd_p->cmd_index, cmd_p->arg, &ctrl);
        assert(CLI_NO_ERROR == cli_result);
        
        CLI_API_Show_MAC_AccessList(0, show_arg, &ctrl);
        
        cli_result = CLI_API_SET_ACL_MAC(cmd_p->action, cmd_p->cmd_index_no, cmd_p->arg, &ctrl);
        assert(CLI_NO_ERROR == cli_result);
    }
    return 0;
}

int
CLI_ACL_UT_SET_ACE_IPv4_Extend()
{
    typedef struct
    {
        char *arg[256];
        UI32_T action;
        UI32_T cmd_index;
        UI32_T cmd_index_no;
        //RULE_TYPE_UI_Ace_Entry_T ui_ace;
    } CMD_T;
    
    CMD_T cmd[] =
    {
        {
            "any",
            "any",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_EXTENDED_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_EXTENDED_CMD_W2_NO_PERMIT,
        },
        {
            "123",
            "192.168.1.0",
            "255.255.255.0",
            "host",
            "192.169.1.1",
            "precedence",
            "7",
            "source-port",
            "1",
            "1",
            "destination-port",
            "2",
            "2",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_EXTENDED_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_EXTENDED_CMD_W2_NO_DENY,
        },
        {
            "123",
            "192.168.1.0",
            "255.255.255.0",
            "host",
            "192.169.1.1",
            "precedence",
            "7",
            "source-port",
            "1",
            "1",
            "destination-port",
            "2",
            "2",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_EXTENDED_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_EXTENDED_CMD_W2_NO_DENY,
        },
        {
            "upd",
            "192.168.1.1",
            "host",
            "192.168.1.1",
            "source-port",
            "1",
            "1",
            "destination-port",
            "2",
            "2",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_EXTENDED_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_EXTENDED_CMD_W2_NO_DENY,
        },
    };
    
    CLI_TASK_WorkingArea_T ctrl;
    unsigned long i;
    char *acl_name = "t1";
    RULE_TYPE_RETURN_TYPE_T result;
    
    strncpy(ctrl.CMenu.acl_name, acl_name, 3);
    ctrl.CMenu.acl_type = RULE_TYPE_IP_EXT_ACL;
    
    result = L4_MGR_ACL_CreateAcl(ctrl.CMenu.acl_name, ctrl.CMenu.acl_type);
    assert(RULE_TYPE_OK == result);
    
    for (i = 0; i < _countof(cmd); ++i)
    {
        CMD_T * cmd_p = &cmd[i];
        UI32_T cli_result;
        char * show_arg[256] =
        {
            "extended",
            "t1"
        };
        
        cli_result = CLI_API_SET_ACL_Extended(cmd_p->action, cmd_p->cmd_index, cmd_p->arg, &ctrl);
        assert(CLI_NO_ERROR == cli_result);
        
        CLI_API_Show_IP_AccessList(PRIVILEGE_EXEC_CMD_W3_SHOW_ACCESSLIST_IP,
                                   show_arg, &ctrl);
        
        cli_result = CLI_API_SET_ACL_Extended(cmd_p->action, cmd_p->cmd_index_no, cmd_p->arg, &ctrl);
        assert(CLI_NO_ERROR == cli_result);
    }
    return 0;
}

int
CLI_ACL_UT_SET_ACE_IPv6_Standard()
{
    typedef struct
    {
        char *arg[256];
        UI32_T action;
        UI32_T cmd_index;
        UI32_T cmd_index_no;
        //RULE_TYPE_UI_Ace_Entry_T ui_ace;
    } CMD_T;
    
    CMD_T cmd[] =
    {
        {
            "any",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_STANDARD_IPV6_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_STANDARD_IPV6_CMD_W2_NO_PERMIT,
        },
        {
            "0::1/1",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_STANDARD_IPV6_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_STANDARD_IPV6_CMD_W2_NO_DENY,
        },
    };
    
    CLI_TASK_WorkingArea_T ctrl;
    unsigned long i;
    char *acl_name = "t1";
    RULE_TYPE_RETURN_TYPE_T result;

    strncpy(ctrl.CMenu.acl_name, acl_name, 3);
    ctrl.CMenu.acl_type = RULE_TYPE_IPV6_STD_ACL;
    
    result = L4_MGR_ACL_CreateAcl(ctrl.CMenu.acl_name, ctrl.CMenu.acl_type);
    assert(RULE_TYPE_OK == result);

    for (i = 0; i < _countof(cmd); ++i)
    {
        CMD_T * cmd_p = &cmd[i];
        UI32_T cli_result;
        char * show_arg[256] =
        {
            "standard",
            "t1"
        };

        cli_result = CLI_API_SET_ACL_Standard_IPV6(cmd_p->action, cmd_p->cmd_index, cmd_p->arg, &ctrl);
        assert(CLI_NO_ERROR == cli_result);
        
        
        CLI_API_Show_IP_AccessList(PRIVILEGE_EXEC_CMD_W3_SHOW_ACCESSLIST_IPV6,
                                   show_arg, &ctrl);
        
        cli_result = CLI_API_SET_ACL_Standard_IPV6(cmd_p->action, cmd_p->cmd_index_no, cmd_p->arg, &ctrl);
        assert(CLI_NO_ERROR == cli_result);
    }
    
    return 0;
}

int
CLI_ACL_UT_SET_ACE_IPv6_Extended()
{
    typedef struct
    {
        char *arg[256];
        UI32_T action;
        UI32_T cmd_index;
        UI32_T cmd_index_no;
        //RULE_TYPE_UI_Ace_Entry_T ui_ace;
    } CMD_T;
    
    CMD_T cmd[] =
    {
        {
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
            "any",
#endif
#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
            "any",
#endif
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_EXTENDED_IPV6_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_EXTENDED_IPV6_CMD_W2_NO_PERMIT,
        },
        {
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
            "2009:DB9:2229::79/8",
#endif
#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
            "2009:DB9:2229::80/8",
#endif
            "dscp",
            "2",
            "flow-label",
            "1",
            "next-header",
            "17",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_EXTENDED_IPV6_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_EXTENDED_IPV6_CMD_W2_NO_DENY,
        },
        {
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
            "2009:DB9:2229::79/8",
#endif
#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
            "2009:DB9:2229::80/8",
#endif
            "dscp",
            "2",
            "flow-label",
            "1",
            "next-header",
            "17",
            "source-port",
            "11",
            "11",
            "destination-port",
            "22",
            "22",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_EXTENDED_IPV6_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_EXTENDED_IPV6_CMD_W2_NO_DENY,
        },
        {
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
            "2009:DB9:2229::79/8",
#endif
#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
            "2009:DB9:2229::80/8",
#endif
            "dscp",
            "2",
            "flow-label",
            "1",
            "next-header",
            "17",
            "source-port",
            "11",
            "destination-port",
            "22",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_EXTENDED_IPV6_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_EXTENDED_IPV6_CMD_W2_NO_DENY,
        },
    };
    
    CLI_TASK_WorkingArea_T ctrl;
    unsigned long i;
    char *acl_name = "t1";
    RULE_TYPE_RETURN_TYPE_T result;
    
    strncpy(ctrl.CMenu.acl_name, acl_name, 3);
    ctrl.CMenu.acl_type = RULE_TYPE_IPV6_EXT_ACL;
    
    result = L4_MGR_ACL_CreateAcl(ctrl.CMenu.acl_name, ctrl.CMenu.acl_type);
    assert(RULE_TYPE_OK == result);
    
    for (i = 0; i < _countof(cmd); ++i)
    {
        CMD_T * cmd_p = &cmd[i];
        UI32_T cli_result;
        char * show_arg[256] =
        {
            "extended",
            "t1"
        };
        
        cli_result = CLI_API_SET_ACL_Extended_IPV6(cmd_p->action, cmd_p->cmd_index, cmd_p->arg, &ctrl);
        assert(CLI_NO_ERROR == cli_result);
        
        CLI_API_Show_IP_AccessList(PRIVILEGE_EXEC_CMD_W3_SHOW_ACCESSLIST_IPV6,
                                   show_arg, &ctrl);

        cli_result = CLI_API_SET_ACL_Extended_IPV6(cmd_p->action, cmd_p->cmd_index_no, cmd_p->arg, &ctrl);
        assert(CLI_NO_ERROR == cli_result);
    }
    return 0;
}

int
CLI_ACL_UT_SET_ACE_ARP()
{
#if (SYS_CPNT_DAI == TRUE)
    typedef struct
    {
        char *arg[256];
        UI32_T action;
        UI32_T cmd_index;
        UI32_T cmd_index_no;
        //RULE_TYPE_UI_Ace_Entry_T ui_ace;
    } CMD_T;
    
    CMD_T cmd[] =
    {
        {
            "ip",
            "any",
            "mac",
            "any",
            "log",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_ARP_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_ARP_CMD_W2_NO_PERMIT,
        },
        {
            "request",
            "ip",
            "host",
            "192.168.1.1",
            "mac",
            "host",
            "00-00-00-00-00-01",
            "log",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_ARP_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_ARP_CMD_W2_NO_DENY,
        },
        {
            "response",
            "ip",
            "192.168.1.0",
            "255.255.255.0",
            "any",
            "mac",
            "host",
            "00-00-00-00-00-01",
            "any",
            "log",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_ARP_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_ARP_CMD_W2_NO_DENY,
        },
    };
    
    CLI_TASK_WorkingArea_T ctrl;
    unsigned long i;
    char *acl_name = "t1";
    RULE_TYPE_RETURN_TYPE_T result;
    strncpy(ctrl.CMenu.acl_name, acl_name, 3);
    ctrl.CMenu.acl_type = RULE_TYPE_ARP_ACL;
    
    result = L4_MGR_ACL_CreateAcl(ctrl.CMenu.acl_name, ctrl.CMenu.acl_type);
    assert(RULE_TYPE_OK == result);
    
    for (i = 0; i < _countof(cmd); ++i)
    {
        CMD_T * cmd_p = &cmd[i];
        UI32_T cli_result;
        char * show_arg[256] =
        {
            "t1"
        };
        
        cli_result = CLI_API_SET_ACL_ARP(cmd_p->action, cmd_p->cmd_index, cmd_p->arg, &ctrl);
        assert(CLI_NO_ERROR == cli_result);
        
        CLI_API_Show_Arp_AccessList(0, show_arg, &ctrl);
        
        cli_result = CLI_API_SET_ACL_ARP(cmd_p->action, cmd_p->cmd_index_no, cmd_p->arg, &ctrl);
        assert(CLI_NO_ERROR == cli_result);
    }
#endif /* #if (SYS_CPNT_DAI == TRUE) */
    return 0;
}

int
CLI_ACL_UT_Bind_AccessList_On_Trunk_Port()
{
#if (SYS_CPNT_QOS_V2_TRUNK_PORT == TRUE)

    enum
    {
#if (SYS_CPNT_ACL_COUNTER == TRUE)
        COUNTER_ENABLE_STATUS = RULE_TYPE_COUNTER_ENABLE,
#else
        COUNTER_ENABLE_STATUS = RULE_TYPE_COUNTER_DISABLE,
#endif
    };

    char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_EXT_ACL;

    UI32_T acl_index;

    RULE_TYPE_Ace_Entry_T ace_entry;

    UI32_T ace_count;
    UI32_T a3, a4;

    RULE_TYPE_RETURN_TYPE_T result;

    CLI_TASK_WorkingArea_T ctrl;
    char *arg[256] = {NULL};
    UI16_T cmd_idx;

    memset(arg, 0, sizeof(arg));

    result = L4_MGR_ACL_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    ace_count = 0;

    for (a3 = 1; a3 < 2; ++a3)
    {
        for (a4 = 0; a4 <= 2; ++a4)
        {
            ace_entry.ace_type = acl_type;

            result = L4_MGR_ACL_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            ace_entry.access = RULE_TYPE_ACE_DENY;
            
            SET_IPV4(ace_entry.u.ip.aceDestIpAddr, 172, 16, a3, a4);
            SET_IPV4(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

            result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
            assert(RULE_TYPE_OK == result);

            ace_count ++;
        }
    }

    {
        UI32_T ifindex;

        for (ifindex = SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER;
             ifindex < (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER +
                        SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM);
             ++ ifindex)
        {
            RULE_TYPE_INTERFACE_INFO_T interface;

            memset(&interface, 0, sizeof(interface));
            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
            assert(RULE_TYPE_INTERFACE_TRUNK == interface.type);

            interface.direction = RULE_TYPE_INBOUND;

            ctrl.CMenu.pchannel_id = interface.trunk_id;

            cmd_idx = PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W2_IP_ACCESSGROUP;
            arg[0] = acl_name;
            arg[1] = "input";
            arg[2] = "counter";
            CLI_API_IP_AccessGroup_Pch(cmd_idx, (char **) arg, &ctrl);
            memset(arg, 0, sizeof(arg));

            {
                UI32_T running_acl_index;
                RULE_TYPE_UI_AclEntry_T running_acl_entry;
                UI32_T precedence;
                UI8_T running_time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T running_counter = RULE_TYPE_COUNTER_DISABLE;

                SYS_TYPE_Get_Running_Cfg_T running_result;

                precedence = 0;
                running_result = L4_MGR_ACL_GetNextRunningAclByPort(ifindex,
                                                                    interface.direction,
                                                                    &running_acl_index,
                                                                    &running_acl_entry,
                                                                    &precedence,
                                                                    running_time_range_name,
                                                                    &running_counter);
                assert(SYS_TYPE_GET_RUNNING_CFG_SUCCESS == running_result);

                assert(running_acl_index == acl_index);
                //assert(RULE_TYPE_UNDEF_TIME_RANGE == running_time_range_index);
                assert(RULE_TYPE_COUNTER_ENABLE == running_counter);
            }

//            CLI_API_Show_AccessGroup(cmd_idx, (char **)arg, &ctrl);
//            memset(arg, 0, sizeof(arg));
//
//            CLI_API_Show_AccessList_Hardware_Counters(cmd_idx, (char **)arg, &ctrl);
//            memset(arg, 0, sizeof(arg));
        }

        for (ifindex = SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER;
             ifindex < (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER +
                        SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM);
             ++ ifindex)
        {
            RULE_TYPE_INTERFACE_INFO_T interface;

            memset(&interface, 0, sizeof(interface));
            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
            assert(RULE_TYPE_INTERFACE_TRUNK == interface.type);

            ctrl.CMenu.pchannel_id = interface.trunk_id;

            cmd_idx = PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W3_NO_IP_ACCESSGROUP;
            arg[0] = acl_name;
            arg[1] = "input";
            arg[2] = "counter";
            CLI_API_IP_AccessGroup_Pch(cmd_idx, (char **) arg, &ctrl);
            memset(arg, 0, sizeof(arg));

            {
                UI32_T running_acl_index;
                RULE_TYPE_UI_AclEntry_T running_acl_entry;
                UI32_T precedence;
                UI8_T running_time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T running_counter;

                SYS_TYPE_Get_Running_Cfg_T running_result;

                precedence = 0;
                running_result = L4_MGR_ACL_GetNextRunningAclByPort(ifindex,
                                                                    interface.direction,
                                                                    &running_acl_index,
                                                                    &running_acl_entry,
                                                                    &precedence,
                                                                    running_time_range_name,
                                                                    &running_counter);
                assert(SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE == running_result);
            }

//            CLI_API_Show_AccessGroup(cmd_idx, (char **)arg, &ctrl);
//            memset(arg, 0, sizeof(arg));
        }
    }

#endif // SYS_CPNT_QOS_V2_TRUNK_PORT

    return 0;
}

int
CLI_ACL_UT_Bind_AccessList_On_All_Ports()
{
#if (SYS_CPNT_QOS_V2_ALL_PORTS == TRUE)

    enum
    {
#if (SYS_CPNT_ACL_COUNTER == TRUE)
        COUNTER_ENABLE_STATUS = RULE_TYPE_COUNTER_ENABLE,
#else
        COUNTER_ENABLE_STATUS = RULE_TYPE_COUNTER_DISABLE,
#endif
    };

    char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_EXT_ACL;

    UI32_T acl_index;    

    RULE_TYPE_Ace_Entry_T ace_entry;

    UI32_T ace_count;
    UI32_T a3, a4;

    RULE_TYPE_RETURN_TYPE_T result;

    CLI_TASK_WorkingArea_T ctrl;
    char *arg[256] = {NULL};
    UI16_T cmd_idx;

    result = L4_MGR_ACL_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    ace_count = 0;

    for (a3 = 1; a3 < 2; ++a3)
    {
        for (a4 = 0; a4 <= 2; ++a4)
        {
            ace_entry.ace_type = acl_type;

            result = L4_MGR_ACL_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

//            ace_entry.u.ip.access = RULE_TYPE_ACE_DENY;

            SET_IPV4(ace_entry.u.ip.aceDestIpAddr, 172, 16, a3, a4);
            SET_IPV4(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

            if (0)
            {
                char arg0[] = "any";
                char arg1[] = "host";
                char arg2[100];

                snprintf(arg2, sizeof(arg2), "%d.%d.%lu.%lu", 172,16,a3,a4);

                cmd_idx = PRIVILEGE_CFG_ACL_EXTENDED_CMD_W1_DENY;

                arg[0] = arg0;
                arg[1] = arg1;
                arg[2] = arg2;
                arg[3] = NULL;

                CLI_API_Deny_ACL_Extended(cmd_idx, arg, &ctrl);
            }

            result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
            assert(RULE_TYPE_OK == result);

            ace_count ++;
        }
    }

    {
        UI32_T ifindex = RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER;

        {
            RULE_TYPE_INTERFACE_INFO_T interface;

            memset(&interface, 0, sizeof(interface));
            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
            assert(RULE_TYPE_INTERFACE_ALL_UNIT == interface.type);

            interface.direction = RULE_TYPE_INBOUND;

            cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W2_IP_ACCESSGROUP;
            arg[0] = acl_name;
            arg[1] = "input";
            arg[2] = "counter";
            CLI_API_IP_AccessGroup_Global(cmd_idx, (char **) arg, &ctrl);
            memset(arg, 0, sizeof(arg));

            {
                UI32_T running_acl_index;
                RULE_TYPE_UI_AclEntry_T running_acl_entry;
                UI32_T precedence;
                UI8_T running_time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T running_counter = RULE_TYPE_COUNTER_DISABLE;

                SYS_TYPE_Get_Running_Cfg_T running_result;

                precedence = 0;
                running_result = L4_MGR_ACL_GetNextRunningAclByPort(ifindex,
                                                                    interface.direction,
                                                                    &running_acl_index,
                                                                    &running_acl_entry,
                                                                    &precedence,
                                                                    running_time_range_name,
                                                                    &running_counter);
                assert(SYS_TYPE_GET_RUNNING_CFG_SUCCESS == running_result);

                assert(running_acl_index == acl_index);
                //assert(RULE_TYPE_UNDEF_TIME_RANGE == running_time_range_index);
                assert(RULE_TYPE_COUNTER_ENABLE == running_counter);
            }

//            CLI_API_Show_AccessGroup(cmd_idx, (char **)arg, &ctrl);
//            memset(arg, 0, sizeof(arg));
//
//            CLI_API_Show_AccessList_Hardware_Counters(cmd_idx, (char **)arg, &ctrl);
//            memset(arg, 0, sizeof(arg));
        }

        {
            RULE_TYPE_INTERFACE_INFO_T interface;

            memset(&interface, 0, sizeof(interface));
            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
            assert(RULE_TYPE_INTERFACE_ALL_UNIT == interface.type);

            interface.direction = RULE_TYPE_INBOUND;

            cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W3_NO_IP_ACCESSGROUP;
            arg[0] = acl_name;
            arg[1] = "input";
            arg[2] = "counter";
            CLI_API_IP_AccessGroup_Global(cmd_idx, (char **) arg, &ctrl);
            memset(arg, 0, sizeof(arg));

            {
                UI32_T running_acl_index;
                RULE_TYPE_UI_AclEntry_T running_acl_entry;
                UI32_T precedence;
                UI8_T running_time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T running_counter = RULE_TYPE_COUNTER_DISABLE;

                SYS_TYPE_Get_Running_Cfg_T running_result;

                precedence = 0;
                running_result = L4_MGR_ACL_GetNextRunningAclByPort(ifindex,
                                                                    interface.direction,
                                                                    &running_acl_index,
                                                                    &running_acl_entry,
                                                                    &precedence,
                                                                    running_time_range_name,
                                                                    &running_counter);
                assert(SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE == running_result);
            }
        }
    }

#endif // SYS_CPNT_QOS_V2_ALL_PORTS

    return 0;
}

int
CLI_ACL_UT_Show_AccessList_Hardware_Counters()
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)

	const char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;

    UI32_T ifindex = 1;
    RULE_TYPE_InOutDirection_T inout_profile = RULE_TYPE_INBOUND;
    const char *time_rnage_name = NULL;
    RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_ENABLE;

    CLI_ACL_UT_Proc_CreateOneAcl(acl_name, acl_type);
    CLI_ACL_UT_Proc_BindACLOnPort(ifindex, acl_name, acl_type, inout_profile,
                                  time_rnage_name,
                                  counter_enable);

    CLI_ACL_UT_Proc_Set_ACL_Counter(ifindex,
                                    acl_name,
                                    acl_type,
                                    inout_profile);

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
    ifindex = 2;
    inout_profile = RULE_TYPE_OUTBOUND;
    time_rnage_name = NULL;
    counter_enable = RULE_TYPE_COUNTER_ENABLE;

    CLI_ACL_UT_Proc_BindACLOnPort(ifindex, acl_name, acl_type, inout_profile,
                                  time_rnage_name,
                                  counter_enable);

    CLI_ACL_UT_Proc_Set_ACL_Counter(ifindex,
                                    acl_name,
                                    acl_type,
                                    inout_profile);

    CLI_API_Show_MAC_AccessGroup(0, NULL, NULL);
    CLI_API_Show_AccessList(0, NULL, NULL);
    CLI_API_Show_AccessList_Hardware_Counters(0, NULL, NULL);
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */
#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

int
CLI_ACL_UT_Show_AccessList_Hardware_Counters_Crash_Issue()
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)

	const char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_EXT_ACL;

    UI32_T ifindex = 1;
    RULE_TYPE_InOutDirection_T inout_profile = RULE_TYPE_INBOUND;
    const char *time_range_name = NULL;
    RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_ENABLE;

    RULE_TYPE_RETURN_TYPE_T result;

    result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);

    result = L4_PMGR_ACL_BindPort2Acl(ifindex, acl_name, RULE_TYPE_IP_ACL,
                                      inout_profile== RULE_TYPE_INBOUND ? TRUE : FALSE,
                                      time_range_name,
                                      counter_enable);
    assert(RULE_TYPE_OK == result);

    CLI_API_Show_AccessList_Hardware_Counters(0, NULL, NULL);

#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

int
CLI_ACL_UT_Clear_AccessList_Hardware_Counters()
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)

    CLI_TASK_WorkingArea_T ctrl;
//    char *arg[256] = {NULL};
//    UI16_T cmd_idx;

    UI32_T acl_id;
    UI32_T i;

    typedef struct
    {
        UI32_T lport;
        RULE_TYPE_InOutDirection_T direction;
    } interface_t;

    typedef struct
    {
        const char *name;
        RULE_TYPE_AclType_T type;
        interface_t if_list[10];
    } acl_set_t;

    typedef struct
    {
        char *arg[256];
        UI16_T cmd_idx;
    } cmd_set_t;

    acl_set_t acl_set[] =
    {
        {
            .name = "a1",
            .type = RULE_TYPE_MAC_ACL,
            .if_list =
            {
                {
                    .lport = 1,
                    .direction = RULE_TYPE_INBOUND,
                },

                {
                    .lport = 2,
                    .direction = RULE_TYPE_INBOUND,
                },

            }
        },

        {
            .name = "a2",
            .type = RULE_TYPE_MAC_ACL,
            .if_list =
            {
                {
                    .lport = 5,
                    .direction = RULE_TYPE_INBOUND,
                },

                {
                    .lport = 7,
                    .direction = RULE_TYPE_INBOUND,
                },
            }
        },

    };

    cmd_set_t cmd_set[] =
    {
        {
        },

        {
            "name",
            "a1",
        },

        {
            "direction",
            "in",
        },

        {
            "interface",
            "ethernet",
            "1/2",
        },

        {
            "name",
            "a2",
            "direction",
            "in",
        },

        {
            "name",
            "a1",
            "interface",
            "ethernet",
            "1/2",
        },

        {
            "direction",
            "in",
            "interface",
            "ethernet",
            "1/5",
        },

        {
            "name",
            "a1",
            "direction",
            "in",
            "interface",
            "ethernet",
            "1/2"
        },
    };

    for (acl_id = 0; acl_id < _countof(acl_set); ++acl_id)
    {
        const char *time_range_name = NULL;

        acl_set_t *acl = &acl_set[acl_id];

        CLI_ACL_UT_Proc_CreateOneAcl(acl->name, acl->type);

        {
            UI32_T j;

            for (j = 0; j < _countof(acl->if_list); ++j)
            {
                if (0 == acl->if_list[j].lport)
                {
                    continue;
                }

                CLI_ACL_UT_Proc_BindACLOnPort(acl->if_list[j].lport,
                                              acl->name,
                                              acl->type,
                                              acl->if_list[j].direction,
                                              time_range_name,
                                              RULE_TYPE_COUNTER_ENABLE);
            }
        }
    }

//reset_counter:

    for (i=0; i < _countof(cmd_set); ++i)
    {
        cmd_set_t *cmd = &cmd_set[i];

        ////////////////////////////////////////////////////////////////////////
        // reset counter
        ////////////////////////////////////////////////////////////////////////
        for (acl_id = 0; acl_id < _countof(acl_set); ++acl_id)
        {
            acl_set_t *acl = &acl_set[acl_id];

            {
                UI32_T j;

                for (j = 0; j < _countof(acl->if_list); ++j)
                {
                    if (0 == acl->if_list[j].lport)
                    {
                        continue;
                    }

                    CLI_ACL_UT_Proc_Set_ACL_Counter(acl->if_list[j].lport,
                                                    acl->name,
                                                    acl->type,
                                                    acl->if_list[j].direction);
                }
            }
        }
        ////////////////////////////////////////////////////////////////////////
        {
            UI32_T j;

            printf("> clear access-list hardware counters ");
            for (j = 0; j < _countof(cmd->arg); ++j)
            {
                if (NULL == cmd->arg[j])
                {
                    break;
                }

                printf("%s ", cmd->arg[j]);
            }
            printf("\r\n\r\n");
        }

        CLI_API_Show_AccessList_Hardware_Counters(0, NULL, &ctrl);
        CLI_API_Clear_AccessList_Hardware_Counters(cmd->cmd_idx, cmd->arg, &ctrl);
        CLI_API_Show_AccessList_Hardware_Counters(0, NULL, &ctrl);

        printf("\r\n**********\r\n");
    }


#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

int
CLI_ACL_UT_Clear_AccessList_Hardware_Counters_egress()
{
#if (SYS_CPNT_ACL_COUNTER == TRUE) && (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)

    CLI_TASK_WorkingArea_T ctrl;
    //    char *arg[256] = {NULL};
    //    UI16_T cmd_idx;

    UI32_T acl_id;
    UI32_T i;

    typedef struct
    {
        UI32_T lport;
        RULE_TYPE_InOutDirection_T direction;
    } interface_t;

    typedef struct
    {
        const char *name;
        RULE_TYPE_AclType_T type;
        interface_t if_list[10];
    } acl_set_t;

    typedef struct
    {
        char *arg[256];
        UI16_T cmd_idx;
    } cmd_set_t;

    acl_set_t acl_set[] =
    {
        {
            .name = "a1",
            .type = RULE_TYPE_MAC_ACL,
            .if_list =
            {
                {
                    .lport = 1,
                    .direction = RULE_TYPE_OUTBOUND,
                },

                {
                    .lport = 2,
                    .direction = RULE_TYPE_OUTBOUND,
                },
            }
        },

        {
            .name = "a2",
            .type = RULE_TYPE_MAC_ACL,
            .if_list =
            {
                {
                    .lport = 6,
                    .direction = RULE_TYPE_OUTBOUND,
                },

                {
                    .lport = 8,
                    .direction = RULE_TYPE_OUTBOUND,
                },
            }
        },

    };

    cmd_set_t cmd_set[] =
    {
        {
        },

        {
            "name",
            "a1",
        },

        {
            "interface",
            "ethernet",
            "1/2",
        },

        {
            "name",
            "a1",
            "direction",
            "out",
        },

        {
            "name",
            "a1",
            "interface",
            "ethernet",
            "1/2",
        },
    };

    for (acl_id = 0; acl_id < _countof(acl_set); ++acl_id)
    {
        const char *time_range_name = NULL;

        acl_set_t *acl = &acl_set[acl_id];

        CLI_ACL_UT_Proc_CreateOneAcl(acl->name, acl->type);

        {
            UI32_T j;

            for (j = 0; j < _countof(acl->if_list); ++j)
            {
                if (0 == acl->if_list[j].lport)
                {
                    continue;
                }

                CLI_ACL_UT_Proc_BindACLOnPort(acl->if_list[j].lport,
                                              acl->name,
                                              acl->type,
                                              acl->if_list[j].direction,
                                              time_range_name,
                                              RULE_TYPE_COUNTER_ENABLE);
            }
        }
    }

    //reset_counter:

    for (i=0; i < _countof(cmd_set); ++i)
    {
        cmd_set_t *cmd = &cmd_set[i];

        ////////////////////////////////////////////////////////////////////////
        // reset counter
        ////////////////////////////////////////////////////////////////////////
        for (acl_id = 0; acl_id < _countof(acl_set); ++acl_id)
        {
            acl_set_t *acl = &acl_set[acl_id];

            {
                UI32_T j;

                for (j = 0; j < _countof(acl->if_list); ++j)
                {
                    if (0 == acl->if_list[j].lport)
                    {
                        continue;
                    }

                    CLI_ACL_UT_Proc_Set_ACL_Counter(acl->if_list[j].lport,
                                                    acl->name,
                                                    acl->type,
                                                    acl->if_list[j].direction);
                }
            }
        }
        ////////////////////////////////////////////////////////////////////////

        //        CLI_API_Show_AccessList_Hardware_Counters(0, NULL, &ctrl);

        {
            UI32_T j;

            printf("> clear access-list hardware counters ");
            for (j = 0; j < _countof(cmd->arg); ++j)
            {
                if (NULL == cmd->arg[j])
                {
                    break;
                }
                
                printf("%s ", cmd->arg[j]);
            }
            printf("\r\n\r\n");
        }
        
        CLI_API_Clear_AccessList_Hardware_Counters(cmd->cmd_idx, cmd->arg, &ctrl);
        CLI_API_Show_AccessList_Hardware_Counters(0, NULL, &ctrl);
        
        printf("\r\n**********\r\n");
    }
    
    
#endif // SYS_CPNT_ACL_COUNTER && SYS_CPNT_QOS_V2_EGRESS_PORT
    
    return 0;
}

int
CLI_ACL_UT_Add_ACL_Mirror()
{
#if (SYS_CPNT_ACL_MIRROR == TRUE)

    struct
    {
        char                *acl_name;
        RULE_TYPE_AclType_T acl_type;
        UI32_T              acl_index;
    } exp_grp[] =
    {
        {"a1", RULE_TYPE_MAC_ACL, 0},
        {"a2", RULE_TYPE_MAC_ACL, 0},
    };

    RULE_TYPE_RETURN_TYPE_T result;    
    UI32_T i;

    for (i = 0; i < _countof(exp_grp); ++ i)
    {
        RULE_CTRL_UT_Proc_Create_Acl_And_Add_Ace(exp_grp[i].acl_name,
                                                 exp_grp[i].acl_type,
                                                 5,
                                                 RULE_CTRL_UT_COMPRESS_DISABLE,
                                                 &exp_grp[i].acl_index);
#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        result = RULE_OM_RebuildAggregateAcl(exp_grp[i].acl_index);
        assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE
    }

    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T inout_profile = RULE_TYPE_INBOUND;
        const char *time_rnage_name = NULL;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_ENABLE;

        ifindex = 1;
        CLI_ACL_UT_Proc_BindACLOnPort(ifindex, exp_grp[0].acl_name,
                                      exp_grp[0].acl_type,
                                      inout_profile,
                                      time_rnage_name,
                                      counter_enable);

        ifindex = 2;
        CLI_ACL_UT_Proc_BindACLOnPort(ifindex, exp_grp[0].acl_name,
                                      exp_grp[0].acl_type,
                                      inout_profile,
                                      time_rnage_name,
                                      counter_enable);
    }

    {
        UI32_T ifindex;
        RULE_TYPE_InOutDirection_T inout_profile = RULE_TYPE_INBOUND;
        const char *time_rnage_name = NULL;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

        ifindex = 4;
        inout_profile = RULE_TYPE_OUTBOUND;
        CLI_ACL_UT_Proc_BindACLOnPort(ifindex, exp_grp[1].acl_name,
                                      exp_grp[1].acl_type,
                                      inout_profile,
                                      time_rnage_name,
                                      counter_enable);
    }

    {
        UI32_T ifindex_dest = 5;

        for (i=0; i < _countof(exp_grp); ++i)
        {
            result = L4_PMGR_ACL_AddAclMirrorEntry(ifindex_dest,
                                                             exp_grp[i].acl_name);
            assert(result == RULE_TYPE_OK);

            CLI_ACL_UT_Proc_Validate_ACLs_Mirror();

            CLI_API_Show_AccessList_Hardware_Counters(0, NULL, NULL);
        }

        for (i=0; i < _countof(exp_grp); ++i)
        {
            result = L4_PMGR_ACL_RemoveAclMirrorEntry(ifindex_dest,
                                                             exp_grp[i].acl_name);
            assert(result == RULE_TYPE_OK);

            CLI_ACL_UT_Proc_Validate_ACLs_Mirror();
        }

        for (i=0; i < _countof(exp_grp); ++i)
        {
            result = L4_PMGR_ACL_AddAclMirrorEntry(ifindex_dest,
                                                             exp_grp[i].acl_name);
            assert(result == RULE_TYPE_OK);

            CLI_ACL_UT_Proc_Validate_ACLs_Mirror();
        }

        {
            UI32_T ifindex = 0;
            UI32_T acl_index;
            RULE_TYPE_UI_AclEntry_T acl_entry;

            for (i=0; i < _countof(exp_grp); ++i)
            {
                result = L4_PMGR_ACL_GetNextAclMirrorEntry(&ifindex, &acl_index,
                                                                    &acl_entry);
                assert(result == RULE_TYPE_OK);

                assert(ifindex == ifindex_dest);
                assert(acl_index == exp_grp[i].acl_index);
                assert( 0 == strcmp(acl_entry.acl_name, exp_grp[i].acl_name) );
            }

            result = L4_PMGR_ACL_GetNextAclMirrorEntry(&ifindex, &acl_index,
                                                                    &acl_entry);
            assert(result != RULE_TYPE_OK);
        }

        {
            UI32_T ifindex = 0;
            UI32_T acl_index;
            RULE_TYPE_UI_AclEntry_T acl_entry;

            for (i=0; i < _countof(exp_grp); ++i)
            {
                result = L4_PMGR_ACL_GetNextRunningAclMirrorEntry(&ifindex,
                                                                  &acl_index,
                                                                  &acl_entry);
                assert(result == RULE_TYPE_OK);

                assert(ifindex == ifindex_dest);
                assert(acl_index == exp_grp[i].acl_index);
                assert( 0 == strcmp(acl_entry.acl_name, exp_grp[i].acl_name) );
            }

            result = L4_PMGR_ACL_GetNextRunningAclMirrorEntry(&ifindex,
                                                              &acl_index,
                                                              &acl_entry);
            assert(result != RULE_TYPE_OK);
        }

    }

#endif // SYS_CPNT_ACL_MIRROR

    return 0;
}

int
CLI_ACL_UT_Compress_Continue_IP_ACE()
{
#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    char *acl_name = "1000ace_per_acl";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_EXT_ACL;

    RULE_TYPE_Ace_Entry_T ace_entry;

    UI32_T ace_count;
    UI32_T a3, a4;

    RULE_TYPE_RETURN_TYPE_T result;

    CLI_TASK_WorkingArea_T ctrl;
    char *arg[256] = {NULL};
    UI16_T cmd_idx;


    if (0)
    {
        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W3_ACCESSLIST_IP_EXTENDED;
        arg[0] = acl_name;

        CLI_API_AccessList_IP_Extended(cmd_idx, arg, &ctrl);
    }

    result = L4_MGR_ACL_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);

    ace_count = 0;

    for (a3=1; a3 < 255 && ace_count < SYS_ADPT_MAX_NBRS_OF_ACE; ++a3)
    {
        for (a4=0; a4 <= 255 && ace_count < SYS_ADPT_MAX_NBRS_OF_ACE; ++a4)
        {
            ace_entry.ace_type = acl_type;

            result = L4_MGR_ACL_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            ace_entry.access = RULE_TYPE_ACE_DENY;

            SET_IPV4(ace_entry.u.ip.aceDestIpAddr, 172, 16, a3, a4);
            SET_IPV4(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

            if (0)
            {
            char arg0[] = "any";
            char arg1[] = "host";
            char arg2[100];

            snprintf(arg2, sizeof(arg2), "%d.%d.%lu.%lu", 172,16,a3,a4);

            cmd_idx = PRIVILEGE_CFG_ACL_EXTENDED_CMD_W1_DENY;

            arg[0] = arg0;
            arg[1] = arg1;
            arg[2] = arg2;
            arg[3] = NULL;

            CLI_API_Deny_ACL_Extended(cmd_idx, arg, &ctrl);
            }

            result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
            assert(RULE_TYPE_OK == result);

            ace_count ++;
        }
    }

    {
        UI32_T uport_ifindex = 2;
        RULE_TYPE_AclType_T bind_acl_type = RULE_TYPE_IP_ACL;
        BOOL_T is_ingress = TRUE;

        result = L4_MGR_ACL_BindPort2Acl(uport_ifindex, acl_name, bind_acl_type,
                                         is_ingress, NULL, RULE_TYPE_COUNTER_DISABLE);
        assert(RULE_TYPE_OK == result);

        result = L4_MGR_ACL_UnBindPortFromAcl(uport_ifindex, acl_name, bind_acl_type, is_ingress);
        assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
        is_ingress = FALSE;

        result = L4_MGR_ACL_BindPort2Acl(uport_ifindex, acl_name, bind_acl_type,
                                         is_ingress, NULL, RULE_TYPE_COUNTER_DISABLE);
        assert(RULE_TYPE_OK == result);
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT
    }

#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE    
    return 0;
}

void
CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port(
    const char *acl_name,
    RULE_TYPE_AclType_T acl_type,
    UI32_T number_of_ace,
    UI32_T ifindex)
{
    RULE_TYPE_RETURN_TYPE_T result;

    result = L4_MGR_ACL_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);

    if (RULE_TYPE_IP_STD_ACL == acl_type)
    {
        UI32_T a4;
        BOOL_T is_ingress = TRUE;

        for (a4 = 0; a4 < number_of_ace; ++a4)
        {
            RULE_TYPE_Ace_Entry_T ace_entry;

            ace_entry.ace_type = acl_type;
            result = L4_MGR_ACL_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            ace_entry.access = (a4 % 2) ? RULE_TYPE_ACE_PERMIT :
                                          RULE_TYPE_ACE_DENY;
            
            assert(number_of_ace < 255);

            SET_IPV4(ace_entry.u.ip.aceSourceIpAddr, 172, 16, 1, a4);
            SET_IPV4(ace_entry.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);

            result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }

        result = L4_MGR_ACL_BindPort2Acl(ifindex, acl_name, RULE_TYPE_IP_ACL, is_ingress,
                                         NULL, RULE_TYPE_COUNTER_DISABLE);
        assert(RULE_TYPE_OK == result);
    }
    else if (RULE_TYPE_IP_EXT_ACL == acl_type)
    {
        UI32_T a4;
        BOOL_T is_ingress = TRUE;

        for (a4 = 0; a4 < number_of_ace; ++a4)
        {
            RULE_TYPE_Ace_Entry_T ace_entry;

            ace_entry.ace_type = acl_type;
            result = L4_MGR_ACL_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            ace_entry.access = (a4 % 2) ? RULE_TYPE_ACE_PERMIT :
                                          RULE_TYPE_ACE_DENY;

            assert(number_of_ace < 255);

            SET_IPV4(ace_entry.u.ip.aceDestIpAddr, 172, 16, 1, a4);
            SET_IPV4(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

            result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }

        result = L4_MGR_ACL_BindPort2Acl(ifindex, acl_name, RULE_TYPE_IP_ACL, is_ingress,
                                         NULL, RULE_TYPE_COUNTER_DISABLE);
        assert(RULE_TYPE_OK == result);
    }
    else if (RULE_TYPE_IPV6_STD_ACL == acl_type)
    {
        UI32_T w1;
        BOOL_T is_ingress = TRUE;

        for (w1 = 0; w1 < number_of_ace; ++w1)
        {
            RULE_TYPE_Ace_Entry_T ace_entry;

            ace_entry.ace_type = acl_type;
            result = L4_MGR_ACL_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            ace_entry.access = (w1 % 2) ? RULE_TYPE_ACE_PERMIT :
                                          RULE_TYPE_ACE_DENY;

            assert(number_of_ace < 255);

            SET_IPV6(ace_entry.u.ipv6.aceSourceIpAddr, w1, 0x1122, 0x3344, 0x5566);
            memset(ace_entry.u.ipv6.aceSourceIpAddrBitmask, 0xff, SYS_ADPT_IPV6_ADDR_LEN);
            //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = RULE_TYPE_MAX_OF_SRC_IPV6_PREFIX_LEN;

            result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }

        result = L4_MGR_ACL_BindPort2Acl(ifindex, acl_name, RULE_TYPE_IPV6_ACL, is_ingress,
                                         NULL, RULE_TYPE_COUNTER_DISABLE);
        assert(RULE_TYPE_OK == result);
    }
    else if (RULE_TYPE_IPV6_EXT_ACL == acl_type)
    {
        UI32_T w1;
        BOOL_T is_ingress = TRUE;

        for (w1 = 0; w1 < number_of_ace; ++w1)
        {
            RULE_TYPE_Ace_Entry_T ace_entry;

            ace_entry.ace_type = acl_type;
            result = L4_MGR_ACL_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            ace_entry.access = (w1 % 2) ? RULE_TYPE_ACE_PERMIT :
                                          RULE_TYPE_ACE_DENY;

            assert(number_of_ace < 255);

#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
            SET_IPV6(ace_entry.u.ipv6.aceSourceIpAddr, w1, 0x1122, 0x3344, 0x5566);
            RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_SRC_IPV6_PREFIX_LEN,
                                                ace_entry.u.ipv6.aceSourceIpAddrBitmask);
            //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN;
#elif (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
            SET_IPV6(ace_entry.u.ipv6.aceDestIpAddr, w1, 0x1122, 0x3344, 0x5566);
            RULE_OM_LocalConvertIpv6Prefix2Mask(RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN,
                                                ace_entry.u.ipv6.aceDestIpAddrBitmask);
            //ace_entry.u.ipv6.aceDestIpAddrPrefixLen = RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN;
#endif
            result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }

        result = L4_MGR_ACL_BindPort2Acl(ifindex, acl_name, RULE_TYPE_IPV6_ACL, is_ingress,
                                         NULL, RULE_TYPE_COUNTER_DISABLE);
        assert(RULE_TYPE_OK == result);
    }
    else if (RULE_TYPE_MAC_ACL == acl_type)
    {
        UI32_T a6;
        BOOL_T is_ingress = TRUE;

        for (a6 = 0; a6 < number_of_ace; ++a6)
        {
            RULE_TYPE_Ace_Entry_T ace_entry;

            ace_entry.ace_type = acl_type;
            result = L4_MGR_ACL_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            assert(number_of_ace < 255);

            ace_entry.access = (a6 % 2) ? RULE_TYPE_ACE_PERMIT :
                                          RULE_TYPE_ACE_DENY;

            SET_MAC(ace_entry.u.mac.aceSourceMacAddr, 0x01, 0x02, 0x03, 0x04, 0x05, a6);
            SET_IPV4(ace_entry.u.mac.aceSourceMacAddrBitmask, 255, 255, 255, 255);

            result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
            assert(RULE_TYPE_OK == result);
        }

        result = L4_MGR_ACL_BindPort2Acl(ifindex, acl_name, RULE_TYPE_MAC_ACL, is_ingress,
                                         NULL, RULE_TYPE_COUNTER_DISABLE);
        assert(RULE_TYPE_OK == result);
    }
}

void
CLI_ACL_UT_Proc_Unbind_And_Delete_ACL(
    const char *acl_name,
    UI32_T ifindex)
{
    UI32_T acl_index;
    RULE_TYPE_Acl_T acl_entry;
    RULE_TYPE_AclType_T acl_type;

    BOOL_T is_ingress;

    RULE_TYPE_RETURN_TYPE_T result;

    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
    assert(RULE_TYPE_OK == result);

    switch (acl_entry.acl_type)
    {
        case RULE_TYPE_MAC_ACL:
            acl_type = acl_entry.acl_type;
            break;

        case RULE_TYPE_IP_STD_ACL:
        case RULE_TYPE_IP_EXT_ACL:
            acl_type = RULE_TYPE_IP_ACL;
            break;

        case RULE_TYPE_IPV6_STD_ACL:
        case RULE_TYPE_IPV6_EXT_ACL:
            acl_type = RULE_TYPE_IPV6_ACL;
            break;

        default:
            assert(0);
            break;
    }

    is_ingress = TRUE;
    result = L4_MGR_ACL_UnBindPortFromAcl(ifindex, acl_name, acl_type, is_ingress);
    assert(RULE_TYPE_OK == result);

    result = L4_MGR_ACL_DelAcl(acl_name);
    assert(RULE_TYPE_OK == result);
}

int
CLI_ACL_UT_Dynamic_Selector_Assignment()
{
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
    enum
    {
        MAX_RULE_FOR_MAC_ACL        = SYS_ADPT_MAX_NBRS_OF_MAC_ACE,
        MAX_RULE_FOR_IP_ACL         = SYS_ADPT_MAX_NBRS_OF_IP_ACE,
        MAX_RULE_FOR_IPV6_STD_ACL   = SYS_ADPT_MAX_NBRS_OF_IPV6_ACE,
        MAX_RULE_FOR_IPV6_EXT_ACL   = SYS_ADPT_MAX_NBRS_OF_IPV6_ACE,
    };

    UI32_T ifindex;
    int retry_count = 2;

retry:
    ////////////////////////////////////////////////////////////////////////////

    ifindex = 1;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a1", RULE_TYPE_IP_STD_ACL,
                                                MAX_RULE_FOR_IP_ACL, ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a2", RULE_TYPE_IP_STD_ACL,
                                                MAX_RULE_FOR_IP_ACL, ifindex);

    ifindex = 3;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a3", RULE_TYPE_IP_STD_ACL,
                                                MAX_RULE_FOR_IP_ACL, ifindex);

    ifindex = 4;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a4", RULE_TYPE_IP_STD_ACL,
                                                MAX_RULE_FOR_IP_ACL, ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    ifindex = 1;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a1", ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a2", ifindex);

    ifindex = 3;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a3", ifindex);

    ifindex = 4;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a4", ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    ////////////////////////////////////////////////////////////////////////////

    ifindex = 1;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a1", RULE_TYPE_IP_EXT_ACL,
                                                MAX_RULE_FOR_IP_ACL, ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a2", RULE_TYPE_IP_EXT_ACL,
                                                MAX_RULE_FOR_IP_ACL, ifindex);

    ifindex = 3;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a3", RULE_TYPE_IP_EXT_ACL,
                                                MAX_RULE_FOR_IP_ACL, ifindex);

    ifindex = 4;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a4", RULE_TYPE_IP_EXT_ACL,
                                                MAX_RULE_FOR_IP_ACL, ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    ifindex = 1;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a1", ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a2", ifindex);

    ifindex = 3;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a3", ifindex);

    ifindex = 4;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a4", ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    ////////////////////////////////////////////////////////////////////////////

    ifindex = 1;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a1", RULE_TYPE_MAC_ACL,
                                                MAX_RULE_FOR_MAC_ACL, ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a2", RULE_TYPE_MAC_ACL,
                                                MAX_RULE_FOR_MAC_ACL, ifindex);

    ifindex = 3;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a3", RULE_TYPE_MAC_ACL,
                                                MAX_RULE_FOR_MAC_ACL, ifindex);

    ifindex = 4;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a4", RULE_TYPE_MAC_ACL,
                                                MAX_RULE_FOR_MAC_ACL, ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    ifindex = 1;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a1", ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a2", ifindex);

    ifindex = 3;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a3", ifindex);

    ifindex = 4;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a4", ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    ////////////////////////////////////////////////////////////////////////////

    ifindex = 1;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a1", RULE_TYPE_IPV6_EXT_ACL,
                                                MAX_RULE_FOR_IPV6_EXT_ACL, ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a2", RULE_TYPE_IPV6_EXT_ACL,
                                                MAX_RULE_FOR_IPV6_EXT_ACL, ifindex);

    ifindex = 3;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a3", RULE_TYPE_IPV6_EXT_ACL,
                                                MAX_RULE_FOR_IPV6_EXT_ACL, ifindex);

    ifindex = 4;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a4", RULE_TYPE_IPV6_EXT_ACL,
                                                MAX_RULE_FOR_IPV6_EXT_ACL, ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    ifindex = 1;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a1", ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a2", ifindex);

    ifindex = 3;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a3", ifindex);

    ifindex = 4;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a4", ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    ////////////////////////////////////////////////////////////////////////////

    ifindex = 1;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a1", RULE_TYPE_IPV6_STD_ACL,
                                                MAX_RULE_FOR_IPV6_STD_ACL, ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a2", RULE_TYPE_IPV6_STD_ACL,
                                                MAX_RULE_FOR_IPV6_STD_ACL, ifindex);

    ifindex = 3;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a3", RULE_TYPE_IPV6_STD_ACL,
                                                MAX_RULE_FOR_IPV6_STD_ACL, ifindex);

    ifindex = 4;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a4", RULE_TYPE_IPV6_STD_ACL,
                                                MAX_RULE_FOR_IPV6_STD_ACL, ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    ifindex = 1;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a1", ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a2", ifindex);

    ifindex = 3;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a3", ifindex);

    ifindex = 4;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a4", ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    ////////////////////////////////////////////////////////////////////////////
    // IP(a1) --> IPV6 STD(a2) --> IPV6 EXT(a3) --> MAC(a4)

    ifindex = 1;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a1", RULE_TYPE_IP_STD_ACL,
                                                MAX_RULE_FOR_IP_ACL, ifindex);

    ifindex = 1;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a2", RULE_TYPE_IPV6_STD_ACL,
                                                MAX_RULE_FOR_IPV6_STD_ACL, ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a3", RULE_TYPE_IPV6_EXT_ACL,
                                                MAX_RULE_FOR_IPV6_EXT_ACL, ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a4", RULE_TYPE_MAC_ACL,
                                                MAX_RULE_FOR_MAC_ACL, ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    //
    // Replace IP to MAC
    //
    ifindex = 1;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a1", ifindex);

    ifindex = 1;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a1", RULE_TYPE_MAC_ACL,
                                                MAX_RULE_FOR_MAC_ACL, ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    //
    // Replace IPV6 EXT to IP
    //
    ifindex = 2;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a3", ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Create_ACL_And_Bind_To_Port("a3", RULE_TYPE_IP_STD_ACL,
                                                MAX_RULE_FOR_IP_ACL, ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    ////////////////////////////////////////////////////////////////////////////

    ifindex = 1;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a1", ifindex);

    ifindex = 1;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a2", ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a3", ifindex);

    ifindex = 2;
    CLI_ACL_UT_Proc_Unbind_And_Delete_ACL("a4", ifindex);

    RULE_CTRL_UT_Proc_Validate_Rule();

    if (retry_count -- > 0)
    {
        goto retry;
    }

#endif // SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT

    return 0;
}

int
CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00250()
{
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
    char *mac_1 = "mac-1";
    RULE_TYPE_AclType_T mac_1_type = RULE_TYPE_MAC_ACL;

    char *mac_2 = "mac-2";
    RULE_TYPE_AclType_T mac_2_type = RULE_TYPE_MAC_ACL;

    char *mac_3 = "mac-3";
    RULE_TYPE_AclType_T mac_3_type = RULE_TYPE_MAC_ACL;

    char *mac_4 = "mac-4";
    RULE_TYPE_AclType_T mac_4_type = RULE_TYPE_MAC_ACL;

    char *mac_5 = "mac-5";
    RULE_TYPE_AclType_T mac_5_type = RULE_TYPE_MAC_ACL;

    char *mac_6 = "mac-6";
    RULE_TYPE_AclType_T mac_6_type = RULE_TYPE_MAC_ACL;

    typedef struct
    {
        UI32_T lport;
        RULE_TYPE_InOutDirection_T inout_profile;
        char *acl_name;
        RULE_TYPE_AclType_T acl_type;
    } EXP_GROUP;

    EXP_GROUP exp_1[] =
    {
        {1, RULE_TYPE_INBOUND,  mac_1, mac_1_type},
        {1, RULE_TYPE_OUTBOUND, mac_1, mac_1_type},
        {2, RULE_TYPE_INBOUND,  mac_1, mac_1_type},
        {2, RULE_TYPE_OUTBOUND, mac_1, mac_1_type},
        {5, RULE_TYPE_INBOUND,  mac_1, mac_1_type},
        {5, RULE_TYPE_OUTBOUND, mac_1, mac_1_type},
    };

    EXP_GROUP exp_2[] =
    {
        {1, RULE_TYPE_OUTBOUND, mac_3, mac_3_type},
        {2, RULE_TYPE_OUTBOUND, mac_3, mac_3_type},
        {5, RULE_TYPE_OUTBOUND, mac_3, mac_3_type},
    };


    EXP_GROUP exp_3[] =
    {
        {1, RULE_TYPE_OUTBOUND, mac_5, mac_5_type},
        {1, RULE_TYPE_INBOUND,  mac_2, mac_2_type},
        {2, RULE_TYPE_OUTBOUND, mac_5, mac_5_type},
        {2, RULE_TYPE_INBOUND,  mac_2, mac_2_type},
        {5, RULE_TYPE_OUTBOUND, mac_5, mac_5_type},
        {5, RULE_TYPE_INBOUND,  mac_2, mac_2_type},
    };

    UI32_T i;
    UI32_T result;

    /*
     {
        RULE_OM_SetDebugFlag(7);
        devrm_debug_flag = 1;
    }*/

    {
        char *acl_name = mac_1;
        RULE_TYPE_AclType_T acl_type = mac_1_type;
        //RULE_TYPE_Ace_Entry_T ace_entry;
        RULE_TYPE_UI_Ace_Entry_T ace_entry;

        result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
        assert(result == RULE_TYPE_OK);

        //ace_entry.ace_type = acl_type;
        //L4_PMGR_ACL_InitAce(&ace_entry);
        ace_entry.ace_type = acl_type;
        L4_PMGR_ACL_InitUIAce(&ace_entry);

        ace_entry.ether.vid.u.s.data = 100;
        ace_entry.ether.vid.u.s.mask = 0x0fff;
        ace_entry.ether.vid.op = VAL_diffServMacAceVidOp_equal;
        
//        ace_entry.u.mac.aceMinVid = 100;
//        ace_entry.u.mac.aceVidBitmask = 0x0fff;
//        ace_entry.u.mac.aceVidOp = VAL_diffServMacAceVidOp_equal;

//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);

//        ace_entry.ace_type = acl_type;
        ace_entry.ace_type = acl_type;
//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);

//        ace_entry.u.mac.aceMinVid = 300;
//        ace_entry.u.mac.aceVidBitmask = 0x0fff;
//        ace_entry.u.mac.aceVidOp = VAL_diffServMacAceVidOp_equal;
        
        ace_entry.ether.vid.u.s.data =300;
        ace_entry.ether.vid.u.s.mask = 0x0fff;
        ace_entry.ether.vid.op = VAL_diffServMacAceVidOp_equal;

//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.ace_type = acl_type;
        
//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);
        
        ace_entry.access = RULE_TYPE_ACE_DENY;

//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);
    }

    {
        char *acl_name = mac_2;
        RULE_TYPE_AclType_T acl_type = mac_2_type;
//        RULE_TYPE_Ace_Entry_T ace_entry;
        RULE_TYPE_UI_Ace_Entry_T ace_entry;

        result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
        assert(result == RULE_TYPE_OK);

        ace_entry.ace_type = acl_type;

//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);

//        ace_entry.u.mac.aceMinVid = 100;
//        ace_entry.u.mac.aceVidBitmask = 0x0fff;
//        ace_entry.u.mac.aceVidOp = VAL_diffServMacAceVidOp_equal;
        
        ace_entry.ether.vid.u.s.data = 100;
        ace_entry.ether.vid.u.s.mask = 0x0fff;
        ace_entry.ether.vid.op = VAL_diffServMacAceVidOp_equal;

//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.ace_type = acl_type;
//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);

//        ace_entry.u.mac.aceMinVid = 300;
//        ace_entry.u.mac.aceVidBitmask = 0x0fff;
//        ace_entry.u.mac.aceVidOp = VAL_diffServMacAceVidOp_equal;
        
        ace_entry.ether.vid.u.s.data = 300;
        ace_entry.ether.vid.u.s.mask = 0x0fff;
        ace_entry.ether.vid.op = VAL_diffServMacAceVidOp_equal;

//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.ace_type = acl_type;
//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);
        
        ace_entry.access = RULE_TYPE_ACE_DENY;

//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);
    }

    for (i=0; i<_countof(exp_1); ++i)
    {
        EXP_GROUP *eg = &exp_1[i];
        result = L4_PMGR_ACL_BindPort2Acl(eg->lport,
                                          eg->acl_name,
                                          eg->acl_type,
                                          eg->inout_profile,
                                          NULL,
                                          RULE_TYPE_COUNTER_DISABLE);
        assert(result == RULE_TYPE_OK);
    }

    {
        char *acl_name = mac_3;
        RULE_TYPE_AclType_T acl_type = mac_3_type;
//        RULE_TYPE_Ace_Entry_T ace_entry;
        RULE_TYPE_UI_Ace_Entry_T ace_entry;

        result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
        assert(result == RULE_TYPE_OK);

        ace_entry.ace_type = acl_type;
//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);

//        ace_entry.u.mac.aceMinVid = 400;
//        ace_entry.u.mac.aceVidBitmask = 0x0fff;
//        ace_entry.u.mac.aceVidOp = VAL_diffServMacAceVidOp_equal;

        ace_entry.ether.vid.u.s.data = 400;
        ace_entry.ether.vid.u.s.mask = 0x0fff;
        ace_entry.ether.vid.op = VAL_diffServMacAceVidOp_equal;
        
//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);


        ace_entry.ace_type = acl_type;
        
//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);

        ace_entry.access = RULE_TYPE_ACE_DENY;

//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);
    }

    {
        char *acl_name = mac_4;
        RULE_TYPE_AclType_T acl_type = mac_4_type;
//        RULE_TYPE_Ace_Entry_T ace_entry;
        RULE_TYPE_UI_Ace_Entry_T ace_entry;

        result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
        assert(result == RULE_TYPE_OK);

        ace_entry.ace_type = acl_type;
//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);

//        ace_entry.u.mac.aceMinVid = 400;
//        ace_entry.u.mac.aceVidBitmask = 0x0fff;
//        ace_entry.u.mac.aceVidOp = VAL_diffServMacAceVidOp_equal;
        
        ace_entry.ether.vid.u.s.data = 400;
        ace_entry.ether.vid.u.s.mask = 0x0fff;
        ace_entry.ether.vid.op = VAL_diffServMacAceVidOp_equal;

//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.ace_type = acl_type;
//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);

        ace_entry.access = RULE_TYPE_ACE_DENY;

//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);
    }

    for (i=0; i<_countof(exp_2); ++i)
    {
        EXP_GROUP *eg = &exp_2[i];
        result = L4_PMGR_ACL_BindPort2Acl(eg->lport,
                                          eg->acl_name,
                                          eg->acl_type,
                                          eg->inout_profile,
                                          NULL,
                                          RULE_TYPE_COUNTER_DISABLE);
        assert(result == RULE_TYPE_OK);
    }

    {
        char *acl_name = mac_5;
        RULE_TYPE_AclType_T acl_type = mac_5_type;
//        RULE_TYPE_Ace_Entry_T ace_entry;
        RULE_TYPE_UI_Ace_Entry_T ace_entry;

        result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
        assert(result == RULE_TYPE_OK);

        ace_entry.ace_type = acl_type;
//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);

//        ace_entry.u.mac.aceMinVid = 100;
//        ace_entry.u.mac.aceVidBitmask = 0x0fff;
//        ace_entry.u.mac.aceVidOp = VAL_diffServMacAceVidOp_equal;
        
        ace_entry.ether.vid.u.s.data = 100;
        ace_entry.ether.vid.u.s.mask = 0x0fff;
        ace_entry.ether.vid.op = VAL_diffServMacAceVidOp_equal;

//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);


        ace_entry.ace_type = acl_type;
//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);
        ace_entry.access = RULE_TYPE_ACE_DENY;

        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);
    }

    {
        char *acl_name = mac_6;
        RULE_TYPE_AclType_T acl_type = mac_6_type;
//        RULE_TYPE_Ace_Entry_T ace_entry;
        RULE_TYPE_UI_Ace_Entry_T ace_entry;

        result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
        assert(result == RULE_TYPE_OK);

        ace_entry.ace_type = acl_type;
//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);

//        ace_entry.u.mac.aceMinVid = 100;
//        ace_entry.u.mac.aceVidBitmask = 0x0fff;
//        ace_entry.u.mac.aceVidOp = VAL_diffServMacAceVidOp_equal;
        
        ace_entry.ether.vid.u.s.data = 100;
        ace_entry.ether.vid.u.s.mask = 0x0fff;
        ace_entry.ether.vid.op = VAL_diffServMacAceVidOp_equal;

//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);
        assert(result == RULE_TYPE_OK);

        ace_entry.ace_type = acl_type;
//        L4_PMGR_ACL_InitAce(&ace_entry);
        L4_PMGR_ACL_InitUIAce(&ace_entry);
        ace_entry.access = RULE_TYPE_ACE_DENY;

//        result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, acl_type, &ace_entry);

        assert(result == RULE_TYPE_OK);
    }

    for (i=0; i<_countof(exp_3); ++i)
    {
        EXP_GROUP *eg = &exp_3[i];
        result = L4_PMGR_ACL_BindPort2Acl(eg->lport,
                                          eg->acl_name,
                                          eg->acl_type,
                                          eg->inout_profile,
                                          NULL,
                                          RULE_TYPE_COUNTER_DISABLE);
        assert(result == RULE_TYPE_OK);
    }
#endif // SYS_CPNT_QOS_V2_EGRESS_PORT

    return 0;
}

//
// Failed to bind two dscp6 into a class-map.
//
int
CLI_ACL_UT_EPR_ES4626F_SW_FLF_38_01065()
{
    const char *class_map_name = "c2";
    RULE_TYPE_UI_ClassMapElement_T class_map_element;

    RULE_TYPE_RETURN_TYPE_T result;

    result = L4_MGR_QoS_CreateClassMapByName(class_map_name);
    assert(RULE_TYPE_OK == result);

    memset(&class_map_element, 0, sizeof(class_map_name));
    class_map_element.class_type = RULE_TYPE_CLASS_MF;
    class_map_element.element.mf_entry.mf_type = RULE_TYPE_MF_DSCP_IPV6;
    class_map_element.element.mf_entry.mf_value = 2;

    result = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &class_map_element);
    assert(RULE_TYPE_OK == result);


    memset(&class_map_element, 0, sizeof(class_map_name));
    class_map_element.class_type = RULE_TYPE_CLASS_MF;
    class_map_element.element.mf_entry.mf_type = RULE_TYPE_MF_DSCP_IPV6;
    class_map_element.element.mf_entry.mf_value = 3;

    result = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &class_map_element);
    assert(RULE_TYPE_OK == result);

    return 0;
}

int
CLI_ACL_UT_EPR_ES4626F_SW_FLF_38_01360()
{
#ifdef BROADCOM
#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) && 0 // no this issue

    UI32_T acl_id = 1;
    UI32_T lport;

    UI32_T result;

    for (acl_id=1; acl_id <= 16; ++acl_id)
    {
        char acl_name[SYS_ADPT_ACL_MAX_NAME_LEN + 1];
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_EXT_ACL;

        RULE_TYPE_Ace_Entry_T ace_entry;
        RULE_TYPE_AclType_T ace_type = RULE_TYPE_IP_ACL;

        UI32_T ace_count;
        UI32_T inc_val;

        snprintf(acl_name, sizeof(acl_name), "acl_%lu", acl_id);

        result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
        assert(result == RULE_TYPE_OK);

        ace_count = 0;
        inc_val = 1;
        while (TRUE)
        {
            ace_entry.ace_type = ace_type;
            L4_PMGR_ACL_InitAce(&ace_entry);
            ace_entry.access = RULE_TYPE_ACE_DENY;


            SET_IPV4(ace_entry.u.ip.aceSourceIpAddr, 192, 168, acl_id, inc_val);
            SET_IPV4(ace_entry.u.ip.aceSourceIpAddrBitmask, 255, 255, 255, 255);


            result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
            if (result != RULE_TYPE_OK)
            {
                break;
            }

            ace_count ++;
            inc_val ++;
        }

        assert(0 < ace_count);
        assert(SYS_ADPT_MAX_NBRS_OF_ACE == ace_count);
    }

    for (acl_id=16+1; acl_id <= (16+16); ++acl_id)
    {
        char acl_name[SYS_ADPT_ACL_MAX_NAME_LEN + 1];
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;

        RULE_TYPE_Ace_Entry_T ace_entry;
        RULE_TYPE_AclType_T ace_type = RULE_TYPE_MAC_ACL;

        UI32_T ace_count;
        UI32_T inc_val;

        snprintf(acl_name, sizeof(acl_name), "acl_%lu", acl_id);

        result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
        assert(result == RULE_TYPE_OK);

        ace_count = 0;
        inc_val = 1;
        while (TRUE)
        {
            ace_entry.ace_type = ace_type;
            L4_PMGR_ACL_InitAce(&ace_entry);
            ace_entry.access = RULE_TYPE_ACE_DENY;


            SET_MAC(ace_entry.u.mac.aceSourceMacAddr, 0, 0, 0, 0, acl_id, inc_val);
            SET_MAC(ace_entry.u.mac.aceSourceMacAddrBitmask, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);


            result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
            if (result != RULE_TYPE_OK)
            {
                break;
            }

            ace_count ++;
            inc_val ++;
        }

        assert(0 < ace_count);
        assert(SYS_ADPT_MAX_NBRS_OF_ACE == ace_count);
    }

//RULE_OM_SetDebugFlag(7);

    lport = 1;
    for (acl_id=1; acl_id <= 16; ++acl_id)
    {
        char acl_name[SYS_ADPT_ACL_MAX_NAME_LEN + 1];
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_ACL;

        snprintf(acl_name, sizeof(acl_name), "acl_%lu", acl_id);

        result = L4_PMGR_ACL_BindPort2Acl(lport, acl_name, acl_type,
                                          TRUE, // is_ingress
                                          NULL,
                                          RULE_TYPE_COUNTER_DISABLE);
        assert(result == RULE_TYPE_OK);

        lport ++;
    }

    lport = 1;
    for (acl_id=16+1; acl_id <= (16+16); ++acl_id)
    {
        char acl_name[SYS_ADPT_ACL_MAX_NAME_LEN + 1];
        RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;

        snprintf(acl_name, sizeof(acl_name), "acl_%lu", acl_id);

        result = L4_PMGR_ACL_BindPort2Acl(lport, acl_name, acl_type,
                                          TRUE, // is_ingress
                                          NULL,
                                          RULE_TYPE_COUNTER_DISABLE);
        assert(result == RULE_TYPE_OK);

        lport ++;
    }


#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE
#endif // BROADCOM

    return 0;
}

int
CLI_ACL_UT_C_String_Pass()
{
    enum
    {
        UPORT_IFINDEX = 5,
        DIRECTION     = RULE_TYPE_INBOUND,
        PADDING       = 10,

        ACL_TYPE      = RULE_TYPE_MAC_ACL,
    };

    UI32_T                      uport_ifindex = UPORT_IFINDEX;
    RULE_TYPE_AclType_T         acl_type = ACL_TYPE;
    BOOL_T                      is_ingress = TRUE;
    RULE_TYPE_COUNTER_ENABLE_T  counter_enable = RULE_TYPE_COUNTER_DISABLE;
    RULE_TYPE_InOutDirection_T  direction = DIRECTION;

    UI32_T                      acl_index;
    RULE_TYPE_Ace_Entry_T       ace_entry;

    UI32_T                      result;

    char acl_name[SYS_ADPT_ACL_MAX_NAME_LEN + PADDING];
    char class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + PADDING];
    char policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + PADDING];
    char time_range_name[SYS_ADPT_MAX_LENGTH_OF_TIME_ACL_TABLE_NAME + PADDING];

    memset(acl_name, 0, sizeof(acl_name));
    memset(class_map_name, 0, sizeof(class_map_name));
    memset(policy_map_name, 0, sizeof(policy_map_name));
    memset(time_range_name, 0, sizeof(time_range_name));

    //
    // ACL name
    //
    memset(acl_name, 'a', SYS_ADPT_ACL_MAX_NAME_LEN+1);
    memset(time_range_name, 't', SYS_ADPT_MAX_LENGTH_OF_TIME_ACL_TABLE_NAME);
    time_range_name[SYS_ADPT_MAX_LENGTH_OF_TIME_ACL_TABLE_NAME] = '\0';

    result = L4_PMGR_ACL_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);


    result = L4_PMGR_ACL_GetAclIdByNameAndType(acl_name, acl_type, &acl_index);
    assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);


    result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);


    result = L4_PMGR_ACL_DelAclByNameAndAclType(acl_name, acl_type);
    assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);


    result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
    assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);


    result = L4_PMGR_ACL_DelAceFromAcl(acl_name, acl_type, &ace_entry);
    assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);


#if (SYS_CPNT_ACL_MIRROR == TRUE)
    result = L4_PMGR_ACL_AddAclMirrorEntry(uport_ifindex, acl_name);
    assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);


    result = L4_PMGR_ACL_RemoveAclMirrorEntry(uport_ifindex, acl_name);
    assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);
#endif // SYS_CPNT_ACL_MIRROR


    result = L4_PMGR_ACL_BindPort2Acl(uport_ifindex, acl_name, acl_type, is_ingress,
                                      time_range_name, counter_enable);
    assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);


    result = L4_PMGR_ACL_BindPort2DynamicAcl(uport_ifindex, acl_name, acl_type, is_ingress);
    assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);


//    memset(acl_name, 'a', SYS_ADPT_ACL_MAX_NAME_LEN);
//    acl_name[SYS_ADPT_ACL_MAX_NAME_LEN] = '\0';
//
//    result = L4_PMGR_ACL_BindPort2Acl(uport_ifindex, acl_name, acl_type, is_ingress,
//                                      time_range_name, counter_enable);
//    assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);
//
//
//    memset(acl_name, 'a', SYS_ADPT_ACL_MAX_NAME_LEN+1);
    result = L4_PMGR_ACL_UnBindPortFromAcl(uport_ifindex, acl_name, acl_type, is_ingress);
    assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

    //
    // Policy map name
    //
    {
        UI32_T                      policy_map_index;
        RULE_TYPE_TBParamEntry_T    meter_entry;
        RULE_TYPE_UI_Action_T       action_entry;

        char policy_map_description[ SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + PADDING];


        memset(policy_map_name, 'p', sizeof(policy_map_name));

        memset(class_map_name, 'c', SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);
        class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';

        memset(policy_map_description, 'd', SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH);
        policy_map_description[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH] = '\0';


        result = L4_PMGR_QoS_BindPort2PolicyMap(uport_ifindex, policy_map_name, direction);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_UnBindPortFromPolicyMap(uport_ifindex, policy_map_name, direction);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_BindPort2DynamicPolicyMap(uport_ifindex, policy_map_name, direction);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_CreatePolicyMapByName(policy_map_name);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_RenamePolicyMap(policy_map_name, policy_map_name);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_SetPolicyMapDescByName(policy_map_name, policy_map_description);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_DelPolicyMapByName(policy_map_name);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_AddElement2PolicyMap(policy_map_name, class_map_name, &meter_entry, &action_entry);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_DelElementMeterFromPolicyMap(policy_map_name, class_map_name, &meter_entry);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_DelElementActionFromPolicyMap(policy_map_name, class_map_name, &action_entry);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_DelElementFromPolicyMap(policy_map_name, class_map_name);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_SetDiffServPolicyMapName(policy_map_index, policy_map_name);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);
    }

    {
        UI32_T policy_map_index;
        char policy_map_description[ SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + PADDING];

        memset(policy_map_description, 'd', sizeof(policy_map_description));

        result = L4_PMGR_QoS_SetDiffServPolicyMapDescription(policy_map_index, policy_map_description);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);
    }


    //
    // Class map name
    //
    {
        UI32_T class_map_index;
        RULE_TYPE_UI_ClassMapElement_T ui_class_map_element;

        memset(class_map_name, 'c', sizeof(class_map_name));

        result = L4_PMGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_CreateClassMapByName(class_map_name);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_RenameClassMap(class_map_name, class_map_name);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_DelClassMap(class_map_name);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_AddElement2ClassMap(class_map_name, &ui_class_map_element);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_DelElementFromClassMap(class_map_name, &ui_class_map_element);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);

        result = L4_PMGR_QoS_SetDiffServClassMapName(class_map_index, class_map_name);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);
    }

    {
        UI32_T class_map_index;
        char class_map_description[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + PADDING];

        memset(class_map_description, 'd', sizeof(class_map_description));

        L4_PMGR_QoS_SetDiffServClassMapDescription(class_map_index, class_map_description);
        assert(RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER == result);
    }


    return 0;
}

int
CLI_ACL_UT_EPR_ASE3526MP_FLF_EC_00014()
{
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
//    CLI_TASK_WorkingArea_T ctrl;

//    UI16_T cmd_idx;
//    char *arg[255] =
//    {
//        NULL
//    };

    //
    // access-list mac macOut
    // deny any host 00-00-e8-00-00-01
    // deny any host 00-00-e8-22-22-22
    // exit
    // in e 1/2
    // mac access-group macOut out
    //
    const char *acl_name = "macOut";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
    RULE_TYPE_Ace_Entry_T ace_entry;

    UI32_T result;

    result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);

    ace_entry.ace_type = acl_type;
    result = L4_PMGR_ACL_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.access = RULE_TYPE_ACE_DENY;
    
    ace_entry.u.mac.aceDestMacAddr[0] = 0x0;
    ace_entry.u.mac.aceDestMacAddr[1] = 0x0;
    ace_entry.u.mac.aceDestMacAddr[2] = 0xe8;
    ace_entry.u.mac.aceDestMacAddr[3] = 0x0;
    ace_entry.u.mac.aceDestMacAddr[4] = 0x0;
    ace_entry.u.mac.aceDestMacAddr[5] = 0x1;

    memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff,
                                 sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

    result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
    assert(RULE_TYPE_OK == result);


    ace_entry.ace_type = acl_type;
    result = L4_PMGR_ACL_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.access = RULE_TYPE_ACE_DENY;

    ace_entry.u.mac.aceDestMacAddr[0] = 0x0;
    ace_entry.u.mac.aceDestMacAddr[1] = 0x0;
    ace_entry.u.mac.aceDestMacAddr[2] = 0xe8;
    ace_entry.u.mac.aceDestMacAddr[3] = 0x22;
    ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
    ace_entry.u.mac.aceDestMacAddr[5] = 0x22;

    memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff,
                                 sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

    result = L4_PMGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    {
        UI32_T uport_ifindex = 2;
        BOOL_T is_ingress = FALSE;
        const char *time_rnage_name = NULL;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

        result = L4_PMGR_ACL_BindPort2Acl(uport_ifindex, acl_name, acl_type,
                                   is_ingress, time_rnage_name, counter_enable);
        assert(RULE_TYPE_OK == result);
    }


    {
        UI32_T uport_ifindex = 2;
        RULE_TYPE_InOutDirection_T direction = RULE_TYPE_OUTBOUND;
        RULE_TYPE_UI_AclEntry_T acl_entry;

        UI32_T acl_index;
        UI32_T precedence;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable;
        char  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];

        int i_result;

        acl_index = 0;
        precedence = 0;
        acl_entry.acl_type = acl_type;

        result = L4_PMGR_ACL_GetNextAclByPort(uport_ifindex, direction,
                                              &acl_index, &acl_entry,
                                              &precedence, time_range_name, &counter_enable);
        assert(RULE_TYPE_OK == result);

        i_result = strcmp(acl_entry.acl_name, acl_name);
        assert(0 == i_result);

        {
            UI32_T port_acl_index;
            UI8_T  port_time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];
            RULE_TYPE_COUNTER_ENABLE_T port_counter_enable;

            result = RULE_OM_GetPortAclIndex(uport_ifindex, RULE_TYPE_MAC_ACL,
                                             direction == RULE_TYPE_INBOUND ? TRUE : FALSE,
                                             &port_acl_index,
                                             port_time_range_name,
                                             &port_counter_enable);
            assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
            {
                RULE_TYPE_Acl_T *port_acl_entry_p;

                port_acl_entry_p = RULE_OM_LocalGetAclByIndex(port_acl_index);
                assert(NULL != port_acl_entry_p);

                assert(TRUE == port_acl_entry_p->flag_aggregate_done);
                assert(0 != port_acl_entry_p->aggregate_acl_index);

                port_acl_index = port_acl_entry_p->aggregate_acl_index;
            }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

            {
                RULE_TYPE_INTERFACE_INFO_T interface;

                RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
                RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

                RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;

                result = RULE_CTRL_Ifindex_To_Interface(uport_ifindex, &interface);
                assert(RULE_TYPE_OK == result);

                interface.direction = direction;

                acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, port_acl_index);
                assert(NULL != acl_inst_p);

                RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);
                acl_inst_obj.get_element_iterator(&acl_inst_obj, &ace_inst_iter);

                assert(2 == ace_inst_iter.count(&ace_inst_iter));
            }
        }
    }

    //
    // no deny any host 00-00-e8-22-22-22
    //
    ace_entry.ace_type = acl_type;
    result = L4_PMGR_ACL_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.access = RULE_TYPE_ACE_DENY;

    ace_entry.u.mac.aceDestMacAddr[0] = 0x0;
    ace_entry.u.mac.aceDestMacAddr[1] = 0x0;
    ace_entry.u.mac.aceDestMacAddr[2] = 0xe8;
    ace_entry.u.mac.aceDestMacAddr[3] = 0x22;
    ace_entry.u.mac.aceDestMacAddr[4] = 0x22;
    ace_entry.u.mac.aceDestMacAddr[5] = 0x22;

    memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff,
                                 sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

    result = L4_MGR_ACL_DelAceFromAcl(acl_name, acl_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    {
        UI32_T uport_ifindex = 2;
        RULE_TYPE_InOutDirection_T direction = RULE_TYPE_OUTBOUND;

        UI32_T port_acl_index;
        UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
        RULE_TYPE_COUNTER_ENABLE_T port_counter_enable;

        result = RULE_OM_GetPortAclIndex(uport_ifindex, RULE_TYPE_MAC_ACL,
                                         direction == RULE_TYPE_INBOUND ? TRUE : FALSE,
                                         &port_acl_index,
                                         time_range_name,
                                         &port_counter_enable);
        assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        {
            RULE_TYPE_Acl_T *port_acl_entry_p;

            port_acl_entry_p = RULE_OM_LocalGetAclByIndex(port_acl_index);
            assert(NULL != port_acl_entry_p);

            assert(TRUE == port_acl_entry_p->flag_aggregate_done);
            assert(0 != port_acl_entry_p->aggregate_acl_index);

            port_acl_index = port_acl_entry_p->aggregate_acl_index;
        }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

        {
            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;

            result = RULE_CTRL_Ifindex_To_Interface(uport_ifindex, &interface);
            assert(RULE_TYPE_OK == result);

            interface.direction = direction;

            acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, port_acl_index);
            assert(NULL != acl_inst_p);

            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);
            acl_inst_obj.get_element_iterator(&acl_inst_obj, &ace_inst_iter);

            assert(1 == ace_inst_iter.count(&ace_inst_iter));
        }
    }

    result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    {
        UI32_T uport_ifindex = 2;
        RULE_TYPE_InOutDirection_T direction = RULE_TYPE_OUTBOUND;

        UI32_T port_acl_index;
        UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
        RULE_TYPE_COUNTER_ENABLE_T port_counter_enable;

        result = RULE_OM_GetPortAclIndex(uport_ifindex, RULE_TYPE_MAC_ACL,
                                         direction == RULE_TYPE_INBOUND ? TRUE : FALSE,
                                         &port_acl_index,
                                         time_range_name,
                                         &port_counter_enable);
        assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
        {
            RULE_TYPE_Acl_T *port_acl_entry_p;

            port_acl_entry_p = RULE_OM_LocalGetAclByIndex(port_acl_index);
            assert(NULL != port_acl_entry_p);

            assert(TRUE == port_acl_entry_p->flag_aggregate_done);
            assert(0 != port_acl_entry_p->aggregate_acl_index);

            port_acl_index = port_acl_entry_p->aggregate_acl_index;
        }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

        {
            RULE_TYPE_INTERFACE_INFO_T interface;

            RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
            RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

            RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;

            result = RULE_CTRL_Ifindex_To_Interface(uport_ifindex, &interface);
            assert(RULE_TYPE_OK == result);

            interface.direction = direction;

            acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, port_acl_index);
            assert(NULL != acl_inst_p);

            RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);
            acl_inst_obj.get_element_iterator(&acl_inst_obj, &ace_inst_iter);

            assert(2 == ace_inst_iter.count(&ace_inst_iter));
        }
    }

    //
    // Spend too much time.
    //
    if (1)
    {
        UI32_T pos = 0;
        UI32_T i = 1;

        UI32_T rule_num = 2;

        //
        // Add until error occur
        //
        while (TRUE)
        {
            BOOL_T leave_while = FALSE;

            ace_entry.ace_type = acl_type;
            result = L4_PMGR_ACL_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            ace_entry.access = RULE_TYPE_ACE_DENY;

            memset(ace_entry.u.mac.aceDestMacAddr, 0,
                                        sizeof(ace_entry.u.mac.aceDestMacAddr));

            //
            // Make sure the ACE will not be compressed
            //
            ace_entry.u.mac.aceDestMacAddr[pos++ % sizeof(ace_entry.u.mac.aceDestMacAddr) ] = i++;

            memset(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff,
                                 sizeof(ace_entry.u.mac.aceDestMacAddrBitmask));

            result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);

            if (RULE_TYPE_OK == result)
            {
                rule_num ++;
            }
            else
            {
                leave_while = TRUE;
            }

            {
                UI32_T uport_ifindex = 2;
                RULE_TYPE_InOutDirection_T direction = RULE_TYPE_OUTBOUND;

                UI32_T port_acl_index;
                UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T port_counter_enable;

                result = RULE_OM_GetPortAclIndex(uport_ifindex, RULE_TYPE_MAC_ACL,
                                                 direction == RULE_TYPE_INBOUND ? TRUE : FALSE,
                                                 &port_acl_index,
                                                 time_range_name,
                                                 &port_counter_enable);
                assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
                {
                    RULE_TYPE_Acl_T *port_acl_entry_p;

                    port_acl_entry_p = RULE_OM_LocalGetAclByIndex(port_acl_index);
                    assert(NULL != port_acl_entry_p);

                    assert(TRUE == port_acl_entry_p->flag_aggregate_done);
                    assert(0 != port_acl_entry_p->aggregate_acl_index);

                    port_acl_index = port_acl_entry_p->aggregate_acl_index;
                }
#endif // SYS_CPNT_ACL_AUTO_COMPRESS_ACE

                {
                    RULE_TYPE_INTERFACE_INFO_T interface;

                    RULE_TYPE_CLASS_INSTANCE_PTR_T acl_inst_p;
                    RULE_TYPE_CLASS_INSTANCE_OBJECT_T acl_inst_obj;

                    RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T ace_inst_iter;

                    result = RULE_CTRL_Ifindex_To_Interface(uport_ifindex, &interface);
                    assert(RULE_TYPE_OK == result);

                    interface.direction = direction;
                    
                    acl_inst_p = RULE_CTRL_Priv_GetAclInstanceById(&interface, port_acl_index);
                    assert(NULL != acl_inst_p);
                    
                    RULE_CTRL_IN_Priv_ConstructClassInstanceObject(acl_inst_p, &acl_inst_obj);
                    acl_inst_obj.get_element_iterator(&acl_inst_obj, &ace_inst_iter);
                    
                    assert(rule_num == ace_inst_iter.count(&ace_inst_iter));
                }
            }

            if (TRUE == leave_while)
            {
                break;
            }
        } // while
    }

#endif //SYS_CPNT_QOS_V2_EGRESS_PORT

    return 0;
}

int
CLI_ACL_UT_EPR_ASE3526MP_FLF_EC_00025()
{
    typedef struct
    {
        char acl_name[ SYS_ADPT_ACL_MAX_NAME_LEN+1 ];
        RULE_TYPE_AclType_T acl_type;
        UI32_T acl_index;

        struct
        {
            RULE_TYPE_AclType_T acl_type;
            UI32_T uport_ifindex;
        } bind;
    } EXP_GROUP;

    EXP_GROUP   exp_grp[3] =
    {
        {"aaa", RULE_TYPE_IPV6_EXT_ACL, 0, {RULE_TYPE_IPV6_ACL, 1}},
        {"bbb", RULE_TYPE_IPV6_EXT_ACL, 0, {RULE_TYPE_IPV6_ACL, 2}},
        {"ccc", RULE_TYPE_IPV6_EXT_ACL, 0, {RULE_TYPE_IPV6_ACL, 3}},
    };

    UI32_T i;
    RULE_TYPE_RETURN_TYPE_T result;

    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    char *arg[255] =
    {
        NULL
    };

    for (i=0; i < _countof(exp_grp); ++i)
    {
        BOOL_T is_ingress = TRUE;
        char *time_rnage_name = NULL;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

        result = L4_PMGR_ACL_CreateAcl(exp_grp[i].acl_name, exp_grp[i].acl_type);
        assert(RULE_TYPE_OK == result);

        result = L4_PMGR_ACL_GetAclIdByName(exp_grp[i].acl_name, &exp_grp[i].acl_index);
        assert(RULE_TYPE_OK == result);

// RULE_OM_SetDebugFlag(7);

        result = L4_PMGR_ACL_BindPort2Acl(exp_grp[i].bind.uport_ifindex,
                                          exp_grp[i].acl_name,
                                          exp_grp[i].bind.acl_type,
                                          is_ingress,
                                          time_rnage_name,
                                          counter_enable);
        assert(RULE_TYPE_OK == result);
    }


    //
    // show ipv6 access-group
    //
    cmd_idx = PRIVILEGE_EXEC_CMD_W3_SHOW_IPV6_ACCESSGROUP;
    CLI_API_Show_IP_AccessGroup(cmd_idx, arg, &ctrl);


    //
    // interface ethernet 1/1
    //
    ctrl.sys_info.my_unit_id = 1;

    arg[0] = "1/1";
    CLI_API_Interface_Ethernet(cmd_idx, arg, &ctrl);


    //
    // no ipv6 access-group ccc
    //   --> SHOULD NOT BE REMOVED, due to wrong ACL name
    //
    cmd_idx = PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_IPV6_ACCESSGROUP;

    arg[0] = exp_grp[2].acl_name;
    arg[1] = "ingress";

    CLI_API_IP_AccessGroup(cmd_idx, arg, &ctrl);

    //
    // show ipv6 access-group
    //
    cmd_idx = PRIVILEGE_EXEC_CMD_W3_SHOW_IPV6_ACCESSGROUP;
    CLI_API_Show_IP_AccessGroup(cmd_idx, arg, &ctrl);

    //
    // after fix ~~~
    // Check bind ACL on interface 1/1
    //
    {
        UI32_T uport_ifindex = 1;
        RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;
        RULE_TYPE_UI_AclEntry_T acl_entry;

        UI32_T acl_index;
        UI32_T precedence;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable;
        char  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];

        int i_result;

        acl_index = 0;
        precedence = 0;
        acl_entry.acl_type = exp_grp[0].acl_type;

        result = L4_PMGR_ACL_GetNextAclByPort(uport_ifindex, direction,
                                              &acl_index, &acl_entry,
                                              &precedence, time_range_name, &counter_enable);
        assert(RULE_TYPE_OK == result);

        i_result = strcmp(acl_entry.acl_name, exp_grp[0].acl_name);
        assert(0 == i_result);
    }


    //
    // interface ethernet 1/1
    //
    ctrl.sys_info.my_unit_id = 1;

    arg[0] = "1/1";
    CLI_API_Interface_Ethernet(cmd_idx, arg, &ctrl);

    //
    // no ipv6 access-group aaa
    //
    cmd_idx = PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_IPV6_ACCESSGROUP;

    arg[0] = exp_grp[0].acl_name;
    arg[1] = "ingress";

    CLI_API_IP_AccessGroup(cmd_idx, arg, &ctrl);

    //
    // show ipv6 access-group
    //
    cmd_idx = PRIVILEGE_EXEC_CMD_W3_SHOW_IPV6_ACCESSGROUP;
    CLI_API_Show_IP_AccessGroup(cmd_idx, arg, &ctrl);

    {
        UI32_T uport_ifindex = 1;
        RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;
        RULE_TYPE_UI_AclEntry_T acl_entry;

        UI32_T acl_index;
        UI32_T precedence;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable;
        char  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];

        acl_index = 0;
        precedence = 0;
        acl_entry.acl_type = exp_grp[0].acl_type;

        result = L4_PMGR_ACL_GetNextAclByPort(uport_ifindex, direction,
                                              &acl_index, &acl_entry,
                                              &precedence, time_range_name, &counter_enable);
        assert(RULE_TYPE_OK != result);
    }


    //
    // ipv6 access-group aaa
    //
    cmd_idx = PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_IPV6_ACCESSGROUP;

    arg[0] = exp_grp[0].acl_name;
    arg[1] = "ingress";

    CLI_API_IP_AccessGroup(cmd_idx, arg, &ctrl);

    // (Replace aaa to bbb)
    //
    // ipv6 access-group bbb
    //
    cmd_idx = PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_IPV6_ACCESSGROUP;

    arg[0] = exp_grp[1].acl_name;
    arg[1] = "ingress";

    CLI_API_IP_AccessGroup(cmd_idx, arg, &ctrl);

    cmd_idx = PRIVILEGE_EXEC_CMD_W3_SHOW_IPV6_ACCESSGROUP;
    CLI_API_Show_IP_AccessGroup(cmd_idx, arg, &ctrl);

    //
    // after fix ~~~
    // Check bind ACL on interface 1/1
    //
    {
        UI32_T uport_ifindex = 1;
        RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;
        RULE_TYPE_UI_AclEntry_T acl_entry;

        UI32_T acl_index;
        UI32_T precedence;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable;
        char  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];

        int i_result;

        acl_index = 0;
        precedence = 0;
        acl_entry.acl_type = exp_grp[0].acl_type;

        result = L4_PMGR_ACL_GetNextAclByPort(uport_ifindex, direction,
                                              &acl_index, &acl_entry,
                                              &precedence, time_range_name, &counter_enable);
        assert(RULE_TYPE_OK == result);

        i_result = strcmp(acl_entry.acl_name, exp_grp[1].acl_name);
        assert(0 == i_result);
    }


    return 0;
}

int
CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00160()
{
    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    char *arg[255] =
    {
        NULL
    };

    printf("\r\n============================================="
           "\r\n= Check the format of TCAM Utilization here ="
           "\r\n=============================================\r\n");
    CLI_API_Show_AccessList_TcamUtilization(cmd_idx, arg, &ctrl);

    return 0;
}

int
CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00394()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)

    const char *class_map_name = "c1-match-all";
    RULE_TYPE_MatchType_T match_type = RULE_TYPE_CLASS_MAP_MATCH_ALL;

    RULE_TYPE_UI_ClassMapElement_T ui_class_map_element;
    RULE_TYPE_UI_ClassMap_T        ui_class_map;

    const char *policy_map_name = "p1";
    RULE_TYPE_UI_Action_T ui_action;

    RULE_TYPE_RETURN_TYPE_T result;


    result = L4_MGR_QoS_CreateClassMapByName(class_map_name);
    assert(RULE_TYPE_OK == result);

    memset(&ui_class_map, 0, sizeof(ui_class_map));

    strncpy(ui_class_map.class_map_name, class_map_name, sizeof(ui_class_map.class_map_name)-1);
    ui_class_map.class_map_name[ sizeof(ui_class_map.class_map_name)-1 ] = '\0';

    result = L4_MGR_QoS_GetClassMapByName(&ui_class_map);
    assert(RULE_TYPE_OK == result);

    ui_class_map.class_map_match_type = match_type;

    result = L4_MGR_QoS_SetClassMap(&ui_class_map);
    assert(RULE_TYPE_OK == result);


    ui_class_map_element.class_type = RULE_TYPE_CLASS_MF;
    ui_class_map_element.element.mf_entry.mf_value = 10;
    ui_class_map_element.element.mf_entry.mf_type = RULE_TYPE_MF_DSCP;

    result = L4_MGR_QoS_AddElement2ClassMap(class_map_name, &ui_class_map_element);
    assert(RULE_TYPE_OK == result);

    result = L4_MGR_QoS_CreatePolicyMapByName(policy_map_name);
    assert(RULE_TYPE_OK == result);

    memset(&ui_action, 0, sizeof(ui_action));

    ui_action.in_action_type = IN_ACTION_INVALID;
    ui_action.in_action_value= 8;

    ui_action.confirm_action_type = CONFIRM_ACTION_INVALID;
    ui_action.confirm_action_value = 64;

    ui_action.exceed_action_type = EXCEED_ACTION_INVALID;
    ui_action.exceed_action_value = 64;

    ui_action.violate_action_type = VIOLATE_ACTION_INVALID;
    ui_action.violate_action_value = 64;

    ui_action.in_action_type = IN_ACTION_NEW_COS;
    ui_action.in_action_value = 7;

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    ui_action.class_action_type = CLASS_ACTION_INVALID;
#endif // SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL

    result = L4_MGR_QoS_AddElement2PolicyMap(policy_map_name, class_map_name, NULL, &ui_action);
    assert(RULE_TYPE_OK == result);

    {
        UI32_T uport_ifindex = 4;
        RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

        result = L4_MGR_QoS_BindPort2PolicyMap(uport_ifindex, policy_map_name, direction);
        assert(RULE_TYPE_OK == result);
    }


#endif // SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL

    return 0;
}

int
CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00400()
{
    const char *acl_name = "v6std-1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IPV6_STD_ACL;

    RULE_TYPE_Ace_Entry_T ace_entry;

    UI32_T result;

    result = L4_PMGR_ACL_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);

    ace_entry.ace_type = acl_type;
    result = L4_MGR_ACL_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    {
        const char *ipv6_addr = "2001::1/127";

        L_INET_AddrIp_T ipaddr;

        L_INET_StringToInaddr(L_INET_FORMAT_IPV6_ADDR_RAW, ipv6_addr,
                              (L_INET_Addr_T *) &ipaddr, sizeof(ipaddr));

        memcpy(&ace_entry.u.ipv6.aceSourceIpAddr, &ipaddr.addr,
                                      sizeof(ace_entry.u.ipv6.aceSourceIpAddr));
        RULE_OM_LocalConvertIpv6Prefix2Mask(ipaddr.preflen,
                                            ace_entry.u.ipv6.aceSourceIpAddrBitmask);
        //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = ipaddr.preflen;

        result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = L4_MGR_ACL_DelAceFromAcl(acl_name, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);
    }

    {
        const char *ipv6_addr = "2001::1/1";

        L_INET_AddrIp_T ipaddr;
        
        L_INET_StringToInaddr(L_INET_FORMAT_IPV6_ADDR_RAW, ipv6_addr,
                              (L_INET_Addr_T *)&ipaddr, sizeof(ipaddr));

        memcpy(&ace_entry.u.ipv6.aceSourceIpAddr, &ipaddr.addr,
                                      sizeof(ace_entry.u.ipv6.aceSourceIpAddr));

        RULE_OM_LocalConvertIpv6Prefix2Mask(ipaddr.preflen,
                                            ace_entry.u.ipv6.aceSourceIpAddrBitmask);
        //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = ipaddr.preflen;

        result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = L4_MGR_ACL_DelAceFromAcl(acl_name, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);
    }

    {
        const char *ipv6_addr = "2001::1/128";

        L_INET_AddrIp_T ipaddr;

        L_INET_StringToInaddr(L_INET_FORMAT_IPV6_ADDR_RAW, ipv6_addr,
                              (L_INET_Addr_T *)&ipaddr, sizeof(ipaddr));

        memcpy(&ace_entry.u.ipv6.aceSourceIpAddr, &ipaddr.addr,
                                      sizeof(ace_entry.u.ipv6.aceSourceIpAddr));
        RULE_OM_LocalConvertIpv6Prefix2Mask(ipaddr.preflen,
                                            ace_entry.u.ipv6.aceSourceIpAddrBitmask);
        //ace_entry.u.ipv6.aceSourceIpAddrPrefixLen = ipaddr.preflen;

        result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        result = L4_MGR_ACL_DelAceFromAcl(acl_name, acl_type, &ace_entry);
        assert(RULE_TYPE_OK == result);
    }

    return 0;
}

int
CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00397()
{
    CLI_TASK_WorkingArea_T ctrl;

    const char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IPV6_STD_ACL;

    BOOL_T b_result;

    b_result = CLI_ACL_PrivEnterAccessListMode(acl_name, acl_type, &ctrl);
    assert(TRUE == b_result);

    b_result = CLI_ACL_PrivEnterAccessListMode(acl_name, acl_type, &ctrl);
    assert(TRUE == b_result);


    return 0;
}

int
CLI_ACL_UT_EPR_ASF4628BBS5_FLF_EC_00073()
{
    char *acl_name = "a1";
    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    char *arg[255] =
    {
        NULL
    };

    cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W2_ACCESSLIST_MAC;
    arg[0] = acl_name;

    CLI_API_ACCESSLIST_MAC(cmd_idx, arg, &ctrl);

    cmd_idx = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT;
    arg[0] = "untagged-802.3";
    arg[1] = "any";
    arg[2] = "any";
    CLI_API_Permit_ACL_MAC(cmd_idx, arg, &ctrl);

    {
        UI32_T acl_index;
        RULE_TYPE_Acl_T acl_entry;

        RULE_TYPE_RETURN_TYPE_T result;

        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
        assert(RULE_TYPE_OK == result);

        assert(1 == acl_entry.ace_count);
    }

    return 0;
}

int
CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00881()
{
#if (SYS_CPNT_ACL_COUNTER == TRUE)
    const char *acl_name = "a1";
    UI32_T acl_index;
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_EXT_ACL;

    RULE_TYPE_Ace_Entry_T ace_entry;

    RULE_TYPE_RETURN_TYPE_T result;

    result = L4_MGR_ACL_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    ace_entry.ace_type = acl_type;
    result = RULE_OM_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);

    ace_entry.access = RULE_TYPE_ACE_DENY;

    result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
    assert(RULE_TYPE_OK == result);

    {
        UI32_T ifindex = 5;
        RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;
        const char *time_range_name = NULL;
        RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;

        CLI_ACL_UT_Proc_BindACLOnPort(ifindex, acl_name, acl_type, direction, time_range_name, counter_enable);

        counter_enable = RULE_TYPE_COUNTER_ENABLE;
        CLI_ACL_UT_Proc_BindACLOnPort(ifindex, acl_name, acl_type, direction, time_range_name, counter_enable);

        {
            UI32_T result_acl_index = 0;
            UI32_T precedence = 0;
            UI8_T  time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
            RULE_TYPE_COUNTER_ENABLE_T result_counter_enable;
            RULE_TYPE_UI_AclEntry_T acl_entry;

            result = L4_MGR_ACL_GetNextAclByPort(ifindex, direction,
                                                 &result_acl_index,
                                                 &acl_entry, &precedence,
                                                 time_range_name,
                                                 &result_counter_enable);
            assert(RULE_TYPE_OK == result);
            assert(result_counter_enable == counter_enable);
        }
    }

#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

int
CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00902()
{
//FIXME: time range
#if 0 && (SYS_CPNT_TIME_BASED_ACL == TRUE) && (SYS_CPNT_ACL_COUNTER == TRUE)

    typedef struct
    {
        const char *name;
        RULE_TYPE_AclType_T type;
        UI32_T index;

        const char *time_range_name;
        UI16_T time_range_index;

        UI16_T do_cmd_idx;
        UI16_T undo_cmd_idx;
        UI32_T (*cmd_fn)(UI16_T, char **, CLI_TASK_WorkingArea_T *);
    } acl_set_t;

    acl_set_t acl_set[] =
    {
        {
            .name = "a1",
            .type = RULE_TYPE_IP_EXT_ACL,
            .time_range_name = "r1",
            .do_cmd_idx = PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_IP_ACCESSGROUP,
            .undo_cmd_idx = PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_IP_ACCESSGROUP,
            .cmd_fn = CLI_API_IP_AccessGroup,
        },

#if (SYS_CPNT_ACL_IPV6 == TRUE)

        {
            .name = "a2",
            .type = RULE_TYPE_IPV6_EXT_ACL,
            .time_range_name = "r2",
            .do_cmd_idx = PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_IPV6_ACCESSGROUP,
            .undo_cmd_idx = PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_IPV6_ACCESSGROUP,
            .cmd_fn = CLI_API_IP_AccessGroup,
        },

#endif // SYS_CPNT_ACL_IPV6

        {
            .name = "a3",
            .type = RULE_TYPE_MAC_ACL,
            .time_range_name = "r3",
            .do_cmd_idx = PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_MAC_ACCESSGROUP,
            .undo_cmd_idx = PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_MAC_ACCESSGROUP,
            .cmd_fn = CLI_API_MAC_AccessGroup_Eth,
        },
    };

    RULE_TYPE_Ace_Entry_T ace_entry;

    RULE_TYPE_RETURN_TYPE_T result;
    UI32_T i;

    for (i=0; i < _countof(acl_set); ++i)
    {
        acl_set_t *acl = &acl_set[i];

        result = L4_MGR_ACL_CreateAcl(acl->name, acl->type);
        assert(RULE_TYPE_OK == result);

        result = RULE_OM_GetAclIdByName(acl->name, &acl->index);
        assert(RULE_TYPE_OK == result);

        ace_entry.ace_type = acl->type;
        result = RULE_OM_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);

        result = L4_MGR_ACL_SetAce2Acl(acl->name, acl->type, &ace_entry);
        assert(RULE_TYPE_OK == result);

        {
            UI32_T ifindex = 2;
            RULE_TYPE_InOutDirection_T direction = RULE_TYPE_INBOUND;

            CLI_TASK_WorkingArea_T ctrl;
            char *arg[256] = {NULL};
            UI16_T cmd_idx;

            ctrl.sys_info.my_unit_id = 1;

            arg[0] = "1/2";
            CLI_API_Interface_Ethernet(cmd_idx, arg, &ctrl);

            memset(arg, 0, sizeof(arg));

            arg[0] = (char *)acl->name;
            arg[1] = "in";
            arg[2] = "time-range";
            arg[3] = (char *)acl->time_range_name;
            arg[4] = "counter";

            acl->cmd_fn(acl->do_cmd_idx, arg, &ctrl);

            memset(arg, 0, sizeof(arg));

            {
                UI32_T result_acl_index = 0;
                UI32_T precedence = 0;
                UI8_T  result_time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T result_counter_enable;
                RULE_TYPE_UI_AclEntry_T acl_entry;

                result = L4_MGR_ACL_GetNextAclByPort(ifindex, direction,
                                                     &result_acl_index,
                                                     &acl_entry, &precedence,
                                                     result_time_range_name,
                                                     &result_counter_enable);
                assert(RULE_TYPE_OK == result);
                assert(0 == strcmp((char *)result_time_range_name, acl->time_range_name));
                assert(result_counter_enable == RULE_TYPE_COUNTER_ENABLE);
            }

            arg[0] = (char *)acl->name;
            arg[1] = "in";

            acl->cmd_fn(acl->undo_cmd_idx, arg, &ctrl);

            memset(arg, 0, sizeof(arg));

            {
                UI32_T result_acl_index = 0;
                UI32_T precedence = 0;
                UI8_T  result_time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T result_counter_enable;
                RULE_TYPE_UI_AclEntry_T acl_entry;

                result = L4_MGR_ACL_GetNextAclByPort(ifindex, direction,
                                                     &result_acl_index,
                                                     &acl_entry, &precedence,
                                                     result_time_range_name,
                                                     &result_counter_enable);
                assert(RULE_TYPE_OK != result);
            }

        }
    }

#endif // SYS_CPNT_ACL_COUNTER

    return 0;
}

int
CLI_ACL_UT_Crash_When_Bind_Time_Range_on_MAC_ACL()
{
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    char *acl_name = "a1";
    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    char *arg[255] =
    {
        NULL
    };

    cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W2_ACCESSLIST_MAC;
    arg[0] = acl_name;

    CLI_API_ACCESSLIST_MAC(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY;

    arg[0] = "any";
    arg[1] = "host";
    arg[2] = "00-11-22-33-44-55";
    arg[3] = "ethertype";
    arg[4] = "0800";
    arg[5] = "time-range";
    arg[6] = "cpu";
    CLI_API_Permit_ACL_MAC(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY;

    arg[0] = "any";
    arg[1] = "host";
    arg[2] = "00-11-22-33-44-55";
    arg[3] = "vid";
    arg[4] = "4094";
    arg[5] = "time-range";
    arg[6] = "cpu";
    CLI_API_Permit_ACL_MAC(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));


//    {
//        UI32_T acl_index;
//        RULE_TYPE_Acl_T acl_entry;
//
//        RULE_TYPE_RETURN_TYPE_T result;
//
//        result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
//        assert(RULE_TYPE_OK == result);
//
//        result = RULE_OM_GetAclByIndex(acl_index, &acl_entry);
//        assert(RULE_TYPE_OK == result);
//
//        assert(1 == acl_entry.ace_count);
//    }

#endif /* SYS_CPNT_TIME_BASED_ACL */
    return 0;
}

int
CLI_ACL_UT_EPR_ASF4628BBS5_FLF_EC_00360()
{
    char *acl_name = "a1";
    UI32_T acl_index;

    RULE_TYPE_RETURN_TYPE_T result;

    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    UI32_T arg_pos;
    char *arg[255] =
    {
        NULL
    };

    cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W3_ACCESSLIST_IPV6_EXTENDED;
    arg[0] = acl_name;

    CLI_API_AccessList_IPV6(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT;

    arg_pos = 0;
#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
    arg[arg_pos ++] = "10::/64";
#endif // SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
    arg[arg_pos ++] = "10::/8";
#endif // SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR

    arg[arg_pos ++] = "next-header";
    arg[arg_pos ++] = "51";
    arg[arg_pos ++] = "dscp";
    arg[arg_pos ++] = "10";
    arg[arg_pos ++] = "flow-label";
    arg[arg_pos ++] = "100";
    CLI_API_Permit_ACL_Extended_IPV6(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    {
        RULE_TYPE_AclType_T ace_type;
        UI32_T ace_index;

        RULE_TYPE_Ace_Entry_T ace_entry;

        ace_index = 0;
        result = RULE_OM_GetNextAceByAcl(acl_index, &ace_type, &ace_index, &ace_entry);
        assert(RULE_TYPE_OK == result);

#if (SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR == TRUE)
        assert(0x00 == ace_entry.u.ipv6.aceSourceIpAddr[0]);
        assert(0x10 == ace_entry.u.ipv6.aceSourceIpAddr[1]);
#endif // SYS_CPNT_ACL_IPV6_EXT_SRC_IP_ADDR

#if (SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR == TRUE)
        assert(0x00 == ace_entry.u.ipv6.aceDestIpAddr[0]);
        assert(0x10 == ace_entry.u.ipv6.aceDestIpAddr[1]);
#endif // SYS_CPNT_ACL_IPV6_EXT_DST_IP_ADDR
    }

    return 0;
}

int
CLI_ACL_UT_Crash_When_Bind_ACE_To_ACL_via_SNMP()
{
    const char *acl_name = "a1";

    UI32_T acl_index = 1;
    UI32_T ace_index = 1;

    RULE_TYPE_RETURN_TYPE_T result;

    BOOL_T b_result;

    b_result = L4_MGR_QoS_SetDiffServAclStatus(acl_index, VAL_diffServAclStatus_createAndWait);
    assert(TRUE == b_result);

    b_result = L4_MGR_QoS_SetDiffServAclType(acl_index, VAL_diffServAclType_ipextended);
    assert(TRUE == b_result);

    b_result = L4_MGR_QoS_SetDiffServAclName(acl_index, acl_name);
    assert(TRUE == b_result);

    b_result = L4_MGR_QoS_SetDiffServAclStatus(acl_index, VAL_diffServAclStatus_active);
    assert(TRUE == b_result);

    b_result = L4_MGR_QoS_SetDiffServIpAceStatus(ace_index, VAL_diffServAclStatus_createAndWait);
    assert(TRUE == b_result);

    b_result = L4_MGR_QoS_SetDiffServIpAceAccess(ace_index, VAL_diffServIpAceAccess_permit);
    assert(TRUE == b_result);

    b_result = L4_MGR_QoS_SetDiffServIpAceProtocol(ace_index, 33);
    assert(TRUE == b_result);

    b_result = L4_MGR_QoS_SetDiffServIpAceSourceIpAddr(ace_index, 0x01020304);
    assert(TRUE == b_result);

    b_result = L4_MGR_QoS_SetDiffServIpAceSourceIpAddrBitmask(ace_index, 0xffffff00);
    assert(TRUE == b_result);

    b_result = L4_MGR_QoS_SetDiffServIpAceDestIpAddr(ace_index, 0x00000000);
    assert(TRUE == b_result);

    b_result = L4_MGR_QoS_SetDiffServIpAceDestIpAddrBitmask(ace_index, 0xffffff00);
    assert(TRUE == b_result);

    b_result = L4_MGR_QoS_SetDiffServIpAceStatus(ace_index, VAL_diffServAclStatus_active);
    assert(TRUE == b_result);

    b_result = L4_MGR_QoS_SetDiffServAclAttachCtlAction(acl_index,
                                                        VAL_diffServAclAttachCtlAceType_ipAce,
                                                        ace_index,
                                                        VAL_diffServAclAttachCtlAction_attaching);
    assert(TRUE == b_result);

    {
        RULE_TYPE_AclType_T result_ace_type;
        UI32_T result_ace_index;
        RULE_TYPE_Ace_Entry_T result_ace_entry;

        result_ace_index = 0;
        result = RULE_OM_GetNextAceByAcl(acl_index, &result_ace_type, &result_ace_index, &result_ace_entry);

        assert(RULE_TYPE_OK == result);
        assert(result_ace_index == ace_index);

        result = RULE_OM_GetNextAceByAcl(acl_index, &result_ace_type, &result_ace_index, &result_ace_entry);

        assert(RULE_TYPE_OK != result);
    }

    return 0;
}

int
CLI_ACL_UT_Delete_Binded_ACL()
{
	const char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_Ace_Entry_T ace_entry;

    
    UI32_T ifindex = 1;
    RULE_TYPE_InOutDirection_T inout_profile = RULE_TYPE_INBOUND;
    const char *time_range_name = NULL;
    RULE_TYPE_COUNTER_ENABLE_T counter_enable = RULE_TYPE_COUNTER_DISABLE;
    
    CLI_ACL_UT_Proc_CreateOneAcl(acl_name, acl_type);
    
    ace_entry.ace_type = acl_type;

    result = L4_MGR_ACL_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);
    
    //ace_entry.u.mac.access = RULE_TYPE_ACE_PERMIT;
    ace_entry.access = RULE_TYPE_ACE_PERMIT;
    
    SET_MAC(ace_entry.u.mac.aceDestMacAddr, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01);
    SET_MAC(ace_entry.u.mac.aceDestMacAddrBitmask, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
    
    result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
    assert(RULE_TYPE_OK == result);
 
    result = L4_MGR_ACL_InitAce(&ace_entry);
    assert(RULE_TYPE_OK == result);
    
    //ace_entry.u.mac.access = RULE_TYPE_ACE_PERMIT;
    ace_entry.access = RULE_TYPE_ACE_PERMIT;
    
    SET_MAC(ace_entry.u.mac.aceDestMacAddr, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    SET_MAC(ace_entry.u.mac.aceDestMacAddrBitmask, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
    
    result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
    assert(RULE_TYPE_OK == result);
    
    CLI_ACL_UT_Proc_BindACLOnPort(ifindex, acl_name, acl_type, inout_profile,
                                  time_range_name,
                                  counter_enable);

    result = L4_MGR_ACL_DelAclByNameAndAclType(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);

    if (1)
    {
        UI32_T running_acl_index;
        RULE_TYPE_UI_AclEntry_T running_acl_entry;
        UI32_T precedence;
        UI8_T running_time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
        RULE_TYPE_COUNTER_ENABLE_T running_counter;
        
        SYS_TYPE_Get_Running_Cfg_T running_result;
        
        precedence = 0;
        running_result = L4_MGR_ACL_GetNextRunningAclByPort(ifindex,
                                                            inout_profile,
                                                            &running_acl_index,
                                                            &running_acl_entry,
                                                            &precedence,
                                                            running_time_range_name,
                                                            &running_counter);
        assert(SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE == running_result);
    }

    
    return 1;
}

#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
int
CLI_ACL_UT_Create_Mix_ACE_On_Mac_ACL()
{
#define CLI_PRINT FALSE
//#define CLI_PRINT TRUE
    enum
    {
        permit,
        deny
    };

    typedef struct
    {
        char *arg[256];
        UI32_T access;
    } CMD_T;
    
    CMD_T cmd[] =
    {
        {//0
            "any",
            "host",
            "010000000000",
            "ip",
            "host",
            "192.168.1.1",
            "any",
            .access = permit
        },
        {//1
            "any",
            "any",
            "ip",
            "any",
            "host",
            "192.168.1.1",
            .access = permit
        },
        {//2
            "host",
            "000000000001",
            "host",
            "000000000002",
            "vid",
            "1",
            "ip",
            "host",
            "192.168.1.1",
            "any",
            "l4-source-port",
            "1",
            "65535",
            "l4-destination-port",
            "2",
            "65535",
            .access = permit
        },
        {//3
            "host",
            "000000000003",
            "host",
            "000000000004",
            "vid",
            "1",
            .access = permit
        },
        {//4
            "any",
            "any",
            "vid",
            "1",
            "ip",
            "any",
            "any",
            "protocol",
            "2",
            .access = permit
        },
        {//5
            "any",
            "any",
            "vid",
            "1",
            "ip",
            "any",
            "any",
            "protocol",
            "17",
            "l4-destination-port",
            "2",
            .access = permit
        },
        {//6 max length
            "tagged-eth2",
            "000000000001",
            "0000000000ff",
            "000000000100",
            "000000000fff",
            "vid",
            "2",
            "4095",
            "ip",
            "192.168.1.1",
            "255.255.255.3",
            "192.168.1.100",
            "255.255.255.100",
            "protocol",
            "17",
            "l4-source-port",
            "10",
            "10",
            "l4-destination-port",
            "100",
            "100",
            .access = permit
        },
        {//7
            "any",
            "any",
            .access = deny
        }
    };
    
    const char *acl_name = "a1";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_MAC_ACL;
    unsigned int i;
    RULE_TYPE_Ace_Entry_T ace_entry;
    UI32_T acl_index;
    
    RULE_CTRL_UT_Proc_Create_ACL(acl_name, acl_type, &acl_index);
    
    for (i = 0; i < _countof(cmd); i++)
    {
        CMD_T *cmd_p = &cmd[i];
        UI16_T cmd_idx;
        CLI_TASK_WorkingArea_T ctrl;
        UI32_T result;
        
        ace_entry.ace_type = acl_type;
        result = L4_MGR_ACL_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);
        
        cmd_idx = (cmd_p->access == permit)?PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT :
                      ((cmd_p->access == deny)? PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY : 0xffff);
        assert(cmd_idx!=0xffff);

        memcpy(ctrl.CMenu.acl_name, acl_name, strlen(acl_name)+1);
        ctrl.CMenu.acl_type = acl_type;

        switch (cmd_idx)
        {
            case PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT:
                result = CLI_API_Permit_ACL_MAC(cmd_idx, cmd_p->arg, &ctrl);
                break;
            case PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY:
                result = CLI_API_Deny_ACL_MAC(cmd_idx, cmd_p->arg, &ctrl);
                break;
            default:
                assert(0);
        }
        assert(result == CLI_NO_ERROR);
    }
    
    if(CLI_PRINT == TRUE)
    {
        printf("*** CLI_ACL_UT_Create_Mix_ACE_On_Mac_ACL ***\r\n");
        CLI_API_Show_AccessList(0, NULL, NULL);
        
    }

    for (i = 0; i < _countof(cmd); i++)
    {
        CMD_T *cmd_p = &cmd[i];
        UI16_T cmd_idx;
        CLI_TASK_WorkingArea_T ctrl;
        UI32_T result;
        
        ace_entry.ace_type = acl_type;
        result = L4_MGR_ACL_InitAce(&ace_entry);
        assert(RULE_TYPE_OK == result);
        
        cmd_idx = (cmd_p->access == permit)?PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT :
                      (cmd_p->access == deny)? PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY : 0xffff;
        assert(cmd_idx!=0xffff);

        memcpy(ctrl.CMenu.acl_name, acl_name, strlen(acl_name)+1);
        ctrl.CMenu.acl_type = acl_type;
        
        switch (cmd_idx)
        {
            case PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT:
                result = CLI_API_Permit_ACL_MAC(cmd_idx, cmd_p->arg, &ctrl);
                break;
            case PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY:
                result = CLI_API_Deny_ACL_MAC(cmd_idx, cmd_p->arg, &ctrl);
                break;
            default:
                assert(0);
        }
        assert(result == CLI_NO_ERROR);
    }

    if(CLI_PRINT == TRUE)
    {
        CLI_API_Show_AccessList(0, NULL, NULL);
        printf("*** end CLI_ACL_UT_Create_Mix_ACE_On_Mac_ACL ***\r\n");
    }
#undef CLI_PRINT
    return 1;
}
#endif //if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)

int
CLI_ACL_UT_EPR_ASF4628BBS5_FLF_EC_00545()
{
    /* bind and unbind acl on port sereval times,
     * the ACL could not bind any more
     */

    char *acl_name = "p1";
    UI32_T acl_index;
    RULE_TYPE_RETURN_TYPE_T result;
    RULE_TYPE_UI_Ace_Entry_T  ace;

    CLI_ACL_UT_Proc_CreateOneAcl(acl_name, RULE_TYPE_IP_STD_ACL);

    result = L4_PMGR_ACL_GetAclIdByName(acl_name, &acl_index);
    assert(result == RULE_TYPE_OK);

    memset(&ace, 0, sizeof(ace));
    ace.ace_type = RULE_TYPE_IP_STD_ACL;

    result = L4_PMGR_ACL_InitUIAce(&ace);

    ace.ipv4.sip.addr[0] = 192;
    ace.ipv4.sip.addr[1] = 168;
    ace.ipv4.sip.addr[2] = 1;
    ace.ipv4.sip.addr[3] = 1;

    memset(ace.ipv4.sip.mask, 0xff, sizeof(ace.ipv4.sip.mask));

    result = L4_PMGR_ACL_SetUIAce2Acl(acl_name, ace.ace_type, &ace);
    assert(result = RULE_TYPE_OK);

    //RULE_OM_SetDebugFlag(7);
    for (int i = 0; i < 20; i++)
    {
        for(int ifindex = 1; ifindex < SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT; ifindex++)
        {
            CLI_ACL_UT_Proc_BindACLOnPort(ifindex,
                                          acl_name,
                                          RULE_TYPE_IP_STD_ACL,
                                          RULE_TYPE_INBOUND,
                                          NULL,
                                          RULE_TYPE_COUNTER_DISABLE);

        }

        for(int ifindex = 1; ifindex < SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT; ifindex++)
        {
            result = L4_PMGR_ACL_UnBindPortFromAcl(ifindex, acl_name, RULE_TYPE_IP_STD_ACL, TRUE);
            assert(RULE_TYPE_OK == result);
        }
    }
    //RULE_OM_SetDebugFlag(0);


    return 0;
}

int
CLI_ACL_UT_Delete_ACE_With_Same_IP_Address_Mask()
{
    UI32_T acl_index, result;
    CLI_TASK_WorkingArea_T ctrl;

    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.sys_info.max_port_number = SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD;

    // Create ACL "p1"
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W3_ACCESSLIST_IP_STANDARD,
                            "p1",
                            &ctrl,
                            CLI_API_AccessList_IP_Standard);

    // Add ACE to ACL
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_ACL_STANDARD_CMD_W1_PERMIT,
                            "192.168.1.1 255.255.255.0",
                            &ctrl,
                            CLI_API_Permit_ACL_Standard);

    // Valid ace entry in ACL
    {
        UI32_T ace_index, total_ace;
        RULE_TYPE_UI_Ace_Entry_T ace;

        result = L4_PMGR_ACL_GetAclIdByName("p1", &acl_index);
        assert(result == RULE_TYPE_OK);

        ace_index = 0;
        total_ace = 0;

        while (L4_PMGR_ACL_GetNextUIAceByAcl(acl_index, &ace_index, &ace) == RULE_TYPE_OK)
        {
            RULE_TYPE_UI_Ace_Entry_T cmp_ace;
            char *cli_arg[] =
            {
                "192.168.1.1",
                "255.255.255.0",
                ""
            };

            total_ace++;

            cmp_ace.ace_type = RULE_TYPE_IP_STD_ACL;
            L4_PMGR_ACL_InitUIAce(&cmp_ace);

            cmp_ace.access = RULE_TYPE_ACE_PERMIT;
            assert(CLI_NO_ERROR == CLI_ACL_FillStandard(&cmp_ace, cli_arg));

            assert(memcmp(&ace, &cmp_ace, sizeof(cmp_ace)));
        }
        assert(total_ace == 1);
    }

    // Delete ACE to ACL
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_ACL_STANDARD_CMD_W2_NO_PERMIT,
                            "192.168.1.1 255.255.255.0",
                            &ctrl,
                            CLI_API_Permit_ACL_Standard);

    // Valid no ace entry in ACL
    {
        UI32_T ace_index;
        RULE_TYPE_UI_Ace_Entry_T ace;

        ace_index = 0;

        while (L4_PMGR_ACL_GetNextUIAceByAcl(acl_index, &ace_index, &ace) == RULE_TYPE_OK)
        {
            assert(0);;
        }
    }
    
    return 0;
}

int
CLI_ACL_UT_Bind_AccessList_On_Port_With_TimeRange()
{
    /* 1. test for ACL with time range only could save 4 words time range name
     */
    enum
    {
        COUNTER_ENABLE_STATUS = RULE_TYPE_COUNTER_DISABLE,
    };

    char *acl_name = "a1";
    char *time_range_name = "Time11111";
    RULE_TYPE_AclType_T acl_type = RULE_TYPE_IP_EXT_ACL;
    UI32_T acl_index;
    UI32_T ace_count;
    UI32_T a3, a4;
    RULE_TYPE_Ace_Entry_T ace_entry;
    RULE_TYPE_RETURN_TYPE_T result;

    CLI_TASK_WorkingArea_T ctrl;
    char *arg[256] = {NULL};
    UI16_T cmd_idx;

    TIME_RANGE_PMGR_CreateTimeRangeEntry(time_range_name);

    result = L4_MGR_ACL_CreateAcl(acl_name, acl_type);
    assert(RULE_TYPE_OK == result);

    result = RULE_OM_GetAclIdByName(acl_name, &acl_index);
    assert(RULE_TYPE_OK == result);

    ace_count = 0;

    for (a3 = 1; a3 < 2; ++a3)
    {
        for (a4 = 0; a4 <= 2; ++a4)
        {
            ace_entry.ace_type = acl_type;

            result = L4_MGR_ACL_InitAce(&ace_entry);
            assert(RULE_TYPE_OK == result);

            //            ace_entry.u.ip.access = RULE_TYPE_ACE_DENY;
            SET_IPV4(ace_entry.u.ip.aceDestIpAddr, 172, 16, a3, a4);
            SET_IPV4(ace_entry.u.ip.aceDestIpAddrBitmask, 255, 255, 255, 255);

            strncpy(ace_entry.time_range_name, time_range_name, sizeof(ace_entry.time_range_name));

            if (0)
            {
                char arg0[] = "any";
                char arg1[] = "host";
                char arg2[100];
                char arg3[] = "time-range";
                char arg4[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];

                snprintf(arg2, sizeof(arg2), "%d.%d.%lu.%lu", 172,16,a3,a4);
                strncpy(arg3, time_range_name, sizeof(arg3));

                cmd_idx = PRIVILEGE_CFG_ACL_EXTENDED_CMD_W1_DENY;

                arg[0] = arg0;
                arg[1] = arg1;
                arg[2] = arg2;
                arg[3] = arg3;
                arg[4] = arg4;

                CLI_API_Deny_ACL_Extended(cmd_idx, arg, &ctrl);
            }

            result = L4_MGR_ACL_SetAce2Acl(acl_name, acl_type, &ace_entry);
            assert(RULE_TYPE_OK == result);

            ace_count ++;
        }
    }

//    CLI_API_Show_AccessList(0, NULL, &ctrl);

    {
        UI32_T ifindex = 1;

        ctrl.sys_info.my_unit_id = 1;
        arg[0] = "1/1";
        CLI_API_Interface_Ethernet(cmd_idx, arg, &ctrl);

        {
            RULE_TYPE_INTERFACE_INFO_T interface;

            memset(&interface, 0, sizeof(interface));
            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            interface.direction = RULE_TYPE_INBOUND;

            cmd_idx = PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_IP_ACCESSGROUP;
            arg[0] = acl_name;
            arg[1] = "input";
            arg[2] = "time-range";
            arg[3] = time_range_name;
            CLI_API_IP_AccessGroup(cmd_idx, (char **) arg, &ctrl);
            memset(arg, 0, sizeof(arg));

            {
                UI32_T running_acl_index;
                RULE_TYPE_UI_AclEntry_T running_acl_entry;
                UI32_T precedence;
                UI8_T running_time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T running_counter = RULE_TYPE_COUNTER_DISABLE;

                SYS_TYPE_Get_Running_Cfg_T running_result;

                precedence = 0;
                running_result = L4_MGR_ACL_GetNextRunningAclByPort(ifindex,
                                                                    interface.direction,
                                                                    &running_acl_index,
                                                                    &running_acl_entry,
                                                                    &precedence,
                                                                    running_time_range_name,
                                                                    &running_counter);
                assert(SYS_TYPE_GET_RUNNING_CFG_SUCCESS == running_result);

                assert(running_acl_index == acl_index);
                assert(0 == strncmp(running_time_range_name, time_range_name, sizeof(running_time_range_name)));

                //assert(RULE_TYPE_UNDEF_TIME_RANGE == running_time_range_index);
                //assert(RULE_TYPE_COUNTER_ENABLE == running_counter);
            }
//            CLI_API_Show_AccessGroup(cmd_idx, (char **)arg, &ctrl);
//            CLI_API_Show_AccessList_Hardware_Counters(cmd_idx, (char **)arg, &ctrl);
        }

        {
            RULE_TYPE_INTERFACE_INFO_T interface;

            memset(&interface, 0, sizeof(interface));
            RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);

            interface.direction = RULE_TYPE_INBOUND;

            cmd_idx = PRIVILEGE_CFG_INTERFACE_ETH_CMD_W3_NO_IP_ACCESSGROUP;
            arg[0] = acl_name;
            arg[1] = "input";
            CLI_API_IP_AccessGroup(cmd_idx, (char **) arg, &ctrl);
            memset(arg, 0, sizeof(arg));

            {
                UI32_T running_acl_index;
                RULE_TYPE_UI_AclEntry_T running_acl_entry;
                UI32_T precedence;
                UI8_T running_time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1] = {0};
                RULE_TYPE_COUNTER_ENABLE_T running_counter = RULE_TYPE_COUNTER_DISABLE;

                SYS_TYPE_Get_Running_Cfg_T running_result;

                precedence = 0;
                running_result = L4_MGR_ACL_GetNextRunningAclByPort(ifindex,
                                                                    interface.direction,
                                                                    &running_acl_index,
                                                                    &running_acl_entry,
                                                                    &precedence,
                                                                    running_time_range_name,
                                                                    &running_counter);
                assert(SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE == running_result);
            }
        }
    }
    
    return 0;
}
/*------------------------------------------------------------------------------
 * Entry Pointer
 *------------------------------------------------------------------------------
 */
int
CLI_ACL_RunTestCases()
{
#define CLI_ACL_UT_TEST(func)                             \
    printf("\r\n>>== Test (%s)==\r\n", #func);            \
    UT_Test(CLI_ACL_UT_Setup, CLI_ACL_UT_Teardown, func); \
    printf("<<== End Test (%s)==\r\n", #func)

    if (1)
    {
    }

    if (1)
    {
        CLI_ACL_UT_TEST(CLI_ACL_UT_String_Library);
        
        /* static function 
         */
        CLI_ACL_UT_TEST(CLI_ACL_UT_GetMacAddr);
        CLI_ACL_UT_TEST(CLI_ACL_UT_GetIpAddr);
        CLI_ACL_UT_TEST(CLI_ACL_UT_FillMac);
        CLI_ACL_UT_TEST(CLI_ACL_UT_FillAclStandar);
        CLI_ACL_UT_TEST(CLI_ACL_UT_FillAclExtend);
        
        CLI_ACL_UT_TEST(CLI_ACL_UT_SET_ACE_MAC);
        CLI_ACL_UT_TEST(CLI_ACL_UT_SET_ACE_IPv4_Extend);
        CLI_ACL_UT_TEST(CLI_ACL_UT_SET_ACE_IPv6_Standard);
        CLI_ACL_UT_TEST(CLI_ACL_UT_SET_ACE_IPv6_Extended);
        CLI_ACL_UT_TEST(CLI_ACL_UT_SET_ACE_ARP);

        CLI_ACL_UT_TEST(CLI_ACL_UT_Bind_AccessList_On_Trunk_Port);
        CLI_ACL_UT_TEST(CLI_ACL_UT_Bind_AccessList_On_All_Ports);

        CLI_ACL_UT_TEST(CLI_ACL_UT_Show_AccessList_Hardware_Counters);
        CLI_ACL_UT_TEST(CLI_ACL_UT_Show_AccessList_Hardware_Counters_Crash_Issue);
        CLI_ACL_UT_TEST(CLI_ACL_UT_Clear_AccessList_Hardware_Counters);
        CLI_ACL_UT_TEST(CLI_ACL_UT_Clear_AccessList_Hardware_Counters_egress);

        CLI_ACL_UT_TEST(CLI_ACL_UT_Add_ACL_Mirror);

        CLI_ACL_UT_TEST(CLI_ACL_UT_Compress_Continue_IP_ACE);
        CLI_ACL_UT_TEST(CLI_ACL_UT_Dynamic_Selector_Assignment);

        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00250);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ES4626F_SW_FLF_38_01065);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ES4626F_SW_FLF_38_01360);
        CLI_ACL_UT_TEST(CLI_ACL_UT_C_String_Pass);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ASE3526MP_FLF_EC_00014);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ASE3526MP_FLF_EC_00025);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00160);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00394);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00400);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00397);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ASF4628BBS5_FLF_EC_00073);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00881);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ASF4612MMS_FLF_08_00902);
        CLI_ACL_UT_TEST(CLI_ACL_UT_Crash_When_Bind_Time_Range_on_MAC_ACL);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ASF4628BBS5_FLF_EC_00360);
        CLI_ACL_UT_TEST(CLI_ACL_UT_Crash_When_Bind_ACE_To_ACL_via_SNMP);
        CLI_ACL_UT_TEST(CLI_ACL_UT_Delete_Binded_ACL);
        CLI_ACL_UT_TEST(CLI_ACL_UT_EPR_ASF4628BBS5_FLF_EC_00545);
#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
        CLI_ACL_UT_TEST(CLI_ACL_UT_Create_Mix_ACE_On_Mac_ACL);
#endif
        CLI_ACL_UT_TEST(CLI_ACL_UT_Delete_ACE_With_Same_IP_Address_Mask);

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
        CLI_ACL_UT_TEST(CLI_ACL_UT_Bind_AccessList_On_Port_With_TimeRange);
#endif /* SYS_CPNT_TIME_BASED_ACL */
    }
    else
    {
        printf("%s %d: Skip test items\r\n", __FUNCTION__, __LINE__);
    }

    return 0;

#undef CLI_ACL_UI_Test
}
