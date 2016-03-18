/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvErrataMng.h
*
* DESCRIPTION:
*       Cpss Driver manager for the Errata issues.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __prvCpssDrvErrataMngh
#define __prvCpssDrvErrataMngh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/* number GT_U32 bmps needed to hold the known FEr errata bmp */
#define PRV_CPSS_DRV_ERRATA_BMP_MAX_NUM_CNS \
                (1 + (((PRV_CPSS_DRV_ERRATA_MAX_NUM_E)+1) >> 5))

/* get if the erratum need to be WA by CPSS
    devNum - the device id of the cpss device
    FErNum -- id of the erratum
*/
#define PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,FErNum) \
  ((prvCpssDrvPpConfig[devNum]->errata.FErArray[(FErNum)>>5] &(1<<((FErNum)&0x1f)))? \
      GT_TRUE : GT_FALSE)

/* set that the erratum need to be WA by CPSS
    devNum - the device id of the cpss device
    FErNum -- 1 based number
*/
#define PRV_CPSS_DRV_ERRATA_SET_MAC(devNum,FErNum) \
    prvCpssDrvPpConfig[devNum]->errata.FErArray[(FErNum)>>5] |= (1<<((FErNum)&0x1f))

/*
 * Typedef: enum PRV_CPSS_DRV_ERRATA_ENT
 *
 * Description: enumerator for the numbers of Erratum that handles in the CPSS
 *              driver code
 *
 * Fields:
 *      PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E - WA for Errata of PPs that
 *                            require to SYNC their write actions before doing a
 *                            read action
 *                            (Twist-D FEr#1).
 *
 *      PRV_CPSS_DRV_ERRATA_PCI_SLAVE_UNABLE_TO_ABORT_TRANSACTION_E - WA needed
 *                            for Device PCI slave that is unable to abort
 *                            read/write transaction .
 *                            (Twist-D FEr#68 , Tiger FEr#17 , ).
 *
 *
 */
typedef enum{
    PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E,
    PRV_CPSS_DRV_ERRATA_PCI_SLAVE_UNABLE_TO_ABORT_TRANSACTION_E,

    /* the last one */
    PRV_CPSS_DRV_ERRATA_MAX_NUM_E
}PRV_CPSS_DRV_ERRATA_ENT;


/*
 * Typedef: struct PRV_CPSS_DRV_ERRATA_STC
 *
 * Description: A structure to hold all Errata info that CPSS need to fix
 *              (workaround) for the PP .
 *
 * Fields:
 *      FErArray - array of bmp of FEr errata
 *                 every erratum is represented by 1 bit (32 errata in a bmp)
 */
typedef struct{
    GT_U32          FErArray[PRV_CPSS_DRV_ERRATA_BMP_MAX_NUM_CNS];
}PRV_CPSS_DRV_ERRATA_STC;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvErrataMngh */


