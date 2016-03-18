/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtAppDemoEmulationEvReqDispatcher.h
*
* DESCRIPTION:
*       Dispatcher to decide to call the CPS or to the "AppDemo emulate the
*       CPSS events"
*
*       Includes unified event routine. The routines allow a user application
*       to bind, select and receive events from PP, XBAR and FA devices.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __gtAppDemoEmulationEvReqDispatcherh
#define __gtAppDemoEmulationEvReqDispatcherh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <appDemo/os/appOs.h>
#include <cpss/generic/events/cpssGenEventRequests.h>


/*******************************************************************************
* appDemoEventBind_Dispatcher
*
* DESCRIPTION:
*       This routine binds a use process to unified event. The routine returns
*       a handle that is used when the application wants to wait for the event
*       (gtEventSelect), receive the event (gtEventRecv) or transmit a packet
*       using the Network Interface.
*
* INPUTS:
*       uniEventArr - The unified event list.
*       arrLength   - The unified event list length.
*
* OUTPUTS:
*       hndlPtr     - The user handle for the binded events.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoEventBind_Dispatcher
(
    IN  CPSS_UNI_EV_CAUSE_ENT      uniEventArr[],
    IN  GT_U32               arrLength,
    OUT GT_U32               *hndlPtr
);


/*******************************************************************************
* appDemoEventSelect_Dispatcher
*
* DESCRIPTION:
*       This function gets the first interrupt node information from the
*       selected unified event queue.
*
* INPUTS:
*       hndlPtr             - The user handle for the binded events.
*       timeoutPtr          - Wait timeout in milli(NULL=forever).
*       evBitmapArrLength   - The bitmap array length (in words).
*
* OUTPUTS:
*       evBitmapArr         - The bitmap array.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoEventSelect_Dispatcher
(
    IN  GT_U32              hndl,
    IN  GT_U32              *timeoutPtr,
    OUT GT_U32              evBitmapArr[],
    IN  GT_U32              evBitmapArrLength
);

/*******************************************************************************
* appDemoEventRecv_Dispatcher
*
* DESCRIPTION:
*       This function gets the first interrupt node information from the
*       selected unified event queue.
*
* INPUTS:
*       hndlPtr     - The user handle for the binded events.
*       evCause     - The received unified event.
*
* OUTPUTS:
*       evExtDataPtr- The additional date (port num) the event was received upon.
*       evDevPtr    - The device the event was received upon
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoEventRecv_Dispatcher
(
    IN  GT_U32              hndl,
    IN  CPSS_UNI_EV_CAUSE_ENT     evCause,
    OUT GT_U32              *evExtDataPtr,
    OUT GT_U8               *evDevPtr
);

/*******************************************************************************
* appDemoEventDeviceMaskSet_Dispatcher
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*
* COMMENTS:
*
*       The OS interrupts and the ExtDrv interrupts are locked during the
*       operation of the function.
*
*******************************************************************************/
GT_STATUS appDemoEventDeviceMaskSet_Dispatcher
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN CPSS_EVENT_MASK_SET_ENT  operation
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtAppDemoEmulationEvReqDispatcherh */


