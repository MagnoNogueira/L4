/*
 *  sysfun_utest.c
 *  sysfun
 *
 *  Created by yehjunying on 2011/4/14.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include <assert.h>
#include <stdio.h>
#include "sys_bld.h"
#include "sys_module.h"
#include "sysfun.c"

////////////////////////////////////////////////////////////////////////////////
// dev_rm_pmgr.h
enum
{
    DEVRM_PMGR_RESP_RESULT_FAIL,
};

#define DEVRM_PMGR_REQ_CMD_SIZE       sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->type)
#define DEVRM_PMGR_RESP_RESULT_SIZE   sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->type)

typedef struct
{
    union
    {
        UI32_T cmd;          /*cmd fnction id*/
        BOOL_T result_bool;  /*respond bool return*/
        UI32_T result_ui32;  /*respond ui32 return*/
        UI32_T result_i32;   /*respond i32 return*/
        UI64_T result_ui64;
    }type;

    union
    {
        union
        {
            struct
            {
                UI32_T device_id;
                UI32_T group_id;
                UI32_T vir_group_id;
				int    rule_pri;
            } req;
            struct
            {
                UI32_T dev_rule_id;
            } resp;
        } allocaterule;

        union
        {
            struct
            {
                char fmt[32];
                char string[20];
                int  integer;
            } req;
            struct
            {
                char string[100];
            } resp;
        } formatstring;

    }data;
} DEVRM_PMGR_IPCMSG_T;

enum
{
    DEVRM_IPCCMD_ALLOCATERULE,
    DEVRM_IPCCMD_FORMAT_STRING,
};
////////////////////////////////////////////////////////////////////////////////


static UI32_T ipcmsgq_handle;

void DEVRM_PMGR_InitiateProcessResource()
{
    if(SYSFUN_GetMsgQ(SYS_BLD_DRIVER_GROUP_IPCMSGQ_KEY,
                      SYSFUN_MSGQ_BIDIRECTIONAL, &ipcmsgq_handle)!=SYSFUN_OK)
    {
        printf("%s(): SYSFUN_GetMsgQ fail.\n", __FUNCTION__);
    }
}


/*--------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_AllocateRule
 *---------------------------------------------------------------------------
 * PURPOSE  : Allocate rule resource
 * INPUT    : device_id     - device ID
 *            group_id      - group ID
 *            vir_group_id  - virtual group ID
 *            rule_pri      - rule priority (no used)
 * OUTPUT   : dev_rule_id (per chip unique),
 *
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, call DEVRM_PMGR_CreateGroup() first
 *            2, MUST allocate rule resource before call DEVRM_PMGR_SetRule()
 *---------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_AllocateRule(UI32_T device_id,
                               UI32_T group_id,
                               UI32_T vir_group_id,
                               int rule_pri,
                               UI32_T *dev_rule_id)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.allocaterule.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.allocaterule.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;

    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEVRM_PMGR_IPCMSG_T *msg_data_p;

    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_RM;
    sysfun_msg_p->msg_size = req_msg_size;

    /* assign req cmd*/
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEVRM_IPCCMD_ALLOCATERULE;

    /* assgin req data */
    msg_data_p->data.allocaterule.req.device_id = device_id;
    msg_data_p->data.allocaterule.req.group_id = group_id;
    msg_data_p->data.allocaterule.req.vir_group_id = vir_group_id;
	msg_data_p->data.allocaterule.req.rule_pri = rule_pri;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
                                   SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    *dev_rule_id = msg_data_p->data.allocaterule.resp.dev_rule_id;

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

BOOL_T DEVRM_PMGR_FormatString(const char *fmt,
                               int integer,
                               const char *string,
                               char *outstring)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.formatstring.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.formatstring.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;

    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEVRM_PMGR_IPCMSG_T *msg_data_p;

    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_RM;
    sysfun_msg_p->msg_size = req_msg_size;

    /* assign req cmd*/
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEVRM_IPCCMD_FORMAT_STRING;

    /* assgin req data */
    strncpy(msg_data_p->data.formatstring.req.fmt, fmt,
            sizeof(msg_data_p->data.formatstring.req.fmt));
    if ('\0' != msg_data_p->data.formatstring.req.fmt[ sizeof(msg_data_p->data.formatstring.req.fmt)-1 ])
    {
        return FALSE;
    }

    msg_data_p->data.formatstring.req.integer = integer;

    strncpy(msg_data_p->data.formatstring.req.string, string,
            sizeof(msg_data_p->data.formatstring.req.string));
    if ('\0' != msg_data_p->data.formatstring.req.string[ sizeof(msg_data_p->data.formatstring.req.string)-1 ])
    {
        return FALSE;
    }

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
                                   SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    if (TRUE == msg_data_p->type.result_bool)
    {
        strcpy(outstring, msg_data_p->data.formatstring.resp.string);
    }

    return msg_data_p->type.result_bool;
}

// Test IPC. Use two buffer for req and resp respectively
BOOL_T DEVRM_PMGR_FormatString_2(const char *fmt,
                                 int integer,
                                 const char *string,
                                 char *outstring)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.formatstring.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.formatstring.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;

    UI8_T req_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)]; /* FOR IPC, the req buffer size MUST be the max size of req or resp buffer */
    UI8_T resp_msg_buf[SYSFUN_SIZE_OF_MSG(resp_msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p;
    DEVRM_PMGR_IPCMSG_T *msg_data_p;

    memset(req_msg_buf, 0, sizeof(req_msg_buf));
    memset(resp_msg_buf, 0, sizeof(resp_msg_buf));

    /* prepare req msg */
    sysfun_msg_p = (SYSFUN_Msg_T *)req_msg_buf;

    sysfun_msg_p->cmd = SYS_MODULE_DEV_RM;
    sysfun_msg_p->msg_size = req_msg_size;

    /* assign req cmd*/
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *) sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEVRM_IPCCMD_FORMAT_STRING;

    /* assgin req data */
    strncpy(msg_data_p->data.formatstring.req.fmt, fmt,
            sizeof(msg_data_p->data.formatstring.req.fmt));
    if ('\0' != msg_data_p->data.formatstring.req.fmt[ sizeof(msg_data_p->data.formatstring.req.fmt)-1 ])
    {
        return FALSE;
    }

    msg_data_p->data.formatstring.req.integer = integer;

    strncpy(msg_data_p->data.formatstring.req.string, string,
            sizeof(msg_data_p->data.formatstring.req.string));
    if ('\0' != msg_data_p->data.formatstring.req.string[ sizeof(msg_data_p->data.formatstring.req.string)-1 ])
    {
        return FALSE;
    }

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
                                   SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, (SYSFUN_Msg_T *)resp_msg_buf))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    sysfun_msg_p = (SYSFUN_Msg_T *)resp_msg_buf;

    msg_data_p = (DEVRM_PMGR_IPCMSG_T *) sysfun_msg_p->msg_buf;

    if (TRUE == msg_data_p->type.result_bool)
    {
        strcpy(outstring, msg_data_p->data.formatstring.resp.string);
    }

    return msg_data_p->type.result_bool;
}

/*--------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocateRule
 *---------------------------------------------------------------------------
 * PURPOSE  : Allocate rule resource
 * INPUT    : device_id     - device ID
 *            group_id      - group ID
 *            vir_group_id  - virtual group ID
 *            rule_pri      - rule priority (no used)
 * OUTPUT   : dev_rule_id (per chip unique),
 *
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, call DEVRM_CreateGroup() first
 *            2, MUST allocate rule resource before call DEVRM_SetRule()
 *---------------------------------------------------------------------------
 */
BOOL_T DEVRM_AllocateRule(UI32_T device_id,
                          UI32_T group_id,
                          UI32_T vir_group_id,
                          int rule_pri,
                          UI32_T *dev_rule_id)
{
    *dev_rule_id = 999;

    printf("%s(device_id=%lu, group_id=%lu, vir_group_id=%lu, rule_pri=%d)\n",
           __FUNCTION__, device_id, group_id, vir_group_id, rule_pri);

    *dev_rule_id = 999;

    return TRUE;
}

BOOL_T DEVRM_FormatString(const char *fmt,
                          int integer,
                          const char *string,
                          char *outstring)
{
    //outstring[0] = '\0';

    // fmt have be crack
    if (fmt[0] == '\0')
    {
        assert(0);
    }

    sprintf(outstring, fmt, integer, string);

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME : DEVRM_HandleIPCReqMsg
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Handle the ipc request message for dev_rm.
 * INPUT:
 *    ipcmsg_p  --  input request ipc message buffer
 *
 * OUTPUT:
 *    ipcmsg_p  --  output response ipc message buffer
 *
 * RETURN:
 *    TRUE  - There is a response need to send.
 *    FALSE - There is no response to send.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p)
{
    DEVRM_PMGR_IPCMSG_T   	*dev_rm_ipcmsg;

	/*
     Fix req and resp overlap issue
     */
#if 1
    DEVRM_PMGR_IPCMSG_T		req;
#endif

    if(ipcmsg_p==NULL)
        return FALSE;

    dev_rm_ipcmsg = (DEVRM_PMGR_IPCMSG_T*)ipcmsg_p->msg_buf;

#if 1
    memcpy(&req, ipcmsg_p->msg_buf, sizeof(req));
#endif

    switch(dev_rm_ipcmsg->type.cmd)
    {
        case DEVRM_IPCCMD_ALLOCATERULE:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.allocaterule.resp);
            dev_rm_ipcmsg->type.result_bool = DEVRM_AllocateRule(dev_rm_ipcmsg->data.allocaterule.req.device_id,
                                                                 dev_rm_ipcmsg->data.allocaterule.req.group_id,
                                                                 dev_rm_ipcmsg->data.allocaterule.req.vir_group_id,
                                                                 dev_rm_ipcmsg->data.allocaterule.req.rule_pri,
                                                                 &dev_rm_ipcmsg->data.allocaterule.resp.dev_rule_id
                                                                 );
            break;

        case DEVRM_IPCCMD_FORMAT_STRING:
            ipcmsg_p->msg_size=DEVRM_PMGR_RESP_RESULT_SIZE +
            sizeof(dev_rm_ipcmsg->data.formatstring.resp);
#if 1
            dev_rm_ipcmsg->type.result_bool = DEVRM_FormatString(req.data.formatstring.req.fmt,
                                                                 req.data.formatstring.req.integer,
                                                                 req.data.formatstring.req.string,
                                                                 dev_rm_ipcmsg->data.formatstring.resp.string);

#else
            dev_rm_ipcmsg->type.result_bool = DEVRM_FormatString(dev_rm_ipcmsg->data.formatstring.req.fmt,
                                                                 dev_rm_ipcmsg->data.formatstring.req.integer,
                                                                 dev_rm_ipcmsg->data.formatstring.req.string,
                                                                 dev_rm_ipcmsg->data.formatstring.resp.string);
#endif
            break;

        default:
            SYSFUN_Debug_Printf("%s(): Invalid cmd(%d).\n", __FUNCTION__, (int)(ipcmsg_p->cmd));
            dev_rm_ipcmsg->type.result_bool = DEVRM_PMGR_RESP_RESULT_FAIL;
            ipcmsg_p->msg_size = DEVRM_PMGR_RESP_RESULT_SIZE;
            break;
    }

    return TRUE;
}

static void testMessageQueueBasicFunction()
{
    SYSFUN_Init();

    {
        SYSFUN_MsgQ *msgq;
        SYSFUN_MsgQ *msgq_pool[SYSFUN_MAX_MSGQ_SIZE];
        UI32_T msgq_cnt;
        UI32_T i;

        msgq_cnt = 0;
        while (NULL != (msgq = SYSFUN_L_AllocateMessageQueue()))
        {
            assert(msgq_cnt < SYSFUN_MAX_MSGQ_SIZE);

            msgq_pool[msgq_cnt++] = msgq;
        }

        assert(msgq_cnt == SYSFUN_MAX_MSGQ_SIZE);

        for (i=0; i < SYSFUN_MAX_MSGQ_SIZE/2; i+=2)
        {
            msgq = msgq_pool[i];
            msgq_pool[i] = NULL;

            if (i+1 < SYSFUN_MAX_MSGQ_SIZE)
            {
                memmove(&msgq_pool[i], &msgq_pool[i+1], sizeof(msgq_pool[0]) * (msgq_cnt-(i+1)) );
                msgq_pool[msgq_cnt-1] = NULL;
            }

            --msgq_cnt;

            SYSFUN_L_DestroyMessageQueue(msgq);
        }

        while (NULL != (msgq = SYSFUN_L_AllocateMessageQueue()))
        {
            assert(msgq_cnt < SYSFUN_MAX_MSGQ_SIZE);

            msgq_pool[msgq_cnt++] = msgq;
        }

        assert(msgq_cnt == SYSFUN_MAX_MSGQ_SIZE);

        for (i=0; i < msgq_cnt; ++i)
        {
            SYSFUN_L_DestroyMessageQueue(msgq_pool[i]);
        }
    }


    SYSFUN_Init();
}

static void testMessageQueue()
{
    BOOL_T result;

    DEVRM_PMGR_InitiateProcessResource();
    SYSFUN_RegisterIPCHandler(ipcmsgq_handle, DEVRM_HandleIPCReqMsg);

    if (1)
    {
        UI32_T device_id    = 1;
        UI32_T group_id     = 2;
        UI32_T vir_group_id = 3;
        int rule_pri        = 4;
        UI32_T dev_rule_id;

        result = DEVRM_PMGR_AllocateRule(device_id, group_id, vir_group_id, rule_pri, &dev_rule_id);

        assert(999 == dev_rule_id);
    }

    {
        int integer = 99;
        char *fmt = "integer=%d, string=%s";
        char *string = "string";
        char outstring[100];

    	result = DEVRM_PMGR_FormatString(fmt, integer, string, outstring);

        {
            char outstring2[100];

            sprintf(outstring2, fmt, integer, string);

        	if (!(strcmp(outstring, outstring2) == 0))
            {
                assert(0);
            }
        }
    }

    {
        int integer = 999;
        char *fmt = "integer=%d, string=%s";
        char *string = "string";
        char outstring[100];

    	result = DEVRM_PMGR_FormatString_2(fmt, integer, string, outstring);

        {
            char outstring2[100];

            sprintf(outstring2, fmt, integer, string);

        	if (!(strcmp(outstring, outstring2) == 0))
            {
                assert(0);
            }
        }
    }
}

static void testMessageQueue_Private()
{
    BOOL_T result;

    SYSFUN_CreateMsgQ(SYSFUN_MSGQKEY_PRIVATE, SYSFUN_MSGQ_UNIDIRECTIONAL, &ipcmsgq_handle);
    SYSFUN_RegisterIPCHandler(ipcmsgq_handle, DEVRM_HandleIPCReqMsg);

    {
        UI32_T device_id    = 1;
        UI32_T group_id     = 2;
        UI32_T vir_group_id = 3;
        int rule_pri        = 4;
        UI32_T dev_rule_id;

        result = DEVRM_PMGR_AllocateRule(device_id, group_id, vir_group_id, rule_pri, &dev_rule_id);

        assert(TRUE == result);
        assert(999 == dev_rule_id);
    }

    SYSFUN_DestroyMsgQ(ipcmsgq_handle);

    {
        UI32_T device_id    = 1;
        UI32_T group_id     = 2;
        UI32_T vir_group_id = 3;
        int rule_pri        = 4;
        UI32_T dev_rule_id;

        result = DEVRM_PMGR_AllocateRule(device_id, group_id, vir_group_id, rule_pri, &dev_rule_id);

        assert(FALSE == result);
    }
}

static void testSemaphore()
{
    UI32_T sem_key = 1;
    UI32_T sem_flag = 0;
    UI32_T sem_id;
    UI32_T result;
    UI32_T count;

    count = 0;
    while (1)
    {
        result = SYSFUN_CreateSem(sem_key, 1, sem_flag, &sem_id);
        if (!(result == SYSFUN_OK))
        {
            break;
        }

        {
            UI32_T sem_id2;

            result = SYSFUN_GetSem(sem_key, &sem_id2);
            if (!(result == SYSFUN_OK &&
                  sem_id == sem_id2))
            {
                assert(0);
            }
        }

        if(!(sysfun_sem_array[sem_id].valid == TRUE &&
             sysfun_sem_array[sem_id].count == 1))
        {
            assert(0);
        }

        result = SYSFUN_TakeSem(sem_id, SYSFUN_TIMEOUT_WAIT_FOREVER);
        if (!(result == SYSFUN_OK &&
              sysfun_sem_array[sem_id].count == 0))
        {
            assert(0);
        }

        result = SYSFUN_GiveSem(sem_id);
        if (!(result == SYSFUN_OK &&
              sysfun_sem_array[sem_id].count == 1))
        {
            assert(0);
        }

        ++count;

        ++sem_key;
    }

    if (!(count == SYSFUN_MAX_SEM_SIZE))
    {
        assert(0);
    }

    while (--sem_key)
    {
        result = SYSFUN_GetSem(sem_key, &sem_id);
        if (!(result == SYSFUN_OK))
        {
            assert(0);
        }

        result = SYSFUN_DestroySem(sem_id);
        if (!(result == SYSFUN_OK))
        {
            assert(0);
        }

        result = SYSFUN_GetSem(sem_key, &sem_id);
        if (!(result == SYSFUN_RESULT_NO_SEMAPHORE))
        {
            assert(0);
        }

        --count;
    }

    if (!(count == 0))
    {
        assert(0);
    }

    if (0)
    {
        SYSFUN_CreateSem(1, 0, 0, &sem_id);
        SYSFUN_TakeSem(sem_id, SYSFUN_TIMEOUT_WAIT_FOREVER);
        SYSFUN_TakeSem(sem_id, SYSFUN_TIMEOUT_WAIT_FOREVER);
    }

    if (0)
    {
        SYSFUN_CreateSem(1, 0, 0, &sem_id);
        SYSFUN_GiveSem(sem_id);
        SYSFUN_GiveSem(sem_id);
    }
}

static void testSharedMemory()
{
    UI32_T key  = 1;
    UI32_T size = 1024;
    UI32_T id;
    void *ptr;
    UI32_T result;
    UI32_T count = 0;

    while (1)
    {
        id = 999;

        result = SYSFUN_CreateShMem(key, size, &id);

        if (SYSFUN_OK != result)
        {
            break;
        }

        ++ count;

        ptr = SYSFUN_AttachShMem(id);

        assert(ptr != NULL);

        memset(ptr, 0x97, size);

        SYSFUN_DetachShMem(ptr);

        ++ key;
    }

    assert(count == SYSFUN_MAX_SHMEM_SIZE);

    // clear up
    {
        UI32_T i;

        for (i=0; i < _countof(sysfun_shmem_array); i++)
        {
            free(sysfun_shmem_array[i].ptr);
        }

        memset(sysfun_shmem_array, 0, sizeof(sysfun_shmem_array));
    }
}

int main (int argc, const char * argv[])
{
    SYSFUN_Init();

    testMessageQueueBasicFunction();

    testMessageQueue();
    testMessageQueue_Private();
    testSemaphore();
    testSharedMemory();
    return 0;
}


