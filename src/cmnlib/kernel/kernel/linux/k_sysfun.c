/* MODULE NAME:  sysfun.c (Kernel Space)
 * PURPOSE:
 *      This module is unified system call for all platforms.
 * NOTES:
 *
 * HISTORY
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
 *
 * Copyright(C)      Accton Corporation, 1999, 2000, 2001, 2002, 2004, 2006
 */

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_hwcfg.h"

/* linux specific include files
 */
#include <linux/smp_lock.h>
#include <linux/jiffies.h>  /* jiffies, jiffies_to_clock_t(), clock_t_to_jiffies() */
#include <linux/kernel.h>   /* dump_stack() */
#include <linux/sched.h>    /* for current, jiffies */
#define EXPORT_SYMTAB
#include <linux/module.h>
#include <linux/timer.h>    /* for timer */
#include <linux/kernel_stat.h> /* for kstat_cpu, ... */
#include <asm/semaphore.h>  /* for semaphore */
#include <asm/uaccess.h>    /* for copy_from_user(), copy_to_user() */

/* linux includes files for specific hardware
 */
#if defined (SYS_HWCFG_CPU_82XX) || defined (SYS_HWCFG_CPU_85XX)
#include <asm/reg.h>        /* mtmsr(), mfspr(), mtspr() */
#endif

#ifdef SYS_HWCFG_CPU_MIPS
#include <asm/pgtable.h>
#include <asm/mipsregs.h>
#endif

#include <linux/syscalls.h>

/* Accton include files
 */
#include "sys_type.h"
#include "k_syscall.h"
#include "k_sysfun.h"
#include "sysfun_type.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define SYSFUN_DEBUG  FALSE


/* SYSFUN_LOGLEVEL_XXX are used for the argument log_level
 * of the macro function SYSFUN_LOGMSG()
 */
#define SYSFUN_LOGLEVEL_DEFAULT SYSFUN_LOGLEVEL_WARN
#define SYSFUN_LOGLEVEL_URGENT     KERN_EMERG
#define SYSFUN_LOGLEVEL_ALERT      KERN_ALERT
#define SYSFUN_LOGLEVEL_CRITICAL   KERN_CRIT
#define SYSFUN_LOGLEVEL_ERR        KERN_ERR
#define SYSFUN_LOGLEVEL_WARN       KERN_WARNING
#define SYSFUN_LOGLEVEL_NOTICE     KERN_NOTICE
#define SYSFUN_LOGLEVEL_INFO       KERN_INFO
#define SYSFUN_LOGLEVEL_DEBUG      KERN_DEBUG

/* MACRO FUNCTION DECLARATIONS
 */
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
 *      1.Use printk until SYSLOG_MGR is ready
 */
#define SYSFUN_LOGMSG(log_level, msg, ...)    \
({                                            \
    printk("\r\n");                           \
    printk(log_level);                        \
    printk(msg, ##__VA_ARGS__);                 \
})

#if SYSFUN_DEBUG
#define SYSFUN_DEBUG_Print printk
#else
#define SYSFUN_DEBUG_Print(...)
#endif

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static UI32_T SYSFUN_Operation(UI32_T cmd, UI32_T arg1, UI32_T arg2, UI32_T arg3, UI32_T arg4);
static void SYSFUN_WakeUpRecvEventTimeout(unsigned long event_sem);
static BOOL_T SYSFUN_SuspendThreadSelf (void);
static UI32_T SYSFUN_ResumeThread (UI32_T task_id);
static BOOL_T SYSFUN_SuspendTaskWithTimeout(UI32_T timeout);
static BOOL_T SYSFUN_ResumeTaskInTimeoutSuspense (UI32_T task_id);
static void SYSFUN_InitUTCB(char *task_name);
static void SYSFUN_DestroyUTCB(void);
static UI32_T SYSFUN_GetUserTcb(UI32_T task_id, SYSFUN_UserTcb_T **utcb_p);

/* STATIC VARIABLE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM BODIES
 */

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
void SYSFUN_Init ()
{
    /* jerry.du 20080918, add new system call sys_reboot */
    /* charlie_chen 20100728, sys_reboot is exported as GPL symbol
     * and can only be used by GPL kernel module. In order to
     * change acctonlkm as priorietary license, this function
     * call need to be removed.
     */
#if 0
    SYSFUN_RegisterCallBackFunc(SYSFUN_SYSCALL_REBOOT, sys_reboot);
#endif
    return;
}

/* FUNCTION NAME : SYSFUN_Create_InterCSC_Relation
 * PURPOSE: Create inter-CSC relationships.
 * INPUT:
 *      None.
 * OUTPUT:
 *      None.
 * RETURN:
 *      None.
 * NOTES:
 *      None.
 */
void SYSFUN_Create_InterCSC_Relation(void)
{
    SYSFUN_RegisterCallBackFunc(SYSFUN_SYSCALL_INTERNAL, SYSFUN_Operation);
}

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
UI32_T SYSFUN_TaskIdSelf (void)
{
    return (UI32_T)current->pid;
}

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
UI32_T  SYSFUN_TaskNameToID (char *task_name, UI32_T *task_id)
{
    struct task_struct *task_p;
    struct task_struct *task_g;

    read_lock(&tasklist_lock);
    /*for_each_process(task_p)  this will only traverse each process */
    do_each_thread(task_g, task_p) /* traverse each threads */
    {
        if (strncmp(task_p->utcb.task_name, task_name, sizeof(task_p->utcb.task_name)) == 0)
        {
            *task_id = task_p->pid;
            read_unlock(&tasklist_lock);
            return SYSFUN_OK;
        }
    } while_each_thread(task_g, task_p);

    read_unlock(&tasklist_lock);
    return SYSFUN_RESULT_NO_NAMED_TASK;
}

/* FUNCTION NAME : SYSFUN_TaskIDToName
 * PURPOSE:
 *      Retrieve Task name by task-id.
 * INPUT:
 *      task_id - the specified task ID.
 *                0 : the caller task.
 *      size    - the size of buffer (task_name) to receive task name.
 *
 * OUTPUT:
 *      task_name - Address of pointer which point to name of task retrieved.
 *
 * RETURN:
 *      SYSFUN_OK                   - Successfully.
 *      SYSFUN_RESULT_INVALID_ID    - specified ID is not valid.
 *      SYSFUN_RESULT_NO_NAMED_TASK - specified task was no name.
 *      SYSFUN_RESULT_INVALID_ARG   - not specified task name buffer space.
 *
 * NOTES:
 *      1. If specified buf_size is smaller than task-name, task-name will be
 *         truncated to fit the size (buf_size-1).
 *      2. On linux, task id could be thread id or process id.
 */
UI32_T  SYSFUN_TaskIDToName (UI32_T task_id, char *task_name, UI32_T size)
{
    SYSFUN_UserTcb_T *utcb_p;

    if (SYSFUN_GetUserTcb(task_id, &utcb_p) == SYSFUN_OK)
    {
        strncpy(task_name, utcb_p->task_name, size);
        task_name[size-1] = 0;
        return SYSFUN_OK;
    }

    return SYSFUN_RESULT_INVALID_ID;
}

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
void SYSFUN_NonPreempty (void)
{
    lock_kernel();/*need to fix */
}

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
void SYSFUN_Preempty (void)
{
    unlock_kernel();/*need to fix */
}

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
/* It is better to call sys_times(), however, it is exported by linux kernel
 * as GPL symbol. To keep acctonlkm as priorietary license, we cannot but
 * envaulate system tick by ourselves.
 */
#if 1 
    return (UI32_T)jiffies_64_to_clock_t(get_jiffies_64());
#else
    /* invoke linux kernel API to minimize porting effort */
    return (UI32_T)sys_times(NULL);
#endif
}

// TODO: not support
#if 0
/* FUNCTION NAME : SYSFUN_SetSysTick
 * PURPOSE:
 *      Set system ticks.
 * INPUT:
 *      UI32_T  tick
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
void SYSFUN_SetSysTick(UI32_T tick)
{
    jiffies = (unsigned long)tick;
}
#endif

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
UI32_T SYSFUN_InterruptLock(void)
{
    unsigned long int_mask;

    local_irq_save(int_mask);
    return (UI32_T)int_mask;
}

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
void SYSFUN_InterruptUnlock(UI32_T int_mask)
{
    local_irq_restore((unsigned long)int_mask);
}

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
void SYSFUN_LogMsg(char *msg_text, I32_T arg1, I32_T arg2, I32_T arg3, I32_T arg4, I32_T arg5, I32_T arg6)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_DEFAULT, msg_text, arg1, arg2, arg3, arg4, arg5, arg6);
}

void SYSFUN_LogUrgentMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_URGENT, "%s", msg_text);
}

void SYSFUN_LogAlertMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_ALERT, "%s", msg_text);
}

void SYSFUN_LogCriticalMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_CRITICAL, "%s", msg_text);
}

void SYSFUN_LogErrMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_ERR, "%s", msg_text);
}

void SYSFUN_LogWarnMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_WARN, "%s", msg_text);
}

void SYSFUN_LogNoticeMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_NOTICE, "%s", msg_text);
}

void SYSFUN_LogInfoMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_INFO, "%s", msg_text);
}

void SYSFUN_LogDebugMsg(char *msg_text)
{
    SYSFUN_LOGMSG(SYSFUN_LOGLEVEL_DEBUG, "%s", msg_text);
}

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
UI32_T SYSFUN_SendEvent (UI32_T tid, UI32_T event)
{
    /* LOCAL VARIABLE DECLARATION
     */
    unsigned long int_mask;
    struct task_struct *target_task_p;
    SYSFUN_UserTcb_T   *utcb;

    /* BODY */
    if (0 == tid)
        return SYSFUN_RESULT_INVALID_ID;
    if (0 == event)
        return SYSFUN_RESULT_INVALID_ARG;

    local_irq_save(int_mask);
    target_task_p = find_task_by_pid(tid);
    if(target_task_p==NULL)
    {
        local_irq_restore(int_mask);
        return SYSFUN_RESULT_OBJ_DELETED;
    }

    utcb = &(target_task_p->utcb);

    utcb->event_set |= event;
    SYSFUN_DEBUG_Print("\r\n<7>%s: %d to %d, [event] send: %08x now: %08x", __FUNCTION__, current->pid, tid, event, utcb->event_set);

    if (utcb->event_waited )
    {
        event = utcb->event_set & utcb->event_waited;
        if (utcb->event_is_all)
        {
            if (utcb->event_waited  == event)
                up(&(utcb->event_sem));
        }
        else
        {
            if (event != 0)
                up(&(utcb->event_sem));
        }
    }

    local_irq_restore(int_mask);
    return  SYSFUN_OK;

}   /* End of SYSFUN_SendEvent () */

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
 *                     SYSFUN_TIMEOUT_NOWAIT       - no wait
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
UI32_T SYSFUN_ReceiveEvent (UI32_T wait_events, UI32_T flags, int timeout,
                            UI32_T *received_events)
{
    /* LOCAL VARIABLE DECLARATION
     */
    unsigned long  int_mask;
    UI32_T    all, event;
    SYSFUN_UserTcb_T *utcb = &(current->utcb);
    int       ret;
    struct    timer_list timer;
    unsigned long expired_time;

    /* BODY */
    if ( wait_events == 0 )
        return SYSFUN_RESULT_NO_EVENT_SET;

    all = (flags == SYSFUN_EVENT_WAIT_ALL);

    *received_events = 0;

    expired_time = jiffies + clock_t_to_jiffies((unsigned long)timeout);

RE_GET:
    local_irq_save(int_mask);          /*  Interrupt Disable   */

    event = utcb->event_set & wait_events;
    if ( (all && (event == wait_events)) || (!all && event!=0) )
    {
        /* wait event is satisfied */
        utcb->event_set ^= (*received_events = event);
        utcb->event_waited = 0;
        SYSFUN_DEBUG_Print("\r\n<7>%s: %d, [event] wait: %08x  recv: %08x now: %08x", __FUNCTION__, current->pid, wait_events, *received_events, utcb->event_set);
        local_irq_restore(int_mask);          /*  Interrupt Enable    */
        return SYSFUN_OK;
    }

    /* wait event is not satisfied
     */
    if (timeout==0)     /*  No wait */
    {
        local_irq_restore(int_mask);
        return  SYSFUN_RESULT_TIMEOUT;
    }

    /* 1. Ready to wait semaphore
     * 2. save wait condition
     */
    utcb->event_waited = wait_events;
    utcb->event_is_all = all;

    /* When the process run to here, it means the condition does not meet
     * in previous if block. We need to clear the semaphore here to assure
     * that the semaphore is set due to the condition meets.
     */
    while (down_trylock(&(utcb->event_sem)) == 0);    /* clear the semaphore */

    local_irq_restore(int_mask);

    /* need to add a timer to wake up the blocked process
     * if its timeout setting is not "wait forever".
     */
    if(timeout!=SYSFUN_TIMEOUT_WAIT_FOREVER)
    {
        init_timer(&timer);
        timer.data     = (unsigned long)&(utcb->event_sem);
        timer.function = SYSFUN_WakeUpRecvEventTimeout;
        timer.expires  = expired_time;
        add_timer(&timer);
    }

    /* Race condition will occur, but no harmful!
     */
    ret = down_interruptible(&(utcb->event_sem));

    /* The timer might have not been activated yet.
     * Just clean it up here. Nothing wrong even if the timer has been activated.
     */
    if(timeout!=SYSFUN_TIMEOUT_WAIT_FOREVER)
    {
        del_timer(&timer);
    }

    if(ret==-EINTR)
        return SYSFUN_RESULT_EINTR;

    /* set timeout as 0
     * so the process will be timeout when going to RE_GET
     */
    timeout=0;
    goto RE_GET;
}   /* End of SYSFUN_ReceiveEvent() */

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
void SYSFUN_Debug_DumpTaskStack(void)
{
    dump_stack();
}

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
void SYSFUN_SetMasterStatusRegister(UI32_T val)
{
#if defined (SYS_HWCFG_CPU_82XX) || defined (SYS_HWCFG_CPU_85XX)
    mtmsr(val);
#endif
}

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
UI32_T SYSFUN_EnableDataCache(void)
{
#if defined (SYS_HWCFG_CPU_82XX) || defined (SYS_HWCFG_CPU_85XX)
    mtspr(SPRN_HID0, mfspr(SPRN_HID0) | HID0_DCE);
/*
    unsigned long hid0;

    __asm__ __volatile__(
        "mfspr %0, 0x3F0\n\t"
        "ori %0, %0, 0x4000\n\t"
        "sync\n\t"
        "mtspr 0x3F0, %0"
        : "=r" (hid0));
 */
#endif

#ifdef SYS_HWCFG_CPU_MIPS
/* MIPS32 CPU with standard TLB MMU uses the K0 field in CP0 Config register
 * to control the cache attribute of Kseg0. The cache attribute of useg/kuseg
 * is determined by TLB. The K0 field controls both i-cache and d-cache.
 * This implementation will turn on the cache on Kseg0.
 */

    change_c0_config(CONF_CM_CMASK, PAGE_CACHABLE_DEFAULT>>9);
#endif
    return SYSFUN_OK;
}

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
UI32_T SYSFUN_DisableDataCache(void)
{
#if defined (SYS_HWCFG_CPU_82XX) || defined (SYS_HWCFG_CPU_85XX)
    mtspr(SPRN_HID0, mfspr(SPRN_HID0) & ~HID0_DCE);
#endif

#ifdef SYS_HWCFG_CPU_MIPS
/* MIPS32 CPU with standard TLB MMU uses the K0 field in CP0 Config register
 * to control the cache attribute of Kseg0. The cache attribute of useg/kuseg
 * is determined by TLB. The K0 field controls both i-cache and d-cache.
 * This implementation will turn off the cache on Kseg0.
 */
    change_c0_config(CONF_CM_CMASK, _CACHE_UNCACHED>>9);
#endif

    return SYSFUN_OK;
}

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
UI32_T SYSFUN_EnableInstructionCache(void)
{
#if defined (SYS_HWCFG_CPU_82XX) || defined (SYS_HWCFG_CPU_85XX)
    mtspr(SPRN_HID0, mfspr(SPRN_HID0) | HID0_ICE);
/*
    unsigned long hid0;

    __asm__ __volatile__(
        "mfspr %0, 0x3F0\n\t"
        "ori %0, %0, 0x8000\n\t"
        "sync\n\t"
        "mtspr 0x3F0, %0"
        : "=r" (hid0));
 */
#endif

#ifdef SYS_HWCFG_CPU_MIPS
/* MIPS32 CPU with standard TLB MMU uses the K0 field in CP0 Config register
 * to control the cache attribute of Kseg0. The cache attribute of useg/kuseg
 * is determined by TLB. The K0 field controls both i-cache and d-cache.
 * This implementation will turn on the cache on Kseg0.
 */
    SYSFUN_EnableDataCache();
#endif
    return SYSFUN_OK;
}

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
UI32_T SYSFUN_DisableInstructionCache(void)
{
#if defined (SYS_HWCFG_CPU_82XX) || defined (SYS_HWCFG_CPU_85XX)
    mtspr(SPRN_HID0, mfspr(SPRN_HID0) & ~HID0_ICE);
#endif
#ifdef SYS_HWCFG_CPU_MIPS
/* MIPS32 CPU with standard TLB MMU uses the K0 field in CP0 Config register
 * to control the cache attribute of Kseg0. The cache attribute of useg/kuseg
 * is determined by TLB. The K0 field controls both i-cache and d-cache.
 * This implementation will turn off the cache on Kseg0.
 */
    SYSFUN_DisableDataCache();
#endif

    return SYSFUN_OK;
}

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
void SYSFUN_CopyFromUser(void *dst_p, void *src_p, UI32_T size)
{
    copy_from_user(dst_p, src_p, size);
}

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
void SYSFUN_CopyToUser(void *dst_p, void *src_p, UI32_T size)
{
    copy_to_user(dst_p, src_p, size);
}

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
void SYSFUN_RegisterCallBackFunc(UI32_T syscall_cmd_id, void *func)
{
    SYSCALL_RegisterCallBackFunc(syscall_cmd_id, func);
}

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
UI32_T SYSFUN_GetCpuUsage(UI32_T *busy_ticks_p, UI32_T *idle_ticks_p)
{
    UI32_T user, nice, system, idle, iowait, irq, softirq, steal;
    UI32_T total;
    int i;

    user = nice = system = idle = iowait = irq = softirq = steal = 0;

    for_each_possible_cpu(i)
    {
        user += cputime64_to_clock_t(kstat_cpu(i).cpustat.user);
        nice += cputime64_to_clock_t(kstat_cpu(i).cpustat.nice);
        system += cputime64_to_clock_t(kstat_cpu(i).cpustat.system);
        idle += cputime64_to_clock_t(kstat_cpu(i).cpustat.idle);
        iowait += cputime64_to_clock_t(kstat_cpu(i).cpustat.iowait);
        irq += cputime64_to_clock_t(kstat_cpu(i).cpustat.irq);
        softirq += cputime64_to_clock_t(kstat_cpu(i).cpustat.softirq);
        steal += cputime64_to_clock_t(kstat_cpu(i).cpustat.steal);
    }

    total = user + nice + system + idle + iowait + irq + softirq + steal;

    *idle_ticks_p = idle + iowait;
    *busy_ticks_p = total - *idle_ticks_p;

    return SYSFUN_OK;
}

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
UI32_T SYSFUN_GetMemoryUsage(UI32_T *total_bytes_p, UI32_T *free_bytes_p)
{
/* convert to bytes
 */
#define B(x) ((x) << PAGE_SHIFT)

    struct sysinfo si;

    si_meminfo(&si);

    *total_bytes_p = B(si.totalram);
    *free_bytes_p = B(si.freeram);

    return SYSFUN_OK;
#undef B
}

/* LOCAL SUBPROGRAM BODIES
 */
/* FUNCTION NAME : SYSFUN_Operation
 * PURPOSE:
 *      This function is to dispatch the actions that must be execute in kernel to
 *       thecorresponding handler by command id.
 * INPUT:
 *      cmd  - the command id.
 *      arg1 ~ arg4 - the arguments for the command.
 *
 * OUTPUT:
 *      Depend on the given cmd.
 *
 * RETURN:
 *      Depend on the given cmd.
 *
 * NOTES:
 *      None.
 */
static UI32_T SYSFUN_Operation(UI32_T cmd, UI32_T arg1, UI32_T arg2, UI32_T arg3, UI32_T arg4)
{
    UI32_T tmp_arg, tmp_arg2;
    UI32_T ret = SYSFUN_RESULT_INVALID_ARG;
    char tmp_task_name[SYSFUN_TASK_NAME_LENGTH+1];

    switch(cmd)
    {
        case SYSFUN_INTERNAL_SYSCALL_VALIDITY:
            SYSFUN_DEBUG_Print("\r\n<7>SYSFUN_INTERNAL_SYSCALL_VALIDITY\r\n");
            ret = SYSFUN_INTERNAL_SYSCALL_VALIDITY_VALUE;
            break;
        case SYSFUN_INTERNAL_SYSCALL_INIT_UTCB:
            strncpy_from_user(tmp_task_name, (char *)arg1, sizeof(tmp_task_name));
            tmp_task_name[sizeof(tmp_task_name)-1] = 0;
            SYSFUN_InitUTCB(tmp_task_name);
            break;
        case SYSFUN_INTERNAL_SYSCALL_DESTROY_UTCB:
            SYSFUN_DestroyUTCB();
            break;
        case SYSFUN_INTERNAL_SYSCALL_TASKNAME_TO_ID:
            strncpy_from_user(tmp_task_name, (char *)arg1, sizeof(tmp_task_name));
            tmp_task_name[sizeof(tmp_task_name)-1] = 0;
            ret = SYSFUN_TaskNameToID(tmp_task_name, &tmp_arg);
            SYSFUN_CopyToUser((void*)arg2, &tmp_arg, sizeof(UI32_T));
            break;
        case SYSFUN_INTERNAL_SYSCALL_TASKID_TO_NAME:
            tmp_arg = arg3 < sizeof(tmp_task_name) ? arg3 : sizeof(tmp_task_name);
            ret = SYSFUN_TaskIDToName(arg1, tmp_task_name, tmp_arg);
            SYSFUN_CopyToUser((char *)arg2, tmp_task_name, tmp_arg);
            break;
        case SYSFUN_INTERNAL_SYSCALL_SENDEVENT:
            /* arg1: UI32_T tid
             * arg2: UI32_T event
             * return: UI32_T
             */
            ret = SYSFUN_SendEvent(arg1, arg2);
            break;
        case SYSFUN_INTERNAL_SYSCALL_RECEIVEEVENT:
            /* arg1: UI32_T wait_events
             * arg2: UI32_T flags
             * arg3: int    timeout
             * arg4: UI32_T* received_events
             * return: UI32_T
             */
            ret = SYSFUN_ReceiveEvent(arg1, arg2, arg3, &tmp_arg);
            SYSFUN_CopyToUser((UI32_T *)arg4, &tmp_arg, sizeof(UI32_T));
            break;
#if 0 // TODO: not support
        case SYSFUN_INTERNAL_SYSCALL_SETSYSTICK:
            SYSFUN_SetSysTick(arg1);
            break;
#endif
        case SYSFUN_INTERNAL_SYSCALL_SUSPENDTASKSELF:
            ret = (UI32_T)SYSFUN_SuspendThreadSelf();
            break;
        case SYSFUN_INTERNAL_SYSCALL_RESUMETASK:
            ret = SYSFUN_ResumeThread(arg1);
            break;
        case SYSFUN_INTERNAL_SYSCALL_SUSPENDTASKWITHTIMEOUT:
            ret = (UI32_T)SYSFUN_SuspendTaskWithTimeout(arg1);
            break;
        case SYSFUN_INTERNAL_SYSCALL_RESUMETASKWITHTIMEOUT:
            ret = (UI32_T)SYSFUN_ResumeTaskInTimeoutSuspense(arg1);
            break;
        case SYSFUN_INTERNAL_SYSCALL_DUMPTASKSTACK:
            SYSFUN_Debug_DumpTaskStack();
            break;
        case SYSFUN_INTERNAL_SYSCALL_SETDCACHE:
            if((BOOL_T)arg1 == TRUE)
                ret = SYSFUN_EnableDataCache();
            else
                ret = SYSFUN_DisableDataCache();
            break;
        case SYSFUN_INTERNAL_SYSCALL_SETICACHE:
            if((BOOL_T)arg1 == TRUE)
                ret = SYSFUN_EnableInstructionCache();
            else
                ret = SYSFUN_DisableInstructionCache();
            break;
        case SYSFUN_INTERNAL_SYSCALL_GET_CPU_USAGE:
            /* arg1: UI32_T *busy_ticks_p
             * arg2: UI32_T *idle_ticks_p
             */
            ret = SYSFUN_GetCpuUsage(&tmp_arg, &tmp_arg2);
            SYSFUN_CopyToUser((UI32_T *)arg1, &tmp_arg, sizeof(UI32_T));
            SYSFUN_CopyToUser((UI32_T *)arg2, &tmp_arg2, sizeof(UI32_T));
            break;
        case SYSFUN_INTERNAL_SYSCALL_GET_MEMORY_USAGE:
            /* arg1: UI32_T *total_bytes_p
             * arg2: UI32_T *free_bytes_p
             */
            ret = SYSFUN_GetMemoryUsage(&tmp_arg, &tmp_arg2);
            SYSFUN_CopyToUser((UI32_T *)arg1, &tmp_arg, sizeof(UI32_T));
            SYSFUN_CopyToUser((UI32_T *)arg2, &tmp_arg2, sizeof(UI32_T));
            break;
        case SYSFUN_INTERNAL_SYSCALL_GETSYSTICK:
            /* arg1: UI32_T *tick
             */
            tmp_arg=SYSFUN_GetSysTick();
            SYSFUN_CopyToUser((UI32_T*)arg1, &tmp_arg, sizeof(UI32_T));
            ret = SYSFUN_OK;
            break;
        default:
            printk("\r\n<4>%s:Unknown cmd(%d).", __FUNCTION__, (int)cmd);
    }

    return ret;
}

static void SYSFUN_WakeUpRecvEventTimeout(unsigned long event_sem)
{
    up((struct semaphore*)event_sem);
}

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
BOOL_T SYSFUN_SuspendThreadSelf (void)
{
    SYSFUN_UserTcb_T *utcb_p = &(current->utcb);
    unsigned long int_mask;

    /* clear the semaphore
     */
    local_irq_save(int_mask);
    if (down_trylock(&(utcb_p->suspense_sem)) == 0)
    {
        while (down_trylock(&(utcb_p->suspense_sem)) == 0);
        local_irq_restore(int_mask);
        return TRUE;
    }
    local_irq_restore(int_mask);

    return (down_interruptible(&(utcb_p->suspense_sem)) != -EINTR);
}

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
 *      task_id is invalid if the thread and calling thread are not
 *      in the same process.
 */
UI32_T SYSFUN_ResumeThread (UI32_T task_id)
{
    SYSFUN_UserTcb_T *utcb_p;

    if (SYSFUN_GetUserTcb(task_id, &utcb_p) == SYSFUN_OK)
    {
        up(&(utcb_p->suspense_sem));
        return SYSFUN_OK;
    }

    return SYSFUN_RESULT_INVALID_ID;
}
/* FUNCTION NAME : SYSFUN_ResumeTimeoutThread
 * PURPOSE:
 *      Resume a thread suspended by timer.
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
 *      task_id is invalid if the thread and calling thread are not
 *      in the same process.
 */
static UI32_T SYSFUN_ResumeTimeoutThread  (UI32_T task_id)
{
   struct task_struct *task;
   /*thread can not resume itself
      * and check the value 
       *  Modify By Tony.Lei
      */   
   if (task_id == 0)
       return SYSFUN_RESULT_INVALID_ID;
   /**/
 	rcu_read_lock();
	task = find_task_by_pid(task_id);
	rcu_read_unlock();
	if (!task)
		goto out;
    
   /*wakeup the process that has been delayed by time*/ 
   wake_up_process(task);
   
   return SYSFUN_OK;
   
out:
   return SYSFUN_RESULT_INVALID_ID;
   
}

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
 *            FALSE -  The suspended task is resumed due to time out or interrupt.
 * NOTE     :
 *            1.This function MUST NOT BE called by an ISR.
 *            2.This function may return immediately if
 *              SYSFUN_ResumeTaskInTimeoutSuspense() was called after timeout
 *              last time. Users should take this case into considration.
 *-----------------------------------------------------------------------------*/
static BOOL_T SYSFUN_SuspendTaskWithTimeout(UI32_T timeout)
{
    SYSFUN_UserTcb_T *utcb_p = &(current->utcb);
    struct timer_list timer;
    unsigned long  int_mask;
    UI32_T ret;
    BOOL_T resume_before_expired;

    if (timeout == 0)
        return TRUE;

    /* clear the semaphore
     */
    local_irq_save(int_mask);
    if (down_trylock(&(utcb_p->suspense_sem)) == 0)
    {
        while (down_trylock(&(utcb_p->suspense_sem)) == 0);
        local_irq_restore(int_mask);
        return TRUE;
    }
    local_irq_restore(int_mask);

    init_timer(&timer);
    timer.data     = (unsigned long)&(utcb_p->suspense_sem);
    timer.function = SYSFUN_WakeUpRecvEventTimeout;
	/*jiffies is a 32-bit variable, maybe overflow. */
    timer.expires  = jiffies + clock_t_to_jiffies((unsigned long)timeout);
    add_timer(&timer);

    ret = down_interruptible(&(utcb_p->suspense_sem));
	/*must be use macro time_brfore check timeout. it yield the correct value even if jiffies wraparound occurred.*/
    resume_before_expired = (ret != -EINTR && time_before(jiffies, timer.expires));

    del_timer_sync(&timer);
    return resume_before_expired;
}

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
static BOOL_T SYSFUN_ResumeTaskInTimeoutSuspense (UI32_T task_id)
{
    return (SYSFUN_ResumeThread(task_id) == SYSFUN_OK);
}


static void SYSFUN_InitUTCB(char *task_name)
{
    SYSFUN_UserTcb_T *utcb_p = &(current->utcb);

    /* Initiate sempaphores for event and suspend task
     * suspend with timeout has not been implemented on Linux
     */
    utcb_p->event_set  = utcb_p->event_waited = 0;
    utcb_p->event_is_all = TRUE;
    sema_init(&(utcb_p->event_sem), 0);
    sema_init(&(utcb_p->suspense_sem), 0);

    /* initiate stack peak size
     * note: stack peak size has not been implemented on Linux
     */
    utcb_p->stack_peak_size = 0;

    /* initiate cpu record
     * note: cpu record has not been implemented on Linux
     */
    utcb_p->cpu_rec.tid = current->pid;
    utcb_p->cpu_rec.used_ticks = utcb_p->cpu_rec.accumulated_ticks = 0;

    strncpy(utcb_p->task_name, task_name, sizeof(utcb_p->task_name)-1);
    utcb_p->task_name[sizeof(utcb_p->task_name)-1] = 0;
}

static void SYSFUN_DestroyUTCB(void)
{
    SYSFUN_UserTcb_T *utcb_p = &(current->utcb);

    utcb_p->task_name[0] = 0;

    return;
}

/*------------------------------------------------------------------------------
 * Function : SYSFUN_GetUserTcb
 *------------------------------------------------------------------------------
 * Purpose  : Get task specific UserTcb_T.
 * INPUT    : task_id - The task identifier of the target task.
 * OUTPUT   : utcb_p  - The task specific UserTcb_T.
 * RETURN   : SYSFUN_OK                   - Successfully.
 *            SYSFUN_RESULT_OBJ_DELETED   - specified task is deleted.
 * NOTE     : None
 *-----------------------------------------------------------------------------*/
static UI32_T SYSFUN_GetUserTcb(UI32_T task_id, SYSFUN_UserTcb_T **utcb_p)
{
    struct task_struct *task_p;

    if (task_id == 0)
    {
        *utcb_p = &current->utcb;
        return SYSFUN_OK;
    }

    read_lock(&tasklist_lock);
    if ((task_p = find_task_by_pid((int)task_id)) != NULL)
    {
        *utcb_p = &task_p->utcb;
        read_unlock(&tasklist_lock);
        return SYSFUN_OK;
    }
    read_unlock(&tasklist_lock);
    return SYSFUN_RESULT_OBJ_DELETED;
}

MODULE_AUTHOR("Edge-Core Networks");
MODULE_DESCRIPTION("Edge-Core lkm");
MODULE_LICENSE("Proprietary");
