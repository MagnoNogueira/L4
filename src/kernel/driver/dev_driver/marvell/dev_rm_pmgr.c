#include <string.h>
#include "sysfun.h"
#include "sys_bld.h"
#include "dev_rm_pmgr.h"
#ifdef MARVELL_CPSS /* Yongxin.Zhao added, 2009-05-13, 15:13:39 */
#include <marvell/dev_rm.h>
#else
#include <esw/dev_rm.h>
#endif
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
 * ROUTINE NAME - DEVRM_PMGR_AllocateRule
 *------------------------------------------------------------------------------
 * PURPOSE  : Allocate rule resource
 * INPUT    : device_id, group_id, rule_pri
 * OUTPUT   : dev_rule_id (per chip unique),
 *
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, call DEVRM_PMGR_CreateGroup() first
 *            2, MUST allocate rule resource before call DEVRM_PMGR_SetRule()
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_AllocateRule(
    UI32_T  device_id,
    UI32_T  pcl_id,
    UI32_T  vir_group_id,
    int     rule_pri,
    UI32_T  dev_rule_id)
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
    msg_data_p->data.allocaterule.req.pcl_id = pcl_id;
    msg_data_p->data.allocaterule.req.vir_group_id = vir_group_id;
    msg_data_p->data.allocaterule.req.rule_pri = rule_pri;
    msg_data_p->data.allocaterule.req.dev_rule_id = dev_rule_id;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p,
                                   SYSFUN_TIMEOUT_WAIT_FOREVER,
                                   SYSFUN_SYSTEM_EVENT_IPCMSG,
                                   resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

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
BOOL_T
DEVRM_PMGR_SetRule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry,
    const DEVRM_ACTION_T *action_p)
{
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

    if (NULL != ace_entry)
    {
        memcpy(&msg_data_p->data.setrule.req.ace_entry, ace_entry, sizeof(DEVRM_AceEntry_T));
    }

    msg_data_p->data.setrule.req.action = *action_p;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }


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
BOOL_T
DEVRM_PMGR_DestroyRule(
    UI32_T device_id,
    UI32_T pcl_id,
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
    msg_data_p->data.destroyrule.req.device_id      = device_id;
    msg_data_p->data.destroyrule.req.pcl_id         = pcl_id;
    msg_data_p->data.destroyrule.req.dev_rule_id    = dev_rule_id;

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
 * ROUTINE NAME - DEVRM_PMGR_UpdateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  update a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_UpdateMeter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    const DEVRM_POLICER_METER_T *dev_meter_p)
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
    msg_data_p->data.update_meter.req.pcl_id = pcl_id;
    msg_data_p->data.update_meter.req.dev_rule_id = dev_rule_id;
    msg_data_p->data.update_meter.req.dev_meter = *dev_meter_p;

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
BOOL_T
DEVRM_PMGR_ShareMeter(
    UI32_T device_id,
    UI32_T src_rule_id,
    UI32_T dst_rule_id)
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

/* action */
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
BOOL_T
DEVRM_PMGR_DestroyAction(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id)
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
    msg_data_p->data.destroy_action.req.pcl_id = pcl_id;
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
               sizeof(*dev_info_p));
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

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_PMGR_CreateAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Create ACL.
 * INPUT:    aclId, rule_type
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_CreatePcl(
    UI32_T device_id,
    UI32_T pcl_id,
    BOOL_T is_ingress,
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num,
    UI32_T rule_type)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.create_pcl.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                 sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.create_pcl.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_CREATE_PCL;

    /* assgin req data */
    msg_data_p->data.create_pcl.req.device_id    = device_id;
    msg_data_p->data.create_pcl.req.pcl_id       = pcl_id;
    msg_data_p->data.create_pcl.req.is_ingress   = is_ingress;
    msg_data_p->data.create_pcl.req.lookup_num   = lookup_num;
    msg_data_p->data.create_pcl.req.rule_type    = rule_type;

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
 * ROUTINE NAME  - DEVRM_PMGR_BindPclToPort
 *------------------------------------------------------------------------------
 * PURPOSE  : Bind PCL to port
 * INPUT    : device_id - device ID
 *            phy_port  - phy port
 *            pcl_id    - PCL ID
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_BindPclToPort(
    UI32_T device_id,
    UI32_T phy_port,
    UI32_T pcl_id)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.bind_pcl_to_port.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                 sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.bind_pcl_to_port.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_BIND_PCL_TO_PORT;

    /* assgin req data */
    msg_data_p->data.bind_pcl_to_port.req.device_id = device_id;
    msg_data_p->data.bind_pcl_to_port.req.phy_port = phy_port;
    msg_data_p->data.bind_pcl_to_port.req.pcl_id   = pcl_id;

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
 * ROUTINE NAME  - DEVRM_PMGR_UnbindPclToPort
 *------------------------------------------------------------------------------
 * PURPOSE  : Unbind PCL to port
 * INPUT    : device_id - device ID
 *            phy_port  - phy port
 *            pcl_id    - PCL ID
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_UnbindPclToPort(
    UI32_T device_id,
    UI32_T phy_port,
    UI32_T pcl_id)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
    sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.bind_pcl_to_port.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
    sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.bind_pcl_to_port.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_UNBIND_PCL_TO_PORT;

    /* assgin req data */
    msg_data_p->data.bind_pcl_to_port.req.device_id = device_id;
    msg_data_p->data.bind_pcl_to_port.req.phy_port = phy_port;
    msg_data_p->data.bind_pcl_to_port.req.pcl_id   = pcl_id;

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
 * ROUTINE NAME  - DEVRM_PMGR_BindPclToTrunkPort
 *------------------------------------------------------------------------------
 * PURPOSE  : Bind PCL to trunk port
 * INPUT    : device_id - device ID
 *            trunk_id  - trunk ID
 *            pcl_id    - PCL ID
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_BindPclToTrunkPort(
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.bind_pcl_to_trunk_port.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
                                 sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.bind_pcl_to_trunk_port.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_BIND_PCL_TO_TRUNK_PORT;

    /* assgin req data */
    msg_data_p->data.bind_pcl_to_trunk_port.req.device_id = device_id;
    msg_data_p->data.bind_pcl_to_trunk_port.req.trunk_id = trunk_id;
    msg_data_p->data.bind_pcl_to_trunk_port.req.pcl_id   = pcl_id;

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
 * ROUTINE NAME  - DEVRM_PMGR_UnbindPclToTrunkPort
 *------------------------------------------------------------------------------
 * PURPOSE  : Unbind PCL to trunk port
 * INPUT    : device_id - device ID
 *            trunk_id  - trunk ID
 *            pcl_id    - PCL ID
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_PMGR_UnbindPclToTrunkPort(
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEVRM_PMGR_REQ_CMD_SIZE +
    sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.bind_pcl_to_trunk_port.req);
    const UI32_T resp_msg_size = DEVRM_PMGR_RESP_RESULT_SIZE +
    sizeof(((DEVRM_PMGR_IPCMSG_T *)0)->data.bind_pcl_to_trunk_port.resp);
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
    msg_data_p->type.cmd = DEVRM_IPCCMD_UNBIND_PCL_TO_TRUNK_PORT;

    /* assgin req data */
    msg_data_p->data.bind_pcl_to_trunk_port.req.device_id = device_id;
    msg_data_p->data.bind_pcl_to_trunk_port.req.trunk_id = trunk_id;
    msg_data_p->data.bind_pcl_to_trunk_port.req.pcl_id   = pcl_id;

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
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    UI32_T *green_octets_p,
    UI32_T *yellow_octets_p,
    UI32_T *red_octets_p)
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
    msg_data_p->data.get_counter.req.pcl_id = pcl_id;
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
        *green_octets_p = msg_data_p->data.get_counter.resp.green_octets;
        *yellow_octets_p = msg_data_p->data.get_counter.resp.yellow_octets;
        *red_octets_p = msg_data_p->data.get_counter.resp.red_octets;
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
    UI32_T pcl_id,
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
    msg_data_p->data.clear_counter.req.pcl_id = pcl_id;
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

