/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtAppDemoEmulationEvReqQueues.h
*
* DESCRIPTION:
*       This file includes functions for managing and configuring the event
*       queues structure for Event Driven mode.
*
*       The following sketch shows the data structure involved in the setting
*       of two events for one user event handle. The first event has one event
*       node waiting in the FIFO.
*       The userEvHandle structure is allocated by the coreEventBind routine.
*       Each event bound to the user handler is linked to the
*       userEvHandle.evListPtr field. Every bounded event also has a pointer
*       (userHndlPtr) to the user handler (user in the ISR scan routine). Each
*       event entry has a FIFO for the waiting event nodes (evNodeList), the
*       evNodeList points to the FIFO head.
*
*   uniEvQArr[uniEvent]                                     intNodeList[dev]
*   |============|<-----------|                    |------>|===============|
*   | evNodeList |------------+--------------------|       | devNum        |
*   | uniEvCause |            |                            | intStatus     |
*   | userHndlPtr|---------|  |     userEvHandle           | intRecStatus  |
*   | nextPtr    |-----|   |--+--->|============|          | intType       |
*   |============|     |   |  |    | evType     |          | intCause      |
*   | evNodeList |--*  |   |  |    | semId      |          | uniEvCause    |
*   | uniEvCause |     |   |  |    | extDataPtr |--*       | uniEvExt      |
*   | userHndlPtr|     |   |  |----| evListPtr  |          | intMaskSetFptr|
*   | nextPtr    |--*  |   |       |============|          | intCbFuncPtr  |
*   |============|<----|   |                               | intMaskReg    |
*   | evNodeList |---------+-----------------------|       | intBitMask    |
*   | uniEvCause |         |                       |       | prevPtr       |--*
*   | userHndlPtr|---------|                       |       | nextPtr       |--*
*   | nextPtr    |                                 |       |===============|
*   |============|                                 |       |    ...        |
*   | evNodeList |--*                              |       | prevPtr       |--*
*   | uniEvCause |                                 |       | nextPtr       |--*
*   | userHndlPtr|--*                              |------>|===============|
*   | nextPtr    |--*                                      |    ...        |
*   |============|                                         | prevPtr       |--*
*   |    ...     |                                         | nextPtr       |--*
*   |============|                                         |===============|
*                                                          |    ...        |
*                                                          | prevPtr       |--*
*                                                          | nextPtr       |--*
*                                                          |===============|
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/
#ifndef __gtAppDemoEmulationEvReqQueuesh
#define __gtAppDemoEmulationEvReqQueuesh

#include <appDemo/os/appOs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* Forward struct declaration.  */
struct _evReqNode;
struct _evReqQctrl;


/*
 * Typedef: enum EV_DRVN_INT_MASK_SET
 *
 * Description: Defines the different interrupt node states, that an interrupt
 *              node may have.
 *
 * Fields:
 *      EV_DRVN_INT_MASK_E      -
 *      EV_DRVN_INT_UNMASK_E    -
 *
 */
typedef enum
{
    EV_DRVN_INT_MASK_E          = 0,
    EV_DRVN_INT_UNMASK_E        = 1
}EV_DRVN_INT_MASK_SET;

/*
 * Typedef: FUNCP_REQ_MODE_UNMASK
 *
 * Description: Interrupt unmasking function pointer.
 *
 * Fields:
 *
 */
typedef GT_STATUS (*FUNCP_REQ_DRVN_MASK_SET)
(
    IN struct _evReqNode    *evNode,
    IN EV_DRVN_INT_MASK_SET operation
);

/*
 * typedef: FUNCP_INT_REC_CB
 *
 * Description: Callback function called when an interrupt is received.
 *
 * Fields:
 *      devNum      - Device number.
 *      intIndex    - The interrupt index.
 *
 * Comment:
 */
typedef void* (*FUNCP_INT_REC_CB)
(
    GT_U8  devNum,
    GT_U32 intIndex
);



/*
 * Typedef: enum EV_DRVN_STATUS
 *
 * Description: Defines the different interrupt node states, that an interrupt
 *              node may have.
 *
 * Fields:
 *      EV_DRVN_INT_ACTIVE_E    - The interrupt is active and can be handled,
 *                                was not set to pending state.
 *      EV_DRVN_INT_STOPPED_E   - Used to stop interrupt handling on Hot removal.
 *
 */
typedef enum
{
    EV_DRVN_INT_ACTIVE_E        = 0,
    EV_DRVN_INT_STOPPED_E       = 1
}EV_DRVN_INT_STATUS;


/*
 * Typedef: enum EV_DRVN_RECEPTION_STATUS
 *
 * Description: This enum defines the different types of interrupt receptions.
 *
 * Fields:
 *      EV_DRVN_INT_RCVD_E      - The interrupt was received during regular
 *                                operation of the system (and was not handled yet).
 *      EV_DRVN_INT_MASKED_E    - The interrupt was received while it was masked.
 *                                (and was not handled yet).
 *      EV_DRVN_INT_IDLE_READY_E- The event is ready to be romoved from queue.
 *      EV_DRVN_INT_IDLE_E      - The interrupt was not received lately. (was
 *                                already handled or was not received).
 */
typedef enum
{
    EV_DRVN_INT_RCVD_E          = 0,
    EV_DRVN_INT_MASKED_E        = 1,
    EV_DRVN_INT_IDLE_READY_E    = 2,
    EV_DRVN_INT_IDLE_E          = 3
}EV_DRVN_INT_RECEPTION_STATUS;


/*
 * Typedef: struct EV_REQ_NODE
 *
 * Description: Includes information of received unhandled interrupts, to be
 *              managed through the event queues structure.
 *
 * Fields:
 *      devNum      - The device number.
 *      intStatus   - The interrupt status.
 *      intRecStatus- Indicates the time by which the interrupt was received.
 *      intType     - The interrupt type
 *      intCause    - The interrupt cause index
 *      uniEvCause  - The Unified event cause index
 *      uniEvExt    - The Unified event addition information (port number...)
 *      intMaskSetFptr
 *                  - The interrupt mask setting routine
 *      intCbFuncPtr - A pointer to a callback function ,to be called after an
 *                    interrupt was recived before signalling the waiting app.
 *      intMaskReg  - Register address of the interrupt_mask register
 *                    representing this interrupt.
 *      intBitMask  - Bit mask in intMaskReg register, which represents the bit
 *                    relevant to this interrupt.
 *      prevPtr     - A pointer to the previous EV_REQ_NODE in the linked-list.
 *      nextPtr     - A pointer to the next EV_REQ_NODE in the linked-list.
 */
typedef struct _evReqNode
{
    GT_U8                           devNum;
    GT_U8                           intStatus;
    GT_U8                           intRecStatus;
    GT_U8                           intType;
    GT_U32                          intCause;
    GT_U32                          uniEvCause;
    GT_U32                          uniEvExt;
    FUNCP_REQ_DRVN_MASK_SET         intMaskSetFptr;
    FUNCP_INT_REC_CB                intCbFuncPtr;
    GT_U32                          intMaskReg;
    GT_U32                          intBitMask;
    struct _evReqNode               *prevPtr;
    struct _evReqNode               *nextPtr;
}EV_REQ_NODE, *EV_REQ_NODE_PTR;


/*
 * Typedef: enum EVENT_TYPE_ENT
 *
 * Description: Enumeration that defines the user handle type.
 *
 * Enumerations:
 *      TX_END_EVENT_E  - Tx End event handler.
 *      REGULAR_EVENT_E - Regular event handler.
 */
typedef enum
{
    TX_END_EVENT_E      = 0,
    REGULAR_EVENT_E     = 1
} EVENT_TYPE_ENT;


/*
 * Typedef: struct GT_EVENT_HNDL
 *
 * Description: Includes information for user binded events.
 *
 * Fields:
 *      evType      - The event type.
 *      semId       - The semaphore Id.
 *      extDataPtr  - The additional data for the event handle.
 *      evListPtr   - The user app binded event list.
 */
typedef struct
{
    EVENT_TYPE_ENT      evType;
    GT_U32              semId;
    GT_VOID             *extDataPtr;
    struct _evReqQctrl  *evListPtr;
} GT_EVENT_HNDL;


/*
 * Typedef: struct EV_REQ_Q_CTRL
 *
 * Description: The control block of an interrupt queue.
 *
 * Fields:
 *      evNodeList  - A pointer to the first node in the queue.
 *      uniEvCause  - The unified event cause index.
 *      userHndlPtr - The user handle binded for the event.
 *      nextPtr     - The next binded event control.
 */
typedef struct _evReqQctrl
{
    EV_REQ_NODE         *evNodeList;
    GT_U32              uniEvCause;
    GT_EVENT_HNDL       *userHndlPtr;
    struct _evReqQctrl  *nextPtr;
} EV_REQ_Q_CTRL;



/*******************************************************************************
* evReqQInit
*
* DESCRIPTION:
*       This function initializes the interrupts queues structure, according to
*       the user defined parameters, or according to the default parameters.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS evReqQInit
(
    GT_U32          numOfEvents
);

/*******************************************************************************
* evReqQUserHndlGet
*
* DESCRIPTION:
*       This function returns the user handle pointer for the event.
*
* INPUTS:
*       uniIntIndex - The event number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       !=NULL on success, or
*       NULL if failed.
*
* COMMENTS:
*
*******************************************************************************/
GT_EVENT_HNDL  *evReqQUserHndlGet
(
    IN GT_U32           uniIntIndex
);

/*******************************************************************************
* evReqQUserHndlSet
*
* DESCRIPTION:
*       This function sets the user handle pointer for the event.
*
* INPUTS:
*       uniIntIndex     - The event number.
*       evQNewHndlPtr   - The new user handler
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS  evReqQUserHndlSet
(
    IN GT_U32           uniIntIndex,
    IN GT_EVENT_HNDL    *evQNewHndlPtr
);

/*******************************************************************************
* evReqQUserHndlRemove
*
* DESCRIPTION:
*       This function removes the user handle pointer for the event.
*
* INPUTS:
*       uniEvCause      - The event number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS  evReqQUserHndlRemove
(
    IN GT_U32               uniEvCause
);

/*******************************************************************************
* evReqQInsert
*
* DESCRIPTION:
*       This function inserts a new event node to the tail of the event queue.
*       The active event counter is incremented.
*
* INPUTS:
*       evNodePool  - Pointer to the interrupt nodes pool the interrupt belongs
*                     to.
*       intIndex    - The interrupt index.
*       masked      - Indicates if the interrupt was received while it was
*                     masked.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       This routine is invoked from ISR context !! If Application process needs
*       to invoke the routine, be sure to protect the call with task and
*       interrupt lock.
*
*******************************************************************************/
GT_STATUS evReqQInsert
(
    IN EV_REQ_NODE      evNodePool[],
    IN GT_U32           evIndex,
    IN GT_BOOL          masked
);

/*******************************************************************************
* evQueueGet
*
* DESCRIPTION:
*       This function gets the first interrupt node information from the
*       selected interrupt queue. (The selection is made according to the
*       interrupts handling policy).
*
* INPUTS:
*       evQueueId  - The interrupts queue Id.
*
* OUTPUTS:
*       evWeight - The interrupt weight, according to it the interrupt handling
*                   should be repeated.
*       evQueuePriority  - The interrupt queue priority, from which the event
*                           was received in.
*
* RETURNS:
*       A pointer to the selected interrupt node, or NULL if no interrupts left.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
EV_REQ_NODE *evReqQGet
(
    IN GT_U32           evIndex
);

/*******************************************************************************
* evReqQBitmapGet
*
* DESCRIPTION:
*       This function returns a bitmap reflecting the unified events waiting
*       in queue for the user application.
*
* INPUTS:
*       hndl            - The user handle.
*       evBitmapArr     - The bitmap array.
*       evBitmapLength  - The size of bitmap array in words.
*
* OUTPUTS:
*       evBitmapArr     - The updated bitmap array.
*
* RETURNS:
*       The number of new events in bitmap.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_U32 evReqQBitmapGet
(
    IN GT_U32           hndl,
    INOUT GT_U32        evBitmapArr[],
    IN GT_U32           evBitmapLength
);


/*******************************************************************************
* evReqQRemoveDev
*
* DESCRIPTION:
*       This function is called upon Hot removal of a device, in order to remove
*       the interrupt nodes belonging to this device from the interrupts queues.
*
* INPUTS:
*       devNum  - The device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS evReqQRemoveDev
(
    IN  GT_U8           devNum
);

/*******************************************************************************
* evQueueReInit
*
* DESCRIPTION:
*       This function re-initializes the specified interrupts queues structure,
*       according to the user defined parameters, or according to the default
*       parameters.
*
* INPUTS:
*       None
* OUTPUTS:
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS evReqQReInit
(
    IN  GT_U32          numOfEvents
);

/*******************************************************************************
* evReqQRcvedEvRemoveAll
*
* DESCRIPTION:
*       This routine removes all user bound events that are active and record
*       status is Idle Ready. All removed events are unmasked.
*
* INPUTS:
*       hndl    - The user handle.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID evReqQRcvedEvRemoveAll
(
    IN GT_U32           hndl
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtAppDemoEmulationEvReqQueuesh */


