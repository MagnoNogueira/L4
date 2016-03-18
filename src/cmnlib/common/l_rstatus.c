/*----------------------------------------------------------------
 * Package Name: rstatus.c

 * Purpose: RowStatus operation
 * ----------------------------------------------------------------
 * Note:
 * ----------------------------------------------------------------
 * History:
 *  1/4/01      - Create
 *  9/25/01     - Ryan Modify
 *      Spec changed. The original spec. is:
 *      UI32_T L_RSTATUS_Fsm(UI32_T action, UI32_T state), and return new state
 * ----------------------------------------------------------------
 * Copyright(C)      Accton Corporation, 2001
 * ----------------------------------------------------------------
 */
 /* INCLUDE FILE DECLARATIONS
 */
 #include "sys_type.h"
 #include "l_rstatus.h"


 /* NAMING CONSTANT DECLARATIONS
 */



/* STATIC VARIABLE DECLARATIONS
 */



/* EXPORTED SUBPROGRAM DECLARATIONS
 */
/*  PURPOSE:    Finite State Machine of rowstatus operation
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
 *  OUTPUT  :   state   - rowstatus new state, the state value may be:
 *                        L_RSTATUS_NOT_EXIST(0),
 *                        VAL_RowStatus_active(1),
 *                        VAL_RowStatus_notReady(3)
 *
 *  NOTE    :   1. input value with VAL_RowStatus_xxxxx is defined in rfc 1903
 *                 mib leaf 'leaf_1903.h', as explained at the naming constant
 *                 declaratction for L_RSTATUS_NOT_EXIST and
 *                 L_RSTATUS_SET_OTHER_COLUMN.
 *              2. output state with no value for notInService is explained at
 *                 the naming constant for L_RSTATUS_TRANSITION_STATE_E.
 *              3. The FSM transition table is:
 *
 *                    |              STATE
 *         EVENT      |    NotExist        |  notReady       |  active
 *     ---------------+--------------------+-----------------+-------------
 * 0   createAndGo    |  ROW_ACTIVECHECK   |  ROW_NOTREADY   |  ROW_ACTIVE
 *     ---------------+--------------------+-----------------+-------------
 * 1   createAndWait  |  ROW_NOTREADY      |  ROW_NOTREADY   |  ROW_ACTIVE
 *     ---------------+--------------------+-----------------+-------------
 * 2   active         |  ROW_NOT_EXIST     |  ROW_ACTIVECHECK|  ROW_ACTIVE
 *     ---------------+--------------------+-----------------+-------------
 * 3   notInService   |  ROW_NOT_EXIST     |  ROW_NOTREADY   |  ROW_NOTREADY
 *     ---------------+--------------------+-----------------+-------------
 * 4   destroy        |  ROW_NOT_EXIST     |  ROW_NOT_EXIST  |  ROW_NOT_EXIST
 *     ---------------+--------------------+-----------------+-------------
 *     Set Other      |  ROW_NOTREADY      |  ROW_NOTREADY   |  ROW_ACTIVECHECK
 * 5   column         |                    |                 |
 *     ---------------+--------------------+-----------------+-------------
 */
UI32_T L_RSTATUS_Fsm(UI32_T action,
                     UI32_T *state,
                     BOOL_T (*active_check_fun)(void*rec),
                     void *rec)
{
    /* NAMING CONSTANT DECLARACTION
     */

    /* rowstatus value defined in RFC 1903 */
    enum
    {
       L_RSTATUS_ACTIVE = 1,
       L_RSTATUS_NOTINSERVICE=2,
       L_RSTATUS_NOTREADY=3,
       L_RSTATUS_CREATEANDGO=4,
       L_RSTATUS_CREATEANDWAIT=5,
       L_RSTATUS_DESTROY=6,
       L_RSTATUS_SET_OTHER = L_RSTATUS_SET_OTHER_COLUMN,    /* Not RFC value */
       L_RSTATUS_ACTIVECHECK=8                              /* Not RFC value */
    };

    /* internal event definition */
    enum
    {
        CREATEANDGO_EVENT =0,
        CREATEANDWAIT_EVENT,
        ACTIVE_EVENT,
        NOTINSERVICE_EVENT,
        DESTROY_EVENT,
        SETOTHERCOLUMN_EVENT,
    };


    /* internal state definition */
    enum
    {
        NOTEXIST_ST = 0,
        NOTREADY_ST,
        ACTIVE_ST,
    };

    /* LOCAL VARIABLES DECLARACTION
     */
    UI32_T  old_state = *state;
    UI32_T  transition_action;


    const static UI32_T action_to_event_map[] ={
       /* L_RSTATUS_NOT_EXIST = 0    */    DESTROY_EVENT,   /* INVALID */
       /* L_RSTATUS_ACTIVE = 1       */    ACTIVE_EVENT,
       /* L_RSTATUS_NOTINSERVICE=2   */    NOTINSERVICE_EVENT,
       /* L_RSTATUS_NOTREADY=3       */    DESTROY_EVENT,   /* INVALID */
       /* L_RSTATUS_CREATEANDGO=4    */    CREATEANDGO_EVENT,
       /* L_RSTATUS_CREATEANDWAIT=5  */    CREATEANDWAIT_EVENT,
       /* L_RSTATUS_DESTROY=6        */    DESTROY_EVENT,
       /* L_RSTATUS_SET_OTHER =7     */    SETOTHERCOLUMN_EVENT,
    };


    const static UI32_T rstatus_to_state_map[] ={
       /* L_RSTATUS_NOT_EXIST = 0    */    NOTEXIST_ST,
       /* L_RSTATUS_ACTIVE = 1       */    ACTIVE_ST,
       /* L_RSTATUS_NOTINSERVICE=2   */    NOTEXIST_ST,
       /* L_RSTATUS_NOTREADY=3       */    NOTREADY_ST,
       /* L_RSTATUS_CREATEANDGO=4    */    NOTEXIST_ST,
       /* L_RSTATUS_CREATEANDWAIT=5  */    NOTEXIST_ST,
       /* L_RSTATUS_DESTROY=6        */    NOTEXIST_ST,
       /* L_RSTATUS_SET_OTHER =7     */    NOTEXIST_ST,
    };


    const static UI32_T row_status_Table [6] [3]  =
    {
       {L_RSTATUS_ACTIVECHECK   ,  L_RSTATUS_NOTREADY   ,  L_RSTATUS_ACTIVE      },
       {L_RSTATUS_NOTREADY      ,  L_RSTATUS_NOTREADY   ,  L_RSTATUS_ACTIVE      },
       {L_RSTATUS_NOT_EXIST     ,  L_RSTATUS_ACTIVECHECK,  L_RSTATUS_ACTIVE      },
       {L_RSTATUS_NOT_EXIST     ,  L_RSTATUS_NOTREADY   ,  L_RSTATUS_NOTREADY    },
       {L_RSTATUS_NOT_EXIST     ,  L_RSTATUS_NOT_EXIST  ,  L_RSTATUS_NOT_EXIST   },
       {L_RSTATUS_NOTREADY      ,  L_RSTATUS_NOTREADY   ,  L_RSTATUS_ACTIVECHECK }
    };
    /* check action, state */
    if ( action > L_RSTATUS_SET_OTHER || old_state > L_RSTATUS_SET_OTHER )
        return L_RSTATUS_TRANSITION_STATE_ERROR;
    /* End of if */

    *state = row_status_Table [action_to_event_map[action]]
             [ transition_action = rstatus_to_state_map[old_state] ];

    if ( *state == L_RSTATUS_ACTIVECHECK )
    {
        if ( (*active_check_fun)(rec) )
            *state = L_RSTATUS_ACTIVE;
        else *state = L_RSTATUS_NOTREADY;

    }

    return transition_action*3 + rstatus_to_state_map[ *state];
}

#if 0

static buffer [80];
BOOL_T Sementic_Check(char * rec)
{
    return 1;
}


main()
{
    int state=0, action;

    action = atoi ( gets(buffer) );

    if (action == 0 ) return;

    switch ( L_RSTATUS_Fsm( action, &state, Sementic_Check, 0) )
    {
    case L_RSTATUS_NOTEXIST_2_NOTEXIST:
        break;
    case L_RSTATUS_NOTEXIST_2_ACTIVE:
        break;
    case L_RSTATUS_NOTEXIST_2_NOTREADY:
        break;

    case L_RSTATUS_ACTIVE_2_NOTEXIST:
        break;
    case L_RSTATUS_ACTIVE_2_ACTIVE:
        break;
    case L_RSTATUS_ACTIVE_2_NOTREADY:
        break;

    case L_RSTATUS_NOTREADY_2_NOTEXIST:
        break;
    case L_RSTATUS_NOTREADY_2_ACTIVE:
        break;
    case L_RSTATUS_NOTREADY_2_NOTREADY:
        break;
    case L_RSTATUS_TRANSITION_ACTION_ERROR:
        break;
    }
}


#endif
