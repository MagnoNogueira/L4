/*
 * $Id: * * Broadcom SDK $Copyright: Copyright 2008 Broadcom Corporation.
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
 */
 
#ifndef _BCM53115_SERVICE_H
#define _BCM53115_SERVICE_H

#include <shared/types.h>
#include <soc/error.h>
#include <soc/drv_if.h>

/* ------------------------------------ */
int drv_bcm5398_port_get(int unit, uint32 port, uint32 prop_type, 
                            uint32 *prop_val);
int drv_bcm5398_port_set(int unit, soc_pbmp_t bmp, uint32 prop_type, 
                            uint32 prop_val);
int drv_bcm5395_security_set(int unit, soc_pbmp_t bmp, 
                            uint32 state, uint32 mask);
int drv_bcm5395_security_get(int unit, uint32 port, 
                            uint32 *state, uint32 *mask);
int drv_bcm5395_arl_learn_enable_set(int unit, soc_pbmp_t pbmp, 
                            uint32 mode);
int drv_bcm5395_arl_learn_enable_get(int unit, soc_port_t port, 
                            uint32 * mode);
int drv_bcm53115_mem_length_get(int unit, uint32 mem, uint32 *data);
int drv_bcm53115_mem_width_get(int unit, uint32 mem, uint32 *data);
int drv_bcm53115_mem_read(int unit, uint32 mem, uint32 entry_id, 
                            uint32 count, uint32 *entry);
int drv_bcm53115_mem_write(int unit, uint32 mem, uint32 entry_id, 
                            uint32 count, uint32 *entry);
int drv_bcm53115_mem_field_get(int unit, uint32 mem, uint32 field_index, 
                            uint32 *entry, uint32 *fld_data);
int drv_bcm53115_mem_field_set(int unit, uint32 mem, uint32 field_index, 
                            uint32 *entry, uint32 *fld_data);
int drv_bcm53115_mem_clear(int unit, uint32 mem);
int drv_bcm53115_mem_search(int unit, uint32 mem, uint32 *key, 
                            uint32 *entry, uint32 *entry_1, uint32 flags);
int drv_bcm53115_mem_insert(int unit, uint32 mem, uint32 *entry, 
                            uint32 flags);
int drv_bcm53115_mem_delete(int unit, uint32 mem, uint32 *entry, 
                            uint32 flags);
int drv_bcm53115_mac_set(int unit, soc_pbmp_t pbmp, uint32 mac_type, 
                            uint8* mac, uint32 bpdu_idx);
int drv_bcm53115_mac_get(int unit, uint32 port, uint32 mac_type, 
                            soc_pbmp_t *bmp, uint8* mac);
int drv_bcm53115_trunk_set(int unit, int tid, soc_pbmp_t bmp, 
                            uint32 flag, uint32 hash_op);
int drv_bcm53115_trunk_get(int unit, int tid, soc_pbmp_t *bmp, 
                            uint32 flag, uint32 *hash_op);
int drv_bcm5395_mstp_port_set(int unit, uint32 mstp_gid, 
                                        uint32 port, uint32 port_state);
int drv_bcm5395_mstp_port_get(int unit, uint32 mstp_gid, 
                                        uint32 port, uint32 *port_state);
int drv_bcm53115_trap_set(int unit, uint32 trap_mask);
int drv_bcm53115_trap_get(int unit, uint32 *trap_mask);
int drv_bcm53115_snoop_set(int unit, uint32 snoop_mask);
int drv_bcm53115_snoop_get(int unit, uint32 *snoop_mask);
int  drv_bcm5395_mcast_bmp_get(int unit, uint32 *entry, 
                            soc_pbmp_t *bmp);
int  drv_bcm53115_mcast_bmp_set(int unit, uint32 *entry, soc_pbmp_t bmp, 
                            uint32 flag);
int drv_bcm5395_queue_port_prio_to_queue_set
    (int unit, uint8 port, uint8 prio, uint8 queue_n);
int drv_bcm5395_queue_port_prio_to_queue_get
    (int unit, uint8 port, uint8 prio, uint8 *queue_n);
int drv_bcm5395_queue_port_dfsv_set
    (int unit, uint8 port, uint8 dscp, uint8 prio, uint8 queue_n);
int drv_bcm5395_queue_port_dfsv_get
    (int unit, uint8 port, uint8 dscp, uint8 *prio, uint8 *queue_n);
int drv_bcm5395_queue_mode_set(int unit, uint32 bmp, uint32 mode);
int drv_bcm5395_queue_mode_get(int unit, uint32 port, uint32 *mode);
int drv_bcm5395_queue_count_set(int unit, uint32 port_type, uint8 count);
int drv_bcm5395_queue_count_get(int unit, uint32 port_type, uint8 *count);
int drv_bcm5395_queue_WRR_weight_set(int unit, 
    uint32 port_type, uint8 queue, uint32 weight);
int drv_bcm5395_queue_WRR_weight_get(int unit, uint32 port_type, 
    uint8 queue, uint32 *weight);
int drv_bcm5395_queue_prio_set(int unit, uint8 prio, uint8 queue_n);
int drv_bcm5395_queue_prio_get(int unit, uint8 prio, uint8 *queue_n);
int drv_bcm5395_queue_prio_remap_set(int unit, uint8 pre_prio, uint8 prio);
int drv_bcm5395_queue_prio_remap_get(int unit, uint8 pre_prio, uint8 *prio);
int drv_bcm5395_queue_dfsv_remap_set(int unit, uint8 dscp, uint8 prio);
int drv_bcm5395_queue_dfsv_remap_get(int unit, uint8 dscp, uint8 *prio);
int drv_bcm5395_queue_tos_set(int unit, uint8 precedence, uint8 queue_n);
int drv_bcm5395_queue_tos_get(int unit, uint8 precedence, uint8 *queue_n);
int drv_bcm5395_queue_dfsv_set(int unit, uint8 code_point, uint8 queue_n);
int drv_bcm5395_queue_dfsv_get(int unit, uint8 code_point, uint8 *queue_n);
int drv_bcm5395_queue_mapping_type_set(int unit, soc_pbmp_t bmp, 
    uint32 mapping_type, uint8 state);
int drv_bcm5395_queue_mapping_type_get(int unit, uint32 port, 
    uint32 mapping_type, uint8 *state);
int drv_bcm5395_queue_rx_reason_set
    (int unit, uint8 reason, uint32 queue);
int drv_bcm5395_queue_rx_reason_get
    (int unit, uint8 reason, uint32 *queue);

int drv_bcm53115_dev_prop_get(int unit, uint32 prop_type, uint32 *prop_val);
int drv_bcm53115_trunk_hash_field_remove(int unit, uint32 field_type);
int drv_bcm53115_trunk_hash_field_add(int unit, uint32 field_type);
int drv_bcm53115_rate_config_get(int unit, uint32 port,  
                            uint32 config_type, uint32 * value);
int drv_bcm53115_rate_config_set(int unit, soc_pbmp_t pbmp, 
                            uint32 config_type, uint32 value);
int drv_bcm53115_rate_get(int unit, uint32 port, int direction, 
                            uint32 * limit, uint32 * burst_size);
int drv_bcm53115_rate_set(int unit, soc_pbmp_t bmp, int direction, 
                            uint32 limit, uint32 burst_size);
int drv_bcm53115_storm_control_enable_set(int unit, uint32 port,  
                            uint8 enable);
int drv_bcm53115_storm_control_enable_get(int unit, uint32 port,  
                            uint8 *enable);
int drv_bcm53115_storm_control_set(int unit, soc_pbmp_t bmp,  
                            uint32 type, uint32 limit, uint32 burst_size);
int drv_bcm53115_storm_control_get(int unit, uint32 port,  
                            uint32 *type, uint32 *limit, uint32 *burst_size);
int drv53115_cfp_init(int unit);
int drv53115_cfp_action_get(int unit, uint32* action, drv_cfp_entry_t* entry, uint32* act_param);

int drv53115_cfp_action_set(int unit, uint32 action, drv_cfp_entry_t* entry, uint32 act_param1, uint32 act_param2);

int drv53115_cfp_control_get(int unit, uint32 control_type, uint32 param1, uint32 *param2);

int drv53115_cfp_control_set(int unit, uint32 control_type, uint32 param1, uint32 param2);

int drv53115_cfp_entry_read(int unit, uint32 index, uint32 ram_type, drv_cfp_entry_t *entry);

int drv53115_cfp_entry_search(int unit, uint32 flags, uint32 *index, drv_cfp_entry_t *entry);

int drv53115_cfp_entry_write(int unit, uint32 index, uint32 ram_type, drv_cfp_entry_t *entry);

int drv53115_cfp_field_get(int unit, uint32 mem_type, 
    uint32 field_type, drv_cfp_entry_t* entry, uint32* fld_val);

int drv53115_cfp_field_set(int unit, uint32 mem_type, 
    uint32 field_type, drv_cfp_entry_t* entry, uint32* fld_val);

int drv53115_cfp_meter_get(int unit, drv_cfp_entry_t* entry, 
    uint32 *kbits_sec, uint32 *kbits_burst) ;

int drv53115_cfp_meter_set(int unit, drv_cfp_entry_t* entry, 
    uint32 kbits_sec, uint32 kbits_burst);

int drv5395_cfp_qset_get(int unit, uint32 qual, drv_cfp_entry_t *entry, uint32 *val);

int drv5395_cfp_qset_set(int unit, uint32 qual, drv_cfp_entry_t *entry, uint32 val);

int drv53115_cfp_slice_id_select(int unit, drv_cfp_entry_t *entry, uint32 *slice_id, uint32 flags);

int drv53115_cfp_slice_to_qset(int unit, uint32 slice_id, drv_cfp_entry_t *entry);

int drv53115_cfp_stat_get(int unit, uint32 stat_type, uint32 index, uint32* counter);

int drv53115_cfp_stat_set(int unit, uint32 stat_type, uint32 index, uint32 counter);

int drv53115_cfp_udf_get(int unit, uint32 port, uint32 udf_index, 
    uint32 *offset, uint32 *base);

int drv53115_cfp_udf_set(int unit, uint32 port, uint32 udf_index, 
    uint32 offset, uint32 base);

int drv53115_cfp_ranger(int unit, uint32 flags, uint32 min, uint32 max);
int drv53115_cfp_range_set(int unit, uint32 type, uint32 id, uint32 param1, uint32 param2);
int drv53115_cfp_sub_qual_by_udf(int unit, int enable, int slice_id, uint32 sub_qual, 
    drv_cfp_qual_udf_info_t * qual_udf_info);                            

int drv_bcm5395_eav_control_set(int unit, uint32 type, uint32 param);
int drv_bcm5395_eav_control_get(int unit, uint32 type, uint32 *param);
int drv_bcm5395_eav_enable_set(int unit, uint32 port, uint32 enable);
int drv_bcm5395_eav_enable_get(int unit, uint32 port, uint32 *enable);
int drv_bcm5395_eav_link_status_set(int unit, uint32 port, uint32 link);
int drv_bcm5395_eav_link_status_get(int unit, uint32 port, uint32 *link);
int drv_bcm5395_eav_egress_timestamp_get(int unit, uint32 port,
    uint32 *timestamp);
int drv_bcm5395_eav_time_sync_set(int unit, uint32 type, uint32 p0, uint32 p1);
int drv_bcm5395_eav_time_sync_get(int unit, uint32 type, uint32 *p0, uint32 *p1);
int drv_bcm5395_eav_queue_control_set(int unit, uint32 port, uint32 type, uint32 param);
int drv_bcm5395_eav_queue_control_get(int unit, uint32 port, uint32 type, uint32 *param);
int drv_bcm5395_eav_time_sync_mac_set(int unit, uint8 * mac, uint16 ethertype);
int drv_bcm5395_eav_time_sync_mac_get(int unit, uint8 * mac, uint16 *ethertype);
int drv_bcm53115_dos_enable_set(int unit, uint32 type, uint32 param);
int drv_bcm53115_dos_enable_get(int unit, uint32 type, uint32 *param);
int drv_bcm53115_vlan_prop_get(int unit, uint32 prop_type, uint32 * prop_val);
int drv_bcm53115_vlan_prop_set(int unit, uint32 prop_type, uint32 prop_val);
int drv_bcm53115_vlan_prop_port_enable_get(int unit, uint32 prop_type, 
                            uint32 port_n, uint32 * val);
int drv_bcm53115_vlan_prop_port_enable_set(int unit, uint32 prop_type, 
                            soc_pbmp_t bmp, uint32 val);
int drv_bcm53115_vlan_vt_set(int unit, uint32 prop_type, uint32 vid, 
                            uint32 port, uint32 prop_val);
int drv_bcm53115_vlan_vt_get(int unit, uint32 prop_type, uint32 vid, 
                            uint32 port, uint32 *prop_val);
int drv_bcm53115_vlan_vt_add(int unit, uint32 vt_type, uint32 port, 
                            uint32 cvid, uint32 sp_vid, uint32 pri, 
                            uint32 mode);
int drv_bcm53115_vlan_vt_delete(int unit, uint32 vt_type, uint32 port, 
                            uint32 vid);
int drv_bcm53115_vlan_vt_delete_all(int unit, uint32 vt_type);

/* ------------------------------------ */

drv_if_t drv_bcm53115_services = {
/* reg_read                 */  drv_reg_read,
/* reg_write                */  drv_reg_write,
/* reg_length_get           */  drv_reg_length_get,
/* reg_addr                 */  drv_reg_addr,
/* reg_field_set            */  drv_reg_field_set,
/* reg_field_get            */  drv_reg_field_get,
/* miim_read                */  drv_miim_read,
/* miim_write               */  drv_miim_write,
/* phy_addr_set             */  NULL,
/* mac_driver_set           */  NULL,
/* mem_length_get           */  drv_bcm53115_mem_length_get,
/* mem_width_get            */  drv_bcm53115_mem_width_get,
/* mem_read                 */  drv_bcm53115_mem_read,
/* mem_write                */  drv_bcm53115_mem_write,
/* mem_field_get            */  drv_bcm53115_mem_field_get,
/* mem_field_set            */  drv_bcm53115_mem_field_set,
/* mem_clear                */  drv_bcm53115_mem_clear,
/* mem_search               */  drv_bcm53115_mem_search,
/* mem_insert               */  drv_bcm53115_mem_insert,
/* mem_delete               */  drv_bcm53115_mem_delete,
/* mem_cache_get            */  drv_mem_cache_get,
/* mem_cache_set            */  drv_mem_cache_set,
/* counter_thread_set       */  drv_counter_thread_set,
/* counter_set              */  drv_counter_set,
/* counter_get              */  drv_counter_get,
/* counter_reset            */  drv_counter_reset,
/* vlan_mode_set            */  drv_vlan_mode_set,
/* vlan_mode_get            */  drv_vlan_mode_get,
/* port_vlan_pvid_set       */  drv_port_vlan_pvid_set,
/* port_vlan_pvid_get       */  drv_port_vlan_pvid_get,
/* port_vlan_set            */  drv_port_vlan_set,
/* port_vlan_get            */  drv_port_vlan_get,
/* vlan_property_set        */  drv_bcm53115_vlan_prop_set,
/* vlan_property_get        */  drv_bcm53115_vlan_prop_get,
/* vlan_prop_port_enable_set */ drv_bcm53115_vlan_prop_port_enable_set,
/* vlan_prop_port_enable_get */ drv_bcm53115_vlan_prop_port_enable_get,
/* vlan_vt_set              */  drv_bcm53115_vlan_vt_set,
/* vlan_vt_get              */  drv_bcm53115_vlan_vt_get,
/* vlan_vt_add              */  drv_bcm53115_vlan_vt_add,
/* vlan_vt_delete           */  drv_bcm53115_vlan_vt_delete,
/* vlan_vt_delete_all       */  drv_bcm53115_vlan_vt_delete_all,
/* trunk_set                */  drv_bcm53115_trunk_set,
/* trunk_get                */  drv_bcm53115_trunk_get,
/* trunk_hash_field_add     */  drv_bcm53115_trunk_hash_field_add,
/* trunk_hash_field_remove  */  drv_bcm53115_trunk_hash_field_remove,
/* queue_mode_set           */  drv_bcm5395_queue_mode_set,
/* queue_mode_get           */  drv_bcm5395_queue_mode_get,
/* queue_count_set          */  drv_bcm5395_queue_count_set,
/* queue_count_get          */  drv_bcm5395_queue_count_get,
/* queue_WRR_weight_set     */  drv_bcm5395_queue_WRR_weight_set,
/* queue_WRR_weight_get     */  drv_bcm5395_queue_WRR_weight_get,
/* queue_prio_set           */  drv_bcm5395_queue_prio_set,
/* queue_prio_get           */  drv_bcm5395_queue_prio_get,
/* queue_tos_set            */  drv_bcm5395_queue_tos_set,
/* queue_tos_get            */  drv_bcm5395_queue_tos_get,
/* queue_dfsv_set           */  drv_bcm5395_queue_dfsv_set,
/* queue_dfsv_get           */  drv_bcm5395_queue_dfsv_get,
/* queue_mapping_type_set   */  drv_bcm5395_queue_mapping_type_set,
/* queue_mapping_type_get   */  drv_bcm5395_queue_mapping_type_get,
/* age_timer_set            */  drv_age_timer_set,
/* age_timer_get            */  drv_age_timer_get,
/* mac_set                  */  drv_bcm53115_mac_set,
/* mac_get                  */  drv_bcm53115_mac_get,
/* mirror_set               */  drv_mirror_set,
/* mirror_get               */  drv_mirror_get,
/* port_oper_mode_set       */  drv_port_oper_mode_set,
/* port_oper_mode_get       */  drv_port_oper_mode_get,
/* port_set                 */  drv_bcm5398_port_set, 
/* port_get                 */  drv_bcm5398_port_get, 
/* port_advertise_set       */  drv_port_advertise_set,
/* port_advertise_get       */  drv_port_advertise_get,
/* port_status_get          */  drv_port_status_get,
/* port_sw_mac_update       */  drv_port_sw_mac_update,
/* port_bitmap_get          */  drv_port_bitmap_get,
/* security_set             */  drv_bcm5395_security_set,
/* security_get             */  drv_bcm5395_security_get,
/* security_egress_set      */  drv_security_egress_set,
/* security_egress_get      */  drv_security_egress_get,
/* mstp_config_set          */  drv_mstp_config_set,
/* mstp_config_get          */  drv_mstp_config_get,
/* mstp_port_set            */  drv_bcm5395_mstp_port_set,
/* mstp_port_get            */  drv_bcm5395_mstp_port_get,
/* trap_set                 */  drv_bcm53115_trap_set,
/* trap_get                 */  drv_bcm53115_trap_get,
/* snoop_set                */  drv_bcm53115_snoop_set,
/* snoop_get                */  drv_bcm53115_snoop_get,
/* rate_config_set          */  drv_bcm53115_rate_config_set,
/* rate_config_get          */  drv_bcm53115_rate_config_get,
/* rate_set                 */  drv_bcm53115_rate_set,
/* rate_get                 */  drv_bcm53115_rate_get,
/* storm_control_enable_set */  drv_bcm53115_storm_control_enable_set,
/* storm_control_enable_get */  drv_bcm53115_storm_control_enable_get,
/* storm_control_set        */  drv_bcm53115_storm_control_set,
/* storm_control_get        */  drv_bcm53115_storm_control_get,
/* dev_prop_get             */  drv_bcm53115_dev_prop_get,
/* dev_prop_set             */  drv_dev_prop_set,
/* mcast_init               */  drv_mcast_init,
/* mcast_to_marl            */  drv_mcast_to_marl,
/* mcast_from_marl          */  drv_mcast_from_marl,
/* mcast_bmp_get            */  drv_bcm5395_mcast_bmp_get,
/* mcast_bmp_set            */  drv_bcm53115_mcast_bmp_set,
/* arl_table_process        */  drv_arl_table_process,
/* arl_sync                 */  drv_arl_sync,
/* arl_learn_enable_set     */  drv_bcm5395_arl_learn_enable_set,
/* arl_learn_enable_set     */  drv_bcm5395_arl_learn_enable_get,
/* queue_port_prio_to_queue_set */ drv_bcm5395_queue_port_prio_to_queue_set,
/* queue_port_prio_to_queue_get */ drv_bcm5395_queue_port_prio_to_queue_get,
/* queue_port_dfsv_set      */ drv_bcm5395_queue_port_dfsv_set,
/* queue_port_dfsv_get      */ drv_bcm5395_queue_port_dfsv_get,
/* queue_prio_remap_set     */ drv_bcm5395_queue_prio_remap_set,
/* queue_prio_remap_get     */ drv_bcm5395_queue_prio_remap_get,
/* queue_dfsv_remap_set     */ drv_bcm5395_queue_dfsv_remap_set,
/* queue_dfsv_remap_get     */ drv_bcm5395_queue_dfsv_remap_get,
/* queue_rx_reason_set */ drv_bcm5395_queue_rx_reason_set,
/* queue_rx_reason_get */ drv_bcm5395_queue_rx_reason_get,
/* drv_cfp_init             */ drv53115_cfp_init,
/* drv_cfp_action_get       */ drv53115_cfp_action_get, 
/* drv_cfp_action_set       */ drv53115_cfp_action_set,
/* drv_cfp_control_get      */ drv53115_cfp_control_get,
/* drv_cfp_control_set      */ drv53115_cfp_control_set,
/* drv_cfp_entry_read       */ drv53115_cfp_entry_read,
/* drv_cfp_entry_search     */ drv53115_cfp_entry_search,
/* drv_cfp_entry_write      */ drv53115_cfp_entry_write,
/* drv_cfp_field_get        */ drv53115_cfp_field_get,
/* drv_cfp_field_set        */ drv53115_cfp_field_set,
/* drv_cfp_meter_get        */ drv53115_cfp_meter_get,
/* drv_cfp_meter_set        */ drv53115_cfp_meter_set,
/* drv_cfp_qset_get         */ drv5395_cfp_qset_get,
/* drv_cfp_qset_set         */ drv5395_cfp_qset_set,
/* drv_cfp_slice_id_select  */ drv53115_cfp_slice_id_select,
/* drv_cfp_slice_to_qset    */ drv53115_cfp_slice_to_qset,
/* drv_cfp_stat_get         */ drv53115_cfp_stat_get,
/* drv_cfp_stat_set         */ drv53115_cfp_stat_set,
/* drv_cfp_udf_get          */ drv53115_cfp_udf_get,
/* drv_cfp_udf_set          */ drv53115_cfp_udf_set,
/* drv_cfp_ranger           */ drv53115_cfp_ranger,
/* drv_cfp_range_set        */ drv53115_cfp_range_set,
/* drv_cfp_sub_qual_by_udf */ drv53115_cfp_sub_qual_by_udf,
/* drv_eav_control_set */ drv_bcm5395_eav_control_set,
/* drv_eav_control_get */ drv_bcm5395_eav_control_get,
/* drv_eav_enable_set */ drv_bcm5395_eav_enable_set,
/* drv_eav_enable_get */ drv_bcm5395_eav_enable_get,
/* drv_eav_link_status_set */ drv_bcm5395_eav_link_status_set,
/* drv_eav_link_status_get */ drv_bcm5395_eav_link_status_get,
/* drv_eav_egress_timestamp_get */ drv_bcm5395_eav_egress_timestamp_get,
/* drv_eav_time_sync_set */ drv_bcm5395_eav_time_sync_set,
/* drv_eav_time_sync_get */ drv_bcm5395_eav_time_sync_get,
/* drv_eav_queue_control_set */ drv_bcm5395_eav_queue_control_set,
/* drv_eav_queue_control_get */ drv_bcm5395_eav_queue_control_get,
/* drv_eav_time_sync_mac_set */ drv_bcm5395_eav_time_sync_mac_set,
/* drv_eav_time_sync_mac_get */ drv_bcm5395_eav_time_sync_mac_get,
/* drv_dos_enable_set */ drv_bcm53115_dos_enable_set,
/* drv_dos_enable_get */ drv_bcm53115_dos_enable_get,
};


#endif
