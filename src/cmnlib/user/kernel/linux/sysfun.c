/* MODULE NAME:  sysfun.c (User Space)
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
/* linux specific include files
 */
#include <sys/ioctl.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sem.h>      /* for semget(), semctl(), semop(), semtimedop() */
#include <sys/shm.h>      /* for shmget(), shmat(), shmdt() */
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sched.h>        /* for sched_yield() */
#ifndef ES4308MA_POEPD_FLF_ZZ
#define __USE_GNU
#endif
#include <sys/msg.h>      /* for msgget(), msgsnd(), msgrcv(), msgctl() */
#include <sys/utsname.h>
#include <sys/reboot.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>        /* O_RDWR, O_NOCTTY, O_NONBLOCK */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>

#ifndef ES4308MA_POEPD_FLF_ZZ
#define __USE_XOPEN2K
#endif
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include <syscall.h>      /* for system call */
#include <pthread.h>
#include <semaphore.h>

#define TTYDEFCHARS       /* It is necessary to define TTYDEFCHARS, so that we can use ttydefchars to set UART to default mode. */
#include <termios.h>      /* for tcgetattr(), tcsetattr(), ... */
#undef TTYDEFCHARS

/* Accton include files
 */
#include "sys_dflt.h"
#include "sys_bld.h"
#include "sys_hwcfg.h"
#include "l_math.h"
#include "sysfun.h"
#include "syslog_om.h"
#include "syslog_type.h"
#include "sys_module.h"
#include "syslog_pmgr.h"

/* The include file <time.h> need us to define a Macros that can arise
 * several warnings. So we add the prototypes of these POSIX timer
 * APIs here to avoid compilation warnings
 */
int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid);
int timer_delete(timer_t timerid);
int timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue);
int timer_gettime(timer_t timerid, struct itimerspec *value);

/* SYSFUN internal debugging switches */
#ifdef SYSFUN_DEBUG
# define SYSFUN_DEBUG_TRACE_TASKPRIO                TRUE
# define SYSFUN_DEBUG_MSGQ_SANITY_CHECK             TRUE
#endif /* SYSFUN_DEBUG */

/* NAMING CONSTANT DECLARATIONS
 */
#if 0
#ifdef SYSFUN_RD_DEBUG_ENABLE_DUMP_TASK_STACK
#undef SYSFUN_RD_DEBUG_ENABLE_DUMP_TASK_STACK
#define SYSFUN_RD_DEBUG_ENABLE_DUMP_TASK_STACK    TRUE
#endif
#endif



#define SYSFUN_TIMEOUT_CHECK_INTERVAL_IN_CLOCKTICK  5
#define SYSFUN_TIMEOUT_CHECK_INTERVAL_IN_USEC       (SYSFUN_TIMEOUT_CHECK_INTERVAL_IN_CLOCKTICK * 1000000L / SYS_BLD_TICKS_PER_SECOND)

#define SYSFUN_MSGQ_INVALID_ID                      ((UI32_T)-1)  /* invalid message queue id. */

/* for SYSFUN_EnableDataCache(), SYSFUN_DisableDataCache(),
 *     SYSFUN_EnableInstructionCache(), SYSFUN_DisableInstructionCache()
 */
#if defined (SYS_HWCFG_CPU_82XX) || defined (SYS_HWCFG_CPU_85XX)
#define HID0_ICE    (1<<15)     /* Instruction Cache Enable */
#define HID0_DCE    (1<<14)     /* Data Cache Enable */
#endif

/* SYSFUN_LOGLEVEL_XXX are used for the argument log_level
 * of the macro function SYSFUN_LOGMSG()
 */
#define SYSFUN_LOGLEVEL_DEFAULT SYSFUN_LOGLEVEL_WARN
#define SYSFUN_LOGLEVEL_URGENT     0
#define SYSFUN_LOGLEVEL_ALERT      1
#define SYSFUN_LOGLEVEL_CRITICAL   2
#define SYSFUN_LOGLEVEL_ERR        3
#define SYSFUN_LOGLEVEL_WARN       4
#define SYSFUN_LOGLEVEL_NOTICE     5
#define SYSFUN_LOGLEVEL_INFO       6
#define SYSFUN_LOGLEVEL_DEBUG      7

/* MACRO FUNCTION DECLARATIONS
 */
#define DBG_PrintHex(x) printf(x)

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
// #define SYSFUN_LOGMSG(log_level, msg, ...)  printf(msg, ##__VA_ARGS__)
#define SYSFUN_LOGMSG(log_level, msg, ...)      do {} while (0)

/*-------------------------------------------------------------------------
 * MACRO NAME - SYSFUN_GET_LINUX_MSG_SIZE
 * ------------------------------------------------------------------------
 * PURPOSE  :   Get the key for creating request message queue.
 * INPUT    :   msg_size     - SYSFUN msg size.
 * OUTPUT   :   None.
 * RETURN   :   The linux key to get the request message queue.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
#define SYSFUN_GET_LINUX_MSG_SIZE(msg_size)  \
    ((int)SYSFUN_SIZE_OF_MSG(msg_size) - (int)&(((SYSFUN_Msg_T *)0)->cmd))

/*-------------------------------------------------------------------------
 * MACRO NAME - SYSFUN_GET_LINUX_KEY_FOR_REQUEST_MSGQ
 * ------------------------------------------------------------------------
 * PURPOSE  :   Get the key for creating request message queue.
 * INPUT    :   msgq_key     - SYSFUN msgq key.
 * OUTPUT   :   None.
 * RETURN   :   The linux key to get the request message queue.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
#define SYSFUN_GET_LINUX_KEY_FOR_REQUEST_MSGQ(msgq_key) ((msgq_key) << 1)

/*-------------------------------------------------------------------------
 * MACRO NAME - SYSFUN_GET_LINUX_KEY_FOR_RESPONSE_MSGQ
 * ------------------------------------------------------------------------
 * PURPOSE  :   Get the key for creating response message queue.
 * INPUT    :   msgq_key     - SYSFUN msgq key.
 * OUTPUT   :   None.
 * RETURN   :   The linux key to get the response message queue.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
#define SYSFUN_GET_LINUX_KEY_FOR_RESPONSE_MSGQ(msgq_key)    \
    (SYSFUN_GET_LINUX_KEY_FOR_REQUEST_MSGQ(msgq_key) + 1)

/*-------------------------------------------------------------------------
 * MACRO NAME - SYSFUN_GET_MSGQOWNER_INTERNAL
 * ------------------------------------------------------------------------
 * PURPOSE  :   Get the owner of the specific message queue.
 * INPUT    :   msgq_desc_p - The msgq descriptor.
 * OUTPUT   :   None.
 * RETURN   :   The task id of the owner of the spcicif queue.
 *              0 means the owner task id had been not specified or
 *                          the message queue doesn't exist.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
/* re-written by haiqiang which can reduce the count of system calls */
static inline UI32_T SYSFUN_GET_MSGQOWNER_INTERNAL(SYSFUN_MsgQDesc_T *msgq_desc_p)
{
    UI32_T owner_tid = msgq_desc_p->owner_tid;

    if (owner_tid <= 0) {
        //printf("%s valid msgq owner %ld, request_qid %ld, response_qid %ld, msgq_key %ld\n",
        //        __FUNCTION__, msgq_desc_p->owner_tid, msgq_desc_p->request_qid, msgq_desc_p->response_qid, msgq_desc_p->msgq_key);
        owner_tid = (UI32_T)msgctl(msgq_desc_p->request_qid, MSG_GETOWNER, NULL);
        if (owner_tid > 0)
            msgq_desc_p->owner_tid = owner_tid;
        else
            msgq_desc_p->owner_tid = 0;
    }

    return owner_tid;
}

/*-------------------------------------------------------------------------
 * MACRO NAME - SYSFUN_UART_SET_RAW_MODE
 *              SYSFUN_UART_SET_DEFAULT_MODE
 * ------------------------------------------------------------------------
 * PURPOSE  :   Change UART mode.
 * INPUT    :   termios_p  (struct termios)
 * OUTPUT   :   termios_p  (struct termios)
 * RETURN   :   None.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
/*
void cfmakeraw (struct termios *t)
{
      t->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
      t->c_oflag &= ~OPOST;
      t->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
      t->c_cflag &= ~(CSIZE|PARENB);
      t->c_cflag |= CS8;
      t->c_cc[VMIN] = 1;
      t->c_cc[VTIME] = 0;
}
*/
#if 0
#define SYSFUN_UART_SET_RAW_MODE(termios_p)     \
	do {										\
		cfmakeraw(termios_p);					\
		(termios_p)->c_iflag |= BRKINT;         \
		(termios_p)->c_oflag |= TTYDEF_OFLAG;	\
		(termios_p)->c_lflag |= ISIG;			\
	} while (0)
#else
#define SYSFUN_UART_SET_RAW_MODE(termios_p)     \
	do {										\
		cfmakeraw(termios_p);					\
		(termios_p)->c_oflag |= (ONLCR | OPOST);\
		(termios_p)->c_lflag |= ISIG;			\
	} while (0)
#endif

#if ((SYS_CPNT_POE == TRUE) && (SYS_CPNT_POE_INTERFACE == SYS_CPNT_POE_INTERFACE_UART))
#define SYSFUN_UART_SET_POE_RAW_MODE(termios_p) \
    do {                                        \
        cfmakeraw(termios_p);                   \
        (termios_p)->c_iflag |= BRKINT;         \
        (termios_p)->c_lflag &= ~ISIG;          \
        (termios_p)->c_oflag &= ~ONLCR;         \
    } while (0)
#endif

#define SYSFUN_UART_SET_REAL_RAW_MODE(termios_p)     \
    do {                                        \
        cfmakeraw(termios_p);					\
        (termios_p)->c_iflag &= ~BRKINT;        \
        (termios_p)->c_lflag &= ~ISIG;          \
    } while (0)

#define SYSFUN_UART_SET_DEFAULT_MODE(termios_p)         \
    do {                                                \
        (termios_p)->c_iflag |= TTYDEF_IFLAG;           \
        (termios_p)->c_oflag |= TTYDEF_OFLAG;           \
        (termios_p)->c_lflag |= TTYDEF_LFLAG;           \
    } while (0)


#define SYSFUN_SHOW_TASK_INFO() { \
    char task_name[16]; \
    UI32_T sched_policy, priority; \
    if (SYSFUN_TaskIDToName(0, task_name, sizeof(task_name)) != SYSFUN_OK) \
        task_name[0] = 0; \
    if (SYSFUN_GetTaskPriority(SYSFUN_TaskIdSelf(), &sched_policy, &priority) != SYSFUN_OK) \
        sched_policy = priority = -1; \
    printf("\r\n[pid:%lu][%s][%lu,%lu] ", SYSFUN_TaskIdSelf(), task_name, sched_policy, priority); \
}

#if (SYSFUN_DEBUG == TRUE)
    #define SYSFUN_DEBUG_LINE(fmt, ...)     \
    {                                       \
        SYSFUN_SHOW_TASK_INFO();            \
        printf("%s: ", __FUNCTION__);       \
        printf(fmt, ##__VA_ARGS__);         \
        fflush(stdout);                     \
    }
    #define SYSFUN_DEBUG_DUMP_MSG(msg_p)  DBG_DumpHex("\r\n", SYSFUN_SIZE_OF_MSG((msg_p)->msg_size), (msg_p))
#else
    #define SYSFUN_DEBUG_LINE(fmt, ...)
    #define SYSFUN_DEBUG_DUMP_MSG(msg_p)
#endif /* SYSFUN_DEBUG */

/* DATA TYPE DECLARATIONS
 */

/* union semun is defined here for using semctl()
 * POSIX.1-2001 requires that the caller define this union
 */
union semun {
    int              val;           /* Value for SETVAL */
    struct semid_ds *buf;           /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;         /* Array for GETALL, SETALL */
    struct seminfo  *__buf;         /* Buffer for IPC_INFO */
};

/* periodic timer control block
 */
typedef struct periodic_timer_cb_s
{
    struct sigevent timer_event_spec;
    timer_t         timer_id;
    pid_t           pid;
    UI32_T          event;
    VOID_FUNCTION   callback_func;
} periodic_timer_cb_t;

typedef struct SYSFUN_StartThreadArg_S
{
    void   *user_arg;
    char   thread_name[SYSFUN_TASK_NAME_LENGTH + 1];
    UI16_T sched_policy;
    int    thread_priority;
    sem_t  newThreadID_sem;
    pid_t  newThreadID;
} SYSFUN_StartThreadArg_T;

typedef struct SYSFUN_StartThreadEntry_S
{
    void (*thread_routine)(void*);
    SYSFUN_StartThreadArg_T arg;
} SYSFUN_StartThreadEntry_T;

typedef struct
{
    int  sem_id;
    void (*func_ptr)(void *);
    void *arg;
} SYSFUN_InitThreadArg_T;

struct sem {
    int semval;     /* current value */
    int sempid;     /* pid of last operation */
};

/* declarations for system call
 */
/*
  * jerry.du 2009-05-14, modify for marvell linux 2.6.22.18 platform
  * the diffrent linux kernel syscall mechanism is changed after linux-2.6.19
  * remove the macro _syscallN such as _syscall0(long, gettid)
  * if you want to use syscall gettid, you should use as below:
  * syscall(__NR_gettid)
 */
#ifndef ES4308MA_POEPD_FLF_ZZ
_syscall6(UI32_T, sys_SYSFUN, UI32_T, cmd, UI32_T, arg1, UI32_T, arg2, UI32_T, arg3, UI32_T, arg4, UI32_T, arg5);
_syscall0(long, gettid);
_syscall6(int, ipc, unsigned int, call, int, first, int, second, int, third, void *, ptr, long, fifth);
#else
/* wakka: use following definitions instead of above.
 *   dont use ES4308MA_POEPD_FLF_ZZ in sysfun.c, should remove it later
 */

/* UI32_T sys_SYSFUN(UI32_T cmd, UI32_T arg1, UI32_T arg2, UI32_T arg3, UI32_T arg4, UI32__T arg5); */
#define sys_SYSFUN(cmd, arg1, arg2, arg3, arg4, arg5) syscall(__NR_sys_SYSFUN, (cmd), (arg1), (arg2), (arg3), (arg4), (arg5))

/* pid_t gettid(void); */
#define gettid() syscall(__NR_gettid)

/* ipc is obsoleted in linux kernel 2.6.22.18 which is got from Marvell.
 * Should not use ipc in kernel 2.6.22.18.
 * charlie_chen 2009-09-21
 */
#if 0
/*  int ipc(unsigned int call, int first, int second, int third, void *ptr, long fifth); */
#define ipc(call, first, second, third, ptr, fifth) syscall(__NR_ipc, (call), (first), (second), (third), (ptr), (fifth))
#endif

/* use semtimedop provided in libc.so.6.
 * charlie_chen 2009-09-21
 */
extern int semtimedop (int __semid, struct sembuf *__sops, size_t __nsops, const struct timespec *__timeout);

#endif

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static void *SYSFUN_InternalNewThreadEntry(void* arg);
static void *SYSFUN_InternalInitThreadEntry(void* arg);
static void SYSFUN_PeriodicTimerSignalHandler(int sig_num, siginfo_t *info_p, void *ucontext);
static void SYSFUN_SegmentationFaultSignalHandler(int sig_num, siginfo_t *info_p, void *ucontext);
static void SYSFUN_TaskTerminatedHookFun(void);
static void SYSFUN_ProcessInit(char *process_name);
static void SYSFUN_TaskInit(char *task_name);
static void SYSFUN_DummySingalHandler(int sig_num);
static char SYSFUN_Hex2Ascii (unsigned hex);
static void SYSFUN_TicksToTimespec(UI32_T ticks, struct timespec *ts);
static UI32_T SYSFUN_InternalSpawnThread(UI32_T thread_priority,
                                         UI32_T sched_policy,
                                         char   *thread_name,
                                         UI32_T stack_size,
                                         UI32_T task_option,
                                         void   *func_ptr,
                                         void   *arg,
                                         BOOL_T is_joinable,
                                         UI32_T *thread_id,
                                         void   **thread_obj);

#if 0
#if (SYSFUN_DEBUG==TRUE)
static void SYSFUN_Debug_ShowInfo(void);
static BOOL_T SYSFUN_Debug_ValidateSyscall(void);
#endif

#if (SYSFUN_RD_DEBUG_ENABLE_DUMP_TASK_STACK==TRUE)
static int __backtrace(void **array, int size);
#endif
#endif

/* STATIC VARIABLE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM BODIES
 */
/* FUNCTION NAME : SYSFUN_GetMsgQOwner_Internal
 * PURPOSE:
 *
 * INPUT:
 *      SYSFUN_MsgQDesc_T.
 * OUTPUT:
 *      None.
 * RETURN:
 *      None.
 * NOTES:
 */

static inline UI32_T
SYSFUN_GetMsgQOwner_Internal(SYSFUN_MsgQDesc_T *desc)
{
    register UI32_T     owner;

    owner = (UI32_T) msgctl((int)desc->request_qid, MSG_GETOWNER, NULL);
    desc->owner_tid = (owner == (UI32_T)-1) ? 0 : owner;
    return owner;
}

#if 0
static void
SYSFUN_ShowTaskInfo(void)
{
    char        task_name[32];
    UI32_T      sched_policy, priority;
    UI32_T      thr_id = SYSFUN_TaskIdSelf();

    if (SYSFUN_TaskIDToName(0, task_name, sizeof (task_name)) != SYSFUN_OK) {
        task_name[0] = 0;
    }

    if (SYSFUN_GetTaskPriority(thr_id, &sched_policy, &priority) != SYSFUN_OK) {
        sched_policy = (UI32_T) -1;
        priority     = (UI32_T) -1;
    }

    printf("pid %d, thread-id %lu, task %s, schedule policy %lu, priority %lu\n",
            getpid(), thr_id, task_name, sched_policy, priority);
    fflush(stdout);
}
#endif

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
void    DBG_Print (char *text)
{
#ifdef SYSFUN_DEBUG
    printf ("%s", text);
#endif /* SYSFUN_DEBUG */
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
        printf (title);
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
            printf (hex_buf);
            memset(hex_buf, 0x20, 69);
        }
    }
    if (i%16)
    {
        hex_buf[66] = 0x0d;
        hex_buf[67] = 0x0a;
        hex_buf[68] = 0;
        printf (hex_buf);
    }
    fflush(stdout);
#endif /* SYSFUN_DEBUG */
}   /*  end of DBG_DumpHex  */

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
void SYSFUN_Init (void)
{
    SYSFUN_ProcessInit("");

#ifdef SYSFUN_DEBUG
    // SYSFUN_Debug_ShowInfo();
#endif
}

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
                            UI32_T *process_id)
{
    pid_t pid;

    pid = fork();
    if (pid < 0)
    {
        /* critical error, print error message directly
         */
        SYSFUN_DBG_PRINTF("fork fail: rc = %d, errno = %d\n", (int)pid, errno);
        return SYSFUN_RESULT_ERROR;
    }
    else if (pid > 0)
    {
        /* parent process */
        SYSFUN_DBG_PRINTF("Process %s (pid %d) forked\n", process_name, (int)pid);
        *process_id = (UI32_T)pid;
        return SYSFUN_OK;
    }

    /* reach here, it is the new created child process */
    if (setsid() == -1)
    {
        SYSFUN_DBG_PRINTF("setsid() faile: errno = %d", errno);
        return SYSFUN_RESULT_ERROR;
    }

    /* init process working environment
     */
    SYSFUN_ProcessInit(process_name);

    /* set priority
     */
    if (SYSFUN_SetTaskPriority(sched_policy, 0, process_priority) != SYSFUN_OK)
    {
        return SYSFUN_RESULT_ERROR;
    }

    ((void (*)(void*))(func_ptr))(arg);
    exit(0);
}

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
                           UI32_T *thread_id)
{
    return SYSFUN_InternalSpawnThread(thread_priority,
                                      sched_policy,
                                      thread_name,
                                      stack_size,
                                      task_option,
                                      func_ptr,
                                      arg,
                                      FALSE,
                                      thread_id,
                                      NULL);
}

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
                                   void **thread_obj)
{
    return SYSFUN_InternalSpawnThread(thread_priority,
                                      sched_policy,
                                      thread_name,
                                      stack_size,
                                      task_option,
                                      func_ptr,
                                      arg,
                                      TRUE,
                                      thread_id,
                                      thread_obj);
}

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
UI32_T SYSFUN_JoinThread(void **thread_obj)
{
    pthread_t *thread_p;
    void      *value_ptr;
    int       rc;

    if(thread_obj==NULL)
        return SYSFUN_RESULT_INVALID_ARG;

    thread_p = (pthread_t*)*thread_obj;

    rc = pthread_join(*thread_p, &value_ptr);

    if (rc == 0)
    {
        free(*thread_obj);
        *thread_obj=NULL;
    }

    return (rc==0)?SYSFUN_OK : SYSFUN_RESULT_ERROR;
}

/* FUNCTION NAME : SYSFUN_InternalSpawnThread
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
 *      task_option     -- Task needs supporting.
 *                         SYSFUN_TASK_NO_FP : no floating point coprocessor supporting.
 *                         SYSFUN_TASK_FP    : this task needs floating point coprocessor support.
 *      func_ptr        -- function entry of this thread.
 *                         the function protype must be -> void func(void*)
 *      arg             -- parameter passed to function entry.
 *      is_joinable     -- whether the spawned thread is joinable
 *
 * OUTPUT:
 *      thread_id       -- thread identifier
 *      thread_obj      -- the thread object will be output if a joinable thread is being spawned
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
static UI32_T SYSFUN_InternalSpawnThread(UI32_T thread_priority,
                                         UI32_T sched_policy,
                                         char   *thread_name,
                                         UI32_T stack_size,
                                         UI32_T task_option,
                                         void   *func_ptr,
                                         void   *arg,
                                         BOOL_T is_joinable,
                                         UI32_T *thread_id,
                                         void   **thread_obj)
{
    pthread_t                   thread;
    pthread_attr_t              thread_attribute;
    SYSFUN_StartThreadEntry_T   start_thread_entry;
    int                         rc;

    if ((func_ptr == NULL) || (thread_id == NULL))
        return SYSFUN_RESULT_INVALID_ARG;

    if(is_joinable==TRUE && thread_obj==NULL)
        return SYSFUN_RESULT_INVALID_ARG;

    start_thread_entry.arg.thread_priority = thread_priority;

    if ((rc = pthread_attr_init(&thread_attribute)) != 0)
    {
        SYSFUN_DBG_PRINTF("pthread_attr_init() fail, rc %d, errno %d\n", rc, errno);
        return SYSFUN_RESULT_ERROR;
    }

    {
        int pthread_state = (is_joinable==TRUE)?PTHREAD_CREATE_JOINABLE:PTHREAD_CREATE_DETACHED;

        if ((rc = pthread_attr_setdetachstate(&thread_attribute, pthread_state)) != 0)
        {
            SYSFUN_DBG_PRINTF("pthread_attr_setdetachstate() fail, rc %d, errno %d\n", rc, errno);
            pthread_attr_destroy(&thread_attribute);
            return SYSFUN_RESULT_ERROR;
        }
    }

    /* If stack size == 0 use the default value.
     */
    if (stack_size)
    {
        /* For pthread, stack size must >= PTHREAD_STACK_MIN.
         * If stack_size is less than PTHREAD_STACK_MIN, correct it to PTHREAD_STACK_MIN.
         */
        stack_size = stack_size > PTHREAD_STACK_MIN ? stack_size : PTHREAD_STACK_MIN;
        if ((rc = pthread_attr_setstacksize(&thread_attribute, stack_size))!=0)
        {
            SYSFUN_DBG_PRINTF("pthread_attr_setstacksize() fail. rc %d, errno %d\n", rc, errno);
            pthread_attr_destroy(&thread_attribute);
            return SYSFUN_RESULT_ERROR;
        }

        SYSFUN_DBG_PRINTF("pthread_attr_setstacksize() set stack size %lu\n", stack_size);
    }

    start_thread_entry.thread_routine   = (void (*)(void*))func_ptr;
    start_thread_entry.arg.user_arg     = arg;
    start_thread_entry.arg.sched_policy = sched_policy;

    strncpy(start_thread_entry.arg.thread_name, thread_name, sizeof(start_thread_entry.arg.thread_name)-1);
    start_thread_entry.arg.thread_name[sizeof(start_thread_entry.arg.thread_name) - 1] = 0;

    rc = sem_init(&(start_thread_entry.arg.newThreadID_sem), 0, 0);
    if(rc!=0)
    {
        SYSFUN_DBG_PRINTF("sem_init() fail, rc %d, errno %d\n", rc, errno);
        pthread_attr_destroy(&thread_attribute);
        return SYSFUN_RESULT_ERROR;
    }

    if (pthread_create(&thread, &thread_attribute, SYSFUN_InternalNewThreadEntry, &start_thread_entry)!=0)
    {
        SYSFUN_DBG_PRINTF("pthread_create() fail, rc %d, errno %d\n", rc, errno);
        pthread_attr_destroy(&thread_attribute);
        sem_destroy(&(start_thread_entry.arg.newThreadID_sem));
        return SYSFUN_RESULT_ERROR;
    }
    SYSFUN_DBG_PRINTF("pthread_create() ok\n");

    /* retrieve new created thread id
     */
WAIT_AGAIN:
    rc = sem_wait(&(start_thread_entry.arg.newThreadID_sem));
    if(rc!=0)
    {
        if (errno == EINTR)
            goto WAIT_AGAIN;
        SYSFUN_DBG_PRINTF("sem_wait() fail, rc %d, errno %d\n", rc, errno);
        pthread_attr_destroy(&thread_attribute);
        return SYSFUN_RESULT_ERROR;
    }

    *thread_id = (UI32_T)start_thread_entry.arg.newThreadID;

    pthread_attr_destroy(&thread_attribute);
    sem_destroy(&(start_thread_entry.arg.newThreadID_sem));

    if(is_joinable==TRUE)
    {
        *thread_obj = malloc(sizeof(thread));
        if(*thread_obj!=NULL)
        {
            memcpy(*thread_obj, &thread, sizeof(thread));
        }
        else
        {
            SYSFUN_DBG_PRINTF("Failed to malloc thread_obj\n");
            return SYSFUN_RESULT_ERROR;
        }
    }

    return SYSFUN_OK;
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
    return SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_TASKNAME_TO_ID, (UI32_T)task_name, (UI32_T)task_id, 0, 0);
}

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
UI32_T  SYSFUN_TaskIDToName (UI32_T task_id, char *task_name, UI32_T size)
{
    return SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_TASKID_TO_NAME, (UI32_T)task_id, (UI32_T)task_name, size, 0);
}

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
UI32_T  SYSFUN_DeleteProcess (UI32_T task_id)
{
    if (kill((pid_t)task_id, SIGTERM) == -1)
        return SYSFUN_RESULT_INVALID_ID;
    else
        return SYSFUN_OK;
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
UI32_T SYSFUN_TaskIdSelf(void)
{
    return gettid();
}

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
 *      priority     -- default policy: valid range is from 100 to 139
 *                      first-in-first-out policy and round-robin policy: valid range is from 1 to 99
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
UI32_T  SYSFUN_SetTaskPriority (UI32_T sched_policy, UI32_T tid, UI32_T priority)
{
    struct sched_param prio_param;
    int linux_priority;
    UI32_T ret_val;


    if (tid == 0)
        tid = SYSFUN_TaskIdSelf();

    switch (sched_policy)
    {
        case SYSFUN_SCHED_DEFAULT:
            if (!(priority >=100 && priority <= 139)) {
                printf("%s invalid priority %ld\n", __FUNCTION__, priority);
                return SYSFUN_RESULT_INVALID_ID;
            }
            linux_priority = priority - 120;
            ret_val = setpriority(PRIO_PROCESS, tid, linux_priority);
            break;
        case SYSFUN_SCHED_FIFO:
        case SYSFUN_SCHED_RR:
            if (!(priority >=1 && priority <= 99)) {
                printf("%s invalid priority %ld\n", __FUNCTION__, priority);
                return SYSFUN_RESULT_INVALID_ID;
            }
            prio_param.sched_priority = priority;
            ret_val = sched_setscheduler(tid,
                    ((sched_policy == SYSFUN_SCHED_FIFO) ? SCHED_FIFO : SYSFUN_SCHED_RR),
                    &prio_param);
            break;
        default:
            ret_val = -1;
    }

    if (ret_val == -1)
    {
        printf("Set prio error: task %lu, policy %lu, prio %lu, errno %d\n",
               tid, sched_policy, priority, errno);
        return SYSFUN_RESULT_INVALID_ID;
    }

    return SYSFUN_OK;
}   /*  end of SYSFUN_SetTaskPriority */


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
 *      priority     -- default policy: valid range is from 100 to 139
 *                      first-in-first-out policy and round-robin policy: valid range is from 1 to 99
 *
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
UI32_T  SYSFUN_GetTaskPriority (UI32_T tid, UI32_T *sched_policy_p, UI32_T *priority_p)
{
    struct sched_param  param;
    int rc, linux_priority;

    if (!sched_policy_p || !priority_p)
        return SYSFUN_RESULT_INVALID_ARG;

    rc = sched_getscheduler(tid);

    switch (rc)
    {
        case SCHED_OTHER:
            rc = getpriority(PRIO_PROCESS, tid);
            if (rc == -1)
                return SYSFUN_RESULT_ERROR;
            linux_priority = rc;
            *sched_policy_p = SYSFUN_SCHED_DEFAULT;
            *priority_p = linux_priority + 120;
            break;
        case SCHED_FIFO:
        case SCHED_RR:
            *sched_policy_p = ((rc == SCHED_FIFO) ? SYSFUN_SCHED_FIFO : SYSFUN_SCHED_RR);
            if (sched_getparam(tid, &param) == -1)
                return SYSFUN_RESULT_ERROR;
            linux_priority = param.sched_priority;
            *priority_p = linux_priority;
            break;
        default:
            return SYSFUN_RESULT_ERROR;
    }

    return SYSFUN_OK;
}   /*  end of SYSFUN_GetTaskPriority */

// TODO: unused
#if 0
/* FUNCTION NAME : SYSFUN_SuspendProcess
 * PURPOSE:
 *      Suspend a process.
 *
 * INPUT:
 *      task_id -- the task be suspended.
 *                 0 - caller itself.
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
void SYSFUN_SuspendProcess (UI32_T task_id)
{
    if (task_id == 0)
        raise(SIGSTOP);
    else
        kill((pid_t)task_id, SIGSTOP);
}

/* FUNCTION NAME : SYSFUN_ResumeProcess
 * PURPOSE:
 *      Resume a suspended process.
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
 *      None.
 */
UI32_T SYSFUN_ResumeProcess (UI32_T task_id)
{
    if (kill((pid_t)task_id, SIGCONT) == -1)
        return SYSFUN_RESULT_INVALID_ID;
    else
        return SYSFUN_OK;
}
#endif


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
void SYSFUN_SuspendThreadSelf (void)
{
    while ((BOOL_T)SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_SUSPENDTASKSELF, 0, 0, 0, 0) == FALSE);
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
    return SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_RESUMETASK, task_id, 0, 0, 0);
}

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
void SYSFUN_DelSelfThread (void)
{
    pthread_exit(NULL);
}

// TODO: unused
#if 0
/* FUNCTION NAME : SYSFUN_InitProcess
 * PURPOSE:
 *      To create a child process and suspend the process. The parent process shall
 *       start the child process later on.
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
UI32_T  SYSFUN_InitProcess(UI32_T process_priority,
                            UI32_T sched_policy,
                            char   *process_name,
                            void   *func_ptr,
                            void   *arg,
                            UI32_T *process_id)
{
    pid_t pid;

    pid = fork();
    if (pid < 0)
    {
        /* critical error, print error message directly
         */
        SYSFUN_DBG_PRINTF("fork error");
        return SYSFUN_RESULT_ERROR;
    }
    else if (pid > 0)
    {
        *process_id = (UI32_T)pid;
        return SYSFUN_OK;
    }

    if (setsid() == -1)
    {
        SYSFUN_DBG_PRINTF("setsid() error");
        return SYSFUN_RESULT_ERROR;
    }

    /* init process working environment
     */
    SYSFUN_ProcessInit(process_name);

    /* set priority
     */
    if (SYSFUN_SetTaskPriority(sched_policy, 0, process_priority)!=SYSFUN_OK)
    {
        return SYSFUN_RESULT_ERROR;
    }

    SYSFUN_SuspendProcess(0);

    ((void (*)(void*))(func_ptr))(arg);
    exit(0);
}

/* FUNCTION NAME : SYSFUN_StartProcess
 * PURPOSE:
 *      Wake up a pre-created process.
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
 *      1. The process must be created and init. before activated.
 *
 * NOTES:
 *      None.
 */
UI32_T  SYSFUN_StartProcess (UI32_T task_id)
{
    return SYSFUN_ResumeProcess(task_id);
}
#endif


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
                          UI32_T *thread_id)
{
    SYSFUN_InitThreadArg_T init_arg;
    struct sembuf sop;
    union semun semarg;
    UI32_T ret;

    semarg.val=0;
    if ((init_arg.sem_id = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT)) == -1 ||
        semctl(init_arg.sem_id, 0, SETVAL, semarg) == -1)
        return SYSFUN_RESULT_ERROR;

    init_arg.func_ptr = func_ptr;
    init_arg.arg = arg;

    ret = SYSFUN_SpawnThread(thread_priority,
                             sched_policy,
                             thread_name,
                             stack_size,
                             task_option,
                             SYSFUN_InternalInitThreadEntry,
                             &init_arg,
                             thread_id);

    sop.sem_num = 0;
    sop.sem_op = -1;
    sop.sem_flg = 0;
    while (semop(init_arg.sem_id, &sop, 1) != 0)
        if (errno != EINTR)
            break;

    /* IPC_RMID operation does not need to reference semarg
     * but it still need to pass a dummy argument or we will
     * get an exception
     */
    semctl(init_arg.sem_id, 0, IPC_RMID, semarg);

    return ret;
}

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
UI32_T  SYSFUN_StartThread (UI32_T task_id)
{
    return SYSFUN_ResumeThread(task_id);
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
 *            FALSE -  The suspended task is resumed due to time out.
 * NOTE     : This function MUST NOT BE called by an ISR.
 *-----------------------------------------------------------------------------*/
BOOL_T SYSFUN_SuspendTaskWithTimeout(UI32_T timeout)
{
    UI32_T current_tick, timeout_tick = 0;

    timeout_tick = (UI32_T)times(NULL) + timeout;

    while ((BOOL_T)SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_SUSPENDTASKWITHTIMEOUT, timeout, 0, 0, 0) == FALSE)
    {
        current_tick = (UI32_T)times(NULL);
        if (L_MATH_TimeOut32(current_tick, timeout_tick))
            return FALSE;
        timeout = timeout_tick - current_tick;
    }

    return TRUE;
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
BOOL_T SYSFUN_ResumeTaskInTimeoutSuspense(UI32_T task_id)
{
    return (BOOL_T)SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_RESUMETASKWITHTIMEOUT, task_id, 0, 0, 0);
}

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
void SYSFUN_Debug_DumpTaskStack(void)
{
    void* array[16];
    int   count,i;

    count = __backtrace(array, 16);

    printf("\n\nDump task stack\n");
    for (i = 0; i < count; i++)
    {
        printf("[%d] [0x%08x]\n", i, (int)array[i]);
    }
}
#endif /* SYSFUN_RD_DEBUG_ENABLE_DUMP_TASK_STACK */
#endif

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
    int         rc;
    union semun arg;

    if (sem_id == NULL)
        return SYSFUN_RESULT_INVALID_ARG;

    /* default permission setting -> rwxrwxrwx
     */
    if (sem_key == SYSFUN_SEMKEY_PRIVATE)
        rc = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT | IPC_EXCL);
    else
        rc = semget((key_t)sem_key, 1, 0777 | IPC_CREAT | IPC_EXCL);

    if (rc < 0)
    {
        switch (errno)
        {
            case EEXIST:
                return SYSFUN_RESULT_INVALID_ARG;
            case ENOMEM:
                return SYSFUN_RESULT_NO_SEMAPHORE;
            default:
                SYSFUN_DBG_PRINTF("semget() fail, errno %d\n", errno);
                return SYSFUN_RESULT_ERROR;
        }
    }

    *sem_id = (UI32_T) rc;

    /* init semaphore count
     */
    /* XXX do we just support binary semaphore? */
    arg.val = (sem_count > 0) ? 1 : 0;
    rc = semctl((int)*sem_id, 0, SETVAL, arg);
    if (rc < 0)
        return SYSFUN_RESULT_ERROR;

    return SYSFUN_OK;
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
    int   rc;

    if(sem_id==NULL)
        return SYSFUN_RESULT_INVALID_ARG;

    /* default permission setting -> rwxrwxrwx
     */
    rc = semget((key_t)sem_key, 1, 0777);

    if (rc < 0)
    {
        switch (errno)
        {
            case ENOENT:
                return SYSFUN_RESULT_NO_SEMAPHORE;
            default:
                SYSFUN_DBG_PRINTF("semget() fail, errno %d\n", errno);
                return SYSFUN_RESULT_ERROR;
        }
    }

    *sem_id = (UI32_T) rc;

    return SYSFUN_OK;
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
    int         rc;
    union semun semarg;

    /* IPC_RMID operation does not need to reference semarg
     * but it still need to pass a dummy argument or we will
     * get an exception
     */
    rc = semctl((int)sem_id, 0, IPC_RMID, semarg);

    return (rc < 0) ? SYSFUN_RESULT_INVALID_ID : SYSFUN_OK;
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
UI32_T  SYSFUN_TakeSem (UI32_T sem_id, int timeout)
{
    struct timespec ts;
    struct sembuf   sop;
    int             rc = 0;
    long            timeout_tick = 0;
    BOOL_T          need_check_timeout = FALSE;

    sop.sem_num = 0;
    sop.sem_op = -1;

    switch (timeout)
    {
        case SYSFUN_TIMEOUT_NOWAIT:
            sop.sem_flg = IPC_NOWAIT;
            break;
        case SYSFUN_TIMEOUT_WAIT_FOREVER:
            sop.sem_flg = 0;
            break;
        default:
            sop.sem_flg = 0;
            need_check_timeout = TRUE;
            timeout_tick = times(NULL) + timeout;
            break;
    }

TAKE_AGAIN:
    if (need_check_timeout)
    {
        /* equivalent to:
         * rc = semtimedop((int)sem_id, &sop, 1, &ts);
         */
        SYSFUN_TicksToTimespec(timeout, &ts);
#ifndef ES4308MA_POEPD_FLF_ZZ
        rc = ipc(4, (int)sem_id, 1, 0, &sop, (long)&ts);
#else
        rc = semtimedop((int)sem_id, &sop, 1, &ts);
#endif
    }
    else
    {
        rc = semop((int)sem_id, &sop, 1);
    }

    if (rc == -1)
    {
        switch (errno)
        {
            case EAGAIN:
                if (!need_check_timeout)
                    return SYSFUN_RESULT_SEM_NOSIGNAL;
                else
                    return SYSFUN_RESULT_TIMEOUT;
            case E2BIG:
            case EFBIG:
            case EFAULT:
                return SYSFUN_RESULT_INVALID_ARG;
            case EIDRM:
                return SYSFUN_RESULT_OBJ_DELETED;
            case EINTR:
                if (need_check_timeout && (timeout = timeout_tick - times(NULL)) <= 0)
                    return SYSFUN_RESULT_TIMEOUT;
                goto TAKE_AGAIN;
            case EINVAL:
                return SYSFUN_RESULT_INVALID_ID;
            default:
                SYSFUN_DBG_PRINTF("error with unknow reason, errno %d\n", errno);
                return SYSFUN_RESULT_ERROR;
        }
    }

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
UI32_T  SYSFUN_GiveSem(UI32_T sem_id)
{
    union semun arg;
    int    rc;

    arg.val=1;
    rc = semctl((int)sem_id, 0, SETVAL, arg);

    return (rc == 0) ? SYSFUN_OK : SYSFUN_RESULT_ERROR;
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
    int     ret;

    if ((key == 0) || (size == 0))
        return SYSFUN_RESULT_INVALID_ARG;

    if ((ret = shmget((key_t)key, (size_t)size, IPC_CREAT | 0777)) == -1)
        return SYSFUN_RESULT_CANT_CREATE_SHMEM;

    *shm_id_p = (UI32_T)ret;
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
    void *shmaddr;

    shmaddr = shmat((int)shm_id, NULL, 0);

    return (shmaddr == (void *)-1 ? NULL : shmaddr);
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
    shmdt(attached_addr);
}

/*-------------------------
 * Message Queue Function
 *-------------------------
 */
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
    SYSFUN_MsgQDesc_T * msgq_desc_p;
    key_t               key;

    if (msgq_handle_p == NULL)
        return SYSFUN_RESULT_CANT_CREATE_MSG_Q;

    if ((msgq_desc_p = malloc(sizeof(SYSFUN_MsgQDesc_T))) == NULL)
        return SYSFUN_RESULT_CANT_CREATE_MSG_Q;

    /* Create/get request queue.
     * NOTE: default permission setting -> rwxrwxrwx
     */
    key = (key_t)(msgq_key == SYSFUN_MSGQKEY_PRIVATE ?
                  IPC_PRIVATE :
                  SYSFUN_GET_LINUX_KEY_FOR_REQUEST_MSGQ(msgq_key));

    if ((msgq_desc_p->request_qid = msgget(key, IPC_CREAT | 0777)) == -1)
    {
        SYSFUN_DBG_PRINTF("Create REQ msgq fail, key %d, errno %d\n", (int) key, errno);
        free(msgq_desc_p);
        return SYSFUN_RESULT_CANT_CREATE_MSG_Q;
    }

    /* Destroy response queue if msgq_type = SYSFUN_MSGQ_UNIDIRECTIONAL.
     * Create/get response queue if msgq_type = SYSFUN_MSGQ_BIDIRECTIONAL.
     */
    key = (key_t)(msgq_key == SYSFUN_MSGQKEY_PRIVATE ?
                  IPC_PRIVATE :
                  SYSFUN_GET_LINUX_KEY_FOR_RESPONSE_MSGQ(msgq_key));

    msgq_desc_p->response_qid = SYSFUN_MSGQ_INVALID_ID;

    if (msgq_type == SYSFUN_MSGQ_BIDIRECTIONAL)
    {
        SYSFUN_Msg_T tmp_msgbuf;

        if ((msgq_desc_p->response_qid = msgget(key, IPC_CREAT | 0777)) == -1)
        {
            SYSFUN_DBG_PRINTF("Create RESP msgq fail, key %d, errno %d\n", (int) key, errno);
            msgctl(msgq_desc_p->request_qid, IPC_RMID, NULL);
            free(msgq_desc_p);
            return SYSFUN_RESULT_CANT_CREATE_MSG_Q;
        }

        /* Flush response queue.
         */
        while (msgrcv((int)msgq_desc_p->response_qid, (struct msgbuf *)(void *)&tmp_msgbuf, 0, 0, IPC_NOWAIT | MSG_NOERROR) != -1);
    }

    /* Set owner task id.
     */
    msgq_desc_p->owner_tid = msgctl(msgq_desc_p->request_qid, MSG_SETOWNER, NULL);
    msgq_desc_p->msgq_key = msgq_key;

    *msgq_handle_p = (UI32_T)msgq_desc_p;

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
    SYSFUN_MsgQDesc_T * msgq_desc_p;
    key_t               key;

    if (msgq_key == SYSFUN_MSGQKEY_PRIVATE)
        return SYSFUN_RESULT_CANT_CREATE_MSG_Q;

    if (msgq_handle_p == NULL)
        return SYSFUN_RESULT_CANT_CREATE_MSG_Q;

    if ((msgq_desc_p = malloc(sizeof(SYSFUN_MsgQDesc_T))) == NULL)
        return SYSFUN_RESULT_CANT_CREATE_MSG_Q;

    /* Create/get request queue.
     * NOTE: default permission setting -> rwxrwxrwx
     */
    key = (key_t)SYSFUN_GET_LINUX_KEY_FOR_REQUEST_MSGQ(msgq_key);
    if ((msgq_desc_p->request_qid = msgget(key, IPC_CREAT | 0777)) == -1)
    {
        SYSFUN_DBG_PRINTF("Get REQ msgq fail, key %d, errno %d\n", (int) key, errno);
        free(msgq_desc_p);
        return SYSFUN_RESULT_CANT_CREATE_MSG_Q;
    }

    /* Create/Get response queue if msgq_type=SYSFUN_MSGQ_BIDIRECTIONAL.
     */
    key = (key_t)SYSFUN_GET_LINUX_KEY_FOR_RESPONSE_MSGQ(msgq_key);

    if (msgq_type == SYSFUN_MSGQ_UNIDIRECTIONAL)
    {
        msgq_desc_p->response_qid = SYSFUN_MSGQ_INVALID_ID;
    }
    else /* SYSFUN_MSGQ_BIDIRECTIONAL */
    {
        if ((msgq_desc_p->response_qid = msgget(key, IPC_CREAT | 0777)) == -1)
        {
            SYSFUN_DBG_PRINTF("Get RESP msgq fail, key %d, errno %d\n", (int) key, errno);
            free(msgq_desc_p);
            return SYSFUN_RESULT_CANT_CREATE_MSG_Q;
        }
    }

    /* Get owner task id.
     */
    msgq_desc_p->owner_tid = msgctl(msgq_desc_p->request_qid, MSG_GETOWNER, NULL);
    msgq_desc_p->msgq_key = msgq_key;

    *msgq_handle_p = (UI32_T)msgq_desc_p;

    return SYSFUN_OK;
}

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
UI32_T SYSFUN_ReleaseMsgQ(UI32_T msgq_handle)
{
    SYSFUN_MsgQDesc_T *msgq_desc_p = (SYSFUN_MsgQDesc_T *)msgq_handle;

    if (msgq_desc_p == NULL)
        return SYSFUN_RESULT_INVALID_ID;

    free(msgq_desc_p);
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
    SYSFUN_MsgQDesc_T *msgq_desc_p = (SYSFUN_MsgQDesc_T *)msgq_handle;
    int ret;

    if (msgq_desc_p == NULL)
        return SYSFUN_RESULT_INVALID_ID;

    ret = msgctl(msgq_desc_p->request_qid, IPC_RMID, NULL);
    if ((msgq_desc_p->response_qid != SYSFUN_MSGQ_INVALID_ID &&
        msgctl(msgq_desc_p->response_qid, IPC_RMID, NULL) == -1) ||
        ret == -1)
        goto ERROR;

    free(msgq_desc_p);
    return SYSFUN_OK;

ERROR:
    free(msgq_desc_p);
    switch (errno)
    {
        case EIDRM:
            return SYSFUN_RESULT_OBJ_DELETED;
        case EINVAL:
            return SYSFUN_RESULT_INVALID_ID;
    }

    return SYSFUN_RESULT_ERROR;
}

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
UI32_T SYSFUN_GetMsgQOwner(UI32_T msgq_handle)
{
    return SYSFUN_GET_MSGQOWNER_INTERNAL((SYSFUN_MsgQDesc_T *)msgq_handle);
}

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
UI32_T SYSFUN_CreateRwSem (UI32_T sem_key, UI32_T *sem_id)
{
    int         rc;
    union semun arg;
    int         exist_or_not = FALSE;

    if (sem_id == NULL)
        return SYSFUN_RESULT_INVALID_ARG;

    /* default permission setting -> rwxrwxrwx
     */
    if (sem_key == SYSFUN_SEMKEY_PRIVATE)
        rc = semget(IPC_PRIVATE, 3, 0777 | IPC_CREAT | IPC_EXCL);
    else
        rc = semget((key_t)sem_key, 3, 0777 | IPC_CREAT | IPC_EXCL);

    if (rc < 0)
    {
        switch (errno)
        {
            case EEXIST:
                exist_or_not = TRUE;
                break;
            case ENOMEM:
                printf("SYSFUN_CreateRwSem Error nomem!\r\n");
                return SYSFUN_RESULT_NO_SEMAPHORE;
            default:
                printf("SYSFUN_CreateRwSem semget() fail, errno %d\n", errno);
                return SYSFUN_RESULT_ERROR;
        }
    }
    if ( exist_or_not )
    {
        rc = semget((key_t)sem_key, 3, 0777);
        if (rc < 0)
        {
            printf("SYSFUN_CreateRwSem Get rwsem error.\r\n");
            return SYSFUN_RESULT_ERROR;
        }
        *sem_id = (UI32_T) rc;
        return SYSFUN_OK;
    }

    *sem_id = (UI32_T) rc;

    /* init read semaphore count */
    arg.val = 0;
    rc = semctl((int)*sem_id, 0, SETVAL, arg);
    if (rc < 0)
    {
        printf("SYSFUN_CreateRwSem Semctl Error1.\r\n");
        return SYSFUN_RESULT_ERROR;
    }
    /* init write semaphore count */
    arg.val = 1;
    rc = semctl((int)*sem_id, 1, SETVAL, arg);
    if (rc < 0)
    {
        printf("SYSFUN_CreateRwSem Semctl Error2.\r\n");
        return SYSFUN_RESULT_ERROR;
    }
    /* init write waiting semaphore count */
    arg.val = 0;
    rc = semctl((int)*sem_id, 2, SETVAL, arg);
    if (rc < 0)
    {
        printf("SYSFUN_CreateRwSem Semctl Error3.\r\n");
        return SYSFUN_RESULT_ERROR;
    }

    return SYSFUN_OK;
}

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
UI32_T SYSFUN_GetRwSem (UI32_T sem_key, UI32_T *sem_id)
{
    int   rc;

    if(sem_id==NULL)
        return SYSFUN_RESULT_INVALID_ARG;

    /* default permission setting -> rwxrwxrwx
     */
    rc = semget((key_t)sem_key, 3, 0777);

    if (rc < 0)
    {
        switch (errno)
        {
            case ENOENT:
                printf("SYSFUN_GetRwSem Error noentry!\r\n");
                return SYSFUN_RESULT_NO_SEMAPHORE;
            default:
                SYSFUN_DBG_PRINTF("SYSFUN_GetRwSem semget() fail, errno %d\n", errno);
                return SYSFUN_RESULT_ERROR;
        }
    }

    *sem_id = (UI32_T) rc;

    return SYSFUN_OK;
}


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
UI32_T  SYSFUN_TakeWriteSem (UI32_T sem_id)
{
    struct sembuf   sop[3];
    int             rc = 0;

    /* read sem */
    sop[0].sem_num = 0;
    sop[0].sem_op = 0;
    sop[0].sem_flg = 0;
    /* write sem */
    sop[1].sem_num = 1;
    sop[1].sem_op = -1;
    sop[1].sem_flg = 0;
    /* write waiting sem */
    sop[2].sem_num = 2;
    sop[2].sem_op = 1;
    sop[2].sem_flg = 0;

TAKE_AGAIN1:
    /* Up this sem first, so, other read request will be blocked */
    if (semop((int)sem_id, &sop[2], 1) == -1)
    {
        if( EINTR != errno )
            printf("SYSFUN_TakeWriteSem take write waiting error: %d\r\n", errno);
        switch (errno)
        {
            case EAGAIN:
                return SYSFUN_RESULT_TIMEOUT;
            case E2BIG:
            case EFBIG:
            case EFAULT:
                return SYSFUN_RESULT_INVALID_ARG;
            case EIDRM:
                return SYSFUN_RESULT_OBJ_DELETED;
            case EINTR:
                goto TAKE_AGAIN1;
            case EINVAL:
                return SYSFUN_RESULT_INVALID_ID;
            default:
                printf("SYSFUN_TakeWriteSem error with unknow reason, errno %d\n", errno);
                return SYSFUN_RESULT_ERROR;
        }
    }
TAKE_AGAIN2:
    if (semop((int)sem_id, &sop[0], 2) == -1)
    {
        if( EINTR != errno )
            printf("SYSFUN_TakeWriteSem take write error: %d\r\n", errno);
        switch (errno)
        {
            case EAGAIN:
                return SYSFUN_RESULT_TIMEOUT;
            case E2BIG:
            case EFBIG:
            case EFAULT:
                return SYSFUN_RESULT_INVALID_ARG;
            case EIDRM:
                return SYSFUN_RESULT_OBJ_DELETED;
            case EINTR:
                goto TAKE_AGAIN2;
            case EINVAL:
                return SYSFUN_RESULT_INVALID_ID;
            default:
                printf("SYSFUN_TakeWriteSem error with unknow reason, errno %d\n", errno);
                return SYSFUN_RESULT_ERROR;
        }
    }

    return (rc == 0) ? SYSFUN_OK : SYSFUN_RESULT_ERROR;
}


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
UI32_T  SYSFUN_TakeReadSem (UI32_T sem_id)
{
    struct sembuf   sop[3];
    int             rc = 0;

    /* read sem */
    sop[0].sem_num = 0;
    sop[0].sem_op = 1;
    sop[0].sem_flg = 0;
    /* write waiting sem */
    sop[2].sem_num = 2;
    sop[2].sem_op = 0;
    sop[2].sem_flg = 0;

TAKE_AGAIN1:
    /* wait until No  processes are waiting to write or writing
          In other words, until this signal turn to zero     */
    if (semop((int)sem_id, &sop[2], 1) == -1)
    {
        if( EINTR != errno )
            printf("SYSFUN_TakeReadSem take write waiting error: %d\r\n", errno);
        switch (errno)
        {
            case EAGAIN:
                return SYSFUN_RESULT_TIMEOUT;
            case E2BIG:
            case EFBIG:
            case EFAULT:
                return SYSFUN_RESULT_INVALID_ARG;
            case EIDRM:
                return SYSFUN_RESULT_OBJ_DELETED;
            case EINTR:
                goto TAKE_AGAIN1;
            case EINVAL:
                return SYSFUN_RESULT_INVALID_ID;
            default:
                printf("SYSFUN_TakeReadSem error with unknow reason, errno %d\n", errno);
                return SYSFUN_RESULT_ERROR;
        }
    }
TAKE_AGAIN2:
    /* up the read semaphore */
    if (semop((int)sem_id, &sop[0], 1) == -1)
    {
        if( EINTR != errno )
            printf("SYSFUN_TakeReadSem take writing error: %d\r\n", errno);
        switch (errno)
        {
            case EAGAIN:
                return SYSFUN_RESULT_TIMEOUT;
            case E2BIG:
            case EFBIG:
            case EFAULT:
                return SYSFUN_RESULT_INVALID_ARG;
            case EIDRM:
                return SYSFUN_RESULT_OBJ_DELETED;
            case EINTR:
                goto TAKE_AGAIN2;
            case EINVAL:
                return SYSFUN_RESULT_INVALID_ID;
            default:
                printf("SYSFUN_TakeReadSem error with unknow reason, errno %d\n", errno);
                return SYSFUN_RESULT_ERROR;
        }
    }

    return (rc == 0) ? SYSFUN_OK : SYSFUN_RESULT_ERROR;
}

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
UI32_T  SYSFUN_GiveWriteSem(UI32_T sem_id)
{
    struct sembuf   sop[3];
    int             rc = 0;

    /* read sem */
    sop[0].sem_num = 0;
    sop[0].sem_op = 0;
    sop[0].sem_flg = 0;
    /* write sem */
    sop[1].sem_num = 1;
    sop[1].sem_op = 1;
    sop[1].sem_flg = 0;
    /* write waiting sem */
    sop[2].sem_num = 2;
    sop[2].sem_op = -1;
    sop[2].sem_flg = IPC_NOWAIT;

    /* Give write sem first */
    if (semop((int)sem_id, &sop[1], 1) == -1)
    {
        printf("SYSFUN_GiveWriteSem give write error++: %d\r\n", errno);
        switch (errno)
        {
            case EAGAIN:
                return SYSFUN_RESULT_TIMEOUT;
            case E2BIG:
            case EFBIG:
            case EFAULT:
                return SYSFUN_RESULT_INVALID_ARG;
            case EIDRM:
                return SYSFUN_RESULT_OBJ_DELETED;
            case EINTR:
                return SYSFUN_RESULT_TIMEOUT;
            case EINVAL:
                return SYSFUN_RESULT_INVALID_ID;
            default:
                printf("SYSFUN_GiveWriteSem error with unknow reason, errno %d\n", errno);
                return SYSFUN_RESULT_ERROR;
        }
    }
    /* Give write waiting sem then */
    if (semop((int)sem_id, &sop[2], 1) == -1)
    {
        printf("SYSFUN_GiveWriteSem give write error--: %d\r\n", errno);
        switch (errno)
        {
            case EAGAIN:
                return SYSFUN_RESULT_TIMEOUT;
            case E2BIG:
            case EFBIG:
            case EFAULT:
                return SYSFUN_RESULT_INVALID_ARG;
            case EIDRM:
                return SYSFUN_RESULT_OBJ_DELETED;
            case EINTR:
                return SYSFUN_RESULT_TIMEOUT;
            case EINVAL:
                return SYSFUN_RESULT_INVALID_ID;
            default:
                printf("SYSFUN_GiveWriteSem error with unknow reason, errno %d\n", errno);
                return SYSFUN_RESULT_ERROR;
        }
    }
    return (rc == 0) ? SYSFUN_OK : SYSFUN_RESULT_ERROR;
}


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
UI32_T  SYSFUN_GiveReadSem(UI32_T sem_id)
{
    struct sembuf   sop[3];
    int             rc = 0;

    /* read sem */
    sop[0].sem_num = 0;
    sop[0].sem_op = -1;
    sop[0].sem_flg = IPC_NOWAIT;
    /* write sem */
    sop[1].sem_num = 1;
    sop[1].sem_op = 1;
    sop[1].sem_flg = 0;
    /* write waiting sem */
    sop[2].sem_num = 2;
    sop[2].sem_op = -1;
    sop[2].sem_flg = 0;

    if (semop((int)sem_id, &sop[0], 1) == -1)
    {
        printf("SYSFUN_GiveReadSem give read error: %d\r\n", errno);
        switch (errno)
        {
            case EAGAIN:
                return SYSFUN_RESULT_TIMEOUT;
            case E2BIG:
            case EFBIG:
            case EFAULT:
                return SYSFUN_RESULT_INVALID_ARG;
            case EIDRM:
                return SYSFUN_RESULT_OBJ_DELETED;
            case EINTR:
                return SYSFUN_RESULT_TIMEOUT;
            case EINVAL:
                return SYSFUN_RESULT_INVALID_ID;
            default:
                printf("SYSFUN_GiveReadSem error with unknow reason, errno %d\n", errno);
                return SYSFUN_RESULT_ERROR;
        }
    }

    return (rc == 0) ? SYSFUN_OK : SYSFUN_RESULT_ERROR;
}

void SYSFUN_DebugRwSem(UI32_T rw_sem_id)
{
    int ret;
    unsigned short int sig_val[3];
    union semun arg;
    arg.array = &sig_val[0];
    ret = semctl(rw_sem_id, 0, GETVAL, arg);
    if (ret < 0)
        printf("Semctl Error GETALL1.\r\n");
    else
        printf("Task Num:%d which are reading.\r\n", ret);
    ret = semctl(rw_sem_id, 2, GETVAL, arg);
    if (ret < 0)
        printf("Semctl Error GETALL2.\r\n");
    else
        printf("Task Num:%d which are waiting to write.\r\n", ret);

    ret = semctl(rw_sem_id, 0, GETPID, arg);
    if (ret < 0)
        printf("Semctl Error GETPID.\r\n");
    else
        printf("Task %d which called semop last time.\r\n", ret);
    return;
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
static UI32_T   sysfun_msgq_default_timeout = 200;  /* 2 seconds */
#if 0
static UI32_T   sysfun_msgq_debug           = SYSFUN_MSGQ_DEBUG_SEND_REQ_MSG;
#else
static UI32_T   sysfun_msgq_debug           = SYSFUN_MSGQ_DEBUG_SEND_REQ;
#endif

char *
SYSFUN_GetTaskName(UI32_T taskid, char *buf, size_t size)
{
    if (buf && size) {
        if (taskid == 0) {
            taskid = SYSFUN_TaskIdSelf();
        }
        buf[0] = 0;
        SYSFUN_TaskIDToName(taskid, buf, (size - 1));
        buf[size - 1] = 0;
    }
    return buf;
}

void
SYSFUN_DBG_MsgQDebug(UI32_T flag, int on)
{
    if (on) {
        sysfun_msgq_debug |= flag;
    } else {
        sysfun_msgq_debug &= ~flag;
    }
}

#define SYSFUN_CURRENT_CLOCK        SYSFUN_GetSysTick()

#define SYSFUN_MSG_TIMEOUT_SLEEP(timeout, lefttime)                         \
    do {                                                                    \
        UI32_T  sleeping = 1;                                               \
        if (timeout == SYSFUN_TIMEOUT_WAIT_FOREVER) {                       \
            sleeping = SYSFUN_TIMEOUT_CHECK_INTERVAL_IN_CLOCKTICK;          \
        } else if (lefttime) {                                              \
            SYSFUN_ASSERT(timeout && (msgflag & IPC_NOWAIT));               \
            if (lefttime >= SYSFUN_TIMEOUT_CHECK_INTERVAL_IN_CLOCKTICK) {   \
                sleeping  = SYSFUN_TIMEOUT_CHECK_INTERVAL_IN_CLOCKTICK;     \
                lefttime -= SYSFUN_TIMEOUT_CHECK_INTERVAL_IN_CLOCKTICK;     \
            } else {                                                        \
                sleeping  = lefttime;                                       \
                lefttime  = 0;                                              \
            }                                                               \
        }                                                                   \
        SYSFUN_Sleep(sleeping);                                             \
    } while (0)

static UI32_T
SYSFUN_MsgRecv(int msgqid, void *msgp, size_t msgsz, long msgtype, UI32_T timeout)
{
    UI32_T      deadline, lefttime, curr_time;
    int         msgflag;
    int         ret;
    int local_errno; /* Yongxin.Zhao added, temp solution, need debug detail, 2009-06-25, 16:45:13 */

    deadline = SYSFUN_CURRENT_CLOCK;
    if (timeout && (timeout != SYSFUN_TIMEOUT_WAIT_FOREVER)) {
        deadline += timeout;
        lefttime  = timeout;
    } else {    /* SYSFUN_TIMEOUT_NOWAIT or SYSFUN_TIMEOUT_WAIT_FOREVER */
        lefttime  = 0;
    }

    msgflag = (timeout == SYSFUN_TIMEOUT_WAIT_FOREVER) ? 0 : IPC_NOWAIT;

RETRY_RECV:
    ret = msgrcv(msgqid, (struct msgbuf *)msgp, msgsz, msgtype, msgflag);
    local_errno = errno;

    if (-1 == ret) {
        curr_time = SYSFUN_CURRENT_CLOCK;
        switch (local_errno) {
            case ENOMSG:
                /* message queue does not have a message of type msgtype, and IPC_NOWAIT is set.
                 * this is the most common errno we meet when receiving message failed with
                 * non-blocking mode
                 */
                if (timeout == SYSFUN_TIMEOUT_NOWAIT) {
                    return SYSFUN_RESULT_NO_MESSAGE;
                } else if (timeout == SYSFUN_TIMEOUT_WAIT_FOREVER) {
                    goto RETRY_RECV;    /* this should not happen */
                } else if (L_MATH_TimeOut32(curr_time, deadline) || !lefttime) {
                    return SYSFUN_RESULT_TIMEOUT;
                } else {
                    goto DO_RECV_TIMEOUT;
                }
                break;
            case EINTR: /* syscall interrupted by something, try again */
                if (timeout == SYSFUN_TIMEOUT_NOWAIT) {
                    SYSFUN_DBG_PRINTF("msgrcv() interrupted by signal\n");
                    return SYSFUN_RESULT_EINTR;
                } else {
                    goto RETRY_RECV;
                }
                break;
            case E2BIG:
                /* mtext is greater than msgsz and MSG_NOERROR is not specified */
                if (0 == msgrcv(msgqid, (struct msgbuf *)msgp, msgsz, msgtype,
                                (IPC_NOWAIT | MSG_NOERROR))) {
                    SYSFUN_Msg_T *  msg = (SYSFUN_Msg_T *) msgp;
                    printf("msg too big, msgqid %d, msg (type %lu, cmd %d, size %d)\n",
                                      msgqid, msg->msg_type, msg->cmd, msg->msg_size);
                }
                return SYSFUN_RESULT_BUF_TOO_SMALL;
            case EACCES:
                SYSFUN_DBG_PRINTF("permission denied, pid %d, thr_id %lu, msgqid %d\n",
                                  getpid(), SYSFUN_TaskIdSelf(), msgqid);
                return SYSFUN_RESULT_ACCESS_VIOLATION;
            case EFAULT:
                SYSFUN_DBG_PRINTF("msgp references is a bad addr %lu\n", (UI32_T)msgp);
                return SYSFUN_RESULT_BUF_UNACCESSIBLE;
            case EINVAL:
                SYSFUN_DBG_PRINTF("invalid arguments, msgqid %d, msgsize %lu\n", msgqid, msgsz);
                return SYSFUN_RESULT_INVALID_ID;
            case EIDRM: /* message queue id has been removed */
                SYSFUN_DBG_PRINTF("msgqid %d has been removed\n", msgqid);
                return SYSFUN_RESULT_OBJ_DELETED;
            default:
                SYSFUN_DBG_PRINTF("unknow errno %d\n", local_errno);
                return SYSFUN_RESULT_ERROR;
        }
    }

    return SYSFUN_OK;

DO_RECV_TIMEOUT:
    SYSFUN_MSG_TIMEOUT_SLEEP(timeout, lefttime);
    goto RETRY_RECV;
}

#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
# define SYSFUN_DBG_SHOW_MSGQ_FULL(msgqid)                                      \
    do {                                                                        \
        struct msqid_ds     ds;                                                 \
        memset(&ds, 0, sizeof (ds));                                            \
        msgctl(msgqid, IPC_STAT, &ds);                                          \
        SYSFUN_DBG_PRINTF("msgq %d full, %lu msgs and %lu/%lu bytes in Q\n",    \
                           msgqid, ds.msg_qnum, ds.msg_cbytes, ds.msg_qbytes);  \
    } while (0)
#else
# define SYSFUN_DBG_SHOW_MSGQ_FULL(msgqid)      do {} while (0)
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */

#if (SYS_CPNT_MSG_FLOW_CTL == TRUE)
#define MSG_FLOW_CTL_SIG        0xa5a5a5a5
#define MSG_FLOW_CTL_SLEEP_BASE 25
#define MSG_FLOW_CTL_SLEEP_STEP 1
#endif

static UI32_T
SYSFUN_MsgSend(int msgqid, const void *msgp, size_t msgsz, UI32_T timeout)
{
    UI32_T      deadline, lefttime, curr_time;
    int         msgflag;
    int         ret;
    UI32_T      msg_type;
#if (SYS_CPNT_MSG_FLOW_CTL == TRUE)
    static int  sleep_cnt = 0;
#endif

    deadline = SYSFUN_CURRENT_CLOCK;
    if (timeout && (timeout != SYSFUN_TIMEOUT_WAIT_FOREVER)) {
        deadline += timeout;
        lefttime  = timeout;
    } else {    /* SYSFUN_TIMEOUT_NOWAIT or SYSFUN_TIMEOUT_WAIT_FOREVER */
        lefttime  = 0;
    }

    msgflag = (timeout == SYSFUN_TIMEOUT_WAIT_FOREVER) ? 0 : IPC_NOWAIT;

    msg_type = ((SYSFUN_Msg_T *)msgp)->msg_type;

RETRY_SEND:
    ret = msgsnd(msgqid, msgp, msgsz, msgflag);

    /* send message failed */
    if (-1 == ret) {
        curr_time = SYSFUN_CURRENT_CLOCK;
        switch (errno) {
            case EAGAIN:
                /* msg queue is full, this is the most common errno we can meet while sending
                 * messages fail with non-blocking mode.
                 */
                if (timeout == SYSFUN_TIMEOUT_NOWAIT) {
                    SYSFUN_DBG_SHOW_MSGQ_FULL(msgqid);
                    return SYSFUN_RESULT_MSG_Q_FULL;
                } else if (timeout == SYSFUN_TIMEOUT_WAIT_FOREVER) {
                    /* it should not happen if we use blocking mode to send message */
                    goto RETRY_SEND;
                } else if (L_MATH_TimeOut32(curr_time, deadline) || !lefttime) {
                    SYSFUN_DBG_SHOW_MSGQ_FULL(msgqid);
                    return SYSFUN_RESULT_TIMEOUT;
                } else {
                    goto DO_SEND_TIMEOUT;
                }
                break;
            case EINTR: /* syscall interrupted by something, try again */
                if (timeout == SYSFUN_TIMEOUT_NOWAIT) {
                    SYSFUN_DBG_PRINTF("msgsnd() interrupted by signal\n");
                    return SYSFUN_RESULT_EINTR;
                } else {
                    goto RETRY_SEND;
                }
                break;
            case EACCES:
                SYSFUN_DBG_PRINTF("permission denied, pid %d, thr_id %lu, msgQ %d\n",
                                  getpid(), SYSFUN_TaskIdSelf(), msgqid);
                return SYSFUN_RESULT_ACCESS_VIOLATION;
            case ENOMEM:
                SYSFUN_DBG_PRINTF("insufficient system memory to copy message\n");
                return SYSFUN_RESULT_NO_MEMORY;
            case EFAULT:
                SYSFUN_DBG_PRINTF("msg ptr references is a bad addr %lu\n", (UI32_T) msgp);
                return SYSFUN_RESULT_BUF_UNACCESSIBLE;
            case EINVAL:
                SYSFUN_DBG_PRINTF("invalid arguments, msgQ %d, msgtype %ld, msgsize %lu\n",
                                  msgqid, ((struct msgbuf *)msgp)->mtype, msgsz);
                return SYSFUN_RESULT_INVALID_ID;
            case EIDRM: /* message queue id has been removed */
                SYSFUN_DBG_PRINTF("msgQ id %d has been removed\n", msgqid);
                return SYSFUN_RESULT_OBJ_DELETED;
            default:
                SYSFUN_DBG_PRINTF("unknow errno %d\n", errno);
                break;
        }
    }
#if (SYS_CPNT_MSG_FLOW_CTL == TRUE)
    if (*(long *)msgp == MSG_FLOW_CTL_SIG) {
        int sleep_ticks;

        sleep_cnt += MSG_FLOW_CTL_SLEEP_STEP;
        sleep_ticks = MSG_FLOW_CTL_SLEEP_BASE * sleep_cnt;
        printf("msq queue msgqid %d will full, sleep %d ticks\r\n", msgqid, sleep_ticks);
        SYSFUN_Sleep(sleep_ticks);
    }
    else
        sleep_cnt = 0;
#endif
    ((SYSFUN_Msg_T *)msgp)->msg_type = msg_type;
    return SYSFUN_OK;

DO_SEND_TIMEOUT:
    SYSFUN_MSG_TIMEOUT_SLEEP(timeout, lefttime);
    goto RETRY_SEND;
}

#if 1 ////////////////////////////////////////////////////
UI32_T
__SYSFUN_SendRequestMsg(UI32_T          msgq_handle,
                        SYSFUN_Msg_T *  req_msg_p,
                        UI32_T          wait_time,
                        UI32_T          event,
                        UI32_T          res_msg_size,
                        SYSFUN_Msg_T *  res_msg_p
#ifdef SYSFUN_DEBUG
                        ,
                        const char *    func,
                        unsigned        line
#endif /* SYSFUN_DEBUG */
                        )
{
    SYSFUN_MsgQDesc_T * msgq = (SYSFUN_MsgQDesc_T *) msgq_handle;
    UI32_T              owner;
    size_t              msgsz;
    UI32_T              timeout;
    int                 uni_dir;
    UI32_T              rc;
#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
    UI32_T              start_time;
    int                 retry;
    char                taskname[64] = { 0 };

    SYSFUN_GetTaskName(0, taskname, sizeof (taskname));
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */

#define DEFAULT_MSG_TYPE 0X0FFFFFFF
    if (msgq == NULL) {
        return SYSFUN_RESULT_INVALID_ID;
    }

    uni_dir = (msgq->response_qid == SYSFUN_MSGQ_INVALID_ID);

    if (!req_msg_p) {
        return SYSFUN_RESULT_INVALID_ARG;
    }

    if (uni_dir && (req_msg_p->msg_type == 0))
        req_msg_p->msg_type = DEFAULT_MSG_TYPE;

    /* If msgq is bidirectional, msg_type means the id of the task that send request. */
    if (!uni_dir) {
        req_msg_p->msg_type = SYSFUN_TaskIdSelf();
    }

    /* send request message first */
    msgsz = (size_t) SYSFUN_GET_LINUX_MSG_SIZE(req_msg_p->msg_size);

    switch (wait_time) {
        case SYSFUN_TIMEOUT_NOWAIT:
            timeout = SYSFUN_TIMEOUT_NOWAIT;
            break;
        case SYSFUN_TIMEOUT_WAIT_FOREVER:
            timeout = SYSFUN_TIMEOUT_WAIT_FOREVER;
#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
            if (sysfun_msgq_debug & SYSFUN_MSGQ_DEBUG_SEND_REQ) {
                timeout = sysfun_msgq_default_timeout;
            }
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */
            break;
        default:
            timeout = wait_time;
            break;
    }

#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
RESEND_MSG:
    start_time = SYSFUN_CURRENT_CLOCK;
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */

    rc = SYSFUN_MsgSend((int)msgq->request_qid, req_msg_p, msgsz, timeout);

    if (rc != SYSFUN_OK) {
#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
        if (rc == SYSFUN_RESULT_TIMEOUT) {
            __SYSFUN_DBG_Printf(func, line, "\ntask %s\nsend request timeout %lu ticks, start %lu, now %lu",
                                taskname, timeout, start_time, SYSFUN_CURRENT_CLOCK);
        } else if (rc == SYSFUN_RESULT_MSG_Q_FULL) {
            __SYSFUN_DBG_Printf(func, line, "\ntask %s, send Q full, timeout %lu",
                                taskname, timeout);
        }
        if ((wait_time == SYSFUN_TIMEOUT_WAIT_FOREVER)
            && (sysfun_msgq_debug & SYSFUN_MSGQ_DEBUG_SEND_REQ)) {
            goto RESEND_MSG;
        }
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */

        return rc;
    }

    /* Send event to msgq owner. (not need to care if successfully) */
    owner = SYSFUN_GET_MSGQOWNER_INTERNAL(msgq);
    if (event && owner) {
        SYSFUN_SendEvent(owner, event);
    }

    /* To exit if it is unneeded to wait for response */
    if (uni_dir || !res_msg_p) {
        return SYSFUN_OK;
    }

    msgsz   = SYSFUN_GET_LINUX_MSG_SIZE(res_msg_size);
    timeout = SYSFUN_TIMEOUT_WAIT_FOREVER;

#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
    /* give a limited timeout value to check whether receiving message use a long time */
    if (sysfun_msgq_debug & SYSFUN_MSGQ_DEBUG_RECV_REQ_RESPONSE) {
        timeout = 2 * sysfun_msgq_default_timeout;
    }
    retry   = 0;
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */

RERECV_MSG:

#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
    start_time = SYSFUN_CURRENT_CLOCK;
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */

    rc = SYSFUN_MsgRecv((int)msgq->response_qid, res_msg_p, msgsz, req_msg_p->msg_type, timeout);

    if (rc != SYSFUN_OK) {
#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
        if (rc == SYSFUN_RESULT_TIMEOUT) {
            __SYSFUN_DBG_Printf(func, line, "\ntask %s\nREQ response timeout %lu ticks, start %lu, now %lu",
                                taskname, timeout, start_time, SYSFUN_CURRENT_CLOCK);
            if (++retry > 3) {
                timeout = SYSFUN_TIMEOUT_WAIT_FOREVER;
            }
        } else {
            __SYSFUN_DBG_Printf(func, line, "\ntask %s, recv response error, timeout %lu, rc %lu",
                                taskname, timeout, rc);
        }
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */

        goto RERECV_MSG;
    }

    return rc;
}

#else ////////////////////////////////////////////////////

UI32_T _SYSFUN_SendRequestMsg(UI32_T msgq_handle,
                             SYSFUN_Msg_T *req_msg_p,
                             UI32_T wait_time,
                             UI32_T event,
                             UI32_T res_msg_size,
                             SYSFUN_Msg_T *res_msg_p)
{
    SYSFUN_MsgQDesc_T *msgq_desc_p = (SYSFUN_MsgQDesc_T *)msgq_handle;
    UI32_T  owner_tid;
    int     msgsz, msgflg;
    clock_t timeout_tick = 0;
    BOOL_T  need_check_timeout = FALSE;
    BOOL_T  is_unidirectional;

    if (msgq_desc_p == NULL)
        return SYSFUN_RESULT_INVALID_ID;
    if (req_msg_p == NULL)
        return SYSFUN_RESULT_INVALID_ARG;
    msgsz = SYSFUN_GET_LINUX_MSG_SIZE(req_msg_p->msg_size);
    is_unidirectional = (msgq_desc_p->response_qid == SYSFUN_MSGQ_INVALID_ID);

    if (is_unidirectional && req_msg_p->msg_type == 0)
        return SYSFUN_RESULT_INVALID_ARG;

    /* If msgq is bidirectional, msg_type means the id of the task that sent request. */
    if (!is_unidirectional)
        req_msg_p->msg_type = SYSFUN_TaskIdSelf();

    switch (wait_time)
    {
        case SYSFUN_TIMEOUT_NOWAIT:
            msgflg = IPC_NOWAIT;
            break;
        case SYSFUN_TIMEOUT_WAIT_FOREVER:
            msgflg = 0;
            break;
        default:
            need_check_timeout = TRUE;
            msgflg = IPC_NOWAIT;
            timeout_tick = times(NULL) + wait_time;
    }

SEND_AGAIN:
    if (msgsnd(msgq_desc_p->request_qid, (struct msgbuf *)req_msg_p, msgsz, msgflg) == -1)
    {
        switch(errno)
        {
            case EAGAIN:
                /* msg queue is full, might need to try again if need_check_timeout is TRUE */
                if(!need_check_timeout)
                    return SYSFUN_RESULT_MSG_Q_FULL;
                if(L_MATH_TimeOut32(times(NULL), timeout_tick))
                    return SYSFUN_RESULT_TIMEOUT;
                SYSFUN_Sleep(SYSFUN_TIMEOUT_CHECK_INTERVAL_IN_CLOCKTICK);
                goto SEND_AGAIN;
            case EACCES:
                return SYSFUN_RESULT_ACCESS_VIOLATION;
            case EFAULT:
                return SYSFUN_RESULT_BUF_UNACCESSIBLE;
            case EIDRM:
                return SYSFUN_RESULT_OBJ_DELETED;
            case EINTR:
                if(need_check_timeout && L_MATH_TimeOut32(times(NULL), timeout_tick))
                    return SYSFUN_RESULT_TIMEOUT;
                goto SEND_AGAIN;
            case EINVAL:
                return SYSFUN_RESULT_INVALID_ID;
            case ENOMEM:
                return SYSFUN_RESULT_INVALID_ID;
            default:
                assert(0);
                return SYSFUN_RESULT_ERROR;
        }
    }

    /* Send event to msgq owner. (not need to care if successfully) */
    if (event != 0 && (owner_tid = SYSFUN_GET_MSGQOWNER_INTERNAL(msgq_desc_p)) != 0)
        SYSFUN_SendEvent(owner_tid, event);

    /* To exit if it is unneeded to wait for response. */
    if (is_unidirectional || res_msg_p == NULL)
        return SYSFUN_OK;

    msgsz = SYSFUN_GET_LINUX_MSG_SIZE(res_msg_size);

RECEIVE_AGAIN:
    if (msgrcv(msgq_desc_p->response_qid, (struct msgbuf *)res_msg_p, msgsz, req_msg_p->msg_type, 0) == -1)
    {
        switch (errno)
        {
            case E2BIG:
                /* If ths size of buffer is less than message, drop the message */
                msgrcv((int)msgq_desc_p->response_qid, (struct msgbuf *)res_msg_p, msgsz, req_msg_p->msg_type, IPC_NOWAIT | MSG_NOERROR);
                return SYSFUN_RESULT_BUF_TOO_SMALL;
            case EFAULT:
                return SYSFUN_RESULT_BUF_UNACCESSIBLE;
            case EACCES:
                return SYSFUN_RESULT_ACCESS_VIOLATION;
            case EIDRM:
                return SYSFUN_RESULT_OBJ_DELETED;
            case EINTR:
                goto RECEIVE_AGAIN;
            case EINVAL:
                return SYSFUN_RESULT_INVALID_ID;
            case ENOMSG:
                assert(0);
                return SYSFUN_RESULT_ERROR;
            default:
                assert(0);
                return SYSFUN_RESULT_ERROR;
        }
    }

    return SYSFUN_OK;
}

#endif ////////////////////////////////////////////////////

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
#if 1 ////////////////////////////////////////////////////

UI32_T
__SYSFUN_SendResponseMsg(UI32_T         msgq_handle,
                         SYSFUN_Msg_T * res_msg_p
#ifdef SYSFUN_DEBUG
                         ,
                         const char *   func,
                         unsigned       line
#endif /* SYSFUN_DEBUG */
                         )
{
    SYSFUN_MsgQDesc_T * msgq = (SYSFUN_MsgQDesc_T *)msgq_handle;
    int                 msgsz;
    UI32_T              timeout;
    UI32_T              ret;

    if (!msgq || (msgq->response_qid == SYSFUN_MSGQ_INVALID_ID)) /* is unidirectional */
        return SYSFUN_RESULT_INVALID_ID;

    if (!res_msg_p)
        return SYSFUN_RESULT_INVALID_ARG;

    msgsz   = SYSFUN_GET_LINUX_MSG_SIZE(res_msg_p->msg_size);
    timeout = SYSFUN_TIMEOUT_WAIT_FOREVER;

#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
    if (sysfun_msgq_debug & SYSFUN_MSGQ_DEBUG_SEND_RESPONSE) {
        timeout = sysfun_msgq_default_timeout;
    }
SEND_AGAIN:
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */

    ret = SYSFUN_MsgSend((int)msgq->response_qid, res_msg_p, msgsz, timeout);

#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
    if (ret == SYSFUN_RESULT_TIMEOUT) {
        char    taskname[64];

        __SYSFUN_DBG_Printf(func, line, "\ntask %s, send response timeout %lu ticks, msgtype %lu",
                            SYSFUN_GetTaskName(0, taskname, sizeof (taskname)), timeout, res_msg_p->msg_type);
        goto SEND_AGAIN;
    }
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */

    return ret;
}

#else ////////////////////////////////////////////////////

UI32_T _SYSFUN_SendResponseMsg(UI32_T msgq_handle, SYSFUN_Msg_T *res_msg_p)
{
    SYSFUN_MsgQDesc_T * msgq_desc_p = (SYSFUN_MsgQDesc_T *)msgq_handle;
    int                 msgsz;

    if (!msgq_desc_p || (msgq_desc_p->response_qid == SYSFUN_MSGQ_INVALID_ID)) /* is unidirectional */
        return SYSFUN_RESULT_INVALID_ID;

    if (!res_msg_p)
        return SYSFUN_RESULT_INVALID_ARG;

    msgsz = SYSFUN_GET_LINUX_MSG_SIZE(res_msg_p->msg_size);

SEND_AGAIN:
    if (msgsnd(msgq_desc_p->response_qid, (struct msgbuf *)res_msg_p, msgsz, 0) == -1)
    {
        switch(errno)
        {
            case EACCES:
                return SYSFUN_RESULT_ACCESS_VIOLATION;
            case EFAULT:
                return SYSFUN_RESULT_BUF_UNACCESSIBLE;
            case EIDRM:
                return SYSFUN_RESULT_OBJ_DELETED;
            case EINTR:
                goto SEND_AGAIN;
            case EINVAL:
                return SYSFUN_RESULT_INVALID_ID;
            case ENOMEM:
                return SYSFUN_RESULT_INVALID_ID;
            case EAGAIN:
            default:
                assert(0);
                return SYSFUN_RESULT_ERROR;
        }
    }

    return SYSFUN_OK;
}

#endif ////////////////////////////////////////////////////

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
#if 1 ////////////////////////////////////////////////////

UI32_T
__SYSFUN_ReceiveMsg(UI32_T           msgq_handle,
                           UI32_T           msg_type,
                           UI32_T           wait_time,
                           UI32_T           req_msg_size,
                           SYSFUN_Msg_T *   req_msg_p
#ifdef SYSFUN_DEBUG
                           ,
                           const char *     func,
                           unsigned         line
#endif /* SYSFUN_DEBUG */
                           )
{
    SYSFUN_MsgQDesc_T * msgq = (SYSFUN_MsgQDesc_T *)msgq_handle;
    int                 msgsz;
    BOOL_T              uni_dir;
    UI32_T              timeout;
    UI32_T              ret;

    if (msgq == NULL)
        return SYSFUN_RESULT_INVALID_ID;

    if (req_msg_p == NULL)
        return SYSFUN_RESULT_INVALID_ARG;

    msgsz   = SYSFUN_GET_LINUX_MSG_SIZE(req_msg_size);
    uni_dir = (msgq->response_qid == SYSFUN_MSGQ_INVALID_ID);
    timeout = wait_time;

    /* If msgq is bidirectional,
     * set msg_type=0 to receive the first message in queue.
     */
    if (!uni_dir) {
        msg_type = 0;
    }

#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
    if ((wait_time == SYSFUN_TIMEOUT_WAIT_FOREVER)
        && (sysfun_msgq_debug & SYSFUN_MSGQ_DEBUG_RECV)) {
        timeout  = 5 * sysfun_msgq_default_timeout;
    }
RECEIVE_AGAIN:
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */

    ret = SYSFUN_MsgRecv((int)msgq->request_qid, req_msg_p, msgsz, msg_type, timeout);

#ifdef SYSFUN_DEBUG_MSGQ_TIMEOUT
    if (ret == SYSFUN_RESULT_TIMEOUT) {
        char    taskname[64];

        __SYSFUN_DBG_Printf(func, line, "\ntask %s, recv msg timeout %lu, msgtype %lu",
                            SYSFUN_GetTaskName(0, taskname, sizeof (taskname)), timeout, msg_type);

        if ((wait_time == SYSFUN_TIMEOUT_WAIT_FOREVER)
            && (sysfun_msgq_debug & SYSFUN_MSGQ_DEBUG_RECV)) {
            timeout = SYSFUN_TIMEOUT_WAIT_FOREVER;
            goto RECEIVE_AGAIN;
        }
    }
#endif /* SYSFUN_DEBUG_MSGQ_TIMEOUT */

    return ret;
}

#else ////////////////////////////////////////////////////

UI32_T _SYSFUN_ReceiveMsg(UI32_T        msgq_handle,
                          UI32_T        msg_type,
                          UI32_T        wait_time,
                          UI32_T        req_msg_size,
                          SYSFUN_Msg_T *req_msg_p)
{
    SYSFUN_MsgQDesc_T *msgq_desc_p = (SYSFUN_MsgQDesc_T *)msgq_handle;
    int     msgsz, msgflg;
    clock_t timeout_tick = 0;
    BOOL_T  need_check_timeout = FALSE;
    BOOL_T  is_unidirectional;

    if (msgq_desc_p == NULL)
        return SYSFUN_RESULT_INVALID_ID;
    if (req_msg_p == NULL)
        return SYSFUN_RESULT_INVALID_ARG;

    msgsz = SYSFUN_GET_LINUX_MSG_SIZE(req_msg_size);
    is_unidirectional = (msgq_desc_p->response_qid == SYSFUN_MSGQ_INVALID_ID);

    /* If msgq is bidirectional,
     * set msg_type=0 to receive the first message in queue.
     */
    if (!is_unidirectional)
        msg_type = 0;

    switch(wait_time)
    {
        case SYSFUN_TIMEOUT_NOWAIT:
            msgflg = IPC_NOWAIT;
            break;
        case SYSFUN_TIMEOUT_WAIT_FOREVER:
            msgflg = 0;
            break;
        default:
            need_check_timeout = TRUE;
            msgflg = IPC_NOWAIT;
            timeout_tick = times(NULL) + wait_time;
    }

RECEIVE_AGAIN:
    if (msgrcv(msgq_desc_p->request_qid, (struct msgbuf *)req_msg_p, msgsz, msg_type, msgflg) == -1)
    {
        switch(errno)
        {
            case E2BIG:
                /* If ths size of buffer is less than message, drop the message */
                msgrcv(msgq_desc_p->request_qid, (struct msgbuf *)req_msg_p, msgsz, msg_type, IPC_NOWAIT | MSG_NOERROR);
                return SYSFUN_RESULT_BUF_TOO_SMALL;
            case EFAULT:
                return SYSFUN_RESULT_BUF_UNACCESSIBLE;
            case EACCES:
                return SYSFUN_RESULT_ACCESS_VIOLATION;
            case EIDRM:
                return SYSFUN_RESULT_OBJ_DELETED;
            case EINTR:
                if(need_check_timeout && L_MATH_TimeOut32(times(NULL), timeout_tick))
                    return SYSFUN_RESULT_TIMEOUT;
                goto RECEIVE_AGAIN;
            case EINVAL:
                return SYSFUN_RESULT_INVALID_ID;
            case ENOMSG:
                return SYSFUN_RESULT_NO_MESSAGE;
            default:
                assert(0);
                return SYSFUN_RESULT_ERROR;
        }
    }

    return SYSFUN_OK;
}

#endif ////////////////////////////////////////////////////

#if 0 /* currently unused */
/* FUNCTION NAME : SYSFUN_CreateIPCSocket
 * PURPOSE:
 *      Create a socket for inter-process communication.
 *
 * INPUT:
 *      ipc_socket_name  --  The name of the socket to be created. Null-terminated string.
 *                           The maximum length of ipc_socket_name is SYS_ADPT_MAX_IPC_SOCKET_NAME_LEN.
 *      ipc_socket_id_p  --  The id of the created ipc socket will be put to this variable if sucess.
 *
 * OUTPUT:
 *      ipc_socket_id_p  --  The id of the created ipc socket will be put to this variable if sucess.
 *
 * RETURN:
 *      SYSFUN_OK                   -- Sucess
 *      SYSFUN_RESULT_INVALID_ARG   -- Fail due to invalid argument
 *      SYSFUN_RESULT_ERROR         -- Fail
 *
 * NOTES:
 *      1.An IPC socket can only be created and hold by one process.
 *
 */
UI32_T SYSFUN_CreateIPCSocket(char* ipc_socket_name, UI32_T *ipc_socket_id_p)
{
    int fd;
    struct sockaddr_un my_addr;

    if((ipc_socket_name==NULL)||(ipc_socket_id_p==NULL))
    {
        return SYSFUN_RESULT_INVALID_ARG;
    }

    fd = socket(PF_UNIX, SOCK_DGRAM, 0);

    if(fd<0)
    {
        printf("\r\n%s():Fail to create socket.", __FUNCTION__); fflush(stdout);
        return SYSFUN_RESULT_ERROR;
    }

    /* try to remove the file created last time
     */
    unlink(ipc_socket_name);
    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, ipc_socket_name, UNIX_PATH_MAX);
    if(bind(fd, (struct sockaddr*)&my_addr, sizeof(my_addr)))
    {
        perror(__FUNCTION__);
        close(fd);
        return SYSFUN_RESULT_ERROR;
    }
    *ipc_socket_id_p = fd;

    return SYSFUN_OK;
}

/* FUNCTION NAME : SYSFUN_SendToIPCSocket
 * PURPOSE:
 *      Send message to IPC socket.
 *
 * INPUT:
 *      my_socket_id         -- The ipc socket id of the sender.
 *      dest_ipc_socket_name -- The ipc socket name where the message will be sent to.
 *                              The maximum length of dest_ipc_socket_name is SYS_ADPT_MAX_IPC_SOCKET_NAME_LEN.
 *      msg                  -- The message to be sent to the specified ipc socket.
 *      msg_len              -- The length of the message.
 *      wait_time            -- waiting time(in clock ticks) if the queue of the destination ipc socket is full.
 *                              SYSFUN_TIMEOUT_NOWAIT        : no wait.
 *                              SYSFUN_TIMEOUT_WAIT_FOREVER  : wait forever
 *                              other: the time waiting.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      Sucess -- SYSFUN_RESULT_OK
 *      Fail   -- SYSFUN_RESULT_ERROR.
 *
 * NOTES:
 *
 */
UI32_T SYSFUN_SendToIPCSocket(UI32_T my_socket_id, char* dest_ipc_socket_name, char* msg, UI32_T msg_len, UI32_T wait_time)
{
    return SYSFUN_RESULT_ERROR;
}
#endif

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
 *      2. tid can be process id or thread id on Linux
 */
UI32_T SYSFUN_SendEvent (UI32_T tid, UI32_T event)
{
    // SYSFUN_DEBUG_LINE("send %08x to %d", (unsigned int)event, (int)tid);
    return SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_SENDEVENT, tid, event, 0, 0);
}

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
                            UI32_T *received_events)
{
    UI32_T current_tick, timeout_tick = 0;
    UI32_T ret;
    BOOL_T need_check_timeout = FALSE;

    // SYSFUN_DEBUG_LINE("wait: %08x, flags: %d, timeout: %d", (unsigned int)wait_events, (int)flags, timeout);

    if (timeout != SYSFUN_TIMEOUT_NOWAIT && timeout != SYSFUN_TIMEOUT_WAIT_FOREVER)
    {
        need_check_timeout = TRUE;
        timeout_tick = (UI32_T)times(NULL) + timeout;
    }

    while ((ret = SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_RECEIVEEVENT, wait_events, flags, timeout, (UI32_T)received_events)) == SYSFUN_RESULT_EINTR)
    {
        if (need_check_timeout)
        {
            current_tick = (UI32_T)times(NULL);
            if (L_MATH_TimeOut32(current_tick, timeout_tick))
                break;
            timeout = timeout_tick - current_tick;
        }
    }

    return ret;
}

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
void  *SYSFUN_PeriodicTimer_Create (void)
{
    periodic_timer_cb_t *   timer_cb_p;
    long                    rc;

    timer_cb_p = malloc(sizeof(periodic_timer_cb_t));

    if (timer_cb_p == NULL) {
        return NULL;
    }
    memset(timer_cb_p, 0, sizeof(periodic_timer_cb_t));
    timer_cb_p->timer_event_spec.sigev_signo = (SIGRTMIN + SYS_BLD_SIGNAL_PERIODIC_TIMER);
    timer_cb_p->timer_event_spec.sigev_notify = SIGEV_SIGNAL;
    timer_cb_p->timer_event_spec.sigev_value.sival_int = (int)timer_cb_p;
    timer_cb_p->pid = gettid();

    rc = timer_create(CLOCK_REALTIME, &(timer_cb_p->timer_event_spec), &(timer_cb_p->timer_id));
    if (rc != 0)
    {
        SYSFUN_DBG_PRINTF("create timer failed, pid %d, thr_id %lu\n",
                           getpid(), SYSFUN_TaskIdSelf());
        free(timer_cb_p);
        return NULL;
    }

    return (void *) timer_cb_p;
}

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
BOOL_T SYSFUN_PeriodicTimer_Start (void *tm_id, UI32_T time_interval, UI32_T event)
{
    periodic_timer_cb_t *   timer_cb_p = (periodic_timer_cb_t *) tm_id;
    struct itimerspec       interval;
    long                    rc;

    if (tm_id == NULL || event == 0)
        return FALSE;

    timer_cb_p->event = event;
    timer_cb_p->callback_func = NULL;

    SYSFUN_TicksToTimespec(time_interval, &interval.it_interval);
    interval.it_value = interval.it_interval;

    rc = timer_settime(timer_cb_p->timer_id, 0, &interval, NULL);
    if (rc != 0)
        return FALSE;

    return TRUE;
}

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
BOOL_T SYSFUN_PeriodicTimer_Restart (void *tm_id, UI32_T time_interval)
{
    periodic_timer_cb_t *timer_cb_p = tm_id;

    if (tm_id == NULL)
        return FALSE;

    return SYSFUN_PeriodicTimer_Start(tm_id, time_interval, timer_cb_p->event);
}

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
BOOL_T SYSFUN_PeriodicTimer_Stop (void *tm_id)
{
    periodic_timer_cb_t *   timer_cb_p = tm_id;
    struct itimerspec       interval;
    long                    rc;

    if (tm_id == NULL)
        return FALSE;

    interval.it_interval.tv_sec = interval.it_interval.tv_nsec =
        interval.it_value.tv_sec = interval.it_value.tv_nsec = 0;
    rc = timer_settime(timer_cb_p->timer_id, 0, &interval, NULL);
    return (rc == 0 ? TRUE : FALSE);
}

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
void SYSFUN_PeriodicTimer_Destroy (void *tm_id)
{
    periodic_timer_cb_t *timer_cb_p = tm_id;

    if (tm_id == NULL)
        return;

    if (timer_delete(timer_cb_p->timer_id)) {
        SYSFUN_DBG_PRINTF("\r\n%s: Failed to delete timer %lu", (UI32_T)tm_id);
    }

    free(tm_id);
}

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
UI32_T  SYSFUN_Register_Ticks_Callback(UI32_T  time_interval, VOID_FUNCTION call_back_fun)
{
    periodic_timer_cb_t *   timer_cb_p;
    struct itimerspec       interval;
    long                    rc;

    /* timer_cb_p will be never destroyed.
     */
    if ((timer_cb_p = SYSFUN_PeriodicTimer_Create()) == NULL)
        assert(0);

    timer_cb_p->event = 0;
    timer_cb_p->callback_func = call_back_fun;
    timer_cb_p->timer_event_spec.sigev_value.sival_int = (int)timer_cb_p;

    rc = timer_create(CLOCK_REALTIME, &(timer_cb_p->timer_event_spec), &(timer_cb_p->timer_id));
    if (rc != 0) {
        SYSFUN_DBG_PRINTF("no availabe timer, pid %d, thr_id %lu\n",
                           getpid(), SYSFUN_TaskIdSelf());
        return SYSFUN_RESULT_NO_AVAIL_TIMER;
    }

    SYSFUN_TicksToTimespec(time_interval, &interval.it_interval);
    interval.it_value = interval.it_interval;

    rc = timer_settime(timer_cb_p->timer_id, 0, &interval, NULL);

    /* XXX memory leak here
     * the allocated timer_cb_p never freed by any function ???
     */
    if (rc != 0)
        return SYSFUN_RESULT_INVALID_ARG;
    return SYSFUN_OK;
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
    /* There is a limitation in using 'times' to get system tick.
     * Take ARM for example, the system call handler will see the return value greater
     * than 0xFFFFF000 as error and do error num handling. Thus when system tick
     * falls between 0xFFFF000-0xFFFFFF, the program which calls this function will
     * always get return value as 0xFFFFFFFF. The same problem might also exists
     * on the other CPU archiecture.
     * charlie_chen 2009/9/22
     */
#if 0
    return (UI32_T)times(NULL);
#else
    /* Use argument to get system tick to avoid system call error number mechanism
     * charlie_chen 2009/9/22
     */
    UI32_T tick;

    SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_GETSYSTICK, (UI32_T)(&tick), 0, 0, 0);
    return tick;
#endif
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
UI32_T  SYSFUN_Sleep (UI32_T  ticks)
{
    UI32_T current, timeout;
    struct timespec ts;

    timeout = (UI32_T)times(NULL) + ticks;

SLEEP_AGAIN:
    SYSFUN_TicksToTimespec(ticks, &ts);

    if (nanosleep(&ts, NULL) == -1)
    {
        switch (errno)
        {
            case EINTR:
                current = (UI32_T)times(NULL);
                if (L_MATH_TimeOut32(current, timeout))
                    break;
                ticks = timeout - current;
                goto SLEEP_AGAIN;

            default:
                return SYSFUN_RESULT_ERROR;
        }
    }

    return SYSFUN_OK;
}

/* FUNCTION NAME : SYSFUN_LogMsg
 * PURPOSE:
 *      Log error message to system log, only one text length less than 56.
 *
 * INPUT:
 *      msg-- message (text) body.
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
int snprintf(char *buf, size_t size, const char *fmt, ...);
//int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

#define SYSFUN_LOGMSG_BUFSIZE       512

#if defined (SYSFUN_DEBUG) && (SYSFUN_DEBUG == TRUE)

static void
__SYSFUN_DBG_AssertFailedHandler(void)
{
}


static void (*__SYSFUN_DBG_AtAssertionFail)(void) = __SYSFUN_DBG_AssertFailedHandler;

void
__SYSFUN_DBG_Assert(const char *func, unsigned line, int cond, const char *condstr)
{
    if (!cond) {
        printf("DBG [%s(), %u]: Assertion failed (%s)\n", func, line, condstr);
        fflush(stdout);
        (*__SYSFUN_DBG_AtAssertionFail)();
    }
}

void
__SYSFUN_DBG_Printf(const char *func, unsigned line, const char *fmt, ...)
{
    char        buf[SYSFUN_LOGMSG_BUFSIZE];
    va_list     args;

    va_start(args, fmt);
    vsnprintf(buf, (sizeof (buf) - 1), fmt, args);
    va_end(args);

    printf("DBG [%s(), %u]: %s", func, line, buf);
    fflush(stdout);
}

void
__SYSFUN_DBG_LogMsg(int level, void *mod, const char *func, unsigned line, const char *fmt, ...)
{
    char        buf[SYSFUN_LOGMSG_BUFSIZE];
    va_list     args;

    va_start(args, fmt);
    vsnprintf(buf, (sizeof (buf) - 1), fmt, args);
    va_end(args);

    switch (level) {
        case SYSFUN_LOGLEVEL_URGENT:
            printf("log0 [%s(), %u]: %s", func, line, buf);
            break;
        case SYSFUN_LOGLEVEL_ALERT:
            printf("log1 [%s(), %u]: %s", func, line, buf);
            break;
        case SYSFUN_LOGLEVEL_CRITICAL:
            printf("log2 [%s(), %u]: %s", func, line, buf);
            break;
        case SYSFUN_LOGLEVEL_ERR:
            printf("log3 [%s(), %u]: %s", func, line, buf);
            break;
        case SYSFUN_LOGLEVEL_WARN:
            printf("log4 [%s(), %u]: %s", func, line, buf);
            break;
        case SYSFUN_LOGLEVEL_NOTICE:
            printf("log5 [%s(), %u]: %s", func, line, buf);
            break;
        case SYSFUN_LOGLEVEL_INFO:
            printf("log6 [%s(), %u]: %s", func, line, buf);
            break;
        case SYSFUN_LOGLEVEL_DEBUG:
            printf("log7 [%s(), %u]: %s", func, line, buf);
            break;
        default:
            printf("log [%s(), %u]: %s", func, line, buf);
            break;
    }
    fflush(stdout);
}

#endif /* SYSFUN_DEBUG == TRUE */

void
__SYSFUN_LogMsg(int level, void *mod, const char *fmt, ...)
{
    char        buf[SYSFUN_LOGMSG_BUFSIZE];
    va_list     args;

    va_start(args, fmt);
    vsnprintf(buf, (sizeof (buf) - 1), fmt, args);
    va_end(args);

    switch (level) {
        case SYSFUN_LOGLEVEL_URGENT:
            printf("log0: %s", buf);
            break;
        case SYSFUN_LOGLEVEL_ALERT:
            printf("log1: %s", buf);
            break;
        case SYSFUN_LOGLEVEL_CRITICAL:
            printf("log2: %s", buf);
            break;
        case SYSFUN_LOGLEVEL_ERR:
            printf("log3: %s", buf);
            break;
        case SYSFUN_LOGLEVEL_WARN:
            printf("log4: %s", buf);
            break;
        case SYSFUN_LOGLEVEL_NOTICE:
            printf("log5: %s", buf);
            break;
        case SYSFUN_LOGLEVEL_INFO:
            printf("log6: %s", buf);
            break;
        case SYSFUN_LOGLEVEL_DEBUG:
            printf("log7: %s", buf);
            break;
        default:
            printf("log: %s", buf);
            break;
    }
    fflush(stdout);
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
    /* Set Machine State Register.
     */

    asm volatile("mtmsr %0" : : "r" (val));
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
	unsigned int hid0;
	asm volatile("mfspr %0,1008" : "=r" (hid0) :);
	hid0 |= HID0_DCE;
	asm volatile("mtspr 1008,%0" : : "r" (hid0));
#elif defined(SYS_HWCFG_CPU_MIPS)
    printf("%s is called.\n", __FUNCTION__);
    SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_SETDCACHE, TRUE, 0, 0, 0);
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
	unsigned int hid0;
	asm volatile("mfspr %0,1008" : "=r" (hid0) :);
	hid0 &= ~HID0_DCE;
	asm volatile("mtspr 1008,%0" : : "r" (hid0));
#elif defined(SYS_HWCFG_CPU_MIPS)
    SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_SETDCACHE, FALSE, 0, 0, 0);
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
	unsigned int hid0;
	asm volatile("mfspr %0,1008" : "=r" (hid0) :);
	hid0 |= HID0_ICE;
	asm volatile("mtspr 1008,%0" : : "r" (hid0));
#elif defined(SYS_HWCFG_CPU_MIPS)
    SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_SETICACHE, TRUE, 0, 0, 0);
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
	unsigned int hid0;
	asm volatile("mfspr %0,1008" : "=r" (hid0) :);
	hid0 &= ~HID0_ICE;
	asm volatile("mtspr 1008,%0" : : "r" (hid0));
#elif defined(SYS_HWCFG_CPU_MIPS)
    SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_SETICACHE, FALSE, 0, 0, 0);
#endif
    return SYSFUN_OK;
}

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
UI32_T SYSFUN_OpenUART(UI32_T UART_channel)
{
    struct termios  termios;
    int             fd = -1;

    if (UART_channel == SYSFUN_UART_CHANNEL1)
        fd = open(SYS_ADPT_DEV_UART_NAME_0, O_RDWR | O_NOCTTY | O_NONBLOCK);
    else if (UART_channel == SYSFUN_UART_CHANNEL2)
        fd = open(SYS_ADPT_DEV_UART_NAME_1, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (fd == -1)
        return SYSFUN_RESULT_ERROR;

    /* Set UART to raw mode.
     */
    if (tcgetattr(fd, &termios) == -1)
    {
        close(fd);
        return SYSFUN_RESULT_ERROR;
    }

    /* Eugene modified to fix uart2 special ASCII code issue */
#if ((SYS_CPNT_POE == TRUE) && (SYS_CPNT_POE_INTERFACE == SYS_CPNT_POE_INTERFACE_UART))
    if (UART_channel == SYSFUN_UART_CHANNEL1)
    SYSFUN_UART_SET_RAW_MODE(&termios);
    else if (UART_channel == SYSFUN_UART_CHANNEL2)
        SYSFUN_UART_SET_POE_RAW_MODE(&termios);
#else
    SYSFUN_UART_SET_RAW_MODE(&termios);
#endif

    termios.c_cc[VSUSP] = 0; /* disable ctrl+z */

    tcsetattr(fd, TCSANOW, &termios);

    return (UI32_T)fd;
}

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
UI32_T SYSFUN_OpenDebugUART(UI32_T UART_channel)
{
    struct termios  termios;
    int             fd = -1;

    if (UART_channel == SYSFUN_UART_CHANNEL1)
        fd = open(SYS_ADPT_DEV_UART_NAME_0, O_RDWR | O_NONBLOCK);
    else if (UART_channel == SYSFUN_UART_CHANNEL2)
        fd = open(SYS_ADPT_DEV_UART_NAME_1, O_RDWR | O_NONBLOCK);

    if (fd == -1)
        return SYSFUN_RESULT_ERROR;

    /* Set UART to raw mode.
     */
    if (tcgetattr(fd, &termios) == -1)
    {
        close(fd);
        return SYSFUN_RESULT_ERROR;
    }

    SYSFUN_UART_SET_RAW_MODE(&termios);
    termios.c_cc[VINTR] = 0xc; /* map signal VINTR to CTRL+L */
    tcsetattr(fd, TCSANOW, &termios);

    if (ioctl(fd, TIOCSCTTY, (char *)1) < 0)
    {
        SYSFUN_DBG_PRINTF("ioctl UART TIOCSCTTY error, errno %d\n", errno);
        close(fd);
        return SYSFUN_RESULT_ERROR;
    }

    return (UI32_T)fd;
}

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
UI32_T SYSFUN_CloseUART(UI32_T UART_handle)
{
    if (close((int)UART_handle) == -1)
        return SYSFUN_RESULT_ERROR;

    return SYSFUN_OK;
}

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
UI32_T SYSFUN_UARTPollRxBuff(UI32_T UART_handle, UI32_T buf_len, void* buf_p)
{
    int ret;

    ret = read((int)UART_handle, buf_p, buf_len);

    return (UI32_T)(ret < 0 ? 0 : ret);
}

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
UI32_T SYSFUN_UARTPutData(UI32_T UART_handle, UI32_T buf_len, void *buf_p)
{
    if (write((int)UART_handle, buf_p, buf_len) != -1)
        return SYSFUN_OK;
    return SYSFUN_RESULT_ERROR;
}


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
                            UI8_T  stop_bits)
{
    struct termios termios;
    int serial_io_arg;
    UI32_T ret;

    /*boundary check */
    if ((baudrate != SYSFUN_UART_BAUDRATE_9600) &&
        (baudrate != SYSFUN_UART_BAUDRATE_19200) &&
        (baudrate != SYSFUN_UART_BAUDRATE_38400) &&
        (baudrate != SYSFUN_UART_BAUDRATE_57600) &&
        (baudrate != SYSFUN_UART_BAUDRATE_115200))
    {
        return SYSFUN_RESULT_INVALID_ARG;
    }

    if ((parity != SYSFUN_UART_PARITY_NONE) &&
        (parity != SYSFUN_UART_PARITY_EVEN) &&
        (parity != SYSFUN_UART_PARITY_ODD))
    {
         return SYSFUN_RESULT_INVALID_ARG;
    }

    if ((data_length != SYSFUN_UART_DATA_LENGTH_5_BITS) &&
        (data_length != SYSFUN_UART_DATA_LENGTH_6_BITS) &&
        (data_length != SYSFUN_UART_DATA_LENGTH_7_BITS) &&
        (data_length != SYSFUN_UART_DATA_LENGTH_8_BITS))
    {
         return SYSFUN_RESULT_INVALID_ARG;
    }

    if ((stop_bits != SYSFUN_UART_STOP_BITS_1_BITS) &&
        (stop_bits != SYSFUN_UART_STOP_BITS_2_BITS))
    {
         return SYSFUN_RESULT_INVALID_ARG;
    }

    /*Set baudrate*/
    if ((ret = SYSFUN_SetUartBaudRate(UART_handle, baudrate)) != SYSFUN_OK)
        return ret;

    if (tcgetattr((int)UART_handle, &termios) == -1)
        return errno;

    serial_io_arg = termios.c_cflag;

    /*Set parity*/
    switch(parity)
    {
        case SYSFUN_UART_PARITY_NONE:
            serial_io_arg &= ~PARENB;
            break;
        case SYSFUN_UART_PARITY_EVEN:
            serial_io_arg |= PARENB;
            serial_io_arg &= ~PARODD;
            break;
        case SYSFUN_UART_PARITY_ODD:
            serial_io_arg |= PARENB;
            serial_io_arg |= PARODD;
            break;
        default:
            break;
    }

    /*Set data length*/
    switch(data_length)
    {
        case SYSFUN_UART_DATA_LENGTH_5_BITS:
            serial_io_arg &= ~CSIZE;
            serial_io_arg |= CS5;
            break;
        case SYSFUN_UART_DATA_LENGTH_6_BITS:
            serial_io_arg &= ~CSIZE;
            serial_io_arg |= CS6;
            break;
        case SYSFUN_UART_DATA_LENGTH_7_BITS:
            serial_io_arg &= ~CSIZE;
            serial_io_arg |= CS7;
            break;
        case SYSFUN_UART_DATA_LENGTH_8_BITS:
            serial_io_arg &= ~CSIZE;
            serial_io_arg |= CS8;
            break;
        default:
            break;
    }

    /*Set stop bits*/
    switch(stop_bits)
    {
        case SYSFUN_UART_STOP_BITS_1_BITS:
            serial_io_arg &= ~CSTOPB;
            break;
        case SYSFUN_UART_STOP_BITS_2_BITS:
            serial_io_arg |= CSTOPB;
            break;
        default:
            break;
    }

    termios.c_cflag = serial_io_arg;

    return (tcsetattr((int)UART_handle, TCSANOW, &termios) == 0 ? SYSFUN_OK : errno);
}

/* FUNCTION NAME : SYSFUN_SetUartBaudRate
 * PURPOSE:
 *      Set UART's baud rate.
 * INPUT:
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
UI32_T SYSFUN_SetUartBaudRate(UI32_T UART_handle, UI32_T baudrate)
{
    struct termios termios;
    int speed;

    switch (baudrate)
    {
        case SYSFUN_UART_BAUDRATE_9600:   speed = B9600;   break;
        case SYSFUN_UART_BAUDRATE_19200:  speed = B19200;  break;
        case SYSFUN_UART_BAUDRATE_38400:  speed = B38400;  break;
        case SYSFUN_UART_BAUDRATE_57600:  speed = B57600;  break;
        case SYSFUN_UART_BAUDRATE_115200: speed = B115200; break;
        default:
            return SYSFUN_RESULT_INVALID_ARG;
    }

    if (tcgetattr((int)UART_handle, &termios) == -1)
        return errno;
    if (cfsetispeed(&termios, speed) == -1 || cfsetospeed(&termios, speed) == -1)
        return errno;
    tcflush((int)UART_handle, TCIOFLUSH);

    return (tcsetattr((int)UART_handle, TCSANOW, &termios) == 0 ? SYSFUN_OK : errno);
}

/* FUNCTION NAME : SYSFUN_GetUartBaudRate
 * PURPOSE:
 *      Get UART's baud rate.
 * INPUT: None.
 *
 * OUTPUT: *baudrate
 * RETURN: OK(0) on success, EIO(5) on device error, ENOSYS(71) on unsupported
 *         request.
 *
 * NOTES: None.
 *
 */
UI32_T SYSFUN_GetUartBaudRate(UI32_T UART_handle, UI32_T *baudrate)
{
    struct termios termios;
    int speed;

    if (tcgetattr((int)UART_handle, &termios) == -1)
        return errno;

    speed = cfgetispeed(&termios);

    switch (speed)
    {
        case B9600:   *baudrate = 9600;   break;
        case B19200:  *baudrate = 19200;  break;
        case B38400:  *baudrate = 38400;  break;
        case B57600:  *baudrate = 57600;  break;
        case B115200: *baudrate = 115200; break;
        default:
            return SYSFUN_RESULT_ERROR;
    }
    return SYSFUN_OK;
}

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
void SYSFUN_InvokeLinuxShell(void)
{
#define PASSWD_LEN  16

    struct termios orig_termios, termios;
    int ret, status;
    pid_t pid;
    char passwd[PASSWD_LEN];
    char ch;
    int login = 0, i = 0;
    static int rentry = 0;

	if (rentry)
		return;

	rentry = 1;
	memset(passwd, 0, PASSWD_LEN);

    ret = tcgetattr(0, &termios);
    if (ret != -1)
    {
        memcpy(&orig_termios, &termios, sizeof(orig_termios));
        SYSFUN_UART_SET_DEFAULT_MODE(&termios);
        termios.c_lflag &= ~ECHO;
        tcsetattr(0, TCSANOW, &termios);
        while ((ch = (char)getchar()) != (char)EOF) {
            if (ch == '\r' || ch == '\n')
                break;
            passwd[i] = ch;
            if (++i == PASSWD_LEN)
                break;
        }

        /* check the password */
		if (strcmp(passwd, "mercury") == 0)
			login = 1;

        //printf("passwd %s\n", passwd);
        if (login) {
        	termios.c_lflag |= ECHO;
        	tcsetattr(0, TCSANOW, &termios);

			/*SYSFUN_DEBUG_LINE("Enter Linux shell\r\n");*//*fuzhimin,20090303*/
			if ((pid = fork()) < 0)
				return;

			if (pid == 0)
				execl("/bin/sh", "sh", (char *)0);
			else
				while (waitpid(pid, &status,0) < 0) ;
			/*SYSFUN_DEBUG_LINE("Leave Linux shell\r\n");*//*fuzhimin,20090303*/
		}
		tcsetattr(0, TCSANOW, &orig_termios);
    }
    rentry = 0;
#undef PASSWD_LEN
}

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
UI32_T SYSFUN_Syscall(UI32_T syscall_id, UI32_T arg1, UI32_T arg2, UI32_T arg3, UI32_T arg4, UI32_T arg5)
{
    return sys_SYSFUN(syscall_id, arg1, arg2, arg3, arg4, arg5);
}

/*------------------------------------------------------------------------------
 * Function : SYSFUN_Reboot
 *------------------------------------------------------------------------------
 * Purpose  : This function will start the reboot procedure.
 * INPUT    : None.
 * OUTPUT   : None.
 * RETURN   : None.
 * NOTE     : None.
 *-----------------------------------------------------------------------------*/
void SYSFUN_Reboot(char *command)
{
    sync();

/* from include/linux/reboot.h in linux kernel */
#define	LINUX_REBOOT_MAGIC1	0xfee1dead
#define	LINUX_REBOOT_MAGIC2	672274793
#define	LINUX_REBOOT_CMD_RESTART2	0xA1B2C3D4

    //printf("SYSFUN_Reboot command %s\n", command);
    //SYSFUN_Sleep(10);

    //reboot(RB_AUTOBOOT);
    SYSFUN_Syscall(SYSFUN_SYSCALL_REBOOT, LINUX_REBOOT_MAGIC1,
        LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART2, (UI32_T)command, 0);
}

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
UI32_T SYSFUN_GetKernelVer(char kernel_ver[SYS_ADPT_KERNEL_VER_STR_LEN+1])
{
    struct utsname name;

    if(uname(&name) == -1)
    {
        return SYSFUN_RESULT_ERROR;
    }

    strncpy(kernel_ver, name.release, (SYS_ADPT_KERNEL_VER_STR_LEN + 1));
    kernel_ver[SYS_ADPT_KERNEL_VER_STR_LEN] = 0;

    return SYSFUN_OK;
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
    return SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_GET_CPU_USAGE, (UI32_T)busy_ticks_p, (UI32_T)idle_ticks_p, 0, 0);
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
    return SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_GET_MEMORY_USAGE, (UI32_T)total_bytes_p, (UI32_T)free_bytes_p, 0, 0);
}

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
void SYSFUN_Yield(void)
{
    sched_yield();
}

/* LOCAL SUBPROGRAM BODIES
 */
/* FUNCTION NAME : SYSFUN_InternalNewThreadEntry
 * PURPOSE:
 *      This is a helper routine for SYSFUN_SpawnThread().
 *
 * INPUT:
 *      arg    -- A pointer to SYSFUN_StartThreadArg_T.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      NULL   -- not used
 *
 * NOTES:
 */
static void *SYSFUN_InternalNewThreadEntry(void* arg)
{
    SYSFUN_StartThreadEntry_T * start_thread_entry_p = (SYSFUN_StartThreadEntry_T*)arg;
    void                     (* thread_routine)(void*);
    void *                      user_arg;
    int                         rc;

    start_thread_entry_p->arg.newThreadID = gettid();
    thread_routine = start_thread_entry_p->thread_routine;
    user_arg=start_thread_entry_p->arg.user_arg;

    if(SYSFUN_SetTaskPriority(start_thread_entry_p->arg.sched_policy, 0, start_thread_entry_p->arg.thread_priority)!=SYSFUN_OK)
    {
        SYSFUN_DBG_PRINTF("SYSFUN_SetTaskPriority() error\n");
    }

    /* init process working environment
     */
    SYSFUN_ThreadInit(start_thread_entry_p->arg.thread_name);

    rc = sem_post(&(start_thread_entry_p->arg.newThreadID_sem));
    if(rc!=0)
    {
        SYSFUN_DBG_PRINTF("sem_post() fail, rc %d, errno %d\n", rc, errno);
    }

    thread_routine(user_arg);

    return NULL;
}

/* FUNCTION NAME : SYSFUN_InternalInitThreadEntry
 * PURPOSE:
 *      This is a helper routine for SYSFUN_InitThread().
 *
 * INPUT:
 *      arg    -- A pointer to SYSFUN_InitThreadArg_T.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      NULL   -- not used
 *
 * NOTES:
 */
static void *SYSFUN_InternalInitThreadEntry(void* arg)
{
    SYSFUN_InitThreadArg_T  init_arg;
    union semun             semarg;

    memcpy(&init_arg, arg, sizeof(SYSFUN_InitThreadArg_T));

    semarg.val = 1;
    if (semctl(init_arg.sem_id, 0, SETVAL, semarg) == -1)
    {
        SYSFUN_DBG_PRINTF("semctl() fail, errno %d", errno);
    }

    SYSFUN_SuspendThreadSelf();

    init_arg.func_ptr(init_arg.arg);

    return NULL;
}


/* FUNCTION NAME : SYSFUN_PeriodicTimerSignalHandler
 * PURPOSE:
 *      This is a helper routine for SYSFUN_PeriodicTimer_XXX().
 *
 * INPUT:
 *      sig_num    --  signal number
 *      info_p     --  a pointer to siginfo_t. Contains the timer event to send.
 *      ucontext   --  a pointer to ucontext_t.
 *
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      NULL   -- not used
 *
 * NOTES:
 */
static void SYSFUN_PeriodicTimerSignalHandler(int sig_num, siginfo_t *info_p, void *ucontext)
{
    periodic_timer_cb_t *timer_cb_p = (periodic_timer_cb_t *) (info_p->si_int);

    /* avoid compilation warnings */
    ((void) sig_num);
    ((void) ucontext);

    if (timer_cb_p == NULL)
    {
        SYSFUN_DBG_PRINTF("NULL periodic timer pointer\r\n");
        return;
    }

    /* If event == 0,
     *   the timer was created by SYSFUN_Register_Ticks_Callback(),
     *   so run the registered callback function (si_ptr).
     */
    if (timer_cb_p->event)
    {
        // SYSFUN_DEBUG_LINE("tm_id: %d, SendEvent %08x to %d", info_p->si_int, (unsigned int)timer_cb_p->event, (int)timer_cb_p->pid);
        SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_SENDEVENT, (UI32_T)timer_cb_p->pid, timer_cb_p->event, 0, 0);
    }
    else
    {
        // SYSFUN_DEBUG_LINE("tm_id: %d, CallBack: %08x", info_p->si_int, (unsigned int)timer_cb_p->callback_func);
        timer_cb_p->callback_func();
    }
}

/* FUNCTION NAME : SYSFUN_SegmentationFaultSignalHandler
 * PURPOSE:
 *      Hook the segment fault and print the invalid address.
 *
 * INPUT:
 *      sig_num    --  signal number
 *      info_p     --  a pointer to siginfo_t. Contains the timer event to send.
 *      ucontext   --  a pointer to ucontext_t.
 *
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      NULL   -- not used
 *
 * NOTES:
 */
static void SYSFUN_SegmentationFaultSignalHandler(int sig_num, siginfo_t *info_p, void *ucontext)
{
#if 0
/*when exception, dump user stack in kernel, proc_pid_ustack, base.c*/
    SYSFUN_SHOW_TASK_INFO();
    printf("\r\nSegmemtation fault: invalid address: 0x%08x\r\n",
            (unsigned int)info_p->si_addr);
    printf("Memory Maps\r\n");
    fflush(stdout);

    /* system("cat /proc/self/maps"); */
    {
        char cmd[24];
        sprintf(cmd, "cat /proc/%d/maps", (int)gettid());
        system(cmd);
    }

#if (SYSFUN_RD_DEBUG_ENABLE_DUMP_TASK_STACK==TRUE)
    SYSFUN_Debug_DumpTaskStack();
#endif
#endif
    SYSLOG_OM_Record_T syslog_entry;

    /* execute /etc/exception_log.sh to log exception under /flash
     */
    system("/etc/exception_log.sh");

    memset((UI8_T *)&syslog_entry, 0, sizeof(SYSLOG_OM_Record_T));
    syslog_entry.owner_info.level = SYSLOG_LEVEL_EMERG;
    syslog_entry.owner_info.module_no = SYS_MODULE_SYSTEM;
    syslog_entry.owner_info.function_no = 0;
    syslog_entry.owner_info.error_no = 0;
    sprintf(syslog_entry.message, "An exception occurs.");
    /* SYLOG_PMGR_AddEntrySync might not be able to write
     * message to log if core_util process gets exception
     */
    SYSLOG_PMGR_AddEntrySync(&syslog_entry);

    /* copy this snippet from glibc/debug/segfault.c:catch_segfault()
     * Pass on the signal (so that a core file is produced).
     */
    {
        struct sigaction sa;

        sa.sa_handler = SIG_DFL;
        sigemptyset (&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction (sig_num, &sa, NULL);
    }
}

#if 0
static void SYSFUN_CtrlCSignalHandler(int sig_num, siginfo_t *info_p, void *ucontext)
{
	SYSFUN_InvokeLinuxShell();
}
#endif

/* FUNCTION NAME : SYSFUN_TaskTerminatedHookFun
 * PURPOSE:
 *      This function will be called before a process/thread being terminated.
 *
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
 */
static void SYSFUN_TaskTerminatedHookFun(void)
{
    SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_DESTROY_UTCB, 0, 0, 0, 0);
}

/*
 * send a signal to the process whose process number is pid
 * sig_offset is the signal offset of SIGRTMIN
 */
UI32_T SYSFUN_SendSignal(UI32_T taskno, int sig_offset)
{
    pid_t pid = (pid_t)taskno;

    if ((SIGRTMIN + sig_offset > _NSIG) || (sig_offset < 0))
        return 1;

    if (kill(pid, (SIGRTMIN + sig_offset)) < 0)
        return 1;

    return 0;
}

/*
 * register a signal to linux kernel
 * sig_offset is the signal offset of SIGRTMIN
 * void (*sig_handler)(int) is a function pointer, the parameter is an integal number which means signal no
 */
UI32_T SYSFUN_RegisterSignal(int sig_offset, void (*sig_handler)(int))
{
    struct sigaction act, oact;

    if ((SIGRTMIN + sig_offset > _NSIG) || (sig_offset < 0))
        return 1;

    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(SIGRTMIN + sig_offset, &act, &oact) < 0)
        return 1;

    return 0;
}

/*
 * register an advance signal to linux kernel
 * sig_offset is the signal offset of SIGRTMIN
 * void (*sig_action)(int, siginfo_t *, void *) is a functino pointer which has three parameters
 * parameter1 is a signal no,
 * parameter2 is a pointer to the struct of siginfo, it can include some user information
 * parameter3 is unused.
 */
UI32_T SYSFUN_RegisterAdvSignal(int sig_offset, void (*sig_action)(int, siginfo_t *, void *))
{
    struct sigaction act, oact;

    if ((SIGRTMIN + sig_offset > _NSIG) || (sig_offset < 0))
        return 1;

    act.sa_sigaction = sig_action;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_flags |= SA_SIGINFO;

    if (sigaction(SIGRTMIN + sig_offset, &act, &oact) < 0)
        return 1;

    return 0;
}

UI32_T  SYSFUN_Sleep_Interruptible (UI32_T  seconds)
{
    return sleep(seconds);
}

/* FUNCTION NAME : SYSFUN_ProcessInit
 * PURPOSE:
 *      Initialize working environment when a process is spawned.
 * INPUT:
 *      process_name    -- the name of the process calling this function.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *     1. This function is called by SYSFUN_SpawnProcess() on Linux.
 */
static void SYSFUN_ProcessInit(char *process_name)
{
    struct sigaction    act;
    int                 rc;

    rc = SYSFUN_RegisterAdvSignal(SYS_BLD_SIGNAL_PERIODIC_TIMER, SYSFUN_PeriodicTimerSignalHandler);
    if (rc != 0)
    {
        SYSFUN_DBG_PRINTF("sigaction for periodic timer fail, rc %d, errno %d\n", rc, errno);
    }

    act.sa_sigaction = SYSFUN_SegmentationFaultSignalHandler;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = SA_SIGINFO;
    rc           = sigaction(SIGSEGV, &act, NULL);
    if (rc != 0)
    {
        SYSFUN_DBG_PRINTF("sigaction for SIGSEGV fail, rc %d, errno %d\n", rc, errno);
    }

    act.sa_flags   = 0;
    act.sa_handler = SYSFUN_DummySingalHandler;
    rc             = sigaction(SIGUSR2, &act, NULL);
    if(rc!=0)
    {
        SYSFUN_DBG_PRINTF("sigaction for SIGUSR2 fail, rc %d, errno %d\n", rc, errno);
    }
    act.sa_handler = SIG_IGN;
    rc = sigaction(SIGPIPE, &act, NULL);
    if(rc!=0)
    {
        SYSFUN_DBG_PRINTF("\sigaction for SIGPIPE fail, rc %d, errno %d\n", rc, errno);
    }

    SYSFUN_TaskInit(process_name);
}

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
void SYSFUN_ThreadInit(char *thread_name)
{
    SYSFUN_TaskInit(thread_name);
}

/* FUNCTION NAME : SYSFUN_TaskInit
 * PURPOSE:
 *      Initialize working environment when a thread/process is spawned.
 * INPUT:
 *      task_name    -- the name of the process/thread calling this function.
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 *      1. Both a process in user space and a thread in user space will be
 *         viewed as a task in kernel space. This function do initialization
 *         when a new task is spawned in kernel space.
 */
static void SYSFUN_TaskInit(char *task_name)
{
    int rc;

    // SYSFUN_DEBUG_LINE("pid: %d, name: %s", (int)gettid(), task_name);

    /* init UTCB
     */
    SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_INIT_UTCB,
                   (UI32_T)task_name, 0, 0, 0);

    /* destroy UTCB when the task is terminated
     */
    rc = atexit(SYSFUN_TaskTerminatedHookFun);
    if (rc)
    {
        SYSFUN_DBG_PRINTF("set atexit() callback fail\n");
    }
}


/* FUNCTION NAME : SYSFUN_DummySingalHandler
 * PURPOSE:
 *      This is a dummy signal handler.
 *
 * INPUT:
 *      sig_num    --  signal number
 *
 *
 * OUTPUT:
 *      None.
 *
 * RETURN:
 *      None.
 *
 * NOTES:
 */
static void SYSFUN_DummySingalHandler(int sig_num)
{
    /* do nothing.
     */
    // SYSFUN_DBG_PRINTF("\r\nsignal %d received\r\n", sig_num);
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

/*------------------------------------------------------------------------------
 * Function : SYSFUN_TicksToTimespec
 *------------------------------------------------------------------------------
 * Purpose  : To convert ticks to struct timespec.
 * INPUT    : ticks
 * OUTPUT   : ts - struct timespec
 * RETURN   : None.
 * NOTE     : None.
 *-----------------------------------------------------------------------------*/
static void SYSFUN_TicksToTimespec(UI32_T ticks, struct timespec *ts)
{
    ts->tv_sec = ticks / SYS_BLD_TICKS_PER_SECOND;
    ts->tv_nsec = ((long int)(1000000000) / SYS_BLD_TICKS_PER_SECOND) *
                  (ticks - SYS_BLD_TICKS_PER_SECOND * ts->tv_sec);
}

#if 0
#if (SYSFUN_DEBUG==TRUE)
static void SYSFUN_Debug_ShowInfo(void)
{
    printf("\r\n%s:\r\n", __FUNCTION__);
    printf(" System call test - gettid(): %ld\r\n", gettid());
    printf(" System call test - sys_SYSFUN(): %s\r\n", (SYSFUN_Debug_ValidateSyscall() ? "OK" : "Failed"));
    printf(" System ticks per second: %ld\r\n", sysconf(_SC_CLK_TCK));
}

static BOOL_T SYSFUN_Debug_ValidateSyscall(void)
{
    return (SYSFUN_INTERNAL_SYSCALL_VALIDITY_VALUE ==
        SYSFUN_Syscall(SYSFUN_SYSCALL_INTERNAL, SYSFUN_INTERNAL_SYSCALL_VALIDITY, 0, 0, 0, 0));
}
#endif

#if (SYSFUN_RD_DEBUG_ENABLE_DUMP_TASK_STACK==TRUE)
#if 1
//#ifdef SYS_HWCFG_CPU_85XX
#define BOUNDED_N(PTR, N) (PTR)

#define BOUNDED_1(PTR) BOUNDED_N (PTR, 1)

struct layout
{
    struct layout *__unbounded  next;
    void *__unbounded           return_address;
};

static int
__backtrace (void **array, int size)
{
    struct layout * current;
    int             count;

    /* Force gcc to spill LR.  */
    asm volatile ("" : "=l"(current));

    /* Get the address on top-of-stack.  */
    asm volatile ("lwz %0,0(1)" : "=r"(current));
    current = BOUNDED_1 (current);

  for (				count = 0;
       current != NULL && 	count < size;
       current = BOUNDED_1 (current->next), count++)
    array[count] = current->return_address;

    /* It's possible the second-last stack frame can't return
       (that is, it's __libc_start_main), in which case
        the CRT startup code will have set its LR to 'NULL'.
     */
    if (count > 0 && array[count-1] == NULL)
        count--;

    return count;
}

#else /* SYS_HWCFG_CPU_82XX */

static int
__backtrace (void **array, int size)
{
    return 0;
}

#endif /* SYS_HWCFG_CPU_82XX */

#endif /* SYSFUN_RD_DEBUG_ENABLE_DUMP_TASK_STACK */
#endif

/* FUNCTION NAME : SYSFUN_GetTaskTick
 * PURPOSE:
 *      Get process accumulated ticks, from system start or restart.
 *
 * INPUT:
 *      pid:tid
 *
 * OUTPUT:
 *      usp_time:time in  user space (ticks)
 *      ksp_time:time in kernel space (ticks)
 * RETURN:
 *
 * NOTES:
 *      1. The unit of tick is 0.01 sec, ie. 10 ms. depending on system configuration.
 */
UI32_T SYSFUN_GetTaskTick (pid_t pid,long *usp_time,long *ksp_time)
{
    char procFilename[256],buffer[1024];
    int fd, num_read ,i;
    char *ptrUsr;

#define PROC_STAT_USER_TIME_POSITION 14

    sprintf(procFilename, "/proc/%d/stat",pid) ;
    fd = open(procFilename, O_RDONLY, 0);
    if(fd < 0)
        return SYSFUN_RESULT_ERROR;
    num_read = read(fd, buffer, 1023);
    close(fd);
    buffer[num_read] = '\0';

    ptrUsr = strrchr(buffer, ')') + 1 ;
    for(i = 3 ; i != PROC_STAT_USER_TIME_POSITION ; ++i)
    ptrUsr = strchr(ptrUsr+1, ' ') ;
    ptrUsr++;
    *usp_time = atol(ptrUsr) ;
    *ksp_time = atol(strchr(ptrUsr,' ') + 1) ;

    return SYSFUN_OK;
}

UI32_T SYSFUN_SetSysClock(UI32_T *t)
{
    return stime((time_t *)t);
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

    *seconds1970 = seconds_since_19700101;

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

/*------------------------------------------------------------------------------
 * Function : SYSFUN_EnterAutoBaud
 *------------------------------------------------------------------------------
 * Purpose  : Set the uart attrib for auto baudrate detection.
 * INPUT    : UART_handle  - uart handle.
 * RETURN   : SYSFUN_OK.
 * NOTE     : None.
 *-----------------------------------------------------------------------------*/
UI32_T SYSFUN_EnterAutoBaud(UI32_T UART_handle)
{
    struct termios  termios;

    if (tcgetattr((int)UART_handle, &termios) == -1)
        return SYSFUN_RESULT_ERROR;

    SYSFUN_UART_SET_REAL_RAW_MODE(&termios);

    tcflush((int)UART_handle, TCIOFLUSH);
    tcsetattr((int)UART_handle, TCSANOW, &termios);

    return SYSFUN_OK;
}

/*------------------------------------------------------------------------------
 * Function : SYSFUN_LeaveAutoBaud
 *------------------------------------------------------------------------------
 * Purpose  : Restore the uart attrib.
 * INPUT    : UART_handle  - uart handle.
 * RETURN   : SYSFUN_OK.
 * NOTE     : None.
 *-----------------------------------------------------------------------------*/
UI32_T SYSFUN_LeaveAutoBaud(UI32_T UART_handle)
{
    struct termios  termios;

    if (tcgetattr((int)UART_handle, &termios) == -1)
        return SYSFUN_RESULT_ERROR;

    SYSFUN_UART_SET_RAW_MODE(&termios);

    tcflush((int)UART_handle, TCIOFLUSH);
    tcsetattr((int)UART_handle, TCSANOW, &termios);

    return SYSFUN_OK;
}

