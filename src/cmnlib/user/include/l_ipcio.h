/*-------------------------------------------------------------------------
 * MODULE NAME: l_ipcio.h
 *-------------------------------------------------------------------------
 * PURPOSE: Provide the input/output interface via IPC.
 *          Actually, an IPC input/output implemented by a certain task (ex. CLI).
 *
 * NOTES:
 *      1. L_IPCIO bases on an IPC mechanism.
 *      2. If the IPC resource in demand is available, these APIs ensure that
 *         IPC request is successful and may lead the calling task to sleep.
 *         Hence, the user MUST release other resource to avoid blocking
 *         other tasks.
 *
 * Modification History:
 *      Date                Modifier,           Reason
 *      ------------------------------------------------------------------
 *      05-09-2007          Wakka               Create
 *
 * COPYRIGHT(C)         Accton Corporation, 2007
 *------------------------------------------------------------------------*/
#ifndef  L_IPCIO_H
#define  L_IPCIO_H

/* INCLUDE FILE DECLARATIONS
 */
#include <stdarg.h>
#include "sys_type.h"
#include "sysfun.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define L_IPCIO_MAX_LENGTH_OF_ARG_BUF 80

enum L_IPCIO_ERR_E {
    L_IPCIO_ERR_NONE,
    L_IPCIO_ERR_ARG,
    L_IPCIO_ERR_IPC,
    L_IPCIO_ERR_ALLOC,
    L_IPCIO_ERR_IO,
};

enum L_IPCIO_CMD_E {
    L_IPCIO_CMD_GETCHAR,
    L_IPCIO_CMD_GETLINE,
    L_IPCIO_CMD_PRINT,
};

/* MACRO FUNCTION DECLARATIONS
 */
/*-----------------------------------------------------------------
 * MACRO NAME - L_IPCIO_SIZE_OF_MSG
 *-----------------------------------------------------------------
 * FUNCTION: To calculate the size of IPCIO msg according to size
 *           of its argument.
 * INPUT   : The size of argument. (ipcio_arg_xxx)
 * OUTPUT  : None.
 * RETURN  : The size of the IPCIO msg.
 * NOTE    : None.
 *----------------------------------------------------------------*/
#define L_IPCIO_SIZE_OF_MSG(arg_size) ((UI32_T)(((L_IPCIO_Msg_T *)0)->ipcio_arg_buffer) + (arg_size))

/* DATA TYPE DECLARATIONS
 */
/* Definition of the IPC message structure for IPC I/O.
 *   1. L_IPCIO APIs will send IPC request with this data structure as payload.
 *
 *   2. I/O service provider such as CLI must implement a handler to deal
 *      IPCIO messages (with message command SYS_TYPE_CMD_IPCIO).
 *
 *   3. The detail of IPCIO command.
 *      A IPCIO msg has two parts: cmd and arg. The IPCIO msg handlers
 *      do the corresponding action by cmd and input arg, and then
 *      store output to output arg if need.
 *
 *        IPCIO Command        Type   Input             Output            Return
 *        L_IPCIO_CMD_GETCHAR  sync.                    ipcio_arg_ch      ipcio_ret
 *        L_IPCIO_CMD_GETLINE  sync.  ipcio_arg_size    ipcio_arg_buffer  ipcio_ret
 *        L_IPCIO_CMD_PRINT    async. ipcio_arg_buffer
 */
typedef struct
{
    union {
        UI16_T ipcio_cmd;
        BOOL_T ipcio_ret;
    };
    union {
        int    ipcio_arg_ch;
        UI16_T ipcio_arg_size;
        char   ipcio_arg_buffer[L_IPCIO_MAX_LENGTH_OF_ARG_BUF];
    };
} L_IPCIO_Msg_T;


/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*-----------------------------------------------------------------
 * ROUTINE NAME - L_IPCIO_GetChar
 *-----------------------------------------------------------------
 * FUNCTION: This function will request a character as input via IPC.
 * INPUT   : msgq_handle - This is used to obtain IPC resource.
 * OUTPUT  : ch_p        - The character gotten via IPC.
 * RETURN  : L_IPCIO_ERR_NONE  - successful.
 *           L_IPCIO_ERR_ARG   - invalid parameter.
 *           L_IPCIO_ERR_IPC   - IPC operation is failed.
 *           L_IPCIO_ERR_IO    - I/O operation is failed.
 * NOTE    : None.
 *----------------------------------------------------------------*/
UI32_T L_IPCIO_GetChar(UI32_T msgq_handle, int *ch_p);

/*-----------------------------------------------------------------
 * ROUTINE NAME - L_IPCIO_GetLine
 *-----------------------------------------------------------------
 * FUNCTION: This function will request a null terminal string as
 *           input via IPC. The behavior is similar to fgets().
 * INPUT   : msgq_handle  - This is used to obtain IPC resource.
 *           buf_size     - The size of buffer.
 * OUTPUT  : buf_p        - The buffer to receive the string.
 * RETURN  : L_IPCIO_ERR_NONE  - successful.
 *           L_IPCIO_ERR_ARG   - invalid parameter.
 *           L_IPCIO_ERR_IPC   - IPC operation is failed; fill buf_p with empty string.
 *           L_IPCIO_ERR_ALLOC - out of memory; fill buf_p with empty string.
 *           L_IPCIO_ERR_IO    - I/O operation is failed.
 * NOTE    : It is to get input until enter is pressed, if buf_size
 *           is not enough, the first buf_size-1 are stored
 *           in buf_p, and posterior are truncated.
 *----------------------------------------------------------------*/
UI32_T L_IPCIO_GetLine(UI32_T msgq_handle, UI32_T buf_size, char *buf_p);

/*-----------------------------------------------------------------
 * ROUTINE NAME - L_IPCIO_Print
 *-----------------------------------------------------------------
 * FUNCTION: This function will request remote task to print a null
 *           terminal string via IPC.
 * INPUT   : msgq_handle  - This is used to obtain IPC resource.
 *           wait_time    - The waiting time if the queue is full.
 *           str_p        - The null terminal string.
 * OUTPUT  : None.
 * RETURN  : L_IPCIO_ERR_NONE  - successful.
 *           L_IPCIO_ERR_ARG   - invalid parameter.
 *           L_IPCIO_ERR_IPC   - IPC operation is failed.
 *           L_IPCIO_ERR_ALLOC - out of memory.
 * NOTE    : None.
 *----------------------------------------------------------------*/
UI32_T L_IPCIO_Print(UI32_T msgq_handle, UI32_T wait_time, char *str_p);

/*-----------------------------------------------------------------
 * ROUTINE NAME - L_IPCIO_Printf
 *-----------------------------------------------------------------
 * FUNCTION: This function will request remote task to print a
 *           formatted string via IPC.
 * INPUT   : msgq_handle  - This is used to obtain IPC resource.
 *           wait_time    - The waiting time if the queue is full.
 *           fmt_p        - The format of string.
 * OUTPUT  : None.
 * RETURN  : L_IPCIO_ERR_NONE  - successful.
 *           L_IPCIO_ERR_ARG   - invalid parameter.
 *           L_IPCIO_ERR_IPC   - IPC operation is failed.
 *           L_IPCIO_ERR_ALLOC - out of memory.
 * NOTE    : None.
 *----------------------------------------------------------------*/
UI32_T L_IPCIO_Printf(UI32_T msgq_handle, UI32_T wait_time, char *fmt_p, ...);

/*-----------------------------------------------------------------
 * ROUTINE NAME - L_IPCIO_VPrintf
 *-----------------------------------------------------------------
 * FUNCTION: This function will request remote task to print a
 *           formatted string via IPC.
 * INPUT   : msgq_handle  - This is used to obtain IPC resource.
 *           wait_time    - The waiting time if the queue is full.
 *           fmt_p        - The format of string.
 *           arg_p        - va_list
 * OUTPUT  : None.
 * RETURN  : L_IPCIO_ERR_NONE  - successful.
 *           L_IPCIO_ERR_ARG   - invalid parameter.
 *           L_IPCIO_ERR_IPC   - IPC operation is failed.
 *           L_IPCIO_ERR_ALLOC - out of memory.
 * NOTE    : None.
 *----------------------------------------------------------------*/
UI32_T L_IPCIO_VPrintf(UI32_T msgq_handle, UI32_T wait_time, char *fmt_p, va_list arg_p);

#endif /* L_IPCIO_H */

