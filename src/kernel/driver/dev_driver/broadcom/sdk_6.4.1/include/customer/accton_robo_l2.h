/*
 * $Id: accton_l2.h,v 1.0 2005/02/04 21:48:05 $
 * $Copyright: (c) 2000-2003, 2004 Broadcom Corp.
 * All Rights Reserved.$
 */

#ifdef BCM_ROBO_SUPPORT
#ifndef _ACCTON_ROBO_L2_H_
#define _ACCTON_ROBO_L2_H_

#include <shared/portmode.h>
#include <shared/port.h>
#include <shared/phyconfig.h>

#include <bcm/types.h>

/***************************************************************************
 * 		Accton-specific APIs for L2 Table Management               * 
 ***************************************************************************/

extern int ACCTON_bcm_l2_read_range(int unit, bcm_l2_addr_t *l2_entry, int entry_start_index, int count);
 
#endif	/* !_ACCTON_ROBO_L2_H_ */
#endif

