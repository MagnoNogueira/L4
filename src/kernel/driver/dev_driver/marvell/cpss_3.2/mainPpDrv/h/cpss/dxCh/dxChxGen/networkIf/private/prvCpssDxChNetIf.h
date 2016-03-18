/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChNetIf.h
*
* DESCRIPTION:    Private DxCh networkIf facility types definitions
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*******************************************************************************/

#ifndef __prvCpssDxChNetIfh
#define __prvCpssDxChNetIfh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfDsaTag.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfMii.h>

/* additional user packet header, this is used to indicate if the packet  */
/* was received tagged or not.                                            */
#define PRV_CPSS_DXCH_ETHPRT_TAGGED_PCKT_FLAG_LEN_CNS    4
#define PRV_CPSS_DXCH_ETHPRT_TAGGED_PCKT_CNS             1
#define PRV_CPSS_DXCH_ETHPRT_UNTAGGED_PCKT_CNS           2
#define PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS            12

/* num of bytes save for each rx packet --
   before the start of the buffer (before the offset of the application)

   used similar to PRV_CPSS_DXCH_ETHPRT_TAGGED_PCKT_FLAG_LEN_CNS
*/
#define PRV_CPSS_DXCH_NET_INTERNAL_RX_PACKET_INFO_SIZE_CNS   4

/* max number of destination devices for CPU destined packets */
#define PRV_CPSS_DXCH_NET_DSTDEVS_NUM_CNS 8 /* first not in use, so really it's 7 */

/* max number of statistical rate limit values for CPU destined packets */
#define PRV_CPSS_DXCH_NET_LIMITS_NUM_CNS 32

/* max number of CPU destined packets opcodes */
#define PRV_CPSS_DXCH_NET_CPUCODES_NUM_CNS 256

/* the offset with in every rx packet that the internal info about the packet
   can be save in */
#define PRV_CPSS_DXCH_RX_OFFSET_TO_INTERNAL_INFO_CNS_MAC(hdrOffset)         \
    (hdrOffset + PRV_CPSS_DXCH_NET_INTERNAL_RX_PACKET_INFO_SIZE_CNS)

/* INOUT GT_U8* buffPtr
   buffPtr -- the pointer to the buffer as it is going to be sent to the
              application

   set tagged -- set in the reserved place that this buffer hold packet that was
                    originally tagged

   set untagged -- set in the reserved place that this buffer hold packet that
                    was originally untagged

   get -- get from the reserved place if the buffer hold packet that was
          originally tagged

   --- comment : use those macros only when this is FIRST buffer . ---
*/
#define PRV_CPSS_DXCH_RX_ORIG_PCKT_TAGGED_SET_MAC(bufPtr, hdrOffset)  \
                (*((bufPtr)-(PRV_CPSS_DXCH_RX_OFFSET_TO_INTERNAL_INFO_CNS_MAC(hdrOffset)))) |= 1

#define PRV_CPSS_DXCH_RX_ORIG_PCKT_UNTAGGED_SET_MAC(bufPtr, hdrOffset) \
                (*((bufPtr)-(PRV_CPSS_DXCH_RX_OFFSET_TO_INTERNAL_INFO_CNS_MAC(hdrOffset)))) &= ~1

#define PRV_CPSS_DXCH_RX_ORIG_PCKT_TAGGED_GET_MAC(bufPtr, hdrOffset)   \
                ((*((bufPtr)-(PRV_CPSS_DXCH_RX_OFFSET_TO_INTERNAL_INFO_CNS_MAC(hdrOffset)))) & 1)


/* size of the extended DSA tag */
#define PRV_CPSS_DXCH_EXTENDED_DSA_TAG_SIZE_CNS         8

/* diff between packet with extended DSA tag
    to regular tagged packet == 4 */
#define PRV_CPSS_DXCH_DIFF_EXT_DSA_TAG_2_TAGGED_CNS \
    (PRV_CPSS_DXCH_EXTENDED_DSA_TAG_SIZE_CNS - PRV_CPSS_DXCH_ETHPRT_TAGGED_PCKT_FLAG_LEN_CNS)


/* diff between packet with extended DSA tag
    to regular untagged packet == 8*/
#define PRV_CPSS_DXCH_DIFF_EXT_DSA_TAG_2_UNTAGGED_CNS \
    (PRV_CPSS_DXCH_EXTENDED_DSA_TAG_SIZE_CNS)

/* 2 byte offset for ip header to be aligned */
#define PRV_CPSS_DXCH_IP_ALIGN_OFFSET_CNS  2

/* check that the networkif of the device was initialized
    return GT_NOT_INITIALIZED if not initialized
*/
#define PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum)                   \
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.initDone == GT_FALSE)            \
        return GT_NOT_INITIALIZED

/*
 * Typedef: PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT
 *
 * Description: Defines the different CPU codes that indicate the reason for
 *              sending a packet to the CPU.
 *
 * Enumerations:
 *
 * TRAP CODES:
 *   PRV_CPSS_DXCH_NET_DSA_TAG_REGULAR_FORWARD_E  - regular forwarding packet
 *   PRV_CPSS_DXCH_NET_DSA_TAG_CONTROL_TO_CPU_E   - reserved
 *   PRV_CPSS_DXCH_NET_DSA_TAG_BPDU_E             - Control BPDU packet.
 *   PRV_CPSS_DXCH_NET_DSA_TAG_MAC_ADDR_TRAP_E    - Source & Destination MAC
 *                  trapped packet.
 *   PRV_CPSS_DXCH_NET_DSA_TAG_MAC_RANGE_TRAP_E   - MAC Range Trapped packet.
 * INTERVENTION CODES:
 *   PRV_CPSS_DXCH_NET_DSA_TAG_ARP_BROADCAST_E    - Intervention ARP
 *   PRV_CPSS_DXCH_NET_DSA_TAG_IGMP_PACKET_E      - Intervention IGMP
 *                  can also be trapped according to VLAN)
 *   PRV_CPSS_DXCH_NET_DSA_TAG_INTERVENTION_MAC_ADDR_E     - Intervention Source
 *                  and Destination addresses
 *   PRV_CPSS_DXCH_NET_DSA_TAG_INTERVENTION_LOCK_TO_CPU_E  - Intervention - Port
 *                  Lock to CPU.
 * MIRRORING CODES:
 *   PRV_CPSS_DXCH_NET_DSA_TAG_MIRROR_MAC_RANGE_TO_CPU_E   - Packet Mirrored to
 *                  CPU because MAC range
 *   PRV_CPSS_DXCH_NET_DSA_TAG_MIRROR_LOCK_TO_CPU_E        - Packet Mirrored to
 *                  CPU because port lock
 * MISC CODES:
 *   PRV_CPSS_DXCH_NET_DSA_TAG_UNKNOWN_UNICAST_E        - Unknown unicast packets.
 *   PRV_CPSS_DXCH_NET_DSA_TAG_UNREGISTERED_MULTICAST_E - Unregistered multicast packet.
 *
 *  CHEETAH used additional Codes:
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_E - Packet with MAC
 *                      DA in the IEEE Reserved Multicast range trapped or
 *                      mirrored to the host CPU.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_ICMP_PACKET_E        - IPv6 ICMP packet
 *                      trapped or mirrored to the host CPU.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_DSA_EXTENDED_TAG_TO_CPU_E - Packet with extended
 *                      tag trapped or mirrored to the host CPU.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E - IPv4/ Pv6
 *                      Link Local MC DIP packet trapped or mirrored to the host CPU.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_MIRROR_IPV4_RIPV1_TO_CPU_E   - IPv4 RIPv1
 *                      packet mirrored to the host CPU.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_NEIGHBOR_SOLICITATION_E - IPv6 neighbor
 *                      solicitation packet trapped or mirrored to the host CPU.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_BROADCAST_PACKET_E   - IPv4 Broadcast packet assigned to a VLAN with
 *                                         IPv4 Broadcast Command set to Trap or Mirror.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_NON_IPV4_BROADCAST_PACKET_E - Non IPv4 Broadcast packet assigned to a VLAN
 *                                         with non IPv4 Broadcast Command set to Trap
 *                                         or Mirror.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_CISCO_MCAST_MAC_RANGE_E - A Multicast packet with a MAC DA in the CISCO
 *                                         AUI range trapped or mirrored to the host CPU.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_BRG_UNREGD_MCAST_E - Unregistered Multicast packet assigned
 *                                         to a VLAN with Unregistered Multicast Command set to Trap or Mirror.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_BRG_IPV4_UNREG_MCAST_E  - IPv4 Unregistered Multicast packet assigned
 *                                         to a VLAN with IPv4 Unregistered Multicast
 *                                         Command set to Trap or Mirror.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_BRG_IPV6_UNREG_MCAST_E  - IPv6 Unregistered Multicast packet assigned
 *                                         to a VLAN with IPv6 Unregistered Multicast
 *                                         Command set to Trap or Mirror.
 *
 *      PRV_CPSS_DXCH_NET_DSA_TAG_UNKNOWN_UNICAST_EXT_E   - unknown unicast that where trapped/mirrored
 *                                       to cpu due to vlan command
 *
 *      PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP0_E  - The UDP BC packet matches one of entries
 *                                       in UDP relay table and its CPU code index is 0
 *      PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP1_E  - The UDP BC packet matches one of entries
 *                                       in UDP relay table and its CPU code index is 1
 *      PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP2_E  - The UDP BC packet matches one of entries
 *                                       in UDP relay table and its CPU code index is 2
 *      PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP3_E  - The UDP BC packet matches one of entries
 *                                       in UDP relay table and its CPU code index is 3
 *
 *
 *      PRV_CPSS_DXCH_NET_DSA_TAG_ROUTED_PACKET_FORWARD_E - packet forwarded to the host CPU by the
 *                                       router ("IP to me")
 *      PRV_CPSS_DXCH_NET_DSA_TAG_BRIDGED_PACKET_FRW_E    - Packet forwarded to the host CPU by one of
 *                                         the following engines in the device:
 *                                         1. Redirected by the PCL engine to the CPU
 *                                            port number.
 *                                         2. MAC DA is associated with the CPU port
 *                                            number.
 *                                         3. Private VLAN edge target set to CPU port
 *                                            number.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_INGRESS_MIRRORED_TO_ANALYZER_E - Ingress mirrored packets to the analyzer port,
 *                                         when the ingress analyzer port number is set
 *                                         to the CPU port number.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_EGRESS_MIRRORED_TO_ANALYZER_E - Egress mirrored packets to the analyzer port,
 *                                         when the egress analyzer port number is set
 *                                         to the CPU port number.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_MAIL_FROM_NEIGHBOR_CPU_E  - A packet sent to the local host CPU as a mail
 *                                         from the neighboring CPU.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_CPU_TO_CPU_E              - A packet sent to the local host CPU, from one
 *                                         of the other host CPUs in the system.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_EGRESS_SAMPLED_E          - Packet mirrored to the host CPU by the egress
 *                                         STC mechanism.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_INGRESS_SAMPLED_E         - Packet mirrored to the host CPU by the ingress
 *                                         STC mechanism.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_INVALID_PCL_KEY_TRAP_E    - Packet trapped/mirrored to the host CPU by the
 *                                         Policy Engine due to the following:
 *                                         User-defined bytes in the key could not be
 *                                         parsed due to packet header length or its
 *                                         format.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_TT_HEADER_ERROR_E    - Tunnel-terminated packet is trapped to the CPU
 *                                         due to an IPV4 tunnel header error.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_TT_OPTION_FRAG_ERROR_E - Tunnel-terminated packet is trapped to the CPU
 *                                         due to the IPV4 tunnel header, which is
 *                                         fragmented or has options.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_TT_UNSUP_GRE_ERROR_E - Tunnel-terminated packet is trapped to the CPU
 *                                         due to an unsupported GRE tunnel header format.
 *
 *      PRV_CPSS_DXCH_NET_DSA_TAG_TT_MPLS_HEADER_CHECK_E - Tunnel-terminated MPLS packet is trapped to the CPU
 *                                         due to an illegal MPLS header or MPLS command in action entry.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_TT_MPLS_TTL_EXCEED_E - Tunnel-terminated MPLS packet is trapped to the CPU
 *                                         due to an assigned TTL of ZERO.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_OAM_PDU_TRAP_E  - An OAM-PDU packet is trappedto the CPU
 *
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UDP_E
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_BAD_DIP_SIP_E
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_BAD_SIP_DIP_E
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_BAD_DIP_SIP_E
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_BAD_SIP_DIP_E
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_TTL_EXCEEDED_E    - IPv4 packet, triggered for routing Trapped/Mirrored
 *                                         to the CPU due to TTL exceeded.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_MTU_EXCEEDED_E     - IPv4/6 packet, triggered for routing Trapped/Mirrored
 *                                         to the CPU due packet�s length longer than Route entry <MTU Index>.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_HOP_LIMIT_EXCEEDED_E - IPv6 packet, triggered for routing Trapped/Mirrored
 *                                            to the CPU due to Hop Limit exceeded.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_ILLEGAL_ADDR_ERROR_E - IPv4/6 packet, triggered for routing, is Trapped/Mirrored
 *                                           due to an illegal IPv4/6 destination or source address.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_HEADER_ERROR_E    -  IPv4 packet, triggered for routing Trapped/Mirrored
 *                                          to the CPU due to IPv4 header Error
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_DIP_DA_MISMATCH_E  - IPv4/6 packet, triggered for routing, is Trapped/Mirrored
 *                                         due to a mismatch between the destination IP address
 *                                         and the destination MAC address.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_HEADER_ERROR_E    - IPv6 packet, triggered for routing Trapped/Mirrored
 *                                         to the CPU due to IPv6 header Error.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_UC_SIP_SA_MISMATCH_E - IPv4/6 Unicast packet, triggered for routing,
 *                                           is Trapped/Mirrored due to a mismatch between
 *                                           the source IP address and its configured ARP MAC address.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_OPTIONS_E         - IPv4 packet, triggered for routing Trapped/Mirrored
 *                                         to the CPU due to IPv4 header Options.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_NON_HBH_OPTION_E     - IPv6 packet, triggered for routing, Trapped/Mirrored
 *                                         due to one of the following non hop-by-hop header
 *                                         extension options: Routing, Fragmentation,
 *                                         Encapsulating Security Payload, Authentication, Destination.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_OPTIONS_E         - IPv6 packet, triggered for routing Trapped/Mirrored
 *                                         to the CPU due to IPv6 header Hop-By-Hop Options.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_SCOPE_E              - IPv6 packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU due to IPv6 Scope checking.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE0_E          - IPv4 Unicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 0.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE1_E          - IPv4 Unicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 1.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE2_E          - IPv4 Unicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 2.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE3_E          - IPv4 Unicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 3.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE0_E          - IPv4 Multicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 0.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE1_E          - IPv4 Multicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 1
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE2_E          - IPv4 Multicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 2.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE3_E          - IPv4 Multicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 3.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE0_E          - IPv6 Unicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 0.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE1_E          - IPv6 Unicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 1.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE2_E          - IPv6 Unicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 2.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE3_E          - IPv6 Unicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 3.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE0_E          - IPv6 Multicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 0.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE1_E          - IPv6 Multicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 1.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE2_E          - IPv6 Multicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route entry with <CPUCodeIndex> = 2.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE3_E          - IPv6 Multicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU from Route Entry with <CPUCodeIndex> = 3.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_UC_RPF_FAIL_E      - IPv4/6 Unicast packet, triggered for routing, is Trapped/Mirrored
 *                                         due to Unicast RPF Check failure.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_MC_ROUTE_RPF_FAIL_E - IPv4/6 Multicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU Due to Route entry Multicast RPF Fail command.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_MC_MLL_RPF_FAIL_E  - IPv4/6 Multicast packet, triggered for routing, Trapped/Mirrored
 *                                         to CPU Due to MLL entry Multicast RPF Fail command.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_ARP_BC_TO_ME_E            - ARP Broadcast packet, whose destination IP address is the host
 *                                         address of the router, as indicated in the
 *                                         Route entry <Trap/Mirror ARP BC to CPU>.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ICMP_REDIRECT_E   - IPv4 Unicast packet, triggered for routing, Mirrored
 *                                         to CPU due to ICMP redirect.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ICMP_REDIRECT_E   - IPv6 Unicast packet, triggered for routing, Mirrored
 *                                         to CPU due to ICMP redirect.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_ARP_REPLY_TO_ME_E         - The packet is an ARP reply, and forwarded by the FDB
 *                                         entry with <destination port> = CPU port 63, and <application specific CPU code> = 1.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_CPU_TO_ALL_CPUS_E         - The packet is sent with a FROM_CPU DSA tag with
 *                                         the <mirror2CPU> bit set and this mirrored to the local CPU.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_TCP_SYN_TO_CPU_E          - The packet is Trapped, Mirrored, forwarded to the
 *                                         CPU port 63, or forwarded to the Virtual Router port 63,
 *                                         and the packet is a TCP SYN packet.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_PACKET_TO_VIRTUAL_ROUTER_PORT_E - The packet is
 *                                          sent  to virtual router port
 *      PRV_CPSS_DXCH_NET_DSA_TAG_FIRST_USER_DEFINED_E      - User-Defined CPU codes. The user may use this
 *                                         range for any application specific purpose.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_LAST_USER_DEFINED_E       - User-Defined CPU codes. The user may use this
 *                                         range for any application specific purpose.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_1_E - CPU Code=1 for Ieee Reserved Multicast Addrs.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_2_E - CPU Code=2 for Ieee Reserved Multicast Addrs.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_3_E - CPU Code=3 for Ieee Reserved Multicast Addrs.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E - Ipv4/Ipv6 link local multicast CPU Code index=1
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E - Ipv4/Ipv6 link local multicast CPU Code index=2
 *      PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E - Ipv4/Ipv6 link local multicast CPU Code index=3
 *      PRV_CPSS_DXCH_NET_DSA_TAG_SEC_AUTO_LEARN_UNK_SRC_TRAP_E - In Secure Automatic learning,an uknown source MAC
 *                                             is trapped to CPU.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_CPU_TO_ALL_CPUS_E             - The packet is sent witha FROM_CPU DSA Tag with
 *                                             the <mirror2CPU> bit set and thus mirrored to a
 *                                             local CPU.
 *      PRV_CPSS_DXCH_NET_DSA_TAG_MAX_E       - one more then the last
 *
 */
typedef enum
{
    PRV_CPSS_DXCH_NET_DSA_TAG_REGULAR_FORWARD_E                         = 0,
    PRV_CPSS_DXCH_NET_DSA_TAG_CONTROL_TO_CPU_E                          = 1,
    PRV_CPSS_DXCH_NET_DSA_TAG_BPDU_E                                    = 2,
    PRV_CPSS_DXCH_NET_DSA_TAG_MAC_ADDR_TRAP_E                           = 3,
    PRV_CPSS_DXCH_NET_DSA_TAG_MAC_RANGE_TRAP_E                          = 4,
    PRV_CPSS_DXCH_NET_DSA_TAG_ARP_BROADCAST_E                           = 5,
    PRV_CPSS_DXCH_NET_DSA_TAG_IGMP_PACKET_E                             = 6,
    PRV_CPSS_DXCH_NET_DSA_TAG_INTERVENTION_MAC_ADDR_E                   = 7,
    PRV_CPSS_DXCH_NET_DSA_TAG_INTERVENTION_LOCK_TO_CPU_E                = 8,
    PRV_CPSS_DXCH_NET_DSA_TAG_MIRROR_MAC_RANGE_TO_CPU_E                 = 9,
    PRV_CPSS_DXCH_NET_DSA_TAG_MIRROR_LOCK_TO_CPU_E                      = 10,
    /* cannot be generated by DxCh devices only using IPCL Action
       CPU code assignment */
    PRV_CPSS_DXCH_NET_DSA_TAG_UNKNOWN_UNICAST_E                         = 11,
    PRV_CPSS_DXCH_NET_DSA_TAG_UNREGISTERED_MULTICAST_E                  = 12,
    /* addition values for CHEETAH use only */
    PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_E               = 13,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_ICMP_PACKET_E                        = 14,
    PRV_CPSS_DXCH_NET_DSA_TAG_DSA_EXTENDED_TAG_TO_CPU_E                 = 15,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E     = 16,
    PRV_CPSS_DXCH_NET_DSA_TAG_MIRROR_IPV4_RIPV1_TO_CPU_E                = 17,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_NEIGHBOR_SOLICITATION_E              = 18,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_BROADCAST_PACKET_E                   = 19,
    PRV_CPSS_DXCH_NET_DSA_TAG_NON_IPV4_BROADCAST_PACKET_E               = 20,
    PRV_CPSS_DXCH_NET_DSA_TAG_CISCO_MCAST_MAC_RANGE_E                   = 21,
    PRV_CPSS_DXCH_NET_DSA_TAG_BRG_UNREGD_MCAST_E                        = 22,
    PRV_CPSS_DXCH_NET_DSA_TAG_BRG_IPV4_UNREG_MCAST_E                    = 23,
    PRV_CPSS_DXCH_NET_DSA_TAG_BRG_IPV6_UNREG_MCAST_E                    = 24,
    PRV_CPSS_DXCH_NET_DSA_TAG_UNKNOWN_UNICAST_EXT_E                     = 25,
    PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_1_E             = 26,
    PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_2_E             = 27,
    PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_3_E             = 28,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E   = 29,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E   = 30,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E   = 31,

    PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP0_E                     = 32,
    PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP1_E                     = 33,
    PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP2_E                     = 34,
    PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP3_E                     = 35,
    PRV_CPSS_DXCH_NET_DSA_TAG_SEC_AUTO_LEARN_UNK_SRC_TRAP_E             = 36,



    PRV_CPSS_DXCH_NET_DSA_TAG_ROUTED_PACKET_FORWARD_E                   = 64,
    PRV_CPSS_DXCH_NET_DSA_TAG_BRIDGED_PACKET_FRW_E                      = 65,
    PRV_CPSS_DXCH_NET_DSA_TAG_INGRESS_MIRRORED_TO_ANALYZER_E            = 66,
    PRV_CPSS_DXCH_NET_DSA_TAG_EGRESS_MIRRORED_TO_ANALYZER_E             = 67,
    PRV_CPSS_DXCH_NET_DSA_TAG_MAIL_FROM_NEIGHBOR_CPU_E                  = 68,
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_TO_CPU_E                              = 69,
    PRV_CPSS_DXCH_NET_DSA_TAG_EGRESS_SAMPLED_E                          = 70,
    PRV_CPSS_DXCH_NET_DSA_TAG_INGRESS_SAMPLED_E                         = 71,
    PRV_CPSS_DXCH_NET_DSA_TAG_INVALID_PCL_KEY_TRAP_E                    = 74,

    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_TT_HEADER_ERROR_E                    = 75,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_TT_OPTION_FRAG_ERROR_E               = 76,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_TT_UNSUP_GRE_ERROR_E                 = 77,

    PRV_CPSS_DXCH_NET_DSA_TAG_TT_MPLS_HEADER_CHECK_E                    = 79,
    PRV_CPSS_DXCH_NET_DSA_TAG_TT_MPLS_TTL_EXCEED_E                      = 80,
    PRV_CPSS_DXCH_NET_DSA_TAG_OAM_PDU_TRAP_E                            = 83,

    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_TTL_EXCEEDED_E                    = 133,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_MTU_EXCEEDED_E                     = 134,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_HOP_LIMIT_EXCEEDED_E              = 135,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_ILLEGAL_ADDR_ERROR_E               = 136,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_HEADER_ERROR_E                    = 137,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_DIP_DA_MISMATCH_E                  = 138,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_HEADER_ERROR_E                    = 139,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_UC_SIP_SA_MISMATCH_E               = 140,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_OPTIONS_E                         = 141,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_NON_HBH_OPTION_E                     = 142,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_OPTIONS_E                         = 143,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_SCOPE_E                              = 159,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE0_E                          = 160,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE1_E                          = 161,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE2_E                          = 162,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE3_E                          = 163,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE0_E                          = 164,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE1_E                          = 165,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE2_E                          = 166,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE3_E                          = 167,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE0_E                          = 168,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE1_E                          = 169,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE2_E                          = 170,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE3_E                          = 171,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE0_E                          = 172,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE1_E                          = 173,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE2_E                          = 174,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE3_E                          = 175,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_UC_RPF_FAIL_E                      = 176,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_MC_ROUTE_RPF_FAIL_E                = 177,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_MC_MLL_RPF_FAIL_E                  = 178,
    PRV_CPSS_DXCH_NET_DSA_TAG_ARP_BC_TO_ME_E                            = 179,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ICMP_REDIRECT_E                   = 180,
    PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ICMP_REDIRECT_E                   = 181,

    PRV_CPSS_DXCH_NET_DSA_TAG_ARP_REPLY_TO_ME_E                         = 188,
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_TO_ALL_CPUS_E                         = 189,
    PRV_CPSS_DXCH_NET_DSA_TAG_TCP_SYN_TO_CPU_E                          = 190,

    PRV_CPSS_DXCH_NET_DSA_TAG_PACKET_TO_VIRTUAL_ROUTER_PORT_E           = 191,
    PRV_CPSS_DXCH_NET_DSA_TAG_FIRST_USER_DEFINED_E                      = 192,
    PRV_CPSS_DXCH_NET_DSA_TAG_LAST_USER_DEFINED_E               = PRV_CPSS_DXCH_NET_DSA_TAG_FIRST_USER_DEFINED_E + 63,


    PRV_CPSS_DXCH_NET_DSA_TAG_MAX_E                              /* one more then the last */
} PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT;

/*
 * Typedef: struct PRV_CPSS_DXCH_NET_INFO_STC
 *
 * Description: A structure to hold all PP data needed on networkIf DXCH layer
 *
 * Fields:
 *     initDone - was the initalization of the networkIf done.
 *                it is needed for 'restart after shutdown' with out HW reset.
 *                because we need to synchronize the PP with the SW regarding
 *                the Rx,Tx descriptors , buffers.
 *     prePendTwoBytesHeader - Enables/Disable pre-pending a two-byte
 *                             header to all packets forwarded to the CPU
 *                             (via the CPU port or the PCI interface).
 *                             This two-byte header is used to align the
 *                             IPv4 header to 32 bits.
 *                             GT_TRUE  - Two-byte header is pre-pended to
 *                                        packets forwarded to the CPU.
 *                             GT_FALSE - Two-byte header is NOT pre-pended
 *                                        to packets forwarded to the CPU.
 *
 */
typedef struct{
    GT_BOOL          initDone;
    GT_BOOL          prePendTwoBytesHeader;
}PRV_CPSS_DXCH_NET_INFO_STC;

/*******************************************************************************
* prvCpssDxChNetIfCpuToDsaCode
*
* DESCRIPTION:
*       Convert the CPU code to the DSA CPU code.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       cpuCode      - CPU code.
*
* OUTPUTS:
*       dsaCpuCodePtr - DSA CPU code
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - None DSA code for this CPU code.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not all cpu codes translation implemented
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfCpuToDsaCode
(
    IN  CPSS_NET_RX_CPU_CODE_ENT    cpuCode,
    OUT PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT *dsaCpuCodePtr
);

/*******************************************************************************
* prvCpssDxChNetIfDsaToCpuCode
*
* DESCRIPTION:
*       Convert the DSA CPU code to the CPU code.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dsaCpuCode - DSA CPU code
*
* OUTPUTS:
*       cpuCodePtr - CPU code.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - None DSA code for this CPU code.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not all cpu codes translation implemented
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfDsaToCpuCode
(
    IN  PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT   dsaCpuCode,
    OUT CPSS_NET_RX_CPU_CODE_ENT    *cpuCodePtr
);


/*******************************************************************************
* prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle
*
* DESCRIPTION:
*       This routine frees all transmitted packets descriptors. In addition, all
*       user relevant data in Tx End FIFO.  -- SDMA relate.
*
* APPLICABLE DEVICES: All DxCh devices exclude DxCh1_Diamond
*
* INPUTS:
*       devNum      - The device number the packet was transmitted from
*       queueIdx    - The queue the packet was transmitted upon.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - the CPSS does not handle the ISR for the device
*                                  so function must not be called
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Invoked by ISR routine !!
*       the function is called internally in the CPSS from the driver as a call
*       back , see bind in function hwPpPhase1Part1(...)
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle
(
    IN GT_U8                devNum,
    IN GT_U8                queueIdx
);

/*******************************************************************************
* prvCpssDxChNetIfMiiInitNetIfDev
*
* DESCRIPTION:
*       Initialize the network interface structures, Rx buffers and Tx header
*       buffers (For a single device).
*
* INPUTS:
*       devNum      - device number
*       miiInitPtr  - MII Init parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfMiiInitNetIfDev
(
    IN GT_U8 devNum,
    IN CPSS_DXCH_NETIF_MII_INIT_STC *miiInitPtr
);

GT_STATUS prvCpssDxChNetIfMiiPacketTx
(
    IN GT_U8                devNum,
    IN CPSS_GEN_NETIF_MII_TX_PARAMS_STC *pcktParams,
    IN GT_U8              * packetBuffs[],
    IN GT_U32               buffLen[],
    IN GT_U32               numOfBufs
);

GT_STATUS prvCpssDxChNetIfMiiRxGet
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    INOUT GT_U32                            *numOfBuffPtr,
    OUT GT_U8                               *packetBuffs[],
    OUT GT_U32                              buffLen[],
    OUT CPSS_GEN_NETIF_MII_RX_PARAMS_STC    *rxParamsPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __prvCpssDxChNetIfh */

