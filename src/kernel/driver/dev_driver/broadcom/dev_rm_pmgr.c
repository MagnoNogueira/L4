#include <string.h>
#include "sysfun.h"
#include "sys_bld.h"
#include "dev_rm_pmgr.h"
#include <esw/dev_rm.h>
#include "sys_module.h"

static UI32_T ipcmsgq_handle;

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_InitiateProcessResource
 *------------------------------------------------------------------------------
 * FUNCTION: Initialize for getting the ipc message queue
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    :
 *------------------------------------------------------------------------------
 */
void DEVRM_PMGR_InitiateProcessResource(void)
{

    if(SYSFUN_GetMsgQ(SYS_BLD_DRIVER_GROUP_IPCMSGQ_KEY,
        SYSFUN_MSGQ_BIDIRECTIONAL, &ipcmsgq_handle)!=SYSFUN_OK)
    {
        printf("%s(): SYSFUN_GetMsgQ fail.\n", __FUNCTION__);
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_FieldInit
 *------------------------------------------------------------------------------
 * FUNCTION: Initialize for clear the chip of the slave
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    :
 *------------------------------------------------------------------------------
 */
void DEVRM_PMGR_FieldInit()
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.fieldinit.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.fieldinit.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_FIELDINIT;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        //return FALSE;
        return;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    //return msg_data_p->type.result_bool;
    return;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_CreateGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a group entry.
 * INPUT:    device_id, group_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_CreateGroup(UI32_T device_id, DEVRM_GroupEntry_T *group_entry)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.setgroup.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.setgroup.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_CREATEGROUP;

    /* assgin req data */
    msg_data_p->data.setgroup.req.device_id = device_id;
    memcpy(&msg_data_p->data.setgroup.req.group_entry, group_entry, sizeof(DEVRM_GroupEntry_T));

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    memcpy(group_entry, &msg_data_p->data.setgroup.resp.group_entry, sizeof(DEVRM_GroupEntry_T));

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_SetGroupQualifySet
 *------------------------------------------------------------------------------
 * PURPOSE:  Set qualify set of group
 * INPUT:    device_id, group_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_SetGroupQualifySet(UI32_T device_id, DEVRM_GroupEntry_T *group_entry)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                          sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.setgroup.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                         sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.setgroup.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_SET_GROUP_QUALIFY_SET;

    /* assgin req data */
    msg_data_p->data.setgroup.req.device_id = device_id;
    memcpy(&msg_data_p->data.setgroup.req.group_entry, group_entry, sizeof(DEVRM_GroupEntry_T));

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
                    SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    memcpy(group_entry, &msg_data_p->data.setgroup.resp.group_entry, sizeof(DEVRM_GroupEntry_T));

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_SetGroupPriority
 *------------------------------------------------------------------------------
 * PURPOSE:  Set priority of group
 * INPUT:    device_id, group_id, priority
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_SetGroupPriority(UI32_T device_id, UI32_T group_id, int priority)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.set_group_priority.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.set_group_priority.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_SET_GROUP_PRIORITY;

    /* assgin req data */
    msg_data_p->data.set_group_priority.req.device_id = device_id;
    msg_data_p->data.set_group_priority.req.group_id = group_id;
    msg_data_p->data.set_group_priority.req.priority = priority;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
                        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_DestroyGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a group entry.
 * INPUT:    device_id, group_index
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_DestroyGroup(UI32_T device_id,UI32_T group_id)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.destroygroup.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.destroygroup.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_DESTROYGROUP;

    /* assgin req data */
    msg_data_p->data.destroygroup.req.device_id = device_id;
    msg_data_p->data.destroygroup.req.group_id = group_id;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_CreateVirtualGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create virtual group.
 * INPUT:    group_id       - group ID
 *           vir_group_id   - virtual group ID (Unique per system).
 *           priority       - virtual group priority.
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_CreateVirtualGroup(
    UI32_T group_id,
    UI32_T vir_group_id,
    int priority)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.create_vir_group.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.create_vir_group.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_CREATE_VIR_GROUP;

    /* assgin req data */
    msg_data_p->data.create_vir_group.req.group_id = group_id;
    msg_data_p->data.create_vir_group.req.vir_group_id = vir_group_id;
    msg_data_p->data.create_vir_group.req.priority = priority;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;


    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_AllocateRule
 *------------------------------------------------------------------------------
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
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_AllocateRule(
    UI32_T device_id,
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

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_SetRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    device_id, group_id, dev_rule_id, rule_entry, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     DEVRM_Rule = class_element + meter + action
 *           pass meter_entry with NULL if this rule does not have meter
 *           pass action_entry with NULL if this rule does not have action
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_SetRule(UI32_T device_id,
                          UI32_T group_id,
                          UI32_T dev_rule_id,
                          DEVRM_AceEntry_T *ace_entry,
                          DEVRM_MeterEntry_T *meter_entry,
                          DEVRM_ActionEntry_T *action_entry,
                          BOOL_T counter_enable)
{
    UI32_T action_entry_num = 0;
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.setrule.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.setrule.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_SETRULE;

    /* assgin req data */
    msg_data_p->data.setrule.req.device_id = device_id;
    msg_data_p->data.setrule.req.group_id = group_id;
    msg_data_p->data.setrule.req.dev_rule_id = dev_rule_id;
    msg_data_p->data.setrule.req.counter_enable = counter_enable;

    if(ace_entry != NULL)
    {
        memcpy(&msg_data_p->data.setrule.req.ace_entry, ace_entry, sizeof(DEVRM_AceEntry_T));
    }

    if(meter_entry != NULL)
    {
        memcpy(&msg_data_p->data.setrule.req.meter_entry, meter_entry, sizeof(DEVRM_MeterEntry_T));
        msg_data_p->data.setrule.req.meter_entry_num = 1;
    }

    for(; action_entry != NULL ;action_entry = action_entry->next_action)
    {
        static UI32_T MAX_NBR_OF_ACTION_PER_RULE =
            sizeof(msg_data_p->data.setrule.req.action_entry)/sizeof(msg_data_p->data.setrule.req.action_entry[0]);

        if (MAX_NBR_OF_ACTION_PER_RULE <= action_entry_num)
        {
            printf("%s: action_entry_num(%lu) exceed\n", __FUNCTION__, action_entry_num);
            return FALSE;
        }

        memcpy(&msg_data_p->data.setrule.req.action_entry[action_entry_num], action_entry, sizeof(DEVRM_ActionEntry_T));
        action_entry_num++;
    }
    msg_data_p->data.setrule.req.action_entry_num = action_entry_num;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_UpdateRule
 *------------------------------------------------------------------------------
 * PURPOSE:  update a rule entry.
 * INPUT:    device_id, dev_rule_id, rule_entry, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     DEVRM_Rule = class_element + meter + action
 *           pass meter_entry with NULL if this rule does not have meter
 *           pass action_entry with NULL if this rule does not have action
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_UpdateRule(UI32_T device_id,
                             UI32_T dev_rule_id,
                             DEVRM_AceEntry_T *ace_entry,
                             DEVRM_MeterEntry_T *meter_entry,
                             DEVRM_ActionEntry_T *action_entry)
{
    UI32_T action_entry_num = 0;
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.updaterule.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.updaterule.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_UPDATERULE;

    /* assgin req data */
    msg_data_p->data.updaterule.req.device_id = device_id;
    msg_data_p->data.updaterule.req.dev_rule_id = dev_rule_id;
    if(ace_entry != NULL)
        memcpy(&msg_data_p->data.updaterule.req.ace_entry, ace_entry, sizeof(DEVRM_AceEntry_T));
    if(meter_entry != NULL)
    {
        memcpy(&msg_data_p->data.updaterule.req.meter_entry, meter_entry, sizeof(DEVRM_MeterEntry_T));
        msg_data_p->data.updaterule.req.meter_entry_num = 1;
    }

    for(; action_entry != NULL ;action_entry = action_entry->next_action)
    {
        static UI32_T MAX_NBR_OF_ACTION_PER_RULE =
            sizeof(msg_data_p->data.setrule.req.action_entry)/sizeof(msg_data_p->data.setrule.req.action_entry[0]);

        if (MAX_NBR_OF_ACTION_PER_RULE <= action_entry_num)
        {
            printf("%s: action_entry_num(%lu) exceed\n", __FUNCTION__, action_entry_num);
            return FALSE;
        }

        memcpy(&msg_data_p->data.updaterule.req.action_entry[action_entry_num], action_entry, sizeof(DEVRM_ActionEntry_T));
        action_entry_num++;
    }
    msg_data_p->data.updaterule.req.action_entry_num = action_entry_num;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_DestroyRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a rule entry.
 * INPUT:    dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.If meter is shared, when delete it should check ..
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_DestroyRule(
    UI32_T device_id,
    UI32_T vir_group_id,
    UI32_T dev_rule_id)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.destroyrule.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.destroyrule.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_DESTROYRULE;

    /* assgin req data */
    msg_data_p->data.destroyrule.req.device_id = device_id;
    msg_data_p->data.destroyrule.req.vir_group_id = vir_group_id;
    msg_data_p->data.destroyrule.req.dev_rule_id = dev_rule_id;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

/* meter */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_CreateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  create a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry,
 *           need_reinstalled (TRUE - rule will be reinstalled)
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_CreateMeter(UI32_T device_id,UI32_T dev_rule_id, DEVRM_MeterEntry_T *meter_entry, BOOL_T need_reinstalled)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.create_meter.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.create_meter.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_CREATEMETER;

    /* assgin req data */
    msg_data_p->data.create_meter.req.device_id = device_id;
    msg_data_p->data.create_meter.req.dev_rule_id = dev_rule_id;
    if(meter_entry != NULL)
        memcpy(&msg_data_p->data.create_meter.req.meter_entry, meter_entry, sizeof(DEVRM_MeterEntry_T));
    msg_data_p->data.create_meter.req.need_reinstalled = need_reinstalled;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_UpdateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  update a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_UpdateMeter(UI32_T device_id,UI32_T dev_rule_id, DEVRM_MeterEntry_T *meter_entry)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.update_meter.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.update_meter.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_UPDATEMETER;

    /* assgin req data */
    msg_data_p->data.update_meter.req.device_id = device_id;
    msg_data_p->data.update_meter.req.dev_rule_id = dev_rule_id;

    if(meter_entry != NULL)
    {
        memcpy(&msg_data_p->data.update_meter.req.meter_entry, meter_entry, sizeof(DEVRM_MeterEntry_T));
        msg_data_p->data.update_meter.req.meter_entry_num =1;
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

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_ShareMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  share a meter from src_rule_id to dst_rule_id
 * INPUT:    device_id, src_rule_id, dst_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_ShareMeter(UI32_T device_id, UI32_T src_rule_id, UI32_T dst_rule_id)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.share_meter.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.share_meter.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_SHAREMETER;

    /* assgin req data */
    msg_data_p->data.share_meter.req.device_id = device_id;
    msg_data_p->data.share_meter.req.src_rule_id = src_rule_id;
    msg_data_p->data.share_meter.req.dst_rule_id = dst_rule_id;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_DestroyMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a meter entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_DestroyMeter(UI32_T device_id, UI32_T dev_rule_id)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.destroymeter.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.destroymeter.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_DESTROYMETER;

    /* assgin req data */
    msg_data_p->data.destroymeter.req.device_id = device_id;
    msg_data_p->data.destroymeter.req.dev_rule_id = dev_rule_id;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

/* action */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_SetAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a action entry.
 * INPUT:    device_id, dev_rule_id, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_SetAction(UI32_T device_id,
                            UI32_T dev_rule_id,
                            DEVRM_ActionEntry_T *action_entry)
{
    UI32_T action_entry_num = 0;
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.setaction.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.setaction.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_SETACTION;

    /* assgin req data */
    msg_data_p->data.setaction.req.device_id = device_id;
    msg_data_p->data.setaction.req.dev_rule_id = dev_rule_id;
    for(; action_entry != NULL ;action_entry = action_entry->next_action)
    {
        memcpy(&msg_data_p->data.setaction.req.action_entry[action_entry_num], action_entry, sizeof(DEVRM_ActionEntry_T));
        action_entry_num++;
        if(action_entry_num >= SYS_ADPT_FP_MAX_NBR_OF_RULE_PER_SELECTOR)
        {
            printf("%s:Warning action_entry_num exceed\n", __FUNCTION__);
            break;
        }
    }
    msg_data_p->data.setaction.req.action_entry_num = action_entry_num;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_DestroyAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a action entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_DestroyAction(UI32_T device_id, UI32_T dev_rule_id)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.destroy_action.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.destroy_action.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_DESTROYACTION;

    /* assgin req data */
    msg_data_p->data.destroy_action.req.device_id = device_id;
    msg_data_p->data.destroy_action.req.dev_rule_id = dev_rule_id;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_GetCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  get counter
 * INPUT:    aclId, logical_rule_id
 * OUTPUT:   green_octets_p, yellow_octets_p, red_octets_p
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_GetCounter(
    UI32_T device_id,
    UI32_T dev_rule_id,
    UI32_T *counter_p)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.get_counter.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.get_counter.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_GET_COUNTER;

    /* assgin req data */
    msg_data_p->data.get_counter.req.device_id = device_id;
    msg_data_p->data.get_counter.req.dev_rule_id = dev_rule_id;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    if (msg_data_p->type.result_bool)
    {
        *counter_p = msg_data_p->data.get_counter.resp.counter;
    }

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_ClearCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  clear counter
 * INPUT:    aclId, logical_rule_id
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_ClearCounter(
    UI32_T device_id,
    UI32_T dev_rule_id)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.clear_counter.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.clear_counter.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_CLEAR_COUNTER;

    /* assgin req data */
    msg_data_p->data.clear_counter.req.device_id = device_id;
    msg_data_p->data.clear_counter.req.dev_rule_id = dev_rule_id;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_GetNumberOfChips
 *------------------------------------------------------------------------------
 * PURPOSE  : Get soc_ndev
 * INPUT    : None
 * OUTPUT   : NONE.
 * RETURN   : number of chip.
 * NOTE     : for debug
 *------------------------------------------------------------------------------
 */
UI32_T DEVRM_PMGR_GetNumberOfChips()
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.getnumberofchips.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.getnumberofchips.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_GETNUMBEROFCHIPS;

    /* assgin req data */

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    return msg_data_p->type.result_ui32;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_GetDeviceInfo
 *------------------------------------------------------------------------------
 * PURPOSE  : Get device information
 * INPUT    : None
 * OUTPUT   : num_of_chips
 *            port_bit_map.all_ports
 *            port_bit_map.cpu_port
 * RETURN   : TRUE/FALSE
 * NOTE     : NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_GetDeviceInfo(
    DEVRM_DevInfo_T *dev_info_p)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.get_device_info.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                 sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.get_device_info.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_GET_DEVICE_INFOMATION;
    
    /* assgin req data */
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
                                   SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }
    
    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    if (msg_data_p->type.result_bool)
    {
        memcpy(dev_info_p,
               &msg_data_p->data.get_device_info.resp.device_info,
               sizeof(msg_data_p->data.get_device_info.resp.device_info));
    }
    
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_CheckSoc
 *------------------------------------------------------------------------------
 * PURPOSE  : check soc
 * INPUT    : None
 * OUTPUT   : NONE.
 * RETURN   : TRUE/FALSE
 * NOTE     : None
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_PMGR_CheckSoc(UI32_T device_id)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.checksoc.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.checksoc.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_CHECKSOC;

    /* assgin req data */
    msg_data_p->data.checksoc.req.device_id = device_id;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEVRM_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}

