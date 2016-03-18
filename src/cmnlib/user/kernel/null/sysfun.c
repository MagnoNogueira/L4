//#include "config.h"

#include <assert.h>
#include <memory.h>
#include <unistd.h>
#include "sys_bld.h"
#include "sysfun.h"

#ifndef _countof
#define _countof(ar)    (sizeof(ar)/sizeof(*ar))
#endif

/* MACRO FUNCTION NAME : SYSFUN_LOGMSG
 * PURPOSE:
 *         Macro used by SYSFUN_LogXXX APIs.
 *
 * INPUT:
 *      log_level  -  log level, valid settings are listed below:
 *                    SYSFUN_LOGLEVEL_URGENT
 *                    SYSFUN_LOGLEVEL_ALERT
 *                    SYSFUN_LOGLEVEL_CRITICAL
 *                    SYSFUN_LOGLEVEL_ERR
 *                    SYSFUN_LOGLEVEL_WARN
 *                    SYSFUN_LOGLEVEL_NOTICE
 *                    SYSFUN_LOGLEVEL_INFO
 *                    SYSFUN_LOGLEVEL_DEBUG
 *      msg        -  message to be logged
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      None.
 */
#define SYSFUN_LOGMSG(log_level, msg, ...)  printf(msg, ##__VA_ARGS__)


#define SYSFUN_MAX_MSGQ_SIZE    10
typedef struct
{
    UI32_T  key;
    UI32_T  type;
    BOOL_T  (*handler_fn)(SYSFUN_Msg_T*);
} SYSFUN_MsgQ;

typedef struct
{
    SYSFUN_MsgQ     msgq;
    UI32_T          next;
    BOOL_T          used;
} SYSFUN_MsgQDS;

typedef struct
{
    SYSFUN_MsgQDS   msgq_array[ SYSFUN_MAX_MSGQ_SIZE ];
    UI32_T          msgq_free_list;
} SYSFUN_MsgQList;

#define SYSFUN_MAX_SEM_SIZE     128
typedef struct
{
    UI32_T  key;    // key
    UI32_T  count;
    UI32_T  flag;
    BOOL_T  valid;
} SYSFUN_Sem;

#define SYSFUN_MAX_SHMEM_SIZE   16
typedef struct
{
    UI32_T  key;    // key
    UI32_T  size;
    UI32_T  count;
    void    *ptr;
    BOOL_T  valid;
} SYSFUN_Shmem;

SYSFUN_MsgQList sysfun_msgq_list;
SYSFUN_Sem      sysfun_sem_array[SYSFUN_MAX_SEM_SIZE];
SYSFUN_Shmem    sysfun_shmem_array[SYSFUN_MAX_SHMEM_SIZE];

void SYSFUN_Init()
{
    UI32_T i;

    for (i=0; i < _countof(sysfun_msgq_list.msgq_array)-1; ++i)
    {
        sysfun_msgq_list.msgq_array[i].next = i+1;
    }

    sysfun_msgq_list.msgq_array[i].next = 0xffffffff;
    sysfun_msgq_list.msgq_free_list = 0;
}

SYSFUN_MsgQ * SYSFUN_L_AllocateMessageQueue()
{
    SYSFUN_MsgQDS *msgqds;

    if (sysfun_msgq_list.msgq_free_list == 0xffffffff)
    {
        printf("No message queue");
        return NULL;
    }

    msgqds = sysfun_msgq_list.msgq_array + sysfun_msgq_list.msgq_free_list;
    sysfun_msgq_list.msgq_free_list = msgqds->next;

    msgqds->used = TRUE;

    memset(&msgqds->msgq, 0, sizeof(msgqds->msgq));

    return &msgqds->msgq;
}

void SYSFUN_L_DestroyMessageQueue(SYSFUN_MsgQ *msgq)
{
    SYSFUN_MsgQDS *msgqds = (SYSFUN_MsgQDS *)msgq;

    msgqds->next = sysfun_msgq_list.msgq_free_list;
    sysfun_msgq_list.msgq_free_list = msgqds - sysfun_msgq_list.msgq_array;

    msgqds->used = FALSE;

    memset(&msgqds->msgq, 0xcc, sizeof(msgqds->msgq));
}

SYSFUN_MsgQ * SYSFUN_L_SearchMessageQueue(UI32_T key)
{
    UI32_T i;

    if (SYSFUN_MSGQKEY_PRIVATE == key)
    {
        return NULL;
    }

    for (i=0; i < _countof(sysfun_msgq_list.msgq_array); ++i)
    {
        if (FALSE == sysfun_msgq_list.msgq_array[i].used)
        {
            continue;
        }

        if (sysfun_msgq_list.msgq_array[i].msgq.key == key)
        {
            return &sysfun_msgq_list.msgq_array[i].msgq;
        }
    }

    return NULL;
}

UI32_T SYSFUN_L_ConvertMessageQueueKeyToId(UI32_T key)
{
    SYSFUN_MsgQDS *msgqds = (SYSFUN_MsgQDS *) SYSFUN_L_SearchMessageQueue(key);

    if (NULL == msgqds)
    {
        return 0xffffffff;
    }

    return msgqds - sysfun_msgq_list.msgq_array;
}

SYSFUN_MsgQ * SYSFUN_L_GetMessageQueueAtId(UI32_T msgq_id)
{
    if (_countof(sysfun_msgq_list.msgq_array) < msgq_id)
    {
        return NULL;
    }

    if (NULL == sysfun_msgq_list.msgq_array[msgq_id].used)
    {
        return NULL;
    }

    return &sysfun_msgq_list.msgq_array[msgq_id].msgq;
}

UI32_T SYSFUN_L_MessageQueuePtrToId(SYSFUN_MsgQ *msgq)
{
    return (SYSFUN_MsgQDS *) msgq - sysfun_msgq_list.msgq_array;
}

/* FUNCTION NAME : DBG_DumpHex
 * PURPOSE:
 *      Dump heximal code on screen, this function should be used in debug mode only.
 * INPUT:
 *      title   -- text displays on screen.
 *      len     -- data length to be displayed.
 *      buffer  -- buffer holding displaying data.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. Only workable in debug mode, and do nothing for release version.
 */
void    DBG_DumpHex (char *title, int len, char *buf)
{
#ifdef SYSFUN_DEBUG
    int     idx=0, i;
    char    hex_buf[69];

    if (title != NULL)
        printf ("%s", title);
    printf ("\r\n");
    memset(hex_buf, 0x20, 69);
    for (i=0; i<len; i++,idx++)
    {
        idx %= 16;
        SYSFUN_Val2HexStr (buf[i], &(hex_buf[idx*3]));
        hex_buf[50+idx] = SYSFUN_VisibleCode (buf[i]);
        if (((i+1)%16)==0)
        {
            hex_buf[66] = 0x0d;
            hex_buf[67] = 0x0a;
            hex_buf[68] = 0;
            printf ("%s", hex_buf);
            memset(hex_buf, 0x20, 69);
        }
    }
    if (i%16)
    {
        hex_buf[66] = 0x0d;
        hex_buf[67] = 0x0a;
        hex_buf[68] = 0;
        printf ("%s", hex_buf);
    }
    fflush(stdout);
#endif /* SYSFUN_DEBUG */
}   /*  end of DBG_DumpHex  */

/* FUNCTION NAME : SYSFUN_TaskIdSelf
 * PURPOSE:
 *      get the task ID of a running task.
 *
 * INPUT:
 *      None
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      The task ID of the calling task.
 *
 * NOTES:
 *      1. The task ID will be invalid if called at interrupt level.
 */
UI32_T SYSFUN_TaskIdSelf(void)
{
    return 999;
}

/*------------------------------------------------------------------------------
 * Semaphore Function
 *------------------------------------------------------------------------------
 */

static SYSFUN_Sem * SYSFUN_L_GetSem(UI32_T sem_id)
{
    SYSFUN_Sem * sem;

    if (_countof(sysfun_sem_array) <= sem_id)
    {
        return NULL;
    }

    sem = &sysfun_sem_array[sem_id];

    if (FALSE == sem->valid)
    {
        return NULL;
    }

    return sem;
}

/* FUNCTION NAME : SYSFUN_CreateSem
 * PURPOSE:
 *      Create a binary semaphore.
 *
 * INPUT:
 *      sem_key     -- key for the semaphore.
 *                     If this value is set as SYSFUN_SEMKEY_PRIVATE, the created semaphore is
 *                     only valid in the calling process.
 *                     If this valie is set as value other than SYSFUN_SEMKEY_PRIVATE, the
 *                     created semaphore is valid among all processes.
 *      sem_count   -- 0:Empty, 1:Full.
 *      sem_flag    -- fifo or priority
 *                     SYSFUN_SEM_FIFO : FIFO semaphore;
 *                     SYSFUN_SEM_PRIORITY : Priority semaphore;
 *                                           high priority task will get the semaphore first.
 *
 * OUTPUT:
 *      sem_id      -- created semaphore id.
 *
 * RETURN:
 *      SYSFUN_OK   -- Successfully.
 *      SYSFUN_RESULT_INVALID_ARG   -- Invalid argument(s)
 *      SYSFUN_RESULT_NO_SEMAPHORE  -- Exceeds max number of semaphore.
 *      SYSFUN_RESULT_ERROR         -- Error
 *
 * NOTES:
 *      1. The task ID will be invalid if called at interrupt level.
 *      2. On linux, sem_flag won't take effect.
 *      3. If a semaphore already exists for the key, creation will be failed.
 */
UI32_T SYSFUN_CreateSem (UI32_T sem_key, UI32_T sem_count, UI32_T sem_flag, UI32_T *sem_id)
{
#define SYSFUN_INVALID_ID 0xffffffff

    UI32_T i;
    UI32_T new_id = SYSFUN_INVALID_ID;
    SYSFUN_Sem  *sem;

    if (0 == sem_key)
    {
        return SYSFUN_RESULT_INVALID_ARG;
    }

    for (i=0; i < _countof(sysfun_sem_array); ++i)
    {
        sem = &sysfun_sem_array[i];

        if (TRUE == sem->valid)
        {
            if (sem->key == sem_key)
            {
                if (sem->flag != sem_flag)
                {
                    return SYSFUN_RESULT_ERROR;
                }

                *sem_id = i;
                return SYSFUN_OK;
            }
        }
        else
        {
            if (SYSFUN_INVALID_ID == new_id)
            {
                new_id = i;
            }
        }
    }

    if (SYSFUN_INVALID_ID == new_id)
    {
        return SYSFUN_RESULT_NO_SEMAPHORE;
    }

    sem = &sysfun_sem_array[new_id];

    sem->valid = TRUE;
    sem->key = sem_key;
    sem->count = (sem_count > 0) ? 1 : 0;
    sem->flag = sem_flag;

    *sem_id = new_id;

    return SYSFUN_OK;

#undef SYSFUN_INVALID_ID
}

/* FUNCTION NAME : SYSFUN_GetSem
 * PURPOSE:
 *      Get an existed binary semaphore.
 *
 * INPUT:
 *      sem_key     -- key for the semaphore; must be > 0.
 *                     SYSFUN_SEMKEY_PRIVATE is invalid.
 *
 * OUTPUT:
 *      sem_id      -- created semaphore id.
 *
 * RETURN:
 *      SYSFUN_OK   -- Successfully.
 *      SYSFUN_RESULT_NO_SEMAPHORE  -- No semaphore exists for the key.
 *      SYSFUN_RESULT_ERROR         -- Error
 *
 * NOTES:
 *      None.
 */
UI32_T SYSFUN_GetSem (UI32_T sem_key, UI32_T *sem_id)
{
    UI32_T i;

    if (0 == sem_key)
    {
        return SYSFUN_RESULT_INVALID_ARG;
    }

    for (i=0; i < _countof(sysfun_sem_array); ++i)
    {
        SYSFUN_Sem *sem = &sysfun_sem_array[i];

        if (TRUE == sem->valid)
        {
            if (sem->key == sem_key)
            {
                *sem_id = i;
                return SYSFUN_OK;
            }
        }
    }

    return SYSFUN_RESULT_NO_SEMAPHORE;
}

/* FUNCTION NAME : SYSFUN_DestroySem
 * PURPOSE:
 *      Destroy a created semaphore.
 *
 * INPUT:
 *      sem_id      -- semaphore ID to destroy.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      SYSFUN_OK    -- Successfully.
 *      SYSFUN_RESULT_INVALID_ID    -- Invalid sem ID.
 *
 * NOTES:
 *      None.
 */
UI32_T  SYSFUN_DestroySem(UI32_T sem_id)
{
    SYSFUN_Sem  *sem = SYSFUN_L_GetSem(sem_id);

    if (NULL == sem)
    {
        return SYSFUN_RESULT_INVALID_ID;
    }

    memset(sem, 0, sizeof(*sem));
    sem->valid = FALSE;

    return SYSFUN_OK;
}

/* FUNCTION NAME : SYSFUN_TakeSem
 * PURPOSE:
 *      Wait for semaphore. (Take or P operation)
 *
 * INPUT:
 *      sem_id      -- semaphore ID to wait.
 *      timeout     -- waiting time;
 *                     0x00 : No wait.
 *                     SYSFUN_TIMEOUT_WAIT_FOREVER : Wait Forever
 *                     other: waiting time.
 *
 * OUTPUT:  none
 *
 * RETURN:
 *      SYSFUN_OK    -- Successfully.
 *      SYSFUN_RESULT_TIMEOUT    -- Timeout.
 *      SYSFUN_RESULT_OBJ_DELETED    -- Semaphore been deleted.
 *      SYSFUN_RESULT_INVALID_ID    -- Invalid sem ID.
 *      SYSFUN_RESULT_SEM_NOSIGNAL    -- No semaphore be signal when No_Wait is called.
 *      SYSFUN_RESULT_SEM_NOSIGNAL    -- Semaphore be deleted when waiting semaphore.
 *      SYSFUN_RESULT_ERROR           -- Error
 *
 * NOTES:
 *      1. This function MUST NOT BE called by an ISR.
 *
 */
UI32_T __SYSFUN_TakeSem (UI32_T sem_id, int timeout, const char *function, int line)
{
    SYSFUN_Sem  *sem = SYSFUN_L_GetSem(sem_id);

    if (NULL == sem)
    {
        return SYSFUN_RESULT_INVALID_ID;
    }

//printf("%s: %s:%d\n", __FUNCTION__, function, line);

    //assert(sem->count == 1);
    if (!(sem->count == 1))
    {
        printf("%s:%d sem->count != 1", function, line);
        abort();
    }

    -- sem->count;

    return SYSFUN_OK;
}

/* FUNCTION NAME : SYSFUN_GiveSem
 * PURPOSE:
 *      Signal for semaphore. (Give or V operation)
 *
 * INPUT:
 *      sem_id      -- semaphore ID to signal(Give or V).
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      SYSFUN_OK    -- Successfully.
 *      SYSFUN_RESULT_OBJ_DELETED    -- Semaphore been deleted.
 *      SYSFUN_RESULT_INVALID_ID    -- Invalid sem ID.
 *      SYSFUN_RESULT_ERROR         -- Error
 *
 * NOTES:
 *      None.
 */
UI32_T __SYSFUN_GiveSem(UI32_T sem_id, const char *function, int line)
{
    SYSFUN_Sem  *sem = SYSFUN_L_GetSem(sem_id);

    if (NULL == sem)
    {
        return SYSFUN_RESULT_INVALID_ID;
    }

//printf("%s: %s:%d\n", __FUNCTION__, function, line);

    //assert(sem->count == 0);
    if (!(sem->count == 0))
    {
        printf("%s:%d sem->count == 0\n", function, line);
    }

    ++ sem->count;

    return SYSFUN_OK;
}

/*-------------------------
 * Shared Memory Function
 *-------------------------
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME : SYSFUN_CreateShMem
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Create shared memory.
 *
 * INPUT:
 *    key       --  key for shared memory; must be > 0.
 *    size      --  the size of the shared memory to be created
 *
 * OUTPUT:
 *    shm_id_p  --  the id of the created shared memory
 *
 * RETURN:
 *    SYSFUN_OK                       -- Success
 *    SYSFUN_RESULT_INVALID_ARG       -- Invalid parameters.
 *    SYSFUN_RESULT_CANT_CREATE_SHMEM -- Can't create shared memory.
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
UI32_T SYSFUN_CreateShMem(UI32_T key, UI32_T size, UI32_T *shm_id_p)
{
    SYSFUN_Shmem *shmem;
    UI32_T i;

    if (0 == key || 0 == size || NULL == shm_id_p)
    {
        return SYSFUN_RESULT_INVALID_ARG;
    }

    for (i=0; i<_countof(sysfun_shmem_array); ++i)
    {
        if (sysfun_shmem_array[i].valid == TRUE &&
            sysfun_shmem_array[i].key == key)
        {
            break;
        }
    }

    if (i < _countof(sysfun_shmem_array))
    {
        shmem = &sysfun_shmem_array[i];

        if (size <= shmem->size)
        {
            *shm_id_p = i;
            return SYSFUN_OK;
        }

        if (shmem->count)
        {
            return SYSFUN_RESULT_CANT_CREATE_SHMEM;
        }

        /* No one attached, reallocate.
         */

        free(shmem->ptr);

        memset(shmem, 0, sizeof(*shmem));
        shmem->valid = FALSE;
    }

    for (i=0; i<_countof(sysfun_shmem_array); ++i)
    {
        if (sysfun_shmem_array[i].valid == FALSE)
        {
            break;
        }
    }

    if (_countof(sysfun_shmem_array) == i)
    {
        return SYSFUN_RESULT_CANT_CREATE_SHMEM;
    }

    shmem = &sysfun_shmem_array[i];

    shmem->ptr = calloc(size, 1);
    if (NULL == shmem->ptr)
    {
        return SYSFUN_RESULT_CANT_CREATE_SHMEM;
    }

    shmem->key      = key;
    shmem->size     = size;
    shmem->valid    = TRUE;

    *shm_id_p = i;

    return SYSFUN_OK;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : SYSFUN_AttachShMem
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Attach the shared memory to the context of the calling process.
 *
 * INPUT:
 *    shm_id    --  key for shared memory
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Non-NULL  --  The logical address of the shared memory in the context of
 *                  the calling process
 *    NULL -- Error
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
void *SYSFUN_AttachShMem(UI32_T shm_id)
{
    if (_countof(sysfun_shmem_array) <= shm_id)
    {
        return NULL;
    }

    ++ sysfun_shmem_array[shm_id].count;
    return sysfun_shmem_array[shm_id].ptr;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : SYSFUN_DetachShMem
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Detach the shared memory to the context of the calling process.
 *
 * INPUT:
 *    attached_addr --  the attached address returned by SYSFUN_AttachShMem()
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    None.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
void SYSFUN_DetachShMem(void *attached_addr)
{
    SYSFUN_Shmem *shmem = NULL;
    UI32_T i;

    for (i=0; i<_countof(sysfun_shmem_array); ++i)
    {
        if (sysfun_shmem_array[i].valid == TRUE &&
            sysfun_shmem_array[i].ptr == attached_addr)
        {
            shmem = &sysfun_shmem_array[i];
            break;
        }
    }

    if (NULL == shmem || shmem->count == 0)
    {
        return;
    }

    -- shmem->count;
}

/*-------------------------
 * Message Queue Function
 *-------------------------
 */
UI32_T SYSFUN_RegisterIPCHandler(
    UI32_T msgq_handle,
    BOOL_T (*handler_fn) (SYSFUN_Msg_T*))
{
    SYSFUN_MsgQ *msgq = SYSFUN_L_GetMessageQueueAtId(msgq_handle);
    if (NULL == msgq)
    {
        return SYSFUN_RESULT_ERROR;
    }

    msgq->handler_fn = handler_fn;
    return SYSFUN_OK;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - SYSFUN_CreateMsgQ
 * ------------------------------------------------------------------------
 * PURPOSE  :   Get the message queue for the specific key and set the
 *              queue owner to the calling task.
 * INPUT    :   msgq_key      - The key for the message queue; must be > 0.
 *                              If this value is set as SYSFUN_MSGQKEY_PRIVATE,
 *                              the created message queue is only valid in the
 *                              calling process.
 *              msgq_type     - To specify if the queue is bidirectional.
 *                              SYSFUN_MSGQ_UNIDIRECTIONAL
 *                                Unidirectional message queue.
 *                                For sender, send message via SYSFUN_SendRequestMsg().
 *                                For receiver, receive message via SYSFUN_ReceiveMsg().
 *                              SYSFUN_MSGQ_BIDIRECTIONAL
 *                                The queue will be created for bidirectional
 *                                communication and own two queues, one for
 *                                request and another for response.
 *                                For sender, send request via SYSFUN_SendRequestMsg().
 *                                For receiver, receive request via SYSFUN_ReceiveMsg()
 *                                              and send response via SYSFUN_SendResponseMsg().
 * OUTPUT   :   msgq_handle_p - The queue handle.
 * RETURN   :   SYSFUN_OK                       - Successfully.
 *              SYSFUN_RESULT_CANT_CREATE_MSG_Q - Cannot create message queue.
 * NOTES    :   If the queue doesn't exist, it will be created and owner task
 *              will be set to the calling task.
 *              If the queue exists, get the queue and set owner to the calling task.
 *              If msgq_type doesn't match existed queue, this function
 *              will correct the type of the existed queue.
 * ------------------------------------------------------------------------
 */
UI32_T SYSFUN_CreateMsgQ(UI32_T msgq_key, UI32_T msgq_type, UI32_T *msgq_handle_p)
{
    SYSFUN_MsgQ *msgq = NULL;

    if (SYSFUN_MSGQKEY_PRIVATE != msgq_key)
    {
        msgq = SYSFUN_L_SearchMessageQueue(msgq_key);

        if (NULL != msgq)
        {
            *msgq_handle_p = SYSFUN_L_MessageQueuePtrToId(msgq);
            return SYSFUN_OK;
        }
    }

    msgq = SYSFUN_L_AllocateMessageQueue();
    if (NULL == msgq)
    {
        return SYSFUN_RESULT_CANT_CREATE_MSG_Q;
    }

    *msgq_handle_p = SYSFUN_L_MessageQueuePtrToId(msgq);
    return SYSFUN_OK;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - SYSFUN_DestroyMsgQ
 * ------------------------------------------------------------------------
 * PURPOSE  :   To destroy the message queue.
 * INPUT    :   msgq_handle - The queue to destroy.
 * OUTPUT   :   None.
 * RETURN   :   SYSFUN_OK                   - Successfully.
 *              SYSFUN_RESULT_OBJ_DELETED   - Queue has been deleted.
 *              SYSFUN_RESULT_INVALID_ID    - qid is incorrect; failed validity check.
 *              SYSFUN_RESULT_TASK_WAIT_MSG - Informative only; there were tasks waiting at the queue.
 *              SYSFUN_RESULT_ERROR         - Unknown error
 * NOTES    :   This api will release the handle and destroy the message queue.
 * ------------------------------------------------------------------------
 */
UI32_T SYSFUN_DestroyMsgQ(UI32_T msgq_handle)
{
    SYSFUN_MsgQ *msgq = SYSFUN_L_GetMessageQueueAtId(msgq_handle);

    if (NULL == msgq)
    {
        return SYSFUN_RESULT_OBJ_DELETED;
    }

    SYSFUN_L_DestroyMessageQueue(msgq);
    return SYSFUN_OK;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - SYSFUN_GetMsgQ
 * ------------------------------------------------------------------------
 * PURPOSE  :   Get the message queue for the specific key.
 * INPUT    :   msgq_key      - The key for the message queue.
 *                              It can't be SYSFUN_MSGQKEY_PRIVATE.
 *              msgq_type     - To specify if the queue is bidirectional.
 *                              SYSFUN_MSGQ_UNIDIRECTIONAL
 *                                Unidirectional message queue.
 *                                For sender, send message via SYSFUN_SendRequestMsg().
 *                                For receiver, receive message via SYSFUN_ReceiveMsg().
 *                              SYSFUN_MSGQ_BIDIRECTIONAL
 *                                The queue will be created for bidirectional
 *                                communication and own two queues, one for
 *                                request and another for response.
 *                                For sender, send request via SYSFUN_SendRequestMsg().
 *                                For receiver, receive request via SYSFUN_ReceiveMsg()
 *                                              and send response via SYSFUN_SendResponseMsg().
 * OUTPUT   :   msgq_handle_p - The queue handle.
 * RETURN   :   SYSFUN_OK                       - Successfully.
 *              SYSFUN_RESULT_CANT_CREATE_MSG_Q - Cannot create message queue.
 * NOTES    :   If the queue doesn't exist, it will be created and owner task id
 *              will be set to ,key0.
 *              If the queue exists, get the queue.
 * ------------------------------------------------------------------------
 */
UI32_T SYSFUN_GetMsgQ(UI32_T msgq_key, UI32_T msgq_type, UI32_T *msgq_handle_p)
{
    if (SYSFUN_MSGQKEY_PRIVATE == msgq_key)
    {
        return SYSFUN_RESULT_ERROR;
    }

    return SYSFUN_CreateMsgQ(msgq_key, msgq_type, msgq_handle_p);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - SYSFUN_SendRequestMsg
 * ------------------------------------------------------------------------
 * PURPOSE  :   Send a request message and wait for response.
 * INPUT    :   msgq_handle  - The queue that the request be sent to and
 *                             the reponse be received from.
 *              req_msg_p    - The request message.
 *                             The request message size must be filled in req_msg_p->msg_size
 *                             For unidirectional msg queue,
 *                               req_msg_p->msg_type means type of message
 *                               and must be > 0.
 *                             For bidirectional msg queue,
 *                               req_msg_p->msg_type means task id of the request sender
 *                               and will be filled automatically.
 *              wait_time    - The waiting time if the queue is full.
 *                             SYSFUN_TIMEOUT_NOWAIT        : no wait.
 *                             SYSFUN_TIMEOUT_WAIT_FOREVER  : wait forever
 *                             other: waiting time (in clock ticks).
 *              event        - The event that will be sent to the queue owner
 *                             if request successfully and queue owner is knowable.
 *              res_msg_size - The size of the res_msg_p->msg_buf.
 *                             Set as 0 if this is unidirection message, otherwise its bidirection message.
 * OUTPUT   :   res_msg_p    - The buffer to receive reponse.
 *                             NULL means that it's unnecessary to wait for response
 *                             and the parameter res_msg_size will be ignored.
 *                             Set as NULL if this is unidirection message, otherwise its bidirection message.
 * RETURN   :   SYSFUN_OK                   - Successfully.
 *              SYSFUN_RESULT_TIMEOUT       - Timeout, the queue is full and waiting time is timeout.
 *              SYSFUN_RESULT_OBJ_DELETED   - Message queue is deleted.
 *              SYSFUN_RESULT_INVALID_ID    - queue ID is incorrect.
 *              SYSFUN_RESULT_MSG_Q_FULL    - message queue is full, for NO_WAIT.
 *              SYSFUN_RESULT_INVALID_ARG   - Invalid parameters.
 *              SYSFUN_RESULT_ERROR         - Unknown error
 * NOTES    :   If the queue is unidirectional,
 *                req_msg_p and res_msg_size will be ignored.
 *              If the queue is bidirectional and res_msg_p isn't NULL,
 *                when request is sent successfully, this function will block
 *                and wait for response message.
 * ------------------------------------------------------------------------
 */
UI32_T __SYSFUN_SendRequestMsg(UI32_T msgq_handle,
                              SYSFUN_Msg_T *req_msg_p,
                              UI32_T wait_time,
                              UI32_T event,
                              UI32_T res_msg_size,
                              SYSFUN_Msg_T *res_msg_p
#ifdef SYSFUN_DEBUG
                              ,
                              const char *   func,
                              unsigned       line
#endif /* SYSFUN_DEBUG */
                              )
{
    size_t buflen = (req_msg_p->msg_size < res_msg_size) ? res_msg_size : req_msg_p->msg_size;
    SYSFUN_MsgQ *msgq = SYSFUN_L_GetMessageQueueAtId(msgq_handle);
    SYSFUN_Msg_T *buf;

    if (NULL == msgq || NULL == msgq->handler_fn)
    {
        printf("msgq_handle %lu is invalid\n", msgq_handle);
        return SYSFUN_RESULT_ERROR;
    }

    buflen = SYSFUN_SIZE_OF_MSG(buflen);
    buf = calloc(buflen, 1);
    if (NULL == buf)
    {
        printf("%s:%d Out of memory\n", __FUNCTION__, __LINE__);
        return SYSFUN_RESULT_ERROR;
    }

    memcpy(buf, req_msg_p, buflen);

    msgq->handler_fn(buf);

    memcpy(res_msg_p->msg_buf, buf->msg_buf, res_msg_size);

    free(buf);

    return SYSFUN_OK;
}

#ifdef _SAL_TIME_H
static innnniii llll
#endif

static time_t sysfun_sys_clock;

#define SYSFUN_SEC_TO_TICK(sec) ((sec) * SYS_BLD_TICKS_PER_SECOND)

/* FUNCTION NAME : SYSFUN_GetSysTick
 * PURPOSE:
 *      Get system accumulated ticks, from system start or restart.
 *      Stack topology do not reset the ticks.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *
 * RETURN:
 *      The accumulated ticks from system start or restart.
 *
 * NOTES:
 *      1. The unit of tick is 0.01 sec, ie. 10 ms. depending on system configuration.
 */
UI32_T  SYSFUN_GetSysTick (void)
{
    time_t t;

    UI32_T cur_time = time(&t);
    UI32_T ret = cur_time - sysfun_sys_clock;

    return ret * SYS_BLD_TICKS_PER_SECOND;
}

/* FUNCTION NAME : SYSFUN_MSecondToTick
 * PURPOSE:
 *      Convert msecond to sys tick
 *
 * INPUT:
 *      ms - msecond
 *
 * OUTPUT:
 *
 * RETURN:
 *      The result of sys tick
 *
 * NOTES:
 *      1. The unit of tick is 0.01 sec, ie. 10 ms. depending on system configuration.
 */
UI32_T  SYSFUN_MSecondToTick (UI32_T ms)
{
    return (UI32_T)(ms * SYS_BLD_TICKS_PER_SECOND / 10);
}

void SYSFUN_LogMsg(char *msg_text, I32_T arg1, I32_T arg2, I32_T arg3, I32_T arg4, I32_T arg5, I32_T arg6)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_DEFAULT, (char *)msg_text, arg1, arg2, arg3, arg4, arg5, arg6);
}

void SYSFUN_LogUrgentMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_URGENT, "%s", (char *)msg_text);
}

void SYSFUN_LogAlertMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_ALERT, "%s", (char *)msg_text);
}

void SYSFUN_LogCriticalMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_CRITICAL, "%s", (char *)msg_text);
}

void SYSFUN_LogErrMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_ERR, "%s", (char *)msg_text);
}

void SYSFUN_LogWarnMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_WARN, "%s", (char *)msg_text);
}

void SYSFUN_LogNoticeMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_NOTICE, "%s", (char *)msg_text);
}

void SYSFUN_LogInfoMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_INFO, "%s", (char *)msg_text);
}

void SYSFUN_LogDebugMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_DEBUG, "%s", (char *)msg_text);
}

/*----------------------------------------
 *  Utility for processing
 *----------------------------------------
 */
/* FUNCTION NAME : SYSFUN_VisibleCode
 * PURPOSE:
 *      If the code is invisible, return a '.', otherwise return the code.
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      code    -- to be checked characteristic.
 *
 * RETURN:
 *      visible code -- '.' or original code.
 *
 * NOTES:
 *
 */
char SYSFUN_VisibleCode(unsigned char code)
{
    if ((code<32)||(code>128))
        return  0x2E;
    return (code);
}   /*  end of SYSFUN_VisibleCode   */

/* FUNCTION NAME : SYSFUN_Hex2Ascii
 * PURPOSE:
 *      Translate hexi-code to '0'..'9', 'A'..'F'
 *      eg. 0x0A return 'A'.
 * INPUT:
 *      hex -- the code to be processing.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      char number produced.
 *
 * NOTES:
 *      1. buf MUST have 2 char. space at least.
 */
static char SYSFUN_Hex2Ascii (unsigned hex)
{
    register int bin_code = hex;

    if ((bin_code=bin_code&0x0f)>9)
        return (char)(bin_code + 0x37);
    return  (char) (bin_code + 0x30);
}   /*  end of SYSFUN_Hex2Ascii */

/* FUNCTION NAME : SYSFUN_Val2HexStr
 * PURPOSE:
 *      Translate hexi-code to visible char.
 *      eg. 0x3D to "3D"
 * INPUT:
 *      code    -- the code to be processing.
 *      buf     -- the buffer to hold visible char.
 *                 only 2 char. space is used.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      char number produced.
 *
 * NOTES:
 *      1. buf MUST have 2 char. space at least.
 */
int  SYSFUN_Val2HexStr(unsigned char code, char *buf)
{
    /* LOCAL CONSTANT DECLARATIONS
     */
    /* LOCAL VARIABLE DECLARATIONS
     */
    /* BODY */
    buf[0]  = SYSFUN_Hex2Ascii ((0xF0& code) >> 4);
    buf[1]  = SYSFUN_Hex2Ascii (0x0F & code);
    return  2;
}   /*  end of SYSFUN_Val2Str  */

UI32_T SYSFUN_SetSysClock(UI32_T *t)
{
    time_t buf;

    UI32_T cur_time = (UI32_T)time(&buf);
    UI32_T set_time = *t;

    sysfun_sys_clock = (time_t) difftime(cur_time, set_time);
    return SYSFUN_OK;
}

/* convert the date time to seconds since 1970-1-1 00:00:00 */
UI32_T SYSFUN_mktime(int year, int month, int day, int hour,
                     int minute, int second, UI32_T *seconds1970)
{
    struct tm tms;
    UI32_T seconds_since_19700101;

    memset((void *)&tms, 0, sizeof(tms));
    tms.tm_year = year - 1900;
    tms.tm_mon = month - 1;
    tms.tm_mday = day;
    tms.tm_hour = hour;
    tms.tm_min = minute;
    tms.tm_sec = second;
    seconds_since_19700101 = mktime(&tms);

    *seconds1970 = seconds_since_19700101 + tms.tm_gmtoff;

    return 0;
}

/* convert the seconds since 1970-1-1 00:00:00 to date time */
UI32_T SYSFUN_gmtime(UI32_T seconds1970, struct tm *result)
{
    struct tm *ptms;
    time_t t = seconds1970;

    ptms = gmtime_r(&t, result);

    result->tm_year += 1900;
    result->tm_mon += 1;

    return 0;
}

UI32_T  SYSFUN_Sleep (UI32_T  ticks)
{
    sleep(ticks);

    return ticks;
}