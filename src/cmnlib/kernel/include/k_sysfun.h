#ifndef     _K_SYSFUN_H
#define     _K_SYSFUN_H

/* INCLUDE FILE DECLARATIONS
 */

/* Accton include files
 */
#include "sys_type.h"
#include "sys_bld.h"

/* NAMING CONSTANT DECLARATIONS
 */

/*  TASK option */
#define SYSFUN_TASK_NAME_LENGTH         15

/*  Timeout Parameter   */
#define SYSFUN_TIMEOUT_NOWAIT           0           /* No wait for timeout parameter    */
#define SYSFUN_TIMEOUT_WAIT_FOREVER     -1          /* Wait forever for timeout paramter    */
                                                    /* others is the timeout ticks      */

/*  Event Function  */
#define SYSFUN_EVENT_WAIT_ALL           0x00        /*  Wait all events occur       */
#define SYSFUN_EVENT_NOWAIT             0x01        /*  Check and receive events but no wait    */
#define SYSFUN_EVENT_WAIT_ANY           0x02        /*  Wait any one events occur   */

/*  Return Value of SYSFUN  */
#define SYSFUN_OK                       0x00        /*  Successfully do the function            */
#define SYSFUN_RESULT_TIMEOUT           0x01        /*  It's timeout, no message, semaphore, event is got   */
#define SYSFUN_RESULT_NO_AVAIL_TIMER    0x02        /*  No more timer could be used, exceed max. timer number   */
#define SYSFUN_RESULT_INVALID_ARG       0x03        /*  Invalid argument in calling function    */
#define SYSFUN_RESULT_OBJ_DELETED       0x05        /*  Task already deleted                    */
#define SYSFUN_RESULT_INVALID_ID        0x06        /*  Invalid object ID for task, message, or semaphore   */
#define SYSFUN_RESULT_NO_NAMED_TASK     0x09        /*  Can't find the named task in system */
#define SYSFUN_RESULT_EINTR             0x0A        /*  The operation is interrupted due to a signal being caught */
#define SYSFUN_RESULT_NO_SPACE_4_TASK   0x0F        /*  No more space for task-creation */
#define SYSFUN_RESULT_STACK_TOO_SMALL   0x10        /*  stack size too small to create task */
#define SYSFUN_RESULT_PRIO_OUT_RANGE    0x11        /*  specified priority out of range */
#define SYSFUN_RESULT_CANT_CREATE_MSG_Q 0x33      /*  Can't create the message queue  */
#define SYSFUN_RESULT_MSG_Q_DELETED     0x36        /*  Queue deleted while task waiting.       */
#define SYSFUN_RESULT_NO_MESSAGE        0x37        /*  No message in queue for SYSFUN_TIMEOUT_NOWAIT   */
#define SYSFUN_RESULT_TASK_WAIT_MSG     0x38        /*  there is tasks waiting for message
                                                     *  when Q is deleted. Not all OS could implement this
                                                     *  function, at lease VxWorks do not support
                                                     *  this function, so check it before use it.
                                                     */
#define SYSFUN_RESULT_NO_EVENT_SET      0x3C        /*  No event is set for SYSFUN_EVENT_NOWAIT */
#define SYSFUN_RESULT_NO_SEMAPHORE      0x41        /*  No more available semaphore could be used in this system */
#define SYSFUN_RESULT_SEM_NOSIGNAL      0x42        /*  The waited semaphore is not set/give/v operation */
#define SYSFUN_RESULT_SEM_DELETED       0x43        /*  Semaphore is deleted                    */
#define SYSFUN_RESULT_MSG_Q_FULL        0x80        /*  Message queue is full, can't send message   */
#define SYSFUN_RESULT_CALL_FROM_ISR     0x81        /*  Invalid use system function, call from ISR  */
#define SYSFUN_RESULT_SYSFUN_NOT_INIT   0x82        /*  Not call SYSFUN_Init before call SYSFUN_xxx */
#define SYSFUN_RESULT_Q_FULL            0x83        /*  Queue is full                               */
#define SYSFUN_RESULT_Q_EMPTY           0x84        /*  Queue is empty                              */
#define SYSFUN_RESULT_TOO_MANY_REQUEST  0x85        /*  Can add more request to the function        */
#define SYSFUN_RESULT_ERROR             0xFFFFFFFF  /*  error with unknown reason */

/* constant for sched policy
 */
#define SYSFUN_SCHED_DEFAULT 0
#define SYSFUN_SCHED_FIFO    1
#define SYSFUN_SCHED_RR      2

/* Command ID of syscall for external use.
 */
enum SYSFUN_SYSCALL_ID_E
{
    SYSFUN_SYSCALL_INTERNAL = 0,
    SYSFUN_SYSCALL_L_IPCMEM,
    SYSFUN_SYSCALL_L_MM,
    SYSFUN_SYSCALL_PHYADDR_ACCESS,
    SYSFUN_SYSCALL_PHYSICAL_ADDR_ACCESS,/* anzhen.zheng, 2/19/2008 */
    SYSFUN_SYSCALL_I2C,
    SYSFUN_SYSCALL_SYSJOBQ,  /* unused */
    SYSFUN_SYSCALL_IML_MGR,
    SYSFUN_SYSCALL_VLAN_MGR,
    SYSFUN_SYSCALL_AMTRL3_MGR,
    SYSFUN_SYSCALL_ROUTE_MGR,
    SYSFUN_SYSCALL_DYING_GASP,
    SYSFUN_SYSCALL_VLAN_NET
};

/* MACRO FUNCTION DECLARATIONS
 */

#if (SYS_CPNT_PRINTF == FALSE)
    #define SYSFUN_Debug_Printf   SYSFUN_NullPrintf
#elif SYS_CPNT_PRINTF == TRUE
    #define SYSFUN_Debug_Printf   printk
#endif

/* DATA TYPE DECLARATIONS
 */

typedef struct SYSFUN_CpuRecord_S
{
    UI32_T  used_ticks;          /* ticks used by task between reporting interval
                                  * l_pt will modify this field, so we should not put
                                  * the critical field within first 4 bytes(This 4 bytes
                                  * will be modified by l_pt to maintain free list.
                                  */
    UI32_T  tid;                 /* 0-this utcb is not used, >0- task id */
    UI32_T  accumulated_ticks;   /* ticks used by task from started */
    void*   user_buffer;         /* buffer for user to keep cpu utilization */
}   SYSFUN_CpuRecord_T;



/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
#define SYSFUN_NullPrintf(...)

/* FUNCTION NAME : SYSFUN_Init
 * PURPOSE:
 *      Initialize system function working environment; includes
 *      event working environment for VxWorks - hook function on
 *      task creation.
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
 *      1. This function is called by root().
 */
void SYSFUN_Init(void);

/* FUNCTION NAME : SYSFUN_Create_InterCSC_Relation
 * PURPOSE:
 *      This function initializes all function pointer registration operations.
 * INPUT:
 *      None.
 * OUTPUT:
 *      None.
 * RETURN:
 *      None.
 * NOTES:
 *      None.
 */
void SYSFUN_Create_InterCSC_Relation(void);

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

/* FUNCTION NAME : SYSFUN_NonPreempty
 * PURPOSE:
 *      System enter non-preempty mode, no task schdule is occurs.
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
 *      1. When you call reschedule function, system may change to Preempty(), eg. Semaphore,
 *         wait message Q. Please reference each OS about preempty/nonpreempty function.
 */
void SYSFUN_NonPreempty (void);
#define SYSFUN_Lock()   SYSFUN_NonPreempty()


/* FUNCTION NAME : SYSFUN_Preempty
 * PURPOSE:
 *      System leave non-preempty mode, task schedule will be occured based on priority.
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
void SYSFUN_Preempty (void);
#define SYSFUN_UnLock()   SYSFUN_Preempty()

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

/*--------------------------
 *  Interrupt lock/unlock
 *--------------------------
 */

/* FUNCTION NAME : SYSFUN_InterruptLock
 * PURPOSE:
 *      Set interrupt lock, disable hardware interrupt.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      interrupt mask value for disable interrupt.
 * NOTES:
 *      1. For pSos, use SYSFUN_DISABLE_ALL as parameter.
 *         SYSFUN_DISABLE_ALL : mask off all interrupt
 */
UI32_T SYSFUN_InterruptLock(void);


/* FUNCTION NAME : SYSFUN_InterruptUnlock
 * PURPOSE:
 *      Set interrupt lock, enable hardware interrupt.
 *
 * INPUT:
 *      x   --  interrupt mask value, return-value of SYSFUN_InterruptLock().
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 * NOTES:
 *
 */
void SYSFUN_InterruptUnlock(UI32_T int_mask);

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
UI32_T  SYSFUN_TakeSem (UI32_T sem_id, int timeout);


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
UI32_T  SYSFUN_GiveSem(UI32_T sem_id);


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
 *		2. There is some differenct level message, user can use it.
 */
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
 * Memory Copy Function
 *-------------------------
 */

/* FUNCTION NAME : SYSFUN_CopyFromUser
 * PURPOSE:
 *      This function is copy a memory area in user-space to a memory area in kernel-space.
 * INPUT:
 *      src_p  - original memory area. (in user-space)
 *      size   - the size of destined memory area.
 *
 * OUTPUT:
 *      dst_p  - destined memory area.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      None.
 */
void SYSFUN_CopyFromUser(void *dst_p, void *src_p, UI32_T size);

/* FUNCTION NAME : SYSFUN_CopyToUser
 * PURPOSE:
 *      This function is copy a memory area in kernel-space to a memory area in user-space.
 * INPUT:
 *      src_p  - original memory area. (in kernel-space)
 *      size   - the size of destined memory area.
 *
 * OUTPUT:
 *      dst_p  - destined memory area. (in user-space)
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      None.
 */
void SYSFUN_CopyToUser(void *dst_p, void *src_p, UI32_T size);

/*-------------------------
 * Syscall-related
 *-------------------------
 */

/* FUNCTION NAME : SYSFUN_RegisterCallBackFunc
 * PURPOSE:
 *      This function is to register callback function for specific command of
 *      system call.
 * INPUT:
 *      syscall_cmd_id  - the command id.
 *      func            - the callback function.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      The limitations of callback function:
 *      1. The number of argument of callback function must be less than or equal to 5.
 *      2. The type of return value of callback function is UI32_T.
 */
void SYSFUN_RegisterCallBackFunc(UI32_T syscall_cmd_id, void *func);

/*--------------------------
 *  System Resource
 *--------------------------
 */

/* FUNCTION NAME : SYSFUN_GetCpuUsage
 * PURPOSE:
 *      Get CPU busy/idle accumulated ticks, from system start or restart.
 *
 * INPUT:
 *      None.
 *
 * OUTPUT:
 *      busy_ticks_p  - the accumulated busy ticks.
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

#endif /* _K_SYSFUN_H */
