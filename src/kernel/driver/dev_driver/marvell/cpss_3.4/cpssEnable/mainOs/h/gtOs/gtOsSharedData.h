/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* gtOsSharedData.h
*
* DESCRIPTION:
*       
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __gtOsSharedDatah
#define __gtOsSharedDatah

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

/* for uintptr_t */
#include <stdint.h>

typedef enum {
    CPSS_SHARED_DATA_ETHPORT_POOLARRAY_PTR_E = 0,
    CPSS_SHARED_DATA_MAX_E
} CPSS_SHARED_DATA_TYPE;

/*******************************************************************************
* cpssMultiProcGetSharedData
*
* DESCRIPTION:
*       return enumerated shared data
*
* INPUTS:
*  		id       - data identifier
*
* OUTPUTS:
*       None
*
* RETURNS:
*       data (integer or pointer)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
uintptr_t cpssMultiProcGetSharedData
(
    IN CPSS_SHARED_DATA_TYPE id
);

/*******************************************************************************
* cpssMultiProcSetSharedData
*
* DESCRIPTION:
*       Set enumerated shared data
*
* INPUTS:
*  		id       - data identifier
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcSetSharedData
(
    IN CPSS_SHARED_DATA_TYPE id,
    IN uintptr_t data
);

/*******************************************************************************
* cpssMultiProcSharedDataLock
*
* DESCRIPTION:
*       Lock shared data for critical operations (data alloc)
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcSharedDataLock(void);

/*******************************************************************************
* cpssMultiProcSharedDataUnlock
*
* DESCRIPTION:
*       Unlock shared data
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcSharedDataUnlock(void);


#ifdef __cplusplus
}
#endif

#endif  /* __gtOsSharedDatah */
/* Do Not Add Anything Below This Line */
