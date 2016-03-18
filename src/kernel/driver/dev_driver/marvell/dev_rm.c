/* FUNCTION NAME: DEV_RM.H
* PURPOSE:
*         Only Suport BCM XGSIII 5630X/5650X/5660X chip
* NOTES:
*
* REASON:
*    DESCRIPTION:
*    CREATOR:
*    Date           04/19/2005
*
* Copyright(C)      Accton Corporation, 2005
*/


/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysfun.h>
#include "sys_cpnt.h"
#include "sys_hwcfg.h"
#include "sys_bld.h"
#include "l_stdlib.h"

#include "dev_rm.h"
#include "dev_rm_pmgr.h"
#include "rule_type.h"
#include "dev_swdrv.h"
#include "backdoor_mgr.h"

/* It's not be used by dev_rm.c, but it will cause the compiler error with dragonite source files.
 */
#if (SYS_CPNT_POE == TRUE)

#if (SYS_CPNT_POE_INTERFACE != SYS_CPNT_POE_INTERFACE_DRAGONITE)
#undef BIT_0
#undef BIT_1
#undef BIT_2
#undef BIT_3
#undef BIT_4
#undef BIT_5
#undef BIT_6
#undef BIT_7
#undef BIT_8
#undef BIT_9
#undef BIT_10
#undef BIT_11
#undef BIT_12
#undef BIT_13
#undef BIT_14
#undef BIT_15
#undef BIT_16
#undef BIT_17
#undef BIT_18
#undef BIT_19
#undef BIT_20
#undef BIT_21
#undef BIT_22
#undef BIT_23
#undef BIT_24
#undef BIT_25
#undef BIT_26
#undef BIT_27
#undef BIT_28
#undef BIT_29
#undef BIT_30
#undef BIT_31
#endif

#else
#undef BIT_0
#undef BIT_1
#undef BIT_2
#undef BIT_3
#undef BIT_4
#undef BIT_5
#undef BIT_6
#undef BIT_7
#undef BIT_8
#undef BIT_9
#undef BIT_10
#undef BIT_11
#undef BIT_12
#undef BIT_13
#undef BIT_14
#undef BIT_15
#undef BIT_16
#undef BIT_17
#undef BIT_18
#undef BIT_19
#undef BIT_20
#undef BIT_21
#undef BIT_22
#undef BIT_23
#undef BIT_24
#undef BIT_25
#undef BIT_26
#undef BIT_27
#undef BIT_28
#undef BIT_29
#undef BIT_30
#undef BIT_31
#endif

#include <cpss/generic/pcl/cpssPcl.h>
#include <cpss/generic/cscd/cpssGenCscd.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

/* dxch private */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* NAMING CONSTANT DECLARATIONS
 */

/* constants for "mirror to analyzer port" workaround
 */
#ifdef MARVELL_ALLEYCAT
/* the cpu code used for "mirror to analyzer port" workaround
 */
#define MIRROR_TO_ANALYZER_PORT_WAR_CPU_CODE                 CPSS_NET_LAST_USER_DEFINED_E

/* the desinated device number index used for "mirror to analyzer port" workaround
 */
#define MIRROR_TO_ANALYZER_PORT_WAR_DESIGNATED_DEV_NUM_INDEX 7

/* the device number used for "mirror to analyzer port" workaround
 */
#define MIRROR_TO_ANALYZER_PORT_WAR_DEV_NUM                  31
#endif

/* MACRO FUNCTION DECLARATIONS
 */
#ifndef _countof
#define _countof(ary)   (sizeof(ary)/sizeof(*ary))
#endif

#ifndef ASSERT
#define ASSERT(eq)
#endif

/* trunkid workaround for xCat chips
 * start trunk id from 66 and must be even as 66, 68, ...
 * A2C : Accton to CPSS, C2A : CPSS to Accton
 */
extern UI32_T DEV_SWDRV_WorkaroundForTrunkId(UI32_T device_id, UI32_T trunk_id, BOOL_T to_chip);

#define DEV_SWDRV_CONVER_TRUNKID_A2C( devNum, trunkId ) \
            DEV_SWDRV_WorkaroundForTrunkId(devNum, trunkId, TRUE)
#define DEV_SWDRV_CONVER_TRUNKID_C2A( devNum, trunkId ) \
            DEV_SWDRV_WorkaroundForTrunkId(devNum, trunkId, FALSE)


#define DEVRM_MAX_NBR_OF_TCAM_ENTRIES           (DEVRM_ADPT_MAX_NBR_OF_RULE_PER_UNIT)
#define DEVRM_MAX_NBR_OF_TCAM_ROWS              (DEVRM_MAX_NBR_OF_TCAM_ENTRIES/4)
#define DEVRM_MAX_NBR_OF_PCL                    (DEVRM_ADPT_MAX_NBR_OF_PCL_PER_UNIT)

#ifndef SYS_HWCFG_MAX_NBR_OF_EGRESS_METER_PER_CHIP
#define DEVRM_MAX_NBR_OF_EGRESS_METER_ENTRIES   0
#else
#define DEVRM_MAX_NBR_OF_EGRESS_METER_ENTRIES   (SYS_HWCFG_MAX_NBR_OF_EGRESS_METER_PER_CHIP)
#endif

#define DEVRM_MAX_NUM_OF_DEVICE()                       \
    (appDemoPpConfigDevAmount)

#define DEVRM_CHECK_DEVICE_ID(device_id)                \
    ASSERT(device_id < devrm_data.num_of_device)

#define DEVRM_DEVICE_PTR(device_id)                     \
    (devrm_data.device_table[device_id])


/* define hardware resource used by meter
 *
 * - stage 0 will be used for meter
 *   (managed by DEV_RM)
 * - stage 1 will be used for ingress rate limit
 *   if ingress policer supports two stages.
 *   (managed by DEV_SWDRVL4)
 * - if supports two stages, DEV_SWDRVL4 will init policer memory
 */
#define DEVRM_METER_POLICER_STAGE(device_id)    CPSS_DXCH_POLICER_STAGE_INGRESS_0_E

#define DEVRM_METER_MAX_NUM(device_id)  \
            (PRV_CPSS_DXCH_PP_MAC(device_id)->policer.memSize[DEVRM_METER_POLICER_STAGE(device_id)])

#define DEVRM_METER_MAX_NUM_BY_STAGE(device_id, policer_stage) \
            (PRV_CPSS_DXCH_PP_MAC(device_id)->policer.memSize[policer_stage])

/* 0-27 is used for port based ?? For flow baed we start from 28.
 * Need comment from Marvell to check is it a SDK problem ?
 */
#define DEVRM_METER_INDEX_BASE                  (28)

/* QoS Profile 88-128 is used for QoS remarking
 */
#define DEVRM_QOS_REMARKING_ENTRY_FIRST_IDX     (96)
#define DEVRM_QOS_REMARKING_ENTRY_LAST_IDX      (PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS - 1)
#define DEVRM_QOS_REMARKING_ENTRY_NUM           (DEVRM_QOS_REMARKING_ENTRY_LAST_IDX - DEVRM_QOS_REMARKING_ENTRY_FIRST_IDX + 1)


static void DEVRM_BackdoorMain();

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/* STATIC VARIABLE DECLARATIONS
 */
/* gordon_kao : use for cpss
 */
#if (1400 <= _MSC_VER /* VC2005 */)
#define DEVRM_LOG(fmt, ...)                         \
    {                                               \
        if (DEVRM_IS_DEBUG_ERROR_ON())              \
        {                                           \
            DEBUG_PRINT_STR(fmt, __VA_ARGS__);      \
        }                                           \
    }
#else
#define DEVRM_LOG(fmt, args...)                     \
    {                                               \
        if (DEVRM_IS_DEBUG_ERROR_ON())              \
        {                                           \
            DEBUG_PRINT_STR(fmt, ##args);           \
        }                                           \
    }
#endif /* _MSC_VER */

#define DEBUG_ERROR_MSG(x) printf("%s(%d):Fail! %d\r\n", __FUNCTION__, __LINE__, (x));fflush(stdout);

#if (1400 <= _MSC_VER /* VC2005 */)
#define DEBUG_PRINT_STR(fmt, ...)                   \
    {                                               \
        printf("%s(%d)\r\n  ",                      \
               __FUNCTION__, __LINE__);             \
        printf(fmt, __VA_ARGS__);                   \
        printf("\r\n");                             \
        fflush(stdout);                             \
    }
#else
#define DEBUG_PRINT_STR(fmt, args...)               \
    {                                               \
        printf("%s(%d)\r\n  ",                      \
               __FUNCTION__, __LINE__);             \
        printf(fmt, ##args);                        \
        printf("\r\n");                             \
        fflush(stdout);                             \
    }
#endif

#define DEVRM_IS_DEBUG_ERROR_ON()  (0 !=  devrm_debug_flag)

#define DEVRM_VALIDATE_RULE()                       \
    {                                               \
        if (devrm_validate_rule_func)               \
            devrm_validate_rule_func();             \
    }


#define DEVRM_POLICER_STAGE(pcl_direction)                                  \
    (                                                                       \
        ((pcl_direction) == CPSS_PCL_DIRECTION_INGRESS_E) ?                 \
            DEVRM_METER_POLICER_STAGE(0) :                                  \
            CPSS_DXCH_POLICER_STAGE_EGRESS_E                                \
    )

#define DEVRM_POLICER_STAGE_STR(stage)                                      \
    (                                                                       \
        (stage) == CPSS_DXCH_POLICER_STAGE_INGRESS_0_E ? "ingress-0" :      \
        (stage) == CPSS_DXCH_POLICER_STAGE_INGRESS_1_E ? "ingress-1" :      \
        (stage) == CPSS_DXCH_POLICER_STAGE_EGRESS_E    ? "egress" : "??"    \
    )


static const CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   accMode =
{
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E,   /*ipclAccMode*/
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E,        /*ipclMaxDevPorts*/
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E,          /*ipclDevPortBase*/
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E,       /*epclAccMode*/
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E,        /*epclMaxDevPorts*/
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E           /*epclDevPortBase*/
};


/** The Rule Priority of PCE
 *
 * 1.
 * pi and pj are two PCE.
 * if pi.hw_idx < pj.hw_idx
 *   pi have higher priority to match in TCAM than pj
 * and v.v.(vice versa)
 *
 * pi.hw_idx != pj.hw_idx
 *
 * 2.
 * pi and pj are two PCE. pi is in PCLi and pj is in PCLj.
 *
 * if i < j
 *   pi.hw_idx < pj.hw_idx
 * and v.v.
 *
 * 3.
 * pi and pj are two PCE. pi is in VGi(Virtual Group i) and pj is in VGj.
 * VGi and VGj are in the same PCL.
 *
 * if i < j
 *   pj.hw_idx < pi.hw_idx
 * and v.v.
 *
 * 4.
 * pi and pj are two PCE in the same VG.
 *
 * if pi.hw_idx < pj.hw_idx
 *   pj.priority <= pi.priority
 * and v.v.
 *
 */

typedef struct DEVRM_PCE_S
{
    BOOL_T  in_use;             /* indicate if this pce is in use */

    /** For Std. PCE: is_std_pce = true, hw_idx = extended_hw_idx;
     *
     * For Ext. PCE(need 2 PCEs): is_std_pce = false
     *   1. hw_idx = extended_hw_idx for the 1st PCE.
     *   2. hw_idx != extended_hw_idx for the 2nd PCE.
     *      (extended_hw_idx will equeal to the 1st PCE's hw_idx)
     */
    BOOL_T  is_std_pce;
    UI16_T  hw_idx;
    UI16_T  extended_hw_idx;

    UI16_T  pcl_id;
    UI16_T  vir_group_id;

    int     priority;

    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type;

    I16_T  policer_idx;         /* -1 means empty */
    I16_T  qos_remarking_idx;   /* -1 means empty */

    /** keep track of the upper layer index.
     */
    UI32_T  upper_rule_id;

    struct DEVRM_PCE_S *next;
    struct DEVRM_PCE_S *prev;
}DEVRM_PCE_T;

typedef struct
{
    DEVRM_PCE_T             *pce_list;
    DEVRM_PCE_T             *last_pce;
} DEVRM_RuleVector_T;

typedef struct
{
    int                     pri_lvl;
    UI16_T                  pcl_id;
    UI16_T                  vir_group_id;

    DEVRM_RuleVector_T      rv;

    BOOL_T                  valid;

} DEVRM_VirtualGroup_T;

#define DEVRM_MAX_NBR_OF_VIR_GROUP 128

typedef struct
{
    CPSS_PCL_DIRECTION_ENT      direction;

    DEVRM_PCL_LOOKUP_NUMBER_T   lookup_num;

    UI32_T pce_number;

    DEVRM_PCE_T *pce_list;
    DEVRM_PCE_T *last_pce;

    /** The current design in upper layer will use PCL to
     * group the same kind of PCEs. So we need to store the pcl_type
     */
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pcl_type;

    DEVRM_VirtualGroup_T    vg_tbl[DEVRM_MAX_NBR_OF_VIR_GROUP];

    BOOL_T is_used;

} DEVRM_PCL_T;


typedef void (*DEVRM_VALIDATE_RULE_FUNC_T)();

static BOOL_T
DEVRM_RuleToIngStdNotIp(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr
);

static BOOL_T
DEVRM_RuleToIngStdIpL2Qos(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr
);

static BOOL_T
DEVRM_RuleToIngStdIpv4L4(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr
);

static BOOL_T
DEVRM_RuleToIngStdpv6Dip(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr
);

static BOOL_T
DEVRM_RuleToIngExtNotIpv6(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr
);

static BOOL_T
DEVRM_RuleToIngExtIpv6L2(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr
);

static BOOL_T
DEVRM_RuleToIngExtIpv6L4(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr
);

static BOOL_T
DEVRM_RuleToEgrExtNotIpv6(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr
);

static BOOL_T
DEVRM_RuleToEgrExtIpv6L2(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_UndoRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Release resource which be allocated by DEVRM_SetRule
 * INPUT:    pce    - PCE entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_UndoRule(
    UI32_T device_id,
    DEVRM_PCE_T *pce
);

static CPSS_PCL_RULE_SIZE_ENT
DEVRM_GetRuleSize(
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type
);

static UI32_T
DEVRM_GetNumberOfChips();

static BOOL_T
DEVRM_GetDeviceInfo(
    DEVRM_DevInfo_T *dev_info_p
);

static BOOL_T
DEVRM_SetupKeyTypes(
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pcl_type,
    CPSS_DXCH_PCL_LOOKUP_CFG_STC *lookupCfg
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME : DEVRM_Translate_PCEIDwithType
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Translate our hw_idx to map CPSS hw_index
 * INPUT:
 *    type  --  pce type
 *    hw_id  --  our index, using a standard rule is an index
 *
 * OUTPUT:
 *    None
 *
 * RETURN:
 *    CPSS index
 *
 * NOTES:
 *    Because CPSS use 2 rules in an index with extended mode, so we need to
 *    mapping our index to CPSS's index. our index 2 means CPSS index 1 for
 *    extended mode.
 *------------------------------------------------------------------------------
 */
static UI32_T
DEVRM_Translate_PCEIDwithType(
    UI32_T type,
    UI32_T hw_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetPce
 *------------------------------------------------------------------------------
 * PURPOSE:  Get pce ptr with scan all the pcl
 * INPUT:    upper_rule_id
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetPce(
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_PCE_T **pce
);

static BOOL_T
DEVRM_DestroyAllAcl(
    UI32_T device_id
);

static BOOL_T
DEVRM_AllocateQosRemarking(
    UI32_T device_id,
    UI32_T *qos_remarking_idx_p
);

static void
DEVRM_FreeQosRemarking(
    UI32_T device_id,
    UI32_T qos_remarking_idx
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetPcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Get PCL
 * INPUT    : pcl_id    - PCL ID
 * OUTPUT   : NONE
 * RETURN   : Pointer of a PCL object
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_PCL_T *
DEVRM_GetPcl(
    UI32_T device_id,
    UI32_T pcl_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetVirtualGroupAtIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : Get virtual group by array index
 * INPUT    : pcl_id    - PCL ID
 *            idx       - array index
 * OUTPUT   : NONE
 * RETURN   : Pointer of a virtual group object
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_VirtualGroup_T *
DEVRM_GetVirtualGroupAtIndex(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T idx
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FindVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Find virtual group by ID
 * INPUT    : pcl_id        - PCL ID
 *            vir_group_id  - virtual group ID
 * OUTPUT   : NONE
 * RETURN   : Pointer of a virtual group object
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_VirtualGroup_T *
DEVRM_FindVirtualGroup(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_InsertVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Insert a virtual group to PCL
 * INPUT    : pcl_id        - PCL ID
 *            vir_group_id  - virtual group ID
 * OUTPUT   : NONE
 * RETURN   : TRUE/FALSE
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_VirtualGroup_T *
DEVRM_InsertVirtualGroup(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FindPreviousVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Find previous of virtual group
 * INPUT    : pcl_id        - PCL ID
 *            vir_group_id  - virtual group ID
 * OUTPUT   : NONE
 * RETURN   : pointer of virtual group
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_VirtualGroup_T *
DEVRM_FindPreviousVirtualGroup(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocatePceWithPriority
 *------------------------------------------------------------------------------
 * PURPOSE:  Allocate a PCE
 * INPUT:    pcl_id         - PCL ID
 *           vir_group_id   - Virtual group ID
 *           priority       - Rule priority
 *           pce_typ        - Rule type
 * OUTPUT:   pce            - PCE
 * RETURN:   TRUE/FALSE.
 * NOTE:     PCL must be valid.
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_AllocatePceWithPriority(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id,
    int priority,
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type,
    DEVRM_PCE_T **pce
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PLL_InsertPce
 *------------------------------------------------------------------------------
 * PURPOSE:  Insert a PCE into PCL Linked List
 * INPUT:    pcl_id         - PCL ID
 *           vir_group_id   - Virtual group ID
 *           pce            - PCE list
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_PLL_InsertPce(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id,
    DEVRM_PCE_T *pce
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PLL_RemovePce
 *------------------------------------------------------------------------------
 * PURPOSE:  Remove a PCE from PCL Linked List
 * INPUT:    pce            - PCE entry
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_PLL_RemovePce(
    UI32_T device_id,
    DEVRM_PCE_T *pce
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PLL_Update
 *------------------------------------------------------------------------------
 * PURPOSE:  Link all virtual group rule pointer together.
 * INPUT:    pcl_id         - PCL ID
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE.
 * NOTE:     PCL must be valid.
 *------------------------------------------------------------------------------
 */
static void
DEVRM_PLL_Update(
    UI32_T device_id,
    UI32_T pcl_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AssignPceAtHwIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Allocate a PCE from TCAM
 * INPUT:    hw_idx         - Hardware index
 *           is_std_pce     - The type of PCE
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_AssignPceAtHwIndex(
    UI32_T device_id,
    UI32_T hw_idx,
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SweepPceAtHwIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Sweep a PCE from TCAM
 * INPUT:    hw_idx         - Hardware index
 *           pce_type       - The type of PCE
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_SweepPceAtHwIndex(
    UI32_T device_id,
    UI32_T hw_idx,
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_MallocPce
 *------------------------------------------------------------------------------
 * PURPOSE:  Allocate memory for PCE
 * INPUT:    pcl_id         - PCL ID
 *           hw_idx         - Hardware index
 *           pce_type       - The type of PCE
 * OUTPUT:   None.
 * RETURN:   Pointer of PCE; NULL
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static DEVRM_PCE_T *
DEVRM_MallocPce(
    UI32_T pcl_id,
    UI32_T vir_group_id,
    UI32_T hw_idx,
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type,
    int priority
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_MfreePce
 *------------------------------------------------------------------------------
 * PURPOSE:  Free memory for PCE
 * INPUT:    pce            - PCE obeect
 * OUTPUT:   None.
 * RETURN:   Pointer of PCE; NULL
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static void
DEVRM_MfreePce(
    DEVRM_PCE_T *pce
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetHwIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a hardware index for new rule
 * INPUT:    pcl_id         - PCL ID
 *           vir_group_id   - Virtual group ID
 *           priority       - Priority of rule
 *           is_std_pce     - 0 for std, 1 for ext
 * OUTPUT:   hw_idx
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetHwIndex(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id,
    int priority,
    BOOL_T is_std_pce,
    UI16_T *hw_idx
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FindEmptyHwIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Find an empty hardware index for rule.
 * INPUT:    hw_idx         - Shift rule to right from this index
 *           is_std_pce     - Request rule size. TRUE means request 1 size.
 *                                               FALSE means request 2 size.
 * OUTPUT:   None
 * RETURN:   hardware index; 0xffff if failed
 * NOTE:
 *------------------------------------------------------------------------------
 */
static UI16_T
DEVRM_FindEmptyHwIndex(
    UI32_T device_id,
    UI16_T hw_idx,
    BOOL_T is_std_pce
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FindEmptyHwIndexReverse
 *------------------------------------------------------------------------------
 * PURPOSE:  Reversely find an empty hardware index for rule.
 * INPUT:    hw_idx         - Shift rule to right from this index
 *           is_std_pce     - Request rule size. TRUE means request 1 size.
 *                                               FALSE means request 2 size.
 * OUTPUT:   None
 * RETURN:   hardware index; 0xffff if failed
 * NOTE:
 *------------------------------------------------------------------------------
 */
static UI16_T
DEVRM_FindEmptyHwIndexReverse(
    UI32_T device_id,
    UI16_T hw_idx,
    BOOL_T is_std_pce
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ShiftPceToRight
 *------------------------------------------------------------------------------
 * PURPOSE:  Shift rule to right from hw_idx
 * INPUT:    hw_idx         - Shift rule to right from this index
 *           is_std_pce     - Request rule size. TRUE means request 1 size.
 *                                               FALSE means request 2 size.
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     The caller should make sure whether the position of hw_idx or
 *           hw_idx + 1 have been used or not before call this function. (X)
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_ShiftPceToRight(
    UI32_T device_id,
    UI16_T hw_idx,
    BOOL_T is_std_pce
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ShiftPceToLeft
 *------------------------------------------------------------------------------
 * PURPOSE:  Shift rule to left from hw_idx
 * INPUT:    hw_idx         - Shift rule to right from this index
 *           is_std_pce     - Shift size. TRUE means shift 1 size.
 *                                        FALSE means shift 2 size.
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     The caller should make sure whether the position of hw_idx or
 *           hw_idx + 1 have been used or not before call this function.
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_ShiftPceToLeft(
    UI32_T device_id,
    UI16_T hw_idx,
    BOOL_T is_std_pce
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetPceAtHwIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get previous PCE ptr
 * INPUT:    pce    - *pce = NULL to get a first
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetPceAtHwIndex(
    UI32_T device_id,
    UI16_T hw_idx,
    DEVRM_PCE_T **pce
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetNextPce
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next PCE ptr
 * INPUT:    pce    - *pce = NULL to get a first
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetNextPce(
    UI32_T device_id,
    DEVRM_PCE_T **pce
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetPreviousPce
 *------------------------------------------------------------------------------
 * PURPOSE:  Get previous PCE ptr
 * INPUT:    pce    - *pce = NULL to get a first
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetPreviousPce(
    UI32_T device_id,
    DEVRM_PCE_T **pce
);

typedef struct
{
    /* database for all PCLs
     */
    DEVRM_PCL_T pcls[DEVRM_MAX_NBR_OF_PCL];
    UI8_T pcls_use_status[(DEVRM_MAX_NBR_OF_PCL+7) / 8];

    /* database for all metering(policer) entries.
     * the value of each meter entry represent the refernece count.
     */
    UI16_T policer_use_status[SYS_HWCFG_MAX_METER_ENTRY_NUM];

#if (0 < DEVRM_MAX_NBR_OF_EGRESS_METER_ENTRIES)
    UI16_T egress_policer_use_status[SYS_HWCFG_MAX_NBR_OF_EGRESS_METER_PER_CHIP];
#endif

    /* database for all PCEs.
     * this array represent each PCE using status with one bit.
     * If a PCE is occupied, the bit is on.
     */
    UI8_T pces_use_status[ (DEVRM_MAX_NBR_OF_TCAM_ENTRIES +7) / 8];

    /* database for all qos remarking entires.
     */
    UI8_T qos_remarking_use_status[DEVRM_QOS_REMARKING_ENTRY_NUM / 8];

} DEVRM_DeviceData_T;

typedef struct
{
    UI8_T               num_of_device;  /* 0 also means un-init */
    DEVRM_DeviceData_T  **device_table;
} DEVRM_Data_T;

/* Mapping the Upper layer ACL type to PCL type:
 */

static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT DEVRM_PclTypeConverter[]=
{
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,     /* RULE_TYPE_MAC_ACL        */
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,    /* RULE_TYPE_IP_ACL         */
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,    /* RULE_TYPE_IP_STD_ACL     */
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,   /* RULE_TYPE_IP_EXT_ACL     */
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,   /* RULE_TYPE_MF             */
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,    /* RULE_TYPE_IPV6_ACL       */
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,    /* RULE_TYPE_IPV6_STD_ACL   */
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,    /* RULE_TYPE_IPV6_EXT_ACL   */
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,   /* RULE_TYPE_IP_SRC_GUARD   */
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,    /* RULE_TYPE_MVR_REC_VLAN   */
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E    /* RULE_TYPE_ARP_ACL        */
};

/* Rule Converter :
 *   Convert from Rule to chipset mask/pattern and action.
 */
static BOOL_T (*DEVRM_RuleConverter[CPSS_DXCH_PCL_RULE_FORMAT_LAST_E])
                                    (DEVRM_AceEntry_T *ace_entry,
                                     CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
                                     CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr) =
{
    DEVRM_RuleToIngStdNotIp,    /* CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E   */
    DEVRM_RuleToIngStdIpL2Qos,  /* CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E*/
    DEVRM_RuleToIngStdIpv4L4,   /* CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E  */
    DEVRM_RuleToIngStdpv6Dip,   /* CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E */
    DEVRM_RuleToIngExtNotIpv6,  /* CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E */
    DEVRM_RuleToIngExtIpv6L2,   /* CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E  */
    DEVRM_RuleToIngExtIpv6L4,   /* CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E  */
    NULL,                       /* CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E    */
    NULL,                       /* CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E */
    NULL,                       /* CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E   */
    DEVRM_RuleToEgrExtNotIpv6,  /* CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E  */
    DEVRM_RuleToEgrExtIpv6L2,   /* CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E   */
    NULL,                       /* CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E   */
    NULL,                       /* CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E      */
    NULL,                       /* CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E      */
};

static DEVRM_Data_T                 devrm_data;
static BOOL_T                       devrm_have_req_backdoor;
static UI32_T                       devrm_debug_flag;
static DEVRM_VALIDATE_RULE_FUNC_T   devrm_validate_rule_func;

//
// Move these functions to correct position
//

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_UpToQosProfieIdx
 *------------------------------------------------------------------------------
 * PURPOSE:  Convert up to qos profile idx
 * INPUT:    up
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T DEV_SWDRVL4_UpToQosProfieIdx(UI8_T up)
{
    static int UpToQosProfileIdxMap[] = {64, 65, 66, 67, 68, 69, 70, 71};
    return UpToQosProfileIdxMap[up];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PhbToQosProfieIdx
 *------------------------------------------------------------------------------
 * PURPOSE:  Convert phb to qos profile index
 * INPUT:    phb
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T DEV_SWDRVL4_PhbToQosProfieIdx(UI8_T phb)
{
    static int PhbToQosProfileIdxMap[] = {80, 81, 82, 83, 84, 85, 86, 87};
    return PhbToQosProfileIdxMap[phb];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_QueueToQosProfieIdx
 *------------------------------------------------------------------------------
 * PURPOSE:  Convert queue to qos profile index
 * INPUT:    queue
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T DEV_SWDRVL4_QueueToQosProfieIdx(UI8_T queue)
{
    static int QueueToQosProfileIdxMap[] = {88, 89, 90, 91, 92, 93, 94, 95};
    return QueueToQosProfileIdxMap[queue];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DscpToQosProfileIdx
 *------------------------------------------------------------------------------
 * PURPOSE:  init/clear chip database for PCLs//PCEs
 * INPUT:    device_id -- chip no
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
UI32_T DEV_SWDRVL4_DscpToQosProfileIdx(UI32_T dscp)
{
    return dscp;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_InitiateSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE:  init/clear chip database for PCLs//PCEs
 * INPUT:    device_id -- chip no
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_InitiateSystemResources()
{
    DEVRM_AllocateSystemResource();

    DEVRM_ResetSystemResource();

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocateSystemResource
 *------------------------------------------------------------------------------
 * PURPOSE  : Allocates resource when system was start-up
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
void
DEVRM_AllocateSystemResource()
{
    UI8_T device_id;

    if (0 == devrm_data.num_of_device)
    {
        ASSERT(NULL == devrm_data.device_table);

        devrm_data.num_of_device = DEVRM_MAX_NUM_OF_DEVICE();

        devrm_data.device_table = calloc(sizeof(DEVRM_DeviceData_T *), devrm_data.num_of_device);

        ASSERT(NULL != devrm_data.device_table);

        if (NULL == devrm_data.device_table)
        {
            devrm_data.num_of_device = 0;

            printf("\n*****\n");
            printf("\n*****\n");
            printf("DEVRM: Out of memory for device table");
            printf("\n*****\n");
            printf("\n*****\n");

            goto undo_init_fail;
        }

        for (device_id = 0; device_id < devrm_data.num_of_device; ++ device_id)
        {
            ASSERT(NULL == devrm_data.device_table[device_id]);

            devrm_data.device_table[device_id] = calloc(sizeof(DEVRM_DeviceData_T), 1);

            ASSERT(NULL != devrm_data.device_table[device_id]);

            if (NULL == devrm_data.device_table[device_id])
            {
                printf("\n*****\n");
                printf("DEVRM: Out of memory for device table [%u]", device_id);
                printf("\n*****\n");

                goto undo_init_fail;
            }
        }
    }

    for (device_id = 0; device_id < devrm_data.num_of_device; ++ device_id)
    {
        DEVRM_DestroyAllAcl(device_id);
    }

    return;

undo_init_fail:

    for (device_id = 0; device_id < devrm_data.num_of_device; ++ device_id)
    {
        if (NULL != devrm_data.device_table[device_id])
        {
            free(devrm_data.device_table[device_id]);
            devrm_data.device_table[device_id] = NULL;
        }
    }

    if (0 != devrm_data.num_of_device)
    {
        free(devrm_data.device_table);
        devrm_data.device_table = NULL;
        devrm_data.num_of_device = 0;
    }

    memset(&devrm_data, 0, sizeof(devrm_data));
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ResetSystemResource
 *------------------------------------------------------------------------------
 * PURPOSE  : Reset allocated resource to initialized value
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
void
DEVRM_ResetSystemResource()
{
    UI8_T device_id;

    ASSERT(0 != devrm_data.num_of_device);
    ASSERT(NULL != devrm_data.device_table);

    for (device_id = 0; device_id < devrm_data.num_of_device; ++ device_id)
    {
        ASSERT(NULL != devrm_data.device_table[device_id]);

        ASSERT(sizeof(DEVRM_DeviceData_T) == sizeof(*devrm_data.device_table[device_id]));

        memset(devrm_data.device_table[device_id], 0, sizeof(*devrm_data.device_table[device_id]));
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FreeSystemResource
 *------------------------------------------------------------------------------
 * PURPOSE  : Clean allocated resource
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
void
DEVRM_FreeSystemResource()
{
    UI8_T device_id;

    if (0 == devrm_data.num_of_device)
    {
        return;
    }

    ASSERT(NULL != devrm_data.device_table);

    for (device_id = 0; device_id < devrm_data.num_of_device; ++ device_id)
    {
        DEVRM_DestroyAllAcl(device_id);
    }

    for (device_id = 0; device_id < devrm_data.num_of_device; ++ device_id)
    {
        ASSERT(NULL != devrm_data.device_table[device_id]);

        free(devrm_data.device_table[device_id]);
        devrm_data.device_table[device_id] = NULL;
    }

    free(devrm_data.device_table);
    devrm_data.device_table = NULL;
    devrm_data.num_of_device = 0;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetNumberOfChips
 *------------------------------------------------------------------------------
 * PURPOSE  : Get soc_ndev
 * INPUT    : None
 * OUTPUT   : NONE.
 * RETURN   : number of chip.
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static UI32_T
DEVRM_GetNumberOfChips()
{
    return devrm_data.num_of_device;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetDeviceInfo
 *------------------------------------------------------------------------------
 * PURPOSE  : Get device information
 * INPUT    : None
 * OUTPUT   : num_of_chips
 *            port_bit_map.all_ports
 *            port_bit_map.cpu_port
 * RETURN   : number of chip.
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetDeviceInfo(
    DEVRM_DevInfo_T *dev_info_p)
{
    ASSERT(NULL != dev_info_p);

    memset(dev_info_p, 0, sizeof(DEVRM_DevInfo_T));

    dev_info_p->num_of_chips = DEVRM_GetNumberOfChips();

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PortEngineInitial
 *------------------------------------------------------------------------------
 * PURPOSE  : Init Port Policy engine
 * INPUT    : device_id, phy_port
 * OUTPUT   : None
 * RETURN   : TRE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_PortEngineInitial(
    UI32_T device_id,
    int phy_port)
{
    GT_STATUS rc = GT_OK;
    CPSS_CSCD_PORT_TYPE_ENT cscdPortType;
    GT_BOOL               enable;

    rc = cpssDxChCscdPortTypeGet(device_id, phy_port, &cscdPortType);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    /* policy engine disabled for cascading ports with */
    /*   DSA extended-tagged packets                   */
    /* it enabled on network ports and cascading ports */
    /* with DSA regular-tagged packets                 */
    enable =(cscdPortType == CPSS_CSCD_PORT_DSA_MODE_EXTEND_E) ? GT_FALSE : GT_TRUE;

    rc = cpssDxChPclPortIngressPolicyEnable(device_id, phy_port, enable);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    rc = cpssDxCh2EgressPclPacketTypesSet(device_id, phy_port, CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E, GT_TRUE);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    rc = cpssDxCh2EgressPclPacketTypesSet(device_id, phy_port, CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E, GT_TRUE);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    rc = cpssDxCh2EgressPclPacketTypesSet(device_id, phy_port, CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E, GT_TRUE);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    rc = cpssDxCh2EgressPclPacketTypesSet(device_id, phy_port, CPSS_DXCH_PCL_EGRESS_PKT_TS_E, GT_TRUE);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    rc = cpssDxCh2EgressPclPacketTypesSet(device_id, phy_port, CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E, GT_TRUE);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    /* Port lookup0 (for ACL) configuration.
     */
    rc = cpssDxChPclPortLookupCfgTabAccessModeSet(device_id,
                                                  phy_port,
                                                  CPSS_PCL_DIRECTION_INGRESS_E,
                                                  CPSS_PCL_LOOKUP_0_E,
                                                  0,
                                                  CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    /* lookup 0-1 (for policer QoS) configuration.
     */
    rc = cpssDxChPclPortLookupCfgTabAccessModeSet(device_id,
                                                  phy_port,
                                                  CPSS_PCL_DIRECTION_INGRESS_E,
                                                  CPSS_PCL_LOOKUP_0_E,
                                                  1,
                                                  CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    /* Port lookup1 (for L3 router) configuration.
     */
    rc = cpssDxChPclPortLookupCfgTabAccessModeSet(device_id,
                                                  phy_port,
                                                  CPSS_PCL_DIRECTION_INGRESS_E,
                                                  CPSS_PCL_LOOKUP_1_E,
                                                  0,
                                                  CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E);

    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    /* Engress Port lookup0 (for policer QoS) configuration.
     */
    rc = cpssDxChPclPortLookupCfgTabAccessModeSet(device_id,
                                                  phy_port,
                                                  CPSS_PCL_DIRECTION_EGRESS_E,
                                                  CPSS_PCL_LOOKUP_0_E,
                                                  0,
                                                  CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E);

    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_EngineInitial
 *------------------------------------------------------------------------------
 * PURPOSE  : Init Policy engine
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_EngineInitial(
    UI32_T device_id)
{
    int phy_port;
    GT_STATUS rc = GT_OK;

    /* Initialization policy engine
     */
    rc = cpssDxChPclInit(device_id);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    /* global enabled
     */
    rc = cpssDxChPclIngressPolicyEnable(device_id, GT_TRUE);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    rc = cpssDxCh2PclEgressPolicyEnable(device_id, GT_TRUE);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    rc = cpssDxChPclCfgTblAccessModeSet(device_id, (CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC *) &accMode);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    /* billing counter
     */
    rc = cpssDxChPolicerCountingModeSet(device_id, DEVRM_METER_POLICER_STAGE(device_id), CPSS_DXCH_POLICER_COUNTING_BILLING_IPFIX_E);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    rc = cpssDxCh3PolicerCountingColorModeSet(device_id, DEVRM_METER_POLICER_STAGE(device_id), CPSS_DXCH3_POLICER_COLOR_COUNT_DP_E);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

#if (0 < DEVRM_MAX_NBR_OF_EGRESS_METER_ENTRIES)
    rc = cpssDxChPolicerCountingModeSet(device_id, CPSS_DXCH_POLICER_STAGE_EGRESS_E, CPSS_DXCH_POLICER_COUNTING_BILLING_IPFIX_E);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
    }

    rc = cpssDxCh3PolicerCountingColorModeSet(device_id, CPSS_DXCH_POLICER_STAGE_EGRESS_E, CPSS_DXCH3_POLICER_COLOR_COUNT_DP_E);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
    }
#endif /* #if (0 < DEVRM_MAX_NBR_OF_EGRESS_METER_ENTRIES) */

    /* per-port enabled
     */
    for (phy_port = 0; phy_port < PRV_CPSS_PP_MAC(device_id)->numOfPorts; phy_port++)
    {
        if (!PRV_CPSS_PHY_PORT_IS_EXIST_MAC(device_id, phy_port))
        {
            continue;
        }

        if (!DEVRM_PortEngineInitial(device_id, phy_port))
        {
            continue;
        }
    }

    DEVRM_PortEngineInitial(device_id, CPSS_CPU_PORT_NUM_CNS);

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_MeterInitial
 *------------------------------------------------------------------------------
 * PURPOSE  : Init meter (policer)
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static void
DEVRM_MeterInitial(
    UI32_T device_id,
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT policer_stage)
{
    UI32_T policer_index;
    GT_STATUS rc;
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC policer_entry;
    CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT tbParams;

    /* Meter
     */
    /* Configure Metering mode as flow mode.
     */
    rc = cpssDxChPolicerStageMeterModeSet(device_id,
                                          policer_stage,
                                          CPSS_DXCH_POLICER_STAGE_METER_MODE_FLOW_E);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        SYSFUN_ASSERT(0);
    }

    /* We just enable the global metering mode for flow based policer control
     * It must not enable the port metering mode.
     */
    rc = cpssDxCh3PolicerMeteringEnableSet(device_id,
                                           policer_stage,
                                           GT_TRUE);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
    }

    rc = cpssDxCh3PolicerPacketSizeModeSet(device_id,
                                           policer_stage,
                                           CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
    }

    rc = cpssDxCh3PolicerMeterResolutionSet(device_id,
                                            policer_stage,
                                            CPSS_DXCH3_POLICER_METER_RESOLUTION_BYTES_E);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
    }

    if (CPSS_DXCH_POLICER_STAGE_INGRESS_0_E == policer_stage ||
        CPSS_DXCH_POLICER_STAGE_INGRESS_1_E == policer_stage)
    {
        rc = cpssDxCh3PolicerDropTypeSet(device_id,
                                         policer_stage,
                                         CPSS_DROP_MODE_HARD_E);
        if (rc != GT_OK)
        {
            DEBUG_ERROR_MSG(rc);
        }
    }

    rc = cpssDxCh3PolicerMruSet(device_id,
                                policer_stage,
                                1522);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
    }

    /* Reset meter table
     */
    memset(&policer_entry, 0, sizeof(CPSS_DXCH3_POLICER_METERING_ENTRY_STC));
    memset(&tbParams, 0, sizeof(CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT));

    if (CPSS_DXCH_POLICER_STAGE_EGRESS_E == policer_stage)
    {
        policer_entry.modifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        policer_entry.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        policer_entry.modifyDp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
    }

    for (policer_index = 0; policer_index < DEVRM_METER_MAX_NUM_BY_STAGE(device_id, policer_stage); policer_index++)
    {
        rc = cpssDxCh3PolicerMeteringEntrySet(device_id,
                                              policer_stage,
                                              policer_index,
                                              &policer_entry,
                                              &tbParams);
        if (rc != GT_OK)
        {
            printf("%s(%d):Fail ! rc=%d devNum=%lu, stage=%s, index=%lu\r\n",
                __FUNCTION__, __LINE__, rc, device_id, DEVRM_POLICER_STAGE_STR(policer_stage), policer_index);
        }
    }

}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PceHWTableInitial
 *------------------------------------------------------------------------------
 * PURPOSE:  init/clear Pce table
 * INPUT:    None
 * OUTPUT:   None
 * RETURN:   None
 * Note:
 * the TCAM table is a matrix of PRV_DXCH3_PCL_TCAM_RAW_NUMBER_CNS rows
 * with 4 standard rules or 2 extended rules or one ultra rule. (chip dependent)
 * Rule indexes are increasing first in row and then in column
 * (top to down):
 *    *************************
 *    *  0  *  1  *  2  *  3  *
 *    *************************
 *    *  4  *  5  *  6  *  7  *
 *    *************************
 *    *  .  *  .  *  .  *  .  *
 *    *************************
 *------------------------------------------------------------------------------
 */
static void
DEVRM_PceHWTableInitial(
    UI32_T device_id)
{
    int rows=0, colums=0, rule_idx=0;
    GT_STATUS rc = GT_OK;

    for (rows = 0; rows < DEVRM_MAX_NBR_OF_TCAM_ROWS; rows ++)
    {
        for (colums = 0; colums < 4; colums ++)
        {
            rule_idx = rows * 4 + colums;
            rc = cpssDxChPclRuleInvalidate(device_id, CPSS_PCL_RULE_SIZE_STD_E, rule_idx);
            if (rc != GT_OK)
                DEBUG_ERROR_MSG(rc);
        }
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  Allocate an H/W meter(policer) entry
 * INPUT:    acl_id, pce_type
 * OUTPUT:   meter_idx
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_AllocateMeter(
    UI32_T device_id,
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT policer_stage,
    I16_T *meter_idx)
{
    UI32_T  i = 0;
    UI16_T  *policer_tbl = NULL;
    UI32_T  policer_tbl_size = 0;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (policer_stage == DEVRM_METER_POLICER_STAGE(device_id))
    {
        policer_tbl = DEVRM_DEVICE_PTR(device_id)->policer_use_status;
        policer_tbl_size = _countof(DEVRM_DEVICE_PTR(device_id)->policer_use_status);
    }

#if (0 < DEVRM_MAX_NBR_OF_EGRESS_METER_ENTRIES)
    else
    {
        policer_tbl = DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status;
        policer_tbl_size = _countof(DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status);
    }
#endif

    /* find an unused meter(policer) entry to use
     */
    for (i = DEVRM_METER_INDEX_BASE; i < policer_tbl_size; ++i)
    {
        ASSERT(NULL != policer_tbl);

        if (0 == policer_tbl[i])
        {
            *meter_idx = (I16_T)i;
            policer_tbl[i]++;
            return TRUE;
        }
    }

    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FreeMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  Allocate an H/W meter(policer) entry
 * INPUT:    acl_id, pce_type
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     When allocate the pce, this function will find an available
 *           pce that have the bigger h/w index than the last pce in this pcl
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_FreeMeter(
    UI32_T device_id,
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT policer_stage,
    I16_T *meter_idx)
{
    UI32_T  i;
    UI16_T  *policer_tbl = NULL;
    UI32_T  policer_tbl_size = 0;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (policer_stage == DEVRM_METER_POLICER_STAGE(device_id))
    {
        policer_tbl = DEVRM_DEVICE_PTR(device_id)->policer_use_status;
        policer_tbl_size = _countof(DEVRM_DEVICE_PTR(device_id)->policer_use_status);
    }

#if (0 < DEVRM_MAX_NBR_OF_EGRESS_METER_ENTRIES)
    else
    {
        policer_tbl = DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status;
        policer_tbl_size = _countof(DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status);
    }
#endif

    if (NULL == policer_tbl ||
        0 == policer_tbl_size)
    {
        ASSERT(0);
        return FALSE;
    }

    if (-1 == *meter_idx ||
        *meter_idx < DEVRM_METER_INDEX_BASE ||
        policer_tbl_size <= (UI32_T)*meter_idx)
    {
        ASSERT(0);
        return FALSE;
    }

    i = *meter_idx;

    if (policer_tbl[i] == 0)
    {
        ASSERT(0);
        return FALSE;
    }

    policer_tbl[i]--;
    return TRUE;

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocatePce
 *------------------------------------------------------------------------------
 * PURPOSE:  Allocate an H/W pce entry
 * INPUT:    acl_id, pce_type
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     When allocate the pce, this function will find an available
 *           pce that have the bigger h/w index than the last pce in this pcl
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_AllocatePce(
    UI32_T device_id,
    UI32_T pcl_id,
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type,
    DEVRM_PCE_T **pce)
{
    const UI32_T dflt_vir_group_id = 100;
    const int    dflt_rule_priority = 100;

    return DEVRM_AllocatePceWithPriority(device_id, pcl_id, dflt_vir_group_id,
                                         dflt_rule_priority, pce_type, pce);
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FreePce
 *------------------------------------------------------------------------------
 * PURPOSE:  Allocate an H/W pce entry
 * INPUT:    acl_id, pce_type
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     When allocate the pce, this function will find an available
 *           pce that have the bigger h/w index than the last pce in this pcl
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_FreePce(
    UI32_T device_id,
    UI32_T pcl_id,
    BOOL_T is_std_pce,
    DEVRM_PCE_T *pce)
{
    DEVRM_CHECK_DEVICE_ID(device_id);

    ASSERT(NULL != pce);
    ASSERT(pcl_id == pce->pcl_id);
    ASSERT(is_std_pce == pce->is_std_pce);

    DEVRM_SweepPceAtHwIndex(device_id, pce->hw_idx, pce->pce_type);

    DEVRM_PLL_RemovePce(device_id, pce);
    DEVRM_MfreePce(pce);

    DEVRM_VALIDATE_RULE();

    return TRUE;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyAllAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy all ACLs.
 * INPUT:    None
 * OUTPUT:   NONE.
 * RETURN:   None
 * NOTE:     Use for reset
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_DestroyAllAcl(
    UI32_T device_id)
{
#define WIDTH_OF_PCL_TABLE  (sizeof(DEVRM_DEVICE_PTR(device_id)->pcls_use_status[0]) * 8)

    UI32_T i = 0, j = 0;
    UI32_T count = sizeof(DEVRM_DEVICE_PTR(device_id)->pcls_use_status) / (sizeof(*DEVRM_DEVICE_PTR(device_id)->pcls_use_status));

    DEVRM_CHECK_DEVICE_ID(device_id);

    ASSERT((DEVRM_ADPT_MAX_NBR_OF_PCL_PER_UNIT / 8) == count);

    for (i = 0; i < count; i++)
    {
        if (DEVRM_DEVICE_PTR(device_id)->pcls_use_status[i] != 0)
        {
            for (j = 0; j < WIDTH_OF_PCL_TABLE; j++)
            {
                if (!DEVRM_DEVICE_PTR(device_id)->pcls[i*WIDTH_OF_PCL_TABLE +j].is_used)
                {
                    continue;
                }

                DEVRM_DestroyAcl(device_id, i*WIDTH_OF_PCL_TABLE + j + 1);
            }
        }
    }

    return TRUE;

#undef WIDTH_OF_PCL_TABLE
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_Initial
 *------------------------------------------------------------------------------
 * PURPOSE:  init/clear chip setting
 * INPUT:    device_id -- chip no
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_Initial()
{
    UI32_T device_id;
    UI32_T num_of_device;

    if (FALSE == devrm_have_req_backdoor)
    {
        BACKDOOR_MGR_Register_SubsysBackdoorFunc_CallBack("dev_rm", SYS_BLD_DRIVER_GROUP_IPCMSGQ_KEY, DEVRM_BackdoorMain);
        devrm_have_req_backdoor = TRUE;
    }

    DEVRM_InitiateSystemResources();

    num_of_device = DEVRM_GetNumberOfChips();

    ASSERT(0 != num_of_device);

    for (device_id = 0; device_id < num_of_device; ++ device_id)
    {
        DEVRM_EngineInitial(device_id);
        DEVRM_PceHWTableInitial(device_id);
        DEVRM_MeterInitial(device_id, DEVRM_METER_POLICER_STAGE(device_id));

        if (0 < DEVRM_METER_MAX_NUM_BY_STAGE(device_id, CPSS_DXCH_POLICER_STAGE_EGRESS_E))
        {
            DEVRM_MeterInitial(device_id, CPSS_DXCH_POLICER_STAGE_EGRESS_E);
        }

        /* Self Test
         */
        {
            UI32_T real_tcam_entries =
                ((&PRV_CPSS_DXCH_PP_MAC(device_id)->fineTuning)->tableSize.policyTcamRaws) * 4;

            if (real_tcam_entries != SYS_HWCFG_MAX_NBR_OF_PCE_PER_CHIP)
            {
                printf("\n*****\n");
                printf("Number of TCAM entries = %lu, BUT now defined = %d\n",
                        real_tcam_entries, SYS_HWCFG_MAX_NBR_OF_PCE_PER_CHIP);
                printf("*****\n");
            }

        }

        {
            UI32_T real_ingress_meter_entries =
                    DEVRM_METER_MAX_NUM_BY_STAGE(device_id, DEVRM_METER_POLICER_STAGE(device_id) );

            if (real_ingress_meter_entries != SYS_HWCFG_MAX_METER_ENTRY_NUM)
            {
                printf("\n*****\n");
                printf("Number of ingress meter entries = %lu, BUT now defined = %d\n",
                       real_ingress_meter_entries, SYS_HWCFG_MAX_METER_ENTRY_NUM);
                printf("*****\n");
            }
        }

        if (0 < DEVRM_METER_MAX_NUM_BY_STAGE(device_id, CPSS_DXCH_POLICER_STAGE_EGRESS_E ))
        {
            UI32_T real_egress_meter_entries =
                    DEVRM_METER_MAX_NUM_BY_STAGE(device_id, CPSS_DXCH_POLICER_STAGE_EGRESS_E );

            if (real_egress_meter_entries != SYS_HWCFG_MAX_NBR_OF_EGRESS_METER_PER_CHIP)
            {
                printf("\n*****\n");
                printf("Number of egress meter entries = %lu, BUT now defined = %d\n",
                       real_egress_meter_entries, SYS_HWCFG_MAX_NBR_OF_EGRESS_METER_PER_CHIP);
                printf("*****\n");
            }
        }
    }

    ASSERT(0 != device_id);

    return TRUE;
}

/* rule */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocateRule
 *------------------------------------------------------------------------------
 * PURPOSE  : Allocate rule resource
 * INPUT    : device_id, selector_index
 *          : start_rule_id (per ACL),
 *            rule_qty (how many rules to be added)
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, call DEVRM_SetSelector() first
 *            2, MUST allocate rule resource before call DEVRM_SetRule()
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_AllocateRule(
    UI32_T  device_id,
    UI32_T  pcl_id,
    UI32_T  vir_group_id,
    int     rule_pri,
    UI32_T  dev_rule_id)
{
    DEVRM_PCL_T *pcl;
    DEVRM_PCE_T *pce;

    BOOL_T result;

    DEVRM_CHECK_DEVICE_ID(device_id);

    DEVRM_LOG("device_id=%lu, pcl_id=%lu, vg_id=%lu, rule_pri=%d, dev_rule_id=%lu",
              device_id, pcl_id, vir_group_id, rule_pri, dev_rule_id);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    ASSERT(NULL != pcl);

    if (NULL == pcl)
    {
        DEVRM_LOG("Invalid pclId=%lu", pcl_id);
        return FALSE;
    }

    result = DEVRM_GetPce(device_id, dev_rule_id, &pce);
    if (TRUE == result)
    {
        if (pce->pcl_id == pcl_id &&
            pce->vir_group_id == vir_group_id &&
            pce->priority == rule_pri)
        {
            return TRUE;
        }

        DEVRM_FreePce(device_id, pce->pcl_id, pce->is_std_pce, pce);
        pce = NULL;
    }

    result = DEVRM_AllocatePceWithPriority(device_id, pcl_id, vir_group_id, rule_pri,
                                           pcl->pcl_type, &pce);

    if (FALSE == result)
    {
        return FALSE;
    }

    pce->upper_rule_id = dev_rule_id;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetPceFromPcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Get pce ptr for the upper rule id in a pcl
 * INPUT:    pcl_id, upper_rule_id
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetPceFromPcl(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T upper_rule_id,
    DEVRM_PCE_T **pce)
{
    DEVRM_PCL_T *pcl;
    DEVRM_PCE_T *tmp;

    DEVRM_CHECK_DEVICE_ID(device_id);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    if (NULL == pcl)
    {
        return FALSE;
    }

    tmp = pcl->pce_list;
    while (tmp != NULL)
    {
        if (tmp->upper_rule_id == upper_rule_id)
        {
            *pce = tmp;
            return TRUE;
        }
        tmp = tmp->next;
    }

    *pce = NULL;
    return FALSE;

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetPce
 *------------------------------------------------------------------------------
 * PURPOSE:  Get pce ptr with scan all the pcl
 * INPUT:    upper_rule_id
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetPce(
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_PCE_T **pce)
{
#define WIDTH_OF_PCL_TABLE  (sizeof(DEVRM_DEVICE_PTR(device_id)->pcls_use_status[0]) * 8)

    UI32_T i=0, j = 0;
    UI32_T count = sizeof(DEVRM_DEVICE_PTR(device_id)->pcls_use_status) / (sizeof(*DEVRM_DEVICE_PTR(device_id)->pcls_use_status));
    DEVRM_PCE_T *tmp;

    DEVRM_CHECK_DEVICE_ID(device_id);

    for (i = 0; i < count; i++)
    {
        if (DEVRM_DEVICE_PTR(device_id)->pcls_use_status[i] != 0)
        {
            for (j = 0; j < WIDTH_OF_PCL_TABLE; j++)
            {
                if (!DEVRM_DEVICE_PTR(device_id)->pcls[i*WIDTH_OF_PCL_TABLE +j].is_used)
                {
                    continue;
                }
                tmp = DEVRM_DEVICE_PTR(device_id)->pcls[i*WIDTH_OF_PCL_TABLE +j].pce_list;
                while (tmp != NULL)
                {
                    if (tmp->upper_rule_id == dev_rule_id)
                    {
                        *pce = tmp;
                        return TRUE;
                    }
                    tmp = tmp->next;
                }
            }
        }
    }

    *pce = NULL;
    return FALSE;

#undef WIDTH_OF_PCL_TABLE
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetPceAtHwIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get previous PCE ptr
 * INPUT:    pce    - *pce = NULL to get a first
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetPceAtHwIndex(
    UI32_T device_id,
    UI16_T hw_idx,
    DEVRM_PCE_T **pce)
{
    DEVRM_PCL_T *pcl;
    DEVRM_PCE_T *pp;

    UI32_T pcl_id;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (DEVRM_MAX_NBR_OF_TCAM_ENTRIES <= hw_idx)
    {
        return FALSE;
    }

    if (0 != (DEVRM_DEVICE_PTR(device_id)->pces_use_status[ hw_idx/8 ] & ( 0x80 >> (hw_idx%8) )))
    {

        pp = NULL;

        for (pcl_id=1; pcl_id <= DEVRM_MAX_NBR_OF_PCL; ++pcl_id)
        {
            pcl = DEVRM_GetPcl(device_id, pcl_id);

            if (pcl && pcl->pce_list && pcl->last_pce)
            {
                if (pcl->pce_list->hw_idx <= hw_idx &&
                    hw_idx <= pcl->last_pce->hw_idx)
                {
                    pp = pcl->pce_list;
                    break;
                }
            }
        }

        while (pp)
        {
            if (pp->hw_idx == hw_idx ||
                (pp->is_std_pce == FALSE && pp->next->hw_idx == hw_idx))
            {
                *pce = pp;
                return TRUE;
            }

            DEVRM_GetNextPce(device_id, &pp);
        }

        /*
         * SHOULD NOT GO HERE !!
         * The linked list of PCL/Virtual Group was wrong !!
         */
        ASSERT(0);
    }

    *pce = NULL;
    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetNextPce
 *------------------------------------------------------------------------------
 * PURPOSE:  Get next PCE ptr
 * INPUT:    pce    - *pce = NULL to get a first
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetNextPce(
    UI32_T device_id,
    DEVRM_PCE_T **pce)
{
    DEVRM_PCL_T *pcl;
    DEVRM_PCE_T *next_pce;
    UI32_T pcl_id;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (NULL == *pce)
    {
        pcl_id = 1;
        next_pce = NULL;
    }
    else
    {
        pcl_id = ((DEVRM_PCE_T *) *pce)->pcl_id + 1;

        next_pce = (((DEVRM_PCE_T *) *pce)->is_std_pce) ?
                                            ((DEVRM_PCE_T *) *pce)->next :
                                            ((DEVRM_PCE_T *) *pce)->next->next;

    }

    if (NULL == next_pce)
    {
        for (; pcl_id <= DEVRM_MAX_NBR_OF_PCL;  ++pcl_id)
        {
            pcl = &DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id-1];

            if (pcl->is_used == TRUE &&
                pcl->pce_list)
            {
                *pce = pcl->pce_list;
                return TRUE;
            }
        }

        *pce = NULL;
        return FALSE;
    }

    /* debug check
     */
    if (next_pce && next_pce->is_std_pce == FALSE)
    {
        ASSERT(next_pce->next != NULL &&
               next_pce->extended_hw_idx == next_pce->next->extended_hw_idx);
    }

    *pce = next_pce;
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetPreviousPce
 *------------------------------------------------------------------------------
 * PURPOSE:  Get previous PCE ptr
 * INPUT:    pce    - *pce = NULL to get a first
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetPreviousPce(
    UI32_T device_id,
    DEVRM_PCE_T **pce)
{
    DEVRM_PCL_T *pcl;
    DEVRM_PCE_T *prev_pce;
    UI32_T pcl_id;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (NULL == *pce)
    {
        pcl_id = DEVRM_MAX_NBR_OF_PCL;
        prev_pce = NULL;
    }
    else
    {
        pcl_id = ((DEVRM_PCE_T *) *pce)->pcl_id - 1;

        prev_pce = ((DEVRM_PCE_T *) *pce)->prev;
        if (prev_pce && ((DEVRM_PCE_T *) *pce)->is_std_pce == FALSE)
        {
            prev_pce = prev_pce->prev;
        }
    }

    if (NULL == prev_pce)
    {
        for (; 1 <= pcl_id; --pcl_id)
        {
            pcl = &DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id-1];

            if (pcl->is_used == TRUE &&
                pcl->last_pce)
            {
                prev_pce = pcl->last_pce;

                if (prev_pce->is_std_pce == FALSE)
                {
                    prev_pce = prev_pce->prev;
                }

                *pce = prev_pce;
                return TRUE;
            }
        }

        *pce = NULL;
        return FALSE;
    }

    *pce = prev_pce;
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetPolicerStageFromPce
 *------------------------------------------------------------------------------
 * PURPOSE:  Get pce ptr for the upper rule id in a pcl
 * INPUT:    pcl_id, upper_rule_id
 * OUTPUT:   pce
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetPolicerStageFromPce(
    UI32_T device_id,
    DEVRM_PCE_T *pce,
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT *policer_stage)
{
    DEVRM_PCL_T *pcl;

    DEVRM_CHECK_DEVICE_ID(device_id);

    pcl = DEVRM_GetPcl(device_id, pce->pcl_id);
    ASSERT(NULL != pcl);

    if (NULL == pcl)
    {
        return FALSE;
    }

    *policer_stage = DEVRM_POLICER_STAGE(pcl->direction);

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ConvertRuleToPce
 *------------------------------------------------------------------------------
 * PURPOSE:  Convert Upper layer rule to the pce
 * INPUT:    ace_entry, action_entry, maskPtr, patternPtr, action_ptr
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     DEVRM_Rule = class_element + meter + action
 *           pass meter_entry with NULL if this rule does not have meter
 *           pass action_entry with NULL if this rule does not have action
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_ConvertRuleToPce(
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type,
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr)
{
    if (pce_type >= CPSS_DXCH_PCL_RULE_FORMAT_LAST_E)
    {
        ASSERT(0);
        return FALSE;
    }

    if (NULL == DEVRM_RuleConverter[pce_type])
    {
        ASSERT(0);
        return FALSE;
    }

    if (DEVRM_RuleConverter[pce_type](ace_entry, maskPtr, patternPtr) == FALSE)
    {
        ASSERT(0);
        return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ConvertActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Convert Upper layer action to the chipset action
 * INPUT:    action_entry
 * OUTPUT:   action
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_SetupAction(
    const DEVRM_ACTION_T *dev_action_p,
    CPSS_DXCH_PCL_ACTION_STC *action)
{
    ASSERT(NULL != dev_action_p);

    /** Spell error on SDK 3.2
     */
#ifdef MARVELL_CPSS_3_2
#define bypassBridge bypassBrigge
#endif

    switch (dev_action_p->pkt_cmd)
    {
        case DEVRM_PKT_CMD_FORWARD:

            action->pktCmd = CPSS_PACKET_CMD_FORWARD_E;

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_queue)
            {
                action->qos.qos.ingress.profileIndex = DEV_SWDRVL4_QueueToQosProfieIdx(dev_action_p->qos.queue);
                action->qos.qos.ingress.profileAssignIndex = GT_TRUE;
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_inner_priority)
            {
                action->qos.qos.ingress.profileIndex = DEV_SWDRVL4_PhbToQosProfieIdx(dev_action_p->qos.inner_priority);
                action->qos.qos.ingress.profileAssignIndex = GT_TRUE;
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_priority)
            {
                action->qos.modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                action->qos.qos.ingress.profileIndex = DEV_SWDRVL4_UpToQosProfieIdx(dev_action_p->qos.priority);
                action->qos.qos.ingress.profileAssignIndex = GT_TRUE;
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_dscp)
            {
                action->qos.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                action->qos.qos.ingress.profileIndex = DEV_SWDRVL4_DscpToQosProfileIdx(dev_action_p->qos.dscp);
                action->qos.qos.ingress.profileAssignIndex = GT_TRUE;
            }

            if (DEVRM_MODIFY_VLAN_CMD_DISABLE != dev_action_p->vlan.modify_vlan)
            {
                action->vlan.modifyVlan = (DEVRM_MODIFY_VLAN_CMD_FOR_UNTAGGED_ONLY == dev_action_p->vlan.modify_vlan) ?
                                            CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E :
                                          (DEVRM_MODIFY_VLAN_CMD_FOR_TAGGED_ONLY == dev_action_p->vlan.modify_vlan) ?
                                            CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E :
                                            CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
                action->vlan.vlanId = dev_action_p->vlan.vid;
                action->vlan.nestedVlan = (dev_action_p->vlan.add_outer_tag) ? GT_TRUE : GT_FALSE;
            }

            if (DEVRM_REDIRECT_MODE_ROUTE == dev_action_p->redirect.mode)
            {
                action->redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E;
                action->redirect.data.routerLttIndex = dev_action_p->redirect.ltt_index.row << 2 & dev_action_p->redirect.ltt_index.column;
            }
            break;

        case DEVRM_PKT_CMD_COPY_TO_CPU:
            action->pktCmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            action->mirror.cpuCode = (DEVRM_MODIFY_CMD_ENABLE  == dev_action_p->cpu.modify_cpu_code) ?
                dev_action_p->cpu.cpu_code :
                CPSS_NET_FIRST_USER_DEFINED_E;
            break;

        case DEVRM_PKT_CMD_REDIRECT_TO_CPU:
            action->pktCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            action->mirror.cpuCode = (DEVRM_MODIFY_CMD_ENABLE  == dev_action_p->cpu.modify_cpu_code) ?
                dev_action_p->cpu.cpu_code :
                CPSS_NET_FIRST_USER_DEFINED_E;
            action->actionStop = GT_FALSE;
            break;

        case DEVRM_PKT_CMD_MIRROR_TO_ANALYZER_PORT:
#if (!defined(MARVELL_ALLEYCAT))
            action->mirror.mirrorToRxAnalyzerPort = GT_TRUE;
            action->mirror.cpuCode = CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E;
#else           /* Do workaround for action "Mirror To Analyzer Port" not work properly */

            /* Marvell FAE Vivid said that the action "Mirror To Anaylzer Port" in PCL
             * need to do a workaround to make it works.
             * Here is the description of the workaround provided by Vivid.
             * ''In the IPCL Action entry, instead of setting the <Enable Mirroring to Analyzer>,
             *  set the <Packet Command> to MIRROR-to-CPU and set the <User-Defined CPU Code>
             *  to a value in the user-defined CPU Code range, e.g. 200.
             *  In the CPU Code Table, assign entry 200 with a target device number,
             *  e.g. device 31.  This device number must be different from the Bobcat/Alleycat
             *  device numbers.
             *  In the Device Map Table, assign entry 31 with a target port equal to
             *  the target Analyzer port.
             *  The packet will be forwarded normally to its destination port(s), and
             *  also replicated to the target Analyzer port.''
             */
            action->pktCmd=CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            action->mirror.cpuCode = MIRROR_TO_ANALYZER_PORT_WAR_CPU_CODE;

//{{{
#if 0 // remark to pass linker
//}}}
        {
            UI32_T                                 device_id, dest_ifindex;
            UI32_T                                 unit, port;
            UI32_T                                 analyzer_moudle_id, analyzer_device_id, analyzer_phy_port;
            CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC cpu_code_table_entry;
            CPSS_CSCD_LINK_TYPE_STC                cascadeLink;
            GT_STATUS                              rc, rc_local;

            rc=rc_local=GT_OK;
            cpu_code_table_entry.tc=0;
            cpu_code_table_entry.dp=CPSS_DP_GREEN_E;
            cpu_code_table_entry.truncate=GT_FALSE;
            cpu_code_table_entry.cpuRateLimitMode=CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E;
            cpu_code_table_entry.cpuCodeRateLimiterIndex=0; /* 0 means no rate limit */
            cpu_code_table_entry.cpuCodeStatRateLimitIndex=0; /* don't care */
            cpu_code_table_entry.designatedDevNumIndex=MIRROR_TO_ANALYZER_PORT_WAR_DESIGNATED_DEV_NUM_INDEX; /* 0: local device, 1-7: remote device */

            SWCTRL_POM_GetVlanAndMacMirrorDestPort(&dest_ifindex);

            unit = ((dest_ifindex-1)/SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT) + 1;
            port = dest_ifindex - (unit-1)*SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT;
            if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &analyzer_moudle_id, &analyzer_device_id, &analyzer_phy_port) == FALSE)
            {
                printf("%s(%d): DEV_SWDRV_Logical2PhyDevicePortID() failed unit=%lu , port=%lu\r\n", __FUNCTION__, __LINE__, unit, port);
                return FALSE;
            }

            cascadeLink.linkNum=analyzer_phy_port;
            cascadeLink.linkType=CPSS_CSCD_LINK_TYPE_PORT_E;
            for (device_id = SYSTEM_DEV_NUM_MAC(0); device_id < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount); device_id++)
            {
                if (!PRV_CPSS_IS_DEV_EXISTS_MAC(device_id))
                {
                    continue;
                }

                rc_local=cpssDxChNetIfCpuCodeTableSet(device_id, action->mirror.cpuCode, &cpu_code_table_entry);
                rc |= rc_local;
                if (rc_local!=GT_OK)
                {
                    printf("%s(%d): cpssDxChNetIfCpuCodeTableSet failed.(rc=%d) device_id=%lu\n", __FUNCTION__, __LINE__, rc_local, device_id);
                }

                rc_local=cpssDxChNetIfCpuCodeDesignatedDeviceTableSet(device_id,
                                                                      MIRROR_TO_ANALYZER_PORT_WAR_DESIGNATED_DEV_NUM_INDEX,
                                                                      MIRROR_TO_ANALYZER_PORT_WAR_DEV_NUM);
                rc |= rc_local;
                if (rc_local!=GT_OK)
                {
                    printf("%s(%d): cpssDxChNetIfCpuCodeDesignatedDeviceTableSet failed.(rc=%d) device_id=%lu\n", __FUNCTION__, __LINE__, rc_local, device_id);
                }


                rc_local=cpssDxChCscdDevMapTableSet(device_id,
                                                    MIRROR_TO_ANALYZER_PORT_WAR_DEV_NUM,
#ifdef MARVELL_CPSS_3_4
                                                    analyzer_phy_port,
#endif
                                                    &cascadeLink,
                                                    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E/* don't care */);
                if (rc_local!=GT_OK)
                {
                    printf("%s(%d): cpssDxChCscdDevMapTableSet failed.(rc=%d) device_id=%lu an_devid=%lu an_phyid=%lu\n", __FUNCTION__, __LINE__, rc_local, device_id, analyzer_device_id, analyzer_phy_port);
                }

            }

            if (rc!=GT_OK)
                return FALSE;
        }
//{{{
#endif // #if 0
//}}}

#endif /* end of #else of #if !defined(MARVELL_ALLEYCAT) */
            break;

        case DEVRM_PKT_CMD_HARD_DROP:
            action->pktCmd = CPSS_PACKET_CMD_DROP_HARD_E;
            action->bypassIngressPipe = GT_TRUE;
            action->bypassBridge = GT_TRUE;
            break;

        case DEVRM_PKT_CMD_SOFT_DROP:
            action->pktCmd = CPSS_PACKET_CMD_DROP_SOFT_E;
            action->bypassIngressPipe = GT_TRUE;
            action->bypassBridge = GT_TRUE;
            break;

        default:
            break;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ConvertActionEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  Convert Upper layer action to the chipset action
 * INPUT:    action_entry
 * OUTPUT:   action
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_SetupEgrAction(
    const DEVRM_ACTION_T *dev_action_p,
    CPSS_DXCH_PCL_ACTION_STC *action)
{
    ASSERT(NULL != dev_action_p);

    action->egressPolicy = GT_TRUE;

    switch (dev_action_p->pkt_cmd)
    {
        case DEVRM_PKT_CMD_FORWARD:
            action->pktCmd = CPSS_PACKET_CMD_FORWARD_E;

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_queue)
            {
                DEVRM_LOG("<Warning> No support change queue on egress port");
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_inner_priority)
            {
                DEVRM_LOG("<Warning> No support change inner priority on egress port");
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_priority)
            {
                action->qos.modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                action->qos.qos.egress.up = dev_action_p->qos.priority;
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_dscp)
            {
                action->qos.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                action->qos.qos.egress.dscp = dev_action_p->qos.dscp;
            }

            if (DEVRM_MODIFY_VLAN_CMD_DISABLE != dev_action_p->vlan.modify_vlan)
            {
                action->vlan.modifyVlan = (DEVRM_MODIFY_VLAN_CMD_FOR_UNTAGGED_ONLY == dev_action_p->vlan.modify_vlan) ?
                                            CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E :
                                          (DEVRM_MODIFY_VLAN_CMD_FOR_TAGGED_ONLY == dev_action_p->vlan.modify_vlan) ?
                                            CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E :
                                            CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
                action->vlan.vlanId = dev_action_p->vlan.vid;
                action->vlan.nestedVlan = (dev_action_p->vlan.add_outer_tag) ? GT_TRUE : GT_FALSE;
            }

            break;

        case DEVRM_PKT_CMD_HARD_DROP:
        case DEVRM_PKT_CMD_SOFT_DROP:
            action->pktCmd = CPSS_PACKET_CMD_DROP_HARD_E;
            break;

        default:
            DEVRM_LOG("<Error> unknown pkt_cmd = %d\r\n", dev_action_p->pkt_cmd);
            break;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetupMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  Convert Upper layer action to the chipset action
 * INPUT:    action_entry
 * OUTPUT:   action
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_SetupMeter(
    UI32_T device_id,
    DEVRM_PCE_T *pce,
    const DEVRM_ACTION_T *dev_action_p)
{
    GT_STATUS rc;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT          policer_stage;
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC     policer_entry;
    CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT    tbParams;
    GT_U32 yellowQosTableRemarkIndex = 0;
    GT_U32 redQosTableRemarkIndex = 0;

    DEVRM_CHECK_DEVICE_ID(device_id);

    ASSERT(NULL != dev_action_p);

    ASSERT(DEVRM_POLICER_MODE_DISABLE != dev_action_p->policer.mode);
    ASSERT(DEVRM_POLICER_MODE_METER_ONLY == dev_action_p->policer.mode ||
           DEVRM_POLICER_MODE_METER_AND_COUNTER == dev_action_p->policer.mode ||
           DEVRM_POLICER_MODE_COUNTER_ONLY == dev_action_p->policer.mode);

    if (TRUE != DEVRM_GetPolicerStageFromPce(device_id, pce, &policer_stage))
    {
        printf("%s(%d):PCE(%lu) is not valid!\r\n", __FUNCTION__, __LINE__, pce->upper_rule_id);
        ASSERT(0);
        return FALSE;
    }

    /* allocate a policer if there is no policer bind to PCE before.
     */
    if (DEVRM_AllocateMeter(device_id, policer_stage, &pce->policer_idx) == FALSE)
    {
        DEVRM_LOG("No meter resource!");
        return FALSE;
    }

    memset(&policer_entry, 0, sizeof(CPSS_DXCH3_POLICER_METERING_ENTRY_STC));
    memset(&tbParams, 0, sizeof(CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT));

    policer_entry.mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E;

    if (CPSS_DXCH_POLICER_STAGE_EGRESS_E == policer_stage)
    {
        policer_entry.modifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        policer_entry.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        policer_entry.modifyDp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
    }

    switch (dev_action_p->policer.meter.mode)
    {
        case DEVRM_POLICER_METER_MODE_TRTCM_COLOR_BLIND:
            policer_entry.meterMode = CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E;
            policer_entry.meterColorMode = CPSS_POLICER_COLOR_BLIND_E;
            policer_entry.tokenBucketParams.trTcmParams.cir = dev_action_p->policer.meter.kbits_sec;
            policer_entry.tokenBucketParams.trTcmParams.cbs = dev_action_p->policer.meter.kbytes_burst_size + 7 / 8;
            policer_entry.tokenBucketParams.trTcmParams.pir = dev_action_p->policer.meter.peak_kbits_sec;
            policer_entry.tokenBucketParams.trTcmParams.pbs = dev_action_p->policer.meter.peak_kbytes_burst_size + 7 / 8;
            break;

        case DEVRM_POLICER_METER_MODE_TRTCM_COLOR_AWARE:
            policer_entry.meterMode = CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E;
            policer_entry.meterColorMode = CPSS_POLICER_COLOR_AWARE_E;
            policer_entry.tokenBucketParams.trTcmParams.cir = dev_action_p->policer.meter.kbits_sec;
            policer_entry.tokenBucketParams.trTcmParams.cbs = dev_action_p->policer.meter.kbytes_burst_size + 7 / 8;
            policer_entry.tokenBucketParams.trTcmParams.pir = dev_action_p->policer.meter.peak_kbits_sec;
            policer_entry.tokenBucketParams.trTcmParams.pbs = dev_action_p->policer.meter.peak_kbytes_burst_size + 7 / 8;
            break;

        case DEVRM_POLICER_METER_MODE_SRTCM_COLOR_AWARE:
            policer_entry.meterMode = CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E;
            policer_entry.meterColorMode = CPSS_POLICER_COLOR_AWARE_E;
            policer_entry.tokenBucketParams.srTcmParams.cir = dev_action_p->policer.meter.kbits_sec;
            policer_entry.tokenBucketParams.srTcmParams.cbs = dev_action_p->policer.meter.kbytes_burst_size +7 / 8;
            policer_entry.tokenBucketParams.srTcmParams.ebs = dev_action_p->policer.meter.peak_kbytes_burst_size + 7 / 8;
            break;

        case DEVRM_POLICER_METER_MODE_SRTCM_COLOR_BLIND:
            policer_entry.meterMode = CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E;
            policer_entry.meterColorMode = CPSS_POLICER_COLOR_BLIND_E;
            policer_entry.tokenBucketParams.srTcmParams.cir = dev_action_p->policer.meter.kbits_sec;
            policer_entry.tokenBucketParams.srTcmParams.cbs = dev_action_p->policer.meter.kbytes_burst_size +7 / 8;
            policer_entry.tokenBucketParams.srTcmParams.ebs = dev_action_p->policer.meter.peak_kbytes_burst_size + 7 / 8;
            break;

        case DEVRM_POLICER_METER_MODE_FLOW:
        default:
            policer_entry.meterMode = CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E;
            policer_entry.meterColorMode = CPSS_POLICER_COLOR_BLIND_E;
            policer_entry.tokenBucketParams.srTcmParams.cir = dev_action_p->policer.meter.kbits_sec;
            policer_entry.tokenBucketParams.srTcmParams.cbs = dev_action_p->policer.meter.kbytes_burst_size +7 / 8;

            policer_entry.tokenBucketParams.srTcmParams.ebs =
                         policer_entry.tokenBucketParams.srTcmParams.cbs + 1522;

            break;
    }

    if (DEVRM_PKT_CMD_FORWARD == dev_action_p->policer.yellow_pkt_cmd)
    {
        if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->policer.yellow_pkt_qos.modify_dscp)
        {
            if (CPSS_DXCH_POLICER_STAGE_EGRESS_E == policer_stage)
            {
                DEVRM_LOG("<Error> Not support change DSCP for yellow packet on egress port");
                return FALSE;
            }

            policer_entry.yellowPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E;
            policer_entry.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            yellowQosTableRemarkIndex = DEV_SWDRVL4_DscpToQosProfileIdx(dev_action_p->policer.yellow_pkt_qos.dscp);
        }
    }

    if (DEVRM_PKT_CMD_HARD_DROP == dev_action_p->policer.yellow_pkt_cmd ||
        DEVRM_PKT_CMD_SOFT_DROP == dev_action_p->policer.yellow_pkt_cmd)
    {
        policer_entry.yellowPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E;
    }

    if (DEVRM_PKT_CMD_FORWARD == dev_action_p->policer.red_pkt_cmd)
    {
        if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->policer.red_pkt_qos.modify_dscp)
        {
            if (CPSS_DXCH_POLICER_STAGE_EGRESS_E == policer_stage)
            {
                DEVRM_LOG("<Error> Not support change DSCP for red packet on egress port");
                return FALSE;
            }
        }

        policer_entry.redPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E;
        policer_entry.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        redQosTableRemarkIndex = DEV_SWDRVL4_DscpToQosProfileIdx(dev_action_p->policer.red_pkt_qos.dscp);
    }

    if (DEVRM_PKT_CMD_HARD_DROP == dev_action_p->policer.red_pkt_cmd ||
        DEVRM_PKT_CMD_SOFT_DROP == dev_action_p->policer.red_pkt_cmd)
    {
        policer_entry.redPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E;
    }

    if (policer_entry.redPcktCmd == CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E &&
        policer_entry.yellowPcktCmd == CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E)
    {
        UI32_T qos_remarking_idx;

        policer_entry.redPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E;
        policer_entry.yellowPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E;

        if (!DEVRM_AllocateQosRemarking(device_id, &qos_remarking_idx))
        {
            DEVRM_LOG("<Error> No QoS remarking entry available");
            return FALSE;
        }

        rc = cpssDxCh3PolicerQosRemarkingEntrySet(device_id,
                                                  policer_stage,
                                                  qos_remarking_idx,
                                                  yellowQosTableRemarkIndex,
                                                  redQosTableRemarkIndex);

        if (rc != GT_OK)
        {
            DEVRM_LOG("<Error> QosRemarkingEntrySet: Policer Stage=%s, policer_idx=%hu\r\n",
                DEVRM_POLICER_STAGE_STR(policer_stage),
                pce->policer_idx);

            DEBUG_ERROR_MSG(rc);
        }

        pce->qos_remarking_idx = qos_remarking_idx;
    }
    else if (policer_entry.redPcktCmd == CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E ||
             policer_entry.yellowPcktCmd == CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E)
    {
        policer_entry.qosProfile =
            policer_entry.yellowPcktCmd == CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E ?
                yellowQosTableRemarkIndex : redQosTableRemarkIndex;
    }

    if (DEVRM_POLICER_MODE_METER_AND_COUNTER == dev_action_p->policer.mode ||
        DEVRM_POLICER_MODE_METER_ONLY == dev_action_p->policer.mode)
    {
        CPSS_DXCH3_POLICER_BILLING_ENTRY_STC billingCntr;

        memset(&billingCntr, 0, sizeof(billingCntr));
        billingCntr.billingCntrMode =
                    (dev_action_p->policer.counter_mode == DEVRM_POLICER_COUNTER_MODE_BYTE) ?
                                    CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E :
                                    CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E;

        rc = cpssDxCh3PolicerBillingEntrySet(device_id,
                                             policer_stage,
                                             pce->policer_idx,
                                             &billingCntr);

        if (rc != GT_OK)
        {
            DEVRM_LOG("<Error> BillingEntrySet: Policer Stage=%s, policer_idx=%hu\r\n",
                DEVRM_POLICER_STAGE_STR(policer_stage),
                pce->policer_idx);

            DEBUG_ERROR_MSG(rc);
        }
        else
        {
            policer_entry.countingEntryIndex = pce->policer_idx;

            /* clear counter */
            rc = cpssDxCh3PolicerBillingEntryGet(device_id,
                                                 policer_stage,
                                                 pce->policer_idx,
                                                 GT_TRUE,
                                                 &billingCntr);

            if (rc != GT_OK)
            {
                DEVRM_LOG("<Error> BillingEntryGet: Policer Stage=%s, policer_idx=%hu\r\n",
                    DEVRM_POLICER_STAGE_STR(policer_stage),
                    pce->policer_idx);

                DEBUG_ERROR_MSG(rc);
            }
        }
    }

    rc = cpssDxCh3PolicerMeteringEntrySet(device_id,
                                          policer_stage,
                                          pce->policer_idx,
                                          &policer_entry,
                                          &tbParams);
    if (rc != GT_OK)
    {
        DEVRM_LOG("<Error> Policer Stage = %s\r\n", DEVRM_POLICER_STAGE_STR(policer_stage));

        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    return TRUE;

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    device_id, selector_index, logical_rule_id, rule_entry, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     DEVRM_Rule = class_element + meter + action
 *           pass meter_entry with NULL if this rule does not have meter
 *           pass action_entry with NULL if this rule does not have action
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_SetRule(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T    *ace_entry,
    const DEVRM_ACTION_T *dev_action_p)
{
    GT_STATUS rc;

    DEVRM_PCL_T *pcl;
    DEVRM_PCE_T *pce;

    CPSS_DXCH_PCL_RULE_FORMAT_UNT mask, pattern;
    CPSS_DXCH_PCL_ACTION_STC action;
    UI32_T rule_size;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC lookupCfg;

    ASSERT(NULL != ace_entry);
    ASSERT(NULL != dev_action_p);

    DEVRM_LOG("deviceId=%lu, pclId=%lu, devRuleId=%lu",
              device_id, pcl_id, dev_rule_id);

//    DEVRM_LIB_ActionDump(dev_action_p, BACKDOOR_MGR_Printf, 0);

    DEVRM_CHECK_DEVICE_ID(device_id);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    ASSERT(NULL != pcl);

    if (NULL == pcl)
    {
        DEVRM_LOG("Invalid pclId=%lu", pcl_id);
        return FALSE;
    }

    /* Find the PCE correspond to the logical_rule_id in PCLs
     */
    if (DEVRM_GetPceFromPcl(device_id, pcl_id, dev_rule_id, &pce) ==  FALSE)
    {
        ASSERT(0);
        DEVRM_LOG("Invalid devRuleId=%lu", dev_rule_id);
        return FALSE;
    }

    memset(&mask, 0, sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    memset(&pattern, 0, sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    memset(&action, 0, sizeof(CPSS_DXCH_PCL_ACTION_STC));

    action.pktCmd = CPSS_PACKET_CMD_FORWARD_E;

    rule_size = DEVRM_GetRuleSize(pce->pce_type);

    /* Convert PCL type to PCE type by input rule type
     */
    if (DEVRM_SetupKeyTypes(pcl->pcl_type, &lookupCfg) == FALSE)
    {
        return FALSE;
    }

    /* Convert key format of PCE from ether-type
     */
    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ETHER)
    {
        if (0x86DD == ace_entry->ethertype_data &&
            0xFFFF == ace_entry->ethertype_mask)
        {
            pce->pce_type = lookupCfg.groupKeyTypes.ipv6Key;
        }
        else if (0x0800 == ace_entry->ethertype_data &&
                 0xFFFF == ace_entry->ethertype_mask)
        {
            pce->pce_type = lookupCfg.groupKeyTypes.ipv4Key;
        }
        else
        {
            /** NOTE. Something is wrong when ...
             * If the qualified ethertype across the IPv6 and no IPv6.
             * For example, if the ethertype and mask is 0x86DF/0xFFFD.
             * We will choose a nonIpKey format.
             * But when incomming packet is IPv6 packet (ethertype=86DD)
             * it will not match.
             *
             * Now we only have this problem for IPv6 packet.
             * If the ethertype across the IPv4 and no IPv4, then it
             * is OK (Because the key format for non IP and IPv4 is the
             * same).
             */
            pce->pce_type = lookupCfg.groupKeyTypes.nonIpKey;
        }
    }
    else
    {
        pce->pce_type = lookupCfg.groupKeyTypes.nonIpKey;
    }

    /* Check rule size
     * allocate_rule.size MUST equal set_rule.size
     */
    ASSERT(rule_size == DEVRM_GetRuleSize(pce->pce_type));

    if (rule_size != DEVRM_GetRuleSize(pce->pce_type))
    {
        DEVRM_LOG("<Error> Rule size from alocation is not equal current set!! (pce_type=%d)",
                  pce->pce_type);
        return FALSE;
    }

    /* Convert upper layer rule to Pce
     */
    if (TRUE != DEVRM_ConvertRuleToPce(pce->pce_type,
                                       ace_entry,
                                       &mask,
                                       &pattern))

    {
        ASSERT(0);
        DEVRM_LOG("Failed to convert PCE");
        return FALSE;
    }

    /* Setup Action
     */
    if (CPSS_PCL_DIRECTION_INGRESS_E == pcl->direction)
    {
        if (DEVRM_SetupAction(dev_action_p, &action) == FALSE)
        {
            DEVRM_LOG("DEVRM_SetupAction failed");
            return FALSE;
        }
    }
    else
    {
        if (DEVRM_SetupEgrAction(dev_action_p, &action) == FALSE)
        {
            DEVRM_LOG("DEVRM_SetupEgrAction failed");
            return FALSE;
        }
    }

    /* Setup Meters and Meter Actions
     * Meter actions will be in the upper layer action list, so it needs to passed the
     * action list to extract it again
     */
    if (DEVRM_POLICER_MODE_DISABLE != dev_action_p->policer.mode)
    {
        if (DEVRM_SetupMeter(device_id, pce, dev_action_p) == FALSE)
        {
            DEVRM_LOG("SetupMeter failed");
            return FALSE;
        }

        /* Bind policer to Action
         */
        action.policer.policerEnable =
            DEVRM_POLICER_MODE_METER_ONLY == dev_action_p->policer.mode ?
                                CPSS_DXCH_PCL_POLICER_ENABLE_METER_ONLY_E :
                                CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E;

        action.policer.policerId = pce->policer_idx;

        /* Setup QoS profile for QoS remarking
         */
        if (pce->qos_remarking_idx != -1)
        {
            if (action.qos.qos.ingress.profileAssignIndex)
            {
                CPSS_DXCH_COS_PROFILE_STC cos_profile;

                memset(&cos_profile, 0, sizeof(cos_profile));

                rc = cpssDxChCosProfileEntryGet(device_id,
                                                action.qos.qos.ingress.profileIndex,
                                                &cos_profile);
                if (rc != GT_OK)
                {
                    DEBUG_ERROR_MSG(rc);
                }

                rc = cpssDxChCosProfileEntrySet(device_id,
                                                pce->qos_remarking_idx,
                                                &cos_profile);
                if (rc != GT_OK)
                {
                    DEBUG_ERROR_MSG(rc);
                }
            }

            action.qos.qos.ingress.profileIndex = pce->qos_remarking_idx;
            action.qos.qos.ingress.profileAssignIndex = GT_TRUE;
        }
    }

    /* Finally, set the rule
     */
    rc = cpssDxChPclRuleSet(device_id,
                            pce->pce_type,
                            DEVRM_Translate_PCEIDwithType(pce->pce_type, pce->hw_idx),

#ifdef MARVELL_CPSS_3_4
                            0,
#endif

                            &mask,
                            &pattern,
                            &action);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_UndoRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Release resource which be allocated by DEVRM_SetRule
 * INPUT:    pce    - PCE entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_UndoRule(
    UI32_T device_id,
    DEVRM_PCE_T *pce)
{
    CPSS_PCL_RULE_SIZE_ENT              ruleSize;
    GT_U32                              ruleIndex;
    GT_BOOL                             valid;
    CPSS_PCL_RULE_SIZE_ENT              ruleSizeGet;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage;
    GT_STATUS                           rc;

    DEVRM_CHECK_DEVICE_ID(device_id);
    ASSERT(NULL != pce);

    ruleSize = (pce->is_std_pce == TRUE) ? CPSS_PCL_RULE_SIZE_STD_E :
    CPSS_PCL_RULE_SIZE_EXT_E;
    ruleIndex = DEVRM_Translate_PCEIDwithType(pce->pce_type, pce->hw_idx);

    rc = cpssDxChPclRuleAnyStateGet(device_id, ruleSize, ruleIndex, &valid, &ruleSizeGet);

    if (GT_OK == rc && GT_TRUE == valid)
    {
        ASSERT(ruleSizeGet == ruleSize);

        rc = cpssDxChPclRuleInvalidate(device_id, ruleSize, ruleIndex);

        if (GT_OK != rc)
        {
            DEVRM_LOG("cpssDxChPclRuleInvalidate, devNum=%lu, ruleSize=%s, ruleIndex=%lu",
                      device_id,
                      CPSS_PCL_RULE_SIZE_STD_E == ruleSize ? "std" : "ext",
                      ruleIndex);
            return FALSE;
        }
    }

    if (-1 != pce->policer_idx)
    {
        if (TRUE != DEVRM_GetPolicerStageFromPce(device_id, pce, &stage))
        {
            ASSERT(0);
            return FALSE;
        }

        DEVRM_FreeMeter(device_id, stage, &pce->policer_idx);
        pce->policer_idx = -1;
    }

    if (-1 != pce->qos_remarking_idx)
    {
        DEVRM_FreeQosRemarking(device_id, pce->qos_remarking_idx);
        pce->qos_remarking_idx = -1;
    }

    return TRUE;
}

static CPSS_PCL_RULE_SIZE_ENT
DEVRM_GetRuleSize(
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type)
{
    UI32_T ret;
    switch (pce_type)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
            ret = CPSS_PCL_RULE_SIZE_STD_E;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
            ret = CPSS_PCL_RULE_SIZE_EXT_E;
            break;
        default:
            ret = CPSS_PCL_RULE_SIZE_STD_E;
    }
    return ret;

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a rule entry.
 * INPUT:    aclId, logical_rule_id, is_default_acl
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     1. If meter is shared, when delete it should check ..
 *           2. default pcl can not be resized.
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_DestroyRule(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T logical_rule_id)
{
    DEVRM_PCE_T *pce = NULL;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (DEVRM_GetPceFromPcl(device_id, pcl_id, logical_rule_id, &pce) == FALSE)
    {
        ASSERT(0);
        return FALSE;
    }

    if (DEVRM_UndoRule(device_id, pce) == FALSE)
    {
        ASSERT(0);
        return FALSE;
    }

    if (DEVRM_FreePce(device_id, pcl_id, pce->is_std_pce, pce) == FALSE)
    {
        ASSERT(0);
        return FALSE;
    }

    return TRUE;
}

/* meter */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_UpdateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  update a meter entry.
 * INPUT:    device_id, logical_rule_id, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     Only spport to change Token Bucket's parameters.
 *           It should not add / delete action for yellow and red packet.
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_UpdateMeter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    const DEVRM_POLICER_METER_T *dev_meter_p)
{

    GT_STATUS                               rc;

    DEVRM_PCL_T                             *pcl;
    DEVRM_PCE_T                             *pce;

    CPSS_DXCH_POLICER_STAGE_TYPE_ENT        policer_stage;
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC   policer_entry;
    CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT  tbParams;

    ASSERT(NULL != dev_meter_p);

    DEVRM_CHECK_DEVICE_ID(device_id);

    DEVRM_LOG("deviceId=%lu, pclId=%lu, devRuleId=%lu, CIR=%lu",
              device_id, pcl_id, dev_rule_id, dev_meter_p->kbits_sec);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    if (NULL == pcl)
    {
        ASSERT(0);
        return FALSE;
    }

    if (DEVRM_GetPceFromPcl(device_id, pcl_id, dev_rule_id, &pce) == FALSE)
    {
        ASSERT(0);
        return FALSE;
    }

    if (pce->policer_idx < 0)
    {
        ASSERT(0);
        return FALSE;
    }

    policer_stage = DEVRM_POLICER_STAGE( pcl->direction );

    rc = cpssDxCh3PolicerMeteringEntryGet(device_id,
                                          policer_stage,
                                          pce->policer_idx,
                                          &policer_entry);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    /** sync from DEVRM_SetupMeter ...
     */

    switch (dev_meter_p->mode)
    {
        case DEVRM_POLICER_METER_MODE_TRTCM_COLOR_BLIND:
            policer_entry.meterMode = CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E;
            policer_entry.meterColorMode = CPSS_POLICER_COLOR_BLIND_E;
            policer_entry.tokenBucketParams.trTcmParams.cir = dev_meter_p->kbits_sec;
            policer_entry.tokenBucketParams.trTcmParams.cbs = dev_meter_p->kbytes_burst_size + 7 / 8;
            policer_entry.tokenBucketParams.trTcmParams.pir = dev_meter_p->peak_kbits_sec;
            policer_entry.tokenBucketParams.trTcmParams.pbs = dev_meter_p->peak_kbytes_burst_size + 7 / 8;
            break;

        case DEVRM_POLICER_METER_MODE_TRTCM_COLOR_AWARE:
            policer_entry.meterMode = CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E;
            policer_entry.meterColorMode = CPSS_POLICER_COLOR_AWARE_E;
            policer_entry.tokenBucketParams.trTcmParams.cir = dev_meter_p->kbits_sec;
            policer_entry.tokenBucketParams.trTcmParams.cbs = dev_meter_p->kbytes_burst_size + 7 / 8;
            policer_entry.tokenBucketParams.trTcmParams.pir = dev_meter_p->peak_kbits_sec;
            policer_entry.tokenBucketParams.trTcmParams.pbs = dev_meter_p->peak_kbytes_burst_size + 7 / 8;
            break;

        case DEVRM_POLICER_METER_MODE_SRTCM_COLOR_AWARE:
            policer_entry.meterMode = CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E;
            policer_entry.meterColorMode = CPSS_POLICER_COLOR_AWARE_E;
            policer_entry.tokenBucketParams.srTcmParams.cir = dev_meter_p->kbits_sec;
            policer_entry.tokenBucketParams.srTcmParams.cbs = dev_meter_p->kbytes_burst_size +7 / 8;
            policer_entry.tokenBucketParams.srTcmParams.ebs = dev_meter_p->peak_kbytes_burst_size + 7 / 8;
            break;

        case DEVRM_POLICER_METER_MODE_SRTCM_COLOR_BLIND:
            policer_entry.meterMode = CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E;
            policer_entry.meterColorMode = CPSS_POLICER_COLOR_BLIND_E;
            policer_entry.tokenBucketParams.srTcmParams.cir = dev_meter_p->kbits_sec;
            policer_entry.tokenBucketParams.srTcmParams.cbs = dev_meter_p->kbytes_burst_size +7 / 8;
            policer_entry.tokenBucketParams.srTcmParams.ebs = dev_meter_p->peak_kbytes_burst_size + 7 / 8;
            break;

        case DEVRM_POLICER_METER_MODE_FLOW:
        default:
            policer_entry.meterMode = CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E;
            policer_entry.meterColorMode = CPSS_POLICER_COLOR_BLIND_E;
            policer_entry.tokenBucketParams.srTcmParams.cir = dev_meter_p->kbits_sec;
            policer_entry.tokenBucketParams.srTcmParams.cbs = dev_meter_p->kbytes_burst_size +7 / 8;

            policer_entry.tokenBucketParams.srTcmParams.ebs =
                        policer_entry.tokenBucketParams.srTcmParams.cbs + 1522;
            break;
    }

    rc = cpssDxCh3PolicerMeteringEntrySet(device_id,
                                          policer_stage,
                                          pce->policer_idx,
                                          &policer_entry,
                                          &tbParams);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    rc = cpssDxCh3PolicerMeteringEntryRefresh(device_id,
                                              policer_stage,
                                              pce->policer_idx);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ShareMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  share a meter from src_logical_rule_id to dst_logical_rule_id
 * INPUT:    device_id, src_logical_rule_id, dst_logical_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_ShareMeter(
    UI32_T device_id,
    UI32_T src_logical_rule_id,
    UI32_T dst_logical_rule_id)
{
    UI32_T  policer_tbl_size = 0;
    UI16_T  *policer_tbl = NULL;

    GT_STATUS rc;
    DEVRM_PCL_T *pcl_src;
    DEVRM_PCL_T *pcl_dst;
    DEVRM_PCE_T *pce_src = NULL;
    DEVRM_PCE_T *pce_dst = NULL;
    CPSS_DXCH_PCL_ACTION_STC action_src;
    CPSS_DXCH_PCL_ACTION_STC action_dst;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (FALSE == DEVRM_GetPce(device_id, src_logical_rule_id, &pce_src))
    {
        DEVRM_LOG("Invalid src rule id %lu", src_logical_rule_id);
        return FALSE;
    }

    if (FALSE == DEVRM_GetPce(device_id, dst_logical_rule_id, &pce_dst))
    {
        DEVRM_LOG("Invalid dst rule id %lu", dst_logical_rule_id);
        return FALSE;
    }

    ASSERT(NULL != pce_src);
    ASSERT(NULL != pce_dst);

    /* if there is no meter in src pce, then fail
     */
    if (pce_src->policer_idx < 0)
    {
        DEBUG_ERROR_MSG(0);
        return FALSE;
    }

    /* there is one meter entry belong to dst. pce
     */
    if (pce_dst->policer_idx >=0 )
    {
        DEBUG_ERROR_MSG(0);
        return FALSE;
    }

    pcl_src = DEVRM_GetPcl(device_id, pce_src->pcl_id);
    if (NULL == pcl_src)
    {
        DEVRM_LOG("Invalid src pcl id %hu", pce_src->pcl_id);
        return FALSE;
    }

    pcl_dst = DEVRM_GetPcl(device_id, pce_dst->pcl_id);
    if (NULL == pcl_dst)
    {
        DEVRM_LOG("Invalid dst pcl id %hu", pce_dst->pcl_id);
        return FALSE;
    }

    if (pcl_src->direction != pcl_dst->direction)
    {
        DEVRM_LOG("The rule with different stage can not share the same policer");
        return FALSE;
    }

    if (CPSS_PCL_DIRECTION_INGRESS_E == pcl_src->direction)
    {
        policer_tbl = DEVRM_DEVICE_PTR(device_id)->policer_use_status;
        policer_tbl_size = _countof(DEVRM_DEVICE_PTR(device_id)->policer_use_status);
    }
#if (0 < DEVRM_MAX_NBR_OF_EGRESS_METER_ENTRIES)
    else if (CPSS_PCL_DIRECTION_EGRESS_E == pcl_src->direction)
    {
        policer_tbl = DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status;
        policer_tbl_size = _countof(DEVRM_DEVICE_PTR(device_id)->egress_policer_use_status);
    }
#endif /* DEVRM_MAX_NBR_OF_EGRESS_METER_ENTRIES */
    else
    {
        DEBUG_PRINT_STR("Unknown direction %d", pcl_src->direction);
        return FALSE;
    }

    /* Find the correspondent action and derefernce the policer
     */
    rc = cpssDxChPclRuleActionGet(device_id,
                                  DEVRM_GetRuleSize(pce_src->pce_type),
                                  DEVRM_Translate_PCEIDwithType(pce_src->pce_type, pce_src->hw_idx),
                                  pcl_src->direction,
                                  &action_src);
    if (GT_OK != rc)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    rc = cpssDxChPclRuleActionGet(device_id,
                                  DEVRM_GetRuleSize(pce_dst->pce_type),
                                  DEVRM_Translate_PCEIDwithType(pce_dst->pce_type, pce_dst->hw_idx),
                                  pcl_dst->direction,
                                  &action_dst);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    action_dst.policer.policerEnable = action_src.policer.policerEnable;
    action_dst.policer.policerId = action_src.policer.policerId;

    rc = cpssDxChPclRuleActionUpdate(device_id,
                                     DEVRM_GetRuleSize(pce_dst->pce_type),
                                     DEVRM_Translate_PCEIDwithType(pce_dst->pce_type, pce_dst->hw_idx),
                                     &action_dst);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    /* update meter entry reference DB
     */
    pce_dst->policer_idx = pce_src->policer_idx;
    policer_tbl[ pce_dst->policer_idx ]++;

    return TRUE;

}

/* action */
/*------------------------------------------------------------------------------
 * ROUTINE NAME  - DEVRM_CreatePcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Create PCL which used to keep PCE
 * INPUT    : device_id     - device ID
 *            pcl_id        - PCL ID
 *            is_ingress    - TRUE means ingress direction;
 *                            FALSE means egress direction
 *            lookup_num    - lookup number
 *            rule_type     - rule type
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_CreatePcl(
    UI32_T device_id,
    UI32_T pcl_id,
    BOOL_T is_ingress,
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num,
    UI32_T rule_type)
{
    DEVRM_PCL_T *pcl;

    ASSERT(TRUE == is_ingress || FALSE == is_ingress);
    ASSERT(DEVRM_PCL_LOOKUP_MIN <= lookup_num && lookup_num <= DEVRM_PCL_LOOKUP_MAX);
    ASSERT(0 < pcl_id && pcl_id <= _countof(DEVRM_DEVICE_PTR(device_id)->pcls));

    DEVRM_LOG("deviceId=%lu, pclId=%lu, isIngress=%u, lookupNum=%d, ruleType=%lu",
              device_id, pcl_id, is_ingress, lookup_num, rule_type);

    if (pcl_id > DEVRM_MAX_NBR_OF_PCL || pcl_id == 0)
    {
        DEVRM_LOG("Invalie pcl_id=%lu", pcl_id);
        return FALSE;
    }

    pcl = &DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id - 1];

    if (pcl->is_used)
    {
        if (0 != pcl->pce_number)
        {
            DEVRM_LOG("<Error> PCI(%lu) is exist", pcl_id);
            ASSERT(0);
            return FALSE;
        }

        ASSERT(pcl->direction == (is_ingress ?
                                  CPSS_PCL_DIRECTION_INGRESS_E : CPSS_PCL_DIRECTION_EGRESS_E));
        ASSERT(pcl->lookup_num == lookup_num);
        ASSERT(pcl->pcl_type == (is_ingress ?
                                 DEVRM_PclTypeConverter[rule_type] :
                                 CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E));

        if (pcl->direction != (is_ingress ? CPSS_PCL_DIRECTION_INGRESS_E : CPSS_PCL_DIRECTION_EGRESS_E) ||
            pcl->lookup_num != lookup_num ||
            pcl->pcl_type != (is_ingress ?
                              DEVRM_PclTypeConverter[rule_type] :
                              CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E))
        {
            DEVRM_LOG("<Error> PCI(%lu) is exist", pcl_id);
            ASSERT(0);
            return FALSE;
        }

        return TRUE;
    }

    pcl->is_used = TRUE;
    pcl->direction = (is_ingress) ? CPSS_PCL_DIRECTION_INGRESS_E :
                                    CPSS_PCL_DIRECTION_EGRESS_E;

    pcl->lookup_num = lookup_num;

    /* FIXIT: DEVRM_CreateAcl just patch it. Fix it later. */
    pcl->pcl_type  = (is_ingress) ?
                     DEVRM_PclTypeConverter[rule_type] :
                     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;

    if (pcl->pcl_type == CPSS_DXCH_PCL_RULE_FORMAT_LAST_E)
    {
        printf("%s(%d):Type resolve error!\r\n", __FUNCTION__, __LINE__);
        SYSFUN_ASSERT(0);
    }

    DEVRM_DEVICE_PTR(device_id)->pcls_use_status[(pcl_id-1)/8] |= 0x80 >> ((pcl_id-1)%8);

    return TRUE;

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy the ACL.
 * INPUT:    device_id, logical_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_DestroyAcl(
    UI32_T device_id,
    UI32_T pcl_id)
{
    DEVRM_PCL_T *pcl;
    DEVRM_PCE_T *pce = NULL;

    DEVRM_CHECK_DEVICE_ID(device_id);

    DEVRM_LOG("deviceId=%lu, pclId=%lu", device_id, pcl_id);

    pcl = DEVRM_GetPcl(device_id, pcl_id);

    if (NULL == pcl)
    {
        return TRUE;
    }

    while (pcl->last_pce)
    {
        /* For ext_pce
         */
        if (pcl->last_pce->hw_idx != pcl->last_pce->extended_hw_idx)
        {
            pce = pcl->last_pce->prev;
        }
        /* For std_pce
         */
        else
        {
            pce = pcl->last_pce;
        }

        /* Nothing to do, if undo rule failed.
         */
        DEVRM_UndoRule(device_id, pce);

        if (DEVRM_FreePce(device_id, pcl_id, pce->is_std_pce, pce) == FALSE)
        {
            DEBUG_ERROR_MSG(0);
            return FALSE;
        }
    }

    pcl->is_used = FALSE;
    DEVRM_DEVICE_PTR(device_id)->pcls_use_status[(pcl_id-1)/8] &= ~(0x80 >>((pcl_id-1) %8));

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - DEVRM_BindPclToPort
 *------------------------------------------------------------------------------
 * PURPOSE  : Bind PCL to port
 * INPUT    : device_id - device ID
 *            phy_port  - phy port
 *            pcl_id    - PCL ID
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_BindPclToPort(
    UI32_T  device_id,
    UI32_T  phy_port,
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

    DEVRM_LOG("deviceId=%lu, port=%lu, pclId=%lu",
              device_id, phy_port, pcl_id);

    DEVRM_CHECK_DEVICE_ID(device_id);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    ASSERT(NULL != pcl);

    if (NULL == pcl)
    {
        ASSERT(0);
        return FALSE;
    }

    /* For alleycat rev 2, if bind egress pcl,
     * TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA will not work.
     */
    if (PRV_CPSS_PP_MAC(device_id)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E &&
        PRV_CPSS_PP_MAC(device_id)->revision <= 2)
    {
        if (pcl->direction == CPSS_PCL_DIRECTION_EGRESS_E)
        {
            DEVRM_LOG("deviceId=%lu, port=%lu, pclId=%lu, ignroe binding",
                device_id, phy_port, pcl_id);
            printf("%s:%d: devport:%lu/%lu pcl:%lu\n",__func__,__LINE__,device_id,phy_port,pcl_id);
            return TRUE;
        }
    }

    DEVRM_LOG("pcl.direction=%s\r\n"
              "  pcl.lookup_num=%s",
              CPSS_PCL_DIRECTION_INGRESS_E == pcl->direction ? "in" : "out",
              DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ? "0-0" :
              DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num ? "0-1" :
              DEVRM_PCL_LOOKUP_1   == pcl->lookup_num ? "1" : "?");

    direction = pcl->direction;
    lookupNum = (DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ||
                 DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num) ? CPSS_PCL_LOOKUP_0_E :
                (DEVRM_PCL_LOOKUP_1   == pcl->lookup_num) ? CPSS_PCL_LOOKUP_1_E : 0xCCCCCCCC;

    if (STKTPLG_POM_GetMyUnitID(&my_unit_id) == FALSE)
    {
        DEVRM_LOG("<Error> STKTPLG_OM_GetMyUnitID failed");
        return FALSE;
    }

    if (DEV_SWDRV_GetModuleIdFromLocalDeviceId(my_unit_id, device_id, &hw_dev_id) == FALSE)
    {
        DEVRM_LOG("<Error> DEV_SWDRV_GetModuleIdFromLocalDeviceId failed\n");
        return FALSE;
    }

    interfaceInfo.type = CPSS_INTERFACE_PORT_E;
    interfaceInfo.devPort.devNum = hw_dev_id;
    interfaceInfo.devPort.portNum = phy_port;
    interfaceInfo.devNum = hw_dev_id;

    lookupCfg.enableLookup = GT_TRUE;
    lookupCfg.dualLookup   = GT_FALSE;
    lookupCfg.pclIdL01     = 0;

    rc = cpssDxChPclCfgTblGet(device_id,
                              &interfaceInfo,
                              direction,
                              lookupNum,
                              &lookupCfg);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    if (DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ||
        DEVRM_PCL_LOOKUP_1   == pcl->lookup_num)
    {
        lookupCfg.pclId = pcl_id;
    }
    else if (DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num)
    {
        lookupCfg.dualLookup = GT_TRUE;
        lookupCfg.pclIdL01 = pcl_id;
    }

    lookupCfg.enableLookup = GT_TRUE;
    if (DEVRM_SetupKeyTypes(pcl->pcl_type, &lookupCfg) == FALSE)
    {
        DEBUG_ERROR_MSG(0);
        return FALSE;
    }

    /* lookup 1 for L3 router
     */
#if (DEVRM_RESERVED_RUEL_FOR_L3_ROUTING > 0)
    if (DEVRM_PCL_LOOKUP_1 == pcl->lookup_num)
    {
        lookupCfg.pclId = 0;

        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        lookupCfg.groupKeyTypes.ipv4Key  = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
        lookupCfg.groupKeyTypes.ipv6Key  = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E;
    }
#endif

    rc = cpssDxChPclCfgTblSet(device_id,
                              &interfaceInfo,
                              direction,
                              lookupNum,
                              &lookupCfg);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    if (DEVRM_IS_DEBUG_ERROR_ON())
    {
        rc = cpssDxChPclCfgTblGet(device_id,
                                  &interfaceInfo,
                                  direction,
                                  lookupNum,
                                  &lookupCfg);
        if (rc == GT_OK)
        {
            DEVRM_LOG("devNum[%02d], port[%02d]:enabled=%d, pclId=%d, dualLookup=%d, pclIdL01=%d, nonIpKey=%d, ipv4Key=%d, ipv6Key=%d",
                      interfaceInfo.devNum,
                      interfaceInfo.devPort.portNum,
                      lookupCfg.enableLookup, (int)lookupCfg.pclId,
                      lookupCfg.dualLookup, (int)lookupCfg.pclIdL01,
                      lookupCfg.groupKeyTypes.nonIpKey, lookupCfg.groupKeyTypes.ipv4Key,
                      lookupCfg.groupKeyTypes.ipv6Key);
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - DEVRM_UnbindPclToPort
 *------------------------------------------------------------------------------
 * PURPOSE  : Bind PCL to port
 * INPUT    : device_id - device ID
 *            phy_port  - phy port
 *            pcl_id    - PCL ID
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_UnbindPclToPort(
    UI32_T  device_id,
    UI32_T  phy_port,
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

    DEVRM_LOG("deviceId=%lu, port=%lu, pclId=%lu",
              device_id, phy_port, pcl_id);

    DEVRM_CHECK_DEVICE_ID(device_id);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    ASSERT(NULL != pcl);

    if (NULL == pcl)
    {
        ASSERT(0);
        return FALSE;
    }

    /* For alleycat rev 2, if bind egress pcl,
     * TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA will not work.
     */
    if (PRV_CPSS_PP_MAC(device_id)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E &&
        PRV_CPSS_PP_MAC(device_id)->revision <= 2)
    {
        if (pcl->direction == CPSS_PCL_DIRECTION_EGRESS_E)
        {
            DEVRM_LOG("deviceId=%lu, port=%lu, pclId=%lu, ignroe binding",
                      device_id, phy_port, pcl_id);
            return TRUE;
        }
    }

    DEVRM_LOG("pcl.direction=%s\r\n"
              "  pcl.lookup_num=%s",
              CPSS_PCL_DIRECTION_INGRESS_E == pcl->direction ? "in" : "out",
              DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ? "0-0" :
              DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num ? "0-1" :
              DEVRM_PCL_LOOKUP_1   == pcl->lookup_num ? "1" : "?");

    direction = pcl->direction;
    lookupNum = (DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ||
                 DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num) ? CPSS_PCL_LOOKUP_0_E :
    (DEVRM_PCL_LOOKUP_1   == pcl->lookup_num) ? CPSS_PCL_LOOKUP_1_E : 0xCCCCCCCC;

    if (STKTPLG_POM_GetMyUnitID(&my_unit_id) == FALSE)
    {
        DEVRM_LOG("<Error> STKTPLG_OM_GetMyUnitID failed");
        return FALSE;
    }

    if (DEV_SWDRV_GetModuleIdFromLocalDeviceId(my_unit_id, device_id, &hw_dev_id) == FALSE)
    {
        DEVRM_LOG("<Error> DEV_SWDRV_GetModuleIdFromLocalDeviceId failed\n");
        return FALSE;
    }

    interfaceInfo.type = CPSS_INTERFACE_PORT_E;
    interfaceInfo.devPort.devNum = hw_dev_id;
    interfaceInfo.devPort.portNum = phy_port;
    interfaceInfo.devNum = hw_dev_id;

    lookupCfg.enableLookup = GT_FALSE;
    lookupCfg.dualLookup   = GT_FALSE;
    lookupCfg.pclIdL01     = 0;

    rc = cpssDxChPclCfgTblGet(device_id,
                              &interfaceInfo,
                              direction,
                              lookupNum,
                              &lookupCfg);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    if (DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ||
        DEVRM_PCL_LOOKUP_1   == pcl->lookup_num)
    {
        lookupCfg.enableLookup = GT_FALSE;
        lookupCfg.pclId = pcl_id;
    }
    else if (DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num)
    {
        lookupCfg.dualLookup = GT_FALSE;
        lookupCfg.pclIdL01 = pcl_id;
    }

    if (DEVRM_SetupKeyTypes(pcl->pcl_type, &lookupCfg) == FALSE)
    {
        DEBUG_ERROR_MSG(0);
        return FALSE;
    }

    /* lookup 1 for L3 router
     */
#if (DEVRM_RESERVED_RUEL_FOR_L3_ROUTING > 0)
    if (DEVRM_PCL_LOOKUP_1 == pcl->lookup_num)
    {
        lookupCfg.pclId = 0;

        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        lookupCfg.groupKeyTypes.ipv4Key  = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
        lookupCfg.groupKeyTypes.ipv6Key  = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E;
    }
#endif

    rc = cpssDxChPclCfgTblSet(device_id,
                              &interfaceInfo,
                              direction,
                              lookupNum,
                              &lookupCfg);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    if (DEVRM_IS_DEBUG_ERROR_ON())
    {
        rc = cpssDxChPclCfgTblGet(device_id,
                                  &interfaceInfo,
                                  direction,
                                  lookupNum,
                                  &lookupCfg);
        if (rc == GT_OK)
        {
            ASSERT(GT_FALSE == lookupCfg.enableLookup);
            DEVRM_LOG("devNum[%02d], port[%02d]:enabled=%d, pclId=%d, dualLookup=%d, pclIdL01=%d, nonIpKey=%d, ipv4Key=%d, ipv6Key=%d",
                      interfaceInfo.devNum,
                      interfaceInfo.devPort.portNum,
                      lookupCfg.enableLookup, (int)lookupCfg.pclId,
                      lookupCfg.dualLookup, (int)lookupCfg.pclIdL01,
                      lookupCfg.groupKeyTypes.nonIpKey, lookupCfg.groupKeyTypes.ipv4Key,
                      lookupCfg.groupKeyTypes.ipv6Key);
        }
    }
    
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - DEVRM_BindPclToTrunkPort
 *------------------------------------------------------------------------------
 * PURPOSE  : Bind PCL to trunk port
 * INPUT    : device_id - device ID
 *            trunk_id  - trunk ID
 *            pcl_id    - PCL ID
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_BindPclToTrunkPort(
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id)
{
    DEVRM_PCL_T *pcl;

    GT_STATUS rc = GT_OK;

    CPSS_PCL_DIRECTION_ENT          direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum;

    CPSS_INTERFACE_INFO_STC         interfaceInfo;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfg;

    DEVRM_LOG("deviceId=%lu, trunkId=%lu, pclId=%lu",
              device_id, trunk_id, pcl_id);

    DEVRM_CHECK_DEVICE_ID(device_id);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    ASSERT(NULL != pcl);

    if (NULL == pcl)
    {
        DEVRM_LOG("<Error> Invalid pclId = %lu", pcl_id);
        return FALSE;
    }

    /* For alleycat rev 2, if bind egress pcl,
     * TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA will not work.
     */
    if (PRV_CPSS_PP_MAC(device_id)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E &&
        PRV_CPSS_PP_MAC(device_id)->revision <= 2)
    {
        if (pcl->direction == CPSS_PCL_DIRECTION_EGRESS_E)
        {
            DEVRM_LOG("deviceId=%lu, port=%lu, pclId=%lu, ignroe binding",
                device_id, trunk_id, pcl_id);
            printf("%s:%d: dev:%lu trunk:%lu pcl:%lu\n",__func__,__LINE__,device_id,trunk_id,pcl_id);
            return TRUE;
        }
    }

    DEVRM_LOG("pcl.direction=%s\r\n"
              "  pcl.lookup_num=%s",
              CPSS_PCL_DIRECTION_INGRESS_E == pcl->direction ? "in" : "out",
              DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ? "0-0" :
              DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num ? "0-1" :
              DEVRM_PCL_LOOKUP_1   == pcl->lookup_num ? "1" : "?");

    direction = pcl->direction;
    lookupNum = (DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ||
                 DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num) ? CPSS_PCL_LOOKUP_0_E :
                (DEVRM_PCL_LOOKUP_1   == pcl->lookup_num) ? CPSS_PCL_LOOKUP_1_E : 0xCCCCCCCC;

    interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
    interfaceInfo.devNum = device_id;
    interfaceInfo.trunkId = (GT_U16)DEV_SWDRV_CONVER_TRUNKID_A2C(interfaceInfo.devNum, trunk_id);

    lookupCfg.enableLookup = GT_TRUE;
    lookupCfg.dualLookup   = GT_FALSE;
    lookupCfg.pclIdL01     = 0;

    rc = cpssDxChPclCfgTblGet(device_id,
                              &interfaceInfo,
                              direction,
                              lookupNum,
                              &lookupCfg);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    if (DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ||
        DEVRM_PCL_LOOKUP_1   == pcl->lookup_num)
    {
        lookupCfg.pclId = pcl_id;
    }
    else if (DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num)
    {
        lookupCfg.dualLookup = GT_TRUE;
        lookupCfg.pclIdL01 = pcl_id;
    }

    lookupCfg.enableLookup = GT_TRUE;
    if (DEVRM_SetupKeyTypes(pcl->pcl_type, &lookupCfg) == FALSE)
    {
        DEBUG_ERROR_MSG(0);
        return FALSE;
    }

    rc = cpssDxChPclCfgTblSet(device_id,
                              &interfaceInfo,
                              direction,
                              lookupNum,
                              &lookupCfg);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    if (DEVRM_IS_DEBUG_ERROR_ON())
    {
        rc = cpssDxChPclCfgTblGet(device_id,
                                  &interfaceInfo,
                                  direction,
                                  lookupNum,
                                  &lookupCfg);
        if (rc == GT_OK)
        {
            DEVRM_LOG("devNum[%02d], trunkId[%02d]:enabled=%d, pclId=%d, dualLookup=%d, pclIdL01=%d, nonIpKey=%d, ipv4Key=%d, ipv6Key=%d",
                      interfaceInfo.devNum,
                      interfaceInfo.trunkId,
                      lookupCfg.enableLookup, (int)lookupCfg.pclId,
                      lookupCfg.dualLookup, (int)lookupCfg.pclIdL01,
                      lookupCfg.groupKeyTypes.nonIpKey, lookupCfg.groupKeyTypes.ipv4Key,
                      lookupCfg.groupKeyTypes.ipv6Key);
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - DEVRM_UnbindPclToTrunkPort
 *------------------------------------------------------------------------------
 * PURPOSE  : Unbind PCL to trunk port
 * INPUT    : device_id - device ID
 *            trunk_id  - trunk ID
 *            pcl_id    - PCL ID
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_UnbindPclToTrunkPort(
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id)
{
    DEVRM_PCL_T *pcl;

    GT_STATUS rc = GT_OK;

    CPSS_PCL_DIRECTION_ENT          direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum;

    CPSS_INTERFACE_INFO_STC         interfaceInfo;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfg;

    DEVRM_LOG("deviceId=%lu, trunkId=%lu, pclId=%lu",
              device_id, trunk_id, pcl_id);

    DEVRM_CHECK_DEVICE_ID(device_id);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    ASSERT(NULL != pcl);

    if (NULL == pcl)
    {
        DEVRM_LOG("<Error> Invalid pclId = %lu", pcl_id);
        return FALSE;
    }

    /* For alleycat rev 2, if bind egress pcl,
     * TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA will not work.
     */
    if (PRV_CPSS_PP_MAC(device_id)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E &&
        PRV_CPSS_PP_MAC(device_id)->revision <= 2)
    {
        if (pcl->direction == CPSS_PCL_DIRECTION_EGRESS_E)
        {
            DEVRM_LOG("deviceId=%lu, port=%lu, pclId=%lu, ignroe binding",
                      device_id, trunk_id, pcl_id);
            return TRUE;
        }
    }

    DEVRM_LOG("pcl.direction=%s\r\n"
              "  pcl.lookup_num=%s",
              CPSS_PCL_DIRECTION_INGRESS_E == pcl->direction ? "in" : "out",
              DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ? "0-0" :
              DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num ? "0-1" :
              DEVRM_PCL_LOOKUP_1   == pcl->lookup_num ? "1" : "?");

    direction = pcl->direction;
    lookupNum = (DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ||
                 DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num) ? CPSS_PCL_LOOKUP_0_E :
    (DEVRM_PCL_LOOKUP_1   == pcl->lookup_num) ? CPSS_PCL_LOOKUP_1_E : 0xCCCCCCCC;

    interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
    interfaceInfo.devNum = device_id;
    interfaceInfo.trunkId = (GT_U16)DEV_SWDRV_CONVER_TRUNKID_A2C(interfaceInfo.devNum, trunk_id);

    lookupCfg.enableLookup = GT_FALSE;
    lookupCfg.dualLookup   = GT_FALSE;
    lookupCfg.pclIdL01     = 0;

    rc = cpssDxChPclCfgTblGet(device_id,
                              &interfaceInfo,
                              direction,
                              lookupNum,
                              &lookupCfg);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    if (DEVRM_PCL_LOOKUP_0_0 == pcl->lookup_num ||
        DEVRM_PCL_LOOKUP_1   == pcl->lookup_num)
    {
        lookupCfg.pclId = pcl_id;
    }
    else if (DEVRM_PCL_LOOKUP_0_1 == pcl->lookup_num)
    {
        lookupCfg.dualLookup = GT_FALSE;
        lookupCfg.pclIdL01 = pcl_id;
    }

    lookupCfg.enableLookup = GT_FALSE;
    if (DEVRM_SetupKeyTypes(pcl->pcl_type, &lookupCfg) == FALSE)
    {
        DEBUG_ERROR_MSG(0);
        return FALSE;
    }

    rc = cpssDxChPclCfgTblSet(device_id,
                              &interfaceInfo,
                              direction,
                              lookupNum,
                              &lookupCfg);
    if (rc != GT_OK)
    {
        DEBUG_ERROR_MSG(rc);
        return FALSE;
    }

    if (DEVRM_IS_DEBUG_ERROR_ON())
    {
        rc = cpssDxChPclCfgTblGet(device_id,
                                  &interfaceInfo,
                                  direction,
                                  lookupNum,
                                  &lookupCfg);
        if (rc == GT_OK)
        {
            ASSERT(GT_FALSE == lookupCfg.enableLookup);
            DEVRM_LOG("devNum[%02d], trunkId[%02d]:enabled=%d, pclId=%d, dualLookup=%d, pclIdL01=%d, nonIpKey=%d, ipv4Key=%d, ipv6Key=%d",
                      interfaceInfo.devNum,
                      interfaceInfo.trunkId,
                      lookupCfg.enableLookup, (int)lookupCfg.pclId,
                      lookupCfg.dualLookup, (int)lookupCfg.pclIdL01,
                      lookupCfg.groupKeyTypes.nonIpKey, lookupCfg.groupKeyTypes.ipv4Key,
                      lookupCfg.groupKeyTypes.ipv6Key);
        }
    }
    
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  get counter
 * INPUT:    aclId, logical_rule_id
 * OUTPUT:   green_octets_p, yellow_octets_p, red_octets_p
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_GetCounter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    UI32_T *green_octets_p,
    UI32_T *yellow_octets_p,
    UI32_T *red_octets_p)
{
    GT_STATUS rc;

    DEVRM_PCL_T *pcl;
    DEVRM_PCE_T *pce;

    CPSS_DXCH_POLICER_STAGE_TYPE_ENT policer_stage;
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC billingCntr;

    DEVRM_CHECK_DEVICE_ID(device_id);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    if (NULL == pcl)
    {
        DEVRM_LOG("<Error> Invalid pclId = %lu", pcl_id);
        return FALSE;
    }

    if (DEVRM_GetPceFromPcl(device_id, pcl_id, dev_rule_id, &pce) == FALSE)
    {
        DEVRM_LOG("GetPceFromPcl Failed: pclId=%lu, devRuleId=%lu", pcl_id, dev_rule_id);

        if (DEVRM_GetPce(device_id, dev_rule_id, &pce) == FALSE)
        {
            DEVRM_LOG("GetPce Failed: pclId=%lu, devRuleId=%lu", pcl_id, dev_rule_id);
            return FALSE;
        }
    }

    policer_stage = DEVRM_POLICER_STAGE( pcl->direction );

    DEVRM_LOG("pclId=%lu, devRuleId=%lu, policerStage=%s, policerId=%hu",
              pcl_id, dev_rule_id,
              DEVRM_POLICER_STAGE_STR(policer_stage), pce->policer_idx);

    rc = GT_FAIL;
    if (-1 != pce->policer_idx)
    {
        rc = cpssDxCh3PolicerBillingEntryGet(device_id,
                                             policer_stage,
                                             pce->policer_idx,
                                             GT_FALSE,
                                             &billingCntr);
    }
    if (rc != GT_OK)
    {
        *green_octets_p = *yellow_octets_p = *red_octets_p = 0;

        DEVRM_LOG("PolicerBillingEntryGet failed: "
                  "pclId=%lu, devRuleId=%lu, policerStage=%s, policerId=%hu",
                  pcl_id, dev_rule_id,
                  DEVRM_POLICER_STAGE_STR(policer_stage), pce->policer_idx);
        return FALSE;
    }

    *green_octets_p = billingCntr.greenCntr.l[0];
    *yellow_octets_p = billingCntr.yellowCntr.l[0];
    *red_octets_p = billingCntr.redCntr.l[0];

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ClearCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  clear counter
 * INPUT:    aclId, logical_rule_id
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_ClearCounter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id)
{
    GT_STATUS rc = GT_FAIL;

    DEVRM_PCL_T *pcl;
    DEVRM_PCE_T *pce;

    CPSS_DXCH_POLICER_STAGE_TYPE_ENT policer_stage;
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC billingCntr;

    DEVRM_CHECK_DEVICE_ID(device_id);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    if (NULL == pcl)
    {
        ASSERT(0);
        return FALSE;
    }

    if (DEVRM_GetPceFromPcl(device_id, pcl_id, dev_rule_id, &pce) == FALSE)
    {
        DEVRM_LOG("GetPceFromPcl Failed: pclId=%lu, devRuleId=%lu", pcl_id, dev_rule_id);

        if (DEVRM_GetPce(device_id, dev_rule_id, &pce) == FALSE)
        {
            DEVRM_LOG("GetPce Failed: pclId=%lu, ruleId=%lu", pcl_id, dev_rule_id);
            return FALSE;
        }
    }

    policer_stage = DEVRM_POLICER_STAGE( pcl->direction );

    DEVRM_LOG("pclId=%lu, devRuleId=%lu, policerStage=%s, policerId=%hu",
              pcl_id, dev_rule_id,
              DEVRM_POLICER_STAGE_STR(policer_stage), pce->policer_idx);

    if (-1 != pce->policer_idx)
    {
        rc = cpssDxCh3PolicerBillingEntryGet(device_id,
                                             policer_stage,
                                             pce->policer_idx,
                                             GT_TRUE,
                                             &billingCntr);
    }

    if (rc != GT_OK)
    {
        DEVRM_LOG("PolicerBillingEntryGet failed: "
                  "pclId=%lu, devRuleId=%lu, policerStage=%s, policerId=%hu",
                  pcl_id, dev_rule_id,
                  DEVRM_POLICER_STAGE_STR(policer_stage), pce->policer_idx);
        return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocateQosRemarking
 *------------------------------------------------------------------------------
 * PURPOSE:  allocate qos remarking entry
 * INPUT:    NONE
 * OUTPUT:   qos_remarking_idx_p
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_AllocateQosRemarking(
    UI32_T device_id,
    UI32_T *qos_remarking_idx_p)
{
    int i;

    DEVRM_CHECK_DEVICE_ID(device_id);

    for (i = 0; i < DEVRM_QOS_REMARKING_ENTRY_NUM; i++)
    {
        if (!(DEVRM_DEVICE_PTR(device_id)->qos_remarking_use_status[i/8] & (0x80 >> (i%8))))
        {
            DEVRM_DEVICE_PTR(device_id)->qos_remarking_use_status[i/8] |= 0x80 >> (i%8);
            *qos_remarking_idx_p = DEVRM_QOS_REMARKING_ENTRY_FIRST_IDX + i;
            return TRUE;
        }
    }

    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FreeQosRemarking
 *------------------------------------------------------------------------------
 * PURPOSE:  free qos remarking entry
 * INPUT:    NONE
 * OUTPUT:   qos_remarking_idx_p
 * RETURN:   NONE
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
static void
DEVRM_FreeQosRemarking(
    UI32_T device_id,
    UI32_T qos_remarking_idx)
{
    int i;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (qos_remarking_idx < DEVRM_QOS_REMARKING_ENTRY_FIRST_IDX ||
        qos_remarking_idx > DEVRM_QOS_REMARKING_ENTRY_LAST_IDX)
    {
        return;
    }

    i = qos_remarking_idx - DEVRM_QOS_REMARKING_ENTRY_FIRST_IDX;
    DEVRM_DEVICE_PTR(device_id)->qos_remarking_use_status[i/8] &= ~(0x80 >> (i%8));
}

static void
DEVRM_Backdoor_DumpPclCfgTbl()
{
    int i;

    UI32_T device_id;
    UI32_T num_of_device = DEVRM_GetNumberOfChips();

    GT_STATUS rc = GT_OK;
    CPSS_INTERFACE_INFO_STC interfaceInfo;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC lookupCfg;

    printf("\r\n");

    for (device_id = 0; device_id < num_of_device; ++ device_id)
    {
        UI32_T hw_dev_id;
        UI32_T my_unit_id;

        if (STKTPLG_POM_GetMyUnitID(&my_unit_id) == FALSE)
        {
            DEVRM_LOG("<Error> STKTPLG_OM_GetMyUnitID failed");
            return;
        }

        if (DEV_SWDRV_GetModuleIdFromLocalDeviceId(my_unit_id, device_id, &hw_dev_id) == FALSE)
        {
            DEVRM_LOG("<Error> DEV_SWDRV_GetModuleIdFromLocalDeviceId failed\n");
            return;
        }

        printf("[device %lu]\r\n", hw_dev_id);

        printf("Ingress PCL\r\n");
        printf("<lookup 0>\r\n");
        for (i = 0; i < SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP; i++)
        {
            interfaceInfo.type = CPSS_INTERFACE_PORT_E;
            interfaceInfo.devPort.devNum = hw_dev_id;
            interfaceInfo.devPort.portNum = i;
            interfaceInfo.devNum = hw_dev_id;

            rc = cpssDxChPclCfgTblGet(device_id,
                             &interfaceInfo,
                             CPSS_PCL_DIRECTION_INGRESS_E,
                             CPSS_PCL_LOOKUP_0_E,
                             &lookupCfg);
            if (rc != GT_OK)
            {
                DEBUG_ERROR_MSG(rc);
                return ;
            }

            printf("port[%02d]:enabled=%d, pclId=%d, dualLookup=%d, pclIdL01=%d, nonIpKey=%d, ipv4Key=%d, ipv6Key=%d\r\n",
                   i, lookupCfg.enableLookup, (int)lookupCfg.pclId,
                   lookupCfg.dualLookup, (int)lookupCfg.pclIdL01,
                   lookupCfg.groupKeyTypes.nonIpKey, lookupCfg.groupKeyTypes.ipv4Key,
                   lookupCfg.groupKeyTypes.ipv6Key);

        }

        printf("\r\n<lookup 1>\r\n");
        for (i = 0; i < SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP ;i++)
        {
            interfaceInfo.type = CPSS_INTERFACE_PORT_E;
            interfaceInfo.devPort.devNum = hw_dev_id;
            interfaceInfo.devPort.portNum = i;
            interfaceInfo.devNum = hw_dev_id;

            rc = cpssDxChPclCfgTblGet(device_id,
                             &interfaceInfo,
                             CPSS_PCL_DIRECTION_INGRESS_E,
                             CPSS_PCL_LOOKUP_1_E,
                             &lookupCfg);
            if (rc != GT_OK)
            {
                DEBUG_ERROR_MSG(rc);
                return ;
            }

            printf("port[%02d]:enabled=%d, pclid=%d, nonIpKey=%d, ipv4Key=%d, ipv6Key=%d\r\n",
                   i, lookupCfg.enableLookup, (int)lookupCfg.pclId,
                   lookupCfg.groupKeyTypes.nonIpKey, lookupCfg.groupKeyTypes.ipv4Key,
                   lookupCfg.groupKeyTypes.ipv6Key);

        }

        printf("Egress PCL\r\n");
        for (i = 0; i < SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP; i++)
        {
            interfaceInfo.type = CPSS_INTERFACE_PORT_E;
            interfaceInfo.devPort.devNum = hw_dev_id;
            interfaceInfo.devPort.portNum = i;
            interfaceInfo.devNum = hw_dev_id;

            rc = cpssDxChPclCfgTblGet(device_id,
                             &interfaceInfo,
                             CPSS_PCL_DIRECTION_EGRESS_E,
                             CPSS_PCL_LOOKUP_0_E,
                             &lookupCfg);
            if (rc != GT_OK)
            {
                DEBUG_ERROR_MSG(rc);
                return ;
            }

            printf("port[%02d]:enabled=%d, pclId=%d, dualLookup=%d, pclIdL01=%d, nonIpKey=%d, ipv4Key=%d, ipv6Key=%d\r\n",
                   i, lookupCfg.enableLookup, (int)lookupCfg.pclId,
                   lookupCfg.dualLookup, (int)lookupCfg.pclIdL01,
                   lookupCfg.groupKeyTypes.nonIpKey, lookupCfg.groupKeyTypes.ipv4Key,
                   lookupCfg.groupKeyTypes.ipv6Key);
        }
    }
}

#ifdef _FAKE_CPSS
static void
DEVRM_DumpRule(
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    CPSS_DXCH_PCL_ACTION_STC           *actionPtr)
{
#define ZERO_MAC(mac)   (mac[0]==0 && mac[1]==0 && mac[2]==0 && \
                         mac[3]==0 && mac[4]==0 && mac[5]==0)
#define ZERO_IP(ip)     (ip[0]==0 && ip[1]==0 && ip[2]==0 && \
                         ip[3]==0)
#define ZERO_IP6(ip)    (ip[ 0]==0 && ip[ 1]==0 && ip[ 2]==0 && \
                         ip[ 3]==0 && ip[ 4]==0 && ip[ 5]==0 && \
                         ip[ 6]==0 && ip[ 7]==0 && ip[ 8]==0 && \
                         ip[ 9]==0 && ip[10]==0 && ip[11]==0 && \
                         ip[12]==0 && ip[13]==0 && ip[14]==0 && \
                         ip[15]==0)

    if (CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E == ruleFormat)
    {
        CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC *mask = &(maskPtr->ruleExtNotIpv6);
        CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC *pattern = &(patternPtr->ruleExtNotIpv6);

        printf("INGRESS_EXT_NOT_IPV6\r\n");

        if (mask->common.isL2Valid != 0)
        {
            printf("isL2Valid\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->common.isL2Valid,
                   mask->common.isL2Valid
                   );
        }

        if (mask->l2Encap != 0)
        {
            printf("ENCAP\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->l2Encap,
                   mask->l2Encap
                   );
        }

        if (mask->common.isTagged != 0)
        {
            printf("ISTAGGED\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->common.isTagged,
                   mask->common.isTagged
                   );
        }

        if (mask->common.vid != 0)
        {
            printf("VID\r\n"
                   "  Data 0x%04hX\r\n"
                   "  Mask 0x%04hx\r\n",
                   pattern->common.vid,
                   mask->common.vid
                   );
        }

        if (mask->common.up != 0)
        {
            printf("UP\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->common.up,
                   mask->common.up
                   );
        }

        if (!ZERO_MAC(mask->macDa.arEther))
        {
            printf("MACDA\r\n"
                   "  Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                   "  Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                   pattern->macDa.arEther[0],
                   pattern->macDa.arEther[1],
                   pattern->macDa.arEther[2],
                   pattern->macDa.arEther[3],
                   pattern->macDa.arEther[4],
                   pattern->macDa.arEther[5],
                   mask->macDa.arEther[0],
                   mask->macDa.arEther[1],
                   mask->macDa.arEther[2],
                   mask->macDa.arEther[3],
                   mask->macDa.arEther[4],
                   mask->macDa.arEther[5]
                   );
        }

        if (!ZERO_MAC(mask->macSa.arEther))
        {
            printf("MACSA\r\n"
                   "  Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                   "  Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                   pattern->macSa.arEther[0],
                   pattern->macSa.arEther[1],
                   pattern->macSa.arEther[2],
                   pattern->macSa.arEther[3],
                   pattern->macSa.arEther[4],
                   pattern->macSa.arEther[5],
                   mask->macSa.arEther[0],
                   mask->macSa.arEther[1],
                   mask->macSa.arEther[2],
                   mask->macSa.arEther[3],
                   mask->macSa.arEther[4],
                   mask->macSa.arEther[5]
                   );
        }

        if (mask->common.isIp != 0)
        {
            printf("isIp\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->common.isIp,
                   mask->common.isIp
                   );
        }

        if (!ZERO_IP(mask->sip.arIP))
        {
            printf("IPV4SIP\r\n"
                   "  Data 0x%02X%02X%02X%02X\r\n"
                   "  Mask 0x%02X%02X%02X%02X\r\n",
                   pattern->sip.arIP[0],
                   pattern->sip.arIP[1],
                   pattern->sip.arIP[2],
                   pattern->sip.arIP[3],
                   mask->sip.arIP[0],
                   mask->sip.arIP[1],
                   mask->sip.arIP[2],
                   mask->sip.arIP[3]
                   );
        }

        if (!ZERO_IP(mask->dip.arIP))
        {
            printf("IPV4DIP\r\n"
                   "  Data 0x%02X%02X%02X%02X\r\n"
                   "  Mask 0x%02X%02X%02X%02X\r\n",
                   pattern->dip.arIP[0],
                   pattern->dip.arIP[1],
                   pattern->dip.arIP[2],
                   pattern->dip.arIP[3],
                   mask->dip.arIP[0],
                   mask->dip.arIP[1],
                   mask->dip.arIP[2],
                   mask->dip.arIP[3]
                   );
        }

        if (mask->etherType != 0)
        {
            printf("ETHER\r\n"
                   "  Data 0x%04hX\r\n"
                   "  Mask 0x%04hX\r\n",
                   pattern->etherType,
                   mask->etherType
                   );
        }

        if (mask->commonExt.isL4Valid != 0)
        {
            printf("isL4Valid\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->commonExt.isL4Valid,
                   mask->commonExt.isL4Valid
                   );
        }

        if (mask->ipv4Fragmented != 0)
        {
            printf("IPFRAG\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->ipv4Fragmented,
                   mask->ipv4Fragmented
                   );
        }

        if (mask->commonExt.ipProtocol != 0)
        {
            printf("IPFRAG\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->commonExt.ipProtocol,
                   mask->commonExt.ipProtocol
                   );
        }

        if (mask->commonExt.dscp != 0)
        {
            printf("DSCP\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->commonExt.dscp,
                   mask->commonExt.dscp
                   );
        }

        if (mask->commonExt.l4Byte0 != 0 ||
            mask->commonExt.l4Byte1 != 0
            )
        {
            printf("L4SPORT\r\n"
                   "  Data 0x%02X%02X\r\n"
                   "  Mask 0x%02X%02X\r\n",
                   pattern->commonExt.l4Byte0,
                   pattern->commonExt.l4Byte1,
                   mask->commonExt.l4Byte0,
                   mask->commonExt.l4Byte1
                   );
        }

        if (mask->commonExt.l4Byte2 != 0 ||
            mask->commonExt.l4Byte3 != 0
            )
        {
            printf("L4DPORT\r\n"
                   "  Data 0x%02X%02X\r\n"
                   "  Mask 0x%02X%02X\r\n",
                   pattern->commonExt.l4Byte2,
                   pattern->commonExt.l4Byte3,
                   mask->commonExt.l4Byte2,
                   mask->commonExt.l4Byte3
                   );
        }

        if (mask->commonExt.l4Byte13 != 0)
        {
            printf("TCPCONTROL\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->commonExt.l4Byte13,
                   mask->commonExt.l4Byte13
                   );
        }
    }
    else if (CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E == ruleFormat)
    {
        CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC *mask = &(maskPtr->ruleEgrExtNotIpv6);
        CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC *pattern = &(patternPtr->ruleEgrExtNotIpv6);

        printf("EGRESS_EXT_NOT_IPV6\r\n");

        if (mask->common.sourcePort != 0)
        {
            printf("sourcePort\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->common.sourcePort,
                   mask->common.sourcePort
                   );
        }

        if (mask->common.isL2Valid != 0)
        {
            printf("isL2Valid\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->common.isL2Valid,
                   mask->common.isL2Valid
                   );
        }

        if (mask->l2Encap != 0)
        {
            printf("ENCAP\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->l2Encap,
                   mask->l2Encap
                   );
        }

        if (mask->common.isTagged != 0)
        {
            printf("ISTAGGED\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->common.isTagged,
                   mask->common.isTagged
                   );
        }

        if (mask->common.vid != 0)
        {
            printf("VID\r\n"
                   "  Data %04hX\r\n"
                   "  Mask %04hx\r\n",
                   pattern->common.vid,
                   mask->common.vid
                   );
        }

        if (mask->common.up != 0)
        {
            printf("UP\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->common.up,
                   mask->common.up
                   );
        }

        if (!ZERO_MAC(mask->macDa.arEther))
        {
            printf("MACDA\r\n"
                   "  Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                   "  Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                   pattern->macDa.arEther[0],
                   pattern->macDa.arEther[1],
                   pattern->macDa.arEther[2],
                   pattern->macDa.arEther[3],
                   pattern->macDa.arEther[4],
                   pattern->macDa.arEther[5],
                   mask->macDa.arEther[0],
                   mask->macDa.arEther[1],
                   mask->macDa.arEther[2],
                   mask->macDa.arEther[3],
                   mask->macDa.arEther[4],
                   mask->macDa.arEther[5]
                   );
        }

        if (!ZERO_MAC(mask->macSa.arEther))
        {
            printf("MACSA\r\n"
                   "  Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                   "  Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                   pattern->macSa.arEther[0],
                   pattern->macSa.arEther[1],
                   pattern->macSa.arEther[2],
                   pattern->macSa.arEther[3],
                   pattern->macSa.arEther[4],
                   pattern->macSa.arEther[5],
                   mask->macSa.arEther[0],
                   mask->macSa.arEther[1],
                   mask->macSa.arEther[2],
                   mask->macSa.arEther[3],
                   mask->macSa.arEther[4],
                   mask->macSa.arEther[5]
                   );
        }

        if (mask->common.isIp != 0)
        {
            printf("isIp\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->common.isIp,
                   mask->common.isIp
                   );
        }

        if (!ZERO_IP(mask->sip.arIP))
        {
            printf("IPV4SIP\r\n"
                   "  Data 0x%02X%02X%02X%02X\r\n"
                   "  Mask 0x%02X%02X%02X%02X\r\n",
                   pattern->sip.arIP[0],
                   pattern->sip.arIP[1],
                   pattern->sip.arIP[2],
                   pattern->sip.arIP[3],
                   mask->sip.arIP[0],
                   mask->sip.arIP[1],
                   mask->sip.arIP[2],
                   mask->sip.arIP[3]
                   );
        }

        if (!ZERO_IP(mask->dip.arIP))
        {
            printf("IPV4DIP\r\n"
                   "  Data 0x%02X%02X%02X%02X\r\n"
                   "  Mask 0x%02X%02X%02X%02X\r\n",
                   pattern->dip.arIP[0],
                   pattern->dip.arIP[1],
                   pattern->dip.arIP[2],
                   pattern->dip.arIP[3],
                   mask->dip.arIP[0],
                   mask->dip.arIP[1],
                   mask->dip.arIP[2],
                   mask->dip.arIP[3]
                   );
        }

        if (mask->etherType != 0)
        {
            printf("ETHER\r\n"
                   "  Data 0x%04hX\r\n"
                   "  Mask 0x%04hX\r\n",
                   pattern->etherType,
                   mask->etherType
                   );
        }

        if (mask->commonExt.isL4Valid != 0)
        {
            printf("isL4Valid\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->commonExt.isL4Valid,
                   mask->commonExt.isL4Valid
                   );
        }

        if (mask->ipv4Fragmented != 0)
        {
            printf("IPFRAG\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->ipv4Fragmented,
                   mask->ipv4Fragmented
                   );
        }

        if (mask->commonExt.ipProtocol != 0)
        {
            printf("IPFRAG\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->commonExt.ipProtocol,
                   mask->commonExt.ipProtocol
                   );
        }

        if (mask->commonExt.dscp != 0)
        {
            printf("DSCP\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->commonExt.dscp,
                   mask->commonExt.dscp
                   );
        }

        if (mask->commonExt.l4Byte0 != 0 ||
            mask->commonExt.l4Byte1 != 0
            )
        {
            printf("L4SPORT\r\n"
                   "  Data 0x%02X%02X\r\n"
                   "  Mask 0x%02X%02X\r\n",
                   pattern->commonExt.l4Byte0,
                   pattern->commonExt.l4Byte1,
                   mask->commonExt.l4Byte0,
                   mask->commonExt.l4Byte1
                   );
        }

        if (mask->commonExt.l4Byte2 != 0 ||
            mask->commonExt.l4Byte3 != 0
            )
        {
            printf("L4DPORT\r\n"
                   "  Data 0x%02X%02X\r\n"
                   "  Mask 0x%02X%02X\r\n",
                   pattern->commonExt.l4Byte2,
                   pattern->commonExt.l4Byte3,
                   mask->commonExt.l4Byte2,
                   mask->commonExt.l4Byte3
                   );
        }

        if (mask->commonExt.l4Byte13 != 0)
        {
            printf("TCPCONTROL\r\n"
                   "  Data 0x%02X\r\n"
                   "  Mask 0x%02X\r\n",
                   pattern->commonExt.l4Byte13,
                   mask->commonExt.l4Byte13
                   );
        }
    }

    printf("ACTION\r\n");
    if (actionPtr->pktCmd != CPSS_PACKET_CMD_NONE_E)
    {
        printf("  pktCmd %s\r\n",
            (actionPtr->pktCmd == CPSS_PACKET_CMD_FORWARD_E)           ? "Forward" :
            (actionPtr->pktCmd == CPSS_PACKET_CMD_MIRROR_TO_CPU_E)     ? "Mirror-to-CPU" :
            (actionPtr->pktCmd == CPSS_PACKET_CMD_TRAP_TO_CPU_E)       ? "Trap-to-CPU" :
            (actionPtr->pktCmd == CPSS_PACKET_CMD_DROP_HARD_E)         ? "Drop-Hard" :
            (actionPtr->pktCmd == CPSS_PACKET_CMD_DROP_SOFT_E)         ? "Drop-Soft" :
            (actionPtr->pktCmd == CPSS_PACKET_CMD_ROUTE_E)             ? "Route" :
            (actionPtr->pktCmd == CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E)  ? "Route-and-Mirror" :
            (actionPtr->pktCmd == CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E) ? "Bridge-and-Mirror" :
                                                                         "??");
    }

    if (actionPtr->qos.modifyUp == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
    {
        printf("  modifyUp\r\n");
    }

    if (actionPtr->qos.modifyDscp)
    {
        printf("  modifyDscp\r\n");
    }

    if (actionPtr->vlan.modifyVlan != CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
    {
        printf("  modifyVlan for %s to vlanId %hu\r\n",
            (actionPtr->vlan.modifyVlan == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E)   ? "Tagged" :
            (actionPtr->vlan.modifyVlan == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E) ? "Untagged" :
            (actionPtr->vlan.modifyVlan == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E)      ? "All" :
                                                                                          "??",
            actionPtr->vlan.vlanId);
    }
}
#endif /* _FAKE_CPSS */

static void
DEVRM_Backdoor_DumpRuleState()
{
    UI32_T i = 0;
    DEVRM_PCE_T *tmp;
    GT_STATUS rc;
    GT_BOOL valid_status;
    CPSS_PCL_RULE_SIZE_ENT rule_size, tmp_size;
    UI32_T pattern[12];
    UI32_T mask[12];
    UI32_T action[4];

    UI32_T device_id;
    UI32_T num_of_device = DEVRM_GetNumberOfChips();

    printf("\r\n");

    for (device_id = 0; device_id < num_of_device; ++ device_id)
    {
        UI32_T hw_dev_id;
        UI32_T my_unit_id;

        if(FALSE == STKTPLG_POM_GetMyUnitID(&my_unit_id))
        {
            printf("%s[%d]: STKTPLG_OM_GetMyUnitID failed\n", __FUNCTION__, __LINE__);
            return;
        }

        if(FALSE == DEV_SWDRV_GetModuleIdFromLocalDeviceId(my_unit_id,
                                                           (UI32_T) device_id,
                                                           &hw_dev_id))
        {
            printf("%s[%d]: DEV_SWDRV_GetModuleIdFromLocalDeviceId failed\n", __FUNCTION__, __LINE__);
            return;
        }
        printf("[device %lu]\r\n", hw_dev_id);

        for (i=0; i<sizeof(DEVRM_DEVICE_PTR(device_id)->pcls)/sizeof(*DEVRM_DEVICE_PTR(device_id)->pcls);i++)
        {
            if (!DEVRM_DEVICE_PTR(device_id)->pcls[i].is_used)
                continue;

            tmp = DEVRM_DEVICE_PTR(device_id)->pcls[i].pce_list;
            while (tmp != NULL)
            {
                memset(pattern, 0, sizeof(pattern));
                memset(mask, 0, sizeof(mask));
                memset(action, 0, sizeof(action));
                valid_status = FALSE;
                rule_size = (tmp->is_std_pce)?CPSS_PCL_RULE_SIZE_STD_E: CPSS_PCL_RULE_SIZE_EXT_E;

#ifdef _FAKE_CPSS
                rc = cpssDxChPclRuleAnyStateGet(device_id,
                                                rule_size,
                                                DEVRM_Translate_PCEIDwithType(tmp->pce_type, tmp->hw_idx),
                                                &valid_status,
                                                &tmp_size
                                                );
                if (rc != GT_OK)
                {
                    valid_status = GT_FALSE;
                }

                printf("[%03lu]pcl_id=%d, hw_idx=%d, upper_rule_id=%ld, policer_idx=%d, pce_type=%d(%s), valid=%d\r\n",
                        i, tmp->pcl_id-1, tmp->hw_idx, tmp->upper_rule_id, tmp->policer_idx, tmp->pce_type,
                        (tmp->is_std_pce)?"STD":"EXT", valid_status);

                if (GT_TRUE == valid_status)
                {
                    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
                    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask;
                    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern;
                    CPSS_DXCH_PCL_ACTION_STC           action;

                    rc = _cpssDxChPclRuleGet(device_id,
                                             rule_size,
                                             DEVRM_Translate_PCEIDwithType(tmp->pce_type, tmp->hw_idx),
                                             &ruleFormat,
                                             &mask,
                                             &pattern,
                                             &action
                                             );
                    DEVRM_DumpRule(ruleFormat, &mask, &pattern, &action);
                }

#else
                rc = cpssDxChPclRuleGet(device_id,
                                        rule_size,
                                        DEVRM_Translate_PCEIDwithType(tmp->pce_type, tmp->hw_idx),
                                        mask,
                                        pattern,
                                        action);
                if (rc != GT_OK)
                {
                    goto out;
                }
#if 1
                rc = cpssDxChPclRuleAnyStateGet(device_id,
                                                rule_size,
                                                DEVRM_Translate_PCEIDwithType(tmp->pce_type, tmp->hw_idx),
                                                &valid_status,
                                                &tmp_size);
                if (rc != GT_OK)
                {
                    goto out;
                }
#endif

out:
                printf("[%03lu]pcl_id=%d, hw_idx=%d, upper_rule_id=%ld, policer_idx=%d, pce_type=%d(%s), valid=%d\r\n",
                        i, tmp->pcl_id-1, tmp->hw_idx, tmp->upper_rule_id, tmp->policer_idx, tmp->pce_type,
                        (tmp->is_std_pce)?"STD":"EXT", valid_status);
                if (rc == GT_OK)
                {
                    printf("  pattern:");
                    for (j=0;j < 12; j++)
                        printf("%08X ", (unsigned int)pattern[j]);
                    printf("\r\n     mask:");
                    for (j=0; j < 12; j++)
                        printf("%08X ", (unsigned int)mask[j]);
                    printf("\r\n   action:");
                    for (j=0; j < 4; j++)
                        printf("%08X ", (unsigned int)action[j]);

                    switch((action[0] & 0x7))
                    {
                        case 0:
                            printf(" Forward");
                            break;
                        case 1:
                            printf(" Mirror");
                            break;
                        case 2:
                            printf(" Trap");
                            break;
                        case 3:
                            printf(" Hard-Drop");
                            break;
                        case 4:
                            printf(" Soft-Drop");
                            break;
                        default:
                            break;

                    }

                    printf("\r\n");
                }
#endif /* _FAKE_CPSS */

                if (tmp->is_std_pce)
                    tmp = tmp->next;
                else
                    tmp = tmp->next->next;

            }
        }
    } /* for (device_id */
}

static BOOL_T
DEVRM_Backdoor_HaveRefMeter(
    UI32_T device_id,
    GT_U32 entryIndex,
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT stage)
{
    UI32_T i;
    DEVRM_PCE_T *pce;
    CPSS_PCL_DIRECTION_ENT pcl_direction =
        stage == CPSS_DXCH_POLICER_STAGE_INGRESS_0_E ? CPSS_PCL_DIRECTION_INGRESS_E :
        stage == CPSS_DXCH_POLICER_STAGE_INGRESS_1_E ? CPSS_PCL_DIRECTION_INGRESS_E :
                                                       CPSS_PCL_DIRECTION_EGRESS_E;

    DEVRM_CHECK_DEVICE_ID(device_id);

    for (i = 0; i<sizeof(DEVRM_DEVICE_PTR(device_id)->pcls)/sizeof(*DEVRM_DEVICE_PTR(device_id)->pcls);i++)
    {
        if ((!DEVRM_DEVICE_PTR(device_id)->pcls[i].is_used) ||
            (DEVRM_DEVICE_PTR(device_id)->pcls[i].direction != pcl_direction)
            )
        {
            continue;
        }

        pce = DEVRM_DEVICE_PTR(device_id)->pcls[i].pce_list;
        while (pce != NULL)
        {
            if (-1 != pce->policer_idx && (GT_U32)pce->policer_idx == entryIndex)
            {
                return TRUE;
            }

            if (pce->is_std_pce)
                pce = pce->next;
            else
                pce = pce->next->next;
         }
     }

     return FALSE;
}

static void
DEVRM_Backdoor_DumpMeterTable(
    BOOL_T filter_by_ref_pce)
{
#define POLICER_STAGE_STR(stage)           \
        CPSS_DXCH_POLICER_STAGE_INGRESS_0_E == stage ? "Ingress#0": \
        CPSS_DXCH_POLICER_STAGE_INGRESS_1_E == stage ? "Ingress#1": \
        CPSS_DXCH_POLICER_STAGE_EGRESS_E    == stage ? "Egress" :   \
                                                       "Unknonw"

#define POLICER_NO_CONFORM_CMD_STR(cmd)    \
        CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E == cmd ?       "No-Change" :       \
        CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E == cmd ?            "Drop" :            \
        CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E == cmd ?          "Remark" :          \
        CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E == cmd ? "Remark-by-Entry" : \
                                                                      "Unknown"

    GT_STATUS rc = GT_OK;

    GT_U32 entryIndex=0;

    CPSS_DXCH_POLICER_STAGE_TYPE_ENT      policer_stage;
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC policer_entry;

    UI32_T device_id;
    UI32_T num_of_device = DEVRM_GetNumberOfChips();

    printf("\r\n");

    for (device_id = 0; device_id < num_of_device; ++ device_id)
    {
        printf("[device %lu]\r\n", device_id);

        for (policer_stage = CPSS_DXCH_POLICER_STAGE_INGRESS_0_E;
             policer_stage<= CPSS_DXCH_POLICER_STAGE_EGRESS_E;
             policer_stage++)
        {
            if (policer_stage != DEVRM_METER_POLICER_STAGE(device_id) &&
                policer_stage != CPSS_DXCH_POLICER_STAGE_EGRESS_E)
            {
                continue;
            }

            printf("%s\r\n", POLICER_STAGE_STR(policer_stage));

            for (entryIndex = 0; entryIndex < DEVRM_METER_MAX_NUM_BY_STAGE(device_id, policer_stage); entryIndex++)
            {
                if (TRUE == filter_by_ref_pce)
                {
                    if (TRUE != DEVRM_Backdoor_HaveRefMeter(device_id, entryIndex, policer_stage))
                    {
                        continue;
                    }
                }

                memset(&policer_entry, 0, sizeof(CPSS_DXCH3_POLICER_METERING_ENTRY_STC));
                rc = cpssDxCh3PolicerMeteringEntryGet(device_id, policer_stage, entryIndex, &policer_entry);
                if (rc == GT_OK)
                {
                    printf("policer[%d]: %s-%s, ",
                        (int)entryIndex,
                        (policer_entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E) ? "SRTCM":"TRTCM",
                        (policer_entry.meterColorMode==CPSS_POLICER_COLOR_BLIND_E) ? "COLOR-BLIND":"COLOR-AWARE");

                    if (policer_entry.meterMode == CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E)
                    {
                        printf("cir=%d, cbs=%d, ebs=%d, ",
                            (int)policer_entry.tokenBucketParams.srTcmParams.cir,
                            (int)policer_entry.tokenBucketParams.srTcmParams.cbs,
                            (int)policer_entry.tokenBucketParams.srTcmParams.ebs);
                    }
                    else if (policer_entry.meterMode == CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E)
                    {
                        printf("cir=%d, cbs=%d, pir=%d, pbs=%d, ",
                            (int)policer_entry.tokenBucketParams.trTcmParams.cir,
                            (int)policer_entry.tokenBucketParams.trTcmParams.cbs,
                            (int)policer_entry.tokenBucketParams.trTcmParams.pir,
                            (int)policer_entry.tokenBucketParams.trTcmParams.pbs);
                    }

                    printf("yellowCmd=%s, redCmd=%s, ",
                        POLICER_NO_CONFORM_CMD_STR(policer_entry.yellowPcktCmd),
                        POLICER_NO_CONFORM_CMD_STR(policer_entry.redPcktCmd));

                    if (policer_entry.modifyUp == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                    {
                        printf("modifyUp, ");
                    }

                    if (policer_entry.modifyDscp == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                    {
                        printf("modifyDscp, ");
                    }

                    if (policer_entry.modifyDp == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                    {
                        printf("modifyDp, ");
                    }

                    printf("remarkMode=%s",
                           policer_entry.remarkMode == CPSS_DXCH_POLICER_REMARK_MODE_L2_E ? "L2" : "L3");

                    if (policer_entry.yellowPcktCmd == CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E ||
                        policer_entry.redPcktCmd == CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E)
                    {
                        CPSS_DXCH_COS_PROFILE_STC cosProfile;

                        printf(", qosProfile=%d", (int)policer_entry.qosProfile);

                        if (GT_OK == cpssDxChCosProfileEntryGet(device_id, policer_entry.qosProfile, &cosProfile))
                        {
                            printf(" {DP=%s, UP=%lu, TC=%lu, DSCP=%lu}",
                                   cosProfile.dropPrecedence==CPSS_DP_GREEN_E ? "Green" :
                                   cosProfile.dropPrecedence==CPSS_DP_YELLOW_E ? "Yellow" :
                                   cosProfile.dropPrecedence==CPSS_DP_RED_E ? "Red" : "??",
                                   cosProfile.userPriority,
                                   cosProfile.trafficClass,
                                   cosProfile.dscp);
                        }

                    }

                    printf("\r\n");

                    {
                        UI32_T i;
                        DEVRM_PCE_T *pce;
                        UI32_T flag = 0;
                        CPSS_PCL_DIRECTION_ENT direction =
                            policer_stage == CPSS_DXCH_POLICER_STAGE_INGRESS_0_E ? CPSS_PCL_DIRECTION_INGRESS_E :
                            policer_stage == CPSS_DXCH_POLICER_STAGE_INGRESS_1_E ? CPSS_PCL_DIRECTION_INGRESS_E :
                                                                                   CPSS_PCL_DIRECTION_EGRESS_E;

                        for (i = 0; i < sizeof(DEVRM_DEVICE_PTR(device_id)->pcls)/sizeof(*DEVRM_DEVICE_PTR(device_id)->pcls);i++)
                        {
                            if ((!DEVRM_DEVICE_PTR(device_id)->pcls[i].is_used) ||
                                (DEVRM_DEVICE_PTR(device_id)->pcls[i].direction != direction)
                                )
                            {
                                continue;
                            }

                            pce = DEVRM_DEVICE_PTR(device_id)->pcls[i].pce_list;
                            while (pce != NULL)
                            {
                                if (-1 != pce->policer_idx && (GT_U32)pce->policer_idx == entryIndex)
                                {
                                    printf("%s%hu", (flag++==0)?"ref pce = { ":" ,", pce->hw_idx);
                                }

                                if (pce->is_std_pce)
                                    pce = pce->next;
                                else
                                    pce = pce->next->next;
                            }

                            printf("%s", (flag)?" }\r\n": "");
                        }
                    }
                }
            }
        }
    } /* for (device_id */

#undef POLICER_STAGE_STR
#undef POLICER_NO_CONFORM_CMD_STR

}

static void
DEVRM_Backdoor_DumpDevRmDB()
{
    int i;
    int max = (DEVRM_MAX_NBR_OF_TCAM_ENTRIES+7) / 8;

    UI32_T device_id;
    UI32_T num_of_device = DEVRM_GetNumberOfChips();

    printf("\r\n");

    for (device_id = 0; device_id < num_of_device; ++ device_id)
    {
        printf("[device %lu]\r\n", device_id);

        printf("\r\n ========================== \r\n");
        printf("     Dump DB - pces_use_status  ");
        printf("\r\n ========================== \r\n");

        for (i=0; i<max; i++)
        {
            if (i%4 == 0)
                printf("\r\n %2d  ", i);

            printf("%02x", DEVRM_DEVICE_PTR(device_id)->pces_use_status[i]);
        }
    }

}

static void
DEVRM_Backdoor_EnableDebugMode()
{
    devrm_debug_flag = (0 == devrm_debug_flag) ? 1 : 0;

    DEBUG_PRINT_STR("\r\nDebug mode is %s", DEVRM_IS_DEBUG_ERROR_ON() ? "Enabled" : "Disabled");
}

static void
DEVRM_BackdoorMain(void)
{
    UI8_T ch[5] = {0};
    BOOL_T eof = FALSE;

    while (! eof)
    {
        printf("\r\n");
        printf("1 : Dump Pcl config table.\r\n");
        printf("2 : Dump Rules status.\r\n");
        printf("3 : Dump meter table.\r\n");
        printf("4 : Dump meter table which be ref only.\r\n");
        printf("o : Dump Dev_Rm OM settings\r\n");
        printf("d : Enable debug mode\r\n");
        printf("x : exit\r\n");
        printf("=================================================\r\n");
        printf("\tselect:");
        BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
        switch (ch[0])
        {
            case '1':
                DEVRM_Backdoor_DumpPclCfgTbl();
                break;
            case '2':
                DEVRM_Backdoor_DumpRuleState();
                break;
            case '3':
                DEVRM_Backdoor_DumpMeterTable(FALSE);
                break;
            case '4':
                DEVRM_Backdoor_DumpMeterTable(TRUE);
                break;
            case 'o':
                DEVRM_Backdoor_DumpDevRmDB();
                break;
            case 'd': case 'D':
                DEVRM_Backdoor_EnableDebugMode();
                break;
            case 'x':
            default :
                eof = TRUE;
                break;
        }
    }
}

static BOOL_T
DEVRM_RuleToIngStdNotIp(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr)
{
    CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC *mask = &maskPtr->ruleStdNotIp;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC *pattern = &patternPtr->ruleStdNotIp;

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACDA)
    {
        DEVRM_LOG("Qualify MAC DA\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->dstmac_data[0],
                  ace_entry->dstmac_data[1],
                  ace_entry->dstmac_data[2],
                  ace_entry->dstmac_data[3],
                  ace_entry->dstmac_data[4],
                  ace_entry->dstmac_data[5],

                  ace_entry->dstmac_mask[0],
                  ace_entry->dstmac_mask[1],
                  ace_entry->dstmac_mask[2],
                  ace_entry->dstmac_mask[3],
                  ace_entry->dstmac_mask[4],
                  ace_entry->dstmac_mask[5]);

        memcpy(&mask->macDa, ace_entry->dstmac_mask, sizeof(GT_ETHERADDR));
        memcpy(&pattern->macDa, ace_entry->dstmac_data, sizeof(GT_ETHERADDR));
        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACSA)
    {
        DEVRM_LOG("Qualify MAC SA\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->srcmac_data[0],
                  ace_entry->srcmac_data[1],
                  ace_entry->srcmac_data[2],
                  ace_entry->srcmac_data[3],
                  ace_entry->srcmac_data[4],
                  ace_entry->srcmac_data[5],

                  ace_entry->srcmac_mask[0],
                  ace_entry->srcmac_mask[1],
                  ace_entry->srcmac_mask[2],
                  ace_entry->srcmac_mask[3],
                  ace_entry->srcmac_mask[4],
                  ace_entry->srcmac_mask[5]);

        memcpy(&mask->macSa, ace_entry->srcmac_mask, sizeof(GT_ETHERADDR));
        memcpy(&pattern->macSa, ace_entry->srcmac_data, sizeof(GT_ETHERADDR));
        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ETHER)
    {
        DEVRM_LOG("Qualify Ether Type\r\n"
                  "    Data 0x%04hX\r\n"
                  "    Mask 0x%04hX\r\n",
                  ace_entry->ethertype_data,
                  ace_entry->ethertype_mask);

        mask->etherType = L_STDLIB_Hton16(ace_entry->ethertype_mask);
        pattern->etherType = L_STDLIB_Hton16(ace_entry->ethertype_data);

        switch (ace_entry->ethertype_mask & ace_entry->ethertype_data)
        {
            case 0x0800:
                pattern->isIpv4 = 1;
                pattern->common.isIp = 1;
                mask->isIpv4 = 0xFF;
                mask->common.isIp = 0xFF;
                break;
            case 0x0806:
                pattern->isArp = 1;
                mask->isArp = 0xFF;
                break;
            default:
                break;
        }
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ISARP)
    {
        DEVRM_LOG("Qualify Is ARP\r\n"
                  "  Ether Type\r\n"
                  "    Data 0x%04hX\r\n"
                  "    Mask 0x%04hX\r\n",
                  ace_entry->ethertype_data,
                  ace_entry->ethertype_mask);

        if ((ace_entry->ethertype_mask & ace_entry->ethertype_data) == 0x0806)
        {
            pattern->isArp = 1;
            mask->isArp = 0xFF;
        }

        DEVRM_LOG("@isArp\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->isArp,
                  mask->isArp);

    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ENCAP)
    {
        DEVRM_LOG("Qualify ENCAP\r\n"
                  "  Packet Format\r\n"
                  "    Data 0x%08lX(%s)\r\n",
                  ace_entry->packetformat_data,
                  ETHERTYPE_ENCAPS == ace_entry->packetformat_data ? "ENCAPS" : "");

        pattern->l2Encap = (ace_entry->packetformat_data == ETHERTYPE_ENCAPS) ? 1 : 0;
        mask->l2Encap = 0xFF;

        DEVRM_LOG("@l2Encap\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->l2Encap,
                  mask->l2Encap);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VID)
    {
        DEVRM_LOG("Qualify VLAN ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_mask,
                  ace_entry->outvlan_mask);

        pattern->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_data & 0x0FFF);
        mask->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_mask & 0x0FFF);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_UP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP)
    {
        DEVRM_LOG("Qualify UP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_mask,
                  ace_entry->vtagup_mask);

        pattern->common.up = ace_entry->vtagup_data;
        mask->common.up = ace_entry->vtagup_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TAGGED)
    {
        DEVRM_LOG("Qualify Tagged\r\n"
                  "    Data 0x%02X(%u)\r\n",
                  ace_entry->is_tagged,
                  ace_entry->is_tagged);

        pattern->common.isTagged = ace_entry->is_tagged;
        mask->common.isTagged = 0xFF;

        DEVRM_LOG("@isTagged\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->common.isTagged,
                  mask->common.isTagged);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PCL_ID)
    {
        DEVRM_LOG("Qualify PCL ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->pcl_id,
                  ace_entry->pcl_id,
                  ace_entry->pcl_id_mask,
                  ace_entry->pcl_id_mask);

        pattern->common.pclId = ace_entry->pcl_id;
        mask->common.pclId = ace_entry->pcl_id_mask;
    }

    return TRUE;
}

static BOOL_T
DEVRM_RuleToIngStdIpL2Qos(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr)
{
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC *mask = &maskPtr->ruleStdIpL2Qos;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC *pattern = &patternPtr->ruleStdIpL2Qos;

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACDA)
    {
        DEVRM_LOG("Qualify MAC DA\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->dstmac_data[0],
                  ace_entry->dstmac_data[1],
                  ace_entry->dstmac_data[2],
                  ace_entry->dstmac_data[3],
                  ace_entry->dstmac_data[4],
                  ace_entry->dstmac_data[5],

                  ace_entry->dstmac_mask[0],
                  ace_entry->dstmac_mask[1],
                  ace_entry->dstmac_mask[2],
                  ace_entry->dstmac_mask[3],
                  ace_entry->dstmac_mask[4],
                  ace_entry->dstmac_mask[5]);

        memcpy(&mask->macDa, ace_entry->dstmac_mask, sizeof(GT_ETHERADDR));
        memcpy(&pattern->macDa, ace_entry->dstmac_data, sizeof(GT_ETHERADDR));
        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACSA)
    {
        DEVRM_LOG("Qualify MAC SA\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->srcmac_data[0],
                  ace_entry->srcmac_data[1],
                  ace_entry->srcmac_data[2],
                  ace_entry->srcmac_data[3],
                  ace_entry->srcmac_data[4],
                  ace_entry->srcmac_data[5],

                  ace_entry->srcmac_mask[0],
                  ace_entry->srcmac_mask[1],
                  ace_entry->srcmac_mask[2],
                  ace_entry->srcmac_mask[3],
                  ace_entry->srcmac_mask[4],
                  ace_entry->srcmac_mask[5]);

        memcpy(&mask->macSa, ace_entry->srcmac_mask, sizeof(GT_ETHERADDR));
        memcpy(&pattern->macSa, ace_entry->srcmac_data, sizeof(GT_ETHERADDR));
        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ISARP)
    {
        DEVRM_LOG("Qualify Is ARP\r\n");

        pattern->isArp = 1;
        mask->isArp = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VID)
    {
        DEVRM_LOG("Qualify VLAN ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_mask,
                  ace_entry->outvlan_mask);

        pattern->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_data & 0x0FFF);
        mask->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_mask & 0x0FFF);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_UP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP)
    {
        DEVRM_LOG("Qualify UP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_mask,
                  ace_entry->vtagup_mask);

        pattern->common.up = ace_entry->vtagup_data;
        mask->common.up = ace_entry->vtagup_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TAGGED)
    {
        DEVRM_LOG("Qualify Tagged\r\n"
                  "    Data 0x%02X(%u)\r\n",
                  ace_entry->is_tagged,
                  ace_entry->is_tagged);

        pattern->common.isTagged = ace_entry->is_tagged;
        mask->common.isTagged = 0xFF;

        DEVRM_LOG("@isTagged\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->common.isTagged,
                  mask->common.isTagged);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_DSCP)
    {
        DEVRM_LOG("Qualify DSCP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->dscp_data,
                  ace_entry->dscp_data,
                  ace_entry->dscp_mask,
                  ace_entry->dscp_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonStdIp.dscp = ace_entry->dscp_data;
        mask->commonStdIp.dscp = ace_entry->dscp_mask;
    }
    else
    {
        if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PREC)
        {
            DEVRM_LOG("Qualify Precedence\r\n"
                      "    Data 0x%02X(%u)\r\n"
                      "    Mask 0x%02X(%u)\r\n",
                      ace_entry->prec_data,
                      ace_entry->prec_data,
                      ace_entry->prec_mask,
                      ace_entry->prec_mask);

            pattern->common.isIp = 1;
            mask->common.isIp = 0xFF;
            pattern->commonStdIp.dscp = (ace_entry->prec_data & 0x7) << 3;
            mask->commonStdIp.dscp = (ace_entry->prec_mask & 0x7) << 3;

            DEVRM_LOG("@dscp\r\n"
                      "    Data 0x%02X\r\n"
                      "    Mask 0x%02X\r\n",
                      pattern->commonStdIp.dscp,
                      mask->commonStdIp.dscp);
        }
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL)
    {
        DEVRM_LOG("Qualify IP Protocol\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_mask,
                  ace_entry->ipprotocol_mask);

        pattern->commonStdIp.ipProtocol = ace_entry->ipprotocol_data;
        mask->commonStdIp.ipProtocol = ace_entry->ipprotocol_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4SRCPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_srcport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_srcport_mask);

        DEVRM_LOG("Qualify L4 Source Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_mask,
                  (UI16_T)ace_entry->l4_srcport_mask);

        pattern->commonStdIp.l4Byte2 = ((unsigned char *)&tmp_data)[0];
        pattern->commonStdIp.l4Byte3 = ((unsigned char *)&tmp_data)[1];
        mask->commonStdIp.l4Byte2 = ((unsigned char *)&tmp_mask)[0];
        mask->commonStdIp.l4Byte3 = ((unsigned char *)&tmp_mask)[1];
        pattern->commonStdIp.isL4Valid = 1;
        mask->commonStdIp.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PCL_ID)
    {
        DEVRM_LOG("Qualify PCL ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->pcl_id,
                  ace_entry->pcl_id,
                  ace_entry->pcl_id_mask,
                  ace_entry->pcl_id_mask);

        pattern->common.pclId = ace_entry->pcl_id;
        mask->common.pclId = ace_entry->pcl_id_mask;
    }

    return TRUE;
}

static BOOL_T
DEVRM_RuleToIngStdIpv4L4(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr)
{

    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC *mask = &maskPtr->ruleStdIpv4L4;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC *pattern = &patternPtr->ruleStdIpv4L4;

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VID)
    {
        DEVRM_LOG("Qualify VLAN ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_mask,
                  ace_entry->outvlan_mask);

        pattern->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_data & 0x0FFF);
        mask->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_mask & 0x0FFF);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_UP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP)
    {
        DEVRM_LOG("Qualify UP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_mask,
                  ace_entry->vtagup_mask);

        pattern->common.up = ace_entry->vtagup_data;
        mask->common.up = ace_entry->vtagup_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TAGGED)
    {
        DEVRM_LOG("Qualify Tagged\r\n"
                  "    Data 0x%02X(%u)\r\n",
                  ace_entry->is_tagged,
                  ace_entry->is_tagged);

        pattern->common.isTagged = ace_entry->is_tagged;
        mask->common.isTagged = 0xFF;

        DEVRM_LOG("@isTagged\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->common.isTagged,
                  mask->common.isTagged);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ISARP)
    {
        DEVRM_LOG("Qualify Is ARP\r\n");

        pattern->isArp = 1;
        mask->isArp = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ISBC)
    {
        DEVRM_LOG("Qualify is BC\r\n");

        pattern->isBc= 1;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV4SIP)
    {
        pattern->sip.u32Ip = L_STDLIB_Hton32(ace_entry->srcip_data);
        mask->sip.u32Ip = L_STDLIB_Hton32(ace_entry->srcip_mask);
        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonStdIp.isIpv4 = 1;
        mask->commonStdIp.isIpv4 = 0xFF;

        DEVRM_LOG("Qualify IP SIP\r\n"
                  "    Data 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n"
                  "    Mask 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n",
                  pattern->sip.arIP[0],
                  pattern->sip.arIP[1],
                  pattern->sip.arIP[2],
                  pattern->sip.arIP[3],

                  pattern->sip.arIP[0],
                  pattern->sip.arIP[1],
                  pattern->sip.arIP[2],
                  pattern->sip.arIP[3],

                  mask->sip.arIP[0],
                  mask->sip.arIP[1],
                  mask->sip.arIP[2],
                  mask->sip.arIP[3],

                  mask->sip.arIP[0],
                  mask->sip.arIP[1],
                  mask->sip.arIP[2],
                  mask->sip.arIP[3]);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV4DIP)
    {
        pattern->dip.u32Ip = L_STDLIB_Hton32(ace_entry->dstip_data);
        mask->dip.u32Ip = L_STDLIB_Hton32(ace_entry->dstip_mask);
        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonStdIp.isIpv4 = 1;
        mask->commonStdIp.isIpv4 = 0xFF;

        DEVRM_LOG("Qualify IP DIP\r\n"
                  "    Data 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n"
                  "    Mask 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n",
                  pattern->dip.arIP[0],
                  pattern->dip.arIP[1],
                  pattern->dip.arIP[2],
                  pattern->dip.arIP[3],

                  pattern->dip.arIP[0],
                  pattern->dip.arIP[1],
                  pattern->dip.arIP[2],
                  pattern->dip.arIP[3],

                  mask->dip.arIP[0],
                  mask->dip.arIP[1],
                  mask->dip.arIP[2],
                  mask->dip.arIP[3],

                  mask->dip.arIP[0],
                  mask->dip.arIP[1],
                  mask->dip.arIP[2],
                  mask->dip.arIP[3]);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_DSCP)
    {
        DEVRM_LOG("Qualify DSCP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->dscp_data,
                  ace_entry->dscp_data,
                  ace_entry->dscp_mask,
                  ace_entry->dscp_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonStdIp.dscp = ace_entry->dscp_data;
        mask->commonStdIp.dscp = ace_entry->dscp_mask;
    }
    else
    {
        if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PREC)
        {
            DEVRM_LOG("Qualify Precedence\r\n"
                      "    Data 0x%02X(%u)\r\n"
                      "    Mask 0x%02X(%u)\r\n",
                      ace_entry->prec_data,
                      ace_entry->prec_data,
                      ace_entry->prec_mask,
                      ace_entry->prec_mask);

            pattern->common.isIp = 1;
            mask->common.isIp = 0xFF;
            pattern->commonStdIp.dscp = (ace_entry->prec_data & 0x7) << 3;
            mask->commonStdIp.dscp = (ace_entry->prec_mask & 0x7) << 3;

            DEVRM_LOG("@dscp\r\n"
                      "    Data 0x%02X\r\n"
                      "    Mask 0x%02X\r\n",
                      pattern->commonStdIp.dscp,
                      mask->commonStdIp.dscp);
        }
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPFRAG)
    {
        DEVRM_LOG("Qualify IP Fragment\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->ip_fragment_data,
                  ace_entry->ip_fragment_data,
                  ace_entry->ip_fragment_mask,
                  ace_entry->ip_fragment_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonStdIp.ipv4Fragmented = ace_entry->ip_fragment_data;
        mask->commonStdIp.ipv4Fragmented = ace_entry->ip_fragment_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL)
    {
        DEVRM_LOG("Qualify IP Protocol\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_mask,
                  ace_entry->ipprotocol_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonStdIp.ipProtocol = ace_entry->ipprotocol_data;
        mask->commonStdIp.ipProtocol = ace_entry->ipprotocol_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4DSTPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_dstport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_dstport_mask);

        DEVRM_LOG("Qualify L4 Dest Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_dstport_data,
                  (UI16_T)ace_entry->l4_dstport_data,
                  (UI16_T)ace_entry->l4_dstport_mask,
                  (UI16_T)ace_entry->l4_dstport_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonStdIp.l4Byte2 = ((unsigned char *)&tmp_data)[0];
        pattern->commonStdIp.l4Byte3 = ((unsigned char *)&tmp_data)[1];
        mask->commonStdIp.l4Byte2 = ((unsigned char *)&tmp_mask)[0];
        mask->commonStdIp.l4Byte3 = ((unsigned char *)&tmp_mask)[1];
        pattern->commonStdIp.isL4Valid = 1;
        mask->commonStdIp.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4SRCPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_srcport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_srcport_mask);

        DEVRM_LOG("Qualify L4 Source Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_mask,
                  (UI16_T)ace_entry->l4_srcport_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->l4Byte0 = ((unsigned char *)&tmp_data)[0];
        pattern->l4Byte1 = ((unsigned char *)&tmp_data)[1];
        mask->l4Byte0 = ((unsigned char *)&tmp_mask)[0];
        mask->l4Byte1 = ((unsigned char *)&tmp_mask)[1];
        pattern->commonStdIp.isL4Valid = 1;
        mask->commonStdIp.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TCPCONTROL)
    {
        DEVRM_LOG("Qualify ICP Control\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->tcpcontrol_data,
                  ace_entry->tcpcontrol_data,
                  ace_entry->tcpcontrol_mask,
                  ace_entry->tcpcontrol_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->l4Byte13 = ace_entry->tcpcontrol_data;
        mask->l4Byte13 = ace_entry->tcpcontrol_mask;
        pattern->commonStdIp.isL4Valid = 1;
        mask->commonStdIp.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE)
    {
        DEVRM_LOG("Qualify ICMP Type\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->icmptype_data,
                  ace_entry->icmptype_data,
                  ace_entry->icmptype_mask,
                  ace_entry->icmptype_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->l4Byte0 = ace_entry->icmptype_data;
        mask->l4Byte0 = ace_entry->icmptype_mask;
        pattern->commonStdIp.isL4Valid = 1;
        mask->commonStdIp.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PCL_ID)
    {
        DEVRM_LOG("Qualify PCL ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->pcl_id,
                  ace_entry->pcl_id,
                  ace_entry->pcl_id_mask,
                  ace_entry->pcl_id_mask);

        pattern->common.pclId = ace_entry->pcl_id;
        mask->common.pclId = ace_entry->pcl_id_mask;
    }

    return TRUE;
}

static BOOL_T
DEVRM_RuleToIngStdpv6Dip(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr)
{
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC *mask = &maskPtr->ruleStdIpv6Dip;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC *pattern = &patternPtr->ruleStdIpv6Dip;

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ISARP)
    {
        DEVRM_LOG("Qualify Is ARP\r\n");

        pattern->isArp = 1;
        mask->isArp = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VID)
    {
        DEVRM_LOG("Qualify VLAN ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_mask,
                  ace_entry->outvlan_mask);

        pattern->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_data & 0x0FFF);
        mask->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_mask & 0x0FFF);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_UP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP)
    {
        DEVRM_LOG("Qualify UP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_mask,
                  ace_entry->vtagup_mask);

        pattern->common.up = ace_entry->vtagup_data;
        mask->common.up = ace_entry->vtagup_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TAGGED)
    {
        DEVRM_LOG("Qualify Tagged\r\n"
                  "    Data 0x%02X(%u)\r\n",
                  ace_entry->is_tagged,
                  ace_entry->is_tagged);

        pattern->common.isTagged = ace_entry->is_tagged;
        mask->common.isTagged = 0xFF;

        DEVRM_LOG("@isTagged\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->common.isTagged,
                  mask->common.isTagged);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_DSCP)
    {
        DEVRM_LOG("Qualify DSCP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->dscp_data,
                  ace_entry->dscp_data,
                  ace_entry->dscp_mask,
                  ace_entry->dscp_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonStdIp.dscp = ace_entry->dscp_data;
        mask->commonStdIp.dscp = ace_entry->dscp_mask;
    }
    else
    {
        if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PREC)
        {
            DEVRM_LOG("Qualify Precedence\r\n"
                      "    Data 0x%02X(%u)\r\n"
                      "    Mask 0x%02X(%u)\r\n",
                      ace_entry->prec_data,
                      ace_entry->prec_data,
                      ace_entry->prec_mask,
                      ace_entry->prec_mask);

            pattern->common.isIp = 1;
            mask->common.isIp = 0xFF;
            pattern->commonStdIp.dscp = (ace_entry->prec_data & 0x7) << 3;
            mask->commonStdIp.dscp = (ace_entry->prec_mask & 0x7) << 3;

            DEVRM_LOG("@dscp\r\n"
                      "    Data 0x%02X\r\n"
                      "    Mask 0x%02X\r\n",
                      pattern->commonStdIp.dscp,
                      mask->commonStdIp.dscp);
        }
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6DIP)
    {
        DEVRM_LOG("Qualify IP6 DIP\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->dstip6_data[0],
                  ace_entry->dstip6_data[1],
                  ace_entry->dstip6_data[2],
                  ace_entry->dstip6_data[3],
                  ace_entry->dstip6_data[4],
                  ace_entry->dstip6_data[5],
                  ace_entry->dstip6_data[6],
                  ace_entry->dstip6_data[7],
                  ace_entry->dstip6_data[8],
                  ace_entry->dstip6_data[9],
                  ace_entry->dstip6_data[10],
                  ace_entry->dstip6_data[11],
                  ace_entry->dstip6_data[12],
                  ace_entry->dstip6_data[13],
                  ace_entry->dstip6_data[14],
                  ace_entry->dstip6_data[15],

                  ace_entry->dstip6_mask[0],
                  ace_entry->dstip6_mask[1],
                  ace_entry->dstip6_mask[2],
                  ace_entry->dstip6_mask[3],
                  ace_entry->dstip6_mask[4],
                  ace_entry->dstip6_mask[5],
                  ace_entry->dstip6_mask[6],
                  ace_entry->dstip6_mask[7],
                  ace_entry->dstip6_mask[8],
                  ace_entry->dstip6_mask[9],
                  ace_entry->dstip6_mask[10],
                  ace_entry->dstip6_mask[11],
                  ace_entry->dstip6_mask[12],
                  ace_entry->dstip6_mask[13],
                  ace_entry->dstip6_mask[14],
                  ace_entry->dstip6_mask[15]);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        memcpy(pattern->dip.arIP, ace_entry->dstip6_data, SYS_ADPT_IPV6_ADDR_LEN);
        memcpy(mask->dip.arIP, ace_entry->dstip6_mask, SYS_ADPT_IPV6_ADDR_LEN);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6EHHOPBYHOP)
    {
        DEVRM_LOG("Qualify IP6 Is Hop By Hop\r\n");

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->isIpv6HopByHop = 1;
        mask->isIpv6HopByHop = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6EHEXIST)
    {
        DEVRM_LOG("Qualify IP6 Header Exist\r\n");

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->isIpv6ExtHdrExist = 1;
        mask->isIpv6ExtHdrExist = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL)
    {
        DEVRM_LOG("Qualify IP Protocol\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_mask,
                  ace_entry->ipprotocol_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonStdIp.ipProtocol = ace_entry->ipprotocol_data;
        mask->commonStdIp.ipProtocol = ace_entry->ipprotocol_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4SRCPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_srcport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_srcport_mask);

        DEVRM_LOG("Qualify L4 Source Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_mask,
                  (UI16_T)ace_entry->l4_srcport_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonStdIp.l4Byte2 = ((unsigned char *)&tmp_data)[0];
        pattern->commonStdIp.l4Byte3 = ((unsigned char *)&tmp_data)[1];
        mask->commonStdIp.l4Byte2 = ((unsigned char *)&tmp_mask)[0];
        mask->commonStdIp.l4Byte3 = ((unsigned char *)&tmp_mask)[1];
        pattern->commonStdIp.isL4Valid = 1;
        mask->commonStdIp.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PCL_ID)
    {
        DEVRM_LOG("Qualify PCL ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->pcl_id,
                  ace_entry->pcl_id,
                  ace_entry->pcl_id_mask,
                  ace_entry->pcl_id_mask);

        pattern->common.pclId = ace_entry->pcl_id;
        mask->common.pclId = ace_entry->pcl_id_mask;
    }

    return TRUE;
}

static BOOL_T
DEVRM_RuleToIngExtNotIpv6(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr)
{
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC *mask = &(maskPtr->ruleExtNotIpv6);
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC *pattern = &(patternPtr->ruleExtNotIpv6);

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACDA)
    {
        DEVRM_LOG("Qualify MAC DA\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->dstmac_data[0],
                  ace_entry->dstmac_data[1],
                  ace_entry->dstmac_data[2],
                  ace_entry->dstmac_data[3],
                  ace_entry->dstmac_data[4],
                  ace_entry->dstmac_data[5],

                  ace_entry->dstmac_mask[0],
                  ace_entry->dstmac_mask[1],
                  ace_entry->dstmac_mask[2],
                  ace_entry->dstmac_mask[3],
                  ace_entry->dstmac_mask[4],
                  ace_entry->dstmac_mask[5]);

        memcpy(&pattern->macDa, ace_entry->dstmac_data, sizeof(GT_ETHERADDR));
        memcpy(&mask->macDa, ace_entry->dstmac_mask, sizeof(GT_ETHERADDR));
        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 1;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACSA)
    {
        DEVRM_LOG("Qualify MAC SA\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->srcmac_data[0],
                  ace_entry->srcmac_data[1],
                  ace_entry->srcmac_data[2],
                  ace_entry->srcmac_data[3],
                  ace_entry->srcmac_data[4],
                  ace_entry->srcmac_data[5],

                  ace_entry->srcmac_mask[0],
                  ace_entry->srcmac_mask[1],
                  ace_entry->srcmac_mask[2],
                  ace_entry->srcmac_mask[3],
                  ace_entry->srcmac_mask[4],
                  ace_entry->srcmac_mask[5]);

        memcpy(&pattern->macSa, ace_entry->srcmac_data, sizeof(GT_ETHERADDR));
        memcpy(&mask->macSa, ace_entry->srcmac_mask, sizeof(GT_ETHERADDR));
        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 1;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV4SIP)
    {
        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->sip.u32Ip = L_STDLIB_Hton32(ace_entry->srcip_data);
        mask->sip.u32Ip = L_STDLIB_Hton32(ace_entry->srcip_mask);

        DEVRM_LOG("Qualify IP SIP\r\n"
                  "    Data 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n"
                  "    Mask 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n",
                  pattern->sip.arIP[0],
                  pattern->sip.arIP[1],
                  pattern->sip.arIP[2],
                  pattern->sip.arIP[3],

                  pattern->sip.arIP[0],
                  pattern->sip.arIP[1],
                  pattern->sip.arIP[2],
                  pattern->sip.arIP[3],

                  mask->sip.arIP[0],
                  mask->sip.arIP[1],
                  mask->sip.arIP[2],
                  mask->sip.arIP[3],

                  mask->sip.arIP[0],
                  mask->sip.arIP[1],
                  mask->sip.arIP[2],
                  mask->sip.arIP[3]);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV4DIP)
    {
        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->dip.u32Ip = L_STDLIB_Hton32(ace_entry->dstip_data);
        mask->dip.u32Ip = L_STDLIB_Hton32(ace_entry->dstip_mask);

        DEVRM_LOG("Qualify IP DIP\r\n"
                  "    Data 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n"
                  "    Mask 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n",
                  pattern->dip.arIP[0],
                  pattern->dip.arIP[1],
                  pattern->dip.arIP[2],
                  pattern->dip.arIP[3],

                  pattern->dip.arIP[0],
                  pattern->dip.arIP[1],
                  pattern->dip.arIP[2],
                  pattern->dip.arIP[3],

                  mask->dip.arIP[0],
                  mask->dip.arIP[1],
                  mask->dip.arIP[2],
                  mask->dip.arIP[3],

                  mask->dip.arIP[0],
                  mask->dip.arIP[1],
                  mask->dip.arIP[2],
                  mask->dip.arIP[3]);
    }


    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ETHER)
    {
        DEVRM_LOG("Qualify Ether Type\r\n"
                  "    Data 0x%04hX\r\n"
                  "    Mask 0x%04hX\r\n",
                  ace_entry->ethertype_data,
                  ace_entry->ethertype_mask);

        mask->etherType = L_STDLIB_Hton16(ace_entry->ethertype_mask);
        pattern->etherType = L_STDLIB_Hton16(ace_entry->ethertype_data);

        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 1;
        pattern->l2Encap = 1;
        mask->l2Encap = 0x1;
#if 0 /*pttch mark it, if set isIP, the rule will not matched*/
        switch (ace_entry->ethertype_mask & ace_entry->ethertype_data)
        {
            case 0x0800:
                pattern->common.isIp = 1;
                mask->common.isIp = 0xFF;
                break;
            default:
                break;
        }
#endif
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ENCAP)
    {
        DEVRM_LOG("Qualify ENCAP\r\n"
                  "  Packet Format\r\n"
                  "    Data 0x%08lX(%s)\r\n",
                  ace_entry->packetformat_data,
                  ETHERTYPE_ENCAPS == ace_entry->packetformat_data ? "ENCAPS" : "");

        pattern->l2Encap = (ace_entry->packetformat_data == ETHERTYPE_ENCAPS) ? 1 : 0;
        mask->l2Encap = 0xFF;

        DEVRM_LOG("@l2Encap\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->l2Encap,
                  mask->l2Encap);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VID)
    {
        DEVRM_LOG("Qualify VLAN ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_mask,
                  ace_entry->outvlan_mask);

        pattern->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_data & 0x0FFF);
        mask->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_mask & 0x0FFF);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_UP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP)
    {
        DEVRM_LOG("Qualify UP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_mask,
                  ace_entry->vtagup_mask);

        pattern->common.up = ace_entry->vtagup_data;
        mask->common.up = ace_entry->vtagup_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TAGGED)
    {
        DEVRM_LOG("Qualify Tagged\r\n"
                  "    Data 0x%02X(%u)\r\n",
                  ace_entry->is_tagged,
                  ace_entry->is_tagged);

        pattern->common.isTagged = ace_entry->is_tagged;
        mask->common.isTagged = 0xFF;

        DEVRM_LOG("@isTagged\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->common.isTagged,
                  mask->common.isTagged);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPFRAG)
    {
        DEVRM_LOG("Qualify IP Fragment\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->ip_fragment_data,
                  ace_entry->ip_fragment_data,
                  ace_entry->ip_fragment_mask,
                  ace_entry->ip_fragment_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->ipv4Fragmented = ace_entry->ip_fragment_data;
        mask->ipv4Fragmented = ace_entry->ip_fragment_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL)
    {
        DEVRM_LOG("Qualify IP Protocol\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_mask,
                  ace_entry->ipprotocol_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonExt.ipProtocol = ace_entry->ipprotocol_data;
        mask->commonExt.ipProtocol = ace_entry->ipprotocol_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_DSCP)
    {
        DEVRM_LOG("Qualify DSCP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->dscp_data,
                  ace_entry->dscp_data,
                  ace_entry->dscp_mask,
                  ace_entry->dscp_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonExt.dscp = ace_entry->dscp_data;
        mask->commonExt.dscp = ace_entry->dscp_mask;
    }
    else
    {
        if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PREC)
        {
            DEVRM_LOG("Qualify Precedence\r\n"
                      "    Data 0x%02X(%u)\r\n"
                      "    Mask 0x%02X(%u)\r\n",
                      ace_entry->prec_data,
                      ace_entry->prec_data,
                      ace_entry->prec_mask,
                      ace_entry->prec_mask);

            pattern->common.isIp = 1;
            mask->common.isIp = 0xFF;
            pattern->commonExt.dscp = (ace_entry->prec_data & 0x7) << 3;
            mask->commonExt.dscp = (ace_entry->prec_mask & 0x7) << 3;

            DEVRM_LOG("@dscp\r\n"
                      "    Data 0x%02X\r\n"
                      "    Mask 0x%02X\r\n",
                      pattern->commonExt.dscp,
                      mask->commonExt.dscp);
        }
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4DSTPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_dstport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_dstport_mask);

        DEVRM_LOG("Qualify L4 Dest Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_dstport_data,
                  (UI16_T)ace_entry->l4_dstport_data,
                  (UI16_T)ace_entry->l4_dstport_mask,
                  (UI16_T)ace_entry->l4_dstport_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonExt.l4Byte2 = ((unsigned char *)&tmp_data)[0];
        pattern->commonExt.l4Byte3 = ((unsigned char *)&tmp_data)[1];
        mask->commonExt.l4Byte2 = ((unsigned char *)&tmp_mask)[0];
        mask->commonExt.l4Byte3 = ((unsigned char *)&tmp_mask)[1];
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4SRCPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_srcport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_srcport_mask);

        DEVRM_LOG("Qualify L4 Source Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_mask,
                  (UI16_T)ace_entry->l4_srcport_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonExt.l4Byte0 = ((unsigned char *)&tmp_data)[0];
        pattern->commonExt.l4Byte1 = ((unsigned char *)&tmp_data)[1];
        mask->commonExt.l4Byte0 = ((unsigned char *)&tmp_mask)[0];
        mask->commonExt.l4Byte1 = ((unsigned char *)&tmp_mask)[1];
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TCPCONTROL)
    {
        DEVRM_LOG("Qualify ICP Control\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->tcpcontrol_data,
                  ace_entry->tcpcontrol_data,
                  ace_entry->tcpcontrol_mask,
                  ace_entry->tcpcontrol_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonExt.l4Byte13 = ace_entry->tcpcontrol_data;
        mask->commonExt.l4Byte13 = ace_entry->tcpcontrol_mask;
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE)
    {
        DEVRM_LOG("Qualify ICMP Type\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->icmptype_data,
                  ace_entry->icmptype_data,
                  ace_entry->icmptype_mask,
                  ace_entry->icmptype_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonExt.l4Byte0 = ace_entry->icmptype_data;
        mask->commonExt.l4Byte0 = ace_entry->icmptype_mask;
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PCL_ID)
    {
        DEVRM_LOG("Qualify PCL ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->pcl_id,
                  ace_entry->pcl_id,
                  ace_entry->pcl_id_mask,
                  ace_entry->pcl_id_mask);

        pattern->common.pclId = ace_entry->pcl_id;
        mask->common.pclId = ace_entry->pcl_id_mask;
    }

    return TRUE;

}

static BOOL_T
DEVRM_RuleToIngExtIpv6L2(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr)
{
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC *mask = &maskPtr->ruleExtIpv6L2;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC *pattern = &patternPtr->ruleExtIpv6L2;

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACDA)
    {
        DEVRM_LOG("Qualify MAC DA\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->dstmac_data[0],
                  ace_entry->dstmac_data[1],
                  ace_entry->dstmac_data[2],
                  ace_entry->dstmac_data[3],
                  ace_entry->dstmac_data[4],
                  ace_entry->dstmac_data[5],

                  ace_entry->dstmac_mask[0],
                  ace_entry->dstmac_mask[1],
                  ace_entry->dstmac_mask[2],
                  ace_entry->dstmac_mask[3],
                  ace_entry->dstmac_mask[4],
                  ace_entry->dstmac_mask[5]);

        memcpy(&mask->macDa, ace_entry->dstmac_mask, sizeof(GT_ETHERADDR));
        memcpy(&pattern->macDa, ace_entry->dstmac_data, sizeof(GT_ETHERADDR));
        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACSA)
    {
        DEVRM_LOG("Qualify MAC SA\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->srcmac_data[0],
                  ace_entry->srcmac_data[1],
                  ace_entry->srcmac_data[2],
                  ace_entry->srcmac_data[3],
                  ace_entry->srcmac_data[4],
                  ace_entry->srcmac_data[5],

                  ace_entry->srcmac_mask[0],
                  ace_entry->srcmac_mask[1],
                  ace_entry->srcmac_mask[2],
                  ace_entry->srcmac_mask[3],
                  ace_entry->srcmac_mask[4],
                  ace_entry->srcmac_mask[5]);

        memcpy(&mask->macSa, ace_entry->srcmac_mask, sizeof(GT_ETHERADDR));
        memcpy(&pattern->macSa, ace_entry->srcmac_data, sizeof(GT_ETHERADDR));
        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6EHHOPBYHOP)
    {
        DEVRM_LOG("Qualify IP6 Is Hop By Hop\r\n");

        pattern->isIpv6HopByHop = 1;
        mask->isIpv6HopByHop = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6EHEXIST)
    {
        DEVRM_LOG("Qualify IP6 Header Exist\r\n");

        pattern->isIpv6ExtHdrExist = 1;
        mask->isIpv6HopByHop = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6DIPUPPER)
    {
        DEVRM_LOG("Qualify IP6 DIP UPPER\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  ace_entry->dstip6_data[0],
                  ace_entry->dstip6_mask[0]);

        pattern->dipBits127to120 = ace_entry->dstip6_data[0];
        mask->dipBits127to120 = ace_entry->dstip6_mask[0];
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6SIP)
    {
        UI32_T i;

        DEVRM_LOG("Qualify IP6 SIP\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->srcip6_data[0],
                  ace_entry->srcip6_data[1],
                  ace_entry->srcip6_data[2],
                  ace_entry->srcip6_data[3],
                  ace_entry->srcip6_data[4],
                  ace_entry->srcip6_data[5],
                  ace_entry->srcip6_data[6],
                  ace_entry->srcip6_data[7],
                  ace_entry->srcip6_data[8],
                  ace_entry->srcip6_data[9],
                  ace_entry->srcip6_data[10],
                  ace_entry->srcip6_data[11],
                  ace_entry->srcip6_data[12],
                  ace_entry->srcip6_data[13],
                  ace_entry->srcip6_data[14],
                  ace_entry->srcip6_data[15],

                  ace_entry->srcip6_mask[0],
                  ace_entry->srcip6_mask[1],
                  ace_entry->srcip6_mask[2],
                  ace_entry->srcip6_mask[3],
                  ace_entry->srcip6_mask[4],
                  ace_entry->srcip6_mask[5],
                  ace_entry->srcip6_mask[6],
                  ace_entry->srcip6_mask[7],
                  ace_entry->srcip6_mask[8],
                  ace_entry->srcip6_mask[9],
                  ace_entry->srcip6_mask[10],
                  ace_entry->srcip6_mask[11],
                  ace_entry->srcip6_mask[12],
                  ace_entry->srcip6_mask[13],
                  ace_entry->srcip6_mask[14],
                  ace_entry->srcip6_mask[15]);

        /* EPR: ASE3526MP-FLF-EC-00024
         * Rule is not work if includes IPv6 address with prefix length
         */
        for (i=0; i<SYS_ADPT_IPV6_ADDR_LEN; ++i)
        {
            pattern->sip.arIP[i] = ace_entry->srcip6_data[i] & ace_entry->srcip6_mask[i];
        }

        /* memcpy(pattern->sip.arIP, ace_entry->srcip6_data, SYS_ADPT_IPV6_ADDR_LEN); */
        memcpy(mask->sip.arIP, ace_entry->srcip6_mask, SYS_ADPT_IPV6_ADDR_LEN);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_DSCP)
    {
        DEVRM_LOG("Qualify DSCP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->dscp_data,
                  ace_entry->dscp_data,
                  ace_entry->dscp_mask,
                  ace_entry->dscp_mask);

        pattern->commonExt.dscp = ace_entry->dscp_data;
        mask->commonExt.dscp = ace_entry->dscp_mask;
    }
    else
    {
        if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PREC)
        {
            DEVRM_LOG("Qualify Precedence\r\n"
                      "    Data 0x%02X(%u)\r\n"
                      "    Mask 0x%02X(%u)\r\n",
                      ace_entry->prec_data,
                      ace_entry->prec_data,
                      ace_entry->prec_mask,
                      ace_entry->prec_mask);

            pattern->commonExt.dscp = (ace_entry->prec_data & 0x7) << 3;
            mask->commonExt.dscp = (ace_entry->prec_mask & 0x7) << 3;

            DEVRM_LOG("@dscp\r\n"
                      "    Data 0x%02X\r\n"
                      "    Mask 0x%02X\r\n",
                      pattern->commonExt.dscp,
                      mask->commonExt.dscp);
        }
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VID)
    {
        DEVRM_LOG("Qualify VLAN ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_mask,
                  ace_entry->outvlan_mask);

        pattern->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_data & 0x0FFF);
        mask->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_mask & 0x0FFF);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_UP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP)
    {
        DEVRM_LOG("Qualify UP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_mask,
                  ace_entry->vtagup_mask);

        pattern->common.up = ace_entry->vtagup_data;
        mask->common.up = ace_entry->vtagup_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TAGGED)
    {
        DEVRM_LOG("Qualify Tagged\r\n"
                  "    Data 0x%02X(%u)\r\n",
                  ace_entry->is_tagged,
                  ace_entry->is_tagged);

        pattern->common.isTagged = ace_entry->is_tagged;
        mask->common.isTagged = 0xFF;

        DEVRM_LOG("@isTagged\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->common.isTagged,
                  mask->common.isTagged);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL)
    {
        DEVRM_LOG("Qualify IP Protocol\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_mask,
                  ace_entry->ipprotocol_mask);

        pattern->commonExt.ipProtocol = ace_entry->ipprotocol_data;
        mask->commonExt.ipProtocol = ace_entry->ipprotocol_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4DSTPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_dstport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_dstport_mask);

        DEVRM_LOG("Qualify L4 Dest Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_dstport_data,
                  (UI16_T)ace_entry->l4_dstport_data,
                  (UI16_T)ace_entry->l4_dstport_mask,
                  (UI16_T)ace_entry->l4_dstport_mask);

        pattern->commonExt.l4Byte2 = ((unsigned char *)&tmp_data)[0];
        pattern->commonExt.l4Byte3 = ((unsigned char *)&tmp_data)[1];
        mask->commonExt.l4Byte2 = ((unsigned char *)&tmp_mask)[0];
        mask->commonExt.l4Byte3 = ((unsigned char *)&tmp_mask)[1];
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4SRCPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_srcport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_srcport_mask);

        DEVRM_LOG("Qualify L4 Source Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_mask,
                  (UI16_T)ace_entry->l4_srcport_mask);

        pattern->commonExt.l4Byte0 = ((unsigned char *)&tmp_data)[0];
        pattern->commonExt.l4Byte1 = ((unsigned char *)&tmp_data)[1];
        mask->commonExt.l4Byte0 = ((unsigned char *)&tmp_mask)[0];
        mask->commonExt.l4Byte1 = ((unsigned char *)&tmp_mask)[1];
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TCPCONTROL)
    {
        DEVRM_LOG("Qualify ICP Control\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->tcpcontrol_data,
                  ace_entry->tcpcontrol_data,
                  ace_entry->tcpcontrol_mask,
                  ace_entry->tcpcontrol_mask);

        pattern->commonExt.l4Byte13 = ace_entry->tcpcontrol_data;
        mask->commonExt.l4Byte13 = ace_entry->tcpcontrol_mask;
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE)
    {
        DEVRM_LOG("Qualify ICMP Type\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->icmptype_data,
                  ace_entry->icmptype_data,
                  ace_entry->icmptype_mask,
                  ace_entry->icmptype_mask);

        pattern->commonExt.l4Byte0 = ace_entry->icmptype_data;
        mask->commonExt.l4Byte0 = ace_entry->icmptype_mask;
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PCL_ID)
    {
        DEVRM_LOG("Qualify PCL ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->pcl_id,
                  ace_entry->pcl_id,
                  ace_entry->pcl_id_mask,
                  ace_entry->pcl_id_mask);

        pattern->common.pclId = ace_entry->pcl_id;
        mask->common.pclId = ace_entry->pcl_id_mask;
    }

    pattern->commonExt.isIpv6 = 1;
    mask->commonExt.isIpv6 = 0xFF;

    return TRUE;

}

static BOOL_T
DEVRM_RuleToIngExtIpv6L4(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr)
{
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC *mask = &maskPtr->ruleExtIpv6L4;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC *pattern = &patternPtr->ruleExtIpv6L4;

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6SIP)
    {
        UI32_T i;

        DEVRM_LOG("Qualify IP6 SIP\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->srcip6_data[0],
                  ace_entry->srcip6_data[1],
                  ace_entry->srcip6_data[2],
                  ace_entry->srcip6_data[3],
                  ace_entry->srcip6_data[4],
                  ace_entry->srcip6_data[5],
                  ace_entry->srcip6_data[6],
                  ace_entry->srcip6_data[7],
                  ace_entry->srcip6_data[8],
                  ace_entry->srcip6_data[9],
                  ace_entry->srcip6_data[10],
                  ace_entry->srcip6_data[11],
                  ace_entry->srcip6_data[12],
                  ace_entry->srcip6_data[13],
                  ace_entry->srcip6_data[14],
                  ace_entry->srcip6_data[15],

                  ace_entry->srcip6_mask[0],
                  ace_entry->srcip6_mask[1],
                  ace_entry->srcip6_mask[2],
                  ace_entry->srcip6_mask[3],
                  ace_entry->srcip6_mask[4],
                  ace_entry->srcip6_mask[5],
                  ace_entry->srcip6_mask[6],
                  ace_entry->srcip6_mask[7],
                  ace_entry->srcip6_mask[8],
                  ace_entry->srcip6_mask[9],
                  ace_entry->srcip6_mask[10],
                  ace_entry->srcip6_mask[11],
                  ace_entry->srcip6_mask[12],
                  ace_entry->srcip6_mask[13],
                  ace_entry->srcip6_mask[14],
                  ace_entry->srcip6_mask[15]);

        /* EPR: ASE3526MP-FLF-EC-00024
         * Rule is not work if includes IPv6 address with prefix length
         */
        for (i=0; i<SYS_ADPT_IPV6_ADDR_LEN; ++i)
        {
            pattern->sip.arIP[i] = ace_entry->srcip6_data[i] & ace_entry->srcip6_mask[i];
        }

        /* memcpy(pattern->sip.arIP, ace_entry->srcip6_data, SYS_ADPT_IPV6_ADDR_LEN); */
        memcpy(mask->sip.arIP, ace_entry->srcip6_mask, SYS_ADPT_IPV6_ADDR_LEN);

    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6DIP)
    {
        DEVRM_LOG("Qualify IP6 DIP\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->dstip6_data[0],
                  ace_entry->dstip6_data[1],
                  ace_entry->dstip6_data[2],
                  ace_entry->dstip6_data[3],
                  ace_entry->dstip6_data[4],
                  ace_entry->dstip6_data[5],
                  ace_entry->dstip6_data[6],
                  ace_entry->dstip6_data[7],
                  ace_entry->dstip6_data[8],
                  ace_entry->dstip6_data[9],
                  ace_entry->dstip6_data[10],
                  ace_entry->dstip6_data[11],
                  ace_entry->dstip6_data[12],
                  ace_entry->dstip6_data[13],
                  ace_entry->dstip6_data[14],
                  ace_entry->dstip6_data[15],

                  ace_entry->dstip6_mask[0],
                  ace_entry->dstip6_mask[1],
                  ace_entry->dstip6_mask[2],
                  ace_entry->dstip6_mask[3],
                  ace_entry->dstip6_mask[4],
                  ace_entry->dstip6_mask[5],
                  ace_entry->dstip6_mask[6],
                  ace_entry->dstip6_mask[7],
                  ace_entry->dstip6_mask[8],
                  ace_entry->dstip6_mask[9],
                  ace_entry->dstip6_mask[10],
                  ace_entry->dstip6_mask[11],
                  ace_entry->dstip6_mask[12],
                  ace_entry->dstip6_mask[13],
                  ace_entry->dstip6_mask[14],
                  ace_entry->dstip6_mask[15]);

        memcpy(pattern->dip.arIP, ace_entry->dstip6_data, SYS_ADPT_IPV6_ADDR_LEN);
        memcpy(mask->dip.arIP, ace_entry->dstip6_mask, SYS_ADPT_IPV6_ADDR_LEN);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6EHHOPBYHOP)
    {
        DEVRM_LOG("Qualify IP6 Is Hop By Hop\r\n");

        pattern->commonExt.isIpv6 = 1;
        mask->commonExt.isIpv6 = 0xFF;
        pattern->isIpv6HopByHop = 1;
        mask->isIpv6HopByHop = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6EHEXIST)
    {
        DEVRM_LOG("Qualify IP6 Header Exist\r\n");

        pattern->isIpv6ExtHdrExist = 1;
        mask->isIpv6ExtHdrExist = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IP6TRAFFICCLASS)
    {
        DEVRM_LOG("Qualify IP6 Traffic Class\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->ip6trafficclass_data,
                  ace_entry->ip6trafficclass_data,
                  ace_entry->ip6trafficclass_mask,
                  ace_entry->ip6trafficclass_mask);

        pattern->commonExt.dscp = ace_entry->ip6trafficclass_data;
        mask->commonExt.dscp = ace_entry->ip6trafficclass_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_DSCP)
    {
        DEVRM_LOG("Qualify DSCP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->dscp_data,
                  ace_entry->dscp_data,
                  ace_entry->dscp_mask,
                  ace_entry->dscp_mask);

        pattern->commonExt.dscp = ace_entry->dscp_data;
        mask->commonExt.dscp = ace_entry->dscp_mask;
    }
    else
    {
        if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PREC)
        {
            DEVRM_LOG("Qualify Precedence\r\n"
                      "    Data 0x%02X(%u)\r\n"
                      "    Mask 0x%02X(%u)\r\n",
                      ace_entry->prec_data,
                      ace_entry->prec_data,
                      ace_entry->prec_mask,
                      ace_entry->prec_mask);

            pattern->commonExt.dscp = (ace_entry->prec_data & 0x7) << 3;
            mask->commonExt.dscp = (ace_entry->prec_mask & 0x7) << 3;

            DEVRM_LOG("@dscp\r\n"
                      "    Data 0x%02X\r\n"
                      "    Mask 0x%02X\r\n",
                      pattern->commonExt.dscp,
                      mask->commonExt.dscp);
        }
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VID)
    {
        DEVRM_LOG("Qualify VLAN ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_mask,
                  ace_entry->outvlan_mask);

        pattern->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_data & 0x0FFF);
        mask->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_mask & 0x0FFF);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_UP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP)
    {
        DEVRM_LOG("Qualify UP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_mask,
                  ace_entry->vtagup_mask);

        pattern->common.up = ace_entry->vtagup_data;
        mask->common.up = ace_entry->vtagup_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TAGGED)
    {
        DEVRM_LOG("Qualify Tagged\r\n"
                  "    Data 0x%02X(%u)\r\n",
                  ace_entry->is_tagged,
                  ace_entry->is_tagged);

        pattern->common.isTagged = ace_entry->is_tagged;
        mask->common.isTagged = 0xFF;

        DEVRM_LOG("@isTagged\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->common.isTagged,
                  mask->common.isTagged);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL)
    {
        DEVRM_LOG("Qualify IP Protocol\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_mask,
                  ace_entry->ipprotocol_mask);

        pattern->commonExt.ipProtocol = ace_entry->ipprotocol_data;
        mask->commonExt.ipProtocol = ace_entry->ipprotocol_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4DSTPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_dstport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_dstport_mask);

        DEVRM_LOG("Qualify L4 Dest Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_dstport_data,
                  (UI16_T)ace_entry->l4_dstport_data,
                  (UI16_T)ace_entry->l4_dstport_mask,
                  (UI16_T)ace_entry->l4_dstport_mask);

        pattern->commonExt.l4Byte2 = ((unsigned char *)&tmp_data)[0];
        pattern->commonExt.l4Byte3 = ((unsigned char *)&tmp_data)[1];
        mask->commonExt.l4Byte2 = ((unsigned char *)&tmp_mask)[0];
        mask->commonExt.l4Byte3 = ((unsigned char *)&tmp_mask)[1];
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4SRCPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_srcport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_srcport_mask);

        DEVRM_LOG("Qualify L4 Source Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_mask,
                  (UI16_T)ace_entry->l4_srcport_mask);

        pattern->commonExt.l4Byte0 = ((unsigned char *)&tmp_data)[0];
        pattern->commonExt.l4Byte1 = ((unsigned char *)&tmp_data)[1];
        mask->commonExt.l4Byte0 = ((unsigned char *)&tmp_mask)[0];
        mask->commonExt.l4Byte1 = ((unsigned char *)&tmp_mask)[1];
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TCPCONTROL)
    {
        DEVRM_LOG("Qualify ICP Control\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->tcpcontrol_data,
                  ace_entry->tcpcontrol_data,
                  ace_entry->tcpcontrol_mask,
                  ace_entry->tcpcontrol_mask);

        pattern->commonExt.l4Byte13 = ace_entry->tcpcontrol_data;
        mask->commonExt.l4Byte13 = ace_entry->tcpcontrol_mask;
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE)
    {
        DEVRM_LOG("Qualify ICMP Type\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->icmptype_data,
                  ace_entry->icmptype_data,
                  ace_entry->icmptype_mask,
                  ace_entry->icmptype_mask);

        pattern->commonExt.l4Byte0 = ace_entry->icmptype_data;
        mask->commonExt.l4Byte0 = ace_entry->icmptype_mask;
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PCL_ID)
    {
        DEVRM_LOG("Qualify PCL ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->pcl_id,
                  ace_entry->pcl_id,
                  ace_entry->pcl_id_mask,
                  ace_entry->pcl_id_mask);

        pattern->common.pclId = ace_entry->pcl_id;
        mask->common.pclId = ace_entry->pcl_id_mask;
    }

    pattern->commonExt.isIpv6 = 1;
    mask->commonExt.isIpv6 = 0xFF;

    return TRUE;
}

static BOOL_T
DEVRM_RuleToEgrExtNotIpv6(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr)
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC *mask = &(maskPtr->ruleEgrExtNotIpv6);
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC *pattern = &(patternPtr->ruleEgrExtNotIpv6);

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_SRCPORT)
    {
        UI32_T unit = 1;
        UI32_T module_id;
        UI32_T device_id;
        UI32_T phy_port;

        DEVRM_LOG("Qualify Source Port\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->srcport_data,
                  ace_entry->srcport_data,
                  ace_entry->srcport_mask,
                  ace_entry->srcport_mask);

        if (DEV_SWDRV_Logical2PhyDevicePortID(unit, ace_entry->srcport_data, &module_id, &device_id, &phy_port) == FALSE)
        {
            DEBUG_ERROR_MSG(0);
            return FALSE;
        }

        pattern->common.sourcePort = phy_port;
        mask->common.sourcePort = ace_entry->srcport_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACDA)
    {
        DEVRM_LOG("Qualify MAC DA\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->dstmac_data[0],
                  ace_entry->dstmac_data[1],
                  ace_entry->dstmac_data[2],
                  ace_entry->dstmac_data[3],
                  ace_entry->dstmac_data[4],
                  ace_entry->dstmac_data[5],

                  ace_entry->dstmac_mask[0],
                  ace_entry->dstmac_mask[1],
                  ace_entry->dstmac_mask[2],
                  ace_entry->dstmac_mask[3],
                  ace_entry->dstmac_mask[4],
                  ace_entry->dstmac_mask[5]);

        memcpy(&pattern->macDa, ace_entry->dstmac_data, sizeof(GT_ETHERADDR));
        memcpy(&mask->macDa, ace_entry->dstmac_mask, sizeof(GT_ETHERADDR));
        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 1;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACSA)
    {
        DEVRM_LOG("Qualify MAC SA\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->srcmac_data[0],
                  ace_entry->srcmac_data[1],
                  ace_entry->srcmac_data[2],
                  ace_entry->srcmac_data[3],
                  ace_entry->srcmac_data[4],
                  ace_entry->srcmac_data[5],

                  ace_entry->srcmac_mask[0],
                  ace_entry->srcmac_mask[1],
                  ace_entry->srcmac_mask[2],
                  ace_entry->srcmac_mask[3],
                  ace_entry->srcmac_mask[4],
                  ace_entry->srcmac_mask[5]);

        memcpy(&pattern->macSa, ace_entry->srcmac_data, sizeof(GT_ETHERADDR));
        memcpy(&mask->macSa, ace_entry->srcmac_mask, sizeof(GT_ETHERADDR));
        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 1;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV4SIP)
    {
        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->sip.u32Ip = L_STDLIB_Hton32(ace_entry->srcip_data);
        mask->sip.u32Ip = L_STDLIB_Hton32(ace_entry->srcip_mask);

        DEVRM_LOG("Qualify IP SIP\r\n"
                  "    Data 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n"
                  "    Mask 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n",
                  pattern->sip.arIP[0],
                  pattern->sip.arIP[1],
                  pattern->sip.arIP[2],
                  pattern->sip.arIP[3],

                  pattern->sip.arIP[0],
                  pattern->sip.arIP[1],
                  pattern->sip.arIP[2],
                  pattern->sip.arIP[3],

                  mask->sip.arIP[0],
                  mask->sip.arIP[1],
                  mask->sip.arIP[2],
                  mask->sip.arIP[3],

                  mask->sip.arIP[0],
                  mask->sip.arIP[1],
                  mask->sip.arIP[2],
                  mask->sip.arIP[3]);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV4DIP)
    {
        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->dip.u32Ip = L_STDLIB_Hton32(ace_entry->dstip_data);
        mask->dip.u32Ip = L_STDLIB_Hton32(ace_entry->dstip_mask);

        DEVRM_LOG("Qualify IP DIP\r\n"
                  "    Data 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n"
                  "    Mask 0x%02X%02X%02X%02X(%u.%u.%u.%u)\r\n",
                  pattern->dip.arIP[0],
                  pattern->dip.arIP[1],
                  pattern->dip.arIP[2],
                  pattern->dip.arIP[3],

                  pattern->dip.arIP[0],
                  pattern->dip.arIP[1],
                  pattern->dip.arIP[2],
                  pattern->dip.arIP[3],

                  mask->dip.arIP[0],
                  mask->dip.arIP[1],
                  mask->dip.arIP[2],
                  mask->dip.arIP[3],

                  mask->dip.arIP[0],
                  mask->dip.arIP[1],
                  mask->dip.arIP[2],
                  mask->dip.arIP[3]);
    }


    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ETHER)
    {
        DEVRM_LOG("Qualify Ether Type\r\n"
                  "    Data 0x%04hX\r\n"
                  "    Mask 0x%04hX\r\n",
                  ace_entry->ethertype_data,
                  ace_entry->ethertype_mask);

        mask->etherType = L_STDLIB_Hton16(ace_entry->ethertype_mask);
        pattern->etherType = L_STDLIB_Hton16(ace_entry->ethertype_data);

        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 1;
        pattern->l2Encap = 1;
        mask->l2Encap = 0x1;
#if 0 /*pttch mark it, if set isIP, the rule will not matched*/
        switch (ace_entry->ethertype_mask & ace_entry->ethertype_data)
        {
            case 0x0800:
                pattern->common.isIp = 1;
                mask->common.isIp = 0xFF;
                break;
            default:
                break;
        }
#endif
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ENCAP)
    {
        DEVRM_LOG("Qualify ENCAP\r\n"
                  "  Packet Format\r\n"
                  "    Data 0x%08lX(%s)\r\n",
                  ace_entry->packetformat_data,
                  ETHERTYPE_ENCAPS == ace_entry->packetformat_data ? "ENCAPS" : "");

        pattern->l2Encap = (ace_entry->packetformat_data == ETHERTYPE_ENCAPS) ? 1 : 0;
        mask->l2Encap = 0xFF;

        DEVRM_LOG("@l2Encap\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->l2Encap,
                  mask->l2Encap);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VID)
    {
        DEVRM_LOG("Qualify VLAN ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_mask,
                  ace_entry->outvlan_mask);

        pattern->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_data & 0x0FFF);
        mask->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_mask & 0x0FFF);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_UP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP)
    {
        DEVRM_LOG("Qualify UP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_mask,
                  ace_entry->vtagup_mask);

        pattern->common.up = ace_entry->vtagup_data;
        mask->common.up = ace_entry->vtagup_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TAGGED)
    {
        DEVRM_LOG("Qualify Tagged\r\n"
                  "    Data 0x%02X(%u)\r\n",
                  ace_entry->is_tagged,
                  ace_entry->is_tagged);

        pattern->common.isTagged = ace_entry->is_tagged;
        mask->common.isTagged = 0xFF;

        DEVRM_LOG("@isTagged\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->common.isTagged,
                  mask->common.isTagged);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPFRAG)
    {
        DEVRM_LOG("Qualify IP Fragment\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->ip_fragment_data,
                  ace_entry->ip_fragment_data,
                  ace_entry->ip_fragment_mask,
                  ace_entry->ip_fragment_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->ipv4Fragmented = ace_entry->ip_fragment_data;
        mask->ipv4Fragmented = ace_entry->ip_fragment_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL)
    {
        DEVRM_LOG("Qualify IP Protocol\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_mask,
                  ace_entry->ipprotocol_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonExt.ipProtocol = ace_entry->ipprotocol_data;
        mask->commonExt.ipProtocol = ace_entry->ipprotocol_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_DSCP)
    {
        DEVRM_LOG("Qualify DSCP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->dscp_data,
                  ace_entry->dscp_data,
                  ace_entry->dscp_mask,
                  ace_entry->dscp_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonExt.dscp = ace_entry->dscp_data;
        mask->commonExt.dscp = ace_entry->dscp_mask;
    }
    else
    {
        if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PREC)
        {
            DEVRM_LOG("Qualify Precedence\r\n"
                      "    Data 0x%02X(%u)\r\n"
                      "    Mask 0x%02X(%u)\r\n",
                      ace_entry->prec_data,
                      ace_entry->prec_data,
                      ace_entry->prec_mask,
                      ace_entry->prec_mask);

            pattern->common.isIp = 1;
            mask->common.isIp = 0xFF;
            pattern->commonExt.dscp = (ace_entry->prec_data & 0x7) << 3;
            mask->commonExt.dscp = (ace_entry->prec_mask & 0x7) << 3;

            DEVRM_LOG("@dscp\r\n"
                      "    Data 0x%02X\r\n"
                      "    Mask 0x%02X\r\n",
                      pattern->commonExt.dscp,
                      mask->commonExt.dscp);
        }
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4DSTPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_dstport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_dstport_mask);

        DEVRM_LOG("Qualify L4 Dest Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_dstport_data,
                  (UI16_T)ace_entry->l4_dstport_data,
                  (UI16_T)ace_entry->l4_dstport_mask,
                  (UI16_T)ace_entry->l4_dstport_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonExt.l4Byte2 = ((unsigned char *)&tmp_data)[0];
        pattern->commonExt.l4Byte3 = ((unsigned char *)&tmp_data)[1];
        mask->commonExt.l4Byte2 = ((unsigned char *)&tmp_mask)[0];
        mask->commonExt.l4Byte3 = ((unsigned char *)&tmp_mask)[1];
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4SRCPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_srcport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_srcport_mask);

        DEVRM_LOG("Qualify L4 Source Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_mask,
                  (UI16_T)ace_entry->l4_srcport_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonExt.l4Byte0 = ((unsigned char *)&tmp_data)[0];
        pattern->commonExt.l4Byte1 = ((unsigned char *)&tmp_data)[1];
        mask->commonExt.l4Byte0 = ((unsigned char *)&tmp_mask)[0];
        mask->commonExt.l4Byte1 = ((unsigned char *)&tmp_mask)[1];
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TCPCONTROL)
    {
        DEVRM_LOG("Qualify ICP Control\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->tcpcontrol_data,
                  ace_entry->tcpcontrol_data,
                  ace_entry->tcpcontrol_mask,
                  ace_entry->tcpcontrol_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonExt.l4Byte13 = ace_entry->tcpcontrol_data;
        mask->commonExt.l4Byte13 = ace_entry->tcpcontrol_mask;
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE)
    {
        DEVRM_LOG("Qualify ICMP Type\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->icmptype_data,
                  ace_entry->icmptype_data,
                  ace_entry->icmptype_mask,
                  ace_entry->icmptype_mask);

        pattern->common.isIp = 1;
        mask->common.isIp = 0xFF;
        pattern->commonExt.l4Byte0 = ace_entry->icmptype_data;
        mask->commonExt.l4Byte0 = ace_entry->icmptype_mask;
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PCL_ID)
    {
        DEVRM_LOG("Qualify PCL ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->pcl_id,
                  ace_entry->pcl_id,
                  ace_entry->pcl_id_mask,
                  ace_entry->pcl_id_mask);

        pattern->common.pclId = ace_entry->pcl_id;
        mask->common.pclId = ace_entry->pcl_id_mask;
    }

    return TRUE;

}

static BOOL_T
DEVRM_RuleToEgrExtIpv6L2(
    DEVRM_AceEntry_T *ace_entry,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *maskPtr,
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *patternPtr)
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC *mask = &maskPtr->ruleEgrExtIpv6L2;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC *pattern = &patternPtr->ruleEgrExtIpv6L2;

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_SRCPORT)
    {
        UI32_T unit = 1;
        UI32_T module_id;
        UI32_T device_id;
        UI32_T phy_port;

        DEVRM_LOG("Qualify Source Port\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->srcport_data,
                  ace_entry->srcport_data,
                  ace_entry->srcport_mask,
                  ace_entry->srcport_mask);

        if (DEV_SWDRV_Logical2PhyDevicePortID(unit, ace_entry->srcport_data, &module_id, &device_id, &phy_port) == FALSE)
        {
            DEBUG_ERROR_MSG(0);
            return FALSE;
        }

        pattern->common.sourcePort = phy_port;
        mask->common.sourcePort = ace_entry->srcport_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACDA)
    {
        DEVRM_LOG("Qualify MAC DA\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->dstmac_data[0],
                  ace_entry->dstmac_data[1],
                  ace_entry->dstmac_data[2],
                  ace_entry->dstmac_data[3],
                  ace_entry->dstmac_data[4],
                  ace_entry->dstmac_data[5],

                  ace_entry->dstmac_mask[0],
                  ace_entry->dstmac_mask[1],
                  ace_entry->dstmac_mask[2],
                  ace_entry->dstmac_mask[3],
                  ace_entry->dstmac_mask[4],
                  ace_entry->dstmac_mask[5]);

        memcpy(&mask->macDa, ace_entry->dstmac_mask, sizeof(GT_ETHERADDR));
        memcpy(&pattern->macDa, ace_entry->dstmac_data, sizeof(GT_ETHERADDR));
        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_MACSA)
    {
        DEVRM_LOG("Qualify MAC SA\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->srcmac_data[0],
                  ace_entry->srcmac_data[1],
                  ace_entry->srcmac_data[2],
                  ace_entry->srcmac_data[3],
                  ace_entry->srcmac_data[4],
                  ace_entry->srcmac_data[5],

                  ace_entry->srcmac_mask[0],
                  ace_entry->srcmac_mask[1],
                  ace_entry->srcmac_mask[2],
                  ace_entry->srcmac_mask[3],
                  ace_entry->srcmac_mask[4],
                  ace_entry->srcmac_mask[5]);

        memcpy(&mask->macSa, ace_entry->srcmac_mask, sizeof(GT_ETHERADDR));
        memcpy(&pattern->macSa, ace_entry->srcmac_data, sizeof(GT_ETHERADDR));
        pattern->common.isL2Valid = 1;
        mask->common.isL2Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6EHHOPBYHOP)
    {
        DEVRM_LOG("Qualify IP6 Is Hop By Hop\r\n");

//        pattern->isIpv6HopByHop = 1;
//        mask->isIpv6HopByHop = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6EHEXIST)
    {
        DEVRM_LOG("Qualify IP6 Header Exist\r\n");

//        pattern->isIpv6ExtHdrExist = 1;
//        mask->isIpv6HopByHop = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6DIPUPPER)
    {
        DEVRM_LOG("Qualify IP6 DIP UPPER\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  ace_entry->dstip6_data[0],
                  ace_entry->dstip6_mask[0]);

        pattern->dipBits127to120 = ace_entry->dstip6_data[0];
        mask->dipBits127to120 = ace_entry->dstip6_mask[0];
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPV6SIP)
    {
        UI32_T i;

        DEVRM_LOG("Qualify IP6 SIP\r\n"
                  "    Data 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n"
                  "    Mask 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
                  ace_entry->srcip6_data[0],
                  ace_entry->srcip6_data[1],
                  ace_entry->srcip6_data[2],
                  ace_entry->srcip6_data[3],
                  ace_entry->srcip6_data[4],
                  ace_entry->srcip6_data[5],
                  ace_entry->srcip6_data[6],
                  ace_entry->srcip6_data[7],
                  ace_entry->srcip6_data[8],
                  ace_entry->srcip6_data[9],
                  ace_entry->srcip6_data[10],
                  ace_entry->srcip6_data[11],
                  ace_entry->srcip6_data[12],
                  ace_entry->srcip6_data[13],
                  ace_entry->srcip6_data[14],
                  ace_entry->srcip6_data[15],

                  ace_entry->srcip6_mask[0],
                  ace_entry->srcip6_mask[1],
                  ace_entry->srcip6_mask[2],
                  ace_entry->srcip6_mask[3],
                  ace_entry->srcip6_mask[4],
                  ace_entry->srcip6_mask[5],
                  ace_entry->srcip6_mask[6],
                  ace_entry->srcip6_mask[7],
                  ace_entry->srcip6_mask[8],
                  ace_entry->srcip6_mask[9],
                  ace_entry->srcip6_mask[10],
                  ace_entry->srcip6_mask[11],
                  ace_entry->srcip6_mask[12],
                  ace_entry->srcip6_mask[13],
                  ace_entry->srcip6_mask[14],
                  ace_entry->srcip6_mask[15]);

        /* EPR: ASE3526MP-FLF-EC-00024
         * Rule is not work if includes IPv6 address with prefix length
         */
        for (i=0; i<SYS_ADPT_IPV6_ADDR_LEN; ++i)
        {
            pattern->sip.arIP[i] = ace_entry->srcip6_data[i] & ace_entry->srcip6_mask[i];
        }

        /* memcpy(pattern->sip.arIP, ace_entry->srcip6_data, SYS_ADPT_IPV6_ADDR_LEN); */
        memcpy(mask->sip.arIP, ace_entry->srcip6_mask, SYS_ADPT_IPV6_ADDR_LEN);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_DSCP)
    {
        DEVRM_LOG("Qualify DSCP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->dscp_data,
                  ace_entry->dscp_data,
                  ace_entry->dscp_mask,
                  ace_entry->dscp_mask);

        pattern->commonExt.dscp = ace_entry->dscp_data;
        mask->commonExt.dscp = ace_entry->dscp_mask;
    }
    else
    {
        if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PREC)
        {
            DEVRM_LOG("Qualify Precedence\r\n"
                      "    Data 0x%02X(%u)\r\n"
                      "    Mask 0x%02X(%u)\r\n",
                      ace_entry->prec_data,
                      ace_entry->prec_data,
                      ace_entry->prec_mask,
                      ace_entry->prec_mask);

            pattern->commonExt.dscp = (ace_entry->prec_data & 0x7) << 3;
            mask->commonExt.dscp = (ace_entry->prec_mask & 0x7) << 3;

            DEVRM_LOG("@dscp\r\n"
                      "    Data 0x%02X\r\n"
                      "    Mask 0x%02X\r\n",
                      pattern->commonExt.dscp,
                      mask->commonExt.dscp);
        }
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VID)
    {
        DEVRM_LOG("Qualify VLAN ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_data,
                  ace_entry->outvlan_mask,
                  ace_entry->outvlan_mask);

        pattern->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_data & 0x0FFF);
        mask->common.vid = L_STDLIB_Hton16(ace_entry->outvlan_mask & 0x0FFF);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_UP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP)
    {
        DEVRM_LOG("Qualify UP\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_data,
                  ace_entry->vtagup_mask,
                  ace_entry->vtagup_mask);

        pattern->common.up = ace_entry->vtagup_data;
        mask->common.up = ace_entry->vtagup_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_VTAGUP ||
        ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TAGGED)
    {
        DEVRM_LOG("Qualify Tagged\r\n"
                  "    Data 0x%02X(%u)\r\n",
                  ace_entry->is_tagged,
                  ace_entry->is_tagged);

        pattern->common.isTagged = ace_entry->is_tagged;
        mask->common.isTagged = 0xFF;

        DEVRM_LOG("@isTagged\r\n"
                  "    Data 0x%02X\r\n"
                  "    Mask 0x%02X\r\n",
                  pattern->common.isTagged,
                  mask->common.isTagged);
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL)
    {
        DEVRM_LOG("Qualify IP Protocol\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_data,
                  ace_entry->ipprotocol_mask,
                  ace_entry->ipprotocol_mask);

        pattern->commonExt.ipProtocol = ace_entry->ipprotocol_data;
        mask->commonExt.ipProtocol = ace_entry->ipprotocol_mask;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4DSTPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_dstport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_dstport_mask);

        DEVRM_LOG("Qualify L4 Dest Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_dstport_data,
                  (UI16_T)ace_entry->l4_dstport_data,
                  (UI16_T)ace_entry->l4_dstport_mask,
                  (UI16_T)ace_entry->l4_dstport_mask);

        pattern->commonExt.l4Byte2 = ((unsigned char *)&tmp_data)[0];
        pattern->commonExt.l4Byte3 = ((unsigned char *)&tmp_data)[1];
        mask->commonExt.l4Byte2 = ((unsigned char *)&tmp_mask)[0];
        mask->commonExt.l4Byte3 = ((unsigned char *)&tmp_mask)[1];
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_L4SRCPORT)
    {
        UI16_T tmp_data = L_STDLIB_Hton16(ace_entry->l4_srcport_data);
        UI16_T tmp_mask = L_STDLIB_Hton16(ace_entry->l4_srcport_mask);

        DEVRM_LOG("Qualify L4 Source Port\r\n"
                  "    Data 0x%04hX(%hu)\r\n"
                  "    Mask 0x%04hX(%hu)\r\n",
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_data,
                  (UI16_T)ace_entry->l4_srcport_mask,
                  (UI16_T)ace_entry->l4_srcport_mask);

        pattern->commonExt.l4Byte0 = ((unsigned char *)&tmp_data)[0];
        pattern->commonExt.l4Byte1 = ((unsigned char *)&tmp_data)[1];
        mask->commonExt.l4Byte0 = ((unsigned char *)&tmp_mask)[0];
        mask->commonExt.l4Byte1 = ((unsigned char *)&tmp_mask)[1];
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_TCPCONTROL)
    {
        DEVRM_LOG("Qualify ICP Control\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->tcpcontrol_data,
                  ace_entry->tcpcontrol_data,
                  ace_entry->tcpcontrol_mask,
                  ace_entry->tcpcontrol_mask);

        pattern->commonExt.l4Byte13 = ace_entry->tcpcontrol_data;
        mask->commonExt.l4Byte13 = ace_entry->tcpcontrol_mask;
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE)
    {
        DEVRM_LOG("Qualify ICMP Type\r\n"
                  "    Data 0x%02X(%u)\r\n"
                  "    Mask 0x%02X(%u)\r\n",
                  ace_entry->icmptype_data,
                  ace_entry->icmptype_data,
                  ace_entry->icmptype_mask,
                  ace_entry->icmptype_mask);

        pattern->commonExt.l4Byte0 = ace_entry->icmptype_data;
        mask->commonExt.l4Byte0 = ace_entry->icmptype_mask;
        pattern->commonExt.isL4Valid = 1;
        mask->commonExt.isL4Valid = 0xFF;
    }

    if (ace_entry->entry_qualify & DEVRM_FIELD_QUALIFY_MASK_PCL_ID)
    {
        DEVRM_LOG("Qualify PCL ID\r\n"
                  "    Data 0x%04hX(%hd)\r\n"
                  "    Mask 0x%04hX(%hd)\r\n",
                  ace_entry->pcl_id,
                  ace_entry->pcl_id,
                  ace_entry->pcl_id_mask,
                  ace_entry->pcl_id_mask);

        pattern->common.pclId = ace_entry->pcl_id;
        mask->common.pclId = ace_entry->pcl_id_mask;
    }

    pattern->commonExt.isIpv6 = 1;
    mask->commonExt.isIpv6 = 0xFF;

    return TRUE;

}

static BOOL_T
DEVRM_SetupKeyTypes(
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pcl_type,
    CPSS_DXCH_PCL_LOOKUP_CFG_STC *lookupCfg)
{
    /* When pcl_type is belong to STD, then the search mode MUST use all STD type.
     * When pcl_type is belong to EXT, then the search mode MUST use all EXT type.
     */
    switch (pcl_type)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            lookupCfg->groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            lookupCfg->groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
            lookupCfg->groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            lookupCfg->groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            lookupCfg->groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
            lookupCfg->groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            lookupCfg->groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            lookupCfg->groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
            lookupCfg->groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
            lookupCfg->groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            lookupCfg->groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
            lookupCfg->groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            lookupCfg->groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
            lookupCfg->groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
            lookupCfg->groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            lookupCfg->groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
            lookupCfg->groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
            lookupCfg->groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            lookupCfg->groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
            lookupCfg->groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
            lookupCfg->groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
            lookupCfg->groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
            lookupCfg->groupKeyTypes.ipv4Key  = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
            lookupCfg->groupKeyTypes.ipv6Key  = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E;
            break;
        default:
            return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : DEVRM_HandleIPCReqMsg
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Handle the ipc request message for dev_rm.
 * INPUT:
 *    ipcmsg_p  --  input request ipc message buffer
 *
 * OUTPUT:
 *    ipcmsg_p  --  output response ipc message buffer
 *
 * RETURN:
 *    TRUE  - There is a response need to send.
 *    FALSE - There is no response to send.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_HandleIPCReqMsg(
    SYSFUN_Msg_T* ipcmsg_p)
{
    DEVRM_PMGR_IPCMSG_T   *dev_rm_ipcmsg;

    if (ipcmsg_p==NULL)
        return FALSE;

    dev_rm_ipcmsg = (DEVRM_PMGR_IPCMSG_T*)ipcmsg_p->msg_buf;

    switch (dev_rm_ipcmsg->type.cmd)
    {
        case DEVRM_IPCCMD_ALLOCATERULE:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(dev_rm_ipcmsg->data.allocaterule.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_AllocateRule(
                                dev_rm_ipcmsg->data.allocaterule.req.device_id,
                                dev_rm_ipcmsg->data.allocaterule.req.pcl_id,
                                dev_rm_ipcmsg->data.allocaterule.req.vir_group_id,
                                dev_rm_ipcmsg->data.allocaterule.req.rule_pri,
                                dev_rm_ipcmsg->data.allocaterule.req.dev_rule_id);
            break;

        case DEVRM_IPCCMD_SETRULE:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.setrule.resp);

            dev_rm_ipcmsg->type.result_bool = DEVRM_SetRule(
                                dev_rm_ipcmsg->data.setrule.req.device_id,
                                dev_rm_ipcmsg->data.setrule.req.group_id,
                                dev_rm_ipcmsg->data.setrule.req.dev_rule_id,
                                &dev_rm_ipcmsg->data.setrule.req.ace_entry,
                                &dev_rm_ipcmsg->data.setrule.req.action);
            break;

        case DEVRM_IPCCMD_DESTROYRULE:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.destroyrule.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_DestroyRule(
                                                    dev_rm_ipcmsg->data.destroyrule.req.device_id,
                                                    dev_rm_ipcmsg->data.destroyrule.req.pcl_id,
                                                    dev_rm_ipcmsg->data.destroyrule.req.dev_rule_id);
            break;

        case DEVRM_IPCCMD_UPDATEMETER:
            {
                ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.update_meter.resp);
                dev_rm_ipcmsg->type.result_bool = DEVRM_UpdateMeter(
                                                    dev_rm_ipcmsg->data.update_meter.req.device_id,
                                                    dev_rm_ipcmsg->data.update_meter.req.pcl_id,
                                                    dev_rm_ipcmsg->data.update_meter.req.dev_rule_id,
                                                    &dev_rm_ipcmsg->data.update_meter.req.dev_meter);
            }
            break;
        case DEVRM_IPCCMD_SHAREMETER:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.share_meter.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_ShareMeter(
                                                    dev_rm_ipcmsg->data.share_meter.req.device_id,
                                                    dev_rm_ipcmsg->data.share_meter.req.src_rule_id,
                                                    dev_rm_ipcmsg->data.share_meter.req.dst_rule_id);
            break;

        case DEVRM_IPCCMD_GETNUMBEROFCHIPS:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.getnumberofchips.resp);
            dev_rm_ipcmsg->type.result_ui32 = DEVRM_GetNumberOfChips();
            break;

        case DEVRM_IPCCMD_GET_DEVICE_INFOMATION:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.get_device_info.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_GetDeviceInfo(
                                                  &dev_rm_ipcmsg->data.get_device_info.resp.device_info);
            break;

        case DEVRM_IPCCMD_FIELDINIT:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.fieldinit.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_Initial();
            break;

        case DEVRM_IPCCMD_CREATE_PCL:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                 sizeof(dev_rm_ipcmsg->data.create_pcl.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_CreatePcl(
                                                  dev_rm_ipcmsg->data.create_pcl.req.device_id,
                                                  dev_rm_ipcmsg->data.create_pcl.req.pcl_id,
                                                  dev_rm_ipcmsg->data.create_pcl.req.is_ingress,
                                                  dev_rm_ipcmsg->data.create_pcl.req.lookup_num,
                                                  dev_rm_ipcmsg->data.create_pcl.req.rule_type);
            break;

        case DEVRM_IPCCMD_BIND_PCL_TO_PORT:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                 sizeof(dev_rm_ipcmsg->data.bind_pcl_to_port.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_BindPclToPort(
                                              dev_rm_ipcmsg->data.bind_pcl_to_port.req.device_id,
                                              dev_rm_ipcmsg->data.bind_pcl_to_port.req.phy_port,
                                              dev_rm_ipcmsg->data.bind_pcl_to_port.req.pcl_id);
            break;

        case DEVRM_IPCCMD_UNBIND_PCL_TO_PORT:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.bind_pcl_to_port.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_UnbindPclToPort(
                                                  dev_rm_ipcmsg->data.bind_pcl_to_port.req.device_id,
                                                  dev_rm_ipcmsg->data.bind_pcl_to_port.req.phy_port,
                                                  dev_rm_ipcmsg->data.bind_pcl_to_port.req.pcl_id);
            break;

        case DEVRM_IPCCMD_BIND_PCL_TO_TRUNK_PORT:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                 sizeof(dev_rm_ipcmsg->data.bind_pcl_to_trunk_port.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_BindPclToTrunkPort(
                                              dev_rm_ipcmsg->data.bind_pcl_to_trunk_port.req.device_id,
                                              dev_rm_ipcmsg->data.bind_pcl_to_trunk_port.req.trunk_id,
                                              dev_rm_ipcmsg->data.bind_pcl_to_trunk_port.req.pcl_id);
            break;

        case DEVRM_IPCCMD_UNBIND_PCL_TO_TRUNK_PORT:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.bind_pcl_to_trunk_port.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_UnbindPclToTrunkPort(
                                                 dev_rm_ipcmsg->data.bind_pcl_to_trunk_port.req.device_id,
                                                 dev_rm_ipcmsg->data.bind_pcl_to_trunk_port.req.trunk_id,
                                                 dev_rm_ipcmsg->data.bind_pcl_to_trunk_port.req.pcl_id);
            break;

        case DEVRM_IPCCMD_GET_COUNTER:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.get_counter.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_GetCounter(
                                                dev_rm_ipcmsg->data.get_counter.req.device_id,
                                                dev_rm_ipcmsg->data.get_counter.req.pcl_id,
                                                dev_rm_ipcmsg->data.get_counter.req.dev_rule_id,
                                                &dev_rm_ipcmsg->data.get_counter.resp.green_octets,
                                                &dev_rm_ipcmsg->data.get_counter.resp.yellow_octets,
                                                &dev_rm_ipcmsg->data.get_counter.resp.red_octets);
            break;

        case DEVRM_IPCCMD_CLEAR_COUNTER:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE;
            dev_rm_ipcmsg->type.result_bool = DEVRM_ClearCounter(
                                                dev_rm_ipcmsg->data.clear_counter.req.device_id,
                                                dev_rm_ipcmsg->data.clear_counter.req.pcl_id,
                                                dev_rm_ipcmsg->data.clear_counter.req.dev_rule_id);
            break;

        default:
            printf("%s(): Invalid cmd(%d).\n", __FUNCTION__, (int)(ipcmsg_p->cmd));
            dev_rm_ipcmsg->type.result_bool = DEVRM_PMGR_RESP_RESULT_FAIL;
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE;
            break;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : DEVRM_Translate_PCEIDwithType
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Translate our hw_idx to map CPSS hw_index
 * INPUT:
 *    type  --  pce type
 *    hw_id  --  our index, using a standard rule is an index
 *
 * OUTPUT:
 *    None
 *
 * RETURN:
 *    CPSS index
 *
 * NOTES:
 *    Because CPSS use 2 rules in an index with extended mode, so we need to
 *    mapping our index to CPSS's index. our index 2 means CPSS index 1 for
 *    extended mode.
 *------------------------------------------------------------------------------
 */
static UI32_T
DEVRM_Translate_PCEIDwithType(
    UI32_T type,
    UI32_T hw_id)
{
    UI32_T rule_size;

    switch (DEVRM_GetRuleSize(type))
    {
    case CPSS_PCL_RULE_SIZE_STD_E:
        rule_size = 1;
        break;
    case CPSS_PCL_RULE_SIZE_EXT_E:
        rule_size = 2;
        break;

    default:
        rule_size = 1;
        break;
    }

    /*printf("\n%s, %lu hw_id=%lu, cpss_id=%lu\n", __FUNCTION__, __LINE__, hw_id, hw_id/rule_size);*/
    return (hw_id/rule_size);
}

/*******************************************************************************
 *******************************************************************************
 ******************************************************************************/

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetPcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Get PCL
 * INPUT    : pcl_id    - PCL ID
 * OUTPUT   : NONE
 * RETURN   : Pointer of a PCL object
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_PCL_T *
DEVRM_GetPcl(
    UI32_T device_id,
    UI32_T pcl_id)
{
    DEVRM_CHECK_DEVICE_ID(device_id);

    DEVRM_PCL_T *pcl;

    if (0 == pcl_id || _countof(DEVRM_DEVICE_PTR(device_id)->pcls) < pcl_id)
    {
        return NULL;
    }

    pcl = &DEVRM_DEVICE_PTR(device_id)->pcls[pcl_id -1];

    return (pcl->is_used == TRUE) ? pcl : NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetVirtualGroupAtIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : Get virtual group by array index
 * INPUT    : pcl_id    - PCL ID
 *            idx       - array index
 * OUTPUT   : NONE
 * RETURN   : Pointer of a virtual group object
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_VirtualGroup_T *
DEVRM_GetVirtualGroupAtIndex(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T idx)
{
    DEVRM_PCL_T *pcl = DEVRM_GetPcl(device_id, pcl_id);

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (NULL == pcl)
    {
        return NULL;
    }

    if (_countof(DEVRM_DEVICE_PTR(device_id)->pcls->vg_tbl) <= idx)
    {
        return NULL;
    }

    return &pcl->vg_tbl[idx];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FindVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Find virtual group by ID
 * INPUT    : pcl_id        - PCL ID
 *            vir_group_id  - virtual group ID
 * OUTPUT   : NONE
 * RETURN   : Pointer of a virtual group object
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_VirtualGroup_T *
DEVRM_FindVirtualGroup(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id)
{
    DEVRM_PCL_T *pcl = DEVRM_GetPcl(device_id, pcl_id);
    DEVRM_VirtualGroup_T *vg;
    UI32_T i;

    if (FALSE == pcl)
    {
        return NULL;
    }

    for (i=0; i < _countof(pcl->vg_tbl); ++i)
    {
        vg = &pcl->vg_tbl[i];

        if (TRUE == vg->valid &&
            vir_group_id == vg->vir_group_id)
        {
            return vg;
        }
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FindPreviousVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Find previous of virtual group
 * INPUT    : pcl_id        - PCL ID
 *            vir_group_id  - virtual group ID
 * OUTPUT   : NONE
 * RETURN   : pointer of virtual group
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_VirtualGroup_T *
DEVRM_FindPreviousVirtualGroup(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id)
{
    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *prev_vg = NULL;
    DEVRM_VirtualGroup_T *vg;
    UI32_T id;
    UI32_T vid;

    DEVRM_CHECK_DEVICE_ID(device_id);

    for (id = 1; id <= pcl_id; ++id)
    {
        pcl = DEVRM_GetPcl(device_id, id);

        if (NULL == pcl)
        {
            continue;
        }

        for (vid=0; vid < _countof(DEVRM_DEVICE_PTR(device_id)->pcls->vg_tbl); ++vid)
        {
            vg = &pcl->vg_tbl[vid];
            if (vg->valid == TRUE)
            {
                if (id == pcl_id && vg->vir_group_id == vir_group_id)
                {
                    return prev_vg;
                }

                prev_vg = vg;
            }
        }
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CompareVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Compare two DEVRM_VirtualGroup objects
 * INPUT    : arg1  - object1
 *            arg2  - orject2
 * OUTPUT   : NONE
 * RETURN   : -1 if arg1 > arg2
 *             1 if arg2 > arg1
 *             0 if arg1 == arg2
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static int
DEVRM_CompareVirtualGroup(
    const void *arg1,
    const void *arg2)
{
    DEVRM_VirtualGroup_T *vg1 = (DEVRM_VirtualGroup_T *) arg1;
    DEVRM_VirtualGroup_T *vg2 = (DEVRM_VirtualGroup_T *) arg2;

    if (vg2->valid < vg1->valid)
    {
        return -1;
    }
    else if (vg1->valid < vg2->valid)
    {
        return 1;
    }

    if (vg2->pri_lvl < vg1->pri_lvl)
    {
        return -1;
    }
    else if (vg1->pri_lvl < vg2->pri_lvl)
    {
        return 1;
    }

    return 0;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_InsertVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Insert a virtual group to PCL
 * INPUT    : pcl_id        - PCL ID
 *            vir_group_id  - virtual group ID
 * OUTPUT   : NONE
 * RETURN   : TRUE/FALSE
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_VirtualGroup_T *
DEVRM_InsertVirtualGroup(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id)
{
    DEVRM_PCL_T *pcl = DEVRM_GetPcl(device_id, pcl_id);
    DEVRM_VirtualGroup_T *vg;
    UI32_T i;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (NULL == pcl)
    {
        return NULL;
    }

    vg = DEVRM_FindVirtualGroup(device_id, pcl_id, vir_group_id);
    if (NULL != vg)
    {
        return vg;
    }

    for (i=0; i < _countof(pcl->vg_tbl); ++i)
    {
        if (FALSE == pcl->vg_tbl[i].valid)
        {
            if (NULL == vg)
            {
                vg = &pcl->vg_tbl[i];
                break;
            }
        }
    }

    if (NULL == vg)
    {
        return NULL;
    }

    memset(vg, 0, sizeof(*vg));

    vg->pcl_id = pcl_id;
    vg->vir_group_id = vir_group_id;
    vg->pri_lvl = vir_group_id;
    vg->valid = TRUE;

    qsort(pcl->vg_tbl, _countof(pcl->vg_tbl), sizeof(pcl->vg_tbl[0]),
                                                    DEVRM_CompareVirtualGroup);

    vg = DEVRM_FindVirtualGroup(device_id, pcl_id, vir_group_id);

    return vg;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocatePceWithPriority
 *------------------------------------------------------------------------------
 * PURPOSE:  Allocate a PCE
 * INPUT:    pcl_id         - PCL ID
 *           vir_group_id   - Virtual group ID
 *           priority       - Rule priority
 *           pce_typ        - Rule type
 * OUTPUT:   pce            - PCE
 * RETURN:   TRUE/FALSE.
 * NOTE:     PCL must be valid.
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_AllocatePceWithPriority(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id,
    int priority,
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type,
    DEVRM_PCE_T **pce)
{
    enum
    {
        INVALID_HW_INDEX = 0xffff,

        MAX_HW_INDEX = DEVRM_MAX_NBR_OF_TCAM_ENTRIES,
    };

    typedef enum
    {
        SHIFT_TO_RIGHT,
        SHIFT_TO_LEFT
    } SHIFT_T;

    SHIFT_T shift = SHIFT_TO_RIGHT;

    UI16_T advise_hw_idx;
    UI16_T next_free_hw_idx;
    UI16_T prev_free_hw_idx;
    UI16_T fin_hw_idx;

    BOOL_T is_std_pce;
    BOOL_T ret;

    is_std_pce = DEVRM_GetRuleSize(pce_type) == CPSS_PCL_RULE_SIZE_STD_E ? TRUE : FALSE;

    ret = DEVRM_GetHwIndex(device_id, pcl_id, vir_group_id, priority, is_std_pce, &advise_hw_idx);
    if (FALSE == ret)
    {
        DEVRM_LOG("<Error> Failed to get advised hardware index");
        return FALSE;
    }

    next_free_hw_idx = DEVRM_FindEmptyHwIndex(device_id, advise_hw_idx, is_std_pce);
    prev_free_hw_idx = DEVRM_FindEmptyHwIndexReverse(device_id, advise_hw_idx, is_std_pce);

    if (INVALID_HW_INDEX == next_free_hw_idx &&
        INVALID_HW_INDEX == prev_free_hw_idx)
    {
        DEVRM_LOG("<Error> No resource");
        return FALSE;
    }

    if (INVALID_HW_INDEX == prev_free_hw_idx)
    {
        shift = SHIFT_TO_RIGHT;
    }
    else if (INVALID_HW_INDEX == next_free_hw_idx)
    {
        shift = SHIFT_TO_LEFT;
    }
    else
    {
        if ( (next_free_hw_idx - advise_hw_idx) <=
            (advise_hw_idx - prev_free_hw_idx) )
        {
            shift = SHIFT_TO_RIGHT;
        }
        else
        {
            shift = SHIFT_TO_LEFT;
        }
    }

    if (SHIFT_TO_RIGHT == shift)
    {
        ret = DEVRM_ShiftPceToRight(device_id, advise_hw_idx, is_std_pce);
        if (FALSE == ret)
        {
            DEVRM_LOG("<Error> Why ?");
            return FALSE;
        }

        fin_hw_idx = advise_hw_idx;
    }
    else
    {
        ret = DEVRM_ShiftPceToLeft(device_id, advise_hw_idx, is_std_pce);
        if (FALSE == ret)
        {
            DEVRM_LOG("<Error> Why ?");
            return FALSE;
        }

        fin_hw_idx = DEVRM_FindEmptyHwIndexReverse(device_id, advise_hw_idx, is_std_pce);
    }

    ret = DEVRM_AssignPceAtHwIndex(device_id, fin_hw_idx, pce_type);
    ASSERT(TRUE == ret);

    *pce = DEVRM_MallocPce(pcl_id, vir_group_id, fin_hw_idx, pce_type, priority);
    if (NULL == pce)
    {
        DEVRM_LOG("<Error> Out of memory");
        goto free_resource;
    }

    ret = DEVRM_PLL_InsertPce(device_id, pcl_id, vir_group_id, *pce);
    if (FALSE == ret)
    {
        DEVRM_LOG("<Error> Insert PCE failed !?");
        goto free_resource;
    }

    DEVRM_LOG("New rule(device_id=%lu, pcl_id=%lu, vgid=%lu, rule_pri=%d) At HwIdx=%hu",
              device_id, pcl_id, vir_group_id, priority, fin_hw_idx);

    DEVRM_VALIDATE_RULE();

    return TRUE;

free_resource:
    DEVRM_SweepPceAtHwIndex(device_id, fin_hw_idx, pce_type);
    DEVRM_MfreePce(*pce);

    *pce = NULL;

    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_MoveRuleToNewHwIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a hardware index for new rule
 * INPUT:    pcl_id         - PCL ID
 *           vir_group_id   - Virtual group ID
 *           priority       - Priority of rule
 *           is_std_pce     - 0 for std, 1 for ext
 * OUTPUT:   hw_idx
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_MoveRuleToNewHwIndex(
    UI32_T device_id,
    DEVRM_PCE_T *pce,
    UI16_T hw_idx)
{
    CPSS_PCL_RULE_SIZE_ENT  ruleSize;
    GT_U32                  ruleIndex;
    GT_BOOL                 valid;
    CPSS_PCL_RULE_SIZE_ENT  ruleSizeGet;
    GT_STATUS               rc;

    BOOL_T ret;

    DEVRM_CHECK_DEVICE_ID(device_id);

    ASSERT(NULL != pce);

    ruleSize = (pce->is_std_pce == TRUE) ? CPSS_PCL_RULE_SIZE_STD_E :
                                                    CPSS_PCL_RULE_SIZE_EXT_E;
    ruleIndex = DEVRM_Translate_PCEIDwithType(pce->pce_type, pce->hw_idx);

    rc = cpssDxChPclRuleAnyStateGet(device_id, ruleSize, ruleIndex, &valid, &ruleSizeGet);

    if (GT_OK == rc && GT_TRUE == valid)
    {
        if (ruleSizeGet != ruleSize)
        {
            ASSERT(0);
            return FALSE;
        }

        rc = cpssDxChPclRuleCopy(device_id, ruleSize, ruleIndex,
                                 DEVRM_Translate_PCEIDwithType(pce->pce_type, hw_idx));
        if (GT_OK != rc)
        {
            return FALSE;
        }

        rc = cpssDxChPclRuleInvalidate(device_id, ruleSize, ruleIndex);

        if (GT_OK != rc)
        {
            return FALSE;
        }
    }

    ret = DEVRM_SweepPceAtHwIndex(device_id, pce->hw_idx, pce->pce_type);
    if (TRUE != ret)
    {
        ASSERT(0);
        return FALSE;
    }

    ret = DEVRM_AssignPceAtHwIndex(device_id, hw_idx, pce->pce_type);
    ASSERT(TRUE == ret);
    if (TRUE != ret)
    {
        return FALSE;
    }

    pce->hw_idx = hw_idx;
    pce->extended_hw_idx = hw_idx;

    if (pce->is_std_pce == FALSE)
    {
        pce->next->hw_idx = hw_idx+1;
        pce->next->extended_hw_idx = hw_idx;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PLL_InsertPce
 *------------------------------------------------------------------------------
 * PURPOSE:  Insert a PCE into PCL Linked List
 * INPUT:    pcl_id         - PCL ID
 *           vir_group_id   - Virtual group ID
 *           pce            - PCE list
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_PLL_InsertPce(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id,
    DEVRM_PCE_T *pce)
{
    DEVRM_PCL_T *pcl = DEVRM_GetPcl(device_id, pcl_id);
    DEVRM_PCE_T *last;
    DEVRM_PCE_T *pp;
    DEVRM_VirtualGroup_T *vg;
    UI32_T pce_number;

    DEVRM_CHECK_DEVICE_ID(device_id);

    ASSERT(NULL != pce);

    if (NULL == pcl)
    {
        return FALSE;
    }

    vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, vir_group_id);
    if (NULL == vg)
    {
        return FALSE;
    }

    for (last = pce, pce_number = 1; NULL != last->next;
                                            last = last->next, ++pce_number)
    {}

    if (NULL == vg->rv.pce_list)
    {
        vg->rv.pce_list = pce;
        vg->rv.last_pce = last;
    }
    else
    {
        pp = vg->rv.pce_list;

        do
        {
            if ((pp->pcl_id != pcl_id) ||
                (pp->vir_group_id != vir_group_id))
            {
                pp = NULL;
                break;
            }

            if (pce->hw_idx < pp->hw_idx)
            {
                break;
            }

        } while (TRUE == DEVRM_GetNextPce(device_id, &pp));

        if (NULL == pp)
        {
            /* Append to tail
             */
            vg->rv.last_pce->next = pce;
            pce->prev = vg->rv.last_pce;

            vg->rv.last_pce = last;
        }
        else
        {
            /* Insert font of pp
             */
            if (vg->rv.pce_list == pp)
            {
                pp->prev = last;
                last->next = pp;

                vg->rv.pce_list = pce;
            }
            else
            {
                pp->prev->next = pce;
                pce->prev = pp->prev;

                pp->prev = last;
                last->next = pp;
            }
        }
    }

    DEVRM_PLL_Update(device_id, pcl_id);

    pcl->pce_number += pce_number;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PLL_RemovePce
 *------------------------------------------------------------------------------
 * PURPOSE:  Remove a PCE from PCL Linked List
 * INPUT:    pce            - PCE entry
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_PLL_RemovePce(
    UI32_T device_id,
    DEVRM_PCE_T *pce)
{
    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;

    DEVRM_CHECK_DEVICE_ID(device_id);

    ASSERT(NULL != pce);

    pcl = DEVRM_GetPcl(device_id, pce->pcl_id);
    vg = DEVRM_FindVirtualGroup(device_id, pce->pcl_id, pce->vir_group_id);

    ASSERT(NULL != pcl);
    ASSERT(NULL != vg);

    if (TRUE == pce->is_std_pce)
    {
        if (NULL != pce->prev)
        {
            pce->prev->next = pce->next;
        }

        if (NULL != pce->next)
        {
            pce->next->prev = pce->prev;
        }

        if (vg->rv.pce_list == pce)
        {
            vg->rv.pce_list = (pce->next &&
                               pce->next->vir_group_id == pce->vir_group_id) ?
                                                            pce->next : NULL;
        }

        if (vg->rv.last_pce == pce)
        {
            vg->rv.last_pce = (pce->prev &&
                               pce->prev->vir_group_id == pce->vir_group_id) ?
                                                    pce->prev : vg->rv.pce_list;
        }

        pce->prev = NULL;
        pce->next = NULL;
    }
    else
    {
        ASSERT(pce->next != NULL &&
               pce->extended_hw_idx == pce->next->extended_hw_idx);

        if (NULL != pce->prev)
        {
            pce->prev->next = pce->next->next;
        }

        if (NULL != pce->next->next)
        {
            pce->next->next->prev = pce->prev;
        }

        if (vg->rv.pce_list == pce)
        {
            vg->rv.pce_list = (pce->next->next &&
                               pce->next->next->vir_group_id == pce->vir_group_id) ?
                                                        pce->next->next : NULL;
        }

        if (vg->rv.last_pce == pce->next)
        {
            vg->rv.last_pce = (pce->prev &&
                               pce->prev->vir_group_id == pce->vir_group_id) ?
                                                        pce->prev : vg->rv.pce_list;
        }

        pce->prev = NULL;
        pce->next->next = NULL;
    }

    DEVRM_PLL_Update(device_id, pce->pcl_id);

    pcl->pce_number -= ((pce->is_std_pce) ? 1 : 2);

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PLL_Update
 *------------------------------------------------------------------------------
 * PURPOSE:  Link all virtual group rule pointer together.
 * INPUT:    pcl_id         - PCL ID
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE.
 * NOTE:     PCL must be valid.
 *------------------------------------------------------------------------------
 */
static void
DEVRM_PLL_Update(
    UI32_T device_id,
    UI32_T pcl_id)
{
    DEVRM_PCL_T *pcl = DEVRM_GetPcl(device_id, pcl_id);
    UI32_T i;
    DEVRM_VirtualGroup_T *vg;

    DEVRM_PCE_T *head = NULL;
    DEVRM_PCE_T *tail = NULL;

    DEVRM_PCE_T *left_head = NULL;
    DEVRM_PCE_T *left_tail = NULL;

    DEVRM_PCE_T *right_head = NULL;
    DEVRM_PCE_T *right_tail = NULL;

    DEVRM_CHECK_DEVICE_ID(device_id);

    ASSERT(NULL != pcl);

    for (i=0; i < _countof(pcl->vg_tbl); ++i)
    {
        vg = &pcl->vg_tbl[i];
        if (FALSE == vg->valid)
        {
            continue;
        }

        if (NULL == vg->rv.pce_list)
        {
            continue;
        }

        if (NULL == head)
        {
            head = vg->rv.pce_list;
        }

        tail = vg->rv.last_pce;

        if (NULL == left_head)
        {
            left_head = vg->rv.pce_list;
            left_tail = vg->rv.last_pce;
        }
        else if (NULL == right_head)
        {
            right_head = vg->rv.pce_list;
            right_tail = vg->rv.last_pce;
        }
        else
        {
            ASSERT(0);
        }

        if (NULL != left_head && NULL != right_head)
        {
            left_tail->next = right_head;
            right_head->prev = left_tail;

            left_head = right_head;
            left_tail = right_tail;

            right_head = NULL;
            right_tail = NULL;
        }
    }

    pcl->pce_list = head;
    pcl->last_pce = tail;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AssignPceAtHwIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Allocate a PCE from TCAM
 * INPUT:    hw_idx         - Hardware index
 *           is_std_pce     - The type of PCE
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_AssignPceAtHwIndex(
    UI32_T device_id,
    UI32_T hw_idx,
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type)
{
    BOOL_T is_std_pce;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (DEVRM_MAX_NBR_OF_TCAM_ENTRIES <= hw_idx)
    {
        DEVRM_LOG("<Error> Invalid parameter. hw_idx=%lu", hw_idx);
        return FALSE;
    }

    is_std_pce = (CPSS_PCL_RULE_SIZE_STD_E == DEVRM_GetRuleSize(pce_type)) ?
                                                                TRUE : FALSE;

    if (is_std_pce)
    {
        if ( 0 != ( DEVRM_DEVICE_PTR(device_id)->pces_use_status[hw_idx/8] & (0x80 >> (hw_idx % 8)) ) )
        {
            return FALSE;
        }

        DEVRM_DEVICE_PTR(device_id)->pces_use_status[hw_idx/8] |= ( 0X80 >> (hw_idx % 8) );
    }
    else
    {
        if (1 == (hw_idx % 2))
        {
            DEVRM_LOG("<Error> Invalid parameter. hw_idx=%lu", hw_idx);
            return FALSE;
        }

        if ( 0 != ( DEVRM_DEVICE_PTR(device_id)->pces_use_status[hw_idx/8] & (0xC0 >> (hw_idx % 8)) ) )
        {
            return FALSE;
        }

        DEVRM_DEVICE_PTR(device_id)->pces_use_status[hw_idx/8] |= ( 0XC0 >> (hw_idx % 8) );
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SweepPceAtHwIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Sweep a PCE from TCAM
 * INPUT:    hw_idx         - Hardware index
 *           pce_type       - The type of PCE
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_SweepPceAtHwIndex(
    UI32_T device_id,
    UI32_T hw_idx,
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type)
{
    BOOL_T is_std_pce;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (DEVRM_MAX_NBR_OF_TCAM_ENTRIES <= hw_idx)
    {
        DEVRM_LOG("Invalid parameter. hw_idx=%lu", hw_idx);
        return FALSE;
    }

    is_std_pce = (CPSS_PCL_RULE_SIZE_STD_E == DEVRM_GetRuleSize(pce_type)) ?
                                                                TRUE : FALSE;

    if (is_std_pce)
    {

        if ( 0 == ( DEVRM_DEVICE_PTR(device_id)->pces_use_status[hw_idx/8] & (0x80 >> (hw_idx % 8)) ) )
        {
            return FALSE;
        }

        DEVRM_DEVICE_PTR(device_id)->pces_use_status[hw_idx/8] &= ~( 0X80 >> (hw_idx % 8) );
    }
    else
    {
        if (1 == (hw_idx % 2))
        {
            DEVRM_LOG("Invalid parameter. hw_idx=%lu", hw_idx);
            return FALSE;
        }

        if ( 0 == ( DEVRM_DEVICE_PTR(device_id)->pces_use_status[hw_idx/8] & (0xC0 >> (hw_idx % 8)) ) )
        {
            return FALSE;
        }

        DEVRM_DEVICE_PTR(device_id)->pces_use_status[hw_idx/8] &= ~( 0XC0 >> (hw_idx % 8) );
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_MallocPce
 *------------------------------------------------------------------------------
 * PURPOSE:  Allocate memory for PCE
 * INPUT:    pcl_id         - PCL ID
 *           hw_idx         - Hardware index
 *           pce_type       - The type of PCE
 * OUTPUT:   None.
 * RETURN:   Pointer of PCE; NULL
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static DEVRM_PCE_T *
DEVRM_MallocPce(
    UI32_T pcl_id,
    UI32_T vir_group_id,
    UI32_T hw_idx,
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT pce_type,
    int priority)
{
    DEVRM_PCE_T *pce = NULL;
    BOOL_T is_std_pce;

    is_std_pce = (CPSS_PCL_RULE_SIZE_STD_E == DEVRM_GetRuleSize(pce_type)) ?
                                                                TRUE : FALSE;

    if (is_std_pce) /* just allocate one PCE entry */
    {
        pce = (DEVRM_PCE_T *) calloc(1, sizeof(*pce));
        if (NULL == pce)
        {
            return NULL;
        }

        DEVRM_LOG("Allocate memory at: %p\n", pce);

        pce->next = NULL;

        pce->hw_idx = hw_idx;
        pce->extended_hw_idx = hw_idx;
        pce->is_std_pce = is_std_pce;
        pce->pce_type = pce_type;

        pce->pcl_id = pcl_id;
        pce->vir_group_id = vir_group_id;

        pce->priority = priority;

        pce->policer_idx = -1;
        pce->qos_remarking_idx = -1;

    }
    else /* for extend, we need two PCEs*/
    {
        pce = (DEVRM_PCE_T *) calloc(1, sizeof(*pce));
        if (NULL == pce)
        {
            return NULL;
        }

        pce->next = (DEVRM_PCE_T *) calloc(1, sizeof(*pce));
        if (NULL == pce->next)
        {
            free(pce);
            return NULL;
        }

        DEVRM_LOG("Allocate memory at: %p\n", pce);
        DEVRM_LOG("Allocate memory at: %p\n", pce->next);

        pce->next->prev = pce;
        pce->next->next = NULL;

        /* assign value for the 1st PCE
         */
        pce->hw_idx = hw_idx;
        pce->extended_hw_idx = hw_idx;
        pce->is_std_pce = is_std_pce;
        pce->pce_type = pce_type;

        pce->pcl_id = pcl_id;
        pce->vir_group_id = vir_group_id;

        pce->priority = priority;

        pce->policer_idx = -1;
        pce->qos_remarking_idx = -1;

        /* assign value for the 2nd PCE
         */
        pce->next->hw_idx = hw_idx + 1;     /* the hw index will be continuous with 1st PCE */
        pce->next->extended_hw_idx = hw_idx;
        pce->next->is_std_pce = is_std_pce;
        pce->next->pce_type = pce_type;

        pce->next->pcl_id = pcl_id;
        pce->next->vir_group_id = vir_group_id;

        pce->next->priority = priority;

        pce->next->policer_idx = -1;
        pce->next->qos_remarking_idx = -1;
    }

    return pce;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_MfreePce
 *------------------------------------------------------------------------------
 * PURPOSE:  Free memory for PCE
 * INPUT:    pce            - PCE object
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static void
DEVRM_MfreePce(
    DEVRM_PCE_T *pce)
{
    while (pce)
    {
        DEVRM_PCE_T *pp = pce->next;

        DEVRM_LOG("Free memory at: %p\n", pce);

        free(pce);

        pce = pp;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetHwIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Get a hardware index for new rule
 * INPUT:    pcl_id         - PCL ID
 *           vir_group_id   - Virtual group ID
 *           priority       - Priority of rule
 *           is_std_pce     - 0 for std, 1 for ext
 * OUTPUT:   hw_idx
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_GetHwIndex(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id,
    int priority,
    BOOL_T is_std_pce,
    UI16_T *hw_idx)
{
    DEVRM_PCL_T *pcl;
    DEVRM_VirtualGroup_T *vg;

    DEVRM_CHECK_DEVICE_ID(device_id);

    ASSERT(NULL != hw_idx);

    pcl = DEVRM_GetPcl(device_id, pcl_id);
    if (NULL == pcl)
    {
        return FALSE;
    }

    *hw_idx = 0xffff;

    vg = DEVRM_FindVirtualGroup(device_id, pcl_id, vir_group_id);
    if (NULL == vg)
    {
        vg = DEVRM_InsertVirtualGroup(device_id, pcl_id, vir_group_id);
        if (NULL == vg)
        {
            return FALSE;
        }
    }

    if (NULL == vg->rv.pce_list)
    {
        DEVRM_VirtualGroup_T *prev_vg;

        prev_vg = DEVRM_FindPreviousVirtualGroup(device_id, pcl_id, vir_group_id);

        while (prev_vg && NULL == prev_vg->rv.last_pce)
        {
            prev_vg = DEVRM_FindPreviousVirtualGroup(device_id, prev_vg->pcl_id,
                                                     prev_vg->vir_group_id);
        }

        if (NULL == prev_vg)
        {
            *hw_idx = 0;
        }
        else
        {
            *hw_idx = prev_vg->rv.last_pce->hw_idx + 1;
        }
    }
    else
    {
        DEVRM_PCE_T *pce = vg->rv.pce_list;
        DEVRM_PCE_T *candidate = NULL;

        do
        {
            if ((pce->pcl_id != pcl_id) ||
                (pce->vir_group_id != vir_group_id))
            {
                break;
            }

            if (priority <= pce->priority)
            {
                candidate = pce;
            }
            else if (pce->priority < priority)
            {
                break;
            }

        } while ( DEVRM_GetNextPce(device_id, &pce) == TRUE );

        if (candidate)
        {
            pce = candidate;
        }
        else
        {
            pce = NULL;
        }

        if (NULL != pce)
        {
            *hw_idx = pce->hw_idx + (TRUE == pce->is_std_pce ? 1 : 2);
        }
        else
        {
            *hw_idx = vg->rv.pce_list->hw_idx;
        }
    }

    /* make sure hw_idx for extened rule is even number
     */
    if (FALSE == is_std_pce)
    {
        *hw_idx += (*hw_idx % 2);
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FindEmptyHwIndex
 *------------------------------------------------------------------------------
 * PURPOSE:  Find an empty hardware index for rule.
 * INPUT:    hw_idx         - Shift rule to right from this index
 *           is_std_pce     - Request rule size. TRUE means request 1 size.
 *                                               FALSE means request 2 size.
 * OUTPUT:   None
 * RETURN:   hardware index; 0xffff if failed
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static UI16_T
DEVRM_FindEmptyHwIndex(
    UI32_T device_id,
    UI16_T hw_idx,
    BOOL_T is_std_pce)
{
    DEVRM_PCE_T *r_pce;
    DEVRM_PCE_T *l_pce;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (1 == (hw_idx % 2) && (FALSE == is_std_pce))
    {
        return 0xffff;
    }

    if (DEVRM_MAX_NBR_OF_TCAM_ENTRIES <= hw_idx)
    {
        return 0xffff;
    }

    if (1 == (hw_idx % 2))
    {
        DEVRM_GetPceAtHwIndex(device_id, hw_idx, &r_pce);

        if (NULL == r_pce)
        {
            return hw_idx;
        }

        hw_idx += 1;

        if (r_pce && FALSE == r_pce->is_std_pce)
        {
            is_std_pce = FALSE;
        }
    }

find_empty_hw_index:

    if ( hw_idx < DEVRM_MAX_NBR_OF_TCAM_ENTRIES )
    {
        hw_idx -= (hw_idx % 2);

        DEVRM_GetPceAtHwIndex(device_id, hw_idx, &l_pce);
        DEVRM_GetPceAtHwIndex(device_id, hw_idx+1, &r_pce);

        if (TRUE == is_std_pce)
        {
            if (NULL == r_pce || NULL == l_pce)
            {
                if (l_pce && NULL == r_pce)
                {
                    return hw_idx + 1;
                }

                return hw_idx;
            }

            /* This space have a extended width rule
             */
            if (r_pce && FALSE == r_pce->is_std_pce)
            {
                /* return DEVRM_FindEmptyHwIndex(hw_idx+2, FALSE);
                 */

                hw_idx += 2;
                is_std_pce = FALSE;

                goto find_empty_hw_index;
            }

            /* This space have two standard width rule.
             * We can move ONE RULE out to satisfy the request. So we need
             * more space for the rule.
             */

            /* return DEVRM_FindEmptyHwIndex(hw_idx+2, TRUE);
             */

            hw_idx += 2;
            is_std_pce = TRUE;

            goto find_empty_hw_index;
        }
        else
        {
            if (NULL == r_pce && NULL == l_pce)
            {
                return hw_idx;
            }

            if ((r_pce && FALSE == r_pce->is_std_pce) ||
                (r_pce && l_pce))
            {
                /* return DEVRM_FindEmptyHwIndex(hw_idx+2, FALSE);
                 */

                hw_idx += 2;
                is_std_pce = FALSE;

                goto find_empty_hw_index;
            }

            /* This space have ONE standard rule only.
             * We can move this rule out THEN this entr will satisfy
             * the request.
             */

            /* return DEVRM_FindEmptyHwIndex(hw_idx+2, TRUE);
             */

            hw_idx += 2;
            is_std_pce = TRUE;

            goto find_empty_hw_index;
        }
    }

    return 0xffff;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FindEmptyHwIndexReverse
 *------------------------------------------------------------------------------
 * PURPOSE:  Reversely find an empty hardware index for rule.
 * INPUT:    hw_idx         - Shift rule to right from this index
 *           is_std_pce     - Request rule size. TRUE means request 1 size.
 *                                               FALSE means request 2 size.
 * OUTPUT:   None
 * RETURN:   hardware index; 0xffff if failed
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static UI16_T
DEVRM_FindEmptyHwIndexReverse(
    UI32_T device_id,
    UI16_T hw_idx,
    BOOL_T is_std_pce)
{
    DEVRM_PCE_T *r_pce;
    DEVRM_PCE_T *l_pce;

    DEVRM_CHECK_DEVICE_ID(device_id);

    if (1 == (hw_idx % 2) && (FALSE == is_std_pce))
    {
        return 0xffff;
    }

    if (0 == hw_idx)
    {
        return 0xffff;
    }

    /* If a hardware index over than MAX_HW_INDEX, it means it want to add
     * a lowest priority rule.
     */
    if (DEVRM_MAX_NBR_OF_TCAM_ENTRIES <= hw_idx)
    {
        hw_idx = DEVRM_MAX_NBR_OF_TCAM_ENTRIES;
    }

    if (1 == (hw_idx % 2))
    {
        DEVRM_GetPceAtHwIndex(device_id, hw_idx-1, &l_pce);

        if (NULL == l_pce)
        {
            return hw_idx - 1;
        }

        hw_idx -= 1;

        if (l_pce && FALSE == l_pce->is_std_pce)
        {
            is_std_pce = FALSE;
        }
    }

    while ( 0 != hw_idx )
    {
        hw_idx -= (hw_idx % 2);

        DEVRM_GetPceAtHwIndex(device_id, hw_idx - 2, &l_pce);
        DEVRM_GetPceAtHwIndex(device_id, hw_idx - 1, &r_pce);

        if (TRUE == is_std_pce)
        {
            if (NULL == r_pce || NULL == l_pce)
            {
                if (l_pce && NULL == r_pce)
                {
                    return hw_idx - 1;
                }

                return hw_idx - 2;
            }

            /* This space have a extended width rule
             */
            if (r_pce && FALSE == r_pce->is_std_pce)
            {
                is_std_pce = FALSE;
            }
            else
            {
                /* This space have TWO standard width rule.
                 * We can move ONE RULE out to satisfy the request. So we need
                 * more space for the rule.
                 */
                is_std_pce = TRUE;
            }
        }
        else
        {
            if (NULL == r_pce && NULL == l_pce)
            {
                return hw_idx - 2;
            }

            if ((r_pce && FALSE == r_pce->is_std_pce) ||
                (r_pce && l_pce && r_pce != l_pce))
            {
                is_std_pce = FALSE;
            }
            else
            {
                /* This space have ONE standard rule only.
                 * We can move this rule out THEN this entr will satisfy
                 * the request.
                 */
                is_std_pce = TRUE;
            }
        }

        hw_idx -= 2;
    }

    return 0xffff;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ShiftPceToRight
 *------------------------------------------------------------------------------
 * PURPOSE:  Shift rule to right from hw_idx
 * INPUT:    hw_idx         - Shift rule to right from this index
 *           is_std_pce     - Request rule size. TRUE means request 1 size.
 *                                               FALSE means request 2 size.
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_ShiftPceToRight(
    UI32_T device_id,
    UI16_T hw_idx,
    BOOL_T is_std_pce)
{
    DEVRM_PCE_T *pce;
    UI16_T idx;

    DEVRM_CHECK_DEVICE_ID(device_id);

    idx = DEVRM_FindEmptyHwIndex(device_id, hw_idx, is_std_pce);
    if (0xffff == idx)
    {
        return FALSE;
    }

    ASSERT(hw_idx <= idx);

    /* No need to shift
     */
    if (hw_idx == idx)
    {
        return TRUE;
    }

    if (FALSE == DEVRM_GetPceAtHwIndex(device_id, idx-1, &pce))
    {
        if (FALSE == DEVRM_GetPceAtHwIndex(device_id, idx-2, &pce))
        {
            return FALSE;
        }
    }

    do {
        UI16_T adjust;
        UI16_T new_hw_idx;

        adjust = 0;

        /* If current rule is standard rule. The offset between current HW
         * index to new HW index is dependent on whether exist an previous
         * extended rule.
         */
        if (TRUE == pce->is_std_pce)
        {
            if (is_std_pce == FALSE)
            {
                adjust += 1;
            }
            else
            {
                DEVRM_PCE_T *prev_pce = pce;

                while (TRUE == DEVRM_GetPreviousPce(device_id, &prev_pce))
                {
                    if (hw_idx <= prev_pce->hw_idx)
                    {
                        if (FALSE == prev_pce->is_std_pce)
                        {
                            adjust += 1;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        new_hw_idx = pce->hw_idx + (TRUE == pce->is_std_pce ? 1 : 2) + adjust;

        if (FALSE == DEVRM_MoveRuleToNewHwIndex(device_id, pce, new_hw_idx) )
        {
            DEVRM_LOG("DEVRM_MoveRuleToNewHwIndex failed."
                      " orig_hw_idx=%hu, new_hw_idx=%hu", pce->hw_idx, new_hw_idx);
            return FALSE;
        }

        DEVRM_GetPreviousPce(device_id, &pce);

    } while ( NULL != pce && hw_idx <= pce->hw_idx );

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ShiftPceToLeft
 *------------------------------------------------------------------------------
 * PURPOSE:  Shift rule to left from hw_idx
 * INPUT:    hw_idx         - Shift rule to right from this index
 *           is_std_pce     - Shift size. TRUE means shift 1 size.
 *                                        FALSE means shift 2 size.
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE.
 * NOTE:     The caller should make sure whether the position of hw_idx or
 *           hw_idx + 1 have been used or not before call this function.
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_ShiftPceToLeft(
    UI32_T device_id,
    UI16_T hw_idx,
    BOOL_T is_std_pce)
{
    UI16_T idx;
    DEVRM_PCE_T *pce;

    DEVRM_CHECK_DEVICE_ID(device_id);

    idx = DEVRM_FindEmptyHwIndexReverse(device_id, hw_idx, is_std_pce);
    if (0xffff == idx)
    {
        return FALSE;
    }

    /* adjust hw_idx to sensible value
     */
    if (DEVRM_MAX_NBR_OF_TCAM_ENTRIES <= hw_idx)
    {
        hw_idx = DEVRM_MAX_NBR_OF_TCAM_ENTRIES;
    }

    ASSERT(idx <= hw_idx);

    if (TRUE == is_std_pce)
    {
        if ((0 == (idx % 2)) && (idx == hw_idx-2))
        {
            /* There have two cases that found an even hardware index for standard
             * rule.
             * 1. The idx and idx+1 are available both.
             * 2. The idx is available but idx+1 is not. So idx+1 can move into idx.
             *
             * We just can skip shift by case 1.
             *
             */

            DEVRM_PCE_T *r_pce;
            DEVRM_GetPceAtHwIndex(device_id, idx+1, &r_pce);

            if (NULL == r_pce)
            {
                return TRUE;
            }
        }
        else if (idx == (hw_idx - 1))
        {
            return TRUE;
        }
    }
    else
    {
        if (idx == hw_idx-2)
        {
            return TRUE;
        }
    }

    /* For shifting left, max offset is 3
     */
    if (FALSE == DEVRM_GetPceAtHwIndex(device_id, idx+1, &pce))
    {
        if (FALSE == DEVRM_GetPceAtHwIndex(device_id, idx+2, &pce))
        {
            if (FALSE == DEVRM_GetPceAtHwIndex(device_id, idx+3, &pce))
            {
                return FALSE;
            }
        }
    }

    do {
        UI16_T adjust;
        UI16_T new_hw_idx;

        adjust = 0;

        /* If current rule is standard rule. The offset between current HW
         * index to new HW index is dependent on whether exist an previous
         * extended rule.
         */
        if (TRUE == pce->is_std_pce)
        {
            if (is_std_pce == FALSE)
            {
                adjust += 1;
            }
            else
            {
                DEVRM_PCE_T *next_pce = pce;

                while (TRUE == DEVRM_GetNextPce(device_id, &next_pce))
                {
                    if (next_pce->hw_idx < hw_idx)
                    {
                        if (FALSE == next_pce->is_std_pce)
                        {
                            adjust += 1;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        new_hw_idx = pce->hw_idx - (TRUE == pce->is_std_pce ? 1 : 2) - adjust;

        if (FALSE == DEVRM_MoveRuleToNewHwIndex(device_id, pce, new_hw_idx))
        {
            DEVRM_LOG("DEVRM_MoveRuleToNewHwIndex failed."
                      " orig_hw_idx=%hu, new_hw_idx=%hu", pce->hw_idx, new_hw_idx);
            return FALSE;
        }

        DEVRM_GetNextPce(device_id, &pce);

    } while ( NULL != pce && pce->hw_idx < hw_idx );

    return TRUE;
}


