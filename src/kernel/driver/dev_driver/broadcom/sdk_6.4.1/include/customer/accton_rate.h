/*
 * File:	accton_rate.h
 * Purpose:     Accton-specific APIs 
 *
 */

#ifndef _ACCTON_RATE_H_
#define _ACCTON_RATE_H_

/* Configure egress shaping counter */
extern int accton_bcm_egress_packet_ifg_set(int unit, uint32 value);
#define bcm_egress_packet_ifg_set accton_bcm_egress_packet_ifg_set

#endif /* _ACCTON_RATE_H_ */

