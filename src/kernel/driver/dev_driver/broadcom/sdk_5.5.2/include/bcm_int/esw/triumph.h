/*
 * $Id: triumph.h,v 1.54.4.14 Broadcom SDK $
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
 * File:        triumph.h
 * Purpose:     Function declarations for Triumph bcm functions
 */

#ifndef _BCM_INT_TRIUMPH_H_
#define _BCM_INT_TRIUMPH_H_
#include <bcm_int/esw/l2.h>
#ifdef BCM_FIELD_SUPPORT 
#include <bcm_int/esw/field.h>
#endif /* BCM_FIELD_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
/*
 * Routines for internal use
 */
extern int _bcm_tr_l2_traverse_mem(int unit, soc_mem_t mem, 
                                   _bcm_l2_traverse_t *trav_st);
extern int _bcm_tr_l2_from_l2x(int unit,
                     bcm_l2_addr_t *l2addr, l2x_entry_t *l2x_entry);
extern int _bcm_tr_l2_to_l2x(int unit,
                   l2x_entry_t *l2x_entry, bcm_l2_addr_t *l2addr);

/****************************************************************
 *
 * Triumph functions
 *
 ****************************************************************/
extern int bcm_tr_l2_init(int unit);
extern int bcm_tr_l2_term(int unit);
extern int bcm_tr_l2_addr_get(int unit, bcm_mac_t mac_addr, bcm_vlan_t vid,
                              bcm_l2_addr_t *l2addr);
extern int bcm_tr_l2_addr_add(int unit, bcm_l2_addr_t *l2addr);
extern int bcm_tr_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid);
extern int bcm_tr_l2_addr_register(int unit, bcm_l2_addr_callback_t callback, 
                                   void *userdata);
extern int bcm_tr_l2_addr_unregister(int unit, bcm_l2_addr_callback_t callback, 
                                     void *userdata);
extern int bcm_tr_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
                                  bcm_l2_addr_t *cf_array, int cf_max,
                                  int *cf_count);
extern int bcm_tr_l2_cross_connect_add(int unit, bcm_vlan_t outer_vlan, 
                                       bcm_vlan_t inner_vlan,
                                       bcm_gport_t port_1, bcm_gport_t port_2);
extern int bcm_tr_l2_cross_connect_delete(int unit, bcm_vlan_t outer_vlan, 
                                          bcm_vlan_t inner_vlan);
extern int bcm_tr_l2_cross_connect_delete_all(int unit);
extern int bcm_tr_l2_cross_connect_traverse(int unit,
                                            bcm_vlan_cross_connect_traverse_cb cb,
                                            void *user_data);
extern int _bcm_tr_l2_addr_replace_by_mac(int unit, uint32 flags, _bcm_l2_replace_t *rep_st);
extern int _bcm_tr_l2_addr_replace_by_vlan_dest(int unit, uint32 flags, _bcm_l2_replace_t *rep_st);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_tr_l2_reload_mbi(int unit);
#else
#define _bcm_tr_l2_reload_mbi(_u)    (BCM_E_UNAVAIL)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_tr_l2_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

extern int _bcm_tr_vlan_action_profile_init(int unit);
extern int _bcm_tr_vlan_action_profile_entry_add(int unit,
                                      bcm_vlan_action_set_t *action,
                                      uint32 *index);
extern void _bcm_tr_vlan_action_profile_entry_increment(int unit, uint32 index);
extern void _bcm_tr_vlan_action_profile_entry_get(int unit,
                                      bcm_vlan_action_set_t *action,
                                      uint32 index);
extern int _bcm_tr_vlan_action_profile_entry_delete(int unit, uint32 index);
extern int _bcm_tr_vlan_range_profile_init(int unit);

extern int _bcm_tr_egr_vlan_action_profile_entry_add(int unit,
                                      bcm_vlan_action_set_t *action,
                                      uint32 *index);
extern void _bcm_tr_egr_vlan_action_profile_entry_increment(int unit, uint32 index);
extern void _bcm_tr_egr_vlan_action_profile_entry_get(int unit,
                                          bcm_vlan_action_set_t *action,
                                          uint32 index);
extern int _bcm_tr_egr_vlan_action_profile_entry_delete(int unit, uint32 index);

extern int _bcm_tr_vlan_ip_action_add(int unit, bcm_vlan_ip_t *vlan_ip,
                                      bcm_vlan_action_set_t *action);
extern int _bcm_tr_vlan_ip_delete(int unit, bcm_vlan_ip_t *vlan_ip);
extern int _bcm_tr_vlan_ip_delete_all(int unit);

extern int _bcm_tr_vlan_port_default_action_set(int unit, bcm_port_t port,
                                                bcm_vlan_action_set_t *action);
extern int _bcm_tr_vlan_port_default_action_get(int unit, bcm_port_t port,
                                                bcm_vlan_action_set_t *action);

extern int _bcm_tr_vlan_port_egress_default_action_set(int unit, bcm_port_t port,
                                                       bcm_vlan_action_set_t *action);
extern int _bcm_tr_vlan_port_egress_default_action_get(int unit, bcm_port_t port,
                                                       bcm_vlan_action_set_t *action);

extern int _bcm_tr_vlan_port_protocol_action_add(int unit, bcm_port_t port,
                                                 bcm_port_frametype_t frame,
                                                 bcm_port_ethertype_t ether,
                                                 bcm_vlan_action_set_t *action);
extern int _bcm_tr_vlan_port_protocol_delete(int unit, bcm_port_t port,
                                             bcm_port_frametype_t frame,
                                             bcm_port_ethertype_t ether);
extern int _bcm_tr_vlan_port_protocol_delete_all(int unit, bcm_port_t port);

extern int _bcm_tr_vlan_mac_action_add(int unit, bcm_mac_t mac,
                                       bcm_vlan_action_set_t *action);
extern int _bcm_tr_vlan_mac_delete(int unit, bcm_mac_t mac);
extern int _bcm_tr_vlan_mac_delete_all(int unit);

extern int _bcm_tr_vlan_translate_action_add(int unit,
                                             bcm_gport_t port,
                                             bcm_vlan_translate_key_t key_type,
                                             bcm_vlan_t outer_vlan,
                                             bcm_vlan_t inner_vlan,
                                             bcm_vlan_action_set_t *action);
extern int _bcm_tr_vlan_translate_action_delete(int unit,
                                                bcm_gport_t port,
                                                bcm_vlan_translate_key_t key_type,
                                                bcm_vlan_t outer_vlan,
                                                bcm_vlan_t inner_vlan);
extern int _bcm_tr_vlan_translate_action_delete_all(int unit);
extern int _bcm_tr_vlan_translate_action_get (int unit, 
                                              bcm_gport_t port, 
                                              bcm_vlan_translate_key_t key_type, 
                                              bcm_vlan_t outer_vid,
                                              bcm_vlan_t inner_vid,
                                              bcm_vlan_action_set_t *action);
extern int _bcm_tr_vlan_translate_action_range_add(int unit,
                                                   bcm_gport_t port,
                                                   bcm_vlan_t outer_vlan_low,
                                                   bcm_vlan_t outer_vlan_high,
                                                   bcm_vlan_t inner_vlan_low,
                                                   bcm_vlan_t inner_vlan_high,
                                                   bcm_vlan_action_set_t *action);
extern int _bcm_tr_vlan_translate_action_range_get(int unit,
                                                   bcm_gport_t port,
                                                   bcm_vlan_t outer_vlan_low,
                                                   bcm_vlan_t outer_vlan_high,
                                                   bcm_vlan_t inner_vlan_low,
                                                   bcm_vlan_t inner_vlan_high,
                                                   bcm_vlan_action_set_t *action);
extern int _bcm_tr_vlan_translate_action_range_delete(int unit,
                                                      bcm_gport_t port,
                                                      bcm_vlan_t outer_vlan_low,
                                                      bcm_vlan_t outer_vlan_high,
                                                      bcm_vlan_t inner_vlan_low,
                                                      bcm_vlan_t inner_vlan_high);
extern int _bcm_tr_vlan_translate_action_range_delete_all(int unit);
extern int _bcm_tr_vlan_translate_action_range_traverse(int unit, 
                                  _bcm_vlan_translate_traverse_t *trvs_info);

extern int _bcm_tr_vlan_translate_parse(int unit, soc_mem_t mem, uint32* vent, 
                                  _bcm_vlan_translate_traverse_t *trvs_info);
extern int 
_bcm_tr_vlan_translate_egress_action_add(int unit, int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_action_set_t *action);
extern int 
_bcm_tr_vlan_translate_egress_action_get(int unit, int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_action_set_t *action);
extern int 
_bcm_tr_vlan_translate_egress_action_delete(int unit, int port_class,
                                            bcm_vlan_t outer_vlan,
                                            bcm_vlan_t inner_vlan);
extern int _bcm_tr_vlan_translate_egress_action_delete_all(int unit);

extern int bcm_tr_port_rate_pause_get(int unit, bcm_port_t port,
                                      uint32 *kbits_pause,
                                      uint32 *kbits_resume);
extern int _bcm_tr_port_control_bridge_set(int unit, bcm_port_t port, 
                                           int value);

extern int bcm_tr_port_rate_egress_set(int unit, bcm_port_t port,
                                       uint32 kbits_sec,
                                       uint32 kbits_burst);
extern int bcm_tr_port_rate_egress_get(int unit, bcm_port_t port,
                                       uint32 *kbits_sec,
                                       uint32 *kbits_burst);

#ifdef BCM_FIELD_SUPPORT 
extern int _bcm_tr_field_init(int unit, _field_control_t *fc);
extern int _bcm_tr_field_external_tcam_key_mask_get(int unit, 
                                                    _field_entry_t *f_ent);
extern int _bcm_field_triumph_external_entry_move(int unit, 
                                                  _field_entry_t *f_ent,
                                                  int index1, int index2);
extern int _bcm_field_triumph_external_qualify_split_quals(int unit, 
                            _qual_info_t *qi, _field_entry_t *f_ent, 
                                      uint32 data[], uint32 mask[]);
extern int _bcm_field_triumph_external_counters_collect(int unit, 
                                       _field_stage_t *stage_fc);
extern int _bcm_trx_field_control_arp_set(int unit,  
                                          bcm_field_control_t control,
                                          uint32 state);
#endif /* BCM_FIELD_SUPPORT */

#ifdef INCLUDE_L3

struct _bcm_l3_trvrs_data_s;
extern int _bcm_tr_l3_ingress_interface_clr(int unit, int intf_id);
extern int _bcm_tr_l3_intf_class_set(int unit, bcm_vlan_t vid, uint32 intf_class);
extern int _bcm_tr_l3_intf_vrf_get(int unit, bcm_vlan_t vid, bcm_vrf_t *vrf);
extern int _bcm_tr_l3_intf_vrf_bind(int unit, bcm_vlan_t vid, bcm_vrf_t vrf);
extern int _bcm_tr_l3_ipmc_get(int unit, _bcm_l3_cfg_t *l3cfg);
extern int _bcm_tr_l3_ipmc_add(int unit, _bcm_l3_cfg_t *l3cfg);
extern int _bcm_tr_l3_ipmc_del(int unit, _bcm_l3_cfg_t *l3cfg);
extern int _bcm_tr_l3_ipmc_get_by_idx(int unit, void *dma_ptr, int idx,
                                      _bcm_l3_cfg_t *l3cfg);
extern int _bcm_tr_l3_defip_mem_get(int unit, uint32 flags, 
                                    int plen, soc_mem_t *mem);
extern int _bcm_tr_defip_init(int unit);
extern int _bcm_tr_defip_deinit(int unit);
extern int _bcm_tr_l3_intf_mtu_get(int unit, _bcm_l3_intf_cfg_t *intf_info);
extern int _bcm_tr_l3_intf_mtu_set(int unit, _bcm_l3_intf_cfg_t *intf_info);
extern int _bcm_tr_ext_lpm_init(int unit, soc_mem_t mem);
extern int _bcm_tr_ext_lpm_deinit(int unit, soc_mem_t mem);
extern int _bcm_tr_ext_lpm_reinit_done(int unit, soc_mem_t mem);
extern int _bcm_tr_ext_lpm_reinit(int unit, soc_mem_t mem, 
                                  int idx, _bcm_defip_cfg_t *lpm_cfg);
extern int _bcm_tr_ext_lpm_add(int unit, _bcm_defip_cfg_t *data, 
                               int nh_ecmp_idx);
extern int _bcm_tr_ext_lpm_delete(int unit, _bcm_defip_cfg_t *key);
extern int _bcm_tr_ext_lpm_match(int unit, _bcm_defip_cfg_t *key, 
                                 int *next_hop_index);

extern int _bcm_tr_defip_traverse(int unit, struct _bcm_l3_trvrs_data_s *trv_data);

extern int _bcm_tr_ipmc_create(int unit, int *ipmc_id);
extern int _bcm_tr_ipmc_id_alloc(int unit, int ipmc_id);
extern int _bcm_tr_ipmc_id_free(int unit, int ipmc_id);
extern int bcm_tr_ipmc_init(int unit);
extern int bcm_tr_ipmc_detach(int unit);
extern int bcm_tr_ipmc_get(int unit, int index, bcm_ipmc_addr_t *ipmc);
extern int bcm_tr_ipmc_lookup(int unit, int *index, bcm_ipmc_addr_t *ipmc);
extern int bcm_tr_ipmc_add(int unit, bcm_ipmc_addr_t *ipmc);
extern int bcm_tr_ipmc_put(int unit, int index, bcm_ipmc_addr_t *ipmc);
extern int bcm_tr_ipmc_delete(int unit, bcm_ipmc_addr_t *ipmc);
extern int bcm_tr_ipmc_delete_all(int unit);
extern int bcm_tr_ipmc_enable(int unit, int enable);
extern int bcm_tr_ipmc_src_port_check(int unit, int enable);
extern int bcm_tr_ipmc_src_ip_search(int unit, int enable);
extern int bcm_tr_ipmc_egress_port_set(int unit, bcm_port_t port,
                                       const bcm_mac_t mac, int untag,
                                       bcm_vlan_t vid, int ttl_threshold);
extern int bcm_tr_ipmc_egress_port_get(int unit, bcm_port_t port,
                                       mac_addr_t mac, int *untag,
                                       bcm_vlan_t *vid, int *ttl_threshold);


extern int _bcm_tr_mpls_vp_alloc(int unit, int start, int end, int count, int *base_vp);
extern int _bcm_tr_mpls_vp_free(int unit, int count, int base_vp);

extern int bcm_tr_mpls_lock (int unit);
extern void bcm_tr_mpls_unlock(int unit);

extern int bcm_tr_mpls_init(int unit);
extern int bcm_tr_mpls_cleanup(int unit);
extern int bcm_tr_mpls_l2_tunnel_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan);
extern int bcm_tr_mpls_l2_tunnel_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan);
extern int bcm_tr_mpls_l2_tunnel_delete_all(int unit);
extern int bcm_tr_mpls_vpn_id_create(int unit, bcm_mpls_vpn_config_t *info);
extern int bcm_tr_mpls_vpn_id_destroy(int unit, bcm_vpn_t vpn);
extern int bcm_tr_mpls_vpn_id_destroy_all(int unit);
extern int bcm_tr_mpls_port_add(int unit, bcm_vpn_t vpn, 
                                bcm_mpls_port_t *mpls_port);
extern int bcm_tr_mpls_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t mpls_port_id);
extern int bcm_tr_mpls_port_delete_all(int unit, bcm_vpn_t vpn);
extern int bcm_tr_mpls_port_get(int unit, bcm_vpn_t vpn, 
                                bcm_mpls_port_t *mpls_port);
extern int bcm_tr_mpls_port_get_all(int unit, bcm_vpn_t vpn, int port_max,
                                    bcm_mpls_port_t *port_array, int *port_count);
extern int bcm_tr_mpls_tunnel_initiator_set(int unit, bcm_if_t intf, int num_labels,
                                            bcm_mpls_egress_label_t *label_array);
extern int bcm_tr_mpls_tunnel_initiator_clear(int unit, bcm_if_t intf);
extern int bcm_tr_mpls_tunnel_initiator_get(int unit, bcm_if_t intf, int label_max,
                                            bcm_mpls_egress_label_t *label_array,
                                            int *label_count);
extern int bcm_tr_mpls_tunnel_initiator_clear_all(int unit);
extern int bcm_tr_mpls_tunnel_switch_add(int unit, bcm_mpls_tunnel_switch_t *info);
extern int bcm_tr_mpls_tunnel_switch_delete(int unit, bcm_mpls_tunnel_switch_t *info);
extern int bcm_tr_mpls_tunnel_switch_delete_all(int unit);
extern int bcm_tr_mpls_tunnel_switch_get(int unit, bcm_mpls_tunnel_switch_t *info);
extern int bcm_tr_mpls_tunnel_switch_traverse(int unit,
                                              bcm_mpls_tunnel_switch_traverse_cb cb,
                                              void *user_data);
extern int bcm_tr_mpls_exp_map_create(int unit, uint32 flags, int *exp_map_id);
extern int bcm_tr_mpls_exp_map_destroy(int unit, int exp_map_id);
extern int bcm_tr_mpls_exp_map_destroy_all(int unit);
extern int bcm_tr_mpls_exp_map_set(int unit, int exp_map_id,
                                   bcm_mpls_exp_map_t *exp_map);
extern int bcm_tr_mpls_exp_map_get(int unit, int exp_map_id,
                                   bcm_mpls_exp_map_t *exp_map);
extern int bcm_tr_mpls_label_stat_get(int unit, bcm_mpls_label_t label, bcm_gport_t port,
                                      bcm_mpls_stat_t stat, uint64 *val);
extern int bcm_tr_mpls_label_stat_get32(int unit, bcm_mpls_label_t label, bcm_gport_t port,
                                        bcm_mpls_stat_t stat, uint32 *val);
extern int _bcm_tr_mpls_port_resolve(int unit, bcm_gport_t mpls_port,
                                     bcm_module_t *modid, bcm_port_t *port,
                                     bcm_trunk_t *trunk_id, int *id);
extern int bcm_tr_mpls_port_learn_set(int unit, bcm_gport_t port, uint32 flags);
extern int bcm_tr_mpls_port_learn_get(int unit, bcm_gport_t port, uint32 *flags);
extern int bcm_tr_mpls_mcast_flood_set(int unit, bcm_vlan_t vlan,
                                       bcm_vlan_mcast_flood_t mode);
extern int bcm_tr_mpls_mcast_flood_get(int unit, bcm_vlan_t vlan,
                                       bcm_vlan_mcast_flood_t *mode);

extern int bcm_tr_subport_init(int unit);
extern int bcm_tr_subport_cleanup(int unit);
extern int bcm_tr_subport_group_create(int unit, bcm_subport_group_config_t *config,
                                       bcm_gport_t *group);
extern int bcm_tr_subport_group_destroy(int unit, bcm_gport_t group);
extern int bcm_tr_subport_group_get(int unit, bcm_gport_t group,
                                    bcm_subport_group_config_t *config);
extern int bcm_tr_subport_port_add(int unit, bcm_subport_config_t *config,
                                   bcm_gport_t *port);
extern int bcm_tr_subport_port_delete(int unit, bcm_gport_t port);
extern int bcm_tr_subport_port_get(int unit, bcm_gport_t port,
                                   bcm_subport_config_t *config);
extern int bcm_tr_subport_port_traverse(int unit, bcm_subport_port_traverse_cb cb,
                                        void *user_data);
extern int _bcm_tr_subport_group_resolve(int unit, bcm_gport_t gport,
                                         bcm_module_t *modid, bcm_port_t *port,
                                         bcm_trunk_t *trunk_id, int *id);
extern int _bcm_tr_subport_port_resolve(int unit, bcm_gport_t gport,
                                        bcm_module_t *modid, bcm_port_t *port,
                                        bcm_trunk_t *trunk_id, int *id);
extern int bcm_tr_subport_learn_set(int unit, bcm_gport_t port, uint32 flags);
extern int bcm_tr_subport_learn_get(int unit, bcm_gport_t port, uint32 *flags);

extern int bcm_tr_multicast_create(int unit, uint32 flags,
                                   bcm_multicast_t *group);
extern int bcm_tr_multicast_destroy(int unit, bcm_multicast_t group);
extern int bcm_tr_multicast_vpls_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                           bcm_gport_t mpls_port_id, bcm_if_t *encap_id);
extern int bcm_tr_multicast_subport_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                              bcm_gport_t subport, bcm_if_t *encap_id);
extern int bcm_tr_multicast_egress_add(int unit, bcm_multicast_t group,
                                       bcm_gport_t port, bcm_if_t encap_id);
extern int bcm_tr_multicast_egress_delete(int unit, bcm_multicast_t group,
                                          bcm_gport_t port, bcm_if_t encap_id);
extern int bcm_tr_multicast_egress_delete_all(int unit, bcm_multicast_t group);
extern int bcm_tr_multicast_egress_set(int unit, bcm_multicast_t group,
                                       int port_count, bcm_gport_t *port_array,
                                       bcm_if_t *encap_array);
extern int bcm_tr_multicast_egress_get(int unit, bcm_multicast_t group,
                                       int port_max, bcm_gport_t *port_array,
                                       bcm_if_t *encap_array, int *port_count);
#endif /* INCLUDE_L3 */

extern int bcm_tr_cosq_init(int unit);
extern int bcm_tr_cosq_detach(int unit);
extern int bcm_tr_cosq_config_set(int unit, int numq);
extern int bcm_tr_cosq_config_get(int unit, int *numq);
extern int bcm_tr_cosq_mapping_set(int unit, bcm_port_t port,
                                   bcm_cos_t priority, bcm_cos_queue_t cosq);
extern int bcm_tr_cosq_mapping_get(int unit, bcm_port_t port,
                                   bcm_cos_t priority, bcm_cos_queue_t *cosq);
extern int bcm_tr_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                          uint32 kbits_sec_max, uint32 flags);
extern int bcm_tr_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                          uint32 *kbits_sec_max, uint32 *flags);
extern int bcm_tr_cosq_port_pps_set(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int pps);
extern int bcm_tr_cosq_port_pps_get(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int *pps);
extern int bcm_tr_cosq_port_burst_set(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int burst);
extern int bcm_tr_cosq_port_burst_get(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int *burst);
extern int bcm_tr_cosq_discard_set(int unit, uint32 flags);
extern int bcm_tr_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_tr_cosq_discard_port_set(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int drop_start, int drop_slope,
                                        int average_time);
extern int bcm_tr_cosq_discard_port_get(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int *drop_start, int *drop_slope,
                                        int *average_time);
extern int bcm_tr_cosq_gport_discard_set(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_tr_cosq_gport_discard_get(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_tr_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                                      int mode, const int weights[], int delay);
extern int bcm_tr_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                                      int *mode, int weights[], int *delay);
extern int bcm_tr_cosq_gport_add(int unit, bcm_gport_t port, int numq,
                                 uint32 flags, bcm_gport_t *gport);
extern int bcm_tr_cosq_gport_delete(int unit, bcm_gport_t gport);
extern int bcm_tr_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                                      void *user_data);

extern int bcm_tr_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq, 
                                           uint32 kbits_sec_min,
                                           uint32 kbits_sec_max, uint32 flags);
extern int bcm_tr_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq, 
                                           uint32 *kbits_sec_min,
                                           uint32 *kbits_sec_max, uint32 *flags);
extern int bcm_tr_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int mode,
                                       int weight);
extern int bcm_tr_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                                       bcm_cos_queue_t cosq, int *mode,
                                       int *weight);
extern int bcm_tr_cosq_gport_attach(int unit, bcm_gport_t sched_gport, 
                                    bcm_cos_queue_t cosq, bcm_gport_t input_gport);
extern int bcm_tr_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                                    bcm_cos_queue_t cosq, bcm_gport_t input_gport);
extern int bcm_tr_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                                        bcm_cos_queue_t *cosq, bcm_gport_t *input_gport);
extern int _bcm_tr_cosq_port_resolve(int unit, bcm_gport_t gport,
                                     bcm_module_t *modid, bcm_port_t *port,
                                     bcm_trunk_t *trunk_id, int *id);

#endif  /* BCM_TRX_SUPPORT */
#endif  /* !_BCM_INT_TRIUMPH_H_ */
