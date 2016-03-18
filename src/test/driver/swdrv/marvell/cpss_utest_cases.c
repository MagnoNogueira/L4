/*
 *  cpss_utest_cases.c
 *  rule_ctrl
 *
 *  Created by yehjunying on 2011/5/2.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "utest.h"

int
CPSS_UT_Setup()
{
    _cpssInit();
    return 0;
}

int
CPSS_UT_Teardown()
{
    return 0;
}

int
CPSS_UT_cpssDxChPclRuleSet_Capacity_Test_For_Standard_Rule()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleIndex;

#ifdef MARVELL_CPSS_3_4
    CPSS_DXCH_PCL_RULE_OPTION_ENT       ruleOptionsBmp = 0;
#endif

    CPSS_DXCH_PCL_RULE_FORMAT_UNT       pattern = {0};
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       mask = {0};
    CPSS_DXCH_PCL_ACTION_STC            action = {0};
    GT_STATUS                           result;
    GT_U32                              ruleCount;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    ruleIndex = 0;
    ruleCount = 0;

    while (1)
    {
        assert(cpss_db[devNum].rule_tbl[ruleIndex].valid == GT_FALSE);

        result = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex,
#ifdef MARVELL_CPSS_3_4
                                    ruleOptionsBmp,
#endif
                                    &mask, &pattern, &action);

        if (result != GT_OK) {
            break;
        }

        assert(cpss_db[devNum].rule_tbl[ruleIndex].valid == GT_TRUE);

        ruleIndex ++;
        ruleCount ++;
    }

    assert(ruleCount == CPSS_MAX_NUM_RULE);

    return 0;
}

int
CPSS_UT_cpssDxChPclRuleSet_Capacity_Test_For_Extended_Rule()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleIndex;

#ifdef MARVELL_CPSS_3_4
    CPSS_DXCH_PCL_RULE_OPTION_ENT       ruleOptionsBmp = 0;
#endif

    CPSS_DXCH_PCL_RULE_FORMAT_UNT       pattern = {0};
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       mask = {0};
    CPSS_DXCH_PCL_ACTION_STC            action = {0};
    GT_STATUS                           result;
    GT_U32                              ruleCount;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
    ruleIndex = 0;
    ruleCount = 0;

    while (1)
    {
        assert(cpss_db[devNum].rule_tbl[ruleIndex*2].valid == GT_FALSE);

        result = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex,
#ifdef MARVELL_CPSS_3_4
                                    ruleOptionsBmp,
#endif
                                    &mask, &pattern, &action);

        if (result != GT_OK) {
            break;
        }

        assert(cpss_db[devNum].rule_tbl[ruleIndex*2].valid == GT_TRUE);

        ruleIndex ++;
        ruleCount ++;
    }

    assert(ruleCount == CPSS_MAX_NUM_RULE/2);

    return 0;
}

int
CPSS_UT_cpssDxChPclRuleSet_Convert_Standard_Rule_To_Extended()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleIndex;

#ifdef MARVELL_CPSS_3_4
    CPSS_DXCH_PCL_RULE_OPTION_ENT       ruleOptionsBmp = 0;
#endif

    CPSS_DXCH_PCL_RULE_FORMAT_UNT       pattern = {0};
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       mask = {0};
    CPSS_DXCH_PCL_ACTION_STC            action = {0};
    GT_STATUS                           result;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    ruleIndex = 0;

    while (1)
    {
        assert(cpss_db[devNum].rule_tbl[ruleIndex].valid == GT_FALSE);

        result = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex,
#ifdef MARVELL_CPSS_3_4
                                    ruleOptionsBmp,
#endif
                                    &mask, &pattern, &action);

        if (result != GT_OK) {
            break;
        }

        assert(cpss_db[devNum].rule_tbl[ruleIndex].valid == GT_TRUE);

        ruleIndex += 2;
    }

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;

    for (ruleIndex=0; ruleIndex < CPSS_MAX_NUM_RULE/2; ++ruleIndex)
    {
        result = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex,
#ifdef MARVELL_CPSS_3_4
                                    ruleOptionsBmp,
#endif
                                    &mask, &pattern, &action);

        assert(result == GT_OK);
    }

    return 0;
}

int
CPSS_UT_cpssDxChPclRuleSet_Convert_Extended_Rule_To_Standard()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleIndex;

#ifdef MARVELL_CPSS_3_4
    CPSS_DXCH_PCL_RULE_OPTION_ENT       ruleOptionsBmp = 0;
#endif

    CPSS_DXCH_PCL_RULE_FORMAT_UNT       pattern = {0};
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       mask = {0};
    CPSS_DXCH_PCL_ACTION_STC            action = {0};
    GT_STATUS                           result;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
    ruleIndex = 0;

    while (1)
    {
        assert(cpss_db[devNum].rule_tbl[ruleIndex*2].valid == GT_FALSE);

        result = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex,
#ifdef MARVELL_CPSS_3_4
                                    ruleOptionsBmp,
#endif
                                    &mask, &pattern, &action);

        if (result != GT_OK) {
            break;
        }

        assert(cpss_db[devNum].rule_tbl[ruleIndex*2].valid == GT_TRUE);

        ruleIndex ++;
    }

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

    for (ruleIndex=0; ruleIndex < CPSS_MAX_NUM_RULE; ++ruleIndex)
    {
        result = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex,
#ifdef MARVELL_CPSS_3_4
                                    ruleOptionsBmp,
#endif
                                    &mask, &pattern, &action);

        assert(result == GT_OK);
    }

    return 0;
}

int
CPSS_UT_cpssDxChPclRuleSet_Rule_Size_Doesnt_Match_For_Standard_Rule()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleIndex;

#ifdef MARVELL_CPSS_3_4
    CPSS_DXCH_PCL_RULE_OPTION_ENT       ruleOptionsBmp = 0;
#endif

    CPSS_DXCH_PCL_RULE_FORMAT_UNT       pattern = {0};
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       mask = {0};
    CPSS_DXCH_PCL_ACTION_STC            action = {0};
    GT_STATUS                           result;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    ruleIndex = 1;

    while (1)
    {
        assert(cpss_db[devNum].rule_tbl[ruleIndex].valid == GT_FALSE);

        result = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex,
#ifdef MARVELL_CPSS_3_4
                                    ruleOptionsBmp,
#endif
                                    &mask, &pattern, &action);

        if (result != GT_OK) {
            break;
        }

        assert(cpss_db[devNum].rule_tbl[ruleIndex].valid == GT_TRUE);

        ruleIndex += 2;
    }

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;

    for (ruleIndex=0; ruleIndex < CPSS_MAX_NUM_RULE/2; ++ruleIndex)
    {
        result = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex,
#ifdef MARVELL_CPSS_3_4
                                    ruleOptionsBmp,
#endif
                                    &mask, &pattern, &action);

        assert(result != GT_OK);
    }

    return 0;
}

int
CPSS_UT_cpssDxChPclRuleSet_Rule_Size_Doesnt_Match_For_Extended_Rule()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleIndex;

#ifdef MARVELL_CPSS_3_4
    CPSS_DXCH_PCL_RULE_OPTION_ENT       ruleOptionsBmp = 0;
#endif

    CPSS_DXCH_PCL_RULE_FORMAT_UNT       pattern = {0};
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       mask = {0};
    CPSS_DXCH_PCL_ACTION_STC            action = {0};
    GT_STATUS                           result;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
    ruleIndex = 0;

    while (1)
    {
        assert(cpss_db[devNum].rule_tbl[ruleIndex*2].valid == GT_FALSE);

        result = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex,
#ifdef MARVELL_CPSS_3_4
                                    ruleOptionsBmp,
#endif
                                    &mask, &pattern, &action);

        if (result != GT_OK) {
            break;
        }

        assert(cpss_db[devNum].rule_tbl[ruleIndex*2].valid == GT_TRUE);

        ruleIndex ++;
    }

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

    for (ruleIndex=1; ruleIndex < CPSS_MAX_NUM_RULE; ruleIndex+=2)
    {
        result = cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex,
#ifdef MARVELL_CPSS_3_4
                                    ruleOptionsBmp,
#endif
                                    &mask, &pattern, &action);

        assert(result != GT_OK);
    }

    return 0;
}

int
CPSS_UT_cpssDxChPclRuleValidStatusSet_Capacity_Test_For_Standard_Rule()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleIndex;
    GT_STATUS                           result;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    ruleIndex = 0;

    while (1)
    {
        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleFormat, ruleIndex);

        if (_countof(cpss_db[devNum].rule_tbl) <= tcamIndex)
        {
            break;
        }

        cpss_db[devNum].rule_tbl[tcamIndex].valid = GT_TRUE;
        cpss_db[devNum].rule_tbl[tcamIndex].ruleFormat = ruleFormat;

        ruleIndex ++;
    }

    for (ruleIndex=0; ruleIndex < CPSS_MAX_NUM_RULE; ruleIndex++)
    {
        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleFormat, ruleIndex);

        result = cpssDxChPclRuleValidStatusSet(devNum,
                                               _cpssRuleFormatToRuleSize(ruleFormat),
                                               ruleIndex,
                                               GT_FALSE);

        assert(result == GT_OK);
        assert(cpss_db[devNum].rule_tbl[tcamIndex].valid == GT_FALSE);
    }

    return 0;
}

int
CPSS_UT_cpssDxChPclRuleValidStatusSet_Capacity_Test_For_Extended_Rule()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleIndex;
    GT_STATUS                           result;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
    ruleIndex = 0;

    while (1)
    {
        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleFormat, ruleIndex);

        if (_countof(cpss_db[devNum].rule_tbl) <= tcamIndex)
        {
            break;
        }

        cpss_db[devNum].rule_tbl[tcamIndex].valid = GT_TRUE;
        cpss_db[devNum].rule_tbl[tcamIndex].ruleFormat = ruleFormat;

        ruleIndex ++;
    }

    for (ruleIndex=0; ruleIndex < CPSS_MAX_NUM_RULE/2; ruleIndex++)
    {
        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleFormat, ruleIndex);

        result = cpssDxChPclRuleValidStatusSet(devNum,
                                    _cpssRuleFormatToRuleSize(ruleFormat),
                                    ruleIndex,
                                    GT_FALSE);

        assert(result == GT_OK);
        assert(cpss_db[devNum].rule_tbl[tcamIndex].valid == GT_FALSE);
    }

    return 0;
}

/*
 *        The invalidation of the rule is performed by next steps:
 *        1. ...
 *        2. ... If the given the rule found with size
 *        different from the given rule-size an error code returned.
 *        ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 */
int
CPSS_UT_cpssDxChPclRuleValidStatusSet_Standard_Conflict()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleIndex;
    GT_STATUS                           result;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    ruleIndex = 0;

    while (1)
    {
        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleFormat, ruleIndex);

        if (_countof(cpss_db[devNum].rule_tbl) <= tcamIndex)
        {
            break;
        }

        cpss_db[devNum].rule_tbl[tcamIndex].valid = GT_TRUE;
        cpss_db[devNum].rule_tbl[tcamIndex].ruleFormat = ruleFormat;

        ruleIndex ++;
    }

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
    for (ruleIndex=0; ruleIndex < CPSS_MAX_NUM_RULE/2; ruleIndex++)
    {
        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleFormat, ruleIndex);

        result = cpssDxChPclRuleValidStatusSet(devNum,
                                               _cpssRuleFormatToRuleSize(ruleFormat),
                                               ruleIndex,
                                               GT_FALSE);

        assert(result != GT_OK);
        assert(cpss_db[devNum].rule_tbl[tcamIndex].valid == GT_TRUE);
    }

    return 0;
}

/*
 *        The invalidation of the rule is performed by next steps:
 *        1. ...
 *        2. ... If the given the rule found with size
 *        different from the given rule-size an error code returned.
 *        ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 */
int
CPSS_UT_cpssDxChPclRuleValidStatusSet_Extended_Conflict()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleIndex;
    GT_STATUS                           result;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
    ruleIndex = 0;

    while (1)
    {
        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleFormat, ruleIndex);

        if (_countof(cpss_db[devNum].rule_tbl) <= tcamIndex)
        {
            break;
        }

        cpss_db[devNum].rule_tbl[tcamIndex].valid = GT_TRUE;
        cpss_db[devNum].rule_tbl[tcamIndex].ruleFormat = ruleFormat;

        ruleIndex ++;
    }

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    for (ruleIndex=0; ruleIndex < CPSS_MAX_NUM_RULE; ruleIndex++)
    {
        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleFormat, ruleIndex);

        result = cpssDxChPclRuleValidStatusSet(devNum,
                                               _cpssRuleFormatToRuleSize(ruleFormat),
                                               ruleIndex,
                                               GT_FALSE);

        assert(result != GT_OK);
        if ((tcamIndex % 2) == 0)
        {
        	assert(cpss_db[devNum].rule_tbl[tcamIndex].valid == GT_TRUE);
        }
    }

    return 0;
}

int
CPSS_UT_cpssDxChPclRuleCopy_Capacity_Test_For_Standard_Rule()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleSrcIndex;
    GT_U32                              ruleDstIndex;
    GT_STATUS                           result;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    ruleSrcIndex = 0;

    cpss_db[devNum].rule_tbl[0].valid = GT_TRUE;
    cpss_db[devNum].rule_tbl[0].ruleFormat = ruleFormat;

    for (ruleDstIndex = 1; ruleDstIndex < CPSS_MAX_NUM_RULE; ++ruleDstIndex)
    {
        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleFormat, ruleDstIndex);

        assert(cpss_db[devNum].rule_tbl[tcamIndex].valid == GT_FALSE);

        result = cpssDxChPclRuleCopy(devNum,
                                     _cpssRuleFormatToRuleSize(ruleFormat),
                                     ruleSrcIndex,
                                     ruleDstIndex);

        assert(result == GT_OK);
        assert(cpss_db[devNum].rule_tbl[tcamIndex].valid == GT_TRUE);
    }

    // boundary test
    ruleDstIndex = CPSS_MAX_NUM_RULE + 1;
    result = cpssDxChPclRuleCopy(devNum,
                                 _cpssRuleFormatToRuleSize(ruleFormat),
                                 ruleSrcIndex,
                                 ruleDstIndex);
    assert(result != GT_OK);

    return 0;
}

int
CPSS_UT_cpssDxChPclRuleCopy_Capacity_Test_For_Extended_Rule()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleSrcIndex;
    GT_U32                              ruleDstIndex;
    GT_STATUS                           result;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
    ruleSrcIndex = 0;

    cpss_db[devNum].rule_tbl[0].valid = GT_TRUE;
    cpss_db[devNum].rule_tbl[0].ruleFormat = ruleFormat;

    for (ruleDstIndex = 1; ruleDstIndex < CPSS_MAX_NUM_RULE/2; ++ruleDstIndex)
    {
        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleFormat, ruleDstIndex);

        assert(cpss_db[devNum].rule_tbl[tcamIndex].valid == GT_FALSE);

        result = cpssDxChPclRuleCopy(devNum,
                                     _cpssRuleFormatToRuleSize(ruleFormat),
                                     ruleSrcIndex,
                                     ruleDstIndex);

        assert(result == GT_OK);
        assert(cpss_db[devNum].rule_tbl[tcamIndex].valid == GT_TRUE);
    }

    // boundary test
    ruleDstIndex = CPSS_MAX_NUM_RULE/2 + 1;
    result = cpssDxChPclRuleCopy(devNum,
                                 _cpssRuleFormatToRuleSize(ruleFormat),
                                 ruleSrcIndex,
                                 ruleDstIndex);
    assert(result != GT_OK);

    return 0;
}

int
CPSS_UT_cpssDxChPclRuleCopy_Bad_Dst_RuleIndex_For_Standard_Rule()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleSrcIndex;
    GT_U32                              ruleDstIndex;
    GT_STATUS                           result;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    ruleSrcIndex = 0;

    //
    // copy standard rule to new space which be occupied by standard rule
    //
    memset(cpss_db[devNum].rule_tbl, 0, sizeof(cpss_db[devNum].rule_tbl));

    cpss_db[devNum].rule_tbl[0].valid = GT_TRUE;
    cpss_db[devNum].rule_tbl[0].ruleFormat = ruleFormat;

    for (ruleDstIndex = 1; ruleDstIndex < CPSS_MAX_NUM_RULE; ++ruleDstIndex)
    {
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleDstFormat =
                                CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleDstFormat,
                                                      ruleDstIndex);

        cpss_db[devNum].rule_tbl[tcamIndex].valid = GT_TRUE;
        cpss_db[devNum].rule_tbl[tcamIndex].ruleFormat = ruleDstFormat;
    }

    for (ruleDstIndex = 1; ruleDstIndex < CPSS_MAX_NUM_RULE; ++ruleDstIndex)
    {
        result = cpssDxChPclRuleCopy(devNum,
                                     _cpssRuleFormatToRuleSize(ruleFormat),
                                     ruleSrcIndex,
                                     ruleDstIndex);

        assert(result != GT_OK);
    }

    //
    // copy standard rule to new space which be occupied by extended rule
    //
    memset(cpss_db[devNum].rule_tbl, 0, sizeof(cpss_db[devNum].rule_tbl));

    cpss_db[devNum].rule_tbl[0].valid = GT_TRUE;
    cpss_db[devNum].rule_tbl[0].ruleFormat = ruleFormat;

    for (ruleDstIndex = 1; ruleDstIndex < CPSS_MAX_NUM_RULE/2; ++ruleDstIndex)
    {
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleDstFormat =
                                CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;

        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleDstFormat,
                                                      ruleDstIndex);

        cpss_db[devNum].rule_tbl[tcamIndex].valid = GT_TRUE;
        cpss_db[devNum].rule_tbl[tcamIndex].ruleFormat = ruleDstFormat;
    }

    for (ruleDstIndex = 1; ruleDstIndex < CPSS_MAX_NUM_RULE; ++ruleDstIndex)
    {
        result = cpssDxChPclRuleCopy(devNum,
                                     _cpssRuleFormatToRuleSize(ruleFormat),
                                     ruleSrcIndex,
                                     ruleDstIndex);

        if (ruleDstIndex == 1)
        {
            assert(result == GT_OK);
        }
        else
        {
            assert(result != GT_OK);
        }
    }

    return 0;
}

int
CPSS_UT_cpssDxChPclRuleCopy_Bad_Dst_RuleIndex_For_Extended_Rule()
{
    GT_U8                               devNum = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleSrcIndex;
    GT_U32                              ruleDstIndex;
    GT_STATUS                           result;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
    ruleSrcIndex = 0;

    //
    // copy extended rule to new space which be occupied by standard rule (1)
    //
    memset(cpss_db[devNum].rule_tbl, 0, sizeof(cpss_db[devNum].rule_tbl));

    cpss_db[devNum].rule_tbl[0].valid = GT_TRUE;
    cpss_db[devNum].rule_tbl[0].ruleFormat = ruleFormat;

    for (ruleDstIndex = 2; ruleDstIndex < CPSS_MAX_NUM_RULE; ++ruleDstIndex)
    {
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleDstFormat =
                                CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleDstFormat,
                                                      ruleDstIndex);

        cpss_db[devNum].rule_tbl[tcamIndex].valid = GT_TRUE;
        cpss_db[devNum].rule_tbl[tcamIndex].ruleFormat = ruleDstFormat;
    }

    for (ruleDstIndex = 2; ruleDstIndex < CPSS_MAX_NUM_RULE; ++ruleDstIndex)
    {
        result = cpssDxChPclRuleCopy(devNum,
                                     _cpssRuleFormatToRuleSize(ruleFormat),
                                     ruleSrcIndex,
                                     ruleDstIndex);

        assert(result != GT_OK);
    }

    //
    // copy extended rule to new space which be occupied by standard rule(2)
    //
    memset(cpss_db[devNum].rule_tbl, 0, sizeof(cpss_db[devNum].rule_tbl));

    cpss_db[devNum].rule_tbl[0].valid = GT_TRUE;
    cpss_db[devNum].rule_tbl[0].ruleFormat = ruleFormat;

    for (ruleDstIndex = 2; ruleDstIndex < CPSS_MAX_NUM_RULE; ++ruleDstIndex)
    {
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleDstFormat =
                                CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleDstFormat,
                                                      ruleDstIndex);

        if (tcamIndex %2)
        {
            cpss_db[devNum].rule_tbl[tcamIndex].valid = GT_TRUE;
            cpss_db[devNum].rule_tbl[tcamIndex].ruleFormat = ruleDstFormat;
        }
    }

    for (ruleDstIndex = 2; ruleDstIndex < CPSS_MAX_NUM_RULE; ++ruleDstIndex)
    {
        result = cpssDxChPclRuleCopy(devNum,
                                     _cpssRuleFormatToRuleSize(ruleFormat),
                                     ruleSrcIndex,
                                     ruleDstIndex);

        assert(result != GT_OK);
    }

    //
    // copy extended rule to new space which be occupied by standard rule(3)
    //
    memset(cpss_db[devNum].rule_tbl, 0, sizeof(cpss_db[devNum].rule_tbl));

    cpss_db[devNum].rule_tbl[0].valid = GT_TRUE;
    cpss_db[devNum].rule_tbl[0].ruleFormat = ruleFormat;

    for (ruleDstIndex = 2; ruleDstIndex < CPSS_MAX_NUM_RULE; ++ruleDstIndex)
    {
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleDstFormat =
        CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleDstFormat,
                                                      ruleDstIndex);

        if ((tcamIndex %2) == 0)
        {
            cpss_db[devNum].rule_tbl[tcamIndex].valid = GT_TRUE;
            cpss_db[devNum].rule_tbl[tcamIndex].ruleFormat = ruleDstFormat;
        }
    }

    for (ruleDstIndex = 2; ruleDstIndex < CPSS_MAX_NUM_RULE; ++ruleDstIndex)
    {
        result = cpssDxChPclRuleCopy(devNum,
                                     _cpssRuleFormatToRuleSize(ruleFormat),
                                     ruleSrcIndex,
                                     ruleDstIndex);

        assert(result != GT_OK);
    }

    //
    // copy extended rule to new space which be occupied by extended rule
    //
    memset(cpss_db[devNum].rule_tbl, 0, sizeof(cpss_db[devNum].rule_tbl));

    cpss_db[devNum].rule_tbl[0].valid = GT_TRUE;
    cpss_db[devNum].rule_tbl[0].ruleFormat = ruleFormat;

    for (ruleDstIndex = 1; ruleDstIndex < CPSS_MAX_NUM_RULE/2; ++ruleDstIndex)
    {
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleDstFormat =
                                CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;

        GT_U32 tcamIndex = _cpssRuleFormatToTcamIndex(ruleDstFormat,
                                                      ruleDstIndex);

        cpss_db[devNum].rule_tbl[tcamIndex].valid = GT_TRUE;
        cpss_db[devNum].rule_tbl[tcamIndex].ruleFormat = ruleDstFormat;
    }

    for (ruleDstIndex = 1; ruleDstIndex < CPSS_MAX_NUM_RULE/2; ++ruleDstIndex)
    {
        result = cpssDxChPclRuleCopy(devNum,
                                     _cpssRuleFormatToRuleSize(ruleFormat),
                                     ruleSrcIndex,
                                     ruleDstIndex);

        assert(result != GT_OK);
    }


    return 0;
}

int
CPSS_UT_RunTestCaese()
{
    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleSet_Capacity_Test_For_Standard_Rule);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleSet_Capacity_Test_For_Extended_Rule);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleSet_Convert_Standard_Rule_To_Extended);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleSet_Convert_Extended_Rule_To_Standard);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleSet_Rule_Size_Doesnt_Match_For_Standard_Rule);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleSet_Rule_Size_Doesnt_Match_For_Extended_Rule);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleValidStatusSet_Capacity_Test_For_Standard_Rule);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleValidStatusSet_Capacity_Test_For_Extended_Rule);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleValidStatusSet_Standard_Conflict);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleValidStatusSet_Extended_Conflict);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleCopy_Capacity_Test_For_Standard_Rule);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleCopy_Capacity_Test_For_Extended_Rule);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleCopy_Bad_Dst_RuleIndex_For_Standard_Rule);

    UT_Test(CPSS_UT_Setup, CPSS_UT_Teardown,
            CPSS_UT_cpssDxChPclRuleCopy_Bad_Dst_RuleIndex_For_Extended_Rule);

    return 0;
}
