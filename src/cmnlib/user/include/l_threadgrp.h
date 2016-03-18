/*-------------------------------------------------------------------------
 * MODULE NAME: l_threadgrp.h
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

#ifndef     L_THREADGRP_H
#define     L_THREADGRP_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
/* The handle of a thread group.
 */
typedef void *L_THREADGRP_Handle_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
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
L_THREADGRP_Handle_T L_THREADGRP_Create();

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
BOOL_T L_THREADGRP_Join(L_THREADGRP_Handle_T tg_handle, UI16_T priority, UI32_T *member_id_p);

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
void L_THREADGRP_Leave(L_THREADGRP_Handle_T tg_handle, UI32_T member_id);

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
BOOL_T L_THREADGRP_Execution_Request(L_THREADGRP_Handle_T tg_handle, UI32_T member_id);

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
BOOL_T L_THREADGRP_Execution_Release(L_THREADGRP_Handle_T tg_handle, UI32_T member_id);

#endif /* L_THREADGRP_H */
