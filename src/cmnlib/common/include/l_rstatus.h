/*------------------------------------------------------------------------
 * FILE NAME - rstatus.h
 *------------------------------------------------------------------------
 * ABSTRACT : RowStatus operation, defined in rfc1903
 *
 * -----------------------------------------------------------------------
 * Copyright(C) Accton Technology Corporation, 2001

 * -----------------------------------------------------------------------*/

#ifndef L_RSTATUS_H
#define L_RSTATUS_H



/* NAMING CONSTANT DECLARACTION
 */

/*
 * if someone include this file, should also include the rfc 1903
 * leaf file, such as 'leaf_1903' which is automatically generated
 * by snmp mib generated tool like 'epilogue'. In the 'leaf_1903',
 * it will declare the raw status with the following values:
 *
 * #define VAL_RowStatus_active	        1L
 * #define VAL_RowStatus_notInService	2L
 * #define VAL_RowStatus_notReady	    3L
 * #define VAL_RowStatus_createAndGo	4L
 * #define VAL_RowStatus_createAndWait	5L
 * #define VAL_RowStatus_destroy	    6L
 *
 * we define two following extra row status values to cover all the
 * states and actions in the state diagram of rfc 1903. We'll treat
 * the row status values are the union of the states and actions.
 * #define L_RSTATUS_NOT_EXIST	        0
 * #define L_RSTATUS_SET_OTHER_COLUMN	7
 *
 * Note:
 * 1. the L_RSTATUS_NOT_EXIST is defined for the 'state' in the state
 *    diagram. As the states in the diagram is with four states:
 *    'status column does not exist', 'status column is notReady',
 *    'status column is notInService', and 'status column is active'.
      'status column is not exist' is not defined in the 'VAL_RowStatus_xxxxx'.
 * 2. the L_RSTATUS_SET_OTHER_COLUMN is defined for the 'action' in the
 *    state diagram. As the actions in the diagram is with six actions:
 *    'set status column to createAndGo', 'set status column to creaeAndWait',
 *    'set status column to active', 'set status column to notInService',
 *    'set status column to destory', and 'set any other column to some value'
 *    'not exist', 'notReady', 'notInService', 'active'.
 *    'set any other column to some value' is not defined in the
 *    'VAL_RowStatus_xxxxx'.
 *
 */

/* rowstatus value not defined in RFC 1903 */
#define     L_RSTATUS_NOT_EXIST             0
#define     L_RSTATUS_SET_OTHER_COLUMN      7


/*
 * We think of the 'status column is notReady' state and
 * 'status column is notInService' state as the identical state
 * for their functionality. 'status column is notInService' state
 * is removed. Then we still have 'status column does not exist',
 * 'status column is notReady', and 'status column is active'.
 * For each state to state, there will be 3x3=9 transition states,
 * and plus an error condition, are defined as following.
 */

/* rowstatus state transition state constant */
enum L_RSTATUS_TRANSITION_STATE_E
{
    L_RSTATUS_NOTEXIST_2_NOTEXIST=0,
    L_RSTATUS_NOTEXIST_2_NOTREADY,
    L_RSTATUS_NOTEXIST_2_ACTIVE,

    L_RSTATUS_NOTREADY_2_NOTEXIST,      /* Amy modified 10/22*/
    L_RSTATUS_NOTREADY_2_NOTREADY,      /* Amy modified 10/22*/
    L_RSTATUS_NOTREADY_2_ACTIVE,        /* Amy modified 10/22*/

    L_RSTATUS_ACTIVE_2_NOTEXIST,
    L_RSTATUS_ACTIVE_2_NOTREADY,
    L_RSTATUS_ACTIVE_2_ACTIVE,

/*                                      Amy modified. 10/22

    L_RSTATUS_NOTREADY_2_NOTEXIST,
    L_RSTATUS_NOTREADY_2_NOTREADY,
    L_RSTATUS_NOTREADY_2_ACTIVE,
*/
    L_RSTATUS_TRANSITION_STATE_ERROR=100,
};

/* EXPORTED SUBPROGRAM DECLARATIONS
 */
/* -------------------------------------------------------------------------------
 *  ROUTINE NAME  - L_RSTATUS_Fsm
 * -------------------------------------------------------------------------------
 *  FUNCTION:   Finite State Machine of rowstatus operation
 *  INPUT   :   action  - rowstatus action, the action value may be:
 *                        VAL_RowStatus_active(1),
 *                        VAL_RowStatus_notInService(2),
 *                        VAL_RowStatus_createAndGo(4),
 *                        VAL_RowStatus_createAndWait(5),
 *                        VAL_RowStatus_destroy(6),
 *                        L_RSTATUS_SET_OTHER_COLUMN(7)
 *              state   - rowstatus current state, the state value may be:
 *                        L_RSTATUS_NOT_EXIST(0),
 *                        VAL_RowStatus_active(1),
 *                        VAL_RowStatus_notReady(3)
 *
 *              active_check_fun  - Do a sementic check for a conceptual row.
 *                                  It's a callback function for caller to check
 *                                  the current record is valid or not. If it's
 *                                  valid the caller will get VAL_RowStatus_active(1)
 *                                  output state, otherwise VAL_RowStatus_notReady(3)
 *                                  output state is got.
 *
 *
 *  OUTPUT  :   state   - rowstatus new state, the state value may be:
 *                        L_RSTATUS_NOT_EXIST(0),
 *                        VAL_RowStatus_active(1),
 *                        VAL_RowStatus_notReady(3)
 *
 *  RETURN  :   transition_state - defined in L_RSTATUS_TRANSITION_STATE_E
 *  NOTE    :   1. input value with VAL_RowStatus_xxxxx is defined in rfc 1903
 *                 mib leaf 'leaf_1903.h', as explained at the naming constant
 *                 declaratction for L_RSTATUS_NOT_EXIST and
 *                 L_RSTATUS_SET_OTHER_COLUMN.
 *              2. output state with no value for notInService is explained at
 *                 the naming constant for L_RSTATUS_TRANSITION_STATE_E.
 * --------------------------------------------------------------------------------
 */

 UI32_T L_RSTATUS_Fsm(UI32_T action,
                      UI32_T *state,
                      BOOL_T (*active_check_fun)(void*rec),
                      void *rec) ;

#endif /* L_RSTATUS_H */

