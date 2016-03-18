/*
 * $Id: tunnel.h,v 1.6 Broadcom SDK $
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
 * File:    include/bcmx/tunnel.h
 * Purpose: BCMX tunnel APIs
 */

#ifndef _BCMX_TUNNEL_H
#define _BCMX_TUNNEL_H

#ifdef INCLUDE_L3

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include <bcm/tunnel.h>

/*
 * Tunnel Structure
 *
 *   Contains information about tunneling terminator information
 */

typedef struct bcmx_tunnel_terminator_s {
    uint32               flags;        /* Config flags */
    bcm_vrf_t            vrf;          /* Virtual router instance */
    bcm_ip_t             sip;          /* SIP for tunnel header match */
    bcm_ip_t             dip;          /* DIP for tunnel header match */
    bcm_ip_t             sip_mask;     /* SIP mask */
    bcm_ip_t             dip_mask;     /* DIP mask */
    bcm_ip6_t            sip6;         /* SIP for tunnel header match (IPv6).*/
    bcm_ip6_t            dip6;         /* DIP for tunnel header match (IPv6).*/
    bcm_ip6_t            sip6_mask;    /* Source IP mask              (IPv6).*/
    bcm_ip6_t            dip6_mask;    /* Destination IP mask         (IPv6).*/
    uint32               udp_dst_port; /* UDP dst port for UDP packets */
    uint32               udp_src_port; /* UDP src port for UDP packets */
    bcm_tunnel_type_t    type;         /* Tunnel type */
    bcm_vlan_t           vlan;         /* The VLAN ID for IPMC lookup */
    bcmx_lplist_t        ports;        /* port list */
} bcmx_tunnel_terminator_t;

typedef bcm_tunnel_initiator_t bcmx_tunnel_initiator_t;
typedef bcm_tunnel_config_t bcmx_tunnel_config_t;

/*
 * Tunneling APIs
 */

extern int bcmx_tunnel_terminator_add(bcmx_tunnel_terminator_t *info);
extern int bcmx_tunnel_terminator_delete(bcmx_tunnel_terminator_t *info);
extern int bcmx_tunnel_terminator_update(bcmx_tunnel_terminator_t *info);
extern int bcmx_tunnel_terminator_get(bcmx_tunnel_terminator_t *info);
extern int bcmx_tunnel_initiator_set(bcmx_l3_intf_t *intf,
                                     bcmx_tunnel_initiator_t *tunnel);
extern int bcmx_tunnel_initiator_clear(bcmx_l3_intf_t *intf);
extern int bcmx_tunnel_initiator_get(bcmx_l3_intf_t *intf,
                                     bcmx_tunnel_initiator_t *tunnel);
extern int bcmx_tunnel_dscp_map_create(uint32 flags, int *dscp_map_id);
extern int bcmx_tunnel_dscp_map_destroy(int dscp_map_id);
extern int bcmx_tunnel_dscp_map_set(int dscp_map_id,
                                    bcm_tunnel_dscp_map_t *dscp_map);
extern int bcmx_tunnel_dscp_map_get(int dscp_map_id,
                                    bcm_tunnel_dscp_map_t *dscp_map);
extern int bcmx_tunnel_dscp_map_port_set(bcmx_lport_t port,
                                         bcm_tunnel_dscp_map_t *dscp_map);
extern int bcmx_tunnel_dscp_map_port_get(bcmx_lport_t port,
                                         bcm_tunnel_dscp_map_t *dscp_map);
extern int bcmx_tunnel_config_set(bcmx_tunnel_config_t *tconfig);
extern int bcmx_tunnel_config_get(bcmx_tunnel_config_t *tconfig);
extern void bcmx_tunnel_terminator_t_init(bcmx_tunnel_terminator_t *info);
extern void bcmx_tunnel_terminator_t_free(bcmx_tunnel_terminator_t *info);
extern void bcmx_tunnel_initiator_t_init(bcmx_tunnel_initiator_t *tunnel);
extern void bcmx_tunnel_config_t_init(bcmx_tunnel_config_t *tconfig);

#endif  /* INCLUDE_L3 */

#endif  /* !_BCMX_TUNNEL_H */
