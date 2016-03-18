/*
 * $Id: ipmc.h,v 1.18.6.2 Broadcom SDK $
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
 * File:	include/bcmx/ipmc.h
 * Purpose:	BCMX IP Multicast APIs
 */

#ifndef	_BCMX_IPMC_H
#define	_BCMX_IPMC_H

#ifdef	INCLUDE_L3

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include <bcm/ipmc.h>

typedef struct bcmx_ipmc_addr_s {
    bcm_ip_t		s_ip_addr;
    bcm_ip_t		mc_ip_addr;
    bcm_ip6_t		s_ip6_addr;	/* IPv6 Source address */
    bcm_ip6_t		mc_ip6_addr;	/* IPv6 Destination address */
    bcm_vlan_t		vid;
    bcm_vrf_t		vrf;		/* Virtual Router */
    bcm_cos_t		cos;
    bcmx_lplist_t	l2_ports;
    bcmx_lplist_t	l2_untag_ports;
    bcmx_lplist_t	l3_ports;
    int			ts;
    bcmx_lport_t	port_tgid;
    int			ttl;
    int			v;
    int			cd;
    int			ipmc_index;
    uint32		flags;
    int         lookup_class;     /* Classification lookup class id. */
} bcmx_ipmc_addr_t;

extern int bcmx_ipmc_init(void);
extern int bcmx_ipmc_detach(void);
extern int bcmx_ipmc_enable(int enable);
extern int bcmx_ipmc_source_port_check(int enable);
extern int bcmx_ipmc_source_ip_search(int enable);
extern void bcmx_ipmc_addr_t_init(bcmx_ipmc_addr_t * data, bcm_ip_t s_ip_addr,
                                  bcm_ip_t mc_ip_addr, bcm_vlan_t vid);
#define bcmx_ipmc_addr_init bcmx_ipmc_addr_t_init	/* compat. */
extern void bcmx_ipmc_addr_t_free(bcmx_ipmc_addr_t * data);
#define bcmx_ipmc_addr_free bcmx_ipmc_addr_t_free	/* compat. */

extern int bcmx_ipmc_add(bcmx_ipmc_addr_t * data);
extern int bcmx_ipmc_find(bcmx_ipmc_addr_t *data);
extern int bcmx_ipmc_remove(bcmx_ipmc_addr_t *data);
extern int bcmx_ipmc_remove_all(void);

extern int bcmx_ipmc_delete(bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr, 
                            bcm_vlan_t vid, int keep);
extern int bcmx_ipmc_delete_all(void);
extern int bcmx_ipmc_get(bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr, 
                         bcm_vlan_t vid, bcmx_ipmc_addr_t * data);
extern int bcmx_ipmc_get_by_index(int index, bcmx_ipmc_addr_t * data);
extern int bcmx_ipmc_entry_enable_set(bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr, 
                                      bcm_vlan_t vid, int enable);
extern int bcmx_ipmc_sport_mismatch_action(bcm_ip_t s_ip_addr, 
                                           bcm_ip_t mc_ip_addr, 
                                           bcm_vlan_t vid, 
                                           int cd);
extern int bcmx_ipmc_cos_set(bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr, 
                             bcm_vlan_t vid, int cos);
extern int bcmx_ipmc_ttl_threshold_set(bcm_ip_t s_ip_addr, 
                                       bcm_ip_t mc_ip_addr, 
                                       bcm_vlan_t vid, 
                                       int ttl_threshold);
extern int bcmx_ipmc_port_tgid_set(bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr, 
                                   bcm_vlan_t vid, int ts, int port_tgid);

extern int bcmx_ipmc_add_l2_ports(bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr, 
                                  bcm_vlan_t vid, bcmx_lplist_t lplist, 
                                  bcmx_lplist_t ut_lplist);
extern int bcmx_ipmc_delete_l2_ports(bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr, 
                                     bcm_vlan_t vid, bcmx_lport_t port);
extern int bcmx_ipmc_add_l3_ports(bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr, 
                                  bcm_vlan_t vid, bcmx_lplist_t lplist);
extern int bcmx_ipmc_delete_l3_port(bcm_ip_t s_ip_addr, bcm_ip_t mc_ip_addr, 
                                    bcm_vlan_t vid, bcmx_lport_t port);

extern int bcmx_ipmc_egress_port_init(void);
extern int bcmx_ipmc_egress_port_set(bcmx_lport_t port, const bcm_mac_t mac, 
                                     int untag, bcm_vlan_t vid, 
                                     int ttl_threshold);
extern int bcmx_ipmc_egress_port_get(bcmx_lport_t port, bcm_mac_t mac, 
                                     int * untag, bcm_vlan_t * vid, 
                                     int * ttl_threshold);
extern int bcmx_ipmc_egress_intf_add(int group, 
                                     bcmx_lport_t port, 
                                     bcm_l3_intf_t *l3_intf);
extern int bcmx_ipmc_egress_intf_delete(int group, 
                                        bcmx_lport_t port, 
                                        bcm_l3_intf_t *l3_intf);
extern int bcmx_ipmc_egress_intf_delete_all(int group, bcmx_lport_t port);
extern int bcmx_ipmc_egress_intf_set(int group, bcmx_lport_t port,
				     int if_count, bcm_if_t *if_array);
extern int bcmx_ipmc_egress_intf_get(int group, bcmx_lport_t port,
				     int if_max, bcm_if_t *if_array,
				     int *if_count);
                          
extern int bcmx_ipmc_counters_get(bcmx_lport_t port, 
                                  bcm_ipmc_counters_t * counters);

extern int bcmx_ipmc_repl_get(int index, bcmx_lport_t port, 
                              bcm_vlan_vector_t vlan_vec);
extern int bcmx_ipmc_repl_add(int index, bcmx_lplist_t lplist, 
                              bcm_vlan_t vlan);
extern int bcmx_ipmc_repl_delete(int index, bcmx_lplist_t lplist, 
                                 bcm_vlan_t vlan);
extern int bcmx_ipmc_repl_delete_all(int index, bcmx_lplist_t lplist);
extern int bcmx_ipmc_repl_set(int index, bcmx_lport_t port,
			      bcm_vlan_vector_t vlan_vec);

extern int bcmx_ipmc_age(uint32 flags,
                         bcm_ipmc_traverse_cb age_cb, void *user_data);
extern int bcmx_ipmc_traverse(uint32 flags,
                              bcm_ipmc_traverse_cb cb, void *user_data);
#endif	/* INCLUDE_L3 */

#endif	/* _BCMX_IPMC_H */
