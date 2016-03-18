/*
 * $Id: debug.h,v 1.8 Broadcom SDK $
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
 * File:        debug.h
 * Purpose:     Debug flag definitions for the bcm directory
 */

#ifndef   _BCMX_DEBUG_H_
#define   _BCMX_DEBUG_H_

#include <bcm/types.h>

#if defined(BROADCOM_DEBUG)

/*
 * These are the possible debug types/flags.
 */

#define BCMX_DBG_NORMAL    (1 << 0)    /* Normal output */
#define BCMX_DBG_ERR       (1 << 1)    /* Print errors */
#define BCMX_DBG_WARN      (1 << 2)    /* Print warnings */
#define BCMX_DBG_VERBOSE   (1 << 3)    /* General verbose output */
#define BCMX_DBG_VVERBOSE  (1 << 4)    /* Very verbose output */
#define BCMX_DBG_PORT      (1 << 5)    /* Port operations */
#define BCMX_DBG_VLAN      (1 << 6)    /* VLAN operations */
#define BCMX_DBG_L2        (1 << 7)   
#define BCMX_DBG_L3        (1 << 8)    /* L3, DefIP and IPMC operations */
#define BCMX_DBG_TX        (1 << 9)    /* Packet transmit */
#define BCMX_DBG_RX        (1 << 10)    /* Packet Receive */
#define BCMX_DBG_PKT_DATA  (1 << 11)   /* Display packet data */
#define BCMX_DBG_LINK      (1 << 12)   /* Show Link status changes */
#define BCMX_DBG_COUNTER   (1 << 13)   /* Counter operations */
#define BCMX_DBG_ATTACH    (1 << 14)   /* Unit maintence */
#define BCMX_DBG_COUNT     15

#define BCMX_DBG_NAMES   \
    "NORmal",            \
    "ERRor",             \
    "WARN",              \
    "VERbose",           \
    "VVERbose",          \
    "POrt",              \
    "VLAN",              \
    "L2",                \
    "L3",                \
    "TX",                \
    "RX",                \
    "PKTdata",           \
    "LInk",              \
    "CounTeR",           \
    "ATTach"

/*
 * Proper use requires parentheses.  E.g.:
 *     BCMX_DEBUG(BCM_DBG_FOO, ("Problem %d with unit %d\n", pr, unit));
 */

extern uint32 bcmx_debug_level;
extern char *bcmx_debug_names[];
extern int (*bcmx_debug_print)(const char *format, ...);

#if defined(NO_DEBUG_OUTPUT_DEFAULT)
#define BCMX_DBG_DEFAULT 0
#else
#define BCMX_DBG_DEFAULT (BCMX_DBG_ERR | BCMX_DBG_WARN)
#endif

#define BCMX_DEBUG_CHECK(flags) (((flags) & bcmx_debug_level) == (flags))
#define BCMX_DEBUG(flags, stuff) \
    if (BCMX_DEBUG_CHECK(flags) && bcmx_debug_print != 0) \
	(*bcmx_debug_print) stuff

#else

#define BCMX_DEBUG_CHECK(flags) 0
#define BCMX_DEBUG(flags, stuff)

#endif /* defined(BROADCOM_DEBUG) */


#endif /* _BCMX_DEBUG_H_ */
