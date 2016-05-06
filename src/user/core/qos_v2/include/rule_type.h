/* FUNCTION NAME: RULE_TYPE.H
* PURPOSE:
*   provide type definitions for DiffServ, ACL
*
* NOTES:
*     none
*
*REASON:
*    DESCRIPTION:
*    CREATOR:   pttch
*    Date 2005/5/1
*
* Copyright(C)      Accton Corporation, 2002
*/
#ifndef _RULE_TYPE_H
#define _RULE_TYPE_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_cpnt.h"
#include "sys_adpt.h"
#include "leaf_es3626a.h"


/* NAMING CONSTANT DECLARATIONS
 */

/* the following naming constants are used to identify an un-modified fields.
 * MIB use maximum value to stand for 'clear' setting, so real valid/meaningful value is
 * (maximum value - 1)
 */
#define RULE_TYPE_UNDEF_IP_CONTROL_CODE                 MAX_diffServIpAceControlCode
#define RULE_TYPE_UNDEF_IP_PROTOCOL                     MAX_diffServIpAceProtocol
#define RULE_TYPE_UNDEF_IP_PRECEDENCE                   MAX_diffServIpAcePrec
#define RULE_TYPE_UNDEF_IP_TOS                          MAX_diffServIpAceTos
#define RULE_TYPE_UNDEF_IP_DSCP                         MAX_diffServIpAceDscp
#define RULE_TYPE_UNDEF_IPV6_NEXT_HEADER                MAX_diffServIpv6AceNextHeader
#define RULE_TYPE_UNDEF_IPV6_DSCP                       MAX_diffServIpv6AceDscp
#define RULE_TYPE_UNDEF_IPV6_TRAFFIC_CLASS              256
#define RULE_TYPE_UNDEF_IPV6_FLOW_LABEL                 MAX_diffServIpv6AceFlowLabel
#define RULE_TYPE_UNDEF_ICMP_TYPE                       256
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
#define RULE_TYPE_UNDEF_TIME_RANGE                      TIME_RANGE_TYPE_UNDEF_TIME_RANGE
#endif
#define RULE_TYPE_UNDEF_RULE_INDEX                      0xFFFF

#define RULE_TYPE_IPV4_PROTOCOL_OP_NO_OPERATION         1
#define RULE_TYPE_IPV4_PROTOCOL_OP_EQUAL                2

#define RULE_TYPE_IPV4_DSCP_OP_NO_OPERATION             1
#define RULE_TYPE_IPV4_DSCP_OP_DSCP                     2
#define RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE               3
#define RULE_TYPE_IPV4_DSCP_OP_TOS                      4
#define RULE_TYPE_IPV4_DSCP_OP_PRECEDENCE_AND_TOS       5

#define RULE_TYPE_IPV6_NEXT_HEADER_OP_NO_OPERATION      1
#define RULE_TYPE_IPV6_NEXT_HEADER_OP_EQUAL             2

#define RULE_TYPE_IPV6_TRAFFIC_CLASS_OP_NO_OPERATION    1
#define RULE_TYPE_IPV6_TRAFFIC_CLASS_OP_EQUAL           2

#define RULE_TYPE_IPV6_FLOW_LABEL_OP_NO_OPERATION       1
#define RULE_TYPE_IPV6_FLOW_LABEL_OP_EQUAL              2

#define RULE_TYPE_ICMP_ICMPTYPE_OP_NO_OPERATION         1
#define RULE_TYPE_ICMP_ICMPTYPE_OP_EQUAL                2

#define RULE_TYPE_ACE_MAC_IP_MIX                        (SYS_CPNT_QOS_V2_CLASS_MAP_MIXED_MODE   \
                                                         || SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL \
                                                         || SYS_CPNT_ACL_MAC_IP_MIX)

/* the following naming constants are used to judge a maximum and meaningful value.
 * MIB use maximum value to stand for 'clear' setting, so real valid/meaningful value is
 * (maximum value - 1)
 */
#define RULE_TYPE_MAX_OF_IP_CONTROL_CODE                (MAX_diffServIpAceControlCode - 1)
#define RULE_TYPE_MAX_OF_IP_PROTOCOL                    (MAX_diffServIpAceProtocol - 1)
#define RULE_TYPE_MAX_OF_IP_PRECEDENCE                  (MAX_diffServIpAcePrec - 1)
#define RULE_TYPE_MAX_OF_IP_TOS                         (MAX_diffServIpAceTos - 1)
#define RULE_TYPE_MAX_OF_IP_DSCP                        (MAX_diffServIpAceDscp - 1)
#define RULE_TYPE_MAX_OF_IPV6_NEXT_HEADER               (MAX_diffServIpv6AceNextHeader - 1)
#define RULE_TYPE_MAX_OF_IPV6_DSCP                      (MAX_diffServIpv6AceDscp - 1)
#define RULE_TYPE_MAX_OF_IPV6_FLOW_LABEL                (MAX_diffServIpv6AceFlowLabel - 1)
#define RULE_TYPE_MAX_OF_ICMP_TYPE                      255
#define RULE_TYPE_MIN_OF_ICMP_TYPE                      0

/* for CLI/Web */
#define RULE_TYPE_MAX_OF_SRC_IPV6_PREFIX_LEN            (MAX_diffServIpv6AceSourceIpAddrPrefixLen)
#define RULE_TYPE_MAX_OF_DST_IPV6_PREFIX_LEN            (SYS_ADPT_ACL_MAX_DST_IPV6_PREFIX_LEN)

#define RULE_TYPE_MAX_IPV6_PREFIX_LEN                   128

#define RULE_TYPE_ACL_IP_ADDR_DEFAULT                   0x0
#define RULE_TYPE_ACL_IP_BITMASK_ANY                    0x0

/*action bit map*/
#define RULE_TYPE_ACTION_PKT_NEW_PRI                    0x0001
#define RULE_TYPE_ACTION_PKT_NEW_TOS                    0x0002
#define RULE_TYPE_ACTION_PKT_NEW_DSCP                   0x0004
#define RULE_TYPE_ACTION_DROP                           0x0008
#define RULE_TYPE_ACTION_DROP_PRECEDENCE                0x0010
#define RULE_TYPE_ACTION_RED_DROP                       0x0020
#define RULE_TYPE_ACTION_RED_DROP_PRECEDENCE            0x0040
#define RULE_TYPE_ACTION_RED_PKT_NEW_DSCP               0x0080
#define RULE_TYPE_ACTION_YELLOW_DROP                    0x0100
#define RULE_TYPE_ACTION_YELLOW_DROP_PRECEDENCE         0x0200
#define RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP            0x0400
#define RULE_TYPE_ACTION_PKT_NEW_PHB                    0x0800
#define RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP             0x1000
#define RULE_TYPE_ACTION_GREEN_PKT_TRANSMIT             0x2000
#define RULE_TYPE_ACTION_GREEN_PKT_DROP                 0x4000
#define RULE_TYPE_ACTION_PKT_PRI                        0x8000  /* Software bundle rate limitation, packet's priority  */

#define RULE_TYPE_IN_PROFILE_ACTION                     \
    (RULE_TYPE_ACTION_PKT_NEW_PRI   |                   \
     RULE_TYPE_ACTION_PKT_NEW_TOS   |                   \
     RULE_TYPE_ACTION_PKT_NEW_DSCP  |                   \
     RULE_TYPE_ACTION_PKT_NEW_PHB)

#define RULE_TYPE_GREEN_ACTION                          \
    (RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP |              \
     RULE_TYPE_ACTION_GREEN_PKT_TRANSMIT |              \
     RULE_TYPE_ACTION_GREEN_PKT_DROP)

#define RULE_TYPE_YELLOW_ACTION                         \
    (RULE_TYPE_ACTION_YELLOW_DROP   |                   \
     RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP)

#define RULE_TYPE_RED_ACTION                            \
    (RULE_TYPE_ACTION_RED_DROP      |                   \
     RULE_TYPE_ACTION_RED_PKT_NEW_DSCP)

#define RULE_TYPE_OUT_OF_PROFILE_ACTION                 \
    (RULE_TYPE_GREEN_ACTION | RULE_TYPE_YELLOW_ACTION | RULE_TYPE_RED_ACTION)


#define RULE_TYPE_ACTION_DEFAULT_PRI                    0
#define RULE_TYPE_ACTION_DEFAULT_PHB                    0
#define RULE_TYPE_ACTION_DEFAULT_PKT_NEW_DSCP           0

/* meter mode */
#define RULE_TYPE_METER_MODE_DEFAULT                    0x0
#define RULE_TYPE_METER_MODE_FLOW                       0x1
#define RULE_TYPE_METER_MODE_TRTCM_COLOR_BLIND          0x2
#define RULE_TYPE_METER_MODE_TRTCM_COLOR_AWARE          0x3
#define RULE_TYPE_METER_MODE_SRTCM_COLOR_BLIND          0x6
#define RULE_TYPE_METER_MODE_SRTCM_COLOR_AWARE          0x7
#define RULE_TYPE_METER_MODE_RATE                       0X8

#define RULE_TYPE_ETHERTYPE_IP                          0x0800
#define RULE_TYPE_ETHERTYPE_IPV6                        0x86DD
#define RULE_TYPE_ETHERTYPE_ARP                         0x0806

#define RULE_TYPE_ARP_OPCODE_REQUEST                    0X1
#define RULE_TYPE_ARP_OPCODE_REPLY                      0X2

/* bmp is an UI8_T array, bno is a zero-based bit index
 */
#define RULE_TYPE_IS_BIT_ON(bmp, bno)                   (bmp[(bno)/8] & (1 << (7 - (bno)%8)))
#define RULE_TYPE_IS_BIT_OFF(bmp, bno)                  (!RULE_TYPE_IS_BIT_ON(bmp, bno))
#define RULE_TYPE_SET_BIT_ON(bmp, bno)                  {bmp[(bno)/8] |= (1 << (7 - (bno)%8));}
#define RULE_TYPE_SET_BIT_OFF(bmp, bno)                 {bmp[(bno)/8] &= ~(1 << (7 - (bno)%8));}
#define RULE_TYPE_GET_BITMAP_ARRAY_SIZE(_max_value)     (_max_value / 8 + 1)

/* TCAM capability
 */
#define RULE_TYPE_UNDEF_DEVICE_ID                       0xff
#define RULE_TYPE_UNDEF_POOL_ID                         0xff

#define RULE_TYEP_TCAM_CAP_MAX_BITMAP_SIZE              (RULE_TYPE_TCAM_CAP_TOTAL / 8 + 1)
#define RULE_TYPE_TCAM_CAP_MAX_STR_LEN                   8
#define RULE_TYPE_TCAM_CAP_MAX_DESCRIPTION_LEN           32

/* Max length of name =        RULE_TYPE_TCAM_CAP_MAX_STR_LEN
 * Max length of description = RULE_TYPE_TCAM_CAP_MAX_DESCRIPTION_LEN
 */
#define RULE_TYPE_TCAM_CAPABILITY_LIST(_)                                                 \
/* (enum_name,                             name,       description) */                    \
_(RULE_TYPE_TCAM_CAP_MAC_ACL,              "AM",       "MAC ACL")                         \
_(RULE_TYPE_TCAM_CAP_IPV4_ACL,             "A4",       "IPv4 ACL")                        \
_(RULE_TYPE_TCAM_CAP_IPV6_STD_ACL,         "A6S",      "IPv6 Standard ACL")               \
_(RULE_TYPE_TCAM_CAP_IPV6_EXT_ACL,         "A6E",      "IPv6 extended ACL")               \
_(RULE_TYPE_TCAM_CAP_MAC_DS,               "DM",       "MAC diffServ")                    \
_(RULE_TYPE_TCAM_CAP_IPV4_DS,              "D4",       "IPv4 diffServ")                   \
_(RULE_TYPE_TCAM_CAP_IPV6_STD_DS,          "D6S",      "IPv6 standard diffServ")          \
_(RULE_TYPE_TCAM_CAP_IPV6_EXT_DS,          "D6E",      "IPv6 extended diffServ")          \
_(RULE_TYPE_TCAM_CAP_EGRESS_MAC_ACL,       "AEM",      "Egress MAC ACL")                  \
_(RULE_TYPE_TCAM_CAP_EGRESS_IPV4_ACL,      "AE4",      "Egress IPv4 ACL")                 \
_(RULE_TYPE_TCAM_CAP_EGRESS_IPV6_STD_ACL,  "AE6S",     "Egress IPv6 standard ACL")        \
_(RULE_TYPE_TCAM_CAP_EGRESS_IPV6_EXT_ACL,  "AE6E",     "Egress IPv6 extended ACL")        \
_(RULE_TYPE_TCAM_CAP_EGRESS_MAC_DS,        "DEM",      "Egress MAC diffServ")             \
_(RULE_TYPE_TCAM_CAP_EGRESS_IPV4_DS,       "DE4",      "Egress IPv4 diffServ")            \
_(RULE_TYPE_TCAM_CAP_EGRESS_IPV6_STD_DS,   "DE6S",     "Egress IPv6 standard diffServ")   \
_(RULE_TYPE_TCAM_CAP_EGRESS_IPV6_EXT_DS,   "DE6E",     "Egress IPv6 extended diffServ")   \
_(RULE_TYPE_TCAM_CAP_WEB_AUTH,             "W",        "Web authentication")              \
_(RULE_TYPE_TCAM_CAP_IP_SOURCE_GUSRD,      "I",        "IP source guard")                 \
_(RULE_TYPE_TCAM_CAP_IPV6_SOURCE_GUSRD,    "I6",       "IPv6 source guard")               \
_(RULE_TYPE_TCAM_CAP_CPU_INTERFACE,        "C",        "CPU interface")                   \
_(RULE_TYPE_TCAM_CAP_RATE_LIMITE,          "R",        "Rate limit")                      \
_(RULE_TYPE_TCAM_CAP_LINK_LOCAL,           "L",        "Link local")                      \
_(RULE_TYPE_TCAM_CAP_MAC_VLAN,             "MV",       "Mac based VLAN")                  \
_(RULE_TYPE_TCAM_CAP_PROTOCOL_VLAN,        "PV",       "Protocol based VLAN")             \
_(RULE_TYPE_TCAM_CAP_VOICE_VLAN,           "VV",       "Voice VLAN")                      \
_(RULE_TYPE_TCAM_CAP_IP_SUBNET_VLAN,       "IPSV",     "IP subnet VLAN")                  \
_(RULE_TYPE_TCAM_CAP_ROUTING,              "R",        "Routing")                         \
                                                                                          \
/* Reserved and ALL should be at the last */                                              \
_(RULE_TYPE_TCAM_CAP_RESERVED,             "Reserved", "Reserved")                        \
_(RULE_TYPE_TCAM_CAP_ALL,                  "ALL",      "All supported function")

#define RULE_TYPE_TCAM_CAPABILITY_ENUM(e_name, name_str, desc) e_name,
#define RULE_TYPE_TCAM_CAPABILITY_STRING(e_name, name_str, desc)  {e_name, name_str, desc},

#define RULE_TYPE_TCAM_CAPABILITY_STRING_ARRAY                       \
{                                                                    \
    RULE_TYPE_TCAM_CAPABILITY_LIST(RULE_TYPE_TCAM_CAPABILITY_STRING) \
}

typedef enum
{
  RULE_TYPE_TCAM_CAPABILITY_LIST(RULE_TYPE_TCAM_CAPABILITY_ENUM)

  RULE_TYPE_TCAM_CAP_TOTAL,
  RULE_TYPE_TCAM_CAP_UNDEF
} RULE_TYPE_TCAM_Capability_T;

typedef enum
{
    RULE_TYPE_NOTIFY_CLASS_MAP_DELETING,
    RULE_TYPE_NOTIFY_METER_CHANGED,
    RULE_TYPE_NOTIFY_ACTION_CHANGED,
    RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_CLASS_DELETING,
    RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_METER_DELETING,
    RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_IN_ACTION_DELETING,
    RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_OUT_OF_PROFILE_ACTION_DELETING,
    RULE_TYPE_NOTIFY_POLICY_MAP_ELEMENT_CLASS_ACTION_DELETING,
} RULE_TYPE_NOTIFY_MESSAGE_T;

/* Invalid character set of name and description of class-map and policy-map
 *
 * Maybe we need to add ACL to following this rule. But we need to change the
 * spec first.
 */
#define RULE_TYPE_INVALIDATE_CHAR_OF_DS_NAME          "\"?! "
#define RULE_TYPE_INVALIDATE_CHAR_OF_DS_DESC          "\"?!"

#define RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_POLICY_MAP_LIST        ((SYS_ADPT_DIFFSERV_MAX_NBR_OF_POLICY_MAP + 7) / 8)
#define RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_CLASS_MAP_LIST         ((SYS_ADPT_DIFFSERV_MAX_NBR_OF_CLASS_MAP + 7) / 8)

#define RULE_TYPE_ACL_TCP_PROTOCOL                      SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_TCP
#define RULE_TYPE_ACL_UDP_PROTOCOL                      SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_UDP
#define RULE_TYPE_ACL_ICMP_PROTOCOL                     SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_ICMP

#define RULE_TYPE_ACL_IPV6_TCP_NEXT_HEADER              SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_TCP
#define RULE_TYPE_ACL_IPV6_UDP_NEXT_HEADER              SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_UDP
#define RULE_TYPE_ACL_IPV6_ICMP_NEXT_HEADER             SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_ICMP6

#define RULE_TYPE_ACE_OWNED_BY_NONE                     0 /* ACE OWNED BY NO BODY */
#define RULE_TYPE_ACE_OWNED_BY_ACL                      1 /* ACE OWNED BY AN ACL */
#define RULE_TYPE_ACE_OWNED_BY_CLASS_MAP                2 /* ACE OWNED BY A CLASS-MAP */

#define RULE_TYPE_TIMER                                 6000  /* every 1 min*/


#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    #define RULE_TYPE_MIN_BUNDLE_GROUP_INDEX            1
    #define RULE_TYPE_MAX_BUNDLE_GROUP_INDEX            \
        (RULE_TYPE_MIN_BUNDLE_GROUP_INDEX+SYS_ADPT_DIFFSERV_MAX_BUNDLE_GROUP_OF_POLICY_MAP-1)

    /* Change RULE_TYPE_TIMER value
     */
    #ifdef RULE_TYPE_TIMER
        #undef RULE_TYPE_TIMER
        #define RULE_TYPE_TIMER                         100
    #endif /* RULE_TYPE_TIMER */

#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */


/* RULE_TYPE_MAX_COUNTER_REC
 * Range is : 0 ~ 255
 */
#if (SYS_CPNT_ACL_COUNTER == TRUE)
    #define RULE_TYPE_MAX_COUNTER_REC                   1
#endif /* SYS_CPNT_ACL_COUNTER */

/* If SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL is enabled
 * RULE_TYPE_MAX_COUNTER_REC range should be 3 ~ 255
 */
#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)

    #ifdef  RULE_TYPE_MAX_COUNTER_REC
        #undef  RULE_TYPE_MAX_COUNTER_REC
    #endif

    #define RULE_TYPE_MAX_COUNTER_REC                   3

#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

/** (ether + trunk) + cpu + all ports
 */
#define RULE_TYPE_TOTAL_NBR_OF_PORT                     (SYS_ADPT_TOTAL_NBR_OF_LPORT + 1 + 1)
#define RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST  ((RULE_TYPE_TOTAL_NBR_OF_PORT + 7) / 8)

/** All ports indicates all eth and trunk ports on each unit
 */
#define RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER           0xffff

#define RULE_TYPE_TOTAL_NBR_OF_UPORT                    \
(                                                       \
    SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK *                \
    SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT                   \
)

#define RULE_TYPE_IS_UPORT(ifindex)                     \
(                                                       \
    SYS_ADPT_ETHER_1_IF_INDEX_NUMBER <= (ifindex) &&    \
    (ifindex) <= (SYS_ADPT_ETHER_1_IF_INDEX_NUMBER + (RULE_TYPE_TOTAL_NBR_OF_UPORT) - 1)    \
)

#define RULE_TYPE_IS_TRUNK(ifindex)                     \
(                                                       \
    SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER <= (ifindex) &&    \
    (ifindex) <= (SYS_ADPT_TRUNK_1_IF_INDEX_NUMBER + SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM - 1) \
)

#define RULE_TYPE_IS_CPU_PORT(ifindex)                  \
(                                                       \
    SYS_ADPT_CPU_1_IF_INDEX_NUMBER == (ifindex)         \
)

#define RULE_TYPE_IS_CRAFT_PORT(ifindex)                \
(                                                       \
    SYS_ADPT_CRAFT_INTERFACE_IFINDEX == (ifindex)       \
)

#define RULE_TYPE_IS_ALL_PORTS(ifindex)                 \
(                                                       \
    RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER == (ifindex)  \
)

#define RULE_TYPE_IFINDEX_TO_INTERFACE_TYPE(ifindex)                \
(                                                                   \
    RULE_TYPE_IS_UPORT(ifindex) ? RULE_TYPE_INTERFACE_UPORT :       \
    RULE_TYPE_IS_TRUNK(ifindex) ? RULE_TYPE_INTERFACE_TRUNK :       \
    RULE_TYPE_IS_CPU_PORT(ifindex) ? RULE_TYPE_INTERFACE_CPU_PORT : \
    RULE_TYPE_IS_ALL_PORTS(ifindex) ? RULE_TYPE_INTERFACE_ALL_UNIT :\
    0xff                                                            \
)

#define RULE_TYPE_VALIDATE_IFINDEX(ifindex)             \
(                                                       \
    (RULE_TYPE_IS_UPORT(ifindex) ||                     \
     RULE_TYPE_IS_TRUNK(ifindex) ||                     \
     RULE_TYPE_IS_CPU_PORT(ifindex) ||                  \
     RULE_TYPE_IS_ALL_PORTS(ifindex)) ?                 \
    RULE_TYPE_OK : RULE_TYPE_FAIL                       \
)

#define RULE_TYPE_IFINDEX_TO_PORT_LIST_INDEX(ifindex)   \
(                                                       \
    (RULE_TYPE_IS_UPORT(ifindex) ||                     \
     RULE_TYPE_IS_TRUNK(ifindex)) ?                     \
        ((ifindex) - 1) :                               \
    (RULE_TYPE_IS_CPU_PORT(ifindex)) ?                  \
        (SYS_ADPT_TOTAL_NBR_OF_LPORT) :                 \
    (RULE_TYPE_IS_ALL_PORTS(ifindex)) ?                 \
        (SYS_ADPT_TOTAL_NBR_OF_LPORT + 1) :             \
        0xffff                                          \
)

#define RULE_TYPE_PORT_LIST_INDEX_TO_IFINDEX(plist_idx) \
(                                                       \
    ((plist_idx) < SYS_ADPT_TOTAL_NBR_OF_LPORT) ?       \
        ((plist_idx) + 1) :                             \
    (SYS_ADPT_TOTAL_NBR_OF_LPORT == (plist_idx)) ?      \
        SYS_ADPT_CPU_1_IF_INDEX_NUMBER :                \
    ((SYS_ADPT_TOTAL_NBR_OF_LPORT + 1) == (plist_idx)) ?\
        RULE_TYPE_ALL_PORTS_1_IF_INDEX_NUMBER :         \
        0xffff                                          \
)

enum RULE_TYPE_EVENT_MASK_E
{
    RULE_TYPE_EVENT_NONE                =   0x0000L,
    RULE_TYPE_EVENT_TIMER               =   0x0001L,
    RULE_TYPE_EVENT_ALL                 =   0xFFFFL
};

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
typedef enum
{
    RULE_TYPE_TIME_RANGE_NO_EXISTED      = 0,
    RULE_TYPE_TIME_RANGE_IN_ACTIVE       = 1,
    RULE_TYPE_TIME_RANGE_UN_ACTIVE       = 2,
}RULE_TYPE_TIME_RANGE_RULE_BEHAVIOR_T;
#endif /* SYS_CPNT_TIME_BASED_ACL */

/* FIXME: THESE 4 constant should be RMOVE !!
 */
#define RULE_TYPE_MAX_NBRS_OF_ACL               (SYS_ADPT_MAX_NBRS_OF_ACL \
                                                 + RULE_TYPE_HIDE_WORKING_ACL_CNT)

#define RULE_TYPE_MAX_NBRS_OF_ACE_OF_SYSTEM     (SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM \
                                                 + RULE_TYPE_HIDE_WORKING_ACE_CNT)

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE || SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    #define RULE_TYPE_HIDE_WORKING_ACL_CNT      SYS_ADPT_MAX_NBRS_OF_ACL
    #define RULE_TYPE_HIDE_WORKING_ACE_CNT      SYS_ADPT_MAX_NBRS_OF_ACE_OF_SYSTEM
#else
    #define RULE_TYPE_HIDE_WORKING_ACL_CNT      0
    #define RULE_TYPE_HIDE_WORKING_ACE_CNT      0
#endif /* SYS_CPNT_ACL_AUTO_COMPRESS_ACE || SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL */

/* MACRO FUNCTION DECLARATIONS
 */
#if (SYS_CPNT_DAI == TRUE)
typedef UI8_T RULE_TYPE_ArpType_T;
enum /* RULE_TYPE_ArpType_T */
{
    RULE_TYPE_ARP_REQUEST,
    RULE_TYPE_ARP_REPLY,
    /* for request and reply
     */
    RULE_TYPE_ARP_BOTH,
};
#endif /* SYS_CPNT_DAI */

/* DATA TYPE DECLARATIONS
 */
typedef UI8_T RULE_TYPE_AclType_T;
enum /* RULE_TYPE_AclType_T */
{
    RULE_TYPE_MAC_ACL,
    RULE_TYPE_IP_ACL,
    RULE_TYPE_IP_STD_ACL,
    RULE_TYPE_IP_EXT_ACL,
    RULE_TYPE_MF,

    RULE_TYPE_IPV6_ACL,
    RULE_TYPE_IPV6_STD_ACL,
    RULE_TYPE_IPV6_EXT_ACL,

#if (SYS_CPNT_DAI == TRUE)
    RULE_TYPE_ARP_ACL,
#endif

    RULE_TYPE_MAX_NBR_OF_ACL_TYPE,
};

typedef enum
{
    RULE_TYPE_INBOUND           = 0,
    RULE_TYPE_OUTBOUND          = 1,
    RULE_TYPE_BOTH_DIRECTION,
    RULE_TYPE_UNSPEC_DIRECTION,
} RULE_TYPE_InOutDirection_T;

#define RULE_TYPE_DIRECTION_TO_IS_INGRESS(dir)                          \
    (                                                                   \
        (dir == RULE_TYPE_INBOUND) ? TRUE : FALSE                       \
    )

#define RULE_TYPE_IS_INGRESS_TO_DIRECTION(is_ingress)                   \
    (                                                                   \
        (is_ingress == TRUE) ? RULE_TYPE_INBOUND : RULE_TYPE_OUTBOUND   \
    )

#define RULE_TYPE_DIRECTION_STR(direction)                          \
    (                                                               \
        (direction == RULE_TYPE_INBOUND) ? "ingress" :              \
        (direction == RULE_TYPE_OUTBOUND) ? "egress" :              \
        (direction == RULE_TYPE_BOTH_DIRECTION) ? "both" :          \
        (direction == RULE_TYPE_UNSPEC_DIRECTION) ? "unspec" : "??" \
    )

/* Note: only support RULE_TYPE_CLASS_MAP_MATCH_ANY now
 */
typedef UI8_T RULE_TYPE_MatchType_T;
enum /* RULE_TYPE_MatchType_T */
{
    RULE_TYPE_CLASS_MAP_MATCH_NONE=1,
    RULE_TYPE_CLASS_MAP_MATCH_ANY,
    RULE_TYPE_CLASS_MAP_MATCH_ALL,
    RULE_TYPE_CLASS_MAP_MATCH_LAST = RULE_TYPE_CLASS_MAP_MATCH_ALL
};

typedef UI8_T RULE_TYPE_ClassType_T;
enum /* RULE_TYPE_ClassType_T */
{
    RULE_TYPE_CLASS_MF=1,
    RULE_TYPE_CLASS_ACL,
    RULE_TYPE_CLASS_CLASS_MAP,
    RULE_TYPE_CLASS_LAST = RULE_TYPE_CLASS_CLASS_MAP
};

typedef UI8_T RULE_TYPE_MfType_T;
enum /* RULE_TYPE_MfType_T */
{
    RULE_TYPE_MF_PRECEDENCE,
    RULE_TYPE_MF_DSCP,
    RULE_TYPE_MF_DSCP_IPV6,
    RULE_TYPE_MF_VLAN,
    RULE_TYPE_MF_COS,
    RULE_TYPE_MF_SOURCE_PORT,
    RULE_TYPE_MF_LAST = RULE_TYPE_MF_SOURCE_PORT
};

enum
{
    RULE_TYPE_PRECEDENCE_FIRST = 0,
    INGRESS_MAC_ACL_PRECEDENCE = 1,
    EGRESS_MAC_ACL_PRECEDENCE,
    INGRESS_IP_ACL_PRECEDENCE,
    EGRESS_IP_ACL_PRECEDENCE,
    INGRESS_IPV6_ACL_PRECEDENCE,
    EGRESS_IPV6_ACL_PRECEDENCE,
    RULE_TYPE_PRECEDENCE_LAST = EGRESS_IPV6_ACL_PRECEDENCE
};

typedef enum
{
    RULE_TYPE_ACE_PERMIT = VAL_aclIpAceAction_permit,
    RULE_TYPE_ACE_DENY = VAL_aclIpAceAction_deny,
    RULE_TYPE_ACE_MIRROR_TO_ANALYZER_PORT,
} RULE_TYPE_AceAction_T;

typedef enum
{
    RULE_TYPE_OK = 1,
    RULE_TYPE_DUPLICATE_ENTRY,
    RULE_TYPE_TABLE_FULLED,
    RULE_TYPE_CONFLICT_TO_QOS_ENTRY,
    RULE_TYPE_CONFLICT_TO_ACL_ENTRY,
    RULE_TYPE_INVALID_PARAMETER,
    RULE_TYPE_HARDWARE_FAIL,
    RULE_TYPE_INSUFFICIENT_RESOURCE,
    RULE_TYPE_POLICY_MAP_EXISTED,
    RULE_TYPE_POLICY_MAP_NONEXISTED,
    RULE_TYPE_CLASS_MAP_EXISTED,
    RULE_TYPE_CLASS_MAP_NONEXISTED,
    RULE_TYPE_CLASS_EXISTED,
    RULE_TYPE_CLASS_NONEXISTED,
    RULE_TYPE_ACL_EXISTED,
    RULE_TYPE_ACL_NONEXISTED,
    RULE_TYPE_ACL_FULLED,
    RULE_TYPE_EMPTY_ACL,
    RULE_TYPE_ACE_EXISTED,
    RULE_TYPE_ACE_NONEXISTED,
    RULE_TYPE_ACE_FULLED,
    RULE_TYPE_METER_FAIL,
    RULE_TYPE_ACTION_FAIL,
    RULE_TYPE_FAIL,
    RULE_TYPE_SAME_CFG,                     /* new cfg == orignal cfg */
    RULE_TYPE_NOT_ALLOW_MIX_SELECTOR,       /* because a class map does not allow mixing MAC,
                                               IPv4, IPv6 extended, and IPv6 standard rules together */
    RULE_TYPE_ALREADY_BIND,
    RULE_TYPE_FREE_SELECTOR_UNAVAILABLE,
    RULE_TYPE_EXISTED,

    RULE_TYPE_E_DEST_PORT,                  /* failed to set destination port for mirror */
    RULE_TYPE_E_MATCH_TYPE,                 /* the match type of class-map is incorrect or not supported
                                             * (this error happen by coding defect)
                                             */
    RULE_TYPE_E_INSUFFICIENT_RULE_INSTANCE, /* insufficient rule instance
                                             * (this error happen by coding defect)
                                             */
    RULE_TYPE_E_INSUFFICIENT_IPC_BUFFER,    /* insufficient IPC buffer
                                             * (e.g., pass a long string parameter
                                             *  which length is over than accepted
                                             *  lenght on IPC buffer)
                                             */
    RULE_TYPE_E_HW_INSUFFICIENT_SELECTOR,   /* Hardware resource error. On Broadcom.
                                             * This error due to no any selector
                                             * is available when enable dynamic
                                             * selector assignment.
                                             */
    RULE_TYPE_E_INVALID_CHARACTER,          /* parameter includes invalid character
                                             */
    RULE_TYPE_E_HW_LIMIT_YELLOW_REMARK_DSCP,/* Hardware limitation. Can not remark
                                             * IP DSCP for yellow packet.
                                             */
    RULE_TYPE_E_HW_LIMIT_RED_REMARK_DSCP,   /* Hardware limitation. Can not remark
                                             * IP DSCP for red packet.
                                             */
    RULE_TYPE_E_HW_LIMIT_REMAP_BY_PHB,      /* Hardware limitation. Can not remap
                                             * egress queue by PHB value.
                                             */
    RULE_TYPE_E_HW_LIMIT_MATCH_SOURCE_PORT, /* Hardware limitation. Can not match
                                             * source port on a ingress port.
                                             */
    RULE_TYPE_E_INVALID_METER_MODE,         /* Invalid meter mode.
                                             * For software bundle rate limit,
                                             * the meter mode should be
                                             * RULE_TYPE_METER_MODE_FLOW.
                                             */
    RULE_TYPE_E_INVALID_ACTION,             /* Invalid action.
                                             * For software bundle rate limit,
                                             * the violate-action should be
                                             * VIOLATE_ACTION_DROP.
                                             */
    RULE_TYPE_MATCH_DENY,
    RULE_TYPE_MATCH_PERMIT,
    RULE_TYPE_TIME_RANGE_NONEXISTED,
    RULE_TYPE_NO_HW_RULE_SET,
    RULE_TYPE_NO_COUNTER_SET,
    RULE_TYPE_NO_MATCH,
    RULE_TYPE_NOT_SUPPORT_BUNDLE_RATE_CONTORL,

    RULE_TYPE_E_ACE_IP_PROTOCOL

} RULE_TYPE_RETURN_TYPE_T;

/*For CPU interface */
typedef enum
{
    /* MAC ACL */
    RULE_TYPE_Pkt2CpuRule_EAPS_MASTER_REDIRECT,
    RULE_TYPE_Pkt2CpuRule_EAPS_MASTER_DROP,
    RULE_TYPE_Pkt2CpuRule_EAPS_TRANSIT_COPY,
    RULE_TYPE_Pkt2CpuRule_EAPS_TRANSIT_DROP,
    RULE_TYPE_Pkt2CpuRule_DOT1X_REDIRECT,
    RULE_TYPE_Pkt2CpuRule_DOT1X_DROP_OTHER,

    RULE_TYPE_Pkt2CpuRule_ARP_REPLY,
    RULE_TYPE_Pkt2CpuRule_ALL_ARP_REPLY,
    RULE_TYPE_Pkt2CpuRule_ARP_REQUEST,
    RULE_TYPE_Pkt2CpuRule_ALL_ARP_REQUEST,
    RULE_TYPE_Pkt2CpuRule_RIP,
    RULE_TYPE_Pkt2CpuRule_OSPF_MAC5,
    RULE_TYPE_Pkt2CpuRule_OSPF_MAC6,
    RULE_TYPE_Pkt2CpuRule_OSPF_OTHERS,
#if (SYS_CPNT_OSPF6 == TRUE)
    /* added by steven.gao for OSPFv3 */
    RULE_TYPE_Pkt2CpuRule_OSPF6_MAC5,
    RULE_TYPE_Pkt2CpuRule_OSPF6_MAC6,
    RULE_TYPE_Pkt2CpuRule_OSPF6_HELLO,
    RULE_TYPE_Pkt2CpuRule_OSPF6_OTHER,
#endif
    RULE_TYPE_Pkt2CpuRule_DVMRP,

    RULE_TYPE_Pkt2CpuRule_PIM,
    RULE_TYPE_Pkt2CpuRule_VRRP,
    RULE_TYPE_Pkt2CpuRule_RIPNG,
    RULE_TYPE_Pkt2CpuRule_RESERVED_UDP,
    RULE_TYPE_Pkt2CpuRule_IPV6_MC,
    RULE_TYPE_Pkt2CpuRule_UNKNOWN_IPMC,
    RULE_TYPE_Pkt2CpuRule_UNKNOWN_L2MC,
    RULE_TYPE_Pkt2CpuRule_IP_BCAST,
    RULE_TYPE_Pkt2CpuRule_L2_SLF,
    RULE_TYPE_Pkt2CpuRule_CFM_CONTROL,
    RULE_TYPE_Pkt2CpuRule_L2PT,
    RULE_TYPE_Pkt2CpuRule_CDP,
    RULE_TYPE_Pkt2CpuRule_PVST,
    RULE_TYPE_Pkt2CpuRule_ORG_SPECIFIC1,
    RULE_TYPE_Pkt2CpuRule_ORG_SPECIFIC2,
    RULE_TYPE_Pkt2CpuRule_ORG_SPECIFIC3,
    /* IPv4 */
    RULE_TYPE_Pkt2CpuRule_ALL_HOST,
    RULE_TYPE_Pkt2CpuRule_ALL_ROUTER,
    RULE_TYPE_Pkt2CpuRule_MY_MAC_MY_IP,
    RULE_TYPE_Pkt2CpuRule_IP_OPTION,
    RULE_TYPE_Pkt2CpuRule_SLF,
    RULE_TYPE_Pkt2CpuRule_IGMP,
    RULE_TYPE_Pkt2CpuRule_DHCP_CLIENT,
    RULE_TYPE_Pkt2CpuRule_DHCP_SERVER,
    RULE_TYPE_Pkt2CpuRule_HBT,
    RULE_TYPE_Pkt2CpuRule_BPDU,
    RULE_TYPE_Pkt2CpuRule_L2CP,
    RULE_TYPE_Pkt2CpuRule_ORG_SPECIFIC,
    RULE_TYPE_Pkt2CpuRule_DOS_CHARGEN,
    RULE_TYPE_Pkt2CpuRule_DOS_ECHO,
    RULE_TYPE_Pkt2CpuRule_DOS_TCP_FLOODING,
    RULE_TYPE_Pkt2CpuRule_DOS_UDP_FLOODING,
    RULE_TYPE_Pkt2CpuRule_DOS_WIN_NUKE,
    RULE_TYPE_Pkt2CpuRule_DOS_SMURF,
    /* IPv6 */
    RULE_TYPE_Pkt2CpuRule_IPV6_ALL_NODES,
    RULE_TYPE_Pkt2CpuRule_IPV6_ALL_ROUTERS,
    RULE_TYPE_Pkt2CpuRule_UNKNOWN_IPV6MC,
    RULE_TYPE_Pkt2CpuRule_DHCP6_CLIENT,
    RULE_TYPE_Pkt2CpuRule_DHCP6_SERVER,
    RULE_TYPE_Pkt2CpuRule_LinkLocal,
    RULE_TYPE_Pkt2CpuRule_PIM6,
    RULE_TYPE_Pkt2CpuRule_MAX

}RULE_TYPE_Pkt2CpuRule_T;

// TODO: Rename RULE_TYPE_Packettype_MY_MAC_CRAFT_PORT to
//              RULE_TYPE_PacketType_MY_MAC_CRAFT_PORT

#define RULE_TYPE_PacketType_LIST(_)                                    \
    /* MAC ACL */                                                       \
    _(RULE_TYPE_PacketType_EAPS),                                       \
    _(RULE_TYPE_PacketType_DOT1X),                                      \
                                                                        \
    _(RULE_TYPE_PacketType_ARP_REPLY),                                  \
    _(RULE_TYPE_PacketType_ARP_REQUEST),                                \
    _(RULE_TYPE_PacketType_RIP),                                        \
    _(RULE_TYPE_PacketType_OSPF),                                       \
    _(RULE_TYPE_PacketType_OSPF6),  /* OSPFv3 */                        \
    _(RULE_TYPE_PacketType_DVMRP),                                      \
                                                                        \
    _(RULE_TYPE_PacketType_PIM),                                        \
    _(RULE_TYPE_PacketType_VRRP),                                       \
    _(RULE_TYPE_PacketType_RESERVED_UDP),                               \
    _(RULE_TYPE_PacketType_UNKNOWN_IPMC),                               \
    _(RULE_TYPE_PacketType_IP_BCAST),                                   \
    _(RULE_TYPE_PacketType_L2_SLF),                                     \
    _(RULE_TYPE_PacketType_CFM),                                        \
    _(RULE_TYPE_PacketType_RAPS),                                       \
    _(RULE_TYPE_PacketType_L2PT),                                       \
    _(RULE_TYPE_PacketType_CDP),                                        \
    _(RULE_TYPE_PacketType_PVST),                                       \
    _(RULE_TYPE_PacketType_ORG_SPECIFIC1),                              \
    _(RULE_TYPE_PacketType_ORG_SPECIFIC2),                              \
    _(RULE_TYPE_PacketType_ORG_SPECIFIC3),                              \
    _(RULE_TYPE_PacketType_PTP_EVENT_ETH),   /* IEEE 1588, PTP. L2  */  \
                                                                        \
    /* IPv4 */                                                          \
    _(RULE_TYPE_PacketType_ALL_HOST),                                   \
    _(RULE_TYPE_PacketType_ALL_ROUTER),                                 \
    _(RULE_TYPE_PacketType_MY_MAC_MY_IP),                               \
    _(RULE_TYPE_PacketType_IP_OPTION),                                  \
    _(RULE_TYPE_PacketType_SLF),                                        \
    _(RULE_TYPE_PacketType_IGMP),                                       \
    _(RULE_TYPE_PacketType_DHCP_CLIENT),                                \
    _(RULE_TYPE_PacketType_DHCP_SERVER),                                \
    _(RULE_TYPE_PacketType_HBT),                                        \
    _(RULE_TYPE_PacketType_BPDU),                                       \
    _(RULE_TYPE_PacketType_L2CP),                                       \
    _(RULE_TYPE_PacketType_ORG_SPECIFIC),                               \
    _(RULE_TYPE_PacketType_MY_MAC_ARP),                                 \
    _(RULE_TYPE_Packettype_MY_MAC_CRAFT_PORT),  /* Trap my MAC on craft port */  \
    _(RULE_TYPE_PacketType_DOS_CHARGEN),                                \
    _(RULE_TYPE_PacketType_DOS_ECHO),                                   \
    _(RULE_TYPE_PacketType_DOS_TCP_FLOODING),                           \
    _(RULE_TYPE_PacketType_DOS_UDP_FLOODING),                           \
    _(RULE_TYPE_PacketType_DOS_WIN_NUKE),                               \
    _(RULE_TYPE_PacketType_DOS_SMURF),                                  \
    _(RULE_TYPE_PacketType_PTP_EVENT_UDP),  /* IEEE 1588, PTP. UDP */   \
    _(RULE_TYPE_PacketType_PTP_GENERAL_UDP),  /* IEEE 1588, PTP. UDP */ \
                                                                        \
    /* IPv6 */                                                          \
    _(RULE_TYPE_PacketType_IPV6_ALL_NODES),                             \
    _(RULE_TYPE_PacketType_IPV6_ALL_ROUTERS),                           \
    _(RULE_TYPE_PacketType_UNKNOWN_IPV6MC),                             \
    _(RULE_TYPE_PacketType_MLD_Query),                                  \
    _(RULE_TYPE_PacketType_MLD_Report),                                 \
    _(RULE_TYPE_PacketType_MLD_Done),                                   \
    _(RULE_TYPE_PacketType_MLDV2_Reports),                              \
    _(RULE_TYPE_PacketType_DHCP6_CLIENT),                               \
    _(RULE_TYPE_PacketType_DHCP6_SERVER),                               \
    _(RULE_TYPE_PacketType_LinkLocal),                                  \
    _(RULE_TYPE_PacketType_PIM6)

#define RULE_TYPE_PacketType_LIST_ENUM(pkt_type)    pkt_type
#define RULE_TYPE_PacketType_LIST_NAME(pkt_type)    #pkt_type

#define RULE_TYPE_PacketType_VALUE_ARRAY                                \
        RULE_TYPE_PacketType_LIST(RULE_TYPE_PacketType_LIST_ENUM)

#define RULE_TYPE_PacketType_STRING_ARRAY                               \
        RULE_TYPE_PacketType_LIST(RULE_TYPE_PacketType_LIST_NAME)

typedef enum
{
    RULE_TYPE_PacketType_VALUE_ARRAY,
    RULE_TYPE_PacketType_MAX
} RULE_TYPE_PacketType_T;

typedef enum
{
    /* MAC ACL */
    RULE_TYPE_PACKET2CPU_TYPE_DOT1X,
    RULE_TYPE_PACKET2CPU_TYPE_CDP,
    RULE_TYPE_PACKET2CPU_TYPE_NBX_SA,
    RULE_TYPE_PACKET2CPU_TYPE_NBX_ETH,
    RULE_TYPE_PACKET2CPU_TYPE_IPMC_DATA,
    RULE_TYPE_PACKET2CPU_TYPE_NBX_DA,
    RULE_TYPE_PACKET2CPU_TYPE_ARP_REPLY,
    RULE_TYPE_PACKET2CPU_TYPE_ARP_REQUEST,
    RULE_TYPE_PACKET2CPU_TYPE_IPV6_MC,

    /* IPv4 */
    RULE_TYPE_PACKET2CPU_TYPE_RIPV1,
    RULE_TYPE_PACKET2CPU_TYPE_IGMP,
    RULE_TYPE_PACKET2CPU_TYPE_DHCPC_BC,
    RULE_TYPE_PACKET2CPU_TYPE_DHCPS_BC,

}RULE_TYPE_Packet2CPUType_T; /* for MO use*/

typedef enum
{
    IN_ACTION_NEW_COS=1,
    IN_ACTION_NEW_IP_DSCP,
    IN_ACTION_NEW_IP_PRECEDENCE,
    IN_ACTION_NEW_IPV6_DSCP,
    IN_ACTION_NEW_PHB,
    IN_ACTION_INVALID
} RULE_TYPE_UI_InActionType_T; /* for CLI/Web */

typedef enum
{
    CONFIRM_ACTION_TRANSMIT,
    CONFIRM_ACTION_REMARK_DSCP_TRANSMIT,
    CONFIRM_ACTION_DROP,
    CONFIRM_ACTION_INVALID
} RULE_TYPE_UI_ConfirmActionType_T; /* confirm action for CLI/Web */

typedef enum
{
    EXCEED_ACTION_TRANSMIT,
    EXCEED_ACTION_REMARK_DSCP_TRANSMIT,
    EXCEED_ACTION_DROP,
    EXCEED_ACTION_INVALID
} RULE_TYPE_UI_ExceedActionType_T; /* exceed action for CLI/Web */

typedef enum
{
    VIOLATE_ACTION_TRANSMIT,
    VIOLATE_ACTION_REMARK_DSCP_TRANSMIT,
    VIOLATE_ACTION_DROP,
    VIOLATE_ACTION_INVALID
} RULE_TYPE_UI_ViolateActionType_T; /* violate action for CLI/Web */

typedef enum
{
    CLASS_ACTION_SET_PRIORITY = 1,
    CLASS_ACTION_INVALID,
} RULE_TYPE_UI_ClassActionType_T;

#define RULE_TYPE_FUNCTION_TYPE_LIST(_)                                                                                    \
/*  (Function type enum,   "Function type string",   Tcam capability mapping(RULE_TYPE_TCAM_Capability_T)) */              \
    _(RULE_TYPE_INGRESS_IP_ACL,                "RT_INGRESS_IP_ACL",                RULE_TYPE_TCAM_CAP_IPV4_ACL)            \
    _(RULE_TYPE_INGRESS_MAC_ACL,               "RT_INGRESS_MAC_ACL",               RULE_TYPE_TCAM_CAP_MAC_ACL)             \
    _(RULE_TYPE_IP_DIFFSERV,                   "RT_IP_DIFFSERV",                   RULE_TYPE_TCAM_CAP_IPV4_DS)             \
    _(RULE_TYPE_MAC_DIFFSERV,                  "RT_MAC_DIFFSERV",                  RULE_TYPE_TCAM_CAP_MAC_DS)              \
    _(RULE_TYPE_TCP_UDP_PORT_MAPPING,          "RT_TCP_UDP_PORT_MAPPING",          RULE_TYPE_TCAM_CAP_RESERVED)            \
/* FIXME: MUST BE DELETED. action is remark CoS or redirect to port. */                                                    \
    _(RULE_TYPE_IFP_PBF,                       "RT_IFP_PBF",                       RULE_TYPE_TCAM_CAP_RESERVED)            \
/* FIXME: MUST BE DELETED. */                                                                                              \
    _(RULE_TYPE_PBR,                           "RT_PBR",                           RULE_TYPE_TCAM_CAP_RESERVED)            \
    _(RULE_TYPE_ROUTING,                       "RT_ROUTING",                       RULE_TYPE_TCAM_CAP_ROUTING)             \
    _(RULE_TYPE_INGRESS_PPORT_RATELIMIT,       "RT_INGRESS_PPORT_RATELIMIT",       RULE_TYPE_TCAM_CAP_RATE_LIMITE)         \
    _(RULE_TYPE_INGRESS_PPORT_PVLAN_RATELIMIT, "RT_INGRESS_PPORT_PVLAN_RATELIMIT", RULE_TYPE_TCAM_CAP_RATE_LIMITE)         \
    _(RULE_TYPE_INGRESS_PVLAN_RATELIMIT,       "RT_INGRESS_PVLAN_RATELIMIT",       RULE_TYPE_TCAM_CAP_RATE_LIMITE)         \
    _(RULE_TYPE_INGRESS_IPV6_STD_ACL,          "RT_INGRESS_IPV6_STD_ACL",          RULE_TYPE_TCAM_CAP_IPV6_STD_ACL)        \
    _(RULE_TYPE_INGRESS_IPV6_EXT_ACL,          "RT_INGRESS_IPV6_EXT_ACL",          RULE_TYPE_TCAM_CAP_IPV6_EXT_ACL)        \
    _(RULE_TYPE_INGRESS_IPV6_STD_DIFFSERV,     "RT_INGRESS_IPV6_STD_DIFFSERV",     RULE_TYPE_TCAM_CAP_IPV6_STD_DS)         \
    _(RULE_TYPE_INGRESS_IPV6_EXT_DIFFSERV,     "RT_INGRESS_IPV6_EXT_DIFFSERV",     RULE_TYPE_TCAM_CAP_IPV6_EXT_DS)         \
    _(RULE_TYPE_WEBAUTH_PRIO_HIGH,             "RT_WEBAUTH_PRIO_HIGH",             RULE_TYPE_TCAM_CAP_WEB_AUTH)            \
    _(RULE_TYPE_WEBAUTH_PRIO_MED,              "RT_WEBAUTH_PRIO_MED",              RULE_TYPE_TCAM_CAP_WEB_AUTH)            \
    _(RULE_TYPE_WEBAUTH_PRIO_LOW,              "RT_WEBAUTH_PRIO_LOW",              RULE_TYPE_TCAM_CAP_WEB_AUTH)            \
    _(RULE_TYPE_IP_SOURCE_GUARD_PRIO_HIGH,     "RT_IP_SOURCE_GUARD_PRIO_HIGH",     RULE_TYPE_TCAM_CAP_IP_SOURCE_GUSRD)     \
    _(RULE_TYPE_IP_SOURCE_GUARD_PRIO_LOW,      "RT_IP_SOURCE_GUARD_PRIO_LOW",      RULE_TYPE_TCAM_CAP_IP_SOURCE_GUSRD)     \
    _(RULE_TYPE_IPV6_SG_PERMIT_HOST,           "RT_IPV6_SG_PERMIT_HOST",           RULE_TYPE_TCAM_CAP_IPV6_SOURCE_GUSRD)   \
    _(RULE_TYPE_IPV6_SG_DENY_ALL,              "RT_IPV6_SG_DENY_ALL",              RULE_TYPE_TCAM_CAP_IPV6_SOURCE_GUSRD)   \
    _(RULE_TYPE_PACKET_TO_CPU_LINK_LOCAL_ADDR, "RT_PACKET_TO_CPU_LINK_LOCAL_ADDR", RULE_TYPE_TCAM_CAP_LINK_LOCAL)          \
    _(RULE_TYPE_LINK_LOCAL_ADDR_L3_ROUTABLE,   "RT_LINK_LOCAL_ADDR_L3_ROUTABLE",   RULE_TYPE_TCAM_CAP_LINK_LOCAL)          \
    _(RULE_TYPE_PACKET_TO_CPU_PIM6,            "RT_PACKET_TO_CPU_PIM6",            RULE_TYPE_TCAM_CAP_CPU_INTERFACE)       \
    _(RULE_TYPE_PACKET_TO_CPU_PRIO_1,          "RT_PACKET_TO_CPU_PRIO_1(HIG)",     RULE_TYPE_TCAM_CAP_CPU_INTERFACE)       \
    _(RULE_TYPE_PACKET_TO_CPU_PRIO_2,          "RT_PACKET_TO_CPU_PRIO_2",          RULE_TYPE_TCAM_CAP_CPU_INTERFACE)       \
    _(RULE_TYPE_PACKET_TO_CPU_PRIO_3,          "RT_PACKET_TO_CPU_PRIO_3",          RULE_TYPE_TCAM_CAP_CPU_INTERFACE)       \
    _(RULE_TYPE_PACKET_TO_CPU_PRIO_4,          "RT_PACKET_TO_CPU_PRIO_4(DEF)",     RULE_TYPE_TCAM_CAP_CPU_INTERFACE)       \
    _(RULE_TYPE_PACKET_TO_CPU_PRIO_5,          "RT_PACKET_TO_CPU_PRIO_5",          RULE_TYPE_TCAM_CAP_CPU_INTERFACE)       \
    _(RULE_TYPE_PACKET_TO_CPU_PRIO_6,          "RT_PACKET_TO_CPU_PRIO_6",          RULE_TYPE_TCAM_CAP_CPU_INTERFACE)       \
    _(RULE_TYPE_PACKET_TO_CPU_PRIO_7,          "RT_PACKET_TO_CPU_PRIO_7",          RULE_TYPE_TCAM_CAP_CPU_INTERFACE)       \
    _(RULE_TYPE_PACKET_TO_CPU_PRIO_8,          "RT_PACKET_TO_CPU_PRIO_8(LOW)",     RULE_TYPE_TCAM_CAP_CPU_INTERFACE)       \
    _(RULE_TYPE_PACKET_TO_CPU_MLD,             "RT_PACKET_TO_CPU_MLD",             RULE_TYPE_TCAM_CAP_CPU_INTERFACE)       \
    _(RULE_TYPE_MAC_VLAN,                      "RT_MAC_VLAN",                      RULE_TYPE_TCAM_CAP_MAC_VLAN)            \
    _(RULE_TYPE_PROTOCOL_VLAN,                 "RT_PROTOCOL_VLAN",                 RULE_TYPE_TCAM_CAP_PROTOCOL_VLAN)       \
    _(RULE_TYPE_VOICE_VLAN,                    "RT_VOICE_VLAN",                    RULE_TYPE_TCAM_CAP_VOICE_VLAN)          \
    _(RULE_TYPE_IP_SUBNET_VLAN,                "RT_IP_SUBNET_VLAN",                RULE_TYPE_TCAM_CAP_IP_SUBNET_VLAN)      \
    /* action is replace vid or add vlan tag. */                                                                           \
    _(RULE_TYPE_VFP_PBF,                       "RT_VFP_PBF",                       RULE_TYPE_TCAM_CAP_RESERVED)            \
    _(RULE_TYPE_EGRESS_PPORT_RATELIMIT,        "RT_EGRESS_PPORT_RATELIMIT",        RULE_TYPE_TCAM_CAP_RESERVED)            \
    _(RULE_TYPE_EGRESS_PPORT_PVLAN_RATELIMIT,  "RT_EGRESS_PPORT_PVLAN_RATELIMIT",  RULE_TYPE_TCAM_CAP_RESERVED)            \
    _(RULE_TYPE_EGRESS_PVLAN_RATELIMIT,        "RT_EGRESS_PVLAN_RATELIMIT",        RULE_TYPE_TCAM_CAP_RESERVED)            \
    _(RULE_TYPE_EGRESS_IPV6_STD_ACL,           "RT_EGRESS_IPV6_STD_ACL",           RULE_TYPE_TCAM_CAP_EGRESS_IPV6_STD_ACL) \
    _(RULE_TYPE_EGRESS_IPV6_EXT_ACL,           "RT_EGRESS_IPV6_EXT_ACL",           RULE_TYPE_TCAM_CAP_EGRESS_IPV6_EXT_ACL) \
    _(RULE_TYPE_EGRESS_IP_ACL,                 "RT_EGRESS_IP_ACL",                 RULE_TYPE_TCAM_CAP_EGRESS_IPV4_ACL)     \
    _(RULE_TYPE_EGRESS_MAC_ACL,                "RT_EGRESS_MAC_ACL",                RULE_TYPE_TCAM_CAP_EGRESS_MAC_ACL)      \
    _(RULE_TYPE_EGRESS_IPV6_STD_DIFFSERV,      "RT_EGRESS_IPV6_STD_DIFFSERV",      RULE_TYPE_TCAM_CAP_EGRESS_IPV6_STD_DS)  \
    _(RULE_TYPE_EGRESS_IPV6_EXT_DIFFSERV,      "RT_EGRESS_IPV6_EXT_DIFFSERV",      RULE_TYPE_TCAM_CAP_EGRESS_IPV6_EXT_DS)  \
    _(RULE_TYPE_EGRESS_IP_DIFFSERV,            "RT_EGRESS_IP_DIFFSERV",            RULE_TYPE_TCAM_CAP_EGRESS_IPV4_DS)      \
    _(RULE_TYPE_EGRESS_MAC_DIFFSERV,           "RT_EGRESS_MAC_DIFFSERV",           RULE_TYPE_TCAM_CAP_EGRESS_MAC_DS)       \
    _(RULE_TYPE_DOT1X,                         "RT_DOT1X",                         RULE_TYPE_TCAM_CAP_RESERVED)            \
    _(RULE_TYPE_FUNCTION_TYPE_MAX,             "RT_FUNCTION_TYPE_MAX",             RULE_TYPE_TCAM_CAP_TOTAL)

#define RULE_TYPE_FUNCTION_TYPE_ENUM(name, str, capability) name,
#define RULE_TYPE_FUNCTION_TYPE_STR(name, str, capability) str,

typedef enum RULE_TYPE_FunctionType_E
{
RULE_TYPE_FUNCTION_TYPE_LIST(RULE_TYPE_FUNCTION_TYPE_ENUM)

    /* redefined specific enum value */
    RULE_TYPE_PACKET_TO_CPU_HIGH    = RULE_TYPE_PACKET_TO_CPU_PRIO_1,
    RULE_TYPE_PACKET_TO_CPU_DEFAULT = RULE_TYPE_PACKET_TO_CPU_PRIO_4,
    RULE_TYPE_PACKET_TO_CPU_LOW     = RULE_TYPE_PACKET_TO_CPU_PRIO_8,
} RULE_TYPE_FunctionType_T;

#define RULE_TYPE_FUNCTION_TYPE_STRING_ARRAY                  \
{                                                             \
    RULE_TYPE_FUNCTION_TYPE_LIST(RULE_TYPE_FUNCTION_TYPE_STR) \
}

typedef struct
{
    UI8_T port_list[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK][SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST];
} RULE_TYPE_TRUNK_MEMBER_T;

/*
 * Typedef: enum RULE_TYPE_INTERFACE_TYPE_E
 *
 * Description: enumerator for interface
 *
 * Fields:
 *      RULE_TYPE_INTERFACE_UPORT    - the interface is of user port type  {unit, port}
 *      RULE_TYPE_INTERFACE_LPORT    - the interface is of logic port type {lport}
 *      RULE_TYPE_INTERFACE_TRUNK    - the interface is of trunk type {trunk_id}
 *      RULE_TYPE_INTERFACE_UNIT     - the interface is of unit {unit}
 *      RULE_TYPE_INTERFACE_ALL_UNIT - the interface is of all unit
 *      RULE_TYPE_INTERFACE_UNSPEC   - uninitial value
 */
typedef enum
{
    RULE_TYPE_INTERFACE_UPORT = 0,
    RULE_TYPE_INTERFACE_LPORT,
    RULE_TYPE_INTERFACE_TRUNK,
    RULE_TYPE_INTERFACE_UNIT,
    RULE_TYPE_INTERFACE_ALL_UNIT,
    RULE_TYPE_INTERFACE_CRAFT_PORT,
    RULE_TYPE_INTERFACE_CPU_PORT,
    RULE_TYPE_INTERFACE_UNSPEC,
    RULE_TYPE_INTERFACE_MAX = RULE_TYPE_INTERFACE_UNSPEC
} RULE_TYPE_INTERFACE_TYPE_E;

#define RULE_TYPE_INTERFACE_TYPE_STR(interface_type)                            \
    (                                                                           \
        (interface_type == RULE_TYPE_INTERFACE_ALL_UNIT) ? "all unit" :         \
        (interface_type == RULE_TYPE_INTERFACE_UNIT) ? "unit" :                 \
        (interface_type == RULE_TYPE_INTERFACE_UPORT) ? "eth" :                 \
        (interface_type == RULE_TYPE_INTERFACE_TRUNK) ? "trunk" :               \
        (interface_type == RULE_TYPE_INTERFACE_CRAFT_PORT) ? "craft port" :     \
        (interface_type == RULE_TYPE_INTERFACE_CPU_PORT) ? "cpu port" : "??"    \
    )

/*
 * Typedef: structure RULE_TYPE_INTERFACE_INFO_T
 *
 * Description: Defines the interface info
 *
 * Fields:
 *      type - the interface type -- port/unit/trunk
 *
 *      uport       -  info about the {unit, port}   - relevant to RULE_TYPE_INTERFACE_UPORT
 *              unit    - the unit number
 *              port    - the port number
 *      lport       -  info about the {lport}        - relevant to RULE_TYPE_INTERFACE_LPORT
 *      trunk_id    -  info about the {trunk_id}     - relevant to RULE_TYPE_INTERFACE_TRUNK
 *      unit        -  info about the {unit number}  - relevant to RULE_TYPE_INTERFACE_UNIT
 */
typedef struct
{
    RULE_TYPE_INTERFACE_TYPE_E  type;

    union
    {
        struct
        {
            UI32_T              unit;
            UI32_T              port;
        } uport;

        UI32_T                  lport;
        UI32_T                  unit;
        UI32_T                  trunk_id;
    };

    RULE_TYPE_InOutDirection_T  direction;
} RULE_TYPE_INTERFACE_INFO_T, *RULE_TYPE_INTERFACE_INFO_PTR_T;

typedef union
{
    struct
    {
        BOOL_T to_cpu;
        BOOL_T flood;
    }common;
    struct
    {
        UI32_T vid;
        UI32_T mode;
    }eaps;
    struct
    {
        UI32_T ifindex;
        UI8_T cpu_mac[6];
    }dot1x;
    struct
    {
        UI16_T slf_vid;
        UI8_T action;
    }slf;
    struct
    {
        BOOL_T all_arp_to_cpu;
    }arp;
    struct
    {
        UI32_T  vlan_id;
        UI8_T   addr[SYS_ADPT_IPV6_ADDR_LEN];
    }linklocal;
    struct
    {
        UI8_T mac[6];
    }l2pt;
    BOOL_T all_dhcp_to_cpu;
    UI16_T dhcp_vid;
    BOOL_T cfm_to_cpu;
}RULE_TYPE_CpuRuleInfo_T;

/*
 * Typedef: enum RULE_TYPE_INSTANCE_TYPE_T
 *
 * Description: Instance type.
 *
 * Fields:
 *  RULE_TYPE_NIL                           - Nil object.
 *  RULE_TYPE_INST_CONTAINER                - Container.
 *  RULE_TYPE_INST_POLICY_MAP               - Policy-map.
 *  RULE_TYPE_INST_CLASS_MAP                - Class-map.
 *  RULE_TYPE_INST_ACL                      - ACL.
 *  RULE_TYPE_INST_MF_ACE                   - MF ACE.
 *  RULE_TYPE_INST_ACE                      - ACE.
 *  RULE_TYPE_INST_CPU_IF                   - CPU if
 *  RULE_TYPE_INST_RULE                     - Rule.
 *
 */

typedef enum
{
    RULE_TYPE_NIL               = 0, /* NULL */
    RULE_TYPE_INST_TYPE_FIRST   = 1,
    RULE_TYPE_INST_CONTAINER    = RULE_TYPE_INST_TYPE_FIRST,
    RULE_TYPE_INST_PBR,
    RULE_TYPE_INST_POLICY_MAP,
    RULE_TYPE_INST_CLASS_MAP,
    RULE_TYPE_INST_ACL,
    RULE_TYPE_INST_MF_ACE,
    RULE_TYPE_INST_ACE,
    RULE_TYPE_INST_CPU_IF,
    RULE_TYPE_INST_RULE,
    RULE_TYPE_INST_OM_USER_ACE,
    RULE_TYPE_INST_OM_WORK_ACE,
    RULE_TYPE_INST_OM_USER_ACL,
    RULE_TYPE_INST_OM_WORK_ACL,
    RULE_TYPE_INST_TYPE_LAST    = RULE_TYPE_INST_RULE,
} RULE_TYPE_INSTANCE_TYPE_T;

/*
 * Typedef: struct RULE_TYPE_SHM_POINTER_T
 *
 * Description: pointer of shared memory.
 *
 * Fields:
 *  type                                    - Type.
 *  offset                                  - Offset from based address.
 *
 */

typedef struct
{
    RULE_TYPE_INSTANCE_TYPE_T   type;
    UI32_T                      offset;
} RULE_TYPE_SHM_POINTER_T;

/*
 * Typedef: struct RULE_TYPE_INSTANCE_LINKS_T
 *
 * Description: Link conllection of double linked list.
 *
 * Fields:
 *  prev                                    - Points to the previous sibling node.
 *  next                                    - Points to the next sibling node.
 *  first_node                              - Points to first child node.
 *  last_node                               - Points to last child node.
 *
 */

typedef struct
{
    RULE_TYPE_SHM_POINTER_T     prev;
    RULE_TYPE_SHM_POINTER_T     next;
    RULE_TYPE_SHM_POINTER_T     first_node;
    RULE_TYPE_SHM_POINTER_T     last_node;
    RULE_TYPE_SHM_POINTER_T     parent;
} RULE_TYPE_INSTANCE_LINKS_T;

/*
 * Typedef: struct RULE_TYPE_INSTANCE_T
 *
 * Description: Class / rule instance's header (base class). Each instance was
 *              a node of double linked list. It also contain a double linked
 *              list.
 *
 * Fields:
 *  links                                   - Points to the previous, next node
 *                                            and new list.
 *  type                                    - Type of the instance.
 *  data                                    - DON'T access this value. Cases
 *                                            the instance to derived data
 *                                            structure based on type.
 *
 */

typedef struct
{
    RULE_TYPE_INSTANCE_LINKS_T  links;
    RULE_TYPE_INSTANCE_TYPE_T   type;
    UI8_T                       data[0];
} RULE_TYPE_INSTANCE_T, *RULE_TYPE_INSTANCE_PTR_T;

/* ace entry of following ace type:
 * RULE_TYPE_IP_ACL,
 * RULE_TYPE_IP_STD_ACL,
 * RULE_TYPE_IP_EXT_ACL,
 * Note: entry's index is key
 */
typedef struct
{
    /*
    acePrecedence read-only
    */
    UI32_T  acePrecedence ;
    UI32_T  aceSourceIpAddr;            /* STD/EXT */
    UI32_T  aceSourceIpAddrBitmask;     /* STD/EXT */
    UI32_T  aceDestIpAddr;              /* EXT */
    UI32_T  aceDestIpAddrBitmask;       /* EXT */
    UI32_T  aceProtocol;                /* EXT */
    UI32_T  acePrec;                    /* EXT /MF */
    UI32_T  aceTos;                     /* EXT */
    UI32_T  aceDscp;                    /* EXT /MF */
    UI32_T  aceSourcePortOp;            /* EXT */
    UI32_T  aceMinSourcePort;           /* EXT */
    UI32_T  aceMaxSourcePort;           /* EXT */
    UI32_T  aceSourcePortBitmask;       /* EXT */
    UI32_T  aceDestPortOp;              /* EXT */
    UI32_T  aceMinDestPort;             /* EXT */
    UI32_T  aceMaxDestPort;             /* EXT */
    UI32_T  aceDestPortBitmask;         /* EXT */
    UI32_T  aceControlCode;             /* EXT */
    UI32_T  aceControlCodeBitmask;      /* EXT */
    /*tenderbulk.w*/
    UI32_T  aceFragmentInfo;            /* EXT */


    UI32_T  aceIpOptionOp;              /* EXT */
    #if 1
    UI32_T  aceMinIpOption;             /* EXT */
    UI32_T  aceMaxIpOption;             /* EXT */
    UI32_T  aceIcmpType;                /* EXT */
    UI32_T  aceIcmpCode;                /* EXT */
    #endif

} RULE_TYPE_IpAce_Entry_T ;

typedef struct
{
    UI32_T  aceDscp;                                    /* EXT /MF */
    UI32_T  aceFlowLabel;                               /* EXT */

    UI16_T  aceNextHeader;/* to present UNDEF value, NextHeader can't be UI8_T */ /* EXT */
    UI8_T   aceSourceIpAddr[SYS_ADPT_IPV6_ADDR_LEN];    /* STD */
    UI8_T   aceSourceIpAddrBitmask[SYS_ADPT_IPV6_ADDR_LEN];
    UI8_T   aceDestIpAddr[SYS_ADPT_IPV6_ADDR_LEN];      /* EXT */
    UI8_T   aceDestIpAddrBitmask[SYS_ADPT_IPV6_ADDR_LEN];

    UI32_T  aceSourcePortOp;
    UI32_T  aceMinSourcePort;
    UI32_T  aceMaxSourcePort;
    UI32_T  aceSourcePortBitmask;
    UI32_T  aceDestPortOp;
    UI32_T  aceMinDestPort;
    UI32_T  aceMaxDestPort;
    UI32_T  aceDestPortBitmask;
    UI32_T  aceIcmpType;                /* EXT */
} RULE_TYPE_Ipv6Ace_Entry_T ;

/* entry's index is key */
typedef struct
{
    UI32_T  acePktformat;
    UI32_T  aceVidOp;                                   /* MF */
    UI32_T  aceMinVid;                                  /* MF */
    UI32_T  aceVidBitmask;
    UI32_T  aceMaxVid;
    /*
    apply to ethe2 packet only
    */
    UI32_T  aceEtherTypeOp ;
    UI32_T  aceEtherTypeBitmask;
    UI32_T  aceMinEtherType ;
    UI32_T  aceMaxEtherType ;

    UI8_T   aceSourceMacAddr[SYS_ADPT_MAC_ADDR_LEN];
    /*
    use aceSourceMacAddrBitmask =0x0000000000
    then aceSourceMacAddr "any"
    */
    UI8_T   aceSourceMacAddrBitmask[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T   aceDestMacAddr[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T   aceDestMacAddrBitmask[SYS_ADPT_MAC_ADDR_LEN];

    /*Added by jinwang,to support match cos and cfi in GF chip,2006-1-24*/
    UI32_T aceCoSOp;
    UI32_T aceCoS;
    UI32_T aceCFIOp;
    UI32_T aceCFI;

    UI8_T   aceCosOp;
    UI8_T   aceMinCos;
    UI8_T   aceMaxCos;
    UI8_T   aceCosBitmask;

    UI8_T   aceSourcePortOp;                            /* MF */
    UI8_T   aceMinSourcePort;                           /* MF */
    UI8_T   aceMaxSourcePort;                           /* MF */
    UI8_T   aceSourcePortBitmask;                       /* MF */
} RULE_TYPE_MacAce_Entry_T;

#if (SYS_CPNT_DAI == TRUE)
typedef struct
{
    UI32_T               aceSenderIpAddr ;
    UI32_T               aceSenderIpAddrBitmask;
    UI8_T                aceSenderMacAddr[SYS_ADPT_MAC_ADDR_LEN];
    /*
    use aceSenderMacAddrBitmask =0x000000000000
    then aceSenderMacAddr "any"
    */
    UI8_T                aceSenderMacAddrBitmask[SYS_ADPT_MAC_ADDR_LEN];
    UI32_T               aceTargetIpAddr ;
    UI32_T               aceTargetIpAddrBitmask;
    UI8_T                aceTargetMacAddr[SYS_ADPT_MAC_ADDR_LEN];
    UI8_T                aceTargetMacAddrBitmask[SYS_ADPT_MAC_ADDR_LEN];
    RULE_TYPE_ArpType_T  aceArpType;
    BOOL_T               aceIsLog;
}RULE_TYPE_ArpAce_Entry_T;

typedef struct RULE_TYPE_ArpPktFormat_S
{
    UI32_T               srcIp;                              /* Source IP address             */
    UI32_T               dstIp;                              /* Destination IP address        */
    UI8_T                srcMacAddr[SYS_ADPT_MAC_ADDR_LEN];  /* Source hardware address       */
    UI8_T                dstMacAddr[SYS_ADPT_MAC_ADDR_LEN];  /* Destination hardware address  */
    RULE_TYPE_ArpType_T  opcode;                             /* Request or Reply              */
}RULE_TYPE_ArpPktFormat_T;

typedef struct
{
    UI32_T                      row_status;
    RULE_TYPE_ArpAce_Entry_T    arp;
} RULE_TYPE_MIB_ArpAce_Entry_T; /* for MIB */


#endif /* #if (SYS_CPNT_DAI == TRUE) */

typedef struct
{
    UI32_T                  new_queue;
    UI32_T                  new_vid;
    BOOL_T                  update_vid;
    BOOL_T                  update_priority;
    BOOL_T                  update_queue;
    UI8_T                   new_priority;
} RULE_TYPE_Action_Extend_Entry_T;


typedef struct
{
    UI32_T                  last_update_tick;

    union
    {
        struct
        {
            UI32_T          green_octets;
            UI32_T          yellow_octets;
            UI32_T          red_octets;
        };

        struct
        {
            UI32_T          green_packets;
            UI32_T          yellow_packets;
            UI32_T          red_packets;

            UI32_T          total_packets;
        };

        struct
        {
            UI32_T          receive_packets;
            UI32_T          drop_packets;
        };
    };

} RULE_TYPE_CounterRec_T;

/* This entry not form a MIB table, just represent as union structure for UI reference
 */
typedef struct
{
    RULE_TYPE_INSTANCE_T                    super;
    UI32_T                                  access;   /* 1:permit, 2:deny */

    struct
    {
#if (RULE_TYPE_ACE_MAC_IP_MIX == TRUE)
        RULE_TYPE_MacAce_Entry_T            mac;
#endif
        union
        {
            RULE_TYPE_IpAce_Entry_T         ip;
            RULE_TYPE_Ipv6Ace_Entry_T       ipv6;
#if (RULE_TYPE_ACE_MAC_IP_MIX != TRUE)
            RULE_TYPE_MacAce_Entry_T        mac;
#endif

#if (SYS_CPNT_DAI == TRUE)
            RULE_TYPE_ArpAce_Entry_T        arp;
#endif

        };
    } u;

    UI32_T                                  row_status;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    UI32_T                          time_range_index;
    UI8_T                           time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];
#endif /* #if (SYS_CPNT_TIME_BASED_ACL == TRUE) */

    RULE_TYPE_AclType_T                     ace_type;

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    RULE_TYPE_CounterRec_T                  counter;
#endif /* #if (SYS_CPNT_ACL_COUNTER == TRUE) */

    /*added for linux platform fb2,tenderbulk.w*/
    /*this is the index of ace entry in the array aceEntry[] stored in RULE_OM.c */
    UI32_T                                  aceEntryIndex;
} RULE_TYPE_Ace_Entry_T ;

#define RULE_TYPE_UI_ACE_PATTERN_TEMPLATE(TYPE_T)  \
        UI32_T op;            \
        union                 \
        {                     \
            struct            \
            {                 \
                TYPE_T data;  \
                TYPE_T mask;  \
            } s;              \
                              \
            struct            \
            {                 \
                TYPE_T min;   \
                TYPE_T max;   \
            } range;          \
        } u;

typedef struct
{
    RULE_TYPE_UI_ACE_PATTERN_TEMPLATE(UI32_T)
} RULE_TYPE_UI_Ace_Mac_Vid_T;

typedef struct
{
    RULE_TYPE_UI_ACE_PATTERN_TEMPLATE(UI32_T)
} RULE_TYPE_UI_Ace_Mac_Ethertype_T;

typedef struct
{
    RULE_TYPE_UI_ACE_PATTERN_TEMPLATE(UI8_T)
} RULE_TYPE_UI_Ace_Mac_Cos_T;

typedef struct
{
    RULE_TYPE_UI_ACE_PATTERN_TEMPLATE(UI8_T)
} RULE_TYPE_UI_Ace_IPv4_Protocol_T;

typedef struct
{
    RULE_TYPE_UI_ACE_PATTERN_TEMPLATE(UI8_T)
} RULE_TYPE_UI_Ace_IPv6_Traffic_Class_T;

typedef struct
{
    RULE_TYPE_UI_ACE_PATTERN_TEMPLATE(UI8_T)
} RULE_TYPE_UI_Ace_IPv6_Next_Header_T;

typedef struct
{
    RULE_TYPE_UI_ACE_PATTERN_TEMPLATE(UI32_T)
} RULE_TYPE_UI_Ace_IPv6_Flow_Label_T;

typedef struct
{
    RULE_TYPE_UI_ACE_PATTERN_TEMPLATE(UI8_T)
} RULE_TYPE_UI_Ace_Icmp_Type_T;

typedef struct
{
    RULE_TYPE_UI_ACE_PATTERN_TEMPLATE(UI32_T)
} RULE_TYPE_UI_Ace_L4_Port_T;

typedef struct
{
    UI8_T  addr[6];
    UI8_T  mask[6];
} RULE_TYPE_UI_ACE_Mac_Addr_T;

typedef struct
{
    UI8_T addr[SYS_ADPT_IPV4_ADDR_LEN];
    UI8_T mask[SYS_ADPT_IPV4_ADDR_LEN];
} RULE_TYPE_UI_ACE_IPv4_Addr_T;

typedef struct
{
    UI8_T  addr[SYS_ADPT_IPV6_ADDR_LEN];
    UI8_T  prefix_len;
} RULE_TYPE_UI_ACE_IPv6_Addr_T;

typedef struct
{
    RULE_TYPE_UI_ACE_Mac_Addr_T      sa;
    RULE_TYPE_UI_ACE_Mac_Addr_T      da;
    RULE_TYPE_UI_Ace_Mac_Ethertype_T ethertype;
    RULE_TYPE_UI_Ace_Mac_Vid_T       vid;
    RULE_TYPE_UI_Ace_Mac_Cos_T       cos;
} RULE_TYPE_UI_Ether_T;

typedef struct
{
    RULE_TYPE_UI_ACE_IPv4_Addr_T   sip;
    RULE_TYPE_UI_ACE_IPv4_Addr_T   dip;
    RULE_TYPE_UI_Ace_IPv4_Protocol_T protocol;

    struct
    {
        UI32_T op;    /* noOp /dscp / precedence / tos / precedence + tos */
        union
        {
            struct
            {
                UI8_T precedence;  // 0-2bit
                UI8_T tos;         // 3-6bit
            } tos;

            UI8_T ds;              // 0-5bit
        } u;
    } dscp;

} RULE_TYPE_UI_IPv4_T;

typedef struct
{
    RULE_TYPE_UI_ACE_IPv6_Addr_T           sip;
    RULE_TYPE_UI_ACE_IPv6_Addr_T           dip;
    RULE_TYPE_UI_Ace_IPv6_Traffic_Class_T  traffic_class;
    RULE_TYPE_UI_Ace_IPv6_Next_Header_T    next_header;
    RULE_TYPE_UI_Ace_IPv6_Flow_Label_T     flow_label;
} RULE_TYPE_UI_IPv6_T;;

typedef struct
{
    RULE_TYPE_UI_Ace_Icmp_Type_T icmp_type;
} RULE_TYPE_UI_ICMP_T;

typedef struct
{
    UI8_T fin: 1;
    UI8_T syn: 1;
    UI8_T rst: 1;
    UI8_T psh: 1;
    UI8_T ack: 1;
    UI8_T urg: 1;
} RULE_TYPE_TCP_Flag_T;

typedef struct
{
    struct
    {
        struct
        {
            union
            {
                RULE_TYPE_TCP_Flag_T bit;
                UI8_T code;
            } u;
        } data;

        struct
        {
            union
            {
                RULE_TYPE_TCP_Flag_T bit;
                UI8_T code;
            } u;
        } mask;
    } flags;
} RULE_TYPE_UI_TCP_T;

typedef struct
{
    RULE_TYPE_UI_Ace_L4_Port_T sport;
    RULE_TYPE_UI_Ace_L4_Port_T dport;
} RULE_TYPE_UI_L4_Common_T;

#if (SYS_CPNT_DAI == TRUE)
typedef struct
{
    RULE_TYPE_ArpType_T  arp_type;
    BOOL_T               is_log;

    struct
    {
        RULE_TYPE_UI_ACE_Mac_Addr_T   mac;
        RULE_TYPE_UI_ACE_IPv4_Addr_T  ip;
    } sender;

    struct
    {
        RULE_TYPE_UI_ACE_Mac_Addr_T   mac;
        RULE_TYPE_UI_ACE_IPv4_Addr_T  ip;
    } target;
} RULE_TYPE_UI_ARP_T;
#endif /* SYS_CPNT_DAI */

typedef struct
{
    RULE_TYPE_AclType_T                     ace_type;
    UI32_T                                  access;
    UI32_T                                  row_status;

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    UI32_T                                  time_range_index;
    UI8_T                                   time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH + 1];
#endif /* SYS_CPNT_TIME_BASED_ACL */

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    RULE_TYPE_CounterRec_T                  counter;
#endif /* SYS_CPNT_ACL_COUNTER */

    UI32_T                                  pkt_format;
    RULE_TYPE_UI_Ether_T                    ether;
    RULE_TYPE_UI_IPv4_T                     ipv4;
    RULE_TYPE_UI_IPv6_T                     ipv6;
    RULE_TYPE_UI_ICMP_T                     icmp;
    RULE_TYPE_UI_TCP_T                      tcp;
    RULE_TYPE_UI_L4_Common_T                l4_common;
#if (SYS_CPNT_DAI == TRUE)
    RULE_TYPE_UI_ARP_T                      arp;
#endif /* SYS_CPNT_DAI */
} RULE_TYPE_UI_Ace_Entry_T;

/*
 * Typedef: enum RULE_TYPE_COUNTER_ENABLE_T
 *
 * Description: enumerator for counter options.
 *
 * Fields:
 *  RULE_TYPE_COUNTER_ENABLE                - Counter is enabled.
 *  RULE_TYPE_COUNTER_DISABLE               - Counter is disabled.
 *
 */

typedef enum
{
    RULE_TYPE_COUNTER_ENABLE        = TRUE,
    RULE_TYPE_COUNTER_DISABLE       = FALSE
} RULE_TYPE_COUNTER_ENABLE_T;

#define RULE_TYPE_COUNTER_ENABLE_STR(cntr_enable)       \
    (                                                   \
        (cntr_enable==RULE_TYPE_COUNTER_ENABLE) ?       \
            "counterEnabled" :                          \
            "counterDisabled"                           \
    )

/*
 * Typedef: enum RULE_TYPE_MIRROR_ENABLE_T
 *
 * Description: enumerator for mirror options.
 *
 * Fields:
 *  RULE_TYPE_MIRROR_ENABLE                 - Mirror is enabled.
 *  RULE_TYPE_MIRROR_DISABLE                - Mirror is disabled.
 *
 */

typedef enum
{
    RULE_TYPE_MIRROR_ENABLE        = TRUE,
    RULE_TYPE_MIRROR_DISABLE       = FALSE
} RULE_TYPE_MIRROR_ENABLE_T;

#define RULE_TYPE_MIRROR_ENABLE_STR(mirr_enable)        \
(                                                       \
    (mirr_enable==RULE_TYPE_MIRROR_ENABLE) ?            \
        "mirrorEnabled" :                               \
        "mirrorDisabled"                                \
)

#define RULE_CTRL_INSTANCE_DEBUG_ON

typedef struct
{
    union
    {
        UI32_T pbits[4];  /* BCM */
        UI32_T bits[4];   /* RTK */
    };
} RULE_TYPE_PHY_PORT_BITMAP_T;

/*
 * Typedef: struct RULE_TYPE_DEV_RULE_INFO_T
 *
 * Description: The data structure was used to keep device rule information.
 *
 * Fields:
 *  unit                                    - Unit. Start from 1.
 *  device                                  - Device. Start from 0.
 *  group_id                                - (Broadcom platform) Group ID.
 *  fun_type                                - (Broadcom platform) Function type, Subgroup ID.
 *  pcl_id                                  - (Marvell platform) PCL ID.
 *  rule_id                                 - Device rule ID.
 */

typedef struct
{
    UI16_T unit;
    UI16_T device;

    RULE_TYPE_PHY_PORT_BITMAP_T port_bmp;
    RULE_TYPE_INTERFACE_INFO_T  interface;

    union
    {
        UI16_T group_id;

        struct
        {
            UI16_T pcl_id;
            UI16_T pcl_id_mask;
            UI32_T lookup_num;
        };
    };

    union
    {
        UI32_T fun_type;
        UI32_T virtual_group_id;    // Smaller value have higher priority
    };

    int priority;                   // Smaller value have higher priority

    UI32_T rule_id;
} RULE_TYPE_DEV_RULE_INFO_T;

/*
 * Typedef: struct RULE_TYPE_CONTAINER_T
 *
 * Description: The data structure was used to keep the MF ACE or ACE instance.
 *
 * Fields:
 *  super                                   - Base instance.
 *  array                                   - Array.
 */

typedef struct
{
    RULE_TYPE_INSTANCE_T        super;

    UI16_T                      id;

    union
    {
        RULE_TYPE_SHM_POINTER_T ptr;
    } array[10];

#ifdef RULE_CTRL_INSTANCE_DEBUG_ON

    UI8_T                       check_code;
#define _CONTAINER_CHECK_CODE           0xCC

#endif /* RULE_CTRL_INSTANCE_DEBUG_ON */

} RULE_TYPE_CONTAINER_T, *RULE_TYPE_CONTAINER_PTR_T;

/*
 * Typedef: struct RULE_TYPE_CLASS_INSTANCE_T
 *
 * Description: The data structure was used to keep the policy-map, class-map,
 *              and ACL instance.
 *
 * Fields:
 *  super                                   - Base instance.
 *  id                                      - Identity. It can be an ID for
 *                                            policy-map, class-map, or ACL ID
 *                                            based on super.type.
 *  hw_id                                   - Corresponding hardware identity.
 *  fun_type                                - This instance was used for which
 *                                            upper layer function. E.g., MAC
 *                                            ACL, IP ACL, ... etc.
 *                                            Used for Broadcom platform only.
 *  remap_fun_type                          - This instnace use which group
 *                                            mapped by funType to group ID
 *                                            table.
 *                                            Used for Broadcom platform only.
 *  counter_enable                          - Counter enable status.
 *  time_range_index                        - Bound time-range ID.
 *
 */

typedef struct
{
    RULE_TYPE_INSTANCE_T        super;

    union
    {
        UI16_T                  id;
        UI16_T                  policy_map_id;
        UI16_T                  class_map_id;
        UI16_T                  acl_id;
    };

    union
    {
        struct
        {
            RULE_TYPE_FunctionType_T    fun_type;
#if (SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT == TRUE)
            RULE_TYPE_FunctionType_T    remap_fun_type;
#endif /* SYS_CPNT_ACL_DYNAMIC_SELECTOR_ASSIGNMENT */
        };

        UI32_T mvl_pkt_type;
    };

#if (SYS_CPNT_ACL_COUNTER == TRUE)
    RULE_TYPE_COUNTER_ENABLE_T  counter_enable;
#endif /* SYS_CPNT_ACL_COUNTER */

#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    UI32_T                      time_range_index;
#endif /* SYS_CPNT_TIME_BASED_ACL */

    UI16_T                      ref_count;

#ifdef RULE_CTRL_INSTANCE_DEBUG_ON

    UI8_T                       check_code;
#define _CLASS_INSTANCE_CHECK_CODE      0xAA

#endif /* RULE_CTRL_INSTANCE_DEBUG_ON */

} RULE_TYPE_CLASS_INSTANCE_T, *RULE_TYPE_CLASS_INSTANCE_PTR_T;

/*
 * Typedef: struct RULE_TYPE_RULE_INSTANCE_T
 *
 * Description: The data structure was used to keep the MF ACE or ACE instance.
 *
 * Fields:
 *  super                                   - Base instance.
 *  id                                      - Identity. It can be an ID for MF
 *                                            ACE or ACE ID based on super.type.
 *  dev_rule_id                             - Corresponding hardware identity.
 *  dev_rule_info                           - Corresponding hardware information.
 *  active                                  - Allocate rule on chip.
 *  counter                                 - Circle queue used to keep the hit
 *                                            rule counter.
 *  current_counter_index                   - Top index of the circle queue.
 *
 */

typedef struct
{
    RULE_TYPE_INSTANCE_T        super;

    /* Software Information
     */
    union
    {
        UI16_T                  id;
        UI16_T                  ace_id;
    };

    RULE_TYPE_DEV_RULE_INFO_T   dev_rule_info;

    UI16_T                      meter_id;
    UI16_T                      action_id;

    UI32_T                      mvl_rule_index;

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE) || (SYS_CPNT_ACL_COUNTER == TRUE)
    RULE_TYPE_CounterRec_T      counter[RULE_TYPE_MAX_COUNTER_REC];
    UI8_T                       current_counter_index;
#endif

    BOOL_T                      active;

#ifdef RULE_CTRL_INSTANCE_DEBUG_ON

    UI8_T                       check_code;
#define _RULE_INSTANCE_CHECK_CODE       0xBB

#endif /* RULE_CTRL_INSTANCE_DEBUG_ON */

} RULE_TYPE_RULE_INSTANCE_T, *RULE_TYPE_RULE_INSTANCE_PTR_T;

/*
 * Typedef: struct RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T
 *
 * Description: Iterator element for RULE_TYPE_CONTAINER_OBJECT_T object.
 *
 * Fields:
 *  ptr                                     - Ponters to the source instance
 *                                            object.
 *  inst                                    - Instance pointer of the source
 *                                            instance object.
 *  curr_ptr                                - Current access instance's pointer.
 *  curr_inst                               - Current access instance object.
 *
 */

typedef struct RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T
{
    /* Source object
     */
    RULE_TYPE_SHM_POINTER_T     ptr;
    RULE_TYPE_CONTAINER_T       *inst;

    /* Current position
     */
    UI32_T                      curr_idx;
    RULE_TYPE_SHM_POINTER_T     curr_ptr;
    RULE_TYPE_INSTANCE_T        *curr_inst;

    RULE_TYPE_INSTANCE_T *
    (*first)
    (
        struct RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this
    );

    RULE_TYPE_INSTANCE_T *
    (*last)
    (
        struct RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this
    );

    RULE_TYPE_INSTANCE_T *
    (*next)
    (
        struct RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this
    );

    RULE_TYPE_INSTANCE_T *
    (*prev)
    (
        struct RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this
    );

    BOOL_T
    (*eof)
    (
        struct  RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this
    );

    BOOL_T
    (*has_next)
    (
        struct  RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this
    );

    BOOL_T
    (*has_prev)
    (
        struct  RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this
    );

    UI32_T
    (*count)
    (
        struct RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this
    );

    RULE_TYPE_SHM_POINTER_T
    (*get_ptr)
    (
        struct RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this
    );

    RULE_TYPE_INSTANCE_T *
    (*get_instance)
    (
        struct RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *this
    );

} RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T;

/*
 * Typedef: struct RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T
 *
 * Description: Iterator element for RULE_TYPE_CLASS_INSTANCE_OBJECT_T object.
 *
 * Fields:
 *  ptr                                     - Ponters to the source instance
 *                                            object.
 *  inst                                    - Instance pointer of the source
 *                                            instance object.
 *  curr_ptr                                - Current access instance's pointer.
 *  curr_inst                               - Current access instance object.
 *
 */

typedef struct RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T
{
    /* Source object
     */
    RULE_TYPE_SHM_POINTER_T     ptr;
    RULE_TYPE_CLASS_INSTANCE_T  *inst;

    /* Current position
     */
    RULE_TYPE_SHM_POINTER_T     curr_ptr;
    RULE_TYPE_INSTANCE_T        *curr_inst;

    RULE_TYPE_INSTANCE_T *
    (*first)
    (
        struct RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this
    );

    RULE_TYPE_INSTANCE_T *
    (*last)
    (
        struct RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this
    );

    RULE_TYPE_INSTANCE_T *
    (*next)
    (
        struct RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this
    );

    RULE_TYPE_INSTANCE_T *
    (*prev)
    (
        struct RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this
    );

    BOOL_T
    (*eof)
    (
        struct  RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this
    );

    BOOL_T
    (*has_next)
    (
        struct  RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this
    );

    BOOL_T
    (*has_prev)
    (
        struct  RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this
    );

    UI32_T
    (*count)
    (
        struct RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this
    );

    RULE_TYPE_SHM_POINTER_T
    (*get_ptr)
    (
        struct RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this
    );

    RULE_TYPE_INSTANCE_T *
    (*get_instance)
    (
        struct RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *this
    );

} RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T;

/*
 * Typedef: struct RULE_TYPE_RULE_INSTANCE_OBJECT_T
 *
 * Description: Rule instance object.
 *
 * Fields:
 *  ptr                                     - Ponters to the instance object.
 *  inst                                    - Instance pointer.
 *
 */

typedef struct RULE_TYPE_RULE_INSTANCE_OBJECT_T
{
    RULE_TYPE_SHM_POINTER_T     ptr;
    RULE_TYPE_RULE_INSTANCE_T   *inst;
} RULE_TYPE_RULE_INSTANCE_OBJECT_T;

/*
 * Typedef: struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T
 *
 * Description: class instance object.
 *
 * Fields:
 *  ptr                                     - Ponters to the instance object.
 *  inst                                    - Instance pointer.
 *
 */

typedef struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T
{
    RULE_TYPE_SHM_POINTER_T     ptr;
    RULE_TYPE_CLASS_INSTANCE_T  *inst;

    RULE_TYPE_RETURN_TYPE_T
    (*add)
    (
        struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
        RULE_TYPE_INSTANCE_T *in
    );

    RULE_TYPE_RETURN_TYPE_T
    (*remove)
    (
        struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
        RULE_TYPE_INSTANCE_T *in
    );

    RULE_TYPE_RETURN_TYPE_T
    (*add_class_obj)
    (
        struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
        struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T *new_obj
    );

    RULE_TYPE_RETURN_TYPE_T
    (*add_rule_obj)
    (
        struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
        struct RULE_TYPE_RULE_INSTANCE_OBJECT_T *new_obj
    );

    RULE_TYPE_RETURN_TYPE_T
    (*remove_class_obj)
    (
        struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
        struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T *obj
    );

    RULE_TYPE_RETURN_TYPE_T
    (*remove_rule_obj)
    (
        struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
        struct RULE_TYPE_RULE_INSTANCE_OBJECT_T *obj
    );

    void
    (*get_element_iterator)
    (
        struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T *this,
        struct RULE_TYPE_CLASS_INSTANCE_ELEMENT_ITERATOR_T *iter
    );

} RULE_TYPE_CLASS_INSTANCE_OBJECT_T;

/*
 * Typedef: struct RULE_TYPE_CONTAINER_OBJECT_T
 *
 * Description: container object.
 *
 * Fields:
 *  ptr                                     - Ponters to the instance object.
 *  inst                                    - Instance pointer.
 *
 */

typedef struct RULE_TYPE_CONTAINER_OBJECT_T
{
    RULE_TYPE_SHM_POINTER_T     ptr;
    RULE_TYPE_CONTAINER_T       *inst;

    RULE_TYPE_RETURN_TYPE_T
    (*add)
    (
        struct RULE_TYPE_CONTAINER_OBJECT_T *this,
        RULE_TYPE_INSTANCE_T *in
    );

    RULE_TYPE_RETURN_TYPE_T
    (*remove)
    (
        struct RULE_TYPE_CONTAINER_OBJECT_T *this,
        RULE_TYPE_INSTANCE_T *in
    );

    RULE_TYPE_RETURN_TYPE_T
    (*add_class_obj)
    (
        struct RULE_TYPE_CONTAINER_OBJECT_T *this,
        struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T *new_obj
    );

    RULE_TYPE_RETURN_TYPE_T
    (*add_rule_obj)
    (
        struct RULE_TYPE_CONTAINER_OBJECT_T *this,
        struct RULE_TYPE_RULE_INSTANCE_OBJECT_T *new_obj
    );

    RULE_TYPE_RETURN_TYPE_T
    (*remove_class_obj)
    (
        struct RULE_TYPE_CONTAINER_OBJECT_T *this,
        struct RULE_TYPE_CLASS_INSTANCE_OBJECT_T *obj
    );

    RULE_TYPE_RETURN_TYPE_T
    (*remove_rule_obj)
    (
        struct RULE_TYPE_CONTAINER_OBJECT_T *this,
        struct RULE_TYPE_RULE_INSTANCE_OBJECT_T *obj
    );

    void
    (*get_element_iterator)
    (
        struct RULE_TYPE_CONTAINER_OBJECT_T *this,
        struct RULE_TYPE_CONTAINER_ELEMENT_ITERATOR_T *iter
    );

} RULE_TYPE_CONTAINER_OBJECT_T;

typedef struct
{
    UI16_T                          class_id;       /*acl id or mf ace id*/
    RULE_TYPE_ClassType_T           class_type;
} RULE_TYPE_ClassMapElement_T;

typedef struct
{
    RULE_TYPE_MIRROR_ENABLE_T       enable;
    UI32_T                          dest_ifindex;
} RULE_TYPE_ACL_MIRROR_T;

typedef struct
{
    RULE_TYPE_INSTANCE_T            super;          /* ACE list */

    UI32_T                          class_map_count;
    UI32_T                          ace_count;
    UI32_T                          row_status;

    /* one chip one instance */
    RULE_TYPE_CLASS_INSTANCE_PTR_T  filtering_acl_instance[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK][SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT];

    UI16_T                          filtering_acl_instance_ref_count[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK][SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT];

    RULE_TYPE_AclType_T             acl_type;
    char                            acl_name[SYS_ADPT_ACL_MAX_NAME_LEN + 1];
    UI8_T                           filtering_in_port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
    UI8_T                           filtering_out_port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];
    UI8_T                           class_map_bit_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_CLASS_MAP_LIST];

#if (SYS_CPNT_ACL_MIRROR == TRUE)
    RULE_TYPE_ACL_MIRROR_T          mirror;
#endif /* #if (SYS_CPNT_ACL_MIRROR == TRUE) */

#if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE)
    RULE_TYPE_ClassMapElement_T     owner;
    UI32_T                          aggregate_acl_index;
    BOOL_T                          flag_aggregate_done;
    BOOL_T                          flag_hide;
#endif /* #if (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE) || (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE) */

} RULE_TYPE_Acl_T;

/* ClassMap template
 * Note: if class_type is ACL then class_element_index is acl_index;
 *       if class_type is MF then class_element_index is mf_index
 */
typedef struct
{
    UI32_T                          class_index_list[SYS_ADPT_DIFFSERV_MAX_CLASS_NBR_OF_CLASS_MAP];
    UI32_T                          class_element_count;
    UI32_T                          row_status;
    RULE_TYPE_MatchType_T           class_map_match_type;
    RULE_TYPE_ClassType_T           class_type[SYS_ADPT_DIFFSERV_MAX_CLASS_NBR_OF_CLASS_MAP]; /* ACL or MF */ /*index is the same class index list*/
    char                            class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1]; /* class map name */
    char                            desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];

#if (SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL == TRUE) || (SYS_CPNT_ACL_AUTO_COMPRESS_ACE == TRUE)
    /* The following data member are used for match-all class-map type
     */
    struct
    {
        UI32_T                      aggregate_index;

        /* Valid flag for aggregate_index.
         * This value is TRUE means aggregate_acl_index or aggregate_ace_index
         * is valid.
         */
        BOOL_T                      flag_aggregate_index_valid;

    } class_ext;
#endif /* SYS_CPNT_QOS_V2_CLASS_MAP_MATCH_ALL || SYS_CPNT_ACL_AUTO_COMPRESS_ACE */

} RULE_TYPE_ClassMap_T;

typedef struct
{
    UI32_T action_bitmap;

    UI32_T pkt_new_pri;             /*RULE_TYPE_ACTION_PKT_NEW_PRI*/
    UI32_T pkt_new_phb;             /*RULE_TYPE_ACTION_PKT_NEW_PHB*/
    UI32_T pkt_new_tos;             /*RULE_TYPE_ACTION_PKT_NEW_TOS*/
    UI32_T pkt_new_dscp;            /*RULE_TYPE_ACTION_PKT_NEW_DSCP*/
    UI32_T green_pkt_new_dscp;      /*RULE_TYPE_ACTION_GREEN_PKT_NEW_DSCP*/
    UI32_T yellow_pkt_new_dscp;     /*RULE_TYPE_ACTION_YELLOW_PKT_NEW_DSCP*/
    UI32_T red_pkt_new_dscp;        /*RULE_TYPE_ACTION_RED_PKT_NEW_DSCP*/
    UI32_T drop_precedence;         /*RULE_TYPE_ACTION_DROP_PRECEDENCE*/
    UI32_T yellow_drop_precedence;  /*RULE_TYPE_ACTION_YELLOW_DROP_PRECEDENCE*/
    UI32_T red_drop_precedence;     /*RULE_TYPE_ACTION_RED_DROP_PRECEDENCE*/
    UI32_T pkt_pri;                 /*RULE_TYPE_ACTION_PKT_PRI*/

    UI32_T row_status;
} RULE_TYPE_Action_T;

typedef struct
{
    /* key is the index of this table */
    UI32_T  meter_model;            /* meter model */
    UI32_T  rate;                   /* average commit rate in kbps */
    UI32_T  burst_size;             /* maxi. # of commit bytes in a single Tx burst */
    UI32_T  peak_rate;              /* average peak rate in kbps */
    UI32_T  peak_burst_size;        /* maxi. # of peak bytes in a single Tx burst */
    UI32_T  interval;               /* microseconds, time interval used with the token bucket */
    UI32_T  row_status;
    UI16_T  ref_count;              /* how many entries are using it */
} RULE_TYPE_TBParamEntry_T;

typedef struct
{
    UI32_T  class_map_index;
    UI32_T  meter_index;
    UI32_T  action_index;
    UI32_T  row_status;
} RULE_TYPE_PolicyElement_T;

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
typedef struct
{
    UI32_T                          input_rate_kbps;

    /* New max bandwidth. Compute by input rate per flow.
     */
    UI32_T                          bandwidth_kbps;
} RULE_TYPE_Flow_T;

typedef struct
{
    UI32_T                          priority;
    UI32_T                          class_map_index;

    /* Max bandwidth. CIR of class-map
     */
    UI32_T                          bandwidth_kbps;
    UI32_T                          burst_size_byte;

    RULE_TYPE_Flow_T                flow;
} RULE_TYPE_Class_T;

typedef struct
{
    UI32_T                          priority;
    UI32_T                          total_input_rate_kpbs;

    UI16_T                          class_count;
    RULE_TYPE_Class_T               class_list[ SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_OF_BUNDLE_GROUP ];
} RULE_TYPE_ChildBundleGroup_T;

typedef struct
{
    UI32_T                          bundle_rate_kbps;
    UI16_T                          class_map_count;
    UI16_T                          class_map_index_list[SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_OF_BUNDLE_GROUP];

    UI16_T                          child_bundle_count;
    RULE_TYPE_ChildBundleGroup_T    child_bundle_list[SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_OF_BUNDLE_GROUP];

    BOOL_T                          valid_status;
} RULE_TYPE_BundleGroup_T;

typedef struct
{
    UI32_T                          index;
    UI32_T                          bundle_rate_kbps;
    UI16_T                          class_map_count;
    char                            class_map_name_list[SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_OF_BUNDLE_GROUP]
                                                       [SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
    BOOL_T                          valid_status;
} RULE_TYPE_UI_BundleGroup_T;
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

/* NOTE: Use this descriptor to describe the template of QoS Profile data.
  */
typedef struct
{
    UI32_T                          mf_value;
    RULE_TYPE_MfType_T              mf_type;
} RULE_TYPE_MfEntry_T;

typedef struct
{
    UI32_T                          row_status;

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    RULE_TYPE_BundleGroup_T         bundle_group[SYS_ADPT_DIFFSERV_MAX_BUNDLE_GROUP_OF_POLICY_MAP];
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

    UI16_T                          policy_map_element_index_list[SYS_ADPT_DIFFSERV_MAX_CLASS_MAP_NBR_OF_POLICY_MAP];
    UI16_T                          element_count;
    UI16_T                          port_count;         /* reference count for ingress and egress port */
    char                            name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
    char                            desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];
    UI8_T                           qos_port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];           /* ingress */
    UI8_T                           egress_qos_port_list[RULE_TYPE_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST];    /* egress  */
} RULE_TYPE_PolicyMap_T;

typedef struct
{
    UI32_T                          acl_index;
    UI32_T                          precedence;
#if (SYS_CPNT_TIME_BASED_ACL == TRUE)
    UI32_T                          time_range_index;
    UI8_T                           time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];
#endif
    RULE_TYPE_COUNTER_ENABLE_T      counter_enable;
    UI32_T                          row_status;
} RULE_TYPE_AclMemento_T, *RULE_TYPE_ACL_MEMENTO_PTR_T;

typedef struct
{
    RULE_TYPE_AclMemento_T          bind_acl[RULE_TYPE_MAX_NBR_OF_ACL_TYPE];

    RULE_TYPE_CLASS_INSTANCE_PTR_T  policy_map_instance_ptr;
    UI16_T                          policy_map_index;            /* id of profile template */
    UI32_T                          policy_map_row_status;

#if (SYS_CPNT_NETACCESS_DYNAMIC_QOS == TRUE)
    UI16_T                          backup_policy_map_index;      /* id of backup profile template */
    UI32_T                          backup_policy_map_row_status;
    BOOL_T                          bind_dynamic_policy_map;

    /* user configured ACL index, this value
     * is valiable when bind a dynamic ACL to
     * this port
     */
    RULE_TYPE_AclMemento_T          acl_memento[RULE_TYPE_MAX_NBR_OF_ACL_TYPE];

    BOOL_T                          bind_dynamic_acl;             /* whether bind a dynamic ACL to this port */
#endif /* SYS_CPNT_NETACCESS_DYNAMIC_QOS */

    /* user configured ACL index,
     * not the aggregate ACL index
     */
    UI32_T                          user_config_acl_index[RULE_TYPE_PRECEDENCE_LAST+1];

} RULE_TYPE_PortEntry_T;

typedef enum
{
    OUT_ACTION_DROP,
    OUT_ACTION_POLICED_DSCP_TRANSMIT,
    OUT_ACTION_INVALID
} RULE_TYPE_UI_OutActionType_T; /* for CLI/Web */

typedef struct
{
    UI32_T                              in_action_value;
    UI32_T                              confirm_action_value;
    UI32_T                              exceed_action_value;
    UI32_T                              violate_action_value;
    UI32_T                              out_action_value;
    UI32_T                              class_action_value;

    RULE_TYPE_UI_InActionType_T         in_action_type;
    RULE_TYPE_UI_OutActionType_T        out_action_type;
    RULE_TYPE_UI_ConfirmActionType_T    confirm_action_type;
    RULE_TYPE_UI_ExceedActionType_T     exceed_action_type;
    RULE_TYPE_UI_ViolateActionType_T    violate_action_type;
    RULE_TYPE_UI_ClassActionType_T      class_action_type;
} RULE_TYPE_UI_Action_T; /* for CLI/Web */

typedef struct
{
    UI32_T                              row_status;
    RULE_TYPE_AclType_T                 acl_type;
    char                                acl_name[SYS_ADPT_ACL_MAX_NAME_LEN + 1];
} RULE_TYPE_UI_AclEntry_T; /* for CLI/Web */

typedef struct
{
    RULE_TYPE_TBParamEntry_T            meter_entry;
    RULE_TYPE_UI_Action_T               action_entry;
    UI32_T                              precedence;
    UI32_T                              row_status;
    char                                class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
} RULE_TYPE_UI_PolicyElement_T; /* for CLI/Web */

typedef struct
{
    union
    {
        char                            acl_name[SYS_ADPT_ACL_MAX_NAME_LEN + 1];
        RULE_TYPE_MfEntry_T             mf_entry;
    } element;
    RULE_TYPE_ClassType_T               class_type;
} RULE_TYPE_UI_ClassMapElement_T; /* for CLI/Web */

typedef struct
{
    UI32_T                              row_status;
    RULE_TYPE_MatchType_T               class_map_match_type;
    char                                class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
    char                                desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];
} RULE_TYPE_UI_ClassMap_T; /* for CLI/Web */

typedef struct
{
    UI32_T                              row_status;
    char                                name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
    char                                desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    RULE_TYPE_UI_BundleGroup_T          bundle_group[SYS_ADPT_DIFFSERV_MAX_BUNDLE_GROUP_OF_POLICY_MAP];
#endif /* SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL */

} RULE_TYPE_UI_PolicyMap_T; /* for CLI/Web */

typedef struct
{
    UI16_T                              ingress_mac_acl_index;
    UI16_T                              ingress_ip_acl_index;
    UI16_T                              ingress_ipv6_acl_index;
    UI16_T                              policy_map_index;

    /* When bind_dynamic_acl is TRUE or bind_dynamic_policy_map is TRUE,
     * the real index may is different with the user configured.
     */
    UI16_T                              user_cfg_ingress_mac_acl_index;
    UI16_T                              user_cfg_ingress_ip_acl_index;
    UI16_T                              user_cfg_ingress_ipv6_acl_index;
    UI16_T                              user_cfg_policy_map_index;

    BOOL_T                              bind_dynamic_acl;
    BOOL_T                              bind_dynamic_policy_map;
} RULE_TYPE_UI_PortEntry_T; /* for CLI/Web */

typedef struct RULE_TYPE_MIB_PolicyMap_S
{
    UI32_T      row_status;

                /* number of bytes in the SNMP variable
                 * policy_map_element_index_list
                 */
    UI16_T      size_of_element_index_list;

                /* two bytes in **network order** for one element
                 */
    UI8_T       policy_map_element_index_list
                    [MAXSIZE_diffServPolicyMapElementIndexList];

    char        name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
    char        desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];
} RULE_TYPE_MIB_PolicyMap_T;

typedef struct
{
    UI32_T      size_of_list; /* implies how many data available in class_type & class_index_list */
    UI32_T      row_status;

    char        class_map_name[SYS_ADPT_DIFFSERV_MAX_NAME_LENGTH + 1];
    char        desc[SYS_ADPT_DIFFSERV_MAX_DESCRIPTION_LENGTH + 1];

    /* according to Accton private MIB definition
     * match type could be:
     *      VAL_diffServClassMapMatchType_matchAny  1L
     *      VAL_diffServClassMapMatchType_matchAll  2L
     */
    UI8_T       class_map_match_type;

    /* according to Accton private MIB definition (two bytes one element)
     * class type could be:
     *      VAL_diffServClassMapAttachCtlElementIndexType_macAce    1L
     *      VAL_diffServClassMapAttachCtlElementIndexType_ipAce     2L
     *      VAL_diffServClassMapAttachCtlElementIndexType_acl       3L
     *      VAL_diffServClassMapAttachCtlElementIndexType_ipv6Ace   4L
     */
    UI8_T       class_type[MAXSIZE_diffServClassMapElementIndexTypeList];
    UI8_T       class_index_list[MAXSIZE_diffServClassMapElementIndexList]; /* NBO, two bytes one element */
} RULE_TYPE_MIB_ClassMap_T;


typedef struct
{
    UI32_T      size_of_ace_index_list; /* implies how many data available in ace_index_list */
    UI32_T      row_status;

    /* according to Accton private MIB definition
     * acl type could be:
     *      VAL_diffServAclType_mac
     *      VAL_diffServAclType_ipstandard
     *      VAL_diffServAclType_ipextended
     *      VAL_diffServAclType_ipv6standard
     *      VAL_diffServAclType_ipv6extended
     */
    UI8_T       acl_type;
    char        acl_name[SYS_ADPT_ACL_MAX_NAME_LEN + 1];
    UI8_T       ace_index_list[MAXSIZE_diffServAclAceIndexList]; /* NBO, two bytes one element */
} RULE_TYPE_MIB_Acl_T;

typedef struct
{
    UI8_T   action_bitmap[SIZE_diffServActionList];

    /* pkt_new_pri, pkt_new_tos, pkt_new_dscp are conflict */
    UI8_T   pkt_new_pri;
    UI8_T   pkt_new_ip_prec;
    UI8_T   pkt_phb;
    UI8_T   pkt_new_dscp;
    UI8_T   green_pkt_new_dscp;
    UI8_T   green_pkt_transmit;
    UI8_T   yellow_pkt_new_dscp;
    UI8_T   drop_precedence;
    UI8_T   red_drop_precedence;
    UI8_T   yellow_drop_precedence;
    UI8_T   yellow_drop;           /* (1) enable, (2) disable */
    UI8_T   red_pkt_new_dscp;
    UI8_T   red_drop;           /* (1) enable, (2) disable */

#if (SYS_CPNT_QOS_V2_SOFTWARE_BUNDLE_RATE_CONTROL == TRUE)
    UI16_T  pkt_pri;
#endif

    UI8_T   row_status;
} RULE_TYPE_MIB_Action_T;

typedef struct
{
    UI32_T ifindex;
    UI32_T direction;
    UI32_T type;
    UI32_T acl_index;
    char   time_range_name[SYS_ADPT_TIME_RANGE_MAX_NAME_LENGTH+1];
    UI32_T counter_status;
    UI32_T row_status;
} RULE_TYPE_MIB_AccessGroupEntry_T;

typedef struct
{
    UI32_T ifindex;
    UI32_T direction;
    UI32_T policy_map_index;
    UI32_T row_status;
} RULE_TYPE_MIB_PolicyMapPortEntry_T;

typedef struct
{
    UI8_T bits[RULE_TYEP_TCAM_CAP_MAX_BITMAP_SIZE];
} RULE_TYPE_TCAM_CAP_BITMAP_T;

typedef struct
{
    UI32_T unit;
    UI32_T device_id;
    UI32_T pool_id;

    UI32_T total;
    UI32_T used;
    UI32_T free;
    RULE_TYPE_TCAM_CAP_BITMAP_T cap_bitmap;
} RULE_TYPE_TCAM_ENTRY_T;

typedef struct
{
    RULE_TYPE_TCAM_Capability_T capability;
    char name[RULE_TYPE_TCAM_CAP_MAX_STR_LEN + 1];
    char description[RULE_TYPE_TCAM_CAP_MAX_DESCRIPTION_LEN + 1];
} RULE_TYPE_TCAM_CAP_STRING_T;

typedef struct
{
    UI32_T pce_total;
    UI32_T pce_used_by_system;  /* system reserved */
    UI32_T pce_used_by_user;    /* current used by ACL and DiffServ */
    UI32_T pce_used;            /* used_by_system + used_by_user */
    UI32_T pce_free;            /* free for ACL and DiffServ */
    float  pce_utility_rate;
} RULE_TYPE_TCAM_DEVICE_UTIL_T;

typedef struct
{
    UI32_T pce_total;
    UI32_T pce_used_by_system;  /* system reserved */
    UI32_T pce_used_by_user;    /* current used by ACL and DiffServ */
    UI32_T pce_used;            /* used_by_system + used_by_user */
    UI32_T pce_free;            /* free for ACL and DiffServ */
    float  pce_utility_rate;

    UI32_T num_of_device;

    RULE_TYPE_TCAM_DEVICE_UTIL_T device[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT]; /* index 0 means device 0 */

} RULE_TYPE_TCAM_UNIT_UTIL_T;

typedef struct
{
    UI32_T pce_total;
    UI32_T pce_used_by_system;  /* system reserved */
    UI32_T pce_used_by_user;    /* current used by ACL and DiffServ */
    UI32_T pce_used;            /* used_by_system + used_by_user */
    UI32_T pce_free;            /* free for ACL and DiffServ */
    float  pce_utility_rate;

    UI32_T num_of_unit;

    RULE_TYPE_TCAM_UNIT_UTIL_T unit[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];    /* index 0 means unit 1 */

} RULE_TYPE_TCAMUtilization_T;

/* MARCO FUNCTION DECLARATIONS
 */
// TODO: Move to rule_log.x ?
#ifndef RULE_TYPE_NEWLINE
#define RULE_TYPE_NEWLINE                                           "\r\n"
#endif

#ifndef RULE_TYPE_LINE_LEADER
#define RULE_TYPE_LINE_LEADER                                       "  "
#endif

#define RULE_TYPE_IS_DEBUG_ERROR_ON(flag)                           (flag)

#define RULE_TYPE_PRINT_HEADER()                                    \
    {                                                               \
        BACKDOOR_MGR_Printf("[%s:%d]" RULE_TYPE_NEWLINE,            \
            __FUNCTION__, __LINE__);                                \
    }

#define RULE_TYPE_PRINT(fmt, ...)                                   \
    {                                                               \
        BACKDOOR_MGR_Printf(fmt, ##__VA_ARGS__);                    \
    }

#define RULE_TYPE_LOG(flag, fmt,...)                                \
    {                                                               \
        if (RULE_TYPE_IS_DEBUG_ERROR_ON((flag)))                    \
        {                                                           \
            RULE_TYPE_PRINT_HEADER();                               \
            RULE_TYPE_PRINT(RULE_TYPE_LINE_LEADER                   \
                            fmt RULE_TYPE_NEWLINE, ##__VA_ARGS__);  \
        }                                                           \
    }

#if (SYS_CPNT_PBR == TRUE)
typedef enum
{
    RULE_TYPE_PBR_PACKET_CMD_PERMIT = 0,
    RULE_TYPE_PBR_PACKET_CMD_DENY
} RULE_TYPE_PBR_PACKET_CMD_T;

typedef enum
{
    RULE_TYPE_PBR_REDIRECT_ACTION_NONE  = 0,
    RULE_TYPE_PBR_REDIRECT_ACTION_ROUNT
} RULE_TYPE_PBR_REDIRECT_ACTION_T;

typedef struct
{
    RULE_TYPE_PBR_REDIRECT_ACTION_T action;
    struct
    {
        UI32_T row;
        UI32_T column;
    } ltt_index;
} RULE_TYPE_PBR_REDIRECT_T;

typedef struct
{
    struct
    {
        BOOL_T is_modified;
        UI32_T value;
    } dscp;

} RULE_TYPE_PBR_QOS_T;

typedef struct
{
    RULE_TYPE_PBR_PACKET_CMD_T cmd;
    RULE_TYPE_PBR_REDIRECT_T   redirect;
    RULE_TYPE_PBR_QOS_T        qos;
} RULE_TYPE_PBR_ACTION_T;
#endif /* SYS_CPNT_PBR */

#endif   /* End of _RULE_TYPE_H */

