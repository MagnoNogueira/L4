/*------------------------------------------------------------------------------
 * File_Name : COS_VM.h
 *
 * Purpose   : Provide service functions of COS module
 *
 * 2002/2/9    : JJ Young Create
 *
 * Copyright(C)      Accton Corporation, 2002, 2003
 *
 * Note    : Designed for Mercury
 *------------------------------------------------------------------------------
 */
#ifndef _COS_VM_H
#define _COS_VM_H

#include "cos_type.h"
#include "sys_type.h"
#include "sys_adpt.h"
#include "sys_dflt.h"

/*------------------------------------------------------------------------------
 * Naming Constants
 *------------------------------------------------------------------------------
 */

/* MIB value :  MAX_prioIpPortValue  , MAX_prioIpPortValue
                MAX_prioIpDscpValue  , MIN_prioIpDscpValue
                MAX_prioIpPrecCos    , MIN_prioIpPrecCos   */

#define COS_VM_TCPPORT_MAX                         MAX_prioIpPortValue
#define COS_VM_TCPPORT_MIN                         MIN_prioIpPortValue

#define COS_VM_DSCP_MAX                            MAX_prioIpDscpValue
#define COS_VM_DSCP_MIN                            MIN_prioIpDscpValue

#define COS_VM_IPPRECEDENCE_MAX                    MAX_prioIpPrecCos
#define COS_VM_IPPRECEDENCE_MIN                    MIN_prioIpPrecCos

#define COS_VM_COS_MAX                             7
#define COS_VM_COS_MIN                             0


#define QOS_MAPING_ALL_ENTRY                       255
#define QOS_MAPING_ALL_IF                          0xFFFFFFFF

#define MIN_COS_VAL                                0
#define MAX_COS_VAL                                7

#define MIN_CFI_VAL                                0
#define MAX_CFI_VAL                                1

#define MIN_PRI_VAL                                0
#define MAX_PRI_VAL                                7

#define MIN_PRE_VAL                                0
#define MAX_PRE_VAL                                7

#define MIN_PHB_VAL                                0
#define MAX_PHB_VAL                                7

#define MIN_DSCP_VAL                               0
#define MAX_DSCP_VAL                               63

#define MIN_PORT_VAL                               0
#define MAX_PORT_VAL                               65535

#define MIN_PROTOCOL_VAL                           0
#define MAX_PROTOCOL_VAL                           255

#define MIN_COLOR_VAL                              0
#define MAX_COLOR_VAL                              3

#define MIN_QUEUE_VAL                              0
#define MAX_QUEUE_VAL                              SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE - 1


#define MIN_IF_VALUE                               0



#define INGRESS_COS_TO_DSCP_MAPPING_CFI            2
#define INGRESS_DSCP_TO_COS_MAPPING_PHB            8
#define INGRESS_DSCP_TO_COS_MAPPING_COLOR          4
#define INGRESS_COS_TO_DSCP_MAPPING_ENTRY_NMB      8
#define INGRESS_PRE_TO_DSCP_MAPPING_ENTRY_NMB      8
#define INGRESS_DSCP_TO_DSCP_MAPPING_ENTRY_NMB     64
#define EGRESS_DSCP_TO_COS_MAPPING_ENTRY_NMB       64
#define EGRESS_DSCP_TO_PRE_MAPPING_ENTRY_NMB       64
#define EGRESS_DSCP_TO_DSCP_MAPPING_ENTRY_NMB      64

#define DSCP_TO_QUEUE_MAPPING_ENTRY_NMB            8

/*priority to queue*/
#define SWITCH_PORT_NUM                            SYS_ADPT_TOTAL_NBR_OF_LPORT

#define QOS_MAPPING_UNVALID_MAPPING_VALUE          (~0)

#define COS_VM_PRIORITY_USER                       COS_TYPE_PRIORITY_USER
#define COS_VM_PRIORITY_ETS                        COS_TYPE_PRIORITY_ETS
#define COS_VM_PRIORITY_CN                         COS_TYPE_PRIORITY_CN

typedef struct
{
    UI8_T CURRENT_DSCP_TO_QUEUE_MAPPING[MAX_PHB_VAL + 1];
}per_port_pri_queue_t;

typedef struct
{
    UI8_T CURRENT_DSCP_TO_COLOR_MAPPING[MAX_PHB_VAL + 1];
}per_port_pri_color_t;

/*dscp to dscp */
typedef struct
{
    UI32_T phb;
    UI32_T color;
} internal_dscp_t;

typedef struct
{
    UI32_T cos;
    UI32_T cfi;
} outer_cos_t;

typedef struct{
	internal_dscp_t CURRENT_DSCP_TO_DSCP_MAPPING[INGRESS_DSCP_TO_DSCP_MAPPING_ENTRY_NMB];
}per_port_dscp_dscp_t;


/*precedence to dscp */
typedef struct{
     internal_dscp_t CURRENT_PRE_TO_DSCP_MAPPING[INGRESS_PRE_TO_DSCP_MAPPING_ENTRY_NMB];
}per_port_pre_dscp_t;


/*cos +cfi to internal dscp mapping */
typedef struct{
	internal_dscp_t CURRENT_COS_TO_DSCP_MAPPING[INGRESS_COS_TO_DSCP_MAPPING_ENTRY_NMB][INGRESS_COS_TO_DSCP_MAPPING_CFI];
}per_port_cos_dscp_t;


/* internal dscp to cos +cfi mapping */
typedef struct{

       outer_cos_t    CURRENT_DSCP_TO_COS_MAPPING[INGRESS_DSCP_TO_COS_MAPPING_PHB][INGRESS_DSCP_TO_COS_MAPPING_COLOR];

}per_port_dscp_cos_t;

/*   tcp /udp to internal dscp*/

typedef struct
{
   UI32_T ifindex;
   UI8_T protocol;
   UI16_T dport;
   UI8_T phb;
   UI8_T color;

} per_port_l4_dscp_t;

#define TCP_UDP_MAPPING_ENTRY    256

typedef enum{

	tcp_protocol = 6,
	udp_protocol = 17,

}tcp_udp_protocol;


typedef enum
{
   COS_VM_IPPRECEDENCE_MAP_TABLE = LEAF_prioCopyIpPrec,
   COS_VM_DSCP_MAP_TABLE         = LEAF_prioCopyIpDscp,
   COS_VM_TCP_PORT_MAP_TABLE     = LEAF_prioCopyIpPort
}COS_VM_MappingTable_T;

typedef enum
{
    COS_VM_DISABLE_BOTH                 = VAL_prioIpPrecDscpStatus_disabled,
    COS_VM_IPPRECEDENCE_MAPPING_ENABLED = VAL_prioIpPrecDscpStatus_precedence,
    COS_VM_DSCP_MAPPING_ENABLED         = VAL_prioIpPrecDscpStatus_dscp
}COS_VM_MAPPING_FLAG_T;

typedef enum
{
    COS_VM_MAPPING_ENABLED  = VAL_prioIpPortEnableStatus_enabled,
    COS_VM_MAPPING_DISABLED = VAL_prioIpPortEnableStatus_disabled
}COS_VM_TCPPORT_MAPPING_FLAG_T;


typedef enum /* function number */
{
    COS_VM_COS_TASK_TASK_FUNCTION_NUMBER = 0,
    COS_VM_COS_TASK_DISPATCH_MSG_FUNCTION_NUMBER,
    COS_VM_COS_OM_FUNCTION_NUMBER
}COS_VM_COS_FUNCTION_NUMBER_T ;


typedef enum /* error code */
{
    COS_VM_COS_TASK_ERROR_NUMBER = 0,
    COS_VM_COS_TASK_DISPATCH_MSG_ERROR_NUMBER,
    COS_VM_COS_OM_FUNCTION_ERROR_NUMBER

}COS_VM_COS_ERROR_NUMBER_T;

typedef enum {
    COS_MAPPING_MODE = 0,
    PRECEDENCE_MAPPING_MODE,
    DSCP_IF_MAPPING_MODE,
    UNVALID_IF_MAPPING_MODE
} qos_if_trust_mode_t;

typedef enum {
    L2_MAPPING_MODE = 0,
    DSCP_MAPPING_MODE = 2,
} qos_if_mapping_mode_t;

typedef enum
{
    COS_VM_SCHEDULING_WEIGHT_ROUND_ROBIN = 1,
    COS_VM_SCHEDULING_STRICT_PRIORITY,
    COS_VM_SCHEDULING_METHOD_DRR,
    COS_VM_SCHEDULING_METHOD_SP_WRR,
    COS_VM_SCHEDULING_METHOD_SP_DRR,
} COS_VM_SCHEDULING_METHOD_T;

typedef enum
{
    COS_VM_QUEUE_STRICT_STATUS_ENABLED = VAL_prioWrrStrictStatus_enabled,
    COS_VM_QUEUE_STRICT_STATUS_DISABLED = VAL_prioWrrStrictStatus_disabled,
} COS_VM_QUEUE_STRICT_STATUS_T;

typedef enum
{
    COS_VM_CONFIG_TABLE_COS2DSCP,
    COS_VM_CONFIG_TABLE_DSCP2DSCP,
    COS_VM_CONFIG_TABLE_DSCP2COS,
    COS_VM_CONFIG_TABLE_DSCP2COLOR,
    COS_VM_CONFIG_TABLE_PRE2DSCP,
    COS_VM_CONFIG_TABLE_DSCP2QUEUE,
    COS_VM_CONFIG_TABLE_QUEUEWEIGHT,
    COS_VM_CONFIG_TABLE_QUEUEMODE,
    COS_VM_CONFIG_TABLE_TRUSTMODE,
} COS_VM_CONFIG_TABLE_T;

/*------------------------------------------------------------------------------
 * Data Types
 *------------------------------------------------------------------------------
 */

typedef struct
{
    UI16_T tcp_port;
    UI8_T  cos;
    UI32_T mapping_index;
}   TCP_Port_Map_T;



/****************************************************************************/
/*                            Local Function                                */
/***************************************************************************/

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_InitiateSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE:
 * INPUT : None
 * OUTPUT: None
 * RETURN: TRUE/FALSE
 * NOTES : This is local function and only will be called by
 *         COS_VM_EnterMasterMode()
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_Initiate_System_Resources();

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_Create_InterCSC_Relation
 *------------------------------------------------------------------------------
 * PURPOSE: This function initializes all function pointer registration operations.
 * INPUT :  None
 * OUTPUT:  None
 * RETURN:  None
 * NOTES :
 *------------------------------------------------------------------------------
 */
void COS_VM_Create_InterCSC_Relation(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_EnterMasterMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the COS_VM enter the master mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void COS_VM_EnterMasterMode(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_EnterSlaveMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the COS_VM enter the slave mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void COS_VM_EnterSlaveMode(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set the COS_VM into the transition mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void COS_VM_SetTransitionMode(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_EnterTransitionMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will make the COS_VM enter the transition mode.
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   None.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
void COS_VM_EnterTransitionMode(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetOperationMode
 *------------------------------------------------------------------------------
 * PURPOSE:  This function return the current opearion mode of COS's task
 * INPUT:    None.
 * OUTPUT:   None.
 * RETURN:   cos_operation_mode
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Stacking_Mode_T COS_VM_GetOperationMode(void);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetIpprecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will enable or disable tos/DSCP/COS mapping of system
 * INPUT:    flag -- COS_VM_ipprecedence_MAPPING_ENABLED/COS_VM_DSCP_MAPPING_ENABLED
 *                   /COS_VM_DISABLE_BOTH
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:  tos/COS mapping and DSCP/COS mapping can not be enabled simultaneously
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetIpPrecedenceDscpMappingStatus (COS_VM_MAPPING_FLAG_T flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetIpprecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get tos/DSCP/COS mapping status of system
 * INPUT:    none
 * OUTPUT:   flag -- COS_VM_ipprecedence_MAPPING_ENABLED/COS_VM_DSCP_MAPPING_ENABLED
 *                   /COS_VM_DISABLE_BOTH
 * RETURN:   TRUE/FALSE
 * NOTE:  tos/COS mapping and DSCP/COS mapping can not be enabled simultaneously
 *------------------------------------------------------------------------------
 */

BOOL_T COS_VM_GetIpPrecedenceDscpMappingStatus (COS_VM_MAPPING_FLAG_T *flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetRunningIpprecedenceDscpMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of tos/DSCP/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_VM_MAPPING_ENABLED/COS_VM_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE: default value is COS_VM_MAPPING_DISABLED
 *------------------------------------------------------------------------------
 */

SYS_TYPE_Get_Running_Cfg_T COS_VM_GetRunningIpPrecedenceDscpMappingStatus(COS_VM_MAPPING_FLAG_T *flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will enable or disable TcpPort/COS mapping of system
 * INPUT:    flag -- COS_VM_MAPPING_ENABLED/COS_VM_MAPPING_DISABLED
 * OUTPUT:   None
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetTcpPortMappingStatus (COS_VM_TCPPORT_MAPPING_FLAG_T flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of TcpPort/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_VM_MAPPING_ENABLED/COS_VM_MAPPING_DISABLED
 * RETURN:   TRUE/FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetTcpPortMappingStatus (COS_VM_TCPPORT_MAPPING_FLAG_T *flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetRunningTcpPortMappingStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the status of TcpPort/COS mapping of system
 * INPUT:    None
 * OUTPUT:   flag -- COS_VM_MAPPING_ENABLED/COS_VM_MAPPING_DISABLED
 * RETURN:   SYS_TYPE_GET_RUNNING_CFG_FAIL -- error (system is not in MASTER mode)
 *           SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE -- same as default
 *           SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different from default value
 * NOTE: default value is COS_VM_MAPPING_DISABLED
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetRunningTcpPortMappingStatus (COS_VM_TCPPORT_MAPPING_FLAG_T *flag);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a tos/CoS map in tos_map_table of specific
 *           logical port
 * INPUT:    l_port, tos (0..7), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetIpPrecedenceToCos(UI32_T l_port , UI8_T tos , UI32_T cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a tos/CoS map in tos_map_table of specific
 *           logical port
 * INPUT:    l_port, tos (0..7)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetIpPrecedenceToCos(UI32_T l_port, UI8_T tos, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tos/CoS map to its default value
 *           in tos_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelIpPrecedenceToCos(UI32_T l_port, UI8_T tos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelIpPrecedenceLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tos/CoS map to its default value
 *           in tos_map_table of specific logical port
 * INPUT:    l_port, tos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelIpPrecedenceLportAllEntry(UI32_T l_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a Dscp/CoS map in dscp_map_table of specific
 *           logical port
 * INPUT:    l_port, dscp (0..63), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a Dscp/CoS map in dscp_map_table of specific
 *           logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetDSCPToCos(UI32_T l_port, UI8_T dscp, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a Dscp/CoS map to its default value
 *           in dscp_map_table of specific logical port
 * INPUT:    l_port, dscp (0..63)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelDSCPToCos(UI32_T l_port, UI8_T dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelDscpLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a Dscp/CoS map to its default value
 *           in dscp_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelDscpLportAllEntry( UI32_T l_port );

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a TCP Port/CoS map in tcp_port_map_table
 *           of specific logical port
 * INPUT:    l_port, tcp_port (1..65535), cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE: 1. For GetNext functions, the entries in table need to be sorted
 *          according to tcp_port number
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a tcp_port/CoS map in tcp_port_map_table of specific
 *           logical port
 * INPUT:    l_port, tcp_port (0..65535)
 * OUTPUT:   cos (0..7)
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetPortToCos(UI32_T l_port, UI16_T tcp_port, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tcp_port/CoS map to its default value
 *           in tcp_port of specific logical port
 * INPUT:    l_port, tcp_port (0..65535)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelPortToCos(UI32_T l_port, UI16_T tcp_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelPortLportAllEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will reset a tcp_port/CoS map to its default value
 *           in tcp_port_map_table of specific logical port
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelPortLportAllEntry( UI32_T l_port );

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in tos_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           tos -- tos index
 *           cos -- cos value
 * OUTPUT:   l_port, tos -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetNextIpPrecedenceToCos(UI32_T *l_port, UI8_T *tos, UI32_T *cos);
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in dscp_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           dscp -- dscp index
 *           cos -- cos value
 * OUTPUT:   l_port, dscp -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetNextDSCPToCos(UI32_T *l_port, UI8_T *dscp, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get the value of indexed entry
 *           in tcp_port_map_table of specific port and output next index
 * INPUT:    l_port -- logical port number
 *           tcp_port -- dscp index
 *           cos -- cos value
 * OUTPUT:   l_port, tcp_port -- next index
 * RETURN:   True/False
 * NOTE:     The returned value will base on the database of core layer
 *           rather than the database of ASIC
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetNextPortToCos(UI32_T *l_port , I32_T *tcp_port , UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetRunningIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default tos/COS mapping is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT:   l_port
 *          tos
 * OUTPUT:  cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetRunningIpPrecedenceToCos(UI32_T l_port , UI8_T tos, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetRunningDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default DSCP/COS mapping is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT:   l_port
 *          tos
 * OUTPUT:  cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetRunningDSCPToCos(UI32_T l_port , UI8_T dscp, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetRunningPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default TCPPort/COS mapping is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT:   l_port
 *          tos
 * OUTPUT:  cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetRunningPortToCos(UI32_T l_port , UI16_T tcp_port, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetNextRunningIpPrecedenceToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function same as GetRunning but also output next index
 * INPUT:   l_port
 *          tos
 * OUTPUT:  l_port
 *          tos
 *          cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */

SYS_TYPE_Get_Running_Cfg_T COS_VM_GetNextRunningIpPrecedenceToCos(UI32_T *l_port , UI8_T *tos, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetNextRunningDSCPToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function same as GetRunning but also output next index
 * INPUT:   l_port
 *          dscp
 * OUTPUT:  l_port
 *          dscp
 *          cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetNextRunningDSCPToCos(UI32_T *l_port , UI8_T *dscp, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetNextRunningPortToCos
 *------------------------------------------------------------------------------
 * PURPOSE: This function same as GetRunning but also output next index
 * INPUT:   l_port
 *          tcp_port
 * OUTPUT:  l_port
 *          tcp_port
 *          cos
 * RETURN:  status : SYS_TYPE_Get_Running_Cfg_T
 *                    1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *                    2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *                    3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetNextRunningPortToCos(UI32_T *l_port , I32_T *tcp_port, UI32_T *cos);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_AddFirstTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_AddFirstTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_AddTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_AddTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex);
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelLastTrunkMember
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the callback function that will be registed to trunk manager
 * INPUT:    l_port , t_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelLastTrunkMember(UI32_T trunk_ifindex, UI32_T member_ifindex);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_CosLportConfigAsic
 *------------------------------------------------------------------------------
 * PURPOSE:  This is the cos function that will be registed to priority manager
 * INPUT:    l_port
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_CosLportConfigAsic(UI32_T l_port);



/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_PortCopy
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will copy the source port set to destport
 * INPUT:    map_table_type, src_port, dst_port_list
 * OUTPUT:   None
 * RETURN:   True/False
 * NOTE: Since it have to support multi-thread, so it is not possible to separate
 *       parameters configuration from copy operation
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_PortCopy(UI32_T map_table_type, UI32_T src_port, UI8_T *dst_port_list);

/*------------------------------------------------------------------------------
 * FUNCTION NAME - COS_VM_ReportSyslogMessage
 *------------------------------------------------------------------------------
 * PURPOSE  : This function is another interface for each API to use syslog
 *            to report any error message.
 * INPUT    : error_type - the type of error.  Value defined in vlan_type.h
 *            error_msg - value defined in syslog_VM.h
 *            function_name - the specific name of API to which error occured.
 * OUTPUT   : none
 * RETURN   : none
 * NOTES    : none
 *------------------------------------------------------------------------------
 */
void COS_VM_ReportSyslogMessage(UI32_T error_type, UI8_T error_msg, UI8_T *function_name);

/* Added GetFirst APIs */

BOOL_T COS_VM_GetLportFirstDSCPToCos(UI32_T *l_port, UI8_T *dscp, UI32_T *cos);
BOOL_T COS_VM_GetLportFirstIpPrecedenceToCos(UI32_T *l_port, UI8_T *tos, UI32_T *cos);

/* Added Global Configuration APIs
 * Add GLOBAL checking in per-port APIs
 * replace l_port with GLOBAL
 * #define GLOBAL   65535
 */


/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_AddCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  to bind a ACL to the interface that perform remark fuction
 * INPUT:    ifindex      --
 *           cos_entry --
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE
 * NOTE:     Note MARK_TYPE_REMARK_DSCP and MARK_TYPE_REMARK_TOS_PRECEDENCE  can't coexist
 *           key is (ifindex, marker_entry.acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_AddCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);




/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DelCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  bind the ACL to interface that performed remark function
 * INPUT:    intf     --
 *           acl_name
 * OUTPUT:   None.
 * RETURN:   TRUE/FALSE
 * NOTE:     search key is (ifindex , acl_name)
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_DelCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);




/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  to get the marker entry
 * INPUT:    ifindex     --
 *           cos_entry.acl_name
 * OUTPUT:   cos_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     this API suppose only used by SNMP,
 *           COS_VM_GetNextMarkerEntry is enough for CLI and WWW
 *           the search key is (intf, cos_entry->acl_name)
 *
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);







/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL binding to the inteface performaned remark function
 * INPUT:    ifindex     -- logical port
 *           cos_entry.acl_name
 * OUTPUT:   ifindex
 *           cos_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (ifindex, mark_entry->acl_name)
 *           cos_entry->acl_name='\0' to get the frist entry
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetNextCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);




/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SNMPGetNextCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get the next ACL binding to the inteface performaned remark function
 * INPUT:    ifindex     -- logical port
 *           cos_entry.acl_name
 * OUTPUT:   ifindex
 *           cos_entry
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (*ifindex, mark_entry->acl_name)
 *           use the *ifindex=0, cos_entry->acl_name='\0' to get the frist entry
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SNMPGetNextCosEntry(UI32_T *ifindex, COS_TYPE_AclCosEntry_T *cos_entry);


/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningCosEntry
 *------------------------------------------------------------------------------
 * PURPOSE:  get marker entry
 * INPUT:    ifindex     --
 *           cos_entry.acl_name
 * OUTPUT:   cos_entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTE:     the search key is (intf, cos_entry->acl_name)
 *
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetNextRunningCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);



/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetCosEntryRowStatus
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    (ifindex, acl_name) is the key
 *           row_status
 * OUTPUT :  None
 * RETURN:   TRUE/FALSE
 * NOTE:     once in active state, only can be destoryed
 *           marker_entry is valid only the row_status=VAL_markerEntryStatus_createAndGo
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetCosEntryRowStatus(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);



/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetCosEntry_Cos
 *------------------------------------------------------------------------------
 * PURPOSE:  for SNMP to set the remark entry
 * INPUT:    ifindex          -- is one of the search key
 *           cos_entry  --
 *
 * OUTPUT :
 * RETURN:   TRUE/FALSE
 * NOTE:     the search key is (intf, cos_entry->acl_name)
 *           SNMP do not need to care of cos_entry->acl_type
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetCosEntry(UI32_T ifindex, COS_TYPE_AclCosEntry_T *cos_entry);






/*
obsolate part
*/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetIntfIpAclToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    l_port, acl name, cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE: 1. if strlen(acl_name) == 0 , means remove ACL binding, don't care cos parameter.
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetIntfIpAclToCos(UI32_T l_port, UI8_T *acl_name ,UI32_T cos );


/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetIntfMacAclToCos
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will set a ACL for COS mapping
 *           of specific logical port
 * INPUT:    l_port, acl name, cos (0..7)
 * OUTPUT:   None
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE: 1. if strlen(acl_name) == 0 , means remove ACL binding, don't care cos parameter.
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetIntfMacAclToCos(UI32_T l_port, UI8_T *acl_name , UI32_T cos );

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetIntfAclCosMapping
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a ACL for COS mapping
 *           of specific logical port
 * INPUT:    l_port
 * OUTPUT:   acl_name, cos
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetIntfAclCosMapping(UI32_T l_port, COS_AclCosMapping_T *acl_mapping);


/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextIntfAclCosMapping
 *------------------------------------------------------------------------------
 * PURPOSE:  This function will get a ACL for COS mapping
 *           of specific logical port
 * INPUT:    l_port
 * OUTPUT:   acl_name, cos
 * RETURN:   Success --> TRUE; Fail --> FALSE
 * NOTE:     use *lport= 0 to get the first entry
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetNextIntfAclCosMapping(UI32_T *l_port, COS_AclCosMapping_T *acl_mapping);


/*------------------------------------------------------------------------------
 * ROUTINE NAME  -COS_VM_GetRunningIntfAclCosMapping
 *------------------------------------------------------------------------------
 * PURPOSE:  for CLI to get the port acl binding
 * INPUT  :  if_index, entry
 * OUTPUT :  entry
 * RETURN:   1.SYS_TYPE_GET_RUNNING_CFG_FAIL -- system not in MASTER mode
 *           2.SYS_TYPE_GET_RUNNING_CFG_SUCCESS -- different with default
 *           3.SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE-- same as default
 * NOTES  :  if entry->in_ip_acl ='\0' means disable, same as the other variable
 *
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T COS_VM_GetRunningIntfAclCosMapping(UI32_T if_index, COS_AclCosMapping_T *acl_mapping);
/*------------------------------------------------------------------------------
 * FUNCTION NAME - COS_VM_HandleHotInsertion
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will initialize the port OM of the module ports
 *            when the option module is inserted.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     inserted
 *            number_of_port        -- the number of ports on the inserted
 *                                     module
 *            use_default           -- the flag indicating the default
 *                                     configuration is used without further
 *                                     provision applied; TRUE if a new module
 *                                     different from the original one is
 *                                     inserted
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is inserted at a time.
 *------------------------------------------------------------------------------
 */
void COS_VM_HandleHotInsertion(UI32_T starting_port_ifindex, UI32_T number_of_port, BOOL_T use_default);
/*------------------------------------------------------------------------------
 * FUNCTION NAME - COS_VM_HandleHotRemoval
 *------------------------------------------------------------------------------
 * PURPOSE  : This function will clear the port OM of the module ports when
 *            the option module is removed.
 * INPUT    : starting_port_ifindex -- the ifindex of the first module port
 *                                     removed
 *            number_of_port        -- the number of ports on the removed
 *                                     module
 * OUTPUT   : None
 * RETURN   : None
 * NOTE     : Only one module is removed at a time.
 *------------------------------------------------------------------------------
 */
void COS_VM_HandleHotRemoval(UI32_T starting_port_ifindex, UI32_T number_of_port);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - L4_COS_MGR_SetDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: this function will set a dscp entry's status
 * INPUT:   l_port
 *          dscp, cos , status
 * OUTPUT:
 * RETURN:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_SetDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_GetDSCPToCosStatus
 *------------------------------------------------------------------------------
 * PURPOSE: this function will get a dscp entry's status
 * INPUT:   l_port
 *          dscp, cos , status
 * OUTPUT:
 * RETURN:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_GetDSCPToCosStatus(UI32_T l_port, UI8_T dscp, BOOL_T *status);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - COS_VM_IsDSCPToCosDefaultStatus
 *------------------------------------------------------------------------------
 * PURPOSE: this function will get a dscp entry's status
 * INPUT:   l_port
 *          dscp, cos , status
 * OUTPUT:
 * RETURN:
 *------------------------------------------------------------------------------
 */
BOOL_T COS_VM_IsDSCPToCosDefaultStatus(UI32_T l_port, UI8_T dscp, BOOL_T status, BOOL_T *is_default);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_Register_Dscp2CosChanged_CallBack
 *------------------------------------------------------------------------------
 * FUNCTION: Register the call-back function when
 *             dscp/prec to cos mapping is changed.
 * INPUT   : fun -- call back function pointer
 * OUTPUT  : none
 * RETURN  : none
 * NOTE    : none
 *------------------------------------------------------------------------------
 */
void COS_VM_Register_Dscp2CosChanged_CallBack(void (*fun)(UI32_T val, BOOL_T is_dscp));

BOOL_T COS_VM_ResetTCPUDP2Dscp(UI32_T ifindex,UI32_T protocol,UI32_T dport,UI32_T phb,UI32_T color);
BOOL_T COS_VM_SetTCPUDP2Dscp(UI32_T ifindex,UI32_T protocol,UI32_T dport,UI32_T phb,UI32_T color);
BOOL_T COS_VM_GetTCPUDP2Dscp(UI32_T ifindex,UI32_T protocol,UI32_T dport,UI32_T *phb,UI32_T *color);
BOOL_T COS_VM_GetNextTCPUDP2Dscp(UI32_T *ifindex,UI32_T *protocol,UI32_T *dport,UI32_T *phb,UI32_T *color);
BOOL_T COS_VM_GetRunningTCPUDP2Dscp(UI32_T ifindex,UI32_T *protocol,UI32_T *dport,UI32_T *phb,UI32_T *color);

#if (__APPLE__ && __MACH__)
#pragma mark - Qos_map_API
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_EnablePortPriorityMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function uses to enable priority's config of COS
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_VM_PRIORITY_XX
 * OUTPUT:  NONE
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_EnablePortPriorityMode (
    UI32_T l_port,
    UI16_T priority
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_DisablePortPriorityMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function use to disable other CSC config on COS
 * INPUT:   l_port   - logic port
 *          priority - COS_TYPE_PRIORITY_XX
 * OUTPUT:  NONE
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    NONE
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_DisablePortPriorityMode (
    UI32_T l_port,
    UI16_T priority
);

#if (__APPLE__ && __MACH__)
#pragma mark - TRUST_MODE
#endif
BOOL_T COS_VM_SetPortListTrustMode(UI8_T *port_list, UI32_T mode);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the COS trust mode
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          mode     - trust mode, qos_if_trust_mode_t
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortTrustMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the COS trust mode
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  mode     - trust mode, qos_if_trust_mode_t
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Gets the active config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortTrustMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T *mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveTrustMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the actively trust mode
 * INPUT:   l_port   - logic port
 * OUTPUT: priority - the priority of CoS config, ex: COS_TYPE_PRIORITY_XX
 *         mode     - trust mode, qos_if_trust_mode_t
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Gets the active config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveTrustMode(
    UI32_T l_port,
    UI16_T *priority_t,
    UI32_T *mode_p
);

#if (__APPLE__ && __MACH__)
#pragma mark - CoS_DSCP
#endif
BOOL_T COS_VM_SetPortListIngressCos2Dscp(UI8_T *port_list,UI32_T cos,UI32_T cfi,UI32_T phb,UI32_T color);
BOOL_T COS_VM_ResetIngressCos2Dscp(UI32_T lport,UI32_T cos,UI32_T cfi);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the mapping of ingress cos to internal dscp
 * INPUT:   priority - COS_TYPE_PRIORITY_XX
 *          cos      - cos queue
 *          cfi      - cos cfi
 *          phb      - phb
 *          color    - color
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetCos2InternalDscp(
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the mapping of ingress cos to internal dscp
 * INPUT:   priority - COS_TYPE_PRIORITY_XX
 *          cos      - cos queue
 *          cfi      - cos cfi
 * OUTPUT:  phb      - phb
 *          color    - color
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    return the config by input priority
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetCos2InternalDscp(
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *phb,
    UI32_T *color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next the mapping of ingress cos to internal dscp
 * INPUT:   priority - COS_TYPE_PRIORITY_XX
 *          cos      - cos queue
 *          cfi      - cos cfi
 *
 * OUTPUT:  cos_p -
 *          cfi_p -
 *          phb_p -
 *          color_p -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextCos2InternalDscp(
    UI16_T priority,
    UI32_T *cos_p,
    UI32_T *cfi_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActiveCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived ingress cos to internal dscp
 * INPUT:   cos      - cos queue
 *          cfi      - cos cfi
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX
 *          phb        - phb
 *          color      - color
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActiveCos2InternalDscp(
    UI32_T cos,
    UI32_T cfi,
    UI16_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_ResetCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: Resets the mapping of ingress cos to internal dscp
 * INPUT:   priority - COS_TYPE_PRIORITY_XX
 *          cos      - cos queue
 *          cfi      - cos cfi
 * OUTPUT:
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_ResetCos2InternalDscp(
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: Gets next running configuration for ingress cos to internal dscp.
 * INPUT:   phb   - PHB, one value of the internal DSCP
 *          color - Color, one value of the internal DSCP
 *
 * OUTPUT:  cos_p - The <cos, cfi> value mapping to the input <phb, color>.
 *          cfi_p -
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:    Using cos = COS_TYPE_UNKOWN_COS, cfi = COS_TYPE_UNKONW_CFI to
 *          get first mapping.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetNextRunningCos2InternalDscp(
    UI32_T phb,
    UI32_T color,
    UI32_T *cos_p,
    UI32_T *cfi_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the mapping of ingress cos to internal dscp
 * INPUT:   priority - COS_TYPE_PRIORITY_XX
 *          l_port   - logic port
 *          cos      - cos queue
 *          cfi      - cos cfi
 *          phb      - phb
 *          color    - color
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortCos2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the mapping of ingress cos to internal dscp
 * INPUT:   priority - COS_TYPE_PRIORITY_XX
 *          l_port   - logic port
 *          cos      - cos queue
 *          cfi      - cos cfi
 * OUTPUT:  phb      - phb
 *          color    - color
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    return the config by input priority
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortCos2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi,
    UI32_T *phb,
    UI32_T *color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next the mapping of ingress cos to internal dscp
 * INPUT:   l_port_p - logical port
 *          priority - COS_TYPE_PRIORITY_XX
 *          cos_p    -
 *          cfi_p    -
 *
 * OUTPUT:  l_port_p -
 *          cos_p    -
 *          cfi_p    -
 *          phb_p    -
 *          color_p  -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortCos2InternalDscp(
    UI32_T *l_port_p,
    UI16_T priority,
    UI32_T *cos_p,
    UI32_T *cfi_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets actived ingress cos to internal dscp
 * INPUT:   cos      - cos queue
 *          cfi      - cos cfi
 * OUTPUT:  priority_p - COS_TYPE_PRIORITY_XX
 *          phb        - phb
 *          color      - color
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveCos2InternalDscp(
    UI32_T l_port,
    UI32_T cos,
    UI32_T cfi,
    UI16_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_ResetPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: Resets the mapping of ingress cos to internal dscp
 * INPUT:   ifindex   - logic port index
 *          priority - COS_TYPE_PRIORITY_XX
 *          cos      - cos queue
 *          cfi      - cos cfi
 * OUTPUT:
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_ResetPortCos2InternalDscp(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T cos,
    UI32_T cfi
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningPortCos2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: Gets next running configuration for ingress cos to internal dscp.
 * INPUT:   l_port - Logical port index
 *          phb    - PHB, one value of the internal DSCP
 *          color  - Color, one value of the internal DSCP
 *
 * OUTPUT:  cos_p - The <cos, cfi> value mapping to the input <phb, color>.
 *          cfi_p -
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:    Using cos = COS_TYPE_UNKOWN_COS, cfi = COS_TYPE_UNKONW_CFI to
 *          get first mapping.
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetNextRunningPortCos2InternalDscp(
    UI32_T l_port,
    UI32_T phb,
    UI32_T color,
    UI32_T *cos_p,
    UI32_T *cfi_p
);

#if (__APPLE__ && __MACH__)
#pragma mark - IP_PREC_DSCP
#endif
BOOL_T COS_VM_SetPortListIngressPre2Dscp(UI8_T *port_list,UI32_T pre,UI32_T phb,UI32_T color);
BOOL_T COS_VM_GetIngressPre2Dscp(UI32_T lport, UI8_T *phb, UI8_T *color);
BOOL_T COS_VM_ResetIngressPre2Dscp(UI32_T lport);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the Precedence to Dscp
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          pre      - Precedence value
 *          phb      - phb value
 *          color    - color value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPre2InternalDscp(
    UI16_T priority,
    UI32_T pre,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the mapping value (phb and color) by input
 *          IP precednec.
 * INPUT:   priority - The priority of COS config, ex: COS_TYPE_PRIORITY_XX.
 *          pre      - IP precedence.
 *
 * OUTPUT:  phb_p    - phb of precedence.
 *          color_p  - color of precedence.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPre2InternalDscp(
    UI16_T priority,
    UI32_T pre,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActivePre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the active mapping value (phb and color) by input
 *          precedence
 * INPUT:   pre      - IP precedence.
 *
 * OUTPUT:  priority_p - The priority of COS config, ex: COS_TYPE_PRIORITY_XX.
 *          phb_p    - phb of precedence.
 *          color_p  - color of precedence.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActivePre2InternalDscp(
    UI32_T pre,
    UI16_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the Precedence to Dscp
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          pre      - Precedence value
 *          phb      - phb value
 *          color    - color value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortPre2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T pre,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the mapping value (phb and color) by input
 *          IP precednec.
 * INPUT:   l_port   - logic port
 *          priority - The priority of COS config, ex: COS_TYPE_PRIORITY_XX.
 *          pre      - IP precedence.
 *
 * OUTPUT:  phb_p    - phb of precedence.
 *          color_p  - color of precedence.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortPre2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T pre,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE  : This function gets next the mapping of ingress IP precedence to
 *            phb and color.
 * INPUT    : l_port_p - Logical port id.
 *            priority - COS_TYPE_PRIORITY_XX, the priority of COS config.
 *            pre      - IP precednece vlaue
 *            phb_p    - phb value
 *            color_p  - color vlaue
 * OUTPUT   : l_port_p -
 *            phb_p    -
 *            color_p  -
 * RETURN   : COS_TYPE_ERROR_CODE_E
 * NOTE     : Using l_port = 0, pre = COS_TYPE_UNKNOWN_PRE to get first entry.
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortPre2InternalDscp(
    UI32_T *l_port_p,
    UI32_T priority,
    UI32_T *pre_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActivePre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the active mapping value (phb and color) by input
 *          IP precednec.
 * INPUT:   l_port   - logic port
 *          priority - The priority of COS config, ex: COS_TYPE_PRIORITY_XX.
 *          pre      - IP precedence.
 *
 * OUTPUT:  priority_p - The priority of COS config, ex: COS_TYPE_PRIORITY_XX.
 *          phb_p      - phb of precedence.
 *          color_p    - color of precedence.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActivePre2InternalDscp(
    UI32_T l_port,
    UI32_T pre,
    UI16_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetRunningPortPre2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets running config of IP precedence to internal DSCP
 *          mapping table.
 * INPUT:   l_port - Logical port id.
 * OUTPUT:  phb_ar[MAX_PRE_VAL+1]   - Array for the mapping of pre to phb.
 *          color_ar[MAX_PRE_VAL+1] - Array for the mapping of pre to color.
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetRunningPortPre2InternalDscp(
    UI32_T l_port,
    UI32_T *phb_ar,
    UI32_T *color_ar
);

#if (__APPLE__ && __MACH__)
#pragma mark - DSCP_DSCP
#endif
BOOL_T COS_VM_SetPortListIngressDscp2Dscp(UI8_T *port_list,UI32_T o_dscp,UI32_T phb,UI32_T color);
BOOL_T COS_VM_GetIngressDscp2Dscp(UI32_T lport,UI32_T dscp,UI32_T *phb,UI32_T *color);
BOOL_T COS_VM_ResetIngressDscp2Dscp(UI32_T lport,UI32_T dscp);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the Dscp to internal Dscp
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          dscp     - DSCP value
 *          phb      - phb value
 *          color    - color value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetDscp2InternalDscp(
    UI16_T priority,
    UI32_T dscp,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the internal Dscp for Dscp
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          dscp     - dscp value
 * OUTPUT:  phb      - phb of dscp
 *          color    - color of dscp
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetDscp2InternalDscp(
    UI16_T priority,
    UI32_T dscp,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next ingress dscp to internal dscp mapping.
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          dscp_p   - dscp value
 *
 * OUTPUT:  dscp_p     - dscp value
 *          phb_p      - phb of dscp
 *          color_p    - color of dscp
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextDscp2InternalDscp(
    UI16_T priority,
    UI32_T *dscp_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActiveDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the internal Dscp for Dscp
 * INPUT:   dscp     - dscp value
 *
 * OUTPUT:  priority_p - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb_p      - phb of dscp
 *          color_p    - color of dscp
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActiveDscp2InternalDscp(
    UI32_T dscp,
    UI16_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next running config of dscp by input phb
 *          and color.
 * INPUT:   phb     - PHB
 *          color   - Color
 * OUTPUT:  dscp_p - ingress dscp value.
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetNextRunningDscp2InternalDscp(
    UI32_T phb,
    UI32_T color,
    UI32_T *dscp_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the Dscp to internal Dscp
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          dscp     - DSCP value
 *          phb      - phb value
 *          color    - color value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortDscp2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T dscp,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the internal Dscp for Dscp
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          dscp     - dscp value
 * OUTPUT:  phb      - phb of dscp
 *          color    - color of dscp
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortDscp2InternalDscp(
    UI32_T l_port,
    UI16_T priority,
    UI32_T dscp,
    UI32_T *phb,
    UI32_T *color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next ingress dscp to internal dscp mapping.
 * INPUT:   l_port_p - logical port index
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          dscp_p   - dscp value
 *
 * OUTPUT:  l_port_p   - current logical port index
 *          dscp_p     - dscp value
 *          phb_p      - phb of dscp
 *          color_p    - color of dscp
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortDscp2InternalDscp(
    UI32_T *l_port_p,
    UI16_T priority,
    UI32_T *dscp_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the internal Dscp for Dscp
 * INPUT:   l_port - logical port index
 *          dscp   - dscp value
 *
 * OUTPUT:  priority_p - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb_p      - phb of dscp
 *          color_p    - color of dscp
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveDscp2InternalDscp(
    UI32_T l_port,
    UI32_T dscp,
    UI16_T *priority_p,
    UI32_T *phb_p,
    UI32_T *color_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningPortDscp2InternalDscp
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the running config of phb and color for internal
 *          DSCP
 * INPUT:   l_port  - Logical port id
 *          phb     - PHB
 *          color   - Color
 *          l_dscp  - The first ingress dscp whitch is used for search the mapping.
 *
 * OUTPUT:  l_dscp     -
 *          dscp_entry - ingress dscp array, dscp_entry[8]
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetNextRunningPortDscp2InternalDscp(
    UI32_T l_port,
    UI32_T phb,
    UI32_T color,
    UI32_T *dscp_p
);

#if (__APPLE__ && __MACH__)
#pragma mark - PHB_QUEUE
#endif
BOOL_T COS_VM_SetPortListIngressDscp2Queue(UI8_T *port_list,UI32_T phb,UI32_T queue);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_ResetIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: Reset the global mapping of ingress DSCP to Queue
 * INPUT:   l_port
 *          phb
 * OUTPUT:  None.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    This API only sets the user config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_ResetIngressDscp2Queue(
    UI32_T phb
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_ResetPortIngressDscp2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: Reset the mapping of ingress DSCP to Queue by port
 * INPUT:   l_port
 *          phb
 * OUTPUT:  None.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    This API only sets the user config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_ResetPortIngressDscp2Queue(
    UI32_T l_port,
    UI32_T phb
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the global mapping of dscp and queue
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb value
 *          queue    - queue value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetIngressPhb2Queue(
    UI16_T priority,
    UI32_T phb,
    UI32_T queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue id mapping to the input phb
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb
 * OUTPUT:  queue_p  - The queue id mapping to the input phb.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetIngressPhb2Queue(
    UI16_T priority,
    UI32_T phb,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue for all Dscp
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb_p    - phb
 * OUTPUT:  phb_p    - phb
 *          queue_p  - The queue id mapping to the input phb.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Using phb = COS_TYPE_UNKNOWN_PHB to get first.
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextIngressPhb2Queue(
    UI16_T priority,
    UI32_T *phb_p,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActiveIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets active config of queue mapping to the input phb.
 * INPUT:   phb - phb
 * OUTPUT:  priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue    - mapping table of dscp to queue , queue[MAX_PHB_VAL+1]
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Return the acitved configuration.
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActiveIngressPhb2Queue(
    UI32_T phb,
    UI16_T *priority_p,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: Get next running config of queue id which is mapping to input phb.
 * INPUT:   queue - queue id.
 * OUTPUT:  phb_p - phb mapping to the input queue id.
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetNextRunningIngressPhb2Queue(
    UI32_T queue,
    UI32_T *phb_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the mapping of dscp and queue by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb value
 *          queue    - queue value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortIngressPhb2Queue(
    UI32_T l_port,
    UI16_T priority,
    UI32_T phb,
    UI32_T queue
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue for all Dscp by port
 * INPUT:   l_port - Logical port id.
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb
 * OUTPUT:  queue_p  - The queue id mapping to the input phb.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortIngressPhb2Queue(
    UI32_T l_port,
    UI16_T priority,
    UI32_T phb,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the active config of queue by input phb.
 * INPUT:   l_port - Logical port id.
 *          phb      - phb
 * OUTPUT:  priority_p - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue_p    - The queue id mapping to the input phb.
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveIngressPhb2Queue(
    UI32_T l_port,
    UI32_T phb,
    UI16_T *priority_p,
    UI32_T *queue_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextRunningPortIngressPhb2Queue
 *------------------------------------------------------------------------------
 * PURPOSE: Get next running config of queue id which is mapping to input phb.
 * INPUT:   l_port - Logical port
 *          queue - queue id.
 * OUTPUT:  phb_p - phb mapping to the input queue id.
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T(SUCCESS/FAIL/NO_CHANGE)
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetNextRunningPortIngressPhb2Queue(
    UI32_T l_port,
    UI32_T queue,
    UI32_T *phb_p
);

#if (__APPLE__ && __MACH__)
#pragma mark - DSCP_COLOR
#endif
BOOL_T COS_VM_SetIngressDscp2Color(UI32_T lport,UI32_T phb,UI32_T color);
BOOL_T COS_VM_ResetIngressDscp2Color(UI32_T lport,UI32_T phb);
BOOL_T COS_VM_SetPortListIngressDscp2Color(UI8_T *port_list,UI32_T phb,UI32_T color);
BOOL_T COS_VM_GetIngressDscp2Color(UI32_T lport,UI8_T *color);
BOOL_T COS_VM_GetRunningIngressDscp2Color(UI32_T lport,UI8_T *color);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortPriorityIngressDscp2Color
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the mapping of dscp and color
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb value
 *          color    - color value
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortPriorityIngressDscp2Color(
    UI32_T l_port,
    UI16_T priority,
    UI32_T phb,
    UI32_T color
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortPriorityIngressDscp2Color
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the color for all Dscp
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  color    - color of Dscp , color[MAX_PHB_VAL+1]
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortPriorityIngressDscp2Color(
    UI32_T l_port,
    UI16_T priority,
    UI8_T *color
);

#if (__APPLE__ && __MACH__)
#pragma mark - DSCP_COS
#endif
BOOL_T COS_VM_SetIngressDscp2Cos(UI32_T lport,UI32_T phb,UI32_T color,UI32_T cos,UI32_T cfi);
BOOL_T COS_VM_SetPortListIngressDscp2Cos(UI8_T *port_list,UI32_T phb,UI32_T color,UI32_T cos,UI32_T cfi);
BOOL_T COS_VM_ResetIngressDscp2Cos(UI32_T lport,UI32_T phb,UI32_T color);
BOOL_T COS_VM_GetIngressDscp2Cos(UI32_T lport,UI32_T phb,UI32_T color,UI32_T *cos,UI32_T *cfi);
BOOL_T COS_VM_GetNextIngressDscp2Cos(UI32_T *l_port,UI32_T *phb,UI32_T *color,UI32_T *cos,UI32_T *cfi);
BOOL_T COS_VM_GetRunningIngressDscp2Cos(UI32_T lport,UI8_T cos,UI8_T cfi,UI8_T *l_phb,UI8_T *l_color,UI8_T *cos_phb,UI8_T *cos_color);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortPriorityIngressDscp2Cos
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the ingress Dscp to cos
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb value
 *          color    - color vlaue
 * OUTPUT:  cos      - cos vlaue
 *          cfi      - cfi value
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortPriorityIngressDscp2Cos(
    UI32_T l_port,
    UI16_T priority,
    UI32_T phb,
    UI32_T color,
    UI32_T cos,
    UI32_T cfi
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortPriorityIngressDscp2Cos
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the ingress dscp to cos
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          phb      - phb value
 *          color    - color vlaue
 * OUTPUT:  cos      - cos vlaue
 *          cfi      - cfi value
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortPriorityIngressDscp2Cos(
    UI32_T l_port,
    UI16_T priority,
    UI32_T phb,
    UI32_T color,
    UI32_T *cos,
    UI32_T *cfi
);

#if (__APPLE__ && __MACH__)
#pragma mark - Queue mode
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the queue mode by global
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          mode     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Change mode will set all queue to be wrr queue,
 *          and set the wrr weight as old config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetEgressQueueMode(
    UI16_T priority,
    UI32_T mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue mode by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  mode     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Gets the config by input priority
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetEgressQueueMode(
    UI16_T priority,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActiveEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue mode
 * INPUT:
 * OUTPUT: priority_p - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *         mode_p     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActiveEgressQueueMode(
    UI16_T *priority_p,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the queue mode by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          mode     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Change mode will set all queue to be wrr queue,
 *          and set the wrr weight as old config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortEgressQueueMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T mode
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue mode by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_VM_PRIORITY_XX
 * OUTPUT:  mode     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Gets the config by input priority
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortEgressQueueMode(
    UI32_T l_port,
    UI16_T priority,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortPriorityEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next queue mode by port
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 * OUTPUT:  l_port_p - Logical port id
 *          mode     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortEgressQueueMode(
    UI16_T priority,
    UI32_T *l_port_p,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue mode by port
 * INPUT:   l_port   - logic port
 *
 * OUTPUT:  priority_p - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          mode_p     - queue mode, COS_VM_SCHEDULING_METHOD_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Gets the active config
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveEgressQueueMode(
    UI32_T l_port,
    UI16_T *priority_p,
    UI32_T *mode_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_ResetPortEgressQueueMode
 *------------------------------------------------------------------------------
 * PURPOSE: This function resets the queue mode to default by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_VM_PRIORITY_XX
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_ResetPortEgressQueueMode(
    UI32_T l_port,
    UI16_T priority
);

#if (__APPLE__ && __MACH__)
#pragma mark - Queue weight
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the wrr queue weight by port
 * INPUT:   l_port   - logic port
 *          q_id     - queue id
 * OUTPUT:  weight   - queue weight
 * RETURN:  TRUE/FALSE
 * NOTE:    Gets the active config
 *------------------------------------------------------------------------------
 */
BOOL_T
COS_VM_GetPortEgressWrrQueueWeight(
    UI32_T l_port,
    UI32_T q_id,
    UI32_T *weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next wrr queue weight
 * INPUT:   queue_p     -
 *
 * OUTPUT:  queue_p     -
 *          weight_p    - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Input queue (COS_TYPE_UNKNOWN_QUEUE) to get first queue weight,
 *
 *          In hybrid mode, this function still gets the configured wrr weight
 *          for strict-queue
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextEgressWrrQueueWeight(
    UI32_T *queue_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortEgressWrrQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next wrr queue weight and queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex_p   -
 *          queue_p     -
 *
 * OUTPUT:  ifindex_p   -
 *          queue_p     -
 *          weight      - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Input ifindex, queue (0, COS_TYPE_UNKNOWN_QUEUE)
 *          to get first queue weight.
 *
 *          In hybrid mode, this function still gets the configured wrr weight
 *          for strict-queue
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortEgressWrrQueueWeight(
    UI32_T *ifindex_p,
    UI32_T *queue_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetRunningEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the config of wrr queue weight
 * INPUT:   queue     - queue id
 * OUTPUT:  weight_p    - queue weight
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetRunningEgressQueueWeight(
    UI32_T queue,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetRunningPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the config of wrr queue weight by port
 * INPUT:   ifindex   - logic port
 *          queue     - queue id
 * OUTPUT:  weight_p    - queue weight
 * RETURN:  SYS_TYPE_Get_Running_Cfg_T
 * NOTE:
 *------------------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T
COS_VM_GetRunningPortEgressQueueWeight(
    UI32_T ifindex,
    UI32_T queue,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the wrr queue weight by global
 * INPUT:   priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          q_id     - queue id
 *          weight   - queue weight
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetEgressQueueWeight(
    UI16_T priority,
    UI32_T q_id,
    UI32_T weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the wrr queue weight and queue status(strict/wrr)
 * INPUT:   priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue
 * OUTPUT:  weight_p     - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetEgressQueueWeight(
    UI16_T priority,
    UI32_T queue,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next the queue weight
 * INPUT:   priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue_p   - queue id
 *
 * OUTPUT:  queue_p   - queue id
 *          weight_p  - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextEgressQueueWeight(
    UI16_T priority,
    UI32_T *queue_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActiveQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the actived queue weight
 * INPUT:  queue     - queue
 *
 * OUTPUT: priority_p - the actived priority of CoS config
 *         weight_p   - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActiveQueueWeight(
    UI32_T queue,
    UI16_T *priority_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function only used for hybrid queue mode (ex: strict+wrr)
 *          to set the queue to be strict queue or wrr queue
 * INPUT:   priority - The priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue    - Queue id
 *          status   -
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetEgressQueueStrictStatus(
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue status(strict/wrr)
 * INPUT:   priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue
 * OUTPUT:  status_p -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetEgressQueueStrictStatus(
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetActiveEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the actived queue status(strict/wrr)
 * INPUT:   queue     - Queue id
 *
 * OUTPUT:  priority_p - The actived priority of COS config.
 *          status_p   - Strict/RR, COS_VM_QUEUE_STRICT_STATUS_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetActiveEgressQueueStrictStatus(
    UI32_T queue,
    UI16_T *priority_p,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function sets the wrr queue weight by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_VM_PRIORITY_XX
 *          q_id     - queue id
 *          weight   - queue weight
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortEgressQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id,
    UI32_T weight
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the wrr queue weight and queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex   - logic port
 *          priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue id
 * OUTPUT:  weight_p     - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortEgressQueueWeight(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetNextPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets next wrr queue weight.
 * INPUT:   priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *
 * OUTPUT:  l_port_p  - logic port
 *          queue     - queue id
 *          weight_p  - queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    Using l_port = 0 to get first.
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetNextPortEgressQueueWeight(
    UI16_T priority,
    UI32_T *l_port_p,
    UI32_T *queue_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the avtived queue weight
 * INPUT:   ifindex   - Logic port
 *          queue     - Queue id
 *
 * OUTPUT:  priority  - The actived priority of CoS config
 *          weight_p  - Queue weight
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveQueueWeight(
    UI32_T ifindex,
    UI32_T queue,
    UI16_T *priority_p,
    UI32_T *weight_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_ResetPortEgressQueueWeight
 *------------------------------------------------------------------------------
 * PURPOSE: This function resets the wrr queue weight to default by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          q_id     - queue id
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_ResetPortEgressQueueWeight(
    UI32_T l_port,
    UI16_T priority,
    UI32_T q_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetPortEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function only used for hybrid queue mode (ex: strict+wrr)
 *          to set the queue to be strict queue or wrr queue
 * INPUT:   ifindex
 *          priority - The priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue    - Queue id
 *          status   -
 * OUTPUT:  None
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_SetPortEgressQueueStrictStatus(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T status
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex   - logic port
 *          priority  - the priority of COS config, ex: COS_TYPE_PRIORITY_XX
 *          queue     - queue id
 * OUTPUT:  status_p -
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortEgressQueueStrictStatus(
    UI32_T ifindex,
    UI16_T priority,
    UI32_T queue,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_GetPortActiveEgressQueueStrictStatus
 *------------------------------------------------------------------------------
 * PURPOSE: This function gets the actived queue status(strict/wrr)
 *          by port
 * INPUT:   ifindex   - Logic port
 *          queue     - Queue id
 *
 * OUTPUT:  priority_p - The actived priority of COS config.
 *          status_p   - Strict/RR, COS_VM_QUEUE_STRICT_STATUS_T
 * RETURN:  COS_TYPE_ERROR_CODE_E
 * NOTE:
 *------------------------------------------------------------------------------
 */
COS_TYPE_ERROR_CODE_E
COS_VM_GetPortActiveEgressQueueStrictStatus(
    UI32_T ifindex,
    UI32_T queue,
    UI16_T *priority_p,
    COS_VM_QUEUE_STRICT_STATUS_T *status_p
);

/* FUNCTION FOR L4_BACKDOOR */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_Backdoor_ShowCosMappingTable
 *------------------------------------------------------------------------------
 * PURPOSE: This function resets the wrr queue weight to default by port
 * INPUT:   l_port   - logic port
 *          priority - the priority of COS config, ex: COS_VM_PRIORITY_XX
 *          table    - to show which table
 * OUTPUT:  None
 * RETURN:  TRUE/FALSE
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
BOOL_T
COS_VM_Backdoor_ShowCosMappingTable(
    UI32_T l_port,
    UI16_T priority,
    UI32_T table
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - COS_VM_SetDebugMode
 *------------------------------------------------------------------------------
 * PURPOSE: Set debug mode flag
 * INPUT:   enable - TRUE/FALSE
 * OUTPUT:  None.
 * RETURN:  None
 * NOTE:    None
 *------------------------------------------------------------------------------
 */
void
COS_VM_SetDebugMode(
    BOOL_T enable
);

#endif /* _COS_VM_H */

