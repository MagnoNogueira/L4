/*
 * $Id: strata.h,v 1.38 Broadcom SDK $
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
 * File:        strata.h
 * Purpose:     Function declarations for Strata bcm functions
 */

#ifndef _BCM_INT_STRATA_H_
#define _BCM_INT_STRATA_H_

#include <bcm_int/esw/mbcm.h>

/*
 * Routines for internal use
 */

extern int _bcm_l2_from_arl(int unit,
		     bcm_l2_addr_t *l2addr, arl_entry_t *arl_entry);
extern int _bcm_l2_to_arl(int unit,
		   arl_entry_t *arl_entry, bcm_l2_addr_t *l2addr);

/* Utility routines for metering and egress rate control */

extern uint8 _bcm_strata_kbits_to_bucketsize(int unit, bcm_port_t port, 
				      int ingress, uint32 kbits_burst);
extern uint32 _bcm_strata_bucketsize_to_kbits(int unit, bcm_port_t port,
				       int ingress, uint8 reg_val);
extern uint16 _bcm_strata_kbps_to_refreshcount(int unit, bcm_port_t port,
					int ingress, uint32 kbits_sec);
extern uint32 _bcm_strata_refreshcount_to_kbps(int unit, bcm_port_t port,
					int ingress, uint16 reg_val);

/****************************************************************
 *
 * Strata BCM functions
 *
 ****************************************************************/

extern int bcm_strata_l2_init(int unit);
extern int bcm_strata_l2_term(int unit);
extern int bcm_strata_l2_addr_get(int unit,
			   bcm_mac_t mac_addr, bcm_vlan_t vid,
			   bcm_l2_addr_t *l2addr);
extern int bcm_strata_l2_addr_add(int unit, bcm_l2_addr_t *l2addr);
extern int bcm_strata_l2_addr_delete(int, bcm_mac_t, bcm_vlan_t);
extern int bcm_strata_l2_addr_delete_by_port(int, bcm_module_t, bcm_port_t,
				      uint32);
extern int bcm_strata_l2_addr_delete_by_mac(int, bcm_mac_t, uint32);
extern int bcm_strata_l2_addr_delete_by_vlan(int, bcm_vlan_t, uint32);
extern int bcm_strata_l2_addr_delete_by_trunk(int, bcm_trunk_t, uint32);
extern int bcm_strata_l2_addr_delete_by_mac_port(int, bcm_mac_t,
					  bcm_module_t, bcm_port_t,
					  uint32);
extern int bcm_strata_l2_addr_delete_by_vlan_port(int, bcm_vlan_t,
					   bcm_module_t, bcm_port_t,
					   uint32);
extern int bcm_strata_l2_addr_delete_by_vlan_trunk(int, bcm_vlan_t,
                                                   bcm_trunk_t, uint32);
extern int bcm_strata_l2_addr_register(int, bcm_l2_addr_callback_t, void *);
extern int bcm_strata_l2_addr_unregister(int, bcm_l2_addr_callback_t, void *);
extern int bcm_strata_l2_conflict_get(int, bcm_l2_addr_t *, bcm_l2_addr_t *,
			       int, int *);

extern int bcm_strata_vlan_init(int, bcm_vlan_data_t *);
extern int bcm_strata_vlan_reload(int, vlist_t **, int *);
extern int bcm_strata_vlan_create(int, bcm_vlan_t);
extern int bcm_strata_vlan_destroy(int, bcm_vlan_t);
extern int bcm_strata_vlan_destroy_all(int);
extern int bcm_strata_vlan_port_add(int, bcm_vlan_t, bcm_pbmp_t, bcm_pbmp_t);
extern int bcm_strata_vlan_port_remove(int, bcm_vlan_t, bcm_pbmp_t);
extern int bcm_strata_vlan_port_get(int, bcm_vlan_t,
			     bcm_pbmp_t *pbmp, bcm_pbmp_t *ubmp);
extern int bcm_strata_vlan_of_port(int, bcm_port_t, vtable_entry_t *);
extern int bcm_strata_vlan_list(int, bcm_vlan_data_t **, int *);
extern int bcm_strata_vlan_stg_get(int, bcm_vlan_t, int *);
extern int bcm_strata_vlan_stg_set(int, bcm_vlan_t, int);

extern int bcm_strata_port_cfg_init(int, bcm_port_t port, bcm_vlan_data_t *);
extern int bcm_strata_port_cfg_get(int, bcm_port_t, bcm_port_cfg_t *);
extern int bcm_strata_port_cfg_set(int, bcm_port_t, bcm_port_cfg_t *);

extern int bcm_strata_trunk_set(int, bcm_trunk_t,
			 bcm_trunk_add_info_t *, trunk_private_t *);
extern int bcm_strata_trunk_get(int, bcm_trunk_t,
			 bcm_trunk_add_info_t *, trunk_private_t *);
extern int bcm_strata_trunk_destroy(int, bcm_trunk_t, trunk_private_t *);
extern int bcm_strata_trunk_mcast_join(int, bcm_trunk_t,
				bcm_vlan_t, bcm_mac_t,
				trunk_private_t *);

extern int bcm_strata_stg_stp_init(int, bcm_stg_t);
extern int bcm_strata_stg_stp_get(int, bcm_stg_t, bcm_port_t, int *);
extern int bcm_strata_stg_stp_set(int, bcm_stg_t, bcm_port_t, int);
extern int bcm_strata_mcast_addr_add(int, bcm_mcast_addr_t *);
extern int bcm_strata_mcast_addr_remove(int, bcm_mac_t, bcm_vlan_t);
extern int bcm_strata_mcast_port_get(int, bcm_mac_t, bcm_vlan_t, 
			      bcm_mcast_addr_t *);
extern int bcm_strata_mcast_init(int);
extern int bcm_strata_mcast_addr_init(bcm_mcast_addr_t *, bcm_mac_t,
			       bcm_vlan_t);
extern int bcm_strata_mcast_addr_add_w_l2mcindex(int unit,
					  bcm_mcast_addr_t *mcaddr);
extern int bcm_strata_mcast_addr_remove_w_l2mcindex(int unit, 
					     bcm_mcast_addr_t *mcaddr);
extern int bcm_strata_mcast_port_add(int unit, bcm_mcast_addr_t *mcaddr);
extern int bcm_strata_mcast_port_remove(int unit, bcm_mcast_addr_t *mcaddr);

extern int bcm_strata_cosq_init(int unit);
extern int bcm_strata_cosq_detach(int unit);
extern int bcm_strata_cosq_config_set(int unit, int numq);
extern int bcm_strata_cosq_config_get(int unit, int *numq);
extern int bcm_strata_cosq_mapping_set(int unit,
				       bcm_port_t port,
				       bcm_cos_t priority,
				       bcm_cos_queue_t cosq);
extern int bcm_strata_cosq_mapping_get(int unit,
				       bcm_port_t port,
				       bcm_cos_t priority,
				       bcm_cos_queue_t *cosq);
extern int bcm_strata_cosq_port_sched_set(int unit, bcm_pbmp_t, int mode,
				   const int weights[], int delay);
extern int bcm_strata_cosq_port_sched_get(int unit, bcm_pbmp_t, int *mode,
				   int weights[], int *delay);
extern int bcm_strata_cosq_sched_weight_max_get(int unit, int mode,
					 int *weight_max);
extern int bcm_strata_cosq_port_bandwidth_set(int unit, bcm_port_t port,
				       bcm_cos_queue_t cosq,
				       uint32 kbits_sec_min,
				       uint32 kbits_sec_max,
				       uint32 flags);
extern int bcm_strata_cosq_port_bandwidth_get(int unit, bcm_port_t port,
				       bcm_cos_queue_t cosq,
				       uint32 *kbits_sec_min,
				       uint32 *kbits_sec_max,
				       uint32 *flags);
extern int bcm_strata_cosq_discard_set(int unit, uint32 flags);
extern int bcm_strata_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_strata_cosq_discard_port_set(int unit, bcm_port_t port,
                                     bcm_cos_queue_t cosq,
                                     uint32 color,
                                     int drop_start,
                                     int drop_slope,
                                     int average_time);
extern int bcm_strata_cosq_discard_port_get(int unit, bcm_port_t port,
                                     bcm_cos_queue_t cosq,
                                     uint32 color,
                                     int *drop_start,
                                     int *drop_slope,
                                     int *average_time);

/* Meter functions */
extern int bcm_strata_meter_init(int unit);
extern int bcm_strata_meter_create(int unit, int port, int *mid);
extern int bcm_strata_meter_delete(int unit, int port, int mid);
extern int bcm_strata_meter_delete_all(int unit);
extern int bcm_strata_meter_get(int unit, int port, int mid, 
			 uint32 *kbits_sec, uint32 *kbits_burst);
extern int bcm_strata_meter_set(int unit, int port, int mid, 
			 uint32 kbits_sec, uint32 kbits_burst);
extern int bcm_strata_ffppacketcounter_set(int unit, int port, int mid, 
				    uint64 val);
extern int bcm_strata_ffppacketcounter_get(int unit, int port, int mid, 
				    uint64 *val);
extern int bcm_strata_ffpcounter_init(int unit);
extern int bcm_strata_ffpcounter_create(int unit, int port, int *ffpcounterid);
extern int bcm_strata_ffpcounter_delete(int unit, int port, int ffpcounterid);
extern int bcm_strata_ffpcounter_delete_all(int unit);
extern int bcm_strata_ffpcounter_set(int unit, int port, int ffpcounterid,
			      uint64 val);
extern int bcm_strata_ffpcounter_get(int unit, int port, int ffpcounterid,
			      uint64 *val);
extern int bcm_strata_port_rate_egress_set(int unit, bcm_port_t port,
				    uint32 kbits_sec,
				    uint32 kbits_burst);
extern int bcm_strata_port_rate_egress_get(int unit, bcm_port_t port,
				    uint32 *kbits_sec,
				    uint32 *kbits_burst);


#ifdef INCLUDE_L3
extern int bcm_strata_l3_tables_init(int);
extern int bcm_strata_l3_tables_cleanup(int);
extern int bcm_strata_l3_enable(int, int);

extern int bcm_strata_l3_intf_get(int, _bcm_l3_intf_cfg_t *);
extern int bcm_strata_l3_intf_get_by_vid(int, _bcm_l3_intf_cfg_t *);
extern int bcm_strata_l3_intf_create(int, _bcm_l3_intf_cfg_t *);
extern int bcm_strata_l3_intf_id_create(int, _bcm_l3_intf_cfg_t *);
extern int bcm_strata_l3_intf_lookup(int, _bcm_l3_intf_cfg_t *);
extern int bcm_strata_l3_intf_del(int, _bcm_l3_intf_cfg_t *);
extern int bcm_strata_l3_intf_del_all(int);

extern int bcm_strata_l3_tables_entries(int);

extern int bcm_strata_l3_get(int, _bcm_l3_cfg_t *);
extern int bcm_strata_l3_get_by_index(int, int, _bcm_l3_cfg_t *);
extern int bcm_strata_l3_add(int, _bcm_l3_cfg_t *);
extern int bcm_strata_l3_del(int, _bcm_l3_cfg_t *);
extern int bcm_strata_l3_del_prefix(int, _bcm_l3_cfg_t *);
extern int bcm_strata_l3_del_intf(int, _bcm_l3_cfg_t *, int);
extern int bcm_strata_l3_del_all(int);
extern int bcm_strata_l3_replace(int, _bcm_l3_cfg_t *);
extern int bcm_strata_l3_age(int unit, uint32 flags, bcm_l3_host_traverse_cb
                             age_out, void *);
extern int bcm_strata_l3_traverse(int unit, uint32 start, uint32 end,
                      bcm_l3_host_traverse_cb cb, void *user_data);

extern int bcm_strata_l3_ip6_get(int, _bcm_l3_cfg_t *);
extern int bcm_strata_l3_ip6_add(int, _bcm_l3_cfg_t *);
extern int bcm_strata_l3_ip6_del(int, _bcm_l3_cfg_t *);
extern int bcm_strata_l3_ip6_del_prefix(int, _bcm_l3_cfg_t *);
extern int bcm_strata_l3_ip6_replace(int, _bcm_l3_cfg_t *);
extern int bcm_strata_l3_ip6_traverse(int unit, uint32 start, uint32 end,
                      bcm_l3_host_traverse_cb trav_fn, void *user_data);

extern int bcm_strata_defip_cfg_get(int, _bcm_defip_cfg_t *);
extern int bcm_strata_defip_ecmp_get_all(int, _bcm_defip_cfg_t *,
                                  bcm_l3_route_t *, int, int *);
extern int bcm_strata_defip_add(int, _bcm_defip_cfg_t *);
extern int bcm_strata_defip_del(int, _bcm_defip_cfg_t *);
extern int bcm_strata_defip_del_intf(int, _bcm_defip_cfg_t *, int);
extern int bcm_strata_defip_del_all(int);
extern int bcm_strata_defip_find_index(int unit, int index, bcm_l3_route_t *info);
extern int bcm_strata_defip_age(int unit, bcm_l3_route_traverse_cb age_out, 
                                void *);
extern int bcm_strata_defip_traverse(int unit, uint32 start, uint32 end,
                      bcm_l3_route_traverse_cb trav_fn, void *user_data);

extern int bcm_strata_ip6_defip_cfg_get(int, _bcm_defip_cfg_t *);
extern int bcm_strata_ip6_defip_ecmp_get_all(int, _bcm_defip_cfg_t *,
                                  bcm_l3_route_t *, int, int *);
extern int bcm_strata_ip6_defip_add(int, _bcm_defip_cfg_t *);
extern int bcm_strata_ip6_defip_del(int, _bcm_defip_cfg_t *);
extern int bcm_strata_ip6_defip_traverse(int unit, uint32 start, uint32 end,
                      bcm_l3_route_traverse_cb trav_fn, void *user_data);

extern int bcm_strata_l3_conflict_get(int unit, bcm_l3_key_t *ipkey,
				      bcm_l3_key_t *cf_array,
				      int cf_max, int *cf_count);
extern int bcm_strata_l3_info(int unit, bcm_l3_info_t *l3_info);

extern int bcm_strata_ipmc_init(int unit);
extern int bcm_strata_ipmc_detach(int unit);
extern int bcm_strata_ipmc_get(int unit, int index, bcm_ipmc_addr_t *ipmc);
extern int bcm_strata_ipmc_lookup(int unit, int *index, bcm_ipmc_addr_t *ipmc);
extern int bcm_strata_ipmc_add(int unit, bcm_ipmc_addr_t *ipmc);
extern int bcm_strata_ipmc_put(int unit, int index, bcm_ipmc_addr_t *ipmc);
extern int bcm_strata_ipmc_delete(int unit, bcm_ipmc_addr_t *ipmc);
extern int bcm_strata_ipmc_delete_all(int unit);
extern int bcm_strata_ipmc_enable(int unit, int enable);
extern int bcm_strata_ipmc_src_port_check(int unit, int enable);
extern int bcm_strata_ipmc_src_ip_search(int unit, int enable);
extern int bcm_strata_ipmc_egress_port_set(int unit, bcm_port_t port, 
				    const bcm_mac_t mac,  int untag, 
				    bcm_vlan_t vid, int ttl_threshold);
extern int bcm_strata_ipmc_egress_port_get(int unit, bcm_port_t port, 
				    mac_addr_t mac, int *untag, 
				    bcm_vlan_t *vid, 
				    int *ttl_threshold);
extern int bcm_strata_ipmc_repl_init(int unit);
extern int bcm_strata_ipmc_repl_detach(int unit);
extern int bcm_strata_ipmc_repl_get(int unit, int index, bcm_port_t port, 
			     bcm_vlan_vector_t vlan_vec);
extern int bcm_strata_ipmc_repl_add(int unit, int index, bcm_port_t port, 
			     bcm_vlan_t vlan);
extern int bcm_strata_ipmc_repl_delete(int unit, int index, bcm_port_t port, 
				bcm_vlan_t vlan);
extern int bcm_strata_ipmc_repl_delete_all(int unit, int index, 
				    bcm_port_t port);
extern int bcm_strata_ipmc_egress_intf_add(int unit, int index, bcm_port_t port, 
			            bcm_l3_intf_t *l3_intf);
extern int bcm_strata_ipmc_egress_intf_delete(int unit, int index, bcm_port_t port, 
			               bcm_l3_intf_t *l3_intf);
extern int bcm_strata_ipmc_traverse(int unit, uint32 flags, bcm_ipmc_traverse_cb cb, 
                                    void *user_data);
extern int bcm_strata_ipmc_age(int unit, uint32 flags, bcm_ipmc_traverse_cb age_cb, 
                               void *user_data);

extern int bcm_strata_lpm_defip_sw_tbl_empty(int unit, int *count);
extern int bcm_strata_lpm_defip_hw_tbl_empty(int unit, int *count);
extern int bcm_strata_lpm_check_table(int unit);
extern int bcm_strata_lpm_delete(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_strata_lpm_insert(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_strata_lpm_lookup(int unit, ip_addr_t iproute, 
			  _bcm_defip_cfg_t* lpm_cfg);
#endif	/* INCLUDE_L3 */

extern int bcm_strata_switch_control_port_set(int unit, bcm_port_t port,
                                       bcm_switch_control_t type, int arg);
extern int bcm_strata_switch_control_port_get(int unit, bcm_port_t port,
                                       bcm_switch_control_t type, int *arg);

#endif	/* !_BCM_INT_STRATA_H_ */
