/* FUNCTION NAME: DEV_RM.H
* PURPOSE:
*         Only Suport BCM XGSIII 5630X/5650X/5660X chip
* NOTES:
*
* REASON:
*    DESCRIPTION:
*    CREATOR:
*    Date           04/19/2005
*
* Copyright(C)      Accton Corporation, 2005
*/

#ifndef _DEV_RM_H
#define _DEV_RM_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_cpnt.h"
#include "sys_adpt.h"
#include "sysfun.h"



/* NAMING CONSTANT DECLARATIONS
 */

#if (SYS_CPNT_ROUTING == TRUE)
#define DEVRM_RESERVED_RUEL_FOR_L3_ROUTING              (SYS_ADPT_PE_MAX_NBR_OF_RULE_FOR_L3_LITE_ROUTING)
#else
#define DEVRM_RESERVED_RUEL_FOR_L3_ROUTING              0
#endif /* SYS_CPNT_ROUTING */

#define DEVRM_ADPT_MAX_NBR_OF_RULE_PER_UNIT             (SYS_HWCFG_MAX_NBR_OF_PCE_PER_CHIP - DEVRM_RESERVED_RUEL_FOR_L3_ROUTING)

#define DEVRM_ADPT_MAX_NBR_OF_PCL_PER_UNIT              (SYS_HWCFG_MAX_NBR_OF_PCL_PER_CHIP)

typedef enum
{
    DEVRM_PCL_LOOKUP_0_0 = 0,
    DEVRM_PCL_LOOKUP_0_1,
    DEVRM_PCL_LOOKUP_1,

    DEVRM_PCL_LOOKUP_MIN = DEVRM_PCL_LOOKUP_0_0,
    DEVRM_PCL_LOOKUP_MAX = DEVRM_PCL_LOOKUP_1,

} DEVRM_PCL_LOOKUP_NUMBER_T;

#define ETHERTYPE_ENCAPS 1

/* MACRO FUNCTION DECLARATIONS
 */
/* DATA TYPE DECLARATIONS
 */

typedef struct
{
    UI32_T num_of_chips;
} DEVRM_DevInfo_T;

typedef enum
{
    DEVRM_FIELD_QUALIFY_MASK_SRCPORT            =   0x00000001,
    DEVRM_FIELD_QUALIFY_MASK_TAGGED             =   0x00000002,
    DEVRM_FIELD_QUALIFY_MASK_VID                =   0x00000004,
    DEVRM_FIELD_QUALIFY_MASK_UP                 =   0x00000008,
//    DEVRM_FIELD_QUALIFY_MASK_QOSPROFILE         =   0x00000010,
    DEVRM_FIELD_QUALIFY_MASK_ETHER              =   0x00000020,
    DEVRM_FIELD_QUALIFY_MASK_ENCAP              =   0x00000040,
    DEVRM_FIELD_QUALIFY_MASK_MACDA              =   0x00000080,
    DEVRM_FIELD_QUALIFY_MASK_MACSA              =   0x00000100,
    DEVRM_FIELD_QUALIFY_MASK_IPPROTOCOL         =   0x00000200,
    DEVRM_FIELD_QUALIFY_MASK_DSCP               =   0x00000400,
    DEVRM_FIELD_QUALIFY_MASK_TCPCONTROL         =   0x00000800,
    DEVRM_FIELD_QUALIFY_MASK_L4SRCPORT          =   0x00001000,
    DEVRM_FIELD_QUALIFY_MASK_L4DSTPORT          =   0x00002000,
    DEVRM_FIELD_QUALIFY_MASK_ISARP              =   0x00004000,
    DEVRM_FIELD_QUALIFY_MASK_IPV6EHEXIST        =   0x00008000,
    DEVRM_FIELD_QUALIFY_MASK_IPV6EHHOPBYHOP     =   0x00010000,
    DEVRM_FIELD_QUALIFY_MASK_IPFRAG             =   0x00020000,
    DEVRM_FIELD_QUALIFY_MASK_ISBC               =   0x00040000,
    DEVRM_FIELD_QUALIFY_MASK_IPV4SIP            =   0x00080000,
    DEVRM_FIELD_QUALIFY_MASK_IPV4DIP            =   0x00100000,
    DEVRM_FIELD_QUALIFY_MASK_IPV6SIP            =   0x00200000,
    DEVRM_FIELD_QUALIFY_MASK_IPV6DIP            =   0x00400000,
    DEVRM_FIELD_QUALIFY_MASK_IPV6DIPUPPER       =   0x00800000,
//    DEVRM_FIELD_QUALIFY_MASK_TOS                =   0x01000000,
    DEVRM_FIELD_QUALIFY_MASK_PREC               =   0x02000000,
    DEVRM_FIELD_QUALIFY_MASK_IP6TRAFFICCLASS    =   0x04000000,
    DEVRM_FIELD_QUALIFY_MASK_VTAGUP             =   0x08000000,
    DEVRM_FIELD_QUALIFY_MASK_ICMPTYPE           =   0x10000000,
    DEVRM_FIELD_QUALIFY_MASK_PCL_ID             =   0x20000000,
} DEVRM_FIELD_QUALIFY_MASK_T;

typedef struct
{
    UI32_T          entry_qualify;

    UI32_T          srcip_data;
    UI32_T          srcip_mask;
    UI32_T          dstip_data;
    UI32_T          dstip_mask;

    UI32_T          packetformat_data;
    UI32_T          packetformat_mask;

    int             l4_srcport_data;
    int             l4_srcport_mask;

    int             l4_dstport_data;
    int             l4_dstport_mask;

    UI16_T          outvlan_data;
    UI16_T          outvlan_mask;

    UI16_T          invlan_data;
    UI16_T          invlan_mask;

    UI16_T          ethertype_data;
    UI16_T          ethertype_mask;

    UI8_T           is_tagged;

    UI8_T           ip_fragment_data;
    UI8_T           ip_fragment_mask;

    UI8_T           ipprotocol_data;
    UI8_T           ipprotocol_mask;

    UI8_T           dscp_data;
    UI8_T           dscp_mask;

    UI8_T           prec_data;
    UI8_T           prec_mask;

    UI8_T           ipflags_data;
    UI8_T           ipflags_mask;

    UI8_T           tcpcontrol_data;
    UI8_T           tcpcontrol_mask;

    UI8_T           ttl_data;
    UI8_T           ttl_mask;

    UI8_T           srcip6_data[SYS_ADPT_IPV6_ADDR_LEN];
    UI8_T           srcip6_mask[SYS_ADPT_IPV6_ADDR_LEN];

    UI8_T           dstip6_data[SYS_ADPT_IPV6_ADDR_LEN];
    UI8_T           dstip6_mask[SYS_ADPT_IPV6_ADDR_LEN];

    UI8_T           ip6trafficclass_data;
    UI8_T           ip6trafficclass_mask;

    UI8_T           ip6hoplimit_data;
    UI8_T           ip6hoplimit_mask;

    UI8_T           srcmac_data[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T           srcmac_mask[SYS_ADPT_MAC_ADDR_LEN];

    UI8_T           dstmac_data[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T           dstmac_mask[SYS_ADPT_MAC_ADDR_LEN];

    UI8_T           vtagup_data;
    UI8_T           vtagup_mask;

    UI8_T           icmptype_data;
    UI8_T           icmptype_mask;

    UI8_T           srcport_data;
    UI8_T           srcport_mask;

    UI16_T          pcl_id;
    UI16_T          pcl_id_mask;

} DEVRM_AceEntry_T;

#pragma mark -
#pragma mark packet command

#define DEVRM_PACKET_COMMAND_TABLE    \
    PKT_CMD(UNSPECIFIC)               \
    PKT_CMD(FORWARD)                  \
    PKT_CMD(REDIRECT_TO_CPU)          \
    PKT_CMD(COPY_TO_CPU)              \
    PKT_CMD(HARD_DROP)                \
    PKT_CMD(SOFT_DROP)                \
    PKT_CMD(MIRROR_TO_ANALYZER_PORT)

typedef enum
{
#define PKT_CMD(pktCmd) DEVRM_PKT_CMD_##pktCmd,

    DEVRM_PACKET_COMMAND_TABLE

#undef PKT_CMD
} DEVRM_PACKET_COMMAND_T;

#pragma mark -
#pragma mark vlan command

#define DEVRM_MODIFY_VLAN_COMMAND_TABLE \
    MODIFY_VLAN_CMD(DISABLE)            \
    MODIFY_VLAN_CMD(FOR_ALL)            \
    MODIFY_VLAN_CMD(FOR_UNTAGGED_ONLY)  \
    MODIFY_VLAN_CMD(FOR_TAGGED_ONLY)

typedef enum
{
#define MODIFY_VLAN_CMD(vlanCmd) DEVRM_MODIFY_VLAN_CMD_##vlanCmd,

    DEVRM_MODIFY_VLAN_COMMAND_TABLE

#undef MODIFY_VLAN_CMD
} DEVRM_MODIFY_VLAN_COMMAND_T;

typedef struct
{
    DEVRM_MODIFY_VLAN_COMMAND_T modify_vlan;
    UI16_T                      vid;
    BOOL_T                      add_outer_tag;
} DEVRM_ACTION_VLAN_T;

#pragma mark -
#pragma mark qos command

#define DEVRM_MODIFY_COMMAND_TABLE      \
    MODIFY_CMD(DISABLE)                 \
    MODIFY_CMD(ENABLE)

typedef enum
{
#define MODIFY_CMD(cmd) DEVRM_MODIFY_CMD_##cmd,

    DEVRM_MODIFY_COMMAND_TABLE

#undef MODIFY_CMD
} DEVRM_MODIFY_COMMAND_T;

typedef struct
{
    DEVRM_MODIFY_COMMAND_T      modify_inner_priority;
    DEVRM_MODIFY_COMMAND_T      modify_priority;        /* 1p */
    DEVRM_MODIFY_COMMAND_T      modify_queue;
    DEVRM_MODIFY_COMMAND_T      modify_dscp;
    DEVRM_MODIFY_COMMAND_T      modify_tos;
    DEVRM_MODIFY_COMMAND_T      drop_precedence;

    UI8_T                       inner_priority;
    UI8_T                       priority;               /* 1p */
    UI8_T                       queue;
    UI8_T                       dscp;
    UI8_T                       tos;
} DEVRM_ACTION_QOS_T;

#pragma mark -
#pragma mark policer

#define DEVRM_POLICER_MODE_TABLE        \
    POLICER_MODE(DISABLE)               \
    POLICER_MODE(METER_ONLY)            \
    POLICER_MODE(COUNTER_ONLY)          \
    POLICER_MODE(METER_AND_COUNTER)

typedef enum
{
#define POLICER_MODE(policerMode) DEVRM_POLICER_MODE_##policerMode,

    DEVRM_POLICER_MODE_TABLE

#undef POLICER_MODE

} DEVRM_POLICER_MODE_T;

#define DEVRM_POLICER_METER_MODE_TABLE  \
    METER_MODE(FLOW)                    \
    METER_MODE(TRTCM_COLOR_BLIND)       \
    METER_MODE(TRTCM_COLOR_AWARE)       \
    METER_MODE(SRTCM_COLOR_BLIND)       \
    METER_MODE(SRTCM_COLOR_AWARE)


typedef enum
{
#define METER_MODE(meterMode) DEVRM_POLICER_METER_MODE_##meterMode,

    DEVRM_POLICER_METER_MODE_TABLE

#undef METER_MODE

} DEVRM_POLICER_METER_MODE_T;

#define DEVRM_POLICER_METER_COLOR_MODE_TABLE    \
    COLOR_MODE(COLOR_BLIND)                     \
    COLOR_MODE(COLOR_AWARE)

typedef enum
{
#define COLOR_MODE(colorMode) DEVRM_POLICER_METER_##colorMode,

    DEVRM_POLICER_METER_COLOR_MODE_TABLE

#undef COLOR_MODE

} DEVRM_POLICER_METER_COLOR_MODE_T;

#define DEVRM_POLICER_METER_TYPE_TABLE  \
    METER_TYPE(BYTE)                    \
    METER_TYPE(PACKET)

typedef enum
{
#define METER_TYPE(type) DEVRM_POLICER_METER_TYPE_##type,
    DEVRM_POLICER_METER_TYPE_TABLE
#undef METER_TYPE

} DEVRM_POLICER_METER_TYPE_T;

typedef struct
{
    DEVRM_POLICER_METER_MODE_T  mode;
    DEVRM_POLICER_METER_TYPE_T  type;

    UI32_T                      kbits_sec;         /*Data rate to associate with meter (1000/sec)*/
    UI32_T                      kbytes_burst_size; /*Max burst to associate with meter (1000)*/
    UI32_T                      peak_kbits_sec;
    UI32_T                      peak_kbytes_burst_size;
} DEVRM_POLICER_METER_T;

/*
 * typedef: enum DEVRM_POLICER_COUNTER_MODE_T
 *
 * Description: Counters Mode. This enumeration controls the
 *              counters resolution.
 *
 * Enumerations:
 *  DEVRM_POLICER_CNTR_BYTE                     - 1 Byte resolution.
 *
 *  DEVRM_POLICER_CNTR_PACKET                   - packet resolution.
 */
typedef enum
{
    DEVRM_POLICER_COUNTER_MODE_BYTE,
    DEVRM_POLICER_COUNTER_MODE_PACKET
} DEVRM_POLICER_COUNTER_MODE_T;

typedef struct
{
    DEVRM_POLICER_MODE_T            mode;

    DEVRM_POLICER_METER_T           meter;
    DEVRM_POLICER_COUNTER_MODE_T    counter_mode;

    DEVRM_PACKET_COMMAND_T          yellow_pkt_cmd;
    DEVRM_ACTION_QOS_T              yellow_pkt_qos;

    DEVRM_PACKET_COMMAND_T          red_pkt_cmd;
    DEVRM_ACTION_QOS_T              red_pkt_qos;
} DEVRM_ACTION_POLICER_T;


typedef enum
{
    DEVRM_REDIRECT_MODE_DISABLE,
    DEVRM_REDIRECT_MODE_ROUTE
} DEVRM_REDIRECT_MODE_T;

typedef struct
{
    DEVRM_REDIRECT_MODE_T mode;

    union
    {
        struct
        {
            UI32_T row;
            UI32_T column;
        } ltt_index;
    };
} DEVRM_ACTION_REDIRECT_T;

#pragma mark -
#pragma mark cpu

typedef struct
{
    DEVRM_MODIFY_COMMAND_T          modify_cpu_code;
    UI32_T                          cpu_code;
} DEVRM_ACTION_CPU_T;

#pragma mark -
#pragma mark action

typedef struct
{
    DEVRM_PACKET_COMMAND_T          pkt_cmd;
    DEVRM_ACTION_VLAN_T             vlan;
    DEVRM_ACTION_QOS_T              qos;
    DEVRM_ACTION_POLICER_T          policer;
    DEVRM_ACTION_CPU_T              cpu;
    DEVRM_ACTION_REDIRECT_T         redirect;
} DEVRM_ACTION_T;

#pragma mark -

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_InitiateSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE:  init/clear chip database for PCLs//PCEs
 * INPUT:    device_id -- chip no
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_InitiateSystemResources();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocateSystemResource
 *------------------------------------------------------------------------------
 * PURPOSE  : Allocates resource when system was start-up
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
void
DEVRM_AllocateSystemResource();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ResetSystemResource
 *------------------------------------------------------------------------------
 * PURPOSE  : Reset allocated resource to initialized value
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
void
DEVRM_ResetSystemResource();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_FreeSystemResource
 *------------------------------------------------------------------------------
 * PURPOSE  : Clean allocated resource
 * INPUT    : None
 * OUTPUT   : None
 * RETURN   : None
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
void
DEVRM_FreeSystemResource();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_Initial
 *------------------------------------------------------------------------------
 * PURPOSE:  clear all pcle//pces
 * INPUT:    device_id -- chip no
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     this function no ready yet.
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_Initial();

/* rule */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocateRule
 *------------------------------------------------------------------------------
 * PURPOSE  : Allocate rule resource
 * INPUT    : device_id, selector_index
 *          : start_rule_id (per ACL),
 *            rule_qty (how many rules to be added)
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, call DEVRM_SetSelector() first
 *            2, MUST allocate rule resource before call DEVRM_SetRule()
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_AllocateRule(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T vir_group_id,
    int rule_pri,
    UI32_T dev_rule_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a rule entry.
 * INPUT:    device_id, selector_index, logical_rule_id, rule_entry, meter_entry, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     DEVRM_Rule = class_element + meter + action
 *           pass meter_entry with NULL if this rule does not have meter
 *           pass action_entry with NULL if this rule does not have action
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_SetRule(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T    *ace_entry,
    const DEVRM_ACTION_T *action_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a rule entry.
 * INPUT:    aclId, logical_rule_id, is_default_acl
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     1. If meter is shared, when delete it should check ..
 *           2. default pcl can not be resized.
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_DestroyRule(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T logical_rule_id
);

/* meter */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_UpdateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  update a meter entry.
 * INPUT:    device_id, logical_rule_id, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     Only spport to change Token Bucket's parameters.
 *           It should not add / delete action for yellow and red packet.
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_UpdateMeter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    const DEVRM_POLICER_METER_T *dev_meter_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ShareMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  share a meter from src_logical_rule_id to dst_logical_rule_id
 * INPUT:    device_id, src_logical_rule_id, dst_logical_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_ShareMeter(
    UI32_T device_id,
    UI32_T src_logical_rule_id,
    UI32_T dst_logical_rule_id
);

/* action */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a action entry.
 * INPUT:    device_id, logical_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_DestroyAction(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - DEVRM_CreatePcl
 *------------------------------------------------------------------------------
 * PURPOSE  : Create PCL which used to keep PCE
 * INPUT    : device_id     - device ID
 *            pcl_id        - PCL ID
 *            is_ingress    - TRUE means ingress direction;
 *                            FALSE means egress direction
 *            lookup_num    - lookup number
 *            rule_type     - rule type
 * OUTPUT   : None
 * RETURN   : TRUE/FALSE
 * NOTES    : None
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_CreatePcl(
    UI32_T device_id,
    UI32_T pcl_id,
    BOOL_T is_ingress,
    DEVRM_PCL_LOOKUP_NUMBER_T lookup_num,
    UI32_T rule_type
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyAcl
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy the ACL.
 * INPUT:    device_id, logical_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_DestroyAcl(
    UI32_T device_id,
    UI32_T pcl_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - DEVRM_BindPclToPort
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
DEVRM_BindPclToPort(
    UI32_T  device_id,
    UI32_T  phy_port,
    UI32_T  pcl_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - DEVRM_UnbindPclToPort
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
DEVRM_UnbindPclToPort(
    UI32_T  device_id,
    UI32_T  phy_port,
    UI32_T  pcl_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - DEVRM_BindPclToTrunkPort
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
DEVRM_BindPclToTrunkPort(
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME  - DEVRM_UnbindPclToTrunkPort
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
DEVRM_UnbindPclToTrunkPort(
    UI32_T device_id,
    UI32_T trunk_id,
    UI32_T pcl_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_GetCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  get counter
 * INPUT:    aclId, logical_rule_id
 * OUTPUT:   green_octets_p, yellow_octets_p, red_octets_p
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_GetCounter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id,
    UI32_T *green_octets_p,
    UI32_T *yellow_octets_p,
    UI32_T *red_octets_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ClearCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  clear counter
 * INPUT:    aclId, logical_rule_id
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_ClearCounter(
    UI32_T device_id,
    UI32_T pcl_id,
    UI32_T dev_rule_id
);

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
BOOL_T
DEVRM_HandleIPCReqMsg(
    SYSFUN_Msg_T* ipcmsg_p
);

#endif   /* End of _DEV_RM_H */
