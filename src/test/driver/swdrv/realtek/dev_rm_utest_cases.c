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
#define IGNORE 0xFFFFFF

#define DEVRM_VECTOR_GRP(_dev)  devrm_data.dev[_dev].group
#define DEVRM_VECTOR_VG(_dev)  devrm_data.dev[_dev].vir_grp
#define DEVRM_VECTOR_SLC(_dev)  devrm_data.dev[_dev].slice

typedef struct
{
    UI32_T group_id;
    UI32_T vir_group_id;
    int vir_group_pri;
} DEVRM_UT_VG_PROFILE_T;

void
DEVRM_UT_Proc_Validate_Virtual_Group()
{
    int dev;

    for (dev=0; dev < soc_ndev; ++dev)
    {
        DEVRM_VirtualGroupVector_T *vg_vec = &DEVRM_VECTOR_VG(dev);
        DEVRM_VirtualGroup_T *vg_1 = NULL;
        DEVRM_VirtualGroup_T *vg_2 = NULL;
        UI32_T i;

        for (i = 0; i < _countof(vg_vec->vg_tbl); ++i)
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

        for (vg_idx = 0; vg_idx < _countof(DEVRM_VECTOR_VG(dev).vg_tbl); ++vg_idx)
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

                    DEVRM_DevRule_T *dev_rule_p;

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
                    dev_rule_p = _DEVRM_FindDevRule(dev, group_id, rule_1->rule_id);
                    assert(dev_rule_p != NULL);
                    phy_pri_1 = dev_rule_p->dr_pri;

                    dev_rule_p = _DEVRM_FindDevRule(dev, group_id, rule_2->rule_id);
                    assert(dev_rule_p != NULL);
                    phy_pri_2 = dev_rule_p->dr_pri;

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

        for (vg_idx = 0; vg_idx < DEVRM_VECTOR_VG(dev).vsel_num; ++vg_idx)
        {
            DEVRM_VirtualGroup_T *vg_p = &DEVRM_VECTOR_VG(dev).vg_tbl[vg_idx];

            uint32 rule_idx;

            if (0 == vg_p->rv.cur_rule_nbr)
            {
                continue;
            }

            for (rule_idx = 0; rule_idx < _countof(vg_p->rv.rule_tbl); ++rule_idx)
            {
                DEVRM_Rule_T *rule_p = &vg_p->rv.rule_tbl[rule_idx];

                if (rule_p->valid == FALSE)
                {
                    continue;
                }

                if (rule_p->rule_id == dev_rule_id)
                {
                    DEVRM_DevRule_T *dev_rule_p;
                    memcpy(vg, vg_p, sizeof(*vg));
                    memcpy(rule, rule_p, sizeof(*rule));

                    dev_rule_p = _DEVRM_FindDevRule(dev, vg_p->group_id, rule_p->rule_id);
                    assert(dev_rule_p != NULL);

                    *hw_prio = dev_rule_p->dr_pri;

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
    /* FIXME:
     */
//    uint32 dev;
//    DEVRM_VirtualGroupVector_T *dup_vir_grp = malloc(sizeof(devrm_vir_grp));
//
//    assert(NULL != dup_vir_grp);
//
//    memcpy(dup_vir_grp, devrm_vir_grp, sizeof(devrm_vir_grp));
//
//    for (dev=0; dev < _countof(g_field_db.device); ++dev)
//    {
//        uint32 grp_idx;
//
//        for (grp_idx = 0; grp_idx < _countof(g_field_db.device[dev].group); ++grp_idx)
//        {
//            uint32 ent_idx;
//
//            for (ent_idx = 0; ent_idx < _countof(g_field_db.device[dev].group[grp_idx].entry_tbl); ++ent_idx)
//            {
//                _field_entry_t *ent = &g_field_db.device[dev].group[grp_idx].entry_tbl[ent_idx];
//
//                if (0 == ent->valid)
//                {
//                    continue;
//                }
//
//                //
//                // find rule by ent->dev_rule_id, and clear valid bit
//                //
//                {
//                    uint32 vg_idx;
//
//                    for (vg_idx = 0; vg_idx < devrm_vir_grp[dev].vsel_num; ++vg_idx)
//                    {
//                        DEVRM_VirtualGroup_T *vg = &dup_vir_grp[dev].vg_tbl[vg_idx];
//
//                        uint32 rule_idx;
//
//                        for (rule_idx = 0; rule_idx < vg->rv.cur_rule_nbr; ++rule_idx)
//                        {
//                            DEVRM_Rule_T *rule = &vg->rv.rule_tbl[rule_idx];
//
//                            if (rule->rule_id == ent->dev_rule_id)
//                            {
//                                rule->valid = 0;
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    for (dev = 0; dev < _countof(devrm_vir_grp); ++dev)
//    {
//        uint32 vg_idx;
//
//        for (vg_idx = 0; vg_idx < devrm_vir_grp[dev].vsel_num; ++vg_idx)
//        {
//            DEVRM_VirtualGroup_T *vg = &dup_vir_grp[dev].vg_tbl[vg_idx];
//
//            uint32 rule_idx;
//
//            for (rule_idx = 0; rule_idx < vg->rv.cur_rule_nbr; ++rule_idx)
//            {
//                DEVRM_Rule_T *rule = &vg->rv.rule_tbl[rule_idx];
//
//                assert(0 == rule->valid);
//            }
//        }
//    }
//
//    free(dup_vir_grp);
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

int
DEVRM_UT_Proc_CreateVirtualGroup(DEVRM_UT_VG_PROFILE_T *payload, int size)
{
    int i, device_id;
    DEVRM_GroupEntry_T grp;
    BOOL_T result;

    //
    // create group
    //
    for (i=0; i < size; ++i)
    {
        DEVRM_UT_VG_PROFILE_T *pl = &payload[i];

        memset(&grp, 0, sizeof(grp));
        grp.group_id = pl->group_id;
        if (SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP != 0 && pl->group_id < SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP)
            grp.phase = DEVRM_PHASE_INGRESS;
        else
            grp.phase = DEVRM_PHASE_EGRESS;
        grp.group_mode = DEVRM_GROUP_MODE_SINGLE;
        grp.template_num = 1;
        grp.template_key[0] = 2;

        for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; ++device_id)
        {
            result = DEVRM_CreateGroup(device_id, &grp);
            assert(TRUE == result);
        }
    }

    //
    // create virtual group
    //
    for (i=0; i < size; ++i)
    {
        DEVRM_UT_VG_PROFILE_T *pl = &payload[i];

        result = DEVRM_CreateVirtualGroup(pl->group_id,
                                          pl->vir_group_id,
                                          pl->vir_group_pri);
        assert(TRUE == result);
    }
    
    DEVRM_UT_Proc_Validate();
    
    return 0;
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
    DEVRM_UT_VG_PROFILE_T payload[] =
    {
        /* g_id, vg_id, vg_pri */
        {0,  0,  0},
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

        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1, 110, 0},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1, 111, -1},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1, 112, -2},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1, 113, -3},

#if (SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0)
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  120, -1},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  121, -2},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  122, -3},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  123, -4},

        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -13},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -12},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -11},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 133, -10},

        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 140, 1},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 141, 2},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 142, 3},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 143, 4},

        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 170, 1},
        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 171, 2},
        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 172, 3},
        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 173, 4},
#endif /* SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0 */

    };

    DEVRM_UT_Proc_CreateVirtualGroup(payload, _countof(payload));

    return 0;
}

int
DEVRM_UT_CreateVirtualGroup_For_Egress()
{
#if (SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0)
    typedef struct
    {
        UI32_T group_id;
        UI32_T vir_group_id;
        int vir_group_pri;
    } _payload_t;

    _payload_t payload[] =
    {
        {12,  1, -1},
        {12,  2, -2},
        {12,  3, -3},

        {13, 10, -13},
        {13, 11, -12},
        {13, 12, -11},
        {13, 13, -10},

        {14, 20, 1},
        {14, 21, 2},
        {14, 22, 3},
        {14, 23, 4},

    };

    int i, device_id;
    DEVRM_GroupEntry_T grp;
    BOOL_T result;

    //
    // create group
    //
    for (i = 0; i < _countof(payload); ++i)
    {
        _payload_t *pl = &payload[i];

        memset(&grp, 0, sizeof(grp));
        grp.group_id = pl->group_id;
        grp.phase = DEVRM_PHASE_EGRESS;
        grp.group_mode = DEVRM_GROUP_MODE_SINGLE;
        grp.template_num = 1;
        grp.template_key[0] = 0;

        for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; ++device_id)
        {
            result = DEVRM_CreateGroup(device_id, &grp);
            assert(TRUE == result);
        }
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

#endif /* SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0 */

    return 0;
}


#include "fp_config.h"
int
DEVRM_UT_CreateVirtualGroup_By_OEM()
{
    FP_CONFIG_STRUCT_T  *fp_config_p;
    UI32_T group_i;
    UI32_T vg_i;
    BOOL_T result;
    int device_id;

    for (group_i = 0; group_i < FP_CONFIG_GetConfigSize(); ++group_i)
    {
        fp_config_p = FP_CONFIG_GetConfig(group_i);
        if (NULL == fp_config_p)
        {
            continue;
        }

        if (FALSE == _DEVRM_IsGroupExisted(0, fp_config_p->group_id))
        {
            DEVRM_GroupEntry_T grp;
            int tk_idx;

            memset(&grp, 0, sizeof(grp));

            grp.group_id = fp_config_p->group_id;
            grp.group_mode = fp_config_p->group_mode;
            grp.phase = fp_config_p->selector_stage;

            for (tk_idx = 0; tk_idx <fp_config_p->tk_vector.count; ++tk_idx)
            {
                grp.template_key[tk_idx] = fp_config_p->tk_vector.tk[tk_idx].template_key;
                grp.template_num++;
            }

            for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; ++device_id)
            {
                result = DEVRM_CreateGroup(device_id, &grp);
                assert(TRUE == result);
            }
        }

        for (vg_i = 0; vg_i < fp_config_p->func_vector.count; ++vg_i)
        {
            result = DEVRM_CreateVirtualGroup(fp_config_p->group_id,
                                              fp_config_p->func_vector.fd[vg_i].func_type,
                                              fp_config_p->func_vector.fd[vg_i].rule_pri);
            assert(TRUE == result);
        }
    }

    DEVRM_UT_Proc_Validate();

//    _DEVRM_BackdoorMain();

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
        {0, 0,  1, -1, IGNORE},
        {0, 0,  1, -1, IGNORE},
        {0, 0,  1, -1, IGNORE},

        {0, 0,  2, -2, IGNORE},
        {0, 0,  2, -3, IGNORE},
        {0, 0,  2, -4, IGNORE},

        {0, 0,  3, -3, IGNORE},
        {0, 0,  3, -2, IGNORE},
        {0, 0,  3, -1, IGNORE},

        // device_id = 0, group_id = 1
        {0, 1, 10, -1, IGNORE},
        {0, 1, 10, -1, IGNORE},
        {0, 1, 10, -1, IGNORE},

        {0, 1, 11, -2, IGNORE},
        {0, 1, 11, -3, IGNORE},
        {0, 1, 11, -4, IGNORE},

        {0, 1, 12, -3, IGNORE},
        {0, 1, 12, -2, IGNORE},
        {0, 1, 12, -1, IGNORE},

#if (SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0)
        // device_id = , group_id = SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 120, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 120, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 120, -1, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 121, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 121, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 121, -4, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 122, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 122, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 122, -1, IGNORE},

        // device_id = , group_id = (SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP + 1)
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -1, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -4, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -1, IGNORE},
#endif /* (SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0) */
    };

    DEVRM_UT_VG_PROFILE_T vg_profile[] =
    {
        /* g_id, vg_id, vg_pri */
        {0,  0,  0},
        {0,  1, -1},
        {0,  2, -2},
        {0,  3, -3},

        {1, 10, -13},
        {1, 11, -12},
        {1, 12, -11},
        {1, 13, -10},

#if (SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0)
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  120, -1},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  121, -2},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  122, -3},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  123, -4},

        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -13},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -12},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -11},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 133, -10},
#endif /* SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0 */
        
    };

    int i;
    BOOL_T result;

    DEVRM_UT_Proc_CreateVirtualGroup(vg_profile, _countof(vg_profile));

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

int DEVRM_UT_DestoryRule()
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
        {0, 0,  1, -1, IGNORE},
        {0, 0,  1, -1, IGNORE},
        {0, 0,  1, -1, IGNORE},

        {0, 0,  2, -2, IGNORE},
        {0, 0,  2, -3, IGNORE},
        {0, 0,  2, -4, IGNORE},

        {0, 0,  3, -3, IGNORE},
        {0, 0,  3, -2, IGNORE},
        {0, 0,  3, -1, IGNORE},

        // device_id = 0, group_id = 1
        {0, 1, 10, -1, IGNORE},
        {0, 1, 10, -1, IGNORE},
        {0, 1, 10, -1, IGNORE},

        {0, 1, 11, -2, IGNORE},
        {0, 1, 11, -3, IGNORE},
        {0, 1, 11, -4, IGNORE},

        {0, 1, 12, -3, IGNORE},
        {0, 1, 12, -2, IGNORE},
        {0, 1, 12, -1, IGNORE},

#if (SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0)
        // device_id = , group_id = SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 120, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 120, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 120, -1, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 121, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 121, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 121, -4, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 122, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 122, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 122, -1, IGNORE},

        // device_id = , group_id = (SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP + 1)
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -1, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -4, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -1, IGNORE},
#endif /* (SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0) */
    };

    DEVRM_UT_VG_PROFILE_T vg_profile[] =
    {
        /* g_id, vg_id, vg_pri */
        {0,  0,  0},
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

        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1, 110, 0},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1, 111, -1},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1, 112, -2},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1, 113, -3},

#if (SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0)
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  120, -1},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  121, -2},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  122, -3},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  123, -4},

        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -13},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -12},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -11},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 133, -10},

        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 140, 1},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 141, 2},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 142, 3},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 143, 4},

        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 170, 1},
        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 171, 2},
        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 172, 3},
        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 173, 4},
#endif /* SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0 */
    };

    int i;
    BOOL_T result;

    DEVRM_UT_Proc_CreateVirtualGroup(vg_profile, _countof(vg_profile));

    for (i=0; i < _countof(payload); ++i)
    {
        _payload_t *pl = &payload[i];

        result = DEVRM_AllocateRule(pl->device_id, pl->group_id, pl->vir_group_id,
                                    pl->rule_pri, &pl->dev_rule_id);
        assert(TRUE == result);

        result = DEVRM_DestroyRule(pl->device_id, pl->vir_group_id, pl->dev_rule_id);
        assert(TRUE == result);
    }
    
    DEVRM_UT_Proc_Validate();

    for (i=0; i < _countof(payload); ++i)
    {
        _payload_t *pl = &payload[i];

        result = DEVRM_AllocateRule(pl->device_id, pl->group_id, pl->vir_group_id,
                                    pl->rule_pri, &pl->dev_rule_id);
        assert(TRUE == result);
    }

    for (i=0; i < _countof(payload); ++i)
    {
        _payload_t *pl = &payload[i];

        result = DEVRM_DestroyRule(pl->device_id, pl->vir_group_id, pl->dev_rule_id);
        assert(TRUE == result);
    }

    DEVRM_UT_Proc_Validate();
    
    return 0;
}

int
DEVRM_UT_SetRule_Validate_Index()
{
    typedef struct
    {
        UI32_T  device_id;
        UI32_T  group_id;
        UI32_T  vir_group_id;
        int     rule_pri;
        UI32_T  dev_rule_id;    // out
        UI32_T  exp_hw_index;   // expect hardware index value of slice
        UI32_T  exp_final_hw_index;   // expect hardware index value of slice
    } _payload_t;

    _payload_t payload[] =
    {
        /* device, group, vgroup, rule_pri, dev_rule_id */
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1,  110, -1, IGNORE, 0, 0},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1,  110, -1, IGNORE, 1, 1},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1,  110, -1, IGNORE, 2, 2},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1,  111, -4, IGNORE, 3, 5},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1,  111, -3, IGNORE, 3, 4},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1,  111, -2, IGNORE, 3, 3},
    };
    DEVRM_UT_VG_PROFILE_T vg_profile[] =
    {
        /* g_id, vg_id, vg_pri */
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1, 110, 0},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP-1, 111, -1}
    };

    int i;
    BOOL_T result;

    DEVRM_UT_Proc_CreateVirtualGroup(vg_profile, _countof(vg_profile));

    for (i = 0; i < _countof(payload); ++i)
    {
        DEVRM_AceEntry_T ace_entry;
        DEVRM_ACTION_T dev_action;
        int slice_n, j;

        DEVRM_Group_T *group_p;
        DEVRM_Slice_T *slice_p;
        DEVRM_DevRule_T *dev_rule_p;

        _payload_t *pl = &payload[i];

        /* Allocate rule
         */
        result = DEVRM_AllocateRule(pl->device_id, pl->group_id, pl->vir_group_id,
                                    pl->rule_pri, &pl->dev_rule_id);
        assert(TRUE == result);

        /* Set rule
         */
        DEVRM_LIB_RulePatternInit(&ace_entry);
        DEVRM_LIB_ActionInit(&dev_action);

        group_p = _DEVRM_GetGroupById(pl->device_id, pl->group_id);
        assert(group_p != NULL);
        dev_rule_p = _DEVRM_FindDevRule(pl->device_id, pl->group_id, pl->dev_rule_id);
        assert(dev_rule_p != NULL);

        for (slice_n = 0; slice_n < group_p->slice_num; ++slice_n)
        {
            slice_p = _DEVRM_GetSliceByIdx(pl->device_id, group_p->slice_id + slice_n);
            assert(NULL != slice_p);
            assert(slice_p->template_num == group_p->rule_size);

            for (j = 0; j < slice_p->template_num; ++j)
            {
                rtk_acl_entry_t * hw_entry_p = NULL;
                UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

                assert(dev_rule_p->slice_rule_index == pl->exp_hw_index);
                assert(hw_rule_idx == ((pl->exp_hw_index + j)
                                       + (slice_p->idx * SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR)));

                hw_entry_p = rtk_ut_acl_getSdkRuleEntry(pl->device_id, group_p->phase, hw_rule_idx);
                assert(hw_entry_p != NULL);
                assert(hw_entry_p->valid == DISABLED);
            }
        }

        result = DEVRM_SetRule(pl->device_id, pl->group_id, pl->dev_rule_id,
                               &ace_entry, &dev_action);
        assert(result == TRUE);

        for (slice_n = 0; slice_n < group_p->slice_num; ++slice_n)
        {
            slice_p = _DEVRM_GetSliceByIdx(pl->device_id, group_p->slice_id + slice_n);
            assert(NULL != slice_p);
            assert(slice_p->template_num == group_p->rule_size);

            for (j = 0; j < slice_p->template_num; ++j)
            {
                rtk_acl_entry_t * hw_entry_p = NULL;
                UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

                hw_entry_p = rtk_ut_acl_getSdkRuleEntry(pl->device_id, group_p->phase, hw_rule_idx);
                assert(hw_entry_p != NULL);

                assert(hw_entry_p->valid == ENABLED);
            }
        }
    }

    /* validate final hw index
     */
    for (i = 0; i < _countof(payload); ++i)
    {
        int slice_n, j;

        DEVRM_Group_T *group_p;
        DEVRM_Slice_T *slice_p;
        DEVRM_DevRule_T *dev_rule_p;

        _payload_t *pl = &payload[i];

        group_p = _DEVRM_GetGroupById(pl->device_id, pl->group_id);
        assert(group_p != NULL);
        dev_rule_p = _DEVRM_FindDevRule(pl->device_id, pl->group_id, pl->dev_rule_id);
        assert(dev_rule_p != NULL);

        for (slice_n = 0; slice_n < group_p->slice_num; ++slice_n)
        {
            slice_p = _DEVRM_GetSliceByIdx(pl->device_id, group_p->slice_id + slice_n);
            assert(NULL != slice_p);
            assert(slice_p->template_num == group_p->rule_size);

            for (j = 0; j < slice_p->template_num; ++j)
            {
                rtk_acl_entry_t * hw_entry_p = NULL;
                UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

                assert(dev_rule_p->slice_rule_index == pl->exp_final_hw_index);
                assert(hw_rule_idx == ((pl->exp_final_hw_index + j)
                                       + (slice_p->idx * SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR)));

                hw_entry_p = rtk_ut_acl_getSdkRuleEntry(pl->device_id, group_p->phase, hw_rule_idx);
                assert(hw_entry_p != NULL);
                
                assert(hw_entry_p->valid == ENABLED);
            }
        }
    }

    DEVRM_UT_Proc_Validate();
    
    return 0;
}

int
DEVRM_UT_SetRule()
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
        {0, 0,  1, -1, IGNORE},
        {0, 0,  1, -1, IGNORE},
        {0, 0,  1, -1, IGNORE},

        {0, 0,  2, -2, IGNORE},
        {0, 0,  2, -3, IGNORE},
        {0, 0,  2, -4, IGNORE},

        // device_id = 0, group_id = 1
        {0, 1, 10, -1, IGNORE},
        {0, 1, 10, -1, IGNORE},
        {0, 1, 10, -1, IGNORE},

        {0, 1, 11, -2, IGNORE},
        {0, 1, 11, -3, IGNORE},
        {0, 1, 11, -4, IGNORE},
    };

    DEVRM_UT_VG_PROFILE_T vg_profile[] =
    {
        /* g_id, vg_id, vg_pri */
        {0,  0,  0},
        {0,  1, -1},
        {0,  2, -2},
        {0,  3, -3},

        {1, 10, -13},
        {1, 11, -12},
        {1, 12, -11},
        {1, 13, -10},
    };

    int i;
    BOOL_T result;

    DEVRM_UT_Proc_CreateVirtualGroup(vg_profile, _countof(vg_profile));

    for (i = 0; i < _countof(payload); ++i)
    {
        _payload_t *pl = &payload[i];

        result = DEVRM_AllocateRule(pl->device_id, pl->group_id, pl->vir_group_id,
                                    pl->rule_pri, &pl->dev_rule_id);
        assert(TRUE == result);
    }
    
    DEVRM_UT_Proc_Validate();

    for (i = 0; i < _countof(payload); ++i)
    {
        DEVRM_AceEntry_T ace_entry;
        DEVRM_ACTION_T dev_action;
        DEVRM_PACKET_COMMAND_T pkt_cmd;
        int slice_n, j;

        DEVRM_Group_T *group_p;
        DEVRM_Slice_T *slice_p;
        DEVRM_DevRule_T *dev_rule_p;

        DEVRM_LIB_RulePatternInit(&ace_entry);
        DEVRM_LIB_ActionInit(&dev_action);

        _payload_t *pl = &payload[i];
        group_p = _DEVRM_GetGroupById(pl->device_id, pl->group_id);
        assert(group_p != NULL);

        dev_rule_p = _DEVRM_FindDevRule(pl->device_id, pl->group_id, pl->dev_rule_id);
        assert(dev_rule_p != NULL);

        for (slice_n = 0; slice_n < group_p->slice_num; ++slice_n)
        {
            slice_p = _DEVRM_GetSliceByIdx(pl->device_id, group_p->slice_id + slice_n);
            assert(NULL != slice_p);
            assert(slice_p->template_num == group_p->rule_size);

            for (j = 0; j < slice_p->template_num; ++j)
            {
                rtk_acl_entry_t * hw_entry_p = NULL;
                UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

                hw_entry_p = rtk_ut_acl_getSdkRuleEntry(pl->device_id, group_p->phase, hw_rule_idx);
                assert(hw_entry_p != NULL);

                assert(hw_entry_p->valid == DISABLED);
            }
        }

        pkt_cmd = ((i%2) == 0) ? DEVRM_PKT_CMD_FORWARD : DEVRM_PKT_CMD_SOFT_DROP;
        DEVRM_LIB_ActionPacketTo(&dev_action, pkt_cmd);

        result = DEVRM_SetRule(pl->device_id, pl->group_id, pl->dev_rule_id,
                               &ace_entry, &dev_action);
        assert(result == TRUE);

        for (slice_n = 0; slice_n < group_p->slice_num; ++slice_n)
        {
            slice_p = _DEVRM_GetSliceByIdx(pl->device_id, group_p->slice_id + slice_n);
            assert(NULL != slice_p);
            assert(slice_p->template_num == group_p->rule_size);

            for (j = 0; j < slice_p->template_num; ++j)
            {
                rtk_acl_entry_t * hw_entry_p = NULL;
                UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

                hw_entry_p = rtk_ut_acl_getSdkRuleEntry(pl->device_id, group_p->phase, hw_rule_idx);
                assert(hw_entry_p != NULL);

                assert(hw_entry_p->valid == ENABLED);

                {
                    assert(hw_entry_p->action.igr_acl.fwd_en == ENABLED);
                    if (pkt_cmd == DEVRM_PKT_CMD_FORWARD)
                    {
                        assert(hw_entry_p->action.igr_acl.fwd_data.fwd_type == ACL_IGR_ACTION_FWD_PERMIT);
                    }
                    else
                    {
                        assert(hw_entry_p->action.igr_acl.fwd_data.fwd_type == ACL_IGR_ACTION_FWD_DROP);
                    }
                }
            }
        }
    }


    return 0;
}

int
DEVRM_UT_SetRule_Egress()
{
#if (SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0)
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

        // device_id = , group_id = 12
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 120, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 120, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 120, -1, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 121, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 121, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 121, -4, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 122, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 122, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 122, -1, IGNORE},

        // device_id = , group_id = 13
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -1, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -4, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -1, IGNORE},


        // device_id = , group_id = 17
        {0, SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 170, -3, IGNORE},
        {0, SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 171, -2, IGNORE},
        {0, SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 173, -1, IGNORE},
    };

    DEVRM_UT_VG_PROFILE_T vg_profile[] =
    {
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  120, -1},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  121, -2},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  122, -3},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  123, -4},

        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -13},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -12},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -11},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 133, -10},

        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 140, 1},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 141, 2},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 142, 3},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 143, 4},

        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 170, 1},
        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 171, 2},
        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 172, 3},
        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 173, 4},
    };

    int i;
    BOOL_T result;

    DEVRM_UT_Proc_CreateVirtualGroup(vg_profile, _countof(vg_profile));

    for (i = 0; i < _countof(payload); ++i)
    {
        _payload_t *pl = &payload[i];

        result = DEVRM_AllocateRule(pl->device_id, pl->group_id, pl->vir_group_id,
                                    pl->rule_pri, &pl->dev_rule_id);
        assert(TRUE == result);
    }

    DEVRM_UT_Proc_Validate();

    for (i = 0; i < _countof(payload); ++i)
    {
        DEVRM_AceEntry_T ace_entry;
        DEVRM_ACTION_T dev_action;
        int slice_n, j;

        DEVRM_Group_T *group_p;
        DEVRM_Slice_T *slice_p;
        DEVRM_DevRule_T *dev_rule_p;

        DEVRM_LIB_RulePatternInit(&ace_entry);
        DEVRM_LIB_ActionInit(&dev_action);

        _payload_t *pl = &payload[i];
        group_p = _DEVRM_GetGroupById(pl->device_id, pl->group_id);
        assert(group_p != NULL);

        dev_rule_p = _DEVRM_FindDevRule(pl->device_id, pl->group_id, pl->dev_rule_id);
        assert(dev_rule_p != NULL);

        for (slice_n = 0; slice_n < group_p->slice_num; ++slice_n)
        {
            slice_p = _DEVRM_GetSliceByIdx(pl->device_id, group_p->slice_id + slice_n);
            assert(NULL != slice_p);
            assert(slice_p->template_num == group_p->rule_size);

            for (j = 0; j < slice_p->template_num; ++j)
            {
                rtk_acl_entry_t * hw_entry_p = NULL;
                UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

                hw_entry_p = rtk_ut_acl_getSdkRuleEntry(pl->device_id, group_p->phase, hw_rule_idx);
                assert(hw_entry_p != NULL);

                assert(hw_entry_p->valid == DISABLED);
            }
        }

        result = DEVRM_SetRule(pl->device_id, pl->group_id, pl->dev_rule_id,
                               &ace_entry, &dev_action);
        assert(result == TRUE);

        for (slice_n = 0; slice_n < group_p->slice_num; ++slice_n)
        {
            slice_p = _DEVRM_GetSliceByIdx(pl->device_id, group_p->slice_id + slice_n);
            assert(NULL != slice_p);
            assert(slice_p->template_num == group_p->rule_size);

            for (j = 0; j < slice_p->template_num; ++j)
            {
                rtk_acl_entry_t * hw_entry_p = NULL;
                UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

                //printf("device = %lu, slice_id = %lu, dev_rule_id = %lu, hw_index = %lu\r\n",
                //       pl->device_id, slice_p->idx, pl->dev_rule_id, hw_rule_idx);

                hw_entry_p = rtk_ut_acl_getSdkRuleEntry(pl->device_id, group_p->phase, hw_rule_idx);
                assert(hw_entry_p != NULL);
                
                assert(hw_entry_p->valid == ENABLED);
            }
        }
    }
    
#endif /* (SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0) */
    return 0;
}

int
DEVRM_UT_SetRule_Validate_Action()
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
        {0, 0,  1, -1, IGNORE},
        {0, 0,  1, -1, IGNORE},
        {0, 0,  1, -1, IGNORE},

        {0, 0,  2, -2, IGNORE},
        {0, 0,  2, -3, IGNORE},
        {0, 0,  2, -4, IGNORE},

        // device_id = 0, group_id = 1
        {0, 1, 10, -1, IGNORE},
        {0, 1, 10, -1, IGNORE},
        {0, 1, 10, -1, IGNORE},

        {0, 1, 11, -2, IGNORE},
        {0, 1, 11, -3, IGNORE},
        {0, 1, 11, -4, IGNORE},
    };

    DEVRM_UT_VG_PROFILE_T vg_profile[] =
    {
        /* g_id, vg_id, vg_pri */
        {0,  0,  0},
        {0,  1, -1},
        {0,  2, -2},
        {0,  3, -3},

        {1, 10, -13},
        {1, 11, -12},
        {1, 12, -11},
        {1, 13, -10},
    };

    int i;
    BOOL_T result;

    DEVRM_UT_Proc_CreateVirtualGroup(vg_profile, _countof(vg_profile));

    for (i = 0; i < _countof(payload); ++i)
    {
        _payload_t *pl = &payload[i];

        result = DEVRM_AllocateRule(pl->device_id, pl->group_id, pl->vir_group_id,
                                    pl->rule_pri, &pl->dev_rule_id);
        assert(TRUE == result);
    }

    DEVRM_UT_Proc_Validate();

    for (i = 0; i < _countof(payload); ++i)
    {
        DEVRM_AceEntry_T ace_entry;
        DEVRM_ACTION_T dev_action;
        DEVRM_PACKET_COMMAND_T pkt_cmd;
        int slice_n, j;

        DEVRM_Group_T *group_p;
        DEVRM_Slice_T *slice_p;
        DEVRM_DevRule_T *dev_rule_p;

        DEVRM_LIB_RulePatternInit(&ace_entry);
        DEVRM_LIB_ActionInit(&dev_action);

        _payload_t *pl = &payload[i];
        group_p = _DEVRM_GetGroupById(pl->device_id, pl->group_id);
        assert(group_p != NULL);

        dev_rule_p = _DEVRM_FindDevRule(pl->device_id, pl->group_id, pl->dev_rule_id);
        assert(dev_rule_p != NULL);

        for (slice_n = 0; slice_n < group_p->slice_num; ++slice_n)
        {
            /* Validate hw rule status
             */
            slice_p = _DEVRM_GetSliceByIdx(pl->device_id, group_p->slice_id + slice_n);
            assert(NULL != slice_p);
            assert(slice_p->template_num == group_p->rule_size);

            for (j = 0; j < slice_p->template_num; ++j)
            {
                rtk_acl_entry_t * hw_entry_p = NULL;
                UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

                hw_entry_p = rtk_ut_acl_getSdkRuleEntry(pl->device_id, group_p->phase, hw_rule_idx);
                assert(hw_entry_p != NULL);

                assert(hw_entry_p->valid == DISABLED);
            }
        }

        {
            /* Setting action 
             */
            pkt_cmd = ((i%4) == 0) ? DEVRM_PKT_CMD_FORWARD
                       : ((i%4) == 1) ? DEVRM_PKT_CMD_SOFT_DROP
                       : ((i%4) == 2) ? DEVRM_PKT_CMD_REDIRECT_TO_CPU
                       : DEVRM_PKT_CMD_COPY_TO_CPU;

            DEVRM_LIB_ActionPacketTo(&dev_action, pkt_cmd);
            if (pkt_cmd == DEVRM_PKT_CMD_FORWARD)
            {
                DEVRM_LIB_ActionModifyInnerPriority(&dev_action, i%7);
                if (HAL_IS_RTL8380_FAMILY_ID(pl->device_id))
                {
                    DEVRM_LIB_ActionModifyPriority(&dev_action, i%7);
                }
            }
        }

        result = DEVRM_SetRule(pl->device_id, pl->group_id, pl->dev_rule_id,
                               &ace_entry, &dev_action);
        assert(result == TRUE);

        for (slice_n = 0; slice_n < group_p->slice_num; ++slice_n)
        {
            /* Validate hw rule status
             */
            slice_p = _DEVRM_GetSliceByIdx(pl->device_id, group_p->slice_id + slice_n);
            assert(NULL != slice_p);
            assert(slice_p->template_num == group_p->rule_size);

            for (j = 0; j < slice_p->template_num; ++j)
            {
                rtk_acl_entry_t * hw_entry_p = NULL;
                UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

                hw_entry_p = rtk_ut_acl_getSdkRuleEntry(pl->device_id, group_p->phase, hw_rule_idx);
                assert(hw_entry_p != NULL);

                assert(hw_entry_p->valid == ENABLED);

                {
                    /* Validate hw rule action status
                     */
                    assert(hw_entry_p->action.igr_acl.fwd_en == ENABLED);
                    switch (pkt_cmd)
                    {
                        case DEVRM_PKT_CMD_FORWARD:
                            assert(hw_entry_p->action.igr_acl.fwd_data.fwd_type == ACL_IGR_ACTION_FWD_PERMIT);

                            assert(hw_entry_p->action.igr_acl.pri_en == ENABLED);
                            assert(hw_entry_p->action.igr_acl.pri_data.pri == (i % 7));

                            if (HAL_IS_RTL8380_FAMILY_ID(pl->device_id))
                            {
                                assert(hw_entry_p->action.igr_acl.remark_en == ENABLED);
                                assert(hw_entry_p->action.igr_acl.remark_data.rmk_act == ACL_ACTION_REMARK_OUTER_USER_PRI);
                                assert(hw_entry_p->action.igr_acl.remark_data.rmk_info == (i % 7));

                            }
                            else if (HAL_IS_RTL8390_FAMILY_ID(pl->device_id))
                            {
                                assert(hw_entry_p->action.igr_acl.pri_en == ENABLED);
                                assert(hw_entry_p->action.igr_acl.pri_data.pri == (i % 7));
                            }
                            break;

                        case DEVRM_PKT_CMD_SOFT_DROP:
                            assert(hw_entry_p->action.igr_acl.fwd_data.fwd_type == ACL_IGR_ACTION_FWD_DROP);
                            break;

                        case DEVRM_PKT_CMD_REDIRECT_TO_CPU:
                            assert(hw_entry_p->action.igr_acl.fwd_data.fwd_type == ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTID);

                            if (HAL_IS_RTL8380_FAMILY_ID(pl->device_id))
                            {
                                assert(hw_entry_p->action.igr_acl.fwd_data.info.copy_redirect_port.force == ENABLED);
                                assert(hw_entry_p->action.igr_acl.fwd_data.info.copy_redirect_port.skip_igrStpDrop == ENABLED);
                                assert(hw_entry_p->action.igr_acl.fwd_data.info.copy_redirect_port.skip_storm_igrVlan == ENABLED);
                                assert(hw_entry_p->action.igr_acl.fwd_data.info.copy_redirect_port.fwd_port_id = CPU_PORT(pl->device_id));

                            }
                            else if (HAL_IS_RTL8390_FAMILY_ID(pl->device_id))
                            {
                                assert(hw_entry_p->action.igr_acl.fwd_data.fwd_info = CPU_PORT(pl->device_id));
                                assert(hw_entry_p->action.igr_acl.bypass_en == ENABLED);
                                assert(hw_entry_p->action.igr_acl.bypass_data.all == 1);
                                assert(hw_entry_p->action.igr_acl.bypass_data.igr_stp == 1);
                            }
                            break;

                        case DEVRM_PKT_CMD_COPY_TO_CPU:
                            assert(hw_entry_p->action.igr_acl.fwd_data.fwd_type == ACL_ACTION_FWD_COPY_TO_PORTID);

                            if (HAL_IS_RTL8380_FAMILY_ID(pl->device_id))
                            {
                                assert(hw_entry_p->action.igr_acl.fwd_data.info.copy_redirect_port.force == ENABLED);
                                assert(hw_entry_p->action.igr_acl.fwd_data.info.copy_redirect_port.skip_igrStpDrop == ENABLED);
                                assert(hw_entry_p->action.igr_acl.fwd_data.info.copy_redirect_port.skip_storm_igrVlan == ENABLED);
                                assert(hw_entry_p->action.igr_acl.fwd_data.info.copy_redirect_port.fwd_port_id = CPU_PORT(pl->device_id));

                            }
                            else if (HAL_IS_RTL8390_FAMILY_ID(pl->device_id))
                            {
                                assert(hw_entry_p->action.igr_acl.fwd_data.fwd_info = CPU_PORT(pl->device_id));
                                assert(hw_entry_p->action.igr_acl.bypass_en == ENABLED);
                                assert(hw_entry_p->action.igr_acl.bypass_data.all == 0);
                                assert(hw_entry_p->action.igr_acl.bypass_data.igr_stp == 1);
                            }
                            break;

                        default:
                            assert(0);
                    }
                }
            }
        }
    }
    
    return 0;
}

int
DEVRM_UT_SetRule_Validate_Action_Egress()
{
#if (SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0)
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

        // device_id = , group_id = 12
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 120, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 120, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 120, -1, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 121, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 121, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 121, -4, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 122, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 122, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP, 122, -1, IGNORE},

        // device_id = , group_id = 13
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -1, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -1, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -4, IGNORE},

        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -3, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -2, IGNORE},
        {0, SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -1, IGNORE},


        // device_id = , group_id = 17
        {0, SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 170, -3, IGNORE},
        {0, SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 171, -2, IGNORE},
        {0, SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 173, -1, IGNORE},
    };

    DEVRM_UT_VG_PROFILE_T vg_profile[] =
    {
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  120, -1},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  121, -2},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  122, -3},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP,  123, -4},

        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 130, -13},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 131, -12},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 132, -11},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+1, 133, -10},

        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 140, 1},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 141, 2},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 142, 3},
        {SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP+2, 143, 4},

        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 170, 1},
        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 171, 2},
        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 172, 3},
        {SYS_ADPT_FP_MAX_NBR_OF_GROUP_PER_CHIP-1, 173, 4},
    };

    int i;
    BOOL_T result;

    DEVRM_UT_Proc_CreateVirtualGroup(vg_profile, _countof(vg_profile));

    for (i = 0; i < _countof(payload); ++i)
    {
        _payload_t *pl = &payload[i];

        result = DEVRM_AllocateRule(pl->device_id, pl->group_id, pl->vir_group_id,
                                    pl->rule_pri, &pl->dev_rule_id);
        assert(TRUE == result);
    }

    DEVRM_UT_Proc_Validate();

    for (i = 0; i < _countof(payload); ++i)
    {
        DEVRM_AceEntry_T ace_entry;
        DEVRM_ACTION_T dev_action;
        DEVRM_PACKET_COMMAND_T pkt_cmd;
        int slice_n, j;

        DEVRM_Group_T *group_p;
        DEVRM_Slice_T *slice_p;
        DEVRM_DevRule_T *dev_rule_p;

        DEVRM_LIB_RulePatternInit(&ace_entry);
        DEVRM_LIB_ActionInit(&dev_action);

        _payload_t *pl = &payload[i];
        group_p = _DEVRM_GetGroupById(pl->device_id, pl->group_id);
        assert(group_p != NULL);

        dev_rule_p = _DEVRM_FindDevRule(pl->device_id, pl->group_id, pl->dev_rule_id);
        assert(dev_rule_p != NULL);

        for (slice_n = 0; slice_n < group_p->slice_num; ++slice_n)
        {
            slice_p = _DEVRM_GetSliceByIdx(pl->device_id, group_p->slice_id + slice_n);
            assert(NULL != slice_p);
            assert(slice_p->template_num == group_p->rule_size);

            for (j = 0; j < slice_p->template_num; ++j)
            {
                rtk_acl_entry_t * hw_entry_p = NULL;
                UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

                hw_entry_p = rtk_ut_acl_getSdkRuleEntry(pl->device_id, group_p->phase, hw_rule_idx);
                assert(hw_entry_p != NULL);

                assert(hw_entry_p->valid == DISABLED);
            }
        }

        /* Setting action
         */
        pkt_cmd = ((i%2) == 0) ? DEVRM_PKT_CMD_FORWARD : DEVRM_PKT_CMD_SOFT_DROP;
        DEVRM_LIB_ActionPacketTo(&dev_action, pkt_cmd);
        if (pkt_cmd == DEVRM_PKT_CMD_FORWARD)
        {
            DEVRM_LIB_ActionModifyInnerPriority(&dev_action, i%7);
            DEVRM_LIB_ActionModifyPriority(&dev_action, (i+1)%7);
        }

        result = DEVRM_SetRule(pl->device_id, pl->group_id, pl->dev_rule_id,
                               &ace_entry, &dev_action);
        assert(result == TRUE);

        for (slice_n = 0; slice_n < group_p->slice_num; ++slice_n)
        {
            slice_p = _DEVRM_GetSliceByIdx(pl->device_id, group_p->slice_id + slice_n);
            assert(NULL != slice_p);
            assert(slice_p->template_num == group_p->rule_size);

            for (j = 0; j < slice_p->template_num; ++j)
            {
                rtk_acl_entry_t * hw_entry_p = NULL;
                UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

                //printf("device = %lu, slice_id = %lu, dev_rule_id = %lu, hw_index = %lu\r\n",
                //       pl->device_id, slice_p->idx, pl->dev_rule_id, hw_rule_idx);

                hw_entry_p = rtk_ut_acl_getSdkRuleEntry(pl->device_id, group_p->phase, hw_rule_idx);
                assert(hw_entry_p != NULL);
                
                assert(hw_entry_p->valid == ENABLED);

                {
                    /* Validate hw rule action status
                     */
                    assert(HAL_IS_RTL8380_FAMILY_ID(pl->device_id) == 0);
                    assert(hw_entry_p->action.egr_acl.fwd_en == ENABLED);
                    if (pkt_cmd == DEVRM_PKT_CMD_FORWARD)
                    {
                        assert(hw_entry_p->action.egr_acl.fwd_data.fwd_type == ACL_IGR_ACTION_FWD_PERMIT);

                        assert(hw_entry_p->action.egr_acl.pri_en == ENABLED);
                        assert(hw_entry_p->action.egr_acl.pri_data.pri == (i % 7));

                        assert(hw_entry_p->action.egr_acl.rmk_en == ENABLED);
                        assert(hw_entry_p->action.egr_acl.rmk_data.rmk_type == ACL_ACTION_REMARK_OUTER_USER_PRI);
                        assert(hw_entry_p->action.egr_acl.rmk_data.rmk_value == ((i+1) % 7));
                    }
                    else
                    {
                        assert(hw_entry_p->action.egr_acl.fwd_data.fwd_type == ACL_IGR_ACTION_FWD_DROP);
                    }
                }
            }
        }
    }
    
#endif /* (SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP != 0) */
    return 0;
}

int
DEVRM_UT_RunTestCaese()
{
#define DEVRM_TEST(func)    \
    UT_Test(DEVRM_UT_Setup, DEVRM_UT_Teardown, func)

    //_DEVRM_BackdoorMain();

    DEVRM_SetDebugFlags(DEVRM_DEBUG_FLAGS_BIT_ERR);

    DEVRM_TEST(DEVRM_UT_CreateVirtualGroup);
    DEVRM_TEST(DEVRM_UT_CreateVirtualGroup_For_Egress);
    DEVRM_TEST(DEVRM_UT_CreateVirtualGroup_By_OEM);
    DEVRM_TEST(DEVRM_UT_AllocateRule);
    DEVRM_TEST(DEVRM_UT_DestoryRule);

    DEVRM_TEST(DEVRM_UT_SetRule_Validate_Index);
    DEVRM_TEST(DEVRM_UT_SetRule);
    DEVRM_TEST(DEVRM_UT_SetRule_Egress);
    DEVRM_TEST(DEVRM_UT_SetRule_Validate_Action);
    DEVRM_TEST(DEVRM_UT_SetRule_Validate_Action_Egress);

    DEVRM_SetDebugFlags(0);
    //DEVRM_SetDebugFlags(DEVRM_DEBUG_FLAGS_BIT_TRACE);

   //devrm_validate_rule_func = DEVRM_UT_Proc_Validate;

    return 0;

#undef DEVRM_TEST
}

