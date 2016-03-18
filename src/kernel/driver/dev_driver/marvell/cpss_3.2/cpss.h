#ifndef __CPSS_H__
#define __CPSS_H__

#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>

#define MARVELL
#define MARVELL_CPSS
#define MARVELL_CPSS_3_2
#define _FAKE_CPSS

void _cpssInit();

GT_STATUS _cpssDxChPclRuleGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT *ruleFormatPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

#endif  //__CPSS_H__
