/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtAppDemoEmulationMsgQueue.h
*
* DESCRIPTION:
*       Message Queue implementation.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/


#ifndef __gtAppDemoEmulationMsgQueueh
#define __gtAppDemoEmulationMsgQueueh

#ifdef __cplusplus
extern "C" {
#endif


/************* Includes *******************************************************/

#include <appDemo/os/appOs.h>


/************* Typedefs *******************************************************/

typedef void * GT_QUE_ID;


/*******************************************************************************
* msgQueueHitlessUpgrdActivate
*
* DESCRIPTION:
*       Activates the msg queue module
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - success
*       GT_FAIL - otherwise
*
* COMMENTS:
*       Needed for hitless upgrade first phase only.
*
*******************************************************************************/
GT_STATUS msgQueueHitlessUpgrdActivate(void);


/*******************************************************************************
* msgQueueHitlessUpgrdActivateInstance
*
* DESCRIPTION:
*       Activates a msg queue instance
*
* INPUTS:
*       queueControlBlockPtr - pointer to the queue control block.
*       queueName - the queue name.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       To be used only in hitless upgrd first phase. Future phases require
*       the msg queue to be initialized again
*       Internal use only
*
*******************************************************************************/
GT_STATUS msgQueueHitlessUpgrdActivateInstance
(
    void    *queueControlBlockPtr,
    GT_8    *queueName
);




/*******************************************************************************
* msgQueueCreate
*
* DESCRIPTION:
*       Creates a msg_queue message queue.
*
* INPUT:
*       queueName - the queue name.
*       max_msgs  - maximum depth of queue
*       msglen    - length of each message in queue (in bytes)
*
* OUTPUT:
*       none
*
* RETURN:
*       pointer to the queue control block if successful,
*       NULL pointer otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void * msgQueueCreate
(
    GT_8    * queueName,
    GT_32     max_msgs,
    GT_U32    msglen
);


/*******************************************************************************
* msgQueueSend
*
* DESCRIPTION:
*       Posts a message to the tail of a msg_queue queue.
*
* INPUT:
*       queueControlBlock - pointer to the queue control block.
*       msg               - pointer to message to be sent
*       msglen            - length of message
*
* OUTPUT:
*       none
*
* RETURN:
*       GT_OK if successful, or
*       GT_ERROR otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS msgQueueSend
(
    void          * queue,
    GT_8          * msg,
    GT_U32          msglen
);


/*******************************************************************************
* msgQueueReceive
*
* DESCRIPTION:
*       Fetches a message from the specified msg_queue queue.
*
* INPUT:
*       queueControlBlock - pointer to the queue control block.
*       msgbuf            - pointer to user message buffer
*       buflen            - length of message
*
* OUTPUT:
*       none
*
* RETURN:
*       the actual length of read message
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_32 msgQueueReceive
(
    void          * queue,
    GT_8          * msgbuf,
    GT_U32          buflen
);

/*******************************************************************************
* msgQueueNumMsgs
*
* DESCRIPTION:
*       Returns the number of messages currently posted to the specified queue
*
* INPUT:
*       queueControlBlock - pointer to the queue control block.
*
* OUTPUT:
*       none
*
* RETURN:
*       the number of messages in queue
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_32 msgQueueNumMsgs(void * queue);

#ifdef __cplusplus
}
#endif

#endif /* __gtAppDemoEmulationMsgQueueh */
