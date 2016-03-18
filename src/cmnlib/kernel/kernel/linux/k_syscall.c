/* Module Name: SYSCALL.H
 * Purpose:
 *      This module is unified system call for all platforms.
 *
 * Notes:
 *      None.
 *
 * HISTORY
 *       Date       --  Modifier,   Reason
 *       2007.04.12 --  Wakka,      Create.
 *
 * Copyright(C)      Accton Corporation, 2007
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <linux/module.h>
#include "sys_type.h"
#include "k_syscall.h"

/* DATA TYPE DECLARATIONS
 */
typedef UI32_T (*SYSCALL_CallBackFunc_T) (UI32_T arg1, UI32_T arg2, UI32_T arg3, UI32_T arg4, UI32_T arg5);


/* STATIC VARIABLE DECLARATIONS
 */
static SYSCALL_CallBackFunc_T syscall_callback_func[SYSCALL_MAX_NBR_OF_CALLBACK_FUNC] = { NULL };


/* FUNCTION NAME : SYSCALL_RegisterCallBackFunc
 * PURPOSE:
 *      This function is to register callback function for specific command of
 *      system call.
 * INPUT:
 *      syscall_cmd_id  - the command id, must be < SYSCALL_MAX_NBR_OF_CALLBACK_FUNC.
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
void SYSCALL_RegisterCallBackFunc(UI32_T syscall_cmd_id, void *func)
{
    if (syscall_cmd_id < SYSCALL_MAX_NBR_OF_CALLBACK_FUNC)
        syscall_callback_func[syscall_cmd_id] = (SYSCALL_CallBackFunc_T)func;
}
EXPORT_SYMBOL(SYSCALL_RegisterCallBackFunc);

/*-------------------------
 * System Call Implementation
 *-------------------------
 */
/* FUNCTION NAME : sys_SYSFUN
 * PURPOSE:
 *      This function is to dispatch the actions that must be execute in kernel to
 *       thecorresponding handler by command id.
 * INPUT:
 *      syscall_cmd_id  - the command id, must be < SYSCALL_MAX_NBR_OF_CALLBACK_FUNC.
 *      arg1 ~ arg5     - the arguments for the command.
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
asmlinkage UI32_T sys_SYSFUN(UI32_T syscall_cmd_id, UI32_T arg1, UI32_T arg2,
                             UI32_T arg3, UI32_T arg4, UI32_T arg5)
{
    if (syscall_cmd_id < SYSCALL_MAX_NBR_OF_CALLBACK_FUNC && syscall_callback_func[syscall_cmd_id] != NULL)
        return syscall_callback_func[syscall_cmd_id](arg1, arg2, arg3, arg4, arg5);
    else
        return 0; /* do nothing. */
}
