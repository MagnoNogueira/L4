/*
 * $Id: accton_bcm.h,v 1.0 2005/02/04 21:48:05 $
 * $Copyright: (c) 2000-2003, 2004 Broadcom Corp.
 * All Rights Reserved.$
 */

#ifndef _ACCTON_BCM_H_
#define _ACCTON_BCM_H_

#include <shared/portmode.h>
#include <shared/port.h>
#include <shared/phyconfig.h>

#include <bcm/types.h>

#include <bcm/port.h>

#include <stddef.h>


/***************************************************************************
 * 	Accton-specific APIs for SDK validation                                * 
 ***************************************************************************/
#define ACCTON_BCM_SDK_VALIDATION_LIST(_) \
            _(sizeof(soc_control_t)), \
            _(sizeof(dcb_op_t)), \
            _(&((soc_control_t*)0)->chip_driver),

#define VALIDATION_NAME(v) #v
#define VALIDATION_VALUE(v) (unsigned int)(v)

#define accton_bcm_sdk_validate() \
    do { \
        unsigned int external_validation_list[] = { ACCTON_BCM_SDK_VALIDATION_LIST(VALIDATION_VALUE) }; \
        _accton_bcm_sdk_validate(external_validation_list); \
    } while (0)

extern int _accton_bcm_sdk_validate(unsigned int *external_validation_list);

#endif	/* !_ACCTON_BCM_H_ */

