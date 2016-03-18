/*
 * $Id: field.h,v 1.44.2.1 Broadcom SDK $
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
 * BCMX Field Processor APIs
 */

#ifndef _BCMX_FIELD_H
#define _BCMX_FIELD_H

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include <bcm/field.h>
#include <bcmx/policer.h>

/* Initialization */

extern int bcmx_field_init(void);

extern int bcmx_field_control_set(bcm_field_control_t control,
                                  uint32 state);

extern int bcmx_field_control_get(bcm_field_control_t control,
                                  uint32 *state);

/* Group Management */

extern int bcmx_field_group_create(bcm_field_qset_t qset,
                                   int pri,
                                   bcm_field_group_t *group);

extern int bcmx_field_group_create_id(bcm_field_qset_t qset,
                                      int pri,
                                      bcm_field_group_t group);

extern int bcmx_field_group_create_mode(bcm_field_qset_t qset,
                                        int pri,
                                        bcm_field_group_mode_t mode,
                                        bcm_field_group_t *group);

extern int bcmx_field_group_create_mode_id(bcm_field_qset_t qset,
                                           int pri,
                                           bcm_field_group_mode_t mode,
                                           bcm_field_group_t group);

extern int bcmx_field_group_port_create_mode(bcmx_lport_t port,
                                             bcm_field_qset_t qset,
                                             int pri,
                                             bcm_field_group_mode_t mode,
                                             bcm_field_group_t *group);

extern int bcmx_field_group_port_create_mode_id(bcmx_lport_t port,
                                                bcm_field_qset_t qset,
                                                int pri,
                                                bcm_field_group_mode_t mode,
                                                bcm_field_group_t group);

extern int bcmx_field_group_ports_create_mode(bcmx_lplist_t lplist,
                                              bcm_field_qset_t qset,
                                              int pri,
                                              bcm_field_group_mode_t mode,
                                              bcm_field_group_t *group);

extern int bcmx_field_group_ports_create_mode_id(bcmx_lplist_t lplist,
                                                 bcm_field_qset_t qset,
                                                 int pri,
                                                 bcm_field_group_mode_t mode,
                                                 bcm_field_group_t group);

extern int bcmx_field_group_mode_get(bcm_field_group_t group,
                                     bcm_field_group_mode_t *mode);

extern int bcmx_field_group_get(bcm_field_group_t group,
                                bcm_field_qset_t *qset);

extern int bcmx_field_group_set(bcm_field_group_t group,
                                bcm_field_qset_t qset);

extern int bcmx_field_group_destroy(bcm_field_group_t group);

extern int bcmx_field_group_enable_set(bcm_field_group_t group,
                                       int enable);

extern int bcmx_field_group_enable_get(bcm_field_group_t group,
                                       int *enable);

extern int bcmx_field_range_create(bcm_field_range_t *range,
                                   uint32 flags,
                                   bcm_l4_port_t min,
                                   bcm_l4_port_t max);

extern int bcmx_field_range_create_id(bcm_field_range_t range,
                                      uint32 flags,
                                      bcm_l4_port_t min,
                                      bcm_l4_port_t max);

extern int bcmx_field_range_group_create(bcm_field_range_t *range,
                                         uint32 flags,
                                         bcm_l4_port_t min,
                                         bcm_l4_port_t max, 
                                         bcm_if_group_t group);

extern int bcmx_field_range_group_create_id(bcm_field_range_t range,
                                            uint32 flags,
                                            bcm_l4_port_t min,
                                            bcm_l4_port_t max, 
                                            bcm_if_group_t group);

extern int bcmx_field_range_get(bcm_field_range_t range,
                                uint32 *flags,
                                bcm_l4_port_t *min,
                                bcm_l4_port_t *max);

extern int bcmx_field_range_destroy(bcm_field_range_t range);


/* User-Defined Fields */

extern int bcmx_field_udf_spec_set(bcm_field_udf_spec_t *udf_spec,
                                   uint32 flags,
                                   uint32 offset);

extern int bcmx_field_udf_spec_get(bcm_field_udf_spec_t *udf_spec,
                                   uint32 *flags,
                                   uint32 *offset);

extern int bcmx_field_udf_create(bcm_field_udf_spec_t *udf_spec,
                                 bcm_field_udf_t *udf_id);

extern int bcmx_field_udf_create_id(bcm_field_udf_spec_t *udf_spec,
                                    bcm_field_udf_t udf_id);

extern int bcmx_field_udf_ethertype_set(int index,
                                        bcm_port_frametype_t frametype,
                                        bcm_port_ethertype_t ethertype);

extern int bcmx_field_udf_ethertype_get(int index,
                                        bcm_port_frametype_t *frametype,
                                        bcm_port_ethertype_t *ethertype);

extern int bcmx_field_udf_ipprotocol_set(int index,
                                         uint32 flags,
                                         uint8 proto);

extern int bcmx_field_udf_ipprotocol_get(int index,
                                         uint32 *flags,
                                         uint8 *proto);

extern int bcmx_field_udf_get(bcm_field_udf_spec_t *udf_spec,
                              bcm_field_udf_t udf_id);

extern int bcmx_field_udf_destroy(bcm_field_udf_t udf_id);

extern int bcmx_field_qset_add_udf(bcm_field_qset_t *qset,
                                   bcm_field_udf_t udf_id);

/* Entry Management */

extern int bcmx_field_entry_create(bcm_field_group_t group,
                                   bcm_field_entry_t *entry);

extern int bcmx_field_entry_create_id(bcm_field_group_t group,
                                      bcm_field_entry_t entry);

extern int bcmx_field_entry_destroy(bcm_field_entry_t entry);

extern int bcmx_field_entry_destroy_all(void);

extern int bcmx_field_entry_copy(bcm_field_entry_t src_entry,
                                 bcm_field_entry_t *dst_entry);

extern int bcmx_field_entry_install(bcm_field_entry_t entry);

extern int bcmx_field_entry_reinstall(bcm_field_entry_t entry);

extern int bcmx_field_entry_remove(bcm_field_entry_t entry);

extern int bcmx_field_entry_prio_get(bcm_field_entry_t entry,
                                     int *prio);

extern int bcmx_field_entry_prio_set(bcm_field_entry_t entry,
                                     int prio);

extern int bcmx_field_entry_policer_attach(bcm_field_entry_t entry_id, 
                                           int level, 
                                           bcm_policer_t policer_id);

extern int bcmx_field_entry_policer_detach(bcm_field_entry_t entry_id, 
                                           int level);

extern int bcmx_field_entry_policer_detach_all(bcm_field_entry_t entry_id);

extern int bcmx_field_entry_policer_get(bcm_field_entry_t entry_id, 
                                        int level, 
                                        bcm_policer_t *policer_id);

extern int bcmx_field_resync(void);


/*
 * Entry Field Qualification
 *
 *   The set of qualify functions are named to match the
 *   bcm_field_qualify_t enumeration names.
 */

extern int bcmx_field_qualify_clear(bcm_field_entry_t entry);

extern int bcmx_field_qualify_InPort(bcm_field_entry_t entry,
                                     bcmx_lport_t port);

extern int bcmx_field_qualify_OutPort(bcm_field_entry_t entry,
                                      bcmx_lport_t port);

extern int bcmx_field_qualify_InPorts(bcm_field_entry_t entry,
                                      bcmx_lplist_t lplist);

extern int bcmx_field_qualify_OutPorts(bcm_field_entry_t entry,
                                       bcmx_lplist_t lplist);

extern int bcmx_field_qualify_Drop(bcm_field_entry_t entry,
                                   uint8 data, uint8 mask);

extern int bcmx_field_qualify_SrcPort(bcm_field_entry_t entry,
                                      bcmx_lport_t port);

extern int bcmx_field_qualify_SrcTrunk(bcm_field_entry_t entry,
                                       bcm_trunk_t tid);

extern int bcmx_field_qualify_DstPort(bcm_field_entry_t entry,
                                      bcmx_lport_t port);

extern int bcmx_field_qualify_DstTrunk(bcm_field_entry_t entry,
                                       bcm_trunk_t tid);

extern int bcmx_field_qualify_SrcModid(bcm_field_entry_t entry,
                                       bcm_module_t data, bcm_module_t mask);

extern int bcmx_field_qualify_DstModid(bcm_field_entry_t entry,
                                       bcm_module_t data, bcm_module_t mask);

extern int bcmx_field_qualify_L4SrcPort(bcm_field_entry_t entry,
                                        bcm_l4_port_t data, bcm_l4_port_t mask);

extern int bcmx_field_qualify_L4DstPort(bcm_field_entry_t entry,
                                        bcm_l4_port_t data, bcm_l4_port_t mask);

extern int bcmx_field_qualify_OuterVlan(bcm_field_entry_t entry,
                                        bcm_vlan_t data, bcm_vlan_t mask);

extern int bcmx_field_qualify_InnerVlan(bcm_field_entry_t entry,
                                        bcm_vlan_t data, bcm_vlan_t mask);

extern int bcmx_field_qualify_EtherType(bcm_field_entry_t entry,
                                        uint16 data, uint16 mask);

extern int bcmx_field_qualify_IpProtocol(bcm_field_entry_t entry,
                                         uint8 data, uint8 mask);

extern int bcmx_field_qualify_LookupStatus(bcm_field_entry_t entry,
                                           uint32 data, uint32 mask);

extern int bcmx_field_qualify_IpInfo(bcm_field_entry_t entry,
                                     uint32 data, uint32 mask);

extern int bcmx_field_qualify_PacketRes(bcm_field_entry_t entry,
                                        uint32 data, uint32 mask);

extern int bcmx_field_qualify_SrcIp(bcm_field_entry_t entry,
                                    bcm_ip_t data, bcm_ip_t mask);

extern int bcmx_field_qualify_DstIp(bcm_field_entry_t entry,
                                    bcm_ip_t data, bcm_ip_t mask);

extern int bcmx_field_qualify_DSCP(bcm_field_entry_t entry,
                                   uint8 data, uint8 mask);

extern int bcmx_field_qualify_Tos(bcm_field_entry_t entry,
                                  uint8 data, uint8 mask);

extern int bcmx_field_qualify_IpFlags(bcm_field_entry_t entry,
                                      uint8 data, uint8 mask);

extern int bcmx_field_qualify_TcpControl(bcm_field_entry_t entry,
                                         uint8 data, uint8 mask);

extern int bcmx_field_qualify_Ttl(bcm_field_entry_t entry,
                                  uint8 data, uint8 mask);

extern int bcmx_field_qualify_RangeCheck(bcm_field_entry_t entry,
                                         int range, int result);

extern int bcmx_field_qualify_SrcIp6(bcm_field_entry_t entry,
                                     bcm_ip6_t data, bcm_ip6_t mask);

extern int bcmx_field_qualify_DstIp6(bcm_field_entry_t entry,
                                     bcm_ip6_t data, bcm_ip6_t mask);

extern int bcmx_field_qualify_SrcIp6High(bcm_field_entry_t entry,
                                         bcm_ip6_t data, bcm_ip6_t mask);

extern int bcmx_field_qualify_DstIp6High(bcm_field_entry_t entry,
                                         bcm_ip6_t data, bcm_ip6_t mask);

extern int bcmx_field_qualify_Ip6NextHeader(bcm_field_entry_t entry,
                                            uint8 data, uint8 mask);

extern int bcmx_field_qualify_Ip6TrafficClass(bcm_field_entry_t entry,
                                              uint8 data, uint8 mask);

extern int bcmx_field_qualify_Ip6FlowLabel(bcm_field_entry_t entry,
                                           uint32 data, uint32 mask);

extern int bcmx_field_qualify_Ip6HopLimit(bcm_field_entry_t entry,
                                          uint8 data, uint8 mask);

extern int bcmx_field_qualify_SrcMac(bcm_field_entry_t entry,
                                     bcm_mac_t data, bcm_mac_t mask);

extern int bcmx_field_qualify_DstMac(bcm_field_entry_t entry,
                                     bcm_mac_t data, bcm_mac_t mask);

extern int bcmx_field_qualify_PacketFormat(bcm_field_entry_t entry,
                                           uint32 data, uint32 mask);

extern int bcmx_field_qualify_SrcMacGroup(bcm_field_entry_t entry,
                                          uint32 data, uint32 mask);

extern int bcmx_field_qualify_SrcClassL2(bcm_field_entry_t entry,
                                         uint32 data, uint32 mask);
extern int bcmx_field_qualify_SrcClassL3(bcm_field_entry_t entry,
                                         uint32 data, uint32 mask);
extern int bcmx_field_qualify_SrcClassField(bcm_field_entry_t entry,
                                            uint32 data, uint32 mask);

extern int bcmx_field_qualify_DstClassL2(bcm_field_entry_t entry,
                                         uint32 data, uint32 mask);
extern int bcmx_field_qualify_DstClassL3(bcm_field_entry_t entry,
                                         uint32 data, uint32 mask);
extern int bcmx_field_qualify_DstClassField(bcm_field_entry_t entry,
                                            uint32 data, uint32 mask);

extern int bcmx_field_qualify_IpType(bcm_field_entry_t entry,
                                     bcm_field_IpType_t type);

extern int bcmx_field_qualify_L2Format(bcm_field_entry_t entry,
                                       bcm_field_L2Format_t type);

extern int bcmx_field_qualify_VlanFormat(bcm_field_entry_t entry,
                                         uint8 data, uint8 mask);

extern int bcmx_field_qualify_MHOpcode(bcm_field_entry_t entry,
                                       uint8 data, uint8 mask);

extern int bcmx_field_qualify_UserDefined(bcm_field_entry_t entry,
                                          bcm_field_udf_t udf_id,
                                          uint8 *data, uint8 *mask);

extern int bcmx_field_qualify_Decap(bcm_field_entry_t entry,
                                    bcm_field_decap_t decap);

extern int bcmx_field_qualify_HiGig(bcm_field_entry_t entry,
                                    uint8 data, uint8 mask);

extern int bcmx_field_qualify_Stage(bcm_field_entry_t entry,
                                    bcm_field_stage_t stage);

extern int bcmx_field_qualify_L3IntfGroup(bcm_field_entry_t entry,
                                          bcm_if_group_t data,
                                          bcm_if_group_t mask);

extern int bcmx_field_qualify_InterfaceClassL2 (bcm_field_entry_t entry,
                                              uint32 data, uint32 mask);
extern int bcmx_field_qualify_InterfaceClassL3 (bcm_field_entry_t entry,
                                                uint32 data, uint32 mask);
extern int bcmx_field_qualify_InterfaceClassPort (bcm_field_entry_t entry,
                                                  uint32 data, uint32 mask);

extern int bcmx_field_qualify_IpProtocolCommon(bcm_field_entry_t entry,
                                               bcm_field_IpProtocolCommon_t protocol);

extern int bcmx_field_qualify_Snap(bcm_field_entry_t entry,
                                   bcm_field_snap_header_t data,
                                   bcm_field_snap_header_t mask);

extern int bcmx_field_qualify_Llc(bcm_field_entry_t entry,
                                  bcm_field_llc_header_t data,
                                  bcm_field_llc_header_t mask);

extern int bcmx_field_qualify_InnerTpid(bcm_field_entry_t entry,
                                        uint16 tpid);

extern int bcmx_field_qualify_OuterTpid(bcm_field_entry_t entry,
                                        uint16 tpid);

extern int bcmx_field_qualify_PortClass(bcm_field_entry_t entry,
                                        uint32 data, uint32 mask);

extern int bcmx_field_qualify_L3Routable(bcm_field_entry_t entry,
                                         uint8 data, uint8 mask);

extern int bcmx_field_qualify_IpFrag(bcm_field_entry_t entry,
                                     bcm_field_IpFrag_t frag_info);

extern int bcmx_field_qualify_LookupClass0(bcm_field_entry_t entry,
                                           uint32 data, uint32 mask);

extern int bcmx_field_qualify_Vrf(bcm_field_entry_t entry,
                                  uint32 data, uint32 mask);

extern int bcmx_field_qualify_ExtensionHeaderType(bcm_field_entry_t entry, 
                                                  uint8 data, uint8 mask);

extern int bcmx_field_qualify_ExtensionHeaderSubCode(bcm_field_entry_t entry, 
                                                     uint8 data, uint8 mask);

extern int bcmx_field_qualify_L4Ports(bcm_field_entry_t entry, 
                                      uint8 data, uint8 mask);

extern int bcmx_field_qualify_MirrorCopy(bcm_field_entry_t entry, 
                                         uint8 data, uint8 mask);

extern int bcmx_field_qualify_TunnelTerminated(bcm_field_entry_t entry, 
                                               uint8 data, uint8 mask);

extern int bcmx_field_qualify_MplsTerminated(bcm_field_entry_t entry, 
                                             uint8 data, uint8 mask);

extern int bcmx_field_qualify_InnerSrcIp(bcm_field_entry_t entry, 
                                         bcm_ip_t data, bcm_ip_t mask);

extern int bcmx_field_qualify_InnerDstIp(bcm_field_entry_t entry, 
                                         bcm_ip_t data, bcm_ip_t mask);

extern int bcmx_field_qualify_InnerSrcIp6(bcm_field_entry_t entry, 
                                          bcm_ip6_t data, bcm_ip6_t mask);

extern int bcmx_field_qualify_InnerDstIp6(bcm_field_entry_t entry, 
                                          bcm_ip6_t data, bcm_ip6_t mask);

extern int bcmx_field_qualify_InnerSrcIp6High(bcm_field_entry_t entry, 
                                              bcm_ip6_t data, bcm_ip6_t mask);

extern int bcmx_field_qualify_InnerDstIp6High(bcm_field_entry_t entry, 
                                              bcm_ip6_t data, bcm_ip6_t mask);

extern int bcmx_field_qualify_InnerTtl(bcm_field_entry_t entry, 
                                       uint8 data, uint8 mask);

extern int bcmx_field_qualify_InnerDSCP(bcm_field_entry_t entry, 
                                        uint8 data, uint8 mask);

extern int bcmx_field_qualify_InnerIpProtocol(bcm_field_entry_t entry, 
                                              uint8 data, uint8 mask);

extern int bcmx_field_qualify_InnerIpFrag(bcm_field_entry_t entry, 
                                          bcm_field_IpFrag_t frag_info);

extern int bcmx_field_qualify_DosAttack(bcm_field_entry_t entry, 
                                        uint8 data, uint8 mask);

extern int bcmx_field_qualify_IpmcStarGroupHit(bcm_field_entry_t entry, 
                                               uint8 data, uint8 mask);

extern int bcmx_field_qualify_L3DestRouteHit(bcm_field_entry_t entry, 
                                             uint8 data, uint8 mask);

extern int bcmx_field_qualify_L3DestHostHit(bcm_field_entry_t entry, 
                                            uint8 data, uint8 mask);

extern int bcmx_field_qualify_L3SrcHostHit(bcm_field_entry_t entry, 
                                           uint8 data, uint8 mask);

extern int bcmx_field_qualify_L2CacheHit(bcm_field_entry_t entry, 
                                         uint8 data, uint8 mask);

extern int bcmx_field_qualify_L2StationMove(bcm_field_entry_t entry, 
                                            uint8 data, uint8 mask);

extern int bcmx_field_qualify_L2DestHit(bcm_field_entry_t entry, 
                                        uint8 data, uint8 mask);

extern int bcmx_field_qualify_L2SrcStatic(bcm_field_entry_t entry, 
                                          uint8 data, uint8 mask);

extern int bcmx_field_qualify_L2SrcHit(bcm_field_entry_t entry, 
                                       uint8 data, uint8 mask);

extern int bcmx_field_qualify_IngressStpState(bcm_field_entry_t entry, 
                                              uint8 data, uint8 mask);

extern int bcmx_field_qualify_ForwardingVlanValid(bcm_field_entry_t entry, 
                                                  uint8 data, uint8 mask);

extern int bcmx_field_qualify_VlanTranslationHit(bcm_field_entry_t entry, 
                                                 uint8 data, uint8 mask);


/* Entry Action Management */

extern int bcmx_field_action_add(bcm_field_entry_t entry,
                                 bcm_field_action_t action,
                                 uint32 param0,
                                 uint32 param1);

extern int bcmx_field_action_get(bcm_field_entry_t entry,
                                 bcm_field_action_t action,
                                 uint32 *param0,
                                 uint32 *param1);

extern int bcmx_field_action_ports_add(bcm_field_entry_t entry,
                                       bcm_field_action_t action,
                                       bcmx_lplist_t lplist);

extern int bcmx_field_action_ports_get(bcm_field_entry_t entry,
                                       bcm_field_action_t action,
                                       bcmx_lplist_t *lplist);

extern int bcmx_field_action_delete(bcm_field_entry_t entry,
                                    bcm_field_action_t action, 
                                    uint32 param0, uint32 param1);

extern int bcmx_field_action_remove(bcm_field_entry_t entry,
                                    bcm_field_action_t action);

extern int bcmx_field_action_remove_all(bcm_field_entry_t entry);

/* Counter Management */

extern int bcmx_field_counter_create(bcm_field_entry_t entry);

extern int bcmx_field_counter_share(bcm_field_entry_t src_entry,
                                    bcm_field_entry_t dst_entry);

extern int bcmx_field_counter_destroy(bcm_field_entry_t entry);

extern int bcmx_field_counter_set(bcm_field_entry_t entry,
                                  int counter_num,
                                  uint64 val);

extern int bcmx_field_counter_set32(bcm_field_entry_t entry,
                                    int counter_num,
                                    uint32 val);

extern int bcmx_field_counter_get(bcm_field_entry_t entry,
                                  int counter_num,
                                  uint64 *valp);

extern int bcmx_field_counter_get32(bcm_field_entry_t entry,
                                    int counter_num,
                                    uint32 *valp);

/* Meter Management */

extern int bcmx_field_meter_create(bcm_field_entry_t entry);

extern int bcmx_field_meter_share(bcm_field_entry_t src_entry,
                                  bcm_field_entry_t dst_entry);

extern int bcmx_field_meter_destroy(bcm_field_entry_t entry);

extern int bcmx_field_meter_set(bcm_field_entry_t entry,
                                int meter_num,
                                uint32 kbits_sec,
                                uint32 kbits_burst);

extern int bcmx_field_meter_get(bcm_field_entry_t entry,
                                int meter_num,
                                uint32 *kbits_sec,
                                uint32 *kbits_burst);

/* SDK 5.2.0 backward compat. */

#define bcmx_field_qualify_OutVlan  bcmx_field_qualify_OuterVlan
#define bcmx_field_qualify_InVlan   bcmx_field_qualify_InnerVlan

#endif  /* !_BCMX_FIELD_H */
