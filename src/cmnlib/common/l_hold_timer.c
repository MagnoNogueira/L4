/* MODULE NAME: l_hold_timer.c
 * PURPOSE: This module provides a generic mechanism for the application
 *          which requires the efficient method to handle the frequent
 *          requests. When the burst of frequent requests occurs, it will
 *          be handled for only once so that the system won't be occupied
 *          in processing these request. And this will be under the
 *          hypothesis that the result will be always correct when there
 *          is only one service for the all requests.
 * NOTES:
 * Modification History:
 *      Date                Modifier,           Reason
 *      ----------------------------------------------
 *      06-17-2005          Kelly_Chen          create
 *
 * COPYRIGHT(C)         Accton Corporation, 2005
 */


/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "l_hold_timer.h"
#include "l_math.h"
#include "sysfun.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

enum L_HOLD_TIMER_ST_E
{
    L_HOLD_TIMER_IDLE_ST = 0,
    L_HOLD_TIMER_WAIT_ST,
    L_HOLD_TIMER_TO_WAIT_ST,
    L_HOLD_TIMER_TO_IDLE_ST
};

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static BOOL_T L_HOLD_TIMER_StateMachineProgress(L_HOLD_TIMER_Entry_T *timer, UI32_T event);
static UI32_T L_HOLD_TIMER_SM[2][2]    =
{
    {L_HOLD_TIMER_TO_WAIT_ST, L_HOLD_TIMER_IDLE_ST},
    {L_HOLD_TIMER_WAIT_ST, L_HOLD_TIMER_TO_IDLE_ST}
};

/* STATIC VARIABLE DECLARATIONS
 */


/* EXPORTED SUBPROGRAM BODIES
 */

/*  FUNCTION NAME: L_HOLD_TIMER_InitHoldTimer
 *  PURPOSE      : Init hold timer entry.
 *  INPUT        : *timer   - Include short time, long time, current short time,
 *                            current long time, and current state.
 *                 init_st  - initial short_time (tick).
 *                 init_lt  - initial long_time (tick).
 *  OUTPUT       : *timer
 *  NOTE         : Default state: IDLE_ST.
 *                 init_st(0)/init_lt(0): SM state is always in IDLE_ST. Don't have any action.
 */
void L_HOLD_TIMER_InitHoldTimer(L_HOLD_TIMER_Entry_T *timer, UI32_T init_st, UI32_T init_lt)
{
    timer->st         = init_st;
    timer->lt         = init_lt;
    timer->state      = L_HOLD_TIMER_IDLE_ST;
    return;
}/* End of L_HOLD_TIMER_InitHoldTimer */


/*  FUNCTION NAME: L_HOLD_TIMER_Handler
 *  PURPOSE      : Process option value.
 *  INPUT        : *timer   - Include short time, long time, current short time,
 *                            current long time, and current state.
 *                            The state value may be:
 *                            IDLE_ST(0),
 *                            WAIT_ST(1),
 *                            TO_WAIT_ST(2),
 *                            TO_IDLE_ST(3)
 *                 event    - event type, the value may be:
 *                            NEW_JOB_EV(0),
 *                            TIME_OUT_EV(1),
 *                            PERIODIC_UPDATE_EV(2)
 *  OUTPUT       : *timer
 *  NOTE         : None.
 */
BOOL_T L_HOLD_TIMER_Handler(L_HOLD_TIMER_Entry_T *timer, UI32_T event)
{
    if (event == L_HOLD_TIMER_PERIODIC_UPDATE_EV)
    {
        UI32_T  current_tick = SYSFUN_GetSysTick();

        if ( L_MATH_TimeOut32 (current_tick, timer->current_st) ||
             L_MATH_TimeOut32 (current_tick, timer->current_lt) )
             return L_HOLD_TIMER_StateMachineProgress(timer, L_HOLD_TIMER_TIME_OUT_EV);
        return FALSE;
    }
    return L_HOLD_TIMER_StateMachineProgress(timer, event);
}/* End of L_HOLD_TIMER_Handler */


/* LOCAL SUBPROGRAM BODIES
 */

/*  FUNCTION NAME: L_HOLD_TIMER_StateMachineProgress
 *  PURPOSE      : Hold timer state machine operation
 *  INPUT        : *timer   - Include short time, long time, current short time,
 *                            current long time, and current state.
 *                            The state value may be:
 *                            IDLE_ST(0),
 *                            WAIT_ST(1),
 *                            TO_WAIT_ST(2),
 *                            TO_IDLE_ST(3)
 *                 event    - event type, the value may be:
 *                            NEW_JOB_EV(0),
 *                            TIME_OUT_EV(1),
 *  OUTPUT       : *timer   - Include short time, long time, current short time,
 *                            current long time and new state.
 *                            New state, the state value may be:
 *                            IDLE_ST(0),
 *                            WAIT_ST(1)
 *  NOTE         : The SM transition table is:
 *
 *                    |              EVENT
 *         STATE      |  NEW_JOB_EV        |  TIME_OUT_EV    |
 *     ---------------+--------------------+-----------------+
 * 0   IDLE_ST        |  TO_WAIT_ST        |  IDLE_ST        |
 *     ---------------+--------------------+-----------------+
 * 1   WAIT_ST        |  WAIT_ST           |  TO_IDLE_ST     |
 *     ---------------+--------------------+-----------------+
 */
static BOOL_T L_HOLD_TIMER_StateMachineProgress(L_HOLD_TIMER_Entry_T *timer, UI32_T event)
{
    UI32_T  current_tick;

    switch (timer->state = L_HOLD_TIMER_SM[timer->state][event])
    {
        case L_HOLD_TIMER_IDLE_ST:
            break;
        case L_HOLD_TIMER_WAIT_ST:
            timer->current_st   = (SYSFUN_GetSysTick() + timer->st);
            break;
        case L_HOLD_TIMER_TO_WAIT_ST:
            current_tick        = SYSFUN_GetSysTick();
            timer->current_lt   = current_tick + timer->lt;
            timer->current_st   = current_tick + timer->st;
            timer->state        = L_HOLD_TIMER_WAIT_ST;
            break;
        case L_HOLD_TIMER_TO_IDLE_ST:
            timer->state        = L_HOLD_TIMER_IDLE_ST;
            return TRUE;
    }
    return FALSE;
}/* End of L_HOLD_TIMER_StateMachineProgress */
/* End of L_HOLD_TIMER.C */
