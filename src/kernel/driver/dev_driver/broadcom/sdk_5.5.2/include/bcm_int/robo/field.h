/*
 * $Id: field.h,v 1.17.2.2 Broadcom SDK $
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
 * File:        field.h
 * Purpose:     Internal Field Processor data structure definitions for the
 *              BCM library.
 *
 */

#ifndef _BCM_INT_FIELD_H
#define _BCM_INT_FIELD_H

#include <bcm/field.h>
#include <bcm/debug.h>

#include <soc/mem.h>
#include <soc/cfp.h>


#include <sal/core/sync.h>

/* #ifdef BCM_FIELD_SUPPORT */

/*
 * Debugging output macros.
 */
#define FP_DEBUG(flags, stuff) BCM_DEBUG((flags) | BCM_DBG_FP, stuff)
#define FP_OUT(stuff)          BCM_DEBUG(BCM_DBG_FP, stuff)
#define FP_WARN(stuff)         FP_DEBUG(BCM_DBG_WARN, stuff)
#define FP_ERR(stuff)          FP_DEBUG(BCM_DBG_ERR, stuff)
#define FP_VERB(stuff)         FP_DEBUG(BCM_DBG_VERBOSE, stuff)
#define FP_VVERB(stuff)        FP_DEBUG(BCM_DBG_VVERBOSE, stuff)

#define FP_SHOW(stuff)         ((*soc_cm_print) stuff)

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

/* 5348 */

#define FP_KEY_PATTERN      (0xdeadbeef)

#define FP_SLICE_SZ_MAX         1024

#define BCM_FIELD_USER_NUM_UDFS_MAX     93 


/* Only for BCM53242 */
#define FP_BCM53242_L2_FRM_FMT_OTHERS   0x00
#define FP_BCM53242_L2_FRM_FMT_ETHER_II 0x01
#define FP_BCM53242_L2_FRM_FMT_SNAP     0x02
#define FP_BCM53242_L2_FRM_FMT_MASK     0x03

#define FP_BCM53242_L3_FRM_FMT_OTHERS   0x00
#define FP_BCM53242_L3_FRM_FMT_IP4      0x01
#define FP_BCM53242_L3_FRM_FMT_IP6      0x02
#define FP_BCM53242_L3_FRM_FMT_MASK     0x03

#define FP_BCM53242_L4_FRM_FMT_OTHERS   0x00
#define FP_BCM53242_L4_FRM_FMT_TCP      0x01
#define FP_BCM53242_L4_FRM_FMT_UDP      0x02
#define FP_BCM53242_L4_FRM_FMT_ICMPIGMP 0x03
#define FP_BCM53242_L4_FRM_FMT_MASK     0x03

/* BCM53115 Vulcan */
#define FP_BCM53115_L3_FRM_FMT_IP4      0x00
#define FP_BCM53115_L3_FRM_FMT_IP6      0x01
#define FP_BCM53115_L3_FRM_FMT_NON_IP   0x03
#define FP_BCM53115_L3_FRM_FMT_MASK     0x03

#define FP_BCM53115_CHAIN_IDX_MAX 255


/*
 * Meter setting limits.
 */
#define BCM_FIELD_METER_KBITS_BURST_MAX   (131072)             /* 128Mb = 16MB      */
#define BCM_FIELD_METER_KBITS_RATE_MAX    (1024000) /* 1000Mb */

#define BCM_53242_FIELD_METER_KBITS_BURST_MAX   (8128)             /* 1016 * 8      */

/* Value for fc->tcam_ext_numb that indicates there is no external TCAM. */
#define FP_EXT_TCAM_NONE -1

/*
 * Initial group IDs and entry IDs.
 */
#define _FP_GROUP_ID_BASE 1
#define _FP_ENTRY_ID_BASE 1

/*
 * State for a structure.
 *   Dirty == memory differs from hardware
 *   Clean == memory matches hardware
 */
#define _FIELD_CLEAN 0
#define _FIELD_DIRTY 1

/*
 * Internal version of qset add that will allow UserDefined[01] to be set. 
 */
#define BCM_FIELD_QSET_ADD_INTERNAL(qset, q)  SHR_BITSET(((qset).w), (q))

/*
 * FPFx field select code macros
 *
 * These are for resolving if the select code has meaning or is really a don't
 * care.
 */

#define _FP_SELCODE_INVALID (-1)

#define _FP_SELCODE_INVALIDATE(selcode) \
     ((selcode) = _FP_SELCODE_INVALID);

#define _FP_SELCODE_IS_VALID(selcode) \
     (((selcode) == _FP_SELCODE_INVALID)?0:1)

#define _FP_SELCODE_IS_INVALID(selcode) \
     (!_FP_SELCODE_IS_VALID(selcode))

/*
 * Macros for _robo_field_entry_find() to specify which of the potentially 3
 * (triple-wide) entries to find.
 */
#define _FP_SLICE_PRIMARY     0 /* Base   slice for group */
#define _FP_SLICE_SECONDARY   1 /* Base+1 slice for group */
#define _FP_SLICE_TERTIARY    2 /* Base+2 slice for group */

/*
 * Macro:
 *     FIELD_ENTRY_SLICE_IDX_CHANGE
 * Purpose:
 *     Move the software entry to a new slice index.
 * Parameters:
 *     f_ent    - entry to be moved
 *     amount   - number of indexes to move + or -
 */
#define FIELD_ENTRY_SLICE_IDX_CHANGE(f_ent, amount)                 \
    assert((f_ent) != NULL);                                         \
    /* Move the software entry to the new index. */                  \
    (f_ent)->fs->entries[(f_ent)->slice_idx]          = NULL;        \
    (f_ent)->fs->entries[(f_ent)->slice_idx + (amount)] = (f_ent);   \
    (f_ent)->slice_idx += (amount);


#define _FIELD_SELCODE_CLEAR(unit, selcode)      \
    sal_memset(&selcode, _FP_SELCODE_INVALID, sizeof(struct _field_sel_s));
/*
 * Typedef:
 *     _field_udf_t
 * Purpose:
 *     Holds user-defined field (UDF) hardware metadata. 
 */
typedef struct _field_udf_s {
    uint8                  valid;     /* Indicates valid UDF             */
    int                    use_count; /* Number of groups using this UDF */
    bcm_field_qualify_t    udf_num;   /* UDFn (UDF0 or UDF1)             */
    uint32       slice_id; /* The slice ID which this UDF reside at */
} _field_udf_t;

/*
 * Struct:
 *     _field_counter32_collect_s
 * Purpose:
 *     Holds the accumulated count of FP Counters 
 *         Useful for wrap around and movement.
 *     This is used when h/w counter width is <= 32 bits
 */
typedef struct _field_counter32_collect_s {
    uint64 accumulated_counter;
    uint32 last_hw_value;
} _field_counter32_collect_t;


/*
 * Typedef:
 *     _field_control_t
 * Purpose:
 *     One structure for each StrataSwitch Device that holds the global
 *     field processor metadata for one device.
 */

typedef struct _field_control_s {
    sal_mutex_t            fc_lock;
    bcm_field_status_t     field_status; /* return status for module       */
    uint8                  color_indep;  /* default color independant flag */
    bcm_field_stage_t      stage;        /* default FP pipeline stage      */
    int                    tcam_sz;      /* number of entries in TCAM      */
    int                    tcam_slices;  /* number of internal slices      */
    int                    tcam_slice_sz;/* number of entries per slice    */
    int                    tcam_ext_numb;/* Slice number for external      */
                                         /* TCAM (-1 if not present)       */
    _field_udf_t           udf[BCM_FIELD_USER_NUM_UDFS_MAX]; 
    /* field_status->group_total elements indexed by priority */
    struct _field_group_s  *groups;      /* List of groups in unit         */
    struct _field_slice_s  *slices;      /* Array of 8, 16 or 17 slices    */
    struct _field_range_s  *ranges;  /* List of all ranges allocated   */
    uint32         range_id;     /* Seed ID for range creation     */
} _field_control_t;

/*
 * Typedef:
 *     _field_sel_t
 * Purpose:
 */
typedef struct _field_sel_s {
    int8               fpf; /* field(s) select 3-bit code */
} _field_sel_t;

/*
 * Typedef:
 *     _field_tcam_t
 * Purpose:
 *     These are the fields that are written into or read from FP_TCAM_xxx.
 */
typedef struct _field_tcam_s {
    uint32                 field[6];
    uint32                 f_mask[6];
} _field_tcam_t;


/*
 * Typedef:
 *     _qual_info_t
 * Purpose:
 *     Holds format info for a particular qualifier's access parameters. These
 *     nodes are stored in qualifier lists for groups and in the FPFx tables
 *     for each chip.
 */
typedef struct _qual_info_s {
    bcm_field_qualify_t    qid;     /* Which Qualifier              */
    drv_cfp_ram_t              mem;     /* Table                        */
    drv_cfp_field_t            field;     /* FPFx field choice            */
    struct _qual_info_s    *next;
} _qual_info_t;

/*
 * Typedef:
 *     _field_counter_t
 * Purpose:
 *     Holds the counter parameters to be written into FP_POLICY_TABLE
 *     (Firebolt) or FP_INTERNAL (Easyrider).
 */
typedef struct _field_counter_s {
    int                    index;
    uint8                  entries;    /* Number of entries using counter */
    _field_counter32_collect_t _field_x32_counters;
} _field_counter_t;

/*
 * Typedef:
 *     _field_slice_t
 * Purpose:
 *     This has the fields specific to a hardware slice.
 * Notes:
 */
typedef struct _field_slice_s {
    struct _field_group_s  *group;        /* ref to group that owns slice  */
    uint8                  slice_numb;    /* Hardware slice number         */
    uint8                  inst_flg;      /* Installed Flag                */
    bcm_field_qset_t       qset;          /* Q-set supported by this slice */
    _field_sel_t           sel_codes;     /* selects meaning of FPF[0-4]   */
    _qual_info_t           *qual_list;    /* offset & width for qualifiers */
    struct _field_entry_s  *entries[FP_SLICE_SZ_MAX]; /* List of entries   */
} _field_slice_t;


/*
 * Typedef:
 *     _field_group_t
 * Purpose:
 *     This is the logical group's internal storage structure. It has 1, 2 or
 *     3 slices, each with physical entry structures.
 * Notes:
 *   'ent_qset' should always be a subset of 'qset'.
 */
typedef struct _field_group_s {
    int                    unit;        /* bcm unit ID                        */
    bcm_field_group_t      gid;         /* opaque handle                      */
    bcm_field_qset_t       qset;        /* This group's Qualifier Set         */
    bcm_field_group_mode_t mode;        /* wide-mode choice                   */
    _field_slice_t         *slices;     /* Pointer into control's slice array */
    /*
     * Public data for each group: The number of used and available entries,
     * counters, and meters for a field group.
     */
    bcm_field_group_status_t group_status;
    struct _field_group_s  *next;         /* For storing in a linked-list  */
} _field_group_t;

/*
 * Typedef:
 *     _field_action_t
 * Purpose:
 *     This is the real action storage structure that is hidden behind
 *     the opaque handle bcm_field_action_t.
 */
typedef struct _field_action_s {
    bcm_field_action_t     action;       /* action type               */
    uint32                 param0;       /* Action specific parameter */
    uint32                 param1;       /* Action specific parameter */
    uint8                  inst_flg;     /* Installed Flag            */
    struct _field_action_s *next;
} _field_action_t;

/*
 * Typedef:
 *     _field_meter_t
 * Purpose:
 *
 */
typedef struct _field_meter_s {
    uint32              peak_kbits_sec;     /* Peak meter rate in Kb/sec     */
    uint32              peak_kbits_burst;   /* Peak burst size in Kbits      */
    uint32              commit_kbits_sec;   /* Committed meter rate Kb/sec   */
    uint32              commit_kbits_burst; /* Committed burst size in Kbits */
    uint32              index;              /* ID of meter pair (FB:0-63)    */
    uint8               entries;            /* Number of entries using meter */
    uint8               inst_flg;           /* Installed Flag                */
} _field_meter_t;


/*
 * Typedef:
 *     _field_range_t
 * Purpose:
 *     Internal management of Range Checkers. There are two styles or range
 *     checkers, the Firebolt style that only chooses source or destination
 *     port, without any sense of TCP vs. UDP or inverting. This style writes
 *     into the FP_RANGE_CHECK table. The Easyrider style is able to specify
 *     TCP vs. UDP.
 *     The choice of styles is based on the user supplied flags.
 *     If a Firebolt style checker is sufficient, that will be used. If an
 *     Easyrider style checker is required then that will be used.
 *
 */
typedef struct _field_range_s {
    uint32                 flags;
    bcm_field_range_t      rid;
    bcm_l4_port_t          min, max;
    int                    hw_index;
    uint8                  style;        /* Simple (FB) or more complex (ER) */
    struct _field_range_s *next;
} _field_range_t;

/*
 * Typedef:
 *     _field_entry_t
 * Purpose:
 *     This is the physical entry structure, hidden behind the logical
 *     bcm_field_entry_t handle.
 * Notes:
 *     Entries are stored in linked list in the under a slice's _field_slice_t
 *     structure.
 *
 *     Each entry can use 0 or 1 counters. Multiple entries may use the
 *     same counter. The only requirement is that the counter be within
 *     the same slice as the entry.
 *
 *     Similarly each entry can use 0 or 1 meter. Multiple entries may use
 *     the same meter. The only requirement is that the meter be within
 *     the same slice as the entry.
 */

typedef struct _field_entry_s {
    int                    unit;       /* BCM unit ID                        */
    bcm_field_entry_t      eid;        /* BCM unit unique entry identifier   */
    bcm_field_group_t      gid;      /* BCM group ideentifier */
    int                    prio;       /* Entry priority                     */
    uint16                 slice_idx;  /* TCAM index w/in slice (0-127 FB)   */
    uint8                  ext;        /* 0 = internal, 1 = external         */
    uint8                  dirty;   /* Dirty Flag                      */
    uint8                  color_indep;/* L3 switch actions color independent*/
    _field_tcam_t          tcam;       /* Fields to be written into FP_TCAM  */
    _field_action_t        *actions;   /* linked list of actions for entry   */
    _field_counter_t       *counter;   /* counter data, if used              */
    uint32                 mode_ctr;   /* BCM_FIELD_COUNTER_MODE_xxx         */
    _field_meter_t         *meter;     /* meter data, if used                */
    uint32                 mode_mtr;   /* flow, trTCM blind, srTCM etc.      */
    _field_slice_t         *fs;        /* Slice where entry lives            */
    drv_cfp_entry_t     drv_entry; /* kram ??? */
    struct _field_entry_s  *next;
} _field_entry_t;

/*
 * Prototypes of Field Processor utility funtions
 */
extern _field_control_t *_robo_field_control_find(int unit);
extern _field_group_t   *_robo_field_group_find(int unit, bcm_field_group_t group);
extern _field_entry_t   *_robo_field_entry_find(int unit, bcm_field_entry_t entry,
                                       int slice);
extern int _robo_field_filter_enable_set(int unit, uint32 state);
extern int  _robo_field_qual_info_create(bcm_field_qualify_t qid, soc_robo_mem_t mem,
                                    soc_robo_field_t fpf, int offset, int width,
                                    _qual_info_t **fq_p);
extern int  _robo_field_qual_list_copy(_qual_info_t **fq_dst,
                                  const _qual_info_t *fq_src);
extern void _robo_field_qual_list_destroy(_qual_info_t **f_qual);
extern int _robo_field_qual_value_set(int unit, bcm_field_qualify_t qi, _field_entry_t *f_ent,
                                 uint32 *p_data, uint32 *p_mask);
extern int _robo_field_qual_value32_set(int unit, bcm_field_qualify_t qi,
                                   _field_entry_t *f_ent,
                                   uint32 data, uint32 mask);

extern int _robo_field_qset_union(const bcm_field_qset_t *qset1,
                             const bcm_field_qset_t *qset2,
                             bcm_field_qset_t *qset_union);
extern int _robo_field_in_odd_slice(_field_entry_t *f_ent);
extern bcm_field_qualify_t _robo_field_qual_next(bcm_field_qset_t qset,
                                            bcm_field_qualify_t qual);
extern bcm_field_qualify_t _robo_field_qual_first(bcm_field_qset_t qset);

extern int _robo_field_qset_is_subset(const bcm_field_qset_t *qset1,
                                 const bcm_field_qset_t *qset2);
extern bcm_field_qset_t _robo_field_qset_diff(const bcm_field_qset_t qset_1,
                                         const bcm_field_qset_t qset_2);
extern int _robo_field_qset_is_empty(bcm_field_qset_t qset);

extern int _robo_bcm_field_setup_post_ethertype_udf(int unit,
                                               bcm_field_udf_t *udf_id);
extern int _robo_field_entry_tcam_idx_get(_field_control_t *fc,
                                     _field_entry_t *f_ent);


#ifdef BROADCOM_DEBUG
extern void _robo_field_selcode_dump(int unit, char *prefix, _field_sel_t sel_codes,
                                char *suffix) ;
extern void _robo_field_qset_debug(bcm_field_qset_t qset);
extern void _robo_field_qset_dump(char *prefix, bcm_field_qset_t qset, char* suffix);
extern void _robo_field_qual_info_dump(const _qual_info_t *qinf);
extern void _robo_field_qual_info_tbl_dump(uint32 size, _qual_info_t *qi_tbl[],
                                      char *tbl_name);
#endif /* DEBUG */

/* #endif */  /* BCM_FIELD_SUPPORT */
#endif  /* !_BCM_INT_FIELD_H */
