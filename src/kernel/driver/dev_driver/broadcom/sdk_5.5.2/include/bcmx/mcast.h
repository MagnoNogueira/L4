/*
 * $Id: mcast.h,v 1.7 Broadcom SDK $
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
 * File:	bcmx/mcast.h
 * Purpose:	BCMX L2 Multicast APIs
 */

#ifndef	_BCMX_MCAST_H
#define	_BCMX_MCAST_H

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include <bcm/mcast.h>

typedef struct bcmx_mcast_addr_s {
	bcm_mac_t	mac;
	bcm_vlan_t	vid;
	bcm_cos_t	cos_dst;
	bcmx_lplist_t	ports;
	bcmx_lplist_t	untag_ports;
	uint32		l2mc_index;
} bcmx_mcast_addr_t;

extern void bcmx_mcast_addr_t_init(bcmx_mcast_addr_t *mcaddr,
				   bcm_mac_t mac,
				   bcm_vlan_t vid);
#define bcmx_mcast_addr_init bcmx_mcast_addr_t_init	/* compat. */
extern void bcmx_mcast_addr_t_free(bcmx_mcast_addr_t *mcaddr);
#define bcmx_mcast_addr_free bcmx_mcast_addr_t_free	/* compat. */
extern int bcmx_mcast_init(void);
extern int bcmx_mcast_addr_add(bcmx_mcast_addr_t *mcaddr);
extern int bcmx_mcast_addr_remove(bcm_mac_t mac, bcm_vlan_t vid);
extern int bcmx_mcast_port_get(bcm_mac_t mac, bcm_vlan_t vid,
			       bcmx_mcast_addr_t *mcaddr);
extern int bcmx_mcast_join(bcm_mac_t mac, bcm_vlan_t vid, bcmx_lport_t port,
			   bcmx_mcast_addr_t *mcaddr, bcmx_lplist_t *allrtr);
extern int bcmx_mcast_leave(bcm_mac_t mac, bcm_vlan_t vid, bcmx_lport_t port);
extern int bcmx_mcast_port_remove(bcmx_mcast_addr_t *mcaddr);
extern int bcmx_mcast_port_add(bcmx_mcast_addr_t *mcaddr);

#endif	/* _BCMX_MCAST_H */
