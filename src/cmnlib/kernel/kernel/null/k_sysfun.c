/* INCLUDE FILE DECLARATIONS
 */

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
#define SYSFUN_LOGLEVEL_DEFAULT    SYSFUN_LOGLEVEL_WARN
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
    printk(msg, ##__VA_ARGS__);               \
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

