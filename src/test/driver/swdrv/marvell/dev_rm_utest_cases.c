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

#define DEVRM_MAX_NBR_OF_STD_RULE       (DEVRM_MAX_NBR_OF_TCAM_ENTRIES)
#define DEVRM_MAX_NBR_OF_EXT_RULE       (DEVRM_MAX_NBR_OF_TCAM_ENTRIES/2)

void
DEVRM_UT_Proc_Validate_Order_Of_Rule()
{
    UI32_T  device_id;

    for (device_id = 0; device_id < DEVRM_GetNumberOfChips(); ++ device_id)
    {
        DEVRM_PCL_T *pcl;

        DEVRM_VirtualGroup_T *vg;

        DEVRM_PCE_T *pce;
        DEVRM_PCE_T *prev_pce;

        UI32_T  pcl_id;
        UI32_T  idx;

        UI8_T  *tcam_map = NULL;

        tcam_map = calloc(1, sizeof(DEVRM_DEVICE_PTR(device_id)->pces_use_status) * 8);
        if (NULL == tcam_map)
        {
            assert(0);
        }

        for (pcl_id = 1; pcl_id <= _countof(DEVRM_DEVICE_PTR(device_id)->pcls); ++pcl_id)
        {
            pcl = DEVRM_GetPcl(device_id, pcl_id);
            if (NULL == pcl)
            {
                continue;
            }

            prev_pce = pcl->pce_list;

            if (NULL != prev_pce)
            {
                assert(0 == tcam_map[prev_pce->hw_idx]);
                tcam_map[prev_pce->hw_idx] = 1;
            }

            pce = pcl->pce_list;
            DEVRM_GetNextPce(device_id, &pce);

            if (NULL != prev_pce && NULL != pce)
            {
                do
                {
                    if (pce->pcl_id != pcl_id)
                    {
                        break;
                    }

                    assert(prev_pce->hw_idx < pce->hw_idx);

                    assert(0 == tcam_map[pce->hw_idx]);

                    tcam_map[pce->hw_idx] = 1;

                    prev_pce = pce;
                } while ( TRUE == DEVRM_GetNextPce(device_id, &pce) );
            }

            for (idx=0; idx < _countof(pcl->vg_tbl); ++idx)
            {
                vg = &pcl->vg_tbl[idx];

                if (TRUE != vg->valid)
                {
                    continue;
                }

                prev_pce = vg->rv.pce_list;
                pce = vg->rv.pce_list;
                DEVRM_GetNextPce(device_id, &pce);

                if (NULL != prev_pce && NULL != pce)
                {
                    do
                    {
                        if (pce->pcl_id != vg->pcl_id)
                        {
                            break;
                        }

                        if (pce->vir_group_id != vg->vir_group_id)
                        {
                            break;
                        }

                        assert(pce->priority <= prev_pce->priority);
                        prev_pce = pce;
                    } while ( TRUE == DEVRM_GetNextPce(device_id, &pce) );
                }
            }
        }

        free(tcam_map);
    }
}

void
DEVRM_UT_Proc_Validate_Order_Of_Rule_Slow()
{
    enum
    {
        MAX_CNT = _countof(DEVRM_DEVICE_PTR(0)->pces_use_status) * (sizeof(*DEVRM_DEVICE_PTR(0)->pces_use_status) * 8)
    };

    UI32_T device_id;

    for (device_id = 0; device_id < DEVRM_GetNumberOfChips(); ++ device_id)
    {
        UI32_T idx;
        UI32_T idx_2;

        DEVRM_PCE_T *l_pce = NULL;
        DEVRM_PCE_T *r_pce = NULL;

        for (idx=0; idx < MAX_CNT; ++idx)
        {
            DEVRM_GetPceAtHwIndex(device_id, idx, &l_pce);
            if (NULL == l_pce)
            {
                continue;
            }

            for (idx_2 = idx+1; idx_2 < MAX_CNT; ++idx_2)
            {
                DEVRM_GetPceAtHwIndex(device_id, idx_2, &r_pce);

                if (NULL == r_pce)
                {
                    continue;
                }

                if (idx == idx_2)
                {
                    continue;
                }

                if (l_pce == r_pce)
                {
                    assert(l_pce->is_std_pce == FALSE);
                    continue;
                }

                assert(l_pce->hw_idx != r_pce->hw_idx);

                if (l_pce->hw_idx < r_pce->hw_idx)
                {
                    assert(l_pce->pcl_id <= r_pce->pcl_id);

                    if (l_pce->pcl_id == r_pce->pcl_id)
                    {
                        assert(l_pce->vir_group_id >= r_pce->vir_group_id);

                        if (l_pce->vir_group_id == r_pce->vir_group_id)
                        {
                            assert(l_pce->priority >= r_pce->priority);
                        }
                    }
                }
                else
                {
                    assert(r_pce->pcl_id <= l_pce->pcl_id);

                    if (r_pce->pcl_id == l_pce->pcl_id)
                    {
                        assert(r_pce->vir_group_id >= l_pce->vir_group_id);

                        if (r_pce->vir_group_id == l_pce->vir_group_id)
                        {
                            assert(r_pce->priority >= l_pce->priority);
                        }
                    }
                }

            }
        }
    }
}

void
DEVRM_UT_Proc_Validate_Rule_Id()
{
    UI32_T device_id;

    for (device_id = 0; device_id < DEVRM_GetNumberOfChips(); ++ device_id)
    {
        DEVRM_PCE_T *pce;

        pce = NULL;
        while (TRUE == DEVRM_GetNextPce(device_id, &pce))
        {
            DEVRM_PCE_T *pce_2;

            pce_2 = pce;

            while (TRUE == DEVRM_GetNextPce(device_id, &pce_2))
            {
                assert(pce != pce_2);
                assert(pce->upper_rule_id != pce_2->upper_rule_id);
            }
        }
    }
}

void
DEVRM_UT_Proc_Validate_Number_Of_Rule()
{
    UI32_T  device_id;

    for (device_id = 0; device_id < DEVRM_GetNumberOfChips(); ++ device_id)
    {
        DEVRM_PCL_T *pcl;

        DEVRM_VirtualGroup_T *vg;

        DEVRM_PCE_T *pce;

        UI32_T  pcl_id;
        UI32_T  idx;

        UI32_T  sum_of_pce_number;
        UI32_T  pce_number;

        sum_of_pce_number = 0;

        for (pcl_id = 1; pcl_id <= _countof(DEVRM_DEVICE_PTR(device_id)->pcls); ++pcl_id)
        {
            pcl = DEVRM_GetPcl(device_id, pcl_id);
            if (NULL == pcl)
            {
                continue;
            }

            pce_number = 0;

            for (pce = pcl->pce_list; NULL != pce; pce = pce->next)
            {
                ++pce_number;
            }

            assert(pce_number == pcl->pce_number);

            sum_of_pce_number += pce_number;

            for (idx=0; idx < _countof(pcl->vg_tbl); ++idx)
            {
                vg = &pcl->vg_tbl[idx];

                if (TRUE != vg->valid)
                {
                    continue;
                }

                for (pce = vg->rv.pce_list; vg->rv.last_pce != pce; pce = pce->next)
                {
                    assert(0 < pce_number);
                    --pce_number;
                }

                if (NULL != pce && vg->rv.last_pce == pce)
                {
                    assert(0 < pce_number);
                    --pce_number;
                }

            }

            assert(0 == pce_number);
        }

        for (idx=0; idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES; ++idx)
        {
            if (0 != (DEVRM_DEVICE_PTR(device_id)->pces_use_status[idx / 8] & (0X80 >> (idx%8))) )
            {
                assert(0 < sum_of_pce_number);
                --sum_of_pce_number;
            }
        }

        assert(0 == sum_of_pce_number);
    }
}

void
DEVRM_UT_Proc_Validate_Rule_Mapping_Table()
{
    UI32_T  device_id;

    for (device_id = 0; device_id < DEVRM_GetNumberOfChips(); ++ device_id)
    {
        DEVRM_PCL_T *pcl;

        DEVRM_PCE_T *pce;

        UI32_T  pcl_id;
        UI32_T  idx;

        UI8_T  *dup_pces_use_status = NULL;

        dup_pces_use_status = malloc(sizeof(DEVRM_DEVICE_PTR(device_id)->pces_use_status));
        if (NULL == dup_pces_use_status)
        {
            assert(0);
        }

        memcpy(dup_pces_use_status, DEVRM_DEVICE_PTR(device_id)->pces_use_status, sizeof(DEVRM_DEVICE_PTR(device_id)->pces_use_status));

        for (pcl_id = 1; pcl_id <= _countof(DEVRM_DEVICE_PTR(device_id)->pcls); ++pcl_id)
        {
            pcl = DEVRM_GetPcl(device_id, pcl_id);
            if (NULL == pcl)
            {
                continue;
            }

            for (pce = pcl->pce_list; NULL != pce; pce = pce->next)
            {
                assert(0 != ( dup_pces_use_status[ pce->hw_idx/8 ] & (0x80 >> (pce->hw_idx%8)) ));

                dup_pces_use_status[ pce->hw_idx/8 ] &= ~( 0x80 >> (pce->hw_idx%8) );
            }
        }

        for (idx=0; idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES; ++idx)
        {
            if (0 != (dup_pces_use_status[idx / 8] & (0X80 >> (idx%8))) )
            {
                assert(0);
            }
        }

        free(dup_pces_use_status);
    }
}

void
DEVRM_UT_Proc_Validate_Leak_Of_Rule()
{
    UI32_T device_id;

    for (device_id = 0; device_id < DEVRM_GetNumberOfChips(); ++ device_id)
    {
        GT_U32 tcam_index;
        DEVRM_PCE_T *pce;

        CPSS_PCL_RULE_ENTRY *dup_rule_tbl = malloc(sizeof( cpss_db[device_id].rule_tbl ));

        assert(NULL != dup_rule_tbl);

        memcpy(dup_rule_tbl, cpss_db[device_id].rule_tbl, sizeof(cpss_db[device_id].rule_tbl) );

        pce = NULL;
        while (DEVRM_GetNextPce(device_id, &pce))
        {
            CPSS_PCL_RULE_SIZE_ENT ruleSize = _cpssRuleFormatToRuleSize(pce->pce_type);
            GT_32 ruleIndex = DEVRM_Translate_PCEIDwithType(pce->pce_type, pce->hw_idx);
            tcam_index = _cpssRuleSizeIndexToTcamIndex(ruleSize, ruleIndex);
            CPSS_PCL_RULE_ENTRY *rule;

            assert( tcam_index < _countof( cpss_db[device_id].rule_tbl ) );

            rule = &dup_rule_tbl[ tcam_index ];

            if (GT_TRUE == rule->valid)
            {
                rule->valid = GT_FALSE;
            }
        }

        for (tcam_index = 0; tcam_index < _countof(cpss_db[device_id].rule_tbl); ++tcam_index)
        {
            CPSS_PCL_RULE_ENTRY *rule = &dup_rule_tbl[ tcam_index ];

            assert(GT_FALSE == rule->valid);
        }

        free(dup_rule_tbl);
    }
}

void
DEVRM_UT_Proc_Validate_Leak_Of_Policer_Metering_Counter()
{
    UI32_T device_id;

    for (device_id = 0; device_id < DEVRM_GetNumberOfChips(); ++ device_id)
    {
        UI32_T i;
        DEVRM_PCE_T *pce;
        CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage;

        I16_T *dup_policer_tbl = malloc( sizeof(DEVRM_DEVICE_PTR(device_id)->policer_use_status) );
        I16_T *dup_egr_policer_tbl = malloc( sizeof(DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status) );

        assert(NULL != dup_policer_tbl);
        assert(NULL != dup_egr_policer_tbl);

        memcpy(dup_policer_tbl, DEVRM_DEVICE_PTR(device_id)->policer_use_status, sizeof(DEVRM_DEVICE_PTR(device_id)->policer_use_status));
        memcpy(dup_egr_policer_tbl, DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status, sizeof(DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status));

        pce = NULL;
        while (DEVRM_GetNextPce(device_id, &pce))
        {
            I16_T   *tbl;

            if (-1 == pce->policer_idx)
            {
                continue;
            }

            DEVRM_GetPolicerStageFromPce(device_id, pce, &stage);
            if (stage == DEVRM_METER_POLICER_STAGE(devNum))
            {
                tbl = dup_policer_tbl;
            }
            else
            {
                tbl = dup_egr_policer_tbl;
            }

            assert( 0 < tbl[pce->policer_idx] );

            tbl[ pce->policer_idx ] -= 1;

            //
            // If one PCE have assigned a police index, it means it must have one
            // rule on SDK and this rule have a policer index point to a meter entry
            //
            {
                CPSS_PCL_RULE_SIZE_ENT ruleSize = _cpssRuleFormatToRuleSize(pce->pce_type);
                GT_32 ruleIndex = DEVRM_Translate_PCEIDwithType(pce->pce_type, pce->hw_idx);
                GT_32 tcam_index = _cpssRuleSizeIndexToTcamIndex(ruleSize, ruleIndex);
                CPSS_PCL_RULE_ENTRY *rule = &cpss_db[device_id].rule_tbl[tcam_index];
                CPSS_POLICER_METERING_ENTRY *meter = &cpss_db[device_id].metering_tbl[stage][pce->policer_idx];

                assert(GT_TRUE == rule->valid);
                assert(CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E != rule->action.policer.policerEnable);

                assert(-1 != pce->policer_idx);
                assert((GT_U32)pce->policer_idx == rule->action.policer.policerId);

                assert((GT_U32)pce->policer_idx == meter->entry.countingEntryIndex);
            }
        }

        for (i=0; i < _countof(DEVRM_DEVICE_PTR(device_id)->policer_use_status); ++i)
        {
            assert(0 == dup_policer_tbl[i]);
        }

        for (i=0; i < _countof(DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status); ++i)
        {
            assert(0 == dup_egr_policer_tbl[i]);
        }

        free(dup_policer_tbl);
        free(dup_egr_policer_tbl);
    }
}

void
DEVRM_UT_Proc_Validate_Leak_Of_Policer_QoS_Remark_Entry()
{
    UI32_T device_id;

    for (device_id = 0; device_id < DEVRM_GetNumberOfChips(); ++ device_id)
    {
        DEVRM_PCE_T *pce;

        UI8_T *dup_qos_remarking_tbl = malloc( sizeof(DEVRM_DEVICE_PTR(device_id)->qos_remarking_use_status) );

        assert(NULL != dup_qos_remarking_tbl);

        memcpy(dup_qos_remarking_tbl, DEVRM_DEVICE_PTR(device_id)->qos_remarking_use_status, sizeof(DEVRM_DEVICE_PTR(device_id)->qos_remarking_use_status));

        pce = NULL;
        while (DEVRM_GetNextPce(device_id, &pce))
        {
            I16_T i = pce->qos_remarking_idx - DEVRM_QOS_REMARKING_ENTRY_FIRST_IDX;

            if (-1 == pce->qos_remarking_idx)
            {
                continue;
            }

            assert( 0 != (dup_qos_remarking_tbl[i/8] & (0x80 >> (i%8))) );

            dup_qos_remarking_tbl[i/8] &= ~(0x80 >> (i%8));
        }

        {
            UI32_T i;

            for (i=0; i < _countof( DEVRM_DEVICE_PTR(device_id)->qos_remarking_use_status ); ++i)
            {
                assert( 0 == dup_qos_remarking_tbl[i] );
            }
        }

        free(dup_qos_remarking_tbl);
    }
}

void
DEVRM_UT_Proc_Validate_Leaks()
{
    DEVRM_UT_Proc_Validate_Leak_Of_Rule();
    DEVRM_UT_Proc_Validate_Leak_Of_Policer_Metering_Counter();
    DEVRM_UT_Proc_Validate_Leak_Of_Policer_QoS_Remark_Entry();
}

void
DEVRM_UT_Proc_Validate_PCE_Status()
{
    DEVRM_UT_Proc_Validate_Order_Of_Rule();
    DEVRM_UT_Proc_Validate_Rule_Id();
    DEVRM_UT_Proc_Validate_Number_Of_Rule();
    DEVRM_UT_Proc_Validate_Rule_Mapping_Table();

    DEVRM_UT_Proc_Validate_Leaks();
}

void
DEVRM_UT_Proc_Validate_PCE_Status_Complete()
{
    DEVRM_UT_Proc_Validate_Order_Of_Rule_Slow();

    DEVRM_UT_Proc_Validate_PCE_Status();
}

void
DEVRM_UT_Proc_Validate_IsPclConfigTableEnable(
    BOOL_T  is_enable,
    BOOL_T  is_trunk,    /* FALSE mean unit port */
    UI32_T  device_id,
    UI32_T  port,
    UI32_T  pcl_id)
{
    DEVRM_PCL_T *pcl;

    GT_STATUS rc = GT_OK;

    CPSS_PCL_DIRECTION_ENT          direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum;

    CPSS_INTERFACE_INFO_STC         interfaceInfo;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfg;

    UI32_T hw_dev_id;
    UI32_T my_unit_id;

    DEVRM_CHECK_DEVICE_ID(device_id);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    assert(NULL != pcl);

    direction = pcl->direction;
    lookupNum = (DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ||
                 DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num) ? CPSS_PCL_LOOKUP_0_E :
    (DEVRM_PCL_LOOKUP_1   == pcl->lookup_num) ? CPSS_PCL_LOOKUP_1_E : 0xCCCCCCCC;

    assert(STKTPLG_POM_GetMyUnitID(&my_unit_id) != FALSE);

    assert(DEV_SWDRV_GetModuleIdFromLocalDeviceId(my_unit_id, device_id, &hw_dev_id) != FALSE);


    if (is_trunk== TRUE)
    {
        interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        interfaceInfo.trunkId = port;
    }
    else
    {
        interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        interfaceInfo.devPort.devNum = hw_dev_id;
        interfaceInfo.devPort.portNum = port;
    }

    interfaceInfo.devNum = hw_dev_id;

    lookupCfg.enableLookup = GT_FALSE;
    lookupCfg.dualLookup   = GT_FALSE;
    if (DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num)
    {
        lookupCfg.pclId = pcl_id;
    }
    else
    {
        lookupCfg.pclIdL01 = pcl_id;
    }

    rc = cpssDxChPclCfgTblGet(device_id,
                              &interfaceInfo,
                              direction,
                              lookupNum,
                              &lookupCfg);
    assert(rc == GT_OK);

    assert(lookupCfg.enableLookup == is_enable);

    if (DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num)
    {
        assert(lookupCfg.dualLookup == is_enable);
    }

    return;
}

/* -----------------------------------------------------------------------------
 * Test Functions
 * -----------------------------------------------------------------------------
 */

int
DEVRM_UT_Setup()
{
    _cpssInit();
    DEVRM_InitiateSystemResources();

    return 0;
}

int
DEVRM_UT_Teardown()
{
    DEVRM_FreeSystemResource();
    return 0;
}

int
DEVRM_UT_Test_Validate_Function_If_Failed_Is_Right()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id  = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI32_T pcl_id;
        UI32_T vir_group_id;
        int    priority;
        BOOL_T is_std_pce;
        UI32_T hw_idx;
    };

    struct setting_pcl setting[] =
    {
        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {8,  8, 5,  1},
                {8, 26, 5,  2},

                {0},
            },
        },
        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {8, 24, 5,  3},
                {8, 28, 5,  4},

                {0},
            }
        },
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);
        }
    }

    DEVRM_UT_Proc_Validate_Order_Of_Rule_Slow();

    return 0;
}

/** DESCRIPTION:
 **   Must create PCL before add PCE on it.
 **/
int
DEVRM_UT_AllocatePce_Can_Not_Allocate_PCE_On_Non_Exist_PCL()
{
    UI32_T  device_id  = 0;
    UI32_T  pcl_id     = 5;
    UI32_T  pce_type   = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
    BOOL_T  result;

    DEVRM_PCE_T *pce[DEVRM_MAX_NBR_OF_TCAM_ENTRIES];

    memset(pce, 0, sizeof(pce));

    result = DEVRM_AllocatePce(device_id, pcl_id, pce_type, &pce[0]);

    assert(result == FALSE);

    return 0;
}

int
DEVRM_UT_AllocatePce_FreePce_Ext_Global_Rule()
{
    enum
    {
        DEVRM_PCL_ID = 2
    };

    UI32_T  device_id  = 0;
    UI32_T  pcl_id     = DEVRM_PCL_ID;
    BOOL_T  is_ingress = TRUE;
    UI32_T  pce_type   = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
    BOOL_T  is_std_pce = DEVRM_GetRuleSize(pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;
    BOOL_T  result;

    UI32_T  rule_id    = 1;

    result = DEVRM_CreatePcl(device_id, pcl_id, is_ingress, DEVRM_PCL_LOOKUP_0_0, pce_type);
    assert(result == TRUE);

    // sample test
    {
        DEVRM_PCE_T *pce[DEVRM_MAX_NBR_OF_TCAM_ENTRIES];

        memset(pce, 0, sizeof(pce));

        result = DEVRM_AllocatePce(device_id, pcl_id, pce_type, &pce[0]);

        assert(result == TRUE);

        pce[0]->upper_rule_id = rule_id ++;

        assert(DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id-1].pce_number == 2);
        assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[0] == 0xc0);

        result = DEVRM_FreePce(device_id, pcl_id, is_std_pce, pce[0]);

        assert(result == TRUE);

        assert(DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id-1].pce_number == 0);
        assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[0] == 0x00);


        // allocate some rule ..., then free they
        result = DEVRM_AllocatePce(device_id, pcl_id, pce_type, &pce[0]);

        assert(result == TRUE);

        pce[0]->upper_rule_id = rule_id ++;

        assert(DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id-1].pce_number == 2);
        assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[0] == 0xc0);

        result = DEVRM_AllocatePce(device_id, pcl_id, pce_type, &pce[1]);

        assert(result == TRUE);

        pce[1]->upper_rule_id = rule_id ++;

        assert(DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id-1].pce_number == 4);
        assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[0] == 0xf0);

        result = DEVRM_AllocatePce(device_id, pcl_id, pce_type, &pce[2]);

        assert(result == TRUE);

        pce[2]->upper_rule_id = rule_id ++;

        assert(DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id-1].pce_number == 6);
        assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[0] == 0xfc);

        result = DEVRM_FreePce(device_id, pcl_id, is_std_pce, pce[0]);

        assert(result == TRUE);

        assert(DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id-1].pce_number == 4);
        assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[0] == 0x3c);

        result = DEVRM_FreePce(device_id, pcl_id, is_std_pce, pce[1]);

        assert(result == TRUE);

        assert(DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id-1].pce_number == 2);
        assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[0] == 0x0c);

        result = DEVRM_FreePce(device_id, pcl_id, is_std_pce, pce[2]);

        assert(result == TRUE);

        assert(DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id-1].pce_number == 0);
        assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[0] == 0x00);
    }

    // capability test
    {
        UI32_T i = 0;
        UI32_T count = 0;
        DEVRM_PCE_T *pce[DEVRM_MAX_NBR_OF_TCAM_ENTRIES];
        UI32_T pce_number;

        memset(pce, 0, sizeof(pce));
        pce_number = 0;

        while (1)
        {
            assert( i < _countof(pce) );

            result = DEVRM_AllocatePce(device_id, pcl_id, pce_type, &pce[i]);

            if (result == FALSE)
                break;

            pce[i]->upper_rule_id = rule_id ++;

            pce_number += 2;

            assert(DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id-1].pce_number == pce_number);

            switch (i%4) {
                case 0:
                    assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[pce[i]->hw_idx/8] == 0xc0);
                    break;
                case 1:
                    assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[pce[i]->hw_idx/8] == 0xf0);
                    break;
                case 2:
                    assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[pce[i]->hw_idx/8] == 0xfc);
                    break;
                case 3:
                    assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[pce[i]->hw_idx/8] == 0xff);
                    break;
            }

            i ++;
            count ++;
        }

        assert(count == DEVRM_MAX_NBR_OF_EXT_RULE);

        // clear all
        for (i=0; i < _countof(pce); ++i)
        {
            if (pce[i])
            {
                result = DEVRM_FreePce(device_id, pcl_id, is_std_pce, pce[i]);
                assert(result == TRUE);
            }
        }

        assert(DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id-1].pce_number == 0);

        for (i=0; i < _countof(DEVRM_DEVICE_PTR(device_id)->pces_use_status); ++i)
        {
            assert(DEVRM_DEVICE_PTR(device_id)->pces_use_status[i] == 0x00);
        }
    }

    return 0;
}

int
DEVRM_UT_GetPce_Capacity_Test_Standard_PCE_1_PCE_1_PCL()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        BOOL_T is_std_pce;
        DEVRM_PCE_T *pce[DEVRM_MAX_NBR_OF_TCAM_ENTRIES];
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI32_T pcl_id;

    DEVRM_PCE_T *pool[DEVRM_MAX_NBR_OF_TCAM_ENTRIES] = {0};
    UI32_T pi = 0;

    BOOL_T result;

    for (pcl_id = 1; pcl_id < DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        struct VARIABLES pcl;
        DEVRM_PCE_T *pce;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        pcl.is_std_pce = DEVRM_GetRuleSize(pcl.pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce[pcl.i]);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce[pcl.i]->upper_rule_id = pcl_id<<16 | 1;

        result = DEVRM_GetPce(device_id, pcl.pce[pcl.i]->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce[pcl.i]);

        pool[pi++] = pce;
    }

    for (pi = 0; pi < _countof(pool); pi++)
    {
        if (pool[pi])
        {
            result = DEVRM_FreePce(device_id,
                                   pool[pi]->pcl_id,
                                   pool[pi]->is_std_pce,
                                   pool[pi]);
            assert(result == TRUE);
        }
    }

    return 0;
}

int
DEVRM_UT_GetPce_Capacity_Test_Standard_PCE_2_PCE_1_PCL()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        BOOL_T is_std_pce;
        DEVRM_PCE_T *pce[DEVRM_MAX_NBR_OF_TCAM_ENTRIES];
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI32_T pcl_id;

    DEVRM_PCE_T *pool[DEVRM_MAX_NBR_OF_TCAM_ENTRIES] = {0};
    UI32_T pi = 0;

    BOOL_T result;

    for (pcl_id = 1; pcl_id < DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        struct VARIABLES pcl;
        DEVRM_PCE_T *pce;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        pcl.is_std_pce = DEVRM_GetRuleSize(pcl.pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce[pcl.i]);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce[pcl.i]->upper_rule_id = pcl.pcl_id<<16 | pcl.i;

        result = DEVRM_GetPce(device_id, pcl.pce[pcl.i]->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce[pcl.i]);

        pcl.i ++;

        pool[pi++] = pce;

        // 2nd PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce[pcl.i]);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce[pcl.i]->upper_rule_id = pcl.pcl_id<<16 | pcl.i;

        result = DEVRM_GetPce(device_id, pcl.pce[pcl.i]->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce[pcl.i]);

        pool[pi++] = pce;
    }

    for (pi = 0; pi < _countof(pool); pi++)
    {
        if (pool[pi])
        {
            result = DEVRM_FreePce(device_id,
                                   pool[pi]->pcl_id,
                                   pool[pi]->is_std_pce,
                                   pool[pi]);
            assert(result == TRUE);
        }
    }

    return 0;
}

int
DEVRM_UT_GetNextPce_For_Standard_Rule()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        BOOL_T is_std_pce;
        DEVRM_PCE_T *pce;
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI32_T pcl_id;

    DEVRM_PCE_T *pce;

    DEVRM_PCE_T *pool[DEVRM_MAX_NBR_OF_TCAM_ENTRIES] = {0};
    UI32_T pi = 0;

    BOOL_T result;

    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        struct VARIABLES pcl;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        pcl.is_std_pce = DEVRM_GetRuleSize(pcl.pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;


        // 2nd PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;
    }

    // make sure we have created PCE
    assert(pi != 0);

    pce = NULL;
    pi = 0;
    while (TRUE == DEVRM_GetNextPce(device_id, &pce))
    {
        assert(pce == pool[pi]);

        pi++;
    }

    for (pi = 0; pi < _countof(pool); pi++)
    {
        if (pool[pi])
        {
            result = DEVRM_FreePce(device_id,
                                   pool[pi]->pcl_id,
                                   pool[pi]->is_std_pce,
                                   pool[pi]);
            assert(result == TRUE);
        }
    }

    return 0;
}

int
DEVRM_UT_GetNextPce_For_Extended_Rule()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        BOOL_T is_std_pce;
        DEVRM_PCE_T *pce;
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI32_T pcl_id;

    DEVRM_PCE_T *pce;

    DEVRM_PCE_T *pool[DEVRM_MAX_NBR_OF_TCAM_ENTRIES] = {0};
    UI32_T pi = 0;

    BOOL_T result;

    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        struct VARIABLES pcl;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
        pcl.is_std_pce = DEVRM_GetRuleSize(pcl.pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;


        // 2nd PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;
    }

    // make sure we have created PCE
    assert(pi != 0);

    pce = NULL;
    pi = 0;
    while (TRUE == DEVRM_GetNextPce(device_id, &pce))
    {
        assert(pce == pool[pi]);

        pi++;
    }

    for (pi = 0; pi < _countof(pool); pi++)
    {
        if (pool[pi])
        {
            result = DEVRM_FreePce(device_id,
                                   pool[pi]->pcl_id,
                                   pool[pi]->is_std_pce,
                                   pool[pi]);
            assert(result == TRUE);
        }
    }

    return 0;
}

int
DEVRM_UT_GetPrevious_For_Standard_Rule()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        BOOL_T is_std_pce;
        DEVRM_PCE_T *pce;
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI32_T pcl_id;

    DEVRM_PCE_T *pce;

    DEVRM_PCE_T *pool[DEVRM_MAX_NBR_OF_TCAM_ENTRIES] = {0};
    UI32_T pi = 0;

    BOOL_T result;

    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        struct VARIABLES pcl;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        pcl.is_std_pce = DEVRM_GetRuleSize(pcl.pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;


        // 2nd PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;
    }

    // make sure we have created PCE
    assert(pi != 0);

    pce = NULL;
    pi --;  // back one position
    while (TRUE == DEVRM_GetPreviousPce(device_id, &pce))
    {
        assert(pce == pool[pi]);

        if (pi == 0)
            break; // if break here, we must call testee once again!

        pi--;
    }

    assert(FALSE == DEVRM_GetPreviousPce(device_id, &pce));
    assert(pi == 0);

    for (pi = 0; pi < _countof(pool); pi++)
    {
        if (pool[pi])
        {
            result = DEVRM_FreePce(device_id,
                                   pool[pi]->pcl_id,
                                   pool[pi]->is_std_pce,
                                   pool[pi]);
            assert(result == TRUE);
        }
    }

    return 0;
}

int
DEVRM_UT_GetPreviousPce_Extended_1_PCE()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        BOOL_T is_std_pce;
        DEVRM_PCE_T *pce;
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI32_T pcl_id;

    DEVRM_PCE_T *pce;

    DEVRM_PCE_T *pool[DEVRM_MAX_NBR_OF_TCAM_ENTRIES] = {0};
    UI32_T pi = 0;

    BOOL_T result;

    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        struct VARIABLES pcl;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
        pcl.is_std_pce = DEVRM_GetRuleSize(pcl.pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;
    }

    // make sure we have created PCE
    assert(pi != 0);

    pce = NULL;
    pi --;  // back one position
    while (TRUE == DEVRM_GetPreviousPce(device_id, &pce))
    {
        assert(pce == pool[pi]);

        if (pi == 0)
            break; // if break here, we must call testee once again!

        pi--;
    }

    assert(FALSE == DEVRM_GetPreviousPce(device_id, &pce));
    assert(pi == 0);

    for (pi = 0; pi < _countof(pool); pi++)
    {
        if (pool[pi])
        {
            result = DEVRM_FreePce(device_id,
                                   pool[pi]->pcl_id,
                                   pool[pi]->is_std_pce,
                                   pool[pi]);
            assert(result == TRUE);
        }
    }

    return 0;
}

int
DEVRM_UT_GetPreviousPce_Extended_2_PCE()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        BOOL_T is_std_pce;
        DEVRM_PCE_T *pce;
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI32_T pcl_id;

    DEVRM_PCE_T *pce;

    DEVRM_PCE_T *pool[DEVRM_MAX_NBR_OF_TCAM_ENTRIES] = {0};
    UI32_T pi = 0;

    BOOL_T result;

    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        struct VARIABLES pcl;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
        pcl.is_std_pce = DEVRM_GetRuleSize(pcl.pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;


        // 2nd PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;
    }

    // make sure we have created PCE
    assert(pi != 0);

    pce = NULL;
    pi --;  // back one position
    while (TRUE == DEVRM_GetPreviousPce(device_id, &pce))
    {
        assert(pce == pool[pi]);

        if (pi == 0)
            break; // if break here, we must call testee once again!

        pi--;
    }

    assert(FALSE == DEVRM_GetPreviousPce(device_id, &pce));
    assert(pi == 0);

    for (pi = 0; pi < _countof(pool); pi++)
    {
        if (pool[pi])
        {
            result = DEVRM_FreePce(device_id,
                                   pool[pi]->pcl_id,
                                   pool[pi]->is_std_pce,
                                   pool[pi]);
            assert(result == TRUE);
        }
    }

    return 0;
}

int
DEVRM_UT_GetPreviousPce_Extended_3_PCE()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        BOOL_T is_std_pce;
        DEVRM_PCE_T *pce;
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI32_T pcl_id;

    DEVRM_PCE_T *pce;

    DEVRM_PCE_T *pool[DEVRM_MAX_NBR_OF_TCAM_ENTRIES] = {0};
    UI32_T pi = 0;

    BOOL_T result;

    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; pcl_id += 3)
    {
        struct VARIABLES pcl;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
        pcl.is_std_pce = DEVRM_GetRuleSize(pcl.pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;


        // 2nd PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;


        // 3th PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;
    }

    // make sure we have created PCE
    assert(pi != 0);

    pce = NULL;
    pi --;  // back one position
    while (TRUE == DEVRM_GetPreviousPce(device_id, &pce))
    {
        assert(pce == pool[pi]);

        if (pi == 0)
            break; // if break here, we must call testee once again!

        pi--;
    }

    assert(FALSE == DEVRM_GetPreviousPce(device_id, &pce));
    assert(pi == 0);

    for (pi = 0; pi < _countof(pool); pi++)
    {
        if (pool[pi])
        {
            result = DEVRM_FreePce(device_id,
                                   pool[pi]->pcl_id,
                                   pool[pi]->is_std_pce,
                                   pool[pi]);
            assert(result == TRUE);
        }
    }

    return 0;
}

int
DEVRM_UT_GetPceAtHwIndex_Full_of_Standard_PCE()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        BOOL_T is_std_pce;
        DEVRM_PCE_T *pce;
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI16_T hw_idx;

    DEVRM_PCE_T *pce;

    DEVRM_PCE_T *pool[DEVRM_MAX_NBR_OF_TCAM_ENTRIES] = {0};
    UI32_T pi = 0;

    BOOL_T result;
    UI32_T pcl_id;

    for (hw_idx=0; hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES; ++hw_idx)
    {
        result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
        assert(result == FALSE);
    }

    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        struct VARIABLES pcl;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        pcl.is_std_pce = DEVRM_GetRuleSize(pcl.pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;
    }

    for (hw_idx=0; hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES; ++hw_idx)
    {
        result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
        assert(result == TRUE);

        assert(hw_idx == pce->hw_idx);
        assert(pce->upper_rule_id != 0);
    }

    for (pi = 0; pi < _countof(pool); pi++)
    {
        if (pool[pi])
        {
            result = DEVRM_FreePce(device_id,
                                   pool[pi]->pcl_id,
                                   pool[pi]->is_std_pce,
                                   pool[pi]);
            assert(result == TRUE);
        }
    }

    return 0;
}

int
DEVRM_UT_GetPceAtHwIndex_Full_of_Extended_PCE()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        BOOL_T is_std_pce;
        DEVRM_PCE_T *pce;
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI16_T hw_idx;

    DEVRM_PCE_T *pce;

    DEVRM_PCE_T *pool[DEVRM_MAX_NBR_OF_TCAM_ENTRIES] = {0};
    UI32_T pi = 0;

    BOOL_T result;
    UI32_T pcl_id;

    for (hw_idx=0; hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES; ++hw_idx)
    {
        result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
        assert(result == FALSE);
    }

    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        struct VARIABLES pcl;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
        pcl.is_std_pce = DEVRM_GetRuleSize(pcl.pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;
    }

    assert(pi != 0);

    pi = 0;

    for (hw_idx=0; hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES; ++hw_idx)
    {
        result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
        assert(result == TRUE);

        assert(pce->upper_rule_id != 0);
        assert(pool[pi] == pce);

        ++hw_idx;
        if (hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES)
        {
            result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
            assert(result == TRUE);

            assert(pce->upper_rule_id != 0);
            assert(pool[pi] == pce);
        }

        pi++;
    }

    assert(pi != 0);

    for (pi = 0; pi < _countof(pool); pi++)
    {
        if (pool[pi])
        {
            result = DEVRM_FreePce(device_id,
                                   pool[pi]->pcl_id,
                                   pool[pi]->is_std_pce,
                                   pool[pi]);
            assert(result == TRUE);
        }
    }

    return 0;
}

int
DEVRM_UT_GetPceAtHwIndex_Mixed_PCE_1_Extended_2_Standard()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        DEVRM_PCE_T *pce;
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI16_T hw_idx;

    DEVRM_PCE_T *pce;

    DEVRM_PCE_T *pool[DEVRM_MAX_NBR_OF_TCAM_ENTRIES] = {0};
    UI32_T pi = 0;

    BOOL_T result;
    UI32_T pcl_id;

    for (hw_idx=0; hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES; ++hw_idx)
    {
        result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
        assert(result == FALSE);
    }

    // 1 extended + 2 standard
    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        struct VARIABLES pcl;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;

        // 2nd
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;

        // 3th
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;
    }

    assert(pi != 0);

    pi = 0;

    for (hw_idx=0; hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES; ++hw_idx)
    {
        result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
        assert(result == TRUE);

        assert(pce->upper_rule_id != 0);
        assert(pool[pi] == pce);

        ++hw_idx;
        if (hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES)
        {
            result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
            assert(result == TRUE);

            assert(pce->upper_rule_id != 0);
            assert(pool[pi] == pce);
        }

        pi++;

        ++hw_idx;
        if (hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES)
        {
            result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
            assert(result == TRUE);

            assert(pce->upper_rule_id != 0);
            assert(pool[pi] == pce);
        }

        pi++;

        ++hw_idx;
        if (hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES)
        {
            result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
            assert(result == TRUE);

            assert(pce->upper_rule_id != 0);
            assert(pool[pi] == pce);
        }

        pi++;

    }

    assert(pi != 0);

    for (pi = 0; pi < _countof(pool); pi++)
    {
        if (pool[pi])
        {
            result = DEVRM_FreePce(device_id,
                                   pool[pi]->pcl_id,
                                   pool[pi]->is_std_pce,
                                   pool[pi]);
            assert(result == TRUE);
        }
    }

    return 0;
}

int
DEVRM_UT_GetPceAtHwIndex_Mixed_PCE_1_Standard_1_Extended()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        DEVRM_PCE_T *pce;
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI16_T hw_idx;

    struct VARIABLES pcl;

    DEVRM_PCE_T *pce;

    DEVRM_PCE_T *pool[DEVRM_MAX_NBR_OF_TCAM_ENTRIES] = {0};
    UI32_T pi = 0;

    BOOL_T result;

    for (hw_idx=0; hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES; ++hw_idx)
    {
        result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
        assert(result == FALSE);
    }

    memset(&pcl, 0, sizeof(pcl));

    pcl.pcl_id = 2;
    pcl.is_ingress = TRUE;
    pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;

    result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
    assert(result == TRUE);

    while (1)
    {
        // 1st PCE
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;

        // 2nd
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;

        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce->upper_rule_id = pcl.pcl_id<<16 | pcl.i++;

        result = DEVRM_GetPce(device_id, pcl.pce->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce);

        pool[pi++] = pce;
    }

    assert(pi != 0);

    pi = 0;

    for (hw_idx=0; hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES; ++hw_idx)
    {
        result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
        assert(result == TRUE);

        assert(pce->upper_rule_id != 0);
        assert(pool[pi] == pce);

        pi++;

        ++hw_idx;
        if (hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES)
        {
            result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
            assert(result == FALSE);
        }

        ++hw_idx;
        if (hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES)
        {
            result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
            assert(result == TRUE);

            assert(pce->upper_rule_id != 0);
            assert(pool[pi] == pce);
        }

        ++hw_idx;
        if (hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES)
        {
            result = DEVRM_GetPceAtHwIndex(device_id, hw_idx, &pce);
            assert(result == TRUE);

            assert(pce->upper_rule_id != 0);
            assert(pool[pi] == pce);
        }

        pi++;
    }

    assert(pi != 0);

    for (pi = 0; pi < _countof(pool); pi++)
    {
        if (pool[pi])
        {
            result = DEVRM_FreePce(device_id,
                                   pool[pi]->pcl_id,
                                   pool[pi]->is_std_pce,
                                   pool[pi]);
            assert(result == TRUE);
        }
    }

    return 0;
}

int
DEVRM_UT_DestroyAllAcl_For_Standard_Rule()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        BOOL_T is_std_pce;
        DEVRM_PCE_T *pce[DEVRM_MAX_NBR_OF_TCAM_ENTRIES];
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI32_T pcl_id;

    UI32_T pi = 0;

    BOOL_T result;

    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        struct VARIABLES pcl;
        DEVRM_PCE_T *pce;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        pcl.is_std_pce = DEVRM_GetRuleSize(pcl.pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce[pcl.i]);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce[pcl.i]->upper_rule_id = pcl_id<<16 | 1;

        result = DEVRM_GetPce(device_id, pcl.pce[pcl.i]->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce[pcl.i]);

        pi++;
    }

    assert(pi != 0);

    DEVRM_DestroyAllAcl(device_id);

    for (pi = 0; pi < _countof(DEVRM_DEVICE_PTR(device_id)->pcls); pi++)
    {
        DEVRM_PCL_T *pcl = &DEVRM_DEVICE_PTR(device_id)->pcls[pi];

//        if ((pi+1) <= DEVRM_PCL_ID_RESERVED_BASE)
//        {
            assert( pcl->is_used == FALSE );
//        }

        assert( pcl->pce_list == NULL );
        assert( pcl->last_pce == NULL );
        assert( pcl->pce_number == 0 );
    }

    return 0;
}

int
DEVRM_UT_DestroyAllAcl_For_Extended_Rule()
{
    struct VARIABLES
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;
        BOOL_T is_std_pce;
        DEVRM_PCE_T *pce[DEVRM_MAX_NBR_OF_TCAM_ENTRIES];
        UI32_T i;
    };

    UI32_T device_id = 0;
    UI32_T pcl_id;

    UI32_T pi = 0;

    BOOL_T result;

    for (pcl_id = 1; pcl_id < DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        struct VARIABLES pcl;
        DEVRM_PCE_T *pce;

        memset(&pcl, 0, sizeof(pcl));

        pcl.pcl_id = pcl_id;
        pcl.is_ingress = TRUE;
        pcl.pce_type = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
        pcl.is_std_pce = DEVRM_GetRuleSize(pcl.pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

        result = DEVRM_CreatePcl(device_id, pcl.pcl_id, pcl.is_ingress, DEVRM_PCL_LOOKUP_0_0, pcl.pce_type);
        assert(result == TRUE);

        // 1st PCE
        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce[pcl.i]);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce[pcl.i]->upper_rule_id = pcl_id<<16 | 1;

        result = DEVRM_GetPce(device_id, pcl.pce[pcl.i]->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce[pcl.i]);

        // 2nd PCE
        ++pcl.i;

        result = DEVRM_AllocatePce(device_id, pcl.pcl_id, pcl.pce_type, &pcl.pce[pcl.i]);
        if (result == FALSE)
        {
            break;
        }

        pcl.pce[pcl.i]->upper_rule_id = pcl_id<<16 |2;

        result = DEVRM_GetPce(device_id, pcl.pce[pcl.i]->upper_rule_id, &pce);
        assert(result == TRUE);

        assert(pce == pcl.pce[pcl.i]);

        pi++;
    }

    assert(pi != 0);

    DEVRM_DestroyAllAcl(device_id);

    for (pi = 0; pi < _countof(DEVRM_DEVICE_PTR(device_id)->pcls); pi++)
    {
        DEVRM_PCL_T *pcl = &DEVRM_DEVICE_PTR(device_id)->pcls[pi];

        assert( pcl->is_used == FALSE );
        assert( pcl->pce_list == NULL );
        assert( pcl->last_pce == NULL );
        assert( pcl->pce_number == 0 );
    }

    return 0;
}

int
DEVRM_UT_InsertVirtualGroup()
{
    UI32_T  pcl_id;
    BOOL_T  is_ingress = TRUE;
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num = DEVRM_PCL_LOOKUP_0_0;
    UI32_T  pce_type   = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;

    UI32_T  device_id = 0;
    UI32_T  vir_group_id;
    UI32_T  idx;
    BOOL_T  result;

    pcl_id = 1;

    result = DEVRM_CreatePcl(device_id, pcl_id, is_ingress, lookup_num, pce_type);
    assert(result == TRUE);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    assert(pcl != FALSE);

    for (idx=0; idx < _countof(pcl->vg_tbl); ++idx)
    {
        assert(pcl->vg_tbl[idx].valid == FALSE);
    }

    /* 1
     */
    vir_group_id = 1;
    vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(vg == &pcl->vg_tbl[0]);

    assert(vg->valid == TRUE);
    assert(vg->vir_group_id == vir_group_id);
    assert(vg->pri_lvl == (int)vir_group_id);

    assert(pcl->vg_tbl[1].valid == FALSE);

    /* 5 -> 1
     */
    vir_group_id = 5;
    vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(vg == &pcl->vg_tbl[0]);

    assert(vg->valid == TRUE);
    assert(vg->vir_group_id == vir_group_id);
    assert(vg->pri_lvl == (int)vir_group_id);

    assert(pcl->vg_tbl[1].valid == TRUE);
    assert(pcl->vg_tbl[2].valid == FALSE);

    /* 5 -> 3 -> 1
     */
    vir_group_id = 3;
    vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(vg == &pcl->vg_tbl[1]);

    assert(vg->valid == TRUE);
    assert(vg->vir_group_id == vir_group_id);
    assert(vg->pri_lvl == (int)vir_group_id);

    assert(pcl->vg_tbl[0].valid == TRUE);
    assert(pcl->vg_tbl[2].valid == TRUE);
    assert(pcl->vg_tbl[3].valid == FALSE);


    pcl_id = 2;

    result = DEVRM_CreatePcl(device_id, pcl_id, is_ingress, lookup_num, pce_type);
    assert(result == TRUE);
    pcl = DEVRM_GetPcl(device_id, pcl_id);
    assert(pcl != FALSE);

    for (idx=0; idx < _countof(pcl->vg_tbl); ++idx)
    {
        assert(pcl->vg_tbl[idx].valid == FALSE);
    }

    for (vir_group_id=DEVRM_MAX_NBR_OF_VIR_GROUP; 0 < vir_group_id;
                                                                --vir_group_id)
    {
        vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, vir_group_id);
        assert(NULL != vg);

        /* Insert twice should not effect the result
         */
        vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, vir_group_id);
        assert(NULL != vg);
    }

    vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, 0);
    assert(NULL == vg);


    for (vir_group_id = DEVRM_MAX_NBR_OF_VIR_GROUP, idx=0;
                                        0 < vir_group_id; --vir_group_id, ++idx)
    {
        vg = DEVRM_FindVirtualGroup(device_id, pcl_id, vir_group_id);
        assert(vg);

        assert(vg == &pcl->vg_tbl[idx]);
    }


    pcl_id = 3;

    result = DEVRM_CreatePcl(device_id, pcl_id, is_ingress, lookup_num, pce_type);
    assert(result == TRUE);
    pcl = DEVRM_GetPcl(device_id, pcl_id);
    assert(pcl != FALSE);

    for (idx=0; idx < _countof(pcl->vg_tbl); ++idx)
    {
        assert(pcl->vg_tbl[idx].valid == FALSE);
    }

    for (vir_group_id=0; vir_group_id < DEVRM_MAX_NBR_OF_VIR_GROUP;
                                                                ++vir_group_id)
    {
        vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, vir_group_id);
        assert(NULL != vg);

        /* Insert twice should not effect the result
         */
        vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, vir_group_id);
        assert(NULL != vg);
    }

    vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, DEVRM_MAX_NBR_OF_VIR_GROUP);
    assert(NULL == vg);

    for (vir_group_id = DEVRM_MAX_NBR_OF_VIR_GROUP-1, idx=0;
                                0 <= (int)vir_group_id; --vir_group_id, ++idx)
    {
        vg = DEVRM_FindVirtualGroup(device_id, pcl_id, vir_group_id);
        assert(vg);

        assert(vg == &pcl->vg_tbl[idx]);
    }

    return 0;
}

int
DEVRM_UT_FindPreviousVirtualGroup_Case_By_Case_1()
{
    UI32_T  pcl_id;
    BOOL_T  is_ingress = TRUE;
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num = DEVRM_PCL_LOOKUP_0_0;
    UI32_T  pce_type   = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_VirtualGroup_T *prev_vg;

    UI32_T  device_id = 0;
    UI32_T  vir_group_id;
    UI32_T  idx;
    BOOL_T  result;

    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        result = DEVRM_CreatePcl(device_id, pcl_id, is_ingress, lookup_num, pce_type);
        assert(result == TRUE);

        pcl = DEVRM_GetPcl(device_id, pcl_id);
        assert(pcl != FALSE);

        for (idx=0; idx < _countof(pcl->vg_tbl); ++idx)
        {
            assert(pcl->vg_tbl[idx].valid == FALSE);
        }

        for (vir_group_id = DEVRM_MAX_NBR_OF_VIR_GROUP; 0 < vir_group_id;
                                                                --vir_group_id)
        {
            vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, vir_group_id);
            assert(NULL != vg);
            assert(vg->vir_group_id == vir_group_id);
        }
    }

    pcl_id = 1;
    vir_group_id = DEVRM_MAX_NBR_OF_VIR_GROUP;

    prev_vg = DEVRM_FindPreviousVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(NULL == prev_vg);

    pcl_id = 1;
    vir_group_id = DEVRM_MAX_NBR_OF_VIR_GROUP-1;

    prev_vg = DEVRM_FindPreviousVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(NULL != prev_vg);
    assert(DEVRM_MAX_NBR_OF_VIR_GROUP == prev_vg->vir_group_id);

    pcl_id = 1;
    vir_group_id = 1;

    prev_vg = DEVRM_FindPreviousVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(NULL != prev_vg);
    assert(2 == prev_vg->vir_group_id);

    pcl_id = 2;
    vir_group_id = DEVRM_MAX_NBR_OF_VIR_GROUP;

    prev_vg = DEVRM_FindPreviousVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(NULL != prev_vg);
    assert(1 == prev_vg->vir_group_id);

    return 0;
}

int
DEVRM_UT_FindPreviousVirtualGroup_Case_By_Case_2()
{
    UI32_T  pcl_id;
    BOOL_T  is_ingress = TRUE;
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num = DEVRM_PCL_LOOKUP_0_0;
    UI32_T  pce_type   = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_VirtualGroup_T *prev_vg;

    UI32_T  device_id = 0;
    UI32_T  vir_group_id;
    UI32_T  idx;
    BOOL_T  result;

    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; pcl_id += 2)
    {
        result = DEVRM_CreatePcl(device_id, pcl_id, is_ingress, lookup_num, pce_type);
        assert(result == TRUE);

        pcl = DEVRM_GetPcl(device_id, pcl_id);
        assert(pcl != FALSE);

        for (idx=0; idx < _countof(pcl->vg_tbl); ++idx)
        {
            assert(pcl->vg_tbl[idx].valid == FALSE);
        }

        for (vir_group_id = DEVRM_MAX_NBR_OF_VIR_GROUP; 0 < vir_group_id;
                                                                 --vir_group_id)
        {
            if (1 == (vir_group_id % 2))
            {
                continue;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, vir_group_id);
            assert(NULL != vg);
            assert(vg->vir_group_id == vir_group_id);
        }
    }

    pcl_id = 3;
    vir_group_id = 4;

    prev_vg = DEVRM_FindPreviousVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(NULL != prev_vg);
    assert(3 == prev_vg->pcl_id);
    assert(6 == prev_vg->vir_group_id);

    pcl_id = 3;
    vir_group_id = DEVRM_MAX_NBR_OF_VIR_GROUP;

    prev_vg = DEVRM_FindPreviousVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(NULL != prev_vg);
    assert(1 == prev_vg->pcl_id);
    assert(2 == prev_vg->vir_group_id);


    return 0;
}

int
DEVRM_UT_InsertPce_Test_Each_Linked_List_Condition()
{
    UI32_T  pcl_id;
    BOOL_T  is_ingress = TRUE;
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num = DEVRM_PCL_LOOKUP_0_0;
    UI32_T  pce_type   = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;
    int priority;

    UI32_T hw_idx;

    UI32_T  device_id = 0;
    UI32_T  vir_group_id;
    UI32_T  idx;

    UI32_T rule_id = 1;

    BOOL_T  result;

    for (pcl_id = 1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
    {
        result = DEVRM_CreatePcl(device_id, pcl_id, is_ingress, lookup_num, pce_type);
        assert(result == TRUE);

        pcl = DEVRM_GetPcl(device_id, pcl_id);
        assert(pcl != FALSE);

        for (idx=0; idx < _countof(pcl->vg_tbl); ++idx)
        {
            assert(pcl->vg_tbl[idx].valid == FALSE);
        }
    }

    pcl_id = 1;
    vir_group_id = 0;

    hw_idx = 10; // <-----
    priority = 4;

    result = DEVRM_AssignPceAtHwIndex(device_id, hw_idx, pce_type);
    assert(TRUE == result);

    pce = DEVRM_MallocPce(pcl_id, vir_group_id, hw_idx, pce_type, priority);
    assert(NULL != pce);

    pce->upper_rule_id = rule_id++;

    result = DEVRM_PLL_InsertPce(device_id, pcl_id, vir_group_id, pce);
    assert(TRUE == result);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    assert(pcl->pce_list == pce);
    assert(pcl->last_pce == pce->next);
    assert(2 == pcl->pce_number);

    vg = DEVRM_FindVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(vg->rv.pce_list == pce);
    assert(vg->rv.last_pce == pce->next);

    DEVRM_UT_Proc_Validate_PCE_Status();

    hw_idx = 20; // <-----
    priority = 4;

    result = DEVRM_AssignPceAtHwIndex(device_id, hw_idx, pce_type);
    assert(TRUE == result);

    pce = DEVRM_MallocPce(pcl_id, vir_group_id, hw_idx, pce_type, priority);
    assert(NULL != pce);

    pce->upper_rule_id = rule_id++;

    result = DEVRM_PLL_InsertPce(device_id, pcl_id, vir_group_id, pce);
    assert(TRUE == result);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    assert(pcl->pce_list == pce->prev->prev);
    assert(pcl->last_pce == pce->next);
    assert(4 == pcl->pce_number);

    vg = DEVRM_FindVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(vg->rv.pce_list == pce->prev->prev);
    assert(vg->rv.last_pce == pce->next);

    DEVRM_UT_Proc_Validate_PCE_Status();

    hw_idx = 6; // <-----
    priority = 5;

    result = DEVRM_AssignPceAtHwIndex(device_id, hw_idx, pce_type);
    assert(TRUE == result);

    pce = DEVRM_MallocPce(pcl_id, vir_group_id, hw_idx, pce_type, priority);
    assert(NULL != pce);

    pce->upper_rule_id = rule_id++;

    result = DEVRM_PLL_InsertPce(device_id, pcl_id, vir_group_id, pce);
    assert(TRUE == result);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    assert(pcl->pce_list == pce);
    assert(pcl->last_pce == pce->next->next->next->next->next);
    assert(6 == pcl->pce_number);

    vg = DEVRM_FindVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(vg->rv.pce_list == pcl->pce_list);
    assert(vg->rv.last_pce == pcl->last_pce);

    DEVRM_UT_Proc_Validate_PCE_Status();

    hw_idx = 12; // <-----
    priority = 4;

    result = DEVRM_AssignPceAtHwIndex(device_id, hw_idx, pce_type);
    assert(TRUE == result);

    pce = DEVRM_MallocPce(pcl_id, vir_group_id, hw_idx, pce_type, priority);
    assert(NULL != pce);

    pce->upper_rule_id = rule_id++;

    result = DEVRM_PLL_InsertPce(device_id, pcl_id, vir_group_id, pce);
    assert(TRUE == result);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    assert(pcl->pce_list == pce->prev->prev->prev->prev);
    assert(pcl->last_pce == pce->next->next->next);
    assert(8 == pcl->pce_number);

    vg = DEVRM_FindVirtualGroup(device_id, pcl_id, vir_group_id);
    assert(vg->rv.pce_list == pcl->pce_list);
    assert(vg->rv.last_pce == pcl->last_pce);

    DEVRM_UT_Proc_Validate_PCE_Status();

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_InsertPce_Capacity_Test()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {3,  6, 6,  1},
                {3,  8, 6,  2},
                {3, 10, 4,  3},
                {3, 12, 4,  4},
                {3, 14, 4,  5},
                {3, 16, 4,  6},
                {3, 18, 4,  7},

                {2, 28, 6,  8},
                {2, 26, 6,  9},
                {2, 24, 6, 10},
                {2, 30, 4, 11},
                {2, 32, 3, 12},

                {1, 40, 4, 13},
                {1, 44, 4, 14},
                {1, 46, 4, 15},
                {1, 48, 4, 16},
                {1, 50, 4, 17},

                {0},
            },
        },

        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5, 56, 6, 18},
                {5, 58, 6, 19},
                {5, 60, 4, 20},
                {5, 62, 4, 21},
                {5, 64, 4, 22},
                {5, 66, 4, 23},
                {5, 68, 4, 24},

                {3, 90, 4, 25},
                {3, 94, 4, 26},
                {3, 96, 4, 27},
                {3, 98, 4, 28},
                {3,100, 4, 29},

                {1,102, 4, 30},
                {1,104, 3, 31},
                {1,106, 1, 32},

                {0},
            }
        },

        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {1,110, 4, 33},
                {1,112, 4, 34},
                {1,114, 4, 35},

                {0},
            }
        },

        {
            4, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {1,120, 4, 36},
                {1,122, 4, 37},
                {1,124, 4, 38},
                {1,126, 4, 39},
//                {1,128, 4, 40},

                {0},
            }
        },

    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);

            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);

            DEVRM_UT_Proc_Validate_PCE_Status();
        }

    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_GetHwIndex()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI32_T pcl_id;
        UI32_T vir_group_id;
        int    priority;
        BOOL_T is_std_pce;
        UI32_T hw_idx;
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},

                {4,  6, 6,  1},
                {4,  8, 5,  2},
                {4, 10, 4,  3},
                {4, 12, 4,  4},
                {4, 14, 4,  5},
                {4, 16, 4,  6},
                {4, 18, 4,  7},

                {3,  0, 0,  0},

                {2, 24, 6,  8},
                {2, 26, 5,  9},
                {2, 28, 4, 10},
                {2, 30, 4, 11},
                {2, 34, 3, 12},

                {1, 40, 4, 13},
                {1, 44, 4, 14},
                {1, 46, 4, 15},
                {1, 48, 4, 16},
                {1, 50, 3, 17},

                {0},
            },
        },
        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},

                {3, 56, 6, 18},
                {3, 60, 5, 19},
                {3, 62, 4, 20},
                {3, 66, 4, 21},
                {3, 68, 4, 22},

                {2, 70, 4, 23},
                {2, 72, 3, 24},
                {2, 74, 1, 25},

                {1, 90, 5, 26},
                {1, 94, 4, 27},
                {1, 96, 4, 28},
                {1, 98, 4, 29},
                {1,100, 4, 30},

                {0},
            }
        },

        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},
                {4,  0, 0,  0},

                {0},
            }
        },

        {
            4, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},
                {4,  0, 0,  0},

                {0},
            }
        },

    };

    struct experimental_group exp_group[] =
    {
        {1, 5,  4, FALSE,  0},
        {1, 3,  4, FALSE, 20},


        {1, 1,  4, FALSE, 50},
        {1, 1,  3, FALSE, 52},
        {1, 1,  5, FALSE, 40},

        {1, 4, 10, FALSE,  6},
        {1, 4,  6, FALSE,  8},
        {1, 4,  5, FALSE, 10},
        {1, 4,  4, FALSE, 20},

        {1, 2, 10, FALSE, 24},
        {1, 2,  6, FALSE, 26},
        {1, 2,  5, FALSE, 28},
        {1, 2,  4, FALSE, 32},
        {1, 2,  3, FALSE, 36},

        {2, 5,  4, FALSE, 52},

        {2, 4,  4, FALSE, 52},

        {2, 3,  7, FALSE, 56},
        {2, 3,  6, FALSE, 58},
        {2, 3,  5, FALSE, 62},
        {2, 3,  4, FALSE, 70},
        {2, 3,  3, FALSE, 70},

        {2, 1,  7, FALSE, 90},
        {2, 1,  6, FALSE, 90},
        {2, 1,  5, FALSE, 92},
        {2, 1,  4, FALSE,102},
        {2, 1,  3, FALSE,102},

        {4, 4,  4, FALSE,102},
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_Order_Of_Rule();
            DEVRM_UT_Proc_Validate_Number_Of_Rule();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];
        UI16_T hw_idx;

        result = DEVRM_GetHwIndex(device_id, eg->pcl_id, eg->vir_group_id, eg->priority,
                                                    eg->is_std_pce, &hw_idx);
        assert(TRUE == result);

        assert(eg->hw_idx == hw_idx);
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_FindEmptyHwIndex_Case_By_Case_1()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI16_T hw_idx;
        BOOL_T is_std_pce;

        UI16_T empty_hw_idx;
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4,  2, 4,  1},

                {0},
            },
        },
        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {3,  5, 4,  2},
                {3,  6, 4,  3},
                {3,  7, 4,  4},
                {3,  9, 4,  5},

                {0},
            }
        },
    };

    struct experimental_group exp_group[] =
    {
        { 0, TRUE,  0},
        { 0, FALSE, 0},
        { 1, TRUE,  1},
        { 1, FALSE, 0xffff},
        { 2, TRUE,  8},
        { 2, FALSE, 8},
        { 3, TRUE,  8},
        { 3, FALSE, 0xffff},

        { 4, TRUE,  4},
        { 4, FALSE, 8},
        { 5, TRUE,  8},
        { 5, FALSE, 0xffff},
        { 6, TRUE,  8},
        { 6, FALSE, 10},
        { 7, TRUE,  8},
        { 7, FALSE, 0xffff},

        { 8, TRUE,  8},
        { 8, FALSE, 10},
        { 9, TRUE,  10},
        { 9, FALSE, 0xffff},
        {10, TRUE,  10},
        {10, FALSE, 10},
        {11, TRUE,  11},
        {11, FALSE, 0xffff},
        {12, TRUE,  12},
        {12, FALSE, 12},
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];
        UI16_T empty_hw_idx;

        empty_hw_idx = DEVRM_FindEmptyHwIndex(device_id, eg->hw_idx, eg->is_std_pce);
        assert(empty_hw_idx == eg->empty_hw_idx);
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_FindEmptyHwIndex_Case_By_Case_2()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI16_T hw_idx;
        BOOL_T is_std_pce;

        UI16_T empty_hw_idx;
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4,  2, 4,  1},

                {0},
            },
        },
        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {3,  5, 4,  2},

                {0},
            }
        },
        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4,  6, 4,  3},

                {0},
            },
        },
        {
            4, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {3,  9, 4,  4},

                {0},
            }
        },
        {
            5, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4, 10, 4,  5},

                {0},
            },
        },
    };

    struct experimental_group exp_group[] =
    {
        {2, FALSE, 12},
        {2, TRUE,  12},
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];
        UI16_T empty_hw_idx;

        empty_hw_idx = DEVRM_FindEmptyHwIndex(device_id, eg->hw_idx, eg->is_std_pce);
        assert(empty_hw_idx == eg->empty_hw_idx);
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_FindEmptyHwIndex_Case_By_Case_3()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI16_T hw_idx;
        BOOL_T is_std_pce;

        UI16_T empty_hw_idx;
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4,  2, 4,  1},

                {0},
            },
        },
        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {3,  4, 4,  2},
                {3,  6, 4,  3},

                {0},
            }
        },
    };

    struct experimental_group exp_group[] =
    {
        {2, FALSE, 7},
        {2, TRUE,  7},
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];
        UI16_T empty_hw_idx;

        empty_hw_idx = DEVRM_FindEmptyHwIndex(device_id, eg->hw_idx, eg->is_std_pce);
        assert(empty_hw_idx == eg->empty_hw_idx);
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_FindEmptyHwIndex_Case_By_Case_4()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI16_T hw_idx;
        BOOL_T is_std_pce;

        UI16_T empty_hw_idx;
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4,  2, 4,  1},

                {0},
            },
        },
        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {3,  5, 4,  2},
                {3,  7, 4,  3},

                {0},
            }
        },
    };

    struct experimental_group exp_group[] =
    {
        {2, FALSE, 6},
        {2, TRUE,  6},
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];
        UI16_T empty_hw_idx;

        empty_hw_idx = DEVRM_FindEmptyHwIndex(device_id, eg->hw_idx, eg->is_std_pce);
        assert(empty_hw_idx == eg->empty_hw_idx);
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_FindEmptyHwIndex_Case_By_Case_5()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI16_T hw_idx;
        BOOL_T is_std_pce;

        UI16_T empty_hw_idx;
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4,  2, 4,  1},

                {0},
            },
        },
        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {3,  4, 4,  2},
                {3,  5, 4,  3},
                {3,  7, 4,  4},

                {0},
            }
        },
    };

    struct experimental_group exp_group[] =
    {
        {2, FALSE, 8},
        {2, TRUE,  8},
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];
        UI16_T empty_hw_idx;

        empty_hw_idx = DEVRM_FindEmptyHwIndex(device_id, eg->hw_idx, eg->is_std_pce);
        assert(empty_hw_idx == eg->empty_hw_idx);
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_FindEmptyHwIndexReverse_Case_By_Case_1()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI16_T hw_idx;
        BOOL_T is_std_pce;

        UI16_T empty_hw_idx;
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {4,  3, 4,  1},

                {0},
            },
        },
        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4,  4, 4,  2},

                {0},
            },
        },
        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {3,  7, 4,  3},
                {3,  8, 4,  4},
                {3,  9, 4,  5},
                {3, 11, 4,  6},

                {0},
            }
        },
    };

    struct experimental_group exp_group[] =
    {
        {11, TRUE,  10},

        {10, FALSE,  0},
        {10, TRUE,   6},

        { 9, TRUE,   6},

        { 8, TRUE,   6},
        { 8, FALSE,  0},

        { 7, TRUE,   6},

        { 6, TRUE,   0},
        { 6, FALSE,  0},

        { 5, TRUE,   0},

        { 4, TRUE,   2},
        { 4, FALSE,  0},

        { 3, TRUE,   2},
        { 2, FALSE,  0},
        { 1, TRUE,   0},

        { 0, TRUE,   0xffff},
        { 0, FALSE,  0xffff},

    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];
        UI16_T empty_hw_idx;

        empty_hw_idx = DEVRM_FindEmptyHwIndexReverse(device_id, eg->hw_idx, eg->is_std_pce);
        assert(empty_hw_idx == eg->empty_hw_idx);
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_FindEmptyHwIndexReverse_Case_By_Case_2()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI16_T hw_idx;
        BOOL_T is_std_pce;

        UI16_T empty_hw_idx;
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4,  2, 4,  1},

                {0},
            },
        },
        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {3,  5, 4,  2},

                {0},
            }
        },
        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4,  6, 4,  3},

                {0},
            },
        },
        {
            4, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {3,  9, 4,  4},

                {0},
            }
        },
        {
            5, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4, 10, 4,  5},

                {0},
            },
        },
    };

    struct experimental_group exp_group[] =
    {
        {14, FALSE, 12},
        {14, TRUE,  12},
        {13, TRUE,  12},
        {12, FALSE,  0},
        {11, TRUE,   0},
        {10, FALSE,  0},
        {10, TRUE,   8},
        { 9, TRUE,   8},
        { 8, FALSE,  0},
        { 8, TRUE,   0},
        { 7, TRUE,   0},
        { 6, FALSE,  0},
        { 6, TRUE,   4},
        { 5, TRUE,   4},
        { 4, TRUE,   0},
        { 4, FALSE,  0},
        { 3, TRUE,   0},
        { 2, TRUE,   0},
        { 2, FALSE,  0},
        { 1, TRUE,   0},
        { 0, TRUE,   0xffff},
        { 0, FALSE,  0Xffff},
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];
        UI16_T empty_hw_idx;

        empty_hw_idx = DEVRM_FindEmptyHwIndexReverse(device_id, eg->hw_idx, eg->is_std_pce);
        assert(empty_hw_idx == eg->empty_hw_idx);
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_FindEmptyHwIndexReverse_Case_By_Case_3()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI16_T hw_idx;
        BOOL_T is_std_pce;

        UI16_T empty_hw_idx;
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4,  2, 4,  1},

                {0},
            },
        },
        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {3,  4, 4,  2},
                {3,  6, 4,  3},

                {0},
            }
        },
    };

    struct experimental_group exp_group[] =
    {
        {10, FALSE, 8},
        {10, TRUE,  8},
        { 9, TRUE,  8},
        { 8, TRUE,  7},
        { 8, FALSE, 5},
        { 7, TRUE,  5},
        { 6, TRUE,  5},
        { 6, FALSE, 0},
        { 5, TRUE,  0},
        { 4, TRUE,  0},
        { 4, FALSE, 0},
        { 3, TRUE,  0},
        { 2, TRUE,  0},
        { 2, FALSE, 0},
        { 1, TRUE,  0},
        { 0, TRUE,  0xffff},
        { 0, FALSE, 0xffff},
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];
        UI16_T empty_hw_idx;

        empty_hw_idx = DEVRM_FindEmptyHwIndexReverse(device_id, eg->hw_idx, eg->is_std_pce);
        assert(empty_hw_idx == eg->empty_hw_idx);
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_FindEmptyHwIndexReverse_Case_By_Case_4()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI16_T hw_idx;
        BOOL_T is_std_pce;

        UI16_T empty_hw_idx;
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4,  0, 4,  1},
                {4,  2, 4,  2},

                {0},
            },
        },
        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {3,  5, 4,  3},
                {3,  7, 4,  4},

                {0},
            }
        },
    };

    struct experimental_group exp_group[] =
    {
        {10, FALSE, 8},
        {10, TRUE,  8},
        { 9, TRUE,  8},
        { 8, TRUE,  6},
        { 8, FALSE, 4},
        { 7, TRUE,  6},
        { 6, TRUE,  4},
        { 6, FALSE, 0xffff},
        { 5, TRUE,  4},
        { 4, TRUE,  0xffff},
        { 4, FALSE, 0xffff},
        { 3, TRUE,  0xffff},
        { 2, TRUE,  0xffff},
        { 2, FALSE, 0xffff},
        { 1, TRUE,  0xffff},
        { 0, TRUE,  0xffff},
        { 0, FALSE, 0xffff},
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];
        UI16_T empty_hw_idx;

        empty_hw_idx = DEVRM_FindEmptyHwIndexReverse(device_id, eg->hw_idx, eg->is_std_pce);
        assert(empty_hw_idx == eg->empty_hw_idx);
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_FindEmptyHwIndexReverse_Case_By_Case_5()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI16_T hw_idx;
        BOOL_T is_std_pce;

        UI16_T empty_hw_idx;
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {3,  5, 4,  1},
                {3,  6, 4,  2},
                {3,  7, 4,  3},

                {0},
            }
        },

        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {4,  8, 4,  4},

                {0},
            },
        },
    };

    struct experimental_group exp_group[] =
    {
        {12, FALSE, 10},
        {12, TRUE,  10},
        {11, TRUE,  10},
        {10, FALSE, 2},
        {10, TRUE,  2},
        { 9, TRUE,  2},
        { 8, TRUE,  4},
        { 8, FALSE, 2},
        { 7, TRUE,  4},
        { 6, TRUE,  4},
        { 6, FALSE, 2},
        { 5, TRUE,  4},
        { 4, TRUE,  2},
        { 4, FALSE, 2},
        { 3, TRUE,  2},
        { 2, TRUE,  0},
        { 2, FALSE, 0},
        { 1, TRUE,  0},
        { 0, TRUE,  0xffff},
        { 0, FALSE, 0xffff},
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];
        UI16_T empty_hw_idx;

        empty_hw_idx = DEVRM_FindEmptyHwIndexReverse(device_id, eg->hw_idx, eg->is_std_pce);
        assert(empty_hw_idx == eg->empty_hw_idx);
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_ShiftToRight_Case_By_Case_For_Extended_Rule()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI32_T hw_idx;
        BOOL_T is_std_pce;

        struct setting_pcl s_pcl[MAX_PCL_ENTRIES];
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},

                {4,  6, 6,  1},
                {4,  8, 5,  2},
                {4, 10, 4,  3},
                {4, 12, 4,  4},
                {4, 14, 4,  5},
                {4, 16, 4,  6},
                {4, 18, 4,  7},

                {3,  0, 0,  0},

                {2, 24, 6,  8},
                {2, 26, 5,  9},
                {2, 28, 4, 10},
                {2, 30, 4, 11},
                {2, 34, 3, 12},

                {1, 40, 4, 13},
                {1, 44, 4, 14},
                {1, 46, 4, 15},
                {1, 48, 4, 16},
                {1, 50, 3, 17},

                {0},
            },
        },
        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},

                {3, 56, 6, 18},
                {3, 60, 5, 19},
                {3, 62, 4, 20},
                {3, 66, 4, 21},
                {3, 68, 4, 22},

                {2, 70, 4, 23},
                {2, 72, 3, 24},
                {2, 74, 1, 25},

                {1, 90, 5, 26},
                {1, 94, 4, 27},
                {1, 96, 4, 28},
                {1, 98, 4, 29},
                {1,100, 4, 30},

                {0},
            }
        },
    };

    struct experimental_group exp_group[] =
    {
        {
            0, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {4, 12, 4,  4},
                        {4, 14, 4,  5},
                        {4, 16, 4,  6},
                        {4, 18, 4,  7},

                        {2, 24, 6,  8},
                        {2, 26, 5,  9},
                        {2, 28, 4, 10},
                        {2, 30, 4, 11},
                        {2, 34, 3, 12},

                        {1, 40, 4, 13},
                        {1, 44, 4, 14},
                        {1, 46, 4, 15},
                        {1, 48, 4, 16},
                        {1, 50, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 56, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 70, 4, 23},
                        {2, 72, 3, 24},
                        {2, 74, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },

        {
            2, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {4, 12, 4,  4},
                        {4, 14, 4,  5},
                        {4, 16, 4,  6},
                        {4, 18, 4,  7},

                        {2, 24, 6,  8},
                        {2, 26, 5,  9},
                        {2, 28, 4, 10},
                        {2, 30, 4, 11},
                        {2, 34, 3, 12},

                        {1, 40, 4, 13},
                        {1, 44, 4, 14},
                        {1, 46, 4, 15},
                        {1, 48, 4, 16},
                        {1, 50, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 56, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 70, 4, 23},
                        {2, 72, 3, 24},
                        {2, 74, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },

        {
            4, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {4, 12, 4,  4},
                        {4, 14, 4,  5},
                        {4, 16, 4,  6},
                        {4, 18, 4,  7},

                        {2, 24, 6,  8},
                        {2, 26, 5,  9},
                        {2, 28, 4, 10},
                        {2, 30, 4, 11},
                        {2, 34, 3, 12},

                        {1, 40, 4, 13},
                        {1, 44, 4, 14},
                        {1, 46, 4, 15},
                        {1, 48, 4, 16},
                        {1, 50, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 56, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 70, 4, 23},
                        {2, 72, 3, 24},
                        {2, 74, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            6, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4,  6+2, 6,  1},
                        {4,  8+2, 5,  2},
                        {4, 10+2, 4,  3},
                        {4, 12+2, 4,  4},
                        {4, 14+2, 4,  5},
                        {4, 16+2, 4,  6},
                        {4, 18+2, 4,  7},

                        {2, 24, 6,  8},
                        {2, 26, 5,  9},
                        {2, 28, 4, 10},
                        {2, 30, 4, 11},
                        {2, 34, 3, 12},

                        {1, 40, 4, 13},
                        {1, 44, 4, 14},
                        {1, 46, 4, 15},
                        {1, 48, 4, 16},
                        {1, 50, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 56, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 70, 4, 23},
                        {2, 72, 3, 24},
                        {2, 74, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            8, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4, 10, 6,  1},
                        {4, 12, 5,  2},
                        {4, 14, 4,  3},
                        {4, 16, 4,  4},
                        {4, 18, 4,  5},
                        {4, 20, 4,  6},
                        {4, 22, 4,  7},

                        {2, 24, 6,  8},
                        {2, 26, 5,  9},
                        {2, 28, 4, 10},
                        {2, 30, 4, 11},
                        {2, 34, 3, 12},

                        {1, 40, 4, 13},
                        {1, 44, 4, 14},
                        {1, 46, 4, 15},
                        {1, 48, 4, 16},
                        {1, 50, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 56, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 70, 4, 23},
                        {2, 72, 3, 24},
                        {2, 74, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            12, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4, 10, 6,  1},
                        {4, 14, 5,  2},
                        {4, 16, 4,  3},
                        {4, 18, 4,  4},
                        {4, 20, 4,  5},
                        {4, 22, 4,  6},
                        {4, 24, 4,  7},

                        {2, 26, 6,  8},
                        {2, 28, 5,  9},
                        {2, 30, 4, 10},
                        {2, 32, 4, 11},
                        {2, 34, 3, 12},

                        {1, 40, 4, 13},
                        {1, 44, 4, 14},
                        {1, 46, 4, 15},
                        {1, 48, 4, 16},
                        {1, 50, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 56, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 70, 4, 23},
                        {2, 72, 3, 24},
                        {2, 74, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            26, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4, 10, 6,  1},
                        {4, 14, 5,  2},
                        {4, 16, 4,  3},
                        {4, 18, 4,  4},
                        {4, 20, 4,  5},
                        {4, 22, 4,  6},
                        {4, 24, 4,  7},

                        {2, 28, 6,  8},
                        {2, 30, 5,  9},
                        {2, 32, 4, 10},
                        {2, 34, 4, 11},
                        {2, 36, 3, 12},

                        {1, 40, 4, 13},
                        {1, 44, 4, 14},
                        {1, 46, 4, 15},
                        {1, 48, 4, 16},
                        {1, 50, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 56, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 70, 4, 23},
                        {2, 72, 3, 24},
                        {2, 74, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            28, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4, 10, 6,  1},
                        {4, 14, 5,  2},
                        {4, 16, 4,  3},
                        {4, 18, 4,  4},
                        {4, 20, 4,  5},
                        {4, 22, 4,  6},
                        {4, 24, 4,  7},

                        {2, 30, 6,  8},
                        {2, 32, 5,  9},
                        {2, 34, 4, 10},
                        {2, 36, 4, 11},
                        {2, 38, 3, 12},

                        {1, 40, 4, 13},
                        {1, 44, 4, 14},
                        {1, 46, 4, 15},
                        {1, 48, 4, 16},
                        {1, 50, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 56, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 70, 4, 23},
                        {2, 72, 3, 24},
                        {2, 74, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            32, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4, 10, 6,  1},
                        {4, 14, 5,  2},
                        {4, 16, 4,  3},
                        {4, 18, 4,  4},
                        {4, 20, 4,  5},
                        {4, 22, 4,  6},
                        {4, 24, 4,  7},

                        {2, 30, 6,  8},
                        {2, 34, 5,  9},
                        {2, 36, 4, 10},
                        {2, 38, 4, 11},
                        {2, 40, 3, 12},

                        {1, 42, 4, 13},
                        {1, 44, 4, 14},
                        {1, 46, 4, 15},
                        {1, 48, 4, 16},
                        {1, 50, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 56, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 70, 4, 23},
                        {2, 72, 3, 24},
                        {2, 74, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            40, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4, 10, 6,  1},
                        {4, 14, 5,  2},
                        {4, 16, 4,  3},
                        {4, 18, 4,  4},
                        {4, 20, 4,  5},
                        {4, 22, 4,  6},
                        {4, 24, 4,  7},

                        {2, 30, 6,  8},
                        {2, 34, 5,  9},
                        {2, 36, 4, 10},
                        {2, 38, 4, 11},
                        {2, 42, 3, 12},

                        {1, 44, 4, 13},
                        {1, 46, 4, 14},
                        {1, 48, 4, 15},
                        {1, 50, 4, 16},
                        {1, 52, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 56, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 70, 4, 23},
                        {2, 72, 3, 24},
                        {2, 74, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            52, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4, 10, 6,  1},
                        {4, 14, 5,  2},
                        {4, 16, 4,  3},
                        {4, 18, 4,  4},
                        {4, 20, 4,  5},
                        {4, 22, 4,  6},
                        {4, 24, 4,  7},

                        {2, 30, 6,  8},
                        {2, 34, 5,  9},
                        {2, 36, 4, 10},
                        {2, 38, 4, 11},
                        {2, 42, 3, 12},

                        {1, 44, 4, 13},
                        {1, 46, 4, 14},
                        {1, 48, 4, 15},
                        {1, 50, 4, 16},
                        {1, 54, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 56, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 70, 4, 23},
                        {2, 72, 3, 24},
                        {2, 74, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            56, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4, 10, 6,  1},
                        {4, 14, 5,  2},
                        {4, 16, 4,  3},
                        {4, 18, 4,  4},
                        {4, 20, 4,  5},
                        {4, 22, 4,  6},
                        {4, 24, 4,  7},

                        {2, 30, 6,  8},
                        {2, 34, 5,  9},
                        {2, 36, 4, 10},
                        {2, 38, 4, 11},
                        {2, 42, 3, 12},

                        {1, 44, 4, 13},
                        {1, 46, 4, 14},
                        {1, 48, 4, 15},
                        {1, 50, 4, 16},
                        {1, 54, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 58, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 70, 4, 23},
                        {2, 72, 3, 24},
                        {2, 74, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            70, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4, 10, 6,  1},
                        {4, 14, 5,  2},
                        {4, 16, 4,  3},
                        {4, 18, 4,  4},
                        {4, 20, 4,  5},
                        {4, 22, 4,  6},
                        {4, 24, 4,  7},

                        {2, 30, 6,  8},
                        {2, 34, 5,  9},
                        {2, 36, 4, 10},
                        {2, 38, 4, 11},
                        {2, 42, 3, 12},

                        {1, 44, 4, 13},
                        {1, 46, 4, 14},
                        {1, 48, 4, 15},
                        {1, 50, 4, 16},
                        {1, 54, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 58, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 72, 4, 23},
                        {2, 74, 3, 24},
                        {2, 76, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            30, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4, 10, 6,  1},
                        {4, 14, 5,  2},
                        {4, 16, 4,  3},
                        {4, 18, 4,  4},
                        {4, 20, 4,  5},
                        {4, 22, 4,  6},
                        {4, 24, 4,  7},

                        {2, 32, 6,  8},
                        {2, 34, 5,  9},
                        {2, 36, 4, 10},
                        {2, 38, 4, 11},
                        {2, 42, 3, 12},

                        {1, 44, 4, 13},
                        {1, 46, 4, 14},
                        {1, 48, 4, 15},
                        {1, 50, 4, 16},
                        {1, 54, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 58, 6, 18},
                        {3, 60, 5, 19},
                        {3, 62, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 72, 4, 23},
                        {2, 74, 3, 24},
                        {2, 76, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            58, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4, 10, 6,  1},
                        {4, 14, 5,  2},
                        {4, 16, 4,  3},
                        {4, 18, 4,  4},
                        {4, 20, 4,  5},
                        {4, 22, 4,  6},
                        {4, 24, 4,  7},

                        {2, 32, 6,  8},
                        {2, 34, 5,  9},
                        {2, 36, 4, 10},
                        {2, 38, 4, 11},
                        {2, 42, 3, 12},

                        {1, 44, 4, 13},
                        {1, 46, 4, 14},
                        {1, 48, 4, 15},
                        {1, 50, 4, 16},
                        {1, 54, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 60, 6, 18},
                        {3, 62, 5, 19},
                        {3, 64, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 72, 4, 23},
                        {2, 74, 3, 24},
                        {2, 76, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
        {
            70, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {4, 10, 6,  1},
                        {4, 14, 5,  2},
                        {4, 16, 4,  3},
                        {4, 18, 4,  4},
                        {4, 20, 4,  5},
                        {4, 22, 4,  6},
                        {4, 24, 4,  7},

                        {2, 32, 6,  8},
                        {2, 34, 5,  9},
                        {2, 36, 4, 10},
                        {2, 38, 4, 11},
                        {2, 42, 3, 12},

                        {1, 44, 4, 13},
                        {1, 46, 4, 14},
                        {1, 48, 4, 15},
                        {1, 50, 4, 16},
                        {1, 54, 3, 17},

                        {0},
                    },
                },
                {
                    2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {3, 60, 6, 18},
                        {3, 62, 5, 19},
                        {3, 64, 4, 20},
                        {3, 66, 4, 21},
                        {3, 68, 4, 22},

                        {2, 72, 4, 23},
                        {2, 74, 3, 24},
                        {2, 76, 1, 25},

                        {1, 90, 5, 26},
                        {1, 94, 4, 27},
                        {1, 96, 4, 28},
                        {1, 98, 4, 29},
                        {1,100, 4, 30},

                        {0},
                    }
                },

                {0},

            },
        },
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];

        result = DEVRM_ShiftPceToRight(device_id, eg->hw_idx, eg->is_std_pce);
        assert(TRUE == result);

        DEVRM_UT_Proc_Validate_PCE_Status();

        for (j=0; j < _countof(eg->s_pcl); ++j)
        {
            struct setting_pcl *s_pcl = &eg->s_pcl[j];
            UI32_T __k;

            if (0 == s_pcl->pcl_id)
            {
                break;
            }

            for (__k = 0; __k < _countof(s_pcl->s_pce); ++__k)
            {
                struct setting_pce *s_pce = &s_pcl->s_pce[__k];

                if (0 == s_pce->upper_rule_id)
                {
                    continue;
                }

                result = DEVRM_GetPce(device_id, s_pce->upper_rule_id, &pce);
                assert(TRUE == result);
                assert(pce->hw_idx == s_pce->hw_idx);
            }
        }
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_ShiftToRight_Capacity_Test_For_Extended_Rule()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {9,  0, 0,  0},
                {8,  0, 0,  0},
                {7,  0, 0,  0},
                {6,  0, 0,  0},
                {5,  0, 4,  1},
                {4,  2, 4,  2},
                {3,  4, 4,  3},
                {2,  6, 4,  4},
                {1,  8, 4,  5},

                {0},
            },
        },

        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},
                {4,  0, 0,  0},
                {3,  0, 0,  0},
                {2,  0, 0,  0},
                {1, 20, 4,  6},
                {1, 22, 4,  7},
                {1, 24, 4,  8},
                {1, 26, 4,  9},
                {1, 28, 4, 10},

                {0},
            },
        },

        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},
                {4,  0, 0,  0},
                {3,  0, 0,  0},
                {2,  0, 0,  0},
                {1, 40, 4, 11},
                {1, 42, 4, 12},
                {1, 44, 4, 13},
                {1, 46, 4, 14},
                {1, 48, 4, 15},

                {0},
            },
        },

        {
            4, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},
                {4,  0, 0,  0},
                {3,  0, 0,  0},
                {2,  0, 0,  0},
                {1, 60, 4, 16},
                {1, 62, 4, 17},
                {1, 64, 4, 18},
                {1, 66, 4, 19},
                {1, 68, 4, 20},

                {0},
            },
        },

        {
            5, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},
                {4,  0, 0,  0},
                {3,  0, 0,  0},
                {2,  0, 0,  0},
                {1, 70, 4, 21},
                {1, 72, 4, 22},
                {1, 74, 4, 23},
                {1, 76, 4, 24},
                {1, 78, 4, 25},

                {0},
            },
        },
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    while (TRUE)
    {
        result = DEVRM_GetPce(device_id, 1, &pce);
        assert(TRUE == result);

        result = DEVRM_ShiftPceToRight(device_id, pce->hw_idx, pce->is_std_pce);

        DEVRM_UT_Proc_Validate_PCE_Status();

        if (FALSE == result)
        {
            UI32_T rule_id = 25; // <<------------------------------------------
            UI32_T hw_idx; // <<------------------------------------------------

            hw_idx = DEVRM_MAX_NBR_OF_TCAM_ENTRIES - (2);

            do {
                result = DEVRM_GetPce(device_id, rule_id, &pce);
                assert(TRUE == result);

                assert(pce->hw_idx == hw_idx);

                hw_idx -= 2;

            } while ( --rule_id > 0 );

            assert(0 == rule_id);

            break;
        }
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_ShiftToLeft_Capacity_Test_For_Extended_Rule()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        UI32_T pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},
                {4,  0, 0,  0},
                {3,  0, 0,  0},
                {2,  0, 0,  0},
                {1,  0, 4, 25},
                {1,  2, 4, 24},
                {1,  4, 4, 23},
                {1,  6, 4, 22},
                {1,  8, 4, 21},

                {0},
            },
        },

        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},
                {4,  0, 0,  0},
                {3,  0, 0,  0},
                {2,  0, 0,  0},
                {1, 20, 4, 20},
                {1, 22, 4, 19},
                {1, 24, 4, 18},
                {1, 26, 4, 17},
                {1, 28, 4, 16},

                {0},
            },
        },

        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},
                {4,  0, 0,  0},
                {3,  0, 0,  0},
                {2,  0, 0,  0},
                {1, 40, 4, 15},
                {1, 42, 4, 14},
                {1, 44, 4, 13},
                {1, 46, 4, 12},
                {1, 48, 4, 11},

                {0},
            },
        },

        {
            4, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},
                {4,  0, 0,  0},
                {3,  0, 0,  0},
                {2,  0, 0,  0},
                {1, 60, 4, 10},
                {1, 62, 4,  9},
                {1, 64, 4,  8},
                {1, 66, 4,  7},
                {1, 68, 4,  6},

                {0},
            },
        },

        {
            5, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},
                {4,  0, 0,  0},
                {3,  0, 0,  0},
                {2,  0, 0,  0},
                {1, 70, 4,  5},
                {1, 72, 4,  4},
                {1, 74, 4,  3},
                {1, 76, 4,  2},
                {1, 78, 4,  1},

                {0},
            },
        },
    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    while (TRUE)
    {
        result = DEVRM_GetPce(device_id, 1, &pce);
        assert(TRUE == result);

        result = DEVRM_ShiftPceToLeft(device_id, pce->hw_idx+2, pce->is_std_pce);

        DEVRM_UT_Proc_Validate_PCE_Status();

        if (FALSE == result)
        {
            UI32_T rule_id = 25; // <<------------------------------------------
            UI32_T hw_idx = 0; // <<--------------------------------------------

            do {
                result = DEVRM_GetPce(device_id, rule_id, &pce);
                assert(TRUE == result);

                assert(pce->hw_idx == hw_idx);

                hw_idx += 2;

            } while ( --rule_id > 0 );

            assert(0 == rule_id);

            break;
        }
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_ShiftToRight_Case_By_Case_For_Mixed_Rule()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI32_T hw_idx;
        BOOL_T is_std_pce;

        struct setting_pcl s_pcl[MAX_PCL_ENTRIES];
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},

                {4,  6, 6,  1},
                {4,  8, 5,  2},
                {4, 10, 4,  3},
                {3, 20, 4,  4},

                {2, 24, 4,  5},
                {2, 26, 4,  6},

                {0},
            },
        },

        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {5,  0, 0,  0},

                {4, 30, 6,  7},
                {4, 31, 5,  8},
                {4, 33, 4,  9},
                {3, 34, 4, 10},

                {2, 40, 4, 11},
                {2, 59, 4, 12},

                {0},
            },
        },

        {
            6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
            {
                {5,  0, 0,  0},

                {4, 60, 6, 13},
                {4, 64, 5, 14},
                {4, 66, 4, 15},
                {3, 70, 4, 16},

                {2, 72, 4, 17},
                {2, 74, 4, 18},

                {0},
            },
        },

        {
            9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {5,  0, 0,  0},

                {4, 76, 6, 19},
                {4, 77, 5, 20},
                {4, 78, 4, 21},
                {3, 79, 4, 22},

                {2, 95, 4, 23},
                {2, 96, 4, 24},

                {0},
            },
        },

    };

    struct experimental_group exp_group[] =
    {
        {
            24, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 26, 4,  5},
                        {2, 28, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            28, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 26, 4,  5},
                        {2, 30, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 32, 6,  7},
                        {4, 33, 5,  8},
                        {4, 35, 4,  9},
                        {3, 36, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            32, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 26, 4,  5},
                        {2, 30, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 33, 6,  7},
                        {4, 34, 5,  8},
                        {4, 35, 4,  9},
                        {3, 36, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            32, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 26, 4,  5},
                        {2, 30, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 33+2, 6,  7},
                        {4, 34+2, 5,  8},
                        {4, 35+2, 4,  9},
                        {3, 36+2, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            59, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 26, 4,  5},
                        {2, 30, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 35, 6,  7},
                        {4, 36, 5,  8},
                        {4, 37, 4,  9},
                        {3, 38, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59+1, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60+2, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            60, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 26, 4,  5},
                        {2, 30, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 35, 6,  7},
                        {4, 36, 5,  8},
                        {4, 37, 4,  9},
                        {3, 38, 4, 10},

                        {2, 40, 4, 11},
                        {2, 60+1, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 62, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            61, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 26, 4,  5},
                        {2, 30, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 35, 6,  7},
                        {4, 36, 5,  8},
                        {4, 37, 4,  9},
                        {3, 38, 4, 10},

                        {2, 40, 4, 11},
                        {2, 61+1, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 64, 6, 13},
                        {4, 66, 5, 14},
                        {4, 68, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            62, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 26, 4,  5},
                        {2, 30, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 35, 6,  7},
                        {4, 36, 5,  8},
                        {4, 37, 4,  9},
                        {3, 38, 4, 10},

                        {2, 40, 4, 11},
                        {2, 62+1, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 64, 6, 13},
                        {4, 66, 5, 14},
                        {4, 68, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            63, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 26, 4,  5},
                        {2, 30, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 35, 6,  7},
                        {4, 36, 5,  8},
                        {4, 37, 4,  9},
                        {3, 38, 4, 10},

                        {2, 40, 4, 11},
                        {2, 63+1, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 64+2, 6, 13},
                        {4, 66+2, 5, 14},
                        {4, 68+2, 4, 15},
                        {3, 70+2, 4, 16},

                        {2, 72+2, 4, 17},
                        {2, 74+2, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76+2, 6, 19},
                        {4, 77+2, 5, 20},
                        {4, 78+2, 4, 21},
                        {3, 79+2, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },
    };


    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];

        result = DEVRM_ShiftPceToRight(device_id, eg->hw_idx, eg->is_std_pce);
        assert(TRUE == result);

        DEVRM_UT_Proc_Validate_PCE_Status();

        for (j=0; j < _countof(eg->s_pcl); ++j)
        {
            struct setting_pcl *s_pcl = &eg->s_pcl[j];
            UI32_T __k;

            if (0 == s_pcl->pcl_id)
            {
                break;
            }

            for (__k = 0; __k < _countof(s_pcl->s_pce); ++__k)
            {
                struct setting_pce *s_pce = &s_pcl->s_pce[__k];

                if (0 == s_pce->upper_rule_id)
                {
                    continue;
                }

                result = DEVRM_GetPce(device_id, s_pce->upper_rule_id, &pce);
                assert(TRUE == result);
                assert(pce->hw_idx == s_pce->hw_idx);
            }
        }
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_ShiftToLeft_Case_By_Case_For_Mixed_Rule()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI32_T hw_idx;
        BOOL_T is_std_pce;

        struct setting_pcl s_pcl[MAX_PCL_ENTRIES];
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},

                {4,  6, 6,  1},
                {4,  8, 5,  2},
                {4, 10, 4,  3},
                {3, 20, 4,  4},

                {2, 24, 4,  5},
                {2, 26, 4,  6},

                {0},
            },
        },

        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {5,  0, 0,  0},

                {4, 30, 6,  7},
                {4, 31, 5,  8},
                {4, 33, 4,  9},
                {3, 34, 4, 10},

                {2, 40, 4, 11},
                {2, 59, 4, 12},

                {0},
            },
        },

        {
            6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
            {
                {5,  0, 0,  0},

                {4, 60, 6, 13},
                {4, 64, 5, 14},
                {4, 66, 4, 15},
                {3, 70, 4, 16},

                {2, 72, 4, 17},
                {2, 74, 4, 18},

                {0},
            },
        },

        {
            9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {5,  0, 0,  0},

                {4, 76, 6, 19},
                {4, 77, 5, 20},
                {4, 78, 4, 21},
                {3, 79, 4, 22},

                {2, 95, 4, 23},
                {2, 96, 4, 24},

                {0},
            },
        },

    };

    struct experimental_group exp_group[] =
    {
        {
            10, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  6-2, 6,  1},
                        {4,  8-2, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 24, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            26, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 24-2, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            31, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30-1, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            79, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70-2, 4, 16},

                        {2, 72-2, 4, 17},
                        {2, 74-2, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76-1, 6, 19},
                        {4, 77-1, 5, 20},
                        {4, 78-1, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            78, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 68, 4, 16},

                        {2, 70, 4, 17},
                        {2, 72, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 75-1, 6, 19},
                        {4, 76-1, 5, 20},
                        {4, 77-1, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            78, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 68, 4, 16},

                        {2, 70, 4, 17},
                        {2, 72, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 74, 6, 19},
                        {4, 75, 5, 20},
                        {4, 76, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            77, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64-2, 5, 14},
                        {4, 66-2, 4, 15},
                        {3, 68-2, 4, 16},

                        {2, 70-2, 4, 17},
                        {2, 72-2, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 74-1, 6, 19},
                        {4, 75-1, 5, 20},
                        {4, 76-1, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            76, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 62, 5, 14},
                        {4, 64, 4, 15},
                        {3, 66, 4, 16},

                        {2, 68, 4, 17},
                        {2, 70, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 73-1, 6, 19},
                        {4, 74-1, 5, 20},
                        {4, 75-1, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            75, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59-2, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60-2, 6, 13},
                        {4, 62-2, 5, 14},
                        {4, 64-2, 4, 15},
                        {3, 66-2, 4, 16},

                        {2, 68-2, 4, 17},
                        {2, 70-2, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 72-1, 6, 19},
                        {4, 73-1, 5, 20},
                        {4, 74-1, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            74, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 57, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 58, 6, 13},
                        {4, 60, 5, 14},
                        {4, 62, 4, 15},
                        {3, 64, 4, 16},

                        {2, 66, 4, 17},
                        {2, 68, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 71-1, 6, 19},
                        {4, 72-1, 5, 20},
                        {4, 73-1, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            73, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 57-2, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 58-2, 6, 13},
                        {4, 60-2, 5, 14},
                        {4, 62-2, 4, 15},
                        {3, 64-2, 4, 16},

                        {2, 66-2, 4, 17},
                        {2, 68-2, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 70-1, 6, 19},
                        {4, 71-1, 5, 20},
                        {4, 72-1, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            55, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 55, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 56, 6, 13},
                        {4, 58, 5, 14},
                        {4, 60, 4, 15},
                        {3, 62, 4, 16},

                        {2, 64, 4, 17},
                        {2, 66, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 69, 6, 19},
                        {4, 70, 5, 20},
                        {4, 71, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            56, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 55-1, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 56, 6, 13},
                        {4, 58, 5, 14},
                        {4, 60, 4, 15},
                        {3, 62, 4, 16},

                        {2, 64, 4, 17},
                        {2, 66, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 69, 6, 19},
                        {4, 70, 5, 20},
                        {4, 71, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            34, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33-1, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 54, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 56, 6, 13},
                        {4, 58, 5, 14},
                        {4, 60, 4, 15},
                        {3, 62, 4, 16},

                        {2, 64, 4, 17},
                        {2, 66, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 69, 6, 19},
                        {4, 70, 5, 20},
                        {4, 71, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            29, TRUE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 32, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 54, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 56, 6, 13},
                        {4, 58, 5, 14},
                        {4, 60, 4, 15},
                        {3, 62, 4, 16},

                        {2, 64, 4, 17},
                        {2, 66, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 69, 6, 19},
                        {4, 70, 5, 20},
                        {4, 71, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            56, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 32, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 54-2, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 56, 6, 13},
                        {4, 58, 5, 14},
                        {4, 60, 4, 15},
                        {3, 62, 4, 16},

                        {2, 64, 4, 17},
                        {2, 66, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 69, 6, 19},
                        {4, 70, 5, 20},
                        {4, 71, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            4, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 32, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 52, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 56, 6, 13},
                        {4, 58, 5, 14},
                        {4, 60, 4, 15},
                        {3, 62, 4, 16},

                        {2, 64, 4, 17},
                        {2, 66, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 69, 6, 19},
                        {4, 70, 5, 20},
                        {4, 71, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            2, FALSE,
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 0,  0},

                        {4,  4, 6,  1},
                        {4,  6, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 22, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 29, 6,  7},
                        {4, 31, 5,  8},
                        {4, 32, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 52, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 56, 6, 13},
                        {4, 58, 5, 14},
                        {4, 60, 4, 15},
                        {3, 62, 4, 16},

                        {2, 64, 4, 17},
                        {2, 66, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 69, 6, 19},
                        {4, 70, 5, 20},
                        {4, 71, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },


    };


    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];

        result = DEVRM_ShiftPceToLeft(device_id, eg->hw_idx, eg->is_std_pce);
        assert(TRUE == result);

        DEVRM_UT_Proc_Validate_PCE_Status();

        for (j=0; j < _countof(eg->s_pcl); ++j)
        {
            struct setting_pcl *s_pcl = &eg->s_pcl[j];
            UI32_T __k;

            if (0 == s_pcl->pcl_id)
            {
                break;
            }

            for (__k = 0; __k < _countof(s_pcl->s_pce); ++__k)
            {
                struct setting_pce *s_pce = &s_pcl->s_pce[__k];

                if (0 == s_pce->upper_rule_id)
                {
                    continue;
                }

                result = DEVRM_GetPce(device_id, s_pce->upper_rule_id, &pce);
                assert(TRUE == result);
                assert(pce->hw_idx == s_pce->hw_idx);
            }
        }
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_AllocatePceWithPriority_Case_By_Case()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI32_T pcl_id;
        UI32_T vir_group_id;
        int    priority;
        UI32_T upper_rule_id;

        struct setting_pcl s_pcl[MAX_PCL_ENTRIES];
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {5,  0, 0,  0},

                {4,  6, 6,  1},
                {4,  8, 5,  2},
                {4, 10, 4,  3},
                {3, 20, 4,  4},

                {2, 24, 4,  5},
                {2, 26, 4,  6},

                {0},
            },
        },

        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {5,  0, 0,  0},

                {4, 30, 6,  7},
                {4, 31, 5,  8},
                {4, 33, 4,  9},
                {3, 34, 4, 10},

                {2, 40, 4, 11},
                {2, 59, 4, 12},

                {0},
            },
        },

        {
            6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
            {
                {5,  0, 0,  0},

                {4, 60, 6, 13},
                {4, 64, 5, 14},
                {4, 66, 4, 15},
                {3, 70, 4, 16},

                {2, 72, 4, 17},
                {2, 74, 4, 18},

                {0},
            },
        },

        {
            9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {5,  0, 0,  0},

                {4, 76, 6, 19},
                {4, 77, 5, 20},
                {4, 78, 4, 21},
                {3, 79, 4, 22},

                {2, 95, 4, 23},
                {2, 96, 4, 24},

                {0},
            },
        },

    };

    struct experimental_group exp_group[] =
    {
        {
            1, 5, 4, 50, //pcl_id  vir_group_id  priority  upper_rule_id
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 4, 50},

                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 24, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            1, 5, 4, 51, //pcl_id  vir_group_id  priority  upper_rule_id
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 4, 50},
                        {5,  2, 4, 51}, //

                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 24, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            1, 5, 4, 52, //pcl_id  vir_group_id  priority  upper_rule_id
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 4, 50},
                        {5,  2, 4, 51},
                        {5,  4, 4, 52}, //

                        {4,  6, 6,  1},
                        {4,  8, 5,  2},
                        {4, 10, 4,  3},
                        {3, 20, 4,  4},

                        {2, 24, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            1, 5, 4, 53, //pcl_id  vir_group_id  priority  upper_rule_id
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 4, 50},
                        {5,  2, 4, 51},
                        {5,  4, 4, 52},
                        {5,  6, 4, 53}, //

                        {4,  6+2, 6,  1},
                        {4,  8+2, 5,  2},
                        {4, 10+2, 4,  3},
                        {3, 20, 4,  4},

                        {2, 24, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            1, 5, 8, 54, //pcl_id  vir_group_id  priority  upper_rule_id
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 8, 54}, //
                        {5,  0+2, 4, 50},
                        {5,  2+2, 4, 51},
                        {5,  4+2, 4, 52},
                        {5,  6+2, 4, 53},

                        {4,  8+2, 6,  1},
                        {4, 10+2, 5,  2},
                        {4, 12+2, 4,  3},
                        {3, 20, 4,  4},

                        {2, 24, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            1, 5, 8, 55, //pcl_id  vir_group_id  priority  upper_rule_id
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 8, 54},
                        {5,  2, 8, 55},
                        {5,  2+2, 4, 50},
                        {5,  4+2, 4, 51},
                        {5,  6+2, 4, 52},
                        {5,  8+2, 4, 53},

                        {4, 10+2, 6,  1},
                        {4, 12+2, 5,  2},
                        {4, 14+2, 4,  3},
                        {3, 20, 4,  4},

                        {2, 24, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            1, 5, 2, 56, //pcl_id  vir_group_id  priority  upper_rule_id
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 8, 54},
                        {5,  2, 8, 55},
                        {5,  4, 4, 50},
                        {5,  6, 4, 51},
                        {5,  8, 4, 52},
                        {5, 10, 4, 53},
                        {5, 12, 4, 56}, //

                        {4, 12+2, 6,  1},
                        {4, 14+2, 5,  2},
                        {4, 16+2, 4,  3},
                        {3, 20, 4,  4},

                        {2, 24, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            3, 4, 8, 57, //pcl_id  vir_group_id  priority  upper_rule_id
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 8, 54},
                        {5,  2, 8, 55},
                        {5,  4, 4, 50},
                        {5,  6, 4, 51},
                        {5,  8, 4, 52},
                        {5, 10, 4, 53},
                        {5, 12, 4, 56},

                        {4, 14, 6,  1},
                        {4, 16, 5,  2},
                        {4, 18, 4,  3},
                        {3, 20, 4,  4},

                        {2, 24, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30, 8, 57}, //
                        {4, 30+1, 6,  7},
                        {4, 31+1, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

        {
            3, 4, 10, 58, //pcl_id  vir_group_id  priority  upper_rule_id
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {5,  0, 8, 54},
                        {5,  2, 8, 55},
                        {5,  4, 4, 50},
                        {5,  6, 4, 51},
                        {5,  8, 4, 52},
                        {5, 10, 4, 53},
                        {5, 12, 4, 56},

                        {4, 14, 6,  1},
                        {4, 16, 5,  2},
                        {4, 18, 4,  3},
                        {3, 20, 4,  4},

                        {2, 24, 4,  5},
                        {2, 26, 4,  6},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 28, 10,58}, //
                        {4, 30, 8, 57},
                        {4, 31, 6,  7},
                        {4, 32, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

                {0},

            },
        },

    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];

        pcl = DEVRM_GetPcl(device_id, eg->pcl_id);
        assert(pcl != NULL);

        result = DEVRM_AllocatePceWithPriority(device_id, eg->pcl_id, eg->vir_group_id,
                                               eg->priority, pcl->pcl_type, &pce);
        assert(TRUE == result);

        pce->upper_rule_id = eg->upper_rule_id;

        DEVRM_UT_Proc_Validate_PCE_Status();

        for (j=0; j < _countof(eg->s_pcl); ++j)
        {
            struct setting_pcl *s_pcl = &eg->s_pcl[j];
            UI32_T __k;

            if (0 == s_pcl->pcl_id)
            {
                break;
            }

            for (__k = 0; __k < _countof(s_pcl->s_pce); ++__k)
            {
                struct setting_pce *s_pce = &s_pcl->s_pce[__k];

                if (0 == s_pce->upper_rule_id)
                {
                    continue;
                }

                result = DEVRM_GetPce(device_id, s_pce->upper_rule_id, &pce);
                assert(TRUE == result);
                assert(pce->hw_idx == s_pce->hw_idx);
            }
        }
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_AllocatePceWithPriority_Case_By_Case_Shift_Left()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI32_T pcl_id;
        UI32_T vir_group_id;
        int    priority;
        UI32_T upper_rule_id;

        struct setting_pcl s_pcl[MAX_PCL_ENTRIES];
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {9,  0, 0,  0},

                {8,  6, 4,  1},
                {8,  8, 4,  2},

                {4, 10, 4,  3},
                {4, 12, 4,  4},
                {4, 14, 4,  5},

                {0},
            },
        },

        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {5,  0, 0,  0},

                {4, 30, 6,  7},
                {4, 31, 5,  8},
                {4, 33, 4,  9},
                {3, 34, 4, 10},

                {2, 40, 4, 11},
                {2, 59, 4, 12},

                {0},
            },
        },

        {
            6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
            {
                {5,  0, 0,  0},

                {4, 60, 6, 13},
                {4, 64, 5, 14},
                {4, 66, 4, 15},
                {3, 70, 4, 16},

                {2, 72, 4, 17},
                {2, 74, 4, 18},

                {0},
            },
        },

        {
            9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
            {
                {5,  0, 0,  0},

                {4, 76, 6, 19},
                {4, 77, 5, 20},
                {4, 78, 4, 21},
                {3, 79, 4, 22},

                {2, 95, 4, 23},
                {2, 96, 4, 24},

                {0},
            },
        },

    };

    struct experimental_group exp_group[] =
    {
        {
            1, 8, 4, 50, //pcl_id  vir_group_id  priority  upper_rule_id
            {
                {
                    1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
                    {
                        {9,  0, 0,  0},

                        {8,  6-2, 4,  1},
                        {8,  8-2, 4,  2},

                        {7,  8, 4, 50}, //

                        {4, 10, 4,  3},
                        {4, 12, 4,  4},
                        {4, 14, 4,  5},

                        {0},
                    },
                },

                {
                    3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 30, 6,  7},
                        {4, 31, 5,  8},
                        {4, 33, 4,  9},
                        {3, 34, 4, 10},

                        {2, 40, 4, 11},
                        {2, 59, 4, 12},

                        {0},
                    },
                },

                {
                    6, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
                    {
                        {5,  0, 0,  0},

                        {4, 60, 6, 13},
                        {4, 64, 5, 14},
                        {4, 66, 4, 15},
                        {3, 70, 4, 16},

                        {2, 72, 4, 17},
                        {2, 74, 4, 18},

                        {0},
                    },
                },

                {
                    9, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    {
                        {5,  0, 0,  0},

                        {4, 76, 6, 19},
                        {4, 77, 5, 20},
                        {4, 78, 4, 21},
                        {3, 79, 4, 22},

                        {2, 95, 4, 23},
                        {2, 96, 4, 24},

                        {0},
                    },
                },

            },
        },

    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    for (i=0; i < _countof(exp_group); ++i)
    {
        struct experimental_group *eg = &exp_group[i];

        pcl = DEVRM_GetPcl(device_id, eg->pcl_id);
        assert(pcl != NULL);

        result = DEVRM_AllocatePceWithPriority(device_id, eg->pcl_id, eg->vir_group_id,
                                               eg->priority, pcl->pcl_type, &pce);
        assert(TRUE == result);

        pce->upper_rule_id = eg->upper_rule_id;

        DEVRM_UT_Proc_Validate_PCE_Status();

        for (j=0; j < _countof(eg->s_pcl); ++j)
        {
            struct setting_pcl *s_pcl = &eg->s_pcl[j];
            UI32_T __k;

            if (0 == s_pcl->pcl_id)
            {
                break;
            }

            for (__k = 0; __k < _countof(s_pcl->s_pce); ++__k)
            {
                struct setting_pce *s_pce = &s_pcl->s_pce[__k];

                if (0 == s_pce->upper_rule_id)
                {
                    continue;
                }

                result = DEVRM_GetPce(device_id, s_pce->upper_rule_id, &pce);
                assert(TRUE == result);
                assert(pce->hw_idx == s_pce->hw_idx);
            }
        }
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_AllocatePceWithPriority_Capacity_Test_Grew_From_HwIndex_0()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,

        PCL_ID = 3,
        VIR_GROUP_ID = 5,
        PRIORITY = 10,
        START_UPPER_RULE_ID = 0,

        RETRY_COUNT = 2,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    UI32_T  upper_rule_id;
    UI32_T pcl_id;
    UI32_T vir_group_id;
    int priority;
    UI32_T retry_count;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI32_T hw_idx;
        BOOL_T is_std_pce;

        struct setting_pcl s_pcl[MAX_PCL_ENTRIES];
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

        {
            4, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

        {
            5, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    retry_count = RETRY_COUNT;

retry:
    upper_rule_id = START_UPPER_RULE_ID;
    pcl_id = PCL_ID;
    vir_group_id = VIR_GROUP_ID;
    priority = PRIORITY;

    while (TRUE)
    {

        pcl = DEVRM_GetPcl(device_id, pcl_id);
        assert(NULL != pcl);

        result = DEVRM_AllocatePceWithPriority(device_id, pcl_id, vir_group_id, priority,
                                               pcl->pcl_type, &pce);

        if (FALSE == result)
        {
            break;
        }

        pce->upper_rule_id = ++upper_rule_id;

        DEVRM_UT_Proc_Validate_PCE_Status();
    }

    assert(upper_rule_id == DEVRM_MAX_NBR_OF_EXT_RULE);

    do
    {
        result = DEVRM_GetPce(device_id, upper_rule_id, &pce);
        assert(TRUE == result);
        assert(NULL != pce);

        result = DEVRM_FreePce(device_id, pcl_id, pce->is_std_pce, pce);
        assert(TRUE == result);

        DEVRM_UT_Proc_Validate_PCE_Status();

    } while ( 0 < --upper_rule_id);


    if (0 < retry_count)
    {
        --retry_count;

        goto retry;
    }


    DEVRM_DestroyAllAcl(device_id);
    DEVRM_UT_Proc_Validate_PCE_Status();

    return 0;
}

int
DEVRM_UT_AllocatePceWithPriority_Capacity_Test_Grew_From_HwIndex_Mid()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,

        PCL_ID = 3,
        VIR_GROUP_ID = 5,
        PRIORITY = 10,
        START_UPPER_RULE_ID = 2,

        RETRY_COUNT = 2,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    UI32_T  upper_rule_id;
    UI32_T pcl_id;
    UI32_T vir_group_id;
    int priority;
    UI32_T retry_count;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI32_T hw_idx;
        BOOL_T is_std_pce;

        struct setting_pcl s_pcl[MAX_PCL_ENTRIES];
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {VIR_GROUP_ID, 100, PRIORITY, 1},
                {0},
            },
        },

        {
            4, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

        {
            5, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    retry_count = RETRY_COUNT;

retry:
    upper_rule_id = START_UPPER_RULE_ID;
    pcl_id = PCL_ID;
    vir_group_id = VIR_GROUP_ID;
    priority = PRIORITY;

    while (TRUE)
    {
        pcl = DEVRM_GetPcl(device_id, pcl_id);
        assert(NULL != pcl);

        result = DEVRM_AllocatePceWithPriority(device_id, pcl_id, vir_group_id, priority,
                                               pcl->pcl_type, &pce);

        if (FALSE == result)
        {
            break;
        }

        pce->upper_rule_id = upper_rule_id++;

        DEVRM_UT_Proc_Validate_PCE_Status();
    }

    upper_rule_id--;

    assert(upper_rule_id == DEVRM_MAX_NBR_OF_EXT_RULE);

    do
    {
        result = DEVRM_GetPce(device_id, upper_rule_id, &pce);
        assert(TRUE == result);
        assert(NULL != pce);

        result = DEVRM_FreePce(device_id, pcl_id, pce->is_std_pce, pce);
        assert(TRUE == result);

        DEVRM_UT_Proc_Validate_PCE_Status();

        if (upper_rule_id == START_UPPER_RULE_ID)
        {
            break;
        }

    } while ( 0 < --upper_rule_id);

    if (0 < retry_count)
    {
        --retry_count;

        goto retry;
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_AllocatePceWithPriority_Capacity_Test_Grew_From_HwIndex_Last()
{
    enum {
        MAX_PCE_ENTRIES = 30,
        MAX_PCL_ENTRIES = 30,

        LAST_HW_INDEX = DEVRM_MAX_NBR_OF_TCAM_ENTRIES-2,

        PCL_ID = 3,
        VIR_GROUP_ID = 5,
        PRIORITY = 10,
        START_UPPER_RULE_ID = 2,

        RETRY_COUNT = 2,
    };

    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;
    DEVRM_PCE_T *pce;

    UI32_T  device_id = 0;
    UI32_T  i;
    UI32_T  j;
    BOOL_T  result;

    UI32_T  upper_rule_id;
    UI32_T pcl_id;
    UI32_T vir_group_id;
    int priority;
    UI32_T retry_count;

    struct setting_pce
    {
        UI32_T vir_group_id;
        UI32_T hw_idx;
        int    priority;
        UI32_T upper_rule_id;
    };

    struct setting_pcl
    {
        UI32_T pcl_id;
        BOOL_T is_ingress;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type;

        struct setting_pce s_pce[MAX_PCE_ENTRIES];
    };

    struct experimental_group
    {
        UI32_T hw_idx;
        BOOL_T is_std_pce;

        struct setting_pcl s_pcl[MAX_PCL_ENTRIES];
    };

    struct setting_pcl setting[] =
    {
        {
            1, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

        {
            2, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

        {
            3, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {VIR_GROUP_ID, LAST_HW_INDEX, PRIORITY, 1},
                {0},
            },
        },

        {
            4, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

        {
            5, TRUE, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
            {
                {0},
            },
        },

    };

    for (i = 0; i < _countof(setting); ++i)
    {
        struct setting_pcl *s_pcl = &setting[i];
        result = DEVRM_CreatePcl(device_id,
                                 s_pcl->pcl_id,
                                 s_pcl->is_ingress,
                                 DEVRM_PCL_LOOKUP_0_0,
                                 s_pcl->pce_type);

        assert(TRUE == result);

        for (j=0; j < _countof(s_pcl->s_pce); ++j)
        {
            struct setting_pce *s_pce = &s_pcl->s_pce[j];
            if (0 == s_pce->vir_group_id)
            {
                break;
            }

            vg = DEVRM_InsertVirtualGroup(device_id, s_pcl->pcl_id, s_pce->vir_group_id);

            assert(NULL != vg);

            if (0 != s_pce->upper_rule_id)
            {
                result = DEVRM_AssignPceAtHwIndex(device_id, s_pce->hw_idx, s_pcl->pce_type);
                assert(TRUE == result);

                pce = DEVRM_MallocPce(s_pcl->pcl_id, s_pce->vir_group_id,
                                      s_pce->hw_idx, s_pcl->pce_type, s_pce->priority);
                assert(NULL != pce);

                pce->upper_rule_id = s_pce->upper_rule_id;

                result = DEVRM_PLL_InsertPce(device_id, s_pcl->pcl_id, s_pce->vir_group_id, pce);
                assert(TRUE == result);
            }

            pcl = DEVRM_GetPcl(device_id, s_pcl->pcl_id);
            assert(NULL != pcl);


            DEVRM_UT_Proc_Validate_PCE_Status();
        }
    }

    retry_count = RETRY_COUNT;

retry:
    upper_rule_id = START_UPPER_RULE_ID;
    pcl_id = PCL_ID;
    vir_group_id = VIR_GROUP_ID;
    priority = PRIORITY;

    while (TRUE)
    {
        pcl = DEVRM_GetPcl(device_id, pcl_id);
        assert(NULL != pcl);

        result = DEVRM_AllocatePceWithPriority(device_id, pcl_id, vir_group_id, priority,
                                               pcl->pcl_type, &pce);

        if (FALSE == result)
        {
            break;
        }

        pce->upper_rule_id = upper_rule_id++;

        DEVRM_UT_Proc_Validate_PCE_Status();
    }

    upper_rule_id--;

    assert(upper_rule_id == DEVRM_MAX_NBR_OF_EXT_RULE);

    do
    {
        result = DEVRM_GetPce(device_id, upper_rule_id, &pce);
        assert(TRUE == result);
        assert(NULL != pce);

        result = DEVRM_FreePce(device_id, pcl_id, pce->is_std_pce, pce);
        assert(TRUE == result);

        DEVRM_UT_Proc_Validate_PCE_Status();

        if (upper_rule_id == START_UPPER_RULE_ID)
        {
            break;
        }

    } while ( 0 < --upper_rule_id);

    if (0 < retry_count)
    {
        --retry_count;

        goto retry;
    }

    DEVRM_DestroyAllAcl(device_id);

    return 0;
}

int
DEVRM_UT_SetRule_With_Meter_Capacity_Test()
{
    enum
    {
        DEVICE_ID = 0,
        INGRESS_PCL_ID = 1,
        EGRESS_PCL_ID = 2,
        DEV_RULE_ID = 10,
        RETRY_COUNT = 5,
        MAX_INGRESS_METER_ID = _countof(DEVRM_DEVICE_PTR(DEVICE_ID)->policer_use_status)-1,
        MAX_EGRESS_METER_ID = _countof(DEVRM_DEVICE_PTR(DEVICE_ID)->egress_policer_use_status)-1,
    };

    UI32_T  device_id  = DEVICE_ID;
    UI32_T  pcl_id;
    UI32_T  vir_group_id = 1;
    int     rule_pri   = 5;
    UI32_T  dev_rule_id;

    BOOL_T  is_ingress;
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num;
    UI32_T  pce_type;
    BOOL_T  result;

    UI32_T  policer_idx;

    UI32_T  i;

    DEVRM_PCE_T         *pce;

    DEVRM_AceEntry_T        ace_entry           = {0};
    DEVRM_ACTION_T          action;

    DEVRM_LIB_ActionInit(&action);
    DEVRM_LIB_ActionMeterConfigFlow(&action, 1000, 1000);

    /*
     *
     * suppose the number of meter is less than rule
     *
     *
     */

    pcl_id      = INGRESS_PCL_ID;
    is_ingress  = TRUE;
    lookup_num  = DEVRM_PCL_LOOKUP_0_0;
    pce_type    = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
    dev_rule_id = DEV_RULE_ID;
    policer_idx = DEVRM_METER_INDEX_BASE;


    result = DEVRM_CreatePcl(device_id, pcl_id, is_ingress, lookup_num, pce_type);
    assert(result == TRUE);

    // allocate rule with meter ...
    while (TRUE)
    {
        result = DEVRM_AllocateRule(device_id, pcl_id, vir_group_id, rule_pri, dev_rule_id);
        assert(result == TRUE);

        result = DEVRM_SetRule(device_id, pcl_id, dev_rule_id, &ace_entry,
                               &action);
        if (result == FALSE)
        {
            result = DEVRM_DestroyRule(device_id, pcl_id, dev_rule_id);
            assert(result == TRUE);

            dev_rule_id -= 1;
            break;
        }

        DEVRM_GetPce(device_id, dev_rule_id, &pce);
        assert(NULL != pce);

        // Validates internal OM
        assert(-1 != pce->policer_idx);
        assert((UI32_T)pce->policer_idx == policer_idx);
        assert(DEVRM_DEVICE_PTR(device_id)->policer_use_status[policer_idx] == 1);

        policer_idx += 1;
        dev_rule_id += 1;
    }

    assert(0 < policer_idx);

    policer_idx -= 1;
    assert(policer_idx == MAX_INGRESS_METER_ID);
    assert( (dev_rule_id - DEV_RULE_ID) == (policer_idx - DEVRM_METER_INDEX_BASE) );

    do
    {
        DEVRM_GetPce(device_id, dev_rule_id, &pce);
        assert(NULL != pce);

        result = DEVRM_DestroyRule(device_id, pcl_id, dev_rule_id);
        assert(TRUE == result);

        // Validates internal OM
        assert(DEVRM_DEVICE_PTR(device_id)->policer_use_status[policer_idx] == 0);

    } while (dev_rule_id -- > DEV_RULE_ID);

    assert(dev_rule_id == DEV_RULE_ID-1);

    for (i=0; i<_countof(DEVRM_DEVICE_PTR(device_id)->policer_use_status); ++i)
    {
        assert(DEVRM_DEVICE_PTR(device_id)->policer_use_status[i] == 0);
    }

#if (0 < DEVRM_MAX_NBR_OF_EGRESS_METER_ENTRIES)

    pcl_id      = EGRESS_PCL_ID;
    is_ingress  = FALSE;
    lookup_num  = DEVRM_PCL_LOOKUP_0_0;
    pce_type    = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
    dev_rule_id = DEV_RULE_ID;
    policer_idx = DEVRM_METER_INDEX_BASE;

    result = DEVRM_CreatePcl(device_id, pcl_id, is_ingress, lookup_num, pce_type);
    assert(result == TRUE);

    // allocate rule with meter ...
    while (TRUE)
    {
        result = DEVRM_AllocateRule(device_id, pcl_id, vir_group_id, rule_pri, dev_rule_id);
        assert(result == TRUE);

        result = DEVRM_SetRule(device_id, pcl_id, dev_rule_id,
                               &ace_entry,
                               &action);
        if (result == FALSE)
        {
            result = DEVRM_DestroyRule(device_id, pcl_id, dev_rule_id);
            assert(result == TRUE);
            dev_rule_id -= 1;
            break;
        }

        DEVRM_GetPce(device_id, dev_rule_id, &pce);
        assert(NULL != pce);

        assert(-1 != pce->policer_idx);
        assert((UI32_T)pce->policer_idx == policer_idx);
        assert(DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status[policer_idx] == 1);

        policer_idx += 1;
        dev_rule_id += 1;
    }

    assert(0 < policer_idx);

    policer_idx -= 1;
    assert(policer_idx == MAX_EGRESS_METER_ID);
    assert( (dev_rule_id - DEV_RULE_ID) == (policer_idx - DEVRM_METER_INDEX_BASE) );

    do
    {
        result = DEVRM_DestroyRule(device_id, pcl_id, dev_rule_id);
        assert(TRUE == result);
    } while (dev_rule_id -- > DEV_RULE_ID);

    assert(dev_rule_id == DEV_RULE_ID-1);

    for (i=0; i<_countof(DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status); ++i)
    {
        assert(DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status[i] == 0);
    }

#endif // #if (0 < DEVRM_MAX_NBR_OF_EGRESS_METER_ENTRIES)

    return 0;
}


int
DEVRM_UT_SetRule_With_Counter()
{
    enum
    {
        DEVRM_PCL_ID = 2,
        DEV_RULE_ID = 10,
        RETRY_COUNT = 5
    };

    UI32_T  device_id  = 0;
    UI32_T  pcl_id     = DEVRM_PCL_ID;
    UI32_T  vir_group_id = 1;
    int     rule_pri   = 5;
    UI32_T  dev_rule_id = DEV_RULE_ID;

    BOOL_T  is_ingress = TRUE;
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num = DEVRM_PCL_LOOKUP_0_0;
    UI32_T  pce_type   = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;

    UI32_T  policer_idx;

    BOOL_T  result;

    UI32_T  retry;

    DEVRM_PCL_T             *pcl;
    DEVRM_PCE_T             *pce;

    DEVRM_AceEntry_T        ace_entry = {0};
    DEVRM_ACTION_T          dev_action;

    DEVRM_LIB_ActionInit(&dev_action);
    DEVRM_LIB_ActionMeterConfigFlow(&dev_action, 1000, 1000);
    DEVRM_LIB_ActionCounterConfig(&dev_action, DEVRM_POLICER_COUNTER_MODE_PACKET);

    /*
     *
     * suppose the number of meter is less than rule
     *
     *
     */

    result = DEVRM_CreatePcl(device_id, pcl_id, is_ingress, lookup_num, pce_type);
    assert(result == TRUE);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    assert(NULL != pcl);


    retry = RETRY_COUNT;

    policer_idx = DEVRM_METER_INDEX_BASE;

    // allocate rule without meter ...
    while (retry -- > 0)
    {
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT      ruleFormat;
        CPSS_DXCH_PCL_RULE_FORMAT_UNT           mask;
        CPSS_DXCH_PCL_RULE_FORMAT_UNT           pattern;
        CPSS_DXCH_PCL_ACTION_STC                action;

        CPSS_DXCH_POLICER_STAGE_TYPE_ENT        policerStage;
        CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    billingCntr;

        GT_STATUS                               rc;

        UI32_T                                  ruleIndex;
        UI32_T                                  green_octets;
        UI32_T                                  yellow_octets;
        UI32_T                                  red_octets;

        result = DEVRM_AllocateRule(device_id, pcl_id, vir_group_id, rule_pri, dev_rule_id);
        assert(result == TRUE);

        result = DEVRM_SetRule(device_id, pcl_id, dev_rule_id,
                               &ace_entry,
                               &dev_action);
        assert(result == TRUE);

        DEVRM_GetPce(device_id, dev_rule_id, &pce);
        assert(NULL != pce);

        policer_idx = pce->policer_idx;

        assert(-1 != pce->policer_idx);
        assert(policer_idx == (UI32_T)pce->policer_idx);

        ruleIndex = DEVRM_Translate_PCEIDwithType(pce_type, pce->hw_idx);

        rc = _cpssDxChPclRuleGet(device_id, _cpssRuleFormatToRuleSize(pce_type), ruleIndex,
                                 &ruleFormat, &mask, &pattern, &action);
        assert(GT_OK == rc);

        assert(action.policer.policerEnable == CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E);

        assert(-1 != pce->policer_idx);
        assert(action.policer.policerId == (GT_U32)pce->policer_idx);

        DEVRM_GetPolicerStageFromPce(device_id, pce, &policerStage);

        rc = cpssDxCh3PolicerBillingEntryGet(device_id, policerStage,
                                             action.policer.policerId, GT_FALSE,
                                             &billingCntr);
        assert(GT_OK == rc);

        assert(billingCntr.billingCntrMode == CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E);


        result = DEVRM_GetCounter(device_id, pcl_id, dev_rule_id, &green_octets, &yellow_octets,
                                  &red_octets);
        assert(TRUE == result);
        assert(green_octets == yellow_octets == red_octets == 0);

        policer_idx += 1;
        dev_rule_id += 1;
    }


    retry = RETRY_COUNT;

    // allocate rule without meter ...
    while (retry -- > 0)
    {
        result = DEVRM_DestroyRule(device_id, pcl_id, --dev_rule_id);
        assert(result == TRUE);
    }

    return 0;
}

int
DEVRM_UT_Debug_Output_For_Converting_Rule_to_Pce()
{
    DEVRM_AceEntry_T dev_ace_entry;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT pattern;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT mask;

    memset(&dev_ace_entry, 0, sizeof(dev_ace_entry));

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_SRCPORT;
    dev_ace_entry.srcport_data = 1;
    dev_ace_entry.srcport_mask = 0xff;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_TAGGED;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_VID;
    dev_ace_entry.outvlan_data  = 1000;
    dev_ace_entry.outvlan_mask  = 0xfff;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_VTAGUP;
    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_UP;
    dev_ace_entry.vtagup_data   = 5;
    dev_ace_entry.vtagup_mask   = 0xff;

//    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_QOSPROFILE;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ETHER;
    dev_ace_entry.ethertype_data = 0x0800;
    dev_ace_entry.ethertype_mask = 0xffff;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ENCAP;
    dev_ace_entry.packetformat_data = ETHERTYPE_ENCAPS;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_MACDA;
    dev_ace_entry.dstmac_data[0] = 0x01;
    dev_ace_entry.dstmac_data[1] = 0x02;
    dev_ace_entry.dstmac_data[2] = 0x03;
    dev_ace_entry.dstmac_data[3] = 0x04;
    dev_ace_entry.dstmac_data[4] = 0x05;
    dev_ace_entry.dstmac_data[5] = 0x06;

    dev_ace_entry.dstmac_mask[0] = 0xff;
    dev_ace_entry.dstmac_mask[1] = 0xff;
    dev_ace_entry.dstmac_mask[2] = 0xff;
    dev_ace_entry.dstmac_mask[3] = 0xff;
    dev_ace_entry.dstmac_mask[4] = 0xff;
    dev_ace_entry.dstmac_mask[5] = 0xff;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_MACSA;
    dev_ace_entry.srcmac_data[0] = 0x01;
    dev_ace_entry.srcmac_data[1] = 0x02;
    dev_ace_entry.srcmac_data[2] = 0x03;
    dev_ace_entry.srcmac_data[3] = 0x04;
    dev_ace_entry.srcmac_data[4] = 0x05;
    dev_ace_entry.srcmac_data[5] = 0x06;

    dev_ace_entry.srcmac_mask[0] = 0x01;
    dev_ace_entry.srcmac_mask[1] = 0x02;
    dev_ace_entry.srcmac_mask[2] = 0x03;
    dev_ace_entry.srcmac_mask[3] = 0x04;
    dev_ace_entry.srcmac_mask[4] = 0x05;
    dev_ace_entry.srcmac_mask[5] = 0x06;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL;
    dev_ace_entry.ipprotocol_data = 6;
    dev_ace_entry.ipprotocol_mask = 0xff;

//    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_DSCP;
//    dev_ace_entry.dscp_data = 42;
//    dev_ace_entry.dscp_mask = 0xff;
//
//    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_TOS;
    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_PREC;
    dev_ace_entry.prec_data = 3;
    dev_ace_entry.prec_mask = 0xff;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_TCPCONTROL;
    dev_ace_entry.tcpcontrol_data = 11;
    dev_ace_entry.tcpcontrol_mask = 0xff;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_L4SRCPORT;
    dev_ace_entry.l4_srcport_data = 80;
    dev_ace_entry.l4_srcport_mask = 0xffff;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_L4DSTPORT;
    dev_ace_entry.l4_dstport_data = 53;
    dev_ace_entry.l4_dstport_mask = 0xffff;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ISARP;
    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV6EHEXIST;
    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV6EHHOPBYHOP;
    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPFRAG;
    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ISBC;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV4SIP;
    dev_ace_entry.srcip_data    = 0x01020304;
    dev_ace_entry.srcip_mask    = 0xffffffff;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV4DIP;
    dev_ace_entry.dstip_data    = 0x01020304;
    dev_ace_entry.dstip_mask    = 0xffffffff;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV6SIP;
    dev_ace_entry.srcip6_data[0] = 0x11;
    dev_ace_entry.srcip6_data[1] = 0x12;
    dev_ace_entry.srcip6_data[2] = 0x13;
    dev_ace_entry.srcip6_data[3] = 0x14;
    dev_ace_entry.srcip6_data[4] = 0x15;
    dev_ace_entry.srcip6_data[5] = 0x16;
    dev_ace_entry.srcip6_data[6] = 0x17;
    dev_ace_entry.srcip6_data[7] = 0x18;
    dev_ace_entry.srcip6_data[8] = 0x19;
    dev_ace_entry.srcip6_data[9] = 0x1a;
    dev_ace_entry.srcip6_data[10] = 0x1b;
    dev_ace_entry.srcip6_data[11] = 0x1c;
    dev_ace_entry.srcip6_data[12] = 0x1d;
    dev_ace_entry.srcip6_data[13] = 0x1e;
    dev_ace_entry.srcip6_data[14] = 0x1f;
    dev_ace_entry.srcip6_data[15] = 0x1f;

    memset(&dev_ace_entry.srcip6_mask, 0xff, sizeof(dev_ace_entry.srcip6_mask));

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV6DIP;
    dev_ace_entry.dstip6_data[0] = 0x01;
    dev_ace_entry.dstip6_data[1] = 0x02;
    dev_ace_entry.dstip6_data[2] = 0x03;
    dev_ace_entry.dstip6_data[3] = 0x04;
    dev_ace_entry.dstip6_data[4] = 0x05;
    dev_ace_entry.dstip6_data[5] = 0x06;
    dev_ace_entry.dstip6_data[6] = 0x07;
    dev_ace_entry.dstip6_data[7] = 0x08;
    dev_ace_entry.dstip6_data[8] = 0x09;
    dev_ace_entry.dstip6_data[9] = 0x0a;
    dev_ace_entry.dstip6_data[10] = 0x0b;
    dev_ace_entry.dstip6_data[11] = 0x0c;
    dev_ace_entry.dstip6_data[12] = 0x0d;
    dev_ace_entry.dstip6_data[13] = 0x0e;
    dev_ace_entry.dstip6_data[14] = 0x0f;
    dev_ace_entry.dstip6_data[15] = 0x0f;

    memset(&dev_ace_entry.dstip6_mask, 0xff, sizeof(dev_ace_entry.dstip6_mask));

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IPV6DIPUPPER;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_IP6TRAFFICCLASS;
    dev_ace_entry.ip6trafficclass_data = 20;
    dev_ace_entry.ip6trafficclass_mask = 0xff;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE;
    dev_ace_entry.icmptype_data = 3;
    dev_ace_entry.icmptype_mask = 0xff;

    dev_ace_entry.entry_qualify |= DEVRM_FIELD_QUALIFY_MASK_PCL_ID;
    dev_ace_entry.pcl_id = 128;
    dev_ace_entry.pcl_id_mask = 128;

    devrm_debug_flag = 0;

//    DEVRM_RuleToIngStdIpL2Qos(&dev_ace_entry, &mask, &pattern);
//    DEVRM_RuleToIngExtIpv6L2(&dev_ace_entry, &mask, &pattern);
//    DEVRM_RuleToIngStdIpv4L4(&dev_ace_entry, &mask, &pattern);
    DEVRM_RuleToEgrExtNotIpv6(&dev_ace_entry, &mask, &pattern);

    return 0;
}

int
DEVRM_UT_BindUnbindPclToPort()
{
    UI32_T device_id = 0;
    UI32_T pcl_id = 1;
    UI32_T is_ingress = TRUE;
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num = DEVRM_PCL_LOOKUP_0_0;
    UI32_T rule_type = RULE_TYPE_MAC_ACL;
    BOOL_T b_result;

    UI32_T  phy_port = 1;

    b_result = DEVRM_CreatePcl(device_id, pcl_id, is_ingress, lookup_num, rule_type);
    assert(b_result == TRUE);

//    DEVRM_Backdoor_EnableDebugMode();

    b_result = DEVRM_BindPclToPort(device_id, phy_port, pcl_id);
    assert(b_result == TRUE);

    DEVRM_UT_Proc_Validate_IsPclConfigTableEnable(TRUE, FALSE, device_id, phy_port, pcl_id);

    b_result = DEVRM_UnbindPclToPort(device_id, phy_port, pcl_id);
    assert(b_result == TRUE);

    DEVRM_UT_Proc_Validate_IsPclConfigTableEnable(FALSE, FALSE, device_id, phy_port, pcl_id);

//    DEVRM_Backdoor_EnableDebugMode();

    return 0;
}

int
DEVRM_UT_RunTestCaese()
{
#define DEVRM_TEST(func)    \
    UT_Test(DEVRM_UT_Setup, DEVRM_UT_Teardown, func)

    if (l4_options.run_dev_rm)
    {

//      DEVRM_TEST(DEVRM_UT_Test_Validate_Function_If_Failed_Is_Right);


        DEVRM_TEST(DEVRM_UT_AllocatePce_Can_Not_Allocate_PCE_On_Non_Exist_PCL);

        DEVRM_TEST(DEVRM_UT_AllocatePce_FreePce_Ext_Global_Rule);


        DEVRM_TEST(DEVRM_UT_GetPce_Capacity_Test_Standard_PCE_1_PCE_1_PCL);

        DEVRM_TEST(DEVRM_UT_GetPce_Capacity_Test_Standard_PCE_2_PCE_1_PCL);

        DEVRM_TEST(DEVRM_UT_GetNextPce_For_Standard_Rule);

        DEVRM_TEST(DEVRM_UT_GetNextPce_For_Extended_Rule);

        DEVRM_TEST(DEVRM_UT_GetPrevious_For_Standard_Rule);

        DEVRM_TEST(DEVRM_UT_GetPreviousPce_Extended_1_PCE);

        DEVRM_TEST(DEVRM_UT_GetPreviousPce_Extended_2_PCE);

        DEVRM_TEST(DEVRM_UT_GetPreviousPce_Extended_3_PCE);

        DEVRM_TEST(DEVRM_UT_GetPceAtHwIndex_Full_of_Standard_PCE);

        DEVRM_TEST(DEVRM_UT_GetPceAtHwIndex_Full_of_Extended_PCE);

        DEVRM_TEST(DEVRM_UT_GetPceAtHwIndex_Mixed_PCE_1_Extended_2_Standard);

        DEVRM_TEST(DEVRM_UT_GetPceAtHwIndex_Mixed_PCE_1_Standard_1_Extended);

        DEVRM_TEST(DEVRM_UT_DestroyAllAcl_For_Standard_Rule);

        DEVRM_TEST(DEVRM_UT_DestroyAllAcl_For_Extended_Rule);



        DEVRM_TEST(DEVRM_UT_InsertVirtualGroup);

        DEVRM_TEST(DEVRM_UT_FindPreviousVirtualGroup_Case_By_Case_1);

        DEVRM_TEST(DEVRM_UT_FindPreviousVirtualGroup_Case_By_Case_2);

        DEVRM_TEST(DEVRM_UT_InsertPce_Test_Each_Linked_List_Condition);

        DEVRM_TEST(DEVRM_UT_InsertPce_Capacity_Test);

        DEVRM_TEST(DEVRM_UT_GetHwIndex);



        DEVRM_TEST(DEVRM_UT_FindEmptyHwIndex_Case_By_Case_1);

        DEVRM_TEST(DEVRM_UT_FindEmptyHwIndex_Case_By_Case_2);

        DEVRM_TEST(DEVRM_UT_FindEmptyHwIndex_Case_By_Case_3);

        DEVRM_TEST(DEVRM_UT_FindEmptyHwIndex_Case_By_Case_4);

        DEVRM_TEST(DEVRM_UT_FindEmptyHwIndex_Case_By_Case_5);

        DEVRM_TEST(DEVRM_UT_FindEmptyHwIndexReverse_Case_By_Case_1);

        DEVRM_TEST(DEVRM_UT_FindEmptyHwIndexReverse_Case_By_Case_2);

        DEVRM_TEST(DEVRM_UT_FindEmptyHwIndexReverse_Case_By_Case_3);

        DEVRM_TEST(DEVRM_UT_FindEmptyHwIndexReverse_Case_By_Case_4);

        DEVRM_TEST(DEVRM_UT_FindEmptyHwIndexReverse_Case_By_Case_5);

        DEVRM_TEST(DEVRM_UT_ShiftToRight_Case_By_Case_For_Extended_Rule);

        DEVRM_TEST(DEVRM_UT_ShiftToRight_Capacity_Test_For_Extended_Rule);

        DEVRM_TEST(DEVRM_UT_ShiftToLeft_Capacity_Test_For_Extended_Rule);

        DEVRM_TEST(DEVRM_UT_ShiftToRight_Case_By_Case_For_Mixed_Rule);

        DEVRM_TEST(DEVRM_UT_ShiftToLeft_Case_By_Case_For_Mixed_Rule);



        DEVRM_TEST(DEVRM_UT_AllocatePceWithPriority_Case_By_Case);
//      DEVRM_TEST(DEVRM_UT_AllocatePceWithPriority_Case_By_Case_Shift_Left);

        DEVRM_TEST(DEVRM_UT_AllocatePceWithPriority_Capacity_Test_Grew_From_HwIndex_0);

        DEVRM_TEST(DEVRM_UT_AllocatePceWithPriority_Capacity_Test_Grew_From_HwIndex_Mid);

        DEVRM_TEST(DEVRM_UT_AllocatePceWithPriority_Capacity_Test_Grew_From_HwIndex_Last);

        DEVRM_TEST(DEVRM_UT_SetRule_With_Meter_Capacity_Test);

        DEVRM_TEST(DEVRM_UT_SetRule_With_Counter);

        DEVRM_TEST(DEVRM_UT_Debug_Output_For_Converting_Rule_to_Pce);

        DEVRM_TEST(DEVRM_UT_BindUnbindPclToPort);
    }
    else
    {
        printf("%s %d: Skip devrm test!!! \r\n", __FUNCTION__, __LINE__);
    }

    //
    // Register validate function
    //
    if (0)
    {
        devrm_validate_rule_func = DEVRM_UT_Proc_Validate_PCE_Status_Complete;
    }
    else if (0)
    {
        devrm_validate_rule_func = DEVRM_UT_Proc_Validate_PCE_Status;
    }
    else
    {
        printf("%s %d: No register validation function !!! \r\n", __FUNCTION__, __LINE__);
    }

    return 0;

#undef DEVRM_TEST
}

