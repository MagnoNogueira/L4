/*
 * File:	accton_field.h
 * Purpose:     Accton-specific APIs 
 *
 */

#ifndef _ACCTON_FIELD_H_
#define _ACCTON_FIELD_H_

#include <bcm/field.h>

#ifdef BCM_FIELD_SUPPORT

//typedef int bcm_field_group_sbmp_t;

extern int accton_bcm_field_group_create_mode_slice_id(
	int	unit,
	bcm_field_qset_t	qset,
	int	pri,
	bcm_field_group_mode_t	mode,
	bcm_field_group_t	group,
	bcm_field_group_sbmp_t sset);
#define bcm_field_group_create_mode_slice_id accton_bcm_field_group_create_mode_slice_id

extern int accton_bcm_field_entry_copy_id(int unit,
                              	          bcm_field_group_t group,
                                          bcm_field_entry_t entry);

extern int accton_bcm_field_meter_control_set(int unit, uint32 value);
#define bcm_meter_control_set accton_bcm_field_meter_control_set


#endif  /* BCM_FIELD_SUPPORT */
#endif /* _ACCTON_FIELD_H_ */

