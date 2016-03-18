/*
 * $Id: accton_vlan.h,v 1.0 2005/02/04 21:48:05 $
 * $Copyright: (c) 2000-2003, 2004 Broadcom Corp.
 * All Rights Reserved.$
 */

#ifndef _ACCTON_VLAN_H_
#define _ACCTON_VLAN_H_

#include <shared/portmode.h>
#include <shared/port.h>
#include <shared/phyconfig.h>

#include <bcm/types.h>

/***************************************************************************
 * 		Accton-specific APIs for VLAN Table Management             * 
 ***************************************************************************/
extern int accton_bcm_vlan_port_set(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp);
 
#endif	/* !_ACCTON_VLAN_H_ */
