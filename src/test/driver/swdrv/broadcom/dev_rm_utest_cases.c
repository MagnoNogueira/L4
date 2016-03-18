/*
 *  dev_rm_utest_cases.c
 *  rule_ctrl
 *
 *  Created by yehjunying on 2011/4/30.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "utest.h"

/* -----------------------------------------------------------------------------
 * Help Functions
 * -----------------------------------------------------------------------------
 */

void
DEVRM_UT_Proc_Validate_Virtual_Group()
{
    int dev;

    for (dev=0; dev < soc_ndev; ++dev)
    {
        DEVRM_VirtualGroupVector_T *vg_vec = &devrm_vir_grp[dev];
        DEVRM_VirtualGroup_T *vg_1 = NULL;
        DEVRM_VirtualGroup_T *vg_2 = NULL;
        UI32_T i;

        for (i=0; i < _countof(vg_vec->vg_tbl); ++i)
        {
            if (FALSE == vg_vec->vg_tbl[i].valid)
            {
                continue;
            }

            if (NULL == vg_1)
            {
                vg_1 = &vg_vec->vg_tbl[i];
            }
            else if (NULL == vg_2)
            {
                vg_2 = &vg_vec->vg_tbl[i];
            }
            else
            {
                assert(0);
            }

            if (NULL != vg_1 && NULL != vg_2)
            {
                if (vg_1->group_id == vg_2->group_id)
                {
                    assert(vg_2->pri_lvl <= vg_1->pri_lvl);
                    assert(vg_2->pri_base < vg_1->pri_base);

                    if (vg_1->pri_lvl == vg_2->pri_lvl)
                    {
                        assert(vg_1->seq_no < vg_2->seq_no);
                    }
                }
                else
                {
                    assert(vg_1->group_id < vg_2->group_id);
                }

                vg_1 = vg_2;
                vg_2 = NULL;
            }
        }
    } // for (dev
}

void
DEVRM_UT_Proc_Validate_Rule()
{
    int dev;

    for (dev=0; dev < soc_ndev; ++dev)
    {
        UI32_T vg_idx;

        DEVRM_Rule_T *rule_1 = NULL;
        DEVRM_Rule_T *rule_2 = NULL;
        UI32_T vir_group_id_1 = 0x7fffffff;
        UI32_T vir_group_id_2 = 0x7fffffff;

        UI32_T group_id = 0x7fffffff;

        for (vg_idx=0; vg_idx < _countof(devrm_vir_grp[dev].vg_tbl); ++vg_idx)
        {
            DEVRM_VirtualGroup_T *vg = _DEVRM_GetVirtualGroupAtIndex(dev, vg_idx);
            UI32_T rule_idx;
            UI32_T rule_nbr = 0;

            if (0 == vg->valid)
            {
                continue;
            }

            //
            // The check will be per group
            //
            if (group_id != vg->group_id)
            {
                group_id = vg->group_id;

                rule_1 = NULL;
                rule_2 = NULL;
            }

            for (rule_idx = 0; rule_idx < _countof(vg->rv.rule_tbl); ++rule_idx)
            {
                if (0 == vg->rv.rule_tbl[rule_idx].valid)
                {
                    continue;
                }

                ++ rule_nbr;

                if (NULL == rule_1)
                {
                    rule_1 = &vg->rv.rule_tbl[rule_idx];
                    vir_group_id_1 = vg->vir_group_id;
                }
                else if (NULL == rule_2)
                {
                    rule_2 = &vg->rv.rule_tbl[rule_idx];
                    vir_group_id_2 = vg->vir_group_id;
                }
                else
                {
                    assert(0);
                }

                if (NULL != rule_1 && NULL != rule_2)
                {
                    int phy_pri_1 = 1;
                    int phy_pri_2 = 2;

                    //
                    // The rule_pri only be effect in the same virtual group
                    //
                    if (vir_group_id_1 == vir_group_id_2)
                    {
                        assert(rule_2->rule_pri <= rule_1->rule_pri);
                    }

                    //
                    // The phy_pri will be effect in the same group
                    //
                    bcm_field_entry_prio_get(dev, rule_1->rule_id, &phy_pri_1);
                    bcm_field_entry_prio_get(dev, rule_2->rule_id, &phy_pri_2);

                    assert(phy_pri_2 < phy_pri_1);

                    rule_1 = rule_2;
                    vir_group_id_1 = vir_group_id_2;

                    rule_2 = NULL;
                    vir_group_id_2 = 0x7fffffff;
                }
            }

            assert(vg->rv.cur_rule_nbr == rule_nbr);
        }
    } // for (dev
}

BOOL_T
DEVRM_UT_Proc_Get_Rule(
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_VirtualGroup_T *vg,
    DEVRM_Rule_T *rule,
    int *hw_prio)
{
    uint32 dev;

    assert(NULL != rule);
    assert(NULL != vg);
    assert(NULL != hw_prio);

//    for (dev=0; dev < _countof(devrm_vir_grp); ++dev)
    dev = device_id;
    {
        uint32 vg_idx;

        for (vg_idx = 0; vg_idx < devrm_vir_grp[dev].vsel_num; ++vg_idx)
        {
            DEVRM_VirtualGroup_T *vg_p = &devrm_vir_grp[dev].vg_tbl[vg_idx];

            uint32 rule_idx;

            if (0 == vg_p->rv.cur_rule_nbr)
            {
                continue;
            }

            for (rule_idx = 0; rule_idx < _countof(vg_p->rv.rule_tbl); ++rule_idx)
            {
                DEVRM_Rule_T *rule_p = &vg_p->rv.rule_tbl[rule_idx];

                if (rule_p->rule_id == dev_rule_id)
                {
                    memcpy(vg, vg_p, sizeof(*vg));
                    memcpy(rule, rule_p, sizeof(*rule));

                    bcm_field_entry_prio_get(device_id, dev_rule_id, hw_prio);

                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

void
DEVRM_UT_Proc_Validate_Leaks_Rule()
{
    uint32 dev;
    DEVRM_VirtualGroupVector_T *dup_vir_grp = malloc(sizeof(devrm_vir_grp));

    assert(NULL != dup_vir_grp);

    memcpy(dup_vir_grp, devrm_vir_grp, sizeof(devrm_vir_grp));

    for (dev=0; dev < _countof(g_field_db.device); ++dev)
    {
        uint32 grp_idx;

        for (grp_idx = 0; grp_idx < _countof(g_field_db.device[dev].group); ++grp_idx)
        {
            uint32 ent_idx;

            for (ent_idx = 0; ent_idx < _countof(g_field_db.device[dev].group[grp_idx].entry_tbl); ++ent_idx)
            {
                _field_entry_t *ent = &g_field_db.device[dev].group[grp_idx].entry_tbl[ent_idx];

                if (0 == ent->valid)
                {
                    continue;
                }

                //
                // find rule by ent->dev_rule_id, and clear valid bit
                //
                {
                    uint32 vg_idx;

                    for (vg_idx = 0; vg_idx < devrm_vir_grp[dev].vsel_num; ++vg_idx)
                    {
                        DEVRM_VirtualGroup_T *vg = &dup_vir_grp[dev].vg_tbl[vg_idx];

                        uint32 rule_idx;

                        for (rule_idx = 0; rule_idx < vg->rv.cur_rule_nbr; ++rule_idx)
                        {
                            DEVRM_Rule_T *rule = &vg->rv.rule_tbl[rule_idx];

                            if (rule->rule_id == ent->dev_rule_id)
                            {
                                rule->valid = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    for (dev = 0; dev < _countof(devrm_vir_grp); ++dev)
    {
        uint32 vg_idx;

        for (vg_idx = 0; vg_idx < devrm_vir_grp[dev].vsel_num; ++vg_idx)
        {
            DEVRM_VirtualGroup_T *vg = &dup_vir_grp[dev].vg_tbl[vg_idx];

            uint32 rule_idx;

            for (rule_idx = 0; rule_idx < vg->rv.cur_rule_nbr; ++rule_idx)
            {
                DEVRM_Rule_T *rule = &vg->rv.rule_tbl[rule_idx];

                assert(0 == rule->valid);
            }
        }
    }

    free(dup_vir_grp);
}

void
DEVRM_UT_Proc_Validate_Leaks()
{
    DEVRM_UT_Proc_Validate_Leaks_Rule();
}

void
DEVRM_UT_Proc_Validate()
{
    DEVRM_UT_Proc_Validate_Virtual_Group();
    DEVRM_UT_Proc_Validate_Rule();

    DEVRM_UT_Proc_Validate_Leaks();
}

/* -----------------------------------------------------------------------------
 * Test Functions
 * -----------------------------------------------------------------------------
 */

int
DEVRM_UT_Setup()
{
    DEVRM_Initial();

    return 0;
}

int
DEVRM_UT_Teardown()
{
    return 0;
}

int
DEVRM_UT_CreateVirtualGroup()
{
    typedef struct
    {
        UI32_T group_id;
        UI32_T vir_group_id;
        int vir_group_pri;
    } _payload_t;

    _payload_t payload[] =
    {
        {0,  1, -1},
        {0,  2, -2},
        {0,  3, -3},

        {1, 10, -13},
        {1, 11, -12},
        {1, 12, -11},
        {1, 13, -10},

        {2, 20, 1},
        {2, 21, 2},
        {2, 22, 3},
        {2, 23, 4},

    };

    int i;
    DEVRM_GroupEntry_T grp;
    BOOL_T result;

    //
    // create group
    //
    for (i=0; i < _countof(payload); ++i)
    {
        _payload_t *pl = &payload[i];

        if (TRUE == _DEVRM_IsGroupExisted(0, pl->group_id))
        {
            continue;
        }

        memset(&grp, 0, sizeof(grp));
        grp.group_id = pl->group_id;
        grp.selector_bitmap = 1;

        result = DEVRM_CreateGroup(0, &grp);
        assert(TRUE == result);

        result = DEVRM_CreateGroup(1, &grp);
        assert(TRUE == result);
    }

    //
    // create virtual group
    //
    for (i=0; i < _countof(payload); ++i)
    {
        _payload_t *pl = &payload[i];

        result = DEVRM_CreateVirtualGroup(pl->group_id,
                                          pl->vir_group_id,
                                          pl->vir_group_pri);
        assert(TRUE == result);
    }

    DEVRM_UT_Proc_Validate();

    return 0;
}

#include "fp_config.h"
int
DEVRM_UT_CreateVirtualGroup_By_OEM()
{
    FP_CONFIG_STRUCT_T  *fp_config_p;
    UI32_T i;
    UI32_T j;
    BOOL_T result;

    for (i=0; i < FP_CONFIG_GetConfigSize(); ++i)
    {
        fp_config_p = FP_CONFIG_GetConfig(i);
        if (NULL == fp_config_p)
        {
            continue;
        }

        if (FALSE == _DEVRM_IsGroupExisted(0, fp_config_p->group_id))
        {
            DEVRM_GroupEntry_T grp;

            memset(&grp, 0, sizeof(grp));

            grp.group_id = fp_config_p->group_id;
            grp.selector_bitmap = 1;

            result = DEVRM_CreateGroup(0, &grp);
            assert(TRUE == result);

            result = DEVRM_CreateGroup(1, &grp);
            assert(TRUE == result);
        }

        for (j=0; j <fp_config_p->func_vector.count; ++j)
        {
            result = DEVRM_PMGR_CreateVirtualGroup(fp_config_p->group_id,
                                       fp_config_p->func_vector.fd[j].func_type,
                                       fp_config_p->func_vector.fd[j].rule_pri);
            assert(TRUE == result);
        }
    }

    DEVRM_UT_Proc_Validate();

    return 0;
}

int
DEVRM_UT_AllocateRule()
{
    typedef struct
    {
        UI32_T  device_id;
        UI32_T  group_id;
        UI32_T  vir_group_id;
        int     rule_pri;
        UI32_T  dev_rule_id;    // out
    } _payload_t;

    _payload_t payload[] =
    {
        // device_id = 0, group_id = 0
        {0, 0,  1, -1, 999},
        {0, 0,  1, -1, 999},
        {0, 0,  1, -1, 999},

        {0, 0,  2, -2, 999},
        {0, 0,  2, -3, 999},
        {0, 0,  2, -4, 999},

        {0, 0,  3, -3, 999},
        {0, 0,  3, -2, 999},
        {0, 0,  3, -1, 999},

        // device_id = 0, group_id = 1
        {0, 1, 10, -1, 999},
        {0, 1, 10, -1, 999},
        {0, 1, 10, -1, 999},

        {0, 1, 11, -2, 999},
        {0, 1, 11, -3, 999},
        {0, 1, 11, -4, 999},

        {0, 1, 12, -3, 999},
        {0, 1, 12, -2, 999},
        {0, 1, 12, -1, 999},
    };

    int i;
    BOOL_T result;

    DEVRM_UT_CreateVirtualGroup();

    for (i=0; i < _countof(payload); ++i)
    {
        _payload_t *pl = &payload[i];

        result = DEVRM_AllocateRule(pl->device_id, pl->group_id, pl->vir_group_id,
                                                pl->rule_pri, &pl->dev_rule_id);
        assert(TRUE == result);
    }

    DEVRM_UT_Proc_Validate();

    return 0;
}

int
DEVRM_UT_RunTestCaese()
{
#define DEVRM_TEST(func)    \
    UT_Test(DEVRM_UT_Setup, DEVRM_UT_Teardown, func)

    DEVRM_TEST(DEVRM_UT_CreateVirtualGroup);
    DEVRM_TEST(DEVRM_UT_CreateVirtualGroup_By_OEM);
    DEVRM_TEST(DEVRM_UT_AllocateRule);

    devrm_validate_rule_func = DEVRM_UT_Proc_Validate;

    return 0;

#undef DEVRM_TEST
}

