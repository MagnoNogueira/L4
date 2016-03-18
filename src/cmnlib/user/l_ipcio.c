/*-------------------------------------------------------------------------
 * MODULE NAME: l_ipcio.c
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

/* INCLUDE FILE DECLARATIONS
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "sys_type.h"
#include "sysfun.h"
#include "l_ipcio.h"

/* NAMING CONSTANT DECLARARTIONS
 */
#define L_IPCIO_UNIT_TEST       FALSE

/* The size of buffer for sending print message.
 */
#define L_IPCIO_MAX_LENGTH_FOR_SEND_PRINT (L_IPCIO_MAX_LENGTH_OF_ARG_BUF - 1)

 /* TYPE DEFINITIONS
 */

/* MACRO DEFINITIONS
 */
/*-----------------------------------------------------------------
 * MACRO NAME - L_IPCIO_Alloc
 *              L_IPCIO_Free
 *-----------------------------------------------------------------
 * FUNCTION: To allocate/free message buffer.
 * INPUT   : arg_size - The size of argument. (ipcio_arg_xxx)
 * OUTPUT  : None.
 * RETURN  : The address that points to the allocated memory.
 * NOTE    : None.
 *----------------------------------------------------------------*/
#define L_IPCIO_Alloc(arg_size) malloc(SYSFUN_SIZE_OF_MSG(L_IPCIO_SIZE_OF_MSG(arg_size)))
#define L_IPCIO_Free(p) free(p)

/* LOCAL FUNCTIONS DECLARATIONS
 */
static void *L_IPCIO_DumpVAList(const char *fmt_p, va_list arg_p, int *len_p);
static UI32_T L_IPCIO_PrintBuffer(UI32_T msgq_handle, UI32_T wait_time, UI32_T buf_len, const void *buf);

/* LOCAL VARIABLES DECLARATIONS
 */

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
UI32_T L_IPCIO_GetChar(UI32_T msgq_handle, int *ch_p)
{
    UI8_T buffer[SYSFUN_SIZE_OF_MSG(L_IPCIO_SIZE_OF_MSG(sizeof(char)))];
    SYSFUN_Msg_T *msg_p = (SYSFUN_Msg_T *)buffer;
    L_IPCIO_Msg_T *ipcio_msg_p;

    if (ch_p == NULL)
        return L_IPCIO_ERR_ARG;
    if (SYSFUN_GetMsgQOwner(msgq_handle) == 0)
        return L_IPCIO_ERR_IPC;

    /* Send synchronous request.
     */
    msg_p->cmd = SYS_TYPE_CMD_IPCIO;
    msg_p->msg_size = L_IPCIO_SIZE_OF_MSG(0);
    ipcio_msg_p = (L_IPCIO_Msg_T *)msg_p->msg_buf;
    ipcio_msg_p->ipcio_cmd = L_IPCIO_CMD_GETCHAR;

    if (SYSFUN_SendRequestMsg(msgq_handle,
                              msg_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER,
                              SYSFUN_SYSTEM_EVENT_IPCMSG,
                              L_IPCIO_SIZE_OF_MSG(sizeof(ipcio_msg_p->ipcio_arg_ch)),
                              msg_p) != SYSFUN_OK)
    {
        printf("\r\n%s: fail to send sync. request.", __FUNCTION__);
        return L_IPCIO_ERR_IPC;
    }

    if (!ipcio_msg_p->ipcio_ret)
    {
        *ch_p = EOF;
        return L_IPCIO_ERR_IO;
    }

    *ch_p = ipcio_msg_p->ipcio_arg_ch;

    return L_IPCIO_ERR_NONE;
}


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
UI32_T L_IPCIO_GetLine(UI32_T msgq_handle, UI32_T buf_size, char *buf_p)
{
    SYSFUN_Msg_T *msg_p;
    L_IPCIO_Msg_T *ipcio_msg_p;

    if (buf_size == 0 || buf_p == NULL)
        return L_IPCIO_ERR_ARG;

    *buf_p = 0;
    if  (buf_size == 1)
        return L_IPCIO_ERR_NONE;
    if (SYSFUN_GetMsgQOwner(msgq_handle) == 0)
        return L_IPCIO_ERR_IPC;

    /* NOTE:
     * limitation: cannot support buf_size > L_IPCIO_MAX_LENGTH_OF_ARG_BUF
     */
    if (buf_size > L_IPCIO_MAX_LENGTH_OF_ARG_BUF)
        buf_size = L_IPCIO_MAX_LENGTH_OF_ARG_BUF;

    /* The size to allocate must be bigger one of buf_size
     * and sizeof(ipcio_arg_size).
     * As sizeof(ipcio_arg_size) = 2 and buf_size always >= 2,
     * use buf_size directly.
     */
    msg_p = L_IPCIO_Alloc(buf_size);

    if (msg_p == NULL)
    {
        printf("\r\n%s: out of memory.", __FUNCTION__);
        return L_IPCIO_ERR_ALLOC;
    }

    /* Send synchronous request.
     */
    msg_p->cmd = SYS_TYPE_CMD_IPCIO;
    msg_p->msg_size = L_IPCIO_SIZE_OF_MSG(sizeof(ipcio_msg_p->ipcio_arg_size));
    ipcio_msg_p = (L_IPCIO_Msg_T *)msg_p->msg_buf;
    ipcio_msg_p->ipcio_cmd = L_IPCIO_CMD_GETLINE;
    ipcio_msg_p->ipcio_arg_size = buf_size;

    if (SYSFUN_SendRequestMsg(msgq_handle,
                              msg_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER,
                              SYSFUN_SYSTEM_EVENT_IPCMSG,
                              L_IPCIO_SIZE_OF_MSG(buf_size),
                              msg_p) != SYSFUN_OK)
    {
        printf("\r\n%s: fail to send sync. request.", __FUNCTION__);
        L_IPCIO_Free(msg_p);
        return L_IPCIO_ERR_IPC;
    }

    if (!ipcio_msg_p->ipcio_ret)
    {
        L_IPCIO_Free(msg_p);
        return L_IPCIO_ERR_IO;
    }

    /* The recevie message MUST be null terminal string.
     */
    strncpy(buf_p, ipcio_msg_p->ipcio_arg_buffer, buf_size);

    L_IPCIO_Free(msg_p);
    return L_IPCIO_ERR_NONE;
}


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
UI32_T L_IPCIO_Print(UI32_T msgq_handle, UI32_T wait_time, char *str_p)
{
    UI32_T str_len;

    if (str_p == NULL)
        return L_IPCIO_ERR_ARG;

    if ((str_len = strlen(str_p)) == 0)
        return L_IPCIO_ERR_ARG;

    if (SYSFUN_GetMsgQOwner(msgq_handle) == 0)
        return L_IPCIO_ERR_IPC;

    return L_IPCIO_PrintBuffer(msgq_handle, wait_time, str_len, str_p);
}

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
UI32_T L_IPCIO_Printf(UI32_T msgq_handle, UI32_T wait_time, char *fmt_p, ...)
{
    UI32_T ret;
    char *buf;
    int len;
    va_list arg_p;

    if (fmt_p == NULL)
        return L_IPCIO_ERR_ARG;
    if (SYSFUN_GetMsgQOwner(msgq_handle) == 0)
        return L_IPCIO_ERR_IPC;

    /* To parse fmt_p and arguments via vsnprintf()
     */
    va_start(arg_p, fmt_p);

    if ((buf = L_IPCIO_DumpVAList(fmt_p, arg_p, &len)) == NULL)
    {
        printf("\r\n%s: out of memory.", __FUNCTION__);
        va_end(arg_p);
        return L_IPCIO_ERR_ALLOC;
    }

    va_end(arg_p);

    ret = L_IPCIO_PrintBuffer(msgq_handle, wait_time, len, buf);

    free(buf);

    return ret;
}

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
UI32_T L_IPCIO_VPrintf(UI32_T msgq_handle, UI32_T wait_time, char *fmt_p, va_list arg_p)
{
    UI32_T ret;
    char *buf;
    int len;

    if (fmt_p == NULL)
        return L_IPCIO_ERR_ARG;
    if (SYSFUN_GetMsgQOwner(msgq_handle) == 0)
        return L_IPCIO_ERR_IPC;

    if ((buf = L_IPCIO_DumpVAList(fmt_p, arg_p, &len)) == NULL)
    {
        printf("\r\n%s: out of memory.", __FUNCTION__);
        return L_IPCIO_ERR_ALLOC;
    }

    ret = L_IPCIO_PrintBuffer(msgq_handle, wait_time, len, buf);

    free(buf);

    return ret;
}

#if L_IPCIO_UNUSED
static UI8_T l_ipcio_msg_buf[SYSFUN_SIZE_OF_MSG(sizeof(L_IPCIO_Msg_T))];
static UI16_T l_ipcio_wait_size = 0;

/*-----------------------------------------------------------------
 * ROUTINE NAME - L_IPCIO_HandleIPCReqMsg
 *-----------------------------------------------------------------
 * FUNCTION: This function will deal the ipcio msg.
 * INPUT   : msg_p             - the ipcio msg.
 *           print_func        - the function for output.
 * OUTPUT  : need_wait_key_p   - a I/O service provider should a flag
 *                               to track if someone is waitting for keyin.
 *                               while TRUE, it means that someone
 *                               needs to get user keyin and
 *                               the I/O service provider must
 *                               call L_IPCIO_ParseKey() when caller(UI)
 *                               get user keyin.
 * RETUEN  : TRUE  - need to send response.
 *           FALSE - not need to send response.
 * NOTE    : None.
 *----------------------------------------------------------------*/
BOOL_T L_IPCIO_HandleIPCReqMsg(SYSFUN_Msg_T *msg_p, void (*print_func)(const char *), BOOL_T *need_wait_key_p)
{
    L_IPCIO_Msg_T *l_ipcio_msg_p = (L_IPCIO_Msg_T *)msg_p->msg_buf;
    BOOL_T need_resp = FALSE;

    if (msg_p == NULL || need_wait_key_p == NULL)
        return FALSE;

    switch (l_ipcio_msg_p->ipcio_cmd)
    {
        case L_IPCIO_CMD_GETCHAR:
            if (*need_wait_key_p)
            {
                msg_p->msg_size = L_IPCIO_SIZE_OF_MSG(sizeof(l_ipcio_msg_p->ipcio_arg_ch));
                l_ipcio_msg_p->ipcio_arg_ch = '\0';
                need_resp = TRUE;
            }
            else
            {
                memcpy(l_ipcio_msg_buf, msg_p, SYSFUN_SIZE_OF_MSG(L_IPCIO_SIZE_OF_MSG(0)));
                *need_wait_key_p = TRUE;
            }
            break;

        case L_IPCIO_CMD_GETLINE:
            if (*need_wait_key_p)
            {
                msg_p->msg_size = L_IPCIO_SIZE_OF_MSG(1);
                l_ipcio_msg_p->ipcio_arg_buffer[0] = '\0';
                need_resp = TRUE;
            }
            else
            {
                memcpy(l_ipcio_msg_buf, msg_p, SYSFUN_SIZE_OF_MSG(L_IPCIO_SIZE_OF_MSG(0)));
                l_ipcio_wait_size = (l_ipcio_msg_p->ipcio_arg_size < sizeof(l_ipcio_msg_p->ipcio_arg_buffer) ?
                                        l_ipcio_msg_p->ipcio_arg_size : sizeof(l_ipcio_msg_p->ipcio_arg_buffer)) - 1;
                *need_wait_key_p = TRUE;
            }
            break;

        case L_IPCIO_CMD_PRINT:
            if (print_func)
                print_func(l_ipcio_msg_p->ipcio_arg_buffer);
            break;

        default:
            printf("%s: invalid command.\n", __FUNCTION__);
    }

    return need_resp;
}

/*-----------------------------------------------------------------
 * ROUTINE NAME - L_IPCIO_ParseKey
 *-----------------------------------------------------------------
 * FUNCTION: This function will parse a characters.
 * INPUT   : msgq_handle       - the ipcio msg.
 *           ch                - the character to deal.
 *           need_wait_key_p   - a I/O service provider should a flag
 *                               to track if someone is waitting for keyin.
 *                               while TRUE, it means that someone
 *                               needs to get user keyin and
 *                               the I/O service provider must
 *                               call L_IPCIO_ParseKey() when caller(UI)
 *                               get user keyin.
 * OUTPUT  : need_wait_key_p
 * RETURN  : None.
 * NOTE    : None.
 *----------------------------------------------------------------*/
void L_IPCIO_ParseKey(UI32_T msgq_handle, char ch, BOOL_T *need_wait_key_p)
{
    static UI16_T current_len = 0;

    SYSFUN_Msg_T *msg_p = (SYSFUN_Msg_T *)l_ipcio_msg_buf;
    L_IPCIO_Msg_T *l_ipcio_msg_p = (L_IPCIO_Msg_T *)msg_p->msg_buf;

    if (need_wait_key_p == NULL || *need_wait_key_p == FALSE)
        return;

    switch (l_ipcio_msg_p->ipcio_cmd)
    {
        case L_IPCIO_CMD_GETCHAR:
            l_ipcio_msg_p->ipcio_arg_ch = (char)ch;
            msg_p->msg_size = L_IPCIO_SIZE_OF_MSG(sizeof(l_ipcio_msg_p->ipcio_arg_ch));
            SYSFUN_SendResponseMsg(msgq_handle, msg_p);
            *need_wait_key_p = FALSE;
            break;

        case L_IPCIO_CMD_GETLINE:
            if (l_ipcio_wait_size > 0 && isprint(ch))
            {
                l_ipcio_msg_p->ipcio_arg_buffer[current_len++] = (char)ch;
                //l_ipcio_msg_p->ipcio_arg_buffer[current_len] = '\0';
                //print_func(&l_ipcio_msg_p->ipcio_arg_buffer[current_len-1]);
                l_ipcio_wait_size--;
            }
            if (ch == '\n' || ch == '\r')
            {
                //print_func("\r\n");
                l_ipcio_msg_p->ipcio_arg_buffer[current_len++] = '\0';
                msg_p->msg_size = L_IPCIO_SIZE_OF_MSG(current_len);
                SYSFUN_SendResponseMsg(msgq_handle, msg_p);
                current_len = 0;
                *need_wait_key_p = FALSE;
            }
            break;
    }
}
#endif /* L_IPCIO_UNUSED */

/* LOCAL SUBPROGRAM SPECIFICATIONS
 */
/*-----------------------------------------------------------------
 * ROUTINE NAME - L_IPCIO_DumpVAList
 *-----------------------------------------------------------------
 * FUNCTION: convert fmt_p and arg_p to a string, also allocate
 *           enough space to store the string.
 * INPUT   : fmt_p
 *           arg_p
 * OUTPUT  : len_p - len of the string.
 * RETURN  : a pointer to string buffer
 * NOTE    : caller need to free the space via free().
 *----------------------------------------------------------------*/
static void *L_IPCIO_DumpVAList(const char *fmt_p, va_list arg_p, int *len_p)
{
    char *buf;
    int len;

    len = vsnprintf(0, 0, fmt_p, arg_p);

    if (len < 0)
    {
        printf("\r\n%s: vsnprintf() is failed.", __FUNCTION__);
        return NULL;
    }

    if ((buf = malloc(len + 1)) == NULL)
    {
        printf("\r\n%s: out of memory.", __FUNCTION__);
        return NULL;
    }

    vsnprintf(buf, len + 1, fmt_p, arg_p);

    *len_p = len;

    return buf;
}

/*-----------------------------------------------------------------
 * ROUTINE NAME - L_IPCIO_PrintBuffer
 *-----------------------------------------------------------------
 * FUNCTION: This function will request remote task to print string
 *           from specified buffer via IPC.
 * INPUT   : msgq_handle  - This is used to obtain IPC resource.
 *           wait_time    - The waiting time if the queue is full.
 *           buf          - buffer to print (not includes null terminal)
 *           buf_len      - length of buffer
 * OUTPUT  : None.
 * RETURN  : L_IPCIO_ERR_NONE  - successful.
 *           L_IPCIO_ERR_ARG   - invalid parameter.
 *           L_IPCIO_ERR_IPC   - IPC operation is failed.
 *           L_IPCIO_ERR_ALLOC - out of memory.
 * NOTE    : None.
 *----------------------------------------------------------------*/
static UI32_T L_IPCIO_PrintBuffer(UI32_T msgq_handle, UI32_T wait_time, UI32_T buf_len, const void *buf)
{
    SYSFUN_Msg_T *msg_p;
    L_IPCIO_Msg_T *ipcio_msg_p;
    UI32_T len, remain_len;
    const char *remain_buf;

    if (buf_len < L_IPCIO_MAX_LENGTH_FOR_SEND_PRINT)
        msg_p = L_IPCIO_Alloc(1 + buf_len);
    else
        msg_p = L_IPCIO_Alloc(1 + L_IPCIO_MAX_LENGTH_FOR_SEND_PRINT);

    if (msg_p == NULL)
    {
        printf("\r\n%s: out of memory.", __FUNCTION__);
        return L_IPCIO_ERR_ALLOC;
    }

    /* Send asynchronous request.
     */
    msg_p->cmd = SYS_TYPE_CMD_IPCIO;
    ipcio_msg_p = (L_IPCIO_Msg_T *)msg_p->msg_buf;
    ipcio_msg_p->ipcio_cmd = L_IPCIO_CMD_PRINT;

    for (remain_buf = buf, remain_len = buf_len;
        remain_len > 0;
        remain_buf += len, remain_len -= len)
    {
        len = remain_len < L_IPCIO_MAX_LENGTH_FOR_SEND_PRINT ?
              remain_len : L_IPCIO_MAX_LENGTH_FOR_SEND_PRINT;

        msg_p->msg_size = L_IPCIO_SIZE_OF_MSG(len + 1);
        memcpy(ipcio_msg_p->ipcio_arg_buffer, remain_buf, len);
        ipcio_msg_p->ipcio_arg_buffer[len] = 0;

        if (SYSFUN_SendRequestMsg(msgq_handle,
                                  msg_p,
                                  wait_time,
                                  SYSFUN_SYSTEM_EVENT_IPCMSG,
                                  0,
                                  NULL) != SYSFUN_OK)
        {
            printf("\r\n%s: fail to send async. request.", __FUNCTION__);
            L_IPCIO_Free(msg_p);
        	return L_IPCIO_ERR_IPC;
        }
    }

    L_IPCIO_Free(msg_p);
    return L_IPCIO_ERR_NONE;
}



/* For Debug
 */
#if (L_IPCIO_UNIT_TEST == TRUE)

#endif /* L_IPCIO_UNIT_TEST */

