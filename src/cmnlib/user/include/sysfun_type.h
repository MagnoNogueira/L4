/* MODULE NAME:  Sysfun_type.h
 * PURPOSE:
 *  Private definition for sysfun.
 *
 * NOTES:
 *
 * HISTORY
 *    2006/11/14 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2006
 */
#ifndef SYSFUN_TYPE_H
#define SYSFUN_TYPE_H

/* INCLUDE FILE DECLARATIONS
 */

/* NAMING CONSTANT DECLARATIONS
 */
/*  TASK option */
#define SYSFUN_TASK_NAME_LENGTH         15

/*  Timeout Parameter   */
#define SYSFUN_TIMEOUT_NOWAIT           0           /* No wait for timeout parameter    */
#define SYSFUN_TIMEOUT_WAIT_FOREVER     0xffffffff  /* Wait forever for timeout paramter    */

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
#define SYSFUN_RESULT_CANT_CREATE_MSG_Q 0x33        /*  Can't create the message queue  */
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
#define SYSFUN_RESULT_CANT_CREATE_SHMEM 0x44        /*  No more available memory for shared memory allocation */
#define SYSFUN_RESULT_MSG_Q_FULL        0x80        /*  Message queue is full, can't send message   */
#define SYSFUN_RESULT_CALL_FROM_ISR     0x81        /*  Invalid use system function, call from ISR  */
#define SYSFUN_RESULT_SYSFUN_NOT_INIT   0x82        /*  Not call SYSFUN_Init before call SYSFUN_xxx */
#define SYSFUN_RESULT_Q_FULL            0x83        /*  Queue is full                               */
#define SYSFUN_RESULT_Q_EMPTY           0x84        /*  Queue is empty                              */
#define SYSFUN_RESULT_TOO_MANY_REQUEST  0x85        /*  Can add more request to the function        */
#define SYSFUN_RESULT_BUF_UNACCESSIBLE  0x86        /*  The pointer to buffer isn't accessible      */
#define SYSFUN_RESULT_BUF_TOO_SMALL     0x87        /*  The buffer is too small                     */
#define SYSFUN_RESULT_ACCESS_VIOLATION  0x88        /*  The calling process have no read or write permission */
#define SYSFUN_RESULT_NO_MEMORY         0x89        /*  The system has not enough memory */
#define SYSFUN_RESULT_ERROR             0xFFFFFFFF  /*  error with unknown reason */

/* constant for sched policy
 */
#define SYSFUN_SCHED_DEFAULT    0
#define SYSFUN_SCHED_FIFO       1
#define SYSFUN_SCHED_RR         2


#define SYSFUN_INTERNAL_SYSCALL_VALIDITY_VALUE   0x0000000a

/* Command ID of syscall.
 */
enum SYSFUN_INTERNAL_SYSCALL_CMD_ID_E
{
    SYSFUN_INTERNAL_SYSCALL_VALIDITY,
    SYSFUN_INTERNAL_SYSCALL_INIT_UTCB,
    SYSFUN_INTERNAL_SYSCALL_DESTROY_UTCB,
    SYSFUN_INTERNAL_SYSCALL_TASKNAME_TO_ID,
    SYSFUN_INTERNAL_SYSCALL_TASKID_TO_NAME,
    SYSFUN_INTERNAL_SYSCALL_SENDEVENT,
    SYSFUN_INTERNAL_SYSCALL_RECEIVEEVENT,
    SYSFUN_INTERNAL_SYSCALL_GETSYSTICK,
    SYSFUN_INTERNAL_SYSCALL_SETSYSTICK, /* not support */
    SYSFUN_INTERNAL_SYSCALL_SUSPENDTASKSELF,
    SYSFUN_INTERNAL_SYSCALL_RESUMETASK,
    SYSFUN_INTERNAL_SYSCALL_SUSPENDTASKWITHTIMEOUT,
    SYSFUN_INTERNAL_SYSCALL_RESUMETASKWITHTIMEOUT,
    SYSFUN_INTERNAL_SYSCALL_DUMPTASKSTACK,
    SYSFUN_INTERNAL_SYSCALL_SETDCACHE,
    SYSFUN_INTERNAL_SYSCALL_SETICACHE,
    SYSFUN_INTERNAL_SYSCALL_GET_CPU_USAGE,
    SYSFUN_INTERNAL_SYSCALL_GET_MEMORY_USAGE
};

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

#endif    /* End of SYSFUN_TYPE_H */

