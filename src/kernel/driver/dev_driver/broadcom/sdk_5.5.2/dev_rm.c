/* FUNCTION NAME: DEV_RM.C
* PURPOSE:
*         Only Support BCM XGSIII 5630X/5650X/5660X chip
* NOTES:
*
* REASON:
*    DESCRIPTION:
*    CREATOR:
*    Date          04/19/2005
*
* Copyright(C)      Accton Corporation, 2005
*/

/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <assert.h>
#include "sysfun.h"
#ifdef BCM_ROBO_SUPPORT
#include "robo/dev_rm.h"
#else
#include "esw/dev_rm.h"
#endif
#include "dev_rm_pmgr.h"
//#include <sal/core/libc.h>
//#include <soc/drv.h>
#include <bcm/types.h>
//#include <bcm/error.h>
#include <bcm/port.h>
//#include <bcm/meter.h>
#include <bcm/field.h>
#include "backdoor_mgr.h"

/* NAMING CONSTANT DECLARATIONS
 */
/* MACRO FUNCTION DECLARATIONS
 */
#define DEVRM_IS_BAD_DEVICE_ID(id)                      (id >= SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT)

#define DEVRM_MAX_NBR_OF_VIR_GROUP                      50 /* RULE_TYPE_FUNCTION_TYPE_MAX */

#ifndef _countof
#define _countof(a)                                     ( sizeof(a) / sizeof(*a) )
#endif

#ifndef ASSERT
#define ASSERT(eq)
#endif

#ifndef NEWLINE
#define NEWLINE                                         "\r\n"
#endif

#define DEVRM_IS_DEBUG_ENABLED()                        (debug_flag)

#define DEVRM_PRINT_HEADER()                            \
    {                                                   \
      BACKDOOR_MGR_Printf("%s (%d): ",                  \
            __FUNCTION__, __LINE__);                    \
    }

#define DEVRM_PRINT(fmt, ...)                           \
    {                                                   \
        BACKDOOR_MGR_Printf(fmt, ##__VA_ARGS__);        \
    }

#define DEVRM_LOG(fmt,...)                              \
    {                                                   \
        if (DEVRM_IS_DEBUG_ENABLED())                   \
        {                                               \
            DEVRM_PRINT_HEADER();                       \
            DEVRM_PRINT(fmt NEWLINE, ##__VA_ARGS__);    \
        }                                               \
    }

#define DEVRM_ERR(fmt,...)                              \
    {                                                   \
        DEVRM_PRINT_HEADER();                           \
        DEVRM_PRINT(fmt NEWLINE, ##__VA_ARGS__);        \
    }

#define DEVRM_VALIDATE_RULE()                           \
    {                                                   \
        if (devrm_validate_rule_func)                   \
        {                                               \
            devrm_validate_rule_func();                 \
        }                                               \
    }

/* whether bno-th bit in bmp is 1 or 0 (bmp is an UI8_T array, bno is a zero-based bit index) */
#define DEVRM_IS_BIT_ON(bmp, bno)         (bmp[bno/8] & (1 << (7 - bno%8)))
#define DEVRM_IS_BIT_OFF(bmp, bno)        (!RULE_OM_IS_BIT_ON(bmp, bno))
#define DEVRM_SET_BIT_ON(bmp, bno)        {bmp[bno/8] |= (1 << (7 - bno%8));}
#define DEVRM_SET_BIT_OFF(bmp, bno)       {bmp[bno/8] &= ~(1 << (7 - bno%8));}

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

#ifdef _DEBUG
    #define DEVRM_MAX_RULE_PER_VIR_GROUP    10
#else
    #define DEVRM_MAX_RULE_PER_VIR_GROUP    SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR
#endif

    DEVRM_Rule_T        rule_tbl[DEVRM_MAX_RULE_PER_VIR_GROUP]; /* index is map to rule priority */
} DEVRM_RuleVector_T;

typedef struct
{
    int                 pri_base;
    int                 pri_lvl;        /* priority level, used for sort each
                                         * virtual group in a group
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

typedef void (*DEVRM_VALIDATE_RULE_FUNC_T)();

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static int DEVRM_FieldQualify(UI32_T device_id, UI32_T dev_rule_id, DEVRM_AceEntry_T *ace_entry);

/*delete device rule and move together*/
static BOOL_T DEVRM_DeleteDevRule(UI32_T device_id, UI32_T dev_rule_id);
static UI32_T DEVRM_GetNumberOfChips();
static BOOL_T DEVRM_CheckSoc(UI32_T device_id);

static BOOL_T
DEVRM_GetDeviceInfo(
    DEVRM_DevInfo_T *dev_info_p
);

static BOOL_T
DEVRM_SetupAction(
    UI32_T device_id,
    UI32_T dev_rule_id,
    UI32_T action_entry_num,
    const DEVRM_ActionEntry_T *action_entry_p
);

/*******************************************************************************
   Group
 *******************************************************************************
 */

static BOOL_T _DEVRM_IsGroupExisted(
    UI32_T device_id,
    UI32_T idx);

/*******************************************************************************
   Virtual Group
 *******************************************************************************
 */
static void _DEVRM_InitVirtualGroup();

static DEVRM_VirtualGroup_T * _DEVRM_GetVirtualGroupAtIndex(
    UI32_T device_id,
    UI32_T idx);

static DEVRM_VirtualGroup_T * _DEVRM_FindVirtualGroup(
    UI32_T device_id,
    UI32_T vir_group_id);

static BOOL_T _DEVRM_AddRuleToVirtualGroupAndSetPriority(
    UI32_T device_id,
    UI32_T vir_group_id,
    int rule_pri,
    UI32_T dev_rule_id);

static BOOL_T _DEVRM_RemoveRuleFromVirtualGroup(
    UI32_T device_id,
    UI32_T vir_group_id,
    UI32_T dev_rule_id);

static BOOL_T _DEVRM_SortVirtualGroup(
    UI32_T device_id);

static BOOL_T _DEVRM_CompactRule(
    DEVRM_RuleVector_T *rv_p);

static BOOL_T _DEVRM_InsertRuleAtIndex(
    DEVRM_RuleVector_T *rv_p,
    UI32_T rule_idx,
    int rule_pri,
    UI32_T dev_rule_id);

static BOOL_T _DEVRM_FindRuleIndexByRuleId(
    DEVRM_RuleVector_T *rv_p,
    UI32_T dev_rule_id,
    UI32_T *rule_idx
    );

static BOOL_T _DEVRM_RemoveRuleAtIndex(
    DEVRM_RuleVector_T *rv_p,
    UI32_T rule_idx);

static BOOL_T _DEVRM_CalculateIndexForNewRuleByPriority(
    const DEVRM_RuleVector_T *rv_p,
    int rule_pri,
    UI32_T *rule_idx_p);

static void _DEVRM_DumbRuleVector(
    DEVRM_RuleVector_T *rv_p);



static DEVRM_VirtualGroupVector_T devrm_vir_grp[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT];

static BOOL_T                       debug_flag = 0;
static DEVRM_VALIDATE_RULE_FUNC_T   devrm_validate_rule_func;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
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
BOOL_T DEVRM_Initial()
{
    int ret_value = BCM_E_NONE;
    int device_id;

    for(device_id = 0; device_id < soc_ndev; device_id++)
    {
       ret_value = bcm_field_init(device_id);

        if (BCM_E_NONE != ret_value)
        {
            printf("DEVRM_Initial Error! bcm_field_init[%d]\n", ret_value);
            return FALSE; /* init fail */
        }

        if ( (SOC_IS_RAVEN(device_id)) || (SOC_IS_FIREBOLT2(device_id)) || (SOC_IS_FIREBOLT(device_id)) )
        {
            /*  2008-08-12, Jinfeng Chen:
                Initialize packet interval as 20 bytes.
             */
            ret_value = bcm_meter_control_set(device_id, 20);
            if (BCM_E_NONE != ret_value)
            {
                printf("DEVRM_Initial Error! bcm_meter_control_set[%d]\n", ret_value);
            }

            ret_value = bcm_egress_packet_ifg_set(device_id, 20);

            if (BCM_E_NONE != ret_value)
            {
                printf("DEVRM_Initial Error! bcm_egress_packet_ifg_set[%d]\n", ret_value);
            }
        }
        else if (SOC_IS_APOLLO(device_id))
        {
            ret_value = bcm_switch_control_set(device_id, bcmSwitchMeterAdjust, 20);
            if (ret_value != BCM_E_NONE && ret_value != BCM_E_UNAVAIL)
            {
                printf("DEVRM_Initial Error! bcm_switch_control_set meterAdjust[%d]\n", ret_value);
            }
        }
    }

    _DEVRM_InitVirtualGroup();

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetPriorityFromIFPSelectorBitmap
 *------------------------------------------------------------------------------
 * PURPOSE:  Convert selector bitmap to priority value
 * INPUT:    sel - selector bitmap
 * OUTPUT:   NONE.
 * RETURN:   Priority value
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
static int DEVRM_GetPriorityFromIFPSelectorBitmap(UI32_T sel)
{
    if (sel & DEVRM_IFP_SELECTOR_0)
        return 0;
    if (sel & DEVRM_IFP_SELECTOR_1)
        return 1;
    if (sel & DEVRM_IFP_SELECTOR_2)
        return 2;
    if (sel & DEVRM_IFP_SELECTOR_3)
        return 3;
    if (sel & DEVRM_IFP_SELECTOR_4)
        return 4;
    if (sel & DEVRM_IFP_SELECTOR_5)
        return 5;
    if (sel & DEVRM_IFP_SELECTOR_6)
        return 6;
    if (sel & DEVRM_IFP_SELECTOR_7)
        return 7;
    if (sel & DEVRM_IFP_SELECTOR_8)
        return 8;
    if (sel & DEVRM_IFP_SELECTOR_9)
        return 9;
    if (sel & DEVRM_IFP_SELECTOR_10)
        return 10;
    if (sel & DEVRM_IFP_SELECTOR_11)
        return 11;
    if (sel & DEVRM_IFP_SELECTOR_12)
        return 12;
    if (sel & DEVRM_IFP_SELECTOR_13)
        return 13;
    if (sel & DEVRM_IFP_SELECTOR_14)
        return 14;
    if (sel & DEVRM_IFP_SELECTOR_15)
        return 15;

    return -1;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a group entry.
 * INPUT:    device_id, group_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_CreateGroup(UI32_T device_id, DEVRM_GroupEntry_T *group_entry)
{
    bcm_field_qset_t    qset;
    int                 pri;
    int                 ret_value = BCM_E_NONE;
    int                 i;
    int                 show_debug_info = 0;


    if(NULL == group_entry)
    {
        return FALSE;
    }

    /* gordon_kao: there is no sal_memset to use, so mark it
     */
    /* BCM_FIELD_QSET_INIT(qset); */
    memset(&qset, 0, sizeof(bcm_field_qset_t));
    memcpy(qset.w, group_entry->w, sizeof(group_entry->w));

    if (show_debug_info)
    {
        UI8_T *select_bitmap = (UI8_T*)&group_entry->selector_bitmap;

        printf("\n");
        printf("%s\n",__FUNCTION__);
        printf("device id=%ld\n", device_id);
        printf("group id=%ld\n", group_entry->group_id);
        printf("group mode=%s\n",
            (group_entry->group_mode == DEVRM_GROUP_MODE_SINGLE)?"Single":
            (group_entry->group_mode == DEVRM_GROUP_MODE_DOUBLE)?"Double":
            (group_entry->group_mode == DEVRM_GROUP_MODE_TRIPLE)?"Triple":
            (group_entry->group_mode == DEVRM_GROUP_MODE_AUTO)  ?"Auto":"Invalid"
        );
        printf("select bitmap=%02X%02X%02X%02X\n",
            select_bitmap[0],
            select_bitmap[1],
            select_bitmap[2],
            select_bitmap[3]
        );
    }


    if (group_entry->selector_bitmap == 0)
    {
        printf("DEVRM_CreateGroup Error! selector bitmap is empty\n");
        return FALSE;
    }

    pri = DEVRM_GetPriorityFromIFPSelectorBitmap(group_entry->selector_bitmap);

    i = device_id;
    {
        int uid;

        /* Add user defined field to qset
         */
        for (uid = 0; uid < DEVRM_FIELD_USER_QUALIFY_Count; uid++)
        {
            if (group_entry->udf[uid].enabled)
            {
                int                     unit    = i;
                uint32                  flags   = group_entry->udf[uid].flags;
                uint32                  chunk   = group_entry->udf[uid].chunk;
                bcm_field_udf_t         udf_id  = group_entry->udf[uid].udf_id;
                bcm_field_udf_spec_t    udf_spec;

                memset(&udf_spec, 0, sizeof(udf_spec));
                ret_value = bcm_field_udf_spec_set(unit, &udf_spec, flags, chunk);
                if(ret_value != BCM_E_NONE)
                {
                    printf("%s Error! bcm_field_udf_spec_set[%d, %s]\n", __FUNCTION__, uid, bcm_errmsg(ret_value));
                    return FALSE;
                }

                if (udf_id == 0)
                {
                    ret_value = bcm_field_udf_create(unit, &udf_spec, &udf_id);
                }
                else
                {
                    ret_value = bcm_field_udf_create_id(unit, &udf_spec, udf_id);
                }

                if(ret_value != BCM_E_NONE)
                {
                    printf("%s Error! bcm_field_udf_create_id[%d, %s]\n", __FUNCTION__, uid, bcm_errmsg(ret_value));
                    return FALSE;
                }

                group_entry->udf[uid].udf_id = udf_id;

                ret_value = bcm_field_qset_add_udf(unit, &qset, udf_id);
                if(ret_value != BCM_E_NONE)
                {
                    printf("%s Error! bcm_field_qset_add_udf[%d, %s]\n", __FUNCTION__, uid, bcm_errmsg(ret_value));
                    return FALSE;
                }
            }
        }

        if (show_debug_info)
        {
            _field_qset_dump("qset", &qset, "\n");
        }

        ret_value = bcm_field_group_create_mode_id(i, qset, pri, group_entry->group_mode, group_entry->group_id);
        if(ret_value != BCM_E_NONE)
        {
            printf("group id=%ld\n", group_entry->group_id);
            printf("DEVRM_CreateGroup Error! bcm_field_group_create_mode_id[%s]\n", bcm_errmsg(ret_value));
            return FALSE;
        }
    }
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetGroupQualifySet
 *------------------------------------------------------------------------------
 * PURPOSE:  Set qualify set of group
 * INPUT:    device_id, group_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_SetGroupQualifySet(UI32_T device_id, DEVRM_GroupEntry_T *group_entry)
{
    bcm_field_qset_t    qset;
    int                 ret_value = BCM_E_NONE;

    memset(&qset, 0, sizeof(bcm_field_qset_t));
    memcpy(qset.w, group_entry->w, sizeof(group_entry->w));

    ret_value = bcm_field_group_set(device_id, group_entry->group_id, qset);

    if (BCM_E_NONE != ret_value)
    {
        DEVRM_LOG("[%s] devId=%lu, groupId=%lu", bcm_errmsg(ret_value), device_id, group_entry->group_id);
    }

    return (BCM_E_NONE == ret_value) ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetGroupPriority
 *------------------------------------------------------------------------------
 * PURPOSE:  Set priority of group
 * INPUT:    device_id, group_id, priority
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_SetGroupPriority(UI32_T device_id, UI32_T group_id, int priority)
{
    int                 ret_value = BCM_E_NONE;

    ret_value = bcm_field_group_priority_set(device_id, group_id, priority);

    if (BCM_E_NONE != ret_value)
    {
        DEVRM_LOG("[%s] devId=%lu, groupId=%lu", bcm_errmsg(ret_value), device_id, group_id);

        if (BCM_E_UNAVAIL == ret_value)
        {
            return TRUE;
        }
    }

    return (BCM_E_NONE == ret_value) ? TRUE : FALSE;
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
BOOL_T DEVRM_DestroyGroup(UI32_T device_id,UI32_T group_id)
{
    int  ret_value = BCM_E_NONE;

    ret_value = bcm_field_group_destroy(device_id, group_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_DestroyGroup Error! bcm_field_group_destroy[%d]\n",ret_value);
        return FALSE;
    }
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
BOOL_T DEVRM_CreateVirtualGroup(
    UI32_T group_id,
    UI32_T vir_group_id,
    int priority)
{
    DEVRM_VirtualGroup_T *vg_p;
    UI32_T device_id;

    for(device_id = 0; device_id < soc_ndev; device_id++)
    {
        if (FALSE == _DEVRM_IsGroupExisted(device_id, group_id))
        {
            DEVRM_ERR("_DEVRM_IsGroupExisted(device_id=%lu, group_id=%lu) failed",
                device_id, group_id);
            ASSERT(0);
            return FALSE;
        }

        vg_p = _DEVRM_GetVirtualGroupAtIndex(device_id, devrm_vir_grp[ device_id ].vsel_num);
        if (NULL == vg_p)
        {
            DEVRM_ERR("_DEVRM_GetVirtualGroupAtIndex(soc=%lu, idx=%lu) failed",
                device_id, devrm_vir_grp[ device_id ].vsel_num);
            return FALSE;
        }

        vg_p->group_id = group_id;
        vg_p->vir_group_id = vir_group_id;
        vg_p->pri_lvl = priority;

        vg_p->seq_no = devrm_vir_grp[ device_id ].vsel_num;

        vg_p->valid = TRUE;
        ++devrm_vir_grp[ device_id ].vsel_num;

        _DEVRM_SortVirtualGroup(device_id);
    }

    DEVRM_VALIDATE_RULE();

    return TRUE;
}

/* rule */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocateRule
 *------------------------------------------------------------------------------
 * PURPOSE  : Allocate rule resource
 * INPUT    : device_id     - device ID
 *            group_id      - group ID
 *            vir_group_id  - virtual group ID
 *            rule_pri      - rule priority (no used)
 * OUTPUT   : dev_rule_id (per chip unique),
 *
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, call DEVRM_CreateGroup() first
 *            2, MUST allocate rule resource before call DEVRM_SetRule()
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_AllocateRule(
    UI32_T device_id,
    UI32_T group_id, /* not necessary */
    UI32_T vir_group_id,
    int rule_pri,
    UI32_T *dev_rule_id)
{
    int                         ret_value = BCM_E_NONE;
    DEVRM_VirtualGroup_T        *vg_p;

    ASSERT(NULL != dev_rule_id);

    vg_p = _DEVRM_FindVirtualGroup(device_id, vir_group_id);
    if (NULL == vg_p)
    {
        DEVRM_ERR("devId = %lu, virGroup = %lu not exist", device_id, vir_group_id);
        return FALSE;
    }

    ASSERT(vg_p->group_id == group_id);

    ret_value = bcm_field_entry_create((int)device_id, (bcm_field_group_t)group_id, (bcm_field_entry_t *)dev_rule_id);
    if (ret_value != BCM_E_NONE)
    {
        printf("(int)device_id=%d,(bcm_field_group_t)group_id=%d\n", (int)device_id, (bcm_field_group_t)group_id);
        printf("DEVRM_AllocateRule Error! bcm_field_entry_create[%s]\n", bcm_errmsg(ret_value));
        return FALSE;
    }

    if (FALSE == _DEVRM_AddRuleToVirtualGroupAndSetPriority(device_id, vir_group_id, rule_pri, *dev_rule_id))
    {
        bcm_field_entry_destroy(device_id, (bcm_field_entry_t)*dev_rule_id);
        return FALSE;
    }

    DEVRM_VALIDATE_RULE();

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetupAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Add action to the field entry by device_id, dev_rule_id
 * INPUT:    device_id, dev_rule_id, action_entry_num, action_entry_p
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
DEVRM_SetupAction(
    UI32_T device_id,
    UI32_T dev_rule_id,
    UI32_T action_entry_num,
    const DEVRM_ActionEntry_T *action_entry_p)
{
    DEVRM_ActionEntry_T  *action_iter_p;
    UI32_T  i;
    int  ret_value;

    for (i = 0; i < action_entry_num; i++)
    {
        action_iter_p = (DEVRM_ActionEntry_T *) &action_entry_p[i];

        if (action_iter_p->action == bcmFieldActionRedirectPbmp ||
            action_iter_p->action == bcmFieldActionEgressMask ||
            action_iter_p->action == bcmFieldActionEgressPortsAdd ||
            action_iter_p->action == bcmFieldActionRedirectBcastPbmp)
        {
            ret_value = bcm_field_action_ports_add(device_id, dev_rule_id, action_iter_p->action, action_iter_p->pbmp);
            if(ret_value != BCM_E_NONE)
            {
                DEVRM_LOG("DEVRM_SetRule Error! bcm_field_action_ports_add[%s]", bcm_errmsg(ret_value));
                DEVRM_LOG("  action=%lu, p0=%lu(%lx), p1=%lu(%lx)",
                    action_iter_p->action,
                    action_iter_p->param0,
                    action_iter_p->param0,
                    action_iter_p->param1,
                    action_iter_p->param1);
                return FALSE;
            }
        }
        else
        {
            ret_value = bcm_field_action_add(device_id, dev_rule_id, action_iter_p->action,
                            action_iter_p->param0, action_iter_p->param1);
            if(ret_value != BCM_E_NONE)
            {
                DEVRM_LOG("DEVRM_SetRule Error! bcm_field_action_add[%s]", bcm_errmsg(ret_value));
                DEVRM_LOG("  action=%lu, p0=%lu(%lx), p1=%lu(%lx)",
                    action_iter_p->action,
                    action_iter_p->param0,
                    action_iter_p->param0,
                    action_iter_p->param1,
                    action_iter_p->param1);
                return FALSE;
            }
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    device_id, group_index, dev_rule_id, rule_entry, meter_entry, action_entry
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
    UI32_T group_index,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    DEVRM_MeterEntry_T *meter_entry,
    DEVRM_ActionEntry_T *action_entry,
    UI32_T action_entry_num,
    BOOL_T counter_enable)
{
    int ret_value = BCM_E_NONE;

    if (DEVRM_FieldQualify(device_id, dev_rule_id, ace_entry) != BCM_E_NONE)
    {
        printf("device id=%lu, group index=%lu, dev rule id=%lu\n", device_id, group_index, dev_rule_id);
    }

    /* MUST set logical id first
     * so that chain action can find itself rule id
     */

    /* add action iteratively */
    if (TRUE != DEVRM_SetupAction(device_id, dev_rule_id, action_entry_num,
                    action_entry))
    {
        return FALSE;
    }

    if(meter_entry != NULL)
    {
        /* we will install entry later, so we does not need to
           install entry in DEVRM_CreateMeter */

        if (FALSE == DEVRM_CreateMeter (device_id, dev_rule_id, meter_entry, FALSE))
        {
            printf("DEVRM_SetRule Error! DEVRM_CreateMeter failed.\n");
            return FALSE;
        }
    }

    if (TRUE == counter_enable)
    {
        DEVRM_CreateCounter(device_id, dev_rule_id, BCM_FIELD_COUNTER_MODE_GREEN_RED, FALSE);

#if 0
        /* green and red packets are calculated. */
        if (FALSE == DEVRM_CreateCounter (device_id, dev_rule_id, BCM_FIELD_COUNTER_MODE_GREEN_RED, FALSE))
        {
            printf("DEVRM_SetRule Error! DEVRM_CreateCounter failed.\n");
            return FALSE;
        }
#endif
    }

    ret_value = bcm_field_entry_install(device_id,dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_SetRule Error! bcm_field_entry_install([%s]\n", bcm_errmsg(ret_value));
        return FALSE;
    }

    DEVRM_VALIDATE_RULE();

    return TRUE;
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
BOOL_T DEVRM_UpdateRule(UI32_T device_id, UI32_T dev_rule_id, DEVRM_AceEntry_T *ace_entry, DEVRM_MeterEntry_T *meter_entry, DEVRM_ActionEntry_T *action_entry, UI32_T action_entry_num)
{
    int                     i, ret_value = BCM_E_NONE;
    DEVRM_ActionEntry_T     *action_iter_p;
    //UI32_T                  dev_chain_id;


    /*clear rule first*/
    ret_value = bcm_field_entry_remove(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_UpdateRule Error! bcm_field_entry_remove[%s]\n", bcm_errmsg(ret_value));
        return FALSE;
    }

    /* remove action setting */
    ret_value = bcm_field_action_remove_all(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_UpdateRule Error! bcm_field_action_remove_all[%s]\n", bcm_errmsg(ret_value));
        return FALSE;
    }

    /* remove meter setting */
    ret_value = bcm_field_meter_destroy(device_id, dev_rule_id);
    if ((BCM_E_NONE != ret_value) && (BCM_E_EMPTY != ret_value))
    {
        printf("DEVRM_UpdateRule Error! bcm_field_meter_destroy[%s]\n", bcm_errmsg(ret_value));
        return FALSE;
    }

    /* clear qualifier */
    ret_value = bcm_field_qualify_clear(device_id,dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_UpdateRule Error! bcm_field_qualify_clear[%s]\n", bcm_errmsg(ret_value));
        return FALSE;
    }

    /*set rule*/
    DEVRM_FieldQualify(device_id, dev_rule_id, ace_entry);

#if 0
    /* add action iteratively */
    for (action_iter_p = action_entry; NULL != action_iter_p; action_iter_p = action_iter_p->next_action)
    {
        /* translate login chain id to physical chain id */
        if ((bcmFieldActionChain == action_iter_p->action) &&
            (TRUE == DEVRM_GetDeviceRuleIDByLogicalID(device_id, action_iter_p->param0, &dev_chain_id)))
        {
            action_iter_p->param0 = dev_chain_id % SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR; /* offset */
        }

        ret_value = bcm_field_action_add(device_id, dev_rule_id, action_iter_p->action,
                        action_iter_p->param0, action_iter_p->param1);
        if(ret_value != BCM_E_NONE)
        {
            printf("DEVRM_UpdateRule Error! bcm_field_action_add[%d]\n",ret_value);
            return FALSE;
        }
    }
#endif
#if 1
    /* add action iteratively */
    for (i = 0; i < action_entry_num; i++)
    {
        action_iter_p = &action_entry[i];
        /* translate login chain id to physical chain id */
        /*if ((bcmFieldActionChain == action_iter_p->action) &&
            (TRUE == DEVRM_GetDeviceRuleIDByLogicalID(device_id, action_iter_p->param0, &dev_chain_id)))
        {
            action_iter_p->param0 = dev_chain_id % SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR;
        }*/

        ret_value = bcm_field_action_add(device_id, dev_rule_id, action_iter_p->action,
                        action_iter_p->param0, action_iter_p->param1);
        if(ret_value != BCM_E_NONE)
        {
            printf("DEVRM_UpdateRule Error! bcm_field_action_add[%s]\n", bcm_errmsg(ret_value));
            return FALSE;
        }
    }
#endif
    if(meter_entry != NULL)
    {
        ret_value = bcm_field_meter_create(device_id, dev_rule_id);
        if((ret_value != BCM_E_NONE) && (ret_value != BCM_E_EXISTS))
        {
            printf("DEVRM_UpdateRule Error! bcm_field_meter_create([%s]\n", bcm_errmsg(ret_value));
            return FALSE;
        }

        /* BCM_FIELD_METER_MODE_FLOW use peak meter, other modes use both meters. */
        if(BCM_FIELD_METER_MODE_FLOW != meter_entry->meter_mode)
        {
            ret_value = bcm_field_meter_set(device_id, dev_rule_id, BCM_FIELD_METER_COMMITTED, meter_entry->kbits_sec, meter_entry->kbytes_burst * 8);
            if(ret_value != BCM_E_NONE)
            {
                printf("DEVRM_UpdateRule Error! bcm_field_meter_set[%s]\n", bcm_errmsg(ret_value));
                return FALSE;
            }
        }

        ret_value = bcm_field_meter_set(device_id, dev_rule_id, BCM_FIELD_METER_PEAK, meter_entry->peak_kbits_sec, meter_entry->peak_kbytes_burst * 8);
        if(ret_value != BCM_E_NONE)
        {
            printf("DEVRM_UpdateRule Error! bcm_field_meter_set[%s]\n", bcm_errmsg(ret_value));
            return FALSE;
        }
        /* meter config action */
        ret_value = bcm_field_action_add(device_id, dev_rule_id, bcmFieldActionMeterConfig,
                        meter_entry->meter_mode, 0);
        if(ret_value != BCM_E_NONE)
        {
            printf("DEVRM_UpdateRule Error! bcm_field_action_add[%s]\n", bcm_errmsg(ret_value));
            return FALSE;
        }
    }
    //counter ??
    ret_value = bcm_field_entry_install(device_id,dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_UpdateRule Error! bcm_field_entry_install([%s]\n",bcm_errmsg(ret_value));
        return FALSE;
    }

    DEVRM_VALIDATE_RULE();

    return TRUE;
}


static int DEVRM_FieldQualify(UI32_T device_id, UI32_T dev_rule_id, DEVRM_AceEntry_T *ace_entry)
{
    int ret_value = BCM_E_NONE;

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_InPort))
    {
        bcm_port_t  data, mask;

        data = ace_entry->inport_data;
        mask = ace_entry->inport_mask;

        ret_value = bcm_field_qualify_InPort(device_id, dev_rule_id, data, mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_InPort[%s]\n", bcm_errmsg(ret_value));

    }

    /* 2008-07-01, Jinfeng Chen:
        Add this for egress support.
     */
    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_OutPort))
    {
        bcm_port_t  data, mask;

        data = ace_entry->outport_data;
        mask = ace_entry->outport_mask;

        ret_value = bcm_field_qualify_OutPort(device_id, dev_rule_id, data, mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_OutPort[%s]\n", bcm_errmsg(ret_value));

    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_InPorts))
    {
        bcm_pbmp_t  data, mask;
#if 0
        UI32_T      bmp_bits_nbr, port;
#endif

#if 1
        BCM_PBMP_CLEAR(data);
        BCM_PBMP_OR(data, ace_entry->inports_data);
        BCM_PBMP_CLEAR(mask);
        BCM_PBMP_OR(mask, ace_entry->inports_mask);
#else
        bmp_bits_nbr = sizeof(ace_entry->inports_data) * 8; /* to bits number */
        BCM_PBMP_CLEAR(data);
        for (port = 0; bmp_bits_nbr > port; ++port) /* chip port count from 0 */
        {
            if (ace_entry->inports_data & (1 << port))
            {
                BCM_PBMP_PORT_ADD(data, port);
            }
        }

        bmp_bits_nbr = sizeof(ace_entry->inports_mask) * 8; /* to bits number */
        BCM_PBMP_CLEAR(mask);
        for (port = 0; bmp_bits_nbr > port; ++port) /* chip port count from 0 */
        {
            if (ace_entry->inports_mask & (1 << port))
            {
                BCM_PBMP_PORT_ADD(mask, port);
            }
        }
#endif

        ret_value = bcm_field_qualify_InPorts(device_id, dev_rule_id, data, mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_InPorts[%s]\n", bcm_errmsg(ret_value));

    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_SrcModid))
    {
        ret_value = bcm_field_qualify_SrcModid(device_id, dev_rule_id, ace_entry->srcmodid_data, ace_entry->srcmodid_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_SrcModid[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_SrcPortTgid))
    {
        ret_value = bcm_field_qualify_SrcPortTgid(device_id, dev_rule_id, ace_entry->srcporttgid_data, ace_entry->srcporttgid_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_SrcPortTgid[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_DstModid))
    {
        ret_value = bcm_field_qualify_DstModid(device_id, dev_rule_id, ace_entry->dstmodid_data, ace_entry->dstmodid_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_DstModid[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_DstPortTgid))
    {
        ret_value = bcm_field_qualify_DstPortTgid(device_id, dev_rule_id, ace_entry->dstporttgid_data, ace_entry->dstporttgid_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_DstPortTgid[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_L4SrcPort))
    {
        ret_value = bcm_field_qualify_L4SrcPort(device_id, dev_rule_id, ace_entry->l4_srcport_data, ace_entry->l4_srcport_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_L4SrcPort[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_L4DstPort))
    {
        ret_value = bcm_field_qualify_L4DstPort(device_id, dev_rule_id, ace_entry->l4_dstport_data, ace_entry->l4_dstport_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_L4DstPort[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_OuterVlan))
    {
        ret_value = bcm_field_qualify_OuterVlan(device_id, dev_rule_id, ace_entry->outvlan_data, ace_entry->outvlan_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_OuterVlan[%s]\n", bcm_errmsg(ret_value));
    }

    if (DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_OuterVlanPri))
    {
        ret_value = bcm_field_qualify_OuterVlanPri(device_id, dev_rule_id, ace_entry->outvlanpri_data, ace_entry->outvlanpri_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_OuterVlanPri[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_InnerVlan))
    {
        ret_value = bcm_field_qualify_InnerVlan(device_id, dev_rule_id, ace_entry->invlan_data, ace_entry->invlan_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_InnerVlan[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_EtherType))
    {
        ret_value = bcm_field_qualify_EtherType(device_id, dev_rule_id, ace_entry->ethertype_data, ace_entry->ethertype_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_EtherType[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_IpProtocol))
    {
        ret_value = bcm_field_qualify_IpProtocol(device_id, dev_rule_id, ace_entry->ipprotocol_data, ace_entry->ipprotocol_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_IpProtocol[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_IpType))
    {
        ret_value = bcm_field_qualify_IpType(device_id, dev_rule_id, ace_entry->iptype);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_IpType[%s]\n", bcm_errmsg(ret_value));
    }

#if 0 /* obsolete by BCM SDK 6, use IngressStpState, L2SrcHit, L2DestHit, L3Routable, L3DestHostHit, ... instead. */
    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_LookupStatus))
    {
        ret_value = bcm_field_qualify_LookupStatus(device_id, dev_rule_id, ace_entry->lookupstatus_data, ace_entry->lookupstatus_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_LookupStatus[%s]\n", bcm_errmsg(ret_value));
    }
#endif

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_IngressStpState))
    {
        ret_value = bcm_field_qualify_IngressStpState(device_id, dev_rule_id, ace_entry->stpstate, BCM_FIELD_EXACT_MATCH_MASK);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_IngressStpState[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_L2SrcHit))
    {
        ret_value = bcm_field_qualify_L2SrcHit(device_id, dev_rule_id, ace_entry->l2srchit_data, ace_entry->l2srchit_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_L2SrcHit[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_L2DestHit))
    {
        ret_value = bcm_field_qualify_L2DestHit(device_id, dev_rule_id, ace_entry->l2dsthit_data, ace_entry->l2dsthit_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_L2DestHit[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_L3Routable))
    {
        ret_value = bcm_field_qualify_L3Routable(device_id, dev_rule_id, ace_entry->l3routable_data, ace_entry->l3routable_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_L3Routable[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_L3DestHostHit))
    {
        ret_value = bcm_field_qualify_L3DestHostHit(device_id, dev_rule_id, ace_entry->l3dsthosthit_data, ace_entry->l3dsthosthit_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_L3DestHostHit[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_HiGig))
    {
        ret_value = bcm_field_qualify_HiGig(device_id, dev_rule_id, ace_entry->higpacket_data, ace_entry->higpacket_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_HiGig[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_IpInfo))
    {
        ret_value = bcm_field_qualify_IpInfo(device_id, dev_rule_id, ace_entry->ipinfo_data, ace_entry->ipinfo_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_IpInfo[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_PacketRes))
    {
        ret_value = bcm_field_qualify_PacketRes(device_id, dev_rule_id, ace_entry->packetres_data, ace_entry->packetres_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_PacketRes[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_SrcIp))
    {
        ret_value = bcm_field_qualify_SrcIp(device_id, dev_rule_id, ace_entry->srcip_data, ace_entry->srcip_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_SrcIp[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_DstIp))
    {
        ret_value = bcm_field_qualify_DstIp(device_id, dev_rule_id, ace_entry->dstip_data, ace_entry->dstip_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_DstIp[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_DSCP))
    {
        ret_value = bcm_field_qualify_DSCP(device_id, dev_rule_id, ace_entry->dscp_data, ace_entry->dscp_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_DSCP[%s]\n", bcm_errmsg(ret_value));
    }


    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_IpFlags))
    {
        ret_value = bcm_field_qualify_IpFlags(device_id, dev_rule_id, ace_entry->ipflags_data, ace_entry->ipflags_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_IpFlags[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_TcpControl))
    {
        ret_value = bcm_field_qualify_TcpControl(device_id, dev_rule_id, ace_entry->tcpcontrol_data, ace_entry->tcpcontrol_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_TcpControl[%s]\n", bcm_errmsg(ret_value));
    }


    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_Ttl))
    {
        ret_value = bcm_field_qualify_Ttl(device_id, dev_rule_id, ace_entry->ttl_data, ace_entry->ttl_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_Ttl[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_RangeCheck))
    {
        ret_value = bcm_field_qualify_RangeCheck(device_id, dev_rule_id, ace_entry->range, ace_entry->invert);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_RangeCheck[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_SrcIp6))
    {
        ret_value = bcm_field_qualify_SrcIp6(device_id, dev_rule_id, ace_entry->srcip6_data, ace_entry->srcip6_mask);
/* patch ipv6
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_SrcIp6[%d]\n",ret_value);
*/
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_DstIp6))
    {
        ret_value = bcm_field_qualify_DstIp6(device_id, dev_rule_id, ace_entry->dstip6_data, ace_entry->dstip6_mask);
/* patch ipv6
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_DstIp6[%d]\n",ret_value);
*/
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_DstIp6High))
    {
        ret_value = bcm_field_qualify_DstIp6High(device_id, dev_rule_id, ace_entry->dstip6high_data, ace_entry->dstip6high_mask);
/* patch ipv6
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_DstIp6High[%d]\n",ret_value);
*/
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_Ip6NextHeader))
    {
        ret_value = bcm_field_qualify_Ip6NextHeader(device_id, dev_rule_id, ace_entry->ip6nextheader_data, ace_entry->ip6nextheader_mask);
/* patch ipv6
      if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_Ip6NextHeader[%d]\n",ret_value);
*/
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_Ip6TrafficClass))
    {
        ret_value = bcm_field_qualify_Ip6TrafficClass(device_id, dev_rule_id, ace_entry->ip6trafficclass_data, ace_entry->ip6trafficclass_mask);
/* patch ipv6
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_Ip6TrafficClass[%d]\n",ret_value);
*/
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_Ip6FlowLabel))
    {
        ret_value = bcm_field_qualify_Ip6FlowLabel(device_id, dev_rule_id, ace_entry->ip6flowlabel_data, ace_entry->ip6flowlabel_mask);
/* patch ipv6
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_Ip6FlowLabel[%d]\n",ret_value);
*/
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_Ip6HopLimit))
    {
        ret_value = bcm_field_qualify_Ip6HopLimit(device_id, dev_rule_id, ace_entry->ip6hoplimit_data, ace_entry->ip6hoplimit_mask);
/* patch ipv6
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_Ip6HopLimit[%d]\n",ret_value);
*/
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_SrcMac))
    {
        ret_value = bcm_field_qualify_SrcMac(device_id, dev_rule_id, ace_entry->srcmac_data, ace_entry->srcmac_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_SrcMac[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_DstMac))
    {
        ret_value = bcm_field_qualify_DstMac(device_id, dev_rule_id, ace_entry->dstmac_data, ace_entry->dstmac_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_DstMac[%s]\n", bcm_errmsg(ret_value));
    }

#if 0 /* obsolete by BCM SDK 6, use IpType, L2Format, VlanFormat instead. */
    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_PacketFormat))
    {
        ret_value = bcm_field_qualify_PacketFormat(device_id, dev_rule_id, ace_entry->packetformat_data, ace_entry->packetformat_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_PacketFormat[%s]\n", bcm_errmsg(ret_value));
    }
#endif

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_L2Format))
    {
        ret_value = bcm_field_qualify_L2Format(device_id, dev_rule_id, ace_entry->l2format);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_L2Format[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_VlanFormat))
    {
        ret_value = bcm_field_qualify_VlanFormat(device_id, dev_rule_id, ace_entry->vlanformat_data, ace_entry->vlanformat_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_VlanFormat[%s]\n", bcm_errmsg(ret_value));
    }

    if(DEVRM_SHR_BITGET(ace_entry->w, DEVRM_FIELD_QUALIFY_MHOpcode))
    {
        ret_value = bcm_field_qualify_MHOpcode(device_id, dev_rule_id, ace_entry->mhopcode_data, ace_entry->mhopcode_mask);
        if (ret_value != BCM_E_NONE)
            printf("DEVRM_FieldQualify Error! bcm_field_qualify_MHOpcode[%s]\n", bcm_errmsg(ret_value));
    }

    {
        int uid;

        for (uid = 0; uid < DEVRM_FIELD_USER_QUALIFY_Count; uid++)
        {
            if (DEVRM_SHR_BITGET(ace_entry->udf_map, uid))
            {
                ret_value = bcm_field_qualify_UserDefined(device_id,
                                                          dev_rule_id,
                                                          ace_entry->udf[uid].udf_id,
                                                          ace_entry->udf[uid].udf_data,
                                                          ace_entry->udf[uid].udf_mask);
                if (ret_value != BCM_E_NONE)
                    printf("DEVRM_FieldQualify Error! bcm_field_qualify_UserDefined[%d, %s]\n", uid, bcm_errmsg(ret_value));
            }
        }
    }

    return ret_value;
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
BOOL_T DEVRM_DestroyRule(
    UI32_T device_id,
    UI32_T vir_group_id,
    UI32_T dev_rule_id)
{
    if (FALSE == DEVRM_DeleteDevRule(device_id, dev_rule_id))
    {
        return FALSE;
    }

    if (FALSE == _DEVRM_RemoveRuleFromVirtualGroup(device_id, vir_group_id, dev_rule_id))
    {
        ASSERT(0);
        return FALSE;
    }

    DEVRM_VALIDATE_RULE();

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
BOOL_T DEVRM_CreateMeter(UI32_T device_id,UI32_T dev_rule_id, DEVRM_MeterEntry_T *meter_entry, BOOL_T need_reinstalled)
{
    int     ret_value = BCM_E_NONE;

    if (NULL == meter_entry)
        return FALSE;

    ret_value = bcm_field_meter_create(device_id, dev_rule_id);
    if((ret_value != BCM_E_NONE) && (ret_value != BCM_E_EXISTS))
    {
        printf("DEVRM_CreateMeter Error! bcm_field_meter_create([%d]\n",ret_value);
        return FALSE;
    }

    /* BCM_FIELD_METER_MODE_FLOW use peak meter, other modes use both meters. */
    if (BCM_FIELD_METER_MODE_FLOW == meter_entry->meter_mode)
    {
        ret_value = bcm_field_meter_set(device_id, dev_rule_id, BCM_FIELD_METER_PEAK, meter_entry->kbits_sec, meter_entry->kbytes_burst * 8);

        if (ret_value != BCM_E_NONE)
        {
            printf("DEVRM_CreateMeter Error! bcm_field_meter_set[%d]\n",ret_value);
            return FALSE;
        }

    }
    else
    {
        ret_value = bcm_field_meter_set(device_id, dev_rule_id, BCM_FIELD_METER_COMMITTED, meter_entry->kbits_sec, meter_entry->kbytes_burst * 8);

        if(ret_value != BCM_E_NONE)
        {
            printf("DEVRM_CreateMeter Error! bcm_field_meter_set[%d]\n",ret_value);
            return FALSE;
        }

        ret_value = bcm_field_meter_set(device_id, dev_rule_id, BCM_FIELD_METER_PEAK, meter_entry->peak_kbits_sec, meter_entry->peak_kbytes_burst * 8);

        if(ret_value != BCM_E_NONE)
        {
            printf("DEVRM_CreateMeter Error! bcm_field_meter_set[%d]\n",ret_value);
            return FALSE;
        }
    }


    /* meter config action */
    ret_value = bcm_field_action_add(device_id, dev_rule_id, bcmFieldActionMeterConfig,
                    meter_entry->meter_mode, 0);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_CreateMeter Error! bcm_field_action_add[%d]\n",ret_value);
        return FALSE;
    }

    if (TRUE == need_reinstalled)
    {
        ret_value = bcm_field_entry_reinstall(device_id, dev_rule_id);
        if(ret_value != BCM_E_NONE)
        {
            printf("DEVRM_CreateMeter Error! bcm_field_entry_reinstall[%d]\n",ret_value);
            return FALSE;
        }
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
BOOL_T DEVRM_UpdateMeter(UI32_T device_id,UI32_T dev_rule_id, DEVRM_MeterEntry_T *meter_entry)
{
    int     ret_value = BCM_E_NONE;

    if (NULL == meter_entry)
        return FALSE;


    /* BCM_FIELD_METER_MODE_FLOW use peak meter, other modes use both meters. */
    if(BCM_FIELD_METER_MODE_FLOW != meter_entry->meter_mode)
    {
        ret_value = bcm_field_meter_set(device_id, dev_rule_id, BCM_FIELD_METER_COMMITTED, meter_entry->kbits_sec, meter_entry->kbytes_burst * 8);
        if(ret_value != BCM_E_NONE)
        {
            printf("DEVRM_UpdateMeter Error! bcm_field_meter_set[%d]\n",ret_value);
            return FALSE;
        }
    }

    ret_value = bcm_field_meter_set(device_id, dev_rule_id, BCM_FIELD_METER_PEAK, meter_entry->peak_kbits_sec, meter_entry->peak_kbytes_burst * 8);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_UpdateMeter Error! bcm_field_meter_set[%d]\n",ret_value);
        return FALSE;
    }

    ret_value = bcm_field_entry_reinstall(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_UpdateMeter Error! bcm_field_entry_reinstall[%d]\n",ret_value);
        return FALSE;
    }

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
BOOL_T DEVRM_ShareMeter(UI32_T device_id, UI32_T src_rule_id, UI32_T dst_rule_id)
{
    int     ret_value = BCM_E_NONE;

    ret_value = bcm_field_meter_share(device_id, src_rule_id, dst_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_ShareMeter Error! bcm_field_meter_share[%d]\n",ret_value);
        return FALSE;
    }

    ret_value = bcm_field_entry_reinstall(device_id, dst_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_ShareMeter Error! bcm_field_entry_reinstall[%d]\n",ret_value);
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
BOOL_T DEVRM_DestroyMeter(UI32_T device_id, UI32_T dev_rule_id)
{
    int     ret_value = BCM_E_NONE;

    ret_value = bcm_field_meter_destroy(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_DestroyMeter Error! bcm_field_entry_destroy[%d]\n",ret_value);
        return FALSE;
    }

    /* must reinstall entry to make changes happen */
    ret_value = bcm_field_entry_reinstall(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_DestroyMeter Error! bcm_field_entry_reinstall[%d]\n",ret_value);
        return FALSE;
    }

    return TRUE;
}

/* counter */
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
BOOL_T DEVRM_CreateCounter(UI32_T device_id,UI32_T dev_rule_id, UI32_T counter_mode, BOOL_T need_reinstalled)
{
    int     ret_value = BCM_E_NONE;

    if (BCM_FIELD_COUNTER_MODE_RED_YELLOW < counter_mode)
        return FALSE;

    ret_value = bcm_field_counter_create(device_id, dev_rule_id);
    if((ret_value != BCM_E_NONE) && (ret_value != BCM_E_EXISTS))
        return FALSE;

    /* counter config action */
    ret_value = bcm_field_action_add(device_id, dev_rule_id, bcmFieldActionUpdateCounter,
                    counter_mode, 0);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_CreateCounter Error! bcm_field_action_add[%d]\n",ret_value);
        return FALSE;
    }

    if (TRUE == need_reinstalled)
    {
        ret_value = bcm_field_entry_reinstall(device_id, dev_rule_id);
        if(ret_value != BCM_E_NONE)
        {
            printf("DEVRM_CreateCounter Error! bcm_field_entry_reinstall[%d]\n",ret_value);
            return FALSE;
        }
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
BOOL_T DEVRM_DestroyCounter(UI32_T device_id, UI32_T dev_rule_id)
{
    int     ret_value = BCM_E_NONE;

    ret_value = bcm_field_counter_destroy(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_DestroyCounter Error! bcm_field_counter_destroy[%d]\n",ret_value);
        return FALSE;
    }

    /* must reinstall entry to make changes happen */
    ret_value = bcm_field_entry_reinstall(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_DestroyCounter Error! bcm_field_entry_reinstall[%d]\n",ret_value);
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
    int ret_value;
    int counter_num = 1;
    uint32 val;

    ret_value = bcm_field_counter_get32(device_id, dev_rule_id, counter_num, &val);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_GetCounter Error! bcm_field_counter_get32[%d]\n", ret_value);
        return FALSE;
    }

    *counter_p = val;

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
    int ret_value;
    int counter_num = 1;

    ret_value = bcm_field_counter_set(device_id, dev_rule_id, counter_num, 0);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_ClearCounter Error! bcm_field_counter_set[%d]\n", ret_value);
        return FALSE;
    }

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
BOOL_T DEVRM_SetAction(UI32_T device_id, UI32_T dev_rule_id, DEVRM_ActionEntry_T *action_entry, UI32_T action_entry_num)
{
    int                     ret_value = BCM_E_NONE;
    int                     i;
    //UI32_T                  dev_chain_id;
    DEVRM_ActionEntry_T     *action_iter_p;

    /* remove action setting */
    ret_value = bcm_field_action_remove_all(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_SetAction Error! bcm_field_action_remove_all[%d]\n", ret_value);
        return FALSE;
    }

    /* add action iteratively */
#if 1
    for (i = 0; i < action_entry_num; i++)
    {
        action_iter_p = &action_entry[i];
        /* translate login chain id to physical chain id */
        /*if ((bcmFieldActionChain == action_iter_p->action) &&
            (TRUE == DEVRM_GetDeviceRuleIDByLogicalID(device_id, action_iter_p->param0, &dev_chain_id)))
        {
            action_iter_p->param0 = dev_chain_id % SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR;
        }*/

        ret_value = bcm_field_action_add(device_id, dev_rule_id, action_iter_p->action,
                        action_iter_p->param0, action_iter_p->param1);
        if(ret_value != BCM_E_NONE)
        {
            printf("DEVRM_SetAction Error! bcm_field_action_add[%d]\n",ret_value);
            return FALSE;
        }
    }
#endif

    ret_value = bcm_field_entry_reinstall(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_SetAction Error! bcm_field_entry_reinstall[%d]\n",ret_value);
        return FALSE;
    }

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
    int     ret_value = BCM_E_NONE;

    /* remove action setting */
    ret_value = bcm_field_action_remove_all(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_DestroyAction Error! bcm_field_action_remove_all[%d]\n", ret_value);
        return FALSE;
    }

    ret_value = bcm_field_entry_reinstall(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_DestroyAction Error! bcm_field_entry_reinstall[%d]\n",ret_value);
        return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CopyRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Copy a rule entry to another.
 * INPUT:    device_id, src_dev_rule_id, dst_dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_CopyRule(UI32_T device_id, UI32_T src_dev_rule_id, UI32_T dst_dev_rule_id)
{
    int    ret_value=BCM_E_NONE;

    /* use accton's api to copy rule.
       1.does not need to destroy the dst rule.
       2.does not pack tcam as bcm's api.
       3.share the meter with src rule if src rule has a meter.
     */
    ret_value = accton_bcm_field_entry_copy_id(device_id, src_dev_rule_id, dst_dev_rule_id);

    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_CopyRule Error! bcm_field_entry_copy_id[%d]\n",ret_value);

        return FALSE;
    }
    /*should install again*/
    ret_value = bcm_field_entry_install(device_id,dst_dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_CopyRule Error! bcm_field_entry_install[%d]\n",ret_value);
        return FALSE;
    }

    /*clear source entry*/
    ret_value = bcm_field_entry_remove(device_id, src_dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_CopyRule Error! bcm_field_entry_remove[%d]\n",ret_value);
        return FALSE;
    }

    /* remove action setting */
    ret_value = bcm_field_action_remove_all(device_id, src_dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_CopyRule Error! bcm_field_action_remove_all[%d]\n", ret_value);
        return FALSE;
    }

    /* remove meter setting */
    ret_value = bcm_field_meter_destroy(device_id, src_dev_rule_id);
    if ((BCM_E_NONE != ret_value) && (BCM_E_EMPTY != ret_value))
    {
        printf("DEVRM_CopyRule Error! bcm_field_meter_destroy[%d]\n", ret_value);
        return FALSE;
    }

    /* clear qualifier */
    ret_value = bcm_field_qualify_clear(device_id,src_dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_CopyRule Error! bcm_field_qualify_clear[%d]\n",ret_value);
        return FALSE;
    }

    return TRUE;

}

static BOOL_T DEVRM_DeleteDevRule(UI32_T device_id, UI32_T dev_rule_id)
{
    //UI32_T i = 0;
    int    ret_value=BCM_E_NONE;


    if (TRUE == DEVRM_IS_BAD_DEVICE_ID(device_id))
    {
        return FALSE;
    }

    /* delete target rule */
    ret_value = bcm_field_entry_remove(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
       printf("DEVRM_DeleteDevRule Error! bcm_field_entry_remove[%d]\n",ret_value);
       return FALSE;
    }

    /* remove action setting */
    ret_value = bcm_field_action_remove_all(device_id, dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_DeleteDevRule Error! bcm_field_action_remove_all[%d]\n", ret_value);
        return FALSE;
    }

    /* remove meter setting */
    ret_value = bcm_field_meter_destroy(device_id, dev_rule_id);
    if ((BCM_E_NONE != ret_value) && (BCM_E_EMPTY != ret_value))
    {
        printf("DEVRM_DeleteDevRule Error! bcm_field_meter_destroy[%d]\n", ret_value);
        return FALSE;
    }

    /* clear qualifier */
    ret_value = bcm_field_qualify_clear(device_id,dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_DeleteDevRule Error! bcm_field_qualify_clear[%d]\n",ret_value);
        return FALSE;
    }

    /* destroy entry */
    ret_value = bcm_field_entry_destroy(device_id,dev_rule_id);
    if(ret_value != BCM_E_NONE)
    {
        printf("DEVRM_DeleteDevRule Error! bcm_field_entry_destroy[%d]\n",ret_value);
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
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.setgroup.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_CreateGroup(
                                                    dev_rm_ipcmsg->data.setgroup.req.device_id,
                                                    &dev_rm_ipcmsg->data.setgroup.req.group_entry);
            memcpy(&dev_rm_ipcmsg->data.setgroup.resp.group_entry,
                   &dev_rm_ipcmsg->data.setgroup.req.group_entry,
                   sizeof(dev_rm_ipcmsg->data.setgroup.resp));
            break;

        case DEVRM_IPCCMD_SET_GROUP_QUALIFY_SET:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                                      sizeof(dev_rm_ipcmsg->data.setgroup.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_SetGroupQualifySet(
                                                    dev_rm_ipcmsg->data.setgroup.req.device_id,
                                                    &dev_rm_ipcmsg->data.setgroup.req.group_entry);
            memcpy(&dev_rm_ipcmsg->data.setgroup.resp.group_entry,
                   &dev_rm_ipcmsg->data.setgroup.req.group_entry,
                   sizeof(dev_rm_ipcmsg->data.setgroup.resp));
            break;

        case DEVRM_IPCCMD_SET_GROUP_PRIORITY:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                            sizeof(dev_rm_ipcmsg->data.set_group_priority.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_SetGroupPriority(
                           dev_rm_ipcmsg->data.set_group_priority.req.device_id,
                            dev_rm_ipcmsg->data.set_group_priority.req.group_id,
                           dev_rm_ipcmsg->data.set_group_priority.req.priority);
            break;

        case DEVRM_IPCCMD_DESTROYGROUP:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.destroygroup.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_DestroyGroup(
                                                    dev_rm_ipcmsg->data.destroygroup.req.device_id,
                                                    dev_rm_ipcmsg->data.destroygroup.req.group_id);
            break;

        case DEVRM_IPCCMD_CREATE_VIR_GROUP:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.create_vir_group.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_CreateVirtualGroup(
                                                    dev_rm_ipcmsg->data.create_vir_group.req.group_id,
                                                    dev_rm_ipcmsg->data.create_vir_group.req.vir_group_id,
                                                    dev_rm_ipcmsg->data.create_vir_group.req.priority);
            break;

        case DEVRM_IPCCMD_ALLOCATERULE:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.allocaterule.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_AllocateRule(
                                                    dev_rm_ipcmsg->data.allocaterule.req.device_id,
                                                    dev_rm_ipcmsg->data.allocaterule.req.group_id,
                                                    dev_rm_ipcmsg->data.allocaterule.req.vir_group_id,
                                                    dev_rm_ipcmsg->data.allocaterule.req.rule_pri,
                                                    &dev_rm_ipcmsg->data.allocaterule.resp.dev_rule_id);
            break;

        case DEVRM_IPCCMD_SETRULE:
        {
            DEVRM_MeterEntry_T  *dev_mtr_p =NULL;

            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.setrule.resp);

            if (dev_rm_ipcmsg->data.setrule.req.meter_entry_num != 0)
                dev_mtr_p = &dev_rm_ipcmsg->data.setrule.req.meter_entry;

            dev_rm_ipcmsg->type.result_bool = DEVRM_SetRule(
                                                    dev_rm_ipcmsg->data.setrule.req.device_id,
                                                    dev_rm_ipcmsg->data.setrule.req.group_id,
                                                    dev_rm_ipcmsg->data.setrule.req.dev_rule_id,
                                                    &dev_rm_ipcmsg->data.setrule.req.ace_entry,
                                                    dev_mtr_p,
                                                    dev_rm_ipcmsg->data.setrule.req.action_entry,
                                                    dev_rm_ipcmsg->data.setrule.req.action_entry_num,
                                                    dev_rm_ipcmsg->data.setrule.req.counter_enable);
        }
            break;

        case DEVRM_IPCCMD_UPDATERULE:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.updaterule.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_UpdateRule(
                                                    dev_rm_ipcmsg->data.updaterule.req.device_id,
                                                    dev_rm_ipcmsg->data.updaterule.req.dev_rule_id,
                                                    &dev_rm_ipcmsg->data.updaterule.req.ace_entry,
                                                    (dev_rm_ipcmsg->data.updaterule.req.meter_entry_num != 0)
                                                        ? &dev_rm_ipcmsg->data.updaterule.req.meter_entry
                                                        : NULL,
                                                    dev_rm_ipcmsg->data.updaterule.req.action_entry,
                                                    dev_rm_ipcmsg->data.updaterule.req.action_entry_num);
            break;

        case DEVRM_IPCCMD_DESTROYRULE:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.destroyrule.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_DestroyRule(
                                                    dev_rm_ipcmsg->data.destroyrule.req.device_id,
                                                    dev_rm_ipcmsg->data.destroyrule.req.vir_group_id,
                                                    dev_rm_ipcmsg->data.destroyrule.req.dev_rule_id);
            break;

        case DEVRM_IPCCMD_CREATEMETER:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.create_meter.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_CreateMeter(
                                                    dev_rm_ipcmsg->data.create_meter.req.device_id,
                                                    dev_rm_ipcmsg->data.create_meter.req.dev_rule_id,
                                                    &dev_rm_ipcmsg->data.create_meter.req.meter_entry,
                                                    dev_rm_ipcmsg->data.create_meter.req.need_reinstalled);
            break;

        case DEVRM_IPCCMD_UPDATEMETER:
            {
                DEVRM_MeterEntry_T  *devrm_me_p;

                ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.update_meter.resp);

                if (0 == dev_rm_ipcmsg->data.update_meter.req.meter_entry_num)
                {
                    devrm_me_p = NULL;
                }
                else
                {
                    devrm_me_p = &dev_rm_ipcmsg->data.update_meter.req.meter_entry;
                }

                dev_rm_ipcmsg->type.result_bool = DEVRM_UpdateMeter(
                                                    dev_rm_ipcmsg->data.update_meter.req.device_id,
                                                    dev_rm_ipcmsg->data.update_meter.req.dev_rule_id,
                                                    devrm_me_p);
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

        case DEVRM_IPCCMD_DESTROYMETER:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.destroymeter.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_DestroyMeter(
                                                    dev_rm_ipcmsg->data.destroymeter.req.device_id,
                                                    dev_rm_ipcmsg->data.destroymeter.req.dev_rule_id);
            break;

        case DEVRM_IPCCMD_SETACTION:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.setaction.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_SetAction(
                                                    dev_rm_ipcmsg->data.setaction.req.device_id,
                                                    dev_rm_ipcmsg->data.setaction.req.dev_rule_id,
                                                    dev_rm_ipcmsg->data.setaction.req.action_entry,
                                                    dev_rm_ipcmsg->data.setaction.req.action_entry_num);
            break;

        case DEVRM_IPCCMD_DESTROYACTION:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.destroy_action.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_DestroyAction(
                                                    dev_rm_ipcmsg->data.destroy_action.req.device_id,
                                                    dev_rm_ipcmsg->data.destroy_action.req.dev_rule_id);
            break;

        case DEVRM_IPCCMD_GET_COUNTER:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.get_counter.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_GetCounter(
                                                dev_rm_ipcmsg->data.get_counter.req.device_id,
                                                dev_rm_ipcmsg->data.get_counter.req.dev_rule_id,
                                                &dev_rm_ipcmsg->data.get_counter.resp.counter);
            break;

        case DEVRM_IPCCMD_CLEAR_COUNTER:
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE;
            dev_rm_ipcmsg->type.result_bool = DEVRM_ClearCounter(
                                                dev_rm_ipcmsg->data.clear_counter.req.device_id,
                                                dev_rm_ipcmsg->data.clear_counter.req.dev_rule_id);
            break;

        case DEVRM_IPCCMD_GETNUMBEROFCHIPS:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_rm_ipcmsg->data.getnumberofchips.resp);
            dev_rm_ipcmsg->type.result_ui32 = DEVRM_GetNumberOfChips();
            break;

        case DEVRM_IPCCMD_GET_DEVICE_INFOMATION:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE+
                               sizeof(dev_rm_ipcmsg->data.get_device_info.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_GetDeviceInfo(
                                                    &dev_rm_ipcmsg->data.get_device_info.resp.device_info);
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
static UI32_T DEVRM_GetNumberOfChips()
{
    return soc_ndev;
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
    int device_id;

    memset(dev_info_p, 0, sizeof(DEVRM_DevInfo_T));

    dev_info_p->num_of_chips = soc_ndev;

    /* TODO: save the port bit map by per device
     */
    for (device_id = 0; device_id < soc_ndev; device_id++)
    {
        BCM_PBMP_OR(dev_info_p->port_bit_map.all_ports, PBMP_PORT_ALL(device_id));

        BCM_PBMP_PORT_ADD(dev_info_p->port_bit_map.cpu_port, CMIC_PORT(device_id));
    }

    BCM_PBMP_OR(dev_info_p->port_bit_map.all_ports, dev_info_p->port_bit_map.cpu_port);

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
static BOOL_T DEVRM_CheckSoc(UI32_T device_id)
{
#if 0/* No need to check soc. anzhen.zheng, 2008-9-3 */
    if(SOC_IS_FIREBOLT2(device_id))
        return TRUE;
    else
        return FALSE;
#else
    return TRUE;
#endif
}

/*******************************************************************************
   GROUP
 *******************************************************************************
 */
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
static BOOL_T _DEVRM_IsGroupExisted(
    UI32_T device_id,
    UI32_T group_id)
{
    int ret_value;
    bcm_field_qset_t qset;

    ret_value = bcm_field_group_get(device_id, group_id, &qset);

    return (ret_value == BCM_E_NONE) ? TRUE : FALSE;
}

/*******************************************************************************
   VIRTUAL GROUP
 *******************************************************************************
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_InitVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Init virtual group struct
 * INPUT    : NONE
 * OUTPUT   : NONE
 * RETURN   : NONE
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static void _DEVRM_InitVirtualGroup()
{
    memset(&devrm_vir_grp, 0, sizeof(devrm_vir_grp));
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
static DEVRM_VirtualGroup_T * _DEVRM_GetVirtualGroupAtIndex(
    UI32_T device_id,
    UI32_T idx)
{
    if (_countof(devrm_vir_grp) <= device_id)
    {
        return NULL;
    }

    if (_countof(devrm_vir_grp[device_id].vg_tbl) <= idx)
    {
        return NULL;
    }

    return &devrm_vir_grp[device_id].vg_tbl[idx];
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - _DEVRM_FindVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE  : Find virtual group by ID
 * INPUT    : device_id     - device ID
 *            vir_group_id  - virtual group ID
 * OUTPUT   : NONE
 * RETURN   : Pointer of a virtual group object
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static DEVRM_VirtualGroup_T * _DEVRM_FindVirtualGroup(
    UI32_T device_id,
    UI32_T vir_group_id)
{
    DEVRM_VirtualGroup_T *vg_p;
    UI32_T i;

    if (_countof(devrm_vir_grp) <= device_id)
    {
        return NULL;
    }

    for (i=0; i < devrm_vir_grp[device_id].vsel_num; ++i)
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
static BOOL_T _DEVRM_AddRuleToVirtualGroupAndSetPriority(
    UI32_T device_id,
    UI32_T vir_group_id,
    int rule_pri,
    UI32_T dev_rule_id)
{
    DEVRM_VirtualGroup_T *vg_p = _DEVRM_FindVirtualGroup(device_id, vir_group_id);
    UI32_T rule_idx;
    UI32_T i;

    if (NULL == vg_p)
    {
        DEVRM_LOG("_DEVRM_FindVirtualGroup(soc=%lu, vir_group_id=%lu) failed",
            device_id, vir_group_id);
        return FALSE;
    }

    if (vg_p->rv.cur_rule_nbr == _countof(vg_p->rv.rule_tbl))
    {
        DEVRM_LOG("No more rule");
        return FALSE;
    }

    if (FALSE == _DEVRM_CalculateIndexForNewRuleByPriority(&vg_p->rv, rule_pri, &rule_idx))
    {
        _DEVRM_CompactRule(&vg_p->rv);

        if (FALSE == _DEVRM_CalculateIndexForNewRuleByPriority(&vg_p->rv, rule_pri, &rule_idx))
        {
            _DEVRM_DumbRuleVector(&vg_p->rv);
            return FALSE;
        }
    }

    if (FALSE == _DEVRM_InsertRuleAtIndex(&vg_p->rv, rule_idx, rule_pri, dev_rule_id))
    {
        return FALSE;
    }

    for (i=0; i<_countof(vg_p->rv.rule_tbl); ++i)
    {
        int ret_value;
        int pri;
        DEVRM_Rule_T *rule_p = &vg_p->rv.rule_tbl[i];

        if (TRUE == rule_p->valid || TRUE == rule_p->dirty_bit)
        {

            /*
             * Convert index to priority
             */
            pri = vg_p->pri_base - i;

            ret_value = bcm_field_entry_prio_set(device_id, rule_p->rule_id, pri);
            if (BCM_E_NONE != ret_value)
            {
                DEVRM_ERR("bcm_field_entry_prio_set(device_id=%lu, rule_id=%lu, pri=%d) %s",
                    device_id, rule_p->rule_id, pri, bcm_errmsg(ret_value));
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
static BOOL_T _DEVRM_RemoveRuleFromVirtualGroup(
    UI32_T device_id,
    UI32_T vir_group_id,
    UI32_T dev_rule_id)
{
    DEVRM_VirtualGroup_T *vg_p = _DEVRM_FindVirtualGroup(device_id, vir_group_id);
    UI32_T rule_idx;

    if (NULL == vg_p)
    {
        DEVRM_LOG("DEVRM_GetVSelectorBySubSelector(soc=%lu, vir_group_id=%lu) failed",
            device_id, vir_group_id);
        return FALSE;
    }

    if (FALSE == _DEVRM_FindRuleIndexByRuleId(&vg_p->rv, dev_rule_id, &rule_idx))
    {
        DEVRM_LOG("No found old rule, device_id=%lu, vir_group_id=%lu, dev_rule_id=%lu",
            device_id, vir_group_id, dev_rule_id);
        return FALSE;
    }

    return _DEVRM_RemoveRuleAtIndex(&vg_p->rv, rule_idx);

}

static int
_DEVRM_SortVirtualGroup_Compare(
    const void *arg_1,
    const void *arg_2)
{
    const DEVRM_VirtualGroup_T *vg_1 = (const DEVRM_VirtualGroup_T *) arg_1;
    const DEVRM_VirtualGroup_T *vg_2 = (const DEVRM_VirtualGroup_T *) arg_2;

    /* 1. valid    (high -> low)
     * 2. group_id (low  -> high)
     * 3. pri_lvl  (high -> low)
     * 4. seq_no   (low  -> high)
     */
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
 * PURPOSE  : Sort virtual group by priority
 * INPUT    : device_id     - device ID
 * OUTPUT   : NONE
 * RETURN   : TRUE/FALSE
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
static BOOL_T _DEVRM_SortVirtualGroup(
    UI32_T device_id)
{
    DEVRM_VirtualGroup_T     *vg_p;
    DEVRM_VirtualGroup_T     *vg_2_p;
    UI32_T i;

    /* Sort virtual group by priority level(high --> low) per group
     */
    qsort(devrm_vir_grp[device_id].vg_tbl, devrm_vir_grp[device_id].vsel_num,
          sizeof(devrm_vir_grp[device_id].vg_tbl[0]), _DEVRM_SortVirtualGroup_Compare);

    /* Assign priority base
     */
    {
        int base = 0;
        for (i=0; i<devrm_vir_grp[device_id].vsel_num; ++i)
        {
            vg_p = _DEVRM_GetVirtualGroupAtIndex(device_id, i);

            vg_p->pri_base = base;

            if (i+1 < devrm_vir_grp[device_id].vsel_num)
            {
                vg_2_p = _DEVRM_GetVirtualGroupAtIndex(device_id, i+1);
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

static BOOL_T _DEVRM_CompactRule(
    DEVRM_RuleVector_T *rv_p)
{
    UI32_T i;
    UI32_T emp_idx; /* index of empty entry */
    UI32_T use_idx; /* index of used entry */

    if (NULL == rv_p)
    {
        DEVRM_LOG("NULL pointer");
        return FALSE;
    }

    emp_idx = 0;

    while (TRUE)
    {
        /* Find index for an empty entry
         */
        for (i = emp_idx; i<_countof(rv_p->rule_tbl); ++i)
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

        emp_idx = i;

        /* Find index for a used entry after entry entry
         */
        for (i = emp_idx+1; i<_countof(rv_p->rule_tbl); ++i)
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

        use_idx = i;

        rv_p->rule_tbl[emp_idx] = rv_p->rule_tbl[use_idx];
        rv_p->rule_tbl[emp_idx].dirty_bit = TRUE;

        memset(&rv_p->rule_tbl[use_idx], 0, sizeof(rv_p->rule_tbl[use_idx]));
        rv_p->rule_tbl[use_idx].valid = FALSE;

        ++ emp_idx;
    }

    return TRUE;
}

static BOOL_T _DEVRM_InsertRuleAtIndex(
    DEVRM_RuleVector_T *rv_p,
    UI32_T rule_idx,
    int rule_pri,
    UI32_T dev_rule_id)
{
    DEVRM_Rule_T *rule_p;

    if (NULL == rv_p)
    {
        DEVRM_LOG("NULL pointer");
        return FALSE;
    }

    if (_countof(rv_p->rule_tbl) <= rule_idx)
    {
        DEVRM_LOG("Invalid rule_idx=%lu", rule_idx);
        return FALSE;
    }

    if (_countof(rv_p->rule_tbl) == rv_p->cur_rule_nbr)
    {
        DEVRM_LOG("No more rule");
        return FALSE;
    }

    if (TRUE == rv_p->rule_tbl[rule_idx].valid)
    {
        UI32_T i;
        UI32_T end_idx = 0;

        for (i=rule_idx+1; i < _countof(rv_p->rule_tbl); ++i)
        {
            if (FALSE == rv_p->rule_tbl[i].valid)
            {
                end_idx = i;
                break;
            }
        }

        if (i == _countof(rv_p->rule_tbl))
        {
            DEVRM_LOG("No free space after index %lu. Must compact rule table first.",
                rule_idx);
            return FALSE;
        }

        for (i=end_idx; rule_idx<i; --i)
        {
            rv_p->rule_tbl[i] = rv_p->rule_tbl[i-1];
            rv_p->rule_tbl[i].dirty_bit = TRUE;
        }
    }

    rule_p = &rv_p->rule_tbl[rule_idx];

    rule_p->rule_id     = dev_rule_id;
    rule_p->rule_pri    = rule_pri;
    rule_p->dirty_bit   = TRUE;
    rule_p->valid       = TRUE;

    rv_p->cur_rule_nbr += 1;
    return TRUE;
}

static BOOL_T _DEVRM_RemoveRuleAtIndex(
    DEVRM_RuleVector_T *rv_p,
    UI32_T rule_idx)
{
    DEVRM_Rule_T * rule_p;

    if (NULL == rv_p)
    {
        DEVRM_LOG("NULL pointer");
        return FALSE;
    }

    if (_countof(rv_p->rule_tbl) <= rule_idx)
    {
        DEVRM_LOG("Invalid rule_idx=%lu", rule_idx);
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

static BOOL_T _DEVRM_FindRuleIndexByRuleId(
    DEVRM_RuleVector_T *rv_p,
    UI32_T dev_rule_id,
    UI32_T *rule_idx)
{
    UI32_T i;

    if (NULL == rv_p || NULL == rule_idx)
    {
        DEVRM_LOG("NULL pointer");
        return FALSE;
    }

    for (i=0; i <_countof(rv_p->rule_tbl); ++i)
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

static BOOL_T _DEVRM_CalculateIndexForNewRuleByPriority(
    const DEVRM_RuleVector_T *rv_p,
    int rule_pri,
    UI32_T *rule_idx_p
    )
{
#define INVALID_INDEX (_countof(rv_p->rule_tbl))

    UI32_T i;
    UI32_T candidate = INVALID_INDEX;

    for (i=0; i<_countof(rv_p->rule_tbl); ++i)
    {
        const DEVRM_Rule_T *rule_p = &rv_p->rule_tbl[i];

        if (TRUE == rule_p->valid && rule_pri <= rule_p->rule_pri)
        {
            candidate = INVALID_INDEX;
            continue;
        }

        if (FALSE == rule_p->valid)
        {
            if (INVALID_INDEX == candidate)
            {
                candidate = i;
            }
        }
        else if (/* TRUE == rule_p->valid && */ rule_p->rule_pri < rule_pri)
        {
            if (INVALID_INDEX == candidate)
            {
                candidate = i;
            }

            break;
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

static void _DEVRM_DumbRuleVector(
    DEVRM_RuleVector_T *rv_p)
{
    UI32_T i;

    DEVRM_PRINT("Current NBR of rule : %lu\r\n", rv_p->cur_rule_nbr);
    DEVRM_PRINT("Rule Table : \r\n");
    for (i=0; i<_countof(rv_p->rule_tbl); ++i)
    {
        DEVRM_Rule_T *rule_p = &rv_p->rule_tbl[i];

        DEVRM_PRINT("[%03lu] val=%u, dirty=%s, pri=%-3d, id=%-3lu\r\n",
            i,
            rule_p->valid,
            rule_p->dirty_bit?"ON ":"OFF",
            rule_p->rule_pri,
            rule_p->rule_id);
    }
}
