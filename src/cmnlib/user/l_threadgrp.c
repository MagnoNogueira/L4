/*-------------------------------------------------------------------------
 * MODULE NAME: l_threadgrp.c
 *-------------------------------------------------------------------------
 * PURPOSE: Provide the thread groups management.
 *
 *      All members in a thread groups have three states: IDLE, READY, RUN,
 *      and can perform two operations: request/release execution.
 *      As join a thread group, the state of the member will initial to IDLE,
 *      When it request execution, its state is chnaged to READY, and enter
 *      the execution competition with other READY members. The one win the
 *      competition, it becomes RUN state and other READY members must wait
 *      until it releases execution.
 *
 * NOTES:
 *      1. It allows SYS_DFLT_NUM_OF_THREADS_IN_THREADGROUP
 *         members at most to join a thread group.
 *      2. The implementation of THREADGRP use a system event
 *         SYSFUN_SYSTEM_EVENT_RESUME_TASK to accomplish blocking a member
 *         thread that is READY state if a member with RUN state exists.
 *
 * Modification History:
 *      Date                Modifier,           Reason
 *      ------------------------------------------------------------------
 *      05-31-2007          Wakka               Create
 *
 * COPYRIGHT(C)         Accton Corporation, 2007
 *------------------------------------------------------------------------*/

/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <stdlib.h>
#include "sys_type.h"
#include "sys_dflt.h"
#include "sysfun.h"
#include "l_threadgrp.h"

/* NAMING CONSTANT DECLARATIONS
 */
/* For debug.
 */
#define L_THREADGRP_UNIT_TEST  FALSE
#define L_THREADGRP_DEBUG      FALSE

/* constant definition for state of thread group member
 */
enum {
    L_THREADGRP_STATE_IDLE,
    L_THREADGRP_STATE_READY,
    L_THREADGRP_STATE_RUN
};

/* MACRO FUNCTION DECLARATIONS
 */
/* For enter/leave critical section.
 */
#define L_THREADGRP_ENTER_CRITICAL_SECTION(tg_desc_p) SYSFUN_ENTER_CRITICAL_SECTION((tg_desc_p)->sem_id, SYSFUN_TIMEOUT_WAIT_FOREVER)
#define L_THREADGRP_LEAVE_CRITICAL_SECTION(tg_desc_p) SYSFUN_LEAVE_CRITICAL_SECTION((tg_desc_p)->sem_id)

/* For suspend/resume a member thread.
 */
#define L_THREADGRP_CLEAR_EVENT() SYSFUN_ReceiveEvent(SYSFUN_SYSTEM_EVENT_RESUME_TASK, SYSFUN_EVENT_WAIT_ANY, SYSFUN_TIMEOUT_NOWAIT, &received_events)
#define L_THREADGRP_SUSPEND() (SYSFUN_ReceiveEvent(SYSFUN_SYSTEM_EVENT_RESUME_TASK, SYSFUN_EVENT_WAIT_ANY, SYSFUN_TIMEOUT_WAIT_FOREVER, &received_events) == SYSFUN_OK)
#define L_THREADGRP_RESUME(member_p) (SYSFUN_SendEvent((member_p)->task_id, SYSFUN_SYSTEM_EVENT_RESUME_TASK) == SYSFUN_OK)

/*-------------------------------------------------------------------------
 * MACRO NAME - L_THREADGRP_IS_USED_MEMBER_ID
 * ------------------------------------------------------------------------
 * PURPOSE  :   To check if the member id is used.
 * INPUT    :   tg_desc_p - The descriptor of the thread group.
 *              member_id - The thread group member id.
 * OUTPUT   :   None.
 * RETURN   :   TRUE  - The member id is used.
 *              FALSE - The member id is unused.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
#define L_THREADGRP_IS_USED_MEMBER_ID(tg_desc_p, member_id) ((tg_desc_p)->member_entry[(member_id)].task_id > 0)

/*-------------------------------------------------------------------------
 * MACRO NAME - L_THREADGRP_IS_VALID_MEMBER_ID
 * ------------------------------------------------------------------------
 * PURPOSE  :   To check the validity of the member id.
 * INPUT    :   tg_desc_p - The descriptor of the thread group.
 *              member_id - The thread group member id.
 * OUTPUT   :   None.
 * RETURN   :   TRUE  - The member id is valid.
 *              FALSE - The member id is invalid.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
#define L_THREADGRP_IS_VALID_MEMBER_ID(member_id) ((member_id) < SYS_DFLT_NUM_OF_THREADS_IN_THREADGROUP)

/*-------------------------------------------------------------------------
 * MACRO NAME - L_THREADGRP_FIND_UNUSED_MEMBER_ID
 * ------------------------------------------------------------------------
 * PURPOSE  :   To find a unused member id.
 * INPUT    :   tg_desc_p - The descriptor of the thread group.
 * OUTPUT   :   None.
 * RETURN   :   A unused member id.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
#define L_THREADGRP_FIND_UNUSED_MEMBER_ID(tg_desc_p)              \
({                                                                \
    UI32_T i;                                                     \
    for (i = 0; i < SYS_DFLT_NUM_OF_THREADS_IN_THREADGROUP; i++)  \
        if (!L_THREADGRP_IS_USED_MEMBER_ID((tg_desc_p), i))       \
            break;                                                \
    i;                                                            \
})

/*-------------------------------------------------------------------------
 * MACRO NAME - L_THREADGRP_GET_ENTRY
 * ------------------------------------------------------------------------
 * PURPOSE  :   To get the member entry by its id.
 * INPUT    :   tg_desc_p - The descriptor of the thread group.
 *              member_id - The thread group member id.
 * OUTPUT   :   None.
 * RETURN   :   The member entry. (L_THREADGRP_MemberEntry_T *)
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
#define L_THREADGRP_GET_ENTRY(tg_desc_p, member_id) ((tg_desc_p)->member_entry + (member_id))

/*-------------------------------------------------------------------------
 * MACRO NAME - L_THREADGRP_GET_RUNNING_MEMBER
 * ------------------------------------------------------------------------
 * PURPOSE  :   To get the running member entry.
 * INPUT    :   tg_desc_p - The descriptor of the thread group.
 * OUTPUT   :   None.
 * RETURN   :   The running member entry.
 *              NULL - no member is RUN state.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
#define L_THREADGRP_GET_RUNNING_MEMBER(tg_desc_p)                 \
({                                                                \
    UI32_T i;                                                     \
    for (i = 0; i < SYS_DFLT_NUM_OF_THREADS_IN_THREADGROUP; i++)  \
        if (L_THREADGRP_IS_USED_MEMBER_ID((tg_desc_p), (i)) &&    \
            (tg_desc_p)->member_entry[i].thread_state == L_THREADGRP_STATE_RUN) \
            break;                                                \
    (L_THREADGRP_IS_VALID_MEMBER_ID(i) ? L_THREADGRP_GET_ENTRY((tg_desc_p), i) : NULL); \
})

/*-------------------------------------------------------------------------
 * MACRO NAME - L_THREADGRP_SHOW_MEMBER_INFO
 * ------------------------------------------------------------------------
 * PURPOSE  :   To print the information of the member thread.
 * INPUT    :   tg_desc_p - The descriptor of the thread group.
 *              member_id - The thread group member id.
 *              action    - A null terminal string that describe the action.
 * OUTPUT   :   None.
 * RETURN   :   None.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
#if L_THREADGRP_DEBUG
#define L_THREADGRP_SHOW_MEMBER_INFO(tg_desc_p, member_id, action) printf("\r\ntid: %-5d  gid: %-10u  mid: %-2d  priority: %d,%d \tstate: %d  %s", (int)SYSFUN_TaskIdSelf(), (unsigned int)(tg_desc_p), (int)(member_id), (int)((tg_desc_p)->member_entry[(member_id)].priority >> 16), (int)((tg_desc_p)->member_entry[(member_id)].priority & 0xffff), (int)((tg_desc_p)->member_entry[(member_id)].thread_state), (action))
#else
#define L_THREADGRP_SHOW_MEMBER_INFO(tg_desc_p, member_id, action)
#endif


/* DATA TYPE DECLARATIONS
 */
typedef struct
{
    UI32_T task_id;      /* The task id of the thread group member; 0 is unused. */
    UI32_T thread_state; /* Enum value: L_THREADGRP_STATE_IDLE
                          *             L_THREADGRP_STATE_READY
                          *             L_THREADGRP_STATE_RUN
                          */
    UI32_T priority;     /* Priority is consisted of two words.
                          * High word: the priority specified by the calling
                          *            thread.
                          * Low  word: the value of the counter in L_THREADGRP_Desc_T
                          *            when the thread is chosen to become RUN
                          *            thread.
                          *            This value will be used to implement
                          *            round-robin policy on the same
                          *            priority threads.
                          * NOTE: The lower value of priority indicates higher
                          *       priority.
                          */
} L_THREADGRP_MemberEntry_T;

typedef struct
{
    UI16_T    counter;         /* For round-robin policy on the same priority threads */
    UI16_T    num_of_member;   /* The number of the member that join this thread group */
    UI32_T    sem_id;

    /* member_entry is an array for keeping info regarding each thread member.
     */
    L_THREADGRP_MemberEntry_T member_entry[SYS_DFLT_NUM_OF_THREADS_IN_THREADGROUP];
} L_THREADGRP_Desc_T;

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static UI32_T L_THREADGRP_GetTopPriorityMember(L_THREADGRP_Desc_T *tg_desc_p);
static BOOL_T L_THREADGRP_ActivateMember(L_THREADGRP_Desc_T *tg_desc_p, UI32_T member_id);

/* STATIC VARIABLE DECLARATIONS
 */
/* For receive event when suspend member thread.
 */
static UI32_T received_events;

/* EXPORTED SUBPROGRAM BODIES
 */
/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_THREADGRP_Create
 * ------------------------------------------------------------------------
 * PURPOSE  :   Create a thread group.
 * INPUT    :   None.
 * OUTPUT   :   None.
 * RETURN   :   The handle of the new thread group if successful.
 *              NULL if failed.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
L_THREADGRP_Handle_T L_THREADGRP_Create()
{
    L_THREADGRP_Desc_T *tg_desc_p;

    tg_desc_p = calloc(1, sizeof(L_THREADGRP_Desc_T));

    if (tg_desc_p != NULL)
    {
        /* Initialize semaphore.
         */
        if (SYSFUN_CreateSem(SYSFUN_SEMKEY_PRIVATE, 1, SYSFUN_SEM_FIFO, &tg_desc_p->sem_id) != SYSFUN_OK)
        {
            free(tg_desc_p);
            printf("\r\n%s(): can't create semaphore.", __FUNCTION__);
            return NULL;
        }
    }

    return (L_THREADGRP_Handle_T)tg_desc_p;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_THREADGRP_Join
 * ------------------------------------------------------------------------
 * PURPOSE  :   Let the calling thread join the specific thread group.
 * INPUT    :   tg_handle - The handle of the thread group.
 *              priority  - The priority for the calling thread in the
 *                          specific group.
 * OUTPUT   :   member_id_p - The thread group member id if successful.
 * RETURN   :   TRUE  - Successful.
 *              FALSE - Failed. The parameters were incorrect.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
BOOL_T L_THREADGRP_Join(L_THREADGRP_Handle_T tg_handle, UI16_T priority, UI32_T *member_id_p)
{
    L_THREADGRP_Desc_T *tg_desc_p = (L_THREADGRP_Desc_T *)tg_handle;
    L_THREADGRP_MemberEntry_T *member_p;
    UI32_T i;

    if (tg_desc_p == NULL)
        return FALSE;

    L_THREADGRP_ENTER_CRITICAL_SECTION(tg_desc_p);

    /* Find a unused entry,
     */
    i = L_THREADGRP_FIND_UNUSED_MEMBER_ID(tg_desc_p);
    if (!L_THREADGRP_IS_VALID_MEMBER_ID(i))
    {
        L_THREADGRP_LEAVE_CRITICAL_SECTION(tg_desc_p);
        printf("\r\n%s(): thread group is full.", __FUNCTION__);
        return FALSE;
    }

    member_p = L_THREADGRP_GET_ENTRY(tg_desc_p, i);
    member_p->task_id = SYSFUN_TaskIdSelf();
    member_p->thread_state = L_THREADGRP_STATE_IDLE;
    member_p->priority = priority << 16 | tg_desc_p->counter;

    L_THREADGRP_SHOW_MEMBER_INFO(tg_desc_p, i, "JOIN");

    L_THREADGRP_LEAVE_CRITICAL_SECTION(tg_desc_p);

    /* Clear event SYSFUN_SYSTEM_EVENT_RESUME_TASK for the calling task.
     */
    L_THREADGRP_CLEAR_EVENT();

    *member_id_p = i;
    return TRUE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_THREADGRP_Leave
 * ------------------------------------------------------------------------
 * PURPOSE  :   Let the calling thread leave the specific thread group.
 * INPUT    :   tg_handle - The handle of the thread group.
 *              member_id - The thread group member id of the calling thread.
 * OUTPUT   :   None.
 * RETURN   :   None.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
void L_THREADGRP_Leave(L_THREADGRP_Handle_T tg_handle, UI32_T member_id)
{
    L_THREADGRP_Desc_T *tg_desc_p = (L_THREADGRP_Desc_T *)tg_handle;

    if (tg_desc_p == NULL || !L_THREADGRP_IS_VALID_MEMBER_ID(member_id))
        return;

    L_THREADGRP_ENTER_CRITICAL_SECTION(tg_desc_p);

    /* Free member entry.
     */
    if (L_THREADGRP_IS_USED_MEMBER_ID(tg_desc_p, member_id))
        L_THREADGRP_GET_ENTRY(tg_desc_p, member_id)->task_id = 0;

    L_THREADGRP_LEAVE_CRITICAL_SECTION(tg_desc_p);
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_THREADGRP_Execution_Request
 * ------------------------------------------------------------------------
 * PURPOSE  :   Request the scheduler of the thread group to schedule the
 *              calling thread to run.
 * INPUT    :   tg_handle - The handle of the thread group.
 *              member_id - The thread group member id of the calling thread.
 * OUTPUT   :   None.
 * RETURN   :   TRUE  - Successful.
 *              FALSE - Failed. The parameters were incorrect.
 * NOTES    :   If the state of the calling thread is RUN,
 *              set to READY and reschedule.
 * ------------------------------------------------------------------------
 */
BOOL_T L_THREADGRP_Execution_Request(L_THREADGRP_Handle_T tg_handle, UI32_T member_id)
{
    L_THREADGRP_Desc_T *tg_desc_p = (L_THREADGRP_Desc_T *)tg_handle;
    L_THREADGRP_MemberEntry_T *member_p;
    UI32_T candidate, orig_state;
    BOOL_T ret = TRUE;
    BOOL_T need_suspend = FALSE;

    if (tg_desc_p == NULL || !L_THREADGRP_IS_VALID_MEMBER_ID(member_id))
        return FALSE;

    L_THREADGRP_ENTER_CRITICAL_SECTION(tg_desc_p);

    if (!L_THREADGRP_IS_USED_MEMBER_ID(tg_desc_p, member_id) ||
        (member_p = L_THREADGRP_GET_ENTRY(tg_desc_p, member_id))->thread_state == L_THREADGRP_STATE_READY)
    {
        printf("\r\n%s(): member_id is invalid. (%d)", __FUNCTION__, (int)member_id);
        return FALSE;
    }

    L_THREADGRP_SHOW_MEMBER_INFO(tg_desc_p, member_id, "REQUEST");

    /* Store original state.
     */
    orig_state = member_p->thread_state;

    /* If original state is RUN, reschedule the task.
     *              ...  is IDLE, schedule the task.
     */
    if (orig_state == L_THREADGRP_STATE_RUN)
    {
        if (member_id == (candidate = L_THREADGRP_GetTopPriorityMember(tg_desc_p)))
        {
            /* If the calling thread thread is the top priority,
             *   do nothing to keep running.
             */
            L_THREADGRP_SHOW_MEMBER_INFO(tg_desc_p, member_id, "RE-RUN");
        }
        else
        {
            /* If the calling thread thread isn't the top priority,
             *   change state to READY and wake up the top priority member.
             */
            member_p->thread_state = L_THREADGRP_STATE_READY;
            need_suspend = TRUE;
            L_THREADGRP_SHOW_MEMBER_INFO(tg_desc_p, member_id, "RE-SUSP");
            ret = L_THREADGRP_ActivateMember(tg_desc_p, candidate);
        }
    }
    else /* state == IDLE */
    {
        if (L_THREADGRP_GET_RUNNING_MEMBER(tg_desc_p) != NULL)
        {
            /* Check if exist a member entry is running.
             * If yes, change state to READY.
             */
            member_p->thread_state = L_THREADGRP_STATE_READY;
            need_suspend = TRUE;
            L_THREADGRP_SHOW_MEMBER_INFO(tg_desc_p, member_id, "SUSPEND");
        }
        else
        {
            /* If no member entry is running,
             * activate this thread and change state to RUN.
             */
            ret = L_THREADGRP_ActivateMember(tg_desc_p, member_id);
        }
    }

    L_THREADGRP_LEAVE_CRITICAL_SECTION(tg_desc_p);

    if (!ret)
        goto ERROR;

    if (need_suspend)
    {
        if (!L_THREADGRP_SUSPEND())
            goto ERROR;
        L_THREADGRP_SHOW_MEMBER_INFO(tg_desc_p, member_id, "WAKEUP");
    }
    return TRUE;

ERROR:
    /* Restore original state if fail to request execution.
     */
    member_p->thread_state = orig_state;
    L_THREADGRP_SHOW_MEMBER_INFO(tg_desc_p, member_id, "REQ-FAIL");
    return FALSE;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_THREADGRP_Execution_Release
 * ------------------------------------------------------------------------
 * PURPOSE  :   Notify the scheduler of the thread group that the calling
 *              thread has finished the transaction and has nothing else
 *              to process now.
 * INPUT    :   tg_handle - The handle of the thread group.
 *              member_id - The thread group member id of the calling thread.
 * OUTPUT   :   None.
 * RETURN   :   TRUE  - Successful.
 *              FALSE - Failed. The parameters were incorrect.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
BOOL_T L_THREADGRP_Execution_Release(L_THREADGRP_Handle_T tg_handle, UI32_T member_id)
{
    L_THREADGRP_Desc_T *tg_desc_p = (L_THREADGRP_Desc_T *)tg_handle;
    L_THREADGRP_MemberEntry_T *member_p;
    UI32_T candidate;
    BOOL_T ret;

    if (tg_handle == NULL || !L_THREADGRP_IS_VALID_MEMBER_ID(member_id))
        return FALSE;

    L_THREADGRP_ENTER_CRITICAL_SECTION(tg_desc_p);

    if (!L_THREADGRP_IS_USED_MEMBER_ID(tg_desc_p, member_id) ||
        (member_p = L_THREADGRP_GET_ENTRY(tg_desc_p, member_id))->thread_state != L_THREADGRP_STATE_RUN)
    {
        printf("\r\n%s(): member_id is invalid. (id:%d)", __FUNCTION__, (int)member_id);
        return FALSE;
    }

    /* Change state to IDLE.
     */
    member_p->thread_state = L_THREADGRP_STATE_IDLE;
    L_THREADGRP_SHOW_MEMBER_INFO(tg_desc_p, member_id, "RELEASE");

    /* Pick the ready thread with the highest priority to RUN state.
     */
    candidate = L_THREADGRP_GetTopPriorityMember(tg_desc_p);
    if (L_THREADGRP_IS_VALID_MEMBER_ID(candidate))
        ret = L_THREADGRP_ActivateMember(tg_desc_p, candidate);
    else
        ret = TRUE;

    L_THREADGRP_LEAVE_CRITICAL_SECTION(tg_desc_p);

    return ret;
}

/* LOCAL SUBPROGRAM BODIES
 */
/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_THREADGRP_GetTopPriorityMember
 * ------------------------------------------------------------------------
 * PURPOSE  :   This function will Pick the ready thread with the highest
 *              priority.
 * INPUT    :   tg_desc_p - The descriptor of the thread group.
 * OUTPUT   :   None.
 * RETURN   :   The thread group member id.
 * NOTES    :   A thread is taken as a ready thread if it's state is RUN or READY.
 * ------------------------------------------------------------------------
 */
static UI32_T L_THREADGRP_GetTopPriorityMember(L_THREADGRP_Desc_T *tg_desc_p)
{
    UI32_T i, candidate = SYS_DFLT_NUM_OF_THREADS_IN_THREADGROUP;

    for (i = 0; i < SYS_DFLT_NUM_OF_THREADS_IN_THREADGROUP; i++)
    {
        /* Find the ready thread with highest priority.
         */
        if (!L_THREADGRP_IS_USED_MEMBER_ID(tg_desc_p, i) ||
            tg_desc_p->member_entry[i].thread_state == L_THREADGRP_STATE_IDLE)
            continue;
        if (candidate == SYS_DFLT_NUM_OF_THREADS_IN_THREADGROUP ||
            tg_desc_p->member_entry[i].priority < tg_desc_p->member_entry[candidate].priority)
            candidate = i;
    }

    return candidate;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME - L_THREADGRP_ActivateMember
 * ------------------------------------------------------------------------
 * PURPOSE  :   This function will activate the member thread.
 * INPUT    :   tg_desc_p - The descriptor of the thread group.
 *              member_id - The id of the member thread to activate.
 * OUTPUT   :   None.
 * RETURN   :   TRUE  - Successful.
 *              FALSE - Failed.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
static BOOL_T L_THREADGRP_ActivateMember(L_THREADGRP_Desc_T *tg_desc_p, UI32_T member_id)
{
    L_THREADGRP_MemberEntry_T *member_p;
    UI32_T i, orig_state;

    /* Increase the counter.
     */
    tg_desc_p->counter++;

    /* When the counter is changed from 0xFFFF to 0,
     * low word of priority of all members of the group
     * must be reset to 0.
     */
    if (tg_desc_p->counter == 0)
        for (i = 0; i < SYS_DFLT_NUM_OF_THREADS_IN_THREADGROUP; i++)
            if (L_THREADGRP_IS_USED_MEMBER_ID(tg_desc_p, i))
                tg_desc_p->member_entry[i].priority &= 0xffff0000;

    /* Set state to RUN.
     */
    member_p = L_THREADGRP_GET_ENTRY(tg_desc_p, member_id);

    orig_state = member_p->thread_state;
    member_p->thread_state = L_THREADGRP_STATE_RUN;

    /* The value of counter of the thread group must be set to
     * low word of priority of the member thread.
     */
    member_p->priority = (member_p->priority & 0xffff0000) | tg_desc_p->counter;

    L_THREADGRP_SHOW_MEMBER_INFO(tg_desc_p, member_id, "ACTIVATE");

    /* If original state of the thread is READY,
     * means that it is suspending now, so WAKE IT UP.
     */
    if (orig_state == L_THREADGRP_STATE_READY)
    {
        if (!L_THREADGRP_RESUME(member_p))
        {
            /* If fail to wake it up, restore its state.
             *
             * NOTE:
             *   It may lead a FATAL ERROR -- no member in execution --
             *   all ready members is blocking until
             *   other IDLE member request execution successful.
             */
            member_p->thread_state = orig_state;
            L_THREADGRP_SHOW_MEMBER_INFO(tg_desc_p, member_id, "ACT-FAIL");
            return FALSE;
        }
    }

    return TRUE;
}

#if L_THREADGRP_UNIT_TEST

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sys_bld.h"
#include "sysfun.h"

#define L_THREADGRP_UNIT_TEST_NUM_OF_THREADGROUPS      1
#define L_THREADGRP_UNIT_TEST_NUM_OF_THREADS_PER_GROUP 10
#define L_THREADGRP_UNIT_TEST_PRIORITY_LEVEL           4
#define L_THREADGRP_UNIT_TEST_GLOBAL_COUNT             100
#define L_THREADGRP_UNIT_TEST_LOCAL_COUNT              (L_THREADGRP_UNIT_TEST_GLOBAL_COUNT / L_THREADGRP_UNIT_TEST_NUM_OF_THREADS_PER_GROUP)

static L_THREADGRP_Handle_T l_threadgrp_test_tg_handle[L_THREADGRP_UNIT_TEST_NUM_OF_THREADGROUPS];
static int l_threadgrp_test_counter[L_THREADGRP_UNIT_TEST_NUM_OF_THREADGROUPS];

static void L_THREADGRP_TEST_thread(void *arg)
{
    L_THREADGRP_Handle_T tg_handle;
    UI32_T member_id;
    UI16_T priority;
    int threadgrp_id = (UI32_T)arg;
    int *count_p;
    int local_count = 0;

    tg_handle = l_threadgrp_test_tg_handle[threadgrp_id];
    count_p = &l_threadgrp_test_counter[threadgrp_id];
    priority = rand() % L_THREADGRP_UNIT_TEST_PRIORITY_LEVEL;

    assert(L_THREADGRP_Join(tg_handle, priority, &member_id));
    printf("\r\n[%d-%d] priority: %d  counter: %d", threadgrp_id, (int)member_id, (int)priority, *count_p);

    while (1)
    {
        assert(L_THREADGRP_Execution_Request(tg_handle, member_id));

        if (local_count >= L_THREADGRP_UNIT_TEST_LOCAL_COUNT ||
            *count_p >= L_THREADGRP_UNIT_TEST_GLOBAL_COUNT)
        {
            assert(L_THREADGRP_Execution_Release(tg_handle, member_id));
            break;
        }
        local_count++;
        (*count_p)++;
        printf("\r\n[%d-%d] priority: %d  counter: %d", threadgrp_id, (int)member_id, (int)priority, *count_p);
        SYSFUN_Sleep(5);
        assert(L_THREADGRP_Execution_Release(tg_handle, member_id));
    }
    L_THREADGRP_Leave(tg_handle, member_id);
    printf("\r\n[%d-%d] priority: %d  counter: %d   EXIT", threadgrp_id, (int)member_id, (int)priority, *count_p);
}

void L_THREADGRP_TEST_main(void)
{
    char task_name[16];
    UI32_T thread_id;
    int i, j;

    srand(time(NULL));

    for (i = 0; i < L_THREADGRP_UNIT_TEST_NUM_OF_THREADGROUPS; i++)
    {
        assert(l_threadgrp_test_tg_handle[i] = L_THREADGRP_Create());

        for (j = 0; j < L_THREADGRP_UNIT_TEST_NUM_OF_THREADS_PER_GROUP; j++)
        {
            sprintf(task_name, "L_THREAD_%d_%d", i, j);
            assert(SYSFUN_SpawnThread(SYS_BLD_PROCESS_DEFAULT_PRIORITY, SYSFUN_SCHED_DEFAULT,
                            task_name, SYS_BLD_TASK_COMMON_STACK_SIZE, 0,
                            L_THREADGRP_TEST_thread, (void *)i, &thread_id) == SYSFUN_OK);
        }
    }
    while(1);
}

#endif /* L_THREADGRP_UNIT_TEST */

