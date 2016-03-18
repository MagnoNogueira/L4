/*---------------------------------------------------------------------
 * File_Name : L_MSG.c
 *
 * Purpose   : This package provides the functions to use priority
 *			   queue for MSG (high priority msg processed first ).
 *
 *
 * Copyright(C)      Accton Corporation, 2003
 *
 * Note    	 : Designed for Mercury
 * History	 : 2003/12/15, Penny created.
 *             2004/04/23, Penny: Add interrupt lock protection.
 *             2004/05/03, Penny: implement WFQ feature. Add one more parameter
 *                              -- method for L_MSG_CreatePriorityQueue()
 *	       2004/09/22, jinwang: Modify the L_MSG_PriorityQueue_T struct to ensure "one priority
 *				map to one queue and all priority have their own queue length limit"
 *---------------------------------------------------------------------
 */
#include <stdio.h>
#include "sys_type.h"
#include <stdlib.h>
#include <string.h>
#include "l_msg.h"


/* INCLUDE FILE DECLARATIONS
 */

/* NAMING CONSTANT DECLARATIONS
 */

/* DATA TYPE DECLARACTIONS
 */

typedef struct L_MSG_QueueElement_S
{
    L_MSG_Message_T msg;
    struct L_MSG_QueueElement_S *next;
    UI32_T priority;
}L_MSG_QueueElement_T;


typedef struct
{
    L_MSG_QueueElement_T *head;
    L_MSG_QueueElement_T *tail;
}L_MSG_QueueList_T;

typedef struct
{
    L_PT_Descriptor_T   q_desc;    /* for l_pt descriptor */
    L_MSG_QueueList_T   *q_list;
    UI16_T              *left_count;      /* the remaining counter for scheduling deduction */
    UI32_T              dq_start_idx;       /* for WFQ use only */

}L_MSG_InternalUsedBuffer_T;

typedef BOOL_T (*L_MSG_Dequeue_Handler_T) (void             *priority_queue,
                                           L_MSG_Message_T   msg,
                                           UI32_T           *priority);


/* STATIC FUNCTION DECLARATIONS
 */
static BOOL_T L_MSG_GetFromWeightedFairQueue(L_MSG_PriorityQueue_T *priority_queue,
                                             L_MSG_Message_T        msg,
                                             UI32_T                *priority);

static BOOL_T L_MSG_GetFromStrictPriorityQueue(L_MSG_PriorityQueue_T *priority_queue,
                                               L_MSG_Message_T          msg,
                                               UI32_T                  *priority);

static int L_MSG_SeekForStartingIndex(L_MSG_PriorityQueue_T *priority_queue);

/* STATIC VARIABLE DECLARATIONS
 */
/* function pointer to dequeue according to deQ method */
static L_MSG_Dequeue_Handler_T  l_msg_dequeue_handler[2] = {(L_MSG_Dequeue_Handler_T)L_MSG_GetFromStrictPriorityQueue,
                                                            (L_MSG_Dequeue_Handler_T)L_MSG_GetFromWeightedFairQueue};


/* PURPOSE: Create a priority queue.
 * INPUT:   priority_queue          -- Descriptor of priority queue
 *          nbr_of_queue            -- total numbers of queue
 *          nbr_of_priority         -- total numbers of priority
 *				       When using dedicate priority queue length mode,make sure
 *				       nbr_of_priority <= nbr_of_queue.
 *          priority_queue_map      -- One dimension array mapping.
 * 									    Index:priority; Content:Queue number
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
 *	    3. To keep one priority map to one queue,make sure nbr_of_priority <= nbr_of_queue.
 *          4. priority_queue->internal_use_buffer
 *          +---------------------------------+
 *          |  L_MSG_InternalUsedBuffer_T     |
 *          |---------------------------------|
 *          |       desc.buffer (pool)        |
 *          |---------------------------------|
 *          |       q_list                |
 *          |---------------------------------|
 *          |       counter_for_schduling     |
 *          +---------------------------------+
 *
 */
BOOL_T L_MSG_CreatePriorityQueue(L_MSG_PriorityQueue_T  *priority_queue,
                                 UI32_T                 nbr_of_queue,
                                 UI32_T                 nbr_of_priority,
				                 L_PRIORITY_QUEUE_MAP_T	*priority_queue_map,
                                 UI32_T	                total_queue_length,
                                 UI16_T                 *schedule,
                                 L_MSG_PriorityQueueMethod_T method)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
	L_MSG_InternalUsedBuffer_T *internal_use_buffer;
    UI32_T i;

    /* BODY
     */
    if (priority_queue == NULL || nbr_of_queue == 0 ||  nbr_of_priority == 0 || priority_queue_map == 0 ||
          schedule == 0 )
    {
	    return FALSE;
    }
    if ( (nbr_of_queue > MAX_NUM_OF_QUEUE) ||(nbr_of_priority > MAX_NUM_PRI) )
    {
	    return FALSE;
    }
    if(!( (method == L_MSG_STRICT) || (method == L_MSG_WFQ) ))
    {
        return FALSE;
    }

    /* added by jinwang 2004-10-19*/
    for(i=0;i<nbr_of_priority;i++)
        priority_queue_map[i].consume_length=0;

    /* init each priority quque length */
	if(total_queue_length == 0)  /* dedicate queue mode */
	{
		for(i=0;i<nbr_of_priority;i++)
     	    total_queue_length+=priority_queue_map[i].queue_length;
	}
	else
	{
		for(i=0;i<nbr_of_priority;i++)
     	    priority_queue_map[i].queue_length=total_queue_length;
	}

    priority_queue->nbr_of_queue            = nbr_of_queue;
    priority_queue->nbr_of_priority         = nbr_of_priority ;
    priority_queue->L_priority_queue_map    = priority_queue_map;
    priority_queue->total_queue_length      = total_queue_length;
    priority_queue->scheduling              = schedule;
    priority_queue->method                  = (UI32_T)method;
	
	/* Allocate once for L_MSG_InternalUsedBuffer_T, q_list,
	**   pool and priority_queue->left_count
	*/
	priority_queue->internal_use_buffer = malloc(sizeof(L_MSG_InternalUsedBuffer_T) +
	                                             nbr_of_queue*sizeof(L_MSG_QueueList_T) +
					                             priority_queue->total_queue_length * (sizeof(L_MSG_QueueElement_T)) +
					                             nbr_of_queue * sizeof(*schedule));
					
	
	if (!priority_queue->internal_use_buffer)
    {
	    return FALSE;
    }

    /* L_pt_create
    */
    internal_use_buffer = (L_MSG_InternalUsedBuffer_T *)priority_queue->internal_use_buffer;
    internal_use_buffer->q_desc.buffer = ((char *)priority_queue->internal_use_buffer + sizeof(L_MSG_InternalUsedBuffer_T));
    internal_use_buffer->q_desc.buffer_len = (priority_queue->total_queue_length * sizeof(L_MSG_QueueElement_T));
    internal_use_buffer->q_desc.partition_len = sizeof(L_MSG_QueueElement_T);
    L_PT_Create(&internal_use_buffer->q_desc);

    internal_use_buffer->q_list = (L_MSG_QueueList_T *)((char *)priority_queue->internal_use_buffer +
                                       sizeof(L_MSG_InternalUsedBuffer_T)  +
                                       internal_use_buffer->q_desc.buffer_len);

    memset(internal_use_buffer->q_list, 0, nbr_of_queue * sizeof(L_MSG_QueueList_T));
	internal_use_buffer->left_count =(UI16_T *) &internal_use_buffer->q_list[nbr_of_queue];
    memcpy(internal_use_buffer->left_count, schedule, nbr_of_queue * sizeof(*internal_use_buffer->left_count));
    internal_use_buffer->dq_start_idx = 0;
    return TRUE;

} /* end of L_MSG_CreatePriorityQueue*/


/* PURPOSE: Put a priority queue.
 * INPUT:   priority_queue
 *          msg            -- the msg buffer; should be 4 unsigned long
 *          priority       -- the msg priority
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 * Note:    1. the large number, the higher priority. (e.g 0 - the lowest priority)
 */
BOOL_T L_MSG_PutToPriorityQueue(L_MSG_PriorityQueue_T *priority_queue,
                                L_MSG_Message_T       msg,
                                UI32_T                priority)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    int index;
    L_MSG_InternalUsedBuffer_T *internal_use_buffer;
    L_MSG_QueueElement_T  *element;
    L_MSG_QueueList_T *queue_list;
    //UI32_T   oint;
    /* BODY
     */
    if (priority_queue == NULL)
        return FALSE;

    if (priority >= priority_queue->nbr_of_priority)
        return FALSE;

    /* On Linux, SYSFUN_InterruptLock() isn't provided in user mode.
     */
    /* oint = SYSFUN_InterruptLock ();  */

    /* Insert to link list */
    index = (int)(priority_queue->L_priority_queue_map[priority].queue_id);
    if(priority_queue->L_priority_queue_map[priority].consume_length >= (priority_queue->L_priority_queue_map[priority].queue_length))
    {
    	/* SYSFUN_InterruptUnlock ( oint ); */
        return FALSE;
    }
    else
    	priority_queue->L_priority_queue_map[priority].consume_length++;

    internal_use_buffer = (L_MSG_InternalUsedBuffer_T *)priority_queue->internal_use_buffer;
    element=(L_MSG_QueueElement_T *)L_PT_Allocate(&internal_use_buffer->q_desc);
    if(element == NULL)
    {
        /* SYSFUN_InterruptUnlock ( oint ); */
        return FALSE;
    }

    memcpy(element->msg, msg, sizeof(L_MSG_Message_T));
    element->priority = priority;
    element->next = NULL;

    queue_list = internal_use_buffer->q_list;
    if (queue_list[index].head == NULL)
    {
        /* insert to list head */
        queue_list[index].head = element;
    }
    else
    {
        queue_list[index].tail->next = element;
    }

    queue_list[index].tail = element;

    /* SYSFUN_InterruptUnlock ( oint );   */
    return TRUE;

}/* end of L_MSG_PutToPriorityQueue */

/* PURPOSE: Get a priority queue.
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
                                  UI32_T                *priority)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
    BOOL_T ret = FALSE;
	//UI32_T oint;
	
    /* BODY
     */
    if (priority_queue == NULL || priority_queue->internal_use_buffer == NULL  ||
	    priority_queue->nbr_of_queue == 0 || priority == NULL)
        return FALSE;

    /* On Linux, SYSFUN_InterruptLock() isn't provided in user mode.
     */
	/* oint=SYSFUN_InterruptLock(); */
	
	ret = (*l_msg_dequeue_handler[priority_queue->method])(priority_queue, msg, priority);
    if(ret)
    {
        priority_queue->L_priority_queue_map[*priority].consume_length--;
    }
	/* SYSFUN_InterruptUnlock(oint); */
	return ret;
    	
}/* end of L_MSG_GetFromPriorityQueue */


/* PURPOSE: Get a from strict priority queue.
 * INPUT:   priority_queue
 *          msg            -- the address of msg buffer; should be 4 unsigned long
 *          priority       -- the address of msg priority
 * OUTPUT:  msg            -- the msg buffer; should be 4 unsigned long
 *          priority       -- the msg priority
 * RETURN:  TRUE/FALSE
 * Note:    1. the large number, the higher priority.
 */
static BOOL_T L_MSG_GetFromStrictPriorityQueue(L_MSG_PriorityQueue_T *priority_queue,
                                               L_MSG_Message_T        msg,
                                               UI32_T                *priority)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
     int index1, index2;
     L_MSG_InternalUsedBuffer_T *internal_use_buffer;
     L_MSG_QueueElement_T  *element;
     L_MSG_QueueList_T *queue_list;
     BOOL_T is_nothing_found = FALSE;

    /* BODY
     */
    internal_use_buffer = (L_MSG_InternalUsedBuffer_T *)priority_queue->internal_use_buffer;

    /* Get highest priority queue msg */
    index1 = (int )priority_queue->nbr_of_priority - 1;
    queue_list = internal_use_buffer->q_list;
    while (index1 >= 0)
    {
    	/* modified by jinwang 2004-10-19 */
	    index2 = (int)(priority_queue->L_priority_queue_map[index1].queue_id);
        if (queue_list[index2].head != NULL)
        {
            element = queue_list[index2].head;
            memcpy(msg, element->msg, sizeof(L_MSG_Message_T));
            *priority = element->priority;

            /* Free the partition */
            if (queue_list[index2].head != queue_list[index2].tail)
            {
                queue_list[index2].head = queue_list[index2].head->next;
            }
            else
            {
                queue_list[index2].head = NULL;
                queue_list[index2].tail = NULL;
            }

            L_PT_Free(&internal_use_buffer->q_desc, element);
            break;
        }

        if (index1 == 0)
        {
            is_nothing_found = TRUE;
            break;
        }
        index1--;

    }/* end of while (index1 >= 0)*/

    if (is_nothing_found == TRUE)
    {
        return FALSE;
    }
    return TRUE;
}/* end of L_MSG_GetFromStrictPriorityQueue */

/* 2004/05/03, Penny: implement WFQ feature
*/
/* PURPOSE: Get from a weighted fair queue.
 * INPUT:   priority_queue
 *          msg            -- the address of msg buffer; should be 4 unsigned long
 *          priority       -- the address of msg priority
 * OUTPUT:  msg            -- the msg buffer; should be 4 unsigned long
 *          priority       -- the msg priority
 * RETURN:  TRUE/FALSE
 * Note:    1. the large number, the higher priority.
 */
static BOOL_T L_MSG_GetFromWeightedFairQueue(L_MSG_PriorityQueue_T *priority_queue,
                                             L_MSG_Message_T        msg,
                                             UI32_T                *priority)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLES DEFINITION
     */
     L_MSG_QueueList_T        *queue_list;
     L_MSG_QueueElement_T   *element;
     UI32_T   i = 0;

     UI32_T num_of_queue_empty = 0;
     BOOL_T is_nothing_found = TRUE;
     L_MSG_InternalUsedBuffer_T *internal_use_buffer;
	
    /* BODY
     */
     /*modified by jinwang,Move interruption protection to the caller function,2004-11-02*/
    internal_use_buffer = (L_MSG_InternalUsedBuffer_T *)priority_queue->internal_use_buffer;

    if (internal_use_buffer->dq_start_idx > priority_queue->nbr_of_queue)
       internal_use_buffer->dq_start_idx = priority_queue->nbr_of_queue;

    queue_list = internal_use_buffer->q_list;
    internal_use_buffer->dq_start_idx = L_MSG_SeekForStartingIndex(priority_queue) ;

    while (1)
    {  		
        /* Queue not empty */
        if (queue_list[internal_use_buffer->dq_start_idx].head != NULL)
        {
            element = queue_list[internal_use_buffer->dq_start_idx].head;
            memcpy(msg, element->msg, sizeof(L_MSG_Message_T));
            *priority = element->priority;

            /* Free the partition */
            if (queue_list[internal_use_buffer->dq_start_idx].head != queue_list[internal_use_buffer->dq_start_idx].tail)
            {
                queue_list[internal_use_buffer->dq_start_idx].head = queue_list[internal_use_buffer->dq_start_idx].head->next;
            }
            else
            {
                queue_list[internal_use_buffer->dq_start_idx].head = NULL;
                queue_list[internal_use_buffer->dq_start_idx].tail = NULL;
            }

            L_PT_Free(&internal_use_buffer->q_desc, element);
            is_nothing_found = FALSE;
            internal_use_buffer->left_count[internal_use_buffer->dq_start_idx]--;
            break;
        }
        else /* if queue is empty, move to next indexed queue */
        {
	     internal_use_buffer->left_count[internal_use_buffer->dq_start_idx]--;

            /* if all queues are empty, break the loop and return false */
            if (num_of_queue_empty >= priority_queue->nbr_of_queue)
            {
                int empty_queue_count = 0;
                /*if all queues are empty, then break the loop */
                for (i=0;i<priority_queue->nbr_of_queue;i++)
                {
                    if (queue_list[i].head == NULL)
                        empty_queue_count++;
                }

                if (empty_queue_count == priority_queue->nbr_of_queue)
                    break;
                else
                    num_of_queue_empty = 0;
            }
            else
            {
                num_of_queue_empty++;
            }
        }

        internal_use_buffer->dq_start_idx = L_MSG_SeekForStartingIndex(priority_queue);

    } /* end of while loop */

    if (is_nothing_found == TRUE)  /* all queues are empty */
    {
        internal_use_buffer->dq_start_idx = priority_queue->nbr_of_queue;
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}/* end of  L_MSG_GetFromWeightedFairQueue */

static int L_MSG_SeekForStartingIndex(L_MSG_PriorityQueue_T *priority_queue)
{
    /* LOCAL CONSTANT DECLARATIONS
     */

    /* LOCAL VARIABLES DEFINITION
     */
    int return_index;
	UI32_T count = 0;
    L_MSG_InternalUsedBuffer_T *internal_use_buffer;
    UI32_T i;
    /* BODY
    */
    internal_use_buffer = (L_MSG_InternalUsedBuffer_T *)priority_queue->internal_use_buffer;
    /* 1. Get the potential return index  */
    return_index = (internal_use_buffer->dq_start_idx) ? internal_use_buffer->dq_start_idx - 1 : priority_queue->nbr_of_queue-1;
    /* 2. Check counter value of the potential return index
    **       if value == 0, adjust to a non-zero counter index
    */
    for (i=0; i<priority_queue->nbr_of_queue; i++)
    {
        if (internal_use_buffer->left_count[return_index] == 0)
        {
            count++;
            return_index = (return_index) ? return_index - 1 : priority_queue->nbr_of_queue-1;
        }
        else
            return return_index;
    }

    /* 2-1. If all counters are zero, restore the ratio to left_count
    */
    if (count == priority_queue->nbr_of_queue)
    {
        memcpy(internal_use_buffer->left_count, priority_queue->scheduling, priority_queue->nbr_of_queue * sizeof(*internal_use_buffer->left_count));
        return_index = priority_queue->nbr_of_queue-1;
    }

	return return_index;
}/* end of L_MSG_SeekForStartingIndex */

#ifdef L_MSG_DEBUG

/* The following code contains the sample program for using l_msg libaray
*/

static UI16_T schedule_strict[]={0};                     /* scheduling CAN NOT be declared as a local function
                                                         **        variable
                                                         */
static UI16_T schedule_WFQ[]={1,2,3,5};                  /* scheduling CAN NOT be declared as a local function
                                                         **        variable
                                                         */
#if 0
static UI8_T priority_queue_mapping[]={0,0,1,2,2,2,3,3}; /* Based on queue creation function spec, the
                                                         **        mapping can not be {3,3,2,1,1,1,0,0}
                                                         */
 #else
 static L_PRIORITY_QUEUE_MAP_T priority_queue_map[]={
 		{0,512},{1,512},
		{2,512},{3,512},
		{4,512},{5,512},
		{6,512},{7,512}
	};
 #endif

void main()
{
   static  L_MSG_PriorityQueue_T priority_queue;
   BOOL_T   ret;
   char msg[16];
   L_MSG_Message_T msg_key;
   char str[3];
   UI32_T priority;


   ungetch( 'm');

   while ( 1 )
   {
      switch ( getch() )
      {
      case 'M':
      case 'm':
         puts("\n\nSet, Get, Create(Strict), createWfq(wfq) eXit");
         continue;

      case 'C':
      case 'c':

        printf("\n Creat strict priority queue!");
        ret = L_MSG_CreatePriorityQueue(&priority_queue,
                                    8, /* number_of Q */
                                    8,
                                    /* priority_queue_mapping */ priority_queue_map,
                                    25,
                                    schedule_strict,
                                    L_MSG_STRICT);

         continue;


      case 'W':
      case 'w':

        printf("\n Creat weighted fair queue!");
         ret = L_MSG_CreatePriorityQueue(&priority_queue,
                                    8, /* number_of Q */
                                    8,
                                    /* priority_queue_mapping */ priority_queue_map,
                                    25,
                                    schedule_WFQ,
                                    L_MSG_WFQ);

         continue;


      case 'g':
      case 'G':

         ret = L_MSG_GetFromPriorityQueue(&priority_queue,
                                    msg_key,
                                    &priority);
         if (ret)
         {
             printf("\npriority = %d",priority);
             printf("\nmsg = %s",msg_key);
         }
         else
         {
            printf("\nNothing in Queue!");
            exit (1);
         }

         continue;

      case 's':
      case 'S':
get_msg:
        printf("\n Key in msg content:");
        if (gets(msg) == NULL)
    	{
    		goto get_msg;
    	} /* end if gets(msg_key) */
    	else
    	{
    		if (strcmp(msg,"") == 0)
    			goto get_msg; //press ENTER			
    	}
    	
    	//msg = (UI32_T)&msg_key[0]; 	
    	
    	printf("\n Priority (0~3)= ");
    	gets(str);
    	printf ("%s\n",str);

		priority = atol(str);

   	  	memcpy(msg_key, msg, sizeof(L_MSG_Message_T));
         ret = L_MSG_PutToPriorityQueue(&priority_queue,
                                        msg_key,
                                         priority) ;
         if (ret)
         {
            printf("\nSet successfully.");
         }
         else
         {
            printf("\nSet failed.");
         }
         continue;

      case 'x':
      case 'X':
         exit(1);

      default:
        ungetch( 'm');
        continue;
      }
   }/* end of while(1) */

}

#endif

