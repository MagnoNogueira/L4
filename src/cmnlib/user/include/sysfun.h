/* Module Name: SYSFUN.H
 * Purpose:
 *      This module is unified system call for all platforms.
 *
 * Notes:
 *      1. base on API declared on SYSFUN, no porting is needed except
 *         SYSFUN_xxx().
 *      2. All SYSFUN_xxx() base on VxWorks's system function, so no SYSFUN.C
 *         is needed for normal system function. If some composited functions
 *         must be created, we will add the function in SYSFUN.C
 *      3. Use event, timer must call SYSFUN_Init to hook event function,
 *         active timer task to handle the API.
 *      4. Following modification is needed for use SYSFUN.
 *          a. Please add SYSFUN_Init () to root to initialize System Function Call.
 *          b. Please add SYSFUN_CreateTimerTask () to create Periodica Timer task.
 *          c. SYSFUN_TickAnnounce () should add to usrClock() in usrConfig.c to trigger
 *             tick events.
 *      5. Before use SYSFUN, SYSFUN_Init() must execute, then SYSFUN_CreateTimerTask().
 *          a. For event application, SYSFUN_Init() when system is up.
 *             and SYSFUN_Terminate() when no more event is needed.
 *          b. For timer application, SYSFUN_Init() and SYSFUN_CreateTimerTask() is needed
 *             for event & timer, and install SYSFUN_TickAnnounce() to system tick ISR.
 *          c. SYSFUN_Terminate() will destroy TimerTask, because timer task use event function.
 *
 * History:
 *       Date       --  Modifier,   Reason
 *  0.2 2001.8.16   --  William,    Add semaphore functions and add description.
 *  0.3 2001.8.26   --  William,    VxWorks platform
 *  0.4 2001.9.04   --  William,    Patch event_init. (Notes.5), & named constants
 *  0.5 2001.9.23   --  William,    Change timeAnnounce using signal. (not this version)
 *  0.6 2001.9.25   --  William,    Use Watch-Dog Timer to implement periodic timer.
 *  0.6a2001.9.29   --  William,    Watch-Dog dependent periodic timer.
 *  0.7 2001.10.29  --  William,    Add SYSFUN_EnableRoundRonbin/SYSFUN_DisableRoundRonbin,
 *                                  SYSFUN_SetTaskPriority, DBG_DumpHex, DBG_PrintText
 *  0.8  2001.11.14 --  William,    SYSFUN_SendEvent(), check tid <>0, to avoid mis-use.
 *  0.9  2002.01.08  -- William,    Mask off error message display for DBG_Print.
 *  0.10 2002.01.23 --  William,    Add SYSFUN_TaskIDToName()
 *  1.1  2002.06.20 --  William,    add macro SYSFUN_Om_WriteLock(), SYSFUN_Om_ReleaseWriteLock
 *                                  and SYSFUN_FlushSem(), SYSFUN_Mgr_EnterCriticalSection(),
 *                                  SYSFUN_Mgr_LeaveCriticalSection() for object lock.
 *  1.02 2002.08.06 --  William,    Add (enter/leave MGR macro)
 *  1.04 2002.09.21 --  William,    Add SYSFUN_Register_Ticks_Callback() for LED task to periodical callback,
 *                                  only one function can be registered in the implementation.
 *  1.05 2002.10.23 --  William,    Add SYSFUN_ConvertTaskIdToIndex() to convert task-id to sequential index.
 *                                  the index can be used as error-code log, task-related record.
 *                                  add a global variable for stacking developing issue,SYSFUN_PrintRefCounterFileName.
 *                                  this variable will be removed after stacking developing is done.
 *  1.06 2004.04.20 --  Penny,      in SYSFUN_TASK_HALT(), remove halting system; instead just log error message.
 *       2007.04.16 --  Wakka,      Add SYSFUN_OM_Operation().
 *       2007.05.29 --  Wakka,      Change the design of message queue.
 *                                  APIs: SYSFUN_CreateMsgQ(), SYSFUN_GetMsgQ(), SYSFUN_DestroyMsgQ()
 *                                        SYSFUN_GetMsgQOwner(), SYSFUN_SendRequestMsg(),
 *                                        SYSFUN_SendResponseMsg(), SYSFUN_ReceiveMsg()
 *
 * Copyright(C)      Accton Corporation, 1999, 2000, 2001, 2002, 2004, 2006
 */

#ifndef     _SYSFUN_H
#define     _SYSFUN_H

/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sys_type.h"
#include "sys_adpt.h"
#include "sys_cpnt.h"
#include "sysfun_type.h"

/*	developing flag	*/
#define	SYSTEM_IN_DEVELOP       0
#define SYSFUN_DEBUG            TRUE

/* The description of the message queue.
 */
typedef struct
{
    UI32_T owner_tid;
    UI32_T request_qid;
    UI32_T response_qid; /* If msgq_type is SYSFUN_MSGQ_UNIDIRECTIONAL,
                          * response_qid will be set to SYSFUN_MSGQ_INVALID_ID.
                          */
    UI32_T msgq_key;
} SYSFUN_MsgQDesc_T;

#if defined (SYSFUN_DEBUG) && (SYSFUN_DEBUG == TRUE)
// # define SYSFUN_DEBUG_CSC_MODE_TRANSITION           1

/*   CHECK_ENTER_TRANSITION_MODE_COUNTER : enable counter checking for enter-transition-mode.
 *                  this declaration is used for developing stage only.
 */
# define CHECK_ENTER_TRANSITION_MODE_COUNTER        1

/*   SYSFUN_RD_DEBUG_MEM_MONITOR: If set TRUE: Engineers may monitor the change of a value
 *                                at a given memory address is performed by
 *                                which task. This mechanism can be set through
 *                                sysfun backdoor. This is useful for tracing
 *                                memory corruption issues. Note that it is
 *                                only useful to use ICE with this option.
 *                                This option MUST BE SET AS FALSE on RELEASE BUILD or it will
 *                                impede system performance.
 */
#define SYSFUN_RD_DEBUG_MEM_MONITOR             FALSE

#if 0
/*   SYSFUN_RD_DEBUG_ENABLE_DUMP_TASK_STACK: If set TRUE: Engineers can have a item in
 *                                           sysfun backdoor menu to dump
 *                                           call stack of the specified task.
 *                                           This might be useful if you doubt
 *                                           that the task is always pending
 *                                           and need to know where does that
 *                                           task excutes currently.
 *                                           With this compiler option, a
 *                                           debug API SYSFUN_Debug_DumpTaskStack()
 *                                           will also be provided.
 *                                           This option MUST BE SET AS FALSE on RELEASE BUILD
 *                                           or it will impede system performance.
 */
#define SYSFUN_RD_DEBUG_ENABLE_DUMP_TASK_STACK  TRUE
#endif

/*   SYSFUN_RD_DEBUG_SHOW_ERROR_MSG: If set TRUE: It will show error message when some critical
 *                                                sysfun api function calls fail.
 */
#define SYSFUN_RD_DEBUG_SHOW_ERROR_MSG          TRUE
# define SYSFUN_DEBUG_ERROR_CHECK               TRUE
# define SYSFUN_DEBUG_MSGQ_TIMEOUT              TRUE
#endif /* SYSFUN_DEBUG */

#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
# define SYSFUN_MSGQ_DEBUG_SEND_REQ             (1 << 0)
# define SYSFUN_MSGQ_DEBUG_RECV_REQ_RESPONSE    (1 << 1)
# define SYSFUN_MSGQ_DEBUG_SEND_RESPONSE        (1 << 2)
# define SYSFUN_MSGQ_DEBUG_RECV                 (1 << 3)
# define SYSFUN_MSGQ_DEBUG_SEND_REQ_MSG         (SYSFUN_MSGQ_DEBUG_SEND_REQ | \
                                                 SYSFUN_MSGQ_DEBUG_RECV_REQ_RESPONSE)
# define SYSFUN_MSGQ_DEBUG_ALL                  (SYSFUN_MSGQ_DEBUG_SEND_REQ |           \
                                                 SYSFUN_MSGQ_DEBUG_RECV_REQ_RESPONSE |  \
                                                 SYSFUN_MSGQ_DEBUG_SEND_RESPONSE |      \
                                                 SYSFUN_MSGQ_DEBUG_RECV                 \
                                                )
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */

/* NAMING CONSTANT DECLARATIONS
 */

/*  TASK option */
#define SYSFUN_TASK_NO_FP               0x00        /* not needs floating point supporting */
#define SYSFUN_TASK_FP                  0x08        /* needs floating point supporting */

#define SYSFUN_TASK_LOWEST_PRIORITY     238

/*  Semaphore       */
#define SYSFUN_SEM_FIFO                 0x00        /*  create a FIFO semaphore */
#define SYSFUN_SEM_PRIORITY             0x01        /*  create a Priority semaphore */
#define SYSFUN_SEM_INVERSION_SAFE       0x08        /* no priority inversion (mutex semaphore opt.) */

/*  Message Queue  */
#define SYSFUN_MSGQ_UNIDIRECTIONAL      0x00        /* create a unidirectional message queue. */
#define SYSFUN_MSGQ_BIDIRECTIONAL       0x01        /* create a bidirectional message queue. */

/*  System Event Definitions */
/* SYSFUN_SYSTEM_EVENT_RESUME_TASK
 * used by sysfun internally to implement task suspend and task resume
 */
#define SYSFUN_SYSTEM_EVENT_RESUME_TASK             BIT_31

/* SYSFUN_SYSTEM_EVENT_IPCFAIL
 * This sytem event must be sent when the async callback ipc msg failed.
 * The lower layer CSC must write the fail info to IPCFAIL and send this
 * event to upper layer CSC Group task.
 */
#define SYSFUN_SYSTEM_EVENT_IPCFAIL                 BIT_30

/* SYSFUN_SYSTEM_EVENT_IPCMSG
 * When a upper layer CSC sends a sync ipc msg to lower layer CSC ipc msgq,
 * upper layer CSC task need to send this event to lower layer CSC task.
 */
#define SYSFUN_SYSTEM_EVENT_IPCMSG                  BIT_29

/* SYSFUN_SYSTEM_EVENT_SET_TRANSITION_MODE
 * This event is sent by STKCTRL to nofiy all CSC to set transition mode.
 * Each CSC group task must handle this event and invoke SetTransitionMode
 * functions of all CSC in this CSC group immediately.
 */
#define SYSFUN_SYSTEM_EVENT_SET_TRANSITION_MODE     BIT_28

/* SYSFUN_SYSTEM_EVENT_SW_WATCHDOG
 * This event is sent by software watchdog to nofiy all CSC
 * Each task must handle this event and invoke SW_WATCHDOG_MGR_ResetTimer
 * to reset its monitor counter.
 */
#define SYSFUN_SYSTEM_EVENT_SW_WATCHDOG             BIT_27


/*  Message Queue Option    */
#define SYSFUN_MSG_FIFO                 0x00        /*  Create a FIFO message queue */
#define SYSFUN_MSG_PRIORITY             0x01        /*  Create a Priority based message queue   */
                                                    /*      Urgent message will be passed first */

/*  Enter/leave MGR checking	*/
#define CSC_MGR_IN_TRANSITION_MODE                 0x80808080
#define SYSFUN_ENTER_TRANSITION_MODE_WAIT_PERIOD   5

/* constant for semaphore
 */
#define SYSFUN_SEMKEY_PRIVATE   0
#define SYSFUN_MSGQKEY_PRIVATE  0

/* UART devices.
 */
#define SYSFUN_UART_CHANNEL1    0
#define SYSFUN_UART_CHANNEL2    1




/* Command ID of syscall for external use.
 */
enum SYSFUN_SYSCALL_ID_E
{
    SYSFUN_SYSCALL_INTERNAL = 0,
    SYSFUN_SYSCALL_L_IPCMEM,
    SYSFUN_SYSCALL_L_MM,
    SYSFUN_SYSCALL_PHYADDR_ACCESS,
    SYSFUN_SYSCALL_PHYSICAL_ADDR_ACCESS,    /* anzhen.zheng, 2/19/2008 */
    SYSFUN_SYSCALL_I2C,
    SYSFUN_SYSCALL_SYSJOBQ,                 /* unused */
    SYSFUN_SYSCALL_IML_MGR,
    SYSFUN_SYSCALL_VLAN_MGR,
    SYSFUN_SYSCALL_AMTRL3_MGR,
    SYSFUN_SYSCALL_ROUTE_MGR,
    SYSFUN_SYSCALL_DYING_GASP,
    SYSFUN_SYSCALL_VLAN_NET
};

/* MACRO FUNCTION DECLARATIONS
 */
/*
 * Debugging macros
 */
#if defined (SYSFUN_DEBUG) && (SYSFUN_DEBUG == TRUE)
//# define SYSFUN_DBG_PRINTF(fmt, args...)    __SYSFUN_DBG_Printf(__func__, __LINE__, fmt, ##args)
//# define SYSFUN_ASSERT(cond)                __SYSFUN_DBG_Assert(__func__, __LINE__, (int)(cond), # cond)
# define SYSFUN_DBG_PRINTF(...)             do {} while (0)
# define SYSFUN_ASSERT(cond)                do {} while (0)
#else
# define SYSFUN_DBG_PRINTF(...)             do {} while (0)
# define SYSFUN_ASSERT(cond)                do {} while (0)
#endif

#ifdef SYSFUN_DEBUG_CSC_MODE_TRANSITION
# define SYSFUN_DBG_CSC_MODE_TRAN           SYSFUN_DBG_PRINTF
#else
# define SYSFUN_DBG_CSC_MODE_TRAN(...)      do {} while (0)
#endif


/*
 *	Define Macro for general enter critical section using semaphore.
 */
#define SYSFUN_ENTER_CRITICAL_SECTION(sem_id,timeout)       SYSFUN_TakeSem(sem_id,timeout)
#define SYSFUN_LEAVE_CRITICAL_SECTION(sem_id)               SYSFUN_GiveSem(sem_id)

/* FUNCTION NAME : SYSFUN_OM_ENTER_CRITICAL_SECTION
 * PURPOSE:
 *      In OM, just non-preempty to prevent other tasks to access the OM,
 *      and as soon as possible to finish all OM access-operation.
 *
 * INPUT:
 *      sem_id - The semaphore to take.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      original priority.
 *
 * NOTES:
 *      Caller MUST keep the original priority, so as to restore the priority
 *      when calling SYSFUN_OM_LEAVE_CRITICAL_SECTION().
 */
#define SYSFUN_OM_ENTER_CRITICAL_SECTION(sem_id) ({        \
    UI32_T __orig_priority = 0;                            \
                                                           \
    SYSFUN_TakeSem((sem_id), SYSFUN_TIMEOUT_WAIT_FOREVER); \
    __orig_priority;                                       \
})

/* FUNCTION NAME : SYSFUN_OM_LEAVE_CRITICAL_SECTION
 * PURPOSE:
 *      In OM, just non-preempty to prevent other tasks to access the OM,
 *      and as soon as possible to finish all OM access-operation.
 *
 * INPUT:
 *      sem_id - The semaphore to give.
 *      orig_priority - The priority to restore.
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
#define SYSFUN_OM_LEAVE_CRITICAL_SECTION(sem_id, orig_priority) do { \
    orig_priority = 0;                                               \
    SYSFUN_GiveSem(sem_id);                                          \
} while (0)

/* MACRO NAME : SYSFUN_DECLARE_CSC
 * PURPOSE:
 *      Declare constant and static variable for MGR enter/leave.
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. This Macro should be used in declaration part, ie. Local variable declaration.
 */
#define	SYSFUN_DECLARE_CSC  \
    static SYS_TYPE_Stacking_Mode_T  ___csc_mgr_operating_mode = SYS_TYPE_STACKING_TRANSITION_MODE;

/* MACRO NAME : SYSFUN_DECLARE_CSC_ON_SHMEM
 * PURPOSE:
 *      Declare fields for MGR enter/leave on shared memory.
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. This Macro should be used in the definition of the structure which
 *         will be used on shared memory.
 *      2. It is required to use SYSFUN_INITIATE_CSC_ON_SHMEM to initialize.
 */
#define	SYSFUN_DECLARE_CSC_ON_SHMEM \
    SYS_TYPE_Stacking_Mode_T ___csc_mgr_operating_mode;

/* MACRO NAME : SYSFUN_INITIATE_CSC_ON_SHMEM
 * PURPOSE:
 *      Initiate fields for MGR enter/leave on shared memory.
 * INPUT:
 *      shmem_data_p  --  pointer to the structure used on shared memory.
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
#define SYSFUN_INITIATE_CSC_ON_SHMEM(shmem_data_p) do {                            \
    (shmem_data_p)->___csc_mgr_operating_mode = SYS_TYPE_STACKING_TRANSITION_MODE; \
    SYSFUN_DBG_CSC_MODE_TRAN("Initiate CSC on SHMEM\n");                           \
} while (0)

/* MACRO NAME : SYSFUN_GET_CSC_OPERATING_MODE
 * PURPOSE:
 *      Get current CSC operation mode.
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      operation mode, one of {SYS_TYPE_STACKING_TRANSITION_MODE, SYS_TYPE_STACKING_MASTER_MODE,
 *                              SYS_TYPE_STACKING_SLAVE_MODE}
 *
 * NOTES:
 *      None.
 */
#define SYSFUN_GET_CSC_OPERATING_MODE()    (___csc_mgr_operating_mode)

/* MACRO NAME : SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM
 * PURPOSE:
 *      Get current CSC operation mode from shared memory.
 * INPUT:
 *      shmem_data_p  --  pointer to the structure used on shared memory.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      operation mode, one of {SYS_TYPE_STACKING_TRANSITION_MODE, SYS_TYPE_STACKING_MASTER_MODE,
 *                              SYS_TYPE_STACKING_SLAVE_MODE}
 *
 * NOTES:
 *      None.
 */
#define SYSFUN_GET_CSC_OPERATING_MODE_ON_SHMEM(shmem_data_p) \
    ((shmem_data_p)->___csc_mgr_operating_mode)

/* MACRO NAME : SYSFUN_SET_TRANSITION_MODE
 * PURPOSE:
 *      Set component mode to Transition.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. This Macro should be used in each time, component's MGR enter transition mode.
 */
#define SYSFUN_SET_TRANSITION_MODE()    do {                        \
    ___csc_mgr_operating_mode = SYS_TYPE_STACKING_TRANSITION_MODE;  \
    SYSFUN_DBG_CSC_MODE_TRAN("Set transition mode\n");              \
} while (0)

/* MACRO NAME : SYSFUN_SET_TRANSITION_MODE_ON_SHMEM
 * PURPOSE:
 *      Set component mode to Transition.
 * INPUT:
 *      shmem_data_p  --  pointer to the structure used on shared memory.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. This Macro should be used in each time, component's MGR enter transition mode.
 */
#define SYSFUN_SET_TRANSITION_MODE_ON_SHMEM(shmem_data_p) do {                      \
    (shmem_data_p)->___csc_mgr_operating_mode = SYS_TYPE_STACKING_TRANSITION_MODE;  \
    SYSFUN_DBG_CSC_MODE_TRAN("Set transition mode on SHMEM\n");                     \
} while (0)

/* MACRO NAME : SYSFUN_ENTER_TRANSITION_MODE
 * PURPOSE:
 *      Do enter transition mode pre-operation, before proceed transition function.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. This Macro should be used in each time, component's MGR enter transition mode.
 */
#define SYSFUN_ENTER_TRANSITION_MODE()  do {                        \
    ___csc_mgr_operating_mode = SYS_TYPE_STACKING_TRANSITION_MODE;  \
    SYSFUN_DBG_CSC_MODE_TRAN("Enter transition mode\n");            \
} while (0)

/* MACRO NAME : SYSFUN_ENTER_TRANSITION_MODE_ON_SHMEM
 * PURPOSE:
 *      Do enter transition mode pre-operation, before proceed transition function.
 * INPUT:
 *      shmem_data_p  --  pointer to the structure used on shared memory.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. This Macro should be used in each time, component's MGR enter transition mode.
 */
#define SYSFUN_ENTER_TRANSITION_MODE_ON_SHMEM(shmem_data_p)  do {                   \
    (shmem_data_p)->___csc_mgr_operating_mode = SYS_TYPE_STACKING_TRANSITION_MODE;  \
    SYSFUN_DBG_CSC_MODE_TRAN("Enter transition mode on SHMEM\n");                   \
} while (0)


/* MACRO NAME : SYSFUN_ENTER_MASTER_MODE
 * PURPOSE:
 *      Do enter master mode pre-operation, before proceed master function.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. This Macro should be used in each time, component's MGR enter master mode.
 */
#define SYSFUN_ENTER_MASTER_MODE()  do {                        \
    ___csc_mgr_operating_mode = SYS_TYPE_STACKING_MASTER_MODE;  \
    SYSFUN_DBG_CSC_MODE_TRAN("Enter master mode\n");            \
} while (0)

/* MACRO NAME : SYSFUN_ENTER_MASTER_MODE_ON_SHMEM
 * PURPOSE:
 *      Do enter master mode pre-operation, before proceed master function.
 *
 * INPUT:
 *      shmem_data_p  --  pointer to the structure used on shared memory.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. This Macro should be used in each time, component's MGR enter master mode.
 */
#define SYSFUN_ENTER_MASTER_MODE_ON_SHMEM(shmem_data_p) do {                    \
    (shmem_data_p)->___csc_mgr_operating_mode = SYS_TYPE_STACKING_MASTER_MODE;  \
    SYSFUN_DBG_CSC_MODE_TRAN("Enter master mode on SHMEM\n");                   \
} while (0)

/* MACRO NAME : SYSFUN_ENTER_SLAVE_MODE
 * PURPOSE:
 *      Do enter slave mode pre-operation, before proceed slave function.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. This Macro should be used in each time, component's MGR enter slave mode.
 */
#define SYSFUN_ENTER_SLAVE_MODE()   do {                        \
    ___csc_mgr_operating_mode = SYS_TYPE_STACKING_SLAVE_MODE;   \
    SYSFUN_DBG_CSC_MODE_TRAN("Enter slave mode\n");             \
} while (0)

/* MACRO NAME : SYSFUN_ENTER_SLAVE_MODE_ON_SHMEM
 * PURPOSE:
 *      Do enter slave mode pre-operation, before proceed slave function.
 *
 * INPUT:
 *      shmem_data_p  --  pointer to the structure used on shared memory.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. This Macro should be used in each time, component's MGR enter slave mode.
 */
#define SYSFUN_ENTER_SLAVE_MODE_ON_SHMEM(shmem_data_p) do {                     \
    (shmem_data_p)->___csc_mgr_operating_mode = SYS_TYPE_STACKING_SLAVE_MODE;   \
    SYSFUN_DBG_CSC_MODE_TRAN("Enter slave mode on SHMEM\n");                    \
} while (0)

/* MACRO NAME : SYSFUN_TASK_ENTER_TRANSITION_MODE
 * PURPOSE:
 *		Leave CSC Task while transition done.
 * INPUT:
 *		transition_done_flag 	-- the flag indicates the transition done.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *     None.
 *
 */
#define SYSFUN_TASK_ENTER_TRANSITION_MODE(transition_done_flag)  do {       \
    SYSFUN_DBG_CSC_MODE_TRAN("Task enter transition mode ... waiting\n");   \
    while ((volatile int)transition_done_flag != TRUE) {                    \
        SYSFUN_Sleep(3);                                                    \
    }                                                                       \
    SYSFUN_DBG_CSC_MODE_TRAN("Task enter transition mode ... done\n");      \
} while (0)

/* MACRO NAME : SYSFUN_TASK_ENTER_TRANSITION_MODE_ON_SHMEM
 * PURPOSE:
 *		Leave CSC Task while transition done.
 * INPUT:
 *		transition_done_flag 	-- the flag indicates the transition done.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      None.
 *
 */
#define SYSFUN_TASK_ENTER_TRANSITION_MODE_ON_SHMEM(transition_done_flag) \
    SYSFUN_TASK_ENTER_TRANSITION_MODE(transition_done_flag)

/*-------------------------------------------------------------------------
 * MACRO NAME - SYSFUN_SIZE_OF_MSG
 * ------------------------------------------------------------------------
 * PURPOSE  :   Calculate the size of the total message.
 * INPUT    :   msg_size - The size of msg_buf.
 * OUTPUT   :   None.
 * RETURN   :   The size of the total message.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
#define SYSFUN_SIZE_OF_MSG(msg_size) ((UI32_T)(((SYSFUN_Msg_T *)0)->msg_buf) + (UI32_T)(msg_size))

/*	System develop macro.
 */
#ifdef SYSFUN_NOTUSED
#if (SYS_CPNT_PRINTF == FALSE)
    #define SYSFUN_Debug_Printf   SYSFUN_NullPrintf
#elif SYS_CPNT_PRINTF == TRUE
    #define SYSFUN_Debug_Printf   printf
#endif
#endif /* SYSFUN_NOTUSED */

#define SYSFUN_Snprintf  snprintf
#define SYSFUN_Strncat  strncat
#define SYSFUN_Sprintf              sprintf

//#ifdef SYSFUN_NOTUSED
#define SYSFUN_Scanf                scanf
#define SYSFUN_Sscanf               sscanf
//#endif /* SYSFUN_NOTUSED */

/* XXX do not use following two macros again, they are not recommended
 * use SYSFUN_DBG_PRINTF() instead. Out long term target is removing
 * SYSFUN_Debug_Printf() and DBG_PrintText().
 * -- 2008-08-05, steven.jiang
 */
#if defined (SYSFUN_DEBUG) && (SYSFUN_DEBUG == TRUE)
// # define SYSFUN_Debug_Printf(fmt, args...)  __SYSFUN_DBG_Printf(__func__, __LINE__, fmt, ##args)
// # define DBG_PrintText(fmt, args...)        __SYSFUN_DBG_Printf(__func__, __LINE__, fmt, ##args)
# define SYSFUN_Debug_Printf(...)           do {} while (0)
# define DBG_PrintText(...)                 do {} while (0)
#else
# define SYSFUN_Debug_Printf(...)           do {} while (0)
# define DBG_PrintText(...)                 do {} while (0)
#endif


/* DATA TYPE DECLARATIONS
 */
typedef  void (*VOID_FUNCTION)(void);

typedef enum
{
    SYSFUN_UART_BAUDRATE_9600 = 9600,
    SYSFUN_UART_BAUDRATE_19200 = 19200,
    SYSFUN_UART_BAUDRATE_38400 = 38400,
    SYSFUN_UART_BAUDRATE_57600 = 57600,
    SYSFUN_UART_BAUDRATE_115200 = 115200
} SYSFUN_Uart_BaudRate_T;


typedef enum
{
    SYSFUN_UART_PARITY_NONE = 0,
    SYSFUN_UART_PARITY_EVEN,
    SYSFUN_UART_PARITY_ODD,
    SYSFUN_UART_PARITY_MARK,
    SYSFUN_UART_PARITY_SPACE
} SYSFUN_Uart_Parity_T;

typedef enum
{
    SYSFUN_UART_DATA_LENGTH_5_BITS = 5,
    SYSFUN_UART_DATA_LENGTH_6_BITS = 6,
    SYSFUN_UART_DATA_LENGTH_7_BITS = 7,
    SYSFUN_UART_DATA_LENGTH_8_BITS = 8
} SYSFUN_Uart_Data_Length_T;

typedef enum
{
    SYSFUN_UART_STOP_BITS_1_BITS   = 1,
    SYSFUN_UART_STOP_BITS_2_BITS   = 2
} SYSFUN_Uart_Stop_Bits_T;

/* The definition of message for msgq.
 * NOTE: Use SYSFUN_SIZE_OF_MSG() to calculate the size of the total message.
 */
typedef struct
{
    UI32_T msg_type;         /* The type of this message.
                              * For unidirectional msg queue, the receiver can
                              *   specify type to receive the first message matched the type.
                              * For bidirectional msg queue, msg_type is reserved for storing
                              *   the id of the task that sent the request.
                              */
    UI16_T cmd;              /* User custom command id. */
    UI16_T msg_size;         /* The size of msg_buf. */
    UI8_T  msg_buf[1];       /* msg_buf: start address of buffer.
                              *          to store the message payload.
                              */
} SYSFUN_Msg_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/* FUNCTION NAME : DBG_Print
 * PURPOSE:
 *      Print a message to special device in release-version.
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. Only debug-in-release function could use this function to display on
 *         screen for notification.
 */
void    DBG_Print (char *text);


/* FUNCTION NAME : DBG_DumpHex
 * PURPOSE:
 *      Dump heximal code on screen, this function should be used in debug mode only.
 * INPUT:
 *      title	-- text displays on screen.
 *		len		-- data length to be displayed.
 *		buffer	-- buffer holding displaying data.
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
void    DBG_DumpHex (char *title, int len, char *buffer);

/* FUNCTION NAME : SYSFUN_Init
 * PURPOSE:
 *      Initialize system function working environment.
 *
 * INPUT:
 *      None.
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
void SYSFUN_Init (void);

/*-------------------------
 * Task management funtion
 *-------------------------
 */


/* FUNCTION NAME : SYSFUN_SpawnProcess
 * PURPOSE:
 *      To spawn a new process.
 *
 * INPUT:
 *      process_priority-- range from 1 to 255, 255 is lowest priority, 255 for
 *                         IDLE task.
 *      sched_policy    -- scheduling policy setting, valid settings are listed below:
 *                         SYSFUN_SCHED_DEFAULT  -- use default policy
 *                         SYSFUN_SCHED_FIFO     -- use first-in-first-out policy
 *                         SYSFUN_SCHED_RR       -- use round-robin policy
 *      process_name    -- process name
 *      func_ptr        -- function entry of the process.
 *                         the function protype must be -> void func(void*)
 *      arg             -- parameter passed to function entry.
 * OUTPUT:
 *      process_id      -- process identifier
 *
 * RETURN:
 *      SYSFUN_OK                     -- Successfully.
 *      0x08                          -- task-object table full.
 *      SYSFUN_RESULT_NO_SPACE_4_TASK -- Insufficient space to create task.
 *      SYSFUN_RESULT_STACK_TOO_SMALL -- Stack size too small.
 *      SYSFUN_RESULT_PRIO_OUT_RANGE  -- Priority out of range.
 *      SYSFUN_RESULT_INVALID_ARG     -- Invalid argument
 *      SYSFUN_RESULT_ERROR           -- Unknwon error
 *
 * NOTES:
 *      1.The process_name must be unique within the system. This API will
 *        fail if the given process name already exists.
 *      2.As for sched_policy on linux: SYSFUN_SCHED_DEFAULT will use default policy
 *        SYSFUN_SCHED_FIFO will use SCHED_FIFO as real-time process
 *        SYSFUN_SCHED_RR will use SCHED_RR as real-time process
 *
 */
UI32_T  SYSFUN_SpawnProcess(UI32_T process_priority,
                            UI32_T sched_policy,
                            char   *process_name,
                            void   *func_ptr,
                            void   *arg,
                            UI32_T *process_id);

/* FUNCTION NAME : SYSFUN_SpawnThread
 * PURPOSE:
 *      Spawn a new thread from the running process.
 *
 * INPUT:
 *      thread_priority-- range from 1 to 255, 255 is lowest priority, 255 for
 *                         IDLE task.
 *      sched_policy    -- scheduling policy setting, valid settings are listed below:
 *                         SYSFUN_SCHED_DEFAULT  -- use default policy
 *                         SYSFUN_SCHED_FIFO     -- use first-in-first-out policy
 *                         SYSFUN_SCHED_RR       -- use round-robin policy
 *      thread_name     -- thread name
 *      stack_size      -- stack size used by this thread; must great than 255.
 *      options         -- Task needs supporting.
 *                         SYSFUN_TASK_NO_FP : no floating point coprocessor supporting.
 *                         SYSFUN_TASK_FP    : this task needs floating point coprocessor support.
 *      func_ptr        -- function entry of this thread.
 *                         the function protype must be -> void func(void*)
 *      arg             -- parameter passed to function entry.
 *
 * OUTPUT:
 *      thread_id       -- thread identifier
 *
 * RETURN:
 *      SYSFUN_OK                     -- Successfully.
 *      0x08                          -- task-object table full.
 *      SYSFUN_RESULT_NO_SPACE_4_TASK -- Insufficient space to create task.
 *      SYSFUN_RESULT_STACK_TOO_SMALL -- Stack size too small.
 *      SYSFUN_RESULT_PRIO_OUT_RANGE  -- Priority out of range.
 *      SYSFUN_RESULT_ERROR           -- Unknown error
 *
 * NOTES:
 *      1.options takes no effect on Linux.
 *      2.stack_size can be 0 on Linux. stack_size 0 will adopt linux default stack size.
 */
UI32_T  SYSFUN_SpawnThread(UI32_T thread_priority,
                           UI32_T sched_policy,
                           char   *thread_name,
                           UI32_T stack_size,
                           UI32_T task_option,
                           void   *func_ptr,
                           void   *arg,
                           UI32_T *thread_id);

/* FUNCTION NAME : SYSFUN_SpawnJoinableThread
 * PURPOSE:
 *      Spawn a new thread that is joinable from the running process.
 *
 * INPUT:
 *      thread_priority-- range from 1 to 255, 255 is lowest priority, 255 for
 *                         IDLE task.
 *      sched_policy    -- scheduling policy setting, valid settings are listed below:
 *                         SYSFUN_SCHED_DEFAULT  -- use default policy
 *                         SYSFUN_SCHED_FIFO     -- use first-in-first-out policy
 *                         SYSFUN_SCHED_RR       -- use round-robin policy
 *      thread_name     -- thread name
 *      stack_size      -- stack size used by this thread; must great than 255.
 *      task_option     -- Task needs supporting.
 *                         SYSFUN_TASK_NO_FP : no floating point coprocessor supporting.
 *                         SYSFUN_TASK_FP    : this task needs floating point coprocessor support.
 *      func_ptr        -- function entry of this thread.
 *                         the function protype must be -> void func(void*)
 *      arg             -- parameter passed to function entry.
 *
 * OUTPUT:
 *      thread_id       -- thread identifier
 *      thread_obj      -- thread object that will be passed to SYSFUN_JoinThread
 *
 * RETURN:
 *      SYSFUN_OK                     -- Successfully.
 *      0x08                          -- task-object table full.
 *      SYSFUN_RESULT_NO_SPACE_4_TASK -- Insufficient space to create task.
 *      SYSFUN_RESULT_STACK_TOO_SMALL -- Stack size too small.
 *      SYSFUN_RESULT_PRIO_OUT_RANGE  -- Priority out of range.
 *      SYSFUN_RESULT_ERROR           -- Unknown error
 *
 * NOTES:
 *      1.options takes no effect on Linux.
 *      2.stack_size can be 0 on Linux. stack_size 0 will adopt linux default stack size.
 *      3.The parent thread must call SYSFUN_JoinThread to join the thread spawned
 *        by this API, or it will lead to resource leakage.
 */
UI32_T  SYSFUN_SpawnJoinableThread(UI32_T thread_priority,
                                   UI32_T sched_policy,
                                   char   *thread_name,
                                   UI32_T stack_size,
                                   UI32_T task_option,
                                   void   *func_ptr,
                                   void   *arg,
                                   UI32_T *thread_id,
                                   void **thread_obj);

/* FUNCTION NAME : SYSFUN_JoinThread
 * PURPOSE:
 *      Wait unitl the specified joinable thread is terminated.
 *
 * INPUT:
 *      thread_obj      -- the argument output from SYSFUN_SpawnJoinableThread
 *
 * OUTPUT:
 *      thread_obj      -- the argument output from SYSFUN_SpawnJoinableThread
 *
 * RETURN:
 *      SYSFUN_OK                     -- Successfully.
 *      SYSFUN_RESULT_ERROR           -- Unknown error
 *
 * NOTES:
 */
UI32_T SYSFUN_JoinThread(void **thread_obj);

/* FUNCTION NAME : SYSFUN_TaskNameToID
 * PURPOSE:
 *      Get the numeric identifier of a named task.
 * INPUT:
 *      task_name - the name of task to be searched, if no task name
 *                  specified, i.e. NULL, return current task ID.
 *
 * OUTPUT:
 *      task_id - the specified task ID.
 *
 * RETURN:
 *      SYSFUN_OK                   - Successfully.
 *      SYSFUN_RESULT_NO_NAMED_TASK - Named task was not found.
 *
 * NOTES:
 *      1. If there are duplicate-named tasks, SYSFUN_TaskNameToID()
 *         always returns the first task created with the duplicate name.
 */
UI32_T  SYSFUN_TaskNameToID (char *task_name, UI32_T *task_id);

char * SYSFUN_GetTaskName(UI32_T taskid, char *buf, size_t size);

/* FUNCTION NAME : SYSFUN_TaskIDToName
 * PURPOSE:
 *      Retrieve Task name by task-id.
 * INPUT:
 *      task_id - the specified task ID.
 *				  0 : the caller task.
 *      size    - the size of buffer (task_name) to receive task name.
 *
 * OUTPUT:
 *      task_name - Address of pointer which point to name of task retrieved.
 *
 * RETURN:
 *      SYSFUN_OK                   - Successfully.
 *		SYSFUN_RESULT_INVALID_ID	- specified ID is not valid.
 *      SYSFUN_RESULT_NO_NAMED_TASK - specified task was no name.
 *		SYSFUN_RESULT_INVALID_ARG	- not specified task name buffer space.
 *
 * NOTES:
 *      1. If specified buf_size is smaller than task-name, task-name will be
 *		   truncated to fit the size (buf_size-1).
 *      2. On linux, task id could be thread id or process id.
 */
UI32_T  SYSFUN_TaskIDToName (UI32_T task_id, char *task_name, UI32_T size);


/* FUNCTION NAME : SYSFUN_DeleteProcess
 * PURPOSE:
 *      Delete a process.
 *
 * INPUT:
 *      task_id - identify whick task be deleted; 0 - the task itself.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      SYSFUN_OK    -- Successfully.
 *      SYSFUN_RESULT_OBJ_DELETED    -- Task already deleted.
 *      SYSFUN_RESULT_INVALID_ID    -- tid incorrect; failed validity check.
 *
 * NOTES:
 *      1. Before delete task, the task must free all resources allcated in execution state.
 *      2. This function is OS dependent, currently do not support.
 *
 */
UI32_T  SYSFUN_DeleteProcess (UI32_T task_id);


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
UI32_T SYSFUN_TaskIdSelf (void);


/* FUNCTION NAME : SYSFUN_SetTaskPriority
 * PURPOSE:
 *      Change task's priority.
 *
 * INPUT:
 *      sched_policy -- scheduling policy setting, valid settings are listed below:
 *                         SYSFUN_SCHED_DEFAULT  -- use default policy
 *                         SYSFUN_SCHED_FIFO     -- use first-in-first-out policy
 *                         SYSFUN_SCHED_RR       -- use round-robin policy
 *      tid          -- the task to be changing priority. Tid can be process id
 *                      or thread id on linux.
 *      priority     -- new priority. Valid range is 1-255. 1 is the highest priority.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      SYSFUN_OK   --  OK;
 *      SYSFUN_RESULT_INVALID_ID    -- tid incorrect; failed validity check.
 *
 * NOTES:
 *      1. This function MUST BE carefully using, the different priority will cause
 *         system generate different result. So, if you want to use this function,
 *         please make sure whole system's priority is correct. NOT take mis-order
 *         result.
 */
UI32_T  SYSFUN_SetTaskPriority (UI32_T sched_policy, UI32_T tid, UI32_T priority);

/* FUNCTION NAME : SYSFUN_GetTaskPriority
 * PURPOSE:
 *      Get task's priority.
 *
 * INPUT:
 *      tid     -- the task to get priority. tid can be process id or thread id
 *                 on linux.
 *
 * OUTPUT:
 *      sched_policy -- scheduling policy setting
 *                         SYSFUN_SCHED_DEFAULT  -- default policy
 *                         SYSFUN_SCHED_FIFO     -- first-in-first-out policy
 *                         SYSFUN_SCHED_RR       -- round-robin policy
 *      priority     -- thread priority or process priority. Valid range is 1-255. 1 is the highest priority.
 * RETURN:
 *      SYSFUN_OK   --  OK;
 *      SYSFUN_RESULT_INVALID_ID    -- tid incorrect; failed validity check.
 *      SYSFUN_RESULT_INVALID_ARG   -- invalid arguments
 *      SYSFUN_RESULT_ERROR         -- error
 *
 * NOTES:
 *      1. This function MUST BE carefully using, the different priority will cause
 *         system generate different result. So, if you want to use this function,
 *         please make sure whole system's priority is correct. NOT take mis-order
 *         result.
 */
UI32_T  SYSFUN_GetTaskPriority (UI32_T tid, UI32_T *sched_policy_p, UI32_T *priority_p);


/* FUNCTION NAME : SYSFUN_SuspendThreadSelf
 * PURPOSE:
 *      Suspend the calling thread.
 *
 * INPUT:
 *      None.
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
void SYSFUN_SuspendThreadSelf (void);


/* FUNCTION NAME : SYSFUN_ResumeThread
 * PURPOSE:
 *      Resume a suspended thread.
 *
 * INPUT:
 *      task_id -- the task be resumed.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      SYSFUN_OK                -- OK, wakeup the task.
 *      SYSFUN_RESULT_INVALID_ID -- task can't be resume, invalid task id.
 *
 * NOTES:
 *      1. task_id is invalid if the thread and calling thread are not
 *         in the same process.
 *      2. if exists more than one thread in a process are suspended
 */
UI32_T SYSFUN_ResumeThread (UI32_T task_id);

/* FUNCTION NAME : SYSFUN_DelSelfThread
 * PURPOSE:
 *      This function will terminate the calling thread.
 *
 * INPUT:
 *      None.
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
void SYSFUN_DelSelfThread (void);

/* FUNCTION NAME : SYSFUN_InitThread
 * PURPOSE:
 *      To create a thread and suspend the thread. The parent process shall
 *      start the thread later on.
 *
 * INPUT:
 *      thread_priority-- range from 1 to 255, 255 is lowest priority, 255 for
 *                         IDLE task.
 *      sched_policy    -- scheduling policy setting, valid settings are listed below:
 *                         SYSFUN_SCHED_DEFAULT  -- use default policy
 *                         SYSFUN_SCHED_FIFO     -- use first-in-first-out policy
 *                         SYSFUN_SCHED_RR       -- use round-robin policy
 *      thread_name     -- thread name
 *      stack_size      -- stack size used by this thread; must great than 255.
 *      options         -- Task needs supporting.
 *                         SYSFUN_TASK_NO_FP : no floating point coprocessor supporting.
 *                         SYSFUN_TASK_FP    : this task needs floating point coprocessor support.
 *      func_ptr        -- function entry of this thread.
 *                         the function protype must be -> void func(void*)
 *      arg             -- parameter passed to function entry.
 *
 * OUTPUT:
 *      thread_id       -- thread identifier
 *
 * RETURN:
 *      SYSFUN_OK                     -- Successfully.
 *      0x08                          -- task-object table full.
 *      SYSFUN_RESULT_NO_SPACE_4_TASK -- Insufficient space to create task.
 *      SYSFUN_RESULT_STACK_TOO_SMALL -- Stack size too small.
 *      SYSFUN_RESULT_PRIO_OUT_RANGE  -- Priority out of range.
 *      SYSFUN_RESULT_ERROR           -- Unknown error
 *
 * NOTES:
 *      1.options takes no effect on Linux.
 *      2.stack_size can be 0 on Linux. stack_size 0 will adopt linux default stack size.
 */
UI32_T  SYSFUN_InitThread(UI32_T thread_priority,
                          UI32_T sched_policy,
                          char   *thread_name,
                          UI32_T stack_size,
                          UI32_T task_option,
                          void   *func_ptr,
                          void   *arg,
                          UI32_T *thread_id);

/* FUNCTION NAME : SYSFUN_StartThread
 * PURPOSE:
 *      Wake up a pre-created thread.
 *
 * INPUT:
 *      task_id - task identifier
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      SYSFUN_OK                     -- Successfully activate the task.
 *      SYSFUN_RESULT_INVALID_ID      -- not a valid task-id, or the task can't be activated.
 *
 * NOTES:
 *      1. The thread must be created and init. before activated.
 *
 * NOTES:
 *      None.
 */
UI32_T  SYSFUN_StartThread (UI32_T task_id);


/*------------------------------------------------------------------------------
 * Function : SYSFUN_SuspendTaskWithTimeout
 *------------------------------------------------------------------------------
 * Purpose  : This function will suspend the task which invoke this function
 *            with the duration given by timeout. The suspended task will be
 *            resumed when the timer expire or SYSFUN_ResumeTaskInTimeoutSuspense()
 *            is called with id of the suspended task.
 * INPUT    : timeout  -  timer in ticks
 * OUTPUT   : None
 * RETURN   : TRUE  -  The suspended task is resumed before the timer expires
 *            FALSE -  The suspended task is resumed due to time out.
 * NOTE     : This function MUST NOT BE called by an ISR.
 *-----------------------------------------------------------------------------*/
BOOL_T SYSFUN_SuspendTaskWithTimeout(UI32_T timeout);

/*------------------------------------------------------------------------------
 * Function : SYSFUN_ResumeTaskInTimeoutSuspense
 *------------------------------------------------------------------------------
 * Purpose  : This function will resume the task with the specified task_id
 * INPUT    : task_id - id of the task which is going to be resumed if that task
 *                      is suspended.
 * OUTPUT   : None
 * RETURN   : TRUE  -  The task is resumed without error.
 *            FALSE -  Fail to resume the task
 * NOTE     : None
 *-----------------------------------------------------------------------------*/
BOOL_T SYSFUN_ResumeTaskInTimeoutSuspense(UI32_T task_id);

#if 0
#if (SYSFUN_RD_DEBUG_ENABLE_DUMP_TASK_STACK==TRUE)
/* FUNCTION NAME : SYSFUN_Debug_DumpTaskStack
 * PURPOSE:
 *      This api will dump the call stack of the caller.
 * INPUT:
 *      None
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      Only address of the called functions will be displayed.
 *      It is required to understand the shown call stack with the help of
 *      objdumpppc(For PPC) to know the called functions.
 */
void SYSFUN_Debug_DumpTaskStack(void);
#endif
#endif

#define SYSFUN_DumpCurrentTaskStack()   \
    do                                  \
    {                                   \
    } while(0)
/*
#define SYSFUN_DumpCurrentTaskStack()                                               \
    do                                                                              \
    {                                                                               \
        char commd_line[128];                                                       \
        SYSFUN_Sprintf(commd_line, "cat /proc/%ld/ustack", SYSFUN_TaskIdSelf());    \
        system(commd_line);                                                         \
    } while (0)
*/

/*-------------------------
 * Semaphore Function
 *-------------------------
 */

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
UI32_T SYSFUN_CreateSem (UI32_T sem_key, UI32_T sem_count, UI32_T sem_flag, UI32_T *sem_id);


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
UI32_T SYSFUN_GetSem (UI32_T sem_key, UI32_T *sem_id);


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
UI32_T  SYSFUN_DestroySem(UI32_T sem_id);


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
UI32_T  __SYSFUN_TakeSem (UI32_T sem_id, int timeout, const char *function, int line);

#define SYSFUN_TakeSem(sem_id, timeout)  __SYSFUN_TakeSem(sem_id, timeout, __FUNCTION__, __LINE__)

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
UI32_T  __SYSFUN_GiveSem(UI32_T sem_id, const char *function, int line);

#define SYSFUN_GiveSem(sem_id)  __SYSFUN_GiveSem(sem_id, __FUNCTION__, __LINE__)


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
UI32_T SYSFUN_CreateShMem(UI32_T key, UI32_T size, UI32_T *shm_id_p);

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
void *SYSFUN_AttachShMem(UI32_T shm_id);

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
void SYSFUN_DetachShMem(void *attached_addr);


/*-------------------------
 * Message Queue Function
 *-------------------------
 */

UI32_T SYSFUN_RegisterIPCHandler(
    UI32_T msgq_handle,
    BOOL_T (*handler_fn) (SYSFUN_Msg_T*)
);

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
UI32_T SYSFUN_CreateMsgQ(UI32_T msgq_key, UI32_T msgq_type, UI32_T *msgq_handle_p);

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
 *              will be set to 0.
 *              If the queue exists, get the queue.
 * ------------------------------------------------------------------------
 */
UI32_T SYSFUN_GetMsgQ(UI32_T msgq_key, UI32_T msgq_type, UI32_T *msgq_handle_p);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - SYSFUN_ReleaseMsgQ
 * ------------------------------------------------------------------------
 * PURPOSE  :   Release the given message queue handle.
 * INPUT    :   msgq_handle - The handle of the message queue to be released
 * OUTPUT   :   None.
 * RETURN   :   SYSFUN_OK                       - Successfully.
 *              SYSFUN_RESULT_INVALID_ID        - Invalid msgq_handle
 * NOTES    :   This api will only release the message queue handle. The message
 *              queue won't be removed.
 * ------------------------------------------------------------------------
 */
UI32_T SYSFUN_ReleaseMsgQ(UI32_T msgq_handle);

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
UI32_T SYSFUN_DestroyMsgQ(UI32_T msgq_handle);

/*-------------------------------------------------------------------------
 * FUNCTION NAME - SYSFUN_GetMsgQOwner
 * ------------------------------------------------------------------------
 * PURPOSE  :   Get the owner of the specific message queue.
 * INPUT    :   msgq_handle - The specific queue.
 * OUTPUT   :   None.
 * RETURN   :   The task id of the owner of the spcicif queue.
 *              0 means the owner task id had been not specified or
 *                          the message queue doesn't exist.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
UI32_T SYSFUN_GetMsgQOwner(UI32_T msgq_handle);

/* FUNCTION NAME : SYSFUN_CreateRwSem
 * PURPOSE:
 *      Create a Read/write semaphore.
 *
 * INPUT:
 *      sem_key     -- key for the semaphore.
 *                     If this value is set as SYSFUN_SEMKEY_PRIVATE, the created semaphore is
 *                     only valid in the calling process.
 *                     If this valie is set as value other than SYSFUN_SEMKEY_PRIVATE, the
 *                     created semaphore is valid among all processes.
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
 *      3. If a semaphore already exists for the key, creation will return the existed one.
 */
UI32_T SYSFUN_CreateRwSem (UI32_T sem_key, UI32_T *sem_id);

/* FUNCTION NAME : SYSFUN_GetRwSem
 * PURPOSE:
 *      Get an existed read/write semaphore.
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
UI32_T SYSFUN_GetRwSem (UI32_T sem_key, UI32_T *sem_id);

/* FUNCTION NAME : SYSFUN_TakeWriteSem
 * PURPOSE:
 *      Wait for semaphore to write. (Take or P operation)
 *
 * INPUT:
 *      sem_id      -- semaphore ID to wait.
 *
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
UI32_T  SYSFUN_TakeWriteSem (UI32_T sem_id);

/* FUNCTION NAME : SYSFUN_TakeReadSem
 * PURPOSE:
 *      Wait for semaphore to read. (Take or P operation)
 *
 * INPUT:
 *      sem_id      -- semaphore ID to wait.
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
UI32_T  SYSFUN_TakeReadSem (UI32_T sem_id);

/* FUNCTION NAME : SYSFUN_GiveWriteSem
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
UI32_T  SYSFUN_GiveWriteSem(UI32_T sem_id);

/* FUNCTION NAME : SYSFUN_GiveReadSem
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
UI32_T  SYSFUN_GiveReadSem(UI32_T sem_id);

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
void SYSFUN_DBG_MsgQDebug(UI32_T flag, int on);

#if 1 /////////////////////////////////////////////////
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
                             );

#if defined (SYSFUN_DEBUG) && defined (SYSFUN_DEBUG_ERROR_CHECK)
# define SYSFUN_SendRequestMsg(msgq_handle, req_msg_p, wait_time, event, res_msg_size, res_msg_p)       \
    ({                                                                                                  \
        UI32_T __rc;                                                                                    \
        __rc = __SYSFUN_SendRequestMsg(msgq_handle, req_msg_p, wait_time, event,                        \
                                               res_msg_size, res_msg_p, __func__, __LINE__);            \
        if (__rc != SYSFUN_OK) {                                                                        \
            printf("\r\n%s() line %d:Failed to send/receive IPC message.(msgq_id=%lu, rc=%lu)",         \
                   __FUNCTION__, __LINE__, msgq_handle, __rc);                                          \
            SYSFUN_DumpCurrentTaskStack();                                                              \
        }                                                                                               \
        __rc;                                                                                           \
    })
#else
# define SYSFUN_SendRequestMsg(msgq_handle, req_msg_p, wait_time, event, res_msg_size, res_msg_p)       \
    __SYSFUN_SendRequestMsg(msgq_handle, req_msg_p, wait_time, event, res_msg_size, res_msg_p)
#endif /* SYSFUN_DEBUG && SYSFUN_DEBUG_ERROR_CHECK */

#else  /////////////////////////////////////////////////

UI32_T _SYSFUN_SendRequestMsg(UI32_T msgq_handle,
                             SYSFUN_Msg_T *req_msg_p,
                             UI32_T wait_time,
                             UI32_T event,
                             UI32_T res_msg_size,
                             SYSFUN_Msg_T *res_msg_p);

#if (SYSFUN_RD_DEBUG_SHOW_ERROR_MSG == TRUE)
# define SYSFUN_SendRequestMsg(msgq_handle, req_msg_p, wait_time, event, res_msg_size, res_msg_p)       \
    ({                                                                                                  \
        UI32_T rc;                                                                                      \
        rc =_SYSFUN_SendRequestMsg(msgq_handle, req_msg_p, wait_time, event, res_msg_size, res_msg_p);  \
        if (rc != SYSFUN_OK)                                                                            \
            printf("\r\n%s() line %d:Failed to send/receive IPC message.(rc=%lu)",                      \
                   __FUNCTION__, __LINE__, rc);                                                         \
        rc;                                                                                             \
    })
#else
# define SYSFUN_SendRequestMsg(msgq_handle, req_msg_p, wait_time, event, res_msg_size, res_msg_p)       \
    _SYSFUN_SendRequestMsg(msgq_handle, req_msg_p, wait_time, event, res_msg_size, res_msg_p)
#endif
#endif /////////////////////////////////////////////////

/*-------------------------------------------------------------------------
 * FUNCTION NAME - SYSFUN_SendResponseMsg
 * ------------------------------------------------------------------------
 * PURPOSE  :   Send a response message to message queue.
 * INPUT    :   msgq_handle  - The queue that the response be sent to.
 *              res_msg_p    - The response message.
 *                             res_msg_p->msg_type must be the id of the
 *                             task that waits for response.
 * OUTPUT   :   None.
 * RETURN   :   SYSFUN_OK                   - Successfully.
 *              SYSFUN_RESULT_OBJ_DELETED   - Message queue is deleted.
 *              SYSFUN_RESULT_INVALID_ID    - queue ID is incorrect.
 *              SYSFUN_RESULT_INVALID_ARG   - Invalid parameters.
 *              SYSFUN_RESULT_ERROR         - Unknown error
 * NOTES    :   If the queue is full, this function will retry until successfully.
 * ------------------------------------------------------------------------
 */
#if 1 /////////////////////////////////////////////////
UI32_T __SYSFUN_SendResponseMsg(UI32_T msgq_handle, SYSFUN_Msg_T *res_msg_p
#ifdef SYSFUN_DEBUG
                             , const char *func, unsigned line
#endif
);

#if defined (SYSFUN_DEBUG) && defined (SYSFUN_DEBUG_ERROR_CHECK)
# define SYSFUN_SendResponseMsg(msgq_handle, res_msg_p)                                            \
    ({                                                                                             \
        UI32_T __rc;                                                                               \
        __rc = __SYSFUN_SendResponseMsg(msgq_handle, res_msg_p, __func__, __LINE__);               \
        if (__rc != SYSFUN_OK) {                                                                   \
            printf("\r\n%s() line %d: Failed to send response IPC message.(msgq_key=%lu, rc=%lu)", \
                    __FUNCTION__, __LINE__, ((SYSFUN_MsgQDesc_T *)msgq_handle)->msgq_key, __rc);   \
        }                                                                                          \
        __rc;                                                                                      \
    })
#else
# define SYSFUN_SendResponseMsg(msgq_handle, res_msg_p)  \
    __SYSFUN_SendResponseMsg(msgq_handle, res_msg_p)
#endif /* SYSFUN_DEBUG && SYSFUN_DEBUG_ERROR_CHECK */

#else /////////////////////////////////////////////////

UI32_T _SYSFUN_SendResponseMsg(UI32_T msgq_handle, SYSFUN_Msg_T *res_msg_p);

#if (SYSFUN_RD_DEBUG_SHOW_ERROR_MSG == TRUE)
# define SYSFUN_SendResponseMsg(msgq_handle, res_msg_p)                                 \
    ({                                                                                  \
        UI32_T rc;                                                                      \
        rc = _SYSFUN_SendResponseMsg(msgq_handle, res_msg_p);                           \
        if (rc != SYSFUN_OK)                                                            \
            printf("\r\n%s() line %d: Failed to send response IPC message.(rc=%lu)",    \
                    __FUNCTION__, __LINE__, rc);                                        \
        rc;                                                                             \
    })
#else
# define SYSFUN_SendResponseMsg(msgq_handle, res_msg_p) \
    _SYSFUN_SendResponseMsg(msgq_handle, res_msg_p)
#endif
#endif /////////////////////////////////////////////////

/*-------------------------------------------------------------------------
 * FUNCTION NAME - SYSFUN_ReceiveMsg
 * ------------------------------------------------------------------------
 * PURPOSE  :   Receive a request message from the message queue.
 * INPUT    :   msgq_handle  - The queue that the request be received from.
 *              msg_type     - The type of message to receive.
 *                             0 means to receive the first message in queue.
 *              wait_time    - The timeout interval.
 *                             SYSFUN_TIMEOUT_NOWAIT        : no wait.
 *                             SYSFUN_TIMEOUT_WAIT_FOREVER  : wait forever
 *                             other: waiting time (in clock ticks).
 *              req_msg_size - The size of the req_msg_p->msg_buf.
 * OUTPUT   :   req_msg_p    - The buffer to received message.
 * RETURN   :   SYSFUN_OK                   - Successfully.
 *              SYSFUN_RESULT_TIMEOUT       - Time out.
 *              SYSFUN_RESULT_INVALID_ID    - Qid incorrect; failed validity checks
 *              SYSFUN_RESULT_MSG_Q_DELETED - Queue deleted while task waiting.
 *              SYSFUN_RESULT_NO_MESSAGE    - Queue empty. Returned by Q_NOWAIT.
 *              SYSFUN_RESULT_INVALID_ARG   - Invalid parameters.
 *              SYSFUN_RESULT_ERROR         - Unknown error
 * NOTES    :   If no message in queue, this function will block and wait
 *              for message.
 * ------------------------------------------------------------------------
 */
#if 1 /////////////////////////////////////////////////
UI32_T __SYSFUN_ReceiveMsg(UI32_T        msgq_handle,
                          UI32_T        msg_type,
                          UI32_T        wait_time,
                          UI32_T        req_msg_size,
                          SYSFUN_Msg_T *req_msg_p
#ifdef SYSFUN_DEBUG
                          ,
                          const char *   func,
                          unsigned       line
#endif
                          );

#if defined (SYSFUN_DEBUG) && defined (SYSFUN_DEBUG_ERROR_CHECK)
# define SYSFUN_ReceiveMsg(msgq_handle, msg_type, wait_time, req_msg_size, res_msg_p)            \
    ({                                                                                           \
        UI32_T __rc;                                                                             \
        __rc = __SYSFUN_ReceiveMsg(msgq_handle, msg_type, wait_time, req_msg_size,               \
                                   res_msg_p, __func__, __LINE__);                               \
        if ((__rc != SYSFUN_OK) && (__rc != SYSFUN_RESULT_NO_MESSAGE)) {                         \
            printf("\r\n%s() line %d:Failed to receive IPC message.(msgq_key=%lu, rc=%lu)",      \
                    __FUNCTION__, __LINE__, ((SYSFUN_MsgQDesc_T *)msgq_handle)->msgq_key, __rc); \
        }                                                                                        \
        __rc;                                                                                    \
    })
#else
# define SYSFUN_ReceiveMsg(msgq_handle, msg_type, wait_time, req_msg_size, res_msg_p)   \
    __SYSFUN_ReceiveMsg(msgq_handle, msg_type, wait_time, req_msg_size, res_msg_p)
#endif /* SYSFUN_DEBUG && SYSFUN_DEBUG_ERROR_CHECK */

#else /////////////////////////////////////////////////

UI32_T _SYSFUN_ReceiveMsg(UI32_T        msgq_handle,
                          UI32_T        msg_type,
                          UI32_T        wait_time,
                          UI32_T        req_msg_size,
                          SYSFUN_Msg_T *req_msg_p);

#if (SYSFUN_RD_DEBUG_SHOW_ERROR_MSG == TRUE)
# define SYSFUN_ReceiveMsg(msgq_handle, msg_type, wait_time, req_msg_size, res_msg_p)       \
    ({                                                                                      \
        UI32_T rc;                                                                          \
        rc = _SYSFUN_ReceiveMsg(msgq_handle, msg_type, wait_time, req_msg_size, res_msg_p); \
        if ((rc != SYSFUN_OK) && (rc != SYSFUN_RESULT_NO_MESSAGE))                          \
            printf("\r\n%s() line %d:Failed to receive IPC message.(rc=%lu)",               \
                    __FUNCTION__, __LINE__, rc);                                            \
        rc;                                                                                 \
    })
#else
# define SYSFUN_ReceiveMsg(msgq_handle, msg_type, wait_time, req_msg_size, res_msg_p)       \
    _SYSFUN_ReceiveMsg(msgq_handle, msg_type, wait_time, req_msg_size, res_msg_p)
#endif
#endif /////////////////////////////////////////////////

/*-------------------------
 * Event Function
 *-------------------------
 */

/* FUNCTION NAME : SYSFUN_SendEvent
 * PURPOSE:
 *      Send events to a task, if the task is waiting for event,
 *      the task will be waked up when the condition is satified.
 *
 * INPUT:
 *      tid     - the task identifier of the target task.
 *      event  - a list of bit-encoded events.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      SYSFUN_OK                   - Successfully.
 *      SYSFUN_RESULT_OBJ_DELETED   - Task has beed deleted.
 *      SYSFUN_RESULT_INVALID_ID    - tid is incorrect
 *      SYSFUN_RESULT_INVALID_ARG   - no event to be set.
 *      SYSFUN_RESULT_SYSFUN_NOT_INIT - SYSFUN not Init. yet.
 *
 * NOTES:
 *      1. Bit-0 to bit-15 is available for use; bit-16 to bit-31 is reserved
 *         for system usage. (Compatbile with pSos).
 */
UI32_T SYSFUN_SendEvent (UI32_T tid, UI32_T event);


/* FUNCTION NAME : SYSFUN_ReceiveEvent
 * PURPOSE:
 *      Receive one or more events.
 *
 * INPUT:
 *      wait_events - the set of events.
 *      flags       - the event processing attributes.
 *                    SYSFUN_EVENT_WAIT_ALL  -- Wait for all event.
 *                    SYSFUN_EVENT_WAIT_ANY  -- Wait for any one event.
 *      timeout     - timeout in units of ticks,
 *                     SYSFUN_TIMEOUT_NOWAIT       - no wait.
 *                     SYSFUN_TIMEOUT_WAIT_FOREVER - wait forever
 *                     others                      - timeout ticks.
 *
 * OUTPUT:
 *      received_events - Points to the variable where SYSFUN_ReceiveEvent ()
 *                        stores the actual events captured.
 *
 * RETURN:
 *      SYSFUN_OK                   - Successfully.
 *      SYSFUN_RESULT_TIMEOUT       - Timed out for waiting a time.
 *      SYSFUN_RESULT_NO_EVENT_SET  - Selected events not occured for flag is NoWait.
 *      SYSFUN_RESULT_CALL_FROM_ISR - Can't call from ISR.
 *      SYSFUN_RESULT_SYSFUN_NOT_INIT - SYSFUN not Init. yet.
 *
 * NOTES:
 *      1. When SYSFUN_EventReceive is called, previous waiting events is cleared.
 *      2. EV_ALL - all the waiting events are satisfied,
 *         EV_ANY - any one of waiting event is satisfied.
 *      3. If the selected event condition is satisfied by events already
 *         pending, SYSFUN_ReceiveEvent () clears those events and returns.
 *      4. For unknown result, all return Timeout if no better status could be return.
 *
 */
UI32_T SYSFUN_ReceiveEvent (UI32_T wait_events,
                            UI32_T flags,
                            int timeout,
                            UI32_T *received_events);

/*-------------------------
 * Timer Function
 *-------------------------
 */
/*------------------------------------------------------------------------------
 * Function : SYSFUN_PeriodicTimer_Create
 *------------------------------------------------------------------------------
 * Purpose  : This function is used to create a periodic timer object.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : timer id. NULL if create failed.
 * NOTE     :
 *            1. If the timer object can't be created, this function will return
 *               NULL.
 *-----------------------------------------------------------------------------*/
void  *SYSFUN_PeriodicTimer_Create (void);

/*------------------------------------------------------------------------------
 * Function : SYSFUN_PeriodicTimer_Start
 *------------------------------------------------------------------------------
 * Purpose  : This function is used to start a periodic timer.
 * INPUT    : tm_id         - timer id, which is created through
 *                            SYSFUN_PeriodicTimer_Create().
 *            time_interval - The periodic interval in system clock ticks.
 *            event         - Specifies the events to deliver upon expiration of
 *                            the timer interval.
 * OUTPUT   : None
 * RETURN   : TRUE  - Timer is started sucessfully.
 *            FALSE - Timer can't be started.
 * NOTE     : None
 *-----------------------------------------------------------------------------*/
BOOL_T SYSFUN_PeriodicTimer_Start (void *tm_id, UI32_T time_interval, UI32_T event);

/*------------------------------------------------------------------------------
 * Function : SYSFUN_PeriodicTimer_Restart
 *------------------------------------------------------------------------------
 * Purpose  : This function is used to restart a periodic timer.
 * INPUT    : tm_id         - timer id, which is created through
 *                            SYSFUN_PeriodicTimer_Create().
 *            time_interval - The periodic interval in system clock ticks.
 * OUTPUT   : None
 * RETURN   : TRUE - Timer is started sucessfully.
 *            FAIL - Timer can't be started.
 * NOTE     : None
 *-----------------------------------------------------------------------------*/
BOOL_T SYSFUN_PeriodicTimer_Restart (void *tm_id, UI32_T time_interval);

/*------------------------------------------------------------------------------
 * Function : SYSFUN_PeriodicTimer_Stop
 *------------------------------------------------------------------------------
 * Purpose  : This function is used to stop a periodic timer.
 * INPUT    : tm_id         - timer id, which is created through
 *                            SYSFUN_PeriodicTimer_Create().
 * OUTPUT   : None
 * RETURN   : TRUE  - Timer is stopped sucessfully.
 *            FALSE - Timer can't be stopped.
 * NOTE     : None
 *-----------------------------------------------------------------------------*/
BOOL_T SYSFUN_PeriodicTimer_Stop (void *tm_id);

/*------------------------------------------------------------------------------
 * Function : SYSFUN_PeriodicTimer_Destroy
 *------------------------------------------------------------------------------
 * Purpose  : This function is used to destroy a periodic timer.
 * INPUT    : tm_id - timer id, which is created through
 *            SYSFUN_PeriodicTimer_Create().
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 *-----------------------------------------------------------------------------*/
void SYSFUN_PeriodicTimer_Destroy (void *tm_id);

/* FUNCTION NAME : SYSFUN_Register_Ticks_Callback
 * PURPOSE:
 *      Register a xxx callback function. The function will be executed for every n ticks.
 *
 * INPUT:
 *      periodical ticks   -- the time between two callback.
 *      ticks_function     -- the callback function for each n ticks.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      SYSFUN_OK    -- Successfully.
 *      SYSFUN_RESULT_TOO_MANY_REQUEST    -- Failed. There is one function registered.
 *      SYSFUN_RESULT_SYSFUN_NOT_INIT
 *      SYSFUN_RESULT_INVALID_ARG
 *      SYSFUN_RESULT_NO_AVAIL_TIMER      -- no more available space to set callback function.
 *
 * NOTES:
 *      1. Only LED component can use this function, ie. this function is for LED only;
 *         LED component uses this function to change display pattern.
 */
UI32_T  SYSFUN_Register_Ticks_Callback(UI32_T  time_interval, VOID_FUNCTION call_back_fun);

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
UI32_T  SYSFUN_GetSysTick (void);

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
UI32_T  SYSFUN_MSecondToTick (UI32_T ms);

/* FUNCTION NAME : SYSFUN_Sleep
 * PURPOSE:
 *      Task sleep a short time.
 *
 * INPUT:
 *      ticks   -- sleeping units; based on 0.01 second.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      SYSFUN_OK    -- Successfully.
 *      SYSFUN_RESULT_CALL_FROM_ISR    -- Failed. Called from ISR.
 *
 * NOTES:
 *      1. Can not be called by ISR routine.
 *      2. Assume system tick is 1/100 sec, if not this freq. should adjust this MACRO.
 *      3. Refer sample code :
            #ifdef  _VXWORKS
            #define SYSFUN_Sleep(x)             taskDelay(x);
            #endif
            #ifdef  _PSOS
            #define SYSFUN_Sleep(x)             tm_wkafter(x);
            #endif
 */
UI32_T  SYSFUN_Sleep (UI32_T  ticks);


/* FUNCTION NAME : SYSFUN_ThreadInit
 * PURPOSE:
 *      Initialize working environment when a thread is spawned.
 * INPUT:
 *      thread_name    -- the name of the thread calling this function.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. This function is called by SYSFUN_InternalNewThreadEntry() on Linux.
 */
void SYSFUN_ThreadInit(char *thread_name);


/* FUNCTION NAME : SYSFUN_LogMsg
 * PURPOSE:
 *      Log error message to system log, only one text length less than 56.
 *
 * INPUT:
 *      msg_text-- message (text) body.
 *      arg1 ~ arg6 - argument.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. This function only log one text message to syslog.
 *      2. There is some differenct level message, user can use it.
 */
void __SYSFUN_DBG_Assert(const char *func, unsigned line, int cond, const char *condstr);
void __SYSFUN_DBG_Printf(const char *func, unsigned line, const char *fmt, ...);
void __SYSFUN_DBG_LogMsg(int level, void *mod, const char *func, unsigned line, const char *fmt, ...);
void __SYSFUN_LogMsg(int level, void *mod, const char *fmt, ...);


void SYSFUN_LogMsg(char *msg_text, I32_T arg1, I32_T arg2, I32_T arg3, I32_T arg4, I32_T arg5, I32_T arg6);

void SYSFUN_LogUrgentMsg(char *msg_text);

void SYSFUN_LogAlertMsg(char *msg_text);

void SYSFUN_LogCriticalMsg(char *msg_text);

void SYSFUN_LogErrMsg(char *msg_text);

void SYSFUN_LogWarnMsg(char *msg_text);

void SYSFUN_LogNoticeMsg(char *msg_text);

void SYSFUN_LogInfoMsg(char *msg_text);

void SYSFUN_LogDebugMsg(char *msg_text);

/*-------------------------
 * Not sure
 *-------------------------
 */

/* FUNCTION NAME : SYSFUN_SetMasterStatusRegister
 * PURPOSE:
 *      Set Master Status Register
 * INPUT:
 *      UI32 val - Value to set to Master Status Register
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
void SYSFUN_SetMasterStatusRegister(UI32_T val);

/* FUNCTION NAME : SYSFUN_EnableDataCache
 * PURPOSE:
 *      Enable CPU's data cache.
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      UI32_T
 *
 * NOTES:
 *      None.
 */
UI32_T SYSFUN_EnableDataCache(void);

/* FUNCTION NAME : SYSFUN_DisableDataCache
 * PURPOSE:
 *      Disable CPU's data cache.
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      UI32_T
 *
 * NOTES:
 *      None.
 */
UI32_T SYSFUN_DisableDataCache(void);

/* FUNCTION NAME : SYSFUN_EnableInstructionCache
 * PURPOSE:
 *      Enable CPU's instruction cache.
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      UI32_T
 *
 * NOTES:
 *      None.
 */
UI32_T SYSFUN_EnableInstructionCache(void);

/* FUNCTION NAME : SYSFUN_DisableInstructionCache
 * PURPOSE:
 *      Disable CPU's instruction cache.
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      UI32_T
 *
 * NOTES:
 *      None.
 */
UI32_T SYSFUN_DisableInstructionCache(void);

/*-------------------------
 * UART Operations
 *-------------------------
 */

/* FUNCTION NAME : SYSFUN_OpenUART
 * PURPOSE:
 *      Open UART device and get the handle.
 *
 * INPUT:
 *      UART_channel  -- SYSFUN_UART_CHANNEL1, open UART channel 1 device
 *                    -- SYSFUN_UART_CHANNEL2, open UART channel 2 device
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      SYSFUN_RESULT_ERROR -- Error
 *      Other values        -- UART handle
 *
 * NOTES:
 *      None.
 */
UI32_T SYSFUN_OpenUART(UI32_T UART_channel);

/* FUNCTION NAME : SYSFUN_OpenDebugUART
 * PURPOSE:
 *      Open UART device as controlling terminal.
 *
 * INPUT:
 *      UART_channel  -- SYSFUN_UART_CHANNEL1, open UART channel 1 device
 *                    -- SYSFUN_UART_CHANNEL2, open UART channel 2 device
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      SYSFUN_RESULT_ERROR -- Error
 *      Other values        -- UART handle
 *
 * NOTES:
 *      None.
 */
UI32_T SYSFUN_OpenDebugUART(UI32_T UART_channel);


/* FUNCTION NAME : SYSFUN_CloseUART
 * PURPOSE:
 *      Close UART device by the given UART handle.
 *
 * INPUT:
 *		UART_handle  --  the handle got from SYSFUN_OpenUART().
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      SYSFUN_RESULT_ERROR -- Error
 *      Other values        -- UART handle
 *
 * NOTES:
 *      None.
 */
UI32_T SYSFUN_CloseUART(UI32_T UART_handle);

/* FUNCTION NAME : SYSFUN_UARTPollRxBuff
 * PURPOSE:
 *		Poll UART rx buffer to get a character.
 * INPUT:
 *		UART_handle  --  the handle got from SYSFUN_OpenUART().
 *      buf_len      --  buffer length of buf_p
 *
 * OUTPUT:
 *      buf_p        --  Received data from UART
 *
 * RETURN:
 *      The length of received data.
 *
 * NOTES:
 *      This api will put all data in Rx buffer of the given UART into buf_p if
 *      the length of data in Rx buffer is smaller or equal to buf_len.
 *      If the length of data in Rx buffer is larger than buf_len, only the data
 *      with size buf_len will be put to buf_p. If no data in Rx buffer, 0 will
 *      be returned.
 *
 */
UI32_T SYSFUN_UARTPollRxBuff(UI32_T UART_handle, UI32_T buf_len, void* buf_p);

/* FUNCTION NAME : SYSFUN_UARTPutData
 * PURPOSE:
 *		Put the given data to UART.
 * INPUT:
 *      UART_handle  --  the handle got from SYSFUN_OpenUART().
 *      buf_len      --  buffer length of buf_p
 *      buf_p        --  Data to be put to UART
 *
 * OUTPUT:
 *      None
 *
 * RETURN:
 *      SYSFUN_OK           --  Success
 *      SYSFUN_RESULT_ERROR --  Fail
 *
 * NOTES:
 *      None
 */
UI32_T SYSFUN_UARTPutData(UI32_T UART_handle, UI32_T buf_len, void *buf_p);

/* FUNCTION NAME : SYSFUN_SetUartCfg
 * PURPOSE:
 *      Set UART's parameter.
 * INPUT:
 *      UI32_T UART_handle
 *      UI32_T baudrate
 *      UI8_T  parity
 *      UI8_T  data_length
 *      UI8_T  stop_bits
 *
 * RETURN:
 *      UI32_T
 *
 * NOTES:
 *      None.
 */
UI32_T SYSFUN_SetUartCfg(   UI32_T UART_handle,
                            UI32_T baudrate,
                            UI8_T  parity,
                            UI8_T  data_length,
                            UI8_T  stop_bits);

/* FUNCTION NAME : SYSFUN_SetUartBaudRate
 * PURPOSE:
 *      Set UART's baud rate.
 * INPUT:
 *      UI32_T UART_handle
 *      UI32_T baudrate
 *
 * OUTPUT: None
 * RETURN: OK(0) on success, SYSFUN_RESULT_INVALID_ARG(3) for invalid arg,
 *         EIO(5) on device error, ENOSYS(71) on unsupported.
 *         request.
 *
 * NOTES: None.
 *
 */
UI32_T SYSFUN_SetUartBaudRate(UI32_T UART_handle, UI32_T baudrate);

/* FUNCTION NAME : SYSFUN_GetUartBaudRate
 * PURPOSE:
 *      Get UART's baud rate.
 * INPUT:  UI32_T UART_handle
 *
 * OUTPUT: *baudrate
 * RETURN: OK(0) on success, EIO(5) on device error, ENOSYS(71) on unsupported
 *         request.
 *
 * NOTES: None.
 *
 */
UI32_T SYSFUN_GetUartBaudRate(UI32_T UART_handle, UI32_T *baudrate);

/*------------------------------------------------------------------------------
 * Function : SYSFUN_InvokeLinuxShell
 *------------------------------------------------------------------------------
 * Purpose  : This function will invoke linux shell and suspend the caller thread
 *            until linux shell terminated.
 * INPUT    : None.
 * OUTPUT   : None.
 * RETURN   : None.
 * NOTE     : None.
 *-----------------------------------------------------------------------------*/
void SYSFUN_InvokeLinuxShell(void);

/*------------------------------------------------------------------------------
 * Function : SYSFUN_Reboot
 *------------------------------------------------------------------------------
 * Purpose  : This function will start the reboot procedure.
 * INPUT    : char *command: reload type and warm start adderss
 * OUTPUT   : None.
 * RETURN   : None.
 * NOTE     : None.
 *-----------------------------------------------------------------------------*/
void SYSFUN_Reboot(char *command);

/*------------------------------------------------------------------------------
 * Function : SYSFUN_GetKernelVer
 *------------------------------------------------------------------------------
 * Purpose  : Get the version of kernel.
 * INPUT    : None.
 * OUTPUT   : kernel_ver  --  The linux kernel version will be written here.
 * RETURN   : SYSFUN_OK -- Sucess.
 *            SYSFUN_RESULT_ERROR -- Fail.
 * NOTE     : None.
 *-----------------------------------------------------------------------------*/
UI32_T SYSFUN_GetKernelVer(char kernel_ver[SYS_ADPT_KERNEL_VER_STR_LEN+1]);

/*-------------------------
 * System Resource
 *-------------------------
 */

/* FUNCTION NAME : SYSFUN_GetCpuUsage
 * PURPOSE:
 *      Get CPU busy/idle accumulated ticks, from system start or restart.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      busy_ticks_p  - the accumulated ticks.
 *      idle_ticks_p  - the accumulated idle ticks.
 *
 * RETURN:
 *      SYSFUN_OK                     -- Successfully.
 *      SYSFUN_RESULT_INVALID_ARG     -- Failed.
 *
 * NOTES:
 *      None.
 */
UI32_T SYSFUN_GetCpuUsage(UI32_T *busy_ticks_p, UI32_T *idle_ticks_p);

/* FUNCTION NAME : SYSFUN_GetMemoryUsage
 * PURPOSE:
 *      Get usable/available memory size.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      total_bytes_p - total usable memory size.
 *      free_bytes_p  - available memory size.
 *
 * RETURN:
 *      SYSFUN_OK                     -- Successfully.
 *      SYSFUN_RESULT_INVALID_ARG     -- Failed.
 *
 * NOTES:
 *      None.
 */
UI32_T SYSFUN_GetMemoryUsage(UI32_T *total_bytes_p, UI32_T *free_bytes_p);

/* FUNCTION NAME : SYSFUN_Yield
 * PURPOSE:
 *      The task that call this API will relinquish
 *      CPU voluntarily.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      On linux, the calling task will then be moved
 *      to  the  end  of the queue for its static
 *      priority and a new task gets to run. However,
 *      if the current task is the only task in the highest
 *      priority list at that time, this task will continue
 *      to run after calling this function.
 */
void SYSFUN_Yield(void);

/*-------------------------
 * Kernel
 *-------------------------
 */

/*------------------------------------------------------------------------------
 * Function : SYSFUN_Syscall
 *------------------------------------------------------------------------------
 * Purpose  : This function will call the corresponding handler via system call.
 * INPUT    : syscall_id  - The command id; ref. SYSFUN_SYSCALL_ID_E
 *            arg1 ~ arg5 - the arguments for the command.
 * OUTPUT   : Depend on the corresponding handler.
 * RETURN   : Depend on the corresponding handler.
 * NOTE     : None.
 *-----------------------------------------------------------------------------*/
UI32_T SYSFUN_Syscall(UI32_T syscall_id, UI32_T arg1, UI32_T arg2, UI32_T arg3, UI32_T arg4, UI32_T arg5);

UI32_T SYSFUN_SetSysClock(UI32_T *t);

UI32_T SYSFUN_RegisterSignal(int sig_offset, void (*sig_handler)(int));

UI32_T  SYSFUN_Sleep_Interruptible (UI32_T  seconds);

UI32_T SYSFUN_SendSignal(UI32_T taskno, int sig_offset);

UI32_T SYSFUN_mktime(int year, int month, int day, int hour,
                     int minute, int second, UI32_T *seconds1970);

//UI32_T SYSFUN_gmtime(UI32_T seconds1970, struct tm *result);

/*------------------------------------------------------------------------------
 * Function : SYSFUN_EnterAutoBaud
 *------------------------------------------------------------------------------
 * Purpose  : Set the uart attrib for auto baudrate detection.
 * INPUT    : UART_handle  - uart handle.
 * RETURN   : SYSFUN_OK.
 * NOTE     : None.
 *-----------------------------------------------------------------------------*/
UI32_T SYSFUN_EnterAutoBaud(UI32_T UART_handle);

/*------------------------------------------------------------------------------
 * Function : SYSFUN_LeaveAutoBaud
 *------------------------------------------------------------------------------
 * Purpose  : Restore the uart attrib.
 * INPUT    : UART_handle  - uart handle.
 * RETURN   : SYSFUN_OK.
 * NOTE     : None.
 *-----------------------------------------------------------------------------*/
UI32_T SYSFUN_LeaveAutoBaud(UI32_T UART_handle);

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
int  SYSFUN_Val2HexStr(unsigned char code, char *buf);

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
char SYSFUN_VisibleCode(unsigned char code);

#endif

