/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvInterrupts.h
*
* DESCRIPTION:
*       Includes general definitions for the interrupts handling unit.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#ifndef __prvCpssDrvInterruptsh
#define __prvCpssDrvInterruptsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>

/*
 * Typedef: struct PRV_CPSS_DRV_INT_ISR_COOKIE_STC
 *
 * Description: a cookie info for the ISR to use , to distinguish
 *              between devices and port groupss .
 *
 * Fields:
 *      devNum      - the device number that associated with the cookie.
 *      portGroupId - the port group Id that associated with the cookie.
 *                    for 'non-multi port groups' device use index
 *                    CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS.
 *
*/
typedef struct{
    GT_U8       devNum;
    GT_U32      portGroupId;
}PRV_CPSS_DRV_INT_ISR_COOKIE_STC;



/*
 * Typedef: struct PRV_CPSS_DRV_INT_CTRL_STC
 *
 * Description: Includes interrupt control and handling data.
 *
 * Fields:
 *      intNodesPool   - An array containing all interrupt nodes to be en-queued
 *                        / de-queued in the interrupt queues.
 *      intCauseType    - Interrupt cause registers type.
 *      isrFuncPtr      - A pointer to an ISR to be called when this device
 *                        should be scanned for interrupts.
 *      intScanRoot     - The interrupt information hierarchy tree root.
 *                        dynamic allocated according to numOfIntBits/32
 *      intMaskShadow   - Holds the interrupt mask registers shadow.
 *                        dynamic allocated according to numOfIntBits/32
 *      accessB4StartInit - Whether the corresponding interrupt mask register
 *                          may be accessed before start Init or not.
 *      numOfIntBits    - Number of interrupt bits defined for this device.
 *      gppIsrFunc      - Array of pointers to the different Gpps interrupt
 *                        service routines.
 *      intVecNum       - The interrupt vector this device is connected throw.
 *      isrConnectionId - Id for access to the isr connection table.
 *      intSumMaskRegAddr - register address for the summary mask
 *                          relevant only for PCI/PEX !!!
 *
 *      needInterruptMaskDefaultValueInit - Do we need to initialize the
 *                          interrupt mask default value for polling mode.
 *                          Only the summary bits need to be unmasked, cause
 *                          bits are unmasked by user App on event bind.
 *
 *      bindTheIsrCbRoutineToFdbTrigEnded - bind the ISR callback routine done
 *                          before signalling app  - to the FDB Trigger Action
 *                          Ended
 *      isrCookieInfo - a cookie 'per port group' ,
 *                      so the ISR is the same for all devices/port groups
 */
typedef struct
{
    struct{
        PRV_CPSS_DRV_EV_REQ_NODE_STC    *intNodesPool;
        PRV_CPSS_DRV_INTERRUPT_SCAN_STC *intScanRoot;
        GT_U32                          *intMaskShadow;
        GT_U32                          isrConnectionId;
        GT_U32                          intVecNum;
        PRV_CPSS_DRV_INT_ISR_COOKIE_STC isrCookieInfo;
    } portGroupInfo[CPSS_MAX_PORT_GROUPS_CNS];

    CPSS_EVENT_ISR_FUNC             isrFuncPtr;

    GT_BOOL             *accessB4StartInit;
    GT_U32              numOfIntBits;

    PRV_CPSS_DRV_GPP_INT_INFO_STC     gppIsrFuncs[CPSS_EVENT_GPP_MAX_NUM_E];

    GT_U32              intSumMaskRegAddr;

    GT_BOOL             needInterruptMaskDefaultValueInit;
    GT_BOOL             bindTheIsrCbRoutineToFdbTrigEnded;
}PRV_CPSS_DRV_INT_CTRL_STC;





/*******************************************************************************
* prvCpssDrvInterruptDeviceRemove
*
* DESCRIPTION:
*       This function removes a list of devices after Hot removal operation.
*
* INPUTS:
*       devNum   - the device number to remove from interrupts
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptDeviceRemove
(
    IN  GT_U8   devNum
);


/*******************************************************************************
* prvCpssDrvInterruptPpSR
*
* DESCRIPTION:
*       This is the Packet Processor interrupt service routine, it scans the
*       interrupts and enqueues each of them to the interrupt queues structure.
*
* INPUTS:
*       cookie  - (devNum) The PP device number to scan.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       1 - if interrupts where received from the given device, or
*       0 - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_U8 prvCpssDrvInterruptPpSR
(
    IN void *cookie
);


/*******************************************************************************
* prvCpssDrvInterruptMaskSet
*
* DESCRIPTION:
*       This function masks/unmasks a given interrupt bit in the relevant
*       interrupt mask register.
*
* INPUTS:
*       evNode  - The interrupt node representing the interrupt to be
*                 unmasked.
*       operation - The operation to perform, mask or unmask the interrupt
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptMaskSet
(
    IN PRV_CPSS_DRV_EV_REQ_NODE_STC  *evNode,
    IN CPSS_EVENT_MASK_SET_ENT operation
);



/*******************************************************************************
* prvCpssDrvEventsMask
*
* DESCRIPTION:
*       enables / disables a given event from reaching the CPU.
*
* INPUTS:
*       devNum      - The PP's device number to mask / unmask the interrupt for.
*       intIndex    - The interrupt cause to enable/disable.
*       enable      - GT_TRUE enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL - on failure.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvEventsMask
(
    IN  GT_U8   devNum,
    IN  GT_U32  intIndex,
    IN  GT_BOOL enable
);

/*******************************************************************************
* prvCpssDrvEventsMaskGet
*
* DESCRIPTION:
*       Gets enable/disable status of a given event reaching the CPU.
*
* INPUTS:
*       devNum      - The PP's device number to mask / unmask the interrupt for.
*       intIndex    - The interrupt cause to get enable/disable status.
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE enable, GT_FALSE disable.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDrvEventsMaskGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  intIndex,
    OUT  GT_BOOL *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvInterruptsh */


