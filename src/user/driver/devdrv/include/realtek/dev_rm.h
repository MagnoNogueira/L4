/* FUNCTION NAME: DEV_RM.H
* PURPOSE:
*         Support Realtek
* NOTES:
*
* REASON:
*    DESCRIPTION:
*    CREATOR:
*    Date           04/12/2015
*
* Copyright(C)      Accton Corporation, 2015
*/

#ifndef _DEV_RM_HELIX_H
#define _DEV_RM_HELIX_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_adpt.h"
#include "sysfun.h"

#include "common/rt_type.h"
#include "common/util/rt_util.h"
#include "rtk/acl.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define DEVRM_DEBUG_FLAGS_BIT_NO        0x00000000
#define DEVRM_DEBUG_FLAGS_BIT_ERR       0x00000001
#define DEVRM_DEBUG_FLAGS_BIT_LOG       0x00000002
#define DEVRM_DEBUG_FLAGS_BIT_TRACE     0x00000004


#define DEVRM_SHR_BITWIDE		32

/* (internal) Generic operation macro on bit array _a, with bit _b */
#define	DEVRM_SHR_BITOP(_a, _b, _op)	\
    (_a[(_b) / DEVRM_SHR_BITWIDE] _op (1 << ((_b) % DEVRM_SHR_BITWIDE)))

/* (internal) Number of SHR_BITDCLs needed to contain _max bits */
#define	DEVRM_SHR_BITDCLSIZE(_max)	(((_max) + DEVRM_SHR_BITWIDE - 1) / DEVRM_SHR_BITWIDE)

/* Specific operations */
#define	DEVRM_SHR_BITGET(_a, _b)	DEVRM_SHR_BITOP(_a, _b, &)
#define	DEVRM_SHR_BITSET(_a, _b)	DEVRM_SHR_BITOP(_a, _b, |=)
#define	DEVRM_SHR_BITCLR(_a, _b)	DEVRM_SHR_BITOP(_a, _b, &= ~)


#define DEVRM_PBMP_PORT_ADD(pbmp, port)       RTK_PORTMASK_PORT_SET(pbmp, port)
#define DEVRM_PBMP_PORT_REMOVE(pbmp, port)    RTK_PORTMASK_PORT_CLEAR(pbmp, port)
#define DEVRM_PBMP_MEMBER(pbmp, port)         RTK_PORTMASK_IS_PORT_SET(pbmp, port)

#define DEVRM_PBMP_ASSIGN(dst_pbmp, src_pbmp) RTK_PORTMASK_ASSIGN(dst_pbmp, src_pbmp)
#define DEVRM_PBMP_OR(dst_pbmp, src_pbmp)     RTK_PORTMASK_OR(dst_pbmp, src_pbmp)
#define DEVRM_PBMP_EQ(dst_pbmp, src_pbmp)     (0 == RTK_PORTMASK_COMPARE(dst_pbmp, src_pbmp))

#define DEVRM_PBMP_CLEAR(pbmp)                RTK_PORTMASK_RESET(pbmp)
#define DEVRM_PBMP_NOT_NULL(pbmp)             (RTK_PORTMASK_GET_PORT_COUNT(pbmp) != 0)


typedef rtk_portmask_t DEVRM_PBMP_T;

/* for the priority of selector
 *   56514 have 16 IFP selectors, 4 VFP selectors and 4 EFP selectors
 *   need to update this design when using different kind of chip.
 */

enum DEVRM_VFP_SELECTOR_S
{
    DEVRM_VFP_SELECTOR_0 = 0x1 , /*VFP slice 0*/
    DEVRM_VFP_SELECTOR_1 = 0x2 , /*VFP slice 1*/
    DEVRM_VFP_SELECTOR_2 = 0x4 , /*VFP slice 2*/
    DEVRM_VFP_SELECTOR_3 = 0x8   /*VFP slice 3*/
};

enum DEVRM_EFP_SELECTOR_S
{
  DEVRM_EFP_SELECTOR_0 = 0x1 , /*EFP slice 0*/
  DEVRM_EFP_SELECTOR_1 = 0x2 , /*EFP slice 1*/
  DEVRM_EFP_SELECTOR_2 = 0x4 , /*EFP slice 2*/
  DEVRM_EFP_SELECTOR_3 = 0x8   /*EFP slice 3*/
};

enum DEVRM_IFP_SELECTOR_S
{
  DEVRM_IFP_SELECTOR_0 = 0x1 ,
  DEVRM_IFP_SELECTOR_1 = 0x2 ,
  DEVRM_IFP_SELECTOR_2 = 0x4 ,
  DEVRM_IFP_SELECTOR_3 = 0x8 ,
  DEVRM_IFP_SELECTOR_4 = 0x10,
  DEVRM_IFP_SELECTOR_5 = 0x20,
  DEVRM_IFP_SELECTOR_6 = 0x40,
  DEVRM_IFP_SELECTOR_7 = 0x80,
  DEVRM_IFP_SELECTOR_8 = 0x100,
  DEVRM_IFP_SELECTOR_9 = 0x200,
  DEVRM_IFP_SELECTOR_10 = 0x400,
  DEVRM_IFP_SELECTOR_11 = 0x800,
  DEVRM_IFP_SELECTOR_12 = 0x1000,
  DEVRM_IFP_SELECTOR_13 = 0x2000,
  DEVRM_IFP_SELECTOR_14 = 0x4000,
  DEVRM_IFP_SELECTOR_15 = 0x8000
};

typedef enum
{
    DEVRM_GROUP_MODE_SINGLE = 0,          /* Single-wide group (default)    */
    DEVRM_GROUP_MODE_DOUBLE,              /* Double-wide group              */
    DEVRM_GROUP_MODE_TRIPLE,              /* Triple-wide group              */
    DEVRM_GROUP_MODE_QUADRUPLE,
    DEVRM_GROUP_MODE_AUTO,                /* Auto-expand group based on Qset  */
    DEVRM_GROUP_MODE_INVALID              /* Always last: not usable mode     */
} DEVRM_GROUP_MODE_T;

/* define rule priority in group
 */
#define  DEVRM_RULE_PRIO_HIGH       (-1)  /* Highest in group  */
#define  DEVRM_RULE_PRIO_DEFAULT    (-2)  /* Middle priority   */
#define  DEVRM_RULE_PRIO_LOW        (-3)  /* Lowest in group   */

/* NOTE for RealTek:
 *    In one selector, rule with smaller index has higher priority.
 *    In multiple selectors, selector with smaller index has higher priority.
 *    Rules of rate-limit should have higher priority than the user
 *    configured ones so rules of rate-limit are reserved from the rear
 *    in one selector only.
 *
 *    Rate-limit will not take effect if user configs rules of IPv6 EXT QoS.
 *
 */


/* MACRO FUNCTION DECLARATIONS
 */
/* DATA TYPE DECLARATIONS
 */

typedef enum
{
    DEVRM_FIELD_QUALIFY_HiGig           , // remove ??

    DEVRM_FIELD_QUALIFY_SwitchMac       , /* CPU mac */
    DEVRM_FIELD_QUALIFY_InPort          ,
    DEVRM_FIELD_QUALIFY_InPorts         ,
    DEVRM_FIELD_QUALIFY_OutPort         ,
    DEVRM_FIELD_QUALIFY_OutPorts        ,
    DEVRM_FIELD_QUALIFY_SrcMac          ,
    DEVRM_FIELD_QUALIFY_DstMac          ,
    DEVRM_FIELD_QUALIFY_OuterVlan       ,
    DEVRM_FIELD_QUALIFY_InnerVlan       ,
    DEVRM_FIELD_QUALIFY_OuterVlanPri    ,
    DEVRM_FIELD_QUALIFY_Snap            ,
    DEVRM_FIELD_QUALIFY_SrcIp           ,
    DEVRM_FIELD_QUALIFY_DstIp           ,
    DEVRM_FIELD_QUALIFY_SrcIp6          ,
    DEVRM_FIELD_QUALIFY_DstIp6          ,
    DEVRM_FIELD_QUALIFY_SrcIp6High      ,
    DEVRM_FIELD_QUALIFY_DstIp6High      ,
    DEVRM_FIELD_QUALIFY_IpType          ,
    DEVRM_FIELD_QUALIFY_L4SrcPort       ,
    DEVRM_FIELD_QUALIFY_L4DstPort       ,
    DEVRM_FIELD_QUALIFY_EtherType       ,
    DEVRM_FIELD_QUALIFY_IpProtocol      ,
    DEVRM_FIELD_QUALIFY_Dscp            ,
    DEVRM_FIELD_QUALIFY_Tos             ,
    DEVRM_FIELD_QUALIFY_Ip6NextHeader   ,
    DEVRM_FIELD_QUALIFY_Ip6TrafficClass ,
    DEVRM_FIELD_QUALIFY_IcmpType        ,
    DEVRM_FIELD_QUALIFY_IcmpCode        ,
    DEVRM_FIELD_QUALIFY_TcpControl      ,
    DEVRM_FIELD_QUALIFY_L2Format        ,
    DEVRM_FIELD_QUALIFY_VlanFormat      ,
    DEVRM_FIELD_QUALIFY_ArpOpcode       ,

    DEVRM_FIELD_QUALIFY_Count             /* Always Last. Not a usable value. */
} DEVRM_FIELD_QUALIFY_T;

typedef enum
{
    DEVRM_FIELD_USER_QUALIFY_Udf1,                /* [0x00] User Defined 1 */
    DEVRM_FIELD_USER_QUALIFY_Udf2,                /* [0x01] User Defined 2 */

    DEVRM_FIELD_USER_QUALIFY_Count                /* Always Last. Not a usable value. */
} DEVRM_FIELD_USER_QUALIFY_T;


typedef enum
{
    DEVRM_FIELD_QUALIFY_IpType_Any,
    DEVRM_FIELD_QUALIFY_IpType_ARP,
    DEVRM_FIELD_QUALIFY_IpType_NonIp, /* L2 only */
    DEVRM_FIELD_QUALIFY_IpType_Ipv4,
    DEVRM_FIELD_QUALIFY_IpType_Ipv6,
} DEVRM_FIELD_QUALIFY_IpType_T;

#define DEVRM_FIELD_QUALIFY_IPTYPE_STRINGS \
{               \
    "Any",      \
    "Arp",      \
    "NonIp",    \
    "Ipv4",     \
    "Ipv6"      \
}

typedef enum
{
    DEVRM_FIELD_QUALIFY_L2Format_Any,
    DEVRM_FIELD_QUALIFY_L2Format_Ethernet,
    DEVRM_FIELD_QUALIFY_L2Format_LLC_SNAP,
    DEVRM_FIELD_QUALIFY_L2Format_LLC_Other,
} DEVRM_FIELD_QUALIFY_L2Format_T;

#define DEVRM_FIELD_QUALIFY_L2FORMAT_STRINGS \
{                   \
    "Any",          \
    "Ethernet",     \
    "LLC_SNAMP",    \
    "LLC_Other",    \
}

typedef enum
{
    DEVRM_FIELD_QUALIFY_VlanFormat_Any,
    DEVRM_FIELD_QUALIFY_VlanFormat_Outer_UntagOrPriTag,
    DEVRM_FIELD_QUALIFY_VlanFormat_Outer_TagOrPriTag,
    DEVRM_FIELD_QUALIFY_VlanFormat_Outer_TagOnly,
    DEVRM_FIELD_QUALIFY_VlanFormat_Inner_UntagOrPriTag,
    DEVRM_FIELD_QUALIFY_VlanFormat_Inner_TagOrPriTag,
} DEVRM_FIELD_QUALIFY_VlanFormat_T;

#define DEVRM_FIELD_QUALIFY_VLANFORMAT_STRINGS \
{                       \
    "Any",              \
    "UntagOrPriTag",    \
    "TagOrPriTag",      \
    "UntagOrPriTag",    \
    "TagOrPriTag",      \
}


typedef struct
{
    UI32_T  udf_id;
    UI32_T  flags;
    UI32_T  chunk;      /* offset */
    BOOL_T  enabled;
} DEVRM_UDF_T;

typedef struct
{
    UI32_T  udf_id;
    //UI8_T   udf_data[BCM_FIELD_USER_FIELD_SIZE];
    //UI8_T   udf_mask[BCM_FIELD_USER_FIELD_SIZE];
} DEVRM_FieldUDF_T;

typedef struct
{
    rtk_portmask_t all_ports;   /* PORT ports + CPU port */
    rtk_portmask_t cpu_port;
} DEVRM_PortBitMap_T;

typedef struct
{
    UI32_T                 num_of_chips;
    DEVRM_PortBitMap_T     port_bit_map;
}DEVRM_DevInfo_T;

typedef struct
{
                        //UI64_T          entry_qualify;
    UI32_T w[DEVRM_SHR_BITDCLSIZE(DEVRM_FIELD_QUALIFY_Count)];
    UI32_T udf_map[DEVRM_SHR_BITDCLSIZE(DEVRM_FIELD_USER_QUALIFY_Count)];

    rtk_portmask_t      inports_data;
    rtk_portmask_t      inports_mask;
    int                 inport_data;
    int                 inport_mask;

    rtk_portmask_t      outports_data;
    rtk_portmask_t      outports_mask;
    int             outport_data;
    int             outport_mask;

    int             srcmodid_data;
    int             srcmodid_mask;

    int             srcporttgid_data;
    int             srcporttgid_mask;

    int             dstmodid_data;
    int             dstmodid_mask;

    int             dstporttgid_data;
    int             dstporttgid_mask;

    int             l4_srcport_data;
    int             l4_srcport_mask;

    int             l4_dstport_data;
    int             l4_dstport_mask;

    int             arp_opcode_data;
    int             arp_opcode_mask;

    UI16_T          outvlan_data;
    UI16_T          outvlan_mask;

    UI16_T          invlan_data;
    UI16_T          invlan_mask;

    UI8_T           outvlanpri_data;
    UI8_T           outvlanpri_mask;

    UI16_T          ethertype_data;
    UI16_T          ethertype_mask;

    UI8_T           ipprotocol_data;
    UI8_T           ipprotocol_mask;

    UI8_T           higpacket_data;
    UI8_T           higpacket_mask;

    UI32_T          lookupstatus_data;
    UI32_T          lookupstatus_mask;

    UI32_T          ipinfo_data;
    UI32_T          ipinfo_mask;

    UI32_T          packetres_data;
    UI32_T          packetres_mask;

    UI32_T          srcip_data;
    UI32_T          srcip_mask;

    UI32_T          dstip_data;
    UI32_T          dstip_mask;

    UI8_T           dscp_data;
    UI8_T           dscp_mask;

    UI8_T           tos_data;
    UI8_T           tos_mask;

    UI8_T           ipflags_data;
    UI8_T           ipflags_mask;

    UI8_T           tcpcontrol_data;
    UI8_T           tcpcontrol_mask;

    UI8_T           ttl_data;
    UI8_T           ttl_mask;

    UI32_T          range;
    int             invert;

    UI8_T           srcip6_data[SYS_ADPT_IPV6_ADDR_LEN];
    UI8_T           srcip6_mask[SYS_ADPT_IPV6_ADDR_LEN];

#if (SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN == 8)
    UI8_T           dstip6_upper_data[SYS_ADPT_IPV6_ADDR_LEN];
    UI8_T           dstip6_upper_mask[SYS_ADPT_IPV6_ADDR_LEN];
#else
    UI8_T           dstip6_data[SYS_ADPT_IPV6_ADDR_LEN];
    UI8_T           dstip6_mask[SYS_ADPT_IPV6_ADDR_LEN];
#endif /* (SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN == 8) */

    UI8_T           ip6nextheader_data;
    UI8_T           ip6nextheader_mask;

    UI8_T           ip6trafficclass_data;
    UI8_T           ip6trafficclass_mask;

    UI32_T          ip6flowlabel_data;
    UI32_T          ip6flowlabel_mask;

    UI8_T           ip6hoplimit_data;
    UI8_T           ip6hoplimit_mask;

    UI8_T           srcmac_data[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T           srcmac_mask[SYS_ADPT_MAC_ADDR_LEN];

    UI8_T           dstmac_data[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T           dstmac_mask[SYS_ADPT_MAC_ADDR_LEN];

    UI32_T          iptype;

    UI32_T          l2format;

    UI8_T           vlanformat_data;
    UI8_T           vlanformat_mask;

    UI8_T           mhopcode_data;
    UI8_T           mhopcode_mask;

    UI8_T           stpstate;

    UI8_T           l2srchit_data;
    UI8_T           l2srchit_mask;

    UI8_T           l2dsthit_data;
    UI8_T           l2dsthit_mask;

    UI8_T           l3routable_data;
    UI8_T           l3routable_mask;

    UI8_T           l3dsthosthit_data;
    UI8_T           l3dsthosthit_mask;

    UI8_T           icmptype_data;
    UI8_T           icmptype_mask;
    UI8_T           icmpcode_data;
    UI8_T           icmpcode_mask;

    //DEVRM_FieldUDF_T    udf[DEVRM_FIELD_USER_QUALIFY_Count];
}DEVRM_AceEntry_T;

//FIXME: should delete this
//typedef struct DEVRM_ActionEntry_S
//{
//    UI32_T                      action;    /* Action to perform (bcmFieldActionXXX)*/
//    UI32_T                      param0;    /* Action parameter (use 0 if not required) */
//    UI32_T                      param1;    /* Action parameter (use 0 if not required) */
//    rtk_portmask_t              pbmp;      /* Action parameter (use 0 if not required) */
//    struct DEVRM_ActionEntry_S  *next_action;
//} DEVRM_ActionEntry_T;

//typedef struct
//{
//    int      meter_num;     /*Entry in meter group (BCM_FIELD_METER_xxx)*/
//                            /*BCM_FIELD_METER_PEAK*/
//                            /*BCM_FIELD_METER_COMMITTED*/
//                            /*BCM_FIELD_METER_BOTH*/
//    UI32_T   kbits_sec;     /*Data rate to associate with commit meter (kbits/sec)*/
//    UI32_T   kbytes_burst;  /*Max burst to associate with commit meter (in kilobytes)*/
//    UI32_T   peak_kbits_sec;     /*Data rate to associate with commit meter (kbits/sec)*/
//    UI32_T   peak_kbytes_burst;  /*Max burst to associate with commit meter (in kilobytes)*/
//
//    UI8_T    meter_mode;    /* BCM_FIELD_METER_MODE_DEFAULT
//                             * BCM_FIELD_METER_MODE_FLOW
//                             * BCM_FIELD_METER_MODE_trTCM_COLOR_BLIND
//                             * BCM_FIELD_METER_MODE_trTCM_COLOR_AWARE
//                             * BCM_FIELD_METER_MODE_srTCM_COLOR_BLIND
//                             * BCM_FIELD_METER_MODE_srTCM_COLOR_AWARE
//                             */
//} DEVRM_MeterEntry_T;

#pragma mark - action.pkt_cmd

#define DEVRM_PACKET_COMMAND_TABLE    \
    PKT_CMD(UNSPECIFIC)               \
    PKT_CMD(FORWARD)                  \
    PKT_CMD(REDIRECT_TO_CPU)          \
    PKT_CMD(COPY_TO_CPU)              \
    PKT_CMD(HARD_DROP)                \
    PKT_CMD(SOFT_DROP)                \
    PKT_CMD(COPY_TO_CPU_CANCEL)       \
    PKT_CMD(MIRROR_TO_ANALYZER_PORT)

typedef enum
{
#define PKT_CMD(pktCmd) DEVRM_PKT_CMD_##pktCmd,

    DEVRM_PACKET_COMMAND_TABLE

#undef PKT_CMD
} DEVRM_PACKET_COMMAND_T;


#pragma mark - action.vlan

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

#pragma mark - action.qos

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

#pragma mark - action.policer

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
    METER_MODE(SRTCM_COLOR_AWARE)       \
    METER_MODE(RATE)


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

    union
    {
        struct
        {
            UI32_T  kbits_sec;
            UI32_T  kbytes_burst_size;
            UI32_T  peak_kbits_sec;
            UI32_T  peak_kbytes_burst_size;
        };

        struct
        {
            UI32_T  packet_sec;
            UI32_T  packet_burst_size;
            UI32_T  peak_packet_sec;
            UI32_T  peak_packet_burst_size;
        };
    };
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

#pragma mark - action.cpu

typedef struct
{
    DEVRM_MODIFY_COMMAND_T          modify_cpu_code;
    UI32_T                          cpu_code;
} DEVRM_ACTION_CPU_T;

#pragma mark - action.redirect

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

#pragma mark - action

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

typedef enum
{
    DEVRM_PHASE_INGRESS = ACL_PHASE_IGR_ACL,
    DEVRM_PHASE_EGRESS  = ACL_PHASE_EGR_ACL,

    DEVRM_PHASE_MAX
} DEVRM_PHASE_T;

typedef struct
{
    UI32_T group_id;
    UI32_T group_mode;

    UI32_T template_num;
    UI32_T template_key[4];
    DEVRM_PHASE_T phase;
}DEVRM_GroupEntry_T;


/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_Initial
 *------------------------------------------------------------------------------
 * PURPOSE:  init/clear chip setting
 * INPUT:    device_id -- chip no
 * OUTPUT:   none
 * RETURN:   TRUE/FALSE.
 * NOTE:     none
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_Initial();

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetDebugFlags
 *------------------------------------------------------------------------------
 * PURPOSE:  Set DEV_RM debug flags
 * INPUT:    flags (DEVRM_DEBUG_FLAGS_BIT_LOG, DEVRM_DEBUG_FLAGS_BIT_ERR)
 * OUTPUT:
 * RETURN:
 * NOTE:
 *------------------------------------------------------------------------------
 */
void DEVRM_SetDebugFlags(UI32_T flags);

/* selector */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Create a group entry.
 * INPUT:    device_id, group_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_CreateGroup(UI32_T device_id, DEVRM_GroupEntry_T *group_entry);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetGroupQualifySet
 *------------------------------------------------------------------------------
 * PURPOSE:  Set qualify template of group
 * INPUT:    device_id, group_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     None.
 *------------------------------------------------------------------------------
 */
BOOL_T
DEVRM_SetGroupQualifySet(
    UI32_T device_id,
    DEVRM_GroupEntry_T *group_entry_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyGroup
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a selector entry.
 * INPUT:    device_id, group_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_DestroyGroup(UI32_T device_id,UI32_T group_id) ;

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateVirtualGroup
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
BOOL_T DEVRM_CreateVirtualGroup(
    UI32_T group_id,
    UI32_T vir_group_id,
    int priority);

/* rule */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_AllocateRule
 *------------------------------------------------------------------------------
 * PURPOSE  : Allocate rule resource
 * INPUT    : device_id     - device ID
 *            group_id      - group ID
 *            vir_group_id  - virtual group ID
 *            rule_pri      - rule priority (no used)
 * OUTPUT   : dev_rule_id (per chip unique),
 *
 * RETURN   : TRUE/FALSE.
 * NOTE     : 1, call DEVRM_CreateGroup() first
 *            2, MUST allocate rule resource before call DEVRM_SetRule()
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_AllocateRule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T vir_group_id,
    int rule_pri,
    UI32_T *dev_rule_id
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetRule
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
DEVRM_SetRule(
    UI32_T device_id,
    UI32_T group_id,
    UI32_T dev_rule_id,
    DEVRM_AceEntry_T *ace_entry_p,
    const DEVRM_ACTION_T *dev_action_p
);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_UpdateRule
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
BOOL_T DEVRM_UpdateRule(UI32_T device_id, UI32_T dev_rule_id, DEVRM_AceEntry_T *ace_entry, DEVRM_ACTION_T *action_p);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyRule
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a rule entry.
 * INPUT:    device_id      - device ID
 *           vir_group_id   - virtual group ID
 *           dev_rule_id    - rule ID
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.If meter is shared, when delete it should check ..
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_DestroyRule(
    UI32_T device_id,
    UI32_T vir_group_id,
    UI32_T dev_rule_id
);

/* meter */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  create a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry,
 *           need_reinstalled (TRUE - rule will be reinstalled)
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_CreateMeter(UI32_T device_id,UI32_T dev_rule_id, DEVRM_POLICER_METER_T *meter_entry_p, BOOL_T need_reinstalled);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_UpdateMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  update a meter entry.
 * INPUT:    device_id, dev_rule_id, meter_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_UpdateMeter(UI32_T device_id,UI32_T dev_rule_id, const DEVRM_POLICER_METER_T *meter_entry_p);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_ShareMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  share a meter from src_rule_id to dst_rule_id
 * INPUT:    device_id, src_rule_id, dst_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_ShareMeter(UI32_T device_id, UI32_T src_rule_id, UI32_T dst_rule_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyMeter
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a meter entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_DestroyMeter(UI32_T device_id, UI32_T dev_rule_id);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_CreateCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  create a counter entry.
 * INPUT:    device_id, dev_rule_id, counter_mode,
 *           need_reinstalled (TRUE - rule will be reinstalled)
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_CreateCounter(UI32_T device_id,UI32_T dev_rule_id, UI32_T counter_mode, BOOL_T need_reinstalled);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyCounter
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a counter entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_DestroyCounter(UI32_T device_id, UI32_T dev_rule_id);

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
    UI32_T dev_rule_id,
    UI32_T *counter_p
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
    UI32_T dev_rule_id
);

/* action */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_SetAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Set a action entry.
 * INPUT:    device_id, dev_rule_id, action_entry
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
//BOOL_T DEVRM_SetAction(UI32_T device_id, UI32_T dev_rule_id, DEVRM_ActionEntry_T *action_entry, UI32_T action_entry_num);
BOOL_T DEVRM_SetAction(UI32_T device_id, UI32_T dev_rule_id, DEVRM_ACTION_T *action_p);

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEVRM_DestroyAction
 *------------------------------------------------------------------------------
 * PURPOSE:  Destroy a action entry.
 * INPUT:    device_id, dev_rule_id
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE
 *------------------------------------------------------------------------------
 */
BOOL_T DEVRM_DestroyAction(UI32_T device_id, UI32_T dev_rule_id);

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
BOOL_T DEVRM_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p) ;

#endif   /* End of _DEV_RM_H */
