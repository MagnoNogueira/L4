/*
 * $Id: pbsmh.h,v 1.8.6.1 Broadcom SDK $
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
 * File:        pbsmh.h
 */

#include <soc/defs.h>

#ifndef _SOC_PBSMH_H
#define _SOC_PBSMH_H

#ifdef BCM_XGS3_SWITCH_SUPPORT

/****************************************************************************
 * PBS MH Header Format
 *
 * On XGS3 devices this is used to send a Packet Byte Stream out a port.
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

typedef struct soc_pbsmh_hdr_s {
#if defined(LE_HOST)
            				/* Byte # */
	uint32	start:8;		/* 0 */
	uint32	_rsvd0:24;		/* 1-3 */
	uint32	_rsvd1;		        /* 4-7 */
	uint32	_rsvd2:16;		/* 8-9 */
	uint32	src_mod:6;              /* 10 */
	uint32	_rsvd3:2;		/* 10 */
	uint32	dst_port:5;             /* 11 */
	uint32	cos:3;                  /* 11 */
#else /* !LE_HOST */
            				/* Byte # */
	uint32	start:8;		/* 0 */
	uint32	_rsvd0:24;		/* 1-3 */
	uint32	_rsvd1;		        /* 4-7 */
	uint32	_rsvd2:16;		/* 8-10 */
	uint32	_rsvd3:2;		/* 10 */
	uint32	src_mod:6;              /* 10 */
	uint32	cos:3;                  /* 11 */
	uint32	dst_port:5;             /* 11 */
#endif /* !LE_HOST */
} soc_pbsmh_hdr_t;

typedef struct soc_pbsmh_v2_hdr_s {
#if defined(LE_HOST)
            				/* Byte # */
	uint32	start:8;		/* 0 */
	uint32	_rsvd0:24;		/* 1-3 */
	uint32	_rsvd1;		        /* 4-7 */
	uint32	_rsvd2:8;		/* 8  */
	uint32	src_mod:8;              /* 9  */
	uint32	cos:4;                  /* 10 */
	uint32	pri:4;                  /* 10 */
	uint32	dst_port:7;             /* 11 */
	uint32	l3pbm_sel:1;            /* 11 */
#else /* !LE_HOST */
            				/* Byte # */
	uint32	start:8;		/* 0 */
	uint32	_rsvd0:24;		/* 1-3 */
	uint32	_rsvd1;		        /* 4-7 */
	uint32	_rsvd2:8;		/* 8  */
	uint32	src_mod:8;              /* 9  */
	uint32	pri:4;                  /* 10 */
	uint32	cos:4;                  /* 10 */
	uint32	l3pbm_sel:1;            /* 11 */
	uint32	dst_port:7;             /* 11 */
#endif /* !LE_HOST */
} soc_pbsmh_v2_hdr_t;

typedef struct soc_pbsmh_v3_hdr_s {
#if defined(LE_HOST)
                                        /* Byte # */
        uint32  start:8;                /* 0 */
        uint32  _rsvd0:24;              /* 1-3 */
        uint32  _rsvd1;                 /* 4-7 */
        uint32  src_mod_hi:4;           /* 8  */ 
        uint32  _rsvd2:4;               /* 8  */
        uint32  pri:4;                  /* 9  */
        uint32  src_mod_lo:4;           /* 9  */
        uint32  cos:6;                  /* 10 */
        uint32  _rsvd3:2;               /* 10 */
        uint32  dst_port:7;             /* 11 */
        uint32  l3pbm_sel:1;            /* 11 */
#else /* !LE_HOST */
                                        /* Byte # */
        uint32  start:8;                /* 0 */
        uint32  _rsvd0:24;              /* 1-3 */
        uint32  _rsvd1;                 /* 4-7 */
        uint32  _rsvd2:4;               /* 8  */
        uint32  src_mod_hi:4;           /* 8  */
        uint32  src_mod_lo:4;           /* 9  */
        uint32  pri:4;                  /* 9  */
        uint32  _rsvd3:2;               /* 10 */
        uint32  cos:6;                  /* 10 */
        uint32  l3pbm_sel:1;            /* 11 */
        uint32  dst_port:7;             /* 11 */
#endif /* !LE_HOST */
} soc_pbsmh_v3_hdr_t;

typedef struct soc_pbsmh_v4_hdr_s {
#if defined(LE_HOST)
            				/* Byte # */
	uint32	start:8;		/* 0 */
	uint32	_rsvd0:24;		/* 1-3 */
	uint32	_rsvd1;		        /* 4-7 */
	uint32	src_mod_hi:1;           /* 8  */
        uint32  _rsvd2:7;               /* 8  */
	uint32	pri_hi:1;               /* 9 */
	uint32	src_mod_lo:7;           /* 9  */
	uint32	cos:5;                  /* 10 */
	uint32	pri_lo:3;               /* 10 */
	uint32	dst_port:7;             /* 11 */
	uint32	l3pbm_sel:1;            /* 11 */
#else /* !LE_HOST */
            				/* Byte # */
	uint32	start:8;		/* 0 */
	uint32	_rsvd0:24;		/* 1-3 */
	uint32	_rsvd1;		        /* 4-7 */
        uint32  _rsvd2:7;               /* 8  */
	uint32	src_mod_hi:1;           /* 8  */
	uint32	src_mod_lo:7;           /* 9  */
	uint32	pri_hi:1;               /* 9 */
	uint32	pri_lo:3;               /* 10 */
	uint32	cos:5;                  /* 10 */
	uint32	l3pbm_sel:1;            /* 11 */
	uint32	dst_port:7;             /* 11 */
#endif /* !LE_HOST */
} soc_pbsmh_v4_hdr_t;

/*
 * Fast macros to setup the PBSMH
 */
#if defined(LE_HOST)
#define PBS_MH_W0_START_SET(mh) (((uint32 *)(mh))[0] = 0x000000FF)
#define PBS_MH_W1_RSVD_SET(mh)  (((uint32 *)(mh))[1] = 0x00000000)

#define PBS_MH_V1_W2_SMOD_DPORT_COS_SET(mh, smod, dport, cos, dc1, dc2) \
        (((uint32 *)(mh))[2] = (smod << 16) | (cos << 29) | (dport << 24))

#define PBS_MH_V2_W2_SMOD_DPORT_COS_SET(mh, smod, dport, cos, ipri, l3pbm_sel) \
        (((uint32 *)(mh))[2] = (smod << 8) | (ipri << 20) | (cos << 16) | \
                               (l3pbm_sel << 31) | (dport << 24))

#define PBS_MH_V3_W2_SMOD_DPORT_COS_SET(mh, smod, dport, cos, ipri, l3pbm_sel) \
        (((uint32 *)(mh))[2] = ((smod >> 4) & 0xf) | ((smod & 0xf) << 12) | \
                               (ipri << 8) | (cos << 16) | \
                               (l3pbm_sel << 31) | (dport << 24))

#define PBS_MH_V4_W2_SMOD_DPORT_COS_SET(mh, smod, dport, cos, ipri, l3pbm_sel) \
        (((uint32 *)(mh))[2] = ((smod >> 7) & 0x1) | ((smod & 0x7f) << 9) | \
                               (((ipri >> 3) & 0x1) << 8) | \
                               ((ipri & 0x7) << 21) | (cos << 16) | \
                               (l3pbm_sel << 31) | (dport << 24))

#define PBS_MH_V2_TS_PKT_SET(mh) (((uint32 *)(mh))[2] |=  (1 << 5))

#else /* !LE_HOST */
#define PBS_MH_W0_START_SET(mh) (((uint32 *)(mh))[0] = 0xFF000000)
#define PBS_MH_W1_RSVD_SET(mh)  (((uint32 *)(mh))[1] = 0x00000000)

#define PBS_MH_V1_W2_SMOD_DPORT_COS_SET(mh, smod, dport, cos, dc1, dc2) \
        (((uint32 *)(mh))[2] = (smod << 8) | (cos << 5) | (dport << 0))

#define PBS_MH_V2_W2_SMOD_DPORT_COS_SET(mh, smod, dport, cos, ipri, l3pbm_sel) \
        (((uint32 *)(mh))[2] = (smod << 16) | (ipri << 12) | (cos << 8) |      \
                               (l3pbm_sel << 7) | (dport << 0))

#define PBS_MH_V3_W2_SMOD_DPORT_COS_SET(mh, smod, dport, cos, ipri, l3pbm_sel) \
        (((uint32 *)(mh))[2] = (smod << 20) | (ipri << 16) | (cos << 8) |      \
                               (l3pbm_sel << 7) | (dport << 0))

#define PBS_MH_V4_W2_SMOD_DPORT_COS_SET(mh, smod, dport, cos, ipri, l3pbm_sel) \
        (((uint32 *)(mh))[2] = (smod << 17) | (ipri << 13) | (cos << 8) |      \
                               (l3pbm_sel << 7) | (dport << 0))

#define PBS_MH_V2_TS_PKT_SET(mh) (((uint32 *)(mh))[2] |=  (1 << 29))

#endif /* !LE_HOST */

#define SOC_PBSMH_HDR_SIZE      ((int) sizeof(soc_pbsmh_hdr_t))

/*
 * PBS MH Field Manipulation
 */

typedef enum {
    /* NOTE: strings in soc_pbsmh_field_names[] must match */
    PBSMH_invalid = -1,
    PBSMH_start = 0,
    PBSMH_src_mod,      /* Source Modid to disable E2E checks */
    PBSMH_dst_port,     /* Destination port */
    PBSMH_cos,          /* COS queue */
    PBSMH_pri,          /* Inupt Priority */
    PBSMH_l3pbm_sel,    /* Use L3 PBM */
    PBSMH_COUNT
} soc_pbsmh_field_t;

extern soc_pbsmh_field_t soc_pbsmh_name_to_field(int unit, char *name);
extern char *soc_pbsmh_field_to_name(int unit, soc_pbsmh_field_t f);

extern uint32 soc_pbsmh_field_get(int unit, soc_pbsmh_hdr_t *mh,
                                  soc_pbsmh_field_t field);
extern void soc_pbsmh_field_set(int unit, soc_pbsmh_hdr_t *mh,
                                soc_pbsmh_field_t field, uint32 val);

extern void soc_pbsmh_dump(int unit, char *pfx, soc_pbsmh_hdr_t *mh);

#define soc_pbsmh_field_init(unit, mh) \
        sal_memset((mh), 0, sizeof (soc_pbsmh_hdr_t))

/* PBSMH field default values */
/* default SOP Symbol */
#define SOC_PBSMH_START         0xff /* PBS MH indicator */

#endif /* BCM_XGS3_SWITCH_SUPPORT */
#endif  /* !_SOC_PBSMH_H */
