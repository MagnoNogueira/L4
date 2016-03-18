#ifndef _FIELD_H_
#define _FIELD_H_

#include <bcm/types.h>
#include <shared/bitop.h>
#include <error.h>
#include <bcm/port.h>
#include "sys_type.h"

/* Entry priority. */
/* 2008-12-11, Jinfeng Chen: Extend to 8 priority */
#if 1
#define BCM_FIELD_ENTRY_PRIO_1          (-1)
#define BCM_FIELD_ENTRY_PRIO_2          (-2)
#define BCM_FIELD_ENTRY_PRIO_3          (-3)
#define BCM_FIELD_ENTRY_PRIO_4          (-4)
#define BCM_FIELD_ENTRY_PRIO_5          (-5)
#define BCM_FIELD_ENTRY_PRIO_6          (-6)
#define BCM_FIELD_ENTRY_PRIO_7          (-7)
#define BCM_FIELD_ENTRY_PRIO_8          (-8)

#define BCM_FIELD_ENTRY_PRIO_HIGHEST    BCM_FIELD_ENTRY_PRIO_1
#define BCM_FIELD_ENTRY_PRIO_DONT_CARE  BCM_FIELD_ENTRY_PRIO_4
#define BCM_FIELD_ENTRY_PRIO_LOWEST     BCM_FIELD_ENTRY_PRIO_8

#else
#define BCM_FIELD_ENTRY_PRIO_HIGHEST    (-1)       /* Highest in group. */
#define BCM_FIELD_ENTRY_PRIO_DONT_CARE  (-2)       /* Just above lowest. */
#define BCM_FIELD_ENTRY_PRIO_LOWEST     (-3)       /* Lowest in group. */
#endif

#define BCM_FIELD_ENTRY_PRIO_DEFAULT    BCM_FIELD_ENTRY_PRIO_DONT_CARE /* Default priority assigned
                                                      to new entries */

/* User-Defined Field (UDF) L2 Flags. */
#define BCM_FIELD_USER_L2_MASK      0x00000003 
#define BCM_FIELD_USER_L2_ETHERNET2 (0 << 0)   
#define BCM_FIELD_USER_L2_SNAP      (1 << 0)   
#define BCM_FIELD_USER_L2_LLC       (2 << 0)   
#define BCM_FIELD_USER_L2_OTHER     (3 << 0)   

/* Packet content (data) qualification object flags. */
#define BCM_FIELD_DATA_QUALIFIER_WITH_ID    (1 << 0)   
#define BCM_FIELD_DATA_QUALIFIER_REPLACE    (1 << 1)   
#define BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST (1 << 2)   
#define BCM_FIELD_DATA_QUALIFIER_OFFSET_IP6_EXTENSIONS_ADJUST (1 << 3)   
#define BCM_FIELD_DATA_QUALIFIER_OFFSET_GRE_OPTIONS_ADJUST (1 << 4)   

/* Packet content (data) qualification vlan tag format. */
#define BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY  (0xffff)   
#define BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG   (1 << 0)   
#define BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED (1 << 1)   
#define BCM_FIELD_DATA_FORMAT_VLAN_DOUBLE_TAGGED (1 << 2)   

/* Packet content (data) qualification L2 header format. */
#define BCM_FIELD_DATA_FORMAT_L2_ANY    (0xffff)   
#define BCM_FIELD_DATA_FORMAT_L2_ETH_II (1 << 0)   
#define BCM_FIELD_DATA_FORMAT_L2_SNAP   (1 << 1)   
#define BCM_FIELD_DATA_FORMAT_L2_LLC    (1 << 2)   

/* Packet content (data) qualification L3 header version. */
#define BCM_FIELD_DATA_FORMAT_IP_ANY    (0xffff)   
#define BCM_FIELD_DATA_FORMAT_IP_NONE   (1 << 0)   
#define BCM_FIELD_DATA_FORMAT_IP4       (1 << 1)   
#define BCM_FIELD_DATA_FORMAT_IP6       (1 << 2)   

/* Packet content (data) qualification Mpls encapsulation. */
#define BCM_FIELD_DATA_FORMAT_MPLS_ANY      (0xffff)   
#define BCM_FIELD_DATA_FORMAT_MPLS_ONE_LABEL (1 << 0)   
#define BCM_FIELD_DATA_FORMAT_MPLS_TWO_LABELS (1 << 1)   

/* Packet content (data) qualification Tunnel. */
#define BCM_FIELD_DATA_FORMAT_TUNNEL_ANY    (0xffff)   
#define BCM_FIELD_DATA_FORMAT_TUNNEL_NONE   (1 << 0)   
#define BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP (1 << 1)   
#define BCM_FIELD_DATA_FORMAT_TUNNEL_GRE    (1 << 2)   
#define BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS   (1 << 3)   

/* bcm_field_data_offset_base_e */
typedef enum bcm_field_data_offset_base_e {
    bcmFieldDataOffsetBasePacketStart,  /* Offset provided from the beginning of
                                           a packet. */
    bcmFieldDataOffsetBaseOuterL3Header, /* Offset provided from the beginning of
                                           outer L3 header. */
    bcmFieldDataOffsetBaseInnerL3Header, /* Offset provided from the beginning of
                                           inner L3 header. */
    bcmFieldDataOffsetBaseOuterL4Header, /* Offset provided from the beginning of
                                           outer L4 header.. */
    bcmFieldDataOffsetBaseInnerL4Header, /* Offset provided from the beginning of
                                           inner L4 header. */
    bcmFieldDataOffsetBaseHigigHeader,  /* Offset provided from the beginning of
                                           Higig header. */
    bcmFieldDataOffsetBaseHigig2Header, /* Offset provided from the beginning of
                                           Higig header. */
    bcmFieldDataOffsetBaseCount         /* Always Last. Not a usable value. */
} bcm_field_data_offset_base_t;

#define BCM_FIELD_DATA_OFFSET_BASE \
{ \
    "bcmFieldDataOffsetBasePacketStart", \
    "bcmFieldDataOffsetBaseOuterL3Header", \
    "bcmFieldDataOffsetBaseInnerL3Header", \
    "bcmFieldDataOffsetBaseOuterL4Header", \
    "bcmFieldDataOffsetBaseInnerL4Header", \
    "bcmFieldDataOffsetBaseHigigHeader", \
    "bcmFieldDataOffsetBaseHigig2Header"  \
}

/* DATA qualifier object, used for creation of custom data qualifiers. */
typedef struct bcm_field_data_qualifier_s {
    int qual_id;                        /* Data qualifier id. */
    uint32 flags;                       /* Data qualifier flags
                                           BCM_FIELD_DATA_QUALIFIER_XXX. */
    bcm_field_data_offset_base_t offset_base; /* Offset base adjustment. */
    uint16 offset;                      /* Packet byte offset. */
    int length;                         /* Matched data byte length. */
} bcm_field_data_qualifier_t;

/* Ethertype based DATA packet qualifier structure. */
typedef struct bcm_field_data_ethertype_s {
    int relative_offset;            /* Packet byte offset relative to qualifier
                                       byte offset. */
    bcm_port_ethertype_t ethertype; /* Ether type. */
    uint16 l2;                      /* Packet l2 format.
                                       (FIELD_DATA_FORMAT_L2_XXX) */
    uint16 vlan_tag;                /* vlan_tag_format.
                                       (FIELD_DATA_FORMAT_VLAN_XXX) */
} bcm_field_data_ethertype_t;

/* Ip protocol/Next header based DATA packet qualifier structure. */
typedef struct bcm_field_data_ip_protocol_s {
    int relative_offset;    /* Packet byte offset relative to qualifier byte
                               offset. */
    uint16 flags;           /* Packet ip header version.
                               (FIELD_DATA_FORMAT_IP_XXX) */
    uint8 ip;               /* Ip protocol id. */
    uint16 l2;              /* Packet l2 format. (FIELD_DATA_FORMAT_L2_XXX) */
    uint16 vlan_tag;        /* vlan_tag_format. (FIELD_DATA_FORMAT_VLAN_XXX) */
} bcm_field_data_ip_protocol_t;

/* Packet format based DATA qualifier specification structure. */
typedef struct bcm_field_data_packet_format_s {
    int relative_offset;    /* Packet byte offset relative to qualifier byte
                               offset. */
    uint16 l2;              /* Packet l2 format. (FIELD_DATA_FORMAT_L2_XXX) */
    uint16 vlan_tag;        /* vlan_tag_format. (FIELD_DATA_FORMAT_VLAN_XXX) */
    uint16 outer_ip;        /* Outer IP header
                               version.(FIELD_DATA_FORMAT_IP_XXX) */
    uint16 inner_ip;        /* Inner IP header
                               version.(FIELD_DATA_FORMAT_IP_XXX) */
    uint16 tunnel;          /* Tunnel.(FIELD_DATA_FORMAT_TUNNEL_XXX) */
    uint16 mpls;            /* Mpls labels.(FIELD_DATA_FORMAT_MPLS_XXX) */
} bcm_field_data_packet_format_t;

/* User-Defined Field (UDF) VLAN Flags. */
#define BCM_FIELD_USER_VLAN_MASK    0x00000018 
#define BCM_FIELD_USER_VLAN_NOTAG   (0 << 3)   
#define BCM_FIELD_USER_VLAN_ONETAG  (1 << 3)   
#define BCM_FIELD_USER_VLAN_TWOTAG  (2 << 3)   
#define BCM_FIELD_USER_VLAN_NOTUSED (3 << 3)   

/* User-Defined Field (UDF) IP Flags. */
#define BCM_FIELD_USER_IP_MASK              0x000003c0 
#define BCM_FIELD_USER_IP4_HDR_ONLY         (0 << 6)   
#define BCM_FIELD_USER_IP6_HDR_ONLY         (1 << 6)   
#define BCM_FIELD_USER_IP6_FRAGMENT         (2 << 6)   
#define BCM_FIELD_USER_IP4_OVER_IP4         (3 << 6)   
#define BCM_FIELD_USER_IP6_OVER_IP4         (4 << 6)   
#define BCM_FIELD_USER_IP4_OVER_IP6         (5 << 6)   
#define BCM_FIELD_USER_IP6_OVER_IP6         (6 << 6)   
#define BCM_FIELD_USER_GRE_IP4_OVER_IP4     (7 << 6)   
#define BCM_FIELD_USER_GRE_IP6_OVER_IP4     (8 << 6)   
#define BCM_FIELD_USER_GRE_IP4_OVER_IP6     (9 << 6)   
#define BCM_FIELD_USER_GRE_IP6_OVER_IP6     (10 << 6)  
#define BCM_FIELD_USER_ONE_MPLS_LABEL       (11 << 6)  
#define BCM_FIELD_USER_TWO_MPLS_LABELS      (12 << 6)  
#define BCM_FIELD_USER_IP6_FRAGMENT_OVER_IP4 (13 << 6)  
#define BCM_FIELD_USER_IP_NOTUSED           (14 << 6)  

/* Flags for Auto Adjust Offset with IP options length. */
#define BCM_FIELD_USER_OPTION_ADJUST    (1 << 12)  

/* Flags for Auto Adjust Offset with GRE header options length. */
#define BCM_FIELD_USER_GRE_OPTION_ADJUST    (1 << 13)  

/* 
 * On devices that support this feature, these flags indicate that the
 * HiGig header is being examined via the UDF mechanism.
 */
#define BCM_FIELD_USER_HIGIG    (1 << 14)  
#define BCM_FIELD_USER_HIGIG2   (1 << 15)  

/* User-Defined Field (UDF) OFFSET BASE Flags. */
#define BCM_FIELD_USER_OFFSET_BASE_MASK     0x000f0000 
#define BCM_FIELD_USER_OFFSET_BASE_START_OF_FRAME 0x00010000 
#define BCM_FIELD_USER_OFFSET_BASE_END_OF_L2HDR 0x00020000 
#define BCM_FIELD_USER_OFFSET_BASE_END_OF_L3HDR 0x00040000 
#define BCM_FIELD_USER_OFFSET_BASE_END_OF_TAG 0x00080000 

/* Packet colors. */
#define BCM_FIELD_COLOR_PRESERVE    0          
#define BCM_FIELD_COLOR_GREEN       1          
#define BCM_FIELD_COLOR_YELLOW      2          
#define BCM_FIELD_COLOR_RED         3          

/* IPv4 Flags from RFC-791 (for bcm_field_qualify_IpFlags). */
#define BCM_FIELD_IPFLAGS_MF    0x1        /* More Fragments. */
#define BCM_FIELD_IPFLAGS_DF    0x2        /* Don't Fragment. */
#define BCM_FIELD_IPFLAGS_RF    0x4        /* Reserved. */
#define BCM_FIELD_IPFLAGS_MASK  0x7        /* All bits. */

/* TCP Control Flags from RFC-793 (for bcm_field_qualify_TcpControl). */
#define BCM_FIELD_TCPCONTROL_FIN    0x01       /* No more data. */
#define BCM_FIELD_TCPCONTROL_SYN    0x02       /* Sync seq numbers. */
#define BCM_FIELD_TCPCONTROL_RST    0x04       /* Reset connection. */
#define BCM_FIELD_TCPCONTROL_PSH    0x08       /* Push. */
#define BCM_FIELD_TCPCONTROL_ACK    0x10       /* Ack field valid. */
#define BCM_FIELD_TCPCONTROL_URG    0x20       /* Urgent field valid. */
#define BCM_FIELD_TCPCONTROL_R40    0x40       /* Reserved. */
#define BCM_FIELD_TCPCONTROL_R80    0x80       /* Reserved. */
#define BCM_FIELD_TCPCONTROL_MASK   0x3f       /* All non-reserved bits. */

/* Packet Lookup Status (for bcm_field_qualify_LookupStatus). */
#define BCM_FIELD_LOOKUP_L3_TUN_HIT     0x0001     /* L3 tunnel table hit. */
#define BCM_FIELD_LOOKUP_VXLT_HIT       0x0002     /* VLAN translation hit. */
#define BCM_FIELD_LOOKUP_VLAN_VALID     0x0004     /* VLAN ID valid. */
#define BCM_FIELD_LOOKUP_ING_STP_MASK   0x0018     /* Ingress spanning tree
                                                      state. */
#define BCM_FIELD_LOOKUP_ING_STP_DIS    0x0000     /* STP state Disabled. */
#define BCM_FIELD_LOOKUP_ING_STP_BLK    0x0008     /* STP state Blocking. */
#define BCM_FIELD_LOOKUP_ING_STP_LRN    0x0010     /* STP state Learning. */
#define BCM_FIELD_LOOKUP_ING_STP_FWD    0x0018     /* STP state Forwarding. */
#define BCM_FIELD_LOOKUP_L2_SA_HIT      0x0020     /* L2 src lookup hit. */
#define BCM_FIELD_LOOKUP_L2_SA_STATIC   0x0040     /* L2 src static bit. */
#define BCM_FIELD_LOOKUP_L2_DA_HIT      0x0080     /* L2 dst lookup hit. */
#define BCM_FIELD_LOOKUP_L3_ROUTABLE    0x0100     /* L2 dst L3 bit. */
#define BCM_FIELD_LOOKUP_L2_CACHE_HIT   0x0200     /* L2 cache lookup hit. */
#define BCM_FIELD_LOOKUP_L3UC_SA_HIT    0x0400     /* L3 unicast src lookup hit. */
#define BCM_FIELD_LOOKUP_L3UC_DA_HIT    0x0800     /* L3 unicast dst lookup hit. */
#define BCM_FIELD_LOOKUP_L3MC_SGV_HIT   0x1000     /* L3 multicast table hit. */
#define BCM_FIELD_LOOKUP_L3_LPM_HIT     0x2000     /* Longest pref. table hit. */
#define BCM_FIELD_LOOKUP_L2_MISS        0x4000     /* L2 src miss/station movmt. */
#define BCM_FIELD_LOOKUP_DOS_ATTACK     0x8000     /* DoS attack drop candidate. */

/* IP Info (for bcm_field_qualify_IpInfo). */
#define BCM_FIELD_IP_HDR_OFFSET_ZERO    0x0001     
#define BCM_FIELD_IP_HDR_FLAGS_MF       0x0002     
#define BCM_FIELD_IP_CHECKSUM_OK        0x0004     

/* Packet Format Values (for bcm_field_qualify_PacketFormat). */
#define BCM_FIELD_PKT_FMT_L2_ETH_II     0x0000     /* Ethernet II frame format. */
#define BCM_FIELD_PKT_FMT_L2_SNAP       0x0001     /* SNAP frame format. */
#define BCM_FIELD_PKT_FMT_L2_LLC        0x0002     /* LLC frame format. */
#define BCM_FIELD_PKT_FMT_L2_802_3      0x0003     /* IEEE 802.3 frame format. */
#define BCM_FIELD_PKT_FMT_L2_MASK       \
    (BCM_FIELD_PKT_FMT_L2_ETH_II | \
     BCM_FIELD_PKT_FMT_L2_SNAP   | \
     BCM_FIELD_PKT_FMT_L2_LLC    | \
     BCM_FIELD_PKT_FMT_L2_802_3) /* L2 format mask. */
#define BCM_FIELD_PKT_FMT_OUTER_TAGGED  0x0004     /* Outer tag present. */
#define BCM_FIELD_PKT_FMT_INNER_TAGGED  0x0008     /* Inner tag present. */
#define BCM_FIELD_PKT_FMT_TAGGED_MASK   \
    (BCM_FIELD_PKT_FMT_OUTER_TAGGED | \
     BCM_FIELD_PKT_FMT_INNER_TAGGED) /* Tag info mask. */
#define BCM_FIELD_PKT_FMT_IPV4          0x0010     /* IPv4 format packet. */
#define BCM_FIELD_PKT_FMT_IPV6          0x0020     /* IPv4 format packet. */
#define BCM_FIELD_PKT_FMT_IP_MASK       \
    (BCM_FIELD_PKT_FMT_IPV4 | \
     BCM_FIELD_PKT_FMT_IPV6) /* Packt format mask. */

/* 
 * Field Qualifier VlanFormat (for bcm_field_qualify_VlanFormat).
 * Used to qualify packets based on vlan tag format.
 */
#define BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED  0x1        /* Outer tag present. */
#define BCM_FIELD_VLAN_FORMAT_INNER_TAGGED  0x2        /* Inner tag present. */
#define BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO 0x4        /* Outer tag present and
                                                          vlan id is zero. */
#define BCM_FIELD_VLAN_FORMAT_INNER_TAGGED_VID_ZERO 0x8        /* Inner tag present and
                                                          vlan id is zero. */

/* Packet Resolution Values (for bcm_field_qualify_PacketRes). */
#define BCM_FIELD_PKT_RES_UNKNOWN       0x0        /* No resolution. */
#define BCM_FIELD_PKT_RES_CONTROL       0x1        /* Ethernet control (8808). */
#define BCM_FIELD_PKT_RES_BPDU          0x2        /* L2 cache BPDU bit. */
#define BCM_FIELD_PKT_RES_L2BC          0x3        /* L2 broadcast. */
#define BCM_FIELD_PKT_RES_L2UC          0x4        /* L2 unicast. */
#define BCM_FIELD_PKT_RES_L2UNKNOWN     0x5        /* Unknown L2 dest (DLF). */
#define BCM_FIELD_PKT_RES_L3MCUNKNOWN   0x6        /* Unknown L3 multicast. */
#define BCM_FIELD_PKT_RES_L3MCKNOWN     0x7        /* Known L3 multicast. */
#define BCM_FIELD_PKT_RES_L2MCKNOWN     0x8        /* Known L2 multicast. */
#define BCM_FIELD_PKT_RES_L2MCUNKNOWN   0x9        /* Unknown L2 multicast. */
#define BCM_FIELD_PKT_RES_L3UCKNOWN     0xa        /* Known L3 unicast. */
#define BCM_FIELD_PKT_RES_L3UCUNKNOWN   0xb        /* Unknown L3 unicast. */
#define BCM_FIELD_PKT_RES_MPLSKNOWN     0xc        /* Known MPLS packet   . */
#define BCM_FIELD_PKT_RES_MPLSL3KNOWN   0xd        /* Known MPLS L3 packet. */
#define BCM_FIELD_PKT_RES_MPLSL2KNOWN   0xe        /* Known MPLS L2 packet. */
#define BCM_FIELD_PKT_RES_MPLSUNKNOWN   0xf        /* Unknown MPLS packet . */
#define BCM_FIELD_PKT_RES_LAST          (BCM_FIELD_PKT_RES_MPLSUNKNOWN) /* Must be >= highest valid
                                                      BCM_FIELD_PKT_RES_* */

/* Module header opcodes (for bcm_field_qualify_MHOpcode). */
#define BCM_FIELD_MHOPCODE_CONTROL          0          
#define BCM_FIELD_MHOPCODE_UNICAST          1          
#define BCM_FIELD_MHOPCODE_BROADCAST_DLF    2          
#define BCM_FIELD_MHOPCODE_MULTICAST        3          
#define BCM_FIELD_MHOPCODE_IPMULTICAST      4          

/* 
 * Chain action param1 values. This is the choice of chaining type,
 * internal vs. external and/or the choice of which slice the action(s)
 * come from.
 */
#define BCM_FIELD_CHAINTYPE_EXTERNAL    (1 << 0)   /* Valid only on devices that
                                                      support external TCAMs. */
#define BCM_FIELD_CHAINTYPE_EVEN        (1 << 1)   
#define BCM_FIELD_CHAINTYPE_ODD         (0)        
#define BCM_FIELD_CHAINTYPE_DEFAULT     BCM_FIELD_CHAINTYPE_ODD 

/* User-defined field maximums. */
#define BCM_FIELD_USER_MAX_UDF_NUM  1          /* UDF0 or UDF1. */
#define BCM_FIELD_USER_MAX_USER_NUM 3          /* Valid range: 0..3 */
#define BCM_FIELD_USER_NUM_UDFS     93         
#define BCM_FIELD_USER_FIELD_SIZE   4          /* Bytes per user_num. */
#define BCM_FIELD_USER_MAX_LENGTH   (1 << 5)   /* Number of bits. */

/* Meter select parameters. */
#define BCM_FIELD_METER_PEAK        1          
#define BCM_FIELD_METER_COMMITTED   2          

/* Metering modes for bcmFieldActionMeterConfig param0. */
#define BCM_FIELD_METER_MODE_DEFAULT        0x0        
#define BCM_FIELD_METER_MODE_FLOW           0x1        
#define BCM_FIELD_METER_MODE_trTCM_COLOR_BLIND 0x2        
#define BCM_FIELD_METER_MODE_trTCM_COLOR_AWARE 0x3        
#define BCM_FIELD_METER_MODE_srTCM_COLOR_BLIND 0x6        
#define BCM_FIELD_METER_MODE_srTCM_COLOR_AWARE 0x7        

/* 
 * Counter pair modes for bcmFieldActionUpdateCounter
 * 
 * Specifies increment mode for upper (counter_num=1) and lower
 * (counter_num=0) counter in the pair. For example: GREEN_YELLOW would
 * increment the upper counter for green packets and the lower counter
 * for yellow packets.
 * 
 * BCM_FIELD_COUNTER_MODE_BYTES can be ORd with any of the other values
 * to make the counter count bytes instead of packets.
 */
#define BCM_FIELD_COUNTER_MODE_NO_NO        0x0000     
#define BCM_FIELD_COUNTER_MODE_NO_YES       0x0001     
#define BCM_FIELD_COUNTER_MODE_YES_NO       0x0002     
#define BCM_FIELD_COUNTER_MODE_RED_NOTRED   0x0003     
#define BCM_FIELD_COUNTER_MODE_GREEN_NOTGREEN 0x0004     
#define BCM_FIELD_COUNTER_MODE_GREEN_RED    0x0005     
#define BCM_FIELD_COUNTER_MODE_GREEN_YELLOW 0x0006     
#define BCM_FIELD_COUNTER_MODE_RED_YELLOW   0x0007     
#define BCM_FIELD_COUNTER_MODE_BYTES_PACKETS 0x0008     /* Upper counter is
                                                          bytes; lower counter
                                                          is packets */
#define BCM_FIELD_COUNTER_MODE_GREEN        0x0010     
#define BCM_FIELD_COUNTER_MODE_YELLOW       0x0020     
#define BCM_FIELD_COUNTER_MODE_RED          0x0040     
#define BCM_FIELD_COUNTER_MODE_ALL          0x0070     
#define BCM_FIELD_COUNTER_MODE_PACKETS      0x0000     
#define BCM_FIELD_COUNTER_MODE_BYTES        0x0100     
#define BCM_FIELD_COUNTER_MODE_DEFAULT      BCM_FIELD_COUNTER_MODE_NO_YES 

/* Opaque handle to a field entry. */
typedef int bcm_field_entry_t;

/* Opaque handle to a field group. */
typedef int bcm_field_group_t;
#if 1/* anzhen.zheng, 5/26/2008 */
typedef int bcm_field_group_sbmp_t;
#endif
/* 
 * Field Qualifiers
 * 
 * Field qualifiers are collected into qualifier sets (qsets), which are
 * used to allocate field groups.
 * 
 * Note: These are in order of their bit width. Do not change the order
 * or the field select code algorithm for wide-mode will get slower.
 */
typedef enum bcm_field_qualify_e {
    bcmFieldQualifySrcIp6,              /* [0x00] Source IPv6 Address. */
    bcmFieldQualifyDstIp6,              /* [0x01] Destination IPv6 Address. */
    bcmFieldQualifySrcIp6High,          /* [0x02] Src. IPv6 Address (High/Upper
                                           64 bits). */
    bcmFieldQualifyDstIp6High,          /* [0x03] Dest IPv6 Address (High/Upper
                                           64 bits). */
    bcmFieldQualifySrcMac,              /* [0x04] Source L2 MAC Address. */
    bcmFieldQualifyDstMac,              /* [0x05] Destination L2 MAC Address. */
    bcmFieldQualifySnap,                /* [0x06] 40: SNAP Header. */
    bcmFieldQualifySrcIp,               /* [0x07] Source IPv4 Address. */
    bcmFieldQualifyDstIp,               /* [0x08] Destination IPv4 Address. */
    bcmFieldQualifyLlc,                 /* [0x09] 24: LLC Header. */
    bcmFieldQualifyInPort,              /* [0x0a] Single Input Port. */
    bcmFieldQualifyInPorts,             /* [0x0b] Input Port Bitmap. */
    bcmFieldQualifyOutPorts,            /* [0x0c] Output Port Bitmap. */
    bcmFieldQualifyDrop,                /* [0x0d] Packet is flagged to be
                                           dropped . */
    bcmFieldQualifyIp6FlowLabel,        /* [0x0e] IPv6 Flow Label. */
    bcmFieldQualifyOuterVlan,           /* [0x0f] Outer VLAN. */
    bcmFieldQualifyInnerVlan,           /* [0x10] Inner VLAN (double-tagged pkts
                                           only). */
    bcmFieldQualifyLookupStatus,        /* [0x11] DEPRECATED Lookup status
                                           (BCM_FIELD_LOOKUP_xxx). */
    bcmFieldQualifyRangeCheck,          /* [0x12] TCP/UDP Port Range Check
                                           Results. */
    bcmFieldQualifyL4SrcPort,           /* [0x13] TCP/UDP Source Port. */
    bcmFieldQualifyL4DstPort,           /* [0x14] TCP/UDP Destination Port. */
    bcmFieldQualifyEtherType,           /* [0x15] Ethernet Type. */
    bcmFieldQualifyIpProtocol,          /* [0x16] IP Protocol Field. */
    bcmFieldQualifyDSCP,                /* [0x17] Diffserv Code Point. */
    bcmFieldQualifyTos = bcmFieldQualifyDSCP, /* [0x17] Tos - same as DSCP. */
    bcmFieldQualifyTtl,                 /* [0x18] Time To Live/Hop Limit. */
    bcmFieldQualifyIp6NextHeader,       /* [0x19] IPv6 Next Header/IP Protocol. */
    bcmFieldQualifyIp6TrafficClass,     /* [0x1a] IPv6 Traffic Class. */
    bcmFieldQualifyIp6HopLimit,         /* [0x1b] IPv6 Hop Limit. */
    bcmFieldQualifySrcModid,            /* [0x1c] Source Module ID (deprecated). */
    bcmFieldQualifySrcPortTgid,         /* [0x1d] Source Port/Trunk Group ID
                                           (deprecated). */
    bcmFieldQualifySrcPort,             /* [0x1e] Source Module ID and Port . */
    bcmFieldQualifySrcTrunk,            /* [0x1f] Source Trunk Group ID . */
    bcmFieldQualifyDstModid,            /* [0x20] Destination Module ID
                                           (deprecated). */
    bcmFieldQualifyDstPortTgid,         /* [0x21] Dest Port/Trunk Group ID
                                           (deprecated. */
    bcmFieldQualifyDstPort,             /* [0x22] Destination Module ID and Port
                                           . */
    bcmFieldQualifyDstTrunk,            /* [0x23] Destination Trunk Group ID . */
    bcmFieldQualifyTcpControl,          /* [0x24] TCP Control Flags. */
    bcmFieldQualifyPacketRes,           /* [0x25] Pkt resolution
                                           (BCM_FIELD_PKT_RES_xxx). */
    bcmFieldQualifyPacketFormat,        /* [0x26] DEPRECATED Packet Format
                                           (BCM_FIELD_PKT_FMT_xxx). */
    bcmFieldQualifySrcClassL2,          /* [0x27] Source Class based on L2
                                           lookup result. */
    bcmFieldQualifySrcMacGroup = bcmFieldQualifySrcClassL2, /* [0x27] Same as
                                           bcmFieldQualifySrcClassL2. */
    bcmFieldQualifySrcClassL3,          /* [0x28] Source Class based on L3
                                           lookup result. */
    bcmFieldQualifySrcClassField,       /* [0x29] Source Class based on
                                           bcmFieldStageLookup result. */
    bcmFieldQualifyDstClassL2,          /* [0x2a] Destination Class based on L2
                                           lookup result. */
    bcmFieldQualifyDstClassL3,          /* [0x2b] Destination Class based on L3
                                           lookup result. */
    bcmFieldQualifyDstClassField,       /* [0x2c] Destination Class based on
                                           bcmFieldStageLookup result. */
    bcmFieldQualifyLookupClass0 = bcmFieldQualifyDstClassField, /* [0x2c] Same as
                                           bcmFieldQualifyDstClassField. */
    bcmFieldQualifyIpInfo,              /* [0x2d] IP Information
                                           (BCM_FIELD_IP_xxx). */
    bcmFieldQualifyIpProtocolCommon,    /* [0x2e] 3: Common IP protocols. */
    bcmFieldQualifyMHOpcode,            /* [0x2f] Module Header Opcode. */
    bcmFieldQualifyIpFlags,             /* [0x30] IPv4 Flags. */
    bcmFieldQualifyInnerTpid,           /* [0x31] 2: Inner VLAN TPID. */
    bcmFieldQualifyOuterTpid,           /* [0x32] 2: Outer VLAN TPID. */
    bcmFieldQualifyIpType,              /* [0x33] IP Type (bcmFieldIpTypeXXX). */
    bcmFieldQualifyChain,               /* [0x34] Internal use only. */
    bcmFieldQualifyDecap,               /* [0x35] Decapsulation occurred. */
    bcmFieldQualifyHiGig,               /* [0x36] HiGig packet. */
    bcmFieldQualifyHiGigProxy,          /* [0x37] HiGig proxy processing is
                                           enabled. */
    bcmFieldQualifyStage,               /* [0x38] Field Processor pipeline
                                           stage. */
    bcmFieldQualifyStageIngress,        /* [0x39] Field Processor pipeline
                                           ingress stage. */
    bcmFieldQualifyStageIngressSecurity, /* [0x3a] Field Processor ingress
                                           security stage. */
    bcmFieldQualifyStageIngressQoS,     /* [0x3b] Field Processor ingress QOS
                                           stage. */
    bcmFieldQualifyStageLookup,         /* [0x3c] Field Processor pipeline
                                           lookup stage. */
    bcmFieldQualifyStageEgress,         /* [0x3d] Field Processor pipeline
                                           egress stage. */
    bcmFieldQualifyStageEgressSecurity, /* [0x3e] Field Processor egress
                                           security stage. */
    bcmFieldQualifyStageExternal,       /* [0x3f] Field Processor Rules in
                                           External slice. */
    bcmFieldQualifySrcIpEqualDstIp,     /* [0x40] Src IP == Dst IP. */
    bcmFieldQualifyEqualL4Port,         /* [0x41] L4 Src Port == Dst Port. */
    bcmFieldQualifyTcpSequenceZero,     /* [0x42] Sequence number = 0. */
    bcmFieldQualifyTcpHeaderSize,       /* [0x43] TCP Header Size. */
    bcmFieldQualifyInterfaceClassL2,    /* [0x44] Vlan Class Id. */
    bcmFieldQualifyInterfaceClassL3,    /* [0x45] L3 Interface Class Id. */
    bcmFieldQualifyL3IntfGroup = bcmFieldQualifyInterfaceClassL3, /* [0x45] Same as
                                           bcmFieldQualifyInterfaceClass. */
    bcmFieldQualifyInterfaceClassPort,  /* [0x46] Port Class Id. */
    bcmFieldQualifyPortClass = bcmFieldQualifyInterfaceClassPort, /* [0x46] Same as
                                           bcmFieldQualifyInterfaceClassPort. */
    bcmFieldQualifyL3Routable,          /* [0x47] L3 routable bit for FB2. */
    bcmFieldQualifyIpFrag,              /* [0x48] IP Fragment info. */
    bcmFieldQualifyVrf,                 /* [0x49] VRF Id for FB 2. */
    bcmFieldQualifyOutPort,             /* [0x4a] Single Output Port. */
    bcmFieldQualifyIp4,                 /* [0x4b] Qualify IpType == IPv4Any. */
    bcmFieldQualifyIp6,                 /* [0x4c] Qualify IpType == Ipv6Any. */
    bcmFieldQualifyL2Format,            /* [0x4d] Qualify on L2 header format. */
    bcmFieldQualifyVlanFormat,          /* [0x4e] Qualify on vlan tag format. */
    bcmFieldQualifyExtensionHeaderType, /* [0x4f] Qualify on  Next Header Field
                                           in First Extension Header. */
    bcmFieldQualifyExtensionHeaderSubCode, /* [0x50] Qualify on First byte after
                                           extentsion header length field. */
    bcmFieldQualifyL4Ports,             /* [0x51] Four bytes after L3 header are
                                           available. */
    bcmFieldQualifyMirrorCopy,          /* [0x52] Qualify mirrored traffic. */
    bcmFieldQualifyTunnelTerminated,    /* [0x53] Qualify tunnel terminated
                                           traffic. */
    bcmFieldQualifyMplsTerminated,      /* [0x54] Qualify mpls terminated
                                           traffic. */
    bcmFieldQualifyInnerSrcIp,          /* [0x55] Inner ip header source IPv6
                                           Address. */
    bcmFieldQualifyInnerDstIp,          /* [0x56] Inner ip header destination
                                           IPv6 Address. */
    bcmFieldQualifyInnerSrcIp6,         /* [0x57] Inner ip header source IPv6
                                           Address. */
    bcmFieldQualifyInnerDstIp6,         /* [0x58] Inner ip header destination
                                           IPv6 Address. */
    bcmFieldQualifyInnerSrcIp6High,     /* [0x59] Inner ip header Src. IPv6
                                           Address (High/Upper 64 bits). */
    bcmFieldQualifyInnerDstIp6High,     /* [0x5a] Inner ip header Dest IPv6
                                           Address (High/Upper 64 bits). */
    bcmFieldQualifyInnerTtl,            /* [0x5b] Inner ip header time To
                                           Live/Hop Limit. */
    bcmFieldQualifyInnerDSCP,           /* [0x5c] Inner ip header diffserv Code
                                           Point. */
    bcmFieldQualifyInnerIpProtocol,     /* [0x5d] Inner ip header IP Protocol
                                           Field. */
    bcmFieldQualifyInnerIpFrag,         /* [0x5e] Inner ip header IP Fragment
                                           info. */
    bcmFieldQualifyVlanTranslationHit,  /* [0x5f] Vlan Translation table hit. */
    bcmFieldQualifyForwardingVlanValid, /* [0x60] Forwarding vlan is valid. */
    bcmFieldQualifyIngressStpState,     /* [0x61] Ingress Vlan STG STP state. */
    bcmFieldQualifyL2SrcHit,            /* [0x62] L2 Source lookup success. */
    bcmFieldQualifyL2SrcStatic,         /* [0x63] L2 Source is static entry. */
    bcmFieldQualifyL2DestHit,           /* [0x64] L2 Destination lookup success. */
    bcmFieldQualifyL2StationMove,       /* [0x65] L2 Source port mismatch. */
    bcmFieldQualifyL2CacheHit,          /* [0x66] L2 Destination cache lookup
                                           sucess. */
    bcmFieldQualifyL3SrcHostHit,        /* [0x67] L3 Source lookup host table
                                           hit. */
    bcmFieldQualifyL3DestHostHit,       /* [0x68] L3 Destination lookup host
                                           table hit. */
    bcmFieldQualifyL3DestRouteHit,      /* [0x69] L3 Destination lookup route
                                           table hit. */
    bcmFieldQualifyIpmcStarGroupHit,    /* [0x6a] L3 Multicast lookup
                                           (Star,Group) hit. */
    bcmFieldQualifyDosAttack,           /* [0x6b] Denial of Service attack
                                           frame. */
    bcmFieldQualifyIpAuth,              /* [0x6c] IP Authenticated. */
    bcmFieldQualifyClassId,             /* [0x6d] internal use only. */
    bcmFieldQualifyBigIcmpCheck,        /* [0x6e] internal use only. */
    bcmFieldQualifyIcmpTypeCode,        /* [0x6f] internal use only. */
    bcmFieldQualifyIgmpTypeMaxRespTime, /* [0x70] internal use only. */
    bcmFieldQualifyCount                /* Always Last. Not a usable value. */
} bcm_field_qualify_t;

#define BCM_FIELD_QUALIFY_STRINGS \
{ \
    "SrcIp6", \
    "DstIp6", \
    "SrcIp6High", \
    "DstIp6High", \
    "SrcMac", \
    "DstMac", \
    "Snap", \
    "SrcIp", \
    "DstIp", \
    "Llc", \
    "InPort", \
    "InPorts", \
    "OutPorts", \
    "Drop", \
    "Ip6FlowLabel", \
    "OuterVlan", \
    "InnerVlan", \
    "LookupStatus", \
    "RangeCheck", \
    "L4SrcPort", \
    "L4DstPort", \
    "EtherType", \
    "IpProtocol", \
    "DSCP", \
    "Ttl", \
    "Ip6NextHeader", \
    "Ip6TrafficClass", \
    "Ip6HopLimit", \
    "SrcModid", \
    "SrcPortTgid", \
    "SrcPort", \
    "SrcTrunk", \
    "DstModid", \
    "DstPortTgid", \
    "DstPort", \
    "DstTrunk", \
    "TcpControl", \
    "PacketRes", \
    "PacketFormat", \
    "SrcClassL2", \
    "SrcClassL3", \
    "SrcClassField", \
    "DstClassL2", \
    "DstClassL3", \
    "DstClassField", \
    "IpInfo", \
    "IpProtocolCommon", \
    "MHOpcode", \
    "IpFlags", \
    "InnerTpid", \
    "OuterTpid", \
    "IpType", \
    "Chain", \
    "Decap", \
    "HiGig", \
    "HiGigProxy", \
    "Stage", \
    "StageIngress", \
    "StageIngressSecurity", \
    "StageIngressQoS", \
    "StageLookup", \
    "StageEgress", \
    "StageEgressSecurity", \
    "StageExternal", \
    "SrcIpEqualDstIp", \
    "EqualL4Port", \
    "TcpSequenceZero", \
    "TcpHeaderSize", \
    "InterfaceClassL2", \
    "InterfaceClassL3", \
    "InterfaceClassPort", \
    "L3Routable", \
    "IpFrag", \
    "Vrf", \
    "OutPort", \
    "Ip4", \
    "Ip6", \
    "L2Format", \
    "VlanFormat", \
    "ExtensionHeaderType", \
    "ExtensionHeaderSubCode", \
    "L4Ports", \
    "MirrorCopy", \
    "TunnelTerminated", \
    "MplsTerminated", \
    "InnerSrcIp", \
    "InnerDstIp", \
    "InnerSrcIp6", \
    "InnerDstIp6", \
    "InnerSrcIp6High", \
    "InnerDstIp6High", \
    "InnerTtl", \
    "InnerDSCP", \
    "InnerIpProtocol", \
    "InnerIpFrag", \
    "VlanTranslationHit", \
    "ForwardingVlanValid", \
    "IngressStpState", \
    "L2SrcHit", \
    "L2SrcStatic", \
    "L2DestHit", \
    "L2StationMove", \
    "L2CacheHit", \
    "L3SrcHostHit", \
    "L3DestHostHit", \
    "L3DestRouteHit", \
    "IpmcStarGroupHit", \
    "DosAttack", \
    "IpAuth", \
    "ClassId", \
    "BigIcmpCheck", \
    "IcmpTypeCode", \
    "IgmpTypeMaxRespTime"  \
}

#define BCM_FIELD_QUALIFY_MAX   (bcmFieldQualifyCount + BCM_FIELD_USER_NUM_UDFS) /* Must be >= bcmFieldQualifyCount. */

#define BCM_FIELD_QUALIFY_WIDTH_MAX 30         

#define BCM_FIELD_QSET_WIDTH_MAX    \
    ((BCM_FIELD_QUALIFY_WIDTH_MAX+1) * \
    bcmFieldQualifyCount + 1) 

#define BCM_FIELD_EXACT_MATCH_MASK  (~0)       /* Mask parameter value. */

/* 
 * Field Qualifier Decap.
 * 
 * Possible data values for bcm_field_qualify_decap().
 */
typedef enum bcm_field_decap_e {
    bcmFieldDecapNone,          /* No Decap. */
    bcmFieldDecapL2Mpls,        /* L2 MPLS Decap. */
    bcmFieldDecapL3Mpls,        /* L3 MPLS Decap. */
    bcmFieldDecapOtherMPLS,     /* non-L2 or L3 MPLS Decap. */
    bcmFieldDecapIpTunnel,      /* IP Tunnel Decap. */
    bcmFieldDecapIpGreTunnel,   /* IP GRE Tunnel Decap. */
    bcmFieldDecapIpUdpTunnel,   /* IP UDP Tunnel Decap. */
    bcmFieldDecapIpPimTunnel,   /* IP PIM Tunnel Decap. */
    bcmFieldDecapCount          /* Always Last. Not a usable value. */
} bcm_field_decap_t;

#define BCM_FIELD_DECAP_STRINGS \
{ \
    "None", \
    "L2Mpls", \
    "L3Mpls", \
    "OtherMPLS", \
    "IpTunnel", \
    "IpGreTunnel", \
    "IpUdpTunnel", \
    "IpPimTunnel"  \
}

/* 
 * Field Statge Type
 * 
 * Selects which pipeline stage an entry applies to.
 */
typedef enum bcm_field_stage_e {
    bcmFieldStageFirst,         /* Earliest Field stage in device. */
    bcmFieldStageIngressEarly,  /* Early Ingress Field stage. */
    bcmFieldStageIngressLate,   /* Earie Ingress Field stage. */
    bcmFieldStageDefault,       /* Default stage for device. */
    bcmFieldStageLast,          /* Latest Field stage in device. */
    bcmFieldStageCount          /* Always Last. Not a usable value. */
} bcm_field_stage_t;

#define BCM_FIELD_STAGE_STRINGS \
{ \
    "First", \
    "IngressEarly", \
    "IngressLate", \
    "Default", \
    "Last"  \
}

/* Field Qualifier IpType (for bcm_field_qualify_IpType). */
typedef enum bcm_field_IpType_e {
    bcmFieldIpTypeAny,              /* Don't care. */
    bcmFieldIpTypeNonIp,            /* Non-Ip packet. */
    bcmFieldIpTypeIpv4Not,          /* Anything but IPv4 packets. */
    bcmFieldIpTypeIpv4NoOpts,       /* IPv4 without options. */
    bcmFieldIpTypeIpv4WithOpts,     /* IPv4 with options. */
    bcmFieldIpTypeIpv4Any,          /* Any IPv4 packet. */
    bcmFieldIpTypeIpv6Not,          /* Anything but IPv6 packets. */
    bcmFieldIpTypeIpv6,             /* IPv6 packet. */
    bcmFieldIpTypeIp,               /* IPv4 and IPv6 packets. */
    bcmFieldIpTypeArp,              /* ARP/RARP. */
    bcmFieldIpTypeArpRequest,       /* ARP Request. */
    bcmFieldIpTypeArpReply,         /* ARP Reply. */
    bcmFieldIpTypeMplsUnicast,      /* Mpls unicast frame (ethertype = 0x8847). */
    bcmFieldIpTypeMplsMulticast,    /* Mpls mcast frame   (ethertype = 0x8848). */
    bcmFieldIpTypeCount             /* Always Last. Not a usable value. */
} bcm_field_IpType_t;

#define BCM_FIELD_IPTYPE_STRINGS \
{ \
    "Any", \
    "NonIp", \
    "Ipv4Not", \
    "Ipv4NoOpts", \
    "Ipv4WithOpts", \
    "Ipv4Any", \
    "Ipv6Not", \
    "Ipv6", \
    "Ip", \
    "Arp", \
    "ArpRequest", \
    "ArpReply", \
    "MplsUnicast", \
    "MplsMulticast"  \
}

/* 
 * Field Qualifier IpProtocolCommon (for
 * bcm_field_qualify_IpProtocolCommon).
 */
typedef enum bcm_field_IpProtocolCommon_e {
    bcmFieldIpProtocolCommonTcp, 
    bcmFieldIpProtocolCommonUdp, 
    bcmFieldIpProtocolCommonIgmp, 
    bcmFieldIpProtocolCommonIcmp, 
    bcmFieldIpProtocolCommonIp6Icmp, 
    bcmFieldIpProtocolCommonIp6HopByHop, 
    bcmFieldIpProtocolCommonIpInIp, 
    bcmFieldIpProtocolCommonTcpUdp, 
    bcmFieldIpProtocolCommonCount       /* Always Last. Not a usable value. */
} bcm_field_IpProtocolCommon_t;

#define BCM_FIELD_IPPROTOCOLCOMMON_STRINGS \
{ \
    "TCP", \
    "UDP", \
    "IGMP", \
    "ICMP", \
    "IPv6 ICMP", \
    "IPv6 Hop-by-Hop", \
    "IPv4 Payload", \
    "TCP and UDP"  \
}

/* Field Qualifier IpFrag (for bcm_field_qualify_IpFrag). */
typedef enum bcm_field_IpFrag_e {
    bcmFieldIpFragNon,          /* Non-fragmented packet. */
    bcmFieldIpFragFirst,        /* First fragment of fragmented packet. */
    bcmFieldIpFragNonOrFirst,   /* Non-fragmented or first fragment. */
    bcmFieldIpFragNotFirst,     /* Not the first fragment. */
    bcmFieldIpFragCount         /* Always last. Not a usable value. */
} bcm_field_IpFrag_t;

/* 
 * Field Qualifier L2Format (for bcm_field_qualify_L2Format).
 * Used to qualify packets based on L2 header format.
 */
typedef enum bcm_field_L2Format_e {
    bcmFieldL2FormatAny,        /* Do not qualify on L2 format. */
    bcmFieldL2FormatEthII,      /* Ethernet 2 (802.2). */
    bcmFieldL2FormatSnap,       /* Sub-Network Access Protocol (SNAP). */
    bcmFieldL2FormatLlc,        /* Logical Link Control. */
    bcmFieldL2Format802dot3,    /* 802.3 frame format. */
    bcmFieldL2FormatCount       /* Always Last. Not a usable value. */
} bcm_field_L2Format_t;

#define BCM_FIELD_L2FORMAT_STRINGS \
{ \
    "Any", \
    "EthII", \
    "Snap", \
    "Llc", \
    "802dot3"  \
}

/* LLC Header Format. */
typedef struct bcm_field_llc_header_s {
    uint8 dsap; 
    uint8 ssap; 
    uint8 control; 
} bcm_field_llc_header_t;

/* SNAP Header Format. */
typedef struct bcm_field_snap_header_s {
    uint32 org_code; 
    uint16 type; 
} bcm_field_snap_header_t;

/* 
 * Range Check.
 * 
 * bcm_field_range_create requires exactly one of SRCPORT or DSTPORT or
 * FIELD_RANGE_OUTER_VLAN or FIELD_RANGE_PACKET_LENGTH.
 */
#define BCM_FIELD_RANGE_SRCPORT         0x00000001 
#define BCM_FIELD_RANGE_DSTPORT         0x00000002 
#define BCM_FIELD_RANGE_TCP             0x00000004 
#define BCM_FIELD_RANGE_UDP             0x00000008 
#define BCM_FIELD_RANGE_INVERT          0x00000010 
#define BCM_FIELD_RANGE_EXTERNAL        0x00000020 
#define BCM_FIELD_RANGE_OUTER_VLAN      0x00000040 
#define BCM_FIELD_RANGE_PACKET_LENGTH   0x00000080 

/* Opaque handle to a field range. */
typedef uint32 bcm_field_range_t;

/* 
 * Opaque handle to a User Defined Field (UDF).
 * Note: bcm_field_range_create requires exactly one of SRCPORT or
 * DSTPORT.
 */
typedef uint32 bcm_field_udf_t;

/* Field udf_spec defines the locations of user-defined byte fields. */
typedef struct bcm_field_udf_spec_s {
    uint32 offset[384]; 
} bcm_field_udf_spec_t;

/* Field qset defines the set of fields used for lookup. */
typedef struct bcm_field_qset_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX)]; 
    SHR_BITDCL udf_map[_SHR_BITDCLSIZE(BCM_FIELD_USER_NUM_UDFS)]; 
} bcm_field_qset_t;


/* Group Modes (single, double or triple wide). */
typedef enum bcm_field_group_mode_e {
    bcmFieldGroupModeSingle,    /* Single-wide group. */
    bcmFieldGroupModeDouble,    /* Double-wide group. */
    bcmFieldGroupModeTriple,    /* Triple-wide group. */
    bcmFieldGroupModeAuto,      /* Auto-expand group based on Qset (default). */
    bcmFieldGroupModeCount      /* Always last. Not a usable value. */
} bcm_field_group_mode_t;

#define BCM_FIELD_GROUP_MODE_STRINGS \
{ \
    "Single", \
    "Double", \
    "Triple", \
    "Auto"  \
}

#define bcmFieldGroupModeDefault    bcmFieldGroupModeAuto 

#define bcm_field_entry_remove  _bcm_field_fb_entry_remove

/*
 * Function:
 *     _bcm_field_fb_entry_remove
 * Purpose:
 *     Remove a previously installed physical entry.
 * Parameters:
 *     unit   - BCM device number
 *     f_ent  - Physical entry data
 * Returns:
 *     BCM_E_XXX
 *     BCM_E_NONE
 * Notes:
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_fb_entry_remove(UI32_T device_id, UI32_T rule_id);

#endif
