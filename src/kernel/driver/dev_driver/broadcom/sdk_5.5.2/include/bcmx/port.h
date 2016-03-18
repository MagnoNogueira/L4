/*
 * $Id: port.h,v 1.29 Broadcom SDK $
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
 * File:        include/bcmx/port.h
 * Purpose:     BCMX port APIs
 */

#ifndef _BCMX_PORT_H
#define _BCMX_PORT_H

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include <bcm/port.h>

/* BCMX LPORT Flags */

#define BCMX_PORT_LP_ALL        (1 << 0)    /* 0x00000001 */
#define BCMX_PORT_LP_FE         (1 << 1)    /* 0x00000002 */
#define BCMX_PORT_LP_GE         (1 << 2)    /* 0x00000004 */
#define BCMX_PORT_LP_XE         (1 << 3)    /* 0x00000008 */
#define BCMX_PORT_LP_HG         (1 << 4)    /* 0x00000010 */
#define BCMX_PORT_LP_E          (1 << 5)    /* 0x00000020 */
#define BCMX_PORT_LP_PORT       (1 << 6)    /* 0x00000040 */
#define BCMX_PORT_LP_CPU        (1 << 7)    /* 0x00000080 */
#define BCMX_PORT_LP_STACK_INT  (1 << 8)    /* 0x00000100 */
#define BCMX_PORT_LP_STACK_EXT  (1 << 9)    /* 0x00000200 */

typedef struct bcmx_port_config_s {
        bcmx_lplist_t   fe;
        bcmx_lplist_t   ge;
        bcmx_lplist_t   xe;
        bcmx_lplist_t   e;
        bcmx_lplist_t   hg;
        bcmx_lplist_t   port;
        bcmx_lplist_t   cpu;
        bcmx_lplist_t   all;
        bcmx_lplist_t   stack_int; /* Deprecated */
        bcmx_lplist_t   stack_ext;
} bcmx_port_config_t;

/* Port ability */
typedef bcm_port_ability_t bcmx_port_ability_t;


extern int bcmx_port_config_get(bcmx_port_config_t *config);
extern int bcmx_port_enable_set(bcmx_lport_t port, int enable);
extern int bcmx_port_enable_get(bcmx_lport_t port, int *enable);
extern void bcmx_port_ability_t_init(bcmx_port_ability_t *ability);
extern int bcmx_port_ability_advert_set(bcmx_lport_t port,
                                        bcmx_port_ability_t *ability_mask);
extern int bcmx_port_ability_advert_get(bcmx_lport_t port,
                                        bcmx_port_ability_t *ability_mask);
extern int bcmx_port_ability_remote_get(bcmx_lport_t port,
                                        bcmx_port_ability_t *ability_mask);
extern int bcmx_port_ability_local_get(bcmx_lport_t port,
                                       bcmx_port_ability_t *ability_mask);
extern int bcmx_port_advert_set(bcmx_lport_t port,
                                bcm_port_abil_t ability_mask);
extern int bcmx_port_advert_get(bcmx_lport_t port,
                                bcm_port_abil_t *ability_mask);
extern int bcmx_port_advert_remote_get(bcmx_lport_t port,
                                       bcm_port_abil_t *ability_mask);
extern int bcmx_port_ability_get(bcmx_lport_t port,
                                 bcm_port_abil_t *local_ability_mask);
extern int bcmx_port_untagged_vlan_set(bcmx_lport_t port, bcm_vlan_t vid);
extern int bcmx_port_untagged_vlan_get(bcmx_lport_t port, bcm_vlan_t *vid_ptr);
extern int bcmx_port_untagged_priority_set(bcmx_lport_t port, int priority);
extern int bcmx_port_untagged_priority_get(bcmx_lport_t port, int *priority);
extern int bcmx_port_dscp_map_mode_set(bcmx_lport_t port, int mode);
extern int bcmx_port_dscp_map_mode_get(bcmx_lport_t port, int *mode);
extern int bcmx_port_dscp_map_set(bcmx_lport_t port, int srccp, int mapcp,
                                  int prio);
extern int bcmx_port_dscp_map_get(bcmx_lport_t port, int srccp, int *mapcp,
                                  int *prio);
extern int bcmx_port_linkscan_get(bcmx_lport_t port, int *linkscan);
extern int bcmx_port_linkscan_set(bcmx_lport_t port, int linkscan);
extern int bcmx_port_autoneg_get(bcmx_lport_t port, int *autoneg);
extern int bcmx_port_autoneg_set(bcmx_lport_t port, int autoneg);
extern int bcmx_port_speed_max(bcmx_lport_t port, int *speed);
extern int bcmx_port_speed_get(bcmx_lport_t port, int *speed);
extern int bcmx_port_speed_set(bcmx_lport_t port, int speed);
extern int bcmx_port_master_get(bcmx_lport_t port, int *ms);
extern int bcmx_port_master_set(bcmx_lport_t port, int ms);
extern int bcmx_port_interface_get(bcmx_lport_t port, bcm_port_if_t *intf);
extern int bcmx_port_interface_set(bcmx_lport_t port, bcm_port_if_t intf);
extern int bcmx_port_duplex_get(bcmx_lport_t port, int *duplex);
extern int bcmx_port_duplex_set(bcmx_lport_t port, int duplex);
extern int bcmx_port_pause_get(bcmx_lport_t port,
                               int *pause_tx, int *pause_rx);
extern int bcmx_port_pause_set(bcmx_lport_t port, int pause_tx, int pause_rx);
extern int bcmx_port_pause_addr_get(bcmx_lport_t port, bcm_mac_t mac);
extern int bcmx_port_pause_addr_set(bcmx_lport_t port, bcm_mac_t mac);
extern int bcmx_port_pause_sym_set(bcmx_lport_t port, int pause);
extern int bcmx_port_pause_sym_get(bcmx_lport_t port, int *pause);
extern int bcmx_port_update(bcmx_lport_t port, int link);
extern int bcmx_port_frame_max_set(bcmx_lport_t port, int size);
extern int bcmx_port_frame_max_get(bcmx_lport_t port, int *size);
extern int bcmx_port_l3_mtu_get(bcmx_lport_t port, int *size);
extern int bcmx_port_l3_mtu_set(bcmx_lport_t port, int size);
extern int bcmx_port_jam_set(bcmx_lport_t port, int enable);
extern int bcmx_port_jam_get(bcmx_lport_t port, int *enable);
extern int bcmx_port_ifg_set(bcmx_lport_t port, int speed,
                             bcm_port_duplex_t duplex, int bit_times);
extern int bcmx_port_ifg_get(bcmx_lport_t port, int speed,
                             bcm_port_duplex_t duplex, int *bit_times);
extern int bcmx_port_phy_get(bcmx_lport_t port, uint32 flags,
                             uint32 phy_reg_addr, uint32 *phy_data);
extern int bcmx_port_phy_set(bcmx_lport_t port, uint32 flags,
                             uint32 phy_reg_addr, uint32 phy_data);
extern int bcmx_port_mdix_set(bcmx_lport_t port, bcm_port_mdix_t mode);
extern int bcmx_port_mdix_get(bcmx_lport_t port, bcm_port_mdix_t *mode);
extern int bcmx_port_mdix_status_get(bcmx_lport_t port,
                                     bcm_port_mdix_status_t *status);
extern int bcmx_port_medium_config_set(bcmx_lport_t port,
                                       bcm_port_medium_t medium,
                                       bcm_phy_config_t  *config);
extern int bcmx_port_medium_config_get(bcmx_lport_t port,
                                       bcm_port_medium_t  medium,
                                       bcm_phy_config_t  *config);
extern int bcmx_port_medium_get(bcmx_lport_t port, bcm_port_medium_t *medium);
extern int bcmx_port_loopback_set(bcmx_lport_t port, int loopback);
extern int bcmx_port_loopback_get(bcmx_lport_t port, int *loopback);
extern int bcmx_port_stp_get(bcmx_lport_t port, int *state);
extern int bcmx_port_stp_set(bcmx_lport_t port, int state);
extern int bcmx_port_discard_get(bcmx_lport_t port, int *mode);
extern int bcmx_port_discard_set(bcmx_lport_t port, int mode);
extern int bcmx_port_learn_get(bcmx_lport_t port, uint32 *flags);
extern int bcmx_port_learn_set(bcmx_lport_t port, uint32 flags);
extern int bcmx_port_learn_modify(bcmx_lport_t port, uint32 add,
                                  uint32 remove);
extern int bcmx_port_link_status_get(bcmx_lport_t port, int *up);
extern int bcmx_port_ifilter_get(bcmx_lport_t port, int *mode);
extern int bcmx_port_ifilter_set(bcmx_lport_t port, int mode);
extern int bcmx_port_vlan_member_get(bcmx_lport_t port, uint32 *flags);
extern int bcmx_port_vlan_member_set(bcmx_lport_t port, uint32 flags);
extern int bcmx_port_bpdu_enable_set(bcmx_lport_t port, int enable);
extern int bcmx_port_bpdu_enable_get(bcmx_lport_t port, int *enable);
extern int bcmx_port_l3_enable_set(bcmx_lport_t port, int enable);
extern int bcmx_port_l3_enable_get(bcmx_lport_t port, int *enable);
extern int bcmx_port_tgid_set(bcmx_lport_t port, int tid, int psc);
extern int bcmx_port_tgid_get(bcmx_lport_t port, int *tid, int *psc);
extern int bcmx_port_pfm_set(bcmx_lport_t port, int mode);
extern int bcmx_port_pfm_get(bcmx_lport_t port, int *mode);
extern int bcmx_port_encap_set(bcmx_lport_t port, int mode);
extern int bcmx_port_encap_get(bcmx_lport_t port, int *mode);
extern int bcmx_port_queued_count_get(bcmx_lport_t port, uint32 *count);
extern int bcmx_port_protocol_vlan_add(bcmx_lport_t port,
                                       bcm_port_frametype_t frame,
                                       bcm_port_ethertype_t ether,
                                       bcm_vlan_t vid);
extern int bcmx_port_protocol_vlan_delete(bcmx_lport_t port,
                                          bcm_port_frametype_t frame,
                                          bcm_port_ethertype_t ether);
extern int bcmx_port_protocol_vlan_delete_all(bcmx_lport_t port);
extern int bcmx_port_egress_set(bcmx_lport_t port,
                                int modid, bcmx_lplist_t lplist);
extern int bcmx_port_egress_get(bcmx_lport_t port,
                                int modid, bcmx_lplist_t *lplist);
extern int bcmx_port_modid_enable_set(bcmx_lport_t port,
                                      int modid, int enable);
extern int bcmx_port_modid_enable_get(bcmx_lport_t port,
                                      int modid, int *enable);
extern int bcmx_port_flood_block_set(bcmx_lport_t ingress_port,
                                     bcmx_lport_t egress_port, uint32 flags);
extern int bcmx_port_flood_block_get(bcmx_lport_t ingress_port,
                                     bcmx_lport_t egress_port, uint32 *flags);
extern int bcmx_port_rate_egress_set(bcmx_lport_t port,
                                     uint32 kbits_sec, uint32 kbits_burst);
extern int bcmx_port_rate_egress_get(bcmx_lport_t port,
                                     uint32 *kbits_sec, uint32 *kbits_burst);
extern int bcmx_port_rate_ingress_set(bcmx_lport_t port,
                                      uint32 kbits_sec, uint32 kbits_burst);
extern int bcmx_port_rate_ingress_get(bcmx_lport_t port,
                                      uint32 *kbits_sec, uint32 *kbits_burst);
extern int bcmx_port_rate_pause_set(bcmx_lport_t port,
                                    uint32 kbits_pause, uint32 kbits_resume);
extern int bcmx_port_rate_pause_get(bcmx_lport_t port,
                                    uint32 *kbits_pause, uint32 *kbits_resume);
extern int bcmx_port_sample_rate_set(bcm_port_t port,
                                     int ingress_rate, int egress_rate);
extern int bcmx_port_sample_rate_get(bcm_port_t port,
                                     int *ingress_rate, int *egress_rate);
extern int bcmx_port_dtag_mode_set(bcmx_lport_t port, int mode);
extern int bcmx_port_dtag_mode_get(bcmx_lport_t port, int *mode);
extern int bcmx_port_tpid_set(bcmx_lport_t port, uint16 tpid);
extern int bcmx_port_tpid_get(bcmx_lport_t port, uint16 *tpid);
extern int bcmx_port_tpid_add(bcmx_lport_t port, uint16 tpid, 
                              int color_select); 
extern int bcmx_port_tpid_delete(bcmx_lport_t port, uint16 tpid);
extern int bcmx_port_tpid_delete_all(bcmx_lport_t port);
extern int bcmx_port_inner_tpid_set(bcmx_lport_t port, uint16 tpid);
extern int bcmx_port_inner_tpid_get(bcmx_lport_t port, uint16 *tpid);
extern int bcmx_port_cable_diag(bcmx_lport_t port,
                                bcm_port_cable_diag_t *status);
extern int bcmx_port_info_get(bcmx_lport_t port, bcm_port_info_t *info);
extern int bcmx_port_info_set(bcmx_lport_t port, bcm_port_info_t *info);
extern int bcmx_port_selective_get(bcmx_lport_t port, bcm_port_info_t *info);
extern int bcmx_port_selective_set(bcmx_lport_t port, bcm_port_info_t *info);
extern int bcmx_port_info_restore(bcmx_lport_t port, bcm_port_info_t *info);
extern int bcmx_port_info_save(bcmx_lport_t port, bcm_port_info_t *info);
extern int bcmx_port_fault_get(bcmx_lport_t port, uint32 *flags);
extern int bcmx_port_priority_color_set(bcmx_lport_t port, int prio,
                                        bcm_color_t color);
extern int bcmx_port_priority_color_get(bcmx_lport_t port, int prio,
                                        bcm_color_t *color);
extern int bcmx_port_cfi_color_set(bcmx_lport_t port, int cfi,
                                   bcm_color_t color);
extern int bcmx_port_cfi_color_get(bcmx_lport_t port, int cfi,
                                   bcm_color_t *color);
extern int bcmx_port_vlan_priority_map_set(bcmx_lport_t port,
                                           int pkt_pri,
                                           int cfi,
                                           int internal_pri,
                                           bcm_color_t color);
extern int bcmx_port_vlan_priority_map_get(bcmx_lport_t port,
                                           int pkt_pri,
                                           int cfi,
                                           int *internal_pri,
                                           bcm_color_t *color);
extern int bcmx_port_vlan_priority_unmap_set(bcmx_lport_t port,
                                             int internal_pri,
                                             bcm_color_t color,
                                             int pkt_pri, int cfi);
extern int bcmx_port_vlan_priority_unmap_get(bcmx_lport_t port,
                                             int internal_pri,
                                             bcm_color_t color,
                                             int *pkt_pri, int *cfi);
extern int bcmx_port_vlan_inner_tag_set(bcmx_lport_t port, 
                                        uint16 inner_tag);
extern int bcmx_port_vlan_inner_tag_get(bcmx_lport_t port,
                                        uint16 *inner_tag);
extern int bcmx_port_class_set(bcmx_lport_t port, bcm_port_class_t pclass,
                               uint32 pclass_id);
extern int bcmx_port_class_get(bcmx_lport_t port, bcm_port_class_t pclass,
                               uint32 *pclass_id);
extern int bcmx_port_l3_modify_set(bcmx_lport_t port,    /* ingress port */
                                   uint32 flags);
extern int bcmx_port_l3_modify_get(bcmx_lport_t port,    /* ingress port */
                                   uint32 *flags);
extern int bcmx_port_ipmc_modify_set(bcmx_lport_t port, /* egress port */
                                     uint32 flags);
extern int bcmx_port_ipmc_modify_get(bcmx_lport_t port, /* egress port */
                                     uint32 *flags);
extern int bcmx_port_lplist_populate(bcmx_lplist_t *list, uint32 flags);
extern int bcmx_port_phy_control_set(bcmx_lport_t port,
                                     bcm_port_phy_control_t type,
                                     uint32 value);
extern int bcmx_port_phy_control_get(bcmx_lport_t port,
                                     bcm_port_phy_control_t type,
                                     uint32 *value);
extern int bcmx_port_force_vlan_set(bcmx_lport_t port, bcm_vlan_t vlan,
                                    int pkt_prio, uint32 flags);
extern int bcmx_port_force_vlan_get(bcmx_lport_t port, bcm_vlan_t *vlan,
                                    int *pkt_prio, uint32 *flags);

extern int bcmx_port_gport_get(bcmx_lport_t port, bcm_gport_t *gport);

#endif  /* _BCMX_PORT_H */
