/* Module Name:	DAI_TYPE.H
 * Purpose:
 *		This module declares all structures used in DAI, structure name and
 *		field name do not be changed.
 *
 * Notes:
 *
 * History:
 *		 Date		-- Modifier,  Reason
 *  0.1 2007.10    --  Joseph Lin, Created
 *
 * Copyright(C)		 Accton	Corporation, 2007.
 */
 
#ifndef _DAI_TYPE_H
#define _DAI_TYPE_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_adpt.h"
#include "sys_dflt.h"
#include "l_mm.h"
#include "leaf_es3626a.h"


/* Function returned value 
 */

/* NAME CONSTANT DECLARATIONS
 */
#define DAI_TYPE_NEED_NO_ADDITIONAL_VALIDATE                0x00000000
#define DAI_TYPE_ADDITIONAL_VALIDATE_DSTMAC                 0x00000001      /* additional checks destination MAC */
#define DAI_TYPE_ADDITIONAL_VALIDATE_IP                     0x00000002      /* additional checks IP address */
#define DAI_TYPE_ADDITIONAL_VALIDATE_SRCMAC                 0x00000004      /* additional checks source MAC */
#define DAI_TYPE_LOG_FUN_NO                                 1
#define DAI_TYPE_LOG_ERR_NO                                 1
#define DAI_TYPE_SN_LAN                                     1 
#define DAI_TYPE_ARP_FORMAT                                 0x0806
#define DAI_TYPE_ARP_OP_REQUEST                             1
#define DAI_TYPE_ARP_OP_REPLY                               2 

 
/* A complier option only for developed test*/
#define DAI_TYPE_DEBUG                          TRUE

/* DAI Global Status, default is disabled. */
#define DAI_TYPE_GLOBAL_ENABLED        VAL_daiGlobalStatus_enabled 
#define DAI_TYPE_GLOBAL_DISABLED       VAL_daiGlobalStatus_disabled 
#define DAI_TYPE_GLOBAL_DEFAULT        SYS_DFLT_DAI_GLOBAL_STATUS

/* DAI Status Per VLAN, default is disabled. */
#define DAI_TYPE_VLAN_ENABLED          VAL_daiVlanStatus_enabled
#define DAI_TYPE_VLAN_DISABLED         VAL_daiVlanStatus_disabled
#define DAI_TYPE_VLAN_DEFAULT          SYS_DFLT_DAI_VLAN_STATUS

/* DAI Additional Check Status, default is disabled. */
#define DAI_TYPE_ADDITIONAL_CHECK_ENABLED     VAL_daiGlobalAdditionalValidStatus_enabled
#define DAI_TYPE_ADDITIONAL_CHECK_DISABLED    VAL_daiGlobalAdditionalValidStatus_disabled
#define DAI_TYPE_ADDITIONAL_CHECK_DEFAULT     SYS_DFLT_DAI_ADDITIONAL_CHECK_STATUS

/* The trust status of the port that would used in DAI, default is untrusted */
#define DAI_TYPE_PORT_TRUSTED                   VAL_daiPortTrustStatus_enabled
#define DAI_TYPE_PORT_UNTRUSTED                 VAL_daiPortTrustStatus_disabled
#define DAI_TYPE_PORT_TRUST_STATUS_DEFAULT      SYS_DFLT_DAI_PORT_TRUST_STATUS

/* The port default rate limit  */
#define DAI_TYPE_PORT_RATE_LIMIT_DEFAULT        SYS_DFLT_DAI_PORT_RATE_LIMIT

/* The default log message number per interval  */
#define DAI_TYPE_LOG_MSG_NBR_PER_INTERVAL_DEFAULT       SYS_DFLT_DAI_LOG_MSG_NBR_PER_INTERVAL

/* The default log message interval  */
#define DAI_TYPE_LOG_MSG_INTERVAL_DEFAULT               SYS_DFLT_DAI_LOG_MSG_INTERVAL

/* The maximum number used in DAI  */
#define DAI_TYPE_MAX_PORT_RATE_LIMIT            SYS_ADPT_DAI_MAX_PORT_RATE_LIMIT
#define DAI_TYPE_MIN_PORT_RATE_LIMIT            SYS_ADPT_DAI_MIN_PORT_RATE_LIMIT
#define DAI_TYPE_MAX_NBR_OF_LOG_ENTRY           SYS_ADPT_DAI_MAX_NBR_OF_LOG_ENTRY
#define DAI_TYPE_LOG_MSG_MAX_INTERVAL           SYS_ADPT_DAI_LOG_MSG_MAX_INTERVAL
#define DAI_TYPE_PORT_RATE_LIMIT_UNLIMITED      0xffffffff


/* The size of the vlan bitmap table mapped with DAI status. */
#define DAI_TYPE_VLAN_BITMAP_SIZE               SYS_ADPT_MAX_VLAN_ID        /* 4094 */
#define DAI_TYPE_VLAN_BITMAP_SIZE_IN_BYTE       DAI_TYPE_VLAN_BITMAP_SIZE/8

/* MACRO FUNCTION DECLARATIONS
 */


/* DATA TYPE DECLARATIONS
 */  
/*
 *  The handling routine of callback function for callback to IPTASK component.
 *  Parameters include :
 *      mem_ref : mref descriptor which hold received packet.
 *      packet_length : the payload of packet, not include frame header.
 *      subnet_addr_type : LAN(1) or PPP or ...
 *      subnet_addr_len  : 6 for MAC in LAN_TYPE
 *      subnet_addr   : real mac address.
 *      rxIfNum : received RIF number.
 *
 */ 
typedef void (*DAI_TYPE_Rx_Phase2ARP_PktHandler_T)(L_MM_Mref_Handle_T *mem_ref, UI32_T packet_length,
                                                      UI32_T subnet_addr_type, UI32_T subnet_addr_len,
                                                      UI8_T *subnet_addr, UI32_T rxIfNum);
enum
{
     DAI_TYPE_OK = 0,                                         
     DAI_TYPE_FAIL,                                       
     DAI_TYPE_INVALID_ARG,                                
     DAI_TYPE_INVALID_RANGE,                              
     DAI_TYPE_LOG_BUFFER_FULL,                            
     DAI_TYPE_LOG_ENTRY_EXISTED,                          
     DAI_TYPE_EXCEED_MAX_RATE_LIMIT,                      
     DAI_TYPE_NO_MORE_ENTRY,                              
     DAI_TYPE_DROP_ARP_PACKET,  
     DAI_TYPE_SEND_MSG_FAIL,                          
};
             
typedef struct DAI_TYPE_VlanInfo_S
{                   
    UI8_T   arp_acl_name[SYS_ADPT_ACL_MAX_NAME_LEN+1];
    BOOL_T  acl_static;                   
} DAI_TYPE_VlanInfo_T;

typedef struct DAI_TYPE_PortInfo_S
{
    UI32_T  rate_limit;             /* rate limit. */
    UI8_T   trust_status;           /* DAI_TYPE_PORT_UNTRUSTED or DAI_TYPE_PORT_TRUSTED */
} DAI_TYPE_PortInfo_T;

typedef struct DAI_TYPE_LogEntry_S
{
    UI32_T vlan_id;
    UI32_T port_num;
    UI32_T src_ip_addr;
    UI32_T dst_ip_addr;
    UI8_T  src_mac[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T  dst_mac[SYS_ADPT_MAC_ADDR_LEN];
} DAI_TYPE_LogEntry_T;

typedef struct DAI_TYPE_PortDosAttack_S
{
    UI32_T cur_tick;
    UI32_T tmp_tick;
    UI32_T count;
    BOOL_T is_first_pkt;
} DAI_TYPE_PortDosAttack_T;

typedef struct DAI_TYPE_DaiStatistics_S
{
     UI32_T  dai_om_log_entry_number;
     UI32_T  dai_om_arp_dos_attack_drop_cnt;
     UI32_T  dai_om_arp_dos_attack_recv_cnt;
     UI32_T  dai_om_total_arp_processed_by_dai;
     UI32_T  dai_om_arp_dropped_by_dstmac_validation;
     UI32_T  dai_om_arp_dropped_by_srcmac_validation;
     UI32_T  dai_om_arp_dropped_by_ip_validation;
     UI32_T  dai_om_arp_dropped_by_arpacl;
     UI32_T  dai_om_arp_dropped_by_dhcpsnp;
} DAI_TYPE_DaiStatistics_T;

typedef struct DAI_TYPE_ArpPktFormat_S
{
    UI16_T  hardwareType;           /* Hardware type                 */
    UI16_T  protocolType;           /* Protocol type                 */
    UI8_T   hardwareLen;            /* Hardware address length       */
    UI8_T   protocolLen;            /* Protocol address length       */
    UI16_T  opcode;                 /* Request or Reply              */
    UI8_T   srcMacAddr[6];          /* Source hardware address       */
    UI32_T  srcIp;                  /* Source IP address             */
    UI8_T   dstMacAddr[6];          /* Destination hardware address  */
    UI32_T  dstIp;                  /* Destination IP address        */
}__attribute__((packed, aligned(1))) DAI_TYPE_ArpPktFormat_T;

typedef enum
{
    DAI_TYPE_SYSTEM_STATE_TRANSITION    = SYS_TYPE_STACKING_TRANSITION_MODE,
    DAI_TYPE_SYSTEM_STATE_MASTER        = SYS_TYPE_STACKING_MASTER_MODE,
    DAI_TYPE_SYSTEM_STATE_SLAVE         = SYS_TYPE_STACKING_SLAVE_MODE,
    DAI_TYPE_SYSTEM_STATE_PROVISION_COMPLETE,
    DAI_TYPE_SYSTEN_STATE_RUNNING
} DAI_TYPE_SYSTEM_STATE_T;

#endif	
