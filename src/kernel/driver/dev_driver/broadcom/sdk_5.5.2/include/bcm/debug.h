/*
 * $Id: debug.h,v 1.18.6.3 Broadcom SDK $
 * 
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
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */

#ifndef __BCM_DEBUG_H__
#define __BCM_DEBUG_H__

#include <bcm/types.h>

#define BCM_DBG_NORMAL          (1 << 0)   /* Print errors. */
#define BCM_DBG_ERR             (1 << 1)   /* Print errors. */
#define BCM_DBG_WARN            (1 << 2)   /* Print warnings. */
#define BCM_DBG_VERBOSE         (1 << 3)   /* General verbose output. */
#define BCM_DBG_VVERBOSE        (1 << 4)   /* Very verbose output. */
#define BCM_DBG_PORT            (1 << 5)   /* Port operations. */
#define BCM_DBG_VLAN            (1 << 6)   /* VLAN operations. */
#define BCM_DBG_L2              (1 << 7)   /* L2 operations. */
#define BCM_DBG_L3              (1 << 8)   /* L3, DefIP and IPMC operations. */
#define BCM_DBG_TX              (1 << 9)   /* Packet transmit. */
#define BCM_DBG_RX              (1 << 10)  /* Packet Receive. */
#define BCM_DBG_PKT_DATA        (1 << 11)  /* Display packet data. */
#define BCM_DBG_LINK            (1 << 12)  /* Show Link status changes. */
#define BCM_DBG_COUNTER         (1 << 13)  /* Counter DMA, etc. */
#define BCM_DBG_INTR            (1 << 14)  /* Interrupt processing (RX). */
#define BCM_DBG_DMA             (1 << 15)  /* DMA info. */
#define BCM_DBG_STK             (1 << 16)  /* Stacking. */
#define BCM_DBG_ATTACH          (1 << 17)  /* Device contol. */
#define BCM_DBG_MCAST           (1 << 18)  /* L2 Multicast. */
#define BCM_DBG_IPMC            (1 << 19)  /* IP Multicast. */
#define BCM_DBG_FP              (1 << 20)  /* Field Processor. */
#define BCM_DBG_FILTER          (1 << 21)  /* Fast Filter Processor. */
#define BCM_DBG_API             (1 << 22)  /* API. */
#define BCM_DBG_POLICER         (1 << 23)  /* Policer. */
#define BCM_DBG_COSQ            (1 << 24)  /* COSQ. */
#define BCM_DBG_OAM             (1 << 25)  /* OAM. */
#define BCM_DBG_MPLS            (1 << 26)  /* MPLS. */
#define BCM_DBG_COUNT           27         

#define BCM_DBG_NAMES           \
    "NORmal", \
    "ERRor", \
    "WARN", \
    "VERbose", \
    "VVERbose", \
    "POrt", \
    "VLAN", \
    "L2", \
    "L3", \
    "TX", \
    "RX", \
    "PKTdata", \
    "LInk", \
    "CounTeR", \
    "INTr", \
    "DMA", \
    "STK", \
    "ATTach", \
    "MCAST", \
    "IPMC", \
    "FieldProcessor", \
    "Filter", \
    "API", \
    "Policer", \
    "COsq", \
    "OAM", \
    "MPLS" 

/* bcm_debug_level */
extern uint32 bcm_debug_level;

/* bcm_debug_names */
extern char *bcm_debug_names[];

/* bcm_debug_print */
extern int (*bcm_debug_print)(const char *format, ... );

/* _bcm_debug_api */
extern void _bcm_debug_api(
    char *api, 
    int nargs, 
    int ninargs, 
    int arg1, 
    int arg2, 
    int arg3, 
    int rv);

#if defined(NO_DEBUG_OUTPUT_DEFAULT)
#define BCM_DBG_DEFAULT         0          
#else
#define BCM_DBG_DEFAULT         (BCM_DBG_ERR | BCM_DBG_WARN | BCM_DBG_NORMAL) 
#endif

#if defined(BROADCOM_DEBUG)
#define BCM_DEBUG_CHECK(flags)  \
    (((flags) & bcm_debug_level) == (flags)) 
#define BCM_DEBUG(flags, stuff)  \
    if (BCM_DEBUG_CHECK(flags) && bcm_debug_print != 0) \
	(*bcm_debug_print) stuff 
#define BCM_WARN(stuff)         \
    BCM_DEBUG(BCM_DBG_WARN, stuff) 
#define BCM_ERR(stuff)          \
    BCM_DEBUG(BCM_DBG_ERR, stuff) 
#define BCM_VERB(stuff)         \
    BCM_DEBUG(BCM_DBG_VERBOSE, stuff) 
#define BCM_API(_1, _2, _3, _4, _5, _6, _7)  \
    do { \
        if (BCM_DEBUG_CHECK(BCM_DBG_API)) \
           _bcm_debug_api(_1,_2,_3,_4,_5,_6,_7); \
    } while (0) 
#else
#define BCM_DEBUG_CHECK(flags)  0          
#define BCM_DEBUG(flags, stuff)             /* BCM_DEBUG_CHECK */
#define BCM_WARN(stuff)                    
#define BCM_ERR(stuff)                     
#define BCM_VERB(stuff)                    
#define BCM_API(_1, _2, _3, _4, _5, _6, _7)             
#endif

#endif /* __BCM_DEBUG_H__ */
