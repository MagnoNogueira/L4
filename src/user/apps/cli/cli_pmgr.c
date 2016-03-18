/* MODULE NAME:  cli_pmgr.c
 * PURPOSE:
 *    This is PMGR for CLI.
 *
 * NOTES:
 *
 * REASON:
 * Description:
 * HISTORY
 *    5/20/2007 - Rich Lee, Created
 *
 * Copyright(C)      Accton Corporation, 2007
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
//#include "l_ipcmsgq.h"
#include "cli_mgr.h"
#include "cli_pmgr.h"
#include "cli_type.h"
#include "sys_module.h"
#include "l_mm.h"
#include "sys_bld.h"
#include "sysfun.h"
#include "buffer_mgr.h"
/* NAMING CONSTANT DECLARATIONS
 */
/* trace id definition when using L_MM
 */
enum
{
    CLI_PMGR_TRACEID_IPCCMD1,
};

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */

/* STATIC VARIABLE DECLARATIONS
 */
static UI32_T ipcmsgq_handle;

/* EXPORTED SUBPROGRAM BODIES
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_PMGR_InitiateProcessResource
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Initiate resource used in the calling process.
 * INPUT:
 *    None.
 *
 * OUTPUT:
 *    None.
 *
 * RETURN:
 *    TRUE  --  Sucess
 *    FALSE --  Error
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T CLI_PMGR_InitiateProcessResource(void)
{
    /* Given that CLI PMGR requests are handled in CLIGroup1 of CLI_PROC
     */
    if(SYSFUN_GetMsgQ(SYS_BLD_CLI_GROUP_IPCMSGQ_KEY, SYSFUN_MSGQ_BIDIRECTIONAL, &ipcmsgq_handle)!=SYSFUN_OK)
    {
        SYSFUN_Debug_Printf("%s(): SYSFUN_GetMsgQ fail.\n", __FUNCTION__);
        return FALSE;
    }

    return TRUE;
}


/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_PMGR_Set_StkctrlSavingConfigStatus
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will be registered to STKCTRL task, STKCTRL
 *            will callback the status of saving config by this function.
 * INPUT    : status - the status of saving config
 *            TRUE: complete, FALSE: not complete
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
void CLI_PMGR_Set_StkctrlSavingConfigStatus(BOOL_T status)
{
    const UI32_T msg_size = CLI_MGR_GET_MSGBUFSIZE(config_status_data);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_MGR_IPCMsg_T  *data_p;
    UI32_T             result;

    /* msgbuf_p will be used on both request and response
     * msg_size shall be max(req_buf_size, resp_buf_size)
     */
    msgbuf_p->cmd = SYS_MODULE_CLI;

    data_p = (CLI_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    data_p->type.cmd = CLI_MGR_IPC_SAVING_CONFIG_STATUS;
    msgbuf_p->msg_size = msg_size;

    if(SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, CLI_MGR_MSGBUF_TYPE_SIZE, msgbuf_p)!=SYSFUN_OK)
    {
        return ;
    }
    result = data_p->type.result;

    return ;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_PMGR_AllCscsKnowProvisionComplete
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This api will call CLI_MGR_AllCscsKnowProvisionComplete through the IPC msgq.
 * INPUT:
 *    None
 *
 * OUTPUT:
 *    *state --  current ssh status.
 *
 * RETURN:
 *    TRUE   -- Success
 *    FALSE -- Fail
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
void CLI_PMGR_AllCscsKnowProvisionComplete(void)
{
    const UI32_T msg_size = CLI_MGR_GET_MSGBUFSIZE(config_status_data);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_MGR_IPCMsg_T  *data_p;
    UI32_T             result, ret;
//printf(" %s %d\n", __FUNCTION__, __LINE__);
    /* msgbuf_p will be used on both request and response
     * msg_size shall be max(req_buf_size, resp_buf_size)
     */
    msgbuf_p->cmd = SYS_MODULE_CLI;

    data_p = (CLI_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    data_p->type.cmd = CLI_MGR_IPC_ALLCSCSKNOWPROVISIONCOMPLETE;
    msgbuf_p->msg_size = msg_size;
//printf(" %s %d\n", __FUNCTION__, __LINE__);

    ret =SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, CLI_MGR_MSGBUF_TYPE_SIZE, msgbuf_p);
    if(ret !=SYSFUN_OK)
    {
        SYSFUN_Debug_Printf(" %s %d\n", __FUNCTION__, __LINE__);
        return ;
    }
    result = data_p->type.result;
//    printf(" %s %d\n", __FUNCTION__, __LINE__);
    return ;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_PMGR_AllCscsKnowProvisionComplete
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This api will call CLI_MGR_AllCscsKnowProvisionComplete through the IPC msgq.
 * INPUT:
 *    None
 *
 * OUTPUT:
 *    *state --  current ssh status.
 *
 * RETURN:
 *    TRUE   -- Success
 *    FALSE -- Fail
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
void CLI_PMGR_Notify_EnterTransitionMode(void *prov_buf)
{
    const UI32_T msg_size = CLI_MGR_GET_MSGBUFSIZE(running_config_data);
    UI8_T ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_MGR_IPCMsg_T  *data_p;
    UI32_T             result, ret;
    /* msgbuf_p will be used on both request and response
     * msg_size shall be max(req_buf_size, resp_buf_size)
     */
    msgbuf_p->cmd = SYS_MODULE_CLI;

    data_p = (CLI_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    data_p->type.cmd = CLI_MGR_IPC_ENTER_TRANSITION_MODE;
    data_p->data.running_config_data.buffer = BUFFER_MGR_GetOffset(prov_buf);
    msgbuf_p->msg_size = msg_size;

    ret =SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p, SYSFUN_TIMEOUT_NOWAIT,
        SYSFUN_SYSTEM_EVENT_IPCMSG, 0, NULL);
    if(ret !=SYSFUN_OK)
    {
        SYSFUN_Debug_Printf(" %s %d\n", __FUNCTION__, __LINE__);
        return ;
    }
    result = data_p->type.result;
    return ;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : CLI_PMGR_Get_RunningCfg
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    This api will call CLI_MGR_AllCscsKnowProvisionComplete through the IPC msgq.
 * INPUT:
 *    None
 *
 * OUTPUT:
 *    *state --  current ssh status.
 *
 * RETURN:
 *    TRUE   -- Success
 *    FALSE -- Fail
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
UI32_T CLI_PMGR_Get_RunningCfg(void *buffer, UI32_T buffer_size)
{
	const UI32_T msg_size = CLI_MGR_GET_MSGBUFSIZE(running_config_data);
    UI8_T               ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T        *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_MGR_IPCMsg_T    *data_p;
    UI32_T              ret;

    msgbuf_p->cmd = SYS_MODULE_CLI;

    data_p = (CLI_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    data_p->type.cmd = CLI_MGR_IPC_GET_RUNNING_CONFIG;
    data_p->data.running_config_data.buffer = BUFFER_MGR_GetOffset(buffer);
	data_p->data.running_config_data.buffer_size = buffer_size;
    msgbuf_p->msg_size = msg_size;

    ret =SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, CLI_MGR_MSGBUF_TYPE_SIZE, msgbuf_p);
    if(ret !=SYSFUN_OK)
    {
        return ret;
    }

    return  data_p->type.result;
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_PMGR_IncreaseRemoteSession
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will increase counter of remote seesion
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : BOOL_T : TRUE  -- success.
 *                     FLASE -- fail.
 * NOTE     : Maximux of total session is 4 include telnet and ssh
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_PMGR_IncreaseRemoteSession(void)
{
    const UI32_T msg_size = CLI_MGR_MSGBUF_TYPE_SIZE;
    UI8_T               ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T        *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_MGR_IPCMsg_T    *data_p;
    UI32_T              ret;


    msgbuf_p->cmd = SYS_MODULE_CLI;

    data_p = (CLI_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    data_p->type.cmd = CLI_MGR_IPC_INC_REMOTE_SESSION;
    msgbuf_p->msg_size = msg_size;

    ret =SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, CLI_MGR_MSGBUF_TYPE_SIZE, msgbuf_p);
    if(ret !=SYSFUN_OK)
    {
        return FALSE;
    }

    return (BOOL_T) data_p->type.result;
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_PMGR_DecreaseRemoteSession
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will decrease counter of remote seesion
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : BOOL_T : TRUE  -- success.
 *                     FLASE -- fail.
 * NOTE     : Maximux of total session is 4 include telnet and ssh
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_PMGR_DecreaseRemoteSession(void)
{
    const UI32_T msg_size = CLI_MGR_MSGBUF_TYPE_SIZE;
    UI8_T               ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T        *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_MGR_IPCMsg_T    *data_p;
    UI32_T              ret;


    msgbuf_p->cmd = SYS_MODULE_CLI;

    data_p = (CLI_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    data_p->type.cmd = CLI_MGR_IPC_DEC_REMOTE_SESSION;
    msgbuf_p->msg_size = msg_size;

    ret =SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, CLI_MGR_MSGBUF_TYPE_SIZE, msgbuf_p);
    if(ret !=SYSFUN_OK)
    {
        return FALSE;
    }

    return (BOOL_T) data_p->type.result;
}

/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_PMGR_SetKillWorkingSpaceFlag
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will set the kill_itself flag to TRUE for
 *              specified sessid.
 * INPUT    : sessid
 * OUTPUT   : None
 * RETURN   : BOOL_T : TRUE /FALSE
 * NOTE     : Maximux of total session is 4 include telnet and ssh
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_PMGR_SetKillWorkingSpaceFlag(UI32_T sessid)
{
    const UI32_T msg_size = CLI_MGR_GET_MSGBUFSIZE(session_data);
    UI8_T               ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T        *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_MGR_IPCMsg_T    *data_p;
    UI32_T              ret;

    msgbuf_p->cmd = SYS_MODULE_CLI;

    data_p = (CLI_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    data_p->type.cmd = CLI_MGR_IPC_SET_KILL_WORKING_SPACE_FLAG;
    data_p->data.session_data.session_id = sessid;
    msgbuf_p->msg_size = msg_size;

    ret =SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, CLI_MGR_MSGBUF_TYPE_SIZE, msgbuf_p);
    if(ret !=SYSFUN_OK)
    {
        return FALSE;
    }

    return (BOOL_T) data_p->type.result;
}

#if (SYS_CPNT_TELNET == TRUE)
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_PMGR_CloseAllTelnetSessions
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will close all telnet sessions.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_PMGR_CloseAllTelnetSessions()
{
    const UI32_T msg_size = CLI_MGR_GET_MSGBUFSIZE(session_data);
    UI8_T               ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T        *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_MGR_IPCMsg_T    *data_p;

    msgbuf_p->cmd = SYS_MODULE_CLI;

    data_p = (CLI_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    data_p->type.cmd = CLI_MGR_IPC_CLOSE_ALL_TELNET_SESSIONS;
    msgbuf_p->msg_size = msg_size;

    if (SYSFUN_OK != SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
        SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
        CLI_MGR_MSGBUF_TYPE_SIZE, msgbuf_p))
    {
        return FALSE;
    }

    return (BOOL_T) data_p->type.result;
}
#endif /* #if (SYS_CPNT_TELNET == TRUE) */

#if (SYS_CPNT_MGMT_IP_FLT == TRUE)
/* ------------------------------------------------------------------------
 * FUNCTION NAME - CLI_PMGR_HandleChangedIpMgmtFilter
 * ------------------------------------------------------------------------
 * PURPOSE  : This function will handle changed IP mgmt filter
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 * ------------------------------------------------------------------------
 */
BOOL_T CLI_PMGR_HandleChangedIpMgmtFilter()
{
    const UI32_T msg_size = CLI_MGR_GET_MSGBUFSIZE(session_data);
    UI8_T               ipc_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T        *msgbuf_p = (SYSFUN_Msg_T*)ipc_buf;
    CLI_MGR_IPCMsg_T    *data_p;

    msgbuf_p->cmd = SYS_MODULE_CLI;

    data_p = (CLI_MGR_IPCMsg_T*)msgbuf_p->msg_buf;
    data_p->type.cmd = CLI_MGR_IPC_HANDLE_CHANGED_IP_MGMT_FILTER;
    msgbuf_p->msg_size = msg_size;

    if (SYSFUN_OK != SYSFUN_SendRequestMsg(ipcmsgq_handle, msgbuf_p,
        SYSFUN_TIMEOUT_WAIT_FOREVER, SYSFUN_SYSTEM_EVENT_IPCMSG,
        CLI_MGR_MSGBUF_TYPE_SIZE, msgbuf_p))
    {
        return FALSE;
    }

    return (BOOL_T) data_p->type.result;
}
#endif /* #if (SYS_CPNT_MGMT_IP_FLT == TRUE) */

/* LOCAL SUBPROGRAM BODIES
 */

