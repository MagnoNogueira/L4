/*
 * $Id: l3.h,v 1.47 Broadcom SDK $
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
 * File:    include/bcmx/l3.h
 * Purpose: BCMX L3 APIs
 */

#ifndef _BCMX_L3_H
#define _BCMX_L3_H

#ifdef INCLUDE_L3

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include <bcm/l3.h>

typedef bcm_l3_intf_t bcmx_l3_intf_t;

typedef struct bcmx_l3_egress_s {
    uint32           flags;     /* Interface flags(BCM_L3_TGID,BCM_L3_L2TOCPU)*/
    bcm_if_t         intf;      /* L3 interface (src mac, tunnel)             */
    bcm_mac_t        mac_addr;  /* Next hop forwarding destination mac.       */
    bcm_vlan_t       vlan;      /* Next hop vlan id.                          */
    bcmx_lport_t     lport;     /* Port packet switched to. (if !BCM_L3_TGID) */
    bcm_trunk_t      trunk;     /* Trunk packet switched to.(if BCM_L3_TGID)  */
    bcm_mpls_label_t mpls_label;/* MPLS label.                                */
} bcmx_l3_egress_t;

typedef struct bcmx_l3_host_s {
    uint32         l3a_flags;
    bcm_vrf_t      l3a_vrf;           /* Virtual router identifier */
    bcm_ip_t       l3a_ip_addr;       /* IP address (IPv4) */
    bcm_ip6_t      l3a_ip6_addr;      /* IP address (IPv6) */
    bcm_cos_t      l3a_pri;           /* New priority in packet */
    bcm_if_t       l3a_intf;          /* Interface ID */
    bcm_mac_t      l3a_nexthop_mac;
    bcmx_lport_t   l3a_lport;         /* if !(flags & BCM_L3_TGID) */
    bcm_trunk_t    l3a_trunk;         /* if  (flags & BCM_L3_TGID) */
    int            l3a_ipmc_ptr;
    int            l3a_lookup_class; /* Classification lookup class id. */
} bcmx_l3_host_t;

typedef bcmx_l3_host_t bcmx_l3_ip_t;    /* Backward compat. */

typedef struct bcmx_l3_route_s {
    uint32         l3a_flags;
    bcm_vrf_t      l3a_vrf;           /* Virtual router instance */
    bcm_ip_t       l3a_subnet;        /* Subnet address (IPv4) */
    bcm_ip6_t      l3a_ip6_net;       /* Subnet address (IPv6) */
    bcm_ip_t       l3a_ip_mask;       /* Subnet mask (IPv4) */
    bcm_ip6_t      l3a_ip6_mask;      /* Subnet mask (IPv4) */
    bcm_if_t       l3a_intf;
    bcm_mac_t      l3a_nexthop_mac;
    bcm_ip_t       l3a_nexthop_ip;    /* Next hop IP address (IPv4) */
    bcmx_lport_t   l3a_lport;         /* if !(flags & BCM_L3_TGID) */
    bcm_trunk_t    l3a_trunk;         /* if  (flags & BCM_L3_TGID) */
    bcm_vlan_t     l3a_vid;
    bcm_cos_t      l3a_pri;           /* Priority (COS) */
    uint32         l3a_tunnel_option; /* Tunnel option value. */ 
    bcm_mpls_label_t l3a_mpls_label;  /* MPLS label.          */
    int            l3a_lookup_class; /* Classification lookup class id. */
} bcmx_l3_route_t;

typedef bcm_l3_info_t bcmx_l3_info_t;
typedef bcm_l3_key_t bcmx_l3_key_t;

/*
 * L3 general APIs
 */

extern int bcmx_l3_init(void);
extern int bcmx_l3_cleanup(void);
extern int bcmx_l3_enable_set(int enable);
extern int bcmx_l3_info(bcm_l3_info_t * l3info);
extern int bcmx_l3_untagged_update(void);
extern int bcmx_l3_host_t_init(bcmx_l3_host_t *ip);
extern int bcmx_l3_route_t_init(bcmx_l3_route_t *route);
extern int bcmx_l3_intf_t_init(bcmx_l3_intf_t *intf);
extern void bcmx_l3_egress_t_init(bcmx_l3_egress_t *egr);

/*
 * L3 interface related APIs
 */

extern int bcmx_l3_intf_create(bcm_l3_intf_t *intf);
extern int bcmx_l3_intf_delete(bcm_l3_intf_t *intf);
extern int bcmx_l3_intf_delete_all(void);
extern int bcmx_l3_intf_find(bcm_l3_intf_t *intf);
extern int bcmx_l3_intf_find_vlan(bcm_l3_intf_t *intf);
extern int bcmx_l3_intf_get(bcm_l3_intf_t *intf);

/* 
 *  Egress object API's
 */

extern int bcmx_l3_egress_create(uint32 flags, bcmx_l3_egress_t *egr,
                                 bcm_if_t *intf);
extern int bcmx_l3_egress_destroy(bcm_if_t intf);
extern int bcmx_l3_egress_get(bcm_if_t intf, bcmx_l3_egress_t *egr);
extern int bcmx_l3_egress_find(bcmx_l3_egress_t *egr, bcm_if_t *intf);
extern int bcmx_l3_egress_traverse(bcm_l3_egress_traverse_cb trav_fn,
                                   void *user_data);
extern int bcmx_l3_egress_multipath_create(uint32 flags, int intf_count,
                                           bcm_if_t *intf_array,
                                           bcm_if_t *mpintf);
extern int bcmx_l3_egress_multipath_destroy(bcm_if_t mpintf);
extern int bcmx_l3_egress_multipath_get(bcm_if_t mpintf, int intf_size,
                                        bcm_if_t *intf_array, 
                                        int *intf_count);
extern int bcmx_l3_egress_multipath_add(bcm_if_t mpintf, bcm_if_t intf);
extern int bcmx_l3_egress_multipath_delete(bcm_if_t mpintf, bcm_if_t intf);
extern int bcmx_l3_egress_multipath_find(int intf_count, bcm_if_t *intf_array,
                                         bcm_if_t *mpintf);
extern int bcmx_l3_egress_multipath_traverse(
                                 bcm_l3_egress_multipath_traverse_cb trav_fn,
                                 void *user_data);

/*
 *  IPv6 prefix map APIs.
 */
extern int bcmx_l3_ip6_prefix_map_get(int map_size, 
                                      bcm_ip6_t *ip6_array, int *ip6_count);
extern int bcmx_l3_ip6_prefix_map_add(bcm_ip6_t ip6_addr);
extern int bcmx_l3_ip6_prefix_map_delete(bcm_ip6_t ip6_addr);
extern int bcmx_l3_ip6_prefix_map_delete_all(void);

/*
 * L3 Host IP table related APIs
 */

extern int bcmx_l3_host_find(bcmx_l3_host_t *info);
extern int bcmx_l3_host_add(bcmx_l3_host_t *info);
extern int bcmx_l3_host_delete(bcmx_l3_host_t *ip_addr);
extern int bcmx_l3_host_delete_by_network(bcmx_l3_route_t *ip_addr);
extern int bcmx_l3_host_delete_by_interface(bcmx_l3_host_t *info);
extern int bcmx_l3_host_delete_all(bcmx_l3_host_t *info);
extern int bcmx_l3_host_conflict_get(bcm_l3_key_t *ipkey,
                                     bcm_l3_key_t *cf_array, int cf_max,
                                     int *cf_count);
extern int bcmx_l3_host_age(uint32 flags,
                            bcm_l3_host_traverse_cb age_cb, void *user_data);
extern int bcmx_l3_host_traverse(uint32 flags,
                                 uint32 start, uint32 end,
                                 bcm_l3_host_traverse_cb cb, void *user_data);
extern int bcmx_l3_host_invalidate_entry(bcm_ip_t info);
extern int bcmx_l3_host_validate_entry(bcm_ip_t info);

/*
 * L3 IP routing table related APIs
 */

extern int bcmx_l3_route_add(bcmx_l3_route_t *info);
extern int bcmx_l3_route_delete(bcmx_l3_route_t *info);
extern int bcmx_l3_route_delete_all(bcmx_l3_route_t *info);
extern int bcmx_l3_route_get(bcmx_l3_route_t *info);
extern int bcmx_l3_route_multipath_get(bcmx_l3_route_t *the_route,
                                       bcmx_l3_route_t *path_array,
                                       int max_path, int *path_count);
extern int bcmx_l3_route_age(uint32 flags,
                             bcm_l3_route_traverse_cb age_out,
                             void *user_data);
extern int bcmx_l3_route_traverse(uint32 flags, uint32 start, uint32 end,
                                  bcm_l3_route_traverse_cb trav_fn,
                                  void *user_data);
extern int bcmx_l3_route_max_ecmp_set(int max);
extern int bcmx_l3_route_max_ecmp_get(int *max);

/*
 * Superseded APIs
 *
 *   These APIs support only IPv4 and may be removed in a future release.
 */
extern int bcmx_l3_interface_create(bcm_mac_t mac_addr, bcm_vlan_t vid,
                                    int add_to_arl);
extern int bcmx_l3_interface_id_create(bcm_mac_t mac_addr, bcm_vlan_t vid,
                                       int intf_idx, int add_to_arl);
extern int bcmx_l3_interface_id_update(bcm_mac_t mac_addr, bcm_vlan_t vid,
                                       int intf_idx, int add_to_arl);
extern int bcmx_l3_interface_destroy(int interface);
extern int bcmx_l3_interface_lookup(bcm_mac_t mac_addr,
                                    bcm_vlan_t vid, int *intf_id);
extern int bcmx_l3_interface_find(int interface, bcm_vlan_t *vid,
                                  bcm_mac_t mac_addr);
extern int bcmx_l3_interface_find_by_vlan(bcm_vlan_t vid, int *interface);
extern int bcmx_l3_ip_find(bcmx_l3_ip_t *info);
extern int bcmx_l3_ip_find_index(int index, bcmx_l3_ip_t *info);
extern int bcmx_l3_ip_add(bcmx_l3_ip_t *info);
extern int bcmx_l3_ip_delete(bcm_ip_t ip_addr);
extern int bcmx_l3_ip_delete_by_prefix(bcm_ip_t ip_addr, bcm_ip_t mask);
extern int bcmx_l3_ip_delete_by_interface(int intf);
extern int bcmx_l3_ip_delete_all(void);
extern int bcmx_l3_ip_update_entry_by_key(bcmx_l3_host_t *info);
extern int bcmx_l3_age(uint32 flags, bcm_l3_age_cb age_out);
extern int bcmx_l3_status(int *free_l3intf, int *free_l3,
                          int *free_defip, int *free_lpm_blk);
extern int bcmx_l3_conflict_get(bcm_l3_key_t *ipkey, bcm_l3_key_t *cf_array,
                                int cf_max, int *cf_count);
extern int bcmx_l3_invalidate_entry(bcm_ip_t l3_addr);
extern int bcmx_l3_validate_entry(bcm_ip_t l3_addr);
extern int bcmx_l3_defip_add(bcmx_l3_route_t *info);
extern int bcmx_l3_defip_delete(bcmx_l3_route_t *info);
extern int bcmx_l3_defip_get(bcmx_l3_route_t *info);
extern int bcmx_l3_defip_delete_by_interface(int intf);
extern int bcmx_l3_defip_delete_all(void);
extern int bcmx_l3_defip_age(bcm_l3_route_traverse_cb age_out);
extern int bcmx_l3_defip_max_ecmp_get(int *max);
extern int bcmx_l3_defip_max_ecmp_set(int max);

/* Definition for backwards compatibility */
#define bcmx_l3_enable    bcmx_l3_enable_set
#define bcmx_l3_defip_ecmp_max_get    bcmx_l3_defip_max_ecmp_get
#define bcmx_l3_defip_ecmp_max_set    bcmx_l3_defip_max_ecmp_set

#endif  /* INCLUDE_L3 */

#endif  /* !_BCMX_L3_H */
