/* Module Name: ISC.C
 * Purpose:
 *      ISC is the short form of Inter-Service-Communication. This module provides four APIs for
 *      CSCs to communicate with other units when stacking. These four APIs are of different
 *      delivery types: unreliable multicast delivery, reliable multicast delivery, reliable
 *      unicast delivery with reply data, and delivery to next unit.
 * Notes:
 *      1. CSCs' Set functions should send ISC packet first, then set CSCs'
 *         local (for a better performance)(otherwise, need to get SA's persmision)
 *      2. CSCs' Set functions should use ISC_SendMCastReliable instead of
 *         ISC_RemoteCall/ISC_Send(for a better performance and more correct)
 *         (otherwise, need to get SA's persmision)
 *      3. CSCs should call ISC APIs only when remote destination unit(s)/bitmap
 *         is(are) all existed (for a better performance)
 * History:
 *
 * Copyright(C)      Accton Corporation, 2005
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <string.h>
#include <stdio.h>
#include "isc.h"

#include <assert.h>


/* NAMING CONSTANT DECLARATIONS
 */

#define ISC_HEADER_LEN  sizeof(ISC_Header_T)

#define ISC_POOL_ID_DEFAULT  0
#define ISC_TRACE_ID_DEFAULT 0

#define NO_PVC_SLEEP_TIME 2

#define ISC_VERSION  0x02

/* how many entity joined the communication
 */
#define ISC_MAX_NBR_OF_UNIT     SYS_ADPT_MAX_NBR_OF_DRIVER_UNIT_PER_STACK

/* define the fragment type of ISC packet
 */
#define ISC_NO_FRAGMENT         0
#define ISC_FIRST_FRAGMENT      1
#define ISC_SECOND_FRAGMENT     2

/* define 802.1p
 */

#define ISC_MIN_OF_8021P       0
#define ISC_MAX_OF_8021P       7

/* DATA TYPE DECLARACTIONS
 */



/* MACRO FUNCTION DECLARATIONS
 */
#define ATOM_EXPRESSION(exp)                \
{                                           \
    SYSFUN_OM_ENTER_CRITICAL_SECTION();     \
    (exp);                                  \
    SYSFUN_OM_LEAVE_CRITICAL_SECTION();     \
}


/* LOCAL FUNCTION DECLARATIONS
 */
#include "sys_module.h"
#include "rule_ctrl.h"

ISC_HANDLER_T isc_handler[ISC_SID_UNSUPPORTED];
ISC_OM_T isc_om[ISC_SID_UNSUPPORTED];

ISC_HANDLER_T* ISC_UT_GetHandlerById(UI32_T id)
{
    if (ISC_SID_UNSUPPORTED > id)
    {
        return &isc_handler[id];
    }

    return NULL;
}

ISC_OM_T * ISC_UT_GetOmById(UI32_T id)
{
    if (ISC_SID_UNSUPPORTED > id)
    {
        return &isc_om[id];
    }

    return NULL;
}

/* EXPORTED FUNCTIONS BODY
 */

/*---------------------------------------------------------------------------------
 * FUNCTION : void ISC_InitiateSystemResources(void)
 *---------------------------------------------------------------------------------
 * PURPOSE  : Initiate system resource for ISC
 * INPUT    : none
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *---------------------------------------------------------------------------------*/
void ISC_InitiateSystemResources(void)
{

}


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_AttachSystemResources
 *---------------------------------------------------------------------------------
 * PURPOSE  : Attach system resource for ISC in the context of the calling process.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *---------------------------------------------------------------------------------*/
void ISC_AttachSystemResources(void)
{
}

/* FUNCTION NAME : ISC_Init
 * PURPOSE:
 *          ISC_Init is called to init values and allocate resource for ISC
 * INPUT:
 *          None
 * OUTPUT:
 *          None
 * RETURN:
 *          None
 * NOTES:
 *          None
 */
void ISC_Init(void)
{
    int i;

    for (i = 0; i < ISC_SID_UNSUPPORTED; i++)
    {
#if (SYS_CPNT_STACKING == TRUE)
//        if (i == ISC_RULE_CTRL_SID)
//        {
//            isc_handler[i].fn = RULE_CTRL_ISC_Handler;
//
//            continue;
//        }
#endif
        isc_handler[i].fn = NULL;

    }
} /* ISC_Init */


/* FUNCTION NAME : ISC_Register_Service_CallBack
 * PURPOSE:
 *          ISC_Register_Service_CallBack is called for any entity who needs ISC's service
 * INPUT:
 *          svc_id      --  Service ID, an calling entiry must have a Service ID to identify itself
 *          svc_func    --  this is func reference, it refers the function invoked once upon
 *                          the arrival of the message belonging to the service.
 * OUTPUT:
 *          None
 * RETURN:
 *          TRUE  -- callback function has been registered successfully
 *          FALSE -- Invalid service ID or duplicted ISC register callback function
 * NOTES:
 *          None
 */
BOOL_T  ISC_Register_Service_CallBack(UI8_T svc_id,  ISC_ServiceFunc_T svc_func )
{
    return TRUE;
}


/* FUNCTION NAME : ISC_Send
 * PURPOSE:
 *          ISC_Send is called for an registered entity who wants to send a message unreliablly
 * INPUT:
 *          dst_bmp     --  the intended receiver in bit map format(Bit0 of dst_bmp is unit1)
 *                          each bit in dst_bmp is corresponding to 16 drv_unit
 *          svc_id      --  Service ID, an calling entiry must have a Service ID to identify itself
 *          mem_ref     --  the pointer to memroy reference of the sending packet
 *          priority    --  priority of the sent packet
 * OUTPUT:
 *          None
 * RETURN:
 *          TRUE    :   the packet is transmited
 *          FALSE   :   the packet is not transmited due to some error or transmitting failed
 * NOTES:
 *          1. This API doesn't process the PDU which needs to be fragmented except from LAN
 */
BOOL_T  ISC_Send(UI16_T              dst_bmp,
                 UI8_T               svc_id,
                 L_MM_Mref_Handle_T  *mem_ref,
                 UI32_T              priority )
{
    return TRUE;
}/* ISC_Send */


/* FUNCTION NAME : ISC_RemoteCall
 * PURPOSE  : ISC_RemoteCall is called for an registered entity who wants to send a
 *            message and wait for its reply
 * INPUT    :
 *          drv_unit    : destination unit id.
 *                          1 ~ n   : Single unit
 *                          where n = SYS_ADPT_MAX_NBR_OF_DRIVER_UNIT_PER_STACK is the max allowable number of unit in stacking
 *          svc_id      : the id of requested service on remote unit.
 *          mem_ref     : the pointer to memroy reference of the sending packet
 *          priority    : priority of the sent packet
 *          try_count   : the number of tries.
 *                        0 means try forever.
 *          time_out    : waiting time for reply, unit :second.
 * OUTPUT   :
 *          rep_buflen  : length of rep_buf
 *          rep_buf     : data replied from the remote service function.
 *                    (This is a pointer pointing to the buffer allocated by user)
 * RETURN   :
 *          TRUE  - the return result saved in reply buffer.
 *          FALSE - can't send this request or time out, reply_frame is not valid.
 * Notes    :
 *            1. it's reply function is ISC_RemoteReply()
 *            2. This API doesn't process the PDU which needs to be fragmented except from LAN
 */
BOOL_T  ISC_RemoteCall(UI8_T              drv_unit,
                       UI8_T              svc_id,
                       L_MM_Mref_Handle_T *mem_ref,
                       UI32_T             priority,
                       UI16_T             rep_buflen,
                       UI8_T              *rep_buf,
                       UI8_T              try_count,
                       UI32_T             time_out   )
{
    assert(ISC_SID_UNSUPPORTED > svc_id);

    if (isc_handler[svc_id].fn != NULL)
    {
        BOOL_T result;
        ISC_Key_T key;

        key.priority = priority;
        key.pvc_no = svc_id;
        key.unit = drv_unit;

        result = isc_handler[svc_id].fn(&key, mem_ref);

        memcpy(rep_buf, isc_handler[svc_id].reply_buf, rep_buflen);

        return TRUE;
    }
    return FALSE;
} /* ISC_RemoteCall */


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_RemoteReply
 *---------------------------------------------------------------------------------
 * PURPOSE  : ISC_RemoteReply is called to send a reply to a remote caller
 * INPUT    : mem_ref -- the pointer to memroy reference of the sending packet
 *            key     -- key for ISC service
 * OUTPUT   : none
 * RETURN   : TRUE    -- the reply message has been transmited
 *            FALSE   -- the reply message has not been transmited
 * Notes    : 1. This is the reply function of ISC_RemoteCall
 *---------------------------------------------------------------------------------*/
BOOL_T ISC_RemoteReply( L_MM_Mref_Handle_T *mem_ref, ISC_Key_T *key)
{

    if (isc_handler[key->pvc_no].fn != NULL)
    {
        ISC_OM_T *isc_om_p;

        isc_om_p = ISC_UT_GetOmById(ISC_RULE_CTRL_SID);
        assert(NULL != isc_om_p);

        memcpy(isc_handler[key->pvc_no].reply_buf,
               mem_ref->parameter_block,
               sizeof(isc_handler[key->pvc_no].reply_buf));

        return TRUE;
    }

    return FALSE;
}

/* FUNCTION NAME : ISC_SendMcastReliable
 * PURPOSE  :   For a registered entity to multicast packets to multiple units
 * INPUT    :   dst_bmp         : destination unit bitmap
 *              svc_id          : the id of requested service on remote unit.
 *              mem_ref         : the pointer to memroy reference of the sending packet
 *              priority        :
 *              try_count       : the number of tries.
 *                                0 means try forever.
 *              time_out        : waiting time for reply, unit :msec.
 *              is_acked_by_app : TRUE  -- CSCs(receiver) will reply ACK by itself and ISC will reply NAK
 *                                         if enqueue failed(Queue in ISC_AGENT) or callback function return FALSE.
 *                                FALSE -- ISC(receiver) will reply ACK or NAK.
 * OUTPUT   : None
 * RETURN   :
                dst_bmp : the result of transmission (0:success; not equal to 0:fail)
 * Notes    : 1. IF(is_acked_by_app==TURE), remote CSC has to reply ISC_ACK by itself(call ISC_MCastReply()).
 *               (similar to ISC_RemoteReply used for ISC_RemoteCall)
 *            2. If (is_acked_by_app==FALSE), ISC will reply ISC_ACK if remote CSC's ISC callback function returns
 *               TRUE, otherwise (return FALSE) reply ISC_NAK. Note that if CSC's ISC callback function is called by
 *               ISC agent task, the callback function return value is not used.
 *            3. This API doesn't process the PDU which needs to be fragmented except from LAN
 *            4. dst_bmp is identified as driver unit. Bit0 of dst_bmp is unit1.
 */
UI16_T ISC_SendMcastReliable (UI16_T                dst_bmp,
                              UI8_T                 svc_id,
                              L_MM_Mref_Handle_T    *mem_ref,
                              UI32_T                priority,
                              UI8_T                 try_count,
                              UI32_T                time_out,
                              BOOL_T                is_acked_by_app)
{
    assert(ISC_SID_UNSUPPORTED > svc_id);

    if (isc_handler[svc_id].fn != NULL)
    {
        BOOL_T result;
        ISC_Key_T key;

        key.priority = priority;
        key.pvc_no = svc_id;
        key.seq_no = dst_bmp;

        result = isc_handler[svc_id].fn(&key, mem_ref);

        if (result != TRUE)
        {
            return 1;
        }
    }

    return 0;
} /* ISC_SendMcastReliable */
