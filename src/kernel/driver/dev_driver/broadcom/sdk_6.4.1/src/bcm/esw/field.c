/* $Id: field.c,v 1.670.2.65.2.3 Broadcom SDK $
 * $Copyright: Copyright 2008 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * Module: Field Processor APIs
 *
 * Purpose:
 *     API for Field Processor (FP) for XGS3 family and later.
 *
 *
 */

#include <stdlib.h>
#include <bcm/error.h>
#include <bcm/field.h>
//#include <bcm_int/esw/field.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define BCM_ESW_FIELD_MAX_DEVICE_NUM    2
#define BCM_ESW_FIELD_MAX_GROUP_NUM     (16 + 4 + 4)
#define BCM_ESW_FIELD_MAX_RULE_NUM      128
#define BCM_ESW_FIELD_MAX_POLICER_NUM   (BCM_ESW_FIELD_MAX_GROUP_NUM * BCM_ESW_FIELD_MAX_RULE_NUM)
#define BCM_ESW_FIELD_MAX_COUNTER_NUM   (BCM_ESW_FIELD_MAX_GROUP_NUM * BCM_ESW_FIELD_MAX_RULE_NUM)

#ifndef _countof
#define _countof(ary)   (sizeof(ary)/sizeof(*ary))
#endif

typedef struct
{
    int                         pri;
    uint32                      dev_rule_id;
    uint32                      policer_id; // 0xffffffff means not-installed
    uint32                      counter_id; // 0xffffffff means not-installed
    uint8                       installed;
    uint8                       valid;
} _field_entry_t;

typedef unsigned int bcm_field_group_sbmp_t;
typedef unsigned int bcm_field_udf_t;

typedef struct
{
    bcm_field_qset_t            qset;
    int                         pri;
    bcm_field_group_mode_t      mode;
    bcm_field_group_sbmp_t      sset;


    _field_entry_t              entry_tbl[ BCM_ESW_FIELD_MAX_RULE_NUM ];
    int                         max_rule_pri;
    int                         min_rule_pri;

    uint8                       valid;
} _field_group_t;

typedef struct
{
    uint32                      ref_cnt;
    uint8                       valid;
} _field_policer_t;

typedef struct
{
    uint64                      counter[2];
    uint32                      ref_cnt;
    uint8                       valid;
} _ut_field_counter_t;

typedef struct
{
    _field_group_t              group[ BCM_ESW_FIELD_MAX_GROUP_NUM ];
    _field_policer_t            policer[ BCM_ESW_FIELD_MAX_POLICER_NUM ];
    _ut_field_counter_t         counter[ BCM_ESW_FIELD_MAX_COUNTER_NUM ];
} _field_device_t;

typedef struct
{

    _field_device_t             device[ BCM_ESW_FIELD_MAX_DEVICE_NUM ];

} _field_db_t;

static _field_db_t              g_field_db;


int bcm_esw_field_entry_remove(int unit, bcm_field_entry_t entry);

#pragma mark -
#pragma mark internal functions

_field_group_t *
_bcm_esw_field_group_get(int unit,
                         bcm_field_group_t group)
{
    if (_countof(g_field_db.device) <= unit)
    {
        return NULL;
    }

    if (_countof(g_field_db.device[unit].group) <= group)
    {
        return NULL;
    }

    return &g_field_db.device[unit].group[group];
}

int
_field_entry_sort_compare(const void *arg1,
                          const void *arg2)
{
    const _field_entry_t *r1 = (const _field_entry_t *)arg1;
    const _field_entry_t *r2 = (const _field_entry_t *)arg2;

    if (r1->valid < r2->valid)
    {
        return 1;
    }
    else if (r2->valid < r1->valid)
    {
        return -1;
    }

    if (r1->pri < r2->pri)
    {
        return 1;
    }
    else if (r2->pri < r1->pri)
    {
        return -1;
    }

    return 0;
}

void
_field_entry_sort(int unit,
                  bcm_field_group_t group)
{
    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);

    assert(NULL != grp);

    qsort(grp->entry_tbl, _countof(grp->entry_tbl), sizeof(grp->entry_tbl[0]),
          _field_entry_sort_compare);
}

void
_field_group_validate(int unit,
                      bcm_field_group_t group)
{
    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);
    uint32 entry_idx;
    _field_entry_t *entry_1 = NULL;
    _field_entry_t *entry_2 = NULL;

    assert(NULL != grp);
    assert(0 != grp->valid);

    for (entry_idx = 0; entry_idx < _countof(grp->entry_tbl); ++entry_idx)
    {
        if (0 == grp->entry_tbl[entry_idx].valid)
        {
            continue;
        }

        if (NULL == entry_1)
        {
            entry_1 = &grp->entry_tbl[entry_idx];
        }
        else if (NULL == entry_2)
        {
            entry_2 = &grp->entry_tbl[entry_idx];
        }
        else
        {
            assert(0);
        }

        if (NULL != entry_1 && NULL != entry_2)
        {
            assert(entry_2->pri <= entry_1->pri);

            entry_1 = entry_2;
            entry_2 = NULL;
        }
    }
}

_field_entry_t *
_field_entry_get(int unit,
                 bcm_field_entry_t entry)
{
    assert(unit < _countof(g_field_db.device));
    assert(unit == ((entry >> 24) & 0xff) );

    bcm_field_group_t group = (entry >> 16) & 0xff;
    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);
    uint32 entry_idx;

    assert(NULL != grp);

    for (entry_idx = 0; entry_idx < _countof(grp->entry_tbl); ++entry_idx)
    {
        _field_entry_t *rule = &grp->entry_tbl[entry_idx];

        if (0 != rule->valid && entry == rule->dev_rule_id)
        {
            return rule;
        }
    }

    return NULL;
}

_field_policer_t *
_field_policer_get(int unit,
                   bcm_policer_t policer_id)
{
    assert(unit < _countof(g_field_db.device));
    assert(policer_id < _countof(g_field_db.device[unit].policer));

    return &g_field_db.device[unit].policer[policer_id];
}

void
_field_policer_attach(int unit,
                      bcm_field_entry_t entry,
                      bcm_policer_t policer_id)
{
    _field_entry_t *ent = _field_entry_get(unit, entry);
    _field_policer_t *pol;

    assert(NULL != ent);
    assert(0xffffffff == ent->policer_id);
    assert(0xffffffff != policer_id);

    pol = _field_policer_get(unit, policer_id);

    assert(NULL != pol);
    assert(0 != pol->valid);

    ++pol->ref_cnt;
    ent->policer_id = policer_id;
}

void
_field_policer_detach(int unit,
                      bcm_field_entry_t entry)
{
    _field_entry_t *ent = _field_entry_get(unit, entry);
    _field_policer_t *pol;

//    if (NULL == ent)
//    {
//        return;
//    }

    assert(NULL != ent);

    if (0xffffffff == ent->policer_id)
    {
        return;
    }

    pol = _field_policer_get(unit, ent->policer_id);

    assert(NULL != pol);
    assert(0 != pol->valid);
    assert(0 < pol->ref_cnt);

    if (0 == -- pol->ref_cnt)
    {
        pol->valid = 0;
    }

    ent->policer_id = 0xffffffff;
}

_ut_field_counter_t *
_field_counter_get(int unit,
                   uint32 counter_id)
{
    assert(unit < _countof(g_field_db.device));
    assert(counter_id < _countof(g_field_db.device[unit].counter));

    return &g_field_db.device[unit].counter[counter_id];
}

void
_field_counter_attach(int unit,
                      bcm_field_entry_t entry,
                      uint32 counter_id)
{
    _field_entry_t *ent = _field_entry_get(unit, entry);
    _ut_field_counter_t *cnt;

    assert(NULL != ent);
    assert(0xffffffff == ent->counter_id);
    assert(0xffffffff != counter_id);

    cnt = _field_counter_get(unit, counter_id);

    assert(NULL != cnt);
    assert(0 != cnt->valid);

    ++cnt->ref_cnt;
    ent->counter_id = counter_id;
}

void
_field_counter_detach(int unit,
                      bcm_field_entry_t entry)
{
    _field_entry_t *ent = _field_entry_get(unit, entry);
    _ut_field_counter_t *cnt;

    assert(NULL != ent);

    if (0xffffffff == ent->counter_id)
    {
        return;
    }

    cnt = _field_counter_get(unit, ent->counter_id);

    assert(NULL != cnt);
    assert(0 != cnt->valid);
    assert(0 < cnt->ref_cnt);

    if (0 == -- cnt->ref_cnt)
    {
        cnt->valid = 0;
    }
}

void
_field_qset_dump(char *prefix, bcm_field_qset_t *qset, char *suffix)
{
    return;
}

#pragma mark -
#pragma mark system functions

/*
 * Function: bcm_esw_field_qualify_XXX (class of routines)
 *
 * Purpose:
 *     Add a field qualification to a filter entry.
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Field entry to operate on
 *     data - Data to qualify with (type varies)
 *     mask - Mask to qualify with (type is same as for data)
 *
 * Returns:
 *     BCM_E_INIT       BCM Unit not initialized.
 *     BCM_E_NOT_FOUND  Entry ID not found in unit.
 *     BCM_E_XXX        Other errors
 */

int
bcm_esw_field_init(int unit)
{
    uint32 dev_num;
    uint32 grp_idx;
    uint32 entry_idx;

    memset(&g_field_db, 0, sizeof(g_field_db));

    for (dev_num = 0; dev_num < _countof(g_field_db.device); ++dev_num)
    {
        for (grp_idx=0; grp_idx < _countof(g_field_db.device[dev_num].group); ++grp_idx)
        {
            _field_group_t *grp = &g_field_db.device[dev_num].group[grp_idx];

            for (entry_idx = 0; entry_idx < _countof(grp->entry_tbl); ++entry_idx)
            {
                _field_entry_t *rule = &grp->entry_tbl[entry_idx];

                rule->dev_rule_id = ((uint8)dev_num << 24) | ((uint8)grp_idx << 16) | (uint16)entry_idx;
            }
        }
    }

    return (BCM_E_NONE);
}

int
bcm_esw_field_detach(int unit)
{
    return (BCM_E_NONE);
}

/* Section: Field Qualifiers  */

/*
 * Function: bcm_esw_field_qualify_clear
 *
 * Purpose:
 *     Remove all field qualifications from a filter entry
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Field entry to operate on
 *
 * Returns:
 *     BCM_E_INIT       BCM Unit not initialized.
 *     BCM_E_NOT_FOUND  Entry ID not found in unit.
 *     BCM_E_NONE       Success
 */
int
bcm_esw_field_qualify_clear(int unit,
                            bcm_field_entry_t entry)
{
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_qualify32
 *
 * Purpose:
 *      Utility routine for qualify APIs taking 32 bits or less
 *
 * Parameters:
 *      unit - BCM device number
 *      entry - Entry ID
 *      qual  - Qualifier field
 *      data  - Matching data
 *      mask  - Bit mask for data
 *
 * Returns:
 *     BCM_E_NONE     - BCM device number
 *     BCM_E_PARAM    - Qualifier not in group's Qset
 */

int
_field_qualify32(int unit, bcm_field_entry_t entry,
                 bcm_field_qualify_t qual, uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

/*
 * Function: bcm_esw_field_meter_control_set
 *
 * Description:
 *      Set metering control for interval bytes between packets.
 *
 * Parameters:
 *      unit - BCM device number
 *      value - Bytes number
 *
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_field_meter_control_set(int unit, uint32 value)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_status_get(int unit, bcm_field_status_t *status)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_control_get(int unit, bcm_field_control_t control, uint32 *state)
{
    return (BCM_E_NONE);
}


int
bcm_esw_field_control_set(int unit, bcm_field_control_t control, uint32 state)
{
    return (BCM_E_NONE);
}

#pragma mark -
#pragma mark group functions
/*
 * Function: bcm_esw_field_group_create_mode_slice_id
 *
 * Purpose:
 *     Create a group with both a mode (single, double, etc.) and a Group ID.
 *
 * Parameters:
 *     unit - BCM device number.
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range (see bcm_field_status_get),
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    mode  - Group mode (single, double, triple or Auto-wide)
 *    group - Requested field group ID
 *    sset   - group slice index bitmap.
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - no select codes will satisfy qualifier set
 *     BCM_E_NONE      - Success
 */

int
bcm_esw_field_group_create_mode_slice_id(int unit,
                                         bcm_field_qset_t qset,
                                         int pri,
                                         bcm_field_group_mode_t mode,
                                         bcm_field_group_t group,
                                         bcm_field_group_sbmp_t sset)
{
    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);

    assert (NULL != grp);

    assert (0 == grp->valid);

    grp->valid = 1;

    grp->qset = qset;
    grp->pri  = pri;
    grp->mode = mode;
    grp->sset = sset;

    grp->max_rule_pri = 0;
    grp->min_rule_pri = 0;

    return (BCM_E_NONE);
}

/*
 * Function: bcm_esw_field_groups_ports_create_mode_slice_id
 *
 * Purpose:
 *     Create a group with a mode (single, double, etc.), a port bitmap, and
 *     a Group ID. In practice, the other group creation APIs call this one.
 *     This confirms that the required slice resources are available and
 *     calculates the sets of select codes needed to satisfy the Qset. It also
 *     creates the necessary internal data structures to manage the group.
 *
 * Parameters:
 *     unit - BCM device number.
 *     pbmp - Ports where group is defined
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range (see bcm_field_status_get),
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    mode  - Group mode (single, double, triple or Auto-wide)
 *    group - Requested Group ID
 *    sset   - group slice index bitmap
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - no select codes will satisfy qualifier set
 *     BCM_E_NONE      - Success
 */

int
bcm_esw_field_group_ports_create_mode_slice_id(int unit, bcm_pbmp_t pbmp,
                                               bcm_field_qset_t qset, int pri,
                                               bcm_field_group_mode_t mode,
                                               bcm_field_group_t group,
                                               bcm_field_group_sbmp_t sset)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_create(int unit,
                           bcm_field_qset_t qset,
                           int pri,
                           bcm_field_group_t *group)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_create_id(int unit,
                              bcm_field_qset_t qset,
                              int pri,
                              bcm_field_group_t group)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_create_mode(int unit,
                                   bcm_field_qset_t qset,
                                   int pri,
                                   bcm_field_group_mode_t mode,
                                   bcm_field_group_t *group)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_create_mode_id(int unit,
                                   bcm_field_qset_t qset,
                                   int pri,
                                   bcm_field_group_mode_t mode,
                                   bcm_field_group_t group)
{
    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);


    assert (NULL != grp);

    assert (0 == grp->valid);

    grp->valid = 1;
    grp->qset = qset;
    grp->pri  = pri;
    grp->mode = mode;

    grp->max_rule_pri = 0;
    grp->min_rule_pri = 0;

    return (BCM_E_NONE);
}

int
bcm_esw_field_group_port_create_mode(int unit, bcm_port_t port,
                                         bcm_field_qset_t qset, int pri,
                                     bcm_field_group_mode_t mode,
                                     bcm_field_group_t *group)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_port_create_mode_id(int unit, bcm_port_t port,
                                        bcm_field_qset_t qset, int pri,
                                        bcm_field_group_mode_t mode,
                                        bcm_field_group_t group)
{
    return (BCM_E_NONE);
}


int
bcm_esw_field_group_ports_create_mode(int unit, bcm_pbmp_t pbmp,
                                      bcm_field_qset_t qset, int pri,
                                      bcm_field_group_mode_t mode,
                                      bcm_field_group_t *group)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_ports_create_mode_id(int unit, bcm_pbmp_t pbmp,
                                         bcm_field_qset_t qset, int pri,
                                         bcm_field_group_mode_t mode,
                                         bcm_field_group_t group)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_mode_get(int unit,
                             bcm_field_group_t group,
                             bcm_field_group_mode_t *mode)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_set(int unit,
                        bcm_field_group_t group,
                        bcm_field_qset_t qset)
{
    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);
    assert(NULL != grp && 0 != grp->valid);

    memcpy(&grp->qset, &qset, sizeof(grp->qset));

    return (BCM_E_NONE);
}

/*
 * Function: bcm_esw_field_group_get
 *
 * Purpose:
 *     Get the qualifier set for a previously created field group
 *
 * Parameters:
 *     unit  - BCM device number
 *     port  - Port number
 *     group - Field group ID
 *     qset  - (OUT) Field qualifier set
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized.
 *     BCM_E_NOT_FOUND - Group ID not found in this unit
 *     BCM_E_PARAM     - qset is NULL
 *     BCM_E_NONE      - Success
 */

int
bcm_esw_field_group_get(int unit,
                        bcm_field_group_t group,
                        bcm_field_qset_t *qset)
{
    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);

    if (NULL == grp || 0 == grp->valid)
    {
        return BCM_E_EXISTS;
    }

    memcpy(qset, &grp->qset, sizeof(*qset));

    return BCM_E_NONE;
}

/*
 * Function: bcm_esw_field_group_destroy
 *
 * Purpose:
 *     Delete a field group
 *
 * Parameters:
 *     unit - BCM device number
 *     port - Port number
 *     group - Field group
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Group ID not found in unit
 *     BCM_E_BUSY      - Entries not destroyed yet
 *     BCM_E_NONE      - Success
 *
 * Notes:
 *      All entries that uses this group should have been destroyed
 *      before calling this routine.
 *      Operation will fail if entries exist that uses this template
 */

int
bcm_esw_field_group_destroy(int unit,
                            bcm_field_group_t group)
{
    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);

    if (NULL == grp)
    {
        return BCM_E_PARAM;
    }

    if (0 == grp->valid)
    {
        return BCM_E_EXISTS;
    }

    grp->valid = 0;

    // check rule leak
    {
        uint32 entry_idx;

        for (entry_idx = 0; entry_idx < _countof(grp->entry_tbl); ++entry_idx)
        {
            _field_entry_t *rule = &grp->entry_tbl[entry_idx];

            assert(0 == rule->valid);
        }
    }

    return (BCM_E_NONE);
}

int
bcm_esw_field_group_status_get(int unit,
                               bcm_field_group_t group,
                               bcm_field_group_status_t *status)
{
    return (BCM_E_NONE);
}

    int
bcm_esw_field_group_enable_set(int unit, bcm_field_group_t group, int enable)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_enable_get(int unit, bcm_field_group_t group, int *enable)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_compress(int unit, bcm_field_group_t group)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_priority_set(int unit, bcm_field_group_t group,
                                 int priority)
{
    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);

    assert(NULL != grp && 0 != grp->valid);

    grp->pri = priority;

    return (BCM_E_NONE);
}

int
bcm_esw_field_group_priority_get(int unit, bcm_field_group_t group,
                                 int *priority)
{
    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);

    assert(NULL != priority);
    assert(NULL != grp && 0 != grp->valid);

    *priority = grp->pri;

    return (BCM_E_NONE);
}

int
bcm_esw_field_group_flush(int unit, bcm_field_group_t group)
{
    return BCM_E_NONE;
}

int
bcm_esw_field_group_satisfies_qset(int unit, bcm_field_group_t group,
                                   bcm_field_qset_t qset)
{
    return BCM_E_NONE;
}

int
bcm_esw_field_group_range_set(int unit,
                              bcm_field_group_t group,
                              uint32 flags,
                              bcm_l4_port_t min,
                              bcm_l4_port_t max)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_range_get(int unit,
                              bcm_field_group_t group,
                              uint32 *flags,
                              bcm_l4_port_t *min,
                              bcm_l4_port_t *max)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_group_dump(int unit,
                         bcm_field_group_t group)
{
    return (BCM_E_NONE);
}

#pragma mark -
#pragma mark range functions
int
bcm_esw_field_range_create_id(int unit,
                              bcm_field_range_t range,
                              uint32 flags,
                              bcm_l4_port_t min,
                              bcm_l4_port_t max)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_range_create(int unit,
                           bcm_field_range_t *range,
                           uint32 flags,
                           bcm_l4_port_t min,
                           bcm_l4_port_t max)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_range_group_create_id(int unit,
                                    bcm_field_range_t range,
                                    uint32 flags,
                                    bcm_l4_port_t min,
                                    bcm_l4_port_t max,
                                    bcm_if_group_t group)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_range_group_create(int unit,
                                 bcm_field_range_t *range,
                                 uint32 flags,
                                 bcm_l4_port_t min,
                                 bcm_l4_port_t max,
                                 bcm_if_group_t group)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_range_get(int unit,
                        bcm_field_range_t range,
                        uint32 *flags,
                        bcm_l4_port_t *min,
                        bcm_l4_port_t *max)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_range_destroy(int unit,
                            bcm_field_range_t range)
{
    return (BCM_E_NONE);
}

#pragma mark - ====== field udf ======
int
bcm_esw_field_udf_spec_set(int unit,
                           bcm_field_udf_spec_t *udf_spec,
                           uint32 flags,
                           uint32 offset)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_udf_spec_get(int unit,
                           bcm_field_udf_spec_t *udf_spec,
                           uint32 *flags,
                           uint32 *offset)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_udf_create(int unit,
                         bcm_field_udf_spec_t *udf_spec,
                         bcm_field_udf_t *udf_id)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_udf_create_id(int unit,
                            bcm_field_udf_spec_t *udf_spec,
                            bcm_field_udf_t udf_id)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_udf_ethertype_set(int unit, int index,
                                bcm_port_frametype_t frametype,
                                bcm_port_ethertype_t ethertype)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_udf_ethertype_get(int unit, int index,
                                bcm_port_frametype_t *frametype,
                                bcm_port_ethertype_t *ethertype)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_udf_ipprotocol_set(int unit, int index, uint32 flags, uint8 proto)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_udf_ipprotocol_get(int unit, int index, uint32 *flags,
                                 uint8 *proto)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_udf_get(int unit,
                      bcm_field_udf_spec_t *udf_spec,
                      bcm_field_udf_t udf_id)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_udf_destroy(int unit,
                          bcm_field_udf_t udf_id)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qset_add_udf(int unit,
                           bcm_field_qset_t *qset,
                           bcm_field_udf_t udf_id)

{
    return (BCM_E_NONE);
}

#pragma mark - ====== field entry ======
/*
 * Function: bcm_esw_field_entry_create
 *
 * Purpose:
 *     Create a blank entry based on a group. Automatically generate an entry
 *     ID.
 *
 * Parameters:
 *     unit  - BCM device number
 *     group - Field group ID
 *     entry - (OUT) New entry
 *
 * Returns:
 *     BCM_E_INIT        BCM unit not initialized
 *     BCM_E_NOT_FOUND   group not found in unit
 *     BCM_E_PARAM       *entry was NULL
 *     BCM_E_RESOURCE    No unused entries available
 *     BCM_E_XXX         Error from bcm_field_entry_create_id
 *
 * See Also:
 * bcm_field_entry_create_id
 */

int
bcm_esw_field_entry_create(int unit,
                           bcm_field_group_t group,
                           bcm_field_entry_t *entry)
{
    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);
    uint32 entry_idx;

    if (NULL == grp)
    {
        return BCM_E_PARAM;
    }

    for (entry_idx = 0; entry_idx < _countof(grp->entry_tbl); ++entry_idx)
    {
        _field_entry_t *rule = &grp->entry_tbl[entry_idx];

        if (0 != rule->valid)
        {
            continue;
        }

        rule->valid = 1;

        assert(-9999 < grp->min_rule_pri);

        rule->pri   = --grp->min_rule_pri;
        rule->policer_id = 0xffffffff;
        rule->counter_id = 0xffffffff;

        *entry = rule->dev_rule_id;

        _field_entry_sort(unit, group);
        _field_group_validate(unit, group);

        return BCM_E_NONE;
    }

    return BCM_E_RESOURCE;
}

int
bcm_esw_field_entry_create_id(int unit,
                              bcm_field_group_t group,
                              bcm_field_entry_t entry)
{
    return (BCM_E_NONE);
}

/*
 * Function: bcm_esw_field_entry_destroy
 *
 * Purpose:
 *     Deallocate the memory used to track an entry. This does not remove the
 *     entry from hardware if it has been installed. In order to remove an
 *     entry from hardware, call bcm_field_entry_remove() prior to calling this
 *     API.
 *
 * Parameters:
 *     unit  - BCM device number
 *     entry - Entry ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found
 *     BCM_E_XXX       - From bcm_field_counter_destroy() or
 *                       bcm_field_meter_destroy()
 *     BCM_E_NONE      - Success
 */

int
bcm_esw_field_entry_destroy(int unit,
                            bcm_field_entry_t entry)
{
    _field_entry_t *ent = _field_entry_get(unit, entry);

    assert(NULL != ent);
    assert(0 != ent->valid);

    if (0xffffffff != ent->policer_id)
    {
        _field_policer_t *pol = _field_policer_get(unit, ent->policer_id);

        assert(NULL != pol);
        assert(0 != pol->valid);
        assert(0 < pol->ref_cnt);

        --pol->ref_cnt;
        if (0 == pol->ref_cnt)
        {
            pol->valid = 0;
        }
    }

    ent->valid = 0;

    return BCM_E_NONE;
}

int
bcm_esw_field_entry_destroy_all(int unit)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_entry_copy(int unit,
                         bcm_field_entry_t src_entry,
                         bcm_field_entry_t *dst_entry)
{
    return (BCM_E_NONE);
}

/*
 * Function: bcm_esw_field_entry_copy_id
 *
 * Purpose:
 *     Create a copy of an existing entry with a requested ID
 *
 * Parameters:
 *     unit      - BCM device number
 *     src_entry - Source entry to copy
 *     dst_entry - Destination entry for copy
 *
 * Returns:
 *     BCM_E_INIT      - BCM Unit not initialized
 *     BCM_E_NOT_FOUND - Source Entry ID not found
 *     BCM_E_XXX       - Error code from bcm_field_entry_create_id()
 *     BCM_E_NONE      - Success
 */

int
bcm_esw_field_entry_copy_id(int unit,
                            bcm_field_entry_t src_entry,
                            bcm_field_entry_t dst_entry)
{
    _field_entry_t *src_ent = _field_entry_get(unit, src_entry);
    _field_entry_t *dst_ent = _field_entry_get(unit, dst_entry);

    assert(NULL != src_ent);
    assert(0 != src_ent->valid);

    assert(NULL != dst_ent);
    assert(0 != dst_ent->valid);

    if (0xffffffff != src_ent->policer_id)
    {
        _field_policer_detach(unit, dst_entry);
        _field_policer_attach(unit, dst_entry, src_ent->policer_id);
    }

    if (0xffffffff != src_ent->counter_id)
    {
        _field_counter_detach(unit, dst_entry);
        _field_counter_attach(unit, dst_entry, src_ent->counter_id);
    }

    memcpy(dst_ent, src_ent, sizeof(*dst_ent));

    return (BCM_E_NONE);
}

/*
 * Function: bcm_esw_field_entry_install
 *
 * Purpose:
 *     Install a entry into the hardware tables.
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Entry to install
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found on unit.
 *     BCM_E_XXX       - Error from _field_XX_tcam_policy_install()
 *     BCM_E_NONE      - Success
 *
 * Notes:
 *     Qualifications should be made and actions should be added
 *     prior to installing the entry.
 */

int
bcm_esw_field_entry_install(int unit,
                            bcm_field_entry_t entry)
{
    _field_entry_t *ent = _field_entry_get(unit, entry);

//    if (NULL == ent)
//    {
//        return BCM_E_PARAM;
//    }

    assert(NULL != ent);
    assert(0 == ent->installed);
    ent->installed = 1;

    return BCM_E_NONE;
}

/*
 * Function:
 *
 * Purpose:
 *     Re-install a entry into the hardware tables.
 *
 * Parameters:
 *     unit  - (IN) BCM device number
 *     entry - (IN) Entry to install
 *
 * Returns:
 *     BCM_E_XXX
 */

int
bcm_esw_field_entry_reinstall(int unit,
                              bcm_field_entry_t entry)
{
    bcm_esw_field_entry_remove(unit, entry);

    return bcm_esw_field_entry_install(unit, entry);
}

/*
 * Function: _field_entry_remove
 *
 * Purpose:
 *     Remove an entry from the hardware tables.
 *
 * Parameters:
 *     unit  - (IN) BCM device number
 *     fc    - (IN) Field control structure.
 *     entry - (IN) Entry to remove.
 *
 * Returns:
 *     BCM_E_XXX
 */

int
bcm_esw_field_entry_remove(int unit,
                           bcm_field_entry_t entry)
{
    _field_entry_t *ent = _field_entry_get(unit, entry);

    assert(NULL != ent);

    ent->installed = 0;

    return BCM_E_NONE;
}

/*
 * Function: bcm_esw_field_entry_prio_get
 *
 * Purpose:
 *     Gets the priority within the group of the entry.
 *
 * Parameters:
 *     unit   - BCM device number
 *     entry  - Field entry to operate on
 *     prio   - (OUT) priority of entry
 *
 * Returns:
 *     BCM_E_NONE       - Success
 *     BCM_E_PARAM      - prio pointing to NULL
 *     BCM_E_NOT_FOUND  - Entry ID not found on unit
 */

int
bcm_esw_field_entry_prio_get(int unit, bcm_field_entry_t entry, int *prio)
{
    _field_entry_t *rule = _field_entry_get(unit, entry);

    assert(NULL != rule);
    assert(NULL != prio);

    *prio = rule->pri;

    return (BCM_E_NONE);
}

/*
 * Function: bcm_esw_field_entry_prio_set
 *
 * Purpose:
 *
 * Parameters:
 *     unit - BCM device number
 *     entry - Field entry to operate on
 *
 * Returns:
 *     BCM_E_NONE       Success
 *
 * Notes:
 *     The earlier checks guarantee that there is a free slot somewhere
 *     i.e. in one of the alloted slices for the group.
 */

int
bcm_esw_field_entry_prio_set(int unit, bcm_field_entry_t entry, int prio)
{
    _field_entry_t *rule = _field_entry_get(unit, entry);
    bcm_field_group_t group = (entry >> 16) & 0xff;

    assert(NULL != rule);

    rule->pri = prio;

    _field_entry_sort(unit, group);

    _field_group_validate(unit, group);

    return (BCM_E_NONE);
}

#pragma mark - ====== field stat ======

int bcm_esw_field_stat_create(int unit, bcm_field_group_t group , int nstat, bcm_field_stat_t *stat_arr, int *stat_id)
{
    _ut_field_counter_t *cnt = NULL;
    uint32 counter_id;

    assert(unit < _countof(g_field_db.device));

    for (counter_id = 0; counter_id < _countof(g_field_db.device[unit].counter); ++counter_id)
    {
        cnt = _field_counter_get(unit, counter_id);

        if (0 == cnt->valid || (0 != cnt->valid && 0 == cnt->ref_cnt))
        {
            cnt->ref_cnt = 0;
            cnt->valid = 1;
            break;
        }

        cnt = NULL;
    }

    if (NULL == cnt)
    {
        return BCM_E_RESOURCE;
    }

    *stat_id = counter_id;

    //_field_counter_attach(unit, entry, counter_id);

    return BCM_E_NONE;
}

int bcm_esw_field_entry_stat_attach(int unit, bcm_field_entry_t entry, int stat_id)
{
    _field_entry_t *rule = _field_entry_get(unit, entry);
    _ut_field_counter_t *cnt = _field_counter_get(unit, stat_id);

    assert(NULL != rule);
    assert(NULL != cnt);

    if (0 == cnt->valid)
        return BCM_E_UNAVAIL;

    _field_counter_attach(unit, entry, stat_id);

    return BCM_E_NONE;
}

int bcm_esw_field_entry_stat_detach(int unit, bcm_field_entry_t entry, int stat_id)
{
    _field_entry_t *rule = _field_entry_get(unit, entry);
    _ut_field_counter_t *cnt = _field_counter_get(unit, stat_id);

    assert(NULL != rule);
    assert(NULL != cnt);

    _field_counter_detach(unit, entry);

    return BCM_E_NONE;
}

int bcm_esw_field_entry_stat_get(int unit,bcm_field_entry_t entry, int *stat_id_p)
{
    _field_entry_t *entry_p = _field_entry_get(unit, entry);

    assert(NULL != entry_p);

    *stat_id_p = entry_p->counter_id;

    return BCM_E_NONE;
}

int bcm_esw_field_stat_set(int unit, int stat_id, bcm_field_stat_t stat, uint64 num)
{
    _ut_field_counter_t *cnt = _field_counter_get(unit, stat_id);

    assert(NULL != cnt);

    if (cnt->valid == 0)
        return BCM_E_UNAVAIL;

    cnt->counter[0] = num;

    return BCM_E_NONE;
}

int bcm_esw_field_stat_get(int unit, int stat_id, bcm_field_stat_t state, uint64 *cnt_p)
{
    _ut_field_counter_t *cnt;
    cnt = _field_counter_get(unit, stat_id);

    assert(NULL != cnt);
    assert(0 != cnt->valid);

    *cnt_p = cnt->counter[0];
    return (BCM_E_NONE);
}

int bcm_esw_field_stat_get32(int unit, int stat_id, bcm_field_stat_t state, uint32 *cnt_p)
{
    int rc;
    uint64 val64;

    rc = bcm_esw_field_stat_get(unit, stat_id, state, &val64);

    *cnt_p = (uint32) val64;

    return rc;
}

int
bcm_esw_field_resync(int unit)
{
    return (BCM_E_NONE);
}

/*------------------------------------------------------------------------------
 * Dependency include
 *------------------------------------------------------------------------------
 */
#include "dev_rm.h"
#pragma mark - ====== field qualify ======
int bcm_esw_field_qualify_validate(int unit, bcm_field_entry_t entry, int qualify)
{
    bcm_field_group_t group = (entry >> 16) & 0xff;
    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);
    assert(0 != DEVRM_SHR_BITGET(grp->qset.w, qualify));

    return (0 != DEVRM_SHR_BITGET(grp->qset.w, qualify)) ? BCM_E_NONE : BCM_E_PARAM;
}

int
bcm_esw_field_qualify_InPort(int unit, bcm_field_entry_t entry,
                             bcm_port_t data, bcm_port_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyInPort);    
}

int
bcm_esw_field_qualify_OutPort(int unit, bcm_field_entry_t entry,
                             bcm_port_t data, bcm_port_t mask)
{
//    assert(bcmFieldQualifyOutPort == DEVRM_FIELD_QUALIFY_OutPort);
    return bcm_esw_field_qualify_validate(unit, entry, DEVRM_FIELD_QUALIFY_OutPort); //bcmFieldQualifyOutPort);
}

int
bcm_esw_field_qualify_InPorts(int unit, bcm_field_entry_t entry,
                              bcm_pbmp_t data, bcm_pbmp_t mask)
{
//    bcm_field_group_t group = (entry >> 16) & 0xff;
//    _field_group_t *grp = _bcm_esw_field_group_get(unit, group);
//
//    assert(0 != DEVRM_SHR_BITGET(grp->qset.w, bcmFieldQualifyInPorts));
//
//
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyInPorts);
}

int
bcm_esw_field_qualify_OutPorts(int unit, bcm_field_entry_t entry,
                               bcm_pbmp_t data, bcm_pbmp_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyOutPorts);
}

int
bcm_esw_field_qualify_Drop(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyDrop);
}

int
bcm_esw_field_qualify_SrcModid(int unit, bcm_field_entry_t entry,
                               bcm_module_t data, bcm_module_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifySrcModid);
}

int
bcm_esw_field_qualify_SrcPortTgid(int unit, bcm_field_entry_t entry,
                                  bcm_port_t data, bcm_port_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifySrcPortTgid);
}

int
bcm_esw_field_qualify_SrcPort(int unit, bcm_field_entry_t entry,
                              bcm_module_t data_modid,
                              bcm_module_t mask_modid,
                              bcm_port_t   data_port,
                              bcm_port_t   mask_port)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifySrcPort);
}

int
bcm_esw_field_qualify_SrcTrunk(int unit, bcm_field_entry_t entry,
                               bcm_trunk_t data, bcm_trunk_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifySrcTrunk);
}

int
bcm_esw_field_qualify_DstModid(int unit, bcm_field_entry_t entry,
                               bcm_module_t data, bcm_module_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyDstModid);
}

int
bcm_esw_field_qualify_DstPortTgid(int unit, bcm_field_entry_t entry,
                                  bcm_port_t data, bcm_port_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyDstPortTgid);
}

int
bcm_esw_field_qualify_DstPort(int unit, bcm_field_entry_t entry,
                              bcm_module_t data_modid,
                              bcm_module_t mask_modid,
                              bcm_port_t   data_port,
                              bcm_port_t   mask_port)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyDstPort);
}

int
bcm_esw_field_qualify_DstTrunk(int unit, bcm_field_entry_t entry,
                               bcm_trunk_t data, bcm_trunk_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyDstTrunk);
}

int
bcm_esw_field_qualify_L4SrcPort(int unit, bcm_field_entry_t entry,
                                bcm_l4_port_t data, bcm_l4_port_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyL4SrcPort);
}

int
bcm_esw_field_qualify_L4DstPort(int unit, bcm_field_entry_t entry,
                                bcm_l4_port_t data, bcm_l4_port_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, DEVRM_FIELD_QUALIFY_L4DstPort); //bcmFieldQualifyL4DstPort);
}

int
bcm_esw_field_qualify_OuterVlan(int unit, bcm_field_entry_t entry,
                                bcm_vlan_t data, bcm_vlan_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyOuterVlan);
}

int
bcm_esw_field_qualify_InnerVlan(int unit, bcm_field_entry_t entry,
                                bcm_vlan_t data, bcm_vlan_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyInnerVlan);
}

int
bcm_esw_field_qualify_EtherType(int unit, bcm_field_entry_t entry,
                                uint16 data, uint16 mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, DEVRM_FIELD_QUALIFY_EtherType); //bcmFieldQualifyEtherType);
}

int
bcm_esw_field_qualify_IpProtocol(int unit, bcm_field_entry_t entry,
                                 uint8 data, uint8 mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, DEVRM_FIELD_QUALIFY_IpProtocol); //bcmFieldQualifyIpProtocol);
}

int
bcm_esw_field_qualify_LookupStatus(int unit, bcm_field_entry_t entry,
                                   uint32 data, uint32 mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, DEVRM_FIELD_QUALIFY_LookupStatus); //bcmFieldQualifyLookupStatus);
}


int
bcm_esw_field_qualify_DosAttack(int unit, bcm_field_entry_t entry,
                                uint8 data, uint8 mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyDosAttack);
}

int
bcm_esw_field_qualify_IpmcStarGroupHit(int unit, bcm_field_entry_t entry,
                                       uint8 data, uint8 mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyIpmcStarGroupHit);
}

int
bcm_esw_field_qualify_L3DestRouteHit(int unit, bcm_field_entry_t entry,
                                uint8 data, uint8 mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyL3DestRouteHit);
}

int
bcm_esw_field_qualify_L3DestHostHit(int unit, bcm_field_entry_t entry,
                                uint8 data, uint8 mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyL3DestHostHit);
}

int
bcm_esw_field_qualify_L3SrcHostHit(int unit, bcm_field_entry_t entry,
                               uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_L2CacheHit(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_L2StationMove(int unit, bcm_field_entry_t entry,
                                    uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_L2DestHit(int unit, bcm_field_entry_t entry,
                                uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_L2SrcStatic(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_L2SrcHit(int unit, bcm_field_entry_t entry,
                               uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_IngressStpState(int unit,bcm_field_entry_t entry,
                                      uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_ForwardingVlanValid(int unit, bcm_field_entry_t entry,
                                          uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_VlanTranslationHit(int unit, bcm_field_entry_t entry,
                                         uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}


int
bcm_esw_field_qualify_IpInfo(int unit, bcm_field_entry_t entry,
                             uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_PacketRes(int unit, bcm_field_entry_t entry,
                                uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_SrcIp(int unit, bcm_field_entry_t entry,
                            bcm_ip_t data, bcm_ip_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifySrcIp);
}

int
bcm_esw_field_qualify_DstIp(int unit, bcm_field_entry_t entry,
                            bcm_ip_t data, bcm_ip_t mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyDstIp);
}

int
bcm_esw_field_qualify_DSCP(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
//    return (BCM_E_NONE);
//    assert(bcmFieldQualifyDSCP == DEVRM_FIELD_QUALIFY_DSCP);
    return bcm_esw_field_qualify_validate(unit, entry, DEVRM_FIELD_QUALIFY_DSCP); //bcmFieldQualifyDSCP);
}

int
bcm_esw_field_qualify_Tos(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
//    return (BCM_E_NONE);
    return bcm_esw_field_qualify_validate(unit, entry, bcmFieldQualifyTos);
}

int
bcm_esw_field_qualify_IpFlags(int unit, bcm_field_entry_t entry,
                              uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_TcpControl(int unit, bcm_field_entry_t entry,
                                 uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_Ttl(int unit, bcm_field_entry_t entry,
                          uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_RangeCheck(int unit, bcm_field_entry_t entry,
                                 bcm_field_range_t range, int invert)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_SrcIp6(int unit, bcm_field_entry_t entry,
                             bcm_ip6_t data, bcm_ip6_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_DstIp6(int unit, bcm_field_entry_t entry,
                             bcm_ip6_t data, bcm_ip6_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_SrcIp6High(int unit, bcm_field_entry_t entry,
                                 bcm_ip6_t data, bcm_ip6_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_DstIp6High(int unit, bcm_field_entry_t entry,
                                 bcm_ip6_t data, bcm_ip6_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_Ip6NextHeader(int unit, bcm_field_entry_t entry,
                                    uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_Ip6TrafficClass(int unit, bcm_field_entry_t entry,
                                      uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_Ip6FlowLabel(int unit, bcm_field_entry_t entry,
                                   uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_Ip6HopLimit(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_SrcMac(int unit, bcm_field_entry_t entry,
                             bcm_mac_t data, bcm_mac_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_DstMac(int unit, bcm_field_entry_t entry,
                             bcm_mac_t data, bcm_mac_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_PacketFormat(int unit, bcm_field_entry_t entry,
                                   uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_SrcMacGroup(int unit, bcm_field_entry_t entry,
                                  uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_IpType(int unit, bcm_field_entry_t entry,
                             bcm_field_IpType_t data)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_L2Format(int unit, bcm_field_entry_t entry,
                               bcm_field_L2Format_t type)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_VlanFormat(int unit, bcm_field_entry_t entry,
                                 uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_MHOpcode(int unit, bcm_field_entry_t entry,
                               uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_UserDefined(int unit, bcm_field_entry_t entry,
                                  bcm_field_udf_t udf_id,
                                  uint8 data[BCM_FIELD_USER_FIELD_SIZE],
                                  uint8 mask[BCM_FIELD_USER_FIELD_SIZE])
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_Decap(int unit, bcm_field_entry_t entry,
                            bcm_field_decap_t decap)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_HiGig(int unit, bcm_field_entry_t entry,
                            uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_L3IntfGroup(int unit, bcm_field_entry_t entry,
                                  bcm_if_group_t data, bcm_if_group_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_InterfaceClassL2(int unit, bcm_field_entry_t entry,
                                       uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_InterfaceClassL3(int unit, bcm_field_entry_t entry,
                                       uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_InterfaceClassPort(int unit, bcm_field_entry_t entry,
                                       uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_SrcClassL2(int unit, bcm_field_entry_t entry,
                                 uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_SrcClassL3(int unit, bcm_field_entry_t entry,
                                 uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_SrcClassField(int unit, bcm_field_entry_t entry,
                                     uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_DstClassL2(int unit, bcm_field_entry_t entry,
                                 uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_DstClassL3(int unit, bcm_field_entry_t entry,
                                 uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_DstClassField(int unit, bcm_field_entry_t entry,
                                     uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

//int
//bcm_esw_field_qualify_Stage(int unit, bcm_field_entry_t entry,
//                            bcm_field_stage_t stage)
//{
//    return (BCM_E_NONE);
//}

int
bcm_esw_field_qualify_SrcIpEqualDstIp(int unit, bcm_field_entry_t entry,
                            uint32 flag)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_EqualL4Port(int unit, bcm_field_entry_t entry,
                            uint32 flag)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_TcpSequenceZero(int unit, bcm_field_entry_t entry,
                            uint32 flag)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_TcpHeaderSize(int unit, bcm_field_entry_t entry,
                                 uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_IpProtocolCommon(int unit, bcm_field_entry_t entry,
                                       bcm_field_IpProtocolCommon_t protocol)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_Snap(int unit, bcm_field_entry_t entry,
                           bcm_field_snap_header_t data,
                           bcm_field_snap_header_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_Llc(int unit, bcm_field_entry_t entry,
                          bcm_field_llc_header_t data,
                          bcm_field_llc_header_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_InnerTpid(int unit, bcm_field_entry_t entry,
                                uint16 tpid)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_OuterTpid(int unit, bcm_field_entry_t entry,
                                uint16 tpid)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_PortClass(int unit, bcm_field_entry_t entry,
                                uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int bcm_esw_field_qualify_L3Routable(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int bcm_esw_field_qualify_IpFrag(int unit, bcm_field_entry_t entry,
                                  bcm_field_IpFrag_t frag_info)
{
    return (BCM_E_NONE);
}

int bcm_esw_field_qualify_LookupClass0(int unit, bcm_field_entry_t entry,
                                       uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int bcm_esw_field_qualify_Vrf(int unit, bcm_field_entry_t entry,
                              uint32 data, uint32 mask)
{
    return (BCM_E_NONE);
}

int bcm_esw_field_qualify_L4Ports(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int bcm_esw_field_qualify_MirrorCopy(int unit, bcm_field_entry_t entry,
                                  uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}


int bcm_esw_field_qualify_TunnelTerminated(int unit, bcm_field_entry_t entry,
                                           uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int bcm_esw_field_qualify_MplsTerminated(int unit, bcm_field_entry_t entry,
                                           uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}


int bcm_esw_field_qualify_ExtensionHeaderType(int unit,
                                              bcm_field_entry_t entry,
                                              uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int bcm_esw_field_qualify_ExtensionHeaderSubCode(int unit,
                                                 bcm_field_entry_t entry,
                                                 uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}


int
bcm_esw_field_qualify_InnerSrcIp(int unit, bcm_field_entry_t entry,
                                 bcm_ip_t data, bcm_ip_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_InnerDstIp(int unit, bcm_field_entry_t entry,
                                 bcm_ip_t data, bcm_ip_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_InnerSrcIp6(int unit, bcm_field_entry_t entry,
                                  bcm_ip6_t data, bcm_ip6_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_InnerDstIp6(int unit, bcm_field_entry_t entry,
                                  bcm_ip6_t data, bcm_ip6_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_InnerSrcIp6High(int unit, bcm_field_entry_t entry,
                                      bcm_ip6_t data, bcm_ip6_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_InnerDstIp6High(int unit, bcm_field_entry_t entry,
                                      bcm_ip6_t data, bcm_ip6_t mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_InnerDSCP(int unit, bcm_field_entry_t entry,
                                uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_InnerIpProtocol(int unit, bcm_field_entry_t entry,
                                      uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}


int bcm_esw_field_qualify_InnerIpFrag(int unit, bcm_field_entry_t entry,
                                      bcm_field_IpFrag_t frag_info)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_InnerTtl(int unit, bcm_field_entry_t entry,
                               uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_IpAuth(int unit, bcm_field_entry_t entry,
                             uint8 data, uint8 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_BigIcmpCheck(int unit, bcm_field_entry_t entry,
                                   uint32 flag, uint32 size)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_IcmpTypeCode(int unit, bcm_field_entry_t entry,
                                   uint16 data,uint16 mask)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_IgmpTypeMaxRespTime(int unit, bcm_field_entry_t entry,
                                          uint16 data,uint16 mask)
{
    return (BCM_E_NONE);
}

#pragma mark - ====== field action ======

int
bcm_esw_field_action_add(int unit,
                         bcm_field_entry_t entry,
                         bcm_field_action_t action,
                         uint32 param0,
                         uint32 param1)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_action_ports_add(int unit,
                               bcm_field_entry_t entry,
                               bcm_field_action_t action,
                               bcm_pbmp_t pbmp)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_action_get(int unit,
                         bcm_field_entry_t entry,
                         bcm_field_action_t action,
                         uint32 *param0,
                         uint32 *param1)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_action_ports_get(int unit,
                               bcm_field_entry_t entry,
                               bcm_field_action_t action,
                               bcm_pbmp_t *pbmp)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_action_delete(int unit, bcm_field_entry_t entry,
                        bcm_field_action_t action,
                        uint32 param0, uint32 param1)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_action_remove(int unit,
                            bcm_field_entry_t entry,
                            bcm_field_action_t action)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_action_remove_all(int unit,
                                bcm_field_entry_t entry)
{
    return (BCM_E_NONE);
}

/* Section: Field Counters */

/*
 * Function: bcm_esw_field_counter_create
 *
 * Description:
 *      Create a field counter pair within an entry.
 *
 * Parameters:
 *      unit  - (IN) BCM device number.
 *      entry - (IN) Entry ID.
 *
 * Returns:
 *      BCM_E_INIT      - BCM unit not initialized.
 *      BCM_E_NOT_FOUND - Entry not found in unit
 *      BCM_E_EXISTS    - Entry already has counter.
 *      BCM_E_RESOURCE  - No counters left.
 *      BCM_E_MEMORY    - Counter allocation error
 *      BCM_E_NONE      - Success
 */

int
bcm_esw_field_counter_create(int unit,
                             bcm_field_entry_t entry)
{
    _ut_field_counter_t *cnt = NULL;
    uint32 counter_id;

    assert(unit < _countof(g_field_db.device));

    for (counter_id = 0; counter_id < _countof(g_field_db.device[unit].counter); ++counter_id)
    {
        cnt = _field_counter_get(unit, counter_id);

        if (0 == cnt->valid || (0 != cnt->valid && 0 == cnt->ref_cnt))
        {
            cnt->ref_cnt = 0;
            cnt->valid = 1;
            break;
        }

        cnt = NULL;
    }

    if (NULL == cnt)
    {
        return BCM_E_RESOURCE;
    }

    _field_counter_attach(unit, entry, counter_id);

    return BCM_E_NONE;
}

/*
 * Function: bcm_esw_field_counter_share
 *
 * Description:
 *      Share the counter pair from one entry with another entry.
 *
 * Parameters:
 *      unit      - (IN) BCM device number
 *      src_entry - (IN) Entry ID that has counter pair
 *      dst_entry - (IN) Entry ID that will get counter pair
 *
 * Returns:
 *      BCM_E_INIT      - BCM unit not initialized
 *      BCM_E_NOT_FOUND - Either source or destination entry not found
 *      BCM_E_PARAM     - Source and destination are not in the same slice.
 *      BCM_E_EXISTS    - Destination entry already has counter.
 *      BCM_E_EMPTY     - No source counter exists.
 *      BCM_E_NONE      - Success
 *
 * Notes:
 *     The src_entry and dst_entry should belong to the same field group
 */

int
bcm_esw_field_counter_share(int unit,
                            bcm_field_entry_t src_entry,
                            bcm_field_entry_t dst_entry)
{
    _field_entry_t *src = _field_entry_get(unit, src_entry);

    assert(NULL != src);

    assert(0xffffffff != src->counter_id);

    _field_counter_detach(unit, dst_entry);
    _field_counter_attach(unit, dst_entry, src->counter_id);

    return BCM_E_NONE;
}

/*
 * Function: bcm_esw_field_counter_destroy
 *
 * Description:
 *     Remove a counter pair from an entry. If no other entries reference
 *     the counter, it is deallocated.
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     entry - (IN) Entry ID.
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NOT_FOUND - Entry not found.
 *     BCM_E_EMPTY     - No source counter exists.
 *     BCM_E_NONE      - Success
 *
 * Notes:
 *     Counter pair becomes available for reallocation if it
 *     is not associated with any other entry rule.
 */

int
bcm_esw_field_counter_destroy(int unit,
                              bcm_field_entry_t entry)
{
    _field_counter_detach(unit, entry);

    return BCM_E_NONE;
}

int
bcm_esw_field_counter_set(int unit,
                          bcm_field_entry_t entry,
                          int counter_num,
                          uint64 val)
{
    _field_entry_t *ent = _field_entry_get(unit, entry);
    _ut_field_counter_t *cnt;

    assert(NULL != ent);
    assert(0xffffffff != ent->counter_id);

    cnt = _field_counter_get(unit, ent->counter_id);

    assert(NULL != cnt);
    assert(0 != cnt->valid);

    cnt->counter[counter_num] = val;
    return (BCM_E_NONE);
}

int
bcm_esw_field_counter_set32(int unit, bcm_field_entry_t entry, int counter_num,
                            uint32 val)
{
    return bcm_esw_field_counter_set(unit, entry, counter_num, val);
}

int
bcm_esw_field_counter_get(int unit,
                          bcm_field_entry_t entry,
                          int counter_num,
                          uint64 *valp)
{
    _field_entry_t *ent = _field_entry_get(unit, entry);
    _ut_field_counter_t *cnt;

    assert(NULL != ent);
    assert(0xffffffff != ent->counter_id);

    cnt = _field_counter_get(unit, ent->counter_id);

    assert(NULL != cnt);
    assert(0 != cnt->valid);

    *valp = cnt->counter[counter_num];
    return (BCM_E_NONE);
}

int
bcm_esw_field_counter_get32(int unit,
                            bcm_field_entry_t entry,
                            int counter_num,
                            uint32 *valp)
{
    int rc;
    uint64 val64;

    rc = bcm_esw_field_counter_get(unit, entry, counter_num, &val64);
    if (BCM_E_NONE != rc)
    {
        return rc;
    }

    *valp = val64;
    return (BCM_E_NONE);
}

/*
 * Function: bcm_esw_field_meter_create
 *
 * Description:
 *      Create a meter group and associate it with the entry.
 *
 * Parameters:
 *      unit - BCM device number
 *      entry - Entry ID
 *
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_field_meter_create(int unit,
                           bcm_field_entry_t entry)
{
    _field_policer_t *pol = NULL;
    bcm_policer_t policer_id;

    assert(unit < _countof(g_field_db.device));

    for (policer_id = 0; policer_id < _countof(g_field_db.device[unit].policer); ++policer_id)
    {
        pol = _field_policer_get(unit, policer_id);

        if (0 == pol->valid || (0 != pol->valid && 0 == pol->ref_cnt))
        {
            pol->ref_cnt = 0;
            pol->valid = 1;
            break;
        }

        pol = NULL;
    }

    if (NULL == pol)
    {
        return BCM_E_RESOURCE;
    }

    _field_policer_attach(unit, entry, policer_id);

    return BCM_E_NONE;
}

/*
 * Function: bcm_esw_field_meter_share
 *
 * Description:
 *     Share the meter pair from one entry with a second entry.
 *
 * Parameters:
 *      unit - BCM device number
 *      src_entry - Entry ID that has meter group
 *      dst_entry - Entry ID that will get meter group
 *
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_field_meter_share(int unit,
                          bcm_field_entry_t src_entry,
                          bcm_field_entry_t dst_entry)
{
    _field_entry_t *src = _field_entry_get(unit, src_entry);

    assert(NULL != src);

    assert(0xffffffff != src->policer_id);

    _field_policer_detach(unit, dst_entry);
    _field_policer_attach(unit, dst_entry, src->policer_id);

    return BCM_E_NONE;
}

/*
 * Function: bcm_esw_field_meter_destroy
 *
 * Description:
 *     Delete a meter associated with a entry.
 *
 * Parameters:
 *     unit  - BCM device number
 *     entry - Entry ID
 *
 * Returns:
 *     BCM_E_XXX
 *
 * Notes:
 *     The meter is only deallocated if no other entries are using it.
 */

int
bcm_esw_field_meter_destroy(int unit,
                            bcm_field_entry_t entry)
{
    _field_policer_detach(unit, entry);

    return BCM_E_NONE;
}

int
bcm_esw_field_meter_set(int unit,
                        bcm_field_entry_t entry,
                        int meter_num,
                        uint32 kbits_sec,
                        uint32 kbits_burst)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_meter_get(int unit,
                        bcm_field_entry_t entry,
                        int meter_num,
                        uint32 *kbits_sec,
                        uint32 *kbits_burst)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_entry_dump(int unit,
                         bcm_field_entry_t entry)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_show(int unit,
                   const char *pfx)
{
    return (BCM_E_NONE);
}

#pragma mark - ====== field data qualifier_ ======
int
bcm_esw_field_data_qualifier_create(int unit,
                                 bcm_field_data_qualifier_t *data_qualifier)
{
    return (BCM_E_NONE);
}


int
bcm_esw_field_data_qualifier_destroy(int unit, int qual_id)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_data_qualifier_destroy_all(int unit)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qset_data_qualifier_add(int unit, bcm_field_qset_t *qset,
                                      int qual_id)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_qualify_data(int unit, bcm_field_entry_t eid, int qual_id,
                           uint8 *data, uint8 *mask, uint16 length)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_data_qualifier_ethertype_add(int unit, int qual_id,
                                 bcm_field_data_ethertype_t *etype)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_data_qualifier_ethertype_delete(int unit, int qual_id,
                                 bcm_field_data_ethertype_t *etype)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_data_qualifier_ip_protocol_add(int unit, int qual_id,
                                 bcm_field_data_ip_protocol_t *ip_protocol)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_data_qualifier_ip_protocol_delete(int unit, int qual_id,
                                 bcm_field_data_ip_protocol_t *ip_protocol)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_data_qualifier_packet_format_add(int unit,  int qual_id,
                                 bcm_field_data_packet_format_t *packet_format)
{
    return (BCM_E_NONE);
}

int
bcm_esw_field_data_qualifier_packet_format_delete(int unit, int qual_id,
                                 bcm_field_data_packet_format_t *packet_format)
{
    return (BCM_E_NONE);
}

#pragma mark - ====== policer ======
int
bcm_esw_policer_init(int unit)
{
    return (BCM_E_NONE);
}
int
bcm_esw_policer_get(int unit, bcm_policer_t policer_id,
                    bcm_policer_config_t *pol_cfg)
{
    return (BCM_E_NONE);
}
int
bcm_esw_policer_set(int unit, bcm_policer_t policer_id,
                    bcm_policer_config_t *pol_cfg)
{
    return (BCM_E_NONE);
}
int
bcm_esw_field_entry_policer_attach(int unit, bcm_field_entry_t entry_id,
                                   int level, bcm_policer_t policer_id)
{
    return (BCM_E_NONE);
}
int
bcm_esw_field_entry_policer_detach(int unit, bcm_field_entry_t entry_id,
                                   int level)
{
    return (BCM_E_NONE);
}
int
bcm_esw_field_entry_policer_detach_all(int unit, bcm_field_entry_t entry_id)
{
    return (BCM_E_NONE);
}
int
bcm_esw_field_entry_policer_get(int unit, bcm_field_entry_t entry_id,
                                int level, bcm_policer_t *policer_id)
{
    return (BCM_E_NONE);
}

int
bcm_esw_policer_group_create(int unit, bcm_policer_group_mode_t mode,
                         bcm_policer_t *policer_id, int *npolicers)
{
    return BCM_E_NONE;
}
int
bcm_esw_policer_create(int unit, bcm_policer_config_t *pol_cfg,
                       bcm_policer_t *policer_id)
{
    return BCM_E_NONE;
}

int
bcm_esw_policer_destroy(int unit, bcm_policer_t policer_id)
{
    return BCM_E_NONE;
}

int
bcm_esw_policer_destroy_all(int unit)
{
    return BCM_E_NONE;
}
int
bcm_esw_policer_traverse(int unit, bcm_policer_traverse_cb cb,
                         void *user_data)
{
    return BCM_E_NONE;
}
