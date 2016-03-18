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

#ifndef     SYSCALL_H
#define     SYSCALL_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */
/* Command ID of syscall for external use.
 */
#define SYSCALL_MAX_NBR_OF_CALLBACK_FUNC 16


/* MACRO FUNCTION DECLARATIONS
 */


/* DATA TYPE DECLARATIONS
 */


/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
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
void SYSCALL_RegisterCallBackFunc(UI32_T syscall_cmd_id, void *func);


/*-------------------------
 * System Call Declarations
 *-------------------------
 */
asmlinkage UI32_T sys_SYSFUN(UI32_T syscall_cmd_id, UI32_T arg1, UI32_T arg2,
                             UI32_T arg3, UI32_T arg4, UI32_T arg5);

#endif /* SYSCALL_H */
