/*
 * $Id: trunk.h,v 1.9 Broadcom SDK $
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
 * File:    bcmx/trunk.h
 * Purpose: BCMX Trunk APIs
 */

#ifndef _BCMX_TRUNK_H
#define _BCMX_TRUNK_H

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include <bcm/trunk.h>

typedef struct bcmx_trunk_add_info_s {
    int            psc;
    bcmx_lport_t   dlf_port;
    bcmx_lport_t   mc_port;
    bcmx_lport_t   ipmc_port;
    bcmx_lplist_t  ports;
} bcmx_trunk_add_info_t;

extern void bcmx_trunk_add_info_t_init(bcmx_trunk_add_info_t *trunk_add_info);
extern void bcmx_trunk_add_info_t_free(bcmx_trunk_add_info_t *trunk_add_info);
extern int bcmx_trunk_init(void);
extern int bcmx_trunk_detach(void);
extern int bcmx_trunk_create(bcm_trunk_t *tid);
extern int bcmx_trunk_create_id(bcm_trunk_t tid);
#define bcmx_trunk_create_with_tid  bcmx_trunk_create_id  /* compat. */
extern int bcmx_trunk_psc_set(bcm_trunk_t tid, int psc);
extern int bcmx_trunk_psc_get(bcm_trunk_t tid, int *psc);
extern int bcmx_trunk_chip_info_get(bcm_trunk_chip_info_t * ta_info);
extern int bcmx_trunk_set(bcm_trunk_t tid, bcmx_trunk_add_info_t *add_info);
extern int bcmx_trunk_destroy(bcm_trunk_t tid);
extern int bcmx_trunk_get(bcm_trunk_t tid, bcmx_trunk_add_info_t *t_data);
extern int bcmx_trunk_mcast_join(bcm_trunk_t tid, bcm_vlan_t vid,
                                 bcm_mac_t mac);
extern int bcmx_trunk_egress_set(bcm_trunk_t tid, bcmx_lplist_t lplist);
extern int bcmx_trunk_egress_get(bcm_trunk_t tid, bcmx_lplist_t *lplist);
extern int bcmx_trunk_find(bcmx_lport_t port, bcm_trunk_t *tid);
#endif  /* _BCMX_TRUNK_H */
