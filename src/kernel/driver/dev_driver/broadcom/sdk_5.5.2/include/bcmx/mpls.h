/*
 * $Id: mpls.h,v 1.8.2.4 Broadcom SDK $
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
 * BCMX Multiprotocol Lable Switching APIs
 */

#ifndef _BCMX_MPLS_H_
#define _BCMX_MPLS_H_

#ifdef INCLUDE_L3

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include <bcm/l3.h>
#include <bcm/mpls.h>

/*
 * MPLS VPN/VFI struct to determine which
 * instance the incoming/outgoing packets belong to
 */
typedef struct bcmx_mpls_vpn_s {
    uint32          flags;
    bcm_vpn_t       vpn;
    bcm_if_t        l3_intf;
    bcmx_lport_t    lport;
    bcm_trunk_t     trunk;
    bcm_vlan_t      vlan;
} bcmx_mpls_vpn_t;

/*
 * bcmx_mpls_circuit_t
 *
 */
typedef struct bcmx_mpls_circuit_s {
    uint32            flags;     /* BCM_MPLS_ORIGINAL_PKT ? */
    bcm_vpn_t         vpn;       /* The VPN this VC belongs to */
    bcm_vlan_t        vlan;      /* New VID in MPLS header */
    bcm_mpls_label_t  label;     /* L2 MPLS VC label */
    bcm_if_t          l3_intf;   /* "L3 interface" (src MAC addr, labels) */
    bcm_mac_t         dst_mac;   /* MPLS packet's egress dest MAC */
    bcmx_lport_t      dst_port;  /* MPLS packet's logical egress port */
    bcm_trunk_t       dst_trunk; /* MPLS packet's egress trunk */
} bcmx_mpls_circuit_t;

/*
 * bcmx_mpls_switch_t
 * These entries are used on LSR (label switching)
 * and on tunnel terminator (LER)
 */
typedef struct bcmx_mpls_switch_s {
    uint32               flags;         /* BCM_MPLS_ONE_LABEL_LOOKUP etc */

    /* Source members */
    bcm_mpls_label_t     src_label1;    /* outer label */
    bcm_mpls_label_t     src_label2;    /* inner label (if appl) */
    bcmx_lport_t         src_lport;     /* Source Logical Port */
    bcm_trunk_t          src_trunk;     /* Source Trunk ID */
    bcm_mpls_action_t    action;        /* MPLS action type */
    bcm_if_t             src_l3_intf;   /* Ingress L3 interface for tunnel */
    int                  label_pri;     /* priority for associated label */

    /* MPLS label swapping */
    bcm_mpls_label_t     swap_label;    /* New label for label swapping */
    bcm_mpls_label_t     push_label1;   /* New label for label pushing  */
    bcm_mpls_label_t     push_label2;   /* New label for label pushing  */

    /* Forwarding to nexthop (actions 0, 1, 2, 3, 4, 8) */
    bcm_if_t             next_intf;     /* Egress L3 intf for the tunnel */
    bcm_mac_t            next_mac;      /* Next hop MAC */
    bcm_vlan_t           next_vlan;     /* VLAN ID */
    bcmx_lport_t         next_lport;    /* Next hop logical port */
    bcm_trunk_t          next_trunk;    /* trunk to get NH index */
    bcm_vpn_t            vpn;           /* VPN ID used for VC table lookup */


    /* L2 MPLS forwarding related (actions 5, 6) */
    bcmx_lport_t         dst_lport;     /* Destination logical port */
    bcm_trunk_t          dst_trunk;     /* trunk L2 payload is sent to */
    bcm_vlan_t           dst_vlan;      /* New VID */
} bcmx_mpls_switch_t;

/*
 * MPLS Information Structure
 *
 * Contains capabilities
 */
typedef bcm_mpls_info_t bcmx_mpls_info_t;

/*
 * MPLS VPN Config Structure
 */
typedef bcm_mpls_vpn_config_t bcmx_mpls_vpn_config_t;

/*
 * MPLS Port Structure
 */
typedef bcm_mpls_port_t bcmx_mpls_port_t;

/*
 * MPLS Egress Label Structure
 */
typedef bcm_mpls_egress_label_t bcmx_mpls_egress_label_t;

/*
 * MPLS Tunnel Switch structure
 */
typedef bcm_mpls_tunnel_switch_t bcmx_mpls_tunnel_switch_t;


/*
 * Initialize the MPLS software module, clear all HW MPLS states
 */
extern int bcmx_mpls_init(void);
extern int bcmx_mpls_cleanup(void);
extern int bcmx_mpls_info(void);

/*
 * Initialize MPLS data structure
 */
extern int bcmx_mpls_vpn_t_init(bcmx_mpls_vpn_t *vpn_info);
extern int bcmx_mpls_l3_initiator_t_init(bcm_mpls_l3_initiator_t *ftn);
extern int bcmx_mpls_switch_t_init(bcmx_mpls_switch_t *mswitch);
extern void bcmx_mpls_vpn_config_t_init(bcmx_mpls_vpn_config_t *info);
extern void bcmx_mpls_port_t_init(bcmx_mpls_port_t *mpls_port);
extern void bcmx_mpls_egress_label_t_init(bcmx_mpls_egress_label_t *label);
extern void bcmx_mpls_tunnel_switch_t_init(bcmx_mpls_tunnel_switch_t *info);


/*
 * L2 MPLS port blocking
 */
extern int bcmx_mpls_port_block_set(bcm_vpn_t vpn, bcmx_lport_t lport,
                                    int enable);
extern int bcmx_mpls_port_block_get(bcm_vpn_t vpn,
                                    bcmx_lport_t lport, int *enable);

/*
 * Virtual Switching Instance
 */
extern int bcmx_mpls_vpn_create(bcm_vpn_t vpn, uint32 flags);
extern int bcmx_mpls_vpn_destroy(bcm_vpn_t vpn);
extern int bcmx_mpls_vpn_add(bcm_vpn_t vpn, bcmx_mpls_vpn_t *vpn_info);
extern int bcmx_mpls_vpn_delete(bcm_vpn_t vpn, bcmx_mpls_vpn_t *vpn_info);
extern int bcmx_mpls_vpn_delete_all(bcm_vpn_t vpn);
extern int bcmx_mpls_vpn_get(bcm_vpn_t vpn, int maxout,
                             bcmx_mpls_vpn_t *vpn_info, int *count);


/*
 * L2 Virtual Circuit
 */
extern int bcmx_mpls_vpn_circuit_add(bcm_vpn_t vpn,
                                     bcmx_mpls_circuit_t *vc_info);
extern int bcmx_mpls_vpn_circuit_delete(bcm_vpn_t vpn,
                                        bcmx_mpls_circuit_t *vc_info);
extern int bcmx_mpls_vpn_circuit_delete_all(bcm_vpn_t vpn);
extern int bcmx_mpls_vpn_circuit_get(bcm_vpn_t vpn, int max,
                                     bcmx_mpls_circuit_t *vc_info, int *count);

/*
 * L3 MPLS Initiation
 */
extern int bcmx_mpls_l3_initiator_set(bcm_l3_intf_t *intf,
                                      bcm_mpls_l3_initiator_t *ftn);
extern int bcmx_mpls_l3_initiator_clear(bcm_l3_intf_t *intf);
extern int bcmx_mpls_l3_initiator_get(bcm_l3_intf_t *intf,
                                      bcm_mpls_l3_initiator_t *ftn,
                                      int *count);
extern int bcmx_mpls_l3_initiator_clear_all(void);

/*
 * MPLS Switching
 */
extern int bcmx_mpls_switch_add(bcmx_mpls_switch_t *mapping);
extern int bcmx_mpls_switch_delete(bcmx_mpls_switch_t *mapping);
extern int bcmx_mpls_switch_get(int max, bcmx_mpls_switch_t *mapping,
                                int *count);
extern int bcmx_mpls_switch_delete_all(int flag);


extern int bcmx_mpls_exp_map_create(uint32 flags, int *exp_map_id);
extern int bcmx_mpls_exp_map_destroy(int exp_map_id);
extern int bcmx_mpls_exp_map_set(int exp_map_id,
                                 bcm_mpls_exp_map_t *exp_map);
extern int bcmx_mpls_exp_map_get(int exp_map_id,
                                 bcm_mpls_exp_map_t *exp_map);


/*
 * L2 VPN
 */
extern int bcmx_mpls_vpn_id_create(bcmx_mpls_vpn_config_t *info);

extern int bcmx_mpls_vpn_id_destroy(bcm_vpn_t vpn);

extern int bcmx_mpls_vpn_id_destroy_all(void);

extern int bcmx_mpls_vpn_id_get(bcm_vpn_t vpn, bcmx_mpls_vpn_config_t *info);

extern int bcmx_mpls_port_add(bcm_vpn_t vpn, 
                              bcmx_mpls_port_t *mpls_port);

extern int bcmx_mpls_port_delete(bcm_vpn_t vpn, 
                                 bcm_gport_t mpls_port_id);

extern int bcmx_mpls_port_delete_all(bcm_vpn_t vpn);

extern int bcmx_mpls_port_get(bcm_vpn_t vpn, 
                              bcmx_mpls_port_t *mpls_port);

extern int bcmx_mpls_port_get_all(bcm_vpn_t vpn, 
                                  int port_max, 
                                  bcmx_mpls_port_t *port_array, 
                                  int *port_count);
/*
 * L3 MPLS
 */
extern int bcmx_mpls_tunnel_initiator_set(bcm_if_t intf,
                                          int num_labels,
                                          bcmx_mpls_egress_label_t *label_array);
extern int bcmx_mpls_tunnel_initiator_get(bcm_if_t intf,
                                          int label_max,
                                          bcmx_mpls_egress_label_t *label_array,
                                          int *label_count);

extern int bcmx_mpls_tunnel_initiator_clear(bcm_if_t intf);

extern int bcmx_mpls_tunnel_initiator_clear_all(void);

extern int bcmx_mpls_tunnel_switch_add(bcmx_mpls_tunnel_switch_t *info);

extern int bcmx_mpls_tunnel_switch_delete(bcmx_mpls_tunnel_switch_t *info);

extern int bcmx_mpls_tunnel_switch_delete_all(void);

extern int bcmx_mpls_tunnel_switch_get(bcmx_mpls_tunnel_switch_t *info);

extern int bcmx_mpls_label_stat_get(bcm_mpls_label_t label, bcm_gport_t port,
                                    bcm_mpls_stat_t stat, uint64 *val);

extern int bcmx_mpls_label_stat_get32(bcm_mpls_label_t label, bcm_gport_t port,
                                      bcm_mpls_stat_t stat, uint32 *val);

#endif  /* INCLUDE_L3 */

#endif  /* ! _BCMX_MPLS_H_ */
