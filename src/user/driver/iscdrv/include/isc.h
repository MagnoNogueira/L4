/* Module Name: ISC.H
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

#ifndef _ISC_H
#define _ISC_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "l_mm.h"
//#include "iuc.h"
//#include "lan_type.h"


/* the number of PVC,
 * Note: can't be larger than 255 because we use 1 byte to save pvc_no in ISC Header
 */
#define ISC_MAX_NBR_OF_PVC      SYS_ADPT_MAX_NBR_OF_TASK_IN_SYSTEM


/* DATA TYPE DECLARATIONS
 */

/*
+---------------------+-----------------+----------------+------------------+
|   version(1 byte)   | opcode(1 byte)  |  SID(1 byte)   |  pvc_no(1 byte)  |
+---------------------+-----------------+----------------+------------------+
|       sequence No(2 bytes)            |       dst_bmp(2 bytes)            |
+---------------------------------------+-----------------------------------+
|       data_len(2 byte)                |Src_unit(1 byte)|priority (1 byte) |
+---------------------------------------------------------------------------+
|                     Service data unit(MTU of IUC - 12)                    |
+---------------------------------------------------------------------------+
*/
typedef struct
{   UI8_T   version;       /* version if ISC     */
    UI8_T   opcode;        /* opcode             */
    UI8_T   sid;           /* Service ID         */
    UI8_T   pvc_no;        /* pvc number         */
    UI16_T  seq_no;        /* sequence number    */
    UI16_T  dst_bmp;       /* destination bitmap */
    UI16_T  fragment:2;    /* the first two bits use for fragment */
    UI16_T  data_len:14;   /* the last 14 bits use for data len   */
    UI8_T   src_unit;      /* source unit */
    UI8_T   priority;      /* for remote unit to reply with this same priority */
}__attribute__((packed, aligned(1)))ISC_Header_T;

/* if ISC_Opcode_T is changed, constant arrays is_opcode_reply_type[] and is_get_PVC[] (in isc.c)
 * need to be modified accordingly
 */
typedef enum
{
    ISC_OP_RCC_REQUEST  =   0,      /* Remote Control Call request (ISC_RemoteCall())*/
    ISC_OP_RCC_REPLY,               /* Remote Control Call reply (ISC_RemoteReply())*/
    ISC_OP_UNRELIABLE_DELIVERY,     /* Send request(ISC_Send())*/
    ISC_OP_RMC_REQUEST,             /* Realiable Multicast request(ACK by ISC)(ISC_SendMcastReliable())*/
    ISC_OP_RMC_RC_REQUEST,          /* Realiable Multicast_Remote Control_request, App does reply(ACK by APP)(ISC_SendMcastReliable()) */
    ISC_OP_ACK,                     /* reply ISC_ACK */
    ISC_OP_NAK,                     /* reply ISC_NAK */
    ISC_OP_NUM_OF_OPCODE
} ISC_Opcode_T;

typedef struct
{
    UI16_T  seq_no;
    UI8_T   unit;
    UI8_T   pvc_no;
    UI8_T   priority;
    UI8_T   reserved[3]; /* make the size of ISC_Key_T the multiple of 4 */
} ISC_Key_T;

typedef enum
{
   ISC_RESERVED_SID = 0,        /* in case we need a SID to present all Serivces or some other purposes  */
   ISC_INTERNAL_SID,            /*for ISC reply packet and backdoor */
   ISC_LAN_DIRECTCALL_SID,
   ISC_LAN_CALLBYAGENT_SID,
   ISC_AMTRDRV_DIRECTCALL_SID,
   ISC_AMTRDRV_CALLBYAGENT_SID,
   ISC_NMTRDRV_SID,
   ISC_SWDRV_SID,
   ISC_LEDDRV_SID,
   ISC_FS_SID,
   ISC_SWDRVL4_SID,
   ISC_SYSDRV_SID,
   ISC_HRDRV_SID,
   ISC_SWDRVL3_SID,
   ISC_SWDRV_CACHE_SID,
   ISC_CFGDB_SID,
   ISC_POEDRV_SID,
   ISC_STK_TPLG_SID,
   ISC_RULE_CTRL_SID,
   ISC_MSL_SID,
  /*MUST read:
   when add the entry of the enum,pls remember you maybe need add the ,
   "isc_abnormal_stack_allowable","ServiceId2ModuleId" in ISC.C and
   "isc_agent_service_mode","isc_agent_handler_func" in ISC_Agent.c if you use isc_agent to receive msg
   sent from master to slave.     add by fen.wang*/
   ISC_SYS_TIME,
   ISC_SID_UNSUPPORTED          /* Unsupported Service = total number + 1 */
} ISC_ServiceId_T;

#pragma mark Add for unit test
typedef struct
{
    union
    {
        struct
        {
            UI32_T unit;
            UI32_T port;
            UI32_T device_id;
            UI32_T pcl_id;
            UI32_T dev_rule_id;

            UI32_T red_cnt;
            UI32_T yellow_cnt;
            UI32_T green_cnt;

            UI32_T number_of_chips;

        } rule_ctrl;
    } u;
} ISC_OM_T ;

typedef struct
{
    BOOL_T (*fn)(ISC_Key_T *key, L_MM_Mref_Handle_T *mref_handle_p);

    UI8_T reply_buf[20];
} ISC_HANDLER_T;


ISC_HANDLER_T* ISC_UT_GetHandlerById(UI32_T id);
ISC_OM_T * ISC_UT_GetOmById(UI32_T id);

#pragma mark End of unit test



/* ISC_Agent use svc_id to identify incoming packet passing to which CSC.
 * When svc_id = ISC_STK_TPLG_SID , the third field will be replaced by rx_port.
 */
typedef BOOL_T (* ISC_ServiceFunc_T)(ISC_Key_T *key, L_MM_Mref_Handle_T *mem_ref, UI8_T svc_id);


/* EXPORTED FUNCTION SPECIFICATIONS
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
void ISC_InitiateSystemResources(void);


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_AttachSystemResources
 *---------------------------------------------------------------------------------
 * PURPOSE  : Attach system resource for ISC in the context of the calling process.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *---------------------------------------------------------------------------------*/
void ISC_AttachSystemResources(void);


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_Init
 *---------------------------------------------------------------------------------
 * PURPOSE  : ISC_Init is called to allocate some resource for ISC
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *---------------------------------------------------------------------------------*/
void ISC_Init(void);


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_EnterMasterMode
 *---------------------------------------------------------------------------------
 * PURPOSE  : ISC_EnterMasterMode is called to Enter Master Mode
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *---------------------------------------------------------------------------------*/
void ISC_EnterMasterMode(void);


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_EnterSlaveMode
 *---------------------------------------------------------------------------------
 * PURPOSE  : ISC_EnterSlaveMode is called to Enter Slave Mode
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *---------------------------------------------------------------------------------*/
void ISC_EnterSlaveMode(void);


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_EnterTransitionMode
 *---------------------------------------------------------------------------------
 * PURPOSE  : ISC_EnterTransitionMode is called to Enter Transition Mode
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *---------------------------------------------------------------------------------*/
void ISC_EnterTransitionMode(void);


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_SetTransitionMode
 *---------------------------------------------------------------------------------
 * PURPOSE  : ISC_SetTransitionMode is called to Enter temporary transition Mode
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *---------------------------------------------------------------------------------*/
void ISC_SetTransitionMode(void);


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_Create_InterCSC_Relation
 *---------------------------------------------------------------------------------
 * PURPOSE  : This function initiates all function pointer registration operations.
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *---------------------------------------------------------------------------------*/
void ISC_Create_InterCSC_Relation(void);


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_Register_Service_CallBack
 *---------------------------------------------------------------------------------
 * PURPOSE  : ISC_Register_Service_CallBack is called for any entity who needs ISC's service
 * INPUT    : svc_id    --  Service ID, an calling entiry must have a Service ID to identify itself
 *            svc_func  --  this is func reference, it refers the function invoked once upon
 *                          the arrival of the message belonging to the service.
 * OUTPUT   : None
 * RETURN   : TRUE  -- callback function has been registered successfully
 *            FALSE -- Invalid service ID or duplicted ISC register callback function
 * NOTES    : None
 *---------------------------------------------------------------------------------*/
BOOL_T  ISC_Register_Service_CallBack(UI8_T svc_id,  ISC_ServiceFunc_T svc_func);


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_Send
 *---------------------------------------------------------------------------------
 * PURPOSE  : ISC_Send is called for an registered entity who wants to send a message unreliablly
 * INPUT    : dst_bmp     --  the intended receiver in bit map format(Bit0 of dst_bmp is unit1)
 *                            each bit in dst_bmp is corresponding to 16 drv_unit
 *            svc_id      --  Service ID, an calling entiry must have a Service ID to identify itself
 *            mem_ref     --  the pointer to memroy reference of the sending packet
 *            priority    --  priority of the sent packet
 * OUTPUT   : None
 * RETURN   : TRUE   --   the packet is transmited
 *            FALSE  --   the packet is not transmited due to some error or transmitting failed
 * NOTES    : None
 */
BOOL_T  ISC_Send (UI16_T dst_bmp, UI8_T svc_id, L_MM_Mref_Handle_T *mem_ref, UI32_T priority);


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_RemoteCall
 *---------------------------------------------------------------------------------
 * PURPOSE  : ISC_RemoteCall is called for an registered entity who wants to send a
 *            message and wait for its reply
 * INPUT    : drv_unit    : destination unit id.
 *                          1 ~ n   : Single unit
 *                          where n = SYS_ADPT_MAX_NBR_OF_DRIVER_UNIT_PER_STACK is the max allowable number of unit in stacking
 *            svc_id      : the id of requested service on remote unit.
 *            mem_ref     : the pointer to memroy reference of the sending packet
 *            priority    : priority of the sent packet
 *            try_count   : the number of tries.
 *                          0 means try forever.
 *            time_out    : waiting time for reply, unit :second.
 * OUTPUT   : rep_buflen  : length of rep_buf
 *            rep_buf     : data replied from the remote service function.
 *                          (This is a pointer pointing to the buffer allocated by user)
 * RETURN   : TRUE  -- the return result saved in reply buffer.
 *            FALSE -- can't send this request or time out, reply_frame is not valid.
 * Notes    : 1. This API doesn't support fragment
 *            2. it's reply function is ISC_RemoteReply()
 *---------------------------------------------------------------------------------*/
BOOL_T  ISC_RemoteCall (UI8_T       drv_unit,   UI8_T   svc_id,
                         L_MM_Mref_Handle_T   *mem_ref,   UI32_T  priority,
                         UI16_T     rep_buflen, UI8_T   *rep_buf,
                         UI8_T     try_count,UI32_T time_out );


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_SendMcastReliable
 *---------------------------------------------------------------------------------
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
 * RETURN   : dst_bmp : the result of transmission (0:success; not equal to 0:fail)
 * Notes    : 1. IF(is_acked_by_app==TURE), remote CSC has to reply ISC_ACK by itself(call ISC_MCastReply()).
 *               (similar to ISC_RemoteReply used for ISC_RemoteCall)
 *            2. If (is_acked_by_app==FALSE), ISC will reply ISC_ACK if remote CSC's ISC callback function returns TRUE.
 *               otherwise (return FALSE) reply ISC_NAK
 *            3. This API doesn't process the PDU which needs to be fragmented
 *            4. dst_bmp is identified as driver unit. Bit0 of dst_bmp is unit1.
 *---------------------------------------------------------------------------------*/
UI16_T ISC_SendMcastReliable (UI16_T    dst_bmp,    UI8_T   svc_id,
                              L_MM_Mref_Handle_T    *mem_ref,   UI32_T  priority,
                              UI8_T try_count,    UI32_T  time_out,
                              BOOL_T    is_acked_by_app);


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_SendPacketToNextHop
 *---------------------------------------------------------------------------------
 * PURPOSE  : Send packet to next hop
 * INPUT    : mem_ref         -- the pointer to memroy reference of the sending packet
 *            Uplink_Downlink -- 1: Uplink
 *                               2: Downlink
 *                               3: Uplink and Downlink
 *            svc_id          -- Service ID, an calling entiry must have a Service ID to identify itself
 * OUTPUT   : None
 * RETURN   : TRUE    --  the packet is transmited
 *            FALSE   --  the packet is not transmited due to some error or transmitting failed
 * Notes    : 1. ISC_SendPacketToNextHop doesn't wait reply (unreliable send)
 *---------------------------------------------------------------------------------*/
BOOL_T ISC_SendPacketToNextHop(L_MM_Mref_Handle_T *mem_ref, UI8_T Uplink_Downlink, UI8_T svc_id);


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
BOOL_T ISC_RemoteReply( L_MM_Mref_Handle_T *mem_ref, ISC_Key_T *key);


/*---------------------------------------------------------------------------------
 * FUNCTION : ISC_MCastReply
 *---------------------------------------------------------------------------------
 * PURPOSE  : reply ISC_ACK or ISC_NAK
 * INPUT    : key  -- key for ISC service
 *            type -- ISC_ACK/ISC_NAK
 * OUTPUT   : none
 * RETURN   : TRUE    -- the reply message has been transmited
 *            FALSE   -- the reply message has not been transmited
 * Notes    : 1. This is the reply function of ISC_SendMcstReliable
 *---------------------------------------------------------------------------------*/
BOOL_T ISC_MCastReply(ISC_Key_T *key, UI8_T type);


/* FUNCTION NAME:   ISC_GetISCHeaderLen
 * PURPOSE:
 *          Get the ISC_HEADER_LEN of ISC packet
 * INPUT:
 *          None
 * OUTPUT:
 *          length
 * RETURN:
 *          TRUE        -- success
 *          FALSE       -- fail
 * NOTES:
 */
BOOL_T ISC_GetISCHeaderLen(UI16_T *length);


void ISC_ProvisionComplete(void);
#if (SYS_CPNT_UNIT_HOT_SWAP == TRUE)

void ISC_HandleHotInsertion(void);

void ISC_HandleHotRemoval(void);

#endif

#endif
