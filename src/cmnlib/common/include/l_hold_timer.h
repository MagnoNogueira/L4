/* MODULE NAME: l_hold_timer.h
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
#ifndef _L_HOLD_TIMER_H
#define _L_HOLD_TIMER_H


/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

typedef struct
{
    UI32_T  st;             /* setting short_time (tick)                 */
    UI32_T  lt;             /* setting long_time (tick)                  */
    UI32_T  current_st;     /* record current system_time for short_time */
    UI32_T  current_lt;     /* record current system_time for long_time  */
    UI32_T  state;          /* current SM state                          */
} L_HOLD_TIMER_Entry_T;

enum L_HOLD_TIMER_EV_E
{
    L_HOLD_TIMER_NEW_JOB_EV = 0,
    L_HOLD_TIMER_TIME_OUT_EV,
    L_HOLD_TIMER_PERIODIC_UPDATE_EV
};

/* LOCAL SUBPROGRAM DECLARATIONS
 */

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
void L_HOLD_TIMER_InitHoldTimer(L_HOLD_TIMER_Entry_T *timer, UI32_T init_st, UI32_T init_lt);


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
BOOL_T L_HOLD_TIMER_Handler(L_HOLD_TIMER_Entry_T *timer, UI32_T event);


#endif /* _L_HOLD_TIMER_H */
