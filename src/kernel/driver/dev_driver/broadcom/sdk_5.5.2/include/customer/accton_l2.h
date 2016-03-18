/*
 * $Id: accton_l2.h,v 1.0 2005/02/04 21:48:05 $
 * $Copyright: (c) 2000-2003, 2004 Broadcom Corp.
 * All Rights Reserved.$
 */

#ifndef _ACCTON_L2_H_
#define _ACCTON_L2_H_

#include <shared/portmode.h>
#include <shared/port.h>
#include <shared/phyconfig.h>

#include <bcm/types.h>

/***************************************************************************
 * 		Accton-specific APIs for L2 Table Management               * 
 ***************************************************************************/
#define ACCTON_BCM_L2_ADDR_UPDATE_L3_BIT_SUPPORTED(unit) \
        ( SOC_IS_DRACO(unit) || SOC_IS_DRACO15(unit) )

extern int accton_bcm_l2_addr_update_l3_bit(int unit, bcm_l2_addr_t *l2addr, uint32 l3_bit);
extern int accton_bcm_l2_read_clear_hit_bits(int unit, mac_addr_t mac, bcm_vlan_t vid, int *hit);
 
#endif	/* !_ACCTON_L2_H_ */
