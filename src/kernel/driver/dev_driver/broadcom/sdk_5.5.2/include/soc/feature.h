/*
 * $Id: feature.h,v 1.228.2.15.4.1 Broadcom SDK $
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
 * SOC_FEATURE definitions
 *
 * File:        feature.h
 * Purpose:     Define features by chip
 */

#ifndef _SOC_FEATURE_H
#define _SOC_FEATURE_H

#include <shared/enumgen.h>

/*
 * Defines enums of the form soc_feature_xxx, and a corresponding list
 * of initializer strings "xxx".  New additions should be made in the
 * following section between the zero and count entries.
 */

#define SHR_G_ENTRIES(P, DONT_CARE)                                          \
SHR_G_ENTRY(P, zero),              /* ALWAYS FIRST PLEASE (DO NOT CHANGE) */ \
SHR_G_ENTRY(P, add_gth_prefix),                                              \
SHR_G_ENTRY(P, arl_sorted),                                                  \
SHR_G_ENTRY(P, arl_hashed),                                                  \
SHR_G_ENTRY(P, arl_lookup_cmd),                                              \
SHR_G_ENTRY(P, arl_lookup_retry),  /* All-0's response requires retry */     \
SHR_G_ENTRY(P, arl_lookup_erratum),/* Pre-read required */                   \
SHR_G_ENTRY(P, arl_ro),                                                      \
SHR_G_ENTRY(P, arl_static_ff),                                               \
SHR_G_ENTRY(P, arl_insert_ovr),    /* ARL ins overwrites if key exists */    \
SHR_G_ENTRY(P, cfap_pool),         /* Software CFAP initialization. */       \
SHR_G_ENTRY(P, cos_rx_dma),        /* COS based RX DMA */                    \
SHR_G_ENTRY(P, dcb_type1),         /* 5600, 5680 */                          \
SHR_G_ENTRY(P, dcb_type2),         /* 5605, 5615, 5625, 5645 */              \
SHR_G_ENTRY(P, dcb_type3),         /* 5690 */                                \
SHR_G_ENTRY(P, dcb_type4),         /* 5670 */                                \
SHR_G_ENTRY(P, dcb_type5),         /* 5673 */                                \
SHR_G_ENTRY(P, dcb_type6),         /* 5665 */                                \
SHR_G_ENTRY(P, dcb_type7),         /* 5695 when enabled */                   \
SHR_G_ENTRY(P, dcb_type8),         /* not used (5345) */                     \
SHR_G_ENTRY(P, dcb_type9),         /* 56504 */                               \
SHR_G_ENTRY(P, dcb_type10),        /* 56601 */                               \
SHR_G_ENTRY(P, dcb_type11),        /* 56580, 56700, 56800 */                 \
SHR_G_ENTRY(P, dcb_type12),        /* 56218 */                               \
SHR_G_ENTRY(P, dcb_type13),        /* 56514 */                               \
SHR_G_ENTRY(P, dcb_type14),        /* 56624, 56680 */                        \
SHR_G_ENTRY(P, dcb_type15),        /* 56224 A0 */                            \
SHR_G_ENTRY(P, dcb_type16),        /* 56820 */                               \
SHR_G_ENTRY(P, dcb_type18),        /* 56224 B0 */                            \
SHR_G_ENTRY(P, fe_gig_macs),                                                 \
SHR_G_ENTRY(P, trimac),                                                      \
SHR_G_ENTRY(P, ffp_cntrs),                                                   \
SHR_G_ENTRY(P, filter),                                                      \
SHR_G_ENTRY(P, filter_extended),                                             \
SHR_G_ENTRY(P, filter_xgs),                                                  \
SHR_G_ENTRY(P, filter_pktfmtext),  /* Enhanced pkt format spec for XGS */    \
SHR_G_ENTRY(P, filter_metering),                                             \
SHR_G_ENTRY(P, ingress_metering),                                            \
SHR_G_ENTRY(P, egress_metering),                                             \
SHR_G_ENTRY(P, filter_tucana),                                               \
SHR_G_ENTRY(P, filter_draco15),                                              \
SHR_G_ENTRY(P, has_d_hit),                                                   \
SHR_G_ENTRY(P, has_gbp),                                                     \
SHR_G_ENTRY(P, l3),                /* Capable of Layer 3 switching.  */      \
SHR_G_ENTRY(P, l3_ip6),            /* IPv6 capable device. */                \
SHR_G_ENTRY(P, l3_lookup_cmd),                                               \
SHR_G_ENTRY(P, ip_mcast),                                                    \
SHR_G_ENTRY(P, ip_mcast_repl),     /* IPMC replication */                    \
SHR_G_ENTRY(P, led_proc),          /* LED microprocessor */                  \
SHR_G_ENTRY(P, ptable_extra_word),                                           \
SHR_G_ENTRY(P, purge_bug),         /* Chip bug in CMIC */                    \
SHR_G_ENTRY(P, schmsg_alias),                                                \
SHR_G_ENTRY(P, fe_gig_ctrs),       /* Has different counters for fe gig */   \
SHR_G_ENTRY(P, stack_my_modid),                                              \
SHR_G_ENTRY(P, stat_dma),                                                    \
SHR_G_ENTRY(P, cpuport_stat_dma),                                            \
SHR_G_ENTRY(P, table_dma),                                                   \
SHR_G_ENTRY(P, tslam_dma),                                                   \
SHR_G_ENTRY(P, unified_tables),                                              \
SHR_G_ENTRY(P, stg),               /* Generic STG */                         \
SHR_G_ENTRY(P, stg_strata2),       /* Strata 2 implementation specific STG */\
SHR_G_ENTRY(P, stg_xgs),           /* XGS implementation specific STG */     \
SHR_G_ENTRY(P, turbo_gigs),                                                  \
SHR_G_ENTRY(P, turbo_25v),         /* must select 2.5V if in turbo mode */   \
SHR_G_ENTRY(P, dlf_trunk_bitmap),                                            \
SHR_G_ENTRY(P, max_packet_1560),                                             \
SHR_G_ENTRY(P, sdram_64bit),                                                 \
SHR_G_ENTRY(P, stp_block_cml),     /* save cml state when in blocked */      \
SHR_G_ENTRY(P, coll_win_erratum),  /* 5600/5680 errata */                    \
SHR_G_ENTRY(P, fix_mcast_bitmap),  /* Needs mcast bitmap fix on join/lv */   \
SHR_G_ENTRY(P, remap_ut_prio),     /* Remap prio of untagged pkts by port */ \
SHR_G_ENTRY(P, mac_lb_erratum),    /* Bug in BCM5600_B0 MAC loopback */      \
SHR_G_ENTRY(P, link_down_hd),      /* Bug in BCM5605/5615 MAC; see mac.c */  \
SHR_G_ENTRY(P, link_down_fd),      /* Bug in BCM5645 MAC; see mac.c */       \
SHR_G_ENTRY(P, phy_5690_link_war),                                           \
SHR_G_ENTRY(P, phymod),            /* Chip has PHYMOD bit (SMI MAC-to-PHY) */\
SHR_G_ENTRY(P, l3x_delete_bf_bug), /* L3 delete fails if bucket full */      \
SHR_G_ENTRY(P, xgxs_v1),           /* FusionCore version 1 (BCM5670/90 A0) */\
SHR_G_ENTRY(P, xgxs_v2),           /* FusionCore version 2 (BCM5670/90 A1) */\
SHR_G_ENTRY(P, xgxs_v3),           /* FusionCore version 3 (BCM5673) */      \
SHR_G_ENTRY(P, xgxs_v4),           /* FusionCore version 4 (BCM5674) */      \
SHR_G_ENTRY(P, xgxs_v5),           /* FusionCore version 5 (BCM56504/56601)*/\
SHR_G_ENTRY(P, xgxs_v6),           /* FusionCore version 6 (BCM56700/56800)*/\
SHR_G_ENTRY(P, xgxs_v7),           /* FusionCore version 7 (BCM56624/56680)*/\
SHR_G_ENTRY(P, lmd),               /* Lane Mux/DMux 2.5 Gbps speed support */\
SHR_G_ENTRY(P, bigmac_fault_stat), /* BigMac supports link fault status*/    \
SHR_G_ENTRY(P, phy_cl45),          /* Clause 45 phy support */               \
SHR_G_ENTRY(P, aging_extended),    /* extended l2 age control */             \
SHR_G_ENTRY(P, dmux),              /* DMUX capability */                     \
SHR_G_ENTRY(P, ext_gth_hd_ipg),    /* Longer half dup. IPG (BCM5690A0/A1) */ \
SHR_G_ENTRY(P, l2x_ins_sets_hit),  /* L2X insert hardcodes HIT bit to 1 */   \
SHR_G_ENTRY(P, l3x_ins_igns_hit),  /* L3X insert does not affect HIT bit */  \
SHR_G_ENTRY(P, fabric_debug),      /* Debug support works on 5670 types */   \
SHR_G_ENTRY(P, filter_krules),     /* All FFP rules usable on 5673 */        \
SHR_G_ENTRY(P, srcmod_filter),     /* Chip can filter on src modid */        \
SHR_G_ENTRY(P, dcb_st0_bug),       /* Unreliable DCB status 0 (BCM5665 A0) */\
SHR_G_ENTRY(P, filter_128_rules),  /* 128 FFP rules usable on 5673 */        \
SHR_G_ENTRY(P, modmap),            /* Module ID remap */                     \
SHR_G_ENTRY(P, recheck_cntrs),     /* Counter collection bug */              \
SHR_G_ENTRY(P, bigmac_rxcnt_bug),  /* BigMAC Rx counter bug */               \
SHR_G_ENTRY(P, block_ctrl_ingress),/* Must block ctrl frames in xPIC */      \
SHR_G_ENTRY(P, mstp_mask),         /* VLAN mask for MSTP drops */            \
SHR_G_ENTRY(P, mstp_lookup),       /* VLAN lookup for MSTP drops */          \
SHR_G_ENTRY(P, mstp_uipmc),        /* MSTP for UIPMC */                      \
SHR_G_ENTRY(P, ipmc_lookup),       /* IPMC echo suppression */               \
SHR_G_ENTRY(P, lynx_l3_expanded),  /* Lynx 1.5 with expanded L3 tables */    \
SHR_G_ENTRY(P, fast_rate_limit),   /* Short pulse rate limits (Lynx !A0) */  \
SHR_G_ENTRY(P, l3_sgv),            /* (S,G,V) hash key in L3 table */        \
SHR_G_ENTRY(P, l3_sgv_aisb_hash),  /* (S,G,V) variant hash */                \
SHR_G_ENTRY(P, l2_hashed),                                                   \
SHR_G_ENTRY(P, l2_lookup_cmd),                                               \
SHR_G_ENTRY(P, l2_lookup_retry),   /* All-0's response requires retry */     \
SHR_G_ENTRY(P, l2_user_table),     /* Implements L2 User table */            \
SHR_G_ENTRY(P, rsv_mask),          /* RSV_MASK needs 10/100 mode adjust */   \
SHR_G_ENTRY(P, vlan_0_war),        /* Chip requires VLAN 0 entry (5645) */   \
SHR_G_ENTRY(P, schan_hw_timeout),  /* H/W can indicate SCHAN timeout    */   \
SHR_G_ENTRY(P, lpm_tcam),          /* LPM table is a TCAM               */   \
SHR_G_ENTRY(P, mdio_enhanced),     /* MDIO address remapping + C45/C22  */   \
SHR_G_ENTRY(P, mem_cmd),           /* Memory commands supported  */          \
SHR_G_ENTRY(P, two_ingress_pipes), /* Has two ingress pipelines           */ \
SHR_G_ENTRY(P, mpls),              /* MPLS for XGS3 */                       \
SHR_G_ENTRY(P, xgxs_lcpll),        /* LCPLL Clock for XGXS */                \
SHR_G_ENTRY(P, dodeca_serdes),     /* Dodeca Serdes */                       \
SHR_G_ENTRY(P, txdma_purge),       /* TX Purge packet after DMA abort */     \
SHR_G_ENTRY(P, rxdma_cleanup),     /* Check RX Packet SOP after DMA abort */ \
SHR_G_ENTRY(P, status_link_fail),  /* H/W linkcsan gives Link fail status */ \
SHR_G_ENTRY(P, fe_maxframe),       /* FE MAC MAXFR is max frame len + 1   */ \
SHR_G_ENTRY(P, l2x_parity),        /* Parity support on L2X table         */ \
SHR_G_ENTRY(P, l3x_parity),        /* Parity support on L3 tables         */ \
SHR_G_ENTRY(P, l3defip_parity),    /* Parity support on L3 DEFIP tables   */ \
SHR_G_ENTRY(P, l3_defip_map),      /* Map out unused L3_DEFIP blocks      */ \
SHR_G_ENTRY(P, l2_modfifo),        /* L2 Modfifo for L2 table sync        */ \
SHR_G_ENTRY(P, l2_multiple),       /* Multiple L2 tables implemented      */ \
SHR_G_ENTRY(P, vlan_mc_flood_ctrl),/* VLAN mutlicast flood control (PFM)  */ \
SHR_G_ENTRY(P, vlan_translation),  /* VLAN translation feature            */ \
SHR_G_ENTRY(P, egr_vlan_pfm),      /* MH PFM control per VLAN FB A0       */ \
SHR_G_ENTRY(P, parity_err_tocpu),  /* Packet to CPU on lookup parity error*/ \
SHR_G_ENTRY(P, nip_l3_err_tocpu),  /* Packet to CPU on non-IP L3 error    */ \
SHR_G_ENTRY(P, l3mtu_fail_tocpu),  /* Packet to CPU on L3 MTU check fail  */ \
SHR_G_ENTRY(P, meter_adjust),      /* Packet length adjustment for meter  */ \
SHR_G_ENTRY(P, prime_ctr_writes),  /* Prepare counter writes with read    */ \
SHR_G_ENTRY(P, xgxs_power),        /* Power up XGSS                       */ \
SHR_G_ENTRY(P, src_modid_blk),     /* SRC_MODID_BLOCK table support       */ \
SHR_G_ENTRY(P, src_modid_blk_ucast_override),/* MODID_BLOCK ucast overide */ \
SHR_G_ENTRY(P, src_modid_blk_opcode_override),/* MODID_BLOCK opcode overide */\
SHR_G_ENTRY(P, egress_blk_ucast_override),  /* egr mask opcode overide    */ \
SHR_G_ENTRY(P, stat_jumbo_adj),    /* Adjustable stats *OVR threshold     */ \
SHR_G_ENTRY(P, stat_xgs3),         /* XGS3 dbg counters and IPv6 supports */ \
SHR_G_ENTRY(P, dbgc_higig_lkup),   /* FB B0 dbg counters w/ HiGig lookup  */ \
SHR_G_ENTRY(P, port_trunk_index),  /* FB B0 trunk programmable hash       */ \
SHR_G_ENTRY(P, port_flow_hash),    /* HB/GW enhanced hashing              */ \
SHR_G_ENTRY(P, seer_bcam_tune),    /* Retune SEER BCAM table              */ \
SHR_G_ENTRY(P, mcu_fifo_suppress), /* MCU fifo error suppression          */ \
SHR_G_ENTRY(P, cpuport_switched),  /* CPU port can act as switch port     */ \
SHR_G_ENTRY(P, cpuport_mirror),    /* CPU port can be mirrored            */ \
SHR_G_ENTRY(P, higig2),            /* Higig2 support                      */ \
SHR_G_ENTRY(P, color),             /* Select color from priority/CFI      */ \
SHR_G_ENTRY(P, color_inner_cfi),   /* Select color from inner tag CFI     */ \
SHR_G_ENTRY(P, color_prio_map),    /* Map color/int_prio from/to prio/CFI */ \
SHR_G_ENTRY(P, untagged_vt_miss),  /* Drop untagged VLAN xlate miss       */ \
SHR_G_ENTRY(P, module_loopback),   /* Allow local module ingress          */ \
SHR_G_ENTRY(P, dscp_map_per_port), /* Per Port DSCP mapping table support */ \
SHR_G_ENTRY(P, dscp_map_mode_all), /* DSCP map mode ALL or NONE support   */ \
SHR_G_ENTRY(P, l3defip_bound_adj), /* L3 defip boundary adjustment        */ \
SHR_G_ENTRY(P, tunnel_dscp_trust), /* Trust incomming tunnel DSCP         */ \
SHR_G_ENTRY(P, higig_lookup),      /* FB B0 Proxy (HG lookup)             */ \
SHR_G_ENTRY(P, egr_l3_mtu),        /* L3 MTU check                        */ \
SHR_G_ENTRY(P, egr_mirror_path),   /* Alternate path for egress mirror    */ \
SHR_G_ENTRY(P, xgs1_mirror),       /* XGS1 mirroring compatibility support*/ \
SHR_G_ENTRY(P, register_hi),       /* _HI designation for > 32 ports      */ \
SHR_G_ENTRY(P, table_hi),          /* _HI designation for > 32 ports      */ \
SHR_G_ENTRY(P, trunk_extended),    /* extended designation for > 32 trunks*/ \
SHR_G_ENTRY(P, hg_trunking),       /* higig trunking support              */ \
SHR_G_ENTRY(P, hg_trunk_override), /* higig trunk override support        */ \
SHR_G_ENTRY(P, trunk_group_size),  /* trunk group size support            */ \
SHR_G_ENTRY(P, egr_vlan_check),    /* Support for Egress VLAN check PBM   */ \
SHR_G_ENTRY(P, mod1),              /* Ports spread over two module Ids    */ \
SHR_G_ENTRY(P, egr_ts_ctrl),       /* Egress Time Slot Control            */ \
SHR_G_ENTRY(P, ipmc_grp_parity),   /* Parity support on IPMC GROUP table  */ \
SHR_G_ENTRY(P, mpls_pop_search),   /* MPLS pop search support             */ \
SHR_G_ENTRY(P, cpu_proto_prio),    /* CPU protocol priority support       */ \
SHR_G_ENTRY(P, ignore_pkt_tag),    /* Ignore packet vlan tag support      */ \
SHR_G_ENTRY(P, port_lag_failover), /* Port LAG failover support           */ \
SHR_G_ENTRY(P, hg_trunk_failover), /* Higig trunk failover support        */ \
SHR_G_ENTRY(P, trunk_egress),      /* Trunk egress support                */ \
SHR_G_ENTRY(P, force_forward),     /* Egress port override support        */ \
SHR_G_ENTRY(P, port_egr_block_ctl),/* L2/L3 port egress block control     */ \
SHR_G_ENTRY(P, ipmc_repl_freeze),  /* IPMC repl config pauses traffic     */ \
SHR_G_ENTRY(P, bucket_support),    /* MAX/MIN BUCKET support              */ \
SHR_G_ENTRY(P, remote_learn_trust),/* DO_NOT_LEARN bit in HiGig Header    */ \
SHR_G_ENTRY(P, ipmc_group_mtu),    /* IPMC MTU Setting for all Groups     */ \
SHR_G_ENTRY(P, bsafe),             /* Broadsafe support                   */ \
SHR_G_ENTRY(P, tx_fast_path),      /* TX Descriptor mapped send           */ \
SHR_G_ENTRY(P, deskew_dll),        /* Deskew DLL present                  */ \
SHR_G_ENTRY(P, src_mac_group),     /* Src MAC Group (MAC Block Index)     */ \
SHR_G_ENTRY(P, storm_control),     /* Strom Control                       */ \
SHR_G_ENTRY(P, hw_stats_calc),     /* Calculation of stats in HW          */ \
SHR_G_ENTRY(P, ext_tcam_sharing),  /* External TCAM sharing support       */ \
SHR_G_ENTRY(P, mac_learn_limit),   /* MAC Learn limit support             */ \
SHR_G_ENTRY(P, mac_learn_limit_rollover),   /* MAC Learn limit counter roll over   */ \
SHR_G_ENTRY(P, linear_drr_weight), /* Linear DRR weight calculation       */ \
SHR_G_ENTRY(P, igmp_mld_support),  /* IGMP MLD snooping support           */ \
SHR_G_ENTRY(P, basic_dos_ctrl),    /* TCP flags, L4 port, TCP frag, ICMP  */ \
SHR_G_ENTRY(P, enhanced_dos_ctrl), /* Enhanced DOS control features       */ \
SHR_G_ENTRY(P, ctr_xaui_activity), /* XAUI counter activity support       */ \
SHR_G_ENTRY(P, proto_pkt_ctrl),    /* Protocol packet control             */ \
SHR_G_ENTRY(P, vlan_ctrl),         /* Per VLAN property control           */ \
SHR_G_ENTRY(P, tunnel_6to4_secure),/* Secure IPv6 to IPv4 tunnel support. */ \
SHR_G_ENTRY(P, tunnel_any_in_6),   /* Any in IPv6 tunnel support.    */ \
SHR_G_ENTRY(P, tunnel_gre),        /* GRE  tunneling support.             */ \
SHR_G_ENTRY(P, unknown_ipmc_tocpu),/* Send unknown IPMC packet to CPU.    */ \
SHR_G_ENTRY(P, src_trunk_port_bridge), /* Source trunk port bridge        */ \
SHR_G_ENTRY(P, stat_dma_error_ack),/* Stat DMA error acknowledge bit offset */ \
SHR_G_ENTRY(P, big_icmpv6_ping_check),/* ICMP V6 oversize packet disacrd  */ \
SHR_G_ENTRY(P, asf),               /* Alternate Store and Forward         */ \
SHR_G_ENTRY(P, asf_no_10_100),     /* No ASF support for 10/100Mb speeds  */ \
SHR_G_ENTRY(P, trunk_group_overlay),/* Direct TGID overlay of modid/port  */ \
SHR_G_ENTRY(P, xport_convertible), /* XPORT <-> Higig support             */ \
SHR_G_ENTRY(P, dual_hash),         /* Dual hash tables support            */ \
SHR_G_ENTRY(P, robo_sw_override),  /* Robo Port Software Override state*/    \
SHR_G_ENTRY(P, igmp_ip),           /* IGMP IP Layer snooping */              \
SHR_G_ENTRY(P, dscp),              /* DiffServ QoS */                        \
SHR_G_ENTRY(P, auth),  /* AUTH 802.1x support */    \
SHR_G_ENTRY(P, mstp),              \
SHR_G_ENTRY(P, arl_mode_control),   /* ARL CONTROL (DS/DD/CPU)*/              \
SHR_G_ENTRY(P, no_stat_mib),                 \
SHR_G_ENTRY(P, rcpu_1),            /* Remote CPU feature (Raptor)         */ \
SHR_G_ENTRY(P, unimac),            /* UniMAC 10/100/1000                  */ \
SHR_G_ENTRY(P, hw_dos_prev), /* hardware dos attack prevention(Robo)      */ \
SHR_G_ENTRY(P, generic_table_ops), /* Generic SCHAN table operations      */ \
SHR_G_ENTRY(P, lpm_prefix_length_max_128),/* Maximum lpm prefix len 128   */ \
SHR_G_ENTRY(P, tag_enforcement),   /* BRCM tag with tag_enforcement       */ \
SHR_G_ENTRY(P, vlan_translation_range),   /* VLAN translation range       */ \
SHR_G_ENTRY(P, mpls_per_vlan),     /* MPLS enable per-vlan                */ \
SHR_G_ENTRY(P, mpls_bos_lookup),   /* Second lookup bottom of stack == 0  */ \
SHR_G_ENTRY(P, class_based_learning),  /* L2 class based learning         */ \
SHR_G_ENTRY(P, static_pfm),        /* Static MH PFM control               */ \
SHR_G_ENTRY(P, ipmc_repl_penultimate), /* Flag last IPMC replication      */ \
SHR_G_ENTRY(P, sgmii_autoneg),     /* Support SGMII autonegotiation       */ \
SHR_G_ENTRY(P, ip_ep_mem_parity),  /* Parity support on IP, EP memories   */ \
SHR_G_ENTRY(P, post),              /* Post after reset init               */ \
SHR_G_ENTRY(P, rcpu_priority),     /* Remote CPU packet QoS handling      */ \
SHR_G_ENTRY(P, rcpu_tc_mapping),   /* Remote CPU traffic class to queue   */ \
SHR_G_ENTRY(P, mem_push_pop),      /* SCHAN push/pop operations           */ \
SHR_G_ENTRY(P, dcb_reason_hi),     /* Number of RX reasons exceeds 32     */ \
SHR_G_ENTRY(P, multi_sbus_cmds),   /* SCHAN multiple sbus commands        */ \
SHR_G_ENTRY(P, esm_support),       /* External TCAM support               */ \
SHR_G_ENTRY(P, fifo_dma),          /* fifo DMA support                    */ \
SHR_G_ENTRY(P, ipfix),             /* IPFIX support                       */ \
SHR_G_ENTRY(P, l3_entry_key_type), /* Data stored in first entry only.    */ \
SHR_G_ENTRY(P, robo_ge_serdes_mac_autosync),    /* serdes_autosync with mac*/ \
SHR_G_ENTRY(P, fp_routing_mirage), /* Mirage fp based routing.            */ \
SHR_G_ENTRY(P, subport),           /* Subport support                     */ \
SHR_G_ENTRY(P, reset_delay),       /* Delay after CMIC soft reset         */ \
SHR_G_ENTRY(P, fast_egr_cell_count), /* HW accelerated cell count retrieval */ \
SHR_G_ENTRY(P, 802_3as),           /* 802.3as                             */ \
SHR_G_ENTRY(P, l2_pending),        /* L2 table pending support            */ \
SHR_G_ENTRY(P, discard_ability),   /* discard/WRED support                */ \
SHR_G_ENTRY(P, distribution_ability),   /* distribution/ESET support      */ \
SHR_G_ENTRY(P, mc_group_ability),   /* McGroup support                    */ \
SHR_G_ENTRY(P, cosq_gport_stat_ability),   /* COS queue stats support     */ \
SHR_G_ENTRY(P, standalone),         /* standalone switch mode             */ \
SHR_G_ENTRY(P, internal_loopback),  /* internal loopback port             */ \
SHR_G_ENTRY(P, packet_adj_len),     /* packet adjust length               */ \
SHR_G_ENTRY(P, vlan_action),       /* VLAN action support                 */ \
SHR_G_ENTRY(P, packet_rate_limit), /* Packet-based rate limitting to CPU  */ \
SHR_G_ENTRY(P, system_mac_learn_limit), /* System MAC Learn limit support */ \
SHR_G_ENTRY(P, fp_based_routing),  /* L3 based on field processor.        */ \
SHR_G_ENTRY(P, field),             /* Field Processor (FP) for XGS3       */ \
SHR_G_ENTRY(P, field_slices2),     /* FP TCAM has 2 slices, instead of 16 */ \
SHR_G_ENTRY(P, field_slices4),     /* FP TCAM has 4 slices, instead of 16 */ \
SHR_G_ENTRY(P, field_slices8),     /* FP TCAM has 8 slices, instead of 16 */ \
SHR_G_ENTRY(P, field_slices12),    /* FP TCAM has 12 slices,instead of 16 */ \
SHR_G_ENTRY(P, field_meter_pools4),/* FP TCAM has 4 global meter pools.   */ \
SHR_G_ENTRY(P, field_mirror_ovr),  /* FP has MirrorOverride action        */ \
SHR_G_ENTRY(P, field_udf_higig),   /* FP UDF window contains HiGig header */ \
SHR_G_ENTRY(P, field_udf_higig2),  /* FP UDF window contains HiGig2 header */ \
SHR_G_ENTRY(P, field_udf_hg),      /* FP UDF2.0->2.2 HiGig header         */ \
SHR_G_ENTRY(P, field_udf_ethertype), /* FP UDF can ues extra ethertypes   */ \
SHR_G_ENTRY(P, field_comb_read),   /* FP can read TCAM & Policy as one    */ \
SHR_G_ENTRY(P, field_wide),        /* FP has wide-mode combining of slices*/ \
SHR_G_ENTRY(P, field_slice_enable),/* FP enable/disable on per slice basis*/ \
SHR_G_ENTRY(P, field_cos),         /* FP has CoS Queue actions            */ \
SHR_G_ENTRY(P, field_ingress_late),/* FP has late ingress pipeline stage  */ \
SHR_G_ENTRY(P, field_color_indep), /* FP can set color in/dependence      */ \
SHR_G_ENTRY(P, field_qual_drop),   /* FP can qualify on drop state        */ \
SHR_G_ENTRY(P, field_qual_IpType), /* FP can qualify on IpType alone      */ \
SHR_G_ENTRY(P, field_qual_Ip6High), /* FP can qualify on Src/Dst Ip 6 High */ \
SHR_G_ENTRY(P, field_mirror_pkts_ctl), /* Enable FP for mirror packtes    */ \
SHR_G_ENTRY(P, field_intraslice_basic_key), /* Only IP4 or IP6 key.       */ \
SHR_G_ENTRY(P, field_ingress_two_slice_types),/* Ingress FP has 2 slice types*/\
SHR_G_ENTRY(P, field_ingress_global_meter_pools), /* Global Meter Pools   */ \
SHR_G_ENTRY(P, field_ingress_ipbm), /* FP IPBM qualifier is always present*/ \
SHR_G_ENTRY(P, field_egress_flexible_v6_key), /* Egress FP KEY2 SIP6/DIP6.*/ \
SHR_G_ENTRY(P, field_egress_global_counters), /* Global Counters          */ \
SHR_G_ENTRY(P, field_ing_egr_separate_packet_byte_counters), /*           */ \
SHR_G_ENTRY(P, field_multi_stage), /* Multi staged field processor.       */ \
SHR_G_ENTRY(P, field_intraslice_double_wide), /* FP double wide in 1 slice*/ \
SHR_G_ENTRY(P, field_int_fsel_adj),/* FP internal field select adjust     */ \
SHR_G_ENTRY(P, field_pkt_res_adj), /* FP field packet resolution adjust   */ \
SHR_G_ENTRY(P, fp_virtual_slice_group), /* Virtual slice/groups in FP     */ \
SHR_G_ENTRY(P, virtual_switching), /* Virtual lan services support        */ \
SHR_G_ENTRY(P, xgport_one_xe_six_ge), /* XGPORT mode with 1x10G + 6x1GE   */ \
SHR_G_ENTRY(P, sample_offset8),  /* Sample threshold is shifted << 8 bits */ \
SHR_G_ENTRY(P, sample_thresh16), /* Sample threshold 16 bit wide          */ \
SHR_G_ENTRY(P, sample_thresh24), /* Sample threshold 24 bit wide          */ \
SHR_G_ENTRY(P, hybrid),          /* global hybrid configuration feature   */ \
SHR_G_ENTRY(P, node_hybrid),     /* node hybrid configuration feature     */ \
SHR_G_ENTRY(P, arbiter),         /* arbiter feature                       */ \
SHR_G_ENTRY(P, node),            /* node hybrid configuration mode        */ \
SHR_G_ENTRY(P, egr_independent_fc), /* egr fifo independent flow control  */ \
SHR_G_ENTRY(P, count)              /* ALWAYS LAST PLEASE (DO NOT CHANGE)  */

/* Make the enums */
#undef  SHR_G_MAKE_STR
#define SHR_G_MAKE_STR(a)     a
SHR_G_MAKE_ENUM(soc_feature_);

/* Make the string array */
#undef  SHR_G_MAKE_STR
#define SHR_G_MAKE_STR(a)     #a
#define SOC_FEATURE_NAME_INITIALIZER                                        \
        SHR_G_NAME_BEGIN(dont_care)                                         \
        SHR_G_ENTRIES(, dont_care)                                          \
        SHR_G_NAME_END(dont_care)

/* Feature-defining functions */
extern int soc_features_bcm5600_c0(int unit, soc_feature_t feature);
extern int soc_features_bcm5605_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5615_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5680_b0(int unit, soc_feature_t feature);
extern int soc_features_bcm5670_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5673_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5690_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5665_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5695_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5675_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5674_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5665_b0(int unit, soc_feature_t feature);
extern int soc_features_bcm56601_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56601_b0(int unit, soc_feature_t feature);
extern int soc_features_bcm56601_c0(int unit, soc_feature_t feature);
extern int soc_features_bcm56602_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56602_b0(int unit, soc_feature_t feature);
extern int soc_features_bcm56602_c0(int unit, soc_feature_t feature);
extern int soc_features_bcm56504_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56504_b0(int unit, soc_feature_t feature);
extern int soc_features_bcm56102_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56304_b0(int unit, soc_feature_t feature);
extern int soc_features_bcm56112_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56314_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5650_c0(int unit, soc_feature_t feature);
extern int soc_features_bcm56800_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56218_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56514_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56624_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56680_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5338_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5338_b0(int unit, soc_feature_t feature);
extern int soc_features_bcm5324_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5324_a1(int unit, soc_feature_t feature);
extern int soc_features_bcm5380_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5388_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5396_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5389_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5398_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5325_a1(int unit, soc_feature_t feature);
extern int soc_features_bcm5348_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5397_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5347_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm5395_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm53242_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56224_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56224_b0(int unit, soc_feature_t feature);
extern int soc_features_bcm53262_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56820_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm56725_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm53115_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm88020_a0(int unit, soc_feature_t feature);
extern int soc_features_bcm53118_a0(int unit, soc_feature_t feature);

extern void soc_feature_init(int unit);
extern char *soc_feature_name[];

#define soc_feature(unit, feature)      SOC_FEATURE_GET(unit, feature)

#endif  /* !_SOC_FEATURE_H */
