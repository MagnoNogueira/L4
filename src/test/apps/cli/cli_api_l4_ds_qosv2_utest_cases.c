/*
 *  cli_api_l4_ds_qosv2_utest_cases.c
 *  l4_mgr
 *
 *  Created by yehjunying on 2011/7/2.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
/*------------------------------------------------------------------------------
 * Test Functions
 *------------------------------------------------------------------------------
 */

int
CLI_DS_UT_Setup()
{
    RULE_CTRL_UT_Setup();

    L4_MGR_InitiateSystemResources();
    L4_MGR_EnterMasterMode();

    return 0;
}

int
CLI_DS_UT_Teardown()
{
    RULE_CTRL_UT_Teardown();
    return 0;
}

#define CLI_DS_UT_HELP_DumpCliCmd(mode_str, fmt, args...)   \
do                                                          \
{                                                           \
    printf("config");                                       \
    if (mode_str != NULL) printf("(%s)", mode_str);         \
    printf("# ");                                           \
    printf(fmt "\r\n", ##args);                             \
}                                                           \
while(0)


int
CLI_DS_Create_Class_Map_With_Invalid_Character()
{
    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    char *arg[] =
    {
        NULL,
        "match-any"
    };

    //
    // Max length of class-map name
    //
    {
        UI32_T  class_map_index;
        char class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];

        UI32_T  result;

        memset(class_map_name, 0, sizeof(class_map_name));
        memset(class_map_name, 'c', sizeof(class_map_name)-1);

        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP;
        arg[0] = class_map_name;

        CLI_API_L4_DS_Class_Map(cmd_idx, arg, &ctrl);

        result = L4_PMGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
        assert(RULE_TYPE_OK == result);


        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W2_NO_CLASSMAP;
        CLI_API_L4_DS_Class_Map(cmd_idx, arg, &ctrl);

        result = L4_PMGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
        assert(RULE_TYPE_OK != result);


        //
        // rename
        //
        {
            UI32_T class_map_index_2;


            memset(class_map_name, 0, sizeof(class_map_name));
            memset(class_map_name, 'c', sizeof(class_map_name)-1);

            cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP;
            arg[0] = class_map_name;

            CLI_API_L4_DS_Class_Map(cmd_idx, arg, &ctrl);

            result = L4_PMGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
            assert(RULE_TYPE_OK == result);


            memset(class_map_name, 0, sizeof(class_map_name));
            memset(class_map_name, 'r', sizeof(class_map_name)-1);

            arg[0] = class_map_name;

            CLI_API_L4_DS_Class_Map_Rename(cmd_idx, arg, &ctrl);

            result = L4_PMGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index_2);
            assert(RULE_TYPE_OK == result);
            assert(class_map_index == class_map_index_2);
        }

    }

    //
    // Invalid character in class-map name
    //
    {
        UI32_T  class_map_index;
        char class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];

        RULE_TYPE_ClassMap_T    class_map;
        RULE_TYPE_UI_ClassMap_T ui_class_map;

        UI32_T  result;
        BOOL_T  b_result;

        memset(class_map_name, 0, sizeof(class_map_name));
        memset(class_map_name, '?', sizeof(class_map_name)-1);

        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP;
        arg[0] = class_map_name;

        CLI_API_L4_DS_Class_Map(cmd_idx, arg, &ctrl);

        result = L4_PMGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
        assert(RULE_TYPE_OK != result);

        //
        // Core layer must check invalid character
        //
        result = L4_PMGR_QoS_CreateClassMapByName(class_map_name);
        assert(RULE_TYPE_E_INVALID_CHARACTER == result);

        result = L4_PMGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
        assert(RULE_TYPE_OK != result);


        //
        // From Mib
        //
        result = RULE_MGR_SetClassMapNameById(1, class_map_name);
        assert(RULE_TYPE_E_INVALID_CHARACTER == result);

        b_result = L4_MGR_QoS_SetDiffServClassMapName(1, class_map_name);
        assert(FALSE == b_result);


        strncpy(class_map.class_map_name, class_map_name, sizeof(class_map.class_map_name)-1);
        class_map.class_map_name[ sizeof(class_map.class_map_name)-1 ] = '\0';

        class_map.class_map_match_type = RULE_TYPE_CLASS_MAP_MATCH_ANY;

        result = RULE_MGR_SetClassMap(1, &class_map);
        assert(RULE_TYPE_E_INVALID_CHARACTER == result);


        ui_class_map.class_map_match_type = RULE_TYPE_CLASS_MAP_MATCH_ANY;

        strncpy(ui_class_map.class_map_name, class_map_name, sizeof(ui_class_map.class_map_name)-1);
        ui_class_map.class_map_name[ sizeof(ui_class_map.class_map_name)-1 ] = '\0';


        result = RULE_MGR_SetUIClassMap(1, &ui_class_map);
        assert(RULE_TYPE_E_INVALID_CHARACTER == result);


        //
        // rename
        //
        {
            UI32_T class_map_index_2;
            char class_map_name_2[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];


            memset(class_map_name, 0, sizeof(class_map_name));
            memset(class_map_name, 'c', sizeof(class_map_name)-1);

            cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP;
            arg[0] = class_map_name;

            CLI_API_L4_DS_Class_Map(cmd_idx, arg, &ctrl);

            memset(class_map_name_2, 0, sizeof(class_map_name_2));
            memset(class_map_name_2, '?', sizeof(class_map_name_2)-1);

            arg[0] = class_map_name_2;

            CLI_API_L4_DS_Class_Map_Rename(cmd_idx, arg, &ctrl);

            result = L4_PMGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
            assert(RULE_TYPE_OK == result);

            result = L4_PMGR_QoS_GetClassMapIdByName(class_map_name_2, &class_map_index_2);
            assert(RULE_TYPE_OK != result);
        }
    }

    return 0;
}

int
CLI_DS_Set_Class_Map_Description_With_Invalid_Character()
{
    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    char *arg[] =
    {
        NULL,
        "match-any"
    };

    {
        char class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        char class_map_desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];

        RULE_TYPE_UI_ClassMap_T    ui_class_map;

        UI32_T  class_map_index;

        UI32_T result;
        int    i_result;

        memset(class_map_name, 0, sizeof(class_map_name));
        memset(class_map_name, 'c', SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);

        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP;
        arg[0] = class_map_name;

        CLI_API_L4_DS_Class_Map(cmd_idx, arg, &ctrl);

        result = L4_PMGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
        assert(RULE_TYPE_OK == result);


        memset(class_map_desc, 0, sizeof(class_map_desc));
        memset(class_map_desc, 'd', SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH);

        cmd_idx = PRIVILEGE_CFG_CLASS_MAP_CMD_W1_DESCRIPTION;
        arg[0] = class_map_desc;


        CLI_API_L4_DS_Class_Map_Desc(cmd_idx, arg, &ctrl);

        strncpy(ui_class_map.class_map_name, class_map_name, sizeof(ui_class_map.class_map_name)-1);
        ui_class_map.class_map_name[ sizeof(ui_class_map.class_map_name)-1 ] = '\0';

        result = L4_PMGR_QoS_GetClassMapByName(&ui_class_map);
        assert(RULE_TYPE_OK == result);

        i_result = strcmp(class_map_desc, ui_class_map.desc);
        assert(0 == i_result);


        //
        // clean
        //
        class_map_desc[0] = '\0';

        cmd_idx = PRIVILEGE_CFG_CLASS_MAP_CMD_W1_DESCRIPTION;
        arg[0] = class_map_desc;

        CLI_API_L4_DS_Class_Map_Desc(cmd_idx, arg, &ctrl);

        strncpy(ui_class_map.class_map_name, class_map_name, sizeof(ui_class_map.class_map_name)-1);
        ui_class_map.class_map_name[ sizeof(ui_class_map.class_map_name)-1 ] = '\0';

        result = L4_PMGR_QoS_GetClassMapByName(&ui_class_map);
        assert(RULE_TYPE_OK == result);

        i_result = strcmp(class_map_desc, ui_class_map.desc);
        assert(0 == i_result);


        //
        // set as invalid character
        //
        memset(class_map_desc, 0, sizeof(class_map_desc));
        memset(class_map_desc, '?', SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH);

        cmd_idx = PRIVILEGE_CFG_CLASS_MAP_CMD_W1_DESCRIPTION;
        arg[0] = class_map_desc;

        CLI_API_L4_DS_Class_Map_Desc(cmd_idx, arg, &ctrl);

        strncpy(ui_class_map.class_map_name, class_map_name, sizeof(ui_class_map.class_map_name)-1);
        ui_class_map.class_map_name[ sizeof(ui_class_map.class_map_name)-1 ] = '\0';

        result = L4_PMGR_QoS_GetClassMapByName(&ui_class_map);
        assert(RULE_TYPE_OK == result);

        i_result = strcmp(class_map_desc, ui_class_map.desc);
        assert(0 != i_result);


        strncpy(ui_class_map.desc, class_map_desc, sizeof(ui_class_map.desc)-1);
        ui_class_map.desc[ sizeof(ui_class_map.desc)-1 ] = '\0';

        result = L4_PMGR_QoS_SetClassMap(&ui_class_map);
        assert(RULE_TYPE_E_INVALID_CHARACTER == result);
    }

    return 0;
}

int
CLI_DS_Rename_Class_Map_With_Existed_Class_Map_Name()
{
    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    char *arg[] =
    {
        NULL,
        "match-any"
    };

    {
        char *exist_class_map_name = "123";
        char class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        char class_map_desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];

        UI32_T  class_map_index, exist_class_map_index;

        UI32_T result;

        memset(class_map_name, 0, sizeof(class_map_name));
        memset(class_map_name, 'c', SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH);

        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP;
        arg[0] = class_map_name;

        result = CLI_API_L4_DS_Class_Map(cmd_idx, arg, &ctrl);
        assert(CLI_NO_ERROR == result);

        result = L4_PMGR_QoS_GetClassMapIdByName(class_map_name, &class_map_index);
        assert(RULE_TYPE_OK == result);

        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP;
        arg[0] = exist_class_map_name;

        result = CLI_API_L4_DS_Class_Map(cmd_idx, arg, &ctrl);
        assert(CLI_NO_ERROR == result);

        result = L4_PMGR_QoS_GetClassMapIdByName(exist_class_map_name, &exist_class_map_index);
        assert(RULE_TYPE_OK == result);
        assert(exist_class_map_index != class_map_index);

        cmd_idx = PRIVILEGE_CFG_CLASS_MAP_CMD_W1_RENAME;
        strncpy(ctrl.CMenu.cmap_name, class_map_name, strlen(class_map_name));
        arg[0] = exist_class_map_name;

        result = CLI_API_L4_DS_Class_Map_Rename(cmd_idx, arg, &ctrl);
        assert(CLI_NO_ERROR == result);
        result = L4_PMGR_QoS_RenameClassMap(ctrl.CMenu.cmap_name, arg[0]);
        assert(RULE_TYPE_CLASS_MAP_EXISTED == result);

        result = L4_PMGR_QoS_GetClassMapIdByName(exist_class_map_name, &class_map_index);
        assert(RULE_TYPE_OK == result);
        assert(class_map_index == exist_class_map_index);

    }

    return 0;
}

int
CLI_DS_Create_Policy_Map_With_Invalid_Character()
{
    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    char *arg[] =
    {
        NULL
    };

    //
    // Max length of policy-map name
    //
    {
        UI32_T  policy_map_index;
        char policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];

        UI32_T  result;

        memset(policy_map_name, 0, sizeof(policy_map_name));
        memset(policy_map_name, 'p', sizeof(policy_map_name)-1);

        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP;
        arg[0] = policy_map_name;

        CLI_API_L4_DS_Policy_Map(cmd_idx, arg, &ctrl);

        result = L4_PMGR_QoS_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);


        {
            char policy_map_name_2[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH +1];

            memset(policy_map_name_2, 0, sizeof(policy_map_name_2));
            memset(policy_map_name_2, '?', sizeof(policy_map_name_2)-1);

            arg[0] = policy_map_name_2;
            CLI_API_L4_DS_Policy_Map_Rename(cmd_idx, arg, &ctrl);

            result = L4_PMGR_QoS_RenamePolicyMap(policy_map_name, policy_map_name_2);
            assert(RULE_TYPE_E_INVALID_CHARACTER == result);
        }


        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W2_NO_POLICYMAP;
        arg[0] = policy_map_name;
        CLI_API_L4_DS_Policy_Map(cmd_idx, arg, &ctrl);

        result = L4_PMGR_QoS_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK != result);
    }

    //
    // Invalid character in policy-map name
    //
    {
        UI32_T  policy_map_index;
        char policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];

        UI32_T  result;
        BOOL_T  b_result;

        memset(policy_map_name, 0, sizeof(policy_map_name));
        memset(policy_map_name, '?', sizeof(policy_map_name)-1);

        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP;
        arg[0] = policy_map_name;

        CLI_API_L4_DS_Policy_Map(cmd_idx, arg, &ctrl);

        result = L4_PMGR_QoS_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK != result);

        //
        // Core layer must check invalid character
        //
        result = L4_PMGR_QoS_CreatePolicyMapByName(policy_map_name);
        assert(RULE_TYPE_E_INVALID_CHARACTER == result);

        result = L4_PMGR_QoS_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK != result);


        //
        // From Mib
        //
        result = RULE_MGR_SetPolicyMapNameById(1, policy_map_name);
        assert(RULE_TYPE_E_INVALID_CHARACTER == result);

        b_result = L4_MGR_QoS_SetDiffServPolicyMapName(1, policy_map_name);
        assert(FALSE == b_result);
    }

    return 0;
}

int
CLI_DS_Rename_Policy_Map_With_Existed_Policy_Map_Name()
{
    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    char *arg[] =
    {
        NULL
    };

    {
        UI32_T  policy_map_index, exist_policy_map_index;
        char *exist_policy_map_name = "123";
        char policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        UI32_T  result;

        memset(policy_map_name, 0, sizeof(policy_map_name));
        memset(policy_map_name, 'p', sizeof(policy_map_name)-1);

        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP;
        arg[0] = policy_map_name;

        result = CLI_API_L4_DS_Policy_Map(cmd_idx, arg, &ctrl);
        assert(CLI_NO_ERROR == result);

        result = L4_PMGR_QoS_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);

        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP;
        arg[0] = exist_policy_map_name;

        result = CLI_API_L4_DS_Policy_Map(cmd_idx, arg, &ctrl);
        assert(CLI_NO_ERROR == result);

        result = L4_PMGR_QoS_GetPolicyMapIdByName(exist_policy_map_name, &exist_policy_map_index);
        assert(RULE_TYPE_OK == result);

        cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_RENAME;
        strncpy(ctrl.CMenu.pmap_name, policy_map_name, strlen(policy_map_name));
        arg[0] = exist_policy_map_name;

        result = CLI_API_L4_DS_Policy_Map_Rename(cmd_idx, arg, &ctrl);
        assert(CLI_NO_ERROR == result);
        result = L4_PMGR_QoS_RenamePolicyMap(ctrl.CMenu.pmap_name, arg[0]);
        assert(result == RULE_TYPE_POLICY_MAP_EXISTED);

        result = L4_PMGR_QoS_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);
        assert(policy_map_index != exist_policy_map_index);

    }
    return 0;
}

int
CLI_DS_Set_Policy_Map_Description_With_Invalid_Character()
{
    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    char *arg[] =
    {
        NULL
    };

    {
        UI32_T  policy_map_index;
        char policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
        char policy_map_desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];

        UI32_T  result;
        int     i_result;

        memset(policy_map_name, 0, sizeof(policy_map_name));
        memset(policy_map_name, 'p', sizeof(policy_map_name)-1);
        policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH] = '\0';

        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP;
        arg[0] = policy_map_name;

        result = CLI_API_L4_DS_Policy_Map(cmd_idx, arg, &ctrl);
        assert(CLI_NO_ERROR == result);

        result = L4_PMGR_QoS_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK == result);


        memset(policy_map_desc, 0, sizeof(policy_map_desc));
        memset(policy_map_desc, 'd', sizeof(policy_map_desc)-1);
        policy_map_desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH] = '\0';

        cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_DESCRIPTION;
        arg[0] = policy_map_desc;
        result = CLI_API_L4_DS_Policy_Map_Desc(cmd_idx, arg, &ctrl);
        assert(CLI_NO_ERROR == result);


        {
            RULE_TYPE_MIB_PolicyMap_T mib_policy_map;

            result = L4_PMGR_QoS_GetDiffServPolicyMapEntry(policy_map_index, &mib_policy_map);
            assert(RULE_TYPE_OK == result);

            i_result = strcmp(policy_map_desc, mib_policy_map.desc);
            assert(0 == i_result);
        }

        //
        // invalid check
        //
        {
            char policy_map_desc_2[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];

            memset(policy_map_desc_2, 0, sizeof(policy_map_desc_2));
            memset(policy_map_desc_2, '?', sizeof(policy_map_desc_2)-1);

            cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_DESCRIPTION;
            arg[0] = policy_map_desc_2;
            result = CLI_API_L4_DS_Policy_Map_Desc(cmd_idx, arg, &ctrl);
            assert(CLI_NO_ERROR == result);

            //
            // should not change
            //
            {
                RULE_TYPE_MIB_PolicyMap_T mib_policy_map;

                result = L4_PMGR_QoS_GetDiffServPolicyMapEntry(policy_map_index, &mib_policy_map);
                assert(RULE_TYPE_OK == result);

                i_result = strcmp(policy_map_desc, mib_policy_map.desc);
                assert(0 == i_result);
            }

            //
            // set to core layer also should not success
            //
            result = L4_PMGR_QoS_SetPolicyMapDescByName(policy_map_name, policy_map_desc_2);
            assert(RULE_TYPE_E_INVALID_CHARACTER == result);

            //
            // should not change
            //
            {
                RULE_TYPE_MIB_PolicyMap_T mib_policy_map;

                result = L4_PMGR_QoS_GetDiffServPolicyMapEntry(policy_map_index, &mib_policy_map);
                assert(RULE_TYPE_OK == result);

                i_result = strcmp(policy_map_desc, mib_policy_map.desc);
                assert(0 == i_result);
            }
        }


        //
        // no description
        //
        cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W2_NO_DESCRIPTION;
        result = CLI_API_L4_DS_Policy_Map_Desc(cmd_idx, arg, &ctrl);
        assert(CLI_NO_ERROR == result);

        {
            RULE_TYPE_MIB_PolicyMap_T mib_policy_map;

            result = L4_PMGR_QoS_GetDiffServPolicyMapEntry(policy_map_index, &mib_policy_map);
            assert(RULE_TYPE_OK == result);

            assert('\0' == mib_policy_map.desc[0]);
        }

        cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W2_NO_POLICYMAP;
        arg[0] = policy_map_name;
        result = CLI_API_L4_DS_Policy_Map(cmd_idx, arg, &ctrl);
        assert(CLI_NO_ERROR == result);

        result = L4_PMGR_QoS_GetPolicyMapIdByName(policy_map_name, &policy_map_index);
        assert(RULE_TYPE_OK != result);
    }

    return 0;
}

int
CLI_DS_UT_Software_Bundle_Rate_Control()
{
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    char *arg[255] =
    {
        NULL
    };

    //
    // Create class-map
    //
    cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP;
    arg[0] = "c1-1234567890123456789012345";

    CLI_API_L4_DS_Class_Map(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP;
    arg[0] = "c2-1234567890123456789012345";

    CLI_API_L4_DS_Class_Map(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP;
    arg[0] = "c3-1234567890123456789012345";

    CLI_API_L4_DS_Class_Map(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP;
    arg[0] = "c4-1234567890123456789012345";

    CLI_API_L4_DS_Class_Map(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    //
    // Create policy-map
    //
    cmd_idx = PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP;
    arg[0] = "p1-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    //
    // Bind eaach class-map to policy-map and set bandwidth
    //
    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS;
    arg[0] = "c1-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map_Class(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE;
    arg[0] = "flow";
    arg[1] = "5000";
    arg[2] = "4000";
    arg[3] = "violate-action";
    arg[4] = "drop";

    CLI_API_L4_DS_Policy_Map_Class_Police(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_PRIORITY;
    arg[0] = "10";
    CLI_API_L4_DS_Policy_Map_Class_Priority(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS;
    arg[0] = "c2-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map_Class(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE;
    arg[0] = "flow";
    arg[1] = "1000";
    arg[2] = "4000";
    arg[3] = "violate-action";
    arg[4] = "drop";

    CLI_API_L4_DS_Policy_Map_Class_Police(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_PRIORITY;
    arg[0] = "200";
    CLI_API_L4_DS_Policy_Map_Class_Priority(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS;
    arg[0] = "c3-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map_Class(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE;
    arg[0] = "flow";
    arg[1] = "1000";
    arg[2] = "4000";
    arg[3] = "violate-action";
    arg[4] = "drop";

    CLI_API_L4_DS_Policy_Map_Class_Police(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_PRIORITY;
    arg[0] = "1000";
    CLI_API_L4_DS_Policy_Map_Class_Priority(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS;
    arg[0] = "c4-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map_Class(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE;
    arg[0] = "flow";
    arg[1] = "9000";
    arg[2] = "4000";
    arg[3] = "violate-action";
    arg[4] = "drop";

    CLI_API_L4_DS_Policy_Map_Class_Police(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_PRIORITY;
    arg[0] = "1000";
    CLI_API_L4_DS_Policy_Map_Class_Priority(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    //
    // Bind the policy-map to port 3 ergress direction
    //
    L4_MGR_QoS_BindPort2PolicyMap(3, "p1-1234567890123456789012345", RULE_TYPE_OUTBOUND);

    RULE_CTRL_DumpDfltAceByPort_BackDoor(1, 3);
    RULE_CTRL_UT_Proc_Validate_Rule();

    //
    // Create bundle group
    //

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_BUNDLE;
    arg[0] = "1";       // index
    arg[1] = "5000";    // kbps
    arg[2] = "c1-1234567890123456789012345";
    arg[3] = "c2-1234567890123456789012345";
    arg[4] = "c3-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map_Bundle(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    RULE_CTRL_UT_Proc_Validate_Rule();

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_BUNDLE;
    arg[0] = "1";       // index
    arg[1] = "30000";   // kbps
    arg[2] = "c1-1234567890123456789012345";
    arg[3] = "c2-1234567890123456789012345";
    arg[4] = "c3-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map_Bundle(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    RULE_CTRL_UT_Proc_Validate_Rule();

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_BUNDLE;
    arg[0] = "4";       // index
    arg[1] = "40000";   // kbps
    arg[2] = "c4-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map_Bundle(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    RULE_CTRL_UT_Proc_Validate_Rule();

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_BUNDLE;
    arg[0] = "5";       // index
    arg[1] = "50000";   // kbps
    arg[2] = "c1-1234567890123456789012345";
    arg[3] = "c2-1234567890123456789012345";
    arg[4] = "c3-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map_Bundle(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    RULE_CTRL_UT_Proc_Validate_Rule();

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_BUNDLE;
    arg[0] = "1";       // index
    arg[1] = "30000";   // kbps
    arg[2] = "c1-1234567890123456789012345";
    arg[3] = "c2-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map_Bundle(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    RULE_CTRL_UT_Proc_Validate_Rule();

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_BUNDLE;
    arg[0] = "5";       // index
    arg[1] = "50000";   // kbps
    arg[2] = "c3-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map_Bundle(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    RULE_CTRL_UT_Proc_Validate_Rule();

    CLI_API_Show_Policy_Map(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    RULE_CTRL_UT_Proc_Validate_Rule();

    //////////////

    //
    // try delete (delete and then re-create)
    //
    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W2_NO_BUNDLE;
    arg[0] = "5";       // index
    CLI_API_L4_DS_Policy_Map_Bundle(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_BUNDLE;
    arg[0] = "5";       // index
    arg[1] = "50000";   // kbps
    arg[2] = "c3-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map_Bundle(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));
    //
    //


    //
    // change priority value --> unbind policy-map, change value, bind again
    //
    //
    L4_MGR_QoS_UnBindPortFromPolicyMap(3, "p1-1234567890123456789012345", RULE_TYPE_OUTBOUND);


    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W2_NO_BUNDLE;
    arg[0] = "4";       // index
    CLI_API_L4_DS_Policy_Map_Bundle(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS;
    arg[0] = "c3-1234567890123456789012345";

    CLI_API_L4_DS_Policy_Map_Class(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_PRIORITY;
    arg[0] = "500";
    CLI_API_L4_DS_Policy_Map_Class_Priority(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W2_NO_PRIORITY;
    CLI_API_L4_DS_Policy_Map_Class_Priority(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    //
    // bind policy-map to port
    //
    L4_MGR_QoS_BindPort2PolicyMap(3, "p1-1234567890123456789012345", RULE_TYPE_OUTBOUND);

    arg[0] = NULL;
    CLI_API_Show_Policy_Map(cmd_idx, arg, &ctrl);
    memset(arg, 0, sizeof(arg));

#endif // SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL

    return 0;
}

int
CLI_DS_UT_Bind_Unbind_Policy_Map_On_CPU_Port()
{
#if (SYS_CPNT_QOS_V2_CPU_PORT == TRUE)
    const char *policy_map_name = "p1";

    UI32_T ifindex = SYS_ADPT_CPU_1_IF_INDEX_NUMBER;
    RULE_TYPE_InOutDirection_T direction;
    char output_policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
    SYS_TYPE_Get_Running_Cfg_T get_running_result;

    int retry_cnt = 2;

    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    const char *arg[255] =
    {
        NULL
    };

    RULE_CTRL_UT_Proc_Create_Policy_Match_Any_Class_Map_With_Match_ACL_MF(policy_map_name);

retry:
    cmd_idx = PRIVILEGE_CFG_CONTROL_PLANE_CMD_LIST_CMD_W1_SERVICEPOLICY;
    arg[0] = "input";
    arg[1] = policy_map_name;
    CLI_API_L4_DS_Service_Policy_Control_Plane(cmd_idx, (char **) arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_CONTROL_PLANE_CMD_LIST_CMD_W1_SERVICEPOLICY;
    arg[0] = "input";
    arg[1] = policy_map_name;
    CLI_API_L4_DS_Service_Policy_Control_Plane(cmd_idx, (char **) arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    arg[0] = "input";
    CLI_API_Show_Policy_Map_Control_Plane(cmd_idx, (char **) arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    direction = RULE_TYPE_INBOUND;
    get_running_result = L4_PMGR_QoS_GetRunningPolicyMapNameByPort(ifindex,
                                                                   direction,
                                                                   output_policy_map_name);
    assert(SYS_TYPE_GET_RUNNING_CFG_SUCCESS != get_running_result);

    direction = RULE_TYPE_OUTBOUND;
    get_running_result = L4_PMGR_QoS_GetRunningPolicyMapNameByPort(ifindex,
                                                                   direction,
                                                                   output_policy_map_name);
    assert(SYS_TYPE_GET_RUNNING_CFG_SUCCESS == get_running_result);
    assert(0 == strcmp(policy_map_name, output_policy_map_name));

    cmd_idx = PRIVILEGE_CFG_CONTROL_PLANE_CMD_LIST_CMD_W2_NO_SERVICEPOLICY;
    arg[0] = "input";
    arg[1] = policy_map_name;
    CLI_API_L4_DS_Service_Policy_Control_Plane(cmd_idx, (char **) arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    arg[0] = "input";
    CLI_API_Show_Policy_Map_Control_Plane(cmd_idx, (char **) arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    direction = RULE_TYPE_INBOUND;
    get_running_result = L4_PMGR_QoS_GetRunningPolicyMapNameByPort(ifindex,
                                                                   direction,
                                                                   output_policy_map_name);
    assert(SYS_TYPE_GET_RUNNING_CFG_SUCCESS != get_running_result);

    direction = RULE_TYPE_OUTBOUND;
    get_running_result = L4_PMGR_QoS_GetRunningPolicyMapNameByPort(ifindex,
                                                                   direction,
                                                                   output_policy_map_name);
    assert(SYS_TYPE_GET_RUNNING_CFG_SUCCESS != get_running_result);

    if (retry_cnt-- > 0)
    {
        goto retry;
    }

#endif // SYS_CPNT_QOS_V2_CPU_PORT
    return 0;
}

int
CLI_DS_UT_Bind_Unbind_Policy_Map_On_Trunk_Port()
{
#if (SYS_CPNT_QOS_V2_TRUNK_PORT == TRUE)
    const char *policy_map_name = "p1";

    UI32_T ifindex = SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER;

    RULE_TYPE_INTERFACE_INFO_T interface;

    RULE_TYPE_InOutDirection_T direction;
    char output_policy_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
    SYS_TYPE_Get_Running_Cfg_T get_running_result;

    int retry_cnt = 2;

    CLI_TASK_WorkingArea_T ctrl;

    UI16_T cmd_idx;
    const char *arg[255] =
    {
        NULL
    };

    RULE_CTRL_UT_Proc_Create_Policy_Match_Any_Class_Map_With_Match_ACL_MF(policy_map_name);

    memset(&interface, 0, sizeof(interface));
    RULE_CTRL_Ifindex_To_Interface(ifindex, &interface);
    assert(RULE_TYPE_INTERFACE_TRUNK == interface.type);

    ctrl.CMenu.pchannel_id = interface.trunk_id;

retry:
    cmd_idx = PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W1_SERVICEPOLICY;
    arg[0] = "input";
    arg[1] = policy_map_name;
    CLI_API_L4_DS_Service_Policy_Pch(cmd_idx, (char **) arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    cmd_idx = PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W1_SERVICEPOLICY;
    arg[0] = "input";
    arg[1] = policy_map_name;
    CLI_API_L4_DS_Service_Policy_Pch(cmd_idx, (char **) arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    arg[0] = "port-channel";
    arg[1] = "1";
    arg[2] = "input";
    CLI_API_Show_Policy_Map_Interface(cmd_idx, (char **) arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    direction = RULE_TYPE_INBOUND;
    get_running_result = L4_PMGR_QoS_GetRunningPolicyMapNameByPort(ifindex,
                                                                   direction,
                                                                   output_policy_map_name);
    assert(SYS_TYPE_GET_RUNNING_CFG_SUCCESS == get_running_result);
    assert(0 == strcmp(policy_map_name, output_policy_map_name));

    direction = RULE_TYPE_OUTBOUND;
    get_running_result = L4_PMGR_QoS_GetRunningPolicyMapNameByPort(ifindex,
                                                                   direction,
                                                                   output_policy_map_name);
    assert(SYS_TYPE_GET_RUNNING_CFG_SUCCESS != get_running_result);

    cmd_idx = PRIVILEGE_CFG_INTERFACE_PCHANNEL_CMD_W2_NO_SERVICEPOLICY;
    arg[0] = "input";
    arg[1] = policy_map_name;
    CLI_API_L4_DS_Service_Policy_Pch(cmd_idx, (char **) arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    arg[0] = "port-channel";
    arg[1] = "1";
    arg[2] = "input";
    CLI_API_Show_Policy_Map_Interface(cmd_idx, (char **) arg, &ctrl);
    memset(arg, 0, sizeof(arg));

    {
        arg[0] = "ethernet";
        arg[1] = "1/1";
        arg[2] = "input";
        CLI_API_Show_Policy_Map_Interface(cmd_idx, (char **) arg, &ctrl);
        memset(arg, 0, sizeof(arg));

        arg[0] = "1/1";
        CLI_API_Show_Policy_Map_Interface(cmd_idx, (char **) arg, &ctrl);
        memset(arg, 0, sizeof(arg));
    }

    direction = RULE_TYPE_INBOUND;
    get_running_result = L4_PMGR_QoS_GetRunningPolicyMapNameByPort(ifindex,
                                                                   direction,
                                                                   output_policy_map_name);
    assert(SYS_TYPE_GET_RUNNING_CFG_SUCCESS != get_running_result);

    direction = RULE_TYPE_OUTBOUND;
    get_running_result = L4_PMGR_QoS_GetRunningPolicyMapNameByPort(ifindex,
                                                                   direction,
                                                                   output_policy_map_name);
    assert(SYS_TYPE_GET_RUNNING_CFG_SUCCESS != get_running_result);

    if (retry_cnt-- > 0)
    {
        goto retry;
    }

#endif // SYS_CPNT_QOS_V2_TRUNK_PORT
    return 0;
}

int
CLI_DS_UT_ASF4612MMS_FLF_EC_00138_Crash_When_Unbind_Policy_Map()
{
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL != TRUE)
    printf("Skip this test (not support SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL)\r\n");
    return 1;
#endif

    CLI_TASK_WorkingArea_T ctrl;

//    ctrl.CMenu.unit_id = 1;
    ctrl.sys_info.max_port_number = SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD;
    
    //
    // Create class-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP,
                            "c1 match-all",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH,
                            "vlan 1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map_Match);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP,
                            "p1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE,
                            "flow 20000 16000000 conform-action transmit violate-action drop",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Police);

    CLI_UT_Proc_Run_Command(0,
                            "1/1",
                            &ctrl,                            
                            CLI_API_Interface_Ethernet);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_INTERFACE_ETH_CMD_W1_SERVICEPOLICY,
                            "input p1",
                            &ctrl,
                            CLI_API_L4_DS_Service_Policy_Eth);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP,
                            "c1 match-any",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH,
                            "cos 1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map_Match);

//    CLI_UT_Proc_Run_Command(0,
//                            "1/1",
//                            &ctrl,
//                            CLI_API_Interface_Ethernet);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_NO_SERVICEPOLICY,
                            "input p1",
                            &ctrl,
                            CLI_API_L4_DS_Service_Policy_Eth);


    return 0;
}

int
CLI_DS_UT_ASF4612MMS_FLF_EC_00139_Priority_Command_Lost()
{
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    CLI_TASK_WorkingArea_T ctrl;

    //
    // Create class-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH,
                            "vlan 1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map_Match);

    //
    // Create policy-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP,
                            "p1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map);

    //
    // Bind eaach class-map to policy-map and set bandwidth
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_PRIORITY,
                            "1000",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Priority);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_SET,
                            "cos 6",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Set);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE,
                            "flow 20000 40000 violate-action drop",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Police);

    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map);

#endif // SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL

    return 0;
}

int
CLI_DS_UT_No_Policy_Flow()
{
    CLI_TASK_WorkingArea_T ctrl;

    ctrl.sys_info.max_port_number = SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD;


    //
    // Create class-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH,
                            "vlan 1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map_Match);

    //
    // Create policy-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP,
                            "p1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map);

    //
    // Add class-map to policy-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_SET,
                            "cos 6",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Set);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE,
                            "flow 20000 40000 conform-action transmit violate-action drop",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Police);

    // Validate policy map
    {
        RULE_TYPE_UI_PolicyMap_T        pmap;
        RULE_TYPE_UI_PolicyElement_T    pmap_element;
        UI32_T pmap_id = 0;
        int policy_map_cnt = 0;
        int policy_map_element_cnt = 0;

        memset (&pmap, 0, sizeof(RULE_TYPE_UI_PolicyMap_T) );

        while (L4_PMGR_QoS_GetNextPolicyMap (&pmap_id, &pmap) == RULE_TYPE_OK &&
               pmap.row_status == VAL_diffServPolicyMapStatus_active)
        {
            UI32_T class_map_index;
            RULE_TYPE_UI_ClassMap_T class_map;

            policy_map_cnt++;
            assert(0 == strncmp(pmap.name, "p1", strlen("p1")));
            assert('\0' == pmap.desc[0]);

            policy_map_element_cnt = 0;
            memset(&pmap_element, 0, sizeof(pmap_element));
            while (L4_PMGR_QoS_GetNextElementFromPolicyMap(pmap_id, &pmap_element) == RULE_TYPE_OK &&
                   pmap_element.row_status == VAL_diffServPolicyMapElementStatus_active)
            {
                policy_map_element_cnt++;

                assert(0 == strncmp(pmap_element.class_map_name, "c1", strlen("c1")));
                // "set cos 6"
                assert(IN_ACTION_NEW_COS == pmap_element.action_entry.in_action_type);
                assert(6 ==pmap_element.action_entry.in_action_value);
                // "police flow 20000 40000 conform-action transmit violate-action drop"
                assert(VAL_diffServMeterStatus_active == pmap_element.meter_entry.row_status);
                assert(RULE_TYPE_METER_MODE_FLOW == pmap_element.meter_entry.meter_model);
                assert(pmap_element.meter_entry.rate == 20000);
                assert(pmap_element.meter_entry.burst_size = 40000);
                assert(pmap_element.meter_entry.peak_rate == 0);
                assert(pmap_element.meter_entry.peak_burst_size == 0);
            }

            assert(policy_map_element_cnt == 1);
        }

        assert(policy_map_cnt == 1);
    }

    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map);

    // No police flow
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W2_NO_POLICE,
                            "flow 20000 40000 conform-action transmit violate-action drop",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Police);

    // Validate policy map
    {
        RULE_TYPE_UI_PolicyMap_T        pmap;
        RULE_TYPE_UI_PolicyElement_T    pmap_element;
        UI32_T pmap_id = 0;
        int policy_map_cnt = 0;
        int policy_map_element_cnt = 0;

        memset (&pmap, 0, sizeof(RULE_TYPE_UI_PolicyMap_T) );

        while (L4_PMGR_QoS_GetNextPolicyMap (&pmap_id, &pmap) == RULE_TYPE_OK &&
               pmap.row_status == VAL_diffServPolicyMapStatus_active)
        {
            UI32_T class_map_index;
            RULE_TYPE_UI_ClassMap_T class_map;

            policy_map_cnt++;
            assert(0 == strncmp(pmap.name, "p1", strlen("p1")));
            assert('\0' == pmap.desc[0]);

            policy_map_element_cnt = 0;
            memset(&pmap_element, 0, sizeof(pmap_element));
            while (L4_PMGR_QoS_GetNextElementFromPolicyMap(pmap_id, &pmap_element) == RULE_TYPE_OK &&
                   pmap_element.row_status == VAL_diffServPolicyMapElementStatus_active)
            {
                policy_map_element_cnt++;

                assert(0 == strncmp(pmap_element.class_map_name, "c1", strlen("c1")));
                // "set cos 6"
                assert(IN_ACTION_NEW_COS == pmap_element.action_entry.in_action_type);
                assert(6 ==pmap_element.action_entry.in_action_value);

                assert(0 == pmap_element.meter_entry.row_status);
                /*
                assert(0 == pmap_element.meter_entry.meter_model);
                assert(pmap_element.meter_entry.rate == 0);
                assert(pmap_element.meter_entry.burst_size == 0);
                assert(pmap_element.meter_entry.peak_rate == 0);
                assert(pmap_element.meter_entry.peak_burst_size == 0);
                 */
            }

            assert(policy_map_element_cnt == 1);
        }
        
        assert(policy_map_cnt == 1);
    }

    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map);

    return 0;
}

int
CLI_DS_UT_BindPolicyFlowOnPort()
{
#if !(SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    
    CLI_TASK_WorkingArea_T ctrl;
    
    //    ctrl.CMenu.unit_id = 1;
    ctrl.sys_info.max_port_number = SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD;
    
    //
    // Create class-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map);
    
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH,
                            "vlan 1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map_Match);
    
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP,
                            "p1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map);
    
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class);
    
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE,
                            "flow 20000 40000 conform-action transmit violate-action drop",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Police);
    
    CLI_DS_UT_HELP_DumpCliCmd("config", "interface ethernet 1/1");
    CLI_UT_Proc_Run_Command(0,
                            "1/1",
                            &ctrl,
                            CLI_API_Interface_Ethernet);
    
    CLI_DS_UT_HELP_DumpCliCmd("config-if", "service-policy input p1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_INTERFACE_ETH_CMD_W1_SERVICEPOLICY,
                            "input p1",
                            &ctrl,
                            CLI_API_L4_DS_Service_Policy_Eth);
    
    CLI_DS_UT_HELP_DumpCliCmd(NULL, "show policy-map");
    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map);
    
    CLI_DS_UT_HELP_DumpCliCmd(NULL, "show policy-map interface ethernet 1/1 input");
    CLI_UT_Proc_Run_Command(0,
                            "ethernet 1/1 input",
                            &ctrl,
                            CLI_API_Show_Policy_Map_Interface);
    
#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_COS == TRUE)
    CLI_DS_UT_HELP_DumpCliCmd("config", "class-map c1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map);
    
    CLI_DS_UT_HELP_DumpCliCmd("config-cmap", "match cos 1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH,
                            "cos 1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map_Match);
    
    CLI_DS_UT_HELP_DumpCliCmd(NULL, "show policy-map interface ethernet 1/1 input");
    CLI_UT_Proc_Run_Command(0,
                            "ethernet 1/1 input",
                            &ctrl,
                            CLI_API_Show_Policy_Map_Interface);
#endif /* SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_COS */
    
    CLI_DS_UT_HELP_DumpCliCmd("config", "interface ethernet 1/1");
    //    CLI_UT_Proc_Run_Command(0,
    //                            "1/1",
    //                            &ctrl,
    //                            CLI_API_Interface_Ethernet);
    
    CLI_DS_UT_HELP_DumpCliCmd("config-if", "no service-policy input p1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_NO_SERVICEPOLICY,
                            "input p1",
                            &ctrl,
                            CLI_API_L4_DS_Service_Policy_Eth);
    
#endif /* (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE) */
    return 0;
}

int
CLI_DS_UT_BindPolicyFlowOnMultiPort()
{
#if !(SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    
    CLI_TASK_WorkingArea_T ctrl;
    //    ctrl.CMenu.unit_id = 1;
    ctrl.sys_info.max_port_number = SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD;
    
    //
    // Create class-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map);
    
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH,
                            "vlan 1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map_Match);
    
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP,
                            "p1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map);
    
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class);
    
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE,
                            "flow 20000 40000 conform-action transmit violate-action drop",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Police);
    
    CLI_DS_UT_HELP_DumpCliCmd("config", "interface ethernet 1/1-11");
    CLI_UT_Proc_Run_Command(0,
                            "1/1-11",
                            &ctrl,
                            CLI_API_Interface_Ethernet);
    
    CLI_DS_UT_HELP_DumpCliCmd("config-if", "service-policy input p1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_INTERFACE_ETH_CMD_W1_SERVICEPOLICY,
                            "input p1",
                            &ctrl,
                            CLI_API_L4_DS_Service_Policy_Eth);
    
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
    CLI_DS_UT_HELP_DumpCliCmd("config-if", "service-policy output p1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_INTERFACE_ETH_CMD_W1_SERVICEPOLICY,
                            "output p1",
                            &ctrl,
                            CLI_API_L4_DS_Service_Policy_Eth);
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */
    
    CLI_DS_UT_HELP_DumpCliCmd(NULL, "show policy-map");
    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map);
    
    CLI_DS_UT_HELP_DumpCliCmd(NULL, "show policy-map interface");
    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map_Interface);
    
    CLI_DS_UT_HELP_DumpCliCmd("config", "interface ethernet 1/1-11");
    //    CLI_UT_Proc_Run_Command(0,
    //                            "1/1",
    //                            &ctrl,
    //                            CLI_API_Interface_Ethernet);
    
    CLI_DS_UT_HELP_DumpCliCmd("config-if", "no service-policy input p1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_NO_SERVICEPOLICY,
                            "input p1",
                            &ctrl,
                            CLI_API_L4_DS_Service_Policy_Eth);
    
#if (SYS_CPNT_QOS_V2_EGRESS_PORT == TRUE)
    CLI_DS_UT_HELP_DumpCliCmd("config-if", "no service-policy output p1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_NO_SERVICEPOLICY,
                            "output p1",
                            &ctrl,
                            CLI_API_L4_DS_Service_Policy_Eth);
#endif /* SYS_CPNT_QOS_V2_EGRESS_PORT */
    
    CLI_DS_UT_HELP_DumpCliCmd(NULL, "show policy-map interface");
    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map_Interface);
    
#endif /* (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE) */
    return 0;
}

int
CLI_DS_UT_Policy_Rate()
{
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    CLI_TASK_WorkingArea_T ctrl;

    ctrl.sys_info.max_port_number = SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD;


    //
    // Create class-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH,
                            "vlan 1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map_Match);

    //
    // Create policy-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP,
                            "p1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map);

    //
    // Add class-map to policy-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_SET,
                            "cos 6",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Set);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE,
                            "rate 50000",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Police);

    // Validate policy map
    {
        RULE_TYPE_UI_PolicyMap_T        pmap;
        RULE_TYPE_UI_PolicyElement_T    pmap_element;
        UI32_T pmap_id = 0;
        int policy_map_cnt = 0;
        int policy_map_element_cnt = 0;

        memset (&pmap, 0, sizeof(RULE_TYPE_UI_PolicyMap_T) );

        while (L4_PMGR_QoS_GetNextPolicyMap (&pmap_id, &pmap) == RULE_TYPE_OK &&
               pmap.row_status == VAL_diffServPolicyMapStatus_active)
        {
            UI32_T class_map_index;
            RULE_TYPE_UI_ClassMap_T class_map;

            policy_map_cnt++;
            assert(0 == strncmp(pmap.name, "p1", strlen("p1")));
            assert('\0' == pmap.desc[0]);

            policy_map_element_cnt = 0;
            memset(&pmap_element, 0, sizeof(pmap_element));
            while (L4_PMGR_QoS_GetNextElementFromPolicyMap(pmap_id, &pmap_element) == RULE_TYPE_OK &&
                   pmap_element.row_status == VAL_diffServPolicyMapElementStatus_active)
            {
                policy_map_element_cnt++;

                assert(0 == strncmp(pmap_element.class_map_name, "c1", strlen("c1")));
                // "set cos 6"
                assert(IN_ACTION_NEW_COS == pmap_element.action_entry.in_action_type);
                assert(6 ==pmap_element.action_entry.in_action_value);
                // "police flow 20000 40000 conform-action transmit violate-action drop"
                assert(VAL_diffServMeterStatus_active == pmap_element.meter_entry.row_status);
                assert(RULE_TYPE_METER_MODE_RATE == pmap_element.meter_entry.meter_model);
                assert(pmap_element.meter_entry.rate == 50000);
                assert(pmap_element.meter_entry.burst_size == 0);
                assert(pmap_element.meter_entry.peak_rate == 0);
                assert(pmap_element.meter_entry.peak_burst_size == 0);
            }

            assert(policy_map_element_cnt == 1);
        }

        assert(policy_map_cnt == 1);
    }

    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map);

    // No police flow
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W2_NO_POLICE,
                            "rate 50000",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Police);

    // Validate policy map
    {
        RULE_TYPE_UI_PolicyMap_T        pmap;
        RULE_TYPE_UI_PolicyElement_T    pmap_element;
        UI32_T pmap_id = 0;
        int policy_map_cnt = 0;
        int policy_map_element_cnt = 0;

        memset (&pmap, 0, sizeof(RULE_TYPE_UI_PolicyMap_T) );

        while (L4_PMGR_QoS_GetNextPolicyMap (&pmap_id, &pmap) == RULE_TYPE_OK &&
               pmap.row_status == VAL_diffServPolicyMapStatus_active)
        {
            UI32_T class_map_index;
            RULE_TYPE_UI_ClassMap_T class_map;

            policy_map_cnt++;
            assert(0 == strncmp(pmap.name, "p1", strlen("p1")));
            assert('\0' == pmap.desc[0]);

            policy_map_element_cnt = 0;
            memset(&pmap_element, 0, sizeof(pmap_element));
            while (L4_PMGR_QoS_GetNextElementFromPolicyMap(pmap_id, &pmap_element) == RULE_TYPE_OK &&
                   pmap_element.row_status == VAL_diffServPolicyMapElementStatus_active)
            {
                policy_map_element_cnt++;

                assert(0 == strncmp(pmap_element.class_map_name, "c1", strlen("c1")));
                // "set cos 6"
                assert(IN_ACTION_NEW_COS == pmap_element.action_entry.in_action_type);
                assert(6 ==pmap_element.action_entry.in_action_value);

                assert(0 == pmap_element.meter_entry.row_status);
                /*
                 assert(0 == pmap_element.meter_entry.meter_model);
                 assert(pmap_element.meter_entry.rate == 0);
                 assert(pmap_element.meter_entry.burst_size == 0);
                 assert(pmap_element.meter_entry.peak_rate == 0);
                 assert(pmap_element.meter_entry.peak_burst_size == 0);
                 */
            }
            
            assert(policy_map_element_cnt == 1);
        }
        
        assert(policy_map_cnt == 1);
    }
    
    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map);

#endif /* (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE) */
    return 0;
}

int
CLI_DS_UT_BindPolicyRateOnPort()
{
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)

    CLI_TASK_WorkingArea_T ctrl;

    //    ctrl.CMenu.unit_id = 1;
    ctrl.sys_info.max_port_number = SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD;

    //
    // Create class-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH,
                            "vlan 1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map_Match);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP,
                            "p1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class);

    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE,
                            "rate 20000",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Police);

    CLI_DS_UT_HELP_DumpCliCmd("config", "interface ethernet 1/1");
    CLI_UT_Proc_Run_Command(0,
                            "1/1",
                            &ctrl,
                            CLI_API_Interface_Ethernet);

    CLI_DS_UT_HELP_DumpCliCmd("config-if", "service-policy input p1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_INTERFACE_ETH_CMD_W1_SERVICEPOLICY,
                            "input p1",
                            &ctrl,
                            CLI_API_L4_DS_Service_Policy_Eth);

    CLI_DS_UT_HELP_DumpCliCmd(NULL, "show policy-map");
    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map);

    CLI_DS_UT_HELP_DumpCliCmd(NULL, "show policy-map interface ethernet 1/1 input");
    CLI_UT_Proc_Run_Command(0,
                            "ethernet 1/1 input",
                            &ctrl,
                            CLI_API_Show_Policy_Map_Interface);

#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_COS == TRUE)
    CLI_DS_UT_HELP_DumpCliCmd("config", "class-map c1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map);

    CLI_DS_UT_HELP_DumpCliCmd("config-cmap", "match cos 1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH,
                            "cos 1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map_Match);
    
    CLI_DS_UT_HELP_DumpCliCmd(NULL, "show policy-map interface ethernet 1/1 input");
    CLI_UT_Proc_Run_Command(0,
                            "ethernet 1/1 input",
                            &ctrl,
                            CLI_API_Show_Policy_Map_Interface);
#endif /* SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_COS */

    CLI_DS_UT_HELP_DumpCliCmd("config", "interface ethernet 1/1");
    //    CLI_UT_Proc_Run_Command(0,
    //                            "1/1",
    //                            &ctrl,
    //                            CLI_API_Interface_Ethernet);

    CLI_DS_UT_HELP_DumpCliCmd("config-if", "no service-policy input p1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_NO_SERVICEPOLICY,
                            "input p1",
                            &ctrl,
                            CLI_API_L4_DS_Service_Policy_Eth);
    
#endif /* (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE) */
    return 0;
}

int
CLI_DS_UT_BindPolicyRateOnMultiPort()
{
#if (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE)
    
    CLI_TASK_WorkingArea_T ctrl;
    
    //    ctrl.CMenu.unit_id = 1;
    ctrl.sys_info.max_port_number = SYS_ADPT_TOTAL_PORTS_PER_UNIT_ON_BOARD;
    
    //
    // Create class-map
    //
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_CLASSMAP,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map);
    
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_CLASS_MAP_CMD_W1_MATCH,
                            "vlan 1",
                            &ctrl,
                            CLI_API_L4_DS_Class_Map_Match);
    
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_GLOBAL_CMD_W1_POLICYMAP,
                            "p1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map);
    
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CMD_W1_CLASS,
                            "c1",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class);
    
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_POLICY_MAP_CLASS_CMD_W1_POLICE,
                            "rate 20000",
                            &ctrl,
                            CLI_API_L4_DS_Policy_Map_Class_Police);
    
    CLI_DS_UT_HELP_DumpCliCmd("config", "interface ethernet 1/1-11");
    CLI_UT_Proc_Run_Command(0,
                            "1/1-11",
                            &ctrl,
                            CLI_API_Interface_Ethernet);
    
    CLI_DS_UT_HELP_DumpCliCmd("config-if", "service-policy input p1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_INTERFACE_ETH_CMD_W1_SERVICEPOLICY,
                            "input p1",
                            &ctrl,
                            CLI_API_L4_DS_Service_Policy_Eth);
    
    CLI_DS_UT_HELP_DumpCliCmd(NULL, "show policy-map");
    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map);
    
    CLI_DS_UT_HELP_DumpCliCmd(NULL, "show policy-map interface");
    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map_Interface);
    
    CLI_DS_UT_HELP_DumpCliCmd("config", "interface ethernet 1/1-11");
    //    CLI_UT_Proc_Run_Command(0,
    //                            "1/1",
    //                            &ctrl,
    //                            CLI_API_Interface_Ethernet);
    
    CLI_DS_UT_HELP_DumpCliCmd("config-if", "no service-policy input p1");
    CLI_UT_Proc_Run_Command(PRIVILEGE_CFG_INTERFACE_ETH_CMD_W2_NO_SERVICEPOLICY,
                            "input p1",
                            &ctrl,
                            CLI_API_L4_DS_Service_Policy_Eth);
    
    CLI_DS_UT_HELP_DumpCliCmd(NULL, "show policy-map interface");
    CLI_UT_Proc_Run_Command(0, "", &ctrl, CLI_API_Show_Policy_Map_Interface);
    
#endif /* (SYS_CPNT_QOS_DIFFSERV_POLICE_MODE & SYS_CPNT_QOS_DIFFSERV_POLICE_RATE) */
    return 0;
}

int table_test()
{
    int i = 0;
    char buff[100] = {0};
    CLI_TBL_Object_T object;
    CLI_TBL_Temp_T template[] =
    {
        {0, 3, CLI_TBL_ALIGN_RIGHT},
        {1, 3, CLI_TBL_ALIGN_RIGHT},
    };

    CLI_TBL_InitWithBuf(&object, buff, sizeof(buff));
    CLI_TBL_SetColIndirect(&object, &template, sizeof(template)/sizeof(template[0]));

    CLI_TBL_SetColTitle(&object, 0, "C1");
    CLI_TBL_SetColTitle(&object, 1, "C2");
    CLI_TBL_Print(&object);

    while (i < 100)
    {
        CLI_TBL_SetLine(&object);
        CLI_TBL_Print(&object);

        CLI_TBL_SetColInt(&object, 0, i);
        CLI_TBL_SetColInt(&object, 1, ++i);
        CLI_TBL_Print(&object);

        i++;
    }

    CLI_TBL_Print(&object);

    return 0;
}


/*------------------------------------------------------------------------------
 * Entry Pointer
 *------------------------------------------------------------------------------
 */
int
CLI_DS_RunTestCases()
{

#define CLI_DS_UT_TEST(func)                            \
    printf("\r\n>>== Test (%s)==\r\n", #func);          \
    UT_Test(CLI_DS_UT_Setup, CLI_DS_UT_Teardown, func); \
    printf("<<== End Test (%s)==\r\n", #func)

    if (0)
    {
        printf("%s (%d): Skip all l4_ds_qosv2 test case!!\r\n", __FUNCTION__, __LINE__);
        return 0;
    }

    CLI_DS_UT_TEST(CLI_DS_Create_Class_Map_With_Invalid_Character);
    CLI_DS_UT_TEST(CLI_DS_Set_Class_Map_Description_With_Invalid_Character);
    CLI_DS_UT_TEST(CLI_DS_Rename_Class_Map_With_Existed_Class_Map_Name);

    CLI_DS_UT_TEST(CLI_DS_Create_Policy_Map_With_Invalid_Character);
    CLI_DS_UT_TEST(CLI_DS_Rename_Policy_Map_With_Existed_Policy_Map_Name);
    CLI_DS_UT_TEST(CLI_DS_Set_Policy_Map_Description_With_Invalid_Character);
    CLI_DS_UT_TEST(CLI_DS_UT_Software_Bundle_Rate_Control);
    CLI_DS_UT_TEST(CLI_DS_UT_Bind_Unbind_Policy_Map_On_CPU_Port);
    CLI_DS_UT_TEST(CLI_DS_UT_Bind_Unbind_Policy_Map_On_Trunk_Port);

    CLI_DS_UT_TEST(CLI_DS_UT_ASF4612MMS_FLF_EC_00138_Crash_When_Unbind_Policy_Map);
    CLI_DS_UT_TEST(CLI_DS_UT_ASF4612MMS_FLF_EC_00139_Priority_Command_Lost);

    CLI_DS_UT_TEST(CLI_DS_UT_No_Policy_Flow);
    CLI_DS_UT_TEST(CLI_DS_UT_BindPolicyFlowOnPort);
    CLI_DS_UT_TEST(CLI_DS_UT_BindPolicyFlowOnMultiPort);

    /* SYS_CPNT_QOS_DIFFSERV_POLICE_RATE
     */
    CLI_DS_UT_TEST(CLI_DS_UT_Policy_Rate);
    CLI_DS_UT_TEST(CLI_DS_UT_BindPolicyRateOnPort);
    CLI_DS_UT_TEST(CLI_DS_UT_BindPolicyRateOnMultiPort);

    //table_test();

    return 0;

#undef CLI_DS_UI_Test
}
