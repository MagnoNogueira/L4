/*
 * $Id: accton_l3.h,v 1.0 2005/02/04 21:48:05 $
 * $Copyright: (c) 2000-2003, 2004 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File: 	accton_l3.h
 * Purpose: 	Accton-specific APIs for L3 Table Management
 */

#ifndef _ACCTON_L3_H_
#define _ACCTON_L3_H_

#ifdef INCLUDE_L3
#include <shared/portmode.h>
#include <shared/port.h>
#include <shared/phyconfig.h>

#include <bcm/types.h>

typedef void (*accton_l3_age_out)(int unit, bcm_ip_t ip);

/*
extern int accton_bcm_l3_host_set(int unit, bcm_l3_host_t *info);
extern int accton_bcm_l3_route_set(int unit, bcm_l3_route_t * info);

extern int accton_bcm_l3_ip_read_clear_hit_bit(int unit, bcm_l3_host_t *info, int *hit);
extern int accton_bcm_l3_ipmc_read_clear_hit_bit(int unit, ip_addr_t ip_src_addr, ip_addr_t ip_grp_addr, bcm_vlan_t vid_index, int *hit);
*/

int accton_bcm_fb_l3_l2cpu_nh_update(int);

/* Enable/disable L3 function without clearing any L3 tables. */
extern int accton_bcm_l3_ip4_enable_set(
    int unit, 
    int enable);

/* Enable/disable L3 function without clearing any L3 tables. */
extern int accton_bcm_l3_ip6_enable_set(
    int unit, 
    int enable);

/* for backward-compatiblity
 */
#define bcm_l3_ip4_enable_set accton_bcm_l3_ip4_enable_set
#define bcm_l3_ip6_enable_set accton_bcm_l3_ip6_enable_set

#endif
#endif	/* !_ACCTON_L3_H_ */

