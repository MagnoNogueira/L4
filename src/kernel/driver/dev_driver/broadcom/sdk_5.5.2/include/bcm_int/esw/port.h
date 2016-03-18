/*
 * $Id: port.h,v 1.34 Broadcom SDK $
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
 * This file contains PORT definitions internal to the BCM library.
 */

#ifndef _BCM_INT_PORT_H
#define _BCM_INT_PORT_H

#include <bcm/port.h>

#define  BCM_PORT_L3_V4_ENABLE     (1 << 0)    /* Enable L3 IPv4 switching. */
#define  BCM_PORT_L3_V6_ENABLE     (1 << 1)    /* Enable L3 IPv6 switching. */

#define  BCM_PORT_JUMBO_MAXSZ    0x3FE8

/*
 * Internal port configuration enum.
 * Used internally by sdk to set port configuration from 
 */
typedef enum _bcm_port_config_e {
   _bcmPortL3UrpfMode,          /* L3 unicast packet RPF check mode.  */
   _bcmPortL3UrpfDefaultRoute,  /* UPRF check accept default gateway. */
   _bcmPortVlanTranslate,       /* Enable Vlan translation on the port. */
   _bcmPortVlanPrecedence,      /* Set Vlan lookup precedence between
                                 * IP table and MAC table             */
   _bcmPortVTMissDrop,          /* Drop if VLAN translate miss.       */
   _bcmPortLookupMACEnable,     /* Enable MAC-based VLAN              */
   _bcmPortLookupIPEnable,      /* Enable subnet-based VLAN           */
   _bcmPortUseInnerPri,         /* Trust the inner vlan tag PRI bits  */
   _bcmPortUseOuterPri,         /* Trust the outer vlan tag PRI bits  */
   _bcmPortVerifyOuterTpid,     /* Verify outer TPID against VLAN TPID*/
   _bcmPortVTKeyTypeFirst,      /* First Vlan Translation Key Type    */
   _bcmPortVTKeyPortFirst,      /* First Per-port VT Key Type         */
   _bcmPortVTKeyTypeSecond,     /* Second Vlan Translation Key Type   */
   _bcmPortVTKeyPortSecond,     /* Second Per-port VT Key Type        */
   _bcmPortIpmcEnable,          /* Enable IPMC processing             */
   _bcmPortIpmcV4Enable,        /* Enable IPv4 IPMC processing        */
   _bcmPortIpmcV6Enable,        /* Enable IPv4 IPMC processing        */
   _bcmPortIpmcVlanKey,         /* Include VLAN in L3 host key for IPMC */
   _bcmPortCfiAsCng,            /* Configure CFI to CNG mapping       */
   _bcmPortNni,                 /* Set port as NNI                    */
   _bcmPortHigigTrunkId,        /* Higig trunk, -1 to disable         */
   _bcmPortModuleLoopback,      /* Allow loopback to same modid       */
   _bcmPortOuterTpidEnables,    /* Bitmap of outer TPID valid bits    */
   _bcmPortCount                /* Always last max value of the enum. */
} _bcm_port_config_t;

/*
 * Port configuration structure.
 * An abstraction of data in the PTABLE (strata) and other places.
 */

typedef struct bcm_port_cfg_s {
    int		pc_frame_type;
    int		pc_ether_type;

    int		pc_stp_state;	  /* spanning tree state of port */
    int		pc_cpu;		  /* CPU learning */
    int		pc_disc;	  /* discard state */
    int		pc_bpdu_disable;  /* Where is this in Draco? */
    int		pc_trunk;	  /* trunking on for this port */
    int		pc_tgid;	  /* trunk group id */
    int		pc_rtag;	  /* trunk port selection criteria */
    int		pc_mirror_ing;	  /* mirror on ingress */
    int		pc_ptype;	  /* port type */
    int		pc_jumbo;
    int		pc_cml;		  /* CML bits */

    bcm_pbmp_t	pc_pbm;		  /* port bitmaps for port based vlan */
    bcm_pbmp_t	pc_ut_pbm;
    bcm_vlan_t	pc_vlan;	  /* port based vlan tag */
    bcm_vlan_t	pc_ivlan;	  /* port based inner-tag vlan tag */
    int         pc_vlan_action;   /* port based vlan action profile pointer */

#if defined(BCM_STRATA2_SUPPORT)
    int		pc_stg;		  /* stg value */
    int		pc_rsvd1;	  /* Reserved field (must be ones) */
#endif

    int		pc_l3_flags;	  /* L3 flags. */

#if defined(BCM_STRATA2_SUPPORT) || defined(BCM_XGS_SWITCH_SUPPORT)
    int		pc_new_pri;	  /* new priority */
    int		pc_remap_pri_en;  /* remap priority enable */
#endif

    int		pc_dse_mode;	  /* DSCP mapping (off, or on/mode) */
    int		pc_dscp;	  /* Resultant diffserv code point */

#if defined(BCM_XGS_SWITCH_SUPPORT)
    int         pc_en_ifilter;    /* Enable Ingress Filtering */
    int         pc_pfm;           /* In the port table for Draco */
    int         pc_dscp_prio;     /* For Draco15 & Tucana */
    int         pc_bridge_port;   /* FB/ER, allows egress=ingress */
    int         pc_nni_port;      /* FB, indicates non-customer port */
#endif

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    int     pc_urpf_mode;         /* Unicast rpf lookup mode.      */
    int     pc_urpf_def_gw_check; /* Unicast rpf check default gw. */ 
    int     pc_pvlan_enable;      /* Private (force) vlan enable */
#endif

} bcm_port_cfg_t;

extern int _bcm_port_software_init(int unit);
extern int _bcm_port_mirror_enable_set(int unit, bcm_port_t port, int enable);
extern int _bcm_port_mirror_enable_get(int unit, bcm_port_t port, int *enable);
extern int _bcm_port_link_get(int unit, bcm_port_t port, int hw, int *link);
extern int _bcm_port_pd10_handler(int unit, bcm_port_t port, int link_up);
extern int _bcm_port_untagged_data_update(int unit, bcm_pbmp_t pbmp);
extern int _bcm_port_untagged_vlan_get(int unit, bcm_port_t port,
				bcm_vlan_t *vid_ptr);
extern int bcm_esw_port_fault_get(int unit, bcm_port_t port, uint32 *flags);
extern int bcm_esw_port_vlan_member_get(int unit, bcm_port_t port, uint32 *flags);
extern int bcm_esw_port_vlan_member_set(int unit, bcm_port_t port, uint32 flags);
extern int bcm_esw_port_trunk_index_set(int unit, bcm_port_t port, int port_index);
extern int bcm_esw_port_trunk_index_get(int unit, bcm_port_t port, int *port_index);
extern int bcm_esw_port_l3_modify_set(int unit, bcm_port_t port, uint32 flags);
extern int bcm_esw_port_l3_modify_get(int unit, bcm_port_t port, uint32 *flags);
extern int bcm_port_l3_modify_set(int unit, bcm_port_t port, uint32 flags);
extern int bcm_port_l3_modify_get(int unit, bcm_port_t port, uint32 *flags);
extern int _bcm_esw_port_config_set(int unit, bcm_port_t port, 
                                    _bcm_port_config_t type, int value);
extern int _bcm_esw_port_config_get(int unit, bcm_port_t port, 
                                    _bcm_port_config_t type, int *value);
extern int _bcm_esw_gport_resolve(int unit, bcm_gport_t gport,
                                  bcm_module_t *modid, bcm_port_t *port, 
                                  bcm_trunk_t *trunk_id, int *id);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_port_cleanup(int unit);
#else
#define _bcm_port_cleanup(u)        (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_port_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif	/* !_BCM_INT_PORT_H */
