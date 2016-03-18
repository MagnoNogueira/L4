/*-----------------------------------------------------------------------------
 * Module   : TIME_RANGE_PMGR.c
 *-----------------------------------------------------------------------------
 * PURPOSE  : TIME_RANGE_PMGR provides all TIME_RANGE accessing functions,
 *            including Create, Delete, and GetNext.
 *
 *-----------------------------------------------------------------------------
 * HISTORY  : 2010.02 - Nelson Dai, Create
 *
 *
 *-----------------------------------------------------------------------------
 * Copyright(C)                               Accton Corporation, 2010
 *-----------------------------------------------------------------------------
 */

/* INCLUDE FILE DECLARATIONS
 */
#include "string.h"
#include "stdio.h"
#include "sysfun.h"
#include "sys_bld.h"
#include "sys_module.h"
#include "time_range_type.h"
#include "time_range_mgr.h"
#include "time_range_pmgr.h"

/* STATIC VARIABLE DECLARATIONS
 */
static UI32_T ipcmsgq_handle;

/*------------------------------------------------------------------------------
 * ROUTINE NAME - STRNCPY
 *------------------------------------------------------------------------------
 * PURPOSE:  Copy strings
 * INPUT:    src    - src string
 *           cch    - count of characters of dst string buffer(include NULL
 *                    terminate)
 * OUTPUT:   dst    - dst string
 * RETURN:   Error code
 * NOTE:     None
 *------------------------------------------------------------------------------
 */
static BOOL_T
STRNCPY(
     char *dst,
     const char *src,
     UI32_T cch)
{
    dst[cch-1] = '\0';
    strncpy(dst, src, cch);

    if (dst[cch-1] != '\0')
    {
        dst[cch-1] = '\0';
        return FALSE;
    }

    return TRUE;
}


/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_InitiateProcessResource
 *
 * PURPOSE:
 *    Initiate resource used in the calling process, means the process that use
 *    this pmgr functions should call this init.
 *
 * INPUT:
 *    None.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    TRUE  --  Success
 *    FALSE --  Error
 *
 * NOTES:
 *    None.
 */
BOOL_T TIME_RANGE_PMGR_InitiateProcessResource(void)
{
    if (SYSFUN_GetMsgQ(SYS_BLD_UTILITY_GROUP_IPCMSGQ_KEY, SYSFUN_MSGQ_BIDIRECTIONAL, &ipcmsgq_handle) != SYSFUN_OK)
    {
        printf("%s(): SYSFUN_GetMsgQ fail.\n", __FUNCTION__);
        return FALSE;
    }

// ezio
#if 1
    SYSFUN_RegisterIPCHandler(ipcmsgq_handle,
                              TIME_RANGE_MGR_HandleIPCReqMsg);
#endif

    return TRUE;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_IsTimeRangeEntryValidByName
 *
 * PURPOSE:
 *    check if the time-range entry is valid by name
 *
 * INPUT:
 *    name: the name of the entry.
 *
 * OUTPUT:
 *    none
 *
 * RETURN:
 *    TIME_RANGE_ERROR_TYPE_NONE: The time-range entry is valid
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_IsTimeRangeEntryValidByName(UI8_T *name)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(name);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (name == NULL)
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_IS_TIME_RANGE_ENTRY_VALID_BY_NAME;
    memset(msg_p->data.name, 0, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1);
    strncpy((char *) msg_p->data.name, (char *) name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
	
    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_GetTimeRangeEntryByName
 *
 * PURPOSE:
 *    Get time_range_entry according to the input name.
 *
 * INPUT:
 *    name: the name of the entry.
 *
 * OUTPUT:
 *    entry: the entry you are looking for.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_GetTimeRangeEntryByName (UI8_T *name, TIME_RANGE_TYPE_ENTRY_T *entry)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(entry);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if ((name == NULL) || (entry == NULL))
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_GET_TIME_RANGE_ENTRY_BY_NAME;
    memset(msg_p->data.entry.name, 0, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1);
    strncpy((char *) msg_p->data.entry.name, (char *) name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    /* copy back */
    memcpy(entry, &(msg_p->data.entry.entry), sizeof(TIME_RANGE_TYPE_ENTRY_T));

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_CreateTimeRangeEntry
 *
 * PURPOSE:
 *    Create a time_range_entry with specific name.
 *
 * INPUT:
 *    name: the name of the new entry you want to create.
 *
 * OUTPUT:
 *    None
 *
 * RETURN   : 
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_CreateTimeRangeEntry(UI8_T *name)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(entry);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (name == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_CREATE_TIME_RANGE_ENTRY;
    if (TRUE != STRNCPY((char *) msg_p->data.entry.name, (char *) name, 
		                                    SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1))
    {
        return TIME_RANGE_ERROR_TYPE_OUT_OF_NAME_LENGTH;
    }


    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_SetTimeRangeAbsolute
 *
 * PURPOSE:
 *    Set absolute time range for specific entry.
 *
 * INPUT:
 *    name:  name of the time_range_entry.
 *    start: start of the absolute time.
 *    end:   end of the absolute time.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_SetTimeRangeAbsolute (UI8_T *name,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(absolute);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if ((name == NULL) || ((start == NULL) && (end == NULL)))
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_SET_TIME_RANGE_ABSOLUTE;
    memset(msg_p->data.absolute.name, 0, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1);
    strncpy((char *) msg_p->data.absolute.name, (char *) name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
    if (start != NULL)
    {
        memcpy(&(msg_p->data.absolute.start), start, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));
        msg_p->data.absolute.is_start_null = FALSE;
    }
    else
        msg_p->data.absolute.is_start_null = TRUE;
    if (end != NULL)
    {
        memcpy(&(msg_p->data.absolute.end),   end,   sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));
        msg_p->data.absolute.is_end_null = FALSE;
    }
    else
        msg_p->data.absolute.is_end_null = TRUE;

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_SetTimeRangePeriodic
 *
 * PURPOSE:
 *    Set periodic time range for specific entry.
 *
 * INPUT:
 *    name : name of the time_range_entry.
 *    entry: periodic time_range for the entry.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_SetTimeRangePeriodic (UI8_T *name,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(periodic);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if ((name == NULL) || (entry == NULL))
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_SET_TIME_RANGE_PERIODIC;
    memset(msg_p->data.periodic.name, 0, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1);
    strncpy((char *) msg_p->data.periodic.name, (char *) name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
    memcpy(&(msg_p->data.periodic.entry), entry, sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_DeleteTimeRangeEntry
 *
 * PURPOSE:
 *    Delete a time_range_entry with specific name.
 *
 * INPUT:
 *    name
 *
 * OUTPUT:
 *    None
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    1. The entry can not be deleted if other CSCs are bound to it.
 *    2. User can delete the entry without destroying absolute/periodic time range first.
 *
 * HISTORY:
 *   mm/dd/yy
 *   04/19/13 -- Add member err_type, please refer struct TIME_RANGE_ERROR_TYPE_T.
 */
UI32_T TIME_RANGE_PMGR_DeleteTimeRangeEntry(UI8_T *name)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(entry);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;


    if (name == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_DELETE_TIME_RANGE_ENTRY;
    memset(msg_p->data.entry.name, 0, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1);
    strncpy((char *) msg_p->data.entry.name, (char *) name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_DestroyTimeRangeAbsolute
 *
 * PURPOSE:
 *    Destroy absolute time range for specific entry.
 *
 * INPUT:
 *    name: name of the time_range_entry.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    It can not be deleted if other CSCs are bound to the entry.
 */
UI32_T TIME_RANGE_PMGR_DestroyTimeRangeAbsolute (UI8_T *name)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(name);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (name == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_DESTROY_TIME_RANGE_ABSOLUTE;
    memset(msg_p->data.name, 0, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1);
    strncpy((char *) msg_p->data.name, (char *) name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_DestroyTimeRangePeriodic
 *
 * PURPOSE:
 *    Destroy periodic time range for specific entry.
 *
 * INPUT:
 *    name : name of the time_range_entry.
 *    entry: periodic time_range for the entry.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    It can not be deleted if other CSCs are bound to the entry.
 */
UI32_T TIME_RANGE_PMGR_DestroyTimeRangePeriodic (UI8_T *name,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(periodic);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if ((name == NULL) || (entry == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_DESTROY_TIME_RANGE_PERIODIC;
    memset(msg_p->data.periodic.name, 0, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1);
    strncpy((char *) msg_p->data.periodic.name, (char *) name, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH);
    memcpy(&(msg_p->data.periodic.entry), entry, sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_GetNextTimeRangeEntry
 *
 * PURPOSE:
 *    Get next time_range_entry according to current index.
 *
 * INPUT:
 *    index: index for current time_range_entry
 *
 * OUTPUT:
 *    index: index for next time_range_entry
 *    entry: next time_range_entry
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    If index == TIME_RANGE_TYPE_UNDEF_TIME_RANGE,
 *    first entry and first index will be returned.
 */
UI32_T TIME_RANGE_PMGR_GetNextTimeRangeEntry (UI32_T *index,
    TIME_RANGE_TYPE_ENTRY_T *entry)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(next);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if ((index == NULL) || (entry == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((*index != TIME_RANGE_TYPE_UNDEF_TIME_RANGE) && (*index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_GET_NEXT_TIME_RANGE_ENTRY;
    msg_p->data.next.index = *index;

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    /* copy back */
    *index = msg_p->data.next.index;
    memset(entry, 0, sizeof(TIME_RANGE_TYPE_ENTRY_T));
    memcpy(entry, &(msg_p->data.next.entry), sizeof(TIME_RANGE_TYPE_ENTRY_T));

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_GetNextRunningTimeRangeEntry
 *
 * PURPOSE:
 *    Get next time_range_entry according to current index.
 *
 * INPUT:
 *    index: index for current time_range_entry
 *
 * OUTPUT:
 *    index: index for next time_range_entry
 *    entry: next time_range_entry
 *
 * RETURN:
 *    SYS_TYPE_GET_RUNNING_CFG_SUCCESS  --  Success
 *    SYS_TYPE_GET_RUNNING_CFG_FAIL --  Error
 *
 * NOTES:
 *    If index == TIME_RANGE_TYPE_UNDEF_TIME_RANGE,
 *    first entry and first index will be returned.
 */
SYS_TYPE_Get_Running_Cfg_T TIME_RANGE_PMGR_GetNextRunningTimeRangeEntry (UI32_T *index,
    TIME_RANGE_TYPE_ENTRY_T *entry)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(next);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if ((index == NULL) || (entry == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((*index != TIME_RANGE_TYPE_UNDEF_TIME_RANGE) && (*index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_GET_NEXT_RUNNING_TIME_RANGE_ENTRY;
    msg_p->data.next.index = *index;

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    /* copy back */
    *index = msg_p->data.next.index;
    memset(entry, 0, sizeof(TIME_RANGE_TYPE_ENTRY_T));
    memcpy(entry, &(msg_p->data.next.entry), sizeof(TIME_RANGE_TYPE_ENTRY_T));

    return msg_p->type.result_running_cfg;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_RequestTimeRangeEntry
 *
 * PURPOSE:
 *    Bind specific entry to other CSC,
 *    or unbind specific entry from other CSC.
 *
 * INPUT:
 *    name: name of the time_range_entry.
 *    bind:  TRUE : bind the entry to CSC.
 *             FALSE: unbind the entry from CSC.
 *
 * OUTPUT:
 *    index: index of the entry.
 *
 * RETURN:
 *    TRUE  --  Success
 *    FALSE --  Error
 *
 * NOTES:     bind is like lock the time-range entry, must be executed first
 *                 unbind is like unlock the time-range entry, must be executed last
 */
UI32_T TIME_RANGE_PMGR_RequestTimeRangeEntry (UI8_T *name, BOOL_T bind, UI32_T *index)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(request);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if ((name == NULL) || (index == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_REQUEST_TIME_RANGE_ENTRY;
    memset(msg_p->data.request.name, 0, SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1);
    strcpy((char *)msg_p->data.request.name, (char *)name);
    msg_p->data.request.bind = bind;

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    /* copy back */
    *index = msg_p->data.request.index;

    return msg_p->type.result_ui32;
}

/***************************************
 * These APIs below are for SNMP only. *
 ***************************************/

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibCreateEmptyTimeRangeEntryById
 *
 * PURPOSE:
 *    Create an empty time_range_entry with specific index
 *
 * INPUT:
 *    index: index of the time_range_entry
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibCreateEmptyTimeRangeEntryById (UI32_T index)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(index);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_MIB_CREATE_EMPTY_TIME_RANGE_ENTRY_BY_ID;
    msg_p->data.index = index;

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibSetTimeRangeEntryNameById
 *
 * PURPOSE:
 *    Set time_range_entry name with specific index
 *
 * INPUT:
 *    index: index of the time_range_entry
 *    name : name of the time_range_entry
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibSetTimeRangeEntryNameById (UI32_T index, UI8_T *name)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(entry);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (name == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_MIB_SET_TIME_RANGE_ENTRY_NAME_BY_ID;
    msg_p->data.entry.index = index;
    if (TRUE != STRNCPY((char *) msg_p->data.entry.name, (char *) name, 
		                                    SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1))
    {
        return TIME_RANGE_ERROR_TYPE_OUT_OF_NAME_LENGTH;
    }


    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibDeleteTimeRangeEntryById
 *
 * PURPOSE:
 *    Delete a time-range entry with specific time-range index
 *
 * INPUT:
 *    index   -- index of the time-range
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error Code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibDeleteTimeRangeEntryById (UI32_T index)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(index);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_MIB_DELETE_TIME_RANGE_ENTRY_BY_ID;
    msg_p->data.index = index;

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}


/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibSetTimeRangePeriodicById
 *
 * PURPOSE:
 *    Set periodic_entry with time-range index
 *
 * INPUT:
 *    index   -- index of the time-range
 *    entry   -- periodic_entry
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibSetTimeRangePeriodicById (UI32_T index,
    TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(periodic_id);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (entry == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_MIB_SET_TIME_RANGE_PERIODIC_BY_ID;
    msg_p->data.periodic_id.index = index;
    memcpy(&(msg_p->data.periodic_id.entry), entry, sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibDestroyTimeRangePeriodicById
 *
 * PURPOSE:
 *    Destroy a periodic_entry with specific time-range index
 *
 * INPUT:
 *    index   -- index of the time-range
 *    entry   -- periodic_entry
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibDestroyTimeRangePeriodicById (UI32_T index, TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(periodic_id);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (entry == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_MIB_DESTROY_TIME_RANGE_PERIODIC_BY_ID;
    msg_p->data.periodic_id.index = index;
    memcpy(&(msg_p->data.periodic_id.entry), entry, sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *	TIME_RANGE_PMGR_MibSetTimeRangeAbsoluteById
 *
 * PURPOSE:
 *    Set absolute entry for specific time-range index.
 *
 * INPUT: 
 *    index  --  index of the time-range.
 *    start  --  start of the absolute time.
 *    end   --  end of the absolute time.
 *
 * OUTPUT: 
 *    None.
 *
 * RETURN: 
 *    Error code
 *
 * NOTES: 
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibSetTimeRangeAbsoluteById (UI32_T index,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(absolute_id);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((start == NULL) || (end == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_MIB_SET_TIME_RANGE_ABSOLUTE_BY_ID;
    msg_p->data.absolute_id.index = index;
    memcpy(&(msg_p->data.absolute_id.start), start, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));
    memcpy(&(msg_p->data.absolute_id.end), end, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *	TIME_RANGE_PMGR_MibDestroyTimeRangeAbsoluteById
 *
 * PURPOSE:
 *    Destroy absolute entry for specific time-range index.
 *
 * INPUT: 
 *    index  --  index of the time-range.
 *    start  --  start of the absolute time.
 *    end   --  end of the absolute time.
 *
 * OUTPUT: 
 *    None.
 *
 * RETURN: 
 *    Error code
 *
 * NOTES: 
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibDestroyTimeRangeAbsoluteById (UI32_T index,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
    TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(absolute_id);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((start == NULL) || (end == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_MIB_DESTROY_TIME_RANGE_ABSOLUTE_BY_ID;
    msg_p->data.absolute_id.index = index;
    memcpy(&(msg_p->data.absolute_id.start), start, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));
    memcpy(&(msg_p->data.absolute_id.end), end, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibGetTimeRangeEntryById
 *
 * PURPOSE:
 *    Get time_range_entry according to the input index.
 *
 * INPUT:
 *    name: the index of the entry.
 *
 * OUTPUT:
 *    entry: the entry you are looking for.
 *
 * RETURN:
 *    Error code
 *
 * NOTES:
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibGetTimeRangeEntryById (UI32_T index, TIME_RANGE_TYPE_ENTRY_T *entry)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(entry);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (entry == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_MIB_GET_TIME_RANGE_ENTRY_BY_ID;
    msg_p->data.entry.index = index;

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    /* copy back */
    memcpy(entry, &(msg_p->data.entry.entry), sizeof(TIME_RANGE_TYPE_ENTRY_T));

    return msg_p->type.result_ui32;
}


/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibGetNextTimeRangePeriodicEntry
 *
 * PURPOSE: 
 *    Get next periodic_entry
 *
 * INPUT: 
 *    index   -- index of the time-range
 *    entry   -- periodic_entry
 *
 * OUTPUT: 
 *    None.
 *
 * RETURN: 
 *    Error code
 *
 * NOTES: 
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibGetNextTimeRangePeriodicEntry(UI32_T *index, TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(periodic_id);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if ((index == NULL) || (entry == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((*index != TIME_RANGE_TYPE_UNDEF_TIME_RANGE) && (*index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_MIB_GET_NEXT_TIME_RANGE_PERIODIC_ENTRY;
    msg_p->data.periodic_id.index = *index;
    memcpy(&(msg_p->data.periodic_id.entry), entry, sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));	

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    /* copy back */
   *index = msg_p->data.periodic_id.index;
    memcpy(entry, &(msg_p->data.periodic_id.entry), sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME:
 *    TIME_RANGE_PMGR_MibGetTimeRangePeriodicEntry
 *
 * PURPOSE: 
 *    Get periodic-entry
 *
 * INPUT: 
 *    index   -- index of the time-range
 *    entry   -- periodic-entry
 *
 * OUTPUT: 
 *    None.
 *
 * RETURN: 
 *    TIME_RANGE_ERROR_TYPE_INVALID_DATA - Success (The periodic-entry exist)
 *    Error code
 *
 * NOTES: 
 *    None.
 */
UI32_T TIME_RANGE_PMGR_MibGetTimeRangePeriodicEntry(UI32_T index, TIME_RANGE_TYPE_PERIODIC_TIME_T *entry)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(periodic_id);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if (entry == NULL)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_MIB_GET_TIME_RANGE_PERIODIC_ENTRY;
    msg_p->data.periodic_id.index = index;
    memcpy(&(msg_p->data.periodic_id.entry), entry, sizeof(TIME_RANGE_TYPE_PERIODIC_TIME_T));

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    /* copy back */

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME: 
 *    TIME_RANGE_PMGR_MibGetNextTimeRangeAbsoluteEntry
 *
 * PURPOSE: 
 *    Get next absolute-entry
 *
 * INPUT: 
 *    index   -- index of the time-range
 *    start  --  start of the absolute time.
 *    end   --  end of the absolute time.
 *
 * OUTPUT: 
 *    None.
 *
 * RETURN: 
 *    TIME_RANGE_ERROR_TYPE_INVALID_DATA - Success (The absolute entry exist)
 *    Error code
 *
 * NOTES: 
 *    The MIB OID of undefined absolute-entry must be 65535.255.255.255.255
 */
UI32_T TIME_RANGE_PMGR_MibGetTimeRangeAbsoluteEntry(
	UI32_T index, 
	TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
	TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(absolute_id);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if (index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY)
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((start == NULL) || (end == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_MIB_GET_TIME_RANGE_ABSOLUTE_ENTRY;
    msg_p->data.absolute_id.index = index;
    memcpy(&(msg_p->data.absolute_id.start), start, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));
    memcpy(&(msg_p->data.absolute_id.end), end, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    /* copy back */

    return msg_p->type.result_ui32;
}

/* FUNCTION NAME: 
 *    TIME_RANGE_PMGR_MibGetNextTimeRangeAbsoluteEntry
 *
 * PURPOSE: 
 *    Get next absolute-entry
 *
 * INPUT: 
 *    index   -- index of the time-range
 *    start  --  start of the absolute time.
 *    end   --  end of the absolute time.
 *
 * OUTPUT: 
 *    None.
 *
 * RETURN: 
 *    Error code
 *
 * NOTES: 
 *    The MIB OID of undefined absolute-entry must be 65535.255.255.255.255
 */
UI32_T TIME_RANGE_PMGR_MibGetNextTimeRangeAbsoluteEntry(
	UI32_T *index, 
	TIME_RANGE_TYPE_ABSOLUTE_TIME_T *start,
	TIME_RANGE_TYPE_ABSOLUTE_TIME_T *end)
{
    TIME_RANGE_MGR_IPCMsg_T *msg_p = NULL;
    UI32_T msg_size = TIME_RANGE_MGR_GET_MSG_SIZE(absolute_id);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;

    if ((index == NULL) || (start == NULL) || (end == NULL))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    if ((*index != TIME_RANGE_TYPE_UNDEF_TIME_RANGE) && (*index >= SYS_ADPT_TIME_RANGE_MAX_NBR_OF_ENTRY))
    {
        return TIME_RANGE_ERROR_TYPE_INVALID_DATA;
    }

    msgbuf_p->cmd = SYS_MODULE_TIME_RANGE;
    msgbuf_p->msg_size = msg_size;

    msg_p = (TIME_RANGE_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    msg_p->type.cmd = TIME_RANGE_MGR_IPCCMD_MIB_GET_NEXT_TIME_RANGE_ABSOLUTE_ENTRY;
    msg_p->data.absolute_id.index = *index;
    memcpy(&(msg_p->data.absolute_id.start), start, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));
    memcpy(&(msg_p->data.absolute_id.end), end, sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));

    if (SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
                              SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
                              msg_size, msgbuf_p) != SYSFUN_OK)
    {
        return TIME_RANGE_ERROR_TYPE_FAIL;
    }

    /* copy back */
    *index = msg_p->data.absolute_id.index;
    memcpy(start, &(msg_p->data.absolute_id.start), sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));
    memcpy(end, &(msg_p->data.absolute_id.end), sizeof(TIME_RANGE_TYPE_ABSOLUTE_TIME_T));

    return msg_p->type.result_ui32;
}


