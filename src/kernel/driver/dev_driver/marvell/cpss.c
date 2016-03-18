#include <stdio.h>
#include <memory.h>


#include "cpss.h"

#include <mainPpDrv/h/cpss/generic/config/private/prvCpssConfigTypes.h>
#include <mainPpDrv/h/cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>

#ifdef _MSC_VER
#pragma warning(disable:4100)   /* unreferenced formal parameter */
#endif

#define CPSS_MAX_NUM_DEV                    2
#define CPSS_MAX_NUM_VLAN                   4096
#define CPSS_MAX_NUM_PORT                   CPSS_MAX_PORTS_NUM_CNS //64
#define CPSS_MAX_NUM_TRUNK                  64

/*
// AllenCat
#define CPSS_MAX_NUM_RULE                   (512)
#define CPSS_MAX_NUM_INGRESS_METER          (256)
#define CPSS_MAX_NUM_EGRESS_METER           0
#define CPSS_MAX_NUM_INGRESS_COUNTER        (256)

// PonCat
 #define CPSS_MAX_NUM_RULE                  (1024)
#define CPSS_MAX_NUM_INGRESS_METER          (256+28)
#define CPSS_MAX_NUM_EGRESS_METER           64
#define CPSS_MAX_NUM_INGRESS_COUNTER        (256+28)
*/

// BobCat
//#define CPSS_MAX_NUM_RULE                   (1024*3)
//#define CPSS_MAX_NUM_INGRESS_METER          (2048)
//#define CPSS_MAX_NUM_EGRESS_METER           (512)
//#define CPSS_MAX_NUM_INGRESS_COUNTER        (2048)

// Test
#define CPSS_MAX_NUM_RULE                   (640+512)
#define CPSS_MAX_NUM_INGRESS_METER          (300)
#define CPSS_MAX_NUM_EGRESS_METER           (300)
#define CPSS_MAX_NUM_INGRESS_COUNTER        (300)


typedef struct
{
    GT_BOOL                                 valid;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT      ruleFormat;
    GT_U32                                  ruleIndex;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT           mask;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT           pattern;
    CPSS_DXCH_PCL_ACTION_STC                action;
} CPSS_PCL_RULE_ENTRY;

typedef struct
{
    GT_BOOL                                 valid;
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC   entry;
} CPSS_POLICER_METERING_ENTRY;

typedef struct
{
    GT_BOOL                                 valid;
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    entry;
} CPSS_POLICER_COUNTER_ENTRY;

typedef struct
{
    PRV_CPSS_DXCH_PP_CONFIG_STC     cpssDxChPpConfig;

    GT_BOOL                         cpss_ingress_policy_status;
    GT_BOOL                         cpss_engress_policy_status;

    CPSS_DXCH_PCL_LOOKUP_CFG_STC    ipcl_cfg_tbl[2]    /* lookup0-0/0-1 and lookup1 */
                                    [CPSS_MAX_NUM_VLAN + CPSS_MAX_NUM_PORT + CPSS_MAX_NUM_TRUNK];

    CPSS_DXCH_PCL_LOOKUP_CFG_STC    epcl_cfg_tbl[CPSS_MAX_NUM_VLAN + CPSS_MAX_NUM_PORT];

    CPSS_PCL_RULE_ENTRY             rule_tbl[CPSS_MAX_NUM_RULE];

    CPSS_POLICER_METERING_ENTRY     metering_tbl[3] /* ingress0, ingress1, engress */
                                                [CPSS_MAX_NUM_INGRESS_METER];
    CPSS_POLICER_COUNTER_ENTRY      counter_tbl[3]  /* ingress0, ingress1, engress */
                                               [CPSS_MAX_NUM_INGRESS_COUNTER];

} CPSS_DB;

typedef struct
{
    GT_BOOL ignore_rule_copy;
} CPSS_DEBUG_FLAG;

void* prvCpssPpConfig[PRV_CPSS_MAX_PP_DEVICES_CNS];
APP_DEMO_SYS_CONFIG_STC appDemoSysConfig;

GT_U32                  appDemoPpConfigDevAmount;

static CPSS_DB          cpss_db[CPSS_MAX_NUM_DEV];
static CPSS_DEBUG_FLAG  _cpss_debug;


void _cpssInit()
{
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage;
    GT_U32 i;

    memset(&cpss_db, 0, sizeof(cpss_db));
    memset(&appDemoSysConfig, 0, sizeof(appDemoSysConfig));

    for (stage=0; stage < _countof(cpss_db[0].metering_tbl); ++stage)
    {
        for (i=0; i < _countof(cpss_db[0].metering_tbl[stage]); ++i)
        {
            CPSS_POLICER_METERING_ENTRY *meter = &cpss_db[0].metering_tbl[stage][i];
            meter->valid = GT_FALSE;
        }
    }


    appDemoSysConfig.firstDevNum = 0;
    appDemoPpConfigDevAmount = _countof(cpss_db);

    for (i=0; i < _countof(cpss_db); ++i)
    {
        cpss_db[i].cpssDxChPpConfig.genInfo.numOfPorts = CPSS_MAX_NUM_PORT;
        cpss_db[i].cpssDxChPpConfig.genInfo.trunkInfo.numberOfTrunks = CPSS_MAX_NUM_TRUNK;

        cpss_db[i].cpssDxChPpConfig.fineTuning.tableSize.policyTcamRaws = (CPSS_MAX_NUM_RULE / 4);
        cpss_db[i].cpssDxChPpConfig.fineTuning.tableSize.policersNum = CPSS_MAX_NUM_INGRESS_METER;
        cpss_db[i].cpssDxChPpConfig.fineTuning.tableSize.egressPolicersNum = CPSS_MAX_NUM_EGRESS_METER;

        cpss_db[i].cpssDxChPpConfig.fineTuning.featureInfo.iplrSecondStageSupported = GT_TRUE;

        cpss_db[i].cpssDxChPpConfig.policer.memSize[0] = CPSS_MAX_NUM_INGRESS_METER;
        cpss_db[i].cpssDxChPpConfig.policer.memSize[1] = CPSS_MAX_NUM_INGRESS_METER;
        cpss_db[i].cpssDxChPpConfig.policer.memSize[2] = CPSS_MAX_NUM_EGRESS_METER;

        prvCpssPpConfig[i] = &cpss_db[i].cpssDxChPpConfig.genInfo;
    }

    _cpss_debug.ignore_rule_copy = GT_FALSE;
}

CPSS_PCL_RULE_SIZE_ENT _cpssRuleFormatToRuleSize
(
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat
)
{
    switch (ruleFormat) {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
            return CPSS_PCL_RULE_SIZE_STD_E;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
            return CPSS_PCL_RULE_SIZE_EXT_E;

        default:
            assert(0);
            break;
    }

    return CPSS_PCL_RULE_SIZE_STD_E;
}

GT_U32 _cpssRuleSizeIndexToTcamIndex
(
 IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
 IN GT_U32                             ruleIndex
 )
{
    return (ruleSize == CPSS_PCL_RULE_SIZE_STD_E) ? ruleIndex : ruleIndex * 2;
}

GT_U32 _cpssRuleFormatToTcamIndex
(
   IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
   IN GT_U32                             ruleIndex
)
{
    return _cpssRuleSizeIndexToTcamIndex( _cpssRuleFormatToRuleSize(ruleFormat), ruleIndex );
}


/*******************************************************************************
 * cpssDxChPclRuleSet
 *
 * DESCRIPTION:
 *   The function sets the Policy Rule Mask, Pattern and Action
 *
 * APPLICABLE DEVICES:
 *        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
 *
 * NOT APPLICABLE DEVICES:
 *        None.
 *
 * INPUTS:
 *       devNum           - device number
 *       ruleFormat       - format of the Rule.
 *
 *       ruleIndex        - index of the rule in the TCAM. The rule index defines
 *                          order of action resolution in the cases of multiple
 *                          rules match with packet's search key. Action of the
 *                          matched rule with lowest index is taken in this case
 *                          With reference to Standard and Extended rules
 *                          indexes, the partitioning is as follows:
 *
 *                          For DxCh and DxCh2 devices:
 *                          - Standard rules.
 *                            Rule index may be in the range from 0 up to 1023.
 *                          - Extended rules.
 *                            Rule index may be in the range from 0 up to 511.
 *                          Extended rule consumes the space of two standard
 *                            rules:
 *                          - Extended rule with index  0 <= n <= 511
 *                            is placed in the space of two standard rules with
 *                            indexes n and n + 512.
 *
 *                          For DxCh3 device:
 *                          The TCAM has up to 3.5K (3584) rows.
 *                          See datasheet of particular device for TCAM size.
 *                          Each row can be used as:
 *                              - 4 standard rules
 *                              - 2 extended rules
 *                              - 1 extended and 2 standard rules
 *                              - 2 standard and 1 extended rules
 *                          The TCAM partitioning is as follows:
 *                          - Standard rules.
 *                            Rule index may be in the range from 0 up to 14335.
 *                          - Extended rules.
 *                            Rule index may be in the range from 0 up to 7167.
 *                            Extended rule consumes the space of two standard
 *                            rules:
 *                          - Extended rule with index  0 <= n < 3584
 *                            is placed in the space of two standard rules with
 *                            indexes n and n + 3584.
 *                          - Extended rule with index  3584 <= n < 7168
 *                            is placed in the space of two standard rules with
 *                            indexes n + 3584, n + 7168.
 *
 *                          For xCat and above devices:
 *                          See datasheet of particular device for TCAM size.
 *                          For example describe the TCAM that has 1/4K (256) rows.
 *                          Each row can be used as:
 *                              - 4 standard rules
 *                              - 2 extended rules
 *                              - 1 extended and 2 standard rules
 *                              - 2 standard and 1 extended rules
 *                          The TCAM partitioning is as follows:
 *                          - Standard rules.
 *                            Rule index may be in the range from 0 up to 1K (1024).
 *                          - Extended rules.
 *                            Rule index may be in the range from 0 up to 0.5K (512).
 *                            Extended rule consumes the space of two standard
 *                            rules:
 *                          - Extended rule with index 2n (even index)
 *                            is placed in the space of two standard rules with
 *                            indexes 4n and 4n + 1.
 *                          - Extended rule with index 2n+1 (odd index)
 *                            is placed in the space of two standard rules with
 *                            indexes 4n+2 and 4n + 3.
 *       ruleOptionsBmp   - Bitmap of rule's options.
 *                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
 *                          Samples:
 *                            ruleOptionsBmp = 0 - no options are defined.
 *                               Write rule to TCAM not using port-list format.
 *                               Rule state is valid.
 *                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E -
 *                               write all fields of rule to TCAM but rule's
 *                               state is invalid (no match during lookups).
 *                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E |
 *                                             CPSS_DXCH_PCL_RULE_OPTION_PORT_LIST_ENABLED_E -
 *                               write all fields of rule to TCAM using port-list format
 *                               but rule's state is invalid
 *                              (no match during lookups).
 *       maskPtr          - rule mask. The rule mask is AND styled one. Mask
 *                          bit's 0 means don't care bit (corresponding bit in
 *                          the pattern is not using in the TCAM lookup).
 *                          Mask bit's 1 means that corresponding bit in the
 *                          pattern is using in the TCAM lookup.
 *                          The format of mask is defined by ruleFormat
 *
 *       patternPtr       - rule pattern.
 *                          The format of pattern is defined by ruleFormat
 *
 *       actionPtr        - Policy rule action that applied on packet if packet's
 *                          search key matched with masked pattern.
 *
 * OUTPUTS:
 *       None
 *
 * RETURNS:
 *       GT_OK                    - on success
 *       GT_BAD_PARAM             - on wrong parameters
 *       GT_BAD_PTR               - on null pointer
 *       GT_OUT_OF_RANGE          - one of the parameters is out of range
 *       GT_HW_ERROR              - on hardware error
 *       GT_TIMEOUT  - after max number of retries checking if PP ready
 *       GT_NOT_APPLICABLE_DEVICE - on not applicable device
 *
 * COMMENTS:
 *       NONE
 *
 *
 *
 *******************************************************************************/
GT_STATUS cpssDxChPclRuleSet
(
    IN GT_U8                              devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN GT_U32                             ruleIndex,

#ifdef MARVELL_CPSS_3_4
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT      ruleOptionsBmp,
#endif

    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    CPSS_PCL_RULE_ENTRY *rule;
    GT_U32  tcamIndex = _cpssRuleFormatToTcamIndex(ruleFormat, ruleIndex);

    if (CPSS_MAX_NUM_RULE <= tcamIndex)
    {
        return GT_BAD_PARAM;
    }

    if (actionPtr->qos.qos.ingress.profileAssignIndex == GT_TRUE)
    {
        assert(actionPtr->qos.qos.ingress.profileIndex < BIT_7);
    }

    rule = &cpss_db[devNum].rule_tbl[tcamIndex];

    // conflict test
    if ( _cpssRuleFormatToRuleSize(ruleFormat) == CPSS_PCL_RULE_SIZE_EXT_E )
    {
        CPSS_PCL_RULE_ENTRY *next = &cpss_db[devNum].rule_tbl[tcamIndex + 1];

        if (next->valid == GT_TRUE)
        {
            assert(_cpssRuleFormatToRuleSize(next->ruleFormat) == CPSS_PCL_RULE_SIZE_STD_E);

            return GT_BAD_PARAM;
        }
    }
    else if (tcamIndex % 2)
    {
        CPSS_PCL_RULE_ENTRY *prev = &cpss_db[devNum].rule_tbl[tcamIndex - 1];

        if (prev->valid == GT_TRUE &&
            _cpssRuleFormatToRuleSize( prev->ruleFormat ) == CPSS_PCL_RULE_SIZE_EXT_E)
        {
            return GT_BAD_PARAM;
        }
    }

    rule->valid = GT_TRUE;
    rule->ruleFormat = ruleFormat;
    memcpy(&rule->mask, maskPtr, sizeof(rule->mask));
    memcpy(&rule->pattern, patternPtr, sizeof(rule->pattern));
    memcpy(&rule->action, actionPtr, sizeof(rule->action));

    return GT_OK;
}

/*******************************************************************************
 * cpssDxChPclRuleInvalidate
 *
 * DESCRIPTION:
 *   The function invalidates the Policy Rule.
 *   For DxCh and DxCh2 devices start indirect rule write operation
 *   with "garbage" pattern and mask content and "valid-flag" == 0
 *   For DxCh3 devices calculates the TCAM position by ruleSize and ruleIndex
 *   parameters and updates the first "valid" bit matching X/Y pair to (1,1)
 *   i.e. don't match any value. If origin of valid or invalid rule found at the
 *   specified TCAM position it will be invalid and available to back validation.
 *   If the specified TCAM position contains the not first 24 byte segment of
 *   extended rule, the rule also will be invalidated,
 *   but unavailable to back validation.
 *   If the garbage found in TCAM the X/Y bits will be updated to be as
 *   in invalid rule.
 *   The function does not check the TCAM contents.
 *   GT_OK will be returned in each of described cases.
 *
 * APPLICABLE DEVICES:  All DxCh Devices
 *
 * INPUTS:
 *       devNum          - device number
 *       ruleSize        - size of Rule.
 *       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
 *
 * OUTPUTS:
 *       None
 *
 * RETURNS:
 *       GT_OK                    - on success
 *       GT_BAD_PARAM             - on wrong parameters
 *       GT_HW_ERROR              - on hardware error
 *       GT_TIMEOUT  - after max number of retries checking if PP ready
 *       GT_NOT_APPLICABLE_DEVICE - on not applicable device
 *
 * COMMENTS:
 *       Backward compatible styled API.
 *       The cpssDxChPclRuleValidStatusSet recommended for using instead.
 *
 *******************************************************************************/
GT_STATUS cpssDxChPclRuleInvalidate
(
 IN GT_U8                              devNum,
 IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
 IN GT_U32                             ruleIndex
 )
{
    CPSS_PCL_RULE_ENTRY *rule;
    GT_U32 tcamIndex = _cpssRuleSizeIndexToTcamIndex(ruleSize, ruleIndex);

    if (CPSS_MAX_NUM_RULE <= tcamIndex)
    {
        return GT_BAD_PARAM;
    }

    rule = &cpss_db[devNum].rule_tbl[tcamIndex];

    rule->valid = GT_FALSE;
    return GT_OK;
}

/*******************************************************************************
 * cpssDxChPclRuleValidStatusSet
 *
 * DESCRIPTION:
 *     Validates/Invalidates the Policy rule.
 *        The validation of the rule is performed by next steps:
 *        1. Retrieve the content of the rule from PP TCAM
 *        2. Write content back to TCAM with Valid indication set.
 *           The function does not check content of the rule before
 *           write it back to TCAM
 *        The invalidation of the rule is performed by next steps:
 *        1. Retrieve the content of the rule from PP TCAM
 *        2. Write content back to TCAM with Invalid indication set.
 *        If the given the rule found already in needed valid state
 *        no write done. If the given the rule found with size
 *        different from the given rule-size an error code returned.
 *
 * APPLICABLE DEVICES:  All DxCh Devices
 *
 * INPUTS:
 *       devNum          - device number
 *       ruleSize        - size of Rule.
 *       ruleIndex       - index of the rule in the TCAM.
 *       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
 *
 * OUTPUTS:
 *       None
 *
 * RETURNS:
 *       GT_OK                    - on success
 *       GT_BAD_PARAM             - on wrong parameters
 *       GT_HW_ERROR              - on hardware error
 *       GT_BAD_STATE             - if in TCAM found rule of size different
 *                        from the specified
 *       GT_TIMEOUT  - after max number of retries checking if PP ready
 *       GT_NOT_APPLICABLE_DEVICE - on not applicable device
 *
 * COMMENTS:
 *
 *
 *******************************************************************************/
GT_STATUS cpssDxChPclRuleValidStatusSet
(
 IN GT_U8                              devNum,
 IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
 IN GT_U32                             ruleIndex,
 IN GT_BOOL                            status
 )
{
    CPSS_PCL_RULE_ENTRY *rule;
    GT_U32 tcamIndex = _cpssRuleSizeIndexToTcamIndex(ruleSize, ruleIndex);

    if (CPSS_MAX_NUM_RULE <= tcamIndex)
    {
        return GT_BAD_PARAM;
    }

    rule = &cpss_db[devNum].rule_tbl[tcamIndex];

    // conflict test
    if ( ruleSize == CPSS_PCL_RULE_SIZE_EXT_E )
    {
        CPSS_PCL_RULE_ENTRY *next = &cpss_db[devNum].rule_tbl[tcamIndex + 1];

        if (next->valid == GT_TRUE)
        {
            assert(_cpssRuleFormatToRuleSize(next->ruleFormat) == CPSS_PCL_RULE_SIZE_STD_E);

            return GT_BAD_PARAM;
        }
    }
    else if (tcamIndex % 2)
    {
        CPSS_PCL_RULE_ENTRY *prev = &cpss_db[devNum].rule_tbl[tcamIndex - 1];

        if (prev->valid == GT_TRUE &&
            _cpssRuleFormatToRuleSize( prev->ruleFormat ) == CPSS_PCL_RULE_SIZE_EXT_E)
        {
            return GT_BAD_PARAM;
        }
    }

    if (rule->valid == GT_TRUE)
    {
        if (_cpssRuleFormatToRuleSize(rule->ruleFormat) !=
            ruleSize)
        {
            return GT_BAD_PARAM;
        }
    }

    rule->valid = status;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclRuleAnyStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - size of rule
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
{
    CPSS_PCL_RULE_ENTRY *rule;
    GT_U32 tcamIndex = _cpssRuleSizeIndexToTcamIndex(ruleSize, ruleIndex);

    assert(validPtr);
    assert(ruleSizePtr);

    if (CPSS_MAX_NUM_RULE <= tcamIndex)
    {
        return GT_BAD_PARAM;
    }

    rule = &cpss_db[devNum].rule_tbl[tcamIndex];

    *validPtr = rule->valid;
    *ruleSizePtr = _cpssRuleFormatToRuleSize( rule->ruleFormat );

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclRuleGet
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in Hw format
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*
* OUTPUTS:
*       maskArr          - rule mask          - 12  words.
*       patternArr       - rule pattern       - 12  words.
*       actionArr        - Policy rule action - 4 words.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleGet
(
    IN  GT_U8                  devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 maskArr[],
    OUT GT_U32                 patternArr[],
    OUT GT_U32                 actionArr[]
)
{
    return GT_FAIL;
}

GT_STATUS _cpssDxChPclRuleGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT *ruleFormatPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    CPSS_PCL_RULE_ENTRY *rule;
    GT_U32 tcamIndex = _cpssRuleSizeIndexToTcamIndex(ruleSize, ruleIndex);

    assert(ruleFormatPtr);
    assert(maskPtr);
    assert(patternPtr);
    assert(actionPtr);

    if (CPSS_MAX_NUM_RULE <= tcamIndex)
    {
        return GT_BAD_PARAM;
    }

    rule = &cpss_db[devNum].rule_tbl[tcamIndex];

    *ruleFormatPtr = rule->ruleFormat;
    memcpy(maskPtr, &rule->mask, sizeof(*maskPtr));
    memcpy(patternPtr, &rule->pattern, sizeof(*patternPtr));
    memcpy(actionPtr, &rule->action, sizeof(*actionPtr));
    return GT_OK;
}


/*******************************************************************************
 * cpssDxChPclRuleCopy
 *
 * DESCRIPTION:
 *  The function copies the Rule's mask, pattern and action to new TCAM position.
 *  The source Rule is not invalidated by the function. To implement move Policy
 *  Rule from old position to new one at first cpssDxChPclRuleCopy should be
 *  called. And after this cpssDxChPclRuleInvalidate should be used to invalidate
 *  Rule in old position
 *
 * APPLICABLE DEVICES:  All DxCh Devices
 *
 * INPUTS:
 *       devNum           - device number
 *       ruleSize         - size of Rule.
 *       ruleSrcIndex     - index of the rule in the TCAM from which pattern,
 *                           mask and action are taken.
 *       ruleDstIndex     - index of the rule in the TCAM to which pattern,
 *                           mask and action are placed
 *
 * OUTPUTS:
 *       None
 *
 * RETURNS:
 *       GT_OK                    - on success
 *       GT_BAD_PARAM             - on wrong parameters
 *       GT_HW_ERROR              - on hardware error
 *       GT_TIMEOUT  - after max number of retries checking if PP ready
 *       GT_NOT_APPLICABLE_DEVICE - on not applicable device
 *
 * COMMENTS:
 *
 *
 *******************************************************************************/
GT_STATUS cpssDxChPclRuleCopy
(
 IN GT_U8                              devNum,
 IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
 IN GT_U32                             ruleSrcIndex,
 IN GT_U32                             ruleDstIndex
 )
{
    CPSS_PCL_RULE_ENTRY *ruleSrc;
    CPSS_PCL_RULE_ENTRY *ruleDst;
    GT_U32 tcamSrcIndex = _cpssRuleSizeIndexToTcamIndex(ruleSize, ruleSrcIndex);
    GT_U32 tcamDstIndex = _cpssRuleSizeIndexToTcamIndex(ruleSize, ruleDstIndex);

    if (CPSS_MAX_NUM_RULE <= tcamSrcIndex)
    {
        return GT_BAD_PARAM;
    }

    if (CPSS_MAX_NUM_RULE <= tcamDstIndex)
    {
        return GT_BAD_PARAM;
    }

    if (_cpss_debug.ignore_rule_copy == GT_TRUE)
    {
        return GT_OK;
    }


    ruleSrc = &cpss_db[devNum].rule_tbl[tcamSrcIndex];
    ruleDst = &cpss_db[devNum].rule_tbl[tcamDstIndex];

    if (ruleSrc == ruleDst)
    {
        return GT_BAD_PARAM;
    }

    if (ruleSrc->valid != GT_TRUE)
    {
        return GT_BAD_PARAM;
    }

    if (_cpssRuleFormatToRuleSize(ruleSrc->ruleFormat) != ruleSize)
    {
        return GT_BAD_PARAM;
    }

    if (ruleDst->valid == GT_TRUE)
    {
        return GT_BAD_PARAM;
    }

    if (ruleSize == CPSS_PCL_RULE_SIZE_EXT_E)
    {
        CPSS_PCL_RULE_ENTRY *dstNext = &cpss_db[devNum].rule_tbl[tcamDstIndex+1];

        if (dstNext->valid == GT_TRUE)
        {
            assert( _cpssRuleFormatToRuleSize(dstNext->ruleFormat) ==
                                                    CPSS_PCL_RULE_SIZE_STD_E );
            return GT_BAD_PARAM;
        }
    }
    else if (tcamDstIndex % 2)
    {
        CPSS_PCL_RULE_ENTRY *dstPrev = &cpss_db[devNum].rule_tbl[tcamDstIndex-1];

        if (dstPrev->valid == GT_TRUE &&
            _cpssRuleFormatToRuleSize(dstPrev->ruleFormat) == CPSS_PCL_RULE_SIZE_EXT_E)
        {
            return GT_BAD_PARAM;
        }
    }


    memcpy(ruleDst, ruleSrc, sizeof(*ruleDst));

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPclCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: port, VLAN, or index for setting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       For DxCh2 and above - full support (beside the Stadard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Stadard IPV6 DIP key allowed only on lookup1
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblSet
(
    IN GT_U8                           devNum,
    IN CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    GT_U32                          ix;

    if (CPSS_MAX_NUM_DEV < devNum)
    {
        return GT_FAIL;
    }

    switch (interfaceInfoPtr->type)
    {
    case CPSS_INTERFACE_PORT_E:
        ix = CPSS_MAX_NUM_VLAN + interfaceInfoPtr->devPort.portNum;
        break;

    case CPSS_INTERFACE_TRUNK_E:
        ix = CPSS_MAX_NUM_VLAN + interfaceInfoPtr->trunkId;
        //printf("%d\n", interfaceInfoPtr->trunkId);
        break;

    default:
        return GT_FAIL;
    }

    if (CPSS_PCL_DIRECTION_EGRESS_E == direction &&
        CPSS_PCL_LOOKUP_0_E != lookupNum)
    {
        return GT_FAIL;
    }

    if (CPSS_PCL_DIRECTION_INGRESS_E == direction)
    {
        memcpy(&cpss_db[devNum].ipcl_cfg_tbl[lookupNum][ix], lookupCfgPtr, sizeof(*lookupCfgPtr));
    }
    else
    {
        if (_countof(cpss_db[devNum].epcl_cfg_tbl) <= ix)
        {
            assert(0);
        }
        memcpy(&cpss_db[devNum].epcl_cfg_tbl[ix], lookupCfgPtr, sizeof(*lookupCfgPtr));
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: port, VLAN, or index for getting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*
* OUTPUTS:
*       lookupCfgPtr      - (pointer to) lookup configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    GT_U32                          ix;

    if (CPSS_MAX_NUM_DEV < devNum)
    {
        return GT_FAIL;
    }

    switch (interfaceInfoPtr->type)
    {
    case CPSS_INTERFACE_PORT_E:
        ix = CPSS_MAX_NUM_VLAN + interfaceInfoPtr->devPort.portNum;
        break;

    case CPSS_INTERFACE_TRUNK_E:
        ix = CPSS_MAX_NUM_VLAN + interfaceInfoPtr->trunkId;
        break;

    default:
        return GT_FAIL;
    }

    if (CPSS_PCL_DIRECTION_EGRESS_E == direction &&
        CPSS_PCL_LOOKUP_0_E != lookupNum)
    {
        return GT_FAIL;
    }

    if (CPSS_PCL_DIRECTION_INGRESS_E == direction)
    {
        memcpy(lookupCfgPtr, &cpss_db[devNum].ipcl_cfg_tbl[lookupNum][ix], sizeof(*lookupCfgPtr));
    }
    else
    {
        if (_countof(cpss_db[devNum].epcl_cfg_tbl) <= ix)
        {
            assert(0);
        }
        memcpy(lookupCfgPtr, &cpss_db[devNum].epcl_cfg_tbl[ix], sizeof(*lookupCfgPtr));
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortLookupCfgTabAccessModeSet
*
* DESCRIPTION:
*    Configures VLAN/PORT access mode to Ingress or Egress PCL
*    configuration table perlookup.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*    direction       - Policy direction:
*                      Ingress or Egress
*    lookupNum       - Lookup number:
*                      lookup0 or lookup1
*    subLookupNum    - sub lookup number for lookup.
*                      Supported only for  DxChXCat and above devices.
*                      Ignored for other devices.
*                      DxChXCat and above devices supports two sub lookups only for
*                      ingress lookup 0, acceptable range 0..1.
*                      All other lookups have not sub lookups,
*                      acceptable value is 0.
*    mode            - PCL Configuration Table access mode
*                      Port or VLAN ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - for DxCh2 and above - full support,
*   - for DxCh1 supports Ingress direction only and set same access type
*     for both lookups regardless the <lookupNum> parameter value
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortLookupCfgTabAccessModeSet
(
    IN GT_U8                                          devNum,
    IN GT_U8                                          port,
    IN CPSS_PCL_DIRECTION_ENT                         direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT                     lookupNum,
    IN GT_U32                                         subLookupNum,
    IN CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT mode
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortIngressPolicyEnable
*
* DESCRIPTION:
*    Enables/disables ingress policy per port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*    enable          - GT_TRUE - Ingress Policy enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressPolicyEnable
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclIngressPolicyEnable
*
* DESCRIPTION:
*    Enables Ingress Policy.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*    devNum           - device number
*    enable           - enable ingress policy
*                      GT_TRUE  - enable,
*                      GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIngressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    cpss_db[devNum].cpss_ingress_policy_status = enable;
    return GT_OK;
}

/*******************************************************************************
* cpssDxCh2PclEgressPolicyEnable
*
* DESCRIPTION:
*    Enables Egress Policy.
*
* APPLICABLE DEVICES: DxCh2 and above Devices.
*
* INPUTS:
*    devNum           - device number
*    enable           - enable Egress Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh2PclEgressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    cpss_db[devNum].cpss_engress_policy_status = enable;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclInit
*
* DESCRIPTION:
*   The function initializes the device for following configuration
*   and using Policy engine
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum             - device number
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclInit
(
    IN GT_U8                           devNum
)
{
    return GT_OK;
}


/*******************************************************************************
* cpssDxChPclRuleActionGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN CPSS_PCL_DIRECTION_ENT              direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    CPSS_PCL_RULE_ENTRY *rule;
    GT_U32 tcamIndex = _cpssRuleSizeIndexToTcamIndex(ruleSize, ruleIndex);

    if (CPSS_MAX_NUM_RULE <= tcamIndex)
    {
        return GT_BAD_PARAM;
    }

    rule = &cpss_db[devNum].rule_tbl[tcamIndex];

    if (rule->valid == GT_FALSE)
    {
        return GT_BAD_PARAM;
    }

    memcpy(actionPtr, &rule->action, sizeof(rule->action));
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    CPSS_PCL_RULE_ENTRY *rule;
    GT_U32 tcamIndex = _cpssRuleSizeIndexToTcamIndex(ruleSize, ruleIndex);

    if (CPSS_MAX_NUM_RULE <= tcamIndex)
    {
        return GT_BAD_PARAM;
    }

    rule = &cpss_db[devNum].rule_tbl[tcamIndex];

    if (rule->valid == GT_FALSE)
    {
        return GT_BAD_PARAM;
    }

    memcpy(&rule->action, actionPtr, sizeof(rule->action));
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclCfgTblAccessModeSet
*
* DESCRIPTION:
*    Configures global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*    devNum          - device number
*    accModePtr      - global configuration of access mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*   - for DxCh2 and above - full support,
*   - for DxCh1 supports Ingress direction only with 32 ports per device and the
*     non-local-device-entries-segment-base == 0
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblAccessModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
)
{
    return GT_OK;
}

/*******************************************************************************
 * cpssDxCh3PolicerMeteringEntrySet
 *
 * DESCRIPTION:
 *      Sets Metering Policer Entry.
 *
 * APPLICABLE DEVICES: DxCh3 and above.
 *
 * INPUTS:
 *       devNum          - device number.
 *       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
 *                         Stage type is significant for DxChXcat and above devices
 *                         and ignored by DxCh3.
 *       entryIndex      - index of Policer Metering Entry.
 *                         Range: see datasheet for specific device.
 *       entryPtr        - pointer to the metering policer entry going to be set.
 *
 * OUTPUTS:
 *       tbParamsPtr     - pointer to actual policer token bucket parameters.
 *                         The token bucket parameters are returned as output
 *                         values. This is due to the hardware rate resolution,
 *                         the exact rate or burst size requested may not be
 *                         honored. The returned value gives the user the
 *                         actual parameters configured in the hardware.
 *
 * RETURNS:
 *       GT_OK                       - on success.
 *       GT_BAD_PTR                  - on NULL pointer.
 *       GT_HW_ERROR                 - on hardware error.
 *       GT_TIMEOUT                  - on time out of Policer Tables
 *                                     indirect access.
 *       GT_BAD_PARAM                - wrong devNum, stage or entryIndex or
 *                                     entry parameters.
 *       GT_OUT_OF_RANGE             - on Billing Entry Index out of range.
 *       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
 *
 * COMMENTS:
 *       None.
 *******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringEntrySet
(
    IN  GT_U8                                   devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    IN  CPSS_DXCH3_POLICER_METERING_ENTRY_STC   *entryPtr,
    OUT CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT  *tbParamsPtr
)
{
    cpss_db[devNum].metering_tbl[stage][entryIndex].valid = GT_TRUE;
    memcpy(&cpss_db[devNum].metering_tbl[stage][entryIndex].entry, entryPtr, sizeof(*entryPtr));
    return GT_OK;
}

/*******************************************************************************
 * cpssDxCh3PolicerMeteringEntryGet
 *
 * DESCRIPTION:
 *      Gets Metering Policer Entry configuration.
 *
 * APPLICABLE DEVICES: DxCh3 and above.
 *
 * INPUTS:
 *       devNum          - device number.
 *       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
 *                         Stage type is significant for DxChXcat and above devices
 *                         and ignored by DxCh3.
 *       entryIndex      - index of Policer Metering Entry.
 *                         Range: see datasheet for specific device.
 *
 * OUTPUTS:
 *       entryPtr        - pointer to the requested metering policer entry.
 *
 * RETURNS:
 *       GT_OK                       - on success.
 *       GT_BAD_PTR                  - on NULL pointer.
 *       GT_HW_ERROR                 - on hardware error.
 *       GT_BAD_PARAM                - wrong devNum or stage or entryIndex.
 *       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
 *       GT_BAD_STATE                - on bad value in a entry.
 *
 * COMMENTS:
 *       None.
 *******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringEntryGet
(
 IN  GT_U8                                   devNum,
 IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
 IN  GT_U32                                  entryIndex,
 OUT CPSS_DXCH3_POLICER_METERING_ENTRY_STC   *entryPtr
 )
{
    CPSS_POLICER_METERING_ENTRY *meter = &cpss_db[devNum].metering_tbl[stage][entryIndex];

    if (meter->valid == GT_FALSE)
    {
        return GT_BAD_PARAM;
    }

    memcpy(entryPtr, &meter->entry, sizeof(*entryPtr));
    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMruSet
*
* DESCRIPTION:
*       Sets the Policer Maximum Receive Unit size.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above devices
*                         and ignored by DxCh3.
*       mruSize         - Policer MRU value in bytes, range of values [0..64K].
*                         This value is used in the metering algorithm.
*                         When the number of bytes in the bucket is lower than
*                         this value a packet is marked as non conforming.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_OUT_OF_RANGE             - on mruSize out of range [0..65535 Bytes].
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMruSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U32                               mruSize
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeterResolutionSet
*
* DESCRIPTION:
*       Sets metering algorithm resolution: packets per
*       second or bytes per second.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above devices
*                         and ignored by DxCh3.
*       resolution      - packet/Byte based Meter resolution.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or resolution.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeterResolutionSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT  resolution
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerDropTypeSet
*
* DESCRIPTION:
*       Sets the Policer out-of-profile drop command type.
*       This setting controls if non-conforming dropped packets
*       (Red or Yellow) will be SOFT DROP or HARD DROP.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*                         Stage type is significant for DxChXcat and above devices
*                         and ignored by DxCh3.
*       dropType        - Policer Drop Type: Soft or Hard.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, dropType or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerDropTypeSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN CPSS_DROP_MODE_TYPE_ENT              dropType
)
{
    return GT_OK;
}


/*******************************************************************************
* cpssDxCh3PolicerPacketSizeModeSet
*
* DESCRIPTION:
*       Sets metered Packet Size Mode that metering and billing is done
*       according to.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum             - physical device number.
*       stage              - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                            Stage type is significant for DxChXcat and above devices
*                            and ignored by DxCh3.
*       packetSize         - Type of packet size:
*                               - L3 datagram size only (this does not include
*                                 the L2 header size and packets CRC).
*                               - L2 packet length including.
*                               - L1 packet length including
*                                 (preamble + IFG + FCS).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or packetSize.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerPacketSizeModeSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN CPSS_POLICER_PACKET_SIZE_MODE_ENT    packetSize
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringEnableSet
*
* DESCRIPTION:
*       Enables or disables metering per device.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above devices
*                         and ignored by DxCh3.
*       enable          - Enable/disable metering:
*                         GT_TRUE  - metering is enabled on the device.
*                         GT_FALSE - metering is disabled on the device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       If metering is enabled, it can be triggered
*       either by Policy engine or per port.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringEnableSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_BOOL                              enable
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPolicerStageMeterModeSet
*
* DESCRIPTION:
*       Sets Policer Global stage mode.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode            - Policer meter mode: FLOW or PORT.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In FLOW mode both the Policy and the port-trigger commands are
*       considered and in case of contention, the Policy command takes
*       precedence.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerStageMeterModeSet
(
    IN  GT_U8                                       devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    IN  CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT      mode
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringEntryRefresh
*
* DESCRIPTION:
*       Refresh the Policer Metering Entry.
*       Meter Bucket's state parameters refreshing are needed in order to
*       prevent mis-behavior due to wrap around of timers.
*
*       The wrap around problem can occur when there are long periods of
*       'silence' on the flow and the bucket's state parameters do not get
*       refreshed (meter is in the idle state). This causes a problem once the
*       flow traffic is re-started since the timers might have already wrapped
*       around which can cause a situation in which the bucket's state is
*       mis-interpreted and the incoming packet is marked as non-conforming
*       even though the bucket was actually supposed to be full.
*       To prevent this from happening the CPU needs to trigger a meter
*       refresh transaction at least once every 10 minutes per meter.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above
*                         devices and ignored by DxCh3.
*       entryIndex      - index of Policer Metering Entry.
*                         Range: see datasheet for specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Once the CPU triggers Refresh transaction the policer accesses the
*       metering entry and performs metering refresh with the packet's Byte
*       Count set to zero.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringEntryRefresh
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN  GT_U32                              entryIndex
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerPortMeteringEnableSet
*
* DESCRIPTION:
*       Enables or disables a port metering trigger for packets
*       arriving on this port.
*       When feature is enabled the meter entry index is a port number.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above devices
*                         and ignored by DxCh3.
*       portNum         - port number (including the CPU port).
*       enable          - Enable/Disable per-port metering for packets arriving
*                         on this port:
*                         GT_TRUE  - Metering is triggered on specified port.
*                         GT_FALSE - Metering isn't triggered on specified port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or portNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerPortMeteringEnableSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U8                                portNum,
    IN GT_BOOL                              enable
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCscdPortTypeGet
*
* DESCRIPTION:
*            Retreive a PP port cascade port configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - port number or CPU port
*
* OUTPUTS:
*       portTypePtr - (pointer to) cascade type regular/extended DSA tag port
*                     or network port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortTypeGet
(
    IN GT_U8                        devNum,
    IN GT_U8                        portNum,
    OUT CPSS_CSCD_PORT_TYPE_ENT     *portTypePtr
)
{
    *portTypePtr = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCscdDevMapTableSet
*
* DESCRIPTION:
*            Set the cascade map table . the map table define the local port or
*            trunk that packets destined to a destination device (which is not
*            the local device)should be transmitted to.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - physical device number
*       targetDevNum - the device to be reached via cascade (0..31)
*       cascadeLinkPtr - (pointer to)A structure holding the cascade link type
*                      (port (0..63) or trunk(0..127)) and the link number
*                      leading to the target device.
*       srcPortTrunkHashEn - Relevant when (cascadeLinkPtr->linkType ==
*                       CPSS_CSCD_LINK_TYPE_TRUNK_E)
*                       Enabled to set the load balancing trunk hash for packets
*                       forwarded via an trunk uplink to be based on the
*                       packetﾒs source port, and not on the packets data.
*                       Indicates the type of uplink.
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
*                       Load balancing trunk hash is based on the ingress pipe
*                       hash mode as configured by function
*                       cpssDxChTrunkHashGeneralModeSet(...)
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load
*                       balancing trunk hash for this cascade trunk interface is
*                       based on the packetﾒs source port, regardless of the
*                       ingress pipe hash mode
*                       NOTE : this parameter is relevant only to DXCH2
*                       and above devices
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on wrong device or target device or bad trunk hash
*                          mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapTableSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        targetDevNum,

#ifdef MARVELL_CPSS_3_4
    IN GT_U8                        targetPortNum,
#endif

    IN CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    IN CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT srcPortTrunkHashEn
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeDesignatedDeviceTableSet
*
* DESCRIPTION:
*       Function to set the "designated device Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:  All DxCh devices.
*
* INPUTS:
*       devNum  - device number
*       index - index into the designated device table (range 1..7)
*       designatedDevNum - The designated device number (range 0..31)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index or bad
*                          designatedDevNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeDesignatedDeviceTableSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_U8        designatedDevNum
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeTableSet
*
* DESCRIPTION:
*       Function to set the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES:  All DxCh devices.
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*       entryInfoPtr - (pointer to) The entry information
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad cpuCode or bad one of
*                          bad one entryInfoPtr parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeTableSet
(
    IN GT_U8                    devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    IN CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *entryInfoPtr
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerQosRemarkingEntrySet
*
* DESCRIPTION:
*       Sets Policer Relative Qos Remarking Entry.
*       The Qos Remarking Entry contains two indexes [0..127] of Qos Profile
*       assigned to Out of profile packets with the Yellow and Red colors.
*       According to this indexes, new QoS parameters: UP,DP,TC and DSCP,
*       are extracted from the Qos Profile Table Entry.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - Policer Stage type: Ingress #0 or Ingress #1.
*                                     Stage type is significant for DxChXcat and above devices
*                                     and ignored by DxCh3.
*       qosProfileIndex             - index of Qos Remarking Entry will be set.
*       yellowQosTableRemarkIndex   - QoS profile (index in the Qos Table)
*                                     assigned to Out-Of-Profile packets with
*                                     the Yellow color. Index range is [0..127].
*       redQosTableRemarkIndex      - QoS profile (index in the Qos Table)
*                                     assigned to Out-Of-Profile packets with
*                                     the Red color. Index range is [0..127].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or qosProfileIndex.
*       GT_OUT_OF_RANGE             - on yellowQosTableRemarkIndex and
*                                     redQosTableRemarkIndex out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Supported only for Ingress Policer.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerQosRemarkingEntrySet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U32                               qosProfileIndex,
    IN GT_U32                               yellowQosTableRemarkIndex,
    IN GT_U32                               redQosTableRemarkIndex
)
{
    if (CPSS_DXCH_POLICER_STAGE_EGRESS_E == stage)
    {
        return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerCountingColorModeSet
*
* DESCRIPTION:
*       Sets the Policer color counting mode.
*       The Color Counting can be done according to the packet's
*       Drop Precedence or Conformance Level.
*       This affects both the Billing and Management counters.
*       If the packet was subject to remarking, the drop precedence used
*       here is AFTER remarking.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above devices
*                         and ignored by DxCh3.
*       mode            - Color counting mode: Drop Precedence or
*                         Conformance Level.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerCountingColorModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT  mode
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPolicerCountingModeSet
*
* DESCRIPTION:
*       Configures counting mode.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above
*                         devices and ignored by DxCh3.
*       mode           -  counting mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerCountingModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH_POLICER_COUNTING_MODE_ENT      mode
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingEntrySet
*
* DESCRIPTION:
*       Sets Policer Billing Counters.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above devices
*                         and ignored by DxCh3.
*       entryIndex      - index of Policer Billing Counters Entry.
*                         Range: see datasheet for specific device.
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out upon counter reset by
*                                     indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*       None-zeroed setting of Billing Counters while metring/counting is
*       enabled may cause data coherency problems.
*
*       In order to set Billing entry under traffic perform the following
*       algorithm:
*           - disconnect entryes from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entries back to traffic
*       This API address the same memory area (counters place) as can be set
*       by "cpssDxChIpfixEntrySet".
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerBillingEntrySet
(
    IN  GT_U8                                   devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    IN  CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    *billingCntrPtr
)
{
    GT_U32 stageIndex;
    CPSS_POLICER_COUNTER_ENTRY *cntr_entry;

    assert(NULL != billingCntrPtr);

    if (CPSS_MAX_NUM_DEV <= devNum)
    {
        return GT_BAD_PARAM;
    }

    switch (stage)
    {
        case CPSS_DXCH_POLICER_STAGE_INGRESS_0_E:
            stageIndex = 0;
            break;
        case CPSS_DXCH_POLICER_STAGE_INGRESS_1_E:
            stageIndex = 1;
            break;

        case CPSS_DXCH_POLICER_STAGE_EGRESS_E:
            stageIndex = 2;
            break;

        default:
            return GT_BAD_PARAM;
    }

    if (CPSS_MAX_NUM_INGRESS_COUNTER <= entryIndex)
    {
        return GT_BAD_PARAM;
    }

    cntr_entry = &cpss_db[devNum].counter_tbl[stageIndex][entryIndex];

    cntr_entry->valid = GT_TRUE;
    memcpy(&cntr_entry->entry, billingCntrPtr, sizeof(*billingCntrPtr));
    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingEntryGet
*
* DESCRIPTION:
*       Gets Policer Billing Counters.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above devices
*                         and ignored by DxCh3.
*       entryIndex      - index of Policer Billing Counters Entry.
*                         Range: see datasheet for specific device.
*       reset           - reset flag:
*                         GT_TRUE  - performing read and reset atomic operation.
*                         GT_FALSE - performing read counters operation only.
*
* OUTPUTS:
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_BAD_STATE                - on counter entry type mismatch.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerBillingEntryGet
(
    IN  GT_U8                                   devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    IN  GT_BOOL                                 reset,
    OUT CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    *billingCntrPtr
)
{
    GT_U32 stageIndex;
    CPSS_POLICER_COUNTER_ENTRY *cntr_entry;

    assert(NULL != billingCntrPtr);

    if (CPSS_MAX_NUM_DEV <= devNum)
    {
        return GT_BAD_PARAM;
    }

    switch (stage)
    {
        case CPSS_DXCH_POLICER_STAGE_INGRESS_0_E:
            stageIndex = 0;
            break;
        case CPSS_DXCH_POLICER_STAGE_INGRESS_1_E:
            stageIndex = 1;
            break;

        case CPSS_DXCH_POLICER_STAGE_EGRESS_E:
            stageIndex = 2;
            break;

        default:
            return GT_BAD_PARAM;
    }

    if (CPSS_MAX_NUM_INGRESS_COUNTER <= entryIndex)
    {
        return GT_BAD_PARAM;
    }

    cntr_entry = &cpss_db[devNum].counter_tbl[stageIndex][entryIndex];

    if (GT_TRUE != cntr_entry->valid)
    {
        return GT_BAD_PARAM;
    }

    memcpy(billingCntrPtr, &cntr_entry->entry, sizeof(*billingCntrPtr));

    if (GT_TRUE == reset)
    {
        memset(&cntr_entry->entry.greenCntr, 0, sizeof(cntr_entry->entry.greenCntr));
        memset(&cntr_entry->entry.yellowCntr, 0, sizeof(cntr_entry->entry.yellowCntr));
        memset(&cntr_entry->entry.redCntr, 0, sizeof(cntr_entry->entry.redCntr));
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCosProfileEntrySet
*
* DESCRIPTION:
*       Configures the Profile Table Entry and
*       Initial Drop Precedence (DP) for Policer.
*
* APPLICABLE DEVICES:  All DxCh Devices.
*
* INPUTS:
*       devNum        - device number.
*       profileIndex  - index of a profile in the profile table
*                       Ch values (0..71); Ch2 and above values (0..127).
*       cosPtr        - Pointer to new CoS values for packet
*                       (dp = CPSS_DP_YELLOW_E is not applicable).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or cos.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh3 devices Initial DP supports 3 levels (Yellow, Red, Green),
*       but QoS profile entry supports only 2 levels by the following way:
*        - The function set value 0 for Green DP
*        - The function set value 1 for both Yellow and Red DPs.
*
*******************************************************************************/
GT_STATUS cpssDxChCosProfileEntrySet
(
     IN  GT_U8                      devNum,
     IN  GT_U32                     profileIndex,
     IN  CPSS_DXCH_COS_PROFILE_STC  *cosPtr
)
{
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCosProfileEntryGet
*
* DESCRIPTION:
*       Get Profile Table Entry configuration and
*       initial Drop Precedence (DP) for Policer.
*
* APPLICABLE DEVICES:  All DxCh Devices.
*
* INPUTS:
*       devNum        - device number.
*       profileIndex  - index of a profile in the profile table
*                       Ch values (0..71); Ch2 and above values (0..127).
*
* OUTPUTS:
*       cosPtr        - Pointer to new CoS values for packet.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or cos.
*       GT_TIMEOUT      - after max number of retries checking if PP ready.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh3 devices Initial DP supports 3 levels (Yellow, Red, Green),
*       but QoS profile entry supports only 2 levels.
*       Therefore DP value is taken from the Initial DP table.
*
*******************************************************************************/
GT_STATUS cpssDxChCosProfileEntryGet
(
     IN  GT_U8                      devNum,
     IN  GT_U32                     profileIndex,
     OUT CPSS_DXCH_COS_PROFILE_STC  *cosPtr
)
{
    return GT_NOT_APPLICABLE_DEVICE;
}

/*******************************************************************************
* cpssDxCh2EgressPclPacketTypesSet
*
* DESCRIPTION:
*   Enables/disables Egress PCL (EPCL) for set of packet types on port
*
* APPLICABLE DEVICES: DxCh2 and above Devices
*
* INPUTS:
*   devNum        - device number
*   port          - port number
*   pktType       - packet type to enable/disable EPCL for it
*   enable        - enable EPCL for specific packet type
*                   GT_TRUE - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*   - after reset EPCL disabled for all packet types on all ports
*
*
*******************************************************************************/
GT_STATUS cpssDxCh2EgressPclPacketTypesSet
(
    IN GT_U8                             devNum,
    IN GT_U8                             port,
    IN CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT pktType,
    IN GT_BOOL                           enable
)
{
    return GT_OK;
}
