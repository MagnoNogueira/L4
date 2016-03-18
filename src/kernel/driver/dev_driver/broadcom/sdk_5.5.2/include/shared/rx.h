/*
 * $Id: rx.h,v 1.8.6.2 Broadcom SDK $
 * $Copyright: Copyright 2008 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:        rx.h
 * Purpose:     Packet Receive Reason Types shared between BCM and SOC layers
 */

#ifndef   _SHR_RX_H_
#define   _SHR_RX_H_

#include <shared/types.h>
#include <shared/bitop.h>

/*
 * PKT RX Packet Reasons; reason CPU received the packet.
 * Notes:
 *      It is possible no reasons are set (directed to CPU from ARL
 *      for example), or multiple reasons may be set.
 */

typedef enum _shr_rx_reason_e {
    _SHR_RX_INVALID,
    _SHR_RX_ARP,                   /* ARP Packet                        */
    _SHR_RX_BPDU,                  /* BPDU Packet                       */
    _SHR_RX_BROADCAST,             /* Broadcast packet                  */
    _SHR_RX_CLASS_BASED_MOVE,      /* Class-based move prevented        */
    _SHR_RX_CLASS_TAG_PACKETS,     /* Higig Header with PPD=1           */
    _SHR_RX_CONTROL,               /* Control frame or reserved addr    */
    _SHR_RX_CPU_LEARN,             /* CPU Learned (or VLAN not found    */
                                   /* on Strata devices)                */
    _SHR_RX_DEST_LOOKUP_FAIL,      /* Destination lookup fail (or L3    */
                                   /* station move on Strata devices)   */
    _SHR_RX_DHCP,                  /* DHCP packets                      */
    _SHR_RX_DOS_ATTACK,            /* DOS Attack Packet                 */
    _SHR_RX_E2E_HOL_IBP,           /* E2E HOL or IBP Packet             */
    _SHR_RX_FILTER_MATCH,          /* Filter Match                      */
    _SHR_RX_GRE_CHECKSUM,          /* GRE checksum                      */
    _SHR_RX_GRE_SOURCE_ROUTE,      /* GRE source routing                */
    _SHR_RX_HIGIG_CONTROL,         /* Higig Packet with Control Opcode  */
    _SHR_RX_HIGIG_HDR_ERROR,       /* Higig+ header errors              */
    _SHR_RX_ICMP_REDIRECT,         /* ICMP Recirect                     */
    _SHR_RX_IGMP,                  /* IGMP packet                       */
    _SHR_RX_INGRESS_FILTER,        /* Ingress Filter (VLAN membership)  */
    _SHR_RX_IP,                    /* IP packet                         */
    _SHR_RX_IP_MCAST_MISS,         /* IPMC miss                         */
    _SHR_RX_IP_OPTION_VERSION,     /* IP options present or IP ver != 4 */
    _SHR_RX_IPMC,                  /* Class D IP multicast packet       */
    _SHR_RX_IPMC_RSVD,             /* IPMC Reserved Packet              */
    _SHR_RX_L2_CPU,                /* L2_TABLE - copy to CPU (on 5690)  */
    _SHR_RX_L2_DEST_MISS,          /* L2 destination lookup failure     */
    _SHR_RX_L2_LEARN_LIMIT,        /* L2 Learn Limit                    */
    _SHR_RX_L2_MOVE,               /* L2 Station Movement               */
    _SHR_RX_L2_MTU_FAIL,           /* L2 MTU check fail                 */
    _SHR_RX_L2_NON_UNICAST_MISS,   /* L2 Non-Unicast lookup miss        */
    _SHR_RX_L2_SOURCE_MISS,        /* L2 source lookup failure          */
    _SHR_RX_L3_DEST_MISS,          /* L3 DIP Miss                       */
    _SHR_RX_L3_HEADER_ERROR,       /* L3 header - IP options,           */
    _SHR_RX_L3_MTU_FAIL,           /* L3 MTU check fail                 */
    _SHR_RX_L3_SLOW_PATH,          /* L3 slow path processed pkt.       */
    _SHR_RX_L3_SOURCE_MISS,        /* L3 SIP Miss                       */
    _SHR_RX_L3_SOUCE_MOVE,         /* L3 Station Movement               */
    _SHR_RX_MARTIAN_ADDR,          /* Pkt. with Martian address         */
    _SHR_RX_MCAST_IDX_ERROR,       /* Multicast index error             */
    _SHR_RX_MCAST_MISS,            /* MC miss                           */
    _SHR_RX_MPLS_ERROR,            /* MPLS error                        */
    _SHR_RX_MPLS_INVALID_ACTION,   /* MPLS Invalid Action               */
    _SHR_RX_MPLS_INVALID_PAYLOAD,  /* MPLS Invalid Payload              */
    _SHR_RX_MPLS_LABEL_MISS,       /* MPLS table miss                   */
    _SHR_RX_MPLS_SEQUENCE_NUMBER,  /* MPLS Sequence number              */
    _SHR_RX_MPLS_TTL,              /* MPLS TTL                          */
    _SHR_RX_MULTICAST,             /* Multicast packet                  */
    _SHR_RX_NHOP,                  /* Copy to CPU from Next Hop Idx Tbl */
    _SHR_RX_PARITY_ERROR,          /* Parity error on IP tables         */
    _SHR_RX_PROTOCOL,              /* Protocol Packet                   */
    _SHR_RX_SAMPLE_DEST,           /* Egress  sFlow sampled             */
    _SHR_RX_SAMPLE_SOURCE,         /* Ingress sFlow sampled             */
    _SHR_RX_SHARED_VLAN_MISMATCH,  /* Private VLAN Mismatch             */
    _SHR_RX_SOURCE_ROUTE,          /* Source routing bit set            */
    _SHR_RX_TTL,                   /* TTL <= 0 or TTL < IPMC threshold  */
    _SHR_RX_TTL1,                  /* L3UC or IPMC packet with TTL      */
                                   /* equal to 1                        */
    _SHR_RX_TUNNEL_ERROR,          /* Tunnel error trap                 */
    _SHR_RX_UDP_CHECKSUM,          /* UDP checksum                      */
    _SHR_RX_UNKNOWN_VLAN,          /* unknown VLAN; VID = 0xfff;        */
                                   /* CPU Learn bit (on 5690 devices)   */
    _SHR_RX_URPF_FAIL,             /* URPF Check Failed                 */
    _SHR_RX_VC_LABEL_MISS,         /* VPLS table miss                   */
    _SHR_RX_VLAN_FILTER_MATCH,     /* VLAN Filter Match                 */
    _SHR_RX_EXCEPTION_FLOOD,       /* Exception processing or flooding  (Robo chips) */
    _SHR_RX_TIMESYNC,              /* Time Sync protocol packet */
    _SHR_RX_EAV_DATA,              /* Ethernet AV data packet */
    _SHR_RX_SAME_PORT_BRIDGE,      /* Hairpin or Same port switching/bridging */
    _SHR_RX_SPLIT_HORIZON,         /* Basic bridging or VPLS Split horizon */
    _SHR_RX_L4_ERROR,              /* TCP/UDP header or port number errors */
    _SHR_RX_STP,                   /* STP Ingress or Egress checks      */
    _SHR_RX_REASON_COUNT           /* MUST BE LAST                      */
} _shr_rx_reason_t;

#define _SHR_RX_REASON_NAMES_INITIALIZER { \
    "Invalid",             \
    "Arp",                 \
    "Bpdu",                \
    "Broadcast",           \
    "ClassBasedMove",      \
    "ClassTagPackets",     \
    "Control",             \
    "CpuLearn",            \
    "DestLookupFail",      \
    "Dhcp",                \
    "DosAttack",           \
    "E2eHolIbp",           \
    "FilterMatch",         \
    "GreChecksum",         \
    "GreSourceRoute",      \
    "HigigControl",        \
    "HigigHdrError",       \
    "IcmpRedirect",        \
    "Igmp",                \
    "IngressFilter",       \
    "Ip",                  \
    "IpMcastMiss",         \
    "IpOptionVersion",     \
    "Ipmc",                \
    "IpmcRsvd",            \
    "L2Cpu",               \
    "L2DestMiss",          \
    "L2LearnLimit",        \
    "L2Move",              \
    "L2MtuFail",           \
    "L2NonUnicastMiss",    \
    "L2SourceMiss",        \
    "L3DestMiss",          \
    "L3HeaderError",       \
    "L3MtuFail",           \
    "L3Slowpath",          \
    "L3SourceMiss",        \
    "L3SourceMove",        \
    "MartianAddr",         \
    "McastIdxError",       \
    "McastMiss",           \
    "MplsError",           \
    "MplsInvalidAction",   \
    "MplsInvalidPayload",  \
    "MplsLabelMiss",       \
    "MplsSequenceNumber",  \
    "MplsTtl",             \
    "Multicast",           \
    "Nhop",                \
    "ParityError",         \
    "Protocol",            \
    "SampleDest",          \
    "SampleSource",        \
    "SharedVlanMismatch",  \
    "SourceRoute",         \
    "Ttl",                 \
    "Ttl1",                \
    "TunnelError",         \
    "UdpChecksum",         \
    "UnknownVlan",         \
    "UrpfFail",            \
    "VcLabelMiss",         \
    "VlanFilterMatch",     \
    "ExceptionFlood",      \
    "Timesync",            \
    "EavData",             \
    "SamePortBridge",      \
    "SplitHorizon",        \
    "L4Error",             \
    "Stp"                  \
}

/*
 * Set of "reasons" (see _SHR_RX_*) why a packet came to the CPU.
 */
typedef struct _shr_rx_reasons_s {
    SHR_BITDCL  w[_SHR_BITDCLSIZE(_SHR_RX_REASON_COUNT)];
} _shr_rx_reasons_t;

/*
 * Macro to check if a reason (_SHR_RX_*) is included in a
 * set of reasons (_shr_rx_reasons_t). Returns:
 *   zero     => reason is not included in the set
 *   non-zero => reason is included in the set
 */
#define _SHR_RX_REASON_GET(_reasons, _reason) \
        SHR_BITGET(((_reasons).w), (_reason))

/*
 * Macro to add a reason (_SHR_RX_*) to a set of
 * reasons (_shr_rx_reasons_t)
 */
#define _SHR_RX_REASON_SET(_reasons, _reason) \
        SHR_BITSET(((_reasons).w), (_reason))

/*
 * Macro to add all reasons (_SHR_RX_*) to a set of
 * reasons (_shr_rx_reasons_t)
 */
#define _SHR_RX_REASON_SET_ALL(_reasons) \
        SHR_BITSET_RANGE(((_reasons).w), 0, _SHR_RX_REASON_COUNT)

/*
 * Macro to clear a reason (_SHR_RX_*) from a set of
 * reasons (_shr_rx_reasons_t)
 */
#define _SHR_RX_REASON_CLEAR(_reasons, _reason) \
        SHR_BITCLR(((_reasons).w), (_reason))

/*
 * Macro to clear a set of reasons (_shr_rx_reasons_t).
 */
#define _SHR_RX_REASON_CLEAR_ALL(_reasons) \
        SHR_BITCLR_RANGE(((_reasons).w), 0, _SHR_RX_REASON_COUNT)

#endif /* _SHR_RX_H_ */
