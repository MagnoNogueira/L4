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

#ifndef DEV_SWDRVL4_PMGR_H
#define DEV_SWDRVL4_PMGR_H



/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "swdrv_type.h"
#include "dev_swdrvl4.h"


enum
{
    DEV_SWDRVL4_PMGR_RESP_RESULT_FAIL,
};
#define DEV_SWDRVL4_PMGR_REQ_CMD_SIZE       sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->type.cmd)
#define DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE   sizeof(((DEV_SWDRVL4_PMGR_IPCMSG_T *)0)->type)

typedef struct
{
    union
    {
        UI32_T cmd;          /*cmd fnction id*/
        BOOL_T result_bool;  /*respond bool return*/
        UI32_T result_ui32;  /*respond ui32 return*/
        UI32_T result_i32;  /*respond i32 return*/
    }type;
    union
    {
        union
        {
            struct
            {
            }req;
            struct
            {
            }resp;
        }init;
        union
        {
            struct
            {
                DEV_SWDRVL4_CosMapType_T maptype;
            }req;
            struct
            {
            }resp;
        }enablecosmapping;
        union
        {
            struct
            {
                DEV_SWDRVL4_CosMapType_T maptype;
                UI32_T unit;
                UI32_T port;
            }req;
            struct
            {
            }resp;
        }enableportcosmapping;
        union
        {
            struct
            {
                DEV_SWDRVL4_CosMapType_T maptype;
            }req;
            struct
            {
            }resp;
        }disablecosmapping;
        union
        {
            struct
            {
                DEV_SWDRVL4_CosMapType_T maptype;
                UI32_T unit;
                UI32_T port;
            }req;
            struct
            {
            }resp;
        }disableportcosmapping;
        union
        {
            struct
            {
                DEV_SWDRVL4_CosMapType_T maptype;
                UI32_T value;
                UI32_T cos;
            }req;
            struct
            {
            }resp;
        }setcosmapping;
        union
        {
            struct
            {
                DEV_SWDRVL4_CosMapType_T maptype;
                UI32_T unit;
                UI32_T port;
                UI32_T value;
                UI32_T cos;
            }req;
            struct
            {
            }resp;
        }setportcosmapping;
        union
        {
            struct
            {
                DEV_SWDRVL4_CosMapType_T maptype;
                UI32_T value;
                UI32_T cos;
            }req;
            struct
            {
            }resp;
        }deletecosmapping;
        union
        {
            struct
            {
                DEV_SWDRVL4_CosMapType_T maptype;
                UI32_T unit;
                UI32_T port;
                UI32_T value;
                UI32_T cos;
            }req;
            struct
            {
            }resp;
        }deleteportcosmapping;
        union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
            }req;
            struct
            {
            }resp;
        }enableportingressratelimit;
        union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
            }req;
            struct
            {
            }resp;
        }disableportingressratelimit;
        union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
                UI32_T rate;
            }req;
            struct
            {
            }resp;
        }setportingressratelimit;
        union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
            }req;
            struct
            {
            }resp;
        }enableportegressratelimit;
        union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
            }req;
            struct
            {
            }resp;
        }disableportegressratelimit;
        union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
                UI32_T rate;
            }req;
            struct
            {
            }resp;
        }setportegressratelimit;
        union
        {
            struct
            {
                DEV_SWDRVL4_EgressSchedulingMethod_T method;
				/* 2008-06-02, Jinfeng.Chen: Add two following fields to support scheduling method for per port */
				UI32_T unit;
				UI32_T port;
            }req;
            struct
            {
            }resp;
        }setegressschedulingmethod;
        union
        {
            struct
            {
                UI32_T unit_id;
                UI32_T port;
                UI32_T start_queue_no;
                UI32_T end_queue_no;
            }req;
            struct
            {
            }resp;
        }setegressspschedulingqueues;
        union
        {
            struct
            {
                UI32_T unit_id;
                UI32_T port;
                UI32_T q_id;
                UI32_T weight;
            }req;
            struct
            {
            }resp;
        }setportwrrqueueweight;
        union
        {
            struct
            {
            }req;
            struct
            {
            }resp;
        }enabletrafficsegmatation;
        union
        {
            struct
            {
            }req;
            struct
            {
            }resp;
        }disabletrafficsegmatation;
#if 0
        union
        {
            struct
            {
                SYS_TYPE_Uport_T    uplink_uport_list[SYS_ADPT_TOTAL_NBR_OF_LPORT];
                UI32_T              uplink_uport_list_cnt;
                SYS_TYPE_Uport_T    downlink_uport_list[SYS_ADPT_TOTAL_NBR_OF_LPORT];
                UI32_T              downlink_uport_list_cnt;
            }req;
            struct
            {
            }resp;
        }settrafficsegmatation;
#endif
        union
        {
            struct
            {
            }req;
            struct
            {
            }resp;
        }resettrafficsegmatation;
        union
        {
            struct
            {
                UI32_T session_id;
                UI8_T uplink_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
                UI8_T downlink_port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
            }req;
            struct
            {
            }resp;
        }settrafficsegmatationbyportlist;
        union
        {
            struct
            {
                BOOL_T mode;
            }req;
            struct
            {
            }resp;
        }setprivatevlanuplinkportmode;
        union
        {
            struct
            {
                UI32_T session_id;
                UI32_T trunk_id;
                BOOL_T is_uplink;
            }req;
            struct
            {
            }resp;
        }settrafficsegmatationtrunkmode;
        union
        {
            struct
            {
                UI32_T unit_id;
                UI32_T port;
            }req;
            struct
            {
            }resp;
        }enableportsecurity;
        union
        {
            struct
            {
                UI32_T unit_id;
                UI32_T port;                
            }req;
            struct
            {
            }resp;
        }disableportsecurity;
        union
        {
            struct
            {
                UI32_T unit_id;
                UI32_T port;
                UI32_T security_mode;
            }req;
            struct
            {
            }resp;
        }setportsecuritymode;
        union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
                UI32_T rate;
                UI32_T mode;
            }req;
            struct
            {
            }resp;
        }setratebasedstormcontrol;
        union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
                UI32_T mode;
            }req;
            struct
            {
            }resp;
        }setcostrustmode;
        union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
                UI32_T cos;
                UI32_T cfi;
                UI32_T phb;
                UI32_T color;
	    }req;
            struct
            {
            }resp;
        }setcosingcos2dscp;
	union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
                UI32_T dscp;
                UI32_T phb;
                UI32_T color;
	    }req;
            struct
            {
            }resp;
        }setcosingpre2dscp;
	union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
                UI32_T dscp;
                UI32_T phb;
                UI32_T color;
	    }req;
            struct
            {
            }resp;
        }setcosingdscp2dscp;
	union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
                UI32_T phb;
                UI32_T queue;
	    }req;
            struct
            {
            }resp;
        }setcosingdscp2queue;
	union
        {
            struct
            {
                UI32_T unit;
                UI32_T port;
                UI32_T phb;
                UI32_T color;
	    }req;
            struct
            {
            }resp;
        }setcosingdscp2color;
        union
        {
            struct
            {
                UI32_T unit_id;
                UI32_T port;
                UI32_T enable;
            }req;
            struct
            {
            }resp;
        }setsflowportstatus;
        union
        {
            struct
            {
                UI32_T unit_id;
                UI32_T port;
                UI32_T rate;
            }req;
            struct
            {
            }resp;
        }setsflowportrate;
#if (SYS_CPNT_DOS == TRUE)
        union
        {
            struct
            {
                UI32_T type;
                UI32_T arg;
            }req;
            struct
            {
            }resp;
        }setdosprotection;
#endif
    }data;
}DEV_SWDRVL4_PMGR_IPCMSG_T;

enum
{
    DEV_SWDRVL4_IPCCMD_INIT,
    DEV_SWDRVL4_IPCCMD_ENABLECOSMAPPING,
    DEV_SWDRVL4_IPCCMD_ENABLEPORTCOSMAPPING,
    DEV_SWDRVL4_IPCCMD_DISABLECOSMAPPING,
    DEV_SWDRVL4_IPCCMD_DISABLEPORTCOSMAPPING,
    DEV_SWDRVL4_IPCCMD_SETCOSMAPPING,
    DEV_SWDRVL4_IPCCMD_SETPORTCOSMAPPING,
    DEV_SWDRVL4_IPCCMD_DELETECOSMAPPING,
    DEV_SWDRVL4_IPCCMD_DELETEPORTCOSMAPPING,
    DEV_SWDRVL4_IPCCMD_ENABLEPORTINGRESSRATELIMIT,
    DEV_SWDRVL4_IPCCMD_DISABLEPORTINGRESSRATELIMIT,
    DEV_SWDRVL4_IPCCMD_SETPORTINGRESSRATELIMIT,
    DEV_SWDRVL4_IPCCMD_ENABLEPORTEGRESSRATELIMIT,
    DEV_SWDRVL4_IPCCMD_DISABLEPORTEGRESSRATELIMIT,
    DEV_SWDRVL4_IPCCMD_SETPORTEGRESSRATELIMIT,
    DEV_SWDRVL4_IPCCMD_SETEGRESSSCHEDULINGMETHOD,
    DEV_SWDRVL4_IPCCMD_SETEGRESSSPSCHEDULINGQUEUES,
    DEV_SWDRVL4_IPCCMD_SETPORTWRRQUEUEWEIGHT,
    DEV_SWDRVL4_IPCCMD_ENABLETRAFFICSEGMATATION,
    DEV_SWDRVL4_IPCCMD_DISABLETRAFFICSEGMATATION,
    DEV_SWDRVL4_IPCCMD_SETTRAFFICSEGMATATION,
    DEV_SWDRVL4_IPCCMD_RESETTRAFFICSEGMATATION,
    DEV_SWDRVL4_IPCCMD_SETTRAFFICSEGMATATIONBYPORTLIST,
    DEV_SWDRVL4_IPCCMD_SETPRIVATEVLANPORTLISTBYSESSIONID,
    DEV_SWDRVL4_IPCCMD_DELETEPRIVATEVLANPORTLISTBYSESSIONID,
    DEV_SWDRVL4_IPCCMD_SETPRIVATEVLANUPLINKPORTMODE,
    DEV_SWDRVL4_IPCCMD_SETPRIVATEVLANTRUNKMODE,
    DEV_SWDRVL4_IPCCMD_ENABLEPORTSECURITY,
    DEV_SWDRVL4_IPCCMD_DISABLEPORTSECURITY,
    DEV_SWDRVL4_IPCCMD_SETPORTSECURITYMODE,
    DEV_SWDRVL4_IPCCMD_SETRATEBASEDSTORMCONTROL,
    DEV_SWDRVL4_IPCCMD_SETPORTEGRESSSCHEDULINGMETHOD, /*2008-06-02: Jinfeng.Chen: to support scheduling per port */
    DEV_SWDRVL4_IPCCMD_SETCOSTRUSTMODE,
    DEV_SWDRVL4_IPCCMD_SETCOSINGRESSCOS2DSCP,
    DEV_SWDRVL4_IPCCMD_SETCOSINGRESSPRE2DSCP,
    DEV_SWDRVL4_IPCCMD_SETCOSINGRESSDSCP2DSCP,
    DEV_SWDRVL4_IPCCMD_SETCOSINGRESSDSCP2QUEUE,
    DEV_SWDRVL4_IPCCMD_SETCOSINGRESSDSCP2COLOR,
    DEV_SWDRVL4_IPCCMD_SETCOSINGRESSDSCP2COS,
    DEV_SWDRVL4_IPCCMD_SETSFLOWPORTSTATUS,
    DEV_SWDRVL4_IPCCMD_SETSFLOWPORTRATE,
    DEV_SWDRVL4_IPCCMD_SETDOSPROTECTIONFILTER,          /* SYS_CPNT_DOS */
};

/* -------------------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_InitiateProcessResource
 * -------------------------------------------------------------------------------------------
 * PURPOSE : This function is used to initiate the information for IPC
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    :
 * -------------------------------------------------------------------------------------------
 */
void DEV_SWDRVL4_PMGR_InitiateProcessResource();


/*=====================================================================
 * Exported functions for COS
 *=====================================================================
 */

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_EnableCosMapping
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
BOOL_T DEV_SWDRVL4_PMGR_EnableCosMapping(DEV_SWDRVL4_CosMapType_T maptype);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_EnablePortCosMapping
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
BOOL_T DEV_SWDRVL4_PMGR_EnablePortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                        UI32_T unit,
                                        UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DisableCosMapping
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
BOOL_T DEV_SWDRVL4_PMGR_DisableCosMapping(DEV_SWDRVL4_CosMapType_T maptype);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DisablePortCosMapping
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
BOOL_T DEV_SWDRVL4_PMGR_DisablePortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                         UI32_T unit,
                                         UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetCosMapping
 * -------------------------------------------------------------------------
 * FUNCTION: This function globally sets the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with global enable
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                 UI32_T value,
                                 UI32_T cos);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPortCosMapping
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
BOOL_T DEV_SWDRVL4_PMGR_SetPortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                     UI32_T unit,
                                     UI32_T port,
                                     UI32_T value,
                                     UI32_T cos);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DeleteCosMapping
 * -------------------------------------------------------------------------
 * FUNCTION: This function globally deletes the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with global disable
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DeleteCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                    UI32_T value,
                                    UI32_T cos);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DeletePortCosMapping
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
BOOL_T DEV_SWDRVL4_PMGR_DeletePortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                        UI32_T unit,
                                        UI32_T port,
                                        UI32_T value,
                                        UI32_T cos);

/*=====================================================================
 * Exported functions for Rate-Limit
 *=====================================================================
 */

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_EnablePortIngressRateLimit
 * -------------------------------------------------------------------------
 * FUNCTION: his function per port enables the ingress rate limit
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : Module_ID will be translated from unit, port by device driver
 *           The DevicePortMapping table will be passed down from core layer
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_EnablePortIngressRateLimit(UI32_T unit,
                                              UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DisablePortIngressRateLimit
 * -------------------------------------------------------------------------
 * FUNCTION: his function per port disables the ingress rate limit
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : Module_ID will be translated from unit, port by device driver
 *           The DevicePortMapping table will be passed down from core layer
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DisablePortIngressRateLimit(UI32_T unit,
                                              UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPortIngressRateLimit
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
BOOL_T DEV_SWDRVL4_PMGR_SetPortIngressRateLimit(UI32_T unit,
                                           UI32_T port,
                                           UI32_T rate);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_EnablePortEgressRateLimit
 * -------------------------------------------------------------------------
 * FUNCTION: This function enables the port egress rate limit
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_EnablePortEgressRateLimit(UI32_T unit, UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DisablePortEgressRateLimit
 * -------------------------------------------------------------------------
 * FUNCTION: This function disables the port egress rate limit
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DisablePortEgressRateLimit(UI32_T unit, UI32_T port);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPortEgressRateLimit
 * -------------------------------------------------------------------------
 * FUNCTION: This function sets the port gress rate limit
 * INPUT   : unit -- device
 *           port -- which port to set
 *           rate -- port ingress rate limit
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPortEgressRateLimit(UI32_T unit,
                                          UI32_T port,
                                          UI32_T rate);


/*=====================================================================
 * Exported functions for Egress Scheduling
 *=====================================================================
 */

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetEgressSchedulingMethod
 * -------------------------------------------------------------------------
 * FUNCTION: This function sets the scheduling method for egress queues
 * INPUT   : method -- scheduling method
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : For Strata Switch, default is StrictPriorityScheduling
 *           This function is for global configuration
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetEgressSchedulingMethod(DEV_SWDRVL4_EgressSchedulingMethod_T method);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetEgressSchedulingMethod
 * -------------------------------------------------------------------------
 * FUNCTION: This function sets the scheduling method for egress queues
 * INPUT   : 
			unit -- which unit to set
			port -- which port to set
			method -- scheduling method
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : For Strata Switch, default is StrictPriorityScheduling
 *           This function is for individual configuration
 * -------------------------------------------------------------------------*/

BOOL_T DEV_SWDRVL4_PMGR_SetPortEgressSchedulingMethod(UI32_T unit, UI32_T port, DEV_SWDRVL4_EgressSchedulingMethod_T method);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetEgressSchedulingMethod
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
BOOL_T DEV_SWDRVL4_PMGR_SetEgressSpSchedulingQueues(UI32_T unit_id,
                                               UI32_T port,
                                               UI32_T start_queue_no,
                                               UI32_T end_queue_no);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPortWrrQueueWeight
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
BOOL_T DEV_SWDRVL4_PMGR_SetPortWrrQueueWeight(UI32_T unit_id,
                                         UI32_T port,
                                         UI32_T q_id,
                                         UI32_T weight);

/*=====================================================================
 * Exported functions for Traffic Segematation
 *=====================================================================
 */

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_EnableTrafficSegmatation
 * -------------------------------------------------------------------------
 * FUNCTION: Enable Traffic Segematation functions
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    :
 *
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_EnableTrafficSegmatation();


/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DisableTrafficSegmatation
 * -------------------------------------------------------------------------
 * FUNCTION: Disable Traffic Segematation functions
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    :
 *
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DisableTrafficSegmatation();

#if 0
/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetTrafficSegmatation
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
BOOL_T DEV_SWDRVL4_PMGR_SetTrafficSegmatation (  SYS_TYPE_Uport_T    *uplink_uport_list,
                                            UI32_T              uplink_uport_list_cnt,
                                            SYS_TYPE_Uport_T    *downlink_uport_list,
                                            UI32_T              downlink_uport_list_cnt
                                         );
#endif

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_ResetTrafficSegmatation
 * -------------------------------------------------------------------------
 * FUNCTION: Reset the Traffic Segematation (Delete the membership in the H/W)
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : Clear all the private vlan irules
 *
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_ResetTrafficSegmatation();


/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetTrafficSegmatationByPortlist
 * -------------------------------------------------------------------------
 * FUNCTION: Set the Traffic Segematation membership
 * INPUT   : uplink_port_list     - uplink port list member set
 *	     downlink_port_list    - downlink port list member set
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : MSB is port 1
 *
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetTrafficSegmatationByPortlist(UI8_T *uplink_port_list,
                                                   UI8_T *downlink_port_list);


#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE) 
#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION)
/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPrivateVlanPortlistBySessionId
 * -------------------------------------------------------------------------
 * FUNCTION: This function will set the private vlan by session group
 * INPUT   : session_id         -- session id to pvlan group
 *           uplink_port_list   -- uplink port list
 *           downlink_port_list -- downlink port list
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPrivateVlanPortlistBySessionId(UI32_T session_id,
                                                     UI8_T  *uplink_port_list,
                                                     UI8_T  *downlink_port_list);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_DeletePrivateVlanPortlistBySessionId
 * -------------------------------------------------------------------------
 * FUNCTION: This function will delete the private vlan by session group
 * INPUT   : session_id         -- session id to pvlan group  
 *           uplink_port_list   -- uplink port list
 *           downlink_port_list -- downlink port list
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : session_id is 0, means remove pvlan port throw internal session_id
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_DeletePrivateVlanPortlistBySessionId(UI32_T session_id,
                                                     UI8_T  *uplink_port_list,
                                                     UI8_T  *downlink_port_list);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPrivateVlanUplinkPortMode
 * -------------------------------------------------------------------------
 * FUNCTION: This function will set uplink port mode to private vlan,  
 *           means uplink's traffic are blocking each groups
 * INPUT   : Mode  TRUE is blocking, FALSE is forwarding 
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPrivateVlanUplinkPortMode(BOOL_T mode);

/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPrivateVlanTrunkMode
 * -------------------------------------------------------------------------
 * FUNCTION: This function will set status of private vlan of trunk id
 * INPUT   : session_id
 *           trunk_id
 *           is_uplink
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetPrivateVlanTrunkMode(UI32_T session_id, UI32_T trunk_id, BOOL_T is_uplink);
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION) */
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE) */ 


/*=====================================================================
 * Exported functions for Port Security
 *=====================================================================
 */
BOOL_T DEV_SWDRVL4_PMGR_EnablePortSecurity(UI32_T unit_id, UI32_T port);

BOOL_T DEV_SWDRVL4_PMGR_DisablePortSecurity(UI32_T unit_id, UI32_T port);


/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetPortSecurityMode
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
BOOL_T DEV_SWDRVL4_PMGR_SetPortSecurityMode(UI32_T unit_id,
                                       UI32_T port,
                                       UI32_T security_mode);

#if (SYS_CPNT_RATE_BASED_STORM_CONTROL == TRUE)
/* -------------------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetRateBasedStormControl
 * -------------------------------------------------------------------------------------------
 * PURPOSE : Set rate based storm control
 * INPUT   : unit, port, rate, mode
 * OUTPUT  : None
 * RETURN  : TRUE               -- Success
             FALSE              -- If a given (unit, port) is not available
 * NOTE    : None
 * -------------------------------------------------------------------------------------------
 */
BOOL_T DEV_SWDRVL4_PMGR_SetRateBasedStormControl(UI32_T unit, UI32_T port, UI32_T rate, UI32_T mode);
#endif

BOOL_T DEV_SWDRVL4_PMGR_SetCosTrustMode(UI32_T unit,UI32_T port,UI32_T mode);

BOOL_T DEV_SWDRVL4_PMGR_SetQosIngCos2Dscp(UI32_T unit, UI32_T port,UI32_T cos,UI32_T cfi,UI32_T phb,UI32_T color);

BOOL_T DEV_SWDRVL4_PMGR_SetQosIngPre2Dscp(UI32_T unit, UI32_T port,UI32_T pre,UI32_T phb,UI32_T color);

BOOL_T DEV_SWDRVL4_PMGR_SetQosIngDscp2Dscp(UI32_T unit, UI32_T port,UI32_T dscp,UI32_T phb,UI32_T color);

BOOL_T DEV_SWDRVL4_PMGR_SetQosIngDscp2Queue(UI32_T unit, UI32_T port,UI32_T phb,UI32_T queue);

BOOL_T DEV_SWDRVL4_PMGR_SetQosIngDscp2Cos(UI32_T unit, UI32_T port,UI32_T phb,UI32_T color,UI32_T cos,UI32_T cfi);

BOOL_T DEV_SWDRVL4_PMGR_SetQosIngDscp2Color(UI32_T unit, UI32_T port,UI32_T phb,UI32_T color);
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
BOOL_T DEV_SWDRVL4_PMGR_EnablePortSflow(UI32_T unit_id, UI32_T port,BOOL_T enable);

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
BOOL_T DEV_SWDRVL4_PMGR_SetPortSflowRate(UI32_T unit_id, UI32_T port,UI32_T rate);

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
BOOL_T DEV_SWDRVL4_PMGR_GetSflowSampleCount(UI32_T port,UI32_T *count);

#endif/*#if(SYS_CPNT_SFLOW==TRUE)*/

#if (SYS_CPNT_DOS == TRUE)
/*=====================================================================
 * Exported functions for DOS Protection
 *=====================================================================
 */
/* -------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PMGR_SetDosProtectionFilter
 * -------------------------------------------------------------------------
 * FUNCTION: This function will config DoS protection
 * INPUT   : type   - the type of DOS protection to config
 *           enable - TRUE to enable; FALSE to disable.
 * OUTPUT  : None
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 * -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PMGR_SetDosProtectionFilter(UI32_T type, BOOL_T enable);
#endif /* (SYS_CPNT_DOS == TRUE) */

#endif /* DEV_SWDRVL4_PMGR_H */

