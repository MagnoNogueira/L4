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

/* INCLUDE FILE DECLARATIONS
 */
#include <stdlib.h>
#include <string.h>
#include "sys_type.h"
#include "l_cvrt.h"
#include "l_mq.h"
#include "l_cirlst.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define INFINITE_NBR         (UI16_T)0xFFFF

/* DATA TYPE DECLARACTIONS
 */
typedef struct
{
    UI32_T          total_queue_length;
    UI32_T          msg_nbr_in_q;
    UI16_T          *priority_queue_map;
    UI16_T          *schedule_weight;
    L_CIRLST_List_T *queue;
    UI16_T          *wcnter;          /* working counter of weighted value */
    UI16_T          nbr_of_priority;
    UI16_T          nbr_of_queue;
    UI16_T          last_qno;

    /* 2008-12-15, Jinfeng Chen: for control which queue isn't permitted to dequeue */
    UI32_T          disable_deq_bmp;
    /* the following is the physical memory allocation for fields
     * buf_of_priority_queue_map;
     * buf_of_schedule_weight;
     * buf_of_queue_length;
     * buf_of_queue;
     * buf_of_processed_cnt;
     */
} InDesc_T;


/* MACRO FUNCTION DECLARATIONS
 */
#define ADV(id_p) id_p->last_qno = (UI16_T) ((last_qno)? last_qno-1:id_p->nbr_of_queue-1)

/*---------------------------------
 * Macro for shared memory version
 *---------------------------------
 */
/* to calculate size
 */
#define SHMEM_PR_TO_Q_MAP_SIZE (sizeof(UI16_T) * (nbr_of_priority))
#define SHMEM_WEIGHT_SIZE      (sizeof(UI16_T) * (nbr_of_queue))
#define SHMEM_CNT_SIZE         (sizeof(UI16_T) * (nbr_of_queue))
#define SHMEM_Q_SIZE           (sizeof(L_CIRLST_ShMem_List_T) * (nbr_of_queue))

/* to access the data that were not defined in L_MQ_ShMem_Desc_T
 *  UI16_T          priority_queue_map[];
 *  UI16_T          schedule_weight[];
 *  UI16_T          wcnter[];
 *  L_CIRLST_List_T queue[];
 *  UI8_T           queue_buffer[];
 */
#define SHMEM_PR_TO_Q_MAP(desc_p) ((UI16_T *)L_CVRT_GET_PTR((desc_p), (desc_p)->priority_queue_map_offset))
#define SHMEM_WEIGHT(desc_p)      ((UI16_T *)L_CVRT_GET_PTR((desc_p), (desc_p)->schedule_weight_offset))
#define SHMEM_CNT(desc_p)         ((UI16_T *)L_CVRT_GET_PTR((desc_p), (desc_p)->wcnter_offset))
#define SHMEM_Q(desc_p, i)        ((L_CIRLST_ShMem_List_T *)L_CVRT_GET_PTR((desc_p), (desc_p)->queue_offset) + (i))

/* LOCAL FUNCTION DECLARATIONS
 */
static void RestoreWFQ_Counter (InDesc_T *id_p);

/*---------------------------------
 * Local functions for shared memory version
 *---------------------------------
 */
static void L_MQ_ShMem_RestoreWFQ_Counter (L_MQ_ShMem_Desc_T *desc_p);



/* EXPORTED SUBPROGRAM BODIES
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
    const UI16_T queue_length [] )
{
    /* define buffer size */
    #define PR_TO_Q_MAP_SIZE    (sizeof(*dp->priority_queue_map)*nbr_of_priority)
    #define WEIGHT_SIZE         (sizeof(*dp->schedule_weight)*nbr_of_queue)
    #define Q_SIZE              (sizeof(L_CIRLST_List_T)*nbr_of_queue)
    #define CNT_SIZE            WEIGHT_SIZE

    /* LOCAL VARIABLES */
    InDesc_T    *dp;
    UI8_T       *bp;
    I32_T       i;
    /* BODY
     */

    if( ( nbr_of_priority ==0 ) || ( total_queue_length == 0 ) || \
        ( priority_queue_map == NULL) || ( schedule_weight == NULL ) || \
        ( queue_length == NULL ) || (nbr_of_queue > sizeof(dp->disable_deq_bmp) * 8))
    {
        return NULL;
    }
    /* check Priority_To_Queue_Mapping */
    for ( i=0 ; i<nbr_of_priority ; i++ )
    {
        if ( priority_queue_map[i] >= nbr_of_queue )
            return NULL;
    }

    /* Check Schedule_Weight */
    {
        int now_is_one = 0, next_is_one;

        for ( i=nbr_of_queue-1; i>=0 ; i-- )
        {
            /* caculate next_is_one */
            next_is_one = (schedule_weight[i]!=0);

            /*  !next_is_one == next_is_zero */
            if ( now_is_one &&  (!next_is_one) )
            {
                return NULL;
            }
            now_is_one |= next_is_one;
        }
    } /* end of Check_Schedule_Weight */

    /* allocate memory
     */
    dp = malloc (
        sizeof(InDesc_T) +      /* InDesc_T size */
        PR_TO_Q_MAP_SIZE +      /* buffer of priority_queue_map */
        WEIGHT_SIZE      +      /* buffer of schedule_weight */
        Q_SIZE           +      /* buffer of queue */
        CNT_SIZE                /* buffer of wcnter */
        );

    if ( dp == NULL )
        return NULL;

    bp = ((UI8_T *)dp) + sizeof(InDesc_T);    dp->priority_queue_map = (UI16_T*)bp;
    bp += PR_TO_Q_MAP_SIZE;                 dp->schedule_weight =  (UI16_T*)bp;
    bp += WEIGHT_SIZE;                      dp->queue = (L_CIRLST_List_T*) bp;
    bp += Q_SIZE;                           dp->wcnter =  (UI16_T*)bp;

    dp->last_qno = (UI16_T) (nbr_of_queue - 1);
    dp->nbr_of_priority = nbr_of_priority;
    dp->nbr_of_queue = nbr_of_queue;
    memcpy (dp->priority_queue_map,  priority_queue_map, PR_TO_Q_MAP_SIZE);

    /* initiate queue */
    {
        I32_T   i;
        BOOL_T  fail=FALSE;

        for ( i=0 ; i<nbr_of_queue ; i++ )
        {
            fail |= ((dp->queue[i] = L_CIRLST_Create ( (UI32_T)queue_length[i], L_MQ_MESSAGE_SIZE)) == NULL);
        }
        if ( fail == TRUE )
        {
            /* print fail */
            //printf("initiate queue fail...\n");
        }
    } /* end of queue initialization */

    memcpy (dp->schedule_weight, schedule_weight, WEIGHT_SIZE);
    RestoreWFQ_Counter(dp);
    dp->total_queue_length = total_queue_length;
    dp->msg_nbr_in_q = 0;
    dp->disable_deq_bmp = 0;

    return (L_MQ_Desc_T)dp;
} /* L_MQ_Create */



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
BOOL_T L_MQ_Enq (L_MQ_Desc_T desc, UI32_T  msg[], UI32_T priority)
{
    InDesc_T    *id_p = (InDesc_T*) desc;
    UI16_T      qno;

    if ( (id_p == NULL) || (msg == NULL))
    {
        return FALSE;
    }
    if (id_p->msg_nbr_in_q >= id_p->total_queue_length )
    {
        return FALSE;
    }

    /* trasfer priority to queue */
    if ( priority >= id_p->nbr_of_priority )
        return FALSE;
    qno = id_p->priority_queue_map [priority];
    if ( L_CIRLST_Enq ( id_p->queue[qno], msg) == TRUE )
    {
        id_p->msg_nbr_in_q++;
        return TRUE;
    }
    return FALSE;
} /* L_MQ_Enq */



/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_Deq
 * ---------------------------------------------------------------------
 * PURPOSE  : Get from a priority queue.
 * INPUT    : desc          -- descriptor of priority queue
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

#if 0
Algorithm_of_Deq
{
    /* 1. DEQ process */
    while( msg_nbr_in_q > 0 )
    {
        Is_Idle = TRUE;
        for ( i=0 ; i<nbr_of_queue ; i++ )
        {
            last_qno = id_p->last_qno; /* get back last queue no */
            switch (wcnter[last_qno])
            {
                case INFINITE_NBR: /* strict */
                    if ( DEQ(last_qno) == FALSE )
                    {
                        ADV;
                        continue;

                    }
                    PROCESS, REWIND;
                    return;

                case 0:
                   ADV;
                   break;

                case (>0):
                    if ( DEQ(last_qno) == FALSE )
                    {
                       ADV;
                       break;
                    }

                    PROCESS;
                    wcnter[last_qno] --;
                    ADV;
                    return;
            } /* end of switch */
        } /* end of for */
        if( Is_Idle == TRUE )
            RESTORE WORKING COUNTER;
    }/* end of while */
}
#endif

BOOL_T L_MQ_Deq (L_MQ_Desc_T desc, UI32_T msg[], UI16_T *qno)
{
    InDesc_T    *id_p = (InDesc_T*) desc;
    UI32_T      *msg_p;
    UI16_T      last_qno;
    I32_T       i;
    UI32_T      disable_deq_bmp;
    UI32_T      times;

    if ( (id_p == NULL) || (msg == NULL) )
    {
        return FALSE;
    }

    if ( id_p->msg_nbr_in_q == 0 )
    {
        return FALSE;
    }

    disable_deq_bmp = id_p->disable_deq_bmp;
    times = 0;

    /* DEQ process */
    while( id_p->msg_nbr_in_q > 0 )
    {
        for ( i=id_p->nbr_of_queue-1 ; i>=0 ; i-- )
        {
            last_qno = id_p->last_qno;

            if(disable_deq_bmp & (1 << last_qno))
            {
                ADV(id_p);
                continue;
            }

            switch ( id_p->wcnter[last_qno] ) {
            case INFINITE_NBR: /* strict */
                if ( L_CIRLST_Deq (id_p->queue[last_qno], (void **)(void *)&msg_p) == FALSE )
                {
                    ADV(id_p);
                    continue;
                }
                *qno = last_qno; /* dequeue from which priority queue */
                id_p->last_qno = (UI16_T)(id_p->nbr_of_queue-1);      /* REWIND; */
                memcpy (msg, msg_p, L_MQ_MESSAGE_SIZE);
                id_p->msg_nbr_in_q--;  /* decrease msg number in the mq */
                return TRUE;

            case 0:
                ADV(id_p);
                break;

            default:    /* > 0 */
                if ( L_CIRLST_Deq (id_p->queue[last_qno], (void **)(void *)&msg_p) == FALSE )
                {
                    ADV(id_p);
                    break;
                }
                *qno = last_qno; /* dequeue from which priority queue */
                memcpy (msg, msg_p, L_MQ_MESSAGE_SIZE);
                id_p->wcnter[last_qno]--; /* decrease working counter for this queue */
                id_p->msg_nbr_in_q--;  /* decrease msg number in the mq */
                ADV(id_p);
                return TRUE;

            } /* end of switch */
        } /* end of for */

        id_p->last_qno = id_p->nbr_of_queue-1;
        RestoreWFQ_Counter (id_p);

        times++;
        if(2 == times)
            break;
    } /* end of while */
    return FALSE;
} /* L_MQ_Deq */

BOOL_T L_MQ_EnableDeq(L_MQ_Desc_T desc, UI32_T queue_bmp)
{
    InDesc_T    *id_p = (InDesc_T*) desc;

    if (NULL == id_p)
        return FALSE;

    id_p->disable_deq_bmp &= ~queue_bmp;

    return TRUE;
}

BOOL_T L_MQ_DisableDeq(L_MQ_Desc_T desc, UI32_T queue_bmp)
{
    InDesc_T    *id_p = (InDesc_T*) desc;

    if (NULL == id_p)
        return FALSE;

    id_p->disable_deq_bmp |= queue_bmp;

    return TRUE;
}

BOOL_T L_MQ_GetDisableDeqBmp(L_MQ_Desc_T desc, UI32_T *queue_bmp)
{
    InDesc_T    *id_p = (InDesc_T*) desc;

    if (NULL == id_p || NULL == queue_bmp)
        return FALSE;

    *queue_bmp = id_p->disable_deq_bmp;

    return TRUE;
} /* L_MQ_Deq */

static void RestoreWFQ_Counter (InDesc_T *id_p)
{
    I32_T i;

    for ( i=0 ; i< id_p->nbr_of_queue ; i++ )
    {
        id_p->wcnter[i] = (UI16_T) ( (id_p->schedule_weight[i]==0)?
        INFINITE_NBR: id_p->schedule_weight[i]);
    }
} /* RestoreWFQ_Counter */


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_Clean
 * ---------------------------------------------------------------------
 * PURPOSE  : Clean all records in the priority queue.
 * INPUT    : desc          -- priority queue to operate
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_MQ_Clean (L_MQ_Desc_T desc)
{
    I32_T i;

    InDesc_T *id_p = (InDesc_T*) desc;

    if( id_p == NULL )
    {
        return;
    }

    /* clean inner CIRLST queues */
    for ( i=0 ; i< id_p->nbr_of_queue ; i++ )
    {
         L_CIRLST_Clean( id_p->queue[i] );
    }

    id_p->msg_nbr_in_q = 0;
    RestoreWFQ_Counter( id_p );
    id_p->last_qno = (UI16_T) (id_p->nbr_of_queue - 1);

} /* L_MQ_Clean */

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_Destroy
 * ---------------------------------------------------------------------
 * PURPOSE  : Destory a priority queue.
 * INPUT    : desc          -- priority queue to operate
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_MQ_Destroy (L_MQ_Desc_T desc)
{
    I32_T i;

    InDesc_T *id_p = (InDesc_T*) desc;

    if( id_p == NULL )
    {
        return;
    }

    /* free inner CIRLST queues */
    for ( i=0 ; i< id_p->nbr_of_queue ; i++ )
    {
         L_CIRLST_Destroy( id_p->queue[i] );
    }
    /* free MQ */
    free( (void*) desc );

} /* L_MQ_Destroy */

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_GetTailPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the queue tail pointer in priority queue.
 * INPUT    : desc     -- descriptor of priority queue
 *            qno      -- no of queue in priority queue
 * OUTPUT   : none
 * RETURN   : pointer of the tail
 * NOTE     : none
 */
UI8_T *L_MQ_GetTailPointer (L_MQ_Desc_T desc, UI16_T qno)
{
    InDesc_T *id_p = (InDesc_T*)desc;

    return L_CIRLST_GetTailPointer(id_p->queue[qno]);
}

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_GetHeadPointer
 * ---------------------------------------------------------------------
 * PURPOSE  : get the queue head pointer in priority queue.
 * INPUT    : desc     -- descriptor of priority queue
 *            qno      -- no of queue in priority queue
 * OUTPUT   : none
 * RETURN   : pointer of the head
 * NOTE     : none
 */
UI8_T *L_MQ_GetHeadPointer (L_MQ_Desc_T desc, UI16_T qno)
{
    InDesc_T *id_p = (InDesc_T*)desc;

    return L_CIRLST_GetHeadPointer(id_p->queue[qno]);
}

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
UI32_T L_MQ_DoOperationAndMoveHead(L_MQ_Desc_T desc, UI16_T qno, UI8_T *endp, void *func)
{
    InDesc_T *id_p = (InDesc_T*)desc;
	UI32_T    ret = 0;

    ret = L_CIRLST_DoOperationAndMoveHead(id_p->queue[qno], endp, func, (UI32_T)qno);

    /* On Linux, SYSFUN_InterruptLock() isn't provided in user mode.
     */
    /* prev_mask = SYSFUN_InterruptLock(); */
    id_p->msg_nbr_in_q -= ret;
    /* SYSFUN_InterruptUnlock(prev_mask); */

    return ret;
}



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
    const UI16_T queue_length [] )
{
    UI32_T i, queue_buffer_required_size = 0;

    for (i = 0; i < nbr_of_queue; i++)
        queue_buffer_required_size += L_CIRLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(queue_length[i], L_MQ_MESSAGE_SIZE);

    return (SHMEM_PR_TO_Q_MAP_SIZE +
            SHMEM_WEIGHT_SIZE      +
            SHMEM_CNT_SIZE         +
            SHMEM_Q_SIZE           +
            queue_buffer_required_size);
}


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_ShMem_Create
 * ---------------------------------------------------------------------
 * PURPOSE  : Create a priority queue.
 * INPUT    : working_buffer     -- the buffer for the queue in shared memory.
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
 */
BOOL_T L_MQ_ShMem_Create (
    L_MQ_ShMem_Desc_T *desc_p,
    void *working_buffer,
    UI16_T nbr_of_priority,
    UI16_T nbr_of_queue,
    UI32_T total_queue_length,
    const UI16_T priority_queue_map[],
    const UI16_T schedule_weight [],
    const UI16_T queue_length [] )
{
    /* LOCAL VARIABLES */
    I32_T          queue_buffer_offset;
    I32_T          offset;
    I32_T          i;

    /* BODY
     */

    if( ( desc_p == NULL ) || ( working_buffer == NULL) ||
        ( nbr_of_priority ==0 ) || ( total_queue_length == 0 ) ||
        ( priority_queue_map == NULL) || ( schedule_weight == NULL ) ||
        ( queue_length == NULL ) )
    {
        return FALSE;
    }
    /* check Priority_To_Queue_Mapping */
    for ( i=0 ; i<nbr_of_priority ; i++ )
    {
        if ( priority_queue_map[i] >= nbr_of_queue )
            return FALSE;
    }

    /* Check Schedule_Weight */
    {
        int now_is_one = 0, next_is_one;

        for ( i=nbr_of_queue-1; i>=0 ; i-- )
        {
            /* caculate next_is_one */
            next_is_one = (schedule_weight[i]!=0);

            /*  !next_is_one == next_is_zero */
            if ( now_is_one &&  (!next_is_one) )
            {
                return FALSE;
            }
            now_is_one |= next_is_one;
        }
    } /* end of Check_Schedule_Weight */

    offset = L_CVRT_GET_OFFSET(desc_p, working_buffer);
                                      desc_p->priority_queue_map_offset = offset;
    offset += SHMEM_PR_TO_Q_MAP_SIZE; desc_p->schedule_weight_offset = offset;
    offset += SHMEM_WEIGHT_SIZE;      desc_p->wcnter_offset = offset;
    offset += SHMEM_CNT_SIZE;         desc_p->queue_offset = offset;
    offset += SHMEM_Q_SIZE;           queue_buffer_offset = offset;

    desc_p->last_qno = (UI16_T) (nbr_of_queue - 1);
    desc_p->nbr_of_priority = nbr_of_priority;
    desc_p->nbr_of_queue = nbr_of_queue;

    memcpy (SHMEM_PR_TO_Q_MAP(desc_p),  priority_queue_map, SHMEM_PR_TO_Q_MAP_SIZE);

    /* initiate queue */
    {
        L_CIRLST_ShMem_List_T *list_p = SHMEM_Q(desc_p, 0);
        I32_T   i;
        BOOL_T  fail=FALSE;

        for ( i=0 ; i<nbr_of_queue ; i++ )
        {
            fail |= (L_CIRLST_ShMem_Create (list_p, L_CVRT_GET_PTR(desc_p, queue_buffer_offset), (UI32_T)queue_length[i], L_MQ_MESSAGE_SIZE) == NULL);
            list_p++;
            queue_buffer_offset += L_CIRLST_SHMEM_GET_WORKING_BUFFER_REQUIRED_SZ(queue_length[i], L_MQ_MESSAGE_SIZE);
        }
        if ( fail == TRUE )
        {
            /* print fail */
            //printf("initiate queue fail...\n");
        }
    } /* end of queue initialization */

    memcpy (SHMEM_WEIGHT(desc_p), schedule_weight, SHMEM_WEIGHT_SIZE);
    L_MQ_ShMem_RestoreWFQ_Counter(desc_p);
    desc_p->total_queue_length = total_queue_length;
    desc_p->msg_nbr_in_q = 0;
    return TRUE;
} /* L_MQ_ShMem_Create */



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
BOOL_T L_MQ_ShMem_Enq (L_MQ_ShMem_Desc_T *desc_p, UI32_T  msg[], UI32_T priority)
{
    UI16_T      qno;

    if ( (desc_p == NULL) || (msg == NULL))
    {
        return FALSE;
    }
    if (desc_p->msg_nbr_in_q >= desc_p->total_queue_length )
    {
        return FALSE;
    }

    /* trasfer priority to queue */
    if ( priority >= desc_p->nbr_of_priority )
        return FALSE;
    qno = SHMEM_PR_TO_Q_MAP(desc_p)[priority];
    if ( L_CIRLST_ShMem_Enq ( SHMEM_Q(desc_p, qno), msg) == TRUE )
    {
        desc_p->msg_nbr_in_q++;
        return TRUE;
    }
    return FALSE;
} /* L_MQ_ShMem_Enq */



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
BOOL_T L_MQ_ShMem_Deq (L_MQ_ShMem_Desc_T *desc_p, UI32_T msg[], UI16_T *qno)
{
    UI32_T      *msg_p;
    UI16_T      last_qno;
    I32_T       i;

    if ( (desc_p == NULL) || (msg == NULL) )
    {
        return FALSE;
    }

    if ( desc_p->msg_nbr_in_q == 0 )
    {
        return FALSE;
    }
    /* DEQ process */
    while( desc_p->msg_nbr_in_q > 0 )
    {
        for ( i=desc_p->nbr_of_queue-1 ; i>=0 ; i-- )
        {
            last_qno = desc_p->last_qno;
            switch ( SHMEM_CNT(desc_p)[last_qno] ) {
            case INFINITE_NBR: /* strict */
                if ( L_CIRLST_ShMem_Deq (SHMEM_Q(desc_p, last_qno), (void **)(void *)&msg_p) == FALSE )
                {
                    ADV(desc_p);
                    continue;
                }
                *qno = last_qno; /* dequeue from which priority queue */
                desc_p->last_qno = (UI16_T)(desc_p->nbr_of_queue-1);      /* REWIND; */
                memcpy (msg, msg_p, L_MQ_MESSAGE_SIZE);
                desc_p->msg_nbr_in_q--;  /* decrease msg number in the mq */
                return TRUE;

            case 0:
                ADV(desc_p);
                break;

            default:    /* > 0 */
                if ( L_CIRLST_ShMem_Deq (SHMEM_Q(desc_p, last_qno), (void **)(void *)&msg_p) == FALSE )
                {
                    ADV(desc_p);
                    break;
                }
                *qno = last_qno; /* dequeue from which priority queue */
                memcpy (msg, msg_p, L_MQ_MESSAGE_SIZE);
                SHMEM_CNT(desc_p)[last_qno]--; /* decrease working counter for this queue */
                desc_p->msg_nbr_in_q--;  /* decrease msg number in the mq */
                ADV(desc_p);
                return TRUE;

            } /* end of switch */
        } /* end of for */

        desc_p->last_qno = desc_p->nbr_of_queue-1;
        L_MQ_ShMem_RestoreWFQ_Counter (desc_p);
    } /* end of while */
    return FALSE;
} /* L_MQ_ShMem_Deq */

static void L_MQ_ShMem_RestoreWFQ_Counter (L_MQ_ShMem_Desc_T *desc_p)
{
    I32_T i;
    for ( i=0 ; i< desc_p->nbr_of_queue ; i++ )
    {
        SHMEM_CNT(desc_p)[i] = (UI16_T) ( (SHMEM_WEIGHT(desc_p)[i]==0)?
        INFINITE_NBR: SHMEM_WEIGHT(desc_p)[i]);
    }
} /* L_MQ_ShMem_RestoreWFQ_Counter */


/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_ShMem_Clean
 * ---------------------------------------------------------------------
 * PURPOSE  : Clean all records in the priority queue.
 * INPUT    : desc_p        -- priority queue to operate
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_MQ_ShMem_Clean (L_MQ_ShMem_Desc_T *desc_p)
{
    I32_T i;

    if( desc_p == NULL )
    {
        return;
    }

    /* clean inner CIRLST queues */
    for ( i=0 ; i< desc_p->nbr_of_queue ; i++ )
    {
         L_CIRLST_ShMem_Clean( SHMEM_Q(desc_p, i) );
    }

    desc_p->msg_nbr_in_q = 0;
    L_MQ_ShMem_RestoreWFQ_Counter( desc_p );
    desc_p->last_qno = (UI16_T) (desc_p->nbr_of_queue - 1);

} /* L_MQ_ShMem_Clean */

/* ---------------------------------------------------------------------
 * FUNCTION NAME - L_MQ_ShMem_Destroy
 * ---------------------------------------------------------------------
 * PURPOSE  : Destory a priority queue.
 * INPUT    : desc_p        -- priority queue to operate
 * OUTPUT   : none
 * RETURN   : none
 * NOTE     : none
 */
void L_MQ_ShMem_Destroy (L_MQ_ShMem_Desc_T *desc_p)
{
    I32_T i;

    if( desc_p == NULL )
    {
        return;
    }

    /* free inner CIRLST queues */
    for ( i=0 ; i< desc_p->nbr_of_queue ; i++ )
    {
         L_CIRLST_ShMem_Destroy( SHMEM_Q(desc_p, i) );
    }

} /* L_MQ_ShMem_Destroy */

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
UI8_T *L_MQ_ShMem_GetTailPointer (L_MQ_ShMem_Desc_T *desc_p, UI16_T qno)
{
    return L_CIRLST_ShMem_GetTailPointer(SHMEM_Q(desc_p, qno));
}

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
UI8_T *L_MQ_ShMem_GetHeadPointer (L_MQ_ShMem_Desc_T *desc_p, UI16_T qno)
{
    return L_CIRLST_ShMem_GetHeadPointer(SHMEM_Q(desc_p, qno));
}

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
UI32_T L_MQ_ShMem_DoOperationAndMoveHead(L_MQ_ShMem_Desc_T *desc_p, UI16_T qno, UI8_T *endp, void *func)
{
	UI32_T    ret = 0;

    ret = L_CIRLST_ShMem_DoOperationAndMoveHead(SHMEM_Q(desc_p, qno), endp, func, (UI32_T)qno);

    desc_p->msg_nbr_in_q -= ret;

    return ret;
}

#ifdef L_MQ_UNIT_TEST
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "l_mq.h"
#include "sysfun.h"

#define L_MQ_UNIT_TEST_SHMEM_KEY      10
#define L_MQ_UNIT_TEST_BUFFER_SIZE    1024
#define L_MQ_UNIT_TEST_MAGIC_FLAG     0x24575678

#define L_MQ_UNIT_TEST_NBR_OF_PRIORITY 4
#define L_MQ_UNIT_TEST_NBR_OF_QUEUE    4

static UI16_T test_priority_queue_map[] = { 0, 1, 2, 3 };
static UI16_T test_schedule_weight[] = { 4, 3, 2, 1 };
static UI16_T test_queue_length[] = { 10, 10, 10, 10 };

int L_MQ_ShMem_main()
{
    UI32_T shm_id;
    UI32_T *shm_p;
    L_MQ_ShMem_Desc_T *desc_p;
    int i;

    srand(time(NULL));

    assert(SYSFUN_CreateShMem(L_MQ_UNIT_TEST_SHMEM_KEY, L_MQ_UNIT_TEST_BUFFER_SIZE, &shm_id) == SYSFUN_OK);

    assert(shm_p = SYSFUN_AttachShMem(shm_id));
    desc_p = (L_MQ_ShMem_Desc_T *)(shm_p + 1);

    if (*shm_p != L_MQ_UNIT_TEST_MAGIC_FLAG)
    {
        UI32_T total_queue_length = 0;

        for (i = 0; i < L_MQ_UNIT_TEST_NBR_OF_QUEUE; i++)
            total_queue_length += test_queue_length[i];

        assert(sizeof(UI32_T) + sizeof(L_MQ_ShMem_Desc_T) + L_MQ_ShMem_GetWorkingBufferRequiredSize(L_MQ_UNIT_TEST_NBR_OF_PRIORITY, L_MQ_UNIT_TEST_NBR_OF_QUEUE, test_queue_length) <= L_MQ_UNIT_TEST_BUFFER_SIZE);
        assert(L_MQ_ShMem_Create(
                        desc_p,
                        (char *)desc_p + sizeof(*desc_p),
                        L_MQ_UNIT_TEST_NBR_OF_PRIORITY,
                        L_MQ_UNIT_TEST_NBR_OF_QUEUE,
                        total_queue_length,
                        test_priority_queue_map,
                        test_schedule_weight,
                        test_queue_length));
        *shm_p = L_MQ_UNIT_TEST_MAGIC_FLAG;
    }

    for (i = 0; i < L_MQ_UNIT_TEST_NBR_OF_QUEUE; i++)
    {
        printf("head %08x\ttail %08x\n",
            (unsigned int)L_MQ_ShMem_GetHeadPointer(desc_p, i),
            (unsigned int)L_MQ_ShMem_GetTailPointer(desc_p, i));
    }

    for (i = 0; i < 10; i++)
    {
        char   msg[L_MQ_MESSAGE_SIZE];
        UI16_T qno;
        UI32_T priority;

        switch (rand() % 10)
        {
            case 0:
                L_MQ_ShMem_Clean(desc_p);
                printf("clean\t");
                break;

            case 1:
            case 2:
            case 3:
            case 4:
                if (L_MQ_ShMem_Deq(desc_p, (UI32_T *)msg, &qno))
                {
                    printf("deq from %d", (int)qno);
                    DBG_DumpHex("", sizeof(msg), msg);
                }
                else printf("empty\n");
                break;

            default:
                priority = rand() % L_MQ_UNIT_TEST_NBR_OF_QUEUE;
                memmove(msg, msg+1, sizeof(msg)-1);
                if (L_MQ_ShMem_Enq(desc_p, (UI32_T *)msg, priority))
                {
                    printf("enq pri %d", (int)priority);
                    DBG_DumpHex("", sizeof(msg), msg);
                }
                else printf("full\n");
        }
    }

    printf("\n");

    SYSFUN_DetachShMem(desc_p);

    return 0;
}
#endif /* L_MQ_UNIT_TEST */


