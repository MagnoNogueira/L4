/* FUNCTION NAME: DEV_RM.C
 * PURPOSE:
 *         Support RealTek chip
 * NOTES:
 *
 * REASON:
 *    DESCRIPTION:
 *    CREATOR:
 *    Date          3/13/2015
 *
 * Copyright(C)      Accton Corporation, 2015
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <assert.h>

#include "sys_type.h"
#include "sys_hwcfg.h"

#include "hal/common/halctrl.h"
#include "osal/lib.h"

#include "dev_rm.h"
#include "dev_rm_pmgr.h"
#include "dev_rm_lib.h"
#include "dev_swdrv_common.h"
#include "backdoor_mgr.h"

/* NAMING CONSTANT DECLARATIONS
 */
#ifndef ASSERT
#define ASSERT(e)
#endif

#define DEVRM_MAX_NBR_OF_IACL_SLICE       SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP
#define DEVRM_MAX_NBR_OF_EACL_SLICE       SYS_ADPT_EFP_MAX_NBR_OF_GROUP_PER_CHIP
#define DEVRM_MAX_NBR_OF_SLICE            (DEVRM_MAX_NBR_OF_IACL_SLICE + DEVRM_MAX_NBR_OF_EACL_SLICE)

#define DEVRM_METER_RATE_MINIMAL_UNITS     16 /* Kbps */

#define DEVRM_MAX_NBR_OF_ENTRY_PER_SLICE  SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR
#define DEVRM_MAX_NBR_OF_VIR_GROUP        50 /* Shall same with RULE_TYPE_FUNCTION_TYPE_MAX */
#define DEVRM_MAX_RULE_PER_VIR_GROUP      SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR
#define DEVRM_MIN_RULE_PRIORITY           (-0xFFFF)

#define DEVRM_MAX_NBR_OF_METER_PER_SLICE  16 /* for 839* chip */

#define DEVRM_UNDEF_METER_IDX             SYS_ADPT_DIFFSERV_MAX_NBR_OF_METER
#define DEVRM_UNDEF_COUNTER_IDX           SYS_ADPT_DIFFSERV_MAX_NBR_OF_ACTIONS

#define CPU_PORT(dev)                     HAL_GET_CPU_PORT(dev)

#ifndef soc_ndev
    #define soc_ndev                      1 /* REALTEK now only support 1 */
#endif

/* MACRO FUNCTION DECLARATIONS
 */
#ifndef _countof
#define _countof(a)           (sizeof(a) / sizeof(*a))
#endif

#define DEVRM_NEW_LINE "\r\n"
#define DEVRM_PRINT(fmt, args...) BACKDOOR_MGR_Printf(fmt, ##args)
#define DEVRM_PRINT_HEADER()  DEVRM_PRINT("%s(%d):", __FUNCTION__, __LINE__)

#define DEVRM_LOG(fmt, args...)                             \
{                                                           \
    if (devrm_data.debug_flags & DEVRM_DEBUG_FLAGS_BIT_LOG) \
    {                                                       \
        DEVRM_PRINT_HEADER();                               \
        DEVRM_PRINT(fmt DEVRM_NEW_LINE, ##args);            \
    }                                                       \
}

#define DEVRM_ERR(fmt, args...)                             \
{                                                           \
    if (devrm_data.debug_flags & DEVRM_DEBUG_FLAGS_BIT_ERR) \
    {                                                       \
        DEVRM_PRINT_HEADER();                               \
        DEVRM_PRINT(DEVRM_NEW_LINE "  [Error] ");           \
        DEVRM_PRINT(fmt DEVRM_NEW_LINE, ##args);            \
    }                                                       \
}

#define DEVRM_FATAL_ERR(fmt, args...)                       \
{                                                           \
    DEVRM_PRINT_HEADER();                                   \
    DEVRM_PRINT(DEVRM_NEW_LINE "  [Error] ");               \
    DEVRM_PRINT(fmt DEVRM_NEW_LINE, ##args);                \
}

#define DEVRM_TRACE(fmt, args...)                             \
{                                                             \
    if (devrm_data.debug_flags & DEVRM_DEBUG_FLAGS_BIT_TRACE) \
    {                                                         \
        DEVRM_PRINT("    " fmt DEVRM_NEW_LINE, ##args);       \
    }                                                         \
}

#define DEVRM_TRACE_INPORTS(ports_data, ports_mask)                     \
{                                                                       \
    if (devrm_data.debug_flags & DEVRM_DEBUG_FLAGS_BIT_TRACE)           \
    {                                                                   \
        int i;                                                          \
        for (i = 0; i < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; ++i)  \
        {                                                               \
            DEVRM_PRINT("        ");                                    \
            DEVRM_PRINT("data:0x%012x, mask:0x%012x" DEVRM_NEW_LINE,    \
                ports_data.bits[i], ports_mask.bits[i]);                \
        }                                                               \
    }                                                                   \
}

#define DEVRM_RETURN_RTK_ERROR_CODE(_code)     \
{                                              \
    DEVRM_ERR(" RTK error code = %lu", _code); \
    return _code;                              \
}

#define DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_id, rule_idx)                                                          \
    (slice_id < SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP ?                                                            \
     ((SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR * slice_id) + rule_idx) :                                           \
     (((slice_id - SYS_ADPT_IFP_MAX_NBR_OF_GROUP_PER_CHIP) * SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR) + rule_idx)  \
    )

#define DEVRM_CPU_QUEUE_TO_INTERNAL_PRIORITY DEV_SWDRV_CPU_QUEUE_TO_INTERNAL_PRIORITY

/* CONSTANT STRING
 */
#define DEVRM_STR_GROUP_MODE(mode) ((mode == DEVRM_GROUP_MODE_SINGLE)      ? "SINGLE"    \
                                    : (mode == DEVRM_GROUP_MODE_DOUBLE)    ? "DOUBLE"    \
                                    : (mode == DEVRM_GROUP_MODE_QUADRUPLE) ? "QUADRUPLE" \
                                    : "UNKNOW")

#define DEVRM_STR_PHASE(phase)     ((phase == DEVRM_PHASE_INGRESS) ? "ingress" : "egress")


/* STATIC DATA STRUCTURE DECLARATIONS
 */

typedef struct
{
    UI16_T start;
    UI16_T end;
    int shift;
} DEVRM_MoveParam_T;

typedef struct
{
    int                 rule_pri;       /* logic rule priority. used for sorting
                                         * each rule in a virtual group
                                         */
    UI32_T              rule_id;
    BOOL_T              dirty_bit;
    BOOL_T              valid;
} DEVRM_Rule_T;

typedef struct
{
    UI32_T              cur_rule_nbr;
    DEVRM_Rule_T        rule_tbl[DEVRM_MAX_RULE_PER_VIR_GROUP]; /* index is map to rule priority */
} DEVRM_RuleVector_T;

typedef struct
{
    int                 pri_base;       /* 0, -128, -256 per vg
                                         * Priority for insert rule
                                         */
    int                 pri_lvl;        /* priority level,
                                         *  used for sort each
                                         *  virtual group in a group
                                         */
    UI32_T              group_id;
    UI32_T              vir_group_id;

    DEVRM_RuleVector_T  rv;

    UI32_T              seq_no;         /* used to make sure the sort is stable */

    BOOL_T              valid;
} DEVRM_VirtualGroup_T;

typedef struct
{
    UI32_T vsel_num;
    DEVRM_VirtualGroup_T vg_tbl[DEVRM_MAX_NBR_OF_VIR_GROUP];
} DEVRM_VirtualGroupVector_T;

typedef struct
{
    UI32_T idx;                     /* slice index of slice_tbl */
    UI32_T owner_group_id;          /* ID of owner group */

    UI32_T template_num;            /* numbers of template for this slice */
    UI32_T template_id[2];          /* template id array */

    UI32_T cur_rule_cnt;            /* 0~SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR */

    BOOL_T valid;
} DEVRM_Slice_T;

typedef struct
{
    UI32_T cur_cnt;
    DEVRM_Slice_T slice_tbl[DEVRM_MAX_NBR_OF_SLICE];

    struct
    {
        UI32_T cur_cnt;
        DEVRM_Slice_T *slice_tbl;
    } ingress;

    struct
    {
        UI32_T cur_cnt;
        DEVRM_Slice_T *slice_tbl;
    } egress;

} DEVRM_SliceVector_T;

typedef struct
{
    UI32_T              id;             /* device rule id */
    int                 dr_pri;         /* device rule priority
                                         * 0 ~ -0xffff
                                         * this value come frome (vg.pri_base - vg.rv.rule_tbl's index)
                                         */

    UI32_T              slice_rule_index; /* 0 ~ SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR */
    UI32_T              meter_idx;
    UI32_T              counter_idx;

    BOOL_T              valid;
} DEVRM_DevRule_T;

typedef struct
{
    UI32_T  cur_cnt;
    DEVRM_DevRule_T  rule_tbl[DEVRM_MAX_NBR_OF_ENTRY_PER_SLICE];
} DEVRM_DevRuleVector_T;

typedef struct
{
    UI32_T id; /* group id */

    UI32_T slice_num;       /* Numbers of slice for this group */
    UI32_T slice_id;        /* Fist hardware slice id */

    UI32_T rule_size;               /* rule size of this group */

    DEVRM_DevRuleVector_T drv;  /* Device rule vector */

    DEVRM_PHASE_T phase;
    BOOL_T valid;
} DEVRM_Group_T;

typedef struct
{
    UI32_T cur_cnt;
    DEVRM_Group_T group_tbl[DEVRM_MAX_NBR_OF_SLICE];
} DEVRM_GroupVector_T;

typedef struct
{
    UI32_T id;
    UI32_T dev_rule_id;
    BOOL_T valid;
} DEVRM_DevMeter_T;

typedef struct
{
    UI32_T id;
    rtk_acl_statType_t type;
    UI32_T dev_rule_id;
    BOOL_T valid;
} DEVRM_DevCounter_T;

typedef struct
{
    DEVRM_DevMeter_T   meter[SYS_ADPT_DIFFSERV_MAX_NBR_OF_METER];
    DEVRM_DevCounter_T counter[SYS_ADPT_DIFFSERV_MAX_NBR_OF_ACTIONS];
} DEVRM_Policer_T;

typedef struct
{
    struct
    {
        DEVRM_GroupVector_T         group;
        DEVRM_SliceVector_T         slice;
        DEVRM_VirtualGroupVector_T  vir_grp;
        DEVRM_Policer_T             policer;
    } dev[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT];

    BOOL_T bd_registered;
    UI32_T debug_flags;

} DEVRM_Data_T;

/* STATIC VARIABLE DECLARATIONS
 */
static DEVRM_Data_T devrm_data = {{}, FALSE, 0};

#pragma mark - Local subprogram declearations
/* LOCAL SUBPROGRAM DECLARATIONS
 */
static BOOL_T DEVRM_GetDeviceInfo(DEVRM_DevInfo_T *dev_info_p);
static UI32_T _DEVRM_GetNumberOfChips();

static void _DEVRM_InitCustomerTemplate();

/* hw action control*/
static int _DEVRM_SetupMeterToAction(UI32_T device_id, UI32_T group_id, UI32_T dev_rule_id, const DEVRM_ACTION_T *dev_action_p, rtk_acl_action_t *action_p);
static int _DEVRM_SetupCounterToAction(UI32_T device_id, UI32_T group_id, UI32_T dev_rule_id, const DEVRM_ACTION_T *dev_action_p, rtk_acl_action_t *action_p);
static int _DEVRM_SetupAction(UI32_T device_id, UI32_T group_id, UI32_T dev_rule_id, const DEVRM_ACTION_T *dev_action_p);

/* hw rule control */
static BOOL_T _DEVRM_MoveSliceRuleEntry(UI32_T device_id, DEVRM_PHASE_T direct, UI32_T slice_id, DEVRM_MoveParam_T *move_p);
static BOOL_T _DEVRM_MoveSliceRuleAndUpdateGroup(UI32_T device_id, DEVRM_Group_T *group_p, DEVRM_MoveParam_T *move_p);
static int _DEVRM_SetupFieldQualify(UI32_T device_id, UI32_T group_id, UI32_T dev_rule_id, const DEVRM_AceEntry_T *ace_entry_p);
static void _DEVRM_GetTemplateQualifyBitMap(UI32_T template_id, UI32_T *map);
static int _DEVRM_SetRuleEntryField_SPM(UI32_T device_id, DEVRM_PHASE_T phase, UI32_T hw_rule_idx, rtk_portmask_t ports_data, rtk_portmask_t ports_mask);
static int _DEVRM_SetRuleEntryField_DPM(UI32_T device_id, DEVRM_PHASE_T phase, UI32_T hw_rule_idx, rtk_portmask_t ports_data, rtk_portmask_t ports_mask);
static int _DEVRM_SetRuleEntryFieldForTemplate0(UI32_T device_id, DEVRM_PHASE_T phase, UI32_T hw_rule_idx, const DEVRM_AceEntry_T *ace_p);
static int _DEVRM_SetRuleEntryFieldForTemplate1(UI32_T device_id, DEVRM_PHASE_T phase, UI32_T hw_rule_idx, const DEVRM_AceEntry_T *ace_p);
static int _DEVRM_SetRuleEntryFieldForTemplate2(UI32_T device_id, DEVRM_PHASE_T phase, UI32_T hw_rule_idx, const DEVRM_AceEntry_T *ace_p);
static int _DEVRM_SetRuleEntryFieldForTemplate3(UI32_T device_id, DEVRM_PHASE_T phase, UI32_T hw_rule_idx, const DEVRM_AceEntry_T *ace_p);
static int _DEVRM_SetRuleEntryFieldForTemplate4(UI32_T device_id, DEVRM_PHASE_T phase, UI32_T hw_rule_idx, const DEVRM_AceEntry_T *ace_p);
static int _DEVRM_SetRuleEntryFieldForTemplate5(UI32_T device_id, DEVRM_PHASE_T phase, UI32_T hw_rule_idx, const DEVRM_AceEntry_T *ace_p);
static int _DEVRM_SetRuleEntryFieldForTemplate6(UI32_T device_id, DEVRM_PHASE_T phase, UI32_T hw_rule_idx, const DEVRM_AceEntry_T *ace_p);
static int _DEVRM_SetRuleEntryFieldForTemplate7(UI32_T device_id, DEVRM_PHASE_T phase, UI32_T hw_rule_idx, const DEVRM_AceEntry_T *ace_p);

/* device meter control */
static DEVRM_DevMeter_T * _DEVRM_GetDevMeterEntry(UI32_T device_id, UI32_T meter_id);
static BOOL_T _DEVRM_CreateDevMeter(UI32_T device_id, UI32_T dev_rule_id);
static BOOL_T _DEVRM_DestroyDevMeter(UI32_T device_id, UI32_T dev_rule_id);
static BOOL_T _DEVRM_SetDevMeter(UI32_T device_id, UI32_T dev_rule_id, DEVRM_POLICER_METER_T *meter_p);

/* device counter control */
static DEVRM_DevCounter_T * _DEVRM_GetDevCounterEntry(UI32_T device_id, UI32_T counter_id);
static BOOL_T _DEVRM_CreateDevCounter(UI32_T device_id, UI32_T dev_rule_id);
static BOOL_T _DEVRM_DestroyDevCounter(UI32_T device_id, UI32_T dev_rule_id);

/* device rule control */
static DEVRM_DevRule_T * _DEVRM_FindDevRule(UI32_T device_id, UI32_T group_id, UI32_T device_rule_id);
static BOOL_T _DEVRM_SetDevRulePriority(UI32_T device_id, UI32_T group_id, UI32_T dev_rule_id, int priority);
static BOOL_T _DEVRM_CreateDevRuleEntry(UI32_T device_id, UI32_T group_id, UI32_T *dev_rule_id_p);
static BOOL_T _DEVRM_DestroyDevRuleEntry(UI32_T device_id, UI32_T group_id, UI32_T dev_rule_id);

/* group control */
static BOOL_T _DEVRM_IsGroupExisted(UI32_T device_id, UI32_T idx);
static DEVRM_Group_T * _DEVRM_GetGroupById(UI32_T device_id, UI32_T id);
static BOOL_T _DEVRM_GetSliceNumberByMode(UI32_T group_mode, UI32_T *num_p);
static BOOL_T _DEVRM_GetRuleSizeByMode(UI32_T group_mode, UI32_T *size_p);
static BOOL_T _DEVRM_GetGroupIdByDevRuleId(UI32_T device_rule_id, UI32_T *group_id_p);

/* slice control */
static DEVRM_Slice_T * _DEVRM_CreateSlice(UI32_T device_id, UI32_T slice_id);
static BOOL_T _DEVRM_DestroySlice(UI32_T device_id, UI32_T slice_id);
static DEVRM_Slice_T * _DEVRM_GetSliceByIdx(UI32_T device_id, UI32_T idx);

/* virtual group control */
static DEVRM_VirtualGroup_T * _DEVRM_FindVirtualGroup(UI32_T device_id, UI32_T vir_group_id);
static DEVRM_VirtualGroup_T * _DEVRM_GetVirtualGroupAtIndex(UI32_T device_id, UI32_T idx);
static int _DEVRM_SortVirtualGroup_Compare(const void *arg_1, const void *arg_2);
static BOOL_T _DEVRM_SortVirtualGroup(UI32_T device_id);
static BOOL_T _DEVRM_AddRuleToVirtualGroupAndSetPriority(UI32_T device_id, UI32_T vir_group_id, int rule_pri, UI32_T dev_rule_id);
static BOOL_T _DEVRM_RemoveRuleFromVirtualGroup(UI32_T device_id, UI32_T vir_group_id, UI32_T dev_rule_id);
static BOOL_T _DEVRM_CalculateIndexForNewRuleByPriority(const DEVRM_RuleVector_T *rv_p, int rule_pri, UI32_T *rule_idx_p);

/* virtual group rule control */
static BOOL_T _DEVRM_CompactRule(DEVRM_RuleVector_T *rv_p);
static BOOL_T _DEVRM_InsertRuleAtIndex(UI32_T device_id, DEVRM_VirtualGroup_T *vg_p, UI32_T rule_idx, int rule_pri, UI32_T dev_rule_id);
static BOOL_T _DEVRM_FindRuleIndexByRuleId(DEVRM_RuleVector_T *rv_p, UI32_T dev_rule_id, UI32_T *rule_idx);
static BOOL_T _DEVRM_RemoveRuleAtIndex(DEVRM_RuleVector_T *rv_p, UI32_T rule_idx);

/* vector dump */
static void _DEVRM_DumpRuleVector(DEVRM_RuleVector_T *rv_p, BOOL_T skip_unvalid);
static void _DEVRM_DumpDevRuleVector(DEVRM_DevRuleVector_T *drv_p, BOOL_T skip_unvalid);
static void _DEVRM_DumpSliceVector(DEVRM_SliceVector_T *sv_p, BOOL_T skip_unvalid);

static void _DEVRM_BackdoorMain();

#pragma mark - local subgrogram body
/* LOCAL SUBPROGRAM BODY
 */
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
    int device_id = 0;

    memset(dev_info_p, 0, sizeof(DEVRM_DevInfo_T));

    dev_info_p->num_of_chips = _DEVRM_GetNumberOfChips();

    /* FIXME: 1. check all port need or cpu port ???
     */
    HAL_GET_ALL_PORTMASK(device_id, dev_info_p->port_bit_map.all_ports);
    RTK_PORTMASK_PORT_SET(dev_info_p->port_bit_map.cpu_port, CPU_PORT(device_id));

    return TRUE;
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
static UI32_T _DEVRM_GetNumberOfChips()
{
    return soc_ndev;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_InitCustomerTemplate
 *------------------------------------------------------------------------------
 * PURPOSE  : Set customer template
 * INPUT    : None
 * OUTPUT   : NONE.
 * RETURN   :
 * NOTE     :
 *------------------------------------------------------------------------------
 */
static void _DEVRM_InitCustomerTemplate()
{
    if (HAL_IS_RTL8390_FAMILY_ID(0))
    {
        /* Configure customer template for egress rule
         *  Template
         *         5: Same with template 0, only change the SPM to DPM
         *         6: Same with template 1, only change the SPM to DPM
         *         7: Same with template 4, only change the SPM to DPM
         */
        int ret;
        UI32_T device_id, template_id;
        rtk_acl_template_t template[3];

        template_id = 5;
        memset(&template, 0, sizeof(template));
        template[0].field[0]  = TMPLTE_FIELD_DPM0;
        template[0].field[1]  = TMPLTE_FIELD_DPM1;
        template[0].field[2]  = TMPLTE_FIELD_OTAG;
        template[0].field[3]  = TMPLTE_FIELD_SMAC0;
        template[0].field[4]  = TMPLTE_FIELD_SMAC1;
        template[0].field[5]  = TMPLTE_FIELD_SMAC2;
        template[0].field[6]  = TMPLTE_FIELD_DMAC0;
        template[0].field[7]  = TMPLTE_FIELD_DMAC1;
        template[0].field[8]  = TMPLTE_FIELD_DMAC2;
        template[0].field[9]  = TMPLTE_FIELD_ETHERTYPE;
        template[0].field[10] = TMPLTE_FIELD_DPM2;
        template[0].field[11] = TMPLTE_FIELD_DPM3;

        template[1].field[0]  = TMPLTE_FIELD_SIP0;
        template[1].field[1]  = TMPLTE_FIELD_SIP1;
        template[1].field[2]  = TMPLTE_FIELD_DIP0;
        template[1].field[3]  = TMPLTE_FIELD_DIP1;
        template[1].field[4]  = TMPLTE_FIELD_IP_TOS_PROTO;
        template[1].field[5]  = TMPLTE_FIELD_L4_SPORT;
        template[1].field[6]  = TMPLTE_FIELD_L4_DPORT;
        template[1].field[7]  = TMPLTE_FIELD_ICMP_IGMP;
        template[1].field[8]  = TMPLTE_FIELD_DPM0;
        template[1].field[9]  = TMPLTE_FIELD_DPM1;
        template[1].field[10] = TMPLTE_FIELD_DPM2;
        template[1].field[11] = TMPLTE_FIELD_DPM3;

        template[2].field[0]  = TMPLTE_FIELD_SIP0;
        template[2].field[1]  = TMPLTE_FIELD_SIP1;
        template[2].field[2]  = TMPLTE_FIELD_SIP2;
        template[2].field[3]  = TMPLTE_FIELD_SIP3;
        template[2].field[4]  = TMPLTE_FIELD_SIP4;
        template[2].field[5]  = TMPLTE_FIELD_SIP5;
        template[2].field[6]  = TMPLTE_FIELD_SIP6;
        template[2].field[7]  = TMPLTE_FIELD_SIP7;
        template[2].field[8]  = TMPLTE_FIELD_DPM0;
        template[2].field[9]  = TMPLTE_FIELD_DPM1;
        template[2].field[10] = TMPLTE_FIELD_DPM2;
        template[2].field[11] = TMPLTE_FIELD_DPM3;

        for (template_id = 5; template_id < 8; ++template_id)
        {
            for (device_id = 0; device_id < soc_ndev; ++device_id)
            {
                ret = rtk_acl_template_set(device_id, template_id, &template[template_id-5]);
                if (ret != RT_ERR_OK)
                {
                    DEVRM_FATAL_ERR("Set template %lu fail", template_id);
                }
            }
        }
    }

    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetupMeterToAction
 *------------------------------------------------------------------------------
 * PURPOSE: Setup meter to RTK action entry
 * INPUT:   device_id    -
 *          group_id     -
 *          dev_rule_id  -
 *          dev_action_p - dev_rm action entry
 * OUTPUT:  action_p - RTK action entry
 * RETURN:  RTK error code (RT_ERR_OK/RT_ERR_XXXX)
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetupMeterToAction(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id,
    const DEVRM_ACTION_T *dev_action_p,
    rtk_acl_action_t *action_p)
{
    int ret = RT_ERR_OK;

    if (dev_action_p == NULL || action_p == NULL)
        return RT_ERR_INPUT;

    return ret;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetupCounterToAction
 *------------------------------------------------------------------------------
 * PURPOSE: Setup counter to RTK action entry
 * INPUT:   device_id    -
 *          group_id     -
 *          dev_rule_id  -
 *          dev_action_p - dev_rm action entry
 * OUTPUT:  action_p - RTK action entry
 * RETURN:  RTK error code (RT_ERR_OK/RT_ERR_XXXX)
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetupCounterToAction(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id,
    const DEVRM_ACTION_T *dev_action_p,
    rtk_acl_action_t *action_p)
{
    int ret = RT_ERR_OK;

    if (dev_action_p == NULL || action_p == NULL)
        return RT_ERR_INPUT;

    return ret;
}

static BOOL_T
_DEVRM_SetupAction_ConvertToHwIngressAction(
    UI32_T device_id,
    const DEVRM_ACTION_T *dev_action_p,
    rtk_acl_igrAction_t *igr_action_p,
    int *action_cnt_p)
{
    int remark_cnt = 0, total_cnt = 0;

    ASSERT(dev_action_p != NULL);
    ASSERT(igr_action_p != NULL);

    total_cnt = *action_cnt_p;

    switch (dev_action_p->pkt_cmd)
    {
        case DEVRM_PKT_CMD_COPY_TO_CPU_CANCEL:
        case DEVRM_PKT_CMD_FORWARD:
        case DEVRM_PKT_CMD_UNSPECIFIC:
            if (dev_action_p->pkt_cmd != DEVRM_PKT_CMD_UNSPECIFIC)
            {
                DEVRM_TRACE("[Action] forward");
                igr_action_p->fwd_en = ENABLED;
                igr_action_p->fwd_data.fwd_type = ACL_IGR_ACTION_FWD_PERMIT;
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_inner_priority)
            {
                igr_action_p->pri_en = ENABLED;
                igr_action_p->pri_data.pri = dev_action_p->qos.inner_priority;
                total_cnt++;
                DEVRM_TRACE("[Action] modify_inner_priority %lu", igr_action_p->pri_data.pri);
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_queue)
            {
                if (HAL_IS_RTL8380_FAMILY_ID(device_id))
                {
                    igr_action_p->cpu_pri_en = ENABLED;
                    igr_action_p->cpu_pri_data.pri = DEVRM_CPU_QUEUE_TO_INTERNAL_PRIORITY(dev_action_p->qos.queue);
                    total_cnt++;
                    DEVRM_TRACE("[Action] modify_queue %lu", igr_action_p->cpu_pri_data.pri);
                }
                else if (HAL_IS_RTL8390_FAMILY_ID(device_id))
                {
                    igr_action_p->pri_en = ENABLED;
                    igr_action_p->pri_data.pri = DEVRM_CPU_QUEUE_TO_INTERNAL_PRIORITY(dev_action_p->qos.queue);
                    total_cnt++;
                    DEVRM_TRACE("[Action] modify_inner_priority %lu for CPU", igr_action_p->pri_data.pri);
                }
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_priority)
            {
                if (HAL_IS_RTL8380_FAMILY_ID(device_id))
                {
                    igr_action_p->remark_en = ENABLED;
                    igr_action_p->remark_data.rmk_act = ACL_ACTION_REMARK_OUTER_USER_PRI;
                    igr_action_p->remark_data.rmk_info = dev_action_p->qos.priority;
                    remark_cnt++;
                    total_cnt++;
                    DEVRM_TRACE("[Action] modify_priority(cos) %lu", igr_action_p->remark_data.rmk_info);
                }
                else
                {
                    DEVRM_TRACE("Patch code for 9x chip, remark cos by modified internal priority")
                    igr_action_p->pri_en = ENABLED;
                    igr_action_p->pri_data.pri = dev_action_p->qos.priority;
                    total_cnt++;
                    DEVRM_TRACE("[Action] modify_inner_priority %lu", igr_action_p->pri_data.pri);
                }
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_dscp)
            {
                if (HAL_IS_RTL8380_FAMILY_ID(device_id))
                {
                    igr_action_p->remark_en = ENABLED;
                    igr_action_p->remark_data.rmk_act = ACL_ACTION_REMARK_DSCP;
                    igr_action_p->remark_data.rmk_info = dev_action_p->qos.dscp;
                    remark_cnt++;
                    total_cnt++;
                    DEVRM_TRACE("[Action] modify_dscp) %lu", igr_action_p->remark_data.rmk_info);
                }
                else
                {
                    DEVRM_ERR("Not support modify_dscp) %lu", dev_action_p->qos.dscp);
                    return FALSE;
                }
            }

            if (1 < remark_cnt)
            {
                DEVRM_ERR("Only support remark only 1 field (cos/dscp)");
                ASSERT(0);
                return FALSE;
            }

            switch (dev_action_p->vlan.modify_vlan)
            {
                case DEVRM_MODIFY_VLAN_CMD_FOR_ALL:
                    igr_action_p->outer_vlan_assign_en = ENABLED;
                    igr_action_p->outer_vlan_data.vid_assign_type = ACL_IGR_ACTION_OVLAN_ASSIGN_NEW_VID;
                    igr_action_p->outer_vlan_data.vid_value = dev_action_p->vlan.vid;
                    total_cnt++;
                    DEVRM_TRACE("[Action] modify_vlan %lu", igr_action_p->outer_vlan_data.vid_value);
                    break;

                case DEVRM_MODIFY_VLAN_CMD_DISABLE:                    
                    break;

                case DEVRM_MODIFY_VLAN_CMD_FOR_UNTAGGED_ONLY:
                case DEVRM_MODIFY_VLAN_CMD_FOR_TAGGED_ONLY:
                default:
                    DEVRM_ERR("Not support the vlan modify mode: %lu\r\n",
                        dev_action_p->vlan.modify_vlan);
                    ASSERT(0);
                    break;
            }

            break;

        case DEVRM_PKT_CMD_COPY_TO_CPU:
            DEVRM_TRACE("[Action] copy to CPU");
            igr_action_p->fwd_en = ENABLED;
            igr_action_p->fwd_data.fwd_type = ACL_IGR_ACTION_FWD_COPY_TO_PORTID;

            if (HAL_IS_RTL8380_FAMILY_ID(device_id))
            {
                igr_action_p->fwd_data.info.copy_redirect_port.force = ENABLED;
                igr_action_p->fwd_data.info.copy_redirect_port.fwd_port_id = CPU_PORT(device_id);
            }
            else if (HAL_IS_RTL8390_FAMILY_ID(device_id))
            {
                igr_action_p->fwd_data.fwd_info = CPU_PORT(device_id);
            }
            total_cnt++;

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_queue)
            {
                if (HAL_IS_RTL8380_FAMILY_ID(device_id))
                {
                    igr_action_p->cpu_pri_en = ENABLED;
                    igr_action_p->cpu_pri_data.pri = DEVRM_CPU_QUEUE_TO_INTERNAL_PRIORITY(dev_action_p->qos.queue);
                    total_cnt++;
                    DEVRM_TRACE("[Action] modify_queue %lu", igr_action_p->cpu_pri_data.pri);
                }
                else if (HAL_IS_RTL8390_FAMILY_ID(device_id))
                {
                    igr_action_p->pri_en = ENABLED;
                    igr_action_p->pri_data.pri = DEVRM_CPU_QUEUE_TO_INTERNAL_PRIORITY(dev_action_p->qos.queue);
                    total_cnt++;
                    DEVRM_TRACE("[Action] modify_inner_priority %lu for CPU", igr_action_p->pri_data.pri);
                }
            }
            break;

        case DEVRM_PKT_CMD_REDIRECT_TO_CPU:
            DEVRM_TRACE("[Action] trap to CPU");
            igr_action_p->fwd_en = ENABLED;
            igr_action_p->fwd_data.fwd_type = ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTID;

            if (HAL_IS_RTL8380_FAMILY_ID(device_id))
            {
                igr_action_p->fwd_data.info.copy_redirect_port.force = ENABLED;
                igr_action_p->fwd_data.info.copy_redirect_port.fwd_port_id = CPU_PORT(device_id);
            }
            else if (HAL_IS_RTL8390_FAMILY_ID(device_id))
            {
                igr_action_p->fwd_data.fwd_info = CPU_PORT(device_id);
                igr_action_p->bypass_en = ENABLED;
                igr_action_p->bypass_data.all = 1;
            }

            total_cnt++;

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_queue)
            {
                if (HAL_IS_RTL8380_FAMILY_ID(device_id))
                {
                    igr_action_p->cpu_pri_en = ENABLED;
                    igr_action_p->cpu_pri_data.pri = DEVRM_CPU_QUEUE_TO_INTERNAL_PRIORITY(dev_action_p->qos.queue);
                    total_cnt++;
                    DEVRM_TRACE("[Action] modify_queue %lu", igr_action_p->cpu_pri_data.pri);
                }
                else if (HAL_IS_RTL8390_FAMILY_ID(device_id))
                {
                    igr_action_p->pri_en = ENABLED;
                    igr_action_p->pri_data.pri = DEVRM_CPU_QUEUE_TO_INTERNAL_PRIORITY(dev_action_p->qos.queue);
                    total_cnt++;
                    DEVRM_TRACE("[Action] modify_inner_priority %lu for CPU", igr_action_p->pri_data.pri);
                }
            }
            break;

        case DEVRM_PKT_CMD_SOFT_DROP:
        case DEVRM_PKT_CMD_HARD_DROP:
            DEVRM_TRACE("[Action] forwar-drop");
            //igr_action_p->drop_data = ACL_IGR_ACTION_DROP_DROP;
            igr_action_p->fwd_en = ENABLED;
            igr_action_p->fwd_data.fwd_type = ACL_IGR_ACTION_FWD_DROP;
            total_cnt++;
            // bypass other pipeline ??
            break;

        default:
            ASSERT(0);
            return FALSE;
    }

    *action_cnt_p = total_cnt;

    return TRUE;
}

static BOOL_T
_DEVRM_SetupAction_ConvertToHwEgressAction(
    UI32_T device_id,
    const DEVRM_ACTION_T *dev_action_p,
    rtk_acl_egrAction_t *egr_action_p,
    int *action_cnt_p)
{
    int remark_cnt = 0, total_cnt = 0;

    ASSERT(dev_action_p != NULL);
    ASSERT(egr_action_p != NULL);

    if (HAL_IS_RTL8380_FAMILY_ID(device_id))
    {
        DEVRM_FATAL_ERR("8380 chip not support egress ACL !!");
        ASSERT(0);
        return FALSE;
    }

    total_cnt = *action_cnt_p;

    switch (dev_action_p->pkt_cmd)
    {
        case DEVRM_PKT_CMD_COPY_TO_CPU_CANCEL:
        case DEVRM_PKT_CMD_FORWARD:
        case DEVRM_PKT_CMD_UNSPECIFIC:
            if (dev_action_p->pkt_cmd != DEVRM_PKT_CMD_UNSPECIFIC)
            {
                DEVRM_TRACE("[Action] forward");
                egr_action_p->fwd_en = ENABLED;
                egr_action_p->fwd_data.fwd_type = ACL_EGR_ACTION_FWD_PERMIT;
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_inner_priority)
            {
                egr_action_p->pri_en = ENABLED;
                egr_action_p->pri_data.pri = dev_action_p->qos.inner_priority;
                total_cnt++;
                DEVRM_TRACE("[Action] modify_inner_priority %lu", egr_action_p->pri_data.pri);
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_queue)
            {
                egr_action_p->pri_en = ENABLED;
                egr_action_p->pri_data.pri = DEVRM_CPU_QUEUE_TO_INTERNAL_PRIORITY(dev_action_p->qos.queue);
                if (DEVRM_MODIFY_CMD_DISABLE == dev_action_p->qos.modify_inner_priority)
                {
                    egr_action_p->pri_data.pri_act =  1; /* only for CPU port */
                }
                total_cnt++;
                DEVRM_TRACE("[Action] modify_queue %lu", egr_action_p->pri_data.pri);
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_priority)
            {
                egr_action_p->rmk_en = ENABLED;
                egr_action_p->rmk_data.rmk_type = ACL_ACTION_REMARK_OUTER_USER_PRI;
                egr_action_p->rmk_data.rmk_value = dev_action_p->qos.priority;
                remark_cnt++;
                total_cnt++;
                DEVRM_TRACE("[Action] modify_priority(cos) %lu", egr_action_p->rmk_data.rmk_value);
            }

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_dscp)
            {
                egr_action_p->rmk_en = ENABLED;
                egr_action_p->rmk_data.rmk_type = ACL_ACTION_REMARK_DSCP;
                egr_action_p->rmk_data.rmk_value = dev_action_p->qos.dscp;
                remark_cnt++;
                total_cnt++;
                DEVRM_TRACE("[Action] modify_dscp) %lu", egr_action_p->rmk_data.rmk_value);
            }

            if (1 < remark_cnt)
            {
                DEVRM_ERR("Only support remark only 1 field (cos/dscp)");
                ASSERT(0);
                return FALSE;
            }

            switch (dev_action_p->vlan.modify_vlan)
            {
                case DEVRM_MODIFY_VLAN_CMD_FOR_ALL:
                    egr_action_p->outer_vlan_assign_en = ENABLED;
                    egr_action_p->outer_vlan_data.vid_assign_type = ACL_EGR_ACTION_OVLAN_ASSIGN_NEW_VID;
                    egr_action_p->outer_vlan_data.vid_value = dev_action_p->vlan.vid;
                    total_cnt++;
                    DEVRM_TRACE("[Action] modify_vlan %lu", egr_action_p->outer_vlan_data.vid_value);
                    break;

                case DEVRM_MODIFY_VLAN_CMD_DISABLE:                    
                    break;

                case DEVRM_MODIFY_VLAN_CMD_FOR_UNTAGGED_ONLY:
                case DEVRM_MODIFY_VLAN_CMD_FOR_TAGGED_ONLY:
                default:
                    DEVRM_ERR("Not support the vlan modify mode: %lu\r\n",
                        dev_action_p->vlan.modify_vlan);
                    ASSERT(0);
                    break;
            }

            break;

        case DEVRM_PKT_CMD_COPY_TO_CPU:
            DEVRM_TRACE("[Action] copy to CPU");
            egr_action_p->fwd_en = ENABLED;
            egr_action_p->fwd_data.fwd_type = ACL_EGR_ACTION_FWD_COPY_TO_PORTID;
            egr_action_p->fwd_data.fwd_info = CPU_PORT(device_id);
            total_cnt++;

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_queue)
            {
                egr_action_p->pri_en = ENABLED;
                egr_action_p->pri_data.pri = DEVRM_CPU_QUEUE_TO_INTERNAL_PRIORITY(dev_action_p->qos.queue);
                egr_action_p->pri_data.pri_act = 1; /* only for CPU port */
                total_cnt++;
                DEVRM_TRACE("[Action] modify_queue %lu", egr_action_p->pri_data.pri);
            }
            break;

        case DEVRM_PKT_CMD_REDIRECT_TO_CPU:
            DEVRM_TRACE("[Action] trap to CPU");
            egr_action_p->fwd_en = ENABLED;
            egr_action_p->fwd_data.fwd_type = ACL_EGR_ACTION_FWD_REDIRECT_TO_PORTID;
            egr_action_p->fwd_data.fwd_info = CPU_PORT(device_id);
            total_cnt++;

            if (DEVRM_MODIFY_CMD_ENABLE == dev_action_p->qos.modify_queue)
            {
                egr_action_p->pri_en = ENABLED;
                egr_action_p->pri_data.pri = DEVRM_CPU_QUEUE_TO_INTERNAL_PRIORITY(dev_action_p->qos.queue);
                egr_action_p->pri_data.pri_act = 1; /* only for CPU port */
                total_cnt++;
                DEVRM_TRACE("[Action] modify_queue %lu", egr_action_p->pri_data.pri);
            }
            break;

        case DEVRM_PKT_CMD_SOFT_DROP:
        case DEVRM_PKT_CMD_HARD_DROP:
            DEVRM_TRACE("[Action] forwar-drop");
            //egr_action_p->drop_data = ACL_IGR_ACTION_DROP_DROP;
            egr_action_p->fwd_en = ENABLED;
            egr_action_p->fwd_data.fwd_type = ACL_EGR_ACTION_FWD_DROP;
            total_cnt++;
            break;

       
        default:
            ASSERT(0);
            return FALSE;
    }

    *action_cnt_p = total_cnt;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetupAction
 *------------------------------------------------------------------------------
 * PURPOSE: Setup hw rule action
 * INPUT:   device_id    -
 *          group_id     -
 *          dev_rule_id  -
 *          dev_action_p - dev_rm action entry
 * OUTPUT:
 * RETURN:  RTK error code (RT_ERR_OK/RT_ERR_XXXX)
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetupAction(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id,
    const DEVRM_ACTION_T *dev_action_p)
{
    DEVRM_Group_T *group_p;
    DEVRM_Slice_T *slice_p;
    DEVRM_DevRule_T *dev_rule_p;
    rtk_acl_action_t    action;
    rtk_acl_igrAction_t *igr_action_p = &action.igr_acl;
    rtk_acl_egrAction_t *egr_action_p = &action.egr_acl;
    UI32_T hw_rule_idx;
    int total_cnt = 0;
    int ret;

    DEVRM_LOG("device_id=%lu, group_id=%lu, dev_rule_id=%lu",
              device_id, group_id, dev_rule_id);

    group_p = _DEVRM_GetGroupById(device_id, group_id);
    if (group_p == NULL)
    {
        ASSERT(0);
        return RT_ERR_FAILED;
    }

    slice_p = _DEVRM_GetSliceByIdx(device_id, group_p->slice_id);
    if (NULL == slice_p)
    {
        return RT_ERR_FAILED;
    }

    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (dev_rule_p == NULL)
    {
        ASSERT(0);
        return RT_ERR_FAILED;
    }

    hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index);

    osal_memset(&action, 0, sizeof(action));

    if (group_p->phase == DEVRM_PHASE_INGRESS)
    {
        if (FALSE == _DEVRM_SetupAction_ConvertToHwIngressAction(device_id,
                         dev_action_p, igr_action_p, &total_cnt))
        {
            DEVRM_ERR("Set ingress action fail");
            return RT_ERR_FAILED;
        }
    }
    else
    {
        if (FALSE == _DEVRM_SetupAction_ConvertToHwEgressAction(device_id,
                         dev_action_p, egr_action_p, &total_cnt))
        {
            DEVRM_ERR("Set egress action fail");
            return RT_ERR_FAILED;
        }
    }

    ret = rtk_acl_ruleAction_set(device_id, (rtk_acl_phase_t) group_p->phase, hw_rule_idx, &action);
    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    if (dev_action_p->policer.mode != DEVRM_POLICER_MODE_DISABLE)
    {
        if (dev_action_p->policer.mode == DEVRM_POLICER_MODE_METER_ONLY ||
            dev_action_p->policer.mode == DEVRM_POLICER_MODE_METER_AND_COUNTER)
        {
            if (FALSE == DEVRM_CreateMeter(device_id, dev_rule_id,
                                           &dev_action_p->policer.meter, FALSE))
            {
                return RT_ERR_FAILED;
            }
            total_cnt++;
        }

        if (dev_action_p->policer.mode == DEVRM_POLICER_MODE_COUNTER_ONLY ||
            dev_action_p->policer.mode == DEVRM_POLICER_MODE_METER_AND_COUNTER)
        {
            if (FALSE == DEVRM_CreateCounter(device_id, dev_rule_id, dev_action_p->policer.counter_mode, FALSE))
            {
                return RT_ERR_FAILED;
            }
            total_cnt++;
        }
    }

    if (5 < total_cnt)
    {
        ASSERT(0);
    }

    return RT_ERR_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_MoveSliceRuleEntry
 *------------------------------------------------------------------------------
 * PURPOSE: Move the hardware rule entry for the slice
 * INPUT:   device_id -
 *          direct    - ingress/egress
 *          slice_id  -
 *          move_p    - move paremeter(start, end, shift)
 * OUTPUT:
 * RETURN:  TRUE/FALSE
 * NOTE:    move.shift < 0, move hw rules to less index
 *          move.shift > 0, move hw rules to bigger index
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_MoveSliceRuleEntry(
    UI32_T device_id,
    DEVRM_PHASE_T direct,
    UI32_T slice_id,
    DEVRM_MoveParam_T *move_p)
{
    UI32_T ret;
    rtk_acl_move_t param;

    if (((int)move_p->start + move_p->shift) < 0 ||
        DEVRM_MAX_NBR_OF_ENTRY_PER_SLICE < ((int)move_p->end + move_p->shift))
    {
        return FALSE;
    }

    param.length = move_p->end - move_p->start + 1;
    if (direct == DEVRM_PHASE_INGRESS)
    {
        param.move_from = (slice_id * SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR) + move_p->start;
    }
    else
    {
        param.move_from = ((slice_id - DEVRM_MAX_NBR_OF_IACL_SLICE)* SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR) + move_p->start;
    }
    param.move_to = (UI16_T)((int)param.move_from + move_p->shift);

    ret = rtk_acl_rule_move(device_id, (rtk_acl_phase_t) direct, &param);
    if (ret != RT_ERR_OK)
    {
        DEVRM_ERR("rtk_acl_rule_move error_code = %lu \r\n"
                  "device_id=%lu, %s, move from %u to %u, length=%u",
                  ret, device_id, (direct == DEVRM_PHASE_EGRESS) ? "egress":"ingress",
                  param.move_from, param.move_to, param.length);
        return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_MoveSliceRuleAndUpdateGroup
 *------------------------------------------------------------------------------
 * PURPOSE: Move hardware rule and update device rule info in group
 * INPUT:   device_id -
 *          group_p   - group
 *          move_p    - move parameter(start, end, shift)
 * OUTPUT:
 * RETURN:  TRUE/FALSE
 * NOTE:    move.shift < 0, move hw rules to less index
 *          move.shift > 0, move hw rules to bigger index
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_MoveSliceRuleAndUpdateGroup(
    UI32_T device_id,
    DEVRM_Group_T *group_p,
    DEVRM_MoveParam_T *move_p)
{
    UI32_T i;
    BOOL_T ret;

    ASSERT(move_p->end < SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR);

    for (i = 0; i < group_p->slice_num; ++i)
    {
        ret = _DEVRM_MoveSliceRuleEntry(device_id,
                                        group_p->phase,
                                        group_p->slice_id + i,
                                        move_p);
        if (ret != TRUE)
        {
            return ret;
        }
    }

    for (i = 0; i < _countof(group_p->drv.rule_tbl); i++)
    {
        DEVRM_DevRule_T *dev_rule_p = &group_p->drv.rule_tbl[i];

        if (dev_rule_p->valid == FALSE)
        {
            continue;
        }

        if (dev_rule_p->slice_rule_index < move_p->start)
        {
            continue;
        }

        dev_rule_p->slice_rule_index = (UI32_T)((int)dev_rule_p->slice_rule_index + move_p->shift);
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetupFieldQualify
 *------------------------------------------------------------------------------
 * PURPOSE: Setup hw rule entry by input ace
 * INPUT:   device_id    -
 *          group_id     -
 *          dev_rule_id  -
 *          ace_entry_p  - rule ace, filter pattern.
 * OUTPUT:
 * RETURN:  RTK error code (RT_ERR_OK/RT_ERR_XXXX)
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetupFieldQualify(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id,
    const DEVRM_AceEntry_T *ace_entry_p)
{
    DEVRM_Group_T *group_p;
    DEVRM_Slice_T *slice_p;
    DEVRM_DevRule_T *dev_rule_p;
    UI32_T i, j;
    UI8_T data_8, mask_8;
    int ret;

    DEVRM_LOG("device_id=%lu, group_id=%lu, dev_rule_id=%lu",
        device_id, group_id, dev_rule_id);

    group_p = _DEVRM_GetGroupById(device_id, group_id);
    if (group_p == NULL)
    {
        ASSERT(0);
        return RT_ERR_FAILED;
    }

    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (dev_rule_p == NULL)
    {
        ASSERT(0);
        return RT_ERR_FAILED;
    }

    /* FIXME: check qualify field is OK on this template
     */
    // Use _DEVRM_GetTemplateQualifyBitMap

    for (i = 0; i < group_p->slice_num; ++i)
    {
        slice_p = _DEVRM_GetSliceByIdx(device_id, group_p->slice_id + i);
        if (NULL == slice_p)
        {
            return RT_ERR_FAILED;
        }

        ASSERT(slice_p->template_num == group_p->rule_size);
        for (j = 0; j < slice_p->template_num; ++j)
        {
            UI32_T template_id = slice_p->template_id[j];
            UI32_T hw_rule_idx;

            hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

            {
                /* USER_FIELD_TEMPLATE_ID, (Must set this befor setting template key field)
                 */
                data_8 = j; /* 0: template[0], 1: template[1] */
                mask_8 = 0x3;
                ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) group_p->phase,
                                                   hw_rule_idx, USER_FIELD_TEMPLATE_ID,
                                                   &data_8, &mask_8);
                if (ret != RT_ERR_OK)
                {
                    DEVRM_RETURN_RTK_ERROR_CODE(ret);
                }
            }

            /* fill common
             */
            if (DEVRM_SHR_BITGET(ace_entry_p->w, DEVRM_FIELD_QUALIFY_L2Format)
                && ace_entry_p->l2format != DEVRM_FIELD_QUALIFY_L2Format_Any)
            {
                DEVRM_TRACE("DEVRM_FIELD_QUALIFY_L2Format (data:%lu)", ace_entry_p->l2format);

                data_8 = (ace_entry_p->l2format == DEVRM_FIELD_QUALIFY_L2Format_Ethernet) ? 0x0
                        : (ace_entry_p->l2format == DEVRM_FIELD_QUALIFY_L2Format_LLC_SNAP) ? 0x1
                        : (ace_entry_p->l2format == DEVRM_FIELD_QUALIFY_L2Format_LLC_Other) ? 0x2
                        : 0xff;
                /* make behavior same with marvell
                 *  - Ether2 = DEVRM_FIELD_QUALIFY_L2Format_Ethernet | DEVRM_FIELD_QUALIFY_L2Format_LLC_SNAP
                 *  - 802.3  = DEVRM_FIELD_QUALIFY_L2Format_LLC_Other
                 */
                mask_8 = 0x3;

                if (data_8 == 0xff)
                {
                    DEVRM_RETURN_RTK_ERROR_CODE(RT_ERR_INPUT);
                }

                ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) group_p->phase,
                                                   hw_rule_idx, USER_FIELD_FRAME_TYPE_L2,
                                                   &data_8, &mask_8);
                if (ret != RT_ERR_OK)
                {
                    DEVRM_RETURN_RTK_ERROR_CODE(ret);
                }
            }

            if (DEVRM_SHR_BITGET(ace_entry_p->w, DEVRM_FIELD_QUALIFY_VlanFormat))
            {
                DEVRM_TRACE("DEVRM_FIELD_QUALIFY_VlanFormat (data:%u)", ace_entry_p->vlanformat_data);

                mask_8 = 0x1;
                data_8 = 0x1;

                switch (ace_entry_p->vlanformat_data)
                {
                    case DEVRM_FIELD_QUALIFY_VlanFormat_Outer_UntagOrPriTag:
                        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) group_p->phase,
                                                           hw_rule_idx, USER_FIELD_OTAG_FMT,
                                                           &data_8, &mask_8);
                        if (ret != RT_ERR_OK)
                        {
                            DEVRM_RETURN_RTK_ERROR_CODE(ret);
                        }

                        data_8 = 0;
                        mask_8 = 0;
                        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) group_p->phase,
                                                           hw_rule_idx, USER_FIELD_OTAG_EXIST,
                                                           &data_8, &mask_8);
                        if (ret != RT_ERR_OK)
                        {
                            DEVRM_RETURN_RTK_ERROR_CODE(ret);
                        }
                        break;

                    case DEVRM_FIELD_QUALIFY_VlanFormat_Outer_TagOrPriTag:
                        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) group_p->phase,
                                                           hw_rule_idx, USER_FIELD_OTAG_EXIST,
                                                           &data_8, &mask_8);
                        if (ret != RT_ERR_OK)
                        {
                            DEVRM_RETURN_RTK_ERROR_CODE(ret);
                        }

                        data_8 = 0;
                        mask_8 = 0;
                        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) group_p->phase,
                                                           hw_rule_idx, USER_FIELD_OTAG_FMT,
                                                           &data_8, &mask_8);
                        if (ret != RT_ERR_OK)
                        {
                            DEVRM_RETURN_RTK_ERROR_CODE(ret);
                        }
                        break;

                    case DEVRM_FIELD_QUALIFY_VlanFormat_Outer_TagOnly:
                        data_8 = 0;
                        mask_8 = 1;
                        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) group_p->phase,
                                                           hw_rule_idx, USER_FIELD_OTAG_FMT,
                                                           &data_8, &mask_8);
                        if (ret != RT_ERR_OK)
                        {
                            DEVRM_RETURN_RTK_ERROR_CODE(ret);
                        }
                        break;

                    case DEVRM_FIELD_QUALIFY_VlanFormat_Inner_TagOrPriTag:
                    case DEVRM_FIELD_QUALIFY_VlanFormat_Inner_UntagOrPriTag:
                        DEVRM_LOG("Not implement yet!");
                        break;

                    case DEVRM_FIELD_QUALIFY_VlanFormat_Any:
                    default:
                        data_8 = 0;
                        mask_8 = 0;

                        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) group_p->phase,
                                                           hw_rule_idx, USER_FIELD_OTAG_EXIST,
                                                           &data_8, &mask_8);
                        if (ret != RT_ERR_OK)
                        {
                            DEVRM_RETURN_RTK_ERROR_CODE(ret);
                        }

                        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) group_p->phase,
                                                           hw_rule_idx, USER_FIELD_OTAG_FMT,
                                                           &data_8, &mask_8);
                        if (ret != RT_ERR_OK)
                        {
                            DEVRM_RETURN_RTK_ERROR_CODE(ret);
                        }
                        break;
                }
            }

            if (DEVRM_SHR_BITGET(ace_entry_p->w, DEVRM_FIELD_QUALIFY_IpType)
                && ace_entry_p->iptype != DEVRM_FIELD_QUALIFY_IpType_Any)
            {
                DEVRM_TRACE("DEVRM_FIELD_QUALIFY_IpType (data:%lu)", ace_entry_p->iptype);

                mask_8 = 0x3;
                data_8 =  (ace_entry_p->iptype == DEVRM_FIELD_QUALIFY_IpType_ARP) ? 0x0
                        : (ace_entry_p->iptype == DEVRM_FIELD_QUALIFY_IpType_NonIp) ? 0x1
                        : (ace_entry_p->iptype == DEVRM_FIELD_QUALIFY_IpType_Ipv4) ? 0x2
                        : (ace_entry_p->iptype == DEVRM_FIELD_QUALIFY_IpType_Ipv6) ? 0x3
                        : 0xff;

                if (data_8 == 0xff)
                {
                    DEVRM_RETURN_RTK_ERROR_CODE(RT_ERR_INPUT);
                }

                ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) group_p->phase,
                                                   hw_rule_idx, USER_FIELD_FRAME_TYPE,
                                                   &data_8, &mask_8);
                if (ret != RT_ERR_OK)
                {
                    DEVRM_RETURN_RTK_ERROR_CODE(ret);
                }
            }

            if (DEVRM_SHR_BITGET(ace_entry_p->w, DEVRM_FIELD_QUALIFY_SwitchMac))
            {
                DEVRM_TRACE("DEVRM_FIELD_QUALIFY_SwitchMac");
                data_8 = mask_8 = 1;
                ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) group_p->phase,
                                                   hw_rule_idx, USER_FIELD_SWITCHMAC,
                                                   &data_8, &mask_8);
                if (ret != RT_ERR_OK)
                {
                    DEVRM_RETURN_RTK_ERROR_CODE(ret);
                }
            }

            switch (template_id)
            {
                case 0:
                    ret = _DEVRM_SetRuleEntryFieldForTemplate0(device_id, group_p->phase, hw_rule_idx, ace_entry_p);
                    break;
                case 1:
                    ret = _DEVRM_SetRuleEntryFieldForTemplate1(device_id, group_p->phase, hw_rule_idx, ace_entry_p);
                    break;
                case 2:
                    ret = _DEVRM_SetRuleEntryFieldForTemplate2(device_id, group_p->phase, hw_rule_idx, ace_entry_p);
                    break;
                case 3:
                    ret = _DEVRM_SetRuleEntryFieldForTemplate3(device_id, group_p->phase, hw_rule_idx, ace_entry_p);
                    break;
                case 4:
                    ret = _DEVRM_SetRuleEntryFieldForTemplate4(device_id, group_p->phase, hw_rule_idx, ace_entry_p);
                    break;
                case 5:
                    ret = _DEVRM_SetRuleEntryFieldForTemplate5(device_id, group_p->phase, hw_rule_idx, ace_entry_p);
                    break;
                case 6:
                    ret = _DEVRM_SetRuleEntryFieldForTemplate6(device_id, group_p->phase, hw_rule_idx, ace_entry_p);
                    break;
                case 7:
                    ret = _DEVRM_SetRuleEntryFieldForTemplate7(device_id, group_p->phase, hw_rule_idx, ace_entry_p);
                    break;
                default:
                    ASSERT(0);
                    DEVRM_ERR("Wrong template id");
                    return RT_ERR_FAILED;
            }

            if (ret != RT_ERR_OK)
            {
                DEVRM_RETURN_RTK_ERROR_CODE(ret);
            }
        }
    }

    return RT_ERR_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_GetTemplateQualifyBitMap
 *------------------------------------------------------------------------------
 * PURPOSE:  Get RTK template qualify bitmap
 * INPUT:    template_id - 0~7
 * OUTPUT:   map         - bitmap array
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
static void
_DEVRM_GetTemplateQualifyBitMap(
    UI32_T template_id,
    UI32_T *map)
{
    /* FIXME: Add common pattern
     */
    DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_L2Format);
    DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_VlanFormat);
    DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_IpType);
    DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_SwitchMac);

    switch (template_id)
    {
        case 0:
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_InPorts);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_OuterVlan);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_InnerVlan);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_OuterVlanPri);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_SrcMac);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_DstMac);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_EtherType);
            break;

        case 1:
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_SrcIp);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_DstIp);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_IpProtocol);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_Dscp);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_Tos);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_L4SrcPort);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_L4DstPort);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_ArpOpcode);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_InPorts);

            break;

        case 2:
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_DstMac);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_InnerVlan);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_EtherType);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_IpProtocol);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_Dscp);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_Tos);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_L4SrcPort);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_L4DstPort);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_ArpOpcode);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_InPorts);
            break;

        case 3:
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_DstIp6);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_L4SrcPort);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_L4DstPort);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_ArpOpcode);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_Ip6NextHeader);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_Ip6TrafficClass);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_Dscp);
            break;

        case 4:
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_SrcIp6);
            DEVRM_SHR_BITSET(map, DEVRM_FIELD_QUALIFY_InPorts);
            break;

        default:
            ASSERT(0);
            break;
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetRuleEntryField_SPM
 *------------------------------------------------------------------------------
 * PURPOSE: Set the template SPM field on device rule entry
 * INPUT:   device_id   - device id
 *          phase       - ingress/egress
 *          hw_rule_idx - hardware rule entry index
 *          port_data   - port bit map data
 *          port_mask   - port bit map mask
 * OUTPUT:
 * RETURN: RT_ERR_OK / error code
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetRuleEntryField_SPM(
    UI32_T device_id,
    DEVRM_PHASE_T phase,
    UI32_T hw_rule_idx,
    rtk_portmask_t ports_data,
    rtk_portmask_t ports_mask)
{
    int ret = RT_ERR_OK;

    UI8_T data[8] = {0}; /* In logical, max SPM field has 8 bytes */
    UI8_T mask[8] = {0};

    RTK_PORTMASK_AND(ports_mask, ports_data);
    RTK_PORTMASK_REVERT(ports_mask);

    DEVRM_TRACE_INPORTS(ports_data, ports_mask);

    {
        int    i = 0, byte_shift = 0;
        int    bits_index = 0;
        UI32_T field_size;

        ret = rtk_acl_ruleEntryFieldSize_get(device_id, USER_FIELD_SPM, &field_size);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("rtk_acl_ruleEntryFieldSize_get error");
            return ret;
        }

        field_size = ((field_size + 7) / 8); /* convert bit size to byte size */

        for (i = (field_size - 1); i >= 0; i--)
        {
            /* Fill the byte array in network order
             */
            data[i] = (ports_data.bits[bits_index] >> (byte_shift*8)) & 0xff;
            mask[i] = (ports_mask.bits[bits_index] >> (byte_shift*8)) & 0xff;
#if 0
            DEVRM_TRACE("  data[%02d]: %02X  mask[%02d]: %02X", i, data[i], i, mask[i]);
#endif
            byte_shift++;
            if (byte_shift >= 4)
            {
                bits_index += 1;
                byte_shift = byte_shift % 4;
            }
        }
    }

    ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                       hw_rule_idx, USER_FIELD_SPM,
                                       (UI8_T *)data,
                                       (UI8_T *)mask);

    return ret;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetRuleEntryField_DPM
 *------------------------------------------------------------------------------
 * PURPOSE: Set the template DPM field on device rule entry
 * INPUT:   device_id   - device id
 *          phase       - ingress/egress
 *          hw_rule_idx - hardware rule entry index
 *          port_data   - port bit map data
 *          port_mask   - port bit map mask
 * OUTPUT:
 * RETURN: RT_ERR_OK / error code
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetRuleEntryField_DPM(
    UI32_T device_id,
    DEVRM_PHASE_T phase,
    UI32_T hw_rule_idx,
    rtk_portmask_t ports_data,
    rtk_portmask_t ports_mask)
{
    int ret = RT_ERR_OK;

    UI8_T data[8] = {0}; /* In logical, max DPM field has 8 bytes */
    UI8_T mask[8] = {0};

    RTK_PORTMASK_AND(ports_mask, ports_data);
    RTK_PORTMASK_REVERT(ports_mask);

    DEVRM_TRACE_INPORTS(ports_data, ports_mask);

    {
        int    i = 0, byte_shift = 0;
        int    bits_index = 0;
        UI32_T field_size;

        ret = rtk_acl_ruleEntryFieldSize_get(device_id, USER_FIELD_DPM, &field_size);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("rtk_acl_ruleEntryFieldSize_get error");
            return ret;
        }

        field_size = ((field_size + 7) / 8); /* convert bit size to byte size */

        for (i = (field_size - 1); i >= 0; i--)
        {
            /* Fill the byte array in network order
             */
            data[i] = (ports_data.bits[bits_index] >> (byte_shift*8)) & 0xff;
            mask[i] = (ports_mask.bits[bits_index] >> (byte_shift*8)) & 0xff;
#if 0
            DEVRM_TRACE("  data[%02d]: %02X  mask[%02d]: %02X", i, data[i], i, mask[i]);
#endif
            byte_shift++;
            if (byte_shift >= 4)
            {
                bits_index += 1;
                byte_shift = byte_shift % 4;
            }
        }
    }

    ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                       hw_rule_idx, USER_FIELD_DPM,
                                       (UI8_T *)data,
                                       (UI8_T *)mask);
    
    return ret;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetRuleEntryFieldForTemplate0
 *------------------------------------------------------------------------------
 * PURPOSE: Set rule entry field for template 0
 * INPUT:   device_id   - device id
 *          phase       - ingress/egress
 *          hw_rule_idx - hardware rule entry index
 *          ace_p       - ace entry
 * OUTPUT:
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetRuleEntryFieldForTemplate0(
    UI32_T device_id,
    DEVRM_PHASE_T phase,
    UI32_T hw_rule_idx,
    const DEVRM_AceEntry_T *ace_p)
{
    int ret;

    DEVRM_LOG("device_id=%lu, phase=%s, hw_rule_idx=%lu",
              device_id, (phase == DEVRM_PHASE_INGRESS) ? "ingress" : "egress", hw_rule_idx);

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_InPorts))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_InPorts");
        ret = _DEVRM_SetRuleEntryField_SPM(device_id, phase, hw_rule_idx,
                                           ace_p->inports_data,
                                           ace_p->inports_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_InPorts error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_OuterVlan))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_OuterVlan (data:%u, mask:%u)",
                    ace_p->outvlan_data, ace_p->outvlan_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_OTAG_VID,
                                           (UI8_T *)&ace_p->outvlan_data, (UI8_T *)&ace_p->outvlan_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_OuterVlan error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_InnerVlan))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_InnerVlan (data:%u, mask:%u)",
                    ace_p->invlan_data, ace_p->invlan_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ITAG_VID,
                                           (UI8_T *)&ace_p->invlan_data, (UI8_T *)&ace_p->invlan_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_InnerVlan error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_OuterVlanPri))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_OuterVlanPri (data:%u, mask:%u)",
                    ace_p->outvlanpri_data, ace_p->outvlanpri_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_OTAG_PRI,
                                           &ace_p->outvlanpri_data, &ace_p->outvlanpri_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_OuterVlanPri error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_SrcMac))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_SrcMac (data:0x%02x%02x%02x%02x%02x%02x, mask:0x%02x%02x%02x%02x%02x%02x)",
                    ace_p->srcmac_data[0], ace_p->srcmac_data[1], ace_p->srcmac_data[2],
                    ace_p->srcmac_data[3], ace_p->srcmac_data[4], ace_p->srcmac_data[5],
                    ace_p->srcmac_mask[0], ace_p->srcmac_mask[1], ace_p->srcmac_mask[2],
                    ace_p->srcmac_mask[3], ace_p->srcmac_mask[4], ace_p->srcmac_mask[5]);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_SMAC,
                                           ace_p->srcmac_data, ace_p->srcmac_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_SrcMac error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_DstMac))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_DstMac (data:0x%02x%02x%02x%02x%02x%02x, mask:0x%02x%02x%02x%02x%02x%02x)",
                    ace_p->dstmac_data[0], ace_p->dstmac_data[1], ace_p->dstmac_data[2],
                    ace_p->dstmac_data[3], ace_p->dstmac_data[4], ace_p->dstmac_data[5],
                    ace_p->dstmac_mask[0], ace_p->dstmac_mask[1], ace_p->dstmac_mask[2],
                    ace_p->dstmac_mask[3], ace_p->dstmac_mask[4], ace_p->dstmac_mask[5]);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_DMAC,
                                           ace_p->dstmac_data, ace_p->dstmac_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_DstMac error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_EtherType))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_EtherType (data:0x%04x, mask:0x%04x)",
                    ace_p->ethertype_data, ace_p->ethertype_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ETHERTYPE,
                                           (UI8_T *)&ace_p->ethertype_data, (UI8_T *)&ace_p->ethertype_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_EtherType error");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetRuleEntryFieldForTemplate1
 *------------------------------------------------------------------------------
 * PURPOSE: Set rule entry field for template 1
 * INPUT:   device_id   - device id
 *          phase       - ingress/egress
 *          hw_rule_idx - hardware rule entry index
 *          ace_p       - ace entry
 * OUTPUT:
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetRuleEntryFieldForTemplate1(
    UI32_T device_id,
    DEVRM_PHASE_T phase,
    UI32_T hw_rule_idx,
    const DEVRM_AceEntry_T *ace_p)
{
    int ret;

    DEVRM_LOG("device_id=%lu, phase=%s, hw_rule_idx=%lu",
          device_id, (phase == DEVRM_PHASE_INGRESS) ? "ingress" : "egress", hw_rule_idx);

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_InPorts))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_InPorts");
        ret = _DEVRM_SetRuleEntryField_SPM(device_id, phase, hw_rule_idx,
                                           ace_p->inports_data,
                                           ace_p->inports_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_InPorts error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_SrcIp))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_SrcIp (data:%lu, mask:%lu)",
                    ace_p->srcip_data, ace_p->srcip_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP4_SIP,
                                           &ace_p->srcip_data, &ace_p->srcip_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_SrcIp error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_DstIp))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_DstIp (data:%lu, mask:%lu)",
                    ace_p->dstip_data, ace_p->dstip_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP4_DIP,
                                           &ace_p->dstip_data, &ace_p->dstip_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_DstIp error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_IpProtocol))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_IpProtocol (data:%u, mask:%u)",
                    ace_p->ipprotocol_data, ace_p->ipprotocol_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP4PROTO_IP6NH,
                                           &ace_p->ipprotocol_data, &ace_p->ipprotocol_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_IpProtocol error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_Dscp))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_Dscp (data:%u, mask:%u)",
                    ace_p->dscp_data, ace_p->dscp_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP_DSCP,
                                           &ace_p->dscp_data, &ace_p->dscp_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_Dscp error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_Tos))
    {
        ASSERT(0);
        DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_Tos error");
        return RT_ERR_FAILED;
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_L4SrcPort))
    {
        UI16_T data = (UI16_T)ace_p->l4_srcport_data;
        UI16_T mask = (UI16_T)ace_p->l4_srcport_mask;

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_L4SrcPort (data:%u, mask:%u)",
                    data, mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_L4_SRC_PORT,
                                           (UI8_T *)&data, (UI8_T *)&mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_L4SrcPort error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_L4DstPort))
    {
        UI16_T data = (UI16_T)ace_p->l4_dstport_data;
        UI16_T mask = (UI16_T)ace_p->l4_dstport_mask;

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_L4DstPort (data:%u, mask:%u)",
                    data, mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_L4_DST_PORT,
                                           (UI8_T *)&data, (UI8_T *)&mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_L4DstPort error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_ArpOpcode))
    {
        UI16_T data = (UI16_T)ace_p->arp_opcode_data;
        UI16_T mask = (UI16_T)ace_p->arp_opcode_mask;
        UI8_T  data_8 = 0x0 /* ARP */, mask_8 = 0x3;

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_ArpOpcode (data:%u, mask:%u)",
                    data, mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_FRAME_TYPE,
                                           &data_8, &mask_8);
        if (ret != RT_ERR_OK)
        {
            DEVRM_RETURN_RTK_ERROR_CODE(ret);
        }

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ARPOPCODE,
                                           (UI8_T *)&data, (UI8_T *)&mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_RETURN_RTK_ERROR_CODE(ret);
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_IcmpType))
    {

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_IcmpType (data:%u, mask:%u)",
                    ace_p->icmptype_data, ace_p->icmptype_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ICMP_TYPE,
                                           &ace_p->icmptype_data, &ace_p->icmptype_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_IcmpType error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_IcmpCode))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_IcmpCode (data:%u, mask:%u)",
                    ace_p->icmpcode_data, ace_p->icmpcode_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ICMP_CODE,
                                           &ace_p->icmpcode_data, &ace_p->icmpcode_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_IcmpCode error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_TcpControl))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_TcpControl (data:%u, mask:%u)",
                    ace_p->tcpcontrol_data, ace_p->tcpcontrol_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_TCP_FLAG,
                                           &ace_p->tcpcontrol_data, &ace_p->tcpcontrol_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_TcpControl error");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetRuleEntryFieldForTemplate2
 *------------------------------------------------------------------------------
 * PURPOSE: Set rule entry field for template 2
 * INPUT:   device_id   - device id
 *          phase       - ingress/egress
 *          hw_rule_idx - hardware rule entry index
 *          ace_p       - ace entry
 * OUTPUT:
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetRuleEntryFieldForTemplate2(
    UI32_T device_id,
    DEVRM_PHASE_T phase,
    UI32_T hw_rule_idx,
    const DEVRM_AceEntry_T *ace_p)
{
    int ret;

    DEVRM_LOG("device_id=%lu, phase=%s, hw_rule_idx=%lu",
          device_id, (phase == DEVRM_PHASE_INGRESS) ? "ingress" : "egress", hw_rule_idx);

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_InPorts))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_InPorts");
        ret = _DEVRM_SetRuleEntryField_SPM(device_id, phase, hw_rule_idx,
                                           ace_p->inports_data,
                                           ace_p->inports_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_InPorts error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_DstMac))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_DstMac (data:0x%02x%02x%02x%02x%02x%02x, mask:0x%02x%02x%02x%02x%02x%02x)",
                    ace_p->dstmac_data[0], ace_p->dstmac_data[1], ace_p->dstmac_data[2],
                    ace_p->dstmac_data[3], ace_p->dstmac_data[4], ace_p->dstmac_data[5],
                    ace_p->dstmac_mask[0], ace_p->dstmac_mask[1], ace_p->dstmac_mask[2],
                    ace_p->dstmac_mask[3], ace_p->dstmac_mask[4], ace_p->dstmac_mask[5]);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_DMAC,
                                           (UI8_T *)&ace_p->dstmac_data, (UI8_T *)&ace_p->dstmac_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_DstMac error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_InnerVlan))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_InnerVlan (data:%u, mask:%u)",
                    ace_p->invlan_data, ace_p->invlan_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ITAG_VID,
                                           (UI8_T *)&ace_p->invlan_data, (UI8_T *)&ace_p->invlan_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_InnerVlan error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_IpProtocol))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_IpProtocol (data:%u, mask:%u)",
                    ace_p->ipprotocol_data, ace_p->ipprotocol_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP4PROTO_IP6NH,
                                           &ace_p->ipprotocol_data, &ace_p->ipprotocol_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_IpProtocol error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_Dscp))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_Dscp (data:%u, mask:%u)",
                    ace_p->dscp_data, ace_p->dscp_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP_DSCP,
                                           &ace_p->dscp_data, &ace_p->dscp_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_Dscp error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_Tos))
    {
        ASSERT(0);
        DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_Tos error");
        return RT_ERR_FAILED;
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_L4SrcPort))
    {
        UI16_T data = (UI16_T)ace_p->l4_srcport_data;
        UI16_T mask = (UI16_T)ace_p->l4_srcport_mask;

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_L4SrcPort (data:%u, mask:%u)",
                    data, mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_L4_SRC_PORT,
                                           (UI8_T *)&data, (UI8_T *)&mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_L4SrcPort error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_L4DstPort))
    {
        UI16_T data = (UI16_T)ace_p->l4_dstport_data;
        UI16_T mask = (UI16_T)ace_p->l4_dstport_mask;

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_L4DstPort (data:%u, mask:%u)",
                    data, mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_L4_DST_PORT,
                                           (UI8_T *)&data, (UI8_T *)&mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_L4DstPort error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_ArpOpcode))
    {
        UI16_T data = (UI16_T)ace_p->arp_opcode_data;
        UI16_T mask = (UI16_T)ace_p->arp_opcode_mask;
        UI8_T  data_8 = 0x0 /* ARP */, mask_8 = 0x3;

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_ArpOpcode (data:%u, mask:%u)",
                    data, mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_FRAME_TYPE,
                                           &data_8, &mask_8);
        if (ret != RT_ERR_OK)
        {
            DEVRM_RETURN_RTK_ERROR_CODE(ret);
        }

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ARPOPCODE,
                                           (UI8_T *)&data, (UI8_T *)&mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_RETURN_RTK_ERROR_CODE(ret);
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_IcmpType))
    {

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_IcmpType (data:%u, mask:%u)",
                    ace_p->icmptype_data, ace_p->icmptype_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ICMP_TYPE,
                                           &ace_p->icmptype_data, &ace_p->icmptype_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_IcmpType error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_IcmpCode))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_IcmpCode (data:%u, mask:%u)",
                    ace_p->icmpcode_data, ace_p->icmpcode_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ICMP_CODE,
                                           &ace_p->icmpcode_data, &ace_p->icmpcode_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_IcmpCode error");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetRuleEntryFieldForTemplate3
 *------------------------------------------------------------------------------
 * PURPOSE: Set rule entry field for template 3
 * INPUT:   device_id   - device id
 *          phase       - ingress/egress
 *          hw_rule_idx - hardware rule entry index
 *          ace_p       - ace entry
 * OUTPUT:
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetRuleEntryFieldForTemplate3(
    UI32_T device_id,
    DEVRM_PHASE_T phase,
    UI32_T hw_rule_idx,
    const DEVRM_AceEntry_T *ace_p)
{
    int ret;

    DEVRM_LOG("device_id=%lu, phase=%s, hw_rule_idx=%lu",
          device_id, (phase == DEVRM_PHASE_INGRESS) ? "ingress" : "egress", hw_rule_idx);

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_DstIp6))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_DstIp6");

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP6_DIP,
                                           ace_p->dstip6_data, ace_p->dstip6_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_DstIp6 error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_L4SrcPort))
    {
        UI16_T data = (UI16_T)ace_p->l4_srcport_data;
        UI16_T mask = (UI16_T)ace_p->l4_srcport_mask;

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_L4SrcPort (data:%u, mask:%u)",
                    data, mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_L4_SRC_PORT,
                                           (UI8_T *)&data, (UI8_T *)&mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_L4SrcPort error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_L4DstPort))
    {
        UI16_T data = (UI16_T)ace_p->l4_dstport_data;
        UI16_T mask = (UI16_T)ace_p->l4_dstport_mask;

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_L4DstPort (data:%u, mask:%u)",
                    data, mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_L4_DST_PORT,
                                           (UI8_T *)&data, (UI8_T *)&mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_L4DstPort error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_ArpOpcode))
    {
        UI16_T data = (UI16_T)ace_p->arp_opcode_data;
        UI16_T mask = (UI16_T)ace_p->arp_opcode_mask;
        UI8_T  data_8 = 0x0 /* ARP */, mask_8 = 0x3;

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_ArpOpcode (data:%u, mask:%u)",
                    data, mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_FRAME_TYPE,
                                           &data_8, &mask_8);
        if (ret != RT_ERR_OK)
        {
            DEVRM_RETURN_RTK_ERROR_CODE(ret);
        }

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ARPOPCODE,
                                           (UI8_T *)&data, (UI8_T *)&mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_RETURN_RTK_ERROR_CODE(ret);
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_IcmpType))
    {

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_IcmpType (data:%u, mask:%u)",
                    ace_p->icmptype_data, ace_p->icmptype_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ICMP_TYPE,
                                           &ace_p->icmptype_data, &ace_p->icmptype_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_IcmpType error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_IcmpCode))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_IcmpCode (data:%u, mask:%u)",
                    ace_p->icmpcode_data, ace_p->icmpcode_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ICMP_CODE,
                                           &ace_p->icmpcode_data, &ace_p->icmpcode_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_IcmpCode error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_TcpControl))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_TcpControl (data:%u, mask:%u)",
                    ace_p->tcpcontrol_data, ace_p->tcpcontrol_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_TCP_FLAG,
                                           &ace_p->tcpcontrol_data, &ace_p->tcpcontrol_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_TcpControl error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_Ip6NextHeader))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_Ip6NextHeader (data:%u, mask:%u)",
                    ace_p->ip6nextheader_data, ace_p->ip6nextheader_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP4PROTO_IP6NH,
                                           &ace_p->ip6nextheader_data, &ace_p->ip6nextheader_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_Ip6NextHeader error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_Ip6TrafficClass))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_Ip6TrafficClass (data:%u, mask:%u)",
                    ace_p->ip6trafficclass_data, ace_p->ip6trafficclass_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP4TOS_IP6TC,
                                           &ace_p->ip6trafficclass_data, &ace_p->ip6trafficclass_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_Ip6TrafficClass error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_Dscp))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_Dscp (data:%u, mask:%u)",
                    ace_p->dscp_data, ace_p->dscp_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP_DSCP,
                                           &ace_p->dscp_data, &ace_p->dscp_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_Dscp error");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetRuleEntryFieldForTemplate4
 *------------------------------------------------------------------------------
 * PURPOSE: Set rule entry field for template 4
 * INPUT:   device_id   - device id
 *          phase       - ingress/egress
 *          hw_rule_idx - hardware rule entry index
 *          ace_p       - ace entry
 * OUTPUT:
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetRuleEntryFieldForTemplate4(
    UI32_T device_id,
    DEVRM_PHASE_T phase,
    UI32_T hw_rule_idx,
    const DEVRM_AceEntry_T *ace_p)
{
    int ret;

    DEVRM_LOG("device_id=%lu, phase=%s, hw_rule_idx=%lu",
          device_id, (phase == DEVRM_PHASE_INGRESS) ? "ingress" : "egress", hw_rule_idx);

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_InPorts))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_InPorts");
        ret = _DEVRM_SetRuleEntryField_SPM(device_id, phase, hw_rule_idx,
                                           ace_p->inports_data,
                                           ace_p->inports_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_InPorts error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_SrcIp6))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_SrcIp6");

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP6_SIP,
                                           ace_p->srcip6_data, ace_p->srcip6_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_SrcIp6 error");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetRuleEntryFieldForTemplate5
 *------------------------------------------------------------------------------
 * PURPOSE: Set rule entry field for template 5
 * INPUT:   device_id   - device id
 *          phase       - ingress/egress
 *          hw_rule_idx - hardware rule entry index
 *          ace_p       - ace entry
 * OUTPUT:
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetRuleEntryFieldForTemplate5(
    UI32_T device_id,
    DEVRM_PHASE_T phase,
    UI32_T hw_rule_idx,
    const DEVRM_AceEntry_T *ace_p)
{
    int ret;

    DEVRM_LOG("device_id=%lu, phase=%s, hw_rule_idx=%lu",
              device_id, (phase == DEVRM_PHASE_INGRESS) ? "ingress" : "egress", hw_rule_idx);

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_OutPorts))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_OutPorts");
        ret = _DEVRM_SetRuleEntryField_DPM(device_id, phase, hw_rule_idx,
                                           ace_p->outports_data,
                                           ace_p->outports_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_OutPorts error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_OuterVlan))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_OuterVlan (data:%u, mask:%u)",
                    ace_p->outvlan_data, ace_p->outvlan_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_OTAG_VID,
                                           (UI8_T *)&ace_p->outvlan_data, (UI8_T *)&ace_p->outvlan_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_OuterVlan error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_InnerVlan))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_InnerVlan (data:%u, mask:%u)",
                    ace_p->invlan_data, ace_p->invlan_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ITAG_VID,
                                           (UI8_T *)&ace_p->invlan_data, (UI8_T *)&ace_p->invlan_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_InnerVlan error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_OuterVlanPri))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_OuterVlanPri (data:%u, mask:%u)",
                    ace_p->outvlanpri_data, ace_p->outvlanpri_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_OTAG_PRI,
                                           &ace_p->outvlanpri_data, &ace_p->outvlanpri_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_OuterVlanPri error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_SrcMac))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_SrcMac (data:0x%02x%02x%02x%02x%02x%02x, mask:0x%02x%02x%02x%02x%02x%02x)",
                    ace_p->srcmac_data[0], ace_p->srcmac_data[1], ace_p->srcmac_data[2],
                    ace_p->srcmac_data[3], ace_p->srcmac_data[4], ace_p->srcmac_data[5],
                    ace_p->srcmac_mask[0], ace_p->srcmac_mask[1], ace_p->srcmac_mask[2],
                    ace_p->srcmac_mask[3], ace_p->srcmac_mask[4], ace_p->srcmac_mask[5]);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_SMAC,
                                           ace_p->srcmac_data, ace_p->srcmac_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_SrcMac error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_DstMac))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_DstMac (data:0x%02x%02x%02x%02x%02x%02x, mask:0x%02x%02x%02x%02x%02x%02x)",
                    ace_p->dstmac_data[0], ace_p->dstmac_data[1], ace_p->dstmac_data[2],
                    ace_p->dstmac_data[3], ace_p->dstmac_data[4], ace_p->dstmac_data[5],
                    ace_p->dstmac_mask[0], ace_p->dstmac_mask[1], ace_p->dstmac_mask[2],
                    ace_p->dstmac_mask[3], ace_p->dstmac_mask[4], ace_p->dstmac_mask[5]);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_DMAC,
                                           ace_p->dstmac_data, ace_p->dstmac_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_DstMac error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_EtherType))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_EtherType (data:0x%04x, mask:0x%04x)",
                    ace_p->ethertype_data, ace_p->ethertype_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ETHERTYPE,
                                           (UI8_T *)&ace_p->ethertype_data, (UI8_T *)&ace_p->ethertype_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_EtherType error");
            return ret;
        }
    }
    
    return RT_ERR_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetRuleEntryFieldForTemplate6
 *------------------------------------------------------------------------------
 * PURPOSE: Set rule entry field for template 6
 * INPUT:   device_id   - device id
 *          phase       - ingress/egress
 *          hw_rule_idx - hardware rule entry index
 *          ace_p       - ace entry
 * OUTPUT:
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetRuleEntryFieldForTemplate6(
    UI32_T device_id,
    DEVRM_PHASE_T phase,
    UI32_T hw_rule_idx,
    const DEVRM_AceEntry_T *ace_p)
{
    int ret;

    DEVRM_LOG("device_id=%lu, phase=%s, hw_rule_idx=%lu",
              device_id, (phase == DEVRM_PHASE_INGRESS) ? "ingress" : "egress", hw_rule_idx);

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_OutPorts))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_OutPorts");
        ret = _DEVRM_SetRuleEntryField_DPM(device_id, phase, hw_rule_idx,
                                           ace_p->outports_data,
                                           ace_p->outports_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_OutPorts error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_SrcIp))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_SrcIp (data:%lu, mask:%lu)",
                    ace_p->srcip_data, ace_p->srcip_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP4_SIP,
                                           &ace_p->srcip_data, &ace_p->srcip_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_SrcIp error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_DstIp))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_DstIp (data:%lu, mask:%lu)",
                    ace_p->dstip_data, ace_p->dstip_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP4_DIP,
                                           &ace_p->dstip_data, &ace_p->dstip_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_DstIp error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_IpProtocol))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_IpProtocol (data:%u, mask:%u)",
                    ace_p->ipprotocol_data, ace_p->ipprotocol_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP4PROTO_IP6NH,
                                           &ace_p->ipprotocol_data, &ace_p->ipprotocol_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_IpProtocol error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_Dscp))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_Dscp (data:%u, mask:%u)",
                    ace_p->dscp_data, ace_p->dscp_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP_DSCP,
                                           &ace_p->dscp_data, &ace_p->dscp_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_Dscp error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_Tos))
    {
        ASSERT(0);
        DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_Tos error");
        return RT_ERR_FAILED;
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_L4SrcPort))
    {
        UI16_T data = (UI16_T)ace_p->l4_srcport_data;
        UI16_T mask = (UI16_T)ace_p->l4_srcport_mask;

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_L4SrcPort (data:%u, mask:%u)",
                    data, mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_L4_SRC_PORT,
                                           (UI8_T *)&data, (UI8_T *)&mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_L4SrcPort error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_L4DstPort))
    {
        UI16_T data = (UI16_T)ace_p->l4_dstport_data;
        UI16_T mask = (UI16_T)ace_p->l4_dstport_mask;

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_L4DstPort (data:%u, mask:%u)",
                    data, mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_L4_DST_PORT,
                                           (UI8_T *)&data, (UI8_T *)&mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_L4DstPort error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_ArpOpcode))
    {
        UI16_T data = (UI16_T)ace_p->arp_opcode_data;
        UI16_T mask = (UI16_T)ace_p->arp_opcode_mask;
        UI8_T  data_8 = 0x0 /* ARP */, mask_8 = 0x3;

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_ArpOpcode (data:%u, mask:%u)",
                    data, mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_FRAME_TYPE,
                                           &data_8, &mask_8);
        if (ret != RT_ERR_OK)
        {
            DEVRM_RETURN_RTK_ERROR_CODE(ret);
        }

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ARPOPCODE,
                                           (UI8_T *)&data, (UI8_T *)&mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_RETURN_RTK_ERROR_CODE(ret);
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_IcmpType))
    {

        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_IcmpType (data:%u, mask:%u)",
                    ace_p->icmptype_data, ace_p->icmptype_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ICMP_TYPE,
                                           &ace_p->icmptype_data, &ace_p->icmptype_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_IcmpType error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_IcmpCode))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_IcmpCode (data:%u, mask:%u)",
                    ace_p->icmpcode_data, ace_p->icmpcode_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_ICMP_CODE,
                                           &ace_p->icmpcode_data, &ace_p->icmpcode_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_IcmpCode error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_TcpControl))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_TcpControl (data:%u, mask:%u)",
                    ace_p->tcpcontrol_data, ace_p->tcpcontrol_mask);

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_TCP_FLAG,
                                           &ace_p->tcpcontrol_data, &ace_p->tcpcontrol_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_TcpControl error");
            return ret;
        }
    }

    return RT_ERR_OK;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetRuleEntryFieldForTemplate7
 *------------------------------------------------------------------------------
 * PURPOSE: Set rule entry field for template 7
 * INPUT:   device_id   - device id
 *          phase       - ingress/egress
 *          hw_rule_idx - hardware rule entry index
 *          ace_p       - ace entry
 * OUTPUT:
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SetRuleEntryFieldForTemplate7(
    UI32_T device_id,
    DEVRM_PHASE_T phase,
    UI32_T hw_rule_idx,
    const DEVRM_AceEntry_T *ace_p)
{
    int ret;

    DEVRM_LOG("device_id=%lu, phase=%s, hw_rule_idx=%lu",
              device_id, (phase == DEVRM_PHASE_INGRESS) ? "ingress" : "egress", hw_rule_idx);

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_OutPorts))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_OutPorts");
        ret = _DEVRM_SetRuleEntryField_DPM(device_id, phase, hw_rule_idx,
                                           ace_p->outports_data,
                                           ace_p->outports_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_OutPorts error");
            return ret;
        }
    }

    if (DEVRM_SHR_BITGET(ace_p->w, DEVRM_FIELD_QUALIFY_SrcIp6))
    {
        DEVRM_TRACE("DEVRM_FIELD_QUALIFY_SrcIp6");

        ret = rtk_acl_ruleEntryField_write(device_id, (rtk_acl_phase_t) phase,
                                           hw_rule_idx, USER_FIELD_IP6_SIP,
                                           ace_p->srcip6_data, ace_p->srcip6_mask);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("Set field DEVRM_FIELD_QUALIFY_SrcIp6 error");
            return ret;
        }
    }
    
    return RT_ERR_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_GetDevMeterEntry
 *------------------------------------------------------------------------------
 * PURPOSE: Get meter entry on OM
 * INPUT:   device_id -
 *          meter_id  -
 * OUTPUT:
 * RETURN: DEVRM_DevMeter_T * - meter entry point on OM
 * NOTE:
 *------------------------------------------------------------------------------
 */
static DEVRM_DevMeter_T *
_DEVRM_GetDevMeterEntry(
    UI32_T device_id,
    UI32_T meter_id)
{
    DEVRM_DevMeter_T *meter_p;

    if (_countof(devrm_data.dev) <= device_id)
        return NULL;

    if (meter_id <= _countof(devrm_data.dev[device_id].policer.meter))
        return NULL;

    return &devrm_data.dev[device_id].policer.meter[meter_id];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_CreateDevMeter
 *------------------------------------------------------------------------------
 * PURPOSE: Create device meter entry on OM for specify device rule
 * INPUT:   device_id   -
 *          dev_rule_id -
 * OUTPUT:
 * RETURN: TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_CreateDevMeter(
    UI32_T device_id,
    UI32_T dev_rule_id)
{
    UI32_T i, candidate;
    UI32_T group_id;
    const UI32_T total_meter_entry = _countof(devrm_data.dev[device_id].policer.meter);
    DEVRM_DevRule_T *dev_rule_p;

    _DEVRM_GetGroupIdByDevRuleId(dev_rule_id, &group_id);
    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (NULL == dev_rule_p)
    {
        return FALSE;
    }

    candidate = total_meter_entry;

    if (_countof(devrm_data.dev) <= device_id)
        return FALSE;

    for (i = 0; i < total_meter_entry; ++i)
    {
        if (TRUE == devrm_data.dev[device_id].policer.meter[i].valid)
        {
            if (devrm_data.dev[device_id].policer.meter[i].dev_rule_id == dev_rule_id)
            {
                return TRUE;
            }
        }
        else
        {
            if (candidate == total_meter_entry)
            {
                /* Fisrt empty entry */
                candidate = i;
            }
        }
    }

    if (candidate == total_meter_entry)
    {
        return FALSE;
    }

    devrm_data.dev[device_id].policer.meter[candidate].dev_rule_id = dev_rule_id;
    devrm_data.dev[device_id].policer.meter[candidate].id = candidate;
    devrm_data.dev[device_id].policer.meter[candidate].valid = TRUE;

    dev_rule_p->meter_idx = candidate;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_DestroyDevMeter
 *------------------------------------------------------------------------------
 * PURPOSE: Destroy device meter entry on OM
 * INPUT:   device_id -
 *          meter_id  -
 * OUTPUT:
 * RETURN: TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_DestroyDevMeter(
    UI32_T device_id,
    UI32_T dev_rule_id)
{
    UI32_T i, group_id;
    DEVRM_DevRule_T *dev_rule_p;

    _DEVRM_GetGroupIdByDevRuleId(dev_rule_id, &group_id);
    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (NULL == dev_rule_p)
    {
        return FALSE;
    }

    for (i = 0; i < _countof(devrm_data.dev[device_id].policer.meter); ++i)
    {
        DEVRM_DevMeter_T *dev_meter_p = &devrm_data.dev[device_id].policer.meter[i];

        if (TRUE == dev_meter_p->valid &&
            dev_rule_id == dev_meter_p->dev_rule_id)
        {
            memset(dev_meter_p, 0, sizeof(*dev_meter_p));
            dev_meter_p->valid = FALSE;

            dev_rule_p->meter_idx = DEVRM_UNDEF_METER_IDX;
            break;
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetDevMeter
 *------------------------------------------------------------------------------
 * PURPOSE: Set device meter entry
 * INPUT:   device_id   -
 *          dev_rule_id -
 *          mode        - DEVRM_METER_MODE_XXX
 * OUTPUT:
 * RETURN: TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_SetDevMeter(
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_POLICER_METER_T *meter_p)
{
    /* Only support global burst size setting,
     * 2 for packet mode & 2 for byte mode
     */

#define DEVRM_CONVERT_KBITS_RATE_TO_RTK_RATE(kbits) ((kbits + (DEVRM_METER_RATE_MINIMAL_UNITS/2)) / DEVRM_METER_RATE_MINIMAL_UNITS)

    UI32_T group_id;
    int ret;
    DEVRM_DevRule_T *dev_rule_p;
    rtk_acl_meterEntry_t meter_entry;
    rtk_acl_meterMode_t meter_mode;

    DEVRM_LOG("device_id=%lu, dev_rule_id=%lu", device_id, dev_rule_id);
    ASSERT(meter_p != NULL);

    _DEVRM_GetGroupIdByDevRuleId(dev_rule_id, &group_id);
    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (NULL == dev_rule_p)
    {
        DEVRM_ERR("Not find dev_rule_id = %lu", dev_rule_id);
        return FALSE;
    }

    if (dev_rule_p->meter_idx == DEVRM_UNDEF_METER_IDX)
    {
        DEVRM_ERR("Not create meter for dev_rule_id = %lu", dev_rule_id);
        return FALSE;
    }

    memset(&meter_entry, 0, sizeof(meter_entry));

    switch (meter_p->mode)
    {
        case DEVRM_POLICER_METER_MODE_RATE:
            DEVRM_TRACE("METER_MODE_RATE");

            if (!HAL_IS_RTL8380_FAMILY_ID(device_id))
            {
                DEVRM_ERR("Not support this meter mode");
                return FALSE;
            }

            meter_entry.enable = ENABLED;
            meter_entry.thr_grp = 0;

            if (meter_p->type == DEVRM_POLICER_METER_TYPE_BYTE)
            {
                /* Chip use the 16 kbps as unit, rounding the configured rate.
                 */
                meter_entry.rate = DEVRM_CONVERT_KBITS_RATE_TO_RTK_RATE(meter_p->kbits_sec);
                DEVRM_TRACE("[Action:meter] Byte, rate = %lu", meter_entry.rate);
            }
            else
            {
                meter_entry.rate = meter_p->packet_sec;
                DEVRM_TRACE("[Action:meter] Packet, rate = %lu", meter_entry.rate);
            }
            break;

        case DEVRM_POLICER_METER_MODE_FLOW:
            DEVRM_TRACE("METER_MODE_FLOW");
            if (!HAL_IS_RTL8390_FAMILY_ID(device_id))
            {
                DEVRM_ERR("Not support this meter mode");
                return FALSE;
            }

            meter_entry.type = METER_TYPE_DLB;
            meter_entry.color_aware = DISABLED;

            if (meter_p->type == DEVRM_POLICER_METER_TYPE_BYTE)
            {
                /* Chip use the 16 kbps as unit, rounding the configured rate.
                 */
                meter_entry.lb0_rate = DEVRM_CONVERT_KBITS_RATE_TO_RTK_RATE(meter_p->kbits_sec);
                meter_entry.lb1_rate = DEVRM_CONVERT_KBITS_RATE_TO_RTK_RATE(meter_p->kbits_sec);
                DEVRM_TRACE("[Action:meter] Byte, rate = %lu", meter_entry.lb0_rate);
            }
            else
            {
                meter_entry.lb0_rate = meter_p->packet_sec;
                meter_entry.lb1_rate = meter_p->packet_sec;
                DEVRM_TRACE("[Action:meter] Packet, rate = %lu", meter_entry.lb0_rate);
            }

            break;

        case DEVRM_POLICER_METER_MODE_TRTCM_COLOR_AWARE:
        case DEVRM_POLICER_METER_MODE_TRTCM_COLOR_BLIND:
        case DEVRM_POLICER_METER_MODE_SRTCM_COLOR_AWARE:
        case DEVRM_POLICER_METER_MODE_SRTCM_COLOR_BLIND:
        {
            BOOL_T is_blind = (meter_p->mode == DEVRM_POLICER_METER_MODE_TRTCM_COLOR_BLIND ||
                               meter_p->mode == DEVRM_POLICER_METER_MODE_SRTCM_COLOR_BLIND)
                            ? TRUE : FALSE;
            BOOL_T is_trtcm = (meter_p->mode == DEVRM_POLICER_METER_MODE_TRTCM_COLOR_BLIND ||
                               meter_p->mode == DEVRM_POLICER_METER_MODE_TRTCM_COLOR_AWARE)
                              ? TRUE : FALSE;

            DEVRM_TRACE("METER_MODE_%s", (is_trtcm) ? "TRTCM" : "SRTCM");
            if (!HAL_IS_RTL8390_FAMILY_ID(device_id))
            {
                DEVRM_ERR("Not support this meter mode");
                return FALSE;
            }

            meter_entry.type = is_trtcm ? METER_TYPE_TRTCM : METER_TYPE_SRTCM;
            meter_entry.color_aware = is_blind ? DISABLED : ENABLED;

            if (meter_p->type == DEVRM_POLICER_METER_TYPE_BYTE)
            {
                /* Chip use the 16 kbps as unit, rounding the configured rate.
                 */
                meter_entry.lb0_rate = DEVRM_CONVERT_KBITS_RATE_TO_RTK_RATE(meter_p->kbits_sec);
                meter_entry.lb1_rate = DEVRM_CONVERT_KBITS_RATE_TO_RTK_RATE(meter_p->peak_kbits_sec); /* ignore while SRTCM */

                DEVRM_TRACE("[Action:meter] Byte, CIR rate = %lu", meter_entry.lb0_rate);
                if (is_trtcm)
                {
                    DEVRM_TRACE("[Action:meter]     , PIR rate = %lu", meter_entry.lb1_rate);
                }
            }
            else
            {
                meter_entry.lb0_rate = meter_p->packet_sec;
                meter_entry.lb1_rate = meter_p->peak_packet_sec; /* ignore while SRTCM */
                DEVRM_TRACE("[Action:meter] Packet, rate = %lu", meter_entry.lb0_rate);
                if (is_trtcm)
                {
                    DEVRM_TRACE("[Action:meter]     , PIR rate = %lu", meter_entry.lb1_rate);
                }
            }
        }
            break;

        default:
            ASSERT(0);
            DEVRM_ERR("Not support this meter mode");
            return FALSE;
    }

    ret = rtk_acl_meterEntry_set(device_id, dev_rule_p->meter_idx, &meter_entry);
    ASSERT(ret == RT_ERR_OK);

    meter_mode = (meter_p->type == DEVRM_POLICER_METER_TYPE_PACKET) ? METER_MODE_PACKET : METER_MODE_BYTE;
    if (HAL_IS_RTL8380_FAMILY_ID(device_id))
    {
        ret = rtk_acl_meterMode_set(device_id, dev_rule_p->meter_idx, meter_mode);
        ASSERT(ret == RT_ERR_OK);
    }
    else if (HAL_IS_RTL8390_FAMILY_ID(device_id))
    {
        ret = rtk_acl_meterMode_set(device_id,
                                    (dev_rule_p->meter_idx / DEVRM_MAX_NBR_OF_METER_PER_SLICE),
                                    meter_mode);
        ASSERT(ret == RT_ERR_OK);
    }
    else
    {
        ASSERT(0);
    }

    return TRUE;

#undef DEVRM_CONVERT_KBITS_RATE_TO_RTK_RATE
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_GetDevCounterEntry
 *------------------------------------------------------------------------------
 * PURPOSE: Get counter entry on OM
 * INPUT:   device_id -
 *          counter_id  -
 * OUTPUT:
 * RETURN: DEVRM_DevCounter_T * - counter entry point on OM
 * NOTE:
 *------------------------------------------------------------------------------
 */
static DEVRM_DevCounter_T *
_DEVRM_GetDevCounterEntry(
    UI32_T device_id,
    UI32_T counter_id)
{
    DEVRM_DevCounter_T *counter_p;

    if (_countof(devrm_data.dev) <= device_id)
        return NULL;

    if (_countof(devrm_data.dev[device_id].policer.counter) <= counter_id)
        return NULL;

    return &devrm_data.dev[device_id].policer.counter[counter_id];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_CreateDevCounter
 *------------------------------------------------------------------------------
 * PURPOSE: Create device counter entry on OM for specify device rule
 * INPUT:   device_id   -
 *          dev_rule_id -
 * OUTPUT:
 * RETURN: TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_CreateDevCounter(
    UI32_T device_id,
    UI32_T dev_rule_id)
{
    UI32_T i, candidate;
    UI32_T group_id;
    const UI32_T total_counter_entry = _countof(devrm_data.dev[device_id].policer.counter);
    DEVRM_DevRule_T *dev_rule_p;

    _DEVRM_GetGroupIdByDevRuleId(dev_rule_id, &group_id);
    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (NULL == dev_rule_p)
    {
        return FALSE;
    }

    candidate = total_counter_entry;

    if (_countof(devrm_data.dev) <= device_id)
        return FALSE;

    for (i = 0; i < total_counter_entry; ++i)
    {
        if (TRUE == devrm_data.dev[device_id].policer.counter[i].valid)
        {
            if (devrm_data.dev[device_id].policer.counter[i].dev_rule_id == dev_rule_id)
            {
                return TRUE;
            }
        }
        else
        {
            if (candidate == total_counter_entry)
            {
                /* Fisrt empty entry */
                candidate = i;
            }
        }
    }

    if (candidate == total_counter_entry)
    {
        return FALSE;
    }

    devrm_data.dev[device_id].policer.counter[candidate].dev_rule_id = dev_rule_id;
    devrm_data.dev[device_id].policer.counter[candidate].id = candidate;
    devrm_data.dev[device_id].policer.counter[candidate].valid = TRUE;
    
    dev_rule_p->counter_idx = candidate;
    
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_DestroyDevCounter
 *------------------------------------------------------------------------------
 * PURPOSE: Destroy device counter entry on OM
 * INPUT:   device_id -
 *          dev_rule_id  -
 * OUTPUT:
 * RETURN: TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_DestroyDevCounter(
    UI32_T device_id,
    UI32_T dev_rule_id)
{
    UI32_T i, group_id;
    DEVRM_DevRule_T *dev_rule_p;

    _DEVRM_GetGroupIdByDevRuleId(dev_rule_id, &group_id);
    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (NULL == dev_rule_p)
    {
        return FALSE;
    }

    for (i = 0; i < _countof(devrm_data.dev[device_id].policer.counter); ++i)
    {
        DEVRM_DevCounter_T *dev_counter_p = &devrm_data.dev[device_id].policer.counter[i];

        if (TRUE == dev_counter_p->valid &&
            dev_rule_id == dev_counter_p->dev_rule_id)
        {
            memset(dev_counter_p, 0, sizeof(*dev_counter_p));
            dev_counter_p->valid = FALSE;

            dev_rule_p->counter_idx = DEVRM_UNDEF_COUNTER_IDX;
            break;
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_FindDevRule
 *------------------------------------------------------------------------------
 * PURPOSE: Find device rule entry at OM
 * INPUT:   device_id      -
 *          group_id       -
 *          device_rule_id - (reference to _DEVRM_CreateDevRuleEntry)
 * OUTPUT:
 * RETURN:  DEVRM_DevRule_T *, point to OM
 * NOTE:
 *------------------------------------------------------------------------------
 */
static DEVRM_DevRule_T *
_DEVRM_FindDevRule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T device_rule_id)
{
    DEVRM_Group_T *gp_p;
    int i;

    ASSERT(device_id < _countof(devrm_data.dev));

    gp_p = _DEVRM_GetGroupById(device_id, group_id);
    if (gp_p == NULL)
    {
        return NULL;
    }

    for (i = 0; i < _countof(gp_p->drv.rule_tbl); ++i)
    {
        DEVRM_DevRule_T *dev_rule_p = &gp_p->drv.rule_tbl[i];

        if (dev_rule_p->valid == TRUE && dev_rule_p->id == device_rule_id)
        {
            return dev_rule_p;
        }
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SetDevRulePriority
 *------------------------------------------------------------------------------
 * PURPOSE: Set device rule priority
 * INPUT:   device_id   -
 *          group_id    -
 *          dev_rule_id -
 *          priority    -
 * OUTPUT:
 * RETURN:  TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_SetDevRulePriority(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id,
    int priority)
{
    DEVRM_DevRule_T *dev_rule_p;

    if (priority == DEVRM_MIN_RULE_PRIORITY)
    {
        return FALSE;
    }

    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (dev_rule_p == NULL || dev_rule_p->valid == FALSE)
    {
        return FALSE;
    }

    dev_rule_p->dr_pri = priority;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_CreateDevRuleEntry
 *------------------------------------------------------------------------------
 * PURPOSE: Create device rule entry in group on OM
 * INPUT:   device_id -
 *          group_id  -
 * OUTPUT:  device_rule_id_p - The created device rule id
 * RETURN:  TRUE/FALSE
 * NOTE:    1. device_rule_id is create by following rule
 *             [32:8]group_id [7:0]per-group device rule array index
 *          2. A device rule may reference to [1/2/4] hw rule entry,
 *             decide by group's rule_size and group's slice_num
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_CreateDevRuleEntry(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T *dev_rule_id_p)
{
    DEVRM_Group_T *grp_p;
    UI32_T i;

    grp_p = _DEVRM_GetGroupById(device_id, group_id);
    if (grp_p == NULL || grp_p->valid == FALSE)
    {
        return FALSE;
    }

    if (DEVRM_MAX_NBR_OF_ENTRY_PER_SLICE < (1 + grp_p->drv.cur_cnt) * grp_p->rule_size)
    {
        return FALSE;
    }

    for (i = 0; i < (_countof(grp_p->drv.rule_tbl) / grp_p->rule_size); ++i)
    {
        DEVRM_DevRule_T * dev_rule_p = &grp_p->drv.rule_tbl[i];

        if (dev_rule_p->valid == FALSE)
        {
            memset(dev_rule_p, 0, sizeof(*dev_rule_p));
            dev_rule_p->valid = TRUE;
            dev_rule_p->id = *dev_rule_id_p
                     = (group_id << 8) | i;
            dev_rule_p->dr_pri = DEVRM_MIN_RULE_PRIORITY;
            dev_rule_p->meter_idx = DEVRM_UNDEF_METER_IDX;
            dev_rule_p->counter_idx = DEVRM_UNDEF_COUNTER_IDX;

            /* Using the last slice rule index
             */
            dev_rule_p->slice_rule_index = grp_p->drv.cur_cnt * grp_p->rule_size;

            grp_p->drv.cur_cnt++;

            return TRUE;
        }
    }

    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_DestroyDevRuleEntry
 *------------------------------------------------------------------------------
 * PURPOSE: Destroy device rule entry in group on OM
 * INPUT:   device_id   -
 *          group_id    -
 *          dev_rule_id -
 * OUTPUT:
 * RETURN:  TRUE/FALSE
 * NOTE:    This function will also set the hw rule status to invalid.
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_DestroyDevRuleEntry(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id)
{
    DEVRM_Group_T *grp_p;
    DEVRM_DevRule_T *dev_rule_p;
    UI32_T i, j;

    grp_p = _DEVRM_GetGroupById(device_id, group_id);
    if (grp_p == NULL || grp_p->valid == FALSE)
    {
        return FALSE;
    }

    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (dev_rule_p == NULL)
    {
        return TRUE;
    }

    for (i = 0; i < grp_p->slice_num; ++i)
    {
        DEVRM_Slice_T *slice_p;

        slice_p = _DEVRM_GetSliceByIdx(device_id, grp_p->slice_id + i);
        if (NULL == slice_p)
        {
            ASSERT(0);
            return FALSE;
        }

        ASSERT(slice_p->template_num == grp_p->rule_size);

        for (j = 0; j < slice_p->template_num; ++j)
        {
            UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);
            rtk_acl_clear_t clear_info;
            int ret;

            //ret = rtk_acl_ruleValidate_set(device_id, (rtk_acl_phase_t) grp_p->phase, hw_rule_idx, DISABLED);
            clear_info.start_idx = hw_rule_idx;
            clear_info.end_idx = hw_rule_idx;
            ret = rtk_acl_rule_del(device_id, (rtk_acl_phase_t) grp_p->phase, &clear_info);
            if (ret != RT_ERR_OK)
            {
                /* show error message ?
                 */
            }

            slice_p->cur_rule_cnt--;
        }
    }

    /* Not last rule entry in this group
     */
    if (dev_rule_p->slice_rule_index < (grp_p->drv.cur_cnt - 1) * grp_p->rule_size)
    {
        DEVRM_MoveParam_T move;

        move.start = dev_rule_p->slice_rule_index + grp_p->rule_size;
        move.end = (grp_p->drv.cur_cnt * grp_p->rule_size) - 1;
        move.shift = -1 * grp_p->rule_size;

        if (FALSE == _DEVRM_MoveSliceRuleAndUpdateGroup(device_id, grp_p, &move))
        {
            // FIXEME: Rollback hw rule ??
            return FALSE;
        }
    }

    if (dev_rule_p->meter_idx != DEVRM_UNDEF_METER_IDX)
    {
        _DEVRM_DestroyDevMeter(device_id, dev_rule_id);
    }

    if (dev_rule_p->counter_idx != DEVRM_UNDEF_COUNTER_IDX)
    {
        _DEVRM_DestroyDevCounter(device_id, dev_rule_id);
    }

    memset(dev_rule_p, 0, sizeof(*dev_rule_p));
    dev_rule_p->valid = FALSE;
    grp_p->drv.cur_cnt--;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_IsGroupExisted
 *------------------------------------------------------------------------------
 * PURPOSE  : Check group whether existed
 * INPUT    : device_id - device ID
 *            group_id  - group ID
 * OUTPUT   : NONE.
 * RETURN   : TRUE if existed, otherwise return FALSE
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_IsGroupExisted(
    UI32_T device_id,
    UI32_T group_id)
{
    DEVRM_Group_T *grp_p;

    grp_p = _DEVRM_GetGroupById(device_id, group_id);
    if (NULL == grp_p)
    {
        return FALSE;
    }

    if (grp_p->valid == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_GetGroupById
 *------------------------------------------------------------------------------
 * PURPOSE: Get group entry on OM by id
 * INPUT:   device_id  -
 *          id         - group id
 * OUTPUT:
 * RETURN:  DEVRM_Group_T *, point to OM
 * NOTE:
 *------------------------------------------------------------------------------
 */
static DEVRM_Group_T *
_DEVRM_GetGroupById(
    UI32_T device_id,
    UI32_T id)
{
    if (_countof(devrm_data.dev) <= device_id)
    {
        return NULL;
    }

    if (_countof(devrm_data.dev[device_id].group.group_tbl) <= id)
    {
        return NULL;
    }

    return &devrm_data.dev[device_id].group.group_tbl[id];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_GetSliceNumberByMode
 *------------------------------------------------------------------------------
 * PURPOSE: Get how many slice is used in this group mode
 * INPUT:   group_mode - DEVRM_GROUP_MODE_SINGLE
 *                       DEVRM_GROUP_MODE_DOUBLE
 *                       DEVRM_GROUP_MODE_QUADRUPLE
 * OUTPUT:  num_p      - number of slice using.
 * RETURN:  TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_GetSliceNumberByMode(
    UI32_T group_mode,
    UI32_T *num_p)
{
    switch (group_mode)
    {
        case DEVRM_GROUP_MODE_QUADRUPLE:
            *num_p = 2;
            break;

        case DEVRM_GROUP_MODE_DOUBLE:
            *num_p = 1;
            break;

        case DEVRM_GROUP_MODE_SINGLE:
            *num_p = 1;
            break;
        default:
            ASSERT(0);
            return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_GetRuleSizeByMode
 *------------------------------------------------------------------------------
 * PURPOSE: Get how many rule entries is used in this group mode
 * INPUT:   group_mode - DEVRM_GROUP_MODE_SINGLE
 *                       DEVRM_GROUP_MODE_DOUBLE
 *                       DEVRM_GROUP_MODE_QUADRUPLE
 * OUTPUT:  size_p     - size of rule entry using
 * RETURN:  TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_GetRuleSizeByMode(
    UI32_T group_mode,
    UI32_T *size_p)
{
    switch (group_mode)
    {
        case DEVRM_GROUP_MODE_QUADRUPLE:
            *size_p = 2;
            break;

        case DEVRM_GROUP_MODE_DOUBLE:
            *size_p = 2;
            break;

        case DEVRM_GROUP_MODE_SINGLE:
            *size_p = 1;
            break;

        default:
            ASSERT(0);
            return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_GetGroupIdByDevRuleId
 *------------------------------------------------------------------------------
 * PURPOSE: Get group id by device rule id
 * INPUT:   device_rule_id
 * OUTPUT:  group_id_p
 * RETURN:  TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_GetGroupIdByDevRuleId(
    UI32_T device_rule_id,
    UI32_T *group_id_p)
{
    *group_id_p = device_rule_id >> 8;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_CreateSlice
 *------------------------------------------------------------------------------
 * PURPOSE: Create slice
 * INPUT:   device_id -
 *          slice_idx - slice entry index in OM array
 * OUTPUT:
 * RETURN:  DEVRM_Slice_T *, point to OM entry
 * NOTE:    This function will also initial(enable) the hw slice
 *------------------------------------------------------------------------------
 */
static DEVRM_Slice_T *
_DEVRM_CreateSlice(
    UI32_T device_id,
    UI32_T slice_idx)
{
    int ret;
    DEVRM_Slice_T *slice_p = NULL;

    slice_p = _DEVRM_GetSliceByIdx(device_id, slice_idx);
    if (slice_p == NULL || slice_p->valid == TRUE)
    {
        return NULL;
    }

    ret = rtk_acl_blockLookupEnable_set(device_id, slice_idx, ENABLED);
    if(RT_ERR_OK != ret)
    {
        DEVRM_ERR("rtk_acl_blockLookupEnable_set");
        return NULL;
    }

    ret = rtk_acl_blockPwrEnable_set(device_id, slice_idx, ENABLED);
    if(RT_ERR_OK != ret)
    {
        DEVRM_ERR("rtk_acl_blockPwrEnable_set");
        return NULL;
    }

    memset(slice_p, 0, sizeof(*slice_p));
    slice_p->idx = slice_idx;
    slice_p->valid = TRUE;

    if (slice_idx < DEVRM_MAX_NBR_OF_IACL_SLICE)
    {
        devrm_data.dev[device_id].slice.ingress.cur_cnt++;
    }
    else
    {
        devrm_data.dev[device_id].slice.egress.cur_cnt++;
    }

    devrm_data.dev[device_id].slice.cur_cnt++;

    return slice_p;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_DestroySlice
 *------------------------------------------------------------------------------
 * PURPOSE: Destory slice
 * INPUT:   device_id -
 *          slice_idx - slice entry index in OM array
 * OUTPUT:
 * RETURN:  TRUE/FALSE
 * NOTE:    This function will also initial(disable) the hw slice
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_DestroySlice(
    UI32_T device_id,
    UI32_T slice_idx)
{
    int ret;
    DEVRM_Slice_T *slice_p = NULL;

    slice_p = _DEVRM_GetSliceByIdx(device_id, slice_idx);
    if (slice_p == NULL)
    {
        return FALSE;
    }

    if (slice_p->valid == TRUE)
    {
        ret = rtk_acl_blockLookupEnable_set(device_id, slice_idx, DISABLED);
        if(RT_ERR_OK != ret)
        {
            DEVRM_ERR("rtk_acl_blockLookupEnable_set");
            return FALSE;
        }

        ret = rtk_acl_blockPwrEnable_set(device_id, slice_idx, DISABLED);
        if(RT_ERR_OK != ret)
        {
            DEVRM_ERR("rtk_acl_blockPwrEnable_set");
            return FALSE;
        }

        memset(slice_p, 0, sizeof(*slice_p));

        if (slice_idx < DEVRM_MAX_NBR_OF_IACL_SLICE)
        {
            devrm_data.dev[device_id].slice.ingress.cur_cnt--;
        }
        else

        {
            devrm_data.dev[device_id].slice.egress.cur_cnt--;
        }

        devrm_data.dev[device_id].slice.cur_cnt--;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_GetSliceByIdx
 *------------------------------------------------------------------------------
 * PURPOSE: Get slice entry on OM by index
 * INPUT:   device_id -
 *          idx       - slice index in OM array
 * OUTPUT:
 * RETURN:  DEVRM_Slice_T *, point to OM
 * NOTE:
 *------------------------------------------------------------------------------
 */
static DEVRM_Slice_T *
_DEVRM_GetSliceByIdx(
    UI32_T device_id,
    UI32_T idx)
{
    if (_countof(devrm_data.dev) <= device_id)
    {
        return NULL;
    }

    if (_countof(devrm_data.dev[device_id].slice.slice_tbl) <= idx)
    {
        return NULL;
    }

    return &devrm_data.dev[device_id].slice.slice_tbl[idx];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_FindVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Find virtual group by ID
 * INPUT    : device_id    - device ID
 *            vir_group_id - virtual group ID
 * OUTPUT   : NONE
 * RETURN   : Pointer of a virtual group object
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_VirtualGroup_T *
_DEVRM_FindVirtualGroup(
    UI32_T device_id,
    UI32_T vir_group_id)
{
    DEVRM_VirtualGroup_T *vg_p;
    UI32_T i;

    if (_countof(devrm_data.dev) <= device_id)
    {
        return NULL;
    }

    for (i = 0; i < devrm_data.dev[device_id].vir_grp.vsel_num; ++i)
    {
        vg_p = _DEVRM_GetVirtualGroupAtIndex(device_id, i);

        if (TRUE == vg_p->valid &&
            vir_group_id == vg_p->vir_group_id)
        {
            return vg_p;
        }
    }

    return NULL;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_GetVirtualGroupAtIndex
 *------------------------------------------------------------------------------
 * PURPOSE  : Get virtual group by array index
 * INPUT    : device_id - device ID
 *            idx       - array index
 * OUTPUT   : NONE
 * RETURN   : Pointer of a virtual group object
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_VirtualGroup_T *
_DEVRM_GetVirtualGroupAtIndex(
    UI32_T device_id,
    UI32_T idx)
{
    if (_countof(devrm_data.dev) <= device_id)
    {
        return NULL;
    }

    if (_countof(devrm_data.dev[device_id].vir_grp.vg_tbl) <= idx)
    {
        return NULL;
    }

    return &devrm_data.dev[device_id].vir_grp.vg_tbl[idx];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SortVirtualGroup_Compare
 *------------------------------------------------------------------------------
 * PURPOSE: Compare the virtual group priority
 * INPUT:   arg_1 - DEVRM_VirtualGroup_T
 *          arg_2 - DEVRM_VirtualGroup_T
 * OUTPUT:
 * RETURN:
 * NOTE:    arg_1 > arg_2, return -1
 *          arg_1 < arg_2, return  1
 *          arg_1 = arg_2, return  0
 *
 *          Compare rule to decide whice bigger is as following
 *            1. valid    (high -> low)
 *            2. group_id (low  -> high)
 *            3. pri_lvl  (high -> low)
 *            4. seq_no   (low  -> high)
 *------------------------------------------------------------------------------
 */
static int
_DEVRM_SortVirtualGroup_Compare(
    const void *arg_1,
    const void *arg_2)
{
    const DEVRM_VirtualGroup_T *vg_1 = (const DEVRM_VirtualGroup_T *) arg_1;
    const DEVRM_VirtualGroup_T *vg_2 = (const DEVRM_VirtualGroup_T *) arg_2;

    if (vg_1->valid < vg_2->valid)
    {
        return 1;
    }
    else if (vg_2->valid < vg_1->valid)
    {
        return -1;
    }

    if (vg_1->group_id < vg_2->group_id)
    {
        return -1;
    }
    else if (vg_2->group_id < vg_1->group_id)
    {
        return 1;
    }

    if (vg_1->pri_lvl < vg_2->pri_lvl)
    {
        return 1;
    }
    else if (vg_2->pri_lvl < vg_1->pri_lvl)
    {
        return -1;
    }

    if (vg_1->seq_no < vg_2->seq_no)
    {
        return -1;
    }
    else if (vg_2->seq_no < vg_1->seq_no)
    {
        return 1;
    }

    return 0;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_SortVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Sort virtual group by priority (high -> low)
 * INPUT    : device_id - device ID
 * OUTPUT   : NONE
 * RETURN   : TRUE/FALSE
 * NOTE     : 1. Sort virtural group by
 *               valid(H>L), group_id(L->H), pri_lvl(H->L), seq_no(L->H)
 *            2. Fill vg_p->pri_base on each group 0, -128, -256, ...
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_SortVirtualGroup(
    UI32_T device_id)
{
    DEVRM_VirtualGroup_T     *vg_p;
    DEVRM_VirtualGroup_T     *vg_2_p;
    UI32_T i;

    qsort(devrm_data.dev[device_id].vir_grp.vg_tbl,
          devrm_data.dev[device_id].vir_grp.vsel_num,
          sizeof(devrm_data.dev[device_id].vir_grp.vg_tbl[0]),
          _DEVRM_SortVirtualGroup_Compare);

    /* Assign priority base
     */
    {
        int base = 0;
        for (i = 0; i < devrm_data.dev[device_id].vir_grp.vsel_num; ++i)
        {
            vg_p = _DEVRM_GetVirtualGroupAtIndex(device_id, i);
            ASSERT(vg_p != NULL);

            vg_p->pri_base = base;

            if (i+1 < devrm_data.dev[device_id].vir_grp.vsel_num)
            {
                vg_2_p = _DEVRM_GetVirtualGroupAtIndex(device_id, i+1);
                ASSERT(vg_2_p != NULL);

                if (vg_2_p->group_id != vg_p->group_id)
                {
                    base = 0;
                }
                else
                {
                    base -= SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR;
                }
            }
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_AddRuleToVirtualGroupAndSetPriority
 *------------------------------------------------------------------------------
 * PURPOSE  : Add rule into a virtual group
 * INPUT    : device_id     - device ID
 *            group_id      - group ID
 *            vir_group_id  - virtual group ID
 *            rule_pri      - rule priority
 *            dev_rule_id   - device rule ID
 * OUTPUT   : NONE
 * RETURN   : TRUE/FALSE
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_AddRuleToVirtualGroupAndSetPriority(
    UI32_T device_id,
    UI32_T vir_group_id,
    int rule_pri,
    UI32_T dev_rule_id)
{
    DEVRM_VirtualGroup_T *vg_p = _DEVRM_FindVirtualGroup(device_id, vir_group_id);
    UI32_T vg_rule_idx;
    UI32_T i;

    if (NULL == vg_p)
    {
        DEVRM_ERR("_DEVRM_FindVirtualGroup(soc=%lu, vir_group_id=%lu) failed",
                  device_id, vir_group_id);
        return FALSE;
    }

    if (vg_p->rv.cur_rule_nbr == _countof(vg_p->rv.rule_tbl))
    {
        DEVRM_ERR("No more rule");
        return FALSE;
    }

    if (FALSE == _DEVRM_CalculateIndexForNewRuleByPriority(&vg_p->rv, rule_pri, &vg_rule_idx))
    {
        _DEVRM_CompactRule(&vg_p->rv);

        if (FALSE == _DEVRM_CalculateIndexForNewRuleByPriority(&vg_p->rv, rule_pri, &vg_rule_idx))
        {
            _DEVRM_DumpRuleVector(&vg_p->rv, FALSE);
            return FALSE;
        }
    }

    if (FALSE == _DEVRM_InsertRuleAtIndex(device_id, vg_p, vg_rule_idx, rule_pri, dev_rule_id))
    {
        return FALSE;
    }

    for (i = 0; i < _countof(vg_p->rv.rule_tbl); ++i)
    {
        int pri;
        DEVRM_Rule_T *rule_p = &vg_p->rv.rule_tbl[i];

        if (TRUE == rule_p->valid || TRUE == rule_p->dirty_bit)
        {
            /*
             * Convert index to priority
             */
            pri = vg_p->pri_base - i;

            if (FALSE == _DEVRM_SetDevRulePriority(device_id, vg_p->group_id, rule_p->rule_id, pri))
            {
                DEVRM_ERR("_DEVRM_SetDevRulePriority(device_id=%lu, group_id=%lu, rule_id=%lu, pri=%d)",
                          device_id, vg_p->group_id, rule_p->rule_id, pri);
            }
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_RemoveRuleFromVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Remove rule from a virtual group
 * INPUT    : device_id     - device ID
 *            group_id      - group ID
 *            vir_group_id  - virtual group ID
 *            dev_rule_id   - device rule ID
 * OUTPUT   : NONE
 * RETURN   : TRUE/FALSE
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_RemoveRuleFromVirtualGroup(
    UI32_T device_id,
    UI32_T vir_group_id,
    UI32_T dev_rule_id)
{
    DEVRM_VirtualGroup_T *vg_p = _DEVRM_FindVirtualGroup(device_id, vir_group_id);
    UI32_T rule_idx;

    if (NULL == vg_p)
    {
        DEVRM_ERR("DEVRM_GetVSelectorBySubSelector(soc=%lu, vir_group_id=%lu) failed",
                  device_id, vir_group_id);
        return FALSE;
    }

    if (FALSE == _DEVRM_FindRuleIndexByRuleId(&vg_p->rv, dev_rule_id, &rule_idx))
    {
        DEVRM_ERR("No found old rule, device_id=%lu, vir_group_id=%lu, dev_rule_id=%lu",
                  device_id, vir_group_id, dev_rule_id);
        return FALSE;
    }

    return _DEVRM_RemoveRuleAtIndex(&vg_p->rv, rule_idx);

}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_CalculateIndexForNewRuleByPriority
 *------------------------------------------------------------------------------
 * PURPOSE: Calculate the new vitual group rule index by input priority
 * INPUT:   rv_p     - vitual group rule array
 *          rule_pri - priority of new rule
 * OUTPUT:  rule_idx_p - The index of new rule to insert
 * RETURN:  TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_CalculateIndexForNewRuleByPriority(
    const DEVRM_RuleVector_T *rv_p,
    int rule_pri,
    UI32_T *rule_idx_p)
{
#define INVALID_INDEX (_countof(rv_p->rule_tbl))

    UI32_T i;
    UI32_T candidate = INVALID_INDEX;

    for (i = 0; i < _countof(rv_p->rule_tbl); ++i)
    {
        const DEVRM_Rule_T *rule_p = &rv_p->rule_tbl[i];

        if (TRUE == rule_p->valid)
        {
            if (rule_pri <= rule_p->rule_pri)
            {
                candidate = INVALID_INDEX;
                continue;
            }
            else /* rule_pri > rule_p->rule_pri */
            {
                if (INVALID_INDEX == candidate)
                {
                    candidate = i;
                }
                break;
            }
        }
        else
        {
            if (INVALID_INDEX == candidate)
            {
                candidate = i;
            }
        }
    }

    if (INVALID_INDEX == candidate)
    {
        return FALSE;
    }

    *rule_idx_p = candidate;

    return TRUE;

#undef INVALID_INDEX
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_CompactRule
 *------------------------------------------------------------------------------
 * PURPOSE: Compact virtual group rule
 * INPUT:   rv_p - vitrual group array
 * OUTPUT:
 * RETURN:  TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_CompactRule(
    DEVRM_RuleVector_T *rv_p)
{
    UI32_T i;
    UI32_T empty_idx;
    UI32_T used_idx;

    if (NULL == rv_p)
    {
        DEVRM_ERR("NULL pointer");
        return FALSE;
    }

    empty_idx = 0;

    while (TRUE)
    {
        /* Find index for an empty entry
         */
        for (i = empty_idx; i < _countof(rv_p->rule_tbl); ++i)
        {
            if (FALSE == rv_p->rule_tbl[i].valid)
            {
                break;
            }
        }

        if (_countof(rv_p->rule_tbl) <= i)
        {
            break;
        }

        empty_idx = i;

        /* Find index for a used entry after entry entry
         */
        for (i = empty_idx+1; i<_countof(rv_p->rule_tbl); ++i)
        {
            if (TRUE == rv_p->rule_tbl[i].valid)
            {
                break;
            }
        }

        if (_countof(rv_p->rule_tbl) <= i)
        {
            break;
        }

        used_idx = i;

        rv_p->rule_tbl[empty_idx] = rv_p->rule_tbl[used_idx];
        rv_p->rule_tbl[empty_idx].dirty_bit = TRUE;

        memset(&rv_p->rule_tbl[used_idx], 0, sizeof(rv_p->rule_tbl[used_idx]));
        rv_p->rule_tbl[used_idx].valid = FALSE;

        ++ empty_idx;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_InsertRuleAtIndex
 *------------------------------------------------------------------------------
 * PURPOSE: Insert virtual group rule at specify index
 * INPUT:   device_id   -
 *          vg_p        - virtual group
 *          rule_idx    - virtual group rule index
 *          rule_pri    - virtual group rule priority
 *          dev_rule_id - device rule id to insert
 * OUTPUT:
 * RETURN:  TRUE/FALSE
 * NOTE:    1. Insert rule at virtual group, move the old virtual rule(OM and hw)
 *          2. For the moved device_rule, update their hw rule entry index
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_InsertRuleAtIndex(
    UI32_T device_id,
    DEVRM_VirtualGroup_T *vg_p,
    UI32_T rule_idx,
    int rule_pri,
    UI32_T dev_rule_id)
{
    DEVRM_Group_T *group_p = NULL;
    DEVRM_DevRule_T * dev_rule_p = NULL;
    DEVRM_RuleVector_T *rv_p = &vg_p->rv;
    DEVRM_Rule_T *rule_p;
    UI32_T insert_slice_rule_idx = 0;

    if (_countof(rv_p->rule_tbl) <= rule_idx)
    {
        DEVRM_ERR("Invalid rule_idx=%lu", rule_idx);
        return FALSE;
    }

    if (_countof(rv_p->rule_tbl) == rv_p->cur_rule_nbr)
    {
        DEVRM_ERR("No more rule");
        return FALSE;
    }

    group_p = _DEVRM_GetGroupById(device_id, vg_p->group_id);
    if (group_p == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    dev_rule_p = _DEVRM_FindDevRule(device_id, vg_p->group_id, dev_rule_id);
    if (dev_rule_p == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    if (TRUE == rv_p->rule_tbl[rule_idx].valid)
    {
        UI32_T i;
        UI32_T end_idx = 0;

        for (i = rule_idx + 1; i < _countof(rv_p->rule_tbl); ++i)
        {
            if (FALSE == rv_p->rule_tbl[i].valid)
            {
                end_idx = i;
                break;
            }
        }

        if (end_idx == 0)
        {
            DEVRM_ERR("No free space after index %lu. Must compact rule table first.", rule_idx);
            return FALSE;
        }

        for (i = end_idx; rule_idx < i; --i)
        {
            rv_p->rule_tbl[i] = rv_p->rule_tbl[i-1];
            rv_p->rule_tbl[i].dirty_bit = TRUE;
        }

        {
            DEVRM_MoveParam_T move;
            BOOL_T ret;
            DEVRM_DevRule_T *insert_vr_p;

            insert_vr_p = _DEVRM_FindDevRule(device_id, vg_p->group_id, rv_p->rule_tbl[rule_idx].rule_id);
            if (insert_vr_p == NULL)
            {
                ASSERT(0);
                return FALSE;
            }

            move.start = insert_slice_rule_idx = insert_vr_p->slice_rule_index;
            move.end   = group_p->drv.cur_cnt * group_p->rule_size - 1;
            move.shift = 1 * group_p->rule_size;
            ASSERT(move.end < SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR);

            ret = _DEVRM_MoveSliceRuleAndUpdateGroup(device_id, group_p, &move);
            ASSERT(ret == TRUE);
        }
    }
    else if (1 < group_p->drv.cur_cnt)
    {
        UI32_T i;
        int dr_pri, insert_vr_pri;

        dr_pri = vg_p->pri_base - rule_idx;
        insert_vr_pri = DEVRM_MIN_RULE_PRIORITY;
        insert_slice_rule_idx = dev_rule_p->slice_rule_index;

        for (i = 0; i < _countof(group_p->drv.rule_tbl); ++i)
        {
            //find the max priority rule min than vitual rule priority
            DEVRM_DevRule_T * dev_rule_p = &group_p->drv.rule_tbl[i];

            if (dev_rule_p->valid == FALSE)
            {
                continue;
            }

            if (dev_rule_p->dr_pri < dr_pri &&
                insert_vr_pri < dev_rule_p->dr_pri)
            {
                insert_vr_pri = dev_rule_p->dr_pri;
                insert_slice_rule_idx = dev_rule_p->slice_rule_index;
            }
        }

        if (insert_slice_rule_idx != dev_rule_p->slice_rule_index)
        {
            DEVRM_MoveParam_T move;
            BOOL_T ret;

            move.start = insert_slice_rule_idx;
            move.end   = group_p->drv.cur_cnt * group_p->rule_size - 1;
            move.shift = 1 * group_p->rule_size;
            ASSERT(move.end < SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR);

            ret = _DEVRM_MoveSliceRuleAndUpdateGroup(device_id, group_p, &move);
            ASSERT(ret == TRUE);

            dev_rule_p->slice_rule_index = insert_slice_rule_idx;
        }
     }

    dev_rule_p->slice_rule_index = insert_slice_rule_idx;

    rule_p = &rv_p->rule_tbl[rule_idx];
    rule_p->rule_id   = dev_rule_id;
    rule_p->rule_pri  = rule_pri;
    rule_p->dirty_bit = TRUE;
    rule_p->valid     = TRUE;

    rv_p->cur_rule_nbr += 1;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_FindRuleIndexByRuleId
 *------------------------------------------------------------------------------
 * PURPOSE: Find the index of virtual group rule array by device rule id
 * INPUT:   rv_p        - virtual group rule vector
 *          dev_rule_id -
 * OUTPUT:  rule_idx - Virtual group rule array index
 * RETURN:  TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_FindRuleIndexByRuleId(
    DEVRM_RuleVector_T *rv_p,
    UI32_T dev_rule_id,
    UI32_T *rule_idx)
{
    UI32_T i;

    if (NULL == rv_p || NULL == rule_idx)
    {
        DEVRM_ERR("NULL pointer");
        return FALSE;
    }

    for (i = 0; i < _countof(rv_p->rule_tbl); ++i)
    {
        const DEVRM_Rule_T *rule_p = &rv_p->rule_tbl[i];

        if (TRUE == rule_p->valid && rule_p->rule_id == dev_rule_id)
        {
            *rule_idx = i;
            return TRUE;
        }
    }

    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_RemoveRuleAtIndex
 *------------------------------------------------------------------------------
 * PURPOSE: _DEVRM_RemoveRuleAtIndex
 * INPUT:   rv_p     - virtual group rule vector
 *          rule_idx - index of virtual group rule vector
 * OUTPUT:
 * RETURN:  TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
static BOOL_T
_DEVRM_RemoveRuleAtIndex(
    DEVRM_RuleVector_T *rv_p,
    UI32_T rule_idx)
{
    DEVRM_Rule_T * rule_p;

    if (NULL == rv_p)
    {
        DEVRM_ERR("NULL pointer");
        return FALSE;
    }

    if (_countof(rv_p->rule_tbl) <= rule_idx)
    {
        DEVRM_ERR("Invalid rule_idx=%lu", rule_idx);
        return FALSE;
    }

    rule_p = &rv_p->rule_tbl[rule_idx];

    if (TRUE == rule_p->valid)
    {
        memset(rule_p, 0, sizeof(*rule_p));
        rule_p->valid = FALSE;

        rv_p->cur_rule_nbr -= 1;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_DumpRuleVector
 *------------------------------------------------------------------------------
 * PURPOSE: Dump virtual group rule vector
 * INPUT:
 * OUTPUT:
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
static void
_DEVRM_DumpRuleVector(
    DEVRM_RuleVector_T *rv_p,
    BOOL_T skip_unvalid)
{
    UI32_T i;

    DEVRM_PRINT("Current NBR of rule : %lu\r\n", rv_p->cur_rule_nbr);
    DEVRM_PRINT("Rule Table : \r\n");

    for (i = 0; i < _countof(rv_p->rule_tbl); ++i)
    {
        DEVRM_Rule_T *rule_p = &rv_p->rule_tbl[i];

        if (skip_unvalid && rule_p->valid == FALSE)
        {
            continue;
        }

        DEVRM_PRINT("  [%03lu] val=%u, dirty=%s, pri=%-3d, id=%-3lu\r\n",
                    i,
                    rule_p->valid,
                    rule_p->dirty_bit?"ON ":"OFF",
                    rule_p->rule_pri,
                    rule_p->rule_id);
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_DumpDevRuleVector
 *------------------------------------------------------------------------------
 * PURPOSE: Dump device rule vector
 * INPUT:
 * OUTPUT:
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
static void
_DEVRM_DumpDevRuleVector(
    DEVRM_DevRuleVector_T *drv_p,
    BOOL_T skip_unvalid)
{
    UI32_T i;

    DEVRM_PRINT("Current NBR of rule : %lu\r\n", drv_p->cur_cnt);
    DEVRM_PRINT("Device Rule Table : \r\n");

    for (i = 0; i < _countof(drv_p->rule_tbl); ++i)
    {
        DEVRM_DevRule_T *dr_p = &drv_p->rule_tbl[i];

        if (skip_unvalid && dr_p->valid == FALSE)
        {
            continue;
        }

        DEVRM_PRINT("  [%03lu] valid=%u, id=%lu, pri=%d, slice_rule_idx=%3lu,"
                    " meter_idx=%3lu, counter_idx=%3lu\r\n",
                    i, dr_p->valid, dr_p->id, dr_p->dr_pri,
                    dr_p->slice_rule_index, dr_p->meter_idx, dr_p->counter_idx);

        {
            UI32_T group_id;

            if (TRUE == _DEVRM_GetGroupIdByDevRuleId(dr_p->id, &group_id))
            {
                DEVRM_Group_T *grp_p;
                int i;

                grp_p = _DEVRM_GetGroupById(0, group_id);
                if (grp_p)
                {
                    DEVRM_PRINT("        RTK_rule_entry_idx=[");

                    for (i = 0; i < grp_p->slice_num; ++i)
                    {
                        DEVRM_Slice_T *slice_p;

                        slice_p = _DEVRM_GetSliceByIdx(0, grp_p->slice_id + i);
                        if (NULL != slice_p)
                        {
                            DEVRM_PRINT(" %lu",
                                        DEVRM_SLICE_RULE_IDX_TO_HW_IDX(grp_p->slice_id, dr_p->slice_rule_index));

                            if (slice_p->template_num > 1)
                            {
                                DEVRM_PRINT(" %lu",
                                            DEVRM_SLICE_RULE_IDX_TO_HW_IDX(grp_p->slice_id, dr_p->slice_rule_index+1));
                            }
                        }
                    }
                    DEVRM_PRINT("]\r\n");
                }
            }
        }
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_DumpSliceVector
 *------------------------------------------------------------------------------
 * PURPOSE: Dump slice vector
 * INPUT:
 * OUTPUT:
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
static void
_DEVRM_DumpSliceVector(
    DEVRM_SliceVector_T *sv_p,
    BOOL_T skip_unvalid)
{
    UI32_T i;


    DEVRM_PRINT("Current NBR of slice : %lu\r\n", sv_p->cur_cnt);
    DEVRM_PRINT("Slice Table : \r\n");

    DEVRM_PRINT("- Ingress:\r\n");
    for (i = 0; i < DEVRM_MAX_NBR_OF_IACL_SLICE; ++i)
    {
        DEVRM_Slice_T *slice_p = &sv_p->slice_tbl[i];

        if (skip_unvalid && slice_p->valid == FALSE)
        {
            continue;
        }

        DEVRM_PRINT("  [%02lu] valid=%u, idx=%2lu , owner_group=%2lu, template(%lu)[%lu, %lu], cur_rule_cnt=%3lu\r\n",
                    i, slice_p->valid, slice_p->idx, slice_p->owner_group_id,
                    slice_p->template_num, slice_p->template_id[0],
                    slice_p->template_id[1], slice_p->cur_rule_cnt);
    }

    DEVRM_PRINT("- Egress:\r\n");
    for (; i < DEVRM_MAX_NBR_OF_SLICE; ++i)
    {
        DEVRM_Slice_T *slice_p = &sv_p->slice_tbl[i];

        if (skip_unvalid && slice_p->valid == FALSE)
        {
            continue;
        }

        DEVRM_PRINT("  [%02lu] valid=%u, idx=%2lu , owner_group=%2lu, template(%lu)[%lu, %lu], cur_rule_cnt=%3lu\r\n",
                    i, slice_p->valid, slice_p->idx, slice_p->owner_group_id,
                    slice_p->template_num, slice_p->template_id[0],
                    slice_p->template_id[1], slice_p->cur_rule_cnt);
    }
}

#pragma mark - backdoor function

static void _DEVRM_Backdoor_DumpDevrmData();
static void _DEVRM_Backdoor_DumpDevrmData_Group();
static void _DEVRM_Backdoor_DumpDevrmData_Group_DevRule();
static void _DEVRM_Backdoor_DumpDevrmData_Slice();
static void _DEVRM_Backdoor_DumpDevrmData_VG();
static void _DEVRM_Backdoor_DumpDevrmData_VG_Rule();
static void _DEVRM_Backdoor_DumpDevrmData_Policer();
static void _DEVRM_Backdoor_DumpDevrmData_Policer_Meter();
static void _DEVRM_Backdoor_DumpDevrmData_Policer_Counter();
static void _DEVRM_Backdoor_DumpDevrmData_ConvertDevRuleIdToHwIdx();

static void _DEVRM_Backdoor_RuleOp_Main();
static void _DEVRM_Backdoor_RuleOp_ModifiedCurRule();
static void _DEVRM_Backdoor_RuleOp_ChgBurstSize();

static void
_DEVRM_BackdoorMain(void)
{
    enum
    {
        BD_OPTION_SHOW_HELP = 0x00000001
    };

    UI8_T ch[5] = {0};
    BOOL_T eof = FALSE;
    UI32_T option = BD_OPTION_SHOW_HELP;;

    while (! eof)
    {
        DEVRM_PRINT("\r\n");

        if (option & BD_OPTION_SHOW_HELP)
        {
            DEVRM_PRINT("== DEV_RM backdoor ==============================\r\n");
            DEVRM_PRINT("0 : Set dev_rm debug flag.\r\n");
            DEVRM_PRINT("1 : Dump DEV_RM OM.\r\n");
            DEVRM_PRINT("2 : Rule operation\r\n");
            DEVRM_PRINT("x : exit\r\n");
            DEVRM_PRINT("=================================================\r\n");
        }

        option |= BD_OPTION_SHOW_HELP;
        DEVRM_PRINT("\tselect:\t");
        BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
        DEVRM_PRINT("\r\n");
        switch (ch[0])
        {
            case '0':
                DEVRM_PRINT("\r\n");
                DEVRM_PRINT("== Debug flag bits list =========================\r\n");
                DEVRM_PRINT(" Current = 0x%03x\r\n", devrm_data.debug_flags);
                DEVRM_PRINT(" 0. No\r\n");
                DEVRM_PRINT(" 1. ALL\r\n");
                DEVRM_PRINT(" 2. DEVRM_DEBUG_FLAGS_BIT_ERR(0x%03x)\r\n",   DEVRM_DEBUG_FLAGS_BIT_ERR);
                DEVRM_PRINT(" 3. DEVRM_DEBUG_FLAGS_BIT_LOG(0x%03x)\r\n",   DEVRM_DEBUG_FLAGS_BIT_LOG);
                DEVRM_PRINT(" 4. DEVRM_DEBUG_FLAGS_BIT_TRACE(0x%03x)\r\n", DEVRM_DEBUG_FLAGS_BIT_TRACE);
                DEVRM_PRINT("=================================================\r\n");
                DEVRM_PRINT("\tselect:\t");
                BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
                DEVRM_PRINT("\r\n");
                {
                    UI32_T flags =    (ch[0] == '0') ? 0
                                    : (ch[0] == '1') ? (DEVRM_DEBUG_FLAGS_BIT_ERR | DEVRM_DEBUG_FLAGS_BIT_LOG | DEVRM_DEBUG_FLAGS_BIT_TRACE)
                                    : (ch[0] == '2') ? DEVRM_DEBUG_FLAGS_BIT_ERR
                                    : (ch[0] == '3') ? DEVRM_DEBUG_FLAGS_BIT_LOG
                                    : (ch[0] == '4') ? DEVRM_DEBUG_FLAGS_BIT_TRACE
                                    : 0xffff;

                    if (flags == 0xffff)
                    {
                        flags = atol(ch);
                    }
                    DEVRM_SetDebugFlags(flags);
                }
                break;

            case '1':
                _DEVRM_Backdoor_DumpDevrmData();
                break;

            case '2':
                _DEVRM_Backdoor_RuleOp_Main();
                break;

            case '?':
                option |= BD_OPTION_SHOW_HELP;
                continue;

            case 'x':
            case 'q':
                eof = TRUE;
                break;

            default:
                option &= ~BD_OPTION_SHOW_HELP;
                break;
        }
    }
}


static void
_DEVRM_Backdoor_DumpDevrmData()
{
    char ch[5] = {0};
    BOOL_T eof = FALSE;

    while (!eof)
    {
        DEVRM_PRINT("\r\n");
        DEVRM_PRINT("== Dump devrm_data ==============================\r\n");
        DEVRM_PRINT("1   : Group status.\r\n");
        DEVRM_PRINT("1.1 : Group device rule status.\r\n");
        DEVRM_PRINT("2   : Slice status.\r\n");
        DEVRM_PRINT("3   : Vitual group status.\r\n");
        DEVRM_PRINT("3.1 : Vitual group rule status.\r\n");
        DEVRM_PRINT("4   : Policer status.\r\n");
        DEVRM_PRINT("5   : Convert device rule id to hardward index.\r\n");
        DEVRM_PRINT("x   : exit\r\n");
        DEVRM_PRINT("=================================================\r\n");
        DEVRM_PRINT("\tselect:\t");
        memset(ch, 0, sizeof(ch));
        BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
        DEVRM_PRINT("\r\n");
        switch (ch[0])
        {
            case '1':
                switch (ch[2])
                {
                    case '1':
                        _DEVRM_Backdoor_DumpDevrmData_Group_DevRule();
                        break;
                    default:
                        _DEVRM_Backdoor_DumpDevrmData_Group();
                        break;
                }
                break;

            case '2':
                _DEVRM_Backdoor_DumpDevrmData_Slice();
                break;

            case '3':
                switch (ch[2])
                {
                    case '1':
                        _DEVRM_Backdoor_DumpDevrmData_VG_Rule();
                        break;
                    default:
                        _DEVRM_Backdoor_DumpDevrmData_VG();
                        break;
                }
                break;

            case '4':
                _DEVRM_Backdoor_DumpDevrmData_Policer();
                break;

            case '5':
                _DEVRM_Backdoor_DumpDevrmData_ConvertDevRuleIdToHwIdx();
                break;


            case '?':
                continue;

            default:
            case 'x':
                eof = TRUE;
                break;
        }
    }
}

static void
_DEVRM_Backdoor_DumpDevrmData_Group()
{
    UI32_T device_id;

    DEVRM_PRINT("\r\n== Groups =======================================\r\n");

    for (device_id = 0; device_id < _countof(devrm_data.dev); ++device_id)
    {
        UI32_T group_idx;

        DEVRM_PRINT("Device[%lu]:\r\n", device_id);
        DEVRM_PRINT("  cur_cnt: %lu\r\n", devrm_data.dev[device_id].group.cur_cnt);

        for (group_idx = 0; group_idx < _countof(devrm_data.dev[device_id].group.group_tbl); ++group_idx)
        {
            DEVRM_Group_T *grp_p = &devrm_data.dev[device_id].group.group_tbl[group_idx];

            if (grp_p->valid == FALSE)
                continue;

            DEVRM_PRINT("  [%02lu]: phase=%s, id=%2lu, slice{num=%lu, id=%2lu}, rule_size=%lu \r\n",
                        group_idx, DEVRM_STR_PHASE(grp_p->phase), grp_p->id,
                        grp_p->slice_num, grp_p->slice_id, grp_p->rule_size);
        }
    }

    DEVRM_PRINT("=================================================\r\n\r\n");
}

static void
_DEVRM_Backdoor_DumpDevrmData_Group_DevRule()
{
    char ch[5] = {0};
    UI32_T device_id, group_id;
    BOOL_T show_all;
    DEVRM_Group_T *grp_p;

    DEVRM_PRINT("\r\n");
    DEVRM_PRINT("Input device_id:\t");
    BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
    if (ch[0] == '*')
    {
        DEVRM_PRINT("\r\nShow all (0:false, 1:true):\t");
        BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
        DEVRM_PRINT("\r\n");
        show_all = (ch[0] == '1') ? TRUE : FALSE;

        DEVRM_PRINT("\r\n== Groups Device Rule ===========================\r\n");

        for (device_id = 0; device_id < SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; ++device_id)
        {
            for (group_id = 0; group_id < DEVRM_MAX_NBR_OF_SLICE; ++group_id)
            {
                grp_p = _DEVRM_GetGroupById(device_id, group_id);
                if (NULL == grp_p)
                {
                    continue;
                }

                if (!show_all && grp_p->drv.cur_cnt == 0)
                {
                    continue;
                }

                DEVRM_PRINT("Device:%lu, Group_id:%lu\r\n", device_id, group_id);
                _DEVRM_DumpDevRuleVector(&grp_p->drv, TRUE);
                DEVRM_PRINT("=================================================\r\n");
            }
        }
        DEVRM_PRINT("== End of Groups Device Rule =====================\r\n");
    }
    else
    {
        device_id = atol(ch);

        DEVRM_PRINT("\r\nInput group_id:\t");
        BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
        DEVRM_PRINT("\r\n");
        group_id = atol(ch);

        grp_p = _DEVRM_GetGroupById(device_id, group_id);
        if (NULL == grp_p)
        {
            DEVRM_PRINT("\r\nInput error\r\n");
            return;
        }

        DEVRM_PRINT("\r\nShow all (0:false, 1:true):\t");
        BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
        DEVRM_PRINT("\r\n");
        show_all = (ch[0] == '1') ? TRUE : FALSE;

        DEVRM_PRINT("\r\n== Groups Device Rule ===========================\r\n");
        DEVRM_PRINT("Device:%lu, Group_id:%lu\r\n", device_id, group_id);
        _DEVRM_DumpDevRuleVector(&grp_p->drv, !show_all);
        DEVRM_PRINT("=================================================\r\n\r\n");
    }
}

static void
_DEVRM_Backdoor_DumpDevrmData_Slice()
{
    UI32_T device_id;
    BOOL_T show_all;
    char ch[5] = {0};

    DEVRM_PRINT("\r\nShow all (0:false, 1:true):\t");
    BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
    DEVRM_PRINT("\r\n");
    show_all = (ch[0] == '1') ? TRUE : FALSE;

    DEVRM_PRINT("\r\n== Slice ========================================\r\n");

    for (device_id = 0; device_id < _countof(devrm_data.dev); ++device_id)
    {
        DEVRM_SliceVector_T *sv_p = &devrm_data.dev[device_id].slice;

        DEVRM_PRINT("Device[%lu]:\r\n", device_id);

        _DEVRM_DumpSliceVector(sv_p, !show_all);
    }

    DEVRM_PRINT("=================================================\r\n\r\n");
}

static void
_DEVRM_Backdoor_DumpDevrmData_VG()
{
    UI32_T device_id;

    DEVRM_PRINT("\r\n== Virtual Groups ===============================\r\n");

    for (device_id = 0; device_id < _countof(devrm_data.dev); ++device_id)
    {
        UI32_T vg_idx;

        DEVRM_PRINT("Device[%lu]:\r\n", device_id);
        DEVRM_PRINT("  cur_cnt: %lu\r\n", devrm_data.dev[device_id].vir_grp.vsel_num);

        for (vg_idx = 0; vg_idx < _countof(devrm_data.dev[device_id].vir_grp.vg_tbl); ++vg_idx)
        {
            DEVRM_VirtualGroup_T *vg_p = &devrm_data.dev[device_id].vir_grp.vg_tbl[vg_idx];

            if (vg_p->valid == FALSE)
                continue;

            DEVRM_PRINT("  [%02lu]: group_id=%2lu, vg_id=%2lu, pri_lvl=%d, pri_base=%5d\r\n",
                        vg_idx, vg_p->group_id, vg_p->vir_group_id,
                        vg_p->pri_lvl, vg_p->pri_base);
        }
    }

    DEVRM_PRINT("=================================================\r\n\r\n");
}

static void
_DEVRM_Backdoor_DumpDevrmData_VG_Rule()
{
    char ch[5] = {0};
    UI32_T device_id, vg_id;
    BOOL_T show_all;
    DEVRM_VirtualGroup_T *vg_p;

    DEVRM_PRINT("\r\nInput device_id:\t");
    BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
    DEVRM_PRINT("\r\n");
    device_id = atol(ch);

    DEVRM_PRINT("\r\nInput virtual_group_id:\t");
    BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
    DEVRM_PRINT("\r\n");
    vg_id = atol(ch);

    vg_p = _DEVRM_FindVirtualGroup(device_id, vg_id);
    if (NULL == vg_p)
    {
        DEVRM_PRINT("\r\nInput error\r\n");
        return;
    }

    DEVRM_PRINT("\r\nShow all (0:false, 1:true):\t");
    BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
    DEVRM_PRINT("\r\n");
    show_all = (ch[0] == '1') ? TRUE : FALSE;

    _DEVRM_DumpRuleVector(&vg_p->rv, !show_all);
}

static void
_DEVRM_Backdoor_DumpDevrmData_Policer()
{
    char ch[5] = {0};
    BOOL_T eof = FALSE;

    while (!eof)
    {
        DEVRM_PRINT("\r\n");
        DEVRM_PRINT("== Dump devrm_data.policer ==============================\r\n");
        DEVRM_PRINT("1   : Meter (used).\r\n");
        DEVRM_PRINT("2   : Counter (used).\r\n");
        DEVRM_PRINT("x   : exit\r\n");
        DEVRM_PRINT("=================================================\r\n");
        DEVRM_PRINT("\tselect:\t");

        memset(ch, 0, sizeof(ch));
        BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
        DEVRM_PRINT("\r\n");
        switch (ch[0])
        {
            case '1':
                _DEVRM_Backdoor_DumpDevrmData_Policer_Meter();
                break;

            case '2':
                _DEVRM_Backdoor_DumpDevrmData_Policer_Counter();
                break;

            case '?':
                continue;

            default:
            case 'x':
                eof = TRUE;
                break;
        }
    }
}

static void
_DEVRM_Backdoor_DumpDevrmData_Policer_Meter()
{
    UI32_T device_id, idx;

    DEVRM_PRINT("\r\n== Meter entry  =================================\r\n");

    for (device_id = 0; device_id < _countof(devrm_data.dev); ++device_id)
    {
        DEVRM_PRINT("Device[%lu]:\r\n", device_id);

        for (idx = 0; idx < _countof(devrm_data.dev[device_id].policer.meter); ++idx)
        {
            DEVRM_DevMeter_T *meter_p = &devrm_data.dev[device_id].policer.meter[idx];

            if (meter_p->valid == FALSE)
                continue;

            DEVRM_PRINT("  [%03lu]: id=%3lu, dev_rule_id=%4lu\r\n",
                        idx, meter_p->id, meter_p->dev_rule_id);
        }
    }

    DEVRM_PRINT("\r\n=================================================\r\n");
}

static void
_DEVRM_Backdoor_DumpDevrmData_Policer_Counter()
{
    UI32_T device_id, idx;

    DEVRM_PRINT("\r\n== Counter entry ================================\r\n");

    for (device_id = 0; device_id < _countof(devrm_data.dev); ++device_id)
    {
        DEVRM_PRINT("Device[%lu]:\r\n", device_id);

        for (idx = 0; idx < _countof(devrm_data.dev[device_id].policer.counter); ++idx)
        {
            DEVRM_DevCounter_T *counter_p = &devrm_data.dev[device_id].policer.counter[idx];

            if (counter_p->valid == FALSE)
                continue;

            DEVRM_PRINT("  [%03lu]: id=%3lu, dev_rule_id=%4lu, type=%s\r\n",
                        idx, counter_p->id, counter_p->dev_rule_id,
                        (counter_p->type == STAT_TYPE_BYTE_BASED_64BIT) ? "Byte" : "Packet");
        }
    }
    
    DEVRM_PRINT("\r\n=================================================\r\n");
}

static void
_DEVRM_Backdoor_DumpDevrmData_ConvertDevRuleIdToHwIdx()
{
    UI32_T group_id, dev_rule_id;
    DEVRM_Group_T *group_p;
    DEVRM_Slice_T *slice_p;
    DEVRM_DevRule_T *dev_rule_p;
    int i, j;
    char ch[10] = {0};

    DEVRM_PRINT("\tInput device rule id:\t");
    BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
    DEVRM_PRINT("\r\n");
    dev_rule_id = atol(ch);

    group_id = dev_rule_id >> 8;
    group_p = _DEVRM_GetGroupById(0, group_id);
    if (group_p == NULL)
        return;

    dev_rule_p = _DEVRM_FindDevRule(0, group_id, dev_rule_id);
    if (dev_rule_p == NULL)
        return;

    for (i = 0; i < group_p->slice_num; ++i)
    {
        slice_p = _DEVRM_GetSliceByIdx(0, group_p->slice_id + i);
        if (NULL == slice_p)
            break;

        ASSERT(slice_p->template_num == group_p->rule_size);
        for (j = 0; j < slice_p->template_num; ++j)
        {
            UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

            DEVRM_PRINT("  >> hardware rule index = %lu\r\n", hw_rule_idx);
        }
    }
}

static void
_DEVRM_Backdoor_RuleOp_Main()
{
    char ch[5] = {0};
    BOOL_T eof = FALSE;

    while (!eof)
    {
        DEVRM_PRINT("\r\n");
        DEVRM_PRINT("== Rule Operation ==============================\r\n");
        DEVRM_PRINT("1  : Allocate rule\r\n");
        DEVRM_PRINT("2  : Set rule\r\n");
        DEVRM_PRINT("3  : Destroy rule\r\n");
        DEVRM_PRINT("4  : Modified current rule action\r\n");
        DEVRM_PRINT("5  : Change default burst size of meter\r\n");
        DEVRM_PRINT("x  : exit\r\n");
        DEVRM_PRINT("=================================================\r\n");
        DEVRM_PRINT("\tselect:\t");

        memset(ch, 0, sizeof(ch));
        BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
        DEVRM_PRINT("\r\n");

        switch (ch[0])
        {
            case '1':
            case '2':
            case '3':
            case '4':
                DEVRM_PRINT("\r\nNot support yet!!\r\n");
                break;
            case '5':
                _DEVRM_Backdoor_RuleOp_ChgBurstSize();
                break;

            case '?':
                continue;

            default:
            case 'x':
                eof = TRUE;
                break;
        }
    }
}

static void
_DEVRM_Backdoor_RuleOp_ModifiedCurRule()
{
#if 0
    char ch[5] = {0};
    BOOL_T eof = FALSE;
    UI32_T device_id, group_id, dev_rule_id;
    DEVRM_ACTION_T action;

    DEVRM_LIB_ActionInit(&dev_action);

    DEVRM_PRINT("\r\nInput device_id      :\t");
    BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
    device_id = atol(ch);

    DEVRM_PRINT("\r\nInput group_id   :\t");
    BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
    group_id = atol(ch);

    DEVRM_PRINT("\r\nInput dev_rule_id:\t");
    BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
    dev_rule_id = atol(ch);


    while (eof)
    {
        DEVRM_PRINT("\r\n");
        DEVRM_PRINT("== Configure action =============================\r\n");
        DEVRM_PRINT("Device = %lu, Group = %lu, Dev_rule_id = %lu\r\n", device_id, group_id, dev_rule_id);
        DEVRM_PRINT("=================================================\r\n");
        DEVRM_PRINT("d  : Dump configure action \r\n");
        DEVRM_PRINT("s  : Set to rule\r\n");
        DEVRM_PRINT("x  : Exit, give up to modified action\r\n");
        DEVRM_PRINT("=================================================\r\n");
        DEVRM_PRINT("\tselect:\t");

        BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
        DEVRM_PRINT("\r\n");

        switch (ch[0])
        {

            case '1':
            case '2':
            case '3':
                DEVRM_PRINT("\r\nNot support yet!!\r\n");
                break;
            case 'd':
                DEVRM_LIB_ActionDump(&action, DEVRM_PRINT, 2);
                break;
            case 's':
                _DEVRM_SetupAction(device_id, group_id, dev_rule_id, &action);
                break;

            case '?':
            default:
                break;
        }
    }
#endif /* 0 */
}

static void
_DEVRM_Backdoor_RuleOp_ChgBurstSize()
{
#define DEVRM_MIN_METER_PACKET_BURST 1
#define DEVRM_MAX_METER_PACKET_BURST 65535

    char ch[8] = {0};
    rtk_acl_meterBurstSize_t burst_size;
    rtk_acl_meterMode_t meter_mode;

    DEVRM_PRINT("\tMeter Type (0:byte, 1: packet): \t");

    BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
    meter_mode = (atol(ch) == 0) ? METER_MODE_BYTE : METER_MODE_PACKET;

    memset(&burst_size, 0, sizeof(burst_size));
    rtk_acl_meterBurstSize_get(0, meter_mode, &burst_size);
    DEVRM_PRINT("<<Current burst size is %lu>>\r\n", burst_size.slb_lb0bs);

    if (meter_mode == METER_MODE_BYTE)
    {
        DEVRM_PRINT("\tNew burst size (%lu ~ %lu bytes): \t",
                    SYS_ADPT_DIFFSERV_MIN_POLICE_BURST,
                    SYS_ADPT_DIFFSERV_MAX_POLICE_BURST);
    }
    else
    {
        DEVRM_PRINT("\tNew burst size (%lu ~ %lu packets): \t",
                    DEVRM_MIN_METER_PACKET_BURST,
                    DEVRM_MAX_METER_PACKET_BURST);
    }

    BACKDOOR_MGR_RequestKeyIn(ch, sizeof(ch));
    burst_size.slb_lb0bs = atol(ch);
    DEVRM_PRINT("\r\n");

    if ((meter_mode == METER_MODE_BYTE && (burst_size.slb_lb0bs < SYS_ADPT_DIFFSERV_MIN_POLICE_BURST ||
                                           SYS_ADPT_DIFFSERV_MAX_POLICE_BURST < burst_size.slb_lb0bs))
        ||
        (meter_mode == METER_MODE_PACKET && (burst_size.slb_lb0bs < DEVRM_MIN_METER_PACKET_BURST ||
                                             SYS_ADPT_DIFFSERV_MAX_POLICE_BURST < burst_size.slb_lb0bs)))
    {
        DEVRM_PRINT("Error parameter\r\n");
        return;
    }

    rtk_acl_meterBurstSize_set(0, meter_mode, &burst_size);
    return;
}


#pragma mark - export function
/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_Initial
 *------------------------------------------------------------------------------
 * PURPOSE:  init/clear chip setting
 * INPUT:
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_Initial()
{
    int ret;
    int device_id;

    if (FALSE == devrm_data.bd_registered)
    {
        BACKDOOR_MGR_Register_SubsysBackdoorFunc_CallBack("dev_rm", SYS_BLD_DRIVER_GROUP_IPCMSGQ_KEY, _DEVRM_BackdoorMain);
        devrm_data.bd_registered = TRUE;
    }

    for (device_id = 0; device_id < soc_ndev; ++device_id)
    {
#if (DEVRM_MAX_NBR_OF_EACL_SLICE != 0)
        ret = rtk_acl_partition_set(device_id, DEVRM_MAX_NBR_OF_IACL_SLICE);
        if (ret != RT_ERR_OK)
        {
            DEVRM_PRINT("rtk_acl_partitio_set err_code=%d", ret);
            return FALSE;
        }
#endif /* DEVRM_MAX_NBR_OF_EACL_SLICE != 0 */

        rtk_acl_stat_clearAll(device_id);
		
        if (HAL_IS_RTL8390_FAMILY_ID(device_id))
        {
            rtk_acl_templateFieldIntentVlanTag_set(device_id, VLAN_TAG_TYPE_OUTER);
        }

        {
            rtk_acl_meterBurstSize_t burst_size;

            /* burst size for packet mode
             */
            rtk_acl_meterBurstSize_get(0, METER_MODE_PACKET, &burst_size);

            if (HAL_IS_RTL8380_FAMILY_ID(device_id))
            {
                burst_size.slb_lb0bs = 1;
            }
            else if (HAL_IS_RTL8390_FAMILY_ID(device_id))
            {
                burst_size.dlb_lb0bs = 1;
            }

            rtk_acl_meterBurstSize_set(0, METER_MODE_PACKET, &burst_size);

            /* burst size for byte mode
             */
            if (HAL_IS_RTL8390_FAMILY_ID(device_id))
            {
                rtk_acl_meterBurstSize_get(0, METER_MODE_BYTE, &burst_size);

                burst_size.dlb_lb0bs = 512;
                burst_size.dlb_lb1bs = 512;

                /* FIXME: Need tunning the value
                 */
                burst_size.srtcm_cbs = 512;
                burst_size.srtcm_ebs = 1024;
                burst_size.trtcm_cbs = 512;
                burst_size.trtcm_pbs = 1024;

                rtk_acl_meterBurstSize_set(0, METER_MODE_BYTE, &burst_size);
            }
        }
    }

    _DEVRM_InitCustomerTemplate();

    memset(&devrm_data.dev, 0, sizeof(devrm_data.dev));


    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetDebugFlags
 *------------------------------------------------------------------------------
 * PURPOSE:  Set dev_rm debug flags
 * INPUT:    flags - DEVRM_DEBUG_FLAGS_BIT_NO
 *                   DEVRM_DEBUG_FLAGS_BIT_ERR
 *                   DEVRM_DEBUG_FLAGS_BIT_LOG
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
void DEVRM_SetDebugFlags(UI32_T flags)
{
    devrm_data.debug_flags = flags;

    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateGroup
 *------------------------------------------------------------------------------
 * PURPOSE: Create a group entry.
 * INPUT:   device_id
 *          group_entry
 * OUTPUT:
 * RETURN:  TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_CreateGroup(
    UI32_T device_id,
    DEVRM_GroupEntry_T *group_entry)
{
    DEVRM_Group_T *grp_p;
    UI32_T slice_num, rule_size;

    if(NULL == group_entry)
    {
        return FALSE;
    }

    DEVRM_LOG("device_id=%lu, group_id=%lu,\r\n"
              "    group_mode=%s, template(%lu)[%lu:%lu:%lu:%lu], phase=%s",
              device_id, group_entry->group_id, DEVRM_STR_GROUP_MODE(group_entry->group_mode),
              group_entry->template_num, group_entry->template_key[0],
              group_entry->template_key[1], group_entry->template_key[2],
              group_entry->template_key[3], DEVRM_STR_PHASE(group_entry->phase));

    grp_p = _DEVRM_GetGroupById(device_id, group_entry->group_id);
    if (grp_p == NULL)
    {
        DEVRM_ERR("_DEVRM_GetGroupById");
        return FALSE;
    }

    if (grp_p->valid == FALSE)
    {
        _DEVRM_GetSliceNumberByMode(group_entry->group_mode, &slice_num);
        _DEVRM_GetRuleSizeByMode(group_entry->group_mode, &rule_size);

        if (2 <= slice_num)
        {
            DEVRM_PRINT("Not support QUADRUPLE mode");
            ASSERT(0);
            return FALSE;
        }

        {
            UI32_T slice_id, template_idx;
            DEVRM_Slice_T *slice_p = NULL;

            /* FIXEME: 1.Think the slice id generate,
             *           For example, now is always get the biggest none used index
             *             Ingress: 11 -> 10 -> .. ->  0
             *             Egress:  17 -> 16 -> .. -> 12
             *
             *           But this method is not support to delete some slice and
             *           then add a new slice
             *         2. support egress ?
             */
            slice_id = (group_entry->phase == DEVRM_PHASE_INGRESS)
                        ? (DEVRM_MAX_NBR_OF_IACL_SLICE - devrm_data.dev[device_id].slice.ingress.cur_cnt - 1)
                        : (DEVRM_MAX_NBR_OF_SLICE - devrm_data.dev[device_id].slice.egress.cur_cnt - 1);

            slice_p = _DEVRM_CreateSlice(device_id, slice_id);
            if (slice_p == NULL)
            {
                DEVRM_ERR("_DEVRM_CreateSlice");
                return FALSE;
            }

            slice_p->owner_group_id = group_entry->group_id;
            slice_p->template_num = group_entry->template_num;
            for (template_idx = 0; template_idx <= group_entry->template_num; ++template_idx)
            {
                slice_p->template_id[template_idx] = group_entry->template_key[template_idx];
            }

            grp_p->slice_num++;
            grp_p->slice_id = slice_id;
        }

        grp_p->id = group_entry->group_id;
        grp_p->phase = group_entry->phase;
        grp_p->rule_size = rule_size;
        grp_p->valid = TRUE;
        devrm_data.dev[device_id].group.cur_cnt++;

        if (FALSE == DEVRM_SetGroupQualifySet(device_id, group_entry))
        {
            DEVRM_ERR("DEVRM_SetGroupQualifySet");
            return FALSE;
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetGroupQualifySet
 *------------------------------------------------------------------------------
 * PURPOSE:  Set qualify template of group
 * INPUT:    device_id
 *           group_entry
 * OUTPUT:
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_SetGroupQualifySet(
    UI32_T device_id,
    DEVRM_GroupEntry_T *group_entry_p)
{
    DEVRM_Group_T *grp_p;
    int ret, i;
    rtk_acl_templateIdx_t template_info;

    DEVRM_LOG("device_id=%lu", device_id);

    grp_p = _DEVRM_GetGroupById(device_id, group_entry_p->group_id);
    if (grp_p == NULL || grp_p->valid == FALSE)
    {
        DEVRM_ERR("_DEVRM_GetGroupById");
        return FALSE;
    }

    switch (group_entry_p->template_num)
    {
        case 1:
            if (grp_p->slice_num != 1 || grp_p->rule_size != 1)
            {
                DEVRM_ERR("group_entry_p->template_num %lu is wrong", group_entry_p->template_num);
                return FALSE;
            }
            break;

        case 2:
            if (grp_p->slice_num != 1 || grp_p->rule_size != 2)
            {
                DEVRM_ERR("group_entry_p->template_num %lu is wrong", group_entry_p->template_num);
                return FALSE;
            }
            break;

        case 4:
            if (grp_p->slice_num != 2 || grp_p->rule_size != 2)
            {
                DEVRM_ERR("group_entry_p->template_num %lu is wrong", group_entry_p->template_num);
                return FALSE;
            }
            break;

        default:
            DEVRM_PRINT("%s(%d)\r\n", __FUNCTION__, __LINE__);
            ASSERT(0);
            return FALSE;
    }

    for (i = 0; i < grp_p->slice_num; i++)
    {
        DEVRM_Slice_T *slice_p;
        UI32_T block_idx = grp_p->slice_id + i;
        UI32_T j;

        slice_p = _DEVRM_GetSliceByIdx(device_id, block_idx);
        ASSERT(slice_p != NULL);

        memset(&template_info, 0, sizeof(template_info));
        slice_p->template_num = 0;
        for (j = 0; j < grp_p->rule_size; j++)
        {
            int template_id = group_entry_p->template_key[(i*2) + j];

            slice_p->template_id[j] = template_id;
            slice_p->template_num++;

            template_info.template_id[j] = template_id;
        }

        ret = rtk_acl_templateSelector_set(device_id, block_idx, template_info);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("rtk_acl_templateSelector_set");
            return FALSE;
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a selector group.
 * INPUT:    device_id, group_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_DestroyGroup(
    UI32_T device_id,
    UI32_T group_id)
{
    int i;
    DEVRM_Group_T *grp_p;
    BOOL_T ret;

    DEVRM_LOG("device_id=%lu, group_id=%lu", device_id, group_id);

    grp_p = _DEVRM_GetGroupById(device_id, group_id);
    if (grp_p == NULL)
    {
        return FALSE;
    }

    for (i = 0; i < grp_p->slice_num; ++i)
    {
        ret = _DEVRM_DestroySlice(device_id, grp_p->slice_id + i);
        if (TRUE != ret)
        {
            ASSERT(0);
            return FALSE;
        }
    }

    memset(grp_p, 0, sizeof(*grp_p));
    devrm_data.dev[device_id].group.cur_cnt--;

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create virtual group.
 * INPUT:    group_id       - group ID
 *           vir_group_id   - virtual group ID (Unique per system).
 *           priority       - virtual group priority.
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_CreateVirtualGroup(
    UI32_T group_id,
    UI32_T vir_group_id,
    int priority)
{
    DEVRM_VirtualGroup_T *vg_p;
    UI32_T device_id;

    DEVRM_LOG("group_id=%lu, vir_grp_id=%lu, priority=%d", group_id, vir_group_id, priority);

    for(device_id = 0; device_id < soc_ndev; device_id++)
    {
        if (FALSE == _DEVRM_IsGroupExisted(device_id, group_id))
        {
            DEVRM_ERR("_DEVRM_IsGroupExisted(device_id=%lu, group_id=%lu) failed",
                      device_id, group_id);
            ASSERT(0);
            return FALSE;
        }

        vg_p = _DEVRM_GetVirtualGroupAtIndex(device_id, devrm_data.dev[device_id].vir_grp.vsel_num);
        if (NULL == vg_p)
        {
            DEVRM_ERR("_DEVRM_GetVirtualGroupAtIndex(soc=%lu, idx=%lu) failed",
                      device_id, devrm_data.dev[device_id].vir_grp.vsel_num);
            return FALSE;
        }

        vg_p->group_id = group_id;
        vg_p->vir_group_id = vir_group_id;
        vg_p->pri_lvl = priority;

        vg_p->seq_no = devrm_data.dev[device_id].vir_grp.vsel_num;

        vg_p->valid = TRUE;
        ++devrm_data.dev[device_id].vir_grp.vsel_num;

        _DEVRM_SortVirtualGroup(device_id);
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocateRule
 *------------------------------------------------------------------------------
 * PURPOSE  : Allocate rule resource
 * INPUT    : device_id     - device ID
 *            group_id      - group ID
 *            vir_group_id  - virtual group ID
 *            rule_pri      - rule priority
 * OUTPUT   : dev_rule_id (per chip unique),
 *
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, call DEVRM_CreateGroup() first
 *            2, MUST allocate rule resource before call DEVRM_SetRule()
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_AllocateRule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T vir_group_id,
    int rule_pri,
    UI32_T *dev_rule_id_p)
{
    BOOL_T ret;
    DEVRM_VirtualGroup_T        *vg_p;

    DEVRM_LOG("device_id=%lu, group_id=%lu, vir_grp_id=%lu, rule_pri=%d",
              device_id, group_id, vir_group_id, rule_pri);
    ASSERT(NULL != dev_rule_id_p);

    vg_p = _DEVRM_FindVirtualGroup(device_id, vir_group_id);
    if (NULL == vg_p)
    {
        DEVRM_ERR("devId = %lu, virGroup = %lu not exist", device_id, vir_group_id);
        return FALSE;
    }

    ASSERT(vg_p->group_id == group_id);

    ret = _DEVRM_CreateDevRuleEntry(device_id, vg_p->group_id, dev_rule_id_p);
    if (ret == FALSE)
    {
        DEVRM_ERR("_DEVRM_CreateDevRuleEntry(device_id=%lu, group_id=%lu",
                  device_id, group_id);
        return FALSE;
    }

    ret = _DEVRM_AddRuleToVirtualGroupAndSetPriority(device_id, vir_group_id, rule_pri, *dev_rule_id_p);
    if (ret == FALSE)
    {
        _DEVRM_DestroyDevRuleEntry(device_id, vg_p->group_id, *dev_rule_id_p);
        return FALSE;
    }

    DEVRM_LOG(">>>> allocated dev_rule_id=%lu", *dev_rule_id_p);

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a rule entry.
 * INPUT:    device_id      - device ID
 *           vir_group_id   - virtual group ID
 *           dev_rule_id    - rule ID
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.If meter is shared, when delete it should check ..
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_DestroyRule(
    UI32_T device_id,
    UI32_T vir_group_id,
    UI32_T dev_rule_id)
{
    DEVRM_VirtualGroup_T *vg_p;

    DEVRM_LOG("device_id=%lu, vir_grp_id=%lu, dev_rule_id=%lu",
              device_id, vir_group_id, dev_rule_id);

    vg_p = _DEVRM_FindVirtualGroup(device_id, vir_group_id);
    if (NULL == vg_p)
    {
        DEVRM_ERR("devId = %lu, virGroup = %lu not exist", device_id, vir_group_id);
        return FALSE;
    }

    if (FALSE == _DEVRM_DestroyDevRuleEntry(device_id, vg_p->group_id, dev_rule_id))
    {
        DEVRM_ERR("Destroy rule fail");
        return FALSE;
    }

    if (FALSE == _DEVRM_RemoveRuleFromVirtualGroup(device_id, vir_group_id, dev_rule_id))
    {
        ASSERT(0);
        return FALSE;
    }

    return TRUE;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    device_id, group_id, dev_rule_id, rule_entry, meter_entry, action_entry
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
    UI32_T group_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry_p,
    const DEVRM_ACTION_T *dev_action_p)
{
    int ret = RT_ERR_OK;
    DEVRM_Group_T *group_p;
    DEVRM_Slice_T *slice_p;
    DEVRM_DevRule_T *dev_rule_p;
    UI32_T i, j;

    DEVRM_LOG("device_id=%lu, group_id=%lu, dev_rule_id=%lu",
              device_id, group_id, dev_rule_id);

    if (NULL == ace_entry_p || NULL == dev_action_p)
    {
        return FALSE;
    }

    group_p = _DEVRM_GetGroupById(device_id, group_id);
    if (group_p == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (dev_rule_p == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    ret = _DEVRM_SetupFieldQualify(device_id, group_id, dev_rule_id, ace_entry_p);
    if (ret != RT_ERR_OK)
    {
        DEVRM_ERR("SetupFieldQualify failed. device id=%lu, group index=%lu, dev rule id=%lu",
               device_id, group_id, dev_rule_id);
        return FALSE;
    }

    ret = _DEVRM_SetupAction(device_id, group_id, dev_rule_id, dev_action_p);
    if (ret != RT_ERR_OK)
    {
        DEVRM_ERR("SetupAction failed. device id=%lu, group index=%lu, dev rule id=%lu",
               device_id, group_id, dev_rule_id);
        return FALSE;
    }

    for (i = 0; i < group_p->slice_num; ++i)
    {
        rtk_acl_operation_t operation;

        slice_p = _DEVRM_GetSliceByIdx(device_id, group_p->slice_id + i);
        if (NULL == slice_p)
        {
            return FALSE;
        }

        ASSERT(slice_p->template_num == group_p->rule_size);
        for (j = 0; j < slice_p->template_num; ++j)
        {
            UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

            ret = rtk_acl_ruleValidate_set(device_id, (rtk_acl_phase_t) group_p->phase, hw_rule_idx, ENABLED);
            if (ret != RT_ERR_OK)
                goto UNDO;

            slice_p->cur_rule_cnt++;

            if ((hw_rule_idx%2 == 0) && 1 < slice_p->template_num)
            {
                memset(&operation, 0, sizeof(operation));
                operation.aggr_1 = ENABLED;

                if ((hw_rule_idx%2 == 0) && 1 < group_p->slice_num)
                {
                    operation.aggr_2 = ENABLED;
                }

                ret = rtk_acl_ruleOperation_set(device_id, (rtk_acl_phase_t) group_p->phase, hw_rule_idx, &operation);
                if (ret != RT_ERR_OK)
                {
                    DEVRM_ERR("rtk_acl_ruleOperation_set fail!");
                }
            }
        }
    }

    return TRUE;

UNDO:
    for (i = 0; i < group_p->slice_num; ++i)
    {
        slice_p = _DEVRM_GetSliceByIdx(device_id, group_p->slice_id + i);
        if (NULL == slice_p)
        {
            return FALSE;
        }

        ASSERT(slice_p->template_num == group_p->rule_size);
        for (j = 0; j < slice_p->template_num; ++j)
        {
            UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

            ret = rtk_acl_ruleValidate_set(device_id, (rtk_acl_phase_t) group_p->phase, hw_rule_idx, DISABLED);
        }
    }

    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_UpdateRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    device_id, dev_rule_id, rule_entry, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     DEVRM_Rule = class_element + meter + action
 *           pass meter_entry with NULL if this rule does not have meter
 *           pass action_entry with NULL if this rule does not have action
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_UpdateRule(
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    DEVRM_ACTION_T *action_p)
{
    /* FIXME: check is need to destroy rule first ?
     */
    int ret = RT_ERR_OK;
    DEVRM_Group_T *group_p;
    DEVRM_Slice_T *slice_p;
    DEVRM_DevRule_T *dev_rule_p;
    UI32_T i, j, group_id;

    _DEVRM_GetGroupIdByDevRuleId(dev_rule_id, &group_id);
    DEVRM_LOG("device_id=%lu, group_id=%lu, dev_rule_id=%lu",
              device_id, dev_rule_id);

    group_p = _DEVRM_GetGroupById(device_id, group_id);
    if (group_p == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (dev_rule_p == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    ret = _DEVRM_SetupFieldQualify(device_id, group_id, dev_rule_id, ace_entry);
    if (ret != RT_ERR_OK)
    {
        DEVRM_PRINT("SetupFieldQualify failed. device id=%lu, group index=%lu, dev rule id=%lu\n",
               device_id, group_id, dev_rule_id);
        return FALSE;
    }

    ret = _DEVRM_SetupAction(device_id, group_id, dev_rule_id, action_p);
    if (ret != RT_ERR_OK)
    {
        DEVRM_PRINT("SetupAction failed. device id=%lu, group index=%lu, dev rule id=%lu\n",
               device_id, group_id, dev_rule_id);
        return FALSE;
    }

    for (i = 0; i < group_p->slice_num; ++i)
    {
        slice_p = _DEVRM_GetSliceByIdx(device_id, group_p->slice_id + i);
        if (NULL == slice_p)
        {
            return FALSE;
        }

        ASSERT(slice_p->template_num == group_p->rule_size);
        for (j = 0; j < slice_p->template_num; ++j)
        {
            UI32_T hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index + j);

            ret = rtk_acl_ruleValidate_set(device_id, (rtk_acl_phase_t) group_p->phase, hw_rule_idx, ENABLED);

            if (ret != RT_ERR_OK)
                return FALSE;
        }
    }

    return TRUE;
}


/* meter */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  create a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry,
 *           need_reinstalled (TRUE - rule will be reinstalled)
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_CreateMeter(
    UI32_T device_id,
    UI32_T dev_rule_id,
    DEVRM_POLICER_METER_T *meter_entry_p,
    BOOL_T need_reinstalled)
{
    BOOL_T b_ret;
    int ret;

    DEVRM_LOG("device_id=%lu, dev_rule_id=%lu, mode=%d,\r\n"
              "    kbps=%lu, burst_kbytes=%lu, peak_kbps=%lu, peak_burst_kbytes=%lu, need_reinstall=%s",
              device_id, dev_rule_id, meter_entry_p->mode,
              meter_entry_p->kbits_sec, meter_entry_p->kbytes_burst_size,
              meter_entry_p->peak_kbits_sec, meter_entry_p->peak_kbytes_burst_size,
              (need_reinstalled == TRUE) ? "Y" : "N");

    if (NULL == meter_entry_p)
        return FALSE;

    b_ret = _DEVRM_CreateDevMeter(device_id, dev_rule_id);
    if(b_ret != TRUE)
    {
        DEVRM_ERR("_DEVRM_CreateDevMeter Fail");
        return FALSE;
    }

    b_ret = _DEVRM_SetDevMeter(device_id, dev_rule_id, meter_entry_p);
    if (TRUE != b_ret)
    {
        DEVRM_ERR("_DEVRM_SetDevMeter Fail");
        return FALSE;
    }

    /* meter config action */
    {
        UI32_T group_id, hw_rule_idx;
        DEVRM_Group_T *group_p;
        DEVRM_Slice_T *slice_p;
        DEVRM_DevRule_T *dev_rule_p;
        rtk_acl_action_t action;

        _DEVRM_GetGroupIdByDevRuleId(dev_rule_id, &group_id);
        group_p = _DEVRM_GetGroupById(device_id, group_id);
        if (group_p == NULL)
        {
            ASSERT(0);
            return FALSE;
        }

        slice_p = _DEVRM_GetSliceByIdx(device_id, group_p->slice_id);
        if (NULL == slice_p)
        {
            return FALSE;
        }

        dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
        if (dev_rule_p == NULL)
        {
            ASSERT(0);
            return FALSE;
        }

        hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index);

        ret = rtk_acl_ruleAction_get(device_id, (rtk_acl_phase_t) group_p->phase,
                                     hw_rule_idx, &action);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("rtk_acl_ruleAction_get fail");
            return FALSE;
        }

        if (group_p->phase == DEVRM_PHASE_INGRESS)
        {
            rtk_acl_igrAction_t *igr_action_p = &action.igr_acl;
            igr_action_p->meter_en = ENABLED;
            igr_action_p->meter_data.meter_idx = dev_rule_p->meter_idx;
            DEVRM_TRACE("[Action:meter] hw meter idx = %lu", igr_action_p->meter_data.meter_idx);
        }
        else
        {
            rtk_acl_egrAction_t *egr_action_p = &action.egr_acl;
            egr_action_p->meter_en = ENABLED;
            egr_action_p->meter_data.meter_idx = dev_rule_p->meter_idx;
            DEVRM_TRACE("[Action:meter] hw meter idx = %lu", egr_action_p->meter_data.meter_idx);
        }

        ret = rtk_acl_ruleAction_set(device_id, (rtk_acl_phase_t) group_p->phase,
                                     hw_rule_idx, &action);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("rtk_acl_ruleAction_set fail");
            return FALSE;
        }
    }

    if (TRUE == need_reinstalled)
    {
        /* FIXME: ??
         */
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_UpdateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  update a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_UpdateMeter(
    UI32_T device_id,
    UI32_T dev_rule_id,
    const DEVRM_POLICER_METER_T *meter_entry_p)
{
    BOOL_T b_ret;

    DEVRM_LOG("device_id=%lu, dev_rule_id=%lu, mode=%d,"
              "    kbps=%lu, burst_kbytes=%lu, peak_kbps=%lu, peak_burst_kbytes=%lu,",
              device_id, dev_rule_id, meter_entry_p->mode,
              meter_entry_p->kbits_sec, meter_entry_p->kbytes_burst_size,
              meter_entry_p->peak_kbits_sec, meter_entry_p->peak_kbytes_burst_size);

    if (NULL == meter_entry_p)
        return FALSE;


    b_ret = _DEVRM_SetDevMeter(device_id, dev_rule_id, meter_entry_p);
    if (TRUE != b_ret)
    {
        DEVRM_ERR("_DEVRM_SetDevMeter Fail");
        return FALSE;
    }

    /* FIXEME: reinstll rule ??
     */
    /*
    ret_value = bcm_field_entry_reinstall(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_UpdateMeter Error! bcm_field_entry_reinstall[%d]\n",ret_value);
        return FALSE;
    }
     */

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ShareMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  share a meter from src_rule_id to dst_rule_id
 * INPUT:    device_id, src_rule_id, dst_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_ShareMeter(
    UI32_T device_id,
    UI32_T src_rule_id,
    UI32_T dst_rule_id)
{
    int ret;
    UI32_T group_id, hw_rule_idx;
    DEVRM_Group_T *src_group_p, *dst_group_p;
    DEVRM_Slice_T *dst_slice_p;
    DEVRM_DevRule_T *src_dev_rule_p, *dst_dev_rule_p;
    rtk_acl_action_t action;

    DEVRM_LOG("device_id=%lu, src_rule_id=%lu, dst_rule_id=%lu",
              device_id, src_rule_id, dst_rule_id);

    _DEVRM_GetGroupIdByDevRuleId(src_rule_id, &group_id);
    src_group_p = _DEVRM_GetGroupById(device_id, group_id);
    if (src_group_p == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    src_dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, src_rule_id);
    if (src_dev_rule_p == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    _DEVRM_GetGroupIdByDevRuleId(dst_rule_id, &group_id);
    dst_group_p = _DEVRM_GetGroupById(device_id, group_id);
    if (dst_group_p == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    dst_slice_p = _DEVRM_GetSliceByIdx(device_id, dst_group_p->slice_id);
    if (NULL == dst_slice_p)
    {
        return FALSE;
    }

    dst_dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dst_rule_id);
    if (src_dev_rule_p == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(dst_slice_p->idx, dst_dev_rule_p->slice_rule_index);

    ret = rtk_acl_ruleAction_get(device_id, (rtk_acl_phase_t) dst_group_p->phase,
                                 hw_rule_idx, &action);
    if (ret != RT_ERR_OK)
    {
        DEVRM_ERR("rtk_acl_ruleAction_get fail");
        return FALSE;
    }

    if (dst_group_p->phase == DEVRM_PHASE_INGRESS)
    {
        rtk_acl_igrAction_t *igr_action_p = &action.igr_acl;
        igr_action_p->meter_en = ENABLED;
        igr_action_p->meter_data.meter_idx = src_dev_rule_p->meter_idx;
    }
    else
    {
        rtk_acl_egrAction_t *egr_action_p = &action.egr_acl;
        egr_action_p->meter_en = ENABLED;
        egr_action_p->meter_data.meter_idx = src_dev_rule_p->meter_idx;
    }

    ret = rtk_acl_ruleAction_set(device_id, (rtk_acl_phase_t) dst_group_p->phase,
                                 hw_rule_idx, &action);
    if (ret != RT_ERR_OK)
    {
        DEVRM_ERR("rtk_acl_ruleAction_set fail");
        return FALSE;
    }


    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a meter entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_DestroyMeter(
    UI32_T device_id,
    UI32_T dev_rule_id)
{
    int ret;
    BOOL_T b_ret;

    DEVRM_LOG("device_id=%lu, dev_rule_id=%lu", device_id, dev_rule_id);

    /* meter config action */
    {
        UI32_T group_id, hw_rule_idx;
        DEVRM_Group_T *group_p;
        DEVRM_Slice_T *slice_p;
        DEVRM_DevRule_T *dev_rule_p;
        rtk_acl_action_t action;

        _DEVRM_GetGroupIdByDevRuleId(dev_rule_id, &group_id);
        group_p = _DEVRM_GetGroupById(device_id, group_id);
        if (group_p == NULL)
        {
            ASSERT(0);
            return FALSE;
        }

        slice_p = _DEVRM_GetSliceByIdx(device_id, group_p->slice_id);
        if (NULL == slice_p)
        {
            return FALSE;
        }

        dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
        if (dev_rule_p == NULL)
        {
            ASSERT(0);
            return FALSE;
        }

        hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index);

        ret = rtk_acl_ruleAction_get(device_id, (rtk_acl_phase_t) group_p->phase,
                                     hw_rule_idx, &action);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("rtk_acl_ruleAction_get fail");
            return FALSE;
        }

        if (group_p->phase == DEVRM_PHASE_INGRESS)
        {
            rtk_acl_igrAction_t *igr_action_p = &action.igr_acl;
            igr_action_p->meter_en = DISABLED;
        }
        else
        {
            rtk_acl_egrAction_t *egr_action_p = &action.egr_acl;
            egr_action_p->meter_en = DISABLED;
        }

        ret = rtk_acl_ruleAction_set(device_id, (rtk_acl_phase_t) group_p->phase,
                                     hw_rule_idx, &action);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("rtk_acl_ruleAction_set fail");
            return FALSE;
        }
    }


    b_ret = _DEVRM_DestroyDevMeter(device_id, dev_rule_id);
    if(b_ret == FALSE)
    {
        DEVRM_ERR("_DEVRM_DestroyDevMeter fail");
        return FALSE;
    }

    /* FIXME: reinstall ??
     */
    /*
        ret_value = bcm_field_entry_reinstall(device_id, dev_rule_id);
        if(ret_value != BCM_E_NONE)
        {
            printf("DEVRM_DestroyMeter Error! bcm_field_entry_reinstall[%d]\n",ret_value);
            return FALSE;
        }
     */

    return TRUE;
}


/* action */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a action entry.
 * INPUT:    device_id, dev_rule_id, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_SetAction(UI32_T device_id, UI32_T dev_rule_id, DEVRM_ACTION_T *action_p)
{
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a action entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_DestroyAction(UI32_T device_id, UI32_T dev_rule_id)
{
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  create a counter entry.
 * INPUT:    device_id, dev_rule_id, counter_mode,
 *           need_reinstalled (TRUE - rule will be reinstalled)
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_CreateCounter(
    UI32_T device_id,
    UI32_T dev_rule_id,
    UI32_T counter_mode,
    BOOL_T need_reinstalled)
{
    BOOL_T b_ret;
    int ret;
    rtk_acl_statType_t rtk_counter_mode;
    DEVRM_DevCounter_T *counter_p;

    DEVRM_LOG("device_id=%lu, dev_rule_id=%lu, counter_mode=%s, need_reinstall=%s",
              device_id, dev_rule_id,
              (counter_mode == DEVRM_POLICER_COUNTER_MODE_BYTE) ? "byte"
              :(counter_mode == DEVRM_POLICER_COUNTER_MODE_PACKET) ? "packet" :
              "unknow",
              (need_reinstalled == TRUE) ? "Y" : "N");

    rtk_counter_mode = (counter_mode == DEVRM_POLICER_COUNTER_MODE_BYTE)
                       ? STAT_TYPE_BYTE_BASED_64BIT : STAT_TYPE_PACKET_BASED_32BIT;

    b_ret = _DEVRM_CreateDevCounter(device_id, dev_rule_id);
    if(b_ret != TRUE)
    {
        DEVRM_ERR("_DEVRM_CreateDevCounter Fail");
        return FALSE;
    }

    /* counter config action */
    {
        UI32_T group_id, hw_rule_idx;
        DEVRM_Group_T *group_p;
        DEVRM_Slice_T *slice_p;
        DEVRM_DevRule_T *dev_rule_p;
        rtk_acl_action_t action;

        _DEVRM_GetGroupIdByDevRuleId(dev_rule_id, &group_id);
        group_p = _DEVRM_GetGroupById(device_id, group_id);
        if (group_p == NULL)
        {
            ASSERT(0);
            return FALSE;
        }

        slice_p = _DEVRM_GetSliceByIdx(device_id, group_p->slice_id);
        if (NULL == slice_p)
        {
            return FALSE;
        }

        dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
        if (dev_rule_p == NULL)
        {
            ASSERT(0);
            return FALSE;
        }

        hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index);

        ret = rtk_acl_ruleAction_get(device_id, (rtk_acl_phase_t) group_p->phase,
                                     hw_rule_idx, &action);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("rtk_acl_ruleAction_get fail");
            return FALSE;
        }

        if (group_p->phase == DEVRM_PHASE_INGRESS)
        {
            rtk_acl_igrAction_t *igr_action_p = &action.igr_acl;
            igr_action_p->stat_en = ENABLED;
            igr_action_p->stat_data.stat_type = rtk_counter_mode;
            igr_action_p->stat_data.stat_idx = dev_rule_p->counter_idx;
            DEVRM_TRACE("[Action:counter] hw counter idx = %lu", igr_action_p->stat_data.stat_idx);
        }
        else
        {
            rtk_acl_egrAction_t *egr_action_p = &action.egr_acl;
            egr_action_p->stat_en = ENABLED;
            egr_action_p->stat_data.stat_type = rtk_counter_mode;
            egr_action_p->stat_data.stat_idx = dev_rule_p->counter_idx;
            DEVRM_TRACE("[Action:counter] hw counter idx = %lu", egr_action_p->stat_data.stat_idx);
        }

        ret = rtk_acl_ruleAction_set(device_id, (rtk_acl_phase_t) group_p->phase,
                                     hw_rule_idx, &action);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("rtk_acl_ruleAction_set fail");
            return FALSE;
        }

        /* update local OM
         */
        counter_p = _DEVRM_GetDevCounterEntry(device_id, dev_rule_p->counter_idx);
        if (counter_p == NULL)
        {
            ASSERT(0);
            return FALSE;
        }

        counter_p->type = rtk_counter_mode;
    }

    DEVRM_ClearCounter(device_id, dev_rule_id);

    if (TRUE == need_reinstalled)
    {
        /* FIXME: ??
         */
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a counter entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_DestroyCounter(
    UI32_T device_id,
    UI32_T dev_rule_id)
{
    BOOL_T b_ret;
    int ret;

    DEVRM_LOG("device_id=%lu, dev_rule_id=%lu", device_id, dev_rule_id);

    /* counter config action */
    {
        UI32_T group_id, hw_rule_idx;
        DEVRM_Group_T *group_p;
        DEVRM_Slice_T *slice_p;
        DEVRM_DevRule_T *dev_rule_p;
        rtk_acl_action_t action;

        _DEVRM_GetGroupIdByDevRuleId(dev_rule_id, &group_id);
        group_p = _DEVRM_GetGroupById(device_id, group_id);
        if (group_p == NULL)
        {
            ASSERT(0);
            return FALSE;
        }

        slice_p = _DEVRM_GetSliceByIdx(device_id, group_p->slice_id);
        if (NULL == slice_p)
        {
            return FALSE;
        }

        dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
        if (dev_rule_p == NULL)
        {
            ASSERT(0);
            return FALSE;
        }

        hw_rule_idx = DEVRM_SLICE_RULE_IDX_TO_HW_IDX(slice_p->idx, dev_rule_p->slice_rule_index);

        ret = rtk_acl_ruleAction_get(device_id, (rtk_acl_phase_t) group_p->phase,
                                     hw_rule_idx, &action);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("rtk_acl_ruleAction_get fail");
            return FALSE;
        }

        if (group_p->phase == DEVRM_PHASE_INGRESS)
        {
            rtk_acl_igrAction_t *igr_action_p = &action.igr_acl;
            igr_action_p->stat_en = DISABLED;
        }
        else
        {
            rtk_acl_egrAction_t *egr_action_p = &action.egr_acl;
            egr_action_p->stat_en = DISABLED;
        }

        ret = rtk_acl_ruleAction_set(device_id, (rtk_acl_phase_t) group_p->phase,
                                     hw_rule_idx, &action);
        if (ret != RT_ERR_OK)
        {
            DEVRM_ERR("rtk_acl_ruleAction_set fail");
            return FALSE;
        }
    }

    b_ret = _DEVRM_DestroyDevCounter(device_id, dev_rule_id);
    if(b_ret == FALSE)
    {
        DEVRM_ERR("_DEVRM_DestroyDevMeter fail");
        return FALSE;
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
    UI32_T dev_rule_id,
    UI32_T *counter_p)
{
    int ret;
    UI32_T group_id;
    DEVRM_Group_T *group_p;
    DEVRM_DevRule_T *dev_rule_p;
    DEVRM_DevCounter_T  *dev_counter_p;

    DEVRM_LOG("device_id=%lu, dev_rule_id=%lu", device_id, dev_rule_id);

    _DEVRM_GetGroupIdByDevRuleId(dev_rule_id, &group_id);
    group_p = _DEVRM_GetGroupById(device_id, group_id);
    if (group_p == NULL)
    {
        return FALSE;
    }

    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (dev_rule_p == NULL)
    {
        return FALSE;
    }

    dev_counter_p = _DEVRM_GetDevCounterEntry(device_id, dev_rule_p->counter_idx);
    if (dev_counter_p == NULL)
    {
        return FALSE;
    }

    if (dev_counter_p->type == STAT_TYPE_BYTE_BASED_64BIT)
    {
        uint64 byte_cnt;

        ret = rtk_acl_statByteCnt_get(device_id, dev_rule_p->counter_idx, &byte_cnt);
        if (ret != RT_ERR_OK)
            return FALSE;

        *counter_p = (UI32_T) byte_cnt;
    }
    else
    {
        ret = rtk_acl_statPktCnt_get(device_id, dev_rule_p->counter_idx, counter_p);
        if (ret != RT_ERR_OK)
            return FALSE;
    }


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
    UI32_T dev_rule_id)
{
    int ret;
    UI32_T group_id;
    DEVRM_Group_T *group_p;
    DEVRM_DevRule_T *dev_rule_p;
    DEVRM_DevCounter_T  *dev_counter_p;

    DEVRM_LOG("device_id=%lu, dev_rule_id=%lu", device_id, dev_rule_id);

    _DEVRM_GetGroupIdByDevRuleId(dev_rule_id, &group_id);
    group_p = _DEVRM_GetGroupById(device_id, group_id);
    if (group_p == NULL)
    {
        return FALSE;
    }

    dev_rule_p = _DEVRM_FindDevRule(device_id, group_id, dev_rule_id);
    if (dev_rule_p == NULL)
    {
        return FALSE;
    }

    dev_counter_p = _DEVRM_GetDevCounterEntry(device_id, dev_rule_p->counter_idx);
    if (dev_counter_p == NULL)
    {
        return FALSE;
    }

    if (dev_counter_p->type == STAT_TYPE_BYTE_BASED_64BIT)
    {
        ret = rtk_acl_statByteCnt_clear(device_id, dev_rule_p->counter_idx);
        if (ret != RT_ERR_OK)
            return FALSE;
    }
    else
    {
        ret = rtk_acl_statPktCnt_clear(device_id, dev_rule_p->counter_idx);
        if (ret != RT_ERR_OK)
            return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CheckSoc
 *------------------------------------------------------------------------------
 * PURPOSE  : check soc
 * INPUT    : None
 * OUTPUT   : NONE.
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_CheckSoc(
    UI32_T device_id)
{
    if (soc_ndev <= device_id)
    {
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
BOOL_T DEVRM_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p)
{
    DEVRM_PMGR_IPCMSG_T   *dev_rm_ipcmsg;

    if(ipcmsg_p==NULL)
        return FALSE;

    dev_rm_ipcmsg = (DEVRM_PMGR_IPCMSG_T*)ipcmsg_p->msg_buf;

    switch(dev_rm_ipcmsg->type.cmd)
    {
        case DEVRM_IPCCMD_CREATEGROUP:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                sizeof(dev_rm_ipcmsg->data.setgroup.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_CreateGroup(dev_rm_ipcmsg->data.setgroup.req.device_id,
                                                                &dev_rm_ipcmsg->data.setgroup.req.group_entry);
            memcpy(&dev_rm_ipcmsg->data.setgroup.resp.group_entry,
                   &dev_rm_ipcmsg->data.setgroup.req.group_entry,
                   sizeof(dev_rm_ipcmsg->data.setgroup.resp.group_entry));
            break;

        case DEVRM_IPCCMD_SET_GROUP_QUALIFY_SET:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                sizeof(dev_rm_ipcmsg->data.setgroup.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_SetGroupQualifySet(dev_rm_ipcmsg->data.setgroup.req.device_id,
                                                                       &dev_rm_ipcmsg->data.setgroup.req.group_entry);
            memcpy(&dev_rm_ipcmsg->data.setgroup.resp.group_entry,
                   &dev_rm_ipcmsg->data.setgroup.req.group_entry,
                   sizeof(dev_rm_ipcmsg->data.setgroup.resp.group_entry));
            break;

//        case DEVRM_IPCCMD_SET_GROUP_PRIORITY:
//            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
//                sizeof(dev_rm_ipcmsg->data.set_group_priority.resp);
//            dev_rm_ipcmsg->type.result_bool = DEVRM_SetGroupPriority(dev_rm_ipcmsg->data.set_group_priority.req.device_id,
//                                                                     dev_rm_ipcmsg->data.set_group_priority.req.group_id,
//                                                                     dev_rm_ipcmsg->data.set_group_priority.req.priority);
//            break;

        case DEVRM_IPCCMD_DESTROYGROUP:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                sizeof(dev_rm_ipcmsg->data.destroygroup.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_DestroyGroup(dev_rm_ipcmsg->data.destroygroup.req.device_id,
                                                                 dev_rm_ipcmsg->data.destroygroup.req.group_id);
            break;

        case DEVRM_IPCCMD_CREATE_VIR_GROUP:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                sizeof(dev_rm_ipcmsg->data.create_vir_group.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_CreateVirtualGroup(dev_rm_ipcmsg->data.create_vir_group.req.group_id,
                                                                       dev_rm_ipcmsg->data.create_vir_group.req.vir_group_id,
                                                                       dev_rm_ipcmsg->data.create_vir_group.req.priority);
            break;

        case DEVRM_IPCCMD_ALLOCATERULE:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                sizeof(dev_rm_ipcmsg->data.allocaterule.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_AllocateRule(dev_rm_ipcmsg->data.allocaterule.req.device_id,
                                                                 dev_rm_ipcmsg->data.allocaterule.req.group_id,
                                                                 dev_rm_ipcmsg->data.allocaterule.req.vir_group_id,
                                                                 dev_rm_ipcmsg->data.allocaterule.req.rule_pri,
                                                                 &dev_rm_ipcmsg->data.allocaterule.resp.dev_rule_id);
            break;

        case DEVRM_IPCCMD_SETRULE:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                sizeof(dev_rm_ipcmsg->data.setrule.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_SetRule(dev_rm_ipcmsg->data.setrule.req.device_id,
                                                            dev_rm_ipcmsg->data.setrule.req.group_id,
                                                            dev_rm_ipcmsg->data.setrule.req.dev_rule_id,
                                                            &dev_rm_ipcmsg->data.setrule.req.ace_entry,
                                                            &dev_rm_ipcmsg->data.setrule.req.action);
            break;

        case DEVRM_IPCCMD_UPDATERULE:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.updaterule.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_UpdateRule(dev_rm_ipcmsg->data.updaterule.req.device_id,
                                                               dev_rm_ipcmsg->data.updaterule.req.dev_rule_id,
                                                               &dev_rm_ipcmsg->data.updaterule.req.ace_entry,
                                                               &dev_rm_ipcmsg->data.updaterule.req.action);
            break;

        case DEVRM_IPCCMD_DESTROYRULE:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                sizeof(dev_rm_ipcmsg->data.destroyrule.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_DestroyRule(dev_rm_ipcmsg->data.destroyrule.req.device_id,
                                                                dev_rm_ipcmsg->data.destroyrule.req.vir_group_id,
                                                                dev_rm_ipcmsg->data.destroyrule.req.dev_rule_id);
            break;

        case DEVRM_IPCCMD_CREATEMETER:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                sizeof(dev_rm_ipcmsg->data.create_meter.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_CreateMeter(dev_rm_ipcmsg->data.create_meter.req.device_id,
                                                                dev_rm_ipcmsg->data.create_meter.req.dev_rule_id,
                                                                &dev_rm_ipcmsg->data.create_meter.req.meter_entry,
                                                                dev_rm_ipcmsg->data.create_meter.req.need_reinstalled);
            break;

        case DEVRM_IPCCMD_UPDATEMETER:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.update_meter.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_UpdateMeter(dev_rm_ipcmsg->data.update_meter.req.device_id,
                                                                dev_rm_ipcmsg->data.update_meter.req.dev_rule_id,
                                                                &dev_rm_ipcmsg->data.update_meter.req.meter_entry);
            break;

        case DEVRM_IPCCMD_SHAREMETER:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.share_meter.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_ShareMeter(dev_rm_ipcmsg->data.share_meter.req.device_id,
                                                               dev_rm_ipcmsg->data.share_meter.req.src_rule_id,
                                                               dev_rm_ipcmsg->data.share_meter.req.dst_rule_id);
            break;

        case DEVRM_IPCCMD_DESTROYMETER:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.destroymeter.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_DestroyMeter(dev_rm_ipcmsg->data.destroymeter.req.device_id,
                                                                 dev_rm_ipcmsg->data.destroymeter.req.dev_rule_id);
            break;

        case DEVRM_IPCCMD_SETACTION:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.setaction.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_SetAction(dev_rm_ipcmsg->data.setaction.req.device_id,
                                                              dev_rm_ipcmsg->data.setaction.req.dev_rule_id,
                                                              &dev_rm_ipcmsg->data.setaction.req.action);
            break;

        case DEVRM_IPCCMD_DESTROYACTION:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.destroy_action.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_DestroyAction(dev_rm_ipcmsg->data.destroy_action.req.device_id,
                                                                  dev_rm_ipcmsg->data.destroy_action.req.dev_rule_id);
            break;

        case DEVRM_IPCCMD_GET_COUNTER:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                sizeof(dev_rm_ipcmsg->data.get_counter.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_GetCounter(dev_rm_ipcmsg->data.get_counter.req.device_id,
                                                               dev_rm_ipcmsg->data.get_counter.req.dev_rule_id,
                                                               &dev_rm_ipcmsg->data.get_counter.resp.counter);
            break;

        case DEVRM_IPCCMD_CLEAR_COUNTER:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE;
            dev_rm_ipcmsg->type.result_bool = DEVRM_ClearCounter(dev_rm_ipcmsg->data.clear_counter.req.device_id,
                                                                 dev_rm_ipcmsg->data.clear_counter.req.dev_rule_id);
            break;

        case DEVRM_IPCCMD_GETNUMBEROFCHIPS:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.getnumberofchips.resp);
            dev_rm_ipcmsg->type.result_ui32 = _DEVRM_GetNumberOfChips();
            break;

        case DEVRM_IPCCMD_GET_DEVICE_INFOMATION:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE+
            sizeof(dev_rm_ipcmsg->data.get_device_info.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_GetDeviceInfo(&dev_rm_ipcmsg->data.get_device_info.resp.device_info);
            break;

        case DEVRM_IPCCMD_CHECKSOC:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.checksoc.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_CheckSoc(dev_rm_ipcmsg->data.checksoc.req.device_id);
            break;

        case DEVRM_IPCCMD_FIELDINIT:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.fieldinit.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_Initial();
            break;

        default:
            SYSFUN_Debug_Printf("%s(): Invalid cmd(%d).\n", __FUNCTION__, (int)(ipcmsg_p->cmd));
            dev_rm_ipcmsg->type.result_bool = DEVRM_PMGR_RESP_RESULT_FAIL;
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE;
            break;
    }

    return TRUE;
}
