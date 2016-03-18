/*******************************************************************
 *
 *    DESCRIPTION:
 *       Layer 4 Driver Layer API Specifications
 *
 *    AUTHOR:
 *
 *    HISTORY:
 *
 *   By              Date     Ver.   Modification Description
 *   --------------- -------- -----  ---------------------------------------
 *
 *    JJ Young     2002/09/18 v1.0   Revised for XGS project
 *
 *******************************************************************
 */

#ifndef DEV_SWDRVL4_H
#define DEV_SWDRVL4_H



/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "swdrv_type.h"

/* TYPE DECLARATIONS
 * Note:  If MIB supports, take it out
 *        If not, leave it.
 */
typedef enum DEV_SWDRVL4_CosMapType_E
{
    DEV_SWDRVL4_MapTosToCos = 1,
    DEV_SWDRVL4_MapDscpToCos,
    DEV_SWDRVL4_MapTcpPortToCos,
    DEV_SWDRVL4_MapUdpPortToCos
} DEV_SWDRVL4_CosMapType_T;

typedef enum DEV_SWDRVL4_EgressSchedulingMethod_E
{
/* 2008-06-02, Jinfeng.Chen: Add some constants to support FB2,
	and adjust the order to adapt the file swctrl.h - SWCTRL_Egress_Scheduling_Method_E
 */
    DEV_SWDRVL4_WeightedRoundRobinScheduling = 1,
    DEV_SWDRVL4_StrictPriorityScheduling,
    DEV_SWDRVL4_DeficitRoundRobinScheduling,
    DEV_SWDRVL4_SpWithWrrScheduling,
    DEV_SWDRVL4_SpWithDrrScheduling,
    DEV_SWDRVL4_WeightedFairRoundRobinScheduling,
} DEV_SWDRVL4_EgressSchedulingMethod_T;
typedef enum{

	colorGreen=0,
	colorRed=1,
	colorYellow=3,

}color_t;

#if (SYS_CPNT_SFLOW==TRUE)
typedef enum
{
    DEV_SWDRVL4_SflowIngress= 0,
    DEV_SWDRVL4_SflowEgress
} DEV_SWDRVL4_SflowType_T;
#endif/*End of #if(SYS_CPNT_SFLOW==TRUE)*/

#define DEV_SWDRVL4_INGRESS_COS_TO_DSCP_MAPPING_ENTRY_NMB             8
#define DEV_SWDRVL4_INGRESS_COS_TO_DSCP_MAPPING_CFI                   2

#define DEV_SWDRVL4_INGRESS_DSCP_TO_DSCP_MAPPING_ENTRY_NMB           64

#define DEV_SWDRVL4_INGRESS_DSCP_TO_COS_MAPPING_PHB                   8
#define DEV_SWDRVL4_INGRESS_DSCP_TO_COS_MAPPING_COLOR                 4

#define DEV_SWDRVL4_DSCP_TO_QUEUE_MAPPING_ENTRY_NMB                   8

#define DEV_SWDRVL4_MIN_COS_VAL                                       0
#define DEV_SWDRVL4_MAX_COS_VAL                                       7

#define DEV_SWDRVL4_MIN_PRE_VAL                                       0
#define DEV_SWDRVL4_MAX_PRE_VAL                                       7

#define DEV_SWDRVL4_MIN_DSCP_VAL                                      0
#define DEV_SWDRVL4_MAX_DSCP_VAL                                     63

#define DEV_SWDRVL4_MIN_PHB_VAL                                       0
#define DEV_SWDRVL4_MAX_PHB_VAL                                       7

/* Priority to queue mapping*/
#define DEV_SWDRVL4_DFLT_PRI_0_TO_QUEUE       2
#define DEV_SWDRVL4_DFLT_PRI_1_TO_QUEUE       0
#define DEV_SWDRVL4_DFLT_PRI_2_TO_QUEUE       1
#define DEV_SWDRVL4_DFLT_PRI_3_TO_QUEUE       3
#define DEV_SWDRVL4_DFLT_PRI_4_TO_QUEUE       4
#define DEV_SWDRVL4_DFLT_PRI_5_TO_QUEUE       5
#define DEV_SWDRVL4_DFLT_PRI_6_TO_QUEUE       6
#define DEV_SWDRVL4_DFLT_PRI_7_TO_QUEUE       7

typedef struct{
	UI32_T cos;
	UI32_T cfi;
}dev_swdrvl4_outer_cos_t;

/*dscp to dscp */
typedef struct{
	UI32_T phb;
	UI32_T color;

}dev_swdrvl4_internal_dscp_t;

/* -------------------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_Init
 * -------------------------------------------------------------------------------------------
 * PURPOSE : This function allocates the system resource for low level switch l4
 *           driver initiation.
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    : 
 * -------------------------------------------------------------------------------------------
 */
void DEV_SWDRVL4_Init();

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_Create_InterCSC_Relation
 * -------------------------------------------------------------------------
 * FUNCTION: This function initializes all function pointer registration operations.
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    : None
 * -------------------------------------------------------------------------*/
void DEV_SWDRVL4_Create_InterCSC_Relation(void);

/*=====================================================================
 * Exported functions for COS
 *=====================================================================
 */

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnableCosMapping
 * -------------------------------------------------------------------------
 * FUNCTION: This function globally enables the specific COS mapping
 * INPUT   : maptype -- which type of mapping is being enabled
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : 1. If the system support per port enable, this API
 *              enables the specific mapping of all ports
 *           2. This API may need to config the ASIC if the mapping 
 *              is changed from disable to enable.
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_EnableCosMapping(DEV_SWDRVL4_CosMapType_T maptype);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnablePortCosMapping
 * -------------------------------------------------------------------------
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
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_EnablePortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                        UI32_T unit, 
                                        UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DisableCosMapping
 * -------------------------------------------------------------------------
 * FUNCTION: This function globally disables the specific COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : 1. If the system support per port disable, this API
 *              disables the specific mapping of all ports
 *           2. This API may need to config the ASIC if the mapping 
 *              is changed from disable to disable.
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DisableCosMapping(DEV_SWDRVL4_CosMapType_T maptype);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DisablePortCosMapping
 * -------------------------------------------------------------------------
 * FUNCTION: This function per port disables the specific COS mapping            
 * INPUT   : maptype -- which type of mapping is being disabled
 *           unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : If the system soes not support per port disable/disable, 
 *           this API always returns false
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DisablePortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                         UI32_T unit, 
                                         UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetCosMapping
 * -------------------------------------------------------------------------
 * FUNCTION: This function globally sets the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with global enable
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                 UI32_T value, 
                                 UI32_T cos);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortCosMapping
 * -------------------------------------------------------------------------
 * FUNCTION: This function per port sets the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           unit -- physically stacking unit(box) number
 *           port: user port in this unit
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with per port enable
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                     UI32_T unit, 
                                     UI32_T port, 
                                     UI32_T value, 
                                     UI32_T cos);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DeleteCosMapping
 * -------------------------------------------------------------------------
 * FUNCTION: This function globally deletes the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with global disable
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DeleteCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                    UI32_T value, 
                                    UI32_T cos);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DeletePortCosMapping
 * -------------------------------------------------------------------------
 * FUNCTION: This function per port deletes the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           unit -- physically stacking unit(box) number
 *           port: user port in this unit
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with per port disable
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DeletePortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                        UI32_T unit, 
                                        UI32_T port, 
                                        UI32_T value, 
                                        UI32_T cos);
                                        
/*=====================================================================
 * Exported functions for Rate-Limit
 *=====================================================================
 */

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnablePortIngressRateLimit
 * -------------------------------------------------------------------------
 * FUNCTION: his function per port enables the ingress rate limit            
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : Module_ID will be translated from unit, port by device driver
 *           The DevicePortMapping table will be passed down from core layer
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_EnablePortIngressRateLimit(UI32_T unit, 
                                              UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DisablePortIngressRateLimit
 * -------------------------------------------------------------------------
 * FUNCTION: his function per port disables the ingress rate limit            
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : Module_ID will be translated from unit, port by device driver
 *           The DevicePortMapping table will be passed down from core layer
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DisablePortIngressRateLimit(UI32_T unit, 
                                              UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortIngressRateLimit
 * -------------------------------------------------------------------------
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
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortIngressRateLimit(UI32_T unit, 
                                           UI32_T port, 
                                           UI32_T rate);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnablePortEgressRateLimit
 * -------------------------------------------------------------------------
 * FUNCTION: This function enables the port egress rate limit
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/ 
BOOL_T DEV_SWDRVL4_EnablePortEgressRateLimit(UI32_T unit, UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DisablePortEgressRateLimit
 * -------------------------------------------------------------------------
 * FUNCTION: This function disables the port egress rate limit
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DisablePortEgressRateLimit(UI32_T unit, UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortEgressRateLimit
 * -------------------------------------------------------------------------
 * FUNCTION: This function sets the port gress rate limit
 * INPUT   : unit -- device 
 *           port -- which port to set
 *           rate -- port ingress rate limit
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortEgressRateLimit(UI32_T unit, 
                                          UI32_T port, 
                                          UI32_T rate);
 
 
/*=====================================================================
 * Exported functions for Egress Scheduling
 *=====================================================================
 */

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetEgressSchedulingMethod
 * -------------------------------------------------------------------------
 * FUNCTION: This function sets the scheduling method for egress queues
 * INPUT   : method -- scheduling method
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : For Strata Switch, default is StrictPriorityScheduling
 *           This function is for global configuration
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetEgressSchedulingMethod(DEV_SWDRVL4_EgressSchedulingMethod_T method);

/* -------------------------------------------------------------------------
  ROUTINE NAME - DEV_SWDRVL4_SetPortEgressSchedulingMethod
  -------------------------------------------------------------------------
  FUNCTION: This function sets the scheduling method for egress queues per port
  INPUT   :	unit -- which unit to set
 		port -- which port to set
 		method -- scheduling method, currently support two methods
			DEV_SWDRVL4_WeightedFairRoundRobinScheduling
			DEV_SWDRVL4_StrictPriorityScheduling
			DEV_SWDRVL4_WeightedRoundRobinScheduling
			DEV_SWDRVL4_DeficitRoundRobinScheduling
			DEV_SWDRVL4_DeficitRoundRobinScheduling
			DEV_SWDRVL4_SpWithWrrScheduling
  OUTPUT  : None
  RETURN  : True: Successfully, FALSE: Failed
  NOTE    : For Strata Switch, default is StrictPriorityScheduling
            This function is for individual configuration
  -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortEgressSchedulingMethod(UI32_T unit, UI32_T port, DEV_SWDRVL4_EgressSchedulingMethod_T method);


/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetEgressSchedulingMethod
 * -------------------------------------------------------------------------
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
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetEgressSpSchedulingQueues(UI32_T unit_id,
                                               UI32_T port,
                                               UI32_T start_queue_no, 
                                               UI32_T end_queue_no);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortWrrQueueWeight
 * -------------------------------------------------------------------------
 * FUNCTION: This function sets WRR queue weight
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 *           q_id     -- which queue to set
 *           weight   -- which weight to set
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: failed
 * NOTE    : The weight is a relative value to represent the ratio 
 *           among queues
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortWrrQueueWeight(UI32_T unit_id,
                                         UI32_T port,
                                         UI32_T q_id,
                                         UI32_T weight);
 
/*=====================================================================
 * Exported functions for Traffic Segematation
 *=====================================================================
 */ 

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnableTrafficSegmatation
 * -------------------------------------------------------------------------
 * FUNCTION: Enable Traffic Segematation functions
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : 
 *           
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_EnableTrafficSegmatation();


/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DisableTrafficSegmatation
 * -------------------------------------------------------------------------
 * FUNCTION: Disable Traffic Segematation functions
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : 
 *           
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DisableTrafficSegmatation();


/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetTrafficSegmatation
 * -------------------------------------------------------------------------
 * FUNCTION: Set the Traffic Segematation membership
 * INPUT   : uplink_uport_list     - uplink port list member set
 *           uplink_uport_list_cnt - uplink port list member count
 *	         downlink_port_list    - downlink port list member set
 *           downlink_uport_list_cnt - downlink port list member count
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : MSB is port 1
 *           
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetTrafficSegmatation (  SYS_TYPE_Uport_T    *uplink_uport_list, 
                                            UI32_T              uplink_uport_list_cnt,
                                            SYS_TYPE_Uport_T    *downlink_uport_list,
                                            UI32_T              downlink_uport_list_cnt
                                         );


/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_ResetTrafficSegmatation
 * -------------------------------------------------------------------------
 * FUNCTION: Reset the Traffic Segematation (Delete the membership in the H/W)
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : Clear all the private vlan irules
 *           
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_ResetTrafficSegmatation();


/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetTrafficSegmatationByPortlist
 * -------------------------------------------------------------------------
 * FUNCTION: Set the Traffic Segematation membership
 * INPUT   : uplink_port_list     - uplink port list member set
 *	     downlink_port_list    - downlink port list member set
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : MSB is port 1
 *           
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetTrafficSegmatationByPortlist(UI8_T *uplink_port_list, 
                                                   UI8_T *downlink_port_list);

#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE)
#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION)
/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PrivateVlan_Init
 * -------------------------------------------------------------------------
 * FUNCTION: This function will init private vlan database and chip
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PrivateVlan_Init(void);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPrivateVlanPortlistBySessionId
 * -------------------------------------------------------------------------
 * FUNCTION: This function will set the private vlan by session group
 * INPUT   : session_id         -- session id to pvlan group  
 *           uplink_port_list   -- uplink port list
 *           downlink_port_list -- downlink port list
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPrivateVlanPortlistBySessionId(UI32_T session_id,
                                                     UI8_T  *uplink_port_list,
                                                     UI8_T  *downlink_port_list);
                                                     
/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DeletePrivateVlanPortlistBySessionId
 * -------------------------------------------------------------------------
 * FUNCTION: This function will delete the private vlan by session group
 * INPUT   : session_id         -- session id to pvlan group  
 *           uplink_port_list   -- uplink port list
 *           downlink_port_list -- downlink port list
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DeletePrivateVlanPortlistBySessionId(UI32_T session_id,
                                                     UI8_T  *uplink_port_list,
                                                     UI8_T  *downlink_port_list);
                                                                                                          
/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPrivateVlanUplinkPortMode
 * -------------------------------------------------------------------------
 * FUNCTION: This function will set uplink port mode to private vlan,  
 *           means uplink's traffic are blocking each groups
 * INPUT   : Mode  TRUE is blocking, FALSE is forwarding 
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPrivateVlanUplinkPortMode(BOOL_T mode);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPrivateVlanTrunkMode
 * -------------------------------------------------------------------------
 * FUNCTION: This function will set status of private vlan of trunk id
 * INPUT   : session_id
 *           trunk_id
 *           is_uplink
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPrivateVlanTrunkMode(UI32_T session_id, UI32_T trunk_id, BOOL_T is_uplink);
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION) */
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE) */ 


BOOL_T DEV_SWDRVL4_Get_PSEC_Status(UI32_T unit_id, UI32_T port);

/*=====================================================================
 * Exported functions for Port Security
 *=====================================================================
 */ 
BOOL_T DEV_SWDRVL4_EnablePortSecurity(UI32_T unit_id, UI32_T port);

BOOL_T DEV_SWDRVL4_DisablePortSecurity(UI32_T unit_id, UI32_T port);


/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortSecurityMode
 * -------------------------------------------------------------------------
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
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortSecurityMode(UI32_T unit_id, 
                                       UI32_T port,
                                       UI32_T security_mode);


/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetCosTrustMode
 * -------------------------------------------------------------------------
 * FUNCTION: Set CoS trust mode per port
 * INPUT   : unit_id, port, mode
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE
 * NOTE    : 
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetCosTrustMode(UI32_T unit_id,
                                       UI32_T port,
                                       UI32_T mode);

/*=====================================================================
 * Exported functions for DiffServ
 *=====================================================================
 */ 
 
/* see swdrvl4.h prepared for Acute DiffServ design and implementation */


/*------------------------------------------------------------------------------
 * ROUTINE NAME : DEV_SWDRVL4_HandleIPCReqMsg
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Handle the ipc request message for dev_swdrvl4.
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
BOOL_T DEV_SWDRVL4_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p) ;

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetCosIngressCos2Dscp
 * -------------------------------------------------------------------------
 * FUNCTION: Set ingress CoS mapping to internal DSCP per port 
 * INPUT   : unit_id, port, cos, cfi, phb, color
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE
 * NOTE    : key - cos + cfi
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetCosIngressCos2Dscp(UI32_T unit_id,
                                       UI32_T port,UI32_T cos,UI32_T cfi,UI32_T phb,UI32_T color);
/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetCosIngressPre2Dscp
 * -------------------------------------------------------------------------
 * FUNCTION: Set ingress precedence mapping to internal DSCP per port
 * INPUT   : unit_id, port, dscp, phb, color
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE
 * NOTE    : None.
 * -------------------------------------------------------------------------*/

BOOL_T DEV_SWDRVL4_SetCosIngressPre2Dscp(UI32_T unit_id,
                                       UI32_T port,UI32_T dscp,UI32_T phb,UI32_T color);
/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetCosIngressDscp2Dscp
 * -------------------------------------------------------------------------
 * FUNCTION: Set ingress DSCP mapping to internal DSCP per port
 * INPUT   : unit_id, port, dscp, phb, color
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE
 * NOTE    : None.
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetCosIngressDscp2Dscp(UI32_T unit_id,
                                       UI32_T port,UI32_T dscp,UI32_T phb,UI32_T color);
/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetCosIngressDscp2Queue
 * -------------------------------------------------------------------------
 * FUNCTION: Set internal DSCP mapping to queue ID per port
 * INPUT   : unit_id, port, phb, queue
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE
 * NOTE    : None.
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetCosIngressDscp2Queue(UI32_T unit_id,
                                       UI32_T port,UI32_T phb,UI32_T queue);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetCosIngressDscp2Cos
 *------------------------------------------------------------------------------
 * FUNCTION: Set internal DSCP mapping to COS/CFI per port
 * INPUT   : unit_id, port, phb, color
 *           new_cos/new_cfi
 * OUTPUT  : None.
 * RETURN  : TRUE/FALSE
 * NOTE    : None.
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetCosIngressDscp2Cos(UI32_T unit_id,
                                       UI32_T port,UI32_T cos,UI32_T cfi,UI32_T phb,UI32_T color);


#if (SYS_CPNT_SFLOW==TRUE)
/***************************************************************************/
/*sFlow APIs                                                         */
/***************************************************************************/
/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnablePortSflow
 * -------------------------------------------------------------------------
 * FUNCTION: This function will enable specific port sFlow
 * INPUT   : port -- which port to enable sFlow source
 *           sflow_type--DEV_SWDRVL4_SflowIngressPort
 *                       DEV_SWDRVL4_SflowEgressPort
 *           enable-- TRUE:enable
 *                    FALSE:disable
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_EnablePortSflow(UI32_T unit, UI32_T port,BOOL_T enable);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortSflowRate
 * -------------------------------------------------------------------------
 * FUNCTION: This function will set specific port  sFlow rate
 * INPUT   : port -- which port to set sFlow rate
 *           sflow_type--DEV_SWDRVL4_SflowIngressPort
 *                       DEV_SWDRVL4_SflowEgressPort
 *           rate--sFlow rate
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortSflowRate(UI32_T unit, UI32_T port,UI32_T rate);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_GetSflowSampleCount
 * -------------------------------------------------------------------------
 * FUNCTION: This function will get the sFlow sample count 
 * INPUT   : port --which port to get sflow count
 *           sflow_type--ingress sFlow/egress sFlow
 *           count--sFlow sample counter
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_GetSflowSampleCount(UI32_T port,UI32_T *count);

#endif/*#if(SYS_CPNT_SFLOW==TRUE)*/

#if (SYS_CPNT_DOS == TRUE)
/*=====================================================================
 * Exported functions for DOS Protection
 *=====================================================================
 */
/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetDosProtectionFilter
 * -------------------------------------------------------------------------
 * FUNCTION: This function will config DoS protection
 * INPUT   : type   - the type of DOS protection to config
 *           enable - TRUE to enable; FALSE to disable.
 * OUTPUT  : None
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetDosProtectionFilter(UI32_T type, BOOL_T enable);
#endif /* (SYS_CPNT_DOS == TRUE) */

#endif /* DEV_SWDRVL4_H */

