/*
 * $Id: accton_robo_l2.h,v 1.0 2005/02/04 21:48:05 $
 * $Copyright: (c) 2000-2003, 2004 Broadcom Corp.
 * All Rights Reserved.$
 */
#ifdef BCM_ROBO_SUPPORT
#ifndef _ACCTON_ROBO_UTIL_H_
#define _ACCTON_ROBO_UTIL_H_

#include <shared/portmode.h>
#include <shared/port.h>
#include <shared/phyconfig.h>

#include <bcm/types.h>

/***************************************************************************
 *      Accton-specific APIs for Trap Management                           *
 ***************************************************************************/
extern int ACCTON_bcm_enable_arp(int unit, uint32 en_arp);


/***************************************************************************
 *      Accton-specific APIs for Rate Management                           *
 ***************************************************************************/
extern int ACCTON_bcm_irc_xlenen(int unit, uint32 xlenen);

/***************************************************************************
 *      Accton-specific APIs for Ingress Filter                            *
 ***************************************************************************/
extern int ACCTON_bcm_ingress_vid_chk(int unit, uint32 in_vid_chk);

/***************************************************************************
 *      Accton-specific APIs for Port Control                              *
 ***************************************************************************/
extern void ACCTON_bcm_port_enable_all( int unit, int enable );

/***************************************************************************
 *      Accton-specific APIs for Mcast Group bitmap set                    *
 ***************************************************************************/
extern int ACCTON_bcm_robo_mcast_bitmap_set( int unit, bcm_mac_t mac,
			               bcm_vlan_t vid, soc_pbmp_t pbm );

#endif	/* !_ACCTON_ROBO_L2_H_ */
#endif
