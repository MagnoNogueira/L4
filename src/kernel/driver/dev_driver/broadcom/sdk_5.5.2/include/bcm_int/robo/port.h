/*
 * $Id: port.h,v 1.8 Broadcom SDK $
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

/*
 * Port configuration structure.
 * An abstraction of data in the PTABLE (robo) and other places.
 */

typedef struct bcm_port_cfg_s 
{
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
    int		pc_mirror_eng;	  /* mirror on engress */
    int		pc_ptype;	  /* port type */
    int		pc_jumbo;
    int		pc_cml;		  /* CML bits */

    bcm_pbmp_t	pc_pbm;		  /* port bitmaps for port based vlan */
    bcm_pbmp_t	pc_ut_pbm;
    bcm_vlan_t	pc_vlan;	  /* port based vlan tag */

    int		pc_new_pri;	  /* new priority */
    int		pc_remap_pri_en;  /* remap priority enable */
    
    int		pc_dse_mode;	  /* DSE mode ? */
    int		pc_dscp;	  /* diffserv code point */

} bcm_port_cfg_t;

extern int _bcm_robo_port_software_init(int unit);
extern int _bcm_robo_port_link_get(int unit, 
                bcm_port_t port, int hw, int *link);
extern int _bcm_robo_port_untagged_data_update(int unit, bcm_pbmp_t pbmp);
extern int _bcm_robo_port_untagged_vlan_get(int unit, bcm_port_t port,
				       bcm_vlan_t *vid_ptr);

#define TOS_DIFFSERV_EN_ADDR	soc_reg_addr(unit, QOS_TOSDIF_ENr, 0, 0)
#define TOS_DIFFSERV_CTRL_ADDR	soc_reg_addr(unit, QOS_TOSDIF_CTRLr, 0, 0)
#define DSCP_PRIORITY1_ADDR	soc_reg_addr(unit, QOS_DIFF_DSCP1r, 0, 0)
#define DSCP_PRIORITY2_ADDR	soc_reg_addr(unit, QOS_DIFF_DSCP2r, 0, 0)

#define TTOS_PRIORITY_ADDR	soc_reg_addr(unit, QOS_TTOS_THRSHr, 0, 0)
#define DTOS_PRIORITY_ADDR	soc_reg_addr(unit, QOS_DTOS_THRSHr, 0, 0)
#define MTOS_PRIORITY_ADDR	soc_reg_addr(unit, QOS_MTOS_THRSHr, 0, 0)
#define RTOS_PRIORITY_ADDR	soc_reg_addr(unit, QOS_RTOS_THRSHr, 0, 0)


/* PHY power mode */
#define PHY_HIGH_PERFORMANCE_MODE 1
#define PHY_LOW_POWER_AB0_MODE 2
#define PHY_LOW_POWER_AB6_MODE 3
#define PHY_GREEN_POWER_MODE 4
#define PHY_AUTO_POWER_DOWN_MODE 5
#define PHY_STANDBY_POWER_DOWN_MODE 6
#define PHY_NON_STANDBY_POWER_DOWN_MODE 7

int bcm_robo_port_phy_power_mode_set(int unit, bcm_port_t port, int mode);
int bcm_robo_port_phy_power_mode_get(int unit, bcm_port_t port, int *mode);
int bcm_robo_port_phy_power_mode_select(int unit, bcm_port_t port);



#endif	/* !_BCM_INT_PORT_H */
