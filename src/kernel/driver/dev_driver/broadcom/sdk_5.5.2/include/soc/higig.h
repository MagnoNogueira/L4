/*
 * $Id: higig.h,v 1.12 Broadcom SDK $
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
 * File:        higig.h
 */

#include <soc/defs.h>
#include <soc/enet.h>

#ifndef _SOC_HIGIG_H
#define _SOC_HIGIG_H

#ifdef BCM_XGS_SUPPORT

/****************************************************************************
 * Higig Header Format
 *
 * On BCM5670/BCM5675 (Hercules/Hercules1.5), to send or receive packets
 * on an HG port, the following HIGIG header is always prepended to
 * an UNTAGGED ethernet frame.
 *
 * ANSI only permits signed or unsigned int for bit field type.  This
 * structure will only work for compilers for which uint32 is unsigned
 * int, and which completely pack bit fields beginning at the MSbit for
 * big-endian machines and at the LSbit for little-endian machines.
 *
 * NOTE: these structures already put the the individual bytes in memory
 * in big endian order for both big- and little-endian machines, so no
 * further swapping is required.
 ***************************************************************************/

typedef union soc_higig_hdr_u {
    struct {
        uint8   bytes[12];
    } overlay0;

#if defined(LE_HOST)

    struct {                            /* Byte # */
        uint32  start:8;                /* 0 */
        uint32  hgi:8;                  /* 1 */
        uint32  vlan_id_hi:4;           /* 2 */
        uint32  vlan_cfi:1;
        uint32  vlan_pri:3;
        uint32  vlan_id_lo:8;           /* 3 */
        uint32  opcode:3;               /* 4 */
        uint32  src_mod:5;
        uint32  src_port:6;             /* 5 */
        uint32  pfm:2;
        uint32  cos:3;                  /* 6 */
        uint32  dst_port:5;
        uint32  dst_mod:5;              /* 7 */
        uint32  cng:1;
        uint32  hdr_format:2;
        uint32  mirror:1;               /* 8 */
        uint32  mirror_done:1;
        uint32  mirror_only:1;
        uint32  ingress_tagged:1;
        uint32  dst_tgid:3;
        uint32  dst_t:1;
        uint32  _rsvd1:8;               /* 9 */
        uint32  _rsvd2:8;               /* 10 */
        uint32  _rsvd3:8;               /* 11 */
    } overlay1;

    struct {                            /* Byte # */
        uint32  _rsvd1:8;               /* 0 */
        uint32  _rsvd2:8;               /* 1 */
        uint32  _rsvd3:8;               /* 2 */
        uint32  _rsvd4:8;               /* 3 */
        uint32  _rsvd5:8;               /* 4 */
        uint32  tgid:6;                 /* 5 */
        uint32  _rsvd6:2;
        uint32  _rsvd7:3;               /* 6 */
        uint32  l2mc_ptr_lo:5;
        uint32  l2mc_ptr_hi:5;          /* 7 */
        uint32  _rsvd8:3;
        uint32  ctag_hi:8;              /* 8 */
        uint32  ctag_lo:8;              /* 9 */
        uint32  _rsvd9:8;               /* 10 */
        uint32  _rsvd10:8;              /* 11 */
    } overlay2;

    struct {                            /* Byte # */
        uint32  start:8;                /* 0 */
        uint32  dst_mod_6:1;            /* 1 */
        uint32  src_mod_6:1;
        uint32  hdr_ext_len:3;
        uint32  cng_hi:1;
        uint32  hgi:2;
        uint32  vlan_id_hi:4;           /* 2 */
        uint32  vlan_cfi:1;
        uint32  vlan_pri:3;
        uint32  vlan_id_lo:8;           /* 3 */
        uint32  opcode:3;               /* 4 */
        uint32  src_mod:5;
        uint32  src_port:6;             /* 5 */
        uint32  pfm:2;
        uint32  cos:3;                  /* 6 */
        uint32  dst_port:5;
        uint32  dst_mod:5;              /* 7 */
        uint32  cng:1;
        uint32  hdr_format:2;
        uint32  mirror:1;               /* 8 */
        uint32  mirror_done:1;
        uint32  mirror_only:1;
        uint32  ingress_tagged:1;
        uint32  dst_tgid:3;
        uint32  dst_t:1;
        uint32  vc_label_19_16:4;       /* 9 */
        uint32  label_present:1;
        uint32  l3:1;
        uint32  dst_mod_5:1;
        uint32  src_mod_5:1;
        uint32  vc_label_15_8:8;        /* 10 */
        uint32  vc_label_7_0:8;         /* 11 */
    } hgp_overlay1;

#else /* !LE_HOST */

    struct {                            /* Byte # */
        uint32  start:8;                /* 0 */
        uint32  hgi:8;                  /* 1 */
        uint32  vlan_pri:3;             /* 2 */
        uint32  vlan_cfi:1;
        uint32  vlan_id_hi:4;
        uint32  vlan_id_lo:8;           /* 3 */
        uint32  src_mod:5;              /* 4 */
        uint32  opcode:3;
        uint32  pfm:2;                  /* 5 */
        uint32  src_port:6;
        uint32  dst_port:5;             /* 6 */
        uint32  cos:3;
        uint32  hdr_format:2;           /* 7 */
        uint32  cng:1;
        uint32  dst_mod:5;
        uint32  dst_t:1;                /* 8 */
        uint32  dst_tgid:3;
        uint32  ingress_tagged:1;
        uint32  mirror_only:1;
        uint32  mirror_done:1;
        uint32  mirror:1;
        uint32  _rsvd1:8;               /* 9 */
        uint32  _rsvd2:8;               /* 10 */
        uint32  _rsvd3:8;               /* 11 */
    } overlay1;

    struct {                            /* Byte # */
        uint32  _rsvd1:8;               /* 0 */
        uint32  _rsvd2:8;               /* 1 */
        uint32  _rsvd3:8;               /* 2 */
        uint32  _rsvd4:8;               /* 3 */
        uint32  _rsvd5:8;               /* 4 */
        uint32  _rsvd6:2;               /* 5 */
        uint32  tgid:6;
        uint32  l2mc_ptr_lo:5;          /* 6 */
        uint32  _rsvd7:3;
        uint32  _rsvd8:3;               /* 7 */
        uint32  l2mc_ptr_hi:5;
        uint32  ctag_hi:8;              /* 8 */
        uint32  ctag_lo:8;              /* 9 */
        uint32  _rsvd9:8;               /* 10 */
        uint32  _rsvd10:8;              /* 11 */
    } overlay2;

    struct {                            /* Byte # */
        uint32  start:8;                /* 0 */
        uint32  hgi:2;                  /* 1 */
        uint32  cng_hi:1;
        uint32  hdr_ext_len:3;
        uint32  src_mod_6:1;
        uint32  dst_mod_6:1;
        uint32  vlan_pri:3;             /* 2 */
        uint32  vlan_cfi:1;
        uint32  vlan_id_hi:4;
        uint32  vlan_id_lo:8;           /* 3 */
        uint32  src_mod:5;              /* 4 */
        uint32  opcode:3;
        uint32  pfm:2;                  /* 5 */
        uint32  src_port:6;
        uint32  dst_port:5;             /* 6 */
        uint32  cos:3;
        uint32  hdr_format:2;           /* 7 */
        uint32  cng:1;
        uint32  dst_mod:5;
        uint32  dst_t:1;                /* 8 */
        uint32  dst_tgid:3;
        uint32  ingress_tagged:1;
        uint32  mirror_only:1;
        uint32  mirror_done:1;
        uint32  mirror:1;
        uint32  src_mod_5:1;            /* 9 */
        uint32  dst_mod_5:1;
        uint32  l3:1;
        uint32  label_present:1;
        uint32  vc_label_19_16:4;
        uint32  vc_label_15_8:8;        /* 10 */
        uint32  vc_label_7_0:8; /* 11 */
    } hgp_overlay1;

#endif /* !LE_HOST */

} soc_higig_hdr_t;

#define SOC_HIGIG_HDR_SIZE      ((int) sizeof(soc_higig_hdr_t))

/*
 * Higig Header Field Manipulation
 */

typedef enum {
    /* NOTE: strings in soc_higig_field_names[] must match */
    HG_invalid = -1,
    HG_start = 0,
    HG_hgi,
    HG_vlan_tag,                /* aggregate of pri, cfi, and id */
    HG_vlan_pri,
    HG_vlan_cfi,
    HG_vlan_id,
    HG_src_mod,
    HG_opcode,
    HG_pfm,
    HG_src_port,
    HG_dst_port,
    HG_cos,
    HG_hdr_format,
    HG_cng,
    HG_dst_mod,
    HG_dst_t,
    HG_dst_tgid,
    HG_ingress_tagged,
    HG_mirror_only,
    HG_mirror_done,
    HG_mirror,
    HG_tgid,
    HG_l2mc_ptr,
    HG_ctag,
    HG_hdr_ext_len,     /* HG+ fields */
    HG_l3,
    HG_label_present,
    HG_vc_label,
#if defined(BCM_HIGIG2_SUPPORT)
    HG_tc,              /* HG2 fields */
    HG_mcst,
    HG_mgid,
    HG_lbid,
    HG_dp,
    HG_ppd_type,
    HG_src_t,
    HG_multipoint,
    HG_fwd_type,
    HG_dst_vp,
    HG_src_vp,
    HG_donot_modify,
    HG_donot_learn,
    HG_lag_failover,
#endif /* BCM_HIGIG2_SUPPORT */
    HG_COUNT
} soc_higig_field_t;

#if defined(BCM_HIGIG2_SUPPORT)
#define SOC_HIGIG2_FIELD_NAMES_INIT                     \
    "tc",              /* HG2 fields */                 \
    "mcst",                                             \
    "mgid",                                             \
    "lbid",                                             \
    "dp",                                               \
    "ppd_type",                                         \
    "src_t",                                            \
    "multipoint",                                       \
    "fwd_type",                                         \
    "dst_vp",                                           \
    "src_vp",                                           \
    "donot_modify",                                     \
    "donot_learn",                                      \
    "lag_failover",
#else
#define SOC_HIGIG2_FIELD_NAMES_INIT
#endif /* BCM_HIGIG2_SUPPORT */

/* NOTE: strings must match soc_higig_field_t */
#define SOC_HIGIG_FIELD_NAMES_INIT                      \
    "start",                                            \
    "hgi",                                              \
    "vlan_tag",                                         \
    "vlan_pri",                                         \
    "vlan_cfi",                                         \
    "vlan_id",                                          \
    "src_mod",                                          \
    "opcode",                                           \
    "pfm",                                              \
    "src_port",                                         \
    "dst_port",                                         \
    "cos",                                              \
    "hdr_format",                                       \
    "cng",                                              \
    "dst_mod",                                          \
    "dst_t",                                            \
    "dst_tgid",                                         \
    "ingress_tagged",                                   \
    "mirror_only",                                      \
    "mirror_done",                                      \
    "mirror",                                           \
    "tgid",                                             \
    "l2mc_ptr",                                         \
    "ctag",                                             \
    "hdr_ext_len",     /* HG+ fields */                 \
    "l3",                                               \
    "label_present",                                    \
    "vc_label",                                         \
    SOC_HIGIG2_FIELD_NAMES_INIT                         \
    NULL

#ifdef BCM_HIGIG2_SUPPORT

#define SOC_HIGIG2_START        0xfc /* Higig2 Module Header */

typedef union soc_higig2_hdr_u {
    struct {
        uint8   bytes[16];
    } overlay0;

#if defined(LE_HOST)

    struct {                            /* Byte # */
        /* "Legacy" PPD Overlay 1 */
        uint32  start:8;                /* 0 */
        uint32  tc:4;                   /* 1 */
        uint32  mcst:1;
        uint32  _rsvd1:3;
        uint32  dst_mod:8;              /* 2 */
        uint32  dst_port:8;             /* 3 */
        uint32  src_mod:8;              /* 4 */
        uint32  src_port:8;             /* 5 */
        uint32  lbid:8;                 /* 6 */
        uint32  ppd_type:3;             /* 7 */
        uint32  _rsvd2:3;
        uint32  dp:2;
        uint32  mirror:1;               /* 8 */
        uint32  mirror_done:1;
        uint32  mirror_only:1;
        uint32  ingress_tagged:1;
        uint32  dst_tgid:3;
        uint32  dst_t:1;
        uint32  vc_label_19_16:4;       /* 9 */
        uint32  label_present:1;
        uint32  l3:1;
        uint32  _rsvd3:2;
        uint32  vc_label_15_8:8;        /* 10 */
        uint32  vc_label_7_0:8;         /* 11 */
        uint32  vlan_id_hi:4;           /* 12 */
        uint32  vlan_cfi:1;
        uint32  vlan_pri:3;
        uint32  vlan_id_lo:8;           /* 13 */
        uint32  opcode:3;               /* 14 */
        uint32  _rsvd4:2;
        uint32  src_t:1;
        uint32  pfm:2;
        uint32  _rsvd5:5;               /* 15 */
        uint32  hdr_ext_len:3;
   } ppd_overlay1;

    struct {                            /* Byte # */
        /* "Legacy" PPD Overlay 2 */
        uint32  start:8;                /* 0 */
        uint32  tc:4;                   /* 1 */
        uint32  mcst:1;
        uint32  _rsvd1:3;
        uint32  dst_mod:8;              /* 2 */
        uint32  dst_port:8;             /* 3 */
        uint32  src_mod:8;              /* 4 */
        uint32  src_port:8;             /* 5 */
        uint32  lbid:8;                 /* 6 */
        uint32  ppd_type:3;             /* 7 */
        uint32  _rsvd2:3;
        uint32  dp:2;
        uint32  ctag_hi:8;              /* 8 */
        uint32  ctag_lo:8;              /* 9 */
        uint32  _rsvd3:8;               /* 10 */
        uint32  _rsvd4:8;               /* 11 */
        uint32  vlan_id_hi:4;           /* 12 */
        uint32  vlan_cfi:1;
        uint32  vlan_pri:3;
        uint32  vlan_id_lo:8;           /* 13 */
        uint32  opcode:3;               /* 14 */
        uint32  _rsvd5:2;
        uint32  src_t:1;
        uint32  pfm:2;
        uint32  _rsvd6:5;               /* 15 */
        uint32  hdr_ext_len:3;
    } ppd_overlay2;

    struct {                            /* Byte # */
        /* "Next Gen" PPD Overlay 3 */
        uint32  start:8;                /* 0 */
        uint32  tc:4;                   /* 1 */
        uint32  mcst:1;
        uint32  _rsvd1:3;
        uint32  dst_mod:8;              /* 2 */
        uint32  dst_port:8;             /* 3 */
        uint32  src_mod:8;              /* 4 */
        uint32  src_port:8;             /* 5 */
        uint32  lbid:8;                 /* 6 */
        uint32  ppd_type:3;             /* 7 */
        uint32  _rsvd2:3;
        uint32  dp:2;
        uint32  _rsvd3:2;               /* 8 */
        uint32  fwd_type:5;
        uint32  multipoint:1;
        uint32  _rsvd4:8;               /* 9 */
        uint32  dst_vp_high:8;          /* 10 */
        uint32  dst_vp_low:8;           /* 11 */
        uint32  src_vp_high:8;          /* 12 */
        uint32  src_vp_low:8;           /* 13 */
        uint32  opcode:3;               /* 14 */
        uint32  _rsvd5:1;
        uint32  lag_failover:1;
        uint32  donot_learn:1;
        uint32  donot_modify:1;
        uint32  mirror:1;
        uint32  _rsvd6:8;               /* 15 */
    } ppd_overlay3;

#else /* !LE_HOST */

    struct {                            /* Byte # */
        /* "Legacy" PPD Overlay 1 */
        uint32  start:8;                /* 0 */
        uint32  _rsvd1:3;               /* 1 */
        uint32  mcst:1;
        uint32  tc:4;
        uint32  dst_mod:8;              /* 2 */
        uint32  dst_port:8;             /* 3 */
        uint32  src_mod:8;              /* 4 */
        uint32  src_port:8;             /* 5 */
        uint32  lbid:8;                 /* 6 */
        uint32  dp:2;                   /* 7 */
        uint32  _rsvd2:3;
        uint32  ppd_type:3;
        uint32  dst_t:1;                /* 8 */
        uint32  dst_tgid:3;
        uint32  ingress_tagged:1;
        uint32  mirror_only:1;
        uint32  mirror_done:1;
        uint32  mirror:1;
        uint32  _rsvd3:2;               /* 9 */
        uint32  l3:1;
        uint32  label_present:1;
        uint32  vc_label_19_16:4;
        uint32  vc_label_15_8:8;        /* 10 */
        uint32  vc_label_7_0:8;         /* 11 */
        uint32  vlan_pri:3;             /* 12 */
        uint32  vlan_cfi:1;
        uint32  vlan_id_hi:4;
        uint32  vlan_id_lo:8;           /* 13 */
        uint32  pfm:2;                  /* 14 */
        uint32  src_t:1;
        uint32  _rsvd4:2;
        uint32  opcode:3;
        uint32  hdr_ext_len:3;          /* 15 */
        uint32  _rsvd5:5;
   } ppd_overlay1;

    struct {                            /* Byte # */
        /* "Legacy" PPD Overlay 2 */
        uint32  start:8;                /* 0 */
        uint32  _rsvd1:3;               /* 1 */
        uint32  mcst:1;
        uint32  tc:4;
        uint32  dst_mod:8;              /* 2 */
        uint32  dst_port:8;             /* 3 */
        uint32  src_mod:8;              /* 4 */
        uint32  src_port:8;             /* 5 */
        uint32  lbid:8;                 /* 6 */
        uint32  dp:2;                   /* 7 */
        uint32  _rsvd2:3;
        uint32  ppd_type:3;
        uint32  ctag_hi:8;              /* 8 */
        uint32  ctag_lo:8;              /* 9 */
        uint32  _rsvd3:8;               /* 10 */
        uint32  _rsvd4:8;               /* 11 */
        uint32  vlan_pri:3;             /* 12 */
        uint32  vlan_cfi:1;
        uint32  vlan_id_hi:4;
        uint32  vlan_id_lo:8;           /* 13 */
        uint32  pfm:2;                  /* 14 */
        uint32  src_t:1;
        uint32  _rsvd5:2;
        uint32  opcode:3;
        uint32  hdr_ext_len:3;          /* 15 */
        uint32  _rsvd6:5;
    } ppd_overlay2;

    struct {                            /* Byte # */
        /* "Next Gen" PPD Overlay 3 */
        uint32  start:8;                /* 0 */
        uint32  _rsvd1:3;               /* 1 */
        uint32  mcst:1;
        uint32  tc:4;
        uint32  dst_mod:8;              /* 2 */
        uint32  dst_port:8;             /* 3 */
        uint32  src_mod:8;              /* 4 */
        uint32  src_port:8;             /* 5 */
        uint32  lbid:8;                 /* 6 */
        uint32  dp:2;                   /* 7 */
        uint32  _rsvd2:3;
        uint32  ppd_type:3;
        uint32  multipoint:1;           /* 8 */
        uint32  fwd_type:5;
        uint32  _rsvd3:2;
        uint32  _rsvd4:8;               /* 9 */
        uint32  dst_vp_high:8;          /* 10 */
        uint32  dst_vp_low:8;           /* 11 */
        uint32  src_vp_high:8;          /* 12 */
        uint32  src_vp_low:8;           /* 13 */
        uint32  mirror:1;               /* 14 */
        uint32  donot_modify:1;
        uint32  donot_learn:1;
        uint32  lag_failover:1;
        uint32  _rsvd5:1;
        uint32  opcode:3;
        uint32  _rsvd6:8;               /* 15 */
    } ppd_overlay3;

#endif /* !LE_HOST */

} soc_higig2_hdr_t;

#define SOC_HIGIG2_HDR_SIZE     ((int) sizeof(soc_higig2_hdr_t))

extern void soc_higig2_dump(int unit, char *pfx, soc_higig2_hdr_t *hg);

#endif /* BCM_HIGIG2_SUPPORT */

extern soc_higig_field_t soc_higig_name_to_field(int unit, char *name);
extern char *soc_higig_field_to_name(int unit, soc_higig_field_t f);

extern uint32 soc_higig_field_get(int unit, soc_higig_hdr_t *hg,
                                  soc_higig_field_t field);
extern void soc_higig_field_set(int unit, soc_higig_hdr_t *hg,
                                soc_higig_field_t field, uint32 val);

extern void soc_higig_dump(int unit, char *pfx, soc_higig_hdr_t *hg);

#ifdef BCM_HIGIG2_SUPPORT
#define soc_higig_field_init(unit, hg) \
        sal_memset((hg), 0, SOC_HIGIG2_HDR_SIZE)
#else
#define soc_higig_field_init(unit, hg) \
        sal_memset((hg), 0, SOC_HIGIG_HDR_SIZE)
#endif

/* HIGIG Header field default values */

/* default SOP Symbol */
#define SOC_HIGIG_START         0xfb /* Default Start for BIGMAC */

/* HGI */
#define SOC_HIGIG_HGI           0x80 /* Default HGI */
#define SOC_HIGIG_HGI_MASK      0xc0 /* HGI field mask */

/* Module header Op-Codes */
#define SOC_HIGIG_OP_CPU        0x00 /* CPU Frame */
#define SOC_HIGIG_OP_UC         0x01 /* Unicast Frame */
#define SOC_HIGIG_OP_BC         0x02 /* Broadcast or DLF frame */
#define SOC_HIGIG_OP_MC         0x03 /* Multicast Frame */
#define SOC_HIGIG_OP_IPMC       0x04 /* IP Multicast Frame */

#define SOC_HIGIG_DST_MOD_CPU   0x00

/* Header Format */
#define SOC_HIGIG_HDR_DEFAULT   0x00
#define SOC_HIGIG_HDR_CLASS_TAG 0x01
#define SOC_HIGIG_HDR_EXT_LEN   0x02
#define SOC_HIGIG_HDR_RSVD2     0x03

/* XGS Frame format */
typedef struct xgs_pkt_s {
    soc_higig_hdr_t     higig_hdr;
    enet_hdr_t          enet_hdr;
} xgs_pkt_t;

/* Hercules Header modes */
#define SOC_XGS_ENCAP_IEEE      0x00 /* Standard IEEE 802.3 (Default) */
#define SOC_XGS_ENCAP_HIGIG     0x01 /* Broadcom HIGIG Format */
#define SOC_XGS_ENCAP_ALLYR     0x02 /* BCM5632 compatible mode */
#define SOC_XGS_ENCAP_RSVP      0x03 /* Reserved */

/****************************************************************************
 * BCM5632 Header Format
 * When the MAC is in BCM5632 header mode, we prepend the following
 * two words of data to a standard 802.3 Ethernet Frame.
 *
 * ANSI only permits signed or unsigned int for bit field type.  This
 * structure will only work for compilers for which uint32 is unsigned
 * int, and which completely pack bit fields beginning at the MSbit for
 * big-endian machines and at the LSbit for little-endian machines.
 *
 * NOTE: these structures already put the the individual bytes in memory
 * in big endian order for both big- and little-endian machines, so no
 * further swapping is required.
 ****************************************************************************/

typedef union soc_bcm5632_hdr_u {
    struct {
        uint8   bytes[8];
    } overlay0;

#if defined(LE_HOST)

    struct {                            /* Byte # */
        uint32  d_portid:6;             /* 0 */
        uint32  _rsvd1:2;
        uint32  _rsvd2:8;               /* 1 */
        uint32  _rsvd3:8;               /* 2 */
        uint32  _rsvd4:8;               /* 3 */

        uint32  start:8;                /* 4 */
        uint32  length_hi:8;            /* 5 */
        uint32  length_lo:8;            /* 6 */
        uint32  s_portid:6;             /* 7 */
        uint32  _rsvd5:2;
    } overlay1;

#else /* !LE_HOST */

    struct {                            /* Byte # */
        uint32  _rsvd1:2;               /* 0 */
        uint32  d_portid:6;
        uint32  _rsvd2:8;               /* 1 */
        uint32  _rsvd3:8;               /* 2 */
        uint32  _rsvd4:8;               /* 3 */

        uint32  start:8;                /* 4 */
        uint32  length_hi:8;            /* 5 */
        uint32  length_lo:8;            /* 6 */
        uint32  _rsvd5:2;               /* 7 */
        uint32  s_portid:6;
    } overlay1;

#endif

} soc_bcm5632_hdr_t;

#define SOC_BCM5632_HDR_SIZE    ((int) sizeof(soc_bcm5632_hdr_t))

/* BCM5632 Frame format */
typedef struct b5632_pkt_s {
    soc_bcm5632_hdr_t   b5632_hdr;
    enet_hdr_t          enet_hdr;
} b5632_pkt_t;

/* BCM5632 Header field default values */

/* HGI */
#define SOC_BCM5632_HGI         0x00 /* Default HGI for BCM5632 */

#endif /* BCM_XGS_SUPPORT */

#endif  /* !_SOC_HIGIG_H */
