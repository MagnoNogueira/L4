/*
 * $Id: rx.h,v 1.5 Broadcom SDK $
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
 * File:        rx.h
 * Purpose:     Internal structures and definitions for RX module
 */
#ifndef   _BCM_COMMON_RX_H_
#define   _BCM_COMMON_RX_H_

#include <sal/appl/sal.h>
#include <soc/dma.h>
#include <bcm/error.h>
#include <bcm/rx.h>

/*
 * Typedef:
 *      rx_callout_t
 * Purpose:
 *      Describes a registered callout for RX.
 * Notes:
 *      For each interface, a list of callouts is maintained in sorted
 *      order from highest priority to lowest. The callouts are called
 *      in order until one returns "bcm_rx_handled" or
 *      "bcm_rx_handled_owned". If all handlers called and none process
 *      the packet, the default "discard" handler is called.
 */

typedef struct rx_callout_s {
    volatile struct rx_callout_s *rco_next;      /* Next in sorted list */
    const char          *rco_name;      /* Name for handler */
    void                *rco_cookie;    /* Cookie for call out */
    bcm_rx_cb_f         rco_function;   /* Callout function */
    uint32              rco_flags;      /* RCO_F_  flags; see bcm/rx.h */
    uint8               rco_priority;   /* Callout priority 0 == lowest */
    uint32              rco_pkts_handled; /* Counter */
    uint32              rco_pkts_owned; /* Counter */
    SHR_BITDCL          rco_cos[_SHR_BITDCLSIZE(BCM_RX_COS)]; /* cos bitmap */
} rx_callout_t;

#define SETUP_RCO(rco, name, fun, pri, cookie, next, flags)   \
    do {                                                 \
        (rco)->rco_name = (name);                        \
        (rco)->rco_function = (fun);                     \
        (rco)->rco_priority = (pri);                     \
        (rco)->rco_cookie = (cookie);                    \
        (rco)->rco_next = (next);                        \
        (rco)->rco_flags = (flags);                      \
        (rco)->rco_pkts_owned = 0;                       \
        (rco)->rco_pkts_handled = 0;                     \
        sal_memset((void *)((rco)->rco_cos), 0,          \
                   SHR_BITALLOCSIZE(BCM_RX_COS));        \
    } while (0)

#define SETUP_RCO_COS_SET(rco, cos)                      \
    do {                                                 \
        SHR_BITSET((rco)->rco_cos, (cos));               \
    } while (0)

#define SETUP_RCO_COS_CLR(rco, cos)                      \
    do {                                                 \
        SHR_BITCLR((rco)->rco_cos, (cos));               \
    } while (0)

#endif
