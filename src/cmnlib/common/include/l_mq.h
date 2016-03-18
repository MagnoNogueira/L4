/* -------------------------------------------------------------------------------------
 * FILE NAME : L_MQ.h (Multiple Priority Queue)
 * -------------------------------------------------------------------------------------
 * PURPOSE   : This package provides the functions to use priority
 *             queue for MSG (high priority msg processed first).
 *
 * NOTE      : 1. Designed for Mercury
 *             2. Enq/Deq data size is 4 unsigned long
 *             3. See DEQ NOTE for output order of deq msg
 * MODIFICATION HISTORY :
 * Modifier     Date        Version     Description
 * -------------------------------------------------------------------------------------
 * ryan         3-09-2005   V1.0        First Created
 * peter_yu     3-30-2005   V1.0        Add File Header
 * -------------------------------------------------------------------------------------
 * Copyright(C)                         Accton Technology Corp. 2005
 * -------------------------------------------------------------------------------------
 */

#ifndef L_MQ_H
#define L_MQ_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARACTIONS
 */
#define L_MQ_MESSAGE_SIZE    16

/* DATA TYPE DECLARACTIONS
 */
typedef UI32_T L_MQ_Desc_T;

/*---------------------------------
 * Type for shared memory version
 *---------------------------------
 */
typedef struct
{
    UI32_T          total_queue_length;
    UI32_T          msg_nbr_in_q;
    I32_T           priority_queue_map_offset;
    I32_T           schedule_weight_offset;
    I32_T           wcnter_offset;          /* working counter of weighted value */
    I32_T           queue_offset;
    UI16_T          nbr_of_priority;
    UI16_T          nbr_of_queue;
    UI16_T          last_qno;
    /* the above offset is relative address of the working
     * buffer with desc_p as base addr.
     */
} L_MQ_ShMem_Desc_T;


/* EXPORTED FUNCTION DECLARACTIONS
 */
/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_Create
 * ---------------------------------------------------------------------
 * PURPOSE  : Create a priority queue.
 * INPUT    : nbr_of_priority   --
 *            nbr_of_queue      --
 *            total_queue_length -- total number of message that shared by all queues.
 *            priority_queue_map[nbr_of_priority] -- index is the priority value(from 0) the result is
 *                                 queue number (from 0). value range is 0~(nbr_of_queue-1).
 *            schedule_weight[nbr_of_queue] -- the weighted value of each weight fair queue.
 *                              value 0 has special meaning, it mean it's a strict priority
 *                              relative to the lower indexed queue. But has a limitation,
 *                              if one queue has schedule_weight value 0, then all higer
 *                              priority queue(higher index) must also have 0 value.
 *                              e.g. the weighted value of queue 0 to queue 3 is
 *                              1,2,3,4 respectively. That mean the proportion is 1:2:3:4.
 *                              {4,3,2,1} and {4,4,0,0} are permitted, but (4,4,0,1} is an illegal setting.
 *
 *            queue_length[nbr_of_queue] -- the max queue length of each queue.
 *
 * OUTPUT   : none
 * RETURN   : Descriptor of priority queue or NULL if fail creation
 * NOTE     : 1. The infomation of input argument will copy in internal descriptor, caller free or
 *               re-use this variables are permited.
 *            2. desc->internal_use_buffer
 *            3. fail on nbr_of_priority=0, nbr_of_queue =0, total_queue_length=0,
 *               priority_queue_map[]=NULL, schedule_weight[]=NULL, queue_length[]=NULL,
 *               priority_quque_map[] value not between 0~(nbr_of_queue-1), illegal schedule_weight[].
 */
L_MQ_Desc_T L_MQ_Create (
    UI16_T nbr_of_priority,
    UI16_T nbr_of_queue,
    UI32_T total_queue_length,
    const UI16_T priority_queue_map[],
    const UI16_T schedule_weight [],
    const UI16_T queue_length [] );

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_Enq
 * ---------------------------------------------------------------------
 * PURPOSE  : Put to a strict priority queue.
 * INPUT    : desc          -- descriptor of priority queue
 *            msg           -- the msg buffer; should be 4 unsigned long
 *            priority      -- 0 to (nbr_of_queue-1)
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : 1. the large number, the higher priority.
 *            2. return TRUE when succeed,
 *               FALSE when fail on desc=NULL, msg[]=NULL, priority >(nbr_of_queue-1),
 *               msg in mq  >= total_queue_length.
 */
BOOL_T L_MQ_Enq (L_MQ_Desc_T desc, UI32_T  msg[], UI32_T priority);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_Deq
 * ---------------------------------------------------------------------
 * PURPOSE  : Get from a priority queue.
 * INPUT    : desc          -- descriptor of priority queue
 *            msg           -- the address of msg buffer; should be 4 unsigned long
 * OUTPUT   : msg           -- the msg buffer; should be 4 unsigned long
 * RETURN   : TRUE/FALSE
 * NOTE     : 1. Deq Rules:
 *               (1) high indexed queue will be deqed first.
 *               (2) if no msg is deqed for a round (Idle state), it will reset counter to schedule_weight.
 *            2. Example:
 *               for a schedule_weight=[2,4,0,0]
 *               the internal counter for strict priority (0) is INF
 *               an example queue state [2/101,102] means counter=2, msg=101,102
 *               an example operation is
 *               ACTION     STATE                                               DEQ_OUTPUT
 *               --------------------------------------------------------------------------
 *               INITIAL    [2/101,102], [4/103,104], [INF/105], [INF/106,111]
 *               DEQ        [2/101,102], [4/103,104], [INF/105], [INF/111]      106
 *               DEQ        [2/101,102], [4/103,104], [INF/105], [INF/null]     111
 *               DEQ        [2/101,102], [4/103,104], [INF/null], [INF/null]    105
 *               DEQ        [2/101,102], [3/104], [INF/null], [INF/null]        103
 *               ENQ(107,3) [2/101,102], [3/104], [INF/null], [INF/107]
 *               DEQ        [1/102], [3/104], [INF/null], [INF/107]             101
 *               ENQ(108,3) [1/102], [3/104], [INF/null], [INF/107,108]
 *               DEQ        [1/102], [3/104], [INF/null], [INF/108]             107
 *               DEQ        [1/102], [3/104], [INF/null], [INF/null]            108
 *               DEQ        [1/102], [2/null], [INF/null], [INF/null]           104
 *               ENQ(109,2) [1/102], [2/null], [INF/109], [INF/null]
 *               ENQ(110,0) [1/102,110], [2/null], [INF/109], [INF/null]
 *               DEQ        [0/110], [2/null], [INF/109], [INF/null]            102
 *               DEQ        [0/110], [2/null], [INF/null], [INF/null]           109
 *               reset cntr [2/110], [4/null], [INF/null], [INF/null]
 *               DEQ        [1/null], [4/null], [INF/null], [INF/null]          110
 *               --------------------------------------------------------------------------
 *            3. return TRUE when succeed,
 *               FALSE when fail on desc=NULL, msg[]=NULL, no msg in mq.
 */
BOOL_T L_MQ_Deq (L_MQ_Desc_T desc, UI32_T msg[], UI16_T *qno);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_EnableDeq
 * ---------------------------------------------------------------------
 * PURPOSE  : Set which queues are enabled to dequeue.
 * INPUT    : desc          -- descriptor of priority queue
 *            queue_bmp     -- bit map, the bit 0 is mapped to queue 0.
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : return TRUE when succeed,
 *            FALSE when fail on desc=NULL.
 */
BOOL_T L_MQ_EnableDeq(L_MQ_Desc_T desc, UI32_T queue_bmp);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_DisableDeq
 * ---------------------------------------------------------------------
 * PURPOSE  : Set which queues are disabled to dequeue.
 * INPUT    : desc          -- descriptor of priority queue
 *            queue_bmp     -- bit map, the bit 0 is mapped to queue 0.
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : return TRUE when succeed,
 *            FALSE when fail on desc=NULL.
 */
BOOL_T L_MQ_DisableDeq(L_MQ_Desc_T desc, UI32_T queue_bmp);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_GetDisableDeqBmp
 * ---------------------------------------------------------------------
 * PURPOSE  : Get bit map of disabled dequeue
 * INPUT    : desc          -- descriptor of priority queue
 *            queue_bmp     -- bit map, the bit 0 is mapped to queue 0.
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : return TRUE when succeed,
 *            FALSE when fail on desc=NULL queue_bmp=NULL.
 */
BOOL_T L_MQ_GetDisableDeqBmp(L_MQ_Desc_T desc, UI32_T *queue_bmp);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_Clean
 * ---------------------------------------------------------------------
 * PURPOSE  : Clean all records in the priority queue.
 * INPUT    : desc          -- priority queue to operate
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_MQ_Clean(L_MQ_Desc_T desc);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_Destroy
 * ---------------------------------------------------------------------
 * PURPOSE  : Destory a priority queue.
 * INPUT    : desc          -- priority queue to operate
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_MQ_Destroy(L_MQ_Desc_T desc);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_GetTailPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the queue tail pointer in priority queue.
 * INPUT    : desc          -- descriptor of priority queue
 *            queue_no      -- no of queue in priority queue
 * OUTPUT   : none
 * RETURN   : pointer of the tail
 * NOTE     : none
 */
UI8_T *L_MQ_GetTailPointer (L_MQ_Desc_T desc, UI16_T queue_no);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_GetHeadPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the queue head pointer in priority queue.
 * INPUT    : desc          -- descriptor of priority queue
 *            qno      -- no of queue in priority queue
 * OUTPUT   : none
 * RETURN   : pointer of the head
 * NOTE     : none
 */
UI8_T *L_MQ_GetHeadPointer (L_MQ_Desc_T desc, UI16_T queue_no);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_DoOperationAndMoveHead
 * ---------------------------------------------------------------------
 * PURPOSE  : do specified operation (by pass queue element into function) from head
 *            of the queue in the priority queue until some specified location(pointer),
 *            move the head of the list to that location too.
 * INPUT    : desc      -- priority queue to operate
 *            qno       -- no of queue (in priority queues) to operate
 *            endp      -- end position (pointer) (not include this position)
 *            func      -- pass queue msg in range into this function
 * OUTPUT   : none
 * RETURN   : The number of processed messages in this call
 * NOTE     : 1. the number of func parameters is according to record_len of List_T.
 *               In this function, it only pass in one UI32_T which is assume to
 *               be smaller than record_len (so record_len must larger then 4 bytes).
 *               The definition of func should be like void *func(UI32_T p1).
 */
UI32_T L_MQ_DoOperationAndMoveHead(L_MQ_Desc_T desc, UI16_T qno, UI8_T *endp, void *func);



/*---------------------------------
 * Exported functions for shared memory version
 *---------------------------------
 */
/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_ShMem_GetWorkingBufferRequiredSize
 * ---------------------------------------------------------------------
 * PURPOSE  : Calculate the required size of the buffer for a stack
 * INPUT    : nbr_of_priority   --
 *            nbr_of_queue      --
 *            queue_length[nbr_of_queue] -- the max queue length of each queue.
 *
 * OUTPUT   : none
 * RETURN   : The required size
 * NOTE     : none
 */
UI32_T L_MQ_ShMem_GetWorkingBufferRequiredSize (
    UI16_T nbr_of_priority,
    UI16_T nbr_of_queue,
    const UI16_T queue_length [] );

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_ShMem_Create
 * ---------------------------------------------------------------------
 * PURPOSE  : Create a priority queue.
 * INPUT    : working_buffer     -- the working buffer for the queue in shared memory.
 *                                  the size of working buffer must be L_MQ_ShMem_GetWorkingBufferRequiredSize()
 *            nbr_of_priority    --
 *            nbr_of_queue       --
 *            total_queue_length -- total number of message that shared by all queues.
 *            priority_queue_map[nbr_of_priority] -- index is the priority value(from 0) the result is
 *                                 queue number (from 0). value range is 0~(nbr_of_queue-1).
 *            schedule_weight[nbr_of_queue] -- the weighted value of each weight fair queue.
 *                              value 0 has special meaning, it mean it's a strict priority
 *                              relative to the lower indexed queue. But has a limitation,
 *                              if one queue has schedule_weight value 0, then all higer
 *                              priority queue(higher index) must also have 0 value.
 *                              e.g. the weighted value of queue 0 to queue 3 is
 *                              1,2,3,4 respectively. That mean the proportion is 1:2:3:4.
 *                              {4,3,2,1} and {4,4,0,0} are permitted, but (4,4,0,1} is an illegal setting.
 *
 *            queue_length[nbr_of_queue] -- the max queue length of each queue.
 *
 * OUTPUT   : desc_p             -- the descriptor
 * RETURN   : Descriptor of priority queue or NULL if fail creation
 * NOTE     : 1. The infomation of input argument will copy in internal descriptor, caller free or
 *               re-use this variables are permited.
 *            2. desc->internal_use_buffer
 *            3. fail on nbr_of_priority=0, nbr_of_queue =0, total_queue_length=0,
 *               priority_queue_map[]=NULL, schedule_weight[]=NULL, queue_length[]=NULL,
 *               priority_quque_map[] value not between 0~(nbr_of_queue-1), illegal schedule_weight[].
 *            4. the descriptor and working buffer must be in shared memory
 */
BOOL_T L_MQ_ShMem_Create (
    L_MQ_ShMem_Desc_T *desc_p,
    void *working_buffer,
    UI16_T nbr_of_priority,
    UI16_T nbr_of_queue,
    UI32_T total_queue_length,
    const UI16_T priority_queue_map[],
    const UI16_T schedule_weight [],
    const UI16_T queue_length [] );

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_ShMem_Enq
 * ---------------------------------------------------------------------
 * PURPOSE  : Put to a strict priority queue.
 * INPUT    : desc_p        -- descriptor of priority queue
 *            msg           -- the msg buffer; should be 4 unsigned long
 *            priority      -- 0 to (nbr_of_queue-1)
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTE     : 1. the large number, the higher priority.
 *            2. return TRUE when succeed,
 *               FALSE when fail on desc=NULL, msg[]=NULL, priority >(nbr_of_queue-1),
 *               msg in mq  >= total_queue_length.
 */
BOOL_T L_MQ_ShMem_Enq (L_MQ_ShMem_Desc_T *desc_p, UI32_T  msg[], UI32_T priority);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_ShMem_Deq
 * ---------------------------------------------------------------------
 * PURPOSE  : Get from a priority queue.
 * INPUT    : desc_p        -- descriptor of priority queue
 *            msg           -- the address of msg buffer; should be 4 unsigned long
 * OUTPUT   : msg           -- the msg buffer; should be 4 unsigned long
 *            qno           -- the msg is dequeue from which queue number
 * RETURN   : TRUE/FALSE
 * NOTE     : 1. Deq Rules:
 *               (1) high indexed queue will be deqed first.
 *               (2) if no msg is deqed for a round (Idle state), it will reset counter to schedule_weight.
 *            2. Example:
 *               for a schedule_weight=[2,4,0,0]
 *               the internal counter for strict priority (0) is INF
 *               an example queue state [2/101,102] means counter=2, msg=101,102
 *               an example operation is
 *               ACTION     STATE                                               DEQ_OUTPUT
 *               --------------------------------------------------------------------------
 *               INITIAL    [2/101,102], [4/103,104], [INF/105], [INF/106,111]
 *               DEQ        [2/101,102], [4/103,104], [INF/105], [INF/111]      106
 *               DEQ        [2/101,102], [4/103,104], [INF/105], [INF/null]     111
 *               DEQ        [2/101,102], [4/103,104], [INF/null], [INF/null]    105
 *               DEQ        [2/101,102], [3/104], [INF/null], [INF/null]        103
 *               ENQ(107,3) [2/101,102], [3/104], [INF/null], [INF/107]
 *               DEQ        [1/102], [3/104], [INF/null], [INF/107]             101
 *               ENQ(108,3) [1/102], [3/104], [INF/null], [INF/107,108]
 *               DEQ        [1/102], [3/104], [INF/null], [INF/108]             107
 *               DEQ        [1/102], [3/104], [INF/null], [INF/null]            108
 *               DEQ        [1/102], [2/null], [INF/null], [INF/null]           104
 *               ENQ(109,2) [1/102], [2/null], [INF/109], [INF/null]
 *               ENQ(110,0) [1/102,110], [2/null], [INF/109], [INF/null]
 *               DEQ        [0/110], [2/null], [INF/109], [INF/null]            102
 *               DEQ        [0/110], [2/null], [INF/null], [INF/null]           109
 *               reset cntr [2/110], [4/null], [INF/null], [INF/null]
 *               DEQ        [1/null], [4/null], [INF/null], [INF/null]          110
 *               --------------------------------------------------------------------------
 *            3. return TRUE when succeed,
 *               FALSE when fail on desc=NULL, msg[]=NULL, no msg in mq.
 */
BOOL_T L_MQ_ShMem_Deq (L_MQ_ShMem_Desc_T *desc_p, UI32_T msg[], UI16_T *qno);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_ShMem_Clean
 * ---------------------------------------------------------------------
 * PURPOSE  : Clean all records in the priority queue.
 * INPUT    : desc_p        -- priority queue to operate
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_MQ_ShMem_Clean (L_MQ_ShMem_Desc_T *desc_p);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_ShMem_Destroy
 * ---------------------------------------------------------------------
 * PURPOSE  : Destory a priority queue.
 * INPUT    : desc_p        -- priority queue to operate
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_MQ_ShMem_Destroy (L_MQ_ShMem_Desc_T *desc_p);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_ShMem_GetTailPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the queue tail pointer in priority queue.
 * INPUT    : desc_p   -- descriptor of priority queue
 *            qno      -- no of queue in priority queue
 * OUTPUT   : none
 * RETURN   : pointer of the tail
 * NOTE     : none
 */
UI8_T *L_MQ_ShMem_GetTailPointer (L_MQ_ShMem_Desc_T *desc_p, UI16_T qno);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_ShMem_GetHeadPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the queue head pointer in priority queue.
 * INPUT    : desc_p   -- descriptor of priority queue
 *            qno      -- no of queue in priority queue
 * OUTPUT   : none
 * RETURN   : pointer of the head
 * NOTE     : none
 */
UI8_T *L_MQ_ShMem_GetHeadPointer (L_MQ_ShMem_Desc_T *desc_p, UI16_T qno);

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_ShMem_DoOperationAndMoveHead
 * ---------------------------------------------------------------------
 * PURPOSE  : do specified operation (by pass queue element into function) from head
 *            of the queue in the priority queue until some specified location(pointer),
 *            move the head of the list to that location too.
 * INPUT    : desc_p    -- priority queue to operate
 *            qno       -- no of queue (in priority queues) to operate
 *            endp      -- end position (pointer) (not include this position)
 *            func      -- pass queue msg in range into this function
 * OUTPUT   : none
 * RETURN   : The number of processed messages in this call
 * NOTE     : 1. the number of func parameters is according to record_len of L_CIRLST_ShMem_List_T.
 *               In this function, it only pass in one UI32_T which is assume to
 *               be smaller than record_len (so record_len must larger then 4 bytes).
 *               The definition of func should be like void *func(UI32_T p1).
 */
UI32_T L_MQ_ShMem_DoOperationAndMoveHead(L_MQ_ShMem_Desc_T *desc_p, UI16_T qno, UI8_T *endp, void *func);

#endif
