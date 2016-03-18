/*
 * $Id: field.h,v 1.187.2.17.2.2 Broadcom SDK $
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
#include <soc/profile_mem.h>
#include <soc/er_tcam.h>

#include <sal/core/sync.h>

#ifdef BCM_FIELD_SUPPORT

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

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#define _FP_INVALID_INDEX            (-1)

/* 64 bit software counter collection bucket. */
#define _FP_64_COUNTER_BUCKET  512 


/* Maximum number of paired slices.*/
#define _FP_PAIR_MAX   (3)

/*
 * Meter setting limits.
 */
#define BCM_FIELD_METER_KBITS_BURST_MAX   (0x4000)             /* 16Mb       */
#define BCM_FIELD_METER_KBITS_RATE_MAX    (64 * (1 << 19) - 1) /* 32Gb - 1Kb */

/* 
 * Meter mode. 
 */
#define _BCM_FIELD_METER_BOTH (BCM_FIELD_METER_PEAK | \
                               BCM_FIELD_METER_COMMITTED)

/* Value for control->tcam_ext_numb that indicates there is no external TCAM. */
#define FP_EXT_TCAM_NONE _FP_INVALID_INDEX

/*
 * Initial group IDs and entry IDs.
 */
#define _FP_ID_BASE 1
#define _FP_ID_MAX  (0x1000000)

/*
 * Meter state for a structure.
 *   Dirty == memory differs from hardware
 *   Clean == memory matches hardware
 */
#define _FIELD_METER_CLEAN                    (0)
#define _FIELD_METER_COMMITTED_DIRTY     (1 << 0) 
#define _FIELD_METER_PEAK_DIRTY          (1 << 1) 
#define _FIELD_METER_DIRTY               (_FIELD_METER_COMMITTED_DIRTY | \
                                          _FIELD_METER_PEAK_DIRTY)

/*
 * Action flags. 
 */
/* Action valid. */
#define _FP_ACTION_VALID              (1 << 0)
/* Reinstall is required. */
#define _FP_ACTION_DIRTY              (1 << 1)  
/* Remove action from hw. */
#define _FP_ACTION_RESOURCE_FREE      (1 << 2)
/* Action was updated free previously used hw resources. */
#define _FP_ACTION_OLD_RESOURCE_FREE  (1 << 3)
                                      
#define _FP_ACTION_HW_FREE (_FP_ACTION_RESOURCE_FREE | \
                            _FP_ACTION_OLD_RESOURCE_FREE)
/* 
 * Maximum number of meter pools
 *     Should change when new chips are defined
 */
#define _FIELD_MAX_METER_POOLS 16

/*
 * Internal version of qset add that will allow Data[01] to be set. 
 */
#define BCM_FIELD_QSET_ADD_INTERNAL(qset, q)  SHR_BITSET(((qset).w), (q))

/*
 * FPFx field select code macros
 *
 * These are for resolving if the select code has meaning or is really a don't
 * care.
 */

#define _FP_SELCODE_DONT_CARE (-1)
#define _FP_SELCODE_DONT_USE  (-2)

#define _FP_SELCODE_INVALIDATE(selcode) \
     ((selcode) = _FP_SELCODE_DONT_CARE);

#define _FP_SELCODE_IS_VALID(selcode) \
     (((selcode) == _FP_SELCODE_DONT_CARE || \
       (selcode) == _FP_SELCODE_DONT_USE) ? 0 : 1)

#define _FP_SELCODE_IS_INVALID(selcode) \
     (!_FP_SELCODE_IS_VALID(selcode))

#define _FP_MAX_FPF_BREAK(_selcode, _stage_fc, _fpf_id) \
      if (_FP_SELCODE_IS_VALID(_selcode))  {           \
         if (NULL == (_stage_fc->_field_table_fpf ## _fpf_id[_selcode])) { \
             break; \
         } \
      }

#define _FP_SELCODE_SET(_selcode_qset, _selcode) \
    do {                                         \
        _selcode_qset->fpf0 = _selcode;          \
        _selcode_qset->fpf1 = _selcode;          \
        _selcode_qset->fpf2 = _selcode;          \
        _selcode_qset->fpf3 = _selcode;          \
        _selcode_qset->fpf4 = _selcode;          \
    } while (0)

/*
 * FPF2 code that indicates using TCAM mappings that require delayed
 * qualification because they are dependent on PacketFormat.
 */
#define _FP_DELAYED_QUAL_FPF2   0x10     /* 5'b10000 */
#define _FP_DELAYED_QUAL_FPF3   0x08     /* 4'b1000  */

#define _FP_IS_DELAYED_QUALIFICATION(u_, e_)                   \
  ((SOC_IS_EASYRIDER(u_) &&                                    \
   (e_)->group->sel_codes[0].fpf2 == _FP_DELAYED_QUAL_FPF2)    \
      ? TRUE : FALSE)

/* FPF1 qualifiers in FB 2 and Raven that appear after the discontinuity in the selcodes */
#define _FP_DELAYED_QUAL_FPF1   0xC

/* 
 * Macros for packing and unpacking L3 information for field actions 
 * into and out of a uint32 value.
 */

#define MAX_CNT_BITS 10

#define _FP_L3_ACTION_PACK_ECMP(ecmp_ptr, ecmp_cnt) \
    ( 0x80000000 | ((ecmp_ptr) << MAX_CNT_BITS) | (ecmp_cnt) )
#define _FP_L3_ACTION_UNPACK_ECMP_MODE(cookie) \
    ( (cookie) >> 31 )
#define _FP_L3_ACTION_UNPACK_ECMP_PTR(cookie) \
    ( ((cookie) & 0x7fffffff) >> MAX_CNT_BITS )
#define _FP_L3_ACTION_UNPACK_ECMP_CNT(cookie) \
    ((cookie) & ~(0xffffffff << MAX_CNT_BITS))

#define _FP_L3_ACTION_PACK_NEXT_HOP(next_hop) \
    ( (next_hop) & 0x7fffffff )
#define _FP_L3_ACTION_UNPACK_NEXT_HOP(cookie) \
    ( (cookie) & 0x7fffffff )

/*
 * Macro that will work for chain mask. It's also the maximum allowable
 * value for the chain index.
 */
#define _FP_ER_CHAIN_IDX_WIDTH 14
#define _FP_ER_CHAIN_IDX_MAX ((1 << _FP_ER_CHAIN_IDX_WIDTH) - 1)

#define _FP_FB_CHAIN_IDX_WIDTH 8
#define _FP_FB_CHAIN_IDX_MAX ((1 << _FP_FB_CHAIN_IDX_WIDTH) - 1)

#define _FP_MAX_ENTRY_WIDTH   4 /* Maximum field entry width. */

/*
 * Macros for _field_entry_find() to specify which of the potentially 3
 * (triple-wide) entries to find.
 */
#define _FP_SLICE_PRIMARY     0 /* Base   slice for group */
#define _FP_SLICE_SECONDARY   1 /* Base+1 slice for group */
#define _FP_SLICE_TERTIARY    2 /* Base+2 slice for group */

#define _FP_HAS_EXT_TCAM(unit) (FALSE)
  

/*
 * Macro:
 *     _FIELD_ENTRY_IS_EXT
 * Purpose:
 *     Determine if a group's entries are stored in an External TCAM.
 * Parameters:
 *     fg - pointer to entry's field group struct
 */
#define _FIELD_ENTRY_IS_EXT(_fg_) ((_fg_)->flags & _FP_EXTERNAL_SLICE_GROUP)

/* 
 * Macro: _BCM_FIELD_ENTRIES_IN_SLICE
 * 
 * Purpose:
 *     Returns number of entries in a slice, depending on mode
 *
 * Parameters:
 *     fg - pointer to entry's field group struct
 *     fs - pointer to entry's field slice struct
 *     result - return value
 */
#define _BCM_FIELD_ENTRIES_IN_SLICE(_fg_, _fs_, _result_)     \
    if ((_fg_)->flags & _FP_INTRASLICE_DOUBLEWIDE_GROUP) {    \
        (_result_) = ((_fs_)->entry_count >> 1);              \
    } else {                                                  \
        (_result_) = (_fs_)->entry_count;                     \
    }

/*
 * Macro:
 *     _FIELD_ENTRY_MOVE_ACROSS_SLICES
 * Purpose:
 *     Check if the entry is moving across slices
 * Parameters:
 *     index1 - size of tcam
 *     index2 - index of entry in tcam [0..index1-1]
 *     index3 - amount to be moved
 */
#define _FIELD_ENTRY_MOVE_ACROSS_SLICES(index1, index2, index3) \
    ((((int)(index2 + index3) < 0) || ((index2 + index3) >= index1)) ? 1 : 0)


/*
 * Macro:_FP_QUAL_INFO_SET
 *
 * Purpose:
 *    Set the fields of a _qual_info_t struct.
 */
#define _FP_QUAL_INFO_SET(qual, field, off, wid, fq)  \
    (fq)->qid    = (qual);                                    \
    (fq)->fpf    = (field);                                   \
    (fq)->offset = (off);                                     \
    (fq)->width  = (wid);                                     \
    (fq)->next   = NULL;
/*
 * Macro:
 *     _FIELD_SELCODE_CLEAR (internal)
 * Purpose:
 *     Set all fields of a field select code to invalid
 * Parameters:
 *     selcode - _field_selcode_t struct
 */

#define _FIELD_SELCODE_CLEAR(unit, selcode)      \
    sal_memset(&selcode, _FP_SELCODE_DONT_CARE, sizeof(struct _field_sel_s));

#define _FIELD_FB_SELCODE_CLEAR(selcode)         \
        (selcode).fpf1 = _FP_SELCODE_DONT_CARE;    \
        (selcode).fpf2 = _FP_SELCODE_DONT_CARE;    \
        (selcode).fpf3 = _FP_SELCODE_DONT_CARE;    \
        (selcode).fpf4 = _FP_SELCODE_DONT_CARE;
/*
 * Macro:
 *     _FIELD_NEED_I_WRITE
 * Purpose:
 *     Test if it is necessary to write to "I" version of FP_PORT_FIELD_SEL
 *     table.
 * Parameters:
 *     _u_ - BCM device number
 *     _p_ - port
 *     _m_ - Memory (usually IFP_PORT_FIELD_SEL)
 */
#define _FIELD_NEED_I_WRITE(_u_, _p_, _m_) \
     ((SOC_MEM_IS_VALID((_u_), (_m_))) && \
      (IS_XE_PORT((_u_), (_p_)) || IS_ST_PORT((_u_),(_p_)) || \
       IS_CPU_PORT((_u_),(_p_))))


/* Pipeline stages for packet processing. */
#define _BCM_FIELD_STAGE_INGRESS               (0)  
#define _BCM_FIELD_STAGE_EXTERNAL              (3)
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
#define _BCM_FIELD_STAGE_LOOKUP                (1)  
#define _BCM_FIELD_STAGE_EGRESS                (2)  

/* Egress FP slice modes. */
#define  _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2      (0x0)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3      (0x1)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE         (0x2)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3_ANY  (0x3)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY  (0x4)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6   (0x5)


/* Egress FP slice v6 key  modes. */
#define  _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6         (0x0)
#define  _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_DIP6         (0x1)
#define  _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64   (0x2)



/* Egress FP possible keys. */
#define  _BCM_FIELD_EFP_KEY1                    (0x0)
#define  _BCM_FIELD_EFP_KEY2                    (0x1)
#define  _BCM_FIELD_EFP_KEY3                    (0x2)
#define  _BCM_FIELD_EFP_KEY4                    (0x3)
#define  _BCM_FIELD_EFP_KEY_MAX _BCM_FIELD_EFP_KEY4
/* KEY_MATCH type field for EFP_TCAM encoding. for every key. */
#define  _BCM_FIELD_EFP_KEY1_MATCH_TYPE       (0x1)
#define  _BCM_FIELD_EFP_KEY2_MATCH_TYPE       (0x2)
#define  _BCM_FIELD_EFP_KEY2_KEY3_MATCH_TYPE  (0x3)
#define  _BCM_FIELD_EFP_KEY1_KEY4_MATCH_TYPE  (0x4)
#define  _BCM_FIELD_EFP_KEY4_MATCH_TYPE       (0x5)
#define  _BCM_FIELD_EFP_KEY2_KEY4_MATCH_TYPE  (0x6)
#if 1/*anzhen.zheng, 5/26/2008*/
typedef enum
{
  _BCM_FIELD_VFP_SLICE0= 0x1 , /*VFP slice 0*/
  _BCM_FIELD_VFP_SLICE1 = 0x2 , /*VFP slice 1*/
  _BCM_FIELD_VFP_SLICE2 = 0x4 , /*VFP slice 2*/
  _BCM_FIELD_VFP_SLICE3  =0x8   /*VFP slice 3*/
}_BCM_FIELD_VFP_SLICE_T;

typedef enum
{
  _BCM_FIELD_EFP_SLICE0 = 0x1 , /*EFP slice 0*/
  _BCM_FIELD_EFP_SLICE1 = 0x2 , /*EFP slice 1*/
  _BCM_FIELD_EFP_SLICE2 = 0x4 , /*EFP slice 2*/
  _BCM_FIELD_EFP_SLICE3  =0x8   /*EFP slice 3*/
}_BCM_FIELD_EFP_SLICE_T;

typedef enum
{
  _BCM_FIELD_IFP_SLICE0 = 0x1 , /*IFP slice 0*/
  _BCM_FIELD_IFP_SLICE1 = 0x2 , /*IFP slice 1*/
  _BCM_FIELD_IFP_SLICE2 = 0x4 , /*IFP slice 2*/
  _BCM_FIELD_IFP_SLICE3  =0x8 ,  /*IFP slice 3*/
  _BCM_FIELD_IFP_SLICE4  =0x10,
  _BCM_FIELD_IFP_SLICE5  =0x20,
  _BCM_FIELD_IFP_SLICE6  =0x40,
  _BCM_FIELD_IFP_SLICE7  =0x80,
  _BCM_FIELD_IFP_SLICE8  =0x100,
  _BCM_FIELD_IFP_SLICE9  =0x200,
  _BCM_FIELD_IFP_SLICE10  =0x400,
  _BCM_FIELD_IFP_SLICE11 =0x800,
  _BCM_FIELD_IFP_SLICE12 =0x1000,
  _BCM_FIELD_IFP_SLICE13  =0x2000,
  _BCM_FIELD_IFP_SLICE14  =0x4000,
  _BCM_FIELD_IFP_SLICE15 = 0x8000
}_BCM_FIELD_IFP_SLICE_T;
#endif

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */ 

#define _BCM_FIELD_STAGE_STRINGS \
    {"Ingress Stage",            \
     "Lookup Stage",             \
     "Egress Stage"}               

#define FPF_SZ_MAX                           (32)

#define _FP_HASH_SZ(_fc_)   \
           (((_fc_)->flags & _FP_EXTERNAL_PRESENT) ? (0x1000) : (0x100))
#define _FP_HASH_INDEX_MASK(_fc_) (_FP_HASH_SZ(_fc_) - 1)

#define _FP_HASH_INSERT(_hash_ptr_, _inserted_ptr_, _index_)    \
           do {                                                 \
              (_inserted_ptr_)->next = (_hash_ptr_)[(_index_)]; \
              (_hash_ptr_)[(_index_)] = (_inserted_ptr_);       \
           } while (0)

#define _FP_HASH_REMOVE(_hash_ptr_, _entry_type_, _removed_ptr_, _index_)  \
           do {                                                    \
               _entry_type_ *_prev_ent_ = NULL;                    \
               _entry_type_ *_lkup_ent_ = (_hash_ptr_)[(_index_)]; \
               while (NULL != _lkup_ent_) {                        \
                   if (_lkup_ent_ != (_removed_ptr_))  {           \
                       _prev_ent_ = _lkup_ent_;                    \
                       _lkup_ent_ = _lkup_ent_->next;              \
                       continue;                                   \
                   }                                               \
                   if (_prev_ent_!= NULL) {                        \
                       _prev_ent_->next = (_removed_ptr_)->next;   \
                   } else {                                        \
                       (_hash_ptr_)[(_index_)] = (_removed_ptr_)->next; \
                   }                                               \
                   break;                                          \
               }                                                   \
           } while (0)

#define _FP_ACTION_IS_COLOR_BASED(action)               \
     ((bcmFieldActionRpDrop <= (action) &&              \
      (action) <= bcmFieldActionYpPrioIntCancel) ||  \
      (bcmFieldActionGpDrop <= (action)                 \
       && (action) <= bcmFieldActionGpPrioIntCancel))

#define _BCM_FP_QUALIFY_INNER_IP_HEADER(_qset_)                                \
           ((BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyInnerSrcIp))      || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyInnerDstIp))      || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyInnerSrcIp6))     || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyInnerDstIp6))     || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyInnerSrcIp6High)) || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyInnerDstIp6High)) || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyInnerTtl))        || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyInnerDSCP))       || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyInnerIpProtocol)) || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyInnerIpFrag)))

#define _BCM_FP_QUALIFY_OUTER_IP_HEADER(_qset_)                                \
           ((BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifySrcIp))           || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyDstIp))           || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifySrcIp6))          || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyDstIp6))          || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifySrcIp6High))      || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyDstIp6High))      || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyTtl))             || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyDSCP))            || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyIpProtocol))      || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyIp6FlowLabel))    || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyIp6HopLimit))     || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyIpProtocolCommon))|| \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifySrcIpEqualDstIp)) || \
            (BCM_FIELD_QSET_TEST((_qset_), bcmFieldQualifyIpFrag)))
                                   
/*
 * Structure for Group auto-expansion feature
 *     S/W copy of FP_SLICE_MAP
 */

typedef struct _field_virtual_map_s {
    int valid;
    int physical_slice;
    int virtual_group;
    int priority;
    int flags;
} _field_virtual_map_t;

/*
 * Typedef:
 *     _stage_id_t
 * Purpose:
 *     Holds format info for pipline stage id. 
 */
typedef int _field_stage_id_t;

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
    soc_field_t            fpf;     /* FPFx field choice            */
    int                    offset;  /* Offset within FPFx field     */
    int                    width;   /* Qual width within FPFx field */
    struct _qual_info_s    *next;
} _qual_info_t;

/* Typedef:
 *     _field_fpf_info_t
 * Purpose:
 *     Hardware memory details of field qualifier 
 */
typedef struct _field_fpf_info_s {
    _qual_info_t      **qual_table;
    bcm_field_qset_t  *sel_table;
    soc_field_t       field;
} _field_fpf_info_t;

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
 * Struct:
 *     _field_counter64_collect_s
 * Purpose:
 *     Holds the accumulated count of FP Counters 
 *         Useful for wrap around and movement.
 *     This is used when h/w counter width is > 32 bits
 *         e.g. Bradley, Triumph Byte counters
 */
typedef struct _field_counter64_collect_s {
    uint64 accumulated_counter;
    uint64 last_hw_value;
} _field_counter64_collect_t;

/*
 * Typedef:
 *     _field_meter_bmp_t
 * Purpose:
 *     Meter bit map for tracking allocation state of group's meter pairs.
 */
typedef struct _field_meter_bmp_s {
    SHR_BITDCL  *w;
} _field_meter_bmp_t;

#define _FP_METER_BMP_FREE(bmp, size)   sal_free((bmp).w)
#define _FP_METER_BMP_ADD(bmp, mtr)     SHR_BITSET(((bmp).w), (mtr))
#define _FP_METER_BMP_REMOVE(bmp, mtr)  SHR_BITCLR(((bmp).w), (mtr))
#define _FP_METER_BMP_TEST(bmp, mtr)    SHR_BITGET(((bmp).w), (mtr))

#define _FP_VIRTUAL_MAP_SIZE           (16)


/*
 * struct:
 *     _field_meter_pool_s
 * Purpose:
 *     Global meter pool
 * Note:
 *     Note that the 2nd parameter is slice_id and not group_id. This 
 *     is because more than one group can share the same slice (pbmp based
 *     groups)
 *
 *     Due to this, it needs to be enforced that for an entry in a group
 *     which is not in the group's first slice (due to group auto-expansion/
 *     virtual groups), slice_id denotes the entry's group's first slice.
 */
typedef struct _field_global_meter_pool_s {
    uint8              level;       /* Level meter pool attached.            */
    int                slice_id;    /* First slice which uses this meter pool*/
    uint16             size;        /* Number of meters in pool.             */
    uint16             free_meters; /* Number of free meters                 */
    _field_meter_bmp_t meter_bmp;   /* Meter usage bitmap                    */
} _field_meter_pool_t;


/*
 * DATA qualifiers section. 
 */

/* Maximum packet depth data qualifier can qualify */
#define _FP_DATA_OFFSET_MAX      (128)

/* Maximum packet depth data qualifier can qualify */
#define _FP_DATA_WORDS_COUNT         (8)

/* Number of UDF_OFFSETS allocated for ethertype based qualifiers. */
#define _FP_DATA_ETHERTYPE_MAX              (8)
/* Number of UDF_OFFSETS allocated for IPv4 protocol based qualifiers. */
#define _FP_DATA_IP_PROTOCOL_MAX            (2)
/* Number of UDF_OFFSETS allocated for IPv6 next header based qualifiers. */
#define _FP_DATA_NEXT_HEADER_MAX            (2)

/* Hw specific config.*/
#define _FP_DATA_DATA0_WORD_MIN     (0)
#define _FP_DATA_DATA0_WORD_MAX     (3)
#define _FP_DATA_DATA1_WORD_MIN     (4)
#define _FP_DATA_DATA1_WORD_MAX     (7)

/* Ethertype match L2 format. */
#define  _BCM_FIELD_DATA_FORMAT_ETHERTYPE (0x3)

/* Ip protocol match . */
#define  _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL0 (0x1)
#define  _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL1 (0x2)
#define  _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL0 (0x3)
#define  _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL1 (0x4)

/* Packet content (data) qualification object flags. */
#define _BCM_FIELD_DATA_QUALIFIER_FLAGS                        \
     (BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST ||    \
      BCM_FIELD_DATA_QUALIFIER_OFFSET_IP6_EXTENSIONS_ADJUST || \
      BCM_FIELD_DATA_QUALIFIER_OFFSET_GRE_OPTIONS_ADJUST)


/*
 * Typedef:
 *     _field_data_qualifier_s
 * Purpose:
 *     Data qualifiers description structure.
 */
typedef struct _field_data_qualifier_s {
    int    qid;                     /* Qualifier id.                     */
    bcm_field_udf_spec_t *spec;     /* Hw adjusted offsets array.        */
    bcm_field_data_offset_base_t offset_base; /* Offset base adjustment. */
    int    offset;                  /* Master word offset.               */
    uint8  byte_offset;             /* Data offset inside the word.      */
    uint32 hw_bmap;                 /* Allocated hw words.               */
    uint32 flags;                   /* Offset adjustment flags.          */
    uint8  word_count;              /* Number of hardware words required.*/
    int    length;                  /* Matched data byte length.         */
    struct _field_data_qualifier_s *next;/* Next installed  qualifier.   */
} _field_data_qualifier_t, *_field_data_qualifier_p;

/*
 * Typedef:
 *     _field_data_ethertype_s
 * Purpose:
 *     Ethertype based data qualifiers description structure.
 */
typedef struct _field_data_ethertype_s {
    int ref_count;                  /* Reference count.        */
    uint16 l2;                      /* Packet l2 format.       */
    uint16 vlan_tag;                /* Packet vlan tag format. */
    bcm_port_ethertype_t ethertype; /* Ether type value.       */
    int relative_offset;            /* Packet byte offset      */
                                    /* relative to qualifier   */
                                    /* byte offset.            */
} _field_data_ethertype_t;

/*
 * Typedef:
 *     _field_data_protocol_s
 * Purpose:
 *     Protocol based data qualifiers description structure.
 */
typedef struct _field_data_protocol_s {
    int ip4_ref_count;    /* Ip4 Reference count.    */
    int ip6_ref_count;    /* Ip6 Reference count.    */
    uint32 flags;         /* Ip4/Ip6 flags.          */
    uint8 ip;             /* Ip protocol id.         */
    uint16 l2;            /* Packet l2 format.       */
    uint16 vlan_tag;      /* Packet vlan tag format. */
    int relative_offset;  /* Packet byte offset      */
                          /* relative to qualifier   */
                          /* byte offset.            */
} _field_data_protocol_t;

/*
 * Typedef:
 *     _field_data_control_s
 * Purpose:
 *     Field data qualifiers control structucture.     
 *        
 */
typedef struct _field_data_control_s {
   uint32                  usage_bmap; /* Offset usage bitmap.         */
   _field_data_qualifier_p data_qual; /* list of data qualifiers.      */
                                 /* Ethertype based qualifiers.        */
   _field_data_ethertype_t etype[_FP_DATA_ETHERTYPE_MAX];
                                 /* IP protocol based qualifiers.      */
   _field_data_protocol_t  ip[_FP_DATA_IP_PROTOCOL_MAX];
} _field_data_control_t;


/*
 * Stage flags. 
 */

/* Separate packet byte counters. */
#define _FP_STAGE_SEPARATE_PACKET_BYTE_COUNTERS (1 << 0)

/* Global meter pools . */
#define _FP_STAGE_GLOBAL_METER_POOLS            (1 << 1)

/* Global counters. */
#define _FP_STAGE_GLOBAL_COUNTERS               (1 << 2)


/*
 * Typedef:
 *     _field_stage_t
 * Purpose:
 *     Pipeline stage field processor information.
 */
typedef struct _field_stage_s {
    _field_stage_id_t      stage_id;        /* Pipeline stage id.           */
    uint8                  flags;           /* Stage flags.                 */
    int                    tcam_sz;         /* Number of entries in TCAM.   */
    int                    tcam_slices;     /* Number of internal slices.   */
    struct _field_slice_s  *slices;         /* Array of slices.*/
    struct _field_range_s  *ranges;	    /* List of all ranges allocated.*/
    uint32	       	   range_id;        /* Seed ID for range creation.  */
                                        /* Virtual map for slice extension */
                                        /* and group priority management.  */
    _field_virtual_map_t fp_virtual_map[_FP_VIRTUAL_MAP_SIZE]; 
    
    /* FPF tables */
    _qual_info_t *_field_table_fpf0[FPF_SZ_MAX];
    _qual_info_t *_field_table_fpf1[FPF_SZ_MAX];
    _qual_info_t *_field_table_fpf2[FPF_SZ_MAX];
    _qual_info_t *_field_table_fpf3[FPF_SZ_MAX];
    _qual_info_t *_field_table_fpf4[FPF_SZ_MAX];
    _qual_info_t *_field_table_extn[FPF_SZ_MAX];
    _qual_info_t *_field_table_doublewide_fpf[FPF_SZ_MAX];
    _qual_info_t *_field_table_fixed[1];

    bcm_field_qset_t *_field_sel_f0;
    bcm_field_qset_t *_field_sel_f1;
    bcm_field_qset_t *_field_sel_f2;
    bcm_field_qset_t *_field_sel_f3;
    bcm_field_qset_t *_field_sel_f4;
    bcm_field_qset_t *_field_sel_extn;
    bcm_field_qset_t *_field_sel_doublewide;
    bcm_field_qset_t *_field_sel_fixed;
    bcm_field_qset_t _field_supported_qset;

    int counter_collect_table; /* Used for counter collection in chunks */
    int counter_collect_index; /* Used for counter collection in chunks */
    
    int num_meter_pools;
    _field_meter_pool_t *meter_pool[_FIELD_MAX_METER_POOLS];

    _field_counter32_collect_t *_field_x32_counters;
                              /* X pipeline 32 bit counter collect      */
#if defined(BCM_EASYRIDER_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
    _field_counter32_collect_t *_field_ext_counters; 
                              /* External counter collect */
#endif /* BCM_EASYRIDER_SUPPORT || BCM_TRIUMPH_SUPPORT */

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
    _field_counter64_collect_t *_field_x64_counters; 
                              /* X pipeline 64 bit counter collect */
    _field_counter64_collect_t *_field_y64_counters;
                              /* Y pipeline 64 bit counter collect  */
#endif /* BCM_BRADLEY_SUPPORT || BCM_SCORPION_SUPPORT */

#if defined(BCM_SCORPION_SUPPORT)
    _field_counter32_collect_t *_field_y32_counters;
                              /* Y pipeline packet counter collect */
#endif /* BCM_SCORPION_SUPPORT */

    soc_profile_mem_t redirect_profile;     /* Redirect action memory profile.*/
    soc_profile_mem_t ext_act_profile;      /* Action profile for external.   */
    _field_data_control_t *data_ctrl;       /* Data qualifiers control.       */

    struct _field_stage_s *next;            /* Next pipeline FP stage.        */
} _field_stage_t;

#define _FIELD_FIRST_MEMORY_COUNTERS                 (1 << 0)
#define _FIELD_SECOND_MEMORY_COUNTERS                (1 << 1)

#define FILL_FPF_TABLE(_fpf_info, _qid, _offset, _width, _code)        \
    BCM_IF_ERROR_RETURN                                                \
        (_field_qual_add((_fpf_info), (_qid), (_offset), (_width), (_code)))

/* Generic memory allocation routine. */
#define _FP_XGS3_ALLOC(_ptr_,_size_,_descr_)                 \
            do {                                             \
                if (NULL == (_ptr_)) {                       \
                    FP_ERR(("FP Error: Allocation size %s\n", (_size_))); \
                   (_ptr_) = sal_alloc((_size_), (_descr_)); \
                }                                            \
                if((_ptr_) != NULL) {                        \
                    sal_memset((_ptr_), 0, (_size_));        \
                }  else {                                    \
                    FP_ERR(("FP Error: Allocation failure %s\n", (_descr_))); \
                }                                          \
            } while (0)

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
    uint8                  user_num;  /* Which user field in UDFn        */
} _field_udf_t;

/*
 * Typedef:
 *     _field_sel_t
 * Purpose:
 */
typedef struct _field_sel_s {
    int               fpf0; /* field(s) select 2-bit code */
    int               fpf1; /* field(s) select 3-bit code */
    int               fpf2; /* field(s) select 4-bit code */
    int               fpf3; /* field(s) select 3-bit code */
    int               fpf4; /* field(s) select 1-bit code */
    int               extn; /* field(s) select external   */
} _field_sel_t;

/*
 * Typedef:
 *     _field_tcam_int_t
 * Purpose:
 *     These are the fields that are written into or read from FP_TCAM_xxx.
 */
typedef struct _field_tcam_s {
#ifdef BCM_EASYRIDER_SUPPORT
    uint32                 tcp_flag;      /* TCP protocol flag */
#endif /* BCM_EASYRIDER_SUPPORT */
    uint32                 *key;
    uint32                 *key_hw;  /* Hardware replica */
    uint32                 *mask;
    uint32                 *mask_hw; /* Hardware replica */
    uint16                 key_size;
#ifdef BCM_FIREBOLT_SUPPORT /* Also for Raptor, 4-bits wide */
    uint32                 f4;            /* 5-bits wide, 56504_b0 only    */
    uint32                 f4_mask;       /* 5-bits wide, 56504_b0 only    */
#endif /* BCM_FIREBOLT_SUPPORT */

    uint8                  ip_type;       /* final encoding is 2-bits wide */
    uint8                  ip_type_mask;

    uint8                  higig;         /* 0/1 non-HiGig/HiGig           */
    uint8                  higig_mask;    /* 0/1 non-HiGig/HiGig           */
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    uint8                  drop;         /* 0/1 don't Drop/Drop            */
    uint8                  drop_mask;    /* 0/1 don't Drop/Drop            */
#endif /* BCM_FIREBOLT2_SUPPORT  || BCM_TRX_SUPPORT */
    uint32                 pkt_fmt_mask:2;   /* nearly always 0x3    */
    uint32                 chain:1;
    uint32                 chain_idx:11;
    uint32                 need_l3_switch:1;
    uint32                 l3_hit:1;
    uint32                 decap:3;
    uint32                 decap_mask:3;
    bcm_ip_t               sip;
    bcm_ip_t               sip_mask;
    bcm_ip_t               dip;
    bcm_ip_t               dip_mask;
    bcm_ip6_t              sip6;
    bcm_ip6_t              sip6_mask;
    uint32                 first_frag:1;
    uint32                 src_port_1:1;
    uint32                 dst_port_1:1;
    uint32                 src_port_2:1;
    uint32                 dst_port_2:1;
    uint32                 udf1;
    uint32                 protocol_id;
    uint32                 protocol_id_mask;
    uint32                 tcp_control;
    uint32                 tcp_control_mask;
} _field_tcam_t;

/*
 * Typedef:
 *     _field_tcam_mem_info_t
 * Purpose:
 *     TCAM memory name and chip specific field names. 
 */
typedef struct _field_tcam_mem_info_s{
    soc_mem_t      memory;     /* Tcam memory name.    */  
    soc_field_t    key_field;  /* Tcam key field name. */
    soc_field_t    mask_field; /* Tcam mask field name.*/
} _field_tcam_mem_info_t;

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_TRX_SUPPORT)
typedef struct _field_pbmp_s {
    bcm_pbmp_t data;
    bcm_pbmp_t mask;
} _field_pbmp_t;
#endif /* BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */

/*
 * Typedef:
 *     _field_counter_t
 * Purpose:
 *     Holds the counter parameters to be written into FP_POLICY_TABLE
 *     (Firebolt) or FP_INTERNAL (Easyrider).
 */
typedef struct _field_counter_s {
    int                    index;
    uint16                 entries;    /* Number of entries using counter */
} _field_counter_t;

/*
 * Typedef:
 *     _field_counter_bmp_t
 * Purpose:
 *     Counter bit map for tracking allocation state of slice's counter pairs.
 */
typedef struct _field_counter_bmp_s {
    SHR_BITDCL  *w;
} _field_counter_bmp_t;

#define _FP_COUNTER_BMP_FREE(bmp, size)   sal_free((bmp).w)
#define _FP_COUNTER_BMP_ADD(bmp, ctr)     SHR_BITSET(((bmp).w), (ctr))
#define _FP_COUNTER_BMP_REMOVE(bmp, ctr)  SHR_BITCLR(((bmp).w), (ctr))
#define _FP_COUNTER_BMP_TEST(bmp, ctr)    SHR_BITGET(((bmp).w), (ctr))


/* 
 * Structure for priority management 
 * Currently used only on External TCAM
 */
typedef struct _field_prio_mgmt_s {
    int prio;
    uint32 start_index;
    uint32 end_index;
    uint32 num_free_entries;
    struct _field_prio_mgmt_s *prev;
    struct _field_prio_mgmt_s *next;
} _field_prio_mgmt_t;

/* Different types of packet types for External TCAM */
#define EXT_NUM_PKT_TYPES 3
#define EXT_L2   0
#define EXT_IPV4 1
#define EXT_IPV6 2

/*
 * Different types of Databases in External TCAM (Triumph)
 * Each type of database is considered as a slice (in S/W)
 * The number of entries in each type of database is configurable
 * The field_slice_t.num_ext_entries denotes the number of entries
 *
 * See include/soc/er_tcam.h for the database types:
 * slice_numb    database type (soc_tcam_partition_type_t)   meaning
 *     0         TCAM_PARTITION_ACL_L2C                      ACL_144_L2
 *     1         TCAM_PARTITION_ACL_L2                       ACL_L2
 *     2         TCAM_PARTITION_ACL_IP4C                     ACL_144_IPV4
 *     3         TCAM_PARTITION_ACL_IP4                      ACL_IPV4
 *     4         TCAM_PARTITION_ACL_L2IP4                    ACL_L2_IPV4
 *     5         TCAM_PARTITION_ACL_IP6C                     ACL_144_IPV6
 *     6         TCAM_PARTITION_ACL_IP6S                     ACL_IPV6_SHORT
 *     7         TCAM_PARTITION_ACL_IP6F                     ACL_IPV6_FULL
 *     8         TCAM_PARTITION_ACL_L2IP6                    ACL_L2_IPV6
 */
#define EXT_ACL_144_L2     0
#define EXT_ACL_L2         1
#define EXT_ACL_144_IPV4   2
#define EXT_ACL_IPV4       3
#define EXT_ACL_L2_IPV4    4
#define EXT_ACL_144_IPV6   5
#define EXT_ACL_IPV6_SHORT 6
#define EXT_ACL_IPV6_FULL  7
#define EXT_ACL_L2_IPV6    8

extern soc_tcam_partition_type_t tcam_partitions[9];

/* Slice specific flags. */
#define _BCM_FIELD_SLICE_EXTERNAL                 (1 << 0)
#define _BCM_FIELD_SLICE_INTRASLICE_CAPABLE       (1 << 1)

#define _FP_INTRA_SLICE_PART_0        (0)
#define _FP_INTRA_SLICE_PART_1        (1)
#define _FP_INTRA_SLICE_PART_2        (2)
#define _FP_INTRA_SLICE_PART_3        (3)

#define _FP_INTER_SLICE_PART_0        (0)
#define _FP_INTER_SLICE_PART_1        (1)
#define _FP_INTER_SLICE_PART_2        (2)

/*
 * Typedef:
 *     _field_slice_t
 * Purpose:
 *     This has the fields specific to a hardware slice.
 * Notes:
 */
typedef struct _field_slice_s {
    uint8                  slice_number;  /* Hardware slice number.         */
    int                    start_tcam_idx;/* Slice first entry tcam index.  */
    int                    entry_count;   /* Number of entries in the slice.*/
    int                    free_count;    /* Number of free entries.        */
    int                    counters_count;/* Number of counters accessible. */
    int                    meters_count;  /* Number of meters accessible.   */

    _field_counter_bmp_t   counter_bmp;   /* Bitmap for counter allocation. */
    _field_meter_bmp_t     meter_bmp;     /* Bitmap for meter allocation.   */
    _field_stage_id_t      stage_id;      /* Pipeline stage slice belongs.  */
    bcm_pbmp_t             pbmp;          /* Ports in use by groups.        */
     
    struct _field_entry_s  **entries;     /* List of entries pointers.      */
    _field_prio_mgmt_t     *prio_mgmt;    /* Priority management of entries.*/

    uint8 pkt_type[EXT_NUM_PKT_TYPES];    /* Packet types supported 
                                             by this slice (aka database).  */

    bcm_pbmp_t ext_pbmp[EXT_NUM_PKT_TYPES]; /* Bitmap for each packet type. */

    struct _field_slice_s  *next;  /* Linked list for auto-expand of groups.*/
    struct _field_slice_s  *prev;  /* Linked list for auto-expand of groups.*/
    uint8                  slice_flags;   /* _BCM_FIELD_SLICE_XXX flags.    */
    uint8                  group_flags;   /* Intalled groups _FP_XXX_GROUP. */
	uint8                  doublewide_key_select; 
                                       	  /* Key selection for the          */
                                          /* intraslice double wide mode.   */

#if defined (BCM_WARM_BOOT_SUPPORT)
    _field_sel_t *selcodes;
#endif /* BCM_WARM_BOOT_SUPPORT */
} _field_slice_t;


/* Macro: _BCM_FIELD_SLICE_SIZE
 * Purpose:
 *        Given stage, slice get number of entries in the slice.
 */
#define _BCM_FIELD_SLICE_SIZE(_stage_fc_, _slice_)     \
       (((_stage_fc_)->slices + (_slice_))->entry_count)

/* Macro: _BCM_FIELD_TCAM_IDX_GET
 * Purpose:
 *        Given stage, slice,  entry offset in the slice 
 *        calculate entry offset in the tcam.
 * NOTE: Assuming slice number & offset are  valid 
 *       -> no error condition handling in this macro.
 */
#define _BCM_FIELD_TCAM_IDX_GET(_stage_fc_, _slice_, _slice_offset_)     \
       (((_stage_fc_)->slices + (_slice_))->start_tcam_idx + _slice_offset_)

/* Macro: _BCM_FIELD_SLICE_OFFSET_GET
 * Purpose:
 *        Given stage and tcam_index,
 *        calculate slice and entry offset in the slice. 
 * NOTE: Assuming tcam index is valid - 
 *       -> no error condition handling in this macro. 
 */
#define _BCM_FIELD_SLICE_OFFSET_GET(_stage_fc_, _tcam_idx_, _slice_, _offset_) \
       {                                                                       \
           int _idx_;                                                          \
           _field_slice_t *_fs_;                                               \
           for (_idx_ = 0; _idx_ < (_stage_fc_)->tcam_slices; _idx_++) {       \
              _fs_ = (_stage_fc_)->slices + _idx_;                             \
              if ((_tcam_idx_) < (_fs_->start_tcam_idx + _fs_->entry_count)) { \
                   (_slice_) = _idx_;                                          \
                   (_offset_) = (_tcam_idx_) - _fs_->start_tcam_idx;           \
                   break;                                                      \
              }                                                                \
           }                                                                   \
       }


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
    bcm_field_group_t      gid;            /* Opaque handle.                */
	int                    priority;       /* Field group priority.         */
    bcm_field_qset_t       qset;           /* This group's Qualifier Set.   */
    uint8                  flags;          /* Group configuration flags.    */
    _field_slice_t         *slices;        /* Pointer into slice array.     */
    bcm_pbmp_t             pbmp;           /* Ports in use this group.      */
    _field_sel_t           sel_codes[_FP_MAX_ENTRY_WIDTH]; 
                                           /* Select codes for slice(s).    */
    _qual_info_t           *qual_list[_FP_MAX_ENTRY_WIDTH]; 
                                           /* Offset & width for qualifiers.*/
    bcm_field_qset_t       qset_hw[_FP_MAX_ENTRY_WIDTH];    
                                           /* Qset supported by slice(s).   */
    _field_stage_id_t      stage_id;       /* FP pipeline stage id.         */

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
    bcm_field_action_t     action;       /* action type                  */
    uint32                 param0;       /* Action specific parameter    */
    uint32                 param1;       /* Action specific parameter    */
    int                    hw_index;     /* Allocated hw resource index. */
    int                    old_index;    /* Hw resource to be freed, in  */ 
                                         /* case action parameters were  */ 
                                         /* modified.                    */
    uint8                  flags;        /* Field action flags.          */
    struct _field_action_s *next;
} _field_action_t;

#define _FP_RANGE_STYLE_FIREBOLT    1
#define _FP_RANGE_STYLE_EASYRIDER   2

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
 * Entry status flags. 
 */

/* Software entry differs from one in hardware. */
#define _FP_ENTRY_DIRTY                      (1 << 0)

/* Entry is in primary slice. */
#define _FP_ENTRY_PRIMARY                    (1 << 1)     

/* Entry is in secondary slice of wide-mode group. */
#define _FP_ENTRY_SECONDARY                  (1 << 2)

/* Entry is in tertiary slice of wide-mode group. */
#define _FP_ENTRY_TERTIARY                   (1 << 3)

/* Entry has an ingress Mirror-To-Port reserved. */
#define _FP_ENTRY_MTP_ING0                   (1 << 4)

/* Entry has an ingress 1 Mirror-To-Port reserved. */
#define _FP_ENTRY_MTP_ING1                   (1 << 5)

/* Entry has an egress Mirror-To-Port reserved. */
#define _FP_ENTRY_MTP_EGR0                   (1 << 6)

/* Entry has an egress 1 Mirror-To-Port reserved. */
#define _FP_ENTRY_MTP_EGR1                   (1 << 7)

/* Second part of double wide intraslice entry. */  
#define _FP_ENTRY_SECOND_HALF                (1 << 8)

/* Field entry installed in hw. */
#define _FP_ENTRY_INSTALLED                  (1 << 9)

/* Treat all packets as green. */
#define _FP_ENTRY_COLOR_INDEPENDENT          (1 << 10)

/* Meter installed in secondary slice . */
#define _FP_ENTRY_METER_IN_SECONDARY_SLICE   (1 << 11)

/* Counter installed in secondary slice . */
#define _FP_ENTRY_COUNTER_IN_SECONDARY_SLICE (1 << 12)
#if 1/*anzhen.zheng, 5/26/2008*/
#define _FP_ENTRY_METER_MAX_SHARED  2
#endif

/* Entry slice identification flags. */
#define _FP_ENTRY_SLICE_FLAGS (_FP_ENTRY_PRIMARY |  _FP_ENTRY_SECONDARY | \
                               _FP_ENTRY_TERTIARY)

/* Entry slice identification flags. */
#define _FP_ENTRY_MIRROR_ON  (_FP_ENTRY_MTP_ING0 |  _FP_ENTRY_MTP_ING1 | \
                               _FP_ENTRY_MTP_EGR0 | _FP_ENTRY_MTP_EGR1)


/*
 * Group status flags. 
 */

/* Group resides in a single slice. */
#define _FP_SINGLE_SLICE_GROUP          (1 << 0)     

/* Group resides in a two paired slices. */
#define _FP_DOUBLE_SLICE_GROUP          (1 << 1)

/* Group resides in three paired slices. */
#define _FP_TRIPLE_SLICE_GROUP          (1 << 2)

/* Group entries are double wide in each slice. */  
#define _FP_INTRASLICE_DOUBLEWIDE_GROUP (1 << 3)

/* Group resides in external slice. */  
#define _FP_EXTERNAL_SLICE_GROUP        (1 << 4)

/* 
 * Group has slice lookup enabled 
 *     This is default, unless it is disabled by call to
 *     bcm_field_group_enable_set with enable=0
 */
#define _FP_LOOKUP_ENABLED_GROUP        (1 << 5)

#define _FP_GROUP_STATUS_MASK        (_FP_SINGLE_SLICE_GROUP | \
                                      _FP_DOUBLE_SLICE_GROUP | \
                                      _FP_TRIPLE_SLICE_GROUP | \
                                      _FP_INTRASLICE_DOUBLEWIDE_GROUP )   

 

/* Internal DATA qualifiers. */
typedef enum _bcm_field_internal_qualify_e {
    _bcmFieldQualifyData0 = bcmFieldQualifyCount,/* [0x00] Data qualifier 0.  */
    _bcmFieldQualifyData1,                    /* [0x01] Data qualifier 1.     */
    _bcmFieldQualifyCount                     /* [0x02] Always last not used. */
} _bcm_field_internal_qualify_t;


/* Committed portion in sw doesn't match hw. */
#define _FP_POLICER_COMMITTED_DIRTY   (1 << 0) 

/* Peak portion in sw doesn't match hw. */
#define _FP_POLICER_PEAK_DIRTY        (1 << 1) 

/* Policer created through meter APIs.  */
#define _FP_POLICER_INTERNAL          (1 << 1) 

#define _FP_POLICER_DIRTY             (_FP_POLICER_COMMITTED_DIRTY | \
                                       _FP_POLICER_PEAK_DIRTY)

#define _FP_POLICER_LEVEL_COUNT       (2)
/*
 * Typedef:
 *     _field_policer_t
 * Purpose:
 *     This is the policer description structure. 
 *     Indexed by bcm_policer_t handle.
 */
typedef struct _field_policer_s {
    bcm_policer_t        pid;         /* Unique policer identifier.       */
    bcm_policer_config_t cfg;         /* API level policer configuration. */
    uint16               sw_ref_count;/* SW object use reference count.   */
    uint16               hw_ref_count;/* HW object use reference count.   */
    uint8                level;       /* Policer attachment level.        */
    uint8                pool_index;  /* Meter pool/slice policer resides.*/
    int                  hw_index;    /* HW index policer resides.        */
    uint32               hw_flags;    /* HW installation status flags.    */
    _field_stage_id_t    stage_id;    /* Attached entry stage id.         */
    struct _field_policer_s *next;    /* Policer lookup linked list.      */
}_field_policer_t;

/*
 * Typedef:
 *     _field_stat_t
 * Purpose:
 *     This is the statistics collection entity description structure.
 *     Indexed by int stat_id handle.
 *     
 */
typedef struct _field_stat_s {
    uint32              stat_id;        /* Unique stat entity identifier.*/
    struct _field_stat_s *next;      /* Policer lookup linked list.   */
}_field_stat_t;


#define _FP_ENTRY_POLICER_VALID                (1 << 0)
#define _FP_ENTRY_POLICER_INSTALLED            (1 << 1)
#define _FP_ENTRY_POLICER_IN_SECONDARY_SLICE   (1 << 2)
/*
 * Typedef:
 *     _field_stat_t
 * Purpose:
 *     This is the statistics collection entity description structure.
 *     Indexed by int stat_id handle.
 *     
 */
typedef struct _field_entry_policer_s {
    bcm_policer_t  pid;         /* Unique policer identifier. */
    uint8          flags;       /* Policer/entry flags.       */
}_field_entry_policer_t;

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
    bcm_field_entry_t      eid;        /* BCM unit unique entry identifier   */
    int                    prio;       /* Entry priority                     */
    uint32                 slice_idx;  /* TCAM index w/in slice (0-127 FB)   */
    uint16                 flags;      /* _FP_ENTRY_xxx flags                */
    _field_tcam_t          tcam;       /* Fields to be written into FP_TCAM  */
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_TRX_SUPPORT)
    _field_pbmp_t          pbmp;       /* Port bitmap */
#endif /* BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */    
    _field_action_t        *actions;   /* linked list of actions for entry   */
    _field_counter_t       *counter;   /* counter data, if used              */
    uint32                 mode_ctr;   /* BCM_FIELD_COUNTER_MODE_xxx         */
    uint8                  chain;      /* Chain flag                         */
    uint8                  chain_idx;  /* Chain key, matches in both entires */
    _field_slice_t         *fs;        /* Slice where entry lives            */
    _field_group_t         *group;     /* Group where entry lives            */
    _field_entry_policer_t policer[_FP_POLICER_LEVEL_COUNT];
                                       /* Policers attached to the entry.    */
    struct _field_entry_s  *next;      /* Entry lookup linked list.          */
}_field_entry_t;

typedef struct _field_control_s _field_control_t;
/*
 * Typedef:
 *     _field_funct_t
 * Purpose:
 *     Function pointers to device specific Field functions
 */
typedef struct _field_funct_s {
    int(*fp_detach)(int, _field_control_t *fc);  /* destructor function */
    int(*fp_udf_spec_set)(int, bcm_field_udf_spec_t *, uint32, uint32);
    int(*fp_udf_spec_get)(int, bcm_field_udf_spec_t *, uint32*, uint32*);
    int(*fp_udf_write)(int, bcm_field_udf_spec_t *, uint32, uint32);
    int(*fp_udf_read)(int, bcm_field_udf_spec_t *, uint32, uint32);
    int(*fp_group_install)(int, _field_group_t *fg);
    int(*fp_selcodes_install)(int unit, _field_group_t *fg,
                              uint8 slice_numb, bcm_pbmp_t pbmp,
                              int selcode_index);
    int(*fp_group_remove)(int, _field_group_t *fg);
    int(*fp_entry_remove)(int unit, _field_entry_t *f_ent, int tcam_idx);
    int(*fp_entry_move)(int, _field_entry_t**, int);
    int(*fp_selcode_get)(int unit, _field_stage_t*, bcm_field_qset_t*, 
                         _field_group_t*);
    int(*fp_selcode_to_qset)(int unit, _field_stage_t *stage_fc, 
                             _field_group_t *fg,
                             int code_id,
                             bcm_field_qset_t *qset);
    int(*fp_qual_list_get)(int unit, _field_stage_t *, _field_group_t*);
    int(*fp_tcam_policy_clear)(int unit, _field_stage_id_t stage_id, int idx);
    int(*fp_tcam_policy_install)(int unit, _field_entry_t *f_ent, int idx);
    int(*fp_policer_install)(int unit, _field_entry_t *f_ent,
                             _field_policer_t *f_pl);
    int(*fp_slice_reinit)(int unit, fp_port_field_sel_entry_t *pfs_entry,
		    	  int slice_numb, _field_sel_t *selcodes);
    int(*fp_write_slice_map)(int unit, _field_stage_t *stage_fc, 
                             _field_virtual_map_t new_fp_virtual_map[]);
    int(*fp_qualify_ip_type)(int unit, _field_entry_t *f_ent,
                             bcm_field_IpType_t type); 
    int(*fp_action_support_check)(int unit, _field_entry_t *f_ent,
                                  bcm_field_action_t action, int *result); 
    int (*fp_egress_key_match_type_set)(int unit, _field_entry_t *f_ent);
    int (*fp_external_group_create)(int unit, _field_stage_t *stage_fc, 
                                              _field_group_t *fg);
    int (*fp_external_group_remove)(int unit, _field_group_t *fg);
    int (*fp_external_entry_install)(int unit, _field_entry_t *f_ent);
    int (*fp_external_entry_remove)(int unit, _field_entry_t *f_ent);
    int (*fp_external_entry_prio_set)(int unit, _field_entry_t *f_ent, 
                                      int prio);
    int (*fp_meter_control_set)(int unit, uint32 value);
} _field_funct_t;

#define _FP_COLOR_INDEPENDENT    (1 << 0)
#define _FP_INTRASLICE_ENABLE    (1 << 1)
#define _FP_EXTERNAL_PRESENT     (1 << 2)
#define _FP_HALF_SLICE           (1 << 3)

/*
 * Typedef:
 *     _field_control_t
 * Purpose:
 *     One structure for each StrataSwitch Device that holds the global
 *     field processor metadata for one device.
 */
struct _field_control_s {
    sal_mutex_t            fc_lock;       /* Protection mutex.             */
    bcm_field_status_t     field_status; /* Return status for module.      *//* anzhen.zheng, 6/4/2008 */
    uint8                  flags;         /* Module specific flags.        */
    bcm_field_stage_t      stage;         /* Default FP pipeline stage.    */
    int                    max_stage_id;  /* Number of fp stages.          */
    int                    tcam_sz;      /* number of entries in TCAM      */
    int                    tcam_slices;  /* number of internal slices      */
    int                    tcam_slice_sz;/* number of entries per slice    */
    int                    tcam_ext_numb; /* Slice number for external.    */
                                          /* TCAM (-1 if not present).     */
    _field_udf_t           *udf;          /* field_status->group_total     */
                                          /* elements indexed by priority. */
    struct _field_group_s  *groups;       /* List of groups in unit.       */
    struct _field_stage_s  *stages;       /* Pipeline stage FP info.       */
    _field_funct_t         functions;     /* Device specific functions.    */
    _field_entry_t         **entry_hash;  /* Entry lookup hash.            */
    _field_policer_t       **policer_hash;/* Policer lookup hash.          */
    uint32                 policer_count; /* Number of active policers.    */
    _field_stat_t          **stat_hash;   /* Counter lookup hash.          */
    uint32                 stat_count;    /* Number of active counters.    */
};

/*
 *  _field_group_add_fsm struct. 
 *  Field group creation state machine.  
 *  Handles sanity checks, resources selection, allocation. 
 */
typedef struct _field_group_add_fsm_s {
    /* State machine data. */
    uint8                 fsm_state;     /* FSM state.                     */
    uint8                 fsm_state_prev;/* Previous FSM state.            */
    uint32                flags;         /* State specific flags.          */
    int                   rv;            /* Error code.                    */

    /* Field control info. */
    _field_control_t      *fc;           /* Field control structure.       */
    _field_stage_t        *stage_fc;     /* Field stage control structure. */
    

    /* Group information. */
    int                    priority;     /* New group priority.            */
    bcm_field_group_t      group_id;     /* SW Group id.                   */
    bcm_pbmp_t             pbmp;         /* Input port bitmap.             */
    bcm_field_qset_t       qset;         /* Qualifiers set.                */      
    bcm_field_group_mode_t mode;         /* Group mode.                    */

    /* Allocated data structures. */
    _field_group_t          *fg;         /* Allocated group structure.     */   
} _field_group_add_fsm_t;

#define _BCM_FP_GROUP_ADD_STATE_START                  (0x1)
#define _BCM_FP_GROUP_ADD_STATE_ALLOC                  (0x2)
#define _BCM_FP_GROUP_ADD_STATE_QSET_UPDATE            (0x3)
#define _BCM_FP_GROUP_ADD_STATE_SEL_CODES_GET          (0x4)
#define _BCM_FP_GROUP_ADD_STATE_QSET_ALTERNATE         (0x5)
#define _BCM_FP_GROUP_ADD_STATE_SLICE_ALLOCATE         (0x6)
#define _BCM_FP_GROUP_ADD_STATE_HW_QUAL_LIST_GET       (0x7)
#define _BCM_FP_GROUP_ADD_STATE_UDF_UPDATE             (0x8)
#define _BCM_FP_GROUP_ADD_STATE_ADJUST_VIRTUAL_MAP     (0x9)
#define _BCM_FP_GROUP_ADD_STATE_EXTERNAL_GROUP_CREATE  (0xa)
#define _BCM_FP_GROUP_ADD_STATE_CAM_COMPRESS           (0xb)
#define _BCM_FP_GROUP_ADD_STATE_END                    (0xc)

#define _BCM_FP_GROUP_ADD_STATE_STRINGS \
{                                  \
    "GroupAddStart",               \
    "GroupAddAlloc",               \
    "GroupAddStateQsetUpdate",     \
    "GroupAddSelCodesGet",         \
    "GroupAddQsetAlternate",       \
    "GroupAddSliceAllocate",       \
    "GroupAddHwQualListGet",       \
    "GroupAddUdfUpdate",           \
    "GroupAddAdjustVirtualMap",    \
    "GroupAddExternalGroupCreate", \
    "GroupAddEnd"                  \
}

#define _BCM_FP_GROUP_ADD_INTRA_SLICE          (1 << 0)
#define _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY     (1 << 1)
#define _BCM_FP_GROUP_ADD_FIELD_LOCKED         (1 << 2)

/*
 * Prototypes of Field Processor utility funtions
 */
extern int _field_control_get(int unit, _field_control_t **fc);
extern int _field_stage_control_get(int unit, _field_stage_id_t stage,
                                    _field_stage_t **stage_fc);
extern int _field_group_get(int unit, bcm_field_group_t gid, 
                            _field_group_t **group_p);
extern int _field_entry_get(int unit, bcm_field_entry_t eid, uint32 flags, 
                            _field_entry_t **enty_p);
extern int _bcm_field_entry_tcam_parts_count (int unit, uint32 group_flags, int *part_count);

extern int _bcm_field_entry_flags_to_tcam_part (uint32 entry_flags, 
                                                uint32 group_flags, 
                                                int *entry_part);
extern int _bcm_field_tcam_part_to_entry_flags(int entry_part, 
                                               uint32 group_flags, 
                                               uint32 *entry_flags);
extern int _field_port_filter_enable_set(int unit, _field_control_t *fc, 
                                         uint32 state);
extern int _field_meter_refresh_enable_set(int unit, _field_control_t *fc,
                                           uint32 state);
extern int _field_qual_info_create(bcm_field_qualify_t qid, soc_mem_t mem,
                                   soc_field_t fpf, int offset, int width,
                                    _qual_info_t **fq_p);
extern int _field_qual_list_copy(_qual_info_t **fq_dst,
                                 const _qual_info_t *fq_src, int offset);
extern void _field_qual_list_destroy(_qual_info_t **f_qual);
extern int _field_qual_value_set(int unit, const _qual_info_t *qi,
                                 _field_entry_t *f_ent,
                                 const uint32 *p_data, uint32 *p_mask);
extern int _field_qual_value32_set(int unit, const _qual_info_t *qi,
                                   _field_entry_t *f_ent,
                                   const uint32 data, const uint32 mask);
extern int _field_qualify32(int unit, bcm_field_entry_t entry, 
                            bcm_field_qualify_t qual, uint32 data, uint32 mask);
extern int _field_qualify_ip6(int unit, bcm_field_entry_t entry, 
                              bcm_field_qualify_t qual,
                              bcm_ip6_t data, bcm_ip6_t mask);
extern int _field_qset_union(const bcm_field_qset_t *qset1,
                             const bcm_field_qset_t *qset2,
                             bcm_field_qset_t *qset_union);
extern int _field_qset_is_subset(const bcm_field_qset_t *qset1,
                                 const bcm_field_qset_t *qset2);
extern bcm_field_qset_t _field_qset_diff(const bcm_field_qset_t qset_1,
                                         const bcm_field_qset_t qset_2);
extern int _field_qset_is_empty(const bcm_field_qset_t qset);

extern int _bcm_field_setup_post_ethertype_udf(int unit,
                                               bcm_field_udf_t *udf_id);
extern int _field_trans_flags_to_index(int unit, int flags, uint8 *tbl_idx);
extern int _field_entry_tcam_idx_get(int unit, _field_entry_t *f_ent, int *idx);
extern int _field_qual_add (_field_fpf_info_t *fpf_info,
                            bcm_field_qualify_t qid, int offset,
                            int width, int code);
extern int _bcm_field_tpid_hw_encode(int unit, uint16 tpid, uint32 *hw_code);
extern int _field_entry_prio_cmp(int prio_first, int prio_second);
extern int _bcm_field_prio_mgmt_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_prio_mgmt_deinit(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_entry_prio_mgmt_update(int unit, _field_entry_t *f_ent, 
                                              int flag, uint32 old_location);
extern int _bcm_field_entry_target_location(int unit, _field_stage_t *stage_fc, 
                   _field_entry_t *f_ent, int new_prio, uint32 *new_location);
extern int _bcm_field_entry_counters_move(int unit, _field_stage_t *stage_fc, 
                                          int old_index, int new_index);
extern int _bcm_field_sw_counter_update(int unit, _field_stage_t *stage_fc, 
                                        soc_mem_t mem, int idx_min, int idx_max,
                                        char *buf, int flags);
extern int _bcm_field_policer_get(int unit, bcm_policer_t pid, 
                                  _field_policer_t **policer_p);
extern int 
_bcm_field_meter_pair_mode_get(int unit, _field_policer_t *f_pl,
                               uint32 *mode);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_field_cleanup(int unit);
#else
#define _bcm_field_cleanup(u)        (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BROADCOM_DEBUG
extern void _field_selcode_dump(int unit, char *prefix, _field_sel_t sel_codes,
                                char *suffix) ;
extern void _field_qset_debug(bcm_field_qset_t qset);
extern void _field_qset_dump(char *prefix, bcm_field_qset_t qset, char* suffix);
extern void _field_qual_info_tbl_dump(uint32 size, _qual_info_t *qi_tbl[],
                                      char *tbl_name);
#endif /* BROADCOM_DEBUG */

#endif  /* BCM_FIELD_SUPPORT */
#endif  /* !_BCM_INT_FIELD_H */
