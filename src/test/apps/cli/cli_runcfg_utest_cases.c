//
//  cli_runcfg_utest_cases.c
//  l4_mgr
//
//  Created by swei on 13/9/16.
//
//

int
CLI_RUNCFG_UT_Setup()
{
    RULE_CTRL_UT_Setup();
    
    L4_MGR_InitiateSystemResources();
    L4_MGR_EnterMasterMode();
    
    return 0;
}

int
CLI_RUNCFG_UT_Teardown()
{
    RULE_CTRL_UT_Teardown();
    return 0;
}

int
CLI_RUNCFG_UT_Get_ACL_Parameter()
{
    typedef struct
    {
        char *arg[256];
        UI32_T action;
        UI32_T cmd_index;
        UI32_T cmd_index_no;
    } CMD_T;

    /* These patterns should be test:
     *  tagged-eth2 | untagged-eth2 | tagged-802.3 | untagged-802.3
     *  mac  any/host/single
     *  VID (mask)
     *  ethertype (mask)
     *  ip  any/host/single
     *  ipv6 any/host/single
     *  protocol
     *  l4-source-port (mask)
     *  l4-destination-port (mask)
     */
    CMD_T cmd[] =
    {
        /* mac */
        {
            "tagged-eth2",
            "any",
            "any",
            "VID",
            "1",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
        {
            "untagged-eth2",
            "any",
            "host",
            "fe-dc-ba-98-76-50",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
        {
            "tagged-802.3",
            "any",
            "fe-dc-ba-98-76-50",
            "ff-ff-ff-ff-ff-f0",
            "VID",
            "4095",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
        {
            "untagged-802.3",
            "host",
            "fe-dc-ba-98-76-60",
            "any",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
        {
            "host",
            "fe-dc-ba-98-76-01",
            "host",
            "fe-dc-ba-98-76-02",
            "VID",
            "2",
            "2",
            "ethertype",
            "0800",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
        {
            "host",
            "fe-dc-ba-98-76-01",
            "fe-dc-ba-98-76-50",
            "ff-ff-ff-ff-ff-f0",
            "ethertype",
            "0800",
            "fff0",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
        {
            "fe-dc-ba-98-76-60",
            "ff-ff-ff-ff-ff-f0",
            "any",
            "VID",
            "1",
            "1",
            "ethertype",
            "86dd",
            "fffd",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
        {
            "fe-dc-ba-98-76-60",
            "ff-ff-ff-ff-ff-f0",
            "host",
            "fe-dc-ba-98-76-51",
            "ethertype",
            "86dd",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
        {
            "fe-dc-ba-98-76-60",
            "ff-ff-ff-ff-ff-f0",
            "fe-dc-ba-98-76-50",
            "ff-ff-ff-ff-ff-f0",
            "ethertype",
            "ffff",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },

#if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE)
        /* ipv4 */
        //9
        {
            "any",
            "any",
            "VID",
            "2",
            "ip",
            "any",
            "any",
            "protocol",
            "1",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ip",
            "any",
            "host",
            "192.168.1.2",
            "protocol",
            "17",
            "l4-source-port",
            "1",
            "l4-destination-port",
            "2",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ip",
            "any",
            "192.168.2.0",
            "255.255.255.0",
            "protocol",
            "17",
            "l4-source-port",
            "1",
            "15535",
            "l4-destination-port",
            "2",
            "25535",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ip",
            "host",
            "192.168.2.1",
            "any",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ip",
            "host",
            "192.168.2.1",
            "host",
            "192.168.2.2",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ip",
            "host",
            "192.168.2.1",
            "192.168.2.2",
            "255.255.255.254",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ip",
            "192.168.2.2",
            "255.255.255.254",
            "any",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ip",
            "192.168.2.2",
            "255.255.255.254",
            "host",
            "192.168.3.1",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ip",
            "192.168.2.2",
            "255.255.255.254",
            "192.168.3.2",
            "255.255.255.254",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        /* ipv6 */
        //18
        {
            "any",
            "any",
            "ipv6",
            "any",
            "any",
            "protocol",
            "1",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ipv6",
            "any",
            "::1/2",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ipv6",
            "host",
            "::1",
            "any",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ipv6",
            "host",
            "fe08::1",
            "fe08::2/4",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ipv6",
            "host",
            "fe08::1",
            "fe08::3/4",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ipv6",
            "fe08::2/127",
            "any",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
#if (SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN == 128)
        {
            "any",
            "any",
            "ipv6",
            "fe08::2/127",
            "host",
            "fe08::8",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
#endif /* (SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN == 128) */
        {
            "any",
            "any",
            "ipv6",
            "fe08::2/127",
            "fe08::4/8",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "any",
            "ipv6",
            "3fff::2/127",
            "any",
            .action = RULE_TYPE_ACE_PERMIT,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_PERMIT,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_PERMIT,
        },
        {
            "any",
            "host",
            "00-00-00-00-00-01",
            "VID",
            "1",
            "ipv6",
            "any",
            "fe08::1/1",
            "protocol",
            "17",
            "l4-source-port",
            "3",
            "15535",
            "l4-destination-port",
            "4",
            "25535",
            .action = RULE_TYPE_ACE_DENY,
            .cmd_index = PRIVILEGE_CFG_ACL_MAC_CMD_W1_DENY,
            .cmd_index_no = PRIVILEGE_CFG_ACL_MAC_CMD_W2_NO_DENY,
        },
#endif /* #if (SYS_CPNT_ACL_MAC_IP_MIX == TRUE) */
    };

    CLI_TASK_WorkingArea_T ctrl;
    unsigned long i;
    char *acl_name = "t1";
    RULE_TYPE_RETURN_TYPE_T result;
    char config_buffer[CLI_DEF_MAX_CONFIG_SIZE + 1] = {0};
    char format_str[100] = {0};

    strncpy(ctrl.CMenu.acl_name, acl_name, 3);
    ctrl.CMenu.acl_type = RULE_TYPE_MAC_ACL;

    result = L4_MGR_ACL_CreateAcl(ctrl.CMenu.acl_name, ctrl.CMenu.acl_type);
    assert(RULE_TYPE_OK == result);

    strcat(config_buffer, "access-list MAC");
    sprintf(format_str, " %s\n", acl_name);
    strcat(config_buffer, format_str);

    for (i = 0; i < _countof(cmd); ++i)
    {
        CMD_T * cmd_p = &cmd[i];
        UI32_T cli_result;
        int arg_count;

        RULE_OM_SetDebugFlag(7);
        cli_result = CLI_API_SET_ACL_MAC(cmd_p->action, cmd_p->cmd_index, cmd_p->arg, &ctrl);
        assert(CLI_NO_ERROR == cli_result);
        RULE_OM_SetDebugFlag(0);

        switch (cmd_p->action)
        {
            case RULE_TYPE_ACE_PERMIT:
                strcat(config_buffer, " permit");
                break;
            case RULE_TYPE_ACE_DENY:
                strcat(config_buffer, " deny");
                break;
            default:
                break;
        }

        for (arg_count = 0; cmd_p->arg[arg_count] != NULL; ++arg_count)
        {
            sprintf(format_str, " %s", cmd_p->arg[arg_count]);
            strcat(config_buffer, format_str);
        }
        strcat(config_buffer, "\n");
    }
    strcat(config_buffer, "!\n");

    {
        UI32_T unit_id;
        char buffer[CLI_DEF_MAX_CONFIG_SIZE] = {0};
        char temp_buffer[CLI_DEF_MAX_CONFIG_SIZE] = {0};
        UI32_T buffer_size;
        UI32_T b_count;

        unit_id = 1;
        buffer_size = sizeof(buffer);
        b_count = 0;

        CLI_RUNCFG_Get_ACL_Parameter(unit_id, buffer, temp_buffer, buffer_size, b_count);

#if 0
        printf("\n%s(%d)-------------------------------\n", __FUNCTION__, __LINE__);
        printf("%s", buffer);
        printf("***********************************************************\n");
        printf("%s", config_buffer);
#endif /* if 0 */

        ASSERT(0 == memcmp(buffer, config_buffer, sizeof(buffer)));
    }

    return 0;
}

/*------------------------------------------------------------------------------
 * Entry Pointer
 *------------------------------------------------------------------------------
 */
int
CLI_RUNCFG_RunTestCases()
{
#define CLI_RUNCFG_UT_TEST(func) \
UT_Test(CLI_RUNCFG_UT_Setup, CLI_RUNCFG_UT_Teardown, func)
    
    if (1)
    {
        CLI_RUNCFG_UT_TEST(CLI_RUNCFG_UT_Get_ACL_Parameter);
    }
    else
    {
        printf("%s (%d): Skip all cli_runcfg test case!!\r\n", __FUNCTION__, __LINE__);
        
    }

    return 0;
    
#undef CLI_DS_UI_Test
}