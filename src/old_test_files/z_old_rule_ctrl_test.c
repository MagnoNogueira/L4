typedef int (*TEST_FUNCTION) ();
#define TEST_FUNCTION_MAX_DESC_STR_LEN  64

typedef struct TEST_FUNCTION_ENTRY_
{
    TEST_FUNCTION               setup;
    TEST_FUNCTION               run;
    TEST_FUNCTION               setdown;
    char                        desc[TEST_FUNCTION_MAX_DESC_STR_LEN + 1];
    struct TEST_FUNCTION_ENTRY_ *next;

} TEST_FUNCTION_ENTRY;

TEST_FUNCTION_ENTRY *test_function_entries;

void TEST_FUNCTION_Init()
{
    test_function_entries = NULL;
}

void TEST_FUNCTION_Register(
    TEST_FUNCTION setup,
    TEST_FUNCTION run,
    TEST_FUNCTION setdown,
    char *desc)
{
    TEST_FUNCTION_ENTRY   *func_entry;
    TEST_FUNCTION_ENTRY   *tail_entry;

    func_entry = calloc(1, sizeof(*func_entry));
    if (func_entry == NULL)
    {
        printf("%s: out of memory\n\n", __FUNCTION__);
    }

    func_entry->setup       = setup;
    func_entry->run         = run;
    func_entry->setdown     = setdown;

    if (desc)
    {
        strncpy(func_entry->desc, desc, TEST_FUNCTION_MAX_DESC_STR_LEN);
        func_entry->desc[TEST_FUNCTION_MAX_DESC_STR_LEN] = '\0';
    }

    func_entry->next        = NULL; //test_function_entries;

    if (test_function_entries == NULL)
    {
        test_function_entries = func_entry;
    }
    else
    {
        tail_entry = test_function_entries;

        while (tail_entry)
        {
            if (tail_entry->next == NULL)
            {
                tail_entry->next = func_entry;
                break;
            }
            tail_entry = tail_entry->next;
        }
    }

    //test_function_entries   = func_entry;
}

int TEST_FUNCTION_Run()
{
    TEST_FUNCTION_ENTRY *func_entry;
    int                 res = 0;

    printf("Start Test ...\n");

    func_entry = test_function_entries;

    while (func_entry)
    {
        if (func_entry->desc[0])
        {
            printf("Run Test: %s\n", func_entry->desc);
        }

        if (func_entry->setup)
            func_entry->setup();

        res += func_entry->run();

        if (func_entry->setdown)
            func_entry->setdown();

        func_entry = func_entry->next;
    }

    printf("Finish Test: %s\n", (res) ? "FAIL" : "SUCCEED");

    return res;
}

int RULE_CTRL_Setup()
{
    RULE_CTRL_HELP_CleanRulePool();
    //RULE_CTRL_LocalInitialize();
    RULE_CTRL_EnterMasterMode();

    RULE_OM_Initialize();

    return 0;
}

int RULE_CTRL_Setdown()
{
    RULE_CTRL_HELP_CleanRulePool();
    //RULE_CTRL_LocalInitialize();
    RULE_CTRL_EnterMasterMode();

    RULE_OM_Initialize();
    return 0;
}

int RULE_CTRL_TestHelpFunctions()
{
    int                         res = 0;
    UI32_T                      unit;
    UI32_T                      device;
    UI32_T                      group;
    UI32_T                      rule;
    UI32_T                      dev_rule_id;

    EACH_UNIT(unit)
    {
        EACH_DEVICE(device)
        {
            EACH_GROUP(group)
            {
                EACH_RULE(rule)
                {
                    UI32_T  u, d, g, r;

                    dev_rule_id = RULE_CTRL_HELP_MakeDevRuleID(unit, device, group, rule);

                    if (!(TRUE == RULE_CTRL_HELP_DecomposeDevRuleID(dev_rule_id, &u, &d, &g, &r)))
                    {
                        RULE_CTRL_LOG("RULE_CTRL_HELP_DecomposeDevRuleID failed\n"
                                      "  dev_rule_id = %lu", dev_rule_id);
                        res = 1;
                    }

                    if (!(u == unit && d == device && g == group && r == rule))
                    {
                        RULE_CTRL_LOG("RULE_CTRL_HELP_DecomposeDevRuleID failed");
                        res = 1;
                    }
                }
            }
        }
    }

    return res;
}

int RULE_CTRL_TestFindRuleLookupTable()
{
    int                             res = 0;
    RULE_CTRL_FindRuleLookupTable_T find;

    find.flag = 0;
    find.unit = 0;
    find.device_id = 0;
    find.group_id = 0;
    find.rule_id = 0;

    if (!(RULE_CTRL_FindRuleLookupTable(&find) == FALSE))
    {
        RULE_CTRL_LOG("%s failed\n", "RULE_CTRL_FindRuleLookupTable");
        res = 1;
    }

    find.flag = 0;
    find.unit = 1;
    find.device_id = 0;
    find.group_id = 0;
    find.rule_id = 0;

    if (!(RULE_CTRL_FindRuleLookupTable(&find) == TRUE &&
          find.existed_p ==  NULL &&
          find.first_free_p == NULL))
    {
        RULE_CTRL_LOG("%s failed\n", "RULE_CTRL_FindRuleLookupTable");
        res = 1;
    }

    shmem_data_p->rule_lookup_tbl[0][0][0][3].code.valid = 1;
    shmem_data_p->rule_lookup_tbl[0][0][0][3].rule_id = 99;

    find.flag = RULE_CTRL_FIND_EXISTED_RULE_ENTRY;
    find.unit = 1;
    find.device_id = 0;
    find.group_id = 0;
    find.rule_id = 99;

    if (!(RULE_CTRL_FindRuleLookupTable(&find) == TRUE &&
          find.existed_p &&
          find.existed_p->code.valid == 1 &&
          find.existed_p->rule_id == 99 &&
          find.first_free_p == NULL))
    {
        RULE_CTRL_LOG("%s failed\n", "RULE_CTRL_FindRuleLookupTable");
        res = 1;
    }

    find.flag = RULE_CTRL_FIND_FIRST_FREE_RULE_ENTRY;
    find.unit = 1;
    find.device_id = 0;
    find.group_id = 0;
    find.rule_id = 0;

    if (!(RULE_CTRL_FindRuleLookupTable(&find) == TRUE &&
          find.existed_p == NULL &&
          find.first_free_p &&
          find.first_free_p->code.valid == 0 &&
          find.first_free_p->rule_id == 0))
    {
        RULE_CTRL_LOG("%s failed\n", "RULE_CTRL_FindRuleLookupTable");
        res = 1;
    }

    find.flag = RULE_CTRL_FIND_EXISTED_RULE_ENTRY | RULE_CTRL_FIND_FIRST_FREE_RULE_ENTRY;
    find.unit = 1;
    find.device_id = 0;
    find.group_id = 0;
    find.rule_id = 99;

    if (!(RULE_CTRL_FindRuleLookupTable(&find) == TRUE &&
          find.existed_p &&
          find.existed_p->code.valid == 1 &&
          find.existed_p->rule_id == 99 &&
          find.first_free_p &&
          find.first_free_p->code.valid == 0 &&
          find.first_free_p->rule_id == 0))
    {
        RULE_CTRL_LOG("%s failed\n", "RULE_CTRL_FindRuleLookupTable");
        res = 1;
    }

    // memset(rule_lookup_tbl, 0, sizeof(rule_lookup_tbl));

    return res;
}

int RULE_CTRL_TestGetNextRuleEntryInLookupTable()
{
    int                             res = 0;
    RULE_CTRL_FindRuleLookupTable_T find;
    UI32_T                          unit;
    UI32_T                          device_id;
    UI32_T                          group_id;
    UI32_T                          i;

    /* in here unit is 0 based
     */
    for (unit = 0; unit < SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; unit++)
    {
        for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; device_id++)
        {
            for (group_id = 0; group_id < SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP; group_id++)
            {
                i = 0;
                shmem_data_p->rule_lookup_tbl[unit][device_id][group_id][i].code.valid = 1;
                shmem_data_p->rule_lookup_tbl[unit][device_id][group_id][i].rule_id = RULE_CTRL_HELP_MakeDevRuleID(unit, device_id, group_id, i);

                i = SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR/2;
                shmem_data_p->rule_lookup_tbl[unit][device_id][group_id][i].code.valid = 1;
                shmem_data_p->rule_lookup_tbl[unit][device_id][group_id][i].rule_id = RULE_CTRL_HELP_MakeDevRuleID(unit, device_id, group_id, i);

                i = SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR-1;
                shmem_data_p->rule_lookup_tbl[unit][device_id][group_id][i].code.valid = 1;
                shmem_data_p->rule_lookup_tbl[unit][device_id][group_id][i].rule_id = RULE_CTRL_HELP_MakeDevRuleID(unit, device_id, group_id, i);
            }
        }
    }

    /* query all
     */
    find.flag = RULE_CTRL_FIND_NEXT_EXISTED_RULE_ENTRY;
    find.unit = RULE_CTRL_INVALID;
    find.device_id = RULE_CTRL_INVALID;
    find.group_id = RULE_CTRL_INVALID;
    find.rule_id = RULE_CTRL_INVALID;

    for (unit=1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; unit++)
    {
        for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; device_id++)
        {
            for (group_id = 0; group_id < SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP; group_id++)
            {

                if (!(RULE_CTRL_GetNextRuleEntryInLookupTable(&find) == TRUE &&
                    find.existed_p &&
                    find.unit == unit &&
                    find.device_id == device_id &&
                    find.group_id == group_id &&
                    find._last_index == 0))
                {
                    RULE_CTRL_LOG("%s failed\n"
                                  " unit=%lu, device_id=%lu, group_id=%lu",
                                  "RULE_CTRL_GetNextRuleEntryInLookupTable",
                                  unit, device_id, group_id);
                    res = 1;
                }

                if (!(RULE_CTRL_GetNextRuleEntryInLookupTable(&find) == TRUE &&
                    find.existed_p &&
                    find.unit == unit &&
                    find.device_id == device_id &&
                    find.group_id == group_id &&
                    find._last_index == SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR/2))
                {
                    RULE_CTRL_LOG("%s failed\n"
                                  " unit=%lu, device_id=%lu, group_id=%lu",
                                  "RULE_CTRL_GetNextRuleEntryInLookupTable",
                                  unit, device_id, group_id);
                    res = 1;
                }

                if (!(RULE_CTRL_GetNextRuleEntryInLookupTable(&find) == TRUE &&
                    find.existed_p &&
                    find.unit == unit &&
                    find.device_id == device_id &&
                    find.group_id == group_id &&
                    find._last_index == SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR-1))
                {
                    RULE_CTRL_LOG("%s failed\n"
                                  " unit=%lu, device_id=%lu, group_id=%lu",
                                  "RULE_CTRL_GetNextRuleEntryInLookupTable",
                                  unit, device_id, group_id);
                    res = 1;
                }
            }

        }
    }

    /* query all
     */
    find.flag = RULE_CTRL_FIND_NEXT_EXISTED_RULE_ENTRY;
    find.unit = 1;
    find.device_id = RULE_CTRL_INVALID;
    find.group_id = RULE_CTRL_INVALID;
    find.rule_id = RULE_CTRL_INVALID;

    for (unit=1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; unit++)
    {
        for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; device_id++)
        {
            for (group_id = 0; group_id < SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP; group_id++)
            {

                if (!(RULE_CTRL_GetNextRuleEntryInLookupTable(&find) == TRUE &&
                    find.existed_p &&
                    find.unit == unit &&
                    find.device_id == device_id &&
                    find.group_id == group_id &&
                    find._last_index == 0))
                {
                    RULE_CTRL_LOG("%s failed\n"
                                  " unit=%lu, device_id=%lu, group_id=%lu",
                                  "RULE_CTRL_GetNextRuleEntryInLookupTable",
                                  unit, device_id, group_id);
                    res = 1;
                }

                if (!(RULE_CTRL_GetNextRuleEntryInLookupTable(&find) == TRUE &&
                    find.existed_p &&
                    find.unit == unit &&
                    find.device_id == device_id &&
                    find.group_id == group_id &&
                    find._last_index == SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR/2))
                {
                    RULE_CTRL_LOG("%s failed\n"
                                  " unit=%lu, device_id=%lu, group_id=%lu",
                                  "RULE_CTRL_GetNextRuleEntryInLookupTable",
                                  unit, device_id, group_id);
                    res = 1;
                }

                if (!(RULE_CTRL_GetNextRuleEntryInLookupTable(&find) == TRUE &&
                    find.existed_p &&
                    find.unit == unit &&
                    find.device_id == device_id &&
                    find.group_id == group_id &&
                    find._last_index == SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR-1))
                {
                    RULE_CTRL_LOG("%s failed\n"
                                  " unit=%lu, device_id=%lu, group_id=%lu",
                                  "RULE_CTRL_GetNextRuleEntryInLookupTable",
                                  unit, device_id, group_id);
                    res = 1;
                }
            }

        }
    }

    /* query slave only
     */
    find.flag = RULE_CTRL_FIND_NEXT_EXISTED_RULE_ENTRY;
    find.unit = 2;
    find.device_id = RULE_CTRL_INVALID;
    find.group_id = RULE_CTRL_INVALID;
    find.rule_id = RULE_CTRL_INVALID;

    for (unit=2; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; unit++)
    {
        for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; device_id++)
        {
            for (group_id = 0; group_id < SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP; group_id++)
            {

                if (!(RULE_CTRL_GetNextRuleEntryInLookupTable(&find) == TRUE &&
                    find.existed_p &&
                    find.unit == unit &&
                    find.device_id == device_id &&
                    find.group_id == group_id &&
                    find._last_index == 0))
                {
                    RULE_CTRL_LOG("%s failed\n"
                                  " unit=%lu, device_id=%lu, group_id=%lu",
                                  "RULE_CTRL_GetNextRuleEntryInLookupTable",
                                  unit, device_id, group_id);
                    res = 1;
                }

                if (!(RULE_CTRL_GetNextRuleEntryInLookupTable(&find) == TRUE &&
                    find.existed_p &&
                    find.unit == unit &&
                    find.device_id == device_id &&
                    find.group_id == group_id &&
                    find._last_index == SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR/2))
                {
                    RULE_CTRL_LOG("%s failed\n"
                                  " unit=%lu, device_id=%lu, group_id=%lu",
                                  "RULE_CTRL_GetNextRuleEntryInLookupTable",
                                  unit, device_id, group_id);
                    res = 1;
                }

                if (!(RULE_CTRL_GetNextRuleEntryInLookupTable(&find) == TRUE &&
                    find.existed_p &&
                    find.unit == unit &&
                    find.device_id == device_id &&
                    find.group_id == group_id &&
                    find._last_index == SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR-1))
                {
                    RULE_CTRL_LOG("%s failed\n"
                                  " unit=%lu, device_id=%lu, group_id=%lu",
                                  "RULE_CTRL_GetNextRuleEntryInLookupTable",
                                  unit, device_id, group_id);
                    res = 1;
                }
            }

        }
    }

    //memset(rule_lookup_tbl, 0, sizeof(rule_lookup_tbl));

    return res;
}

int RULE_CTRL_TestTrapHTTPCToCPUForWebAuthRule()
{
#define _TESTTEE      RULE_CTRL_WA_TrapHttpToCPU
#define _TESTTEE_STR  "RULE_CTRL_WA_TrapHttpToCPU"
#define _GET_OM      RULE_CTRL_WA_GETOM_TrapHttpToCPU
#define _CFG_RULE    RULE_CTRL_WA_SETRULE_TrapHttpToCPU

    UI32_T                      unit;
    UI32_T                      port;
    int                         res = 0;
    RULE_TYPE_FunctionType_T    fun_type;


    /****************************************************************/
    /****************************************************************/
    {
        RULE_CTRL_PARAM         param       = {0};
        bcm_pbmp_t              port_pbmp   = {0};

        _CFG_RULE(&param, port_pbmp);
        fun_type = param.func_type;
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 1;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (!(0 == memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp))))
        {
            RULE_CTRL_LOG("Port bitmap in rule does not match");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (!(0 == memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp))))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}, {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 3;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
            {
                RULE_CTRL_LOG("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap does not match\n");
                res = 1;
            }

            /* Check OM
             */
            {
                RULE_CTRL_SPRRuleStorage_T *rs_p;
                RULE_CTRL_PARAM             param   = {0};

                param.unit  = unit;
                param.port  = port;
                rs_p = _GET_OM(&param);

                if (!(rs_p != NULL))
                {
                    RULE_CTRL_LOG("Can not get OM");
                    res = 1;
                }

                if (rs_p)
                {
                    if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                    {
                        RULE_CTRL_LOG("Port bitmap in OM does not match");
                        res = 1;
                    }

                    if (!(TRUE == rs_p->rule_storage.is_enable))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }

                    if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 1;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
            {
                RULE_CTRL_LOG("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap does not match\n");
                res = 1;
            }

            /* Check OM
             */
            {
                RULE_CTRL_SPRRuleStorage_T *rs_p;
                RULE_CTRL_PARAM             param   = {0};

                param.unit  = 1;
                param.port  = 29;
                rs_p = _GET_OM(&param);

                if (!(rs_p != NULL))
                {
                    RULE_CTRL_LOG("Can not get OM");
                    res = 1;
                }

                if (rs_p)
                {
                    if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                    {
                        RULE_CTRL_LOG("Port bitmap in OM does not match");
                        res = 1;
                    }

                    if (!(TRUE == rs_p->rule_storage.is_enable))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }

                    if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }
                }
            }

        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Destory rule failed\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* no any rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Destory rule failed\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                bcm_pbmp_t  port_pbmp;

                memset(&port_pbmp, 0, sizeof(port_pbmp));
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(FALSE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(0 == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    /* bind all port, and then unbind it
     */
    {
        RULE_CTRL_SPRRuleStorage_T *rs_p;
        RULE_CTRL_PARAM             param   = {0};
        RULE_CTRL_RuleEntry_T       re;

        EACH_UNIT(unit)
        {
            EACH_PORT(port)
            {
                if (!(TRUE == _TESTTEE(TRUE, unit+1, port+1)))
                {
                    RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
                    res = 1;
                }

                param.unit = unit+1;
                param.port = port+1;

                rs_p = _GET_OM(&param);

                RULE_CTRL_HELP_GetRuleByDevRuleID(rs_p->rule_storage.rule_id, &re);
                if (!(0 == memcmp(&rs_p->pbmp, &re.ace_entry.inports_data, sizeof(rs_p->pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap does not match");
                    res = 1;
                }
            }
        }

        EACH_UNIT(unit)
        {
            EACH_PORT(port)
            {
                if (!(TRUE == _TESTTEE(FALSE, unit+1, port+1)))
                {
                    RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
                    res = 1;
                }

                param.unit = unit+1;
                param.port = port+1;

                rs_p = _GET_OM(&param);

                {
                    bcm_pbmp_t  port_pbmp;

                    memset(&port_pbmp, 0, sizeof(port_pbmp));

                    if (0 != memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp)))
                    {
                        if (!(0 != rs_p->rule_storage.is_enable))
                        {
                            RULE_CTRL_LOG("Rule storage is inconsistent\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }

                        RULE_CTRL_HELP_GetRuleByDevRuleID(rs_p->rule_storage.rule_id, &re);
                        if (!(0 == memcmp(&rs_p->pbmp, &re.ace_entry.inports_data, sizeof(rs_p->pbmp))))
                        {
                            RULE_CTRL_LOG("Port bitmap does not match\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }
                    }
                    else
                    {
                        if (!(0 == rs_p->rule_storage.is_enable))
                        {
                            RULE_CTRL_LOG("Rule storage is inconsistent\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }
                    }
                }
            }
        }
    }

    return res;

#undef _TESTTEE
#undef _TESTTEE_STR
#undef _GET_OM
#undef _CFG_RULE
}

int RULE_CTRL_Test_PermitDHCPCForWebAuth()
{
#define _TESTTEE      RULE_CTRL_WA_PermitDhcp
#define _TESTTEE_STR  "RULE_CTRL_WA_PermitDhcp"
#define _GET_OM      RULE_CTRL_WA_GETOM_PermitDhcp
#define _CFG_RULE    RULE_CTRL_WA_SETRULE_PermitDhcp

    UI32_T                      unit;
    UI32_T                      port;
    int                         res = 0;
    RULE_TYPE_FunctionType_T    fun_type;


    /****************************************************************/
    /****************************************************************/
    {
        RULE_CTRL_PARAM         param       = {0};
        bcm_pbmp_t              port_pbmp   = {0};

        _CFG_RULE(&param, port_pbmp);
        fun_type = param.func_type;
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 1;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (!(0 == memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp))))
        {
            RULE_CTRL_LOG("Port bitmap in rule does not match");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (!(0 == memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp))))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}, {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 3;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
            {
                RULE_CTRL_LOG("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap does not match\n");
                res = 1;
            }

            /* Check OM
             */
            {
                RULE_CTRL_SPRRuleStorage_T *rs_p;
                RULE_CTRL_PARAM             param   = {0};

                param.unit  = unit;
                param.port  = port;
                rs_p = _GET_OM(&param);

                if (!(rs_p != NULL))
                {
                    RULE_CTRL_LOG("Can not get OM");
                    res = 1;
                }

                if (rs_p)
                {
                    if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                    {
                        RULE_CTRL_LOG("Port bitmap in OM does not match");
                        res = 1;
                    }

                    if (!(TRUE == rs_p->rule_storage.is_enable))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }

                    if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 1;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
            {
                RULE_CTRL_LOG("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap does not match\n");
                res = 1;
            }

            /* Check OM
             */
            {
                RULE_CTRL_SPRRuleStorage_T *rs_p;
                RULE_CTRL_PARAM             param   = {0};

                param.unit  = 1;
                param.port  = 29;
                rs_p = _GET_OM(&param);

                if (!(rs_p != NULL))
                {
                    RULE_CTRL_LOG("Can not get OM");
                    res = 1;
                }

                if (rs_p)
                {
                    if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                    {
                        RULE_CTRL_LOG("Port bitmap in OM does not match");
                        res = 1;
                    }

                    if (!(TRUE == rs_p->rule_storage.is_enable))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }

                    if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }
                }
            }

        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Destory rule failed\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* no any rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Destory rule failed\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                bcm_pbmp_t  port_pbmp;

                memset(&port_pbmp, 0, sizeof(port_pbmp));
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(FALSE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(0 == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    /* bind all port, and then unbind it
     */
    {
        RULE_CTRL_SPRRuleStorage_T *rs_p;
        RULE_CTRL_PARAM             param   = {0};
        RULE_CTRL_RuleEntry_T       re;

        EACH_UNIT(unit)
        {
            EACH_PORT(port)
            {
                if (!(TRUE == _TESTTEE(TRUE, unit+1, port+1)))
                {
                    RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
                    res = 1;
                }

                param.unit = unit+1;
                param.port = port+1;

                rs_p = _GET_OM(&param);

                RULE_CTRL_HELP_GetRuleByDevRuleID(rs_p->rule_storage.rule_id, &re);
                if (!(0 == memcmp(&rs_p->pbmp, &re.ace_entry.inports_data, sizeof(rs_p->pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap does not match");
                    res = 1;
                }
            }
        }

        EACH_UNIT(unit)
        {
            EACH_PORT(port)
            {
                if (!(TRUE == _TESTTEE(FALSE, unit+1, port+1)))
                {
                    RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
                    res = 1;
                }

                param.unit = unit+1;
                param.port = port+1;

                rs_p = _GET_OM(&param);

                {
                    bcm_pbmp_t  port_pbmp;

                    memset(&port_pbmp, 0, sizeof(port_pbmp));

                    if (0 != memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp)))
                    {
                        if (!(0 != rs_p->rule_storage.is_enable))
                        {
                            RULE_CTRL_LOG("Rule storage is inconsistent\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }

                        RULE_CTRL_HELP_GetRuleByDevRuleID(rs_p->rule_storage.rule_id, &re);
                        if (!(0 == memcmp(&rs_p->pbmp, &re.ace_entry.inports_data, sizeof(rs_p->pbmp))))
                        {
                            RULE_CTRL_LOG("Port bitmap does not match\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }
                    }
                    else
                    {
                        if (!(0 == rs_p->rule_storage.is_enable))
                        {
                            RULE_CTRL_LOG("Rule storage is inconsistent\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }
                    }
                }
            }
        }
    }

    return res;

#undef _TESTTEE
#undef _TESTTEE_STR
#undef _GET_OM
#undef _CFG_RULE
}

int RULE_CTRL_Test_PermitDNSForWebAuth()
{
#define _TESTTEE      RULE_CTRL_WA_PermitDns
#define _TESTTEE_STR  "RULE_CTRL_WA_PermitDns"
#define _GET_OM      RULE_CTRL_WA_GETOM_PermitDns
#define _CFG_RULE    RULE_CTRL_WA_SETRULE_PermitDns

    UI32_T                      unit;
    UI32_T                      port;
    int                         res = 0;
    RULE_TYPE_FunctionType_T    fun_type;


    /****************************************************************/
    /****************************************************************/
    {
        RULE_CTRL_PARAM         param       = {0};
        bcm_pbmp_t              port_pbmp   = {0};

        _CFG_RULE(&param, port_pbmp);
        fun_type = param.func_type;
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 1;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (!(0 == memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp))))
        {
            RULE_CTRL_LOG("Port bitmap in rule does not match");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (!(0 == memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp))))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}, {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 3;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
            {
                RULE_CTRL_LOG("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap does not match\n");
                res = 1;
            }

            /* Check OM
             */
            {
                RULE_CTRL_SPRRuleStorage_T *rs_p;
                RULE_CTRL_PARAM             param   = {0};

                param.unit  = unit;
                param.port  = port;
                rs_p = _GET_OM(&param);

                if (!(rs_p != NULL))
                {
                    RULE_CTRL_LOG("Can not get OM");
                    res = 1;
                }

                if (rs_p)
                {
                    if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                    {
                        RULE_CTRL_LOG("Port bitmap in OM does not match");
                        res = 1;
                    }

                    if (!(TRUE == rs_p->rule_storage.is_enable))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }

                    if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 1;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
            {
                RULE_CTRL_LOG("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap does not match\n");
                res = 1;
            }

            /* Check OM
             */
            {
                RULE_CTRL_SPRRuleStorage_T *rs_p;
                RULE_CTRL_PARAM             param   = {0};

                param.unit  = 1;
                param.port  = 29;
                rs_p = _GET_OM(&param);

                if (!(rs_p != NULL))
                {
                    RULE_CTRL_LOG("Can not get OM");
                    res = 1;
                }

                if (rs_p)
                {
                    if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                    {
                        RULE_CTRL_LOG("Port bitmap in OM does not match");
                        res = 1;
                    }

                    if (!(TRUE == rs_p->rule_storage.is_enable))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }

                    if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }
                }
            }

        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Destory rule failed\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* no any rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Destory rule failed\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                bcm_pbmp_t  port_pbmp;

                memset(&port_pbmp, 0, sizeof(port_pbmp));
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(FALSE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(0 == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    /* bind all port, and then unbind it
     */
    {
        RULE_CTRL_SPRRuleStorage_T *rs_p;
        RULE_CTRL_PARAM             param   = {0};
        RULE_CTRL_RuleEntry_T       re;

        EACH_UNIT(unit)
        {
            EACH_PORT(port)
            {
                if (!(TRUE == _TESTTEE(TRUE, unit+1, port+1)))
                {
                    RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
                    res = 1;
                }

                param.unit = unit+1;
                param.port = port+1;

                rs_p = _GET_OM(&param);

                RULE_CTRL_HELP_GetRuleByDevRuleID(rs_p->rule_storage.rule_id, &re);
                if (!(0 == memcmp(&rs_p->pbmp, &re.ace_entry.inports_data, sizeof(rs_p->pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap does not match");
                    res = 1;
                }
            }
        }

        EACH_UNIT(unit)
        {
            EACH_PORT(port)
            {
                if (!(TRUE == _TESTTEE(FALSE, unit+1, port+1)))
                {
                    RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
                    res = 1;
                }

                param.unit = unit+1;
                param.port = port+1;

                rs_p = _GET_OM(&param);

                {
                    bcm_pbmp_t  port_pbmp;

                    memset(&port_pbmp, 0, sizeof(port_pbmp));

                    if (0 != memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp)))
                    {
                        if (!(0 != rs_p->rule_storage.is_enable))
                        {
                            RULE_CTRL_LOG("Rule storage is inconsistent\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }

                        RULE_CTRL_HELP_GetRuleByDevRuleID(rs_p->rule_storage.rule_id, &re);
                        if (!(0 == memcmp(&rs_p->pbmp, &re.ace_entry.inports_data, sizeof(rs_p->pbmp))))
                        {
                            RULE_CTRL_LOG("Port bitmap does not match\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }
                    }
                    else
                    {
                        if (!(0 == rs_p->rule_storage.is_enable))
                        {
                            RULE_CTRL_LOG("Rule storage is inconsistent\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }
                    }
                }
            }
        }
    }

    return res;

#undef _TESTTEE
#undef _TESTTEE_STR
#undef _GET_OM
#undef _CFG_RULE
}

int RULE_CTRL_Test_DenyAnyIPPacketForWebAuth()
{
#define _TESTTEE      RULE_CTRL_WA_DenyAnyIpPacket
#define _TESTTEE_STR  "RULE_CTRL_WA_DenyAnyIpPacket"
#define _GET_OM      RULE_CTRL_WA_GETOM_DenyAnyIpPacket
#define _CFG_RULE    RULE_CTRL_WA_SETRULE_DenyAnyIpPacket

    UI32_T                      unit;
    UI32_T                      port;
    int                         res = 0;
    RULE_TYPE_FunctionType_T    fun_type;


    /****************************************************************/
    /****************************************************************/
    {
        RULE_CTRL_PARAM         param       = {0};
        bcm_pbmp_t              port_pbmp   = {0};

        _CFG_RULE(&param, port_pbmp);
        fun_type = param.func_type;
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 1;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (!(0 == memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp))))
        {
            RULE_CTRL_LOG("Port bitmap in rule does not match");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (!(0 == memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp))))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}, {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 3;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
            {
                RULE_CTRL_LOG("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap does not match\n");
                res = 1;
            }

            /* Check OM
             */
            {
                RULE_CTRL_SPRRuleStorage_T *rs_p;
                RULE_CTRL_PARAM             param   = {0};

                param.unit  = unit;
                param.port  = port;
                rs_p = _GET_OM(&param);

                if (!(rs_p != NULL))
                {
                    RULE_CTRL_LOG("Can not get OM");
                    res = 1;
                }

                if (rs_p)
                {
                    if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                    {
                        RULE_CTRL_LOG("Port bitmap in OM does not match");
                        res = 1;
                    }

                    if (!(TRUE == rs_p->rule_storage.is_enable))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }

                    if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 1;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
            {
                RULE_CTRL_LOG("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap does not match\n");
                res = 1;
            }

            /* Check OM
             */
            {
                RULE_CTRL_SPRRuleStorage_T *rs_p;
                RULE_CTRL_PARAM             param   = {0};

                param.unit  = 1;
                param.port  = 29;
                rs_p = _GET_OM(&param);

                if (!(rs_p != NULL))
                {
                    RULE_CTRL_LOG("Can not get OM");
                    res = 1;
                }

                if (rs_p)
                {
                    if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                    {
                        RULE_CTRL_LOG("Port bitmap in OM does not match");
                        res = 1;
                    }

                    if (!(TRUE == rs_p->rule_storage.is_enable))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }

                    if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }
                }
            }

        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Destory rule failed\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* no any rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Destory rule failed\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                bcm_pbmp_t  port_pbmp;

                memset(&port_pbmp, 0, sizeof(port_pbmp));
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(FALSE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(0 == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    /* bind all port, and then unbind it
     */
    {
        RULE_CTRL_SPRRuleStorage_T *rs_p;
        RULE_CTRL_PARAM             param   = {0};
        RULE_CTRL_RuleEntry_T       re;

        EACH_UNIT(unit)
        {
            EACH_PORT(port)
            {
                if (!(TRUE == _TESTTEE(TRUE, unit+1, port+1)))
                {
                    RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
                    res = 1;
                }

                param.unit = unit+1;
                param.port = port+1;

                rs_p = _GET_OM(&param);

                RULE_CTRL_HELP_GetRuleByDevRuleID(rs_p->rule_storage.rule_id, &re);
                if (!(0 == memcmp(&rs_p->pbmp, &re.ace_entry.inports_data, sizeof(rs_p->pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap does not match");
                    res = 1;
                }
            }
        }

        EACH_UNIT(unit)
        {
            EACH_PORT(port)
            {
                if (!(TRUE == _TESTTEE(FALSE, unit+1, port+1)))
                {
                    RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
                    res = 1;
                }

                param.unit = unit+1;
                param.port = port+1;

                rs_p = _GET_OM(&param);

                {
                    bcm_pbmp_t  port_pbmp;

                    memset(&port_pbmp, 0, sizeof(port_pbmp));

                    if (0 != memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp)))
                    {
                        if (!(0 != rs_p->rule_storage.is_enable))
                        {
                            RULE_CTRL_LOG("Rule storage is inconsistent\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }

                        RULE_CTRL_HELP_GetRuleByDevRuleID(rs_p->rule_storage.rule_id, &re);
                        if (!(0 == memcmp(&rs_p->pbmp, &re.ace_entry.inports_data, sizeof(rs_p->pbmp))))
                        {
                            RULE_CTRL_LOG("Port bitmap does not match\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }
                    }
                    else
                    {
                        if (!(0 == rs_p->rule_storage.is_enable))
                        {
                            RULE_CTRL_LOG("Rule storage is inconsistent\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }
                    }
                }
            }
        }
    }

    return res;

#undef _TESTTEE
#undef _TESTTEE_STR
#undef _GET_OM
#undef _CFG_RULE
}

int RULE_CTRL_Test_PermitIPPacketForWebAuth()
{
#define _TESTTEE        RULE_CTRL_WA_PermitIpPacket
#define _TESTTEE_STR    "RULE_CTRL_WA_PermitIpPacket"
#define _GET_OM         RULE_CTRL_WA_GetPermitIpRuleStorage
#define _CFG_RULE       ?
#define _FUNC_TYPE      RULE_TYPE_WEBAUTH_PRIO_HIGH

    RULE_CTRL_WebAuthPermitIPRuleStorage_T  *rs;
    int                                     res = 0;
    int                                     i;
    UI32_T                                  unit;
    UI32_T                                  port;
    UI8_T                                   ipv4_addr[4]    = {0};

	unit	= 1;
	port	= 1;
	ipv4_addr[0] = 192; ipv4_addr[1] = 168; ipv4_addr[2] = 1; ipv4_addr[3] = 2;

	if (!( TRUE == _TESTTEE(TRUE, unit, port, ipv4_addr) ))
	{
		RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
		res = 1;
	}

    //RULE_CTRL_HELP_DumpRule();
    /* one rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(_FUNC_TYPE,  &re)))
        {
            RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
            res = 1;
        }
    }

	if (!( TRUE == _TESTTEE(TRUE, unit, port, ipv4_addr) ))
	{
		RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
		res = 1;
	}

    //RULE_CTRL_HELP_DumpRule();
    /* one rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(_FUNC_TYPE,  &re)))
        {
            RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
            res = 1;
        }

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(_FUNC_TYPE,  &re)))
        {
            RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
            res = 1;
        }
    }

	if (!( TRUE == _TESTTEE(FALSE, unit, port, ipv4_addr) ))
	{
		RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
		res = 1;
	}

	if (!( TRUE == _TESTTEE(FALSE, unit, port, ipv4_addr) ))
	{
		RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
		res = 1;
	}

    //RULE_CTRL_HELP_DumpRule();
    /* no rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(_FUNC_TYPE,  &re)))
        {
            RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
            res = 1;
        }
    }

    EACH_UNIT(unit)
    {
        EACH_PORT(port)
        {
            for (i=0; i < SYS_ADPT_WEBAUTH_MAX_NBR_OF_HOSTS_PER_PORT; i++)
            {
                ipv4_addr[3] = i;

                //RULE_CTRL_HELP_DumpRule();

            	if (!( TRUE == _TESTTEE(TRUE, unit+1, port+1, ipv4_addr) ))
                {
	                RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
	                res = 1;
                }

                {
                    UI32_T                                      dev_rule_id;

                    dev_rule_id = 0XFFFFFFFF;

                    /* check OM
                     */
                    {
                        UI32_T                                  module_id, device_id, phy_port;
                        RULE_CTRL_WebAuthPermitIPRuleStorage_T  *rs_p;
                        bcm_pbmp_t                              new_pbmp;

                        rs_p = _GET_OM(unit+1,
                                       port+1,
                                       ipv4_addr);

                        if (!(NULL != rs_p))
                        {
                            RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
                        }

                        if (rs_p)
                        {
                            if (!(
                                       0 == memcmp(rs_p->src_ipv4_addr, ipv4_addr, sizeof(ipv4_addr))
                                     ))
                            {
                                RULE_CTRL_LOG("OM does not match\n");
                                res = 1;
                            }

                            dev_rule_id = rs_p->rule_storage.rule_id;
                        }
                    }

                    /* check rule
                     */
                    {
                        RULE_CTRL_USERPORT up[] =
                        {
                            {unit+1,port+1},
                        };

                        bcm_pbmp_t  port_pbmp;

                        RULE_CTRL_RuleEntry_T re;
                        re.dev_rule_id = 0xFFFFFFFF;

                        /* find rule entry by dev_rule_id and check it memory ...
                         */
                        while (TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(_FUNC_TYPE,  &re))
                        {
                            if (re.dev_rule_id == dev_rule_id)
                            {
                                port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

                                if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
                                {
                                    RULE_CTRL_LOG("Port bitmap does not match\n");
                                    res = 1;
                                }
                                break;
                            }
                        }
                    }
                } /* end of test */
            }

            //RULE_CTRL_HELP_DumpRule();
            {
                int                     count = 0;
                RULE_CTRL_RuleEntry_T   re;
                re.dev_rule_id = 0xFFFFFFFF;

                while (TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(_FUNC_TYPE,  &re))
                {
                    count ++;
                }

                if (!(count == SYS_ADPT_WEBAUTH_MAX_NBR_OF_HOSTS_PER_PORT))
                {
                    RULE_CTRL_LOG("Count does not match\n");
                    res = 1;
                }
            }

            for (i=0; i < SYS_ADPT_WEBAUTH_MAX_NBR_OF_HOSTS_PER_PORT; i++)
            {
                ipv4_addr[3] = i;

                //RULE_CTRL_HELP_DumpRule();

            	if (!( TRUE == _TESTTEE(FALSE, unit+1, port+1, ipv4_addr) ))
                {
	                RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
	                res = 1;
                }
            }

            {
                int                     count = 0;
                RULE_CTRL_RuleEntry_T   re;
                re.dev_rule_id = 0xFFFFFFFF;

                while (TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(_FUNC_TYPE,  &re))
                {
                    count ++;
                }

                if (!(count == 0))
                {
                    RULE_CTRL_LOG("Count does not match\n");
                    res = 1;
                }
            }
        }
    }

	return res;

#undef _TESTTEE
#undef _TESTTEE_STR
#undef _GET_OM
#undef _CFG_RULE
}

int RULE_CTRL_TestTrapDot1xUCToCPURule()
{
    UI32_T      unit;
    UI32_T      port;
    bcm_pbmp_t  port_pbmp;
    int         res = 0;
    UI8_T       cpu_mac[] = {0x00, 0x00, 0x00, 0x00, 0x11, 0x22};

    unit = 1;
    port = 1;
    if (FALSE == RULE_CTRL_1X_TrapDot1xUCToCPU(TRUE, unit, port, cpu_mac))
    {
        printf("RULE_CTRL_1X_TrapDot1xUCToCPU failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,1}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,1},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_PACKET_TO_CPU_DEFAULT,  &re))
        {
            printf("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            printf("Port bitmap does not match\n");
            res = 1;
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (FALSE == RULE_CTRL_1X_TrapDot1xUCToCPU(TRUE, unit, port, cpu_mac))
    {
        printf("RULE_CTRL_1X_TrapDot1xUCToCPU failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,1},
            {1,2},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_PACKET_TO_CPU_DEFAULT,  &re))
        {
            printf("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            printf("Port bitmap does not match\n");
            res = 1;
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (FALSE == RULE_CTRL_1X_TrapDot1xUCToCPU(TRUE, unit, port, cpu_mac))
    {
        printf("RULE_CTRL_1X_TrapDot1xUCToCPU failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}, {1,3}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_PACKET_TO_CPU_DEFAULT,  &re))
        {
            printf("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            printf("Port bitmap does not match\n");
            res = 1;
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (FALSE == RULE_CTRL_1X_TrapDot1xUCToCPU(TRUE, unit, port, cpu_mac))
    {
        printf("RULE_CTRL_1X_TrapDot1xUCToCPU failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_PACKET_TO_CPU_DEFAULT,  &re))
        {
            printf("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            printf("Port bitmap does not match\n");
            res = 1;
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_PACKET_TO_CPU_DEFAULT,  &re))
            {
                printf("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                printf("Port bitmap does not match\n");
                res = 1;
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 1;
    if (FALSE == RULE_CTRL_1X_TrapDot1xUCToCPU(FALSE, unit, port, cpu_mac))
    {
        printf("RULE_CTRL_1X_TrapDot1xUCToCPU failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_PACKET_TO_CPU_DEFAULT,  &re))
        {
            printf("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            printf("Port bitmap does not match\n");
            res = 1;
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_PACKET_TO_CPU_DEFAULT,  &re))
            {
                printf("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                printf("Port bitmap does not match\n");
                res = 1;
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (FALSE == RULE_CTRL_1X_TrapDot1xUCToCPU(FALSE, unit, port, cpu_mac))
    {
        printf("RULE_CTRL_1X_TrapDot1xUCToCPU failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_PACKET_TO_CPU_DEFAULT,  &re))
        {
            printf("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            printf("Port bitmap does not match\n");
            res = 1;
        }

        if (TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_PACKET_TO_CPU_DEFAULT,  &re))
        {
            printf("Destory rule failed\n");
            res = 1;
        }

    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (FALSE == RULE_CTRL_1X_TrapDot1xUCToCPU(FALSE, unit, port, cpu_mac))
    {
        printf("RULE_CTRL_1X_TrapDot1xUCToCPU failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_PACKET_TO_CPU_DEFAULT,  &re))
        {
            printf("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            printf("Port bitmap does not match\n");
            res = 1;
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (FALSE == RULE_CTRL_1X_TrapDot1xUCToCPU(FALSE, unit, port, cpu_mac))
    {
        printf("RULE_CTRL_1X_TrapDot1xUCToCPU failed\n");
        res = 1;
    }

    /* no any rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_PACKET_TO_CPU_DEFAULT,  &re))
        {
            printf("Destory rule failed\n");
            res = 1;
        }
    }

    return res;
}

int RULE_CTRL_TestIPSGOM()
{
    RULE_CTRL_IPSGPermitIPRuleStorage_T     *rs;
    int                                     res = 0;
    int                                     i;
    UI32_T                                  unit;
    UI32_T                                  port;
    UI32_T                                  vlan_id;
    UI8_T                                   ipv4_addr[4]    = {0};
    UI8_T                                   mac_addr[SYS_ADPT_MAC_ADDR_LEN] = {0};

    vlan_id         = 1;
    ipv4_addr[3]    = 0x01;
    mac_addr[5]     = 0x01;

    /* Invalid User Port
     */
    unit            = 0;
    port            = 0;
    rs = RULE_CTRL_IPSG_GetPermitIPRuleStorage(unit,
                                              port,
                                              vlan_id,
                                              ipv4_addr,
                                              mac_addr);
    if (!(NULL == rs))
    {
        printf("RULE_CTRL_IPSG_GetPermitIPRuleStorage failed\n");
        res = 1;
    }

    unit            = 1;
    port            = 1;
    rs = RULE_CTRL_IPSG_GetPermitIPRuleStorage(unit,
                                              port,
                                              vlan_id,
                                              ipv4_addr,
                                              mac_addr);
    if (!(NULL == rs))
    {
        printf("RULE_CTRL_IPSG_GetPermitIPRuleStorage failed\n");
        res = 1;
    }

    for (i=0; i < SYS_ADPT_DHCPSNP_MAX_NBR_OF_CLIENT_PER_PORT*2; i++)
    {
        rs = RULE_CTRL_IPSG_AllocatePermitIPRuleStorage(unit,
                                                       port,
                                                       vlan_id,
                                                       ipv4_addr,
                                                       mac_addr);
        if (!(NULL != rs))
        {
            printf("RULE_CTRL_IPSG_AllocatePermitIPRuleStorage failed\n");
            res = 1;
        }

        rs = RULE_CTRL_IPSG_GetPermitIPRuleStorage(unit,
                                                  port,
                                                  vlan_id,
                                                  ipv4_addr,
                                                  mac_addr);
        if (!(NULL == rs))
        {
            printf("RULE_CTRL_IPSG_GetPermitIPRuleStorage failed\n");
            res = 1;
        }
    }

    return res;
}

int RULE_CTRL_Test_PermitPacketForIPSG()
{
    RULE_CTRL_IPSGPermitIPRuleStorage_T     *rs;
    int                                     res = 0;
    int                                     i;
    UI32_T                                  unit;
    UI32_T                                  port;
    UI32_T                                  vlan_id;
    UI8_T                                   ipv4_addr[4]    = {0};
    UI8_T                                   mac_addr[SYS_ADPT_MAC_ADDR_LEN] = {0};

	unit	= 1;
	port	= 1;
	vlan_id	= 1;
	ipv4_addr[0] = 192; ipv4_addr[1] = 168; ipv4_addr[2] = 1; ipv4_addr[3] = 2;
	mac_addr[4] = 0X0C; mac_addr[5] = 0X01;

	if (!( TRUE == RULE_CTRL_IPSG_PermitIpPacket(TRUE, unit, port, vlan_id, ipv4_addr, mac_addr) ))
	{
		RULE_CTRL_LOG("RULE_CTRL_PermitPacketForIPSG failed");
		res = 1;
	}

    //RULE_CTRL_HELP_DumpRule();
    /* one rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_HIGH,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }
    }

	if (!( TRUE == RULE_CTRL_IPSG_PermitIpPacket(TRUE, unit, port, vlan_id, ipv4_addr, NULL) ))
	{
		RULE_CTRL_LOG("RULE_CTRL_PermitPacketForIPSG failed\n");
		res = 1;
	}

    //RULE_CTRL_HELP_DumpRule();
    /* another rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_HIGH,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_HIGH,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }
    }

	if (!( TRUE == RULE_CTRL_IPSG_PermitIpPacket(FALSE, unit, port, vlan_id, ipv4_addr, mac_addr) ))
	{
		RULE_CTRL_LOG("RULE_CTRL_PermitPacketForIPSG failed");
		res = 1;
	}

	if (!( TRUE == RULE_CTRL_IPSG_PermitIpPacket(FALSE, unit, port, vlan_id, ipv4_addr, NULL) ))
	{
		RULE_CTRL_LOG("RULE_CTRL_PermitPacketForIPSG failed");
		res = 1;
	}

    //RULE_CTRL_HELP_DumpRule();
    /* no rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_HIGH,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }
    }

    EACH_UNIT(unit)
    {
        EACH_PORT(port)
        {
            for (i=0; i < SYS_ADPT_DHCPSNP_MAX_NBR_OF_CLIENT_PER_PORT; i++)
            {
                ipv4_addr[3] = i;
                mac_addr[5]  = i;

                //RULE_CTRL_HELP_DumpRule();

            	if (!( TRUE == RULE_CTRL_IPSG_PermitIpPacket(TRUE, unit+1, port+1, vlan_id, ipv4_addr, mac_addr) ))
                {
	                RULE_CTRL_LOG("RULE_CTRL_PermitPacketForIPSG failed");
	                res = 1;
                }

                {
                    UI32_T                                      dev_rule_id;

                    dev_rule_id = 0XFFFFFFFF;

                    /* check OM
                     */
                    {
                        UI32_T                                  module_id, device_id, phy_port;
                        RULE_CTRL_IPSGPermitIPRuleStorage_T    *rs_p;
                        bcm_pbmp_t                              new_pbmp;

                        rs_p = RULE_CTRL_IPSG_GetPermitIPRuleStorage(unit+1,
                                                                    port+1,
                                                                    vlan_id,
                                                                    ipv4_addr,
                                                                    mac_addr);

                        if (!(NULL != rs_p))
                        {
                            RULE_CTRL_LOG("RULE_CTRL_PermitPacketForIPSG failed\n");
                        }

                        if (rs_p)
                        {
                            if (!(  rs_p->vlan_id == vlan_id
                                    && 0 == memcmp(rs_p->ipv4_addr, ipv4_addr, sizeof(ipv4_addr))
                                    && 0 == memcmp(rs_p->mac_addr, mac_addr, sizeof(mac_addr)) ))
                            {
                                RULE_CTRL_LOG("OM does not match\n");
                                res = 1;
                            }

                            dev_rule_id = rs_p->rule_storage.rule_id;
                        }
                    }

                    /* check rule
                     */
                    {
                        RULE_CTRL_USERPORT up[] =
                        {
                            {unit+1,port+1},
                        };

                        bcm_pbmp_t  port_pbmp;

                        RULE_CTRL_RuleEntry_T re;
                        re.dev_rule_id = 0xFFFFFFFF;

                        /* find rule entry by dev_rule_id and check it memory ...
                         */
                        while (TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_HIGH,  &re))
                        {
                            if (re.dev_rule_id == dev_rule_id)
                            {
                                port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

                                if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
                                {
                                    RULE_CTRL_LOG("Port bitmap does not match\n");
                                    res = 1;
                                }
                                break;
                            }
                        }
                    }
                } /* end of test */
            }

            //RULE_CTRL_HELP_DumpRule();
            {
                int                     count = 0;
                RULE_CTRL_RuleEntry_T   re;
                re.dev_rule_id = 0xFFFFFFFF;

                while (TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_HIGH,  &re))
                {
                    count ++;
                }

                if (!(count == SYS_ADPT_DHCPSNP_MAX_NBR_OF_CLIENT_PER_PORT))
                {
                    RULE_CTRL_LOG("Count does not match\n");
                    res = 1;
                }
            }

            for (i=0; i < SYS_ADPT_DHCPSNP_MAX_NBR_OF_CLIENT_PER_PORT; i++)
            {
                ipv4_addr[3] = i;
                mac_addr[5]  = i;

                //RULE_CTRL_HELP_DumpRule();

            	if (!( TRUE == RULE_CTRL_IPSG_PermitIpPacket(FALSE, unit+1, port+1, vlan_id, ipv4_addr, mac_addr) ))
                {
	                RULE_CTRL_LOG("RULE_CTRL_PermitPacketForIPSG failed");
	                res = 1;
                }
            }

            {
                int                     count = 0;
                RULE_CTRL_RuleEntry_T   re;
                re.dev_rule_id = 0xFFFFFFFF;

                while (TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_HIGH,  &re))
                {
                    count ++;
                }

                if (!(count == 0))
                {
                    RULE_CTRL_LOG("Count does not match\n");
                    res = 1;
                }
            }
        }
    }

	return res;
}

int RULE_CTRL_Test_DenyAnyIPPacketForIPSGRule()
{
    UI32_T      unit;
    UI32_T      port;
    bcm_pbmp_t  port_pbmp;
    int         res = 0;
    UI8_T       cpu_mac[] = {0x00, 0x00, 0x00, 0x00, 0x11, 0x22};

    unit = 1;
    port = 1;
    if (FALSE == RULE_CTRL_IPSG_DenyAnyIpPacket(TRUE, unit, port))
    {
        RULE_CTRL_LOG("RULE_CTRL_IPSG_DenyAnyIpPacket failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,1}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,1},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW,  &re))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* check port bitmap on OM
         */
        {
            UI32_T                                  module_id, device_id, phy_port;
            RULE_CTRL_IPSGDenyAnyIPRuleStorage_T    *entry_p;
            bcm_pbmp_t                              new_pbmp;

            if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port,
                &module_id, &device_id, &phy_port))
            {
                RULE_CTRL_LOG("DEV_SWDRV_PMGR_Logical2PhyDevicePortID failed\n");
            }

            entry_p = &shmem_data_p->ipsg_deny_any_ip[unit-1][device_id];

            if (0 != memcmp(&entry_p->pbmp, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap in OM does not match\n");
                res = 1;
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (FALSE == RULE_CTRL_IPSG_DenyAnyIpPacket(TRUE, unit, port))
    {
        RULE_CTRL_LOG("RULE_CTRL_IPSG_DenyAnyIpPacket failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,1},
            {1,2},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW,  &re))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* check port bitmap on OM
         */
        {
            UI32_T                                  module_id, device_id, phy_port;
            RULE_CTRL_IPSGDenyAnyIPRuleStorage_T    *entry_p;
            bcm_pbmp_t                              new_pbmp;

            if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port,
                &module_id, &device_id, &phy_port))
            {
                RULE_CTRL_LOG("DEV_SWDRV_PMGR_Logical2PhyDevicePortID failed\n");
            }

            entry_p = &shmem_data_p->ipsg_deny_any_ip[unit-1][device_id];

            if (0 != memcmp(&entry_p->pbmp, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap in OM does not match\n");
                res = 1;
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (FALSE == RULE_CTRL_IPSG_DenyAnyIpPacket(TRUE, unit, port))
    {
        RULE_CTRL_LOG("RULE_CTRL_IPSG_DenyAnyIpPacket failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}, {1,3}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW,  &re))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* check port bitmap on OM
         */
        {
            UI32_T                                  module_id, device_id, phy_port;
            RULE_CTRL_IPSGDenyAnyIPRuleStorage_T    *entry_p;
            bcm_pbmp_t                              new_pbmp;

            if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port,
                &module_id, &device_id, &phy_port))
            {
                RULE_CTRL_LOG("DEV_SWDRV_PMGR_Logical2PhyDevicePortID failed\n");
            }

            entry_p = &shmem_data_p->ipsg_deny_any_ip[unit-1][device_id];

            if (0 != memcmp(&entry_p->pbmp, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap in OM does not match\n");
                res = 1;
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (FALSE == RULE_CTRL_IPSG_DenyAnyIpPacket(TRUE, unit, port))
    {
        RULE_CTRL_LOG("RULE_CTRL_IPSG_DenyAnyIpPacket failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW,  &re))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* check port bitmap on OM
         */
        {
            UI32_T                                  module_id, device_id, phy_port;
            RULE_CTRL_IPSGDenyAnyIPRuleStorage_T    *entry_p;
            bcm_pbmp_t                              new_pbmp;

            UI32_T                                  unit = 1;
            UI32_T                                  port = 1;

            if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port,
                &module_id, &device_id, &phy_port))
            {
                RULE_CTRL_LOG("DEV_SWDRV_PMGR_Logical2PhyDevicePortID failed\n");
            }

            entry_p = &shmem_data_p->ipsg_deny_any_ip[unit-1][device_id];

            if (0 != memcmp(&entry_p->pbmp, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap in OM does not match\n");
                res = 1;
            }
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW,  &re))
            {
                RULE_CTRL_LOG("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap does not match\n");
                res = 1;
            }

            /* check port bitmap on OM
             */
            {
                UI32_T                                  module_id, device_id, phy_port;
                RULE_CTRL_IPSGDenyAnyIPRuleStorage_T    *entry_p;
                bcm_pbmp_t                              new_pbmp;

                if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port,
                    &module_id, &device_id, &phy_port))
                {
                    RULE_CTRL_LOG("DEV_SWDRV_PMGR_Logical2PhyDevicePortID failed\n");
                }

                entry_p = &shmem_data_p->ipsg_deny_any_ip[unit-1][device_id];

                if (0 != memcmp(&entry_p->pbmp, &port_pbmp, sizeof(port_pbmp)))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match\n");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 1;
    if (FALSE == RULE_CTRL_IPSG_DenyAnyIpPacket(FALSE, unit, port))
    {
        RULE_CTRL_LOG("RULE_CTRL_IPSG_DenyAnyIpPacket failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW,  &re))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW,  &re))
            {
                RULE_CTRL_LOG("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap does not match\n");
                res = 1;
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (FALSE == RULE_CTRL_IPSG_DenyAnyIpPacket(FALSE, unit, port))
    {
        RULE_CTRL_LOG("RULE_CTRL_IPSG_DenyAnyIpPacket failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW,  &re))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        if (TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW,  &re))
        {
            RULE_CTRL_LOG("Destory rule failed\n");
            res = 1;
        }

    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (FALSE == RULE_CTRL_IPSG_DenyAnyIpPacket(FALSE, unit, port))
    {
        RULE_CTRL_LOG("RULE_CTRL_IPSG_DenyAnyIpPacket failed\n");
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        RULE_CTRL_USERPORT up[] =
        {
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW,  &re))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* check port bitmap on OM
         */
        {
            UI32_T                                  module_id, device_id, phy_port;
            RULE_CTRL_IPSGDenyAnyIPRuleStorage_T    *entry_p;
            bcm_pbmp_t                              new_pbmp;

            if (FALSE == DEV_SWDRV_PMGR_Logical2PhyDevicePortID(unit, port,
                &module_id, &device_id, &phy_port))
            {
                RULE_CTRL_LOG("DEV_SWDRV_PMGR_Logical2PhyDevicePortID failed\n");
            }

            entry_p = &shmem_data_p->ipsg_deny_any_ip[unit-1][device_id];

            if (0 != memcmp(&entry_p->pbmp, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap in OM does not match\n");
                res = 1;
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (FALSE == RULE_CTRL_IPSG_DenyAnyIpPacket(FALSE, unit, port))
    {
        RULE_CTRL_LOG("RULE_CTRL_IPSG_DenyAnyIpPacket failed\n");
        res = 1;
    }

    /* no any rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW,  &re))
        {
            RULE_CTRL_LOG("Destory rule failed\n");
            res = 1;
        }
    }

    return res;
}

int RULE_CTRL_Test_TrapRAPSToCPU()
{
#define _TESTTEE        RULE_CTRL_ERPS_TrapRapsPduToCPU
#define _TESTTEE_STR    "RULE_CTRL_ERPS_TrapRapsPduToCPU"
#define _GET_OM         RULE_CTRL_ERPS_GETOM_TrapRapsPduToCPU
#define _CFG_RULE       RULE_CTRL_ERPS_SETRULE_TrapRapsPduToCPU

    UI32_T                      unit;
    UI32_T                      port;
    int                         res = 0;
    RULE_TYPE_FunctionType_T    fun_type;


    /****************************************************************/
    /****************************************************************/
    {
        RULE_CTRL_PARAM         param       = {0};
        bcm_pbmp_t              port_pbmp   = {0};

        _CFG_RULE(&param, port_pbmp);
        fun_type = param.func_type;
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 1;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (!(0 == memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp))))
        {
            RULE_CTRL_LOG("Port bitmap in rule does not match");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (!(0 == memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp))))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2}, {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (!(TRUE == _TESTTEE(TRUE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,1} {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,1},
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 3;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
            {
                RULE_CTRL_LOG("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap does not match\n");
                res = 1;
            }

            /* Check OM
             */
            {
                RULE_CTRL_SPRRuleStorage_T *rs_p;
                RULE_CTRL_PARAM             param   = {0};

                param.unit  = unit;
                param.port  = port;
                rs_p = _GET_OM(&param);

                if (!(rs_p != NULL))
                {
                    RULE_CTRL_LOG("Can not get OM");
                    res = 1;
                }

                if (rs_p)
                {
                    if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                    {
                        RULE_CTRL_LOG("Port bitmap in OM does not match");
                        res = 1;
                    }

                    if (!(TRUE == rs_p->rule_storage.is_enable))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }

                    if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 1;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     * other rule with port bitmap {1,29}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = unit;
            param.port  = port;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }

        {
            RULE_CTRL_USERPORT up[] =
            {
                {1,29},
            };

            if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
            {
                RULE_CTRL_LOG("Create rule failed\n");
                res = 1;
            }

            port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

            if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
            {
                RULE_CTRL_LOG("Port bitmap does not match\n");
                res = 1;
            }

            /* Check OM
             */
            {
                RULE_CTRL_SPRRuleStorage_T *rs_p;
                RULE_CTRL_PARAM             param   = {0};

                param.unit  = 1;
                param.port  = 29;
                rs_p = _GET_OM(&param);

                if (!(rs_p != NULL))
                {
                    RULE_CTRL_LOG("Can not get OM");
                    res = 1;
                }

                if (rs_p)
                {
                    if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                    {
                        RULE_CTRL_LOG("Port bitmap in OM does not match");
                        res = 1;
                    }

                    if (!(TRUE == rs_p->rule_storage.is_enable))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }

                    if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                    {
                        RULE_CTRL_LOG("Dev rule ID in OM does not match");
                        res = 1;
                    }
                }
            }

        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 29;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,2},
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Destory rule failed\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 2;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* one rule with port bitmap {1,2} {1,3}
     */
    {
        bcm_pbmp_t          port_pbmp;
        RULE_CTRL_USERPORT  up[] =
        {
            {1,3},
        };

        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(TRUE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Create rule failed\n");
            res = 1;
        }

        port_pbmp = RULE_CTRL_HELP_MakePortBitmap(up, _countof(up));

        if (0 != memcmp(&re.ace_entry.inports_data, &port_pbmp, sizeof(port_pbmp)))
        {
            RULE_CTRL_LOG("Port bitmap does not match\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(TRUE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(re.dev_rule_id == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    unit = 1;
    port = 3;
    if (!(TRUE == _TESTTEE(FALSE, unit, port)))
    {
        RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
        res = 1;
    }

    /* no any rule
     */
    {
        RULE_CTRL_RuleEntry_T re;
        re.dev_rule_id = 0xFFFFFFFF;

        if (!(FALSE == RULE_CTRL_HELP_GetNextRuleWithFunType(fun_type,  &re)))
        {
            RULE_CTRL_LOG("Destory rule failed\n");
            res = 1;
        }

        /* Check OM
         */
        {
            RULE_CTRL_SPRRuleStorage_T *rs_p;
            RULE_CTRL_PARAM             param   = {0};

            param.unit  = 1;
            param.port  = 1;
            rs_p = _GET_OM(&param);

            if (!(rs_p != NULL))
            {
                RULE_CTRL_LOG("Can not get OM");
                res = 1;
            }

            if (rs_p)
            {
                bcm_pbmp_t  port_pbmp;

                memset(&port_pbmp, 0, sizeof(port_pbmp));
                if (!(0 == memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap in OM does not match");
                    res = 1;
                }

                if (!(FALSE == rs_p->rule_storage.is_enable))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }

                if (!(0 == rs_p->rule_storage.rule_id))
                {
                    RULE_CTRL_LOG("Dev rule ID in OM does not match");
                    res = 1;
                }
            }
        }
    }

    /****************************************************************/
    /****************************************************************/
    /* bind all port, and then unbind it
     */
    {
        RULE_CTRL_SPRRuleStorage_T *rs_p;
        RULE_CTRL_PARAM             param   = {0};
        RULE_CTRL_RuleEntry_T       re;

        EACH_UNIT(unit)
        {
            EACH_PORT(port)
            {
                if (!(TRUE == _TESTTEE(TRUE, unit+1, port+1)))
                {
                    RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
                    res = 1;
                }

                param.unit = unit+1;
                param.port = port+1;

                rs_p = _GET_OM(&param);

                RULE_CTRL_HELP_GetRuleByDevRuleID(rs_p->rule_storage.rule_id, &re);
                if (!(0 == memcmp(&rs_p->pbmp, &re.ace_entry.inports_data, sizeof(rs_p->pbmp))))
                {
                    RULE_CTRL_LOG("Port bitmap does not match");
                    res = 1;
                }
            }
        }

        EACH_UNIT(unit)
        {
            EACH_PORT(port)
            {
                if (!(TRUE == _TESTTEE(FALSE, unit+1, port+1)))
                {
                    RULE_CTRL_LOG("%s failed\n", _TESTTEE_STR);
                    res = 1;
                }

                param.unit = unit+1;
                param.port = port+1;

                rs_p = _GET_OM(&param);

                {
                    bcm_pbmp_t  port_pbmp;

                    memset(&port_pbmp, 0, sizeof(port_pbmp));

                    if (0 != memcmp(&rs_p->pbmp, &port_pbmp, sizeof(port_pbmp)))
                    {
                        if (!(0 != rs_p->rule_storage.is_enable))
                        {
                            RULE_CTRL_LOG("Rule storage is inconsistent\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }

                        RULE_CTRL_HELP_GetRuleByDevRuleID(rs_p->rule_storage.rule_id, &re);
                        if (!(0 == memcmp(&rs_p->pbmp, &re.ace_entry.inports_data, sizeof(rs_p->pbmp))))
                        {
                            RULE_CTRL_LOG("Port bitmap does not match\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }
                    }
                    else
                    {
                        if (!(0 == rs_p->rule_storage.is_enable))
                        {
                            RULE_CTRL_LOG("Rule storage is inconsistent\n"
                                          "  unit = %lu, port = %lu", unit+1, port+1);
                            res = 1;
                        }
                    }
                }
            }
        }
    }

    return res;

#undef _TESTTEE
#undef _TESTTEE_STR
#undef _GET_OM
#undef _CFG_RULE
}

int RULE_CTRL_Test_L4CosMap()
{
    UI32_T  unit;
    UI32_T  port;
    UI16_T  dst_port;
    UI8_T   cos;
    int     res = 0;

    unit = 1;
    port = 1;
    dst_port = 1;
    cos  = 1;
    if (FALSE == RULE_CTRL_SetL4PortCosMap(unit, port, dst_port, cos))
    {
        printf("RULE_CTRL_SetL4PortCosMap failed\n");
        res = 1;
    }

    unit = 1;
    port = 1;
    dst_port = 1;
    cos  = 5;
    if (FALSE == RULE_CTRL_SetL4PortCosMap(unit, port, dst_port, cos))
    {
        printf("RULE_CTRL_SetL4PortCosMap failed\n");
        res = 1;
    }

    unit = 1;
    port = 1;
    dst_port = 17;
    cos  = 7;
    if (FALSE == RULE_CTRL_DelL4PortCosMap(unit, port, dst_port, cos))
    {
        printf("RULE_CTRL_SetL4PortCosMap failed\n");
        res = 1;
    }

    unit = 1;
    port = 1;
    dst_port = 17;
    cos  = 0;
    if (FALSE == RULE_CTRL_SetL4PortCosMap(unit, port, dst_port, cos))
    {
        printf("RULE_CTRL_SetL4PortCosMap failed\n");
        res = 1;
    }

    return res;
}

int RULE_CTRL_Test_TrapLinkLocalAddrToCPU()
{
    int res = 0;

    UI8_T link_local_addr[SYS_ADPT_IPV6_ADDR_LEN] = {0};
    UI32_T vlan_id;

    link_local_addr[0]  = 0xfe;
    link_local_addr[1]  = 0x80;
    link_local_addr[15] = 0x01;
    vlan_id = 1001;

    if (FALSE == RULE_CTRL_IP6_TrapLinkLocalToCPU(TRUE, link_local_addr, vlan_id))
    {
        printf("RULE_CTRL_IP6_TrapLinkLocalToCPU failed\n");
        res = 1;
    }

    link_local_addr[0]  = 0xfe;
    link_local_addr[1]  = 0x80;
    link_local_addr[15] = 0x02;
    vlan_id = 1002;

    if (FALSE == RULE_CTRL_IP6_TrapLinkLocalToCPU(TRUE, link_local_addr, vlan_id))
    {
        printf("RULE_CTRL_IP6_TrapLinkLocalToCPU failed\n");
        res = 1;
    }

    link_local_addr[0]  = 0xfe;
    link_local_addr[1]  = 0x80;
    link_local_addr[15] = 0x03;
    vlan_id = 1003;

    if (FALSE == RULE_CTRL_IP6_TrapLinkLocalToCPU(TRUE, link_local_addr, vlan_id))
    {
        printf("RULE_CTRL_IP6_TrapLinkLocalToCPU failed\n");
        res = 1;
    }

    link_local_addr[0]  = 0xfe;
    link_local_addr[1]  = 0x80;
    link_local_addr[15] = 0x02;
    vlan_id = 1002;

    if (FALSE == RULE_CTRL_IP6_TrapLinkLocalToCPU(FALSE, link_local_addr, vlan_id))
    {
        printf("RULE_CTRL_IP6_TrapLinkLocalToCPU failed\n");
        res = 1;
    }

    link_local_addr[0]  = 0xfe;
    link_local_addr[1]  = 0x80;
    link_local_addr[15] = 0x04;
    vlan_id = 1004;

    if (FALSE == RULE_CTRL_IP6_TrapLinkLocalToCPU(TRUE, link_local_addr, vlan_id))
    {
        printf("RULE_CTRL_IP6_TrapLinkLocalToCPU failed\n");
        res = 1;
    }

    return res;
}

int RULE_CTRL_Test_LookupTable()
{
    int res = 0;
    RULE_CTRL_RuleLookupEntry_T entry;

    if (sizeof(entry.code) != sizeof(unsigned int))
    {
        printf("RuleLookupEntry.code is not equal 4");
        res = 1;
    }

    return res;
}

int RULE_CTRL_Test_HotInsertAndRemove_GlobalRule()
{
    int res = 0;
    UI8_T  sip[4] = {192, 168, 1, 1};
    UI8_T  sip6[16] = {0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

    RULE_CTRL_IP6_TrapLinkLocalToCPU(TRUE, sip6, 1);

    /* UNIT 2 loss
     */
    RULE_CTRL_HandleHotRemoval(SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT+1, SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT);
    RULE_CTRL_HandleHotInsertion(SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT+1, SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT);

    RULE_CTRL_IP6_TrapLinkLocalToCPU(TRUE, sip6, 1);

    return res;
}

int RULE_CTRL_Test_HotInsertAndRemove_PortRule()
{
    int res = 0;
    UI8_T  sip[4] = {192, 168, 1, 1};
    UI8_T  sip6[16] = {0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

    RULE_CTRL_IPSG_DenyAnyIpPacket(TRUE, 1, 1);
    RULE_CTRL_IPSG_DenyAnyIpPacket(TRUE, 1, 30);
    RULE_CTRL_IPSG_DenyAnyIpPacket(TRUE, 2, 1);
    RULE_CTRL_IPSG_PermitIpPacket(TRUE, 2, 1, 1, sip, NULL);
    RULE_CTRL_IPSG_PermitIpPacket(TRUE, 2, 1, 2, sip, NULL);
    RULE_CTRL_IPSG_PermitIpPacket(TRUE, 2, 1, 3, sip, NULL);
    RULE_CTRL_IPSG_PermitIpPacket(TRUE, 2, 1, 4, sip, NULL);

    /* UNIT 2 loss
     */
    RULE_CTRL_HandleHotRemoval(SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT+1, SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT);
    RULE_CTRL_HandleHotInsertion(SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT+1, SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT);

    RULE_CTRL_IPSG_DenyAnyIpPacket(TRUE, 2, 1);
    RULE_CTRL_IPSG_PermitIpPacket(TRUE, 2, 1, 1, sip, NULL);

    return res;
}

int RULE_CTRL_Test_HotInsertAndRemove_Dot1xUCRule()
{
    int res = 0;
    UI32_T count;
    RULE_CTRL_RuleEntry_T   re;
    UI8_T  cpu_mac[SYS_ADPT_MAC_ADDR_LEN] = {0, 0, 0, 0, 0x0c, 0x11};

    RULE_CTRL_1X_TrapDot1xUCToCPU(TRUE, 1,  1, cpu_mac);
    RULE_CTRL_1X_TrapDot1xUCToCPU(TRUE, 1, 30, cpu_mac);
    RULE_CTRL_1X_TrapDot1xUCToCPU(TRUE, 2,  1, cpu_mac);
    RULE_CTRL_1X_TrapDot1xUCToCPU(TRUE, 2, 30, cpu_mac);
    RULE_CTRL_1X_TrapDot1xUCToCPU(TRUE, 3,  1, cpu_mac);
    RULE_CTRL_1X_TrapDot1xUCToCPU(TRUE, 3, 30, cpu_mac);

    count = 0;
    re.dev_rule_id = 0xFFFFFFFF;
    while (TRUE == RULE_CTRL_HELP_GetNextRule(&re))
    {
        count ++;
    }

    if (!(6 == count))
    {
        printf("Set dot1x rule failed\n");
    }

    //RULE_CTRL_HELP_DumpRule();

    /* UNIT 2 loss
     */
    RULE_CTRL_HandleHotRemoval(SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT+1, SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT);
    RULE_CTRL_HandleHotInsertion(SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT+1, SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT);

    count = 0;
    re.dev_rule_id = 0xFFFFFFFF;
    while (TRUE == RULE_CTRL_HELP_GetNextRule(&re))
    {
        count ++;
    }

    if (!(4 == count))
    {
        printf("Set dot1x rule failed\n");
    }

    //RULE_CTRL_HELP_DumpRule();

    RULE_CTRL_1X_TrapDot1xUCToCPU(TRUE, 2,  1, cpu_mac);
    RULE_CTRL_1X_TrapDot1xUCToCPU(TRUE, 2,  30, cpu_mac);

    count = 0;
    re.dev_rule_id = 0xFFFFFFFF;
    while (TRUE == RULE_CTRL_HELP_GetNextRule(&re))
    {
        count ++;
    }

    if (!(6 == count))
    {
        printf("Set dot1x rule failed\n");
    }

    //RULE_CTRL_HELP_DumpRule();

    return res;
}

int RULE_CTRL_Crash_Reproduce_1()
{
    int res = 0;

    UI8_T sip[4] = {0};

    /*
    RULE_CTRL_WA_TrapHttpToCPU(FALSE, 2, 1);
    RULE_CTRL_WA_PermitDhcp(FALSE, 2, 1);
    RULE_CTRL_WA_PermitDns(FALSE, 2, 1);
    RULE_CTRL_WA_DenyAnyIpPacket(FALSE, 2, 1);

    RULE_CTRL_WA_TrapHttpToCPU(TRUE, 2, 1);
    RULE_CTRL_WA_PermitDhcp(TRUE, 2, 1);
    RULE_CTRL_WA_PermitDns(TRUE, 2, 1);
    RULE_CTRL_WA_DenyAnyIpPacket(TRUE, 2, 1);

    RULE_CTRL_HandleHotRemoval(57, 26);
    RULE_CTRL_HandleHotInsertion(57, 26);

    RULE_CTRL_WA_TrapHttpToCPU(TRUE, 2, 1);
    RULE_CTRL_WA_PermitDhcp(TRUE, 2, 1);
    RULE_CTRL_WA_PermitDns(TRUE, 2, 1);
    RULE_CTRL_WA_DenyAnyIpPacket(TRUE, 2, 1);

    RULE_CTRL_WA_TrapHttpToCPU(FALSE, 2, 1);
    RULE_CTRL_WA_PermitDhcp(FALSE, 2, 1);
    RULE_CTRL_WA_PermitDns(FALSE, 2, 1);
    RULE_CTRL_WA_DenyAnyIpPacket(FALSE, 2, 1);
    */

    RULE_CTRL_WA_PermitIpPacket(TRUE, 2, 1, sip);

    RULE_CTRL_WA_TrapHttpToCPU(TRUE, 2, 1);
    RULE_CTRL_WA_PermitDhcp(TRUE, 2, 1);
    RULE_CTRL_WA_PermitDns(TRUE, 2, 1);
    RULE_CTRL_WA_DenyAnyIpPacket(TRUE, 2, 1);

    RULE_CTRL_HandleHotRemoval(57, 26);

    return res;
}

BOOL_T set_eaps_rule()
{
    RULE_TYPE_CpuRuleInfo_T ri;

    ri.eaps.vid = 99;
    ri.eaps.mode = SYS_ADPT_EAPS_MASTER_MODE;

    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_EAPS, &ri);
}

BOOL_T unset_eaps_rule()
{
    RULE_TYPE_CpuRuleInfo_T ri;

    ri.eaps.vid = 99;
    ri.eaps.mode = SYS_ADPT_EAPS_MASTER_MODE;

    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_EAPS, &ri);
}

BOOL_T set_trap_arp_req_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_ARP_REQUEST, NULL);
}

BOOL_T unset_trap_arp_req_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_ARP_REQUEST, NULL);
}

BOOL_T set_redir_arp_req_rule()
{
    RULE_TYPE_CpuRuleInfo_T ri;

    ri.arp.all_arp_to_cpu = TRUE;

    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_ARP_REQUEST, &ri);
}

BOOL_T unset_redir_arp_req_rule()
{
    RULE_TYPE_CpuRuleInfo_T ri;

    ri.arp.all_arp_to_cpu = FALSE;

    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_ARP_REQUEST, &ri);
}

BOOL_T set_trap_arp_reply_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_ARP_REPLY, NULL);
}

BOOL_T unset_trap_arp_reply_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_ARP_REPLY, NULL);
}

BOOL_T set_redir_arp_reply_rule()
{
    RULE_TYPE_CpuRuleInfo_T ri;

    ri.arp.all_arp_to_cpu = TRUE;

    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_ARP_REPLY, &ri);
}

BOOL_T unset_redir_arp_reply_rule()
{
    RULE_TYPE_CpuRuleInfo_T ri;

    ri.arp.all_arp_to_cpu = FALSE;

    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_ARP_REPLY, &ri);
}

BOOL_T set_rip_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_RIP, NULL);
}

BOOL_T unset_rip_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_RIP, NULL);
}

BOOL_T set_ospf_mac5_rule()
{
    return RULE_CTRL_OSPF_TrapOspfMac5ToCPU(TRUE);
}

BOOL_T unset_ospf_mac5_rule()
{
    return RULE_CTRL_OSPF_TrapOspfMac5ToCPU(FALSE);
}

BOOL_T set_ospf_mac6_rule()
{
    return RULE_CTRL_OSPF_TrapOspfMac6ToCPU(TRUE);
}

BOOL_T unset_ospf_mac6_rule()
{
    return RULE_CTRL_OSPF_TrapOspfMac6ToCPU(FALSE);
}

#if (SYS_CPNT_OSPF6 == TRUE)
BOOL_T set_ospf6_mac5_rule()
{
    return RULE_CTRL_OSPF6_TrapOspfMac5ToCPU(TRUE);
}

BOOL_T unset_ospf6_mac5_rule()
{
    return RULE_CTRL_OSPF6_TrapOspfMac5ToCPU(FALSE);
}

BOOL_T set_ospf6_mac6_rule()
{
    return RULE_CTRL_OSPF6_TrapOspfMac6ToCPU(TRUE);
}

BOOL_T unset_ospf6_mac6_rule()
{
    return RULE_CTRL_OSPF6_TrapOspfMac6ToCPU(FALSE);
}
#endif /* SYS_CPNT_OSPF6 */

BOOL_T set_igmp_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_IGMP, NULL);
}

BOOL_T unset_igmp_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_IGMP, NULL);
}

BOOL_T set_l2_slf_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_L2_SLF, NULL);
}

BOOL_T unset_l2_slf_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_L2_SLF, NULL);
}

BOOL_T set_slf_rule()
{
    RULE_TYPE_CpuRuleInfo_T ri;

    ri.slf.action  = SLF_REDIRECT;
    ri.slf.slf_vid = 99;

    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_SLF, &ri);
}

BOOL_T unset_slf_rule()
{
    RULE_TYPE_CpuRuleInfo_T ri;

    ri.slf.action  = SLF_REDIRECT;
    ri.slf.slf_vid = 99;

    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_SLF, &ri);
}

BOOL_T set_unknown_ip_mc_rule()
{
    RULE_TYPE_CpuRuleInfo_T ri;

    ri.unknown_ipmc.to_cpu = TRUE;
    ri.unknown_ipmc.flood  = TRUE;

    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_UNKNOWN_IPMC, &ri);
}

BOOL_T unset_unknown_ip_mc_rule()
{
    RULE_TYPE_CpuRuleInfo_T ri;

    ri.unknown_ipmc.to_cpu = TRUE;
    ri.unknown_ipmc.flood  = TRUE;

    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_UNKNOWN_IPMC, &ri);
}

BOOL_T set_ip_bc_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_IP_BCAST, NULL);
}

BOOL_T unset_ip_bc_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_IP_BCAST, NULL);
}

BOOL_T set_pim_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_PIM, NULL);
}

BOOL_T unset_pim_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_PIM, NULL);
}

BOOL_T set_ip_vrrp_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_VRRP, NULL);
}

BOOL_T unset_ip_vrrp_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_VRRP, NULL);
}

BOOL_T set_ip_all_host_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_ALL_HOST, NULL);
}

BOOL_T unset_ip_all_host_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_ALL_HOST, NULL);
}

BOOL_T set_ip_all_routers_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_ALL_ROUTER, NULL);
}

BOOL_T unset_ip_all_routers_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_ALL_ROUTER, NULL);
}

BOOL_T set_ip_option_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_IP_OPTION, NULL);
}

BOOL_T unset_ip_option_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_IP_OPTION, NULL);
}

BOOL_T set_dhcp_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_DHCP, NULL);
}

BOOL_T unset_dhcp_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_DHCP, NULL);
}

#if (SYS_CPNT_DHCPSNP == TRUE)
BOOL_T set_dhcp_snp_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_DHCP_SNOOPING, NULL);
}

BOOL_T unset_dhcp_snp_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_DHCP_SNOOPING, NULL);
}
#endif /* SYS_CPNT_DHCPSNP */

BOOL_T set_hbt_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_HBT, NULL);
}

BOOL_T unset_hbt_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_HBT, NULL);
}

BOOL_T set_bpdu_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_BPDU, NULL);
}

BOOL_T unset_bpdu_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_BPDU, NULL);
}

BOOL_T set_l2cp_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_L2CP, NULL);
}

BOOL_T unset_l2cp_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_L2CP, NULL);
}

BOOL_T set_cluster_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_CLUSTER, NULL);
}

BOOL_T unset_cluster_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_CLUSTER, NULL);
}

BOOL_T set_cfm_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_CFM, NULL);
}

BOOL_T unset_cfm_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_CFM, NULL);
}

BOOL_T set_cfm_ccm_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_CFM_CCM, NULL);
}

BOOL_T unset_cfm_ccm_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_CFM_CCM, NULL);
}

BOOL_T set_ip6_all_nodes_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_IPV6_ALL_NODES, NULL);
}

BOOL_T unset_ip6_all_nodes_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_IPV6_ALL_NODES, NULL);
}

BOOL_T set_ip6_all_routers_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_IPV6_ALL_ROUTERS, NULL);
}

BOOL_T unset_ip6_all_routers_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_IPV6_ALL_ROUTERS, NULL);
}

BOOL_T set_mld_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_MLD, NULL);
}

BOOL_T unset_mld_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_MLD, NULL);
}

BOOL_T set_pim6_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_PIM6, NULL);
}

BOOL_T unset_pim6_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_PIM6, NULL);
}

BOOL_T set_unknown_ip6_mc_rule()
{
    RULE_TYPE_CpuRuleInfo_T ri;

    ri.unknown_ipmc.to_cpu = TRUE;
    ri.unknown_ipmc.flood  = TRUE;

    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_UNKNOWN_IPV6MC, &ri);
}

BOOL_T unset_unknown_ip6_mc_rule()
{
    RULE_TYPE_CpuRuleInfo_T ri;

    ri.unknown_ipmc.to_cpu = TRUE;
    ri.unknown_ipmc.flood  = TRUE;

    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_UNKNOWN_IPV6MC, &ri);
}

BOOL_T set_dhcp6_relay_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_DHCPC6, NULL);
}

BOOL_T unset_dhcp6_relay_rule()
{
    return RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_DHCPC6, NULL);
}

int RULE_CTRL_Test_CPU_Rule()
{
#define _ADD_RULE_ENTRY(set, unset)     \
    {                                   \
        set,                            \
        #set,                           \
        unset,                          \
        #unset                          \
    }
#define _MAX_FUNCTION_NAME_STR          255

    struct Test_Rule_Entry
    {
        BOOL_T (*set)();
        char   set_name[_MAX_FUNCTION_NAME_STR + 1];
        BOOL_T (*unset)();
        char   unset_name[_MAX_FUNCTION_NAME_STR + 1];
    } rule_entries[] =
    {
        _ADD_RULE_ENTRY(set_eaps_rule,
                        unset_eaps_rule),

        _ADD_RULE_ENTRY(set_trap_arp_req_rule,
                        unset_trap_arp_req_rule),
        _ADD_RULE_ENTRY(set_redir_arp_req_rule,
                        unset_redir_arp_req_rule),

        _ADD_RULE_ENTRY(set_trap_arp_reply_rule,
                        unset_trap_arp_reply_rule),
        _ADD_RULE_ENTRY(set_redir_arp_reply_rule,
                        unset_redir_arp_reply_rule),

        _ADD_RULE_ENTRY(set_rip_rule,
                        unset_rip_rule),

        _ADD_RULE_ENTRY(set_ospf_mac5_rule,
                        unset_ospf_mac5_rule),
        _ADD_RULE_ENTRY(set_ospf_mac6_rule,
                        unset_ospf_mac6_rule),
#if (SYS_CPNT_OSPF6 == TRUE)
        _ADD_RULE_ENTRY(set_ospf6_mac5_rule,
                        unset_ospf6_mac5_rule),
        _ADD_RULE_ENTRY(set_ospf6_mac6_rule,
                        unset_ospf6_mac6_rule),
#endif /* SYS_CPNT_OSPF6 */

        _ADD_RULE_ENTRY(set_igmp_rule,
                        unset_igmp_rule),

        _ADD_RULE_ENTRY(set_l2_slf_rule,
                        unset_l2_slf_rule),
        _ADD_RULE_ENTRY(set_slf_rule,
                        unset_slf_rule),

        _ADD_RULE_ENTRY(set_unknown_ip_mc_rule,
                        unset_unknown_ip_mc_rule),
        _ADD_RULE_ENTRY(set_ip_bc_rule,
                        unset_ip_bc_rule),
        _ADD_RULE_ENTRY(set_pim_rule,
                        unset_pim_rule),
        _ADD_RULE_ENTRY(set_ip_vrrp_rule,
                        unset_ip_vrrp_rule),
        _ADD_RULE_ENTRY(set_ip_all_host_rule,
                        unset_ip_all_host_rule),
        _ADD_RULE_ENTRY(set_ip_all_routers_rule,
                        unset_ip_all_routers_rule),
        _ADD_RULE_ENTRY(set_ip_option_rule,
                        unset_ip_option_rule),

        _ADD_RULE_ENTRY(set_dhcp_rule,
                        unset_dhcp_rule),

#if(SYS_CPNT_DHCPSNP == TRUE)
        _ADD_RULE_ENTRY(set_dhcp_snp_rule,
                        unset_dhcp_snp_rule),
#endif /* SYS_CPNT_DHCPSNP */

        _ADD_RULE_ENTRY(set_hbt_rule,
                        unset_hbt_rule),

        _ADD_RULE_ENTRY(set_bpdu_rule,
                        unset_bpdu_rule),
        _ADD_RULE_ENTRY(set_l2cp_rule,
                        unset_l2cp_rule),
        _ADD_RULE_ENTRY(set_cluster_rule,
                        unset_cluster_rule),
        _ADD_RULE_ENTRY(set_cfm_rule,
                        unset_cfm_rule),
        _ADD_RULE_ENTRY(set_cfm_ccm_rule,
                        unset_cfm_ccm_rule),
        _ADD_RULE_ENTRY(set_ip6_all_nodes_rule,
                        unset_ip6_all_nodes_rule),
        _ADD_RULE_ENTRY(set_ip6_all_routers_rule,
                        unset_ip6_all_routers_rule),
        _ADD_RULE_ENTRY(set_mld_rule,
                        unset_mld_rule),
        _ADD_RULE_ENTRY(set_pim6_rule,
                        unset_pim6_rule),
        _ADD_RULE_ENTRY(set_unknown_ip6_mc_rule,
                        unset_unknown_ip6_mc_rule),
        _ADD_RULE_ENTRY(set_dhcp6_relay_rule,
                        unset_dhcp6_relay_rule),
    };

    int res = 0;
    int i;
    UI32_T count;

    for (i=0; i<_countof(rule_entries); i++)
    {
        struct Test_Rule_Entry *tr = &rule_entries[i];

        if (FALSE == tr->set())
        {
            RULE_CTRL_LOG("%s failed", tr->set_name);
            return 1;
        }

        count = RULE_CTRL_HELP_CountOfRule();

        if (!(SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK*SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT == count))
        {
            RULE_CTRL_LOG("%s failed", tr->set_name);
            return 1;
        }

        if (FALSE == tr->unset())
        {
            RULE_CTRL_LOG("%s failed", tr->unset_name);
            return 1;
        }

        count = RULE_CTRL_HELP_CountOfRule();

        if (!(0 == count))
        {
            RULE_CTRL_LOG("%s failed", tr->unset_name);
            return 1;
        }
    }

    /* hot removal test
     * 1. set all rule
     * 2. remove unit 2
     * 3. reset all rule
     */
    for (i=0; i<_countof(rule_entries); i++)
    {
        struct Test_Rule_Entry *tr = &rule_entries[i];

        tr->set();
    }

    count = RULE_CTRL_HELP_CountOfRule();

    /* remove unit 2
     */
    RULE_CTRL_HandleHotRemoval(SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT+1, SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT);

    if (!(count == RULE_CTRL_HELP_CountOfRule() + (SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT*_countof(rule_entries)) ))
    {
        RULE_CTRL_LOG("Hot removal test failed");
        return 1;
    }

    for (i=0; i<_countof(rule_entries); i++)
    {
        struct Test_Rule_Entry *tr = &rule_entries[i];

        tr->set();
    }

    // RULE_CTRL_HELP_DumpRule();

    if (!(count == RULE_CTRL_HELP_CountOfRule()))
    {
        RULE_CTRL_LOG("Hot removal test failed");
        return 1;
    }

    return 0;
}

int RULE_CTRL_TEST_Slf_Rule()
{
    UI32_T i;

    for (i=0; i < _countof(shmem_data_p->slf); i++)
    {
        RULE_TYPE_CpuRuleInfo_T ri;

        ri.slf.action  = SLF_NO_ACTION;
        ri.slf.slf_vid = i+1;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_SLF, &ri))
        {
            RULE_CTRL_LOG("slf.vid=%hu slf.action=%u", ri.slf.slf_vid, ri.slf.action);
            return 1;
        }

        ri.slf.action  = SLF_REDIRECT;
        ri.slf.slf_vid = i+1;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_SLF, &ri))
        {
            RULE_CTRL_LOG("slf.vid=%hu slf.action=%u", ri.slf.slf_vid, ri.slf.action);
            return 1;
        }

        ri.slf.action  = SLF_DROP;
        ri.slf.slf_vid = i+1;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_SLF, &ri))
        {
            RULE_CTRL_LOG("slf.vid=%hu slf.action=%u", ri.slf.slf_vid, ri.slf.action);
            return 1;
        }
    }

    /* remove all
     */
    for (i=0; i < _countof(shmem_data_p->slf); i++)
    {
        RULE_TYPE_CpuRuleInfo_T ri;

        ri.slf.action  = SLF_NO_ACTION;
        ri.slf.slf_vid = i+1;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(FALSE, RULE_TYPE_PacketType_SLF, &ri))
        {
            RULE_CTRL_LOG("slf.vid=%hu slf.action=%u", ri.slf.slf_vid, ri.slf.action);
            return 1;
        }
    }

    /* try again
     */
    for (i=0; i < _countof(shmem_data_p->slf); i++)
    {
        RULE_TYPE_CpuRuleInfo_T ri;

        ri.slf.action  = SLF_NO_ACTION;
        ri.slf.slf_vid = i+1;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_SLF, &ri))
        {
            RULE_CTRL_LOG("slf.vid=%hu slf.action=%u", ri.slf.slf_vid, ri.slf.action);
            return 1;
        }

        ri.slf.action  = SLF_REDIRECT;
        ri.slf.slf_vid = i+1;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_SLF, &ri))
        {
            RULE_CTRL_LOG("slf.vid=%hu slf.action=%u", ri.slf.slf_vid, ri.slf.action);
            return 1;
        }

        ri.slf.action  = SLF_DROP;
        ri.slf.slf_vid = i+1;

        if (FALSE == RULE_CTRL_TrapPacket2Cpu(TRUE, RULE_TYPE_PacketType_SLF, &ri))
        {
            RULE_CTRL_LOG("slf.vid=%hu slf.action=%u", ri.slf.slf_vid, ri.slf.action);
            return 1;
        }
    }

    return 0;
}

static BOOL_T breakpoint_enable = 0;

int RULE_CTRL_TEST_Rule_Priority()
{
    UI32_T i;

breakpoint_enable = 1;

    for (i=0; i<500; ++i)
    {
        if (FALSE == set_igmp_rule())
        {
            RULE_CTRL_LOG("set_igmp_rule failed. i=%lu",i);
            return 1;
        }

        if (FALSE == unset_igmp_rule())
        {
            RULE_CTRL_LOG("unset_igmp_rule failed. i=%lu",i);
            return 1;
        }
    }

breakpoint_enable = 0;

    return 0;
}

int RULE_CTRL_UnitTest()
{
#ifdef MARVELL
    return 0;
#endif
    RULE_CTRL_InitiateSystemResources();

    TEST_FUNCTION_Init();

    TEST_FUNCTION_Register(
        NULL,
        RULE_CTRL_TestHelpFunctions,
        NULL,
        "Test for Foundation Functions"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_TestFindRuleLookupTable,
        RULE_CTRL_Setdown,
        "Test for FindRuleLookupTable Function"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_TestGetNextRuleEntryInLookupTable,
        RULE_CTRL_Setdown,
        "Test for GetNextRuleEntryInLookupTable Function"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_LookupTable,
        RULE_CTRL_Setdown,
        "Test for Lookup table"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_TestTrapHTTPCToCPUForWebAuthRule,
        RULE_CTRL_Setdown,
        "Test for Trap HTTP client to CPU for WebAuth Rule"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_PermitDHCPCForWebAuth,
        RULE_CTRL_Setdown,
        "Test for Permit DHCP client for WebAuth Rule"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_PermitDNSForWebAuth,
        RULE_CTRL_Setdown,
        "Test for Permit DNS client for WebAuth Rule"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_DenyAnyIPPacketForWebAuth,
        RULE_CTRL_Setdown,
        "Test for Deny Any IP Packetfor WebAuth Rule"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_PermitIPPacketForWebAuth,
        RULE_CTRL_Setdown,
        "Test for Permit IP Packetfor WebAuth Rule"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_TestTrapDot1xUCToCPURule,
        RULE_CTRL_Setdown,
        "Test for Trap Dot1X Unicast to CPU Rule"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_TestIPSGOM,
        RULE_CTRL_Setdown,
        "Test for IPSG OM"
        );

	TEST_FUNCTION_Register(
		RULE_CTRL_Setup,
		RULE_CTRL_Test_PermitPacketForIPSG,
		RULE_CTRL_Setdown,
		"Test for Permit IP Packt For IPSG Rule"
		);

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_DenyAnyIPPacketForIPSGRule,
        RULE_CTRL_Setdown,
        "Test for Deny Any IP Packet For IPSG Rule"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_TrapRAPSToCPU,
        RULE_CTRL_Setdown,
        "Test for RAPS Rule"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_L4CosMap,
        RULE_CTRL_Setdown,
        "Test for L4 CoS Map"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_TrapLinkLocalAddrToCPU,
        RULE_CTRL_Setdown,
        "Test for Link Local Address To CPU"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_HotInsertAndRemove_GlobalRule,
        RULE_CTRL_Setdown,
        "Test for Hot Insert and Remove on Global Rule"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_HotInsertAndRemove_PortRule,
        RULE_CTRL_Setdown,
        "Test for Hot Insert and Remove on Port Rule"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_HotInsertAndRemove_Dot1xUCRule,
        RULE_CTRL_Setdown,
        "Test for Hot Insert and Remvoe for Dot1x UC Rule"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Crash_Reproduce_1,
        RULE_CTRL_Setdown,
        "Crash reproduce 1"
        );

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_Test_CPU_Rule,
        RULE_CTRL_Setdown,
        "Test for CPU rule");

    if (0)
    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_TEST_Slf_Rule,
        RULE_CTRL_Setdown,
        "Test for Slf rule");

    TEST_FUNCTION_Register(
        RULE_CTRL_Setup,
        RULE_CTRL_TEST_Rule_Priority,
        RULE_CTRL_Setdown,
        "Test for rule priority"); /* Each rule in the same group will have different priority */

    TEST_FUNCTION_Run();

    return 0;
}
