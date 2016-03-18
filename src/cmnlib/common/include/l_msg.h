/*---------------------------------------------------------------------
 * File_Name : L_MSG.h
 *
 * Purpose   : This package provides the functions to use priority
 *			   queue for MSG (high priority msg processed first ).
 *
 *
 * Copyright(C)      Accton Corporation, 2003
 *
 * Note    	 : Designed for Mercury
 * History	 : 2003/12/15, Penny created.
 *             2004/05/03, Penny: implement WFQ feature. Add one more parameter
 *                              -- method for L_MSG_CreatePriorityQueue()
 *             2004/09/22, jinwang: Delete the previous priority_queue_mapping definition in L_MSG_PriorityQueue_T.
 *				Add the L_PRIORITY_QUEUE_MAP_T struct to ensure "one priority
 *				map to one queue and all priority have their own queue length limit"
 *---------------------------------------------------------------------
 */

#ifndef L_MSG_H
#define L_MSG_H

/* INCLUDE FILE DECLARATIONS
 */

#include "sys_type.h"
#include "l_pt.h"

/* MACRO DECLARATIONS
 */
#define 	MAX_NUM_PRI 			16
#define 	MAX_NUM_OF_QUEUE		16
/* NAMING CONSTANT DECLARACTIONS
 */



/* DATA TYPE DECLARACTIONS
 */
typedef UI32_T L_MSG_Message_T[4];      /* the msg buffer; should be 4 unsigned long  */

/* each priority maps to one L_PRIORITY_QUEUE_MAP_T structure.
   each priority also only maps to one queue,This means L_PRIORITY_QUEUE_MAP_T[0].queue_id=0,  L_PRIORITY_QUEUE_MAP_T[1].queue_id=1,
    бн    L_PRIORITY_QUEUE_MAP_T[7].queue_id=7.
e.g. the usage:
(1). If using L_MSG_WFQ and the total queue length is for all priority queue share, set each priority's queue length as the arbitrary value (suggest initial value as 0).
     It maybe like following:
static L_PRIORITY_QUEUE_MAP_T priority_queue_map[]=
{
	{0,x,0}, {1,x,0},
	{2,x,0}, {3,x,0},
	{4,x,0}, {5,x,0},
	{6,x,0}, {7,x,0}
}
In corresponding WFQ creation function L_MSG_CreatePriorityQueue, parameter total_queue_length should be set one given value.
The consume_length is generally initialized as 0.

(2). If using L_MSG_WFQ and each priority queue is given dedicated length, just directly use the dedicated queue length to initialize the map relationship.
     It maybe like following:
static L_PRIORITY_QUEUE_MAP_T priority_queue_map[]=
{
	{0,22,0}, {1,3,0},
	{2,20,0}, {3,50,0},
	{4,50,0}, {5,160,0},
	{6,65,0}, {7,100,0}
}
In corresponding WFQ creation function L_MSG_CreatePriorityQueue, parameter total_queue_length should be set as 0.
The consume_length is generally initialized as 0.

(3). If using L_MSG_STRICT, the same as the above form described in (1).



*/
#pragma pack(1)
typedef struct
{
      UI16_T   queue_length;                	/*length of the queue each priority mapped to. 1... 65535 */
      UI16_T   consume_length;		 	/* current used queue length */
      UI8_T    queue_id;                 		/* 0, 1...MAX_NUM_MSG_Q */
}L_PRIORITY_QUEUE_MAP_T;
#pragma pack()

typedef struct
{
   	UI32_T   nbr_of_queue;				/* total numbers of queue */
   	UI32_T   nbr_of_priority;			/* total numbers of priority */
   	L_PRIORITY_QUEUE_MAP_T	 *L_priority_queue_map;	/* One dimension array mapping. */
   							/*   Index:priority; Content: Queue number,queue length and current used queue length. */				
   	
   	UI32_T	 total_queue_length;    /*total_queue_length -- total length of the queue
 					0: dedicated queue length for each priority.
 					x: total queue length for all priority share.
					*/
    	
    	UI16_T 	 *scheduling;               /* the proportion of each queue.
                                           e.g. scheduling[queue_id] = weight
                                           for low priority queue to high one,
                                           the proportion is 1:2:3:4, which means
                                           scheduling[nbr_of_queue-1] = 4
                                           .
                                           .
                                           scheduling[0] = 1
                                         */
    	UI32_T    method;                     /* L_MSG_STRICT or L_MSG_WFQ */

    	void     *internal_use_buffer;       /* the buffer for internal use. */


}L_MSG_PriorityQueue_T;

typedef enum
{
    L_MSG_STRICT,
    L_MSG_WFQ      /* Weighted fair queue */
}L_MSG_PriorityQueueMethod_T;



/* PURPOSE: Create a priority queue.
 * INPUT:   priority_queue          -- Descriptor of priority queue
 *          nbr_of_queue            -- total numbers of queue,max value depends on MAX_NUM_MSG_Q.
 *          nbr_of_priority         -- total numbers of priority,max value depends on MAX_NUM_PRI.
 *				       When using dedicate priority queue length mode,make sure
 *				       nbr_of_priority <= nbr_of_queue.
 *          priority_queue_map      -- One dimension array mapping.
 *				       Index:priority; Content: Queue number,queue length and current used queue length.
 *          total_queue_length      -- total length of the queue
 * 					0: dedicated queue length for each priority.
 *					x: total queue length for all priority share.
 *          schedule                -- array to store the WFQ packet ratio for priority queue associated
 *                                      for create strict queue, input schedule[0]=0
 *          method                  -- priority queue method (L_MSG_STRICT/L_MSG_WFQ)
 *
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 * NOTE:    1. High priority msg processed first which stored inside higher queue number
 *          2. For priority_queue_mapping, the low priority id CAN NOT mapping to high queue id.
 *              In other words, highest queue id contains the highest priority.
 *	    3. To keep one priority map to one queue,make sure nbr_of_priority < nbr_of_queue.
 *          4. priority_queue->internal_use_buffer
 *          +---------------------------------+
 *          |  L_MSG_InternalUsedBuffer_T     |
 *          |---------------------------------|
 *          |       desc.buffer (pool)        |
 *          |---------------------------------|
 *          |       queue_list                |
 *          |---------------------------------|
 *          |       couter_for_schduling      |
 *          +---------------------------------+
 *
 */
BOOL_T L_MSG_CreatePriorityQueue(L_MSG_PriorityQueue_T *priority_queue,
                                 UI32_T   nbr_of_queue,
                                 UI32_T   nbr_of_priority,
                                 L_PRIORITY_QUEUE_MAP_T	 *priority_queue_map,
                                 UI32_T	    total_queue_length,
                                 UI16_T     *schedule,
                                 L_MSG_PriorityQueueMethod_T method);

/* PURPOSE: Put to a strict priority queue.
 * INPUT:   priority_queue
 *          msg            -- the msg buffer; should be 4 unsigned long
 *          priority       -- the msg priority
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 * Note:    1. the large number, the higher priority.
 */
BOOL_T L_MSG_PutToPriorityQueue(L_MSG_PriorityQueue_T *priority_queue,
                                L_MSG_Message_T       msg,
                                UI32_T                priority);

/* PURPOSE: Get from a priority queue.
 * INPUT:   priority_queue
 *          msg            -- the address of msg buffer; should be 4 unsigned long
 *          priority       -- the address of msg priority
 * OUTPUT:  msg            -- the msg buffer; should be 4 unsigned long
 *          priority       -- the msg priority
 * RETURN:  TRUE/FALSE
 * Note:    1. the large number, the higher priority.
 */
BOOL_T L_MSG_GetFromPriorityQueue(L_MSG_PriorityQueue_T *priority_queue,
                                  L_MSG_Message_T        msg,
                                  UI32_T                *priority);

#endif /* L_MSG_H */

