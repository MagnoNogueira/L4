/*
 * $Id: l2.h,v 1.22.6.1 Broadcom SDK $
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
 * File:       bcmx/l2.h
 * Purpose:    BCMX L2 APIs
 */

#ifndef _BCMX_L2_H
#define _BCMX_L2_H

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lport.h>
#include <bcmx/lplist.h>
#include <bcm/l2.h>

typedef struct bcmx_l2_addr_s {
        uint32          flags;
        bcm_mac_t       mac;
        bcm_vlan_t      vid;
        bcmx_lport_t    lport;
        bcm_trunk_t     tgid;
        int             trunk_sel;
        bcm_cos_t       cos_dst;
        bcm_cos_t       cos_src;
        int             l2mc_index;
        int             auth;
        /*
         * The bcm_unit number can be used to record the "reporting"
         * unit for adds and deletes.  This may differ from the
         * unit associated with the lport.
         */
        int             bcm_unit;
        int             group;    /* Group number for FP */
} bcmx_l2_addr_t;

typedef struct bcmx_l2_cache_addr_s {
    uint32          flags;
    bcm_mac_t       mac;
    bcm_mac_t       mac_mask;
    bcm_vlan_t      vlan;
    bcm_vlan_t      vlan_mask;
    bcmx_lport_t    src_port;
    bcmx_lport_t    src_port_mask;
    bcmx_lport_t    dest_lport;
    bcm_trunk_t     dest_trunk;
    int             prio;
    bcmx_lplist_t   dest_ports;
    int             lookup_class;  /* Classification class id. */
} bcmx_l2_cache_addr_t;

/*
 * L2 MAC address learning limit
 */
typedef struct bcmx_l2_learn_limit_s {
    uint32        flags;   /* BCM_L2_LEARN_LIMIT_xxx */
    bcm_vlan_t    vlan;    /* vlan identifier */
    bcm_vpn_t     vpn;     /* vpn identifier */
    bcmx_lport_t  lport;   /* logical port number */
    bcm_trunk_t   trunk;   /* trunk identifier */
    int           limit;   /* max number of learned entry, -1 for unlimit */
} bcmx_l2_learn_limit_t;

/*
 * L2 Notification Callback typedef
 *     insert - Boolean: Was L2 addr inserted or deleted?
 */
typedef void (*bcmx_l2_notify_f)(bcmx_l2_addr_t *addr, int insert,
                                 void *cookie);

extern void bcmx_l2_addr_t_init(bcmx_l2_addr_t * l2addr,
                                bcm_mac_t mac_addr,
                                bcm_vlan_t vid);
extern void bcmx_l2_learn_limit_t_init(bcmx_l2_learn_limit_t *limit);

#define bcmx_l2_addr_init bcmx_l2_addr_t_init    /* compat. */

extern int bcmx_l2_addr_add(bcmx_l2_addr_t * l2addr,
                            bcmx_lplist_t *port_block);

extern int bcmx_l2_addr_delete(bcm_mac_t mac, bcm_vlan_t vid);
extern int bcmx_l2_addr_delete_by_port(bcmx_lport_t port, uint32 flags);
extern int bcmx_l2_addr_delete_by_mac(bcm_mac_t mac, uint32 flags);
extern int bcmx_l2_addr_delete_by_vlan(bcm_vlan_t vid, uint32 flags);
extern int bcmx_l2_addr_delete_by_trunk(bcm_trunk_t tid, uint32 flags);
extern int bcmx_l2_addr_delete_by_mac_port(bcm_mac_t mac, bcmx_lport_t port,
                                           uint32 flags);
extern int bcmx_l2_addr_delete_by_vlan_port(bcm_vlan_t vid, bcmx_lport_t port,
                                            uint32 flags);
extern int bcmx_l2_addr_delete_by_vlan_trunk(bcm_vlan_t vid, bcm_trunk_t tid,
                                             uint32 flags);

extern int bcmx_l2_addr_get(bcm_mac_t mac_addr,
                            bcm_vlan_t vid,
                            bcmx_l2_addr_t * l2addr,
                            bcmx_lplist_t *port_block);

extern int bcmx_l2_age_timer_set(int age_seconds);
extern int bcmx_l2_age_timer_get(int * age_seconds);
extern int bcmx_l2_bpdu_count(void);
extern int bcmx_l2_bpdu_set(int index, bcm_mac_t addr);
extern int bcmx_l2_bpdu_get(int index, bcm_mac_t * addr);
extern int bcmx_l2_addr_freeze(void);
extern int bcmx_l2_addr_thaw(void);
extern int bcmx_l2_conflict_get(bcmx_l2_addr_t * addr,
                                bcmx_l2_addr_t * cf_array,
                                int cf_max,
                                int * cf_count);

extern int bcmx_l2_notify_register(bcmx_l2_notify_f callback,
                                   void *userdata);
extern int bcmx_l2_notify_unregister(bcmx_l2_notify_f callback,
                                     void *userdata);
extern int bcmx_l2_notify_start(void);
extern int bcmx_l2_notify_stop(void);
extern int bcmx_l2_device_add(int bcm_unit);
extern int bcmx_l2_device_remove(int bcm_unit);

extern int bcmx_l2_addr_to_bcm(int bcm_unit,
                               bcm_l2_addr_t *dest,
                               bcmx_l2_addr_t *source,
                               bcmx_lplist_t *port_block);
extern int bcmx_l2_addr_from_bcm(bcmx_l2_addr_t *dest,
                                 bcmx_lplist_t *port_block,
                                 bcm_l2_addr_t *source);
extern int bcmx_l2_addr_refresh(bcmx_l2_addr_t * l2addr,
                                bcmx_lplist_t *port_block);
extern int bcmx_l2_addr_native_get(bcm_mac_t mac_addr, bcm_vlan_t vid,
                                   bcmx_l2_addr_t *l2addr);
extern int bcmx_l2_addr_port_get(bcmx_lport_t port,
                                 bcm_mac_t mac_addr, bcm_vlan_t vid,
                                 bcmx_l2_addr_t *l2addr);

extern int bcmx_l2_cache_init(void);
extern void bcmx_l2_cache_addr_t_init(bcmx_l2_cache_addr_t * addr);
extern void bcmx_l2_cache_addr_t_free(bcmx_l2_cache_addr_t * addr);
extern int bcmx_l2_cache_size_get(int * size);
extern int bcmx_l2_cache_set(int index,
                             bcmx_l2_cache_addr_t * addr,
                             int * index_used);
extern int bcmx_l2_cache_get(int index,
                             bcmx_l2_cache_addr_t * addr);
extern int bcmx_l2_cache_delete(int index);
extern int bcmx_l2_cache_delete_all(void);
extern int bcmx_l2_tunnel_add(bcm_mac_t mac, bcm_vlan_t vlan);
extern int bcmx_l2_tunnel_delete(bcm_mac_t mac, bcm_vlan_t vlan);
extern int bcmx_l2_tunnel_delete_all(void);

/*
 * L2 MAC address learning limit
 */
extern int bcmx_l2_learn_limit_set(bcmx_l2_learn_limit_t *limit);
extern int bcmx_l2_learn_limit_get(bcmx_l2_learn_limit_t *limit);

/*
 * L2 MAC address learning class based movement
 */
extern int bcmx_l2_learn_class_set(int lclass, int lclass_prio, uint32 flags);
extern int bcmx_l2_learn_class_get(int lclass, int *lclass_prio, uint32 *flags);
extern int bcmx_l2_learn_port_class_set(bcmx_lport_t port, int lclass);
extern int bcmx_l2_learn_port_class_get(bcmx_lport_t port, int *lclass);


/*
 * Replace L2 entries matching given criteria
 * Flags: BCM_L2_REPLACE_XXX (see include/bcm/l2.h for flag definitions)
 */
extern int bcmx_l2_replace(uint32 flags, bcmx_l2_addr_t *match_addr, 
                           bcmx_lport_t new_port, bcm_trunk_t new_trunk);


#endif  /* _BCMX_L2_H */
