#include <string.h>
#include "sysfun.h"
#include "sys_bld.h"
#include "dev_swdrvl4_pmgr.h"
#include "dev_swdrvl4.h"
#include "sys_module.h"

static UI32_T ipcmsgq_handle;
#if 0
#define DBG_PRINT(...) printf(__VA_ARGS__);
#else
#define DBG_PRINT(...)
#endif

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_InitiateProcessResource
 *------------------------------------------------------------------------------
 * PURPOSE : This function is used to initiate the information for IPC
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    :
 *------------------------------------------------------------------------------
 */
void DEV_SWDRVL4_PMGR_InitiateProcessResource()
{
    /* Given that L4 PMGR requests are handled in L4GROUP of L2_L4_PROC
     */
    if(SYSFUN_GetMsgQ(SYS_BLD_DRIVER_GROUP_IPCMSGQ_KEY, 
        SYSFUN_MSGQ_BIDIRECTIONAL, &ipcmsgq_handle)!=SYSFUN_OK)
    {
        printf("%s(): SYSFUN_GetMsgQ fail.\n", __FUNCTION__);
    }
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_EnableCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function globally enables the specific COS mapping
 * INPUT   : maptype -- which type of mapping is being enabled
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : 1. If the system support per port enable, this API
 *              enables the specific mapping of all ports
 *           2. This API may need to config the ASIC if the mapping
 *              is changed from disable to enable.
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_EnableCosMapping(DEV_SWDRVL4_CosMapType_T maptype)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.enablecosmapping.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.enablecosmapping.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_ENABLECOSMAPPING;
    
    /* assgin req data */
    memcpy(&msg_data_p->data.enablecosmapping.req.maptype, &maptype, sizeof(DEV_SWDRVL4_CosMapType_T));
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_EnablePortCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function per port enables the specific COS mapping
 * INPUT   : maptype -- which type of mapping is being enabled
 *           unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : 1. If the system soes not support per port enable/disable,
 *              this API always returns false
 *           2. Module_ID will be translated from unit, port by device driver
 *              The DevicePortMapping table will be passed down from core layer
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_EnablePortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                        UI32_T unit,
                                        UI32_T port)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.enableportcosmapping.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.enableportcosmapping.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_ENABLEPORTCOSMAPPING;
    
    /* assgin req data */
    memcpy(&msg_data_p->data.enableportcosmapping.req.maptype, &maptype, sizeof(DEV_SWDRVL4_CosMapType_T));
    msg_data_p->data.enableportcosmapping.req.unit = unit;
    msg_data_p->data.enableportcosmapping.req.port = port;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DisableCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function globally disables the specific COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : 1. If the system support per port disable, this API
 *              disables the specific mapping of all ports
 *           2. This API may need to config the ASIC if the mapping
 *              is changed from disable to disable.
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DisableCosMapping(DEV_SWDRVL4_CosMapType_T maptype)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.disablecosmapping.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.disablecosmapping.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_DISABLECOSMAPPING;
    
    /* assgin req data */
    memcpy(&msg_data_p->data.disablecosmapping.req.maptype, &maptype, sizeof(DEV_SWDRVL4_CosMapType_T));
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DisablePortCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function per port disables the specific COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : If the system soes not support per port disable/disable,
 *           this API always returns false
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DisablePortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                         UI32_T unit,
                                         UI32_T port)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.disableportcosmapping.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.disableportcosmapping.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_DISABLEPORTCOSMAPPING;
    
    /* assgin req data */
    memcpy(&msg_data_p->data.disableportcosmapping.req.maptype, &maptype, sizeof(DEV_SWDRVL4_CosMapType_T));
    msg_data_p->data.disableportcosmapping.req.unit = unit;
    msg_data_p->data.disableportcosmapping.req.port = port;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function globally sets the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with global enable
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                 UI32_T value,
                                 UI32_T cos)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosmapping.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosmapping.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETCOSMAPPING;
    
    /* assgin req data */
    memcpy(&msg_data_p->data.setcosmapping.req.maptype, &maptype, sizeof(DEV_SWDRVL4_CosMapType_T));
    msg_data_p->data.setcosmapping.req.value = value;
    msg_data_p->data.setcosmapping.req.cos = cos;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPortCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function per port sets the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           unit -- physically stacking unit(box) number
 *           port: user port in this unit
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with per port enable
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                     UI32_T unit,
                                     UI32_T port,
                                     UI32_T value,
                                     UI32_T cos)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setportcosmapping.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setportcosmapping.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETPORTCOSMAPPING;
    
    /* assgin req data */
    memcpy(&msg_data_p->data.setportcosmapping.req.maptype, &maptype, sizeof(DEV_SWDRVL4_CosMapType_T));
    msg_data_p->data.setportcosmapping.req.unit = unit;
    msg_data_p->data.setportcosmapping.req.port = port;
    msg_data_p->data.setportcosmapping.req.value = value;
    msg_data_p->data.setportcosmapping.req.cos = cos;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DeleteCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function globally deletes the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with global disable
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DeleteCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                    UI32_T value,
                                    UI32_T cos)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.deletecosmapping.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.deletecosmapping.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_DELETECOSMAPPING;
    
    /* assgin req data */
    memcpy(&msg_data_p->data.deletecosmapping.req.maptype, &maptype, sizeof(DEV_SWDRVL4_CosMapType_T));
    msg_data_p->data.deletecosmapping.req.value = value;
    msg_data_p->data.deletecosmapping.req.cos = cos;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DeletePortCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function per port deletes the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           unit -- physically stacking unit(box) number
 *           port: user port in this unit
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with per port disable
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DeletePortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                        UI32_T unit,
                                        UI32_T port,
                                        UI32_T value,
                                        UI32_T cos)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.deleteportcosmapping.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.deleteportcosmapping.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_DELETEPORTCOSMAPPING;
    
    /* assgin req data */
    memcpy(&msg_data_p->data.deleteportcosmapping.req.maptype, &maptype, sizeof(DEV_SWDRVL4_CosMapType_T));
    msg_data_p->data.deleteportcosmapping.req.unit = unit;
    msg_data_p->data.deleteportcosmapping.req.port = port;
    msg_data_p->data.deleteportcosmapping.req.value = value;
    msg_data_p->data.deleteportcosmapping.req.cos = cos;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_EnablePortIngressRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: his function per port enables the ingress rate limit
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : Module_ID will be translated from unit, port by device driver
 *           The DevicePortMapping table will be passed down from core layer
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_EnablePortIngressRateLimit(UI32_T unit,
                                              UI32_T port)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.enableportingressratelimit.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.enableportingressratelimit.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_ENABLEPORTINGRESSRATELIMIT;
    
    /* assgin req data */
    msg_data_p->data.enableportingressratelimit.req.unit = unit;
    msg_data_p->data.enableportingressratelimit.req.port = port;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DisablePortIngressRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: his function per port disables the ingress rate limit
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : Module_ID will be translated from unit, port by device driver
 *           The DevicePortMapping table will be passed down from core layer
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DisablePortIngressRateLimit(UI32_T unit,
                                              UI32_T port)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.disableportingressratelimit.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.disableportingressratelimit.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_DISABLEPORTINGRESSRATELIMIT;
    
    /* assgin req data */
    msg_data_p->data.disableportingressratelimit.req.unit = unit;
    msg_data_p->data.disableportingressratelimit.req.port = port;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPortIngressRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: This function sets the port ingress rate limit
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- which port to set
 *           rate -- port ingress rate limit
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : 1. The limit is the total limitation for the traffic received
 *              from the specific port
 *           2. For Strata switch, the grain of 125KBytes for EPIC,
 *              and 8MByes for GPIC
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPortIngressRateLimit(UI32_T unit,
                                           UI32_T port,
                                           UI32_T rate)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setportingressratelimit.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setportingressratelimit.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETPORTINGRESSRATELIMIT;
    
    /* assgin req data */
    msg_data_p->data.setportingressratelimit.req.unit = unit;
    msg_data_p->data.setportingressratelimit.req.port = port;
    msg_data_p->data.setportingressratelimit.req.rate = rate;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_EnablePortEgressRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: This function enables the port egress rate limit
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_EnablePortEgressRateLimit(UI32_T unit, UI32_T port)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.enableportegressratelimit.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.enableportegressratelimit.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_ENABLEPORTEGRESSRATELIMIT;
    
    /* assgin req data */
    msg_data_p->data.enableportegressratelimit.req.unit = unit;
    msg_data_p->data.enableportegressratelimit.req.port = port;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DisablePortEgressRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: This function disables the port egress rate limit
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DisablePortEgressRateLimit(UI32_T unit, UI32_T port)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.disableportegressratelimit.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.disableportegressratelimit.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_DISABLEPORTEGRESSRATELIMIT;
    
    /* assgin req data */
    msg_data_p->data.disableportegressratelimit.req.unit = unit;
    msg_data_p->data.disableportegressratelimit.req.port = port;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPortEgressRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: This function sets the port gress rate limit
 * INPUT   : unit -- device
 *           port -- which port to set
 *           rate -- port ingress rate limit
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPortEgressRateLimit(UI32_T unit,
                                          UI32_T port,
                                          UI32_T rate)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setportegressratelimit.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setportegressratelimit.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETPORTEGRESSRATELIMIT;
    
    /* assgin req data */
    msg_data_p->data.setportegressratelimit.req.unit = unit;
    msg_data_p->data.setportegressratelimit.req.port = port;
    msg_data_p->data.setportegressratelimit.req.rate = rate;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetEgressSchedulingMethod
 *------------------------------------------------------------------------------
 * FUNCTION: This function sets the scheduling method for egress queues
 * INPUT   : method -- scheduling method
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : For Strata Switch, default is StrictPriorityScheduling
 *           This function is for global configuration
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetEgressSchedulingMethod(DEV_SWDRVL4_EgressSchedulingMethod_T method)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setegressschedulingmethod.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setegressschedulingmethod.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETEGRESSSCHEDULINGMETHOD;
    
    /* assgin req data */
    msg_data_p->data.setegressschedulingmethod.req.method = method;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);

    return msg_data_p->type.result_bool;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetEgressSchedulingMethod
 *------------------------------------------------------------------------------
 * FUNCTION: This function sets the scheduling method for egress queues
 * INPUT   : 
			unit -- which unit to set
			port -- which port to set
			method -- scheduling method
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : For Strata Switch, default is StrictPriorityScheduling
 *           This function is for individual configuration
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPortEgressSchedulingMethod(UI32_T unit, UI32_T port, DEV_SWDRVL4_EgressSchedulingMethod_T method)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setegressschedulingmethod.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setegressschedulingmethod.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETPORTEGRESSSCHEDULINGMETHOD;
    
    /* assgin req data */
    msg_data_p->data.setegressschedulingmethod.req.unit = unit;
	msg_data_p->data.setegressschedulingmethod.req.port = port;
	msg_data_p->data.setegressschedulingmethod.req.method = method;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;

    return msg_data_p->type.result_bool;
}
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetEgressSchedulingMethod
 *------------------------------------------------------------------------------
 * FUNCTION: This function identify which queues are SP scheduling
 *           when the scheduling method is SP + WRR
 * INPUT   : unit_id -- physically stacking unit(box) number
 *           port    -- user port in this unitstart_queue_no -- the starting
 *                      queue number for SP scheduling
 *           end_queue_no   -- the ending queue number for SP scheduling
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : The queue numbers not included in start_queue_no and end_queue_no
 *           will run WRR scheduling
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetEgressSpSchedulingQueues(UI32_T unit_id,
                                               UI32_T port,
                                               UI32_T start_queue_no,
                                               UI32_T end_queue_no)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setegressspschedulingqueues.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setegressspschedulingqueues.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETEGRESSSPSCHEDULINGQUEUES;
    
    /* assgin req data */
    msg_data_p->data.setegressspschedulingqueues.req.unit_id = unit_id;
    msg_data_p->data.setegressspschedulingqueues.req.port = port;
    msg_data_p->data.setegressspschedulingqueues.req.start_queue_no = start_queue_no;
    msg_data_p->data.setegressspschedulingqueues.req.end_queue_no = end_queue_no;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPortWrrQueueWeight
 *------------------------------------------------------------------------------
 * FUNCTION: This function sets WRR queue weight
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 *           q_id     -- which queue to set
 *           weight   -- which weight to set
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: failed
 * NOTE    : The weight is a relative value to represent the ratio
 *           among queues
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPortWrrQueueWeight(UI32_T unit_id,
                                         UI32_T port,
                                         UI32_T q_id,
                                         UI32_T weight)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setportwrrqueueweight.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setportwrrqueueweight.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETPORTWRRQUEUEWEIGHT;
    
    /* assgin req data */
    msg_data_p->data.setportwrrqueueweight.req.unit_id = unit_id;
    msg_data_p->data.setportwrrqueueweight.req.port = port;
    msg_data_p->data.setportwrrqueueweight.req.q_id = q_id;
    msg_data_p->data.setportwrrqueueweight.req.weight = weight;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_EnableTrafficSegmatation
 *------------------------------------------------------------------------------
 * FUNCTION: Enable Traffic Segematation functions
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    :
 *
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_EnableTrafficSegmatation()
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.enabletrafficsegmatation.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.enabletrafficsegmatation.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_ENABLETRAFFICSEGMATATION;
    
    /* assgin req data */
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DisableTrafficSegmatation
 *------------------------------------------------------------------------------
 * FUNCTION: Disable Traffic Segematation functions
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    :
 *
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DisableTrafficSegmatation()
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.disabletrafficsegmatation.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.disabletrafficsegmatation.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_DISABLETRAFFICSEGMATATION;
    
    /* assgin req data */
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_ResetTrafficSegmatation
 *------------------------------------------------------------------------------
 * FUNCTION: Reset the Traffic Segematation (Delete the membership in the H/W)
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : Clear all the private vlan irules
 *
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_ResetTrafficSegmatation()
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.resettrafficsegmatation.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.resettrafficsegmatation.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];
    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_RESETTRAFFICSEGMATATION;
    
    /* assgin req data */
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetTrafficSegmatationByPortlist
 *------------------------------------------------------------------------------
 * FUNCTION: Set the Traffic Segematation membership
 * INPUT   : uplink_port_list     - uplink port list member set
 *	     downlink_port_list    - downlink port list member set
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : MSB is port 1
 *
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetTrafficSegmatationByPortlist(UI8_T *uplink_port_list,
                                                   UI8_T *downlink_port_list)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.settrafficsegmatationbyportlist.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.settrafficsegmatationbyportlist.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETTRAFFICSEGMATATIONBYPORTLIST;
    
    /* assgin req data */
    memcpy(msg_data_p->data.settrafficsegmatationbyportlist.req.uplink_port_list, uplink_port_list, SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);
    memcpy(msg_data_p->data.settrafficsegmatationbyportlist.req.downlink_port_list, downlink_port_list, SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE) 
#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPrivateVlanPortlistBySessionId
 *------------------------------------------------------------------------------
 * FUNCTION: This function will set the private vlan by session group
 * INPUT   : session_id         -- session id to pvlan group
 *           uplink_port_list   -- uplink port list
 *           downlink_port_list -- downlink port list
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPrivateVlanPortlistBySessionId(UI32_T session_id,
                                                     UI8_T  *uplink_port_list,
                                                     UI8_T  *downlink_port_list)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.settrafficsegmatationbyportlist.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.settrafficsegmatationbyportlist.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;

    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;

    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;

    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETPRIVATEVLANPORTLISTBYSESSIONID;

    /* assgin req data */
    msg_data_p->data.settrafficsegmatationbyportlist.req.session_id = session_id;
    memcpy(msg_data_p->data.settrafficsegmatationbyportlist.req.uplink_port_list, uplink_port_list, SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);
    memcpy(msg_data_p->data.settrafficsegmatationbyportlist.req.downlink_port_list, downlink_port_list, SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        DBG_PRINT("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		DBG_PRINT("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DeletePrivateVlanPortlistBySessionId
 *------------------------------------------------------------------------------
 * FUNCTION: This function will delete the private vlan by session group
 * INPUT   : session_id         -- session id to pvlan group  
 *           uplink_port_list   -- uplink port list
 *           downlink_port_list -- downlink port list
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : session_id is 0, means remove pvlan port throw internal session_id
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DeletePrivateVlanPortlistBySessionId(UI32_T session_id,
                                                     UI8_T  *uplink_port_list,
                                                     UI8_T  *downlink_port_list)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.settrafficsegmatationbyportlist.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.settrafficsegmatationbyportlist.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;

    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;

    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;

    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_DELETEPRIVATEVLANPORTLISTBYSESSIONID;

    /* assgin req data */
    msg_data_p->data.settrafficsegmatationbyportlist.req.session_id = session_id;
    memcpy(msg_data_p->data.settrafficsegmatationbyportlist.req.uplink_port_list, uplink_port_list, SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);
    memcpy(msg_data_p->data.settrafficsegmatationbyportlist.req.downlink_port_list, downlink_port_list, SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST);

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        DBG_PRINT("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		DBG_PRINT("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPrivateVlanUplinkPortMode
 *------------------------------------------------------------------------------
 * FUNCTION: This function will set uplink port mode to private vlan,  
 *           means uplink's traffic are blocking each groups
 * INPUT   : Mode  TRUE is blocking, FALSE is forwarding 
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPrivateVlanUplinkPortMode(BOOL_T mode)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setprivatevlanuplinkportmode.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setprivatevlanuplinkportmode.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;

    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;

    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;

    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETPRIVATEVLANUPLINKPORTMODE;

    /* assgin req data */
    msg_data_p->data.setprivatevlanuplinkportmode.req.mode = mode;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        DBG_PRINT("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		DBG_PRINT("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPrivateVlanTrunkMode
 *------------------------------------------------------------------------------
 * FUNCTION: This function will set status of private vlan of trunk id
 * INPUT   : session_id
 *           trunk_id
 *           is_uplink
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPrivateVlanTrunkMode(UI32_T session_id, UI32_T trunk_id, BOOL_T is_uplink)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.settrafficsegmatationtrunkmode.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.settrafficsegmatationtrunkmode.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;

    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;

    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;

    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETPRIVATEVLANTRUNKMODE;

    /* assgin req data */
    msg_data_p->data.settrafficsegmatationtrunkmode.req.session_id = session_id;
    msg_data_p->data.settrafficsegmatationtrunkmode.req.trunk_id = trunk_id;
    msg_data_p->data.settrafficsegmatationtrunkmode.req.is_uplink = is_uplink;

    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        DBG_PRINT("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		DBG_PRINT("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION) */
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE) */ 

/*=====================================================================
 * Exported functions for Port Security
 *=====================================================================
 */
BOOL_T DEV_SWDRVL4_PMGR_EnablePortSecurity(UI32_T unit_id, UI32_T port)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.enableportsecurity.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.enableportsecurity.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_ENABLEPORTSECURITY;
    
    /* assgin req data */
    msg_data_p->data.enableportsecurity.req.unit_id = unit_id;
    msg_data_p->data.enableportsecurity.req.port = port;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

BOOL_T DEV_SWDRVL4_PMGR_DisablePortSecurity(UI32_T unit_id, UI32_T port)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.disableportsecurity.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.disableportsecurity.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_DISABLEPORTSECURITY;
    
    /* assgin req data */
    msg_data_p->data.disableportsecurity.req.unit_id = unit_id;
    msg_data_p->data.disableportsecurity.req.port = port;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPortSecurityMode
 *------------------------------------------------------------------------------
 * FUNCTION: This function sets port security mode
 * INPUT   : unit_id -- physically stacking unit(box) number
 *           port    -- user port in this unit
 *           security_mode -- which mode to set
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: failed
 * NOTE    : Please refere the definition in MIB header file "leaf_es3626a.h":
 *             #define VAL_portSecAction_none	            1L
 *             #define VAL_portSecAction_trap	            2L
 *             #define VAL_portSecAction_shutdown	        3L
 *             #define VAL_portSecAction_trapAndShutdown	4L
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPortSecurityMode(UI32_T unit_id,
                                       UI32_T port,
                                       UI32_T security_mode)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setportsecuritymode.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setportsecuritymode.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETPORTSECURITYMODE;
    
    /* assgin req data */
    msg_data_p->data.setportsecuritymode.req.unit_id = unit_id;
    msg_data_p->data.setportsecuritymode.req.port = port;
    msg_data_p->data.setportsecuritymode.req.security_mode = security_mode;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

#if (SYS_CPNT_RATE_BASED_STORM_CONTROL == TRUE)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetRateBasedStormControl
 *------------------------------------------------------------------------------
 * PURPOSE : Set rate based storm control
 * INPUT   : unit, port, rate, mode
 * OUTPUT  : None
 * RETURN  : TRUE               -- Success
             FALSE              -- If a given (unit, port) is not available
 * NOTE    : None
 *------------------------------------------------------------------------------
 */
BOOL_T DEV_SWDRVL4_PMGR_SetRateBasedStormControl(UI32_T unit, UI32_T port, UI32_T rate, UI32_T mode)
{
	UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setratebasedstormcontrol.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setratebasedstormcontrol.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;

    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;

    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;

    /* assign req cmd !!*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETRATEBASEDSTORMCONTROL;

    /* assgin req data !!*/
    msg_data_p->data.setratebasedstormcontrol.req.unit = unit;
    msg_data_p->data.setratebasedstormcontrol.req.port = port;
    msg_data_p->data.setratebasedstormcontrol.req.rate = rate;
    msg_data_p->data.setratebasedstormcontrol.req.mode = mode;

    /* send ipc msg */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg !!*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    if(FALSE == msg_data_p->type.result_bool)
        printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);

	return msg_data_p->type.result_bool;
}
#endif

BOOL_T DEV_SWDRVL4_PMGR_SetCosTrustMode(UI32_T unit,
                                          UI32_T port,
                                          UI32_T mode)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcostrustmode.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcostrustmode.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETCOSTRUSTMODE;
    
    /* assgin req data */
    msg_data_p->data.setcostrustmode.req.unit = unit;
    msg_data_p->data.setcostrustmode.req.port = port;
    msg_data_p->data.setcostrustmode.req.mode = mode;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

BOOL_T DEV_SWDRVL4_PMGR_SetQosIngCos2Dscp(UI32_T unit, UI32_T port,UI32_T cos,UI32_T cfi,UI32_T phb,UI32_T color)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosingcos2dscp.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosingcos2dscp.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETCOSINGRESSCOS2DSCP;
    /* assgin req data */
    msg_data_p->data.setcosingcos2dscp.req.unit = unit;
    msg_data_p->data.setcosingcos2dscp.req.port = port;
    msg_data_p->data.setcosingcos2dscp.req.cos = cos;
    msg_data_p->data.setcosingcos2dscp.req.cfi = cfi;
    msg_data_p->data.setcosingcos2dscp.req.phb = phb;
    msg_data_p->data.setcosingcos2dscp.req.color = color;
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

BOOL_T DEV_SWDRVL4_PMGR_SetQosIngPre2Dscp(UI32_T unit, UI32_T port,UI32_T dscp,UI32_T phb,UI32_T color)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosingpre2dscp.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosingpre2dscp.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETCOSINGRESSPRE2DSCP;
    /* assgin req data */
    msg_data_p->data.setcosingpre2dscp.req.unit = unit;
    msg_data_p->data.setcosingpre2dscp.req.port = port;
    msg_data_p->data.setcosingpre2dscp.req.dscp = dscp;
    msg_data_p->data.setcosingpre2dscp.req.phb = phb;
    msg_data_p->data.setcosingpre2dscp.req.color = color;
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

BOOL_T DEV_SWDRVL4_PMGR_SetQosIngDscp2Dscp(UI32_T unit, UI32_T port,UI32_T dscp,UI32_T phb,UI32_T color)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosingdscp2dscp.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosingdscp2dscp.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETCOSINGRESSDSCP2DSCP;
    /* assgin req data */
    msg_data_p->data.setcosingdscp2dscp.req.unit = unit;
    msg_data_p->data.setcosingdscp2dscp.req.port = port;
    msg_data_p->data.setcosingdscp2dscp.req.dscp = dscp;
    msg_data_p->data.setcosingdscp2dscp.req.phb = phb;
    msg_data_p->data.setcosingdscp2dscp.req.color = color;
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

BOOL_T DEV_SWDRVL4_PMGR_SetQosIngDscp2Queue(UI32_T unit, UI32_T port,UI32_T phb,UI32_T queue)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosingdscp2queue.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosingdscp2queue.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETCOSINGRESSDSCP2QUEUE;
    /* assgin req data */
    msg_data_p->data.setcosingdscp2queue.req.unit = unit;
    msg_data_p->data.setcosingdscp2queue.req.port = port;
    msg_data_p->data.setcosingdscp2queue.req.phb = phb;
    msg_data_p->data.setcosingdscp2queue.req.queue = queue;
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

BOOL_T DEV_SWDRVL4_PMGR_SetQosIngDscp2Color(UI32_T unit, UI32_T port,UI32_T phb,UI32_T color)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosingdscp2color.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosingdscp2color.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETCOSINGRESSDSCP2COLOR;
    /* assgin req data */
    msg_data_p->data.setcosingdscp2color.req.unit = unit;
    msg_data_p->data.setcosingdscp2color.req.port = port;
    msg_data_p->data.setcosingdscp2color.req.phb = phb;
    msg_data_p->data.setcosingdscp2color.req.color = color;
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}
BOOL_T DEV_SWDRVL4_PMGR_SetQosIngDscp2Cos(UI32_T unit, UI32_T port,UI32_T phb,UI32_T color,UI32_T cos,UI32_T cfi)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosingcos2dscp.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setcosingcos2dscp.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETCOSINGRESSDSCP2COS;
    /* assgin req data */
    msg_data_p->data.setcosingcos2dscp.req.unit = unit;
    msg_data_p->data.setcosingcos2dscp.req.port = port;
    msg_data_p->data.setcosingcos2dscp.req.cos = cos;
    msg_data_p->data.setcosingcos2dscp.req.cfi = cfi;
    msg_data_p->data.setcosingcos2dscp.req.phb = phb;
    msg_data_p->data.setcosingcos2dscp.req.color = color;
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

#if (SYS_CPNT_SFLOW==TRUE)
/***************************************************************************/
/*sFlow APIs                                                         */
/***************************************************************************/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnablePortSflow
 *------------------------------------------------------------------------------
 * FUNCTION: This function will enable specific port sFlow
 * INPUT   : port -- which port to enable sFlow source
 *           sflow_type--DEV_SWDRVL4_SflowIngressPort
 *                       DEV_SWDRVL4_SflowEgressPort
 *           enable-- TRUE:enable
 *                    FALSE:disable
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_EnablePortSflow(UI32_T unit_id, UI32_T port,BOOL_T enable)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setsflowportstatus.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setsflowportstatus.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETSFLOWPORTSTATUS;
    
    /* assgin req data */
    msg_data_p->data.setsflowportstatus.req.unit_id = unit_id;
    msg_data_p->data.setsflowportstatus.req.port = port;
    msg_data_p->data.setsflowportstatus.req.enable = enable;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortSflowRate
 *------------------------------------------------------------------------------
 * FUNCTION: This function will set specific port  sFlow rate
 * INPUT   : port -- which port to set sFlow rate
 *           sflow_type--DEV_SWDRVL4_SflowIngressPort
 *                       DEV_SWDRVL4_SflowEgressPort
 *           rate--sFlow rate
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPortSflowRate(UI32_T unit_id, UI32_T port,UI32_T rate)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setsflowportrate.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setsflowportrate.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETSFLOWPORTRATE;
    
    /* assgin req data */
    msg_data_p->data.setsflowportrate.req.unit_id = unit_id;
    msg_data_p->data.setsflowportrate.req.port = port;
    msg_data_p->data.setsflowportrate.req.rate = rate;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_GetSflowSampleCount
 *------------------------------------------------------------------------------
 * FUNCTION: This function will get the sFlow sample count 
 * INPUT   : port --which port to get sflow count
 *           sflow_type--ingress sFlow/egress sFlow
 *           count--sFlow sample counter
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_GetSflowSampleCount(UI32_T port,UI32_T *count)
{
    return TRUE;
}


#endif/*#if(SYS_CPNT_SFLOW==TRUE)*/

#if (SYS_CPNT_DOS == TRUE)
/*=====================================================================
 * Exported functions for DOS Protection
 *=====================================================================
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetDosProtectionFilter
 *------------------------------------------------------------------------------
 * FUNCTION: This function will config DoS protection
 * INPUT   : type   - the type of DOS protection to config
 *           enable - TRUE to enable; FALSE to disable.
 * OUTPUT  : None
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetDosProtectionFilter(UI32_T type, BOOL_T enable)
{
    UI32_T rv = 0;
    const UI32_T req_msg_size = DEV_SWDRVL4_PMGR_REQ_CMD_SIZE +
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setsflowportstatus.req);
    const UI32_T resp_msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE + 
                                sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->data.setsflowportstatus.resp);
    const UI32_T msg_size = (req_msg_size > resp_msg_size) ? req_msg_size : resp_msg_size;
	
    UI8_T ipc_msg_buf[SYSFUN_SIZE_OF_MSG(msg_size)];

    SYSFUN_Msg_T *sysfun_msg_p = (SYSFUN_Msg_T *)ipc_msg_buf;

    DEV_SWDRVL4_PMGR_IPCMSG_T *msg_data_p;
    
    memset(ipc_msg_buf, 0, sizeof(ipc_msg_buf));
    /* prepare req msg */
    sysfun_msg_p->cmd = SYS_MODULE_DEV_SWDRVL4;
    sysfun_msg_p->msg_size = req_msg_size;
    
    /* assign req cmd*/
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
    msg_data_p->type.cmd = DEV_SWDRVL4_IPCCMD_SETDOSPROTECTIONFILTER;
    
    /* assgin req data */
    msg_data_p->data.setdosprotection.req.type = type;
    msg_data_p->data.setdosprotection.req.arg = enable;
    
    /* Send IPC */
    if((rv = SYSFUN_SendRequestMsg(ipcmsgq_handle, sysfun_msg_p, SYSFUN_TIMEOUT_WAIT_FOREVER,
        SYSFUN_SYSTEM_EVENT_IPCMSG, resp_msg_size, sysfun_msg_p))!=SYSFUN_OK)
    {
        printf("%s:SYSFUN_SendRequestMsg fail! (%ld)\n", __FUNCTION__, rv);
        return FALSE;
    }

    /* prepare resp msg */
    msg_data_p = (DEV_SWDRVL4_PMGR_IPCMSG_T *)sysfun_msg_p->msg_buf;
	if(FALSE == msg_data_p->type.result_bool)
		printf("%s: return_value=%d\n", __FUNCTION__, msg_data_p->type.result_bool);
    return msg_data_p->type.result_bool;
}
#endif /* (SYS_CPNT_DOS == TRUE) */

