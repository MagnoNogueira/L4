/*
 * $Id: rx.h,v 1.6 Broadcom SDK $
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
 */

#ifndef _SOC_RX_H
#define _SOC_RX_H

#include <sal/types.h>

#include <shared/rx.h>

/*
 * PKT RX Packet Reasons; reason CPU received the packet.
 * Notes:
 *      It is possible no reasons are set (directed to CPU from ARL
 *      for example), or multiple reasons may be set.
 *
 * See "include/shared/rx.h" for full description.
 */

typedef enum soc_rx_reason_e {
    socRxReasonInvalid               = _SHR_RX_INVALID,
    socRxReasonBroadcast             = _SHR_RX_BROADCAST,
    socRxReasonClassBasedMove        = _SHR_RX_CLASS_BASED_MOVE,
    socRxReasonControl               = _SHR_RX_CONTROL,
    socRxReasonCpuLearn              = _SHR_RX_CPU_LEARN,
    socRxReasonDestLookupFail        = _SHR_RX_DEST_LOOKUP_FAIL,
    socRxReasonDosAttack             = _SHR_RX_DOS_ATTACK,
    socRxReasonFilterMatch           = _SHR_RX_FILTER_MATCH,
    socRxReasonGreChecksum           = _SHR_RX_GRE_CHECKSUM,
    socRxReasonGreSourceRoute        = _SHR_RX_GRE_SOURCE_ROUTE,
    socRxReasonHigigHdrError         = _SHR_RX_HIGIG_HDR_ERROR,
    socRxReasonIcmpRedirect          = _SHR_RX_ICMP_REDIRECT,
    socRxReasonIgmp                  = _SHR_RX_IGMP,
    socRxReasonIngressFilter         = _SHR_RX_INGRESS_FILTER,
    socRxReasonIp                    = _SHR_RX_IP,
    socRxReasonIpMcastMiss           = _SHR_RX_IP_MCAST_MISS,
    socRxReasonIpOptionVersion       = _SHR_RX_IP_OPTION_VERSION,
    socRxReasonIpmc                  = _SHR_RX_IPMC,
    socRxReasonL2Cpu                 = _SHR_RX_L2_CPU,
    socRxReasonL2DestMiss            = _SHR_RX_L2_DEST_MISS,
    socRxReasonL2LearnLimit          = _SHR_RX_L2_LEARN_LIMIT,
    socRxReasonL2Move                = _SHR_RX_L2_MOVE,
    socRxReasonL2MtuFail             = _SHR_RX_L2_MTU_FAIL,
    socRxReasonL2NonUnicastMiss      = _SHR_RX_L2_NON_UNICAST_MISS,
    socRxReasonL2SourceMiss          = _SHR_RX_L2_SOURCE_MISS,
    socRxReasonL3DestMiss            = _SHR_RX_L3_DEST_MISS,
    socRxReasonL3HeaderError         = _SHR_RX_L3_HEADER_ERROR,
    socRxReasonL3MtuFail             = _SHR_RX_L3_MTU_FAIL,
    socRxReasonL3Slowpath            = _SHR_RX_L3_SLOW_PATH,
    socRxReasonL3SourceMiss          = _SHR_RX_L3_SOURCE_MISS,
    socRxReasonL3SourceMove          = _SHR_RX_L3_SOUCE_MOVE,
    socRxReasonMartianAddr           = _SHR_RX_MARTIAN_ADDR,
    socRxReasonMcastIdxError         = _SHR_RX_MCAST_IDX_ERROR,
    socRxReasonMcastMiss             = _SHR_RX_MCAST_MISS,
    socRxReasonMplsError             = _SHR_RX_MPLS_ERROR,
    socRxReasonMplsInvalidAction     = _SHR_RX_MPLS_INVALID_ACTION,
    socRxReasonMplsInvalidPayload    = _SHR_RX_MPLS_INVALID_PAYLOAD,
    socRxReasonMplsLabelMiss         = _SHR_RX_MPLS_LABEL_MISS,
    socRxReasonMplsSequenceNumber    = _SHR_RX_MPLS_SEQUENCE_NUMBER,
    socRxReasonMplsTtl               = _SHR_RX_MPLS_TTL,
    socRxReasonMulticast             = _SHR_RX_MULTICAST,
    socRxReasonNhop                  = _SHR_RX_NHOP,
    socRxReasonParityError           = _SHR_RX_PARITY_ERROR,
    socRxReasonProtocol              = _SHR_RX_PROTOCOL,
    socRxReasonSampleDest            = _SHR_RX_SAMPLE_DEST,
    socRxReasonSampleSource          = _SHR_RX_SAMPLE_SOURCE,
    socRxReasonSourceRoute           = _SHR_RX_SOURCE_ROUTE,
    socRxReasonTtl                   = _SHR_RX_TTL,
    socRxReasonTunnelError           = _SHR_RX_TUNNEL_ERROR,
    socRxReasonUdpChecksum           = _SHR_RX_UDP_CHECKSUM,
    socRxReasonUnknownVlan           = _SHR_RX_UNKNOWN_VLAN,
    socRxReasonVcLabelMiss           = _SHR_RX_VC_LABEL_MISS,
    socRxReasonVlanFilterMatch       = _SHR_RX_VLAN_FILTER_MATCH,
    socRxReasonExceptionFlood        = _SHR_RX_EXCEPTION_FLOOD,
    socRxReasonTimeSync              = _SHR_RX_TIMESYNC,
    socRxReasonEAVData               = _SHR_RX_EAV_DATA,
    
    socRxReasonCount                 = _SHR_RX_REASON_COUNT /* MUST BE LAST */
} soc_rx_reason_t;

/*
 * Set of "reasons" (see socRxReason*) why a packet came to the CPU.
 */
typedef _shr_rx_reasons_t soc_rx_reasons_t;

#define SOC_RX_REASON_NAMES_INITIALIZER _SHR_RX_REASON_NAMES_INITIALIZER

/*
 * Macro to check if a reason (socRxReason*) is included in a
 * set of reasons (soc_rx_reasons_t). Returns:
 *   zero     => reason is not included in the set
 *   non-zero => reason is included in the set
 */
#define SOC_RX_REASON_GET(_reasons, _reason) \
       _SHR_RX_REASON_GET(_reasons, _reason)

/*
 * Macro to add a reason (socRxReason*) to a set of
 * reasons (soc_rx_reasons_t)
 */
#define SOC_RX_REASON_SET(_reasons, _reason) \
       _SHR_RX_REASON_SET(_reasons, _reason)

/*
 * Macro to add all reasons (socRxReason*) to a set of
 * reasons (soc_rx_reasons_t)
 */
#define SOC_RX_REASON_SET_ALL(_reasons) \
       _SHR_RX_REASON_SET_ALL(_reasons)

/*
 * Macro to clear a reason (socRxReason*) from a set of
 * reasons (soc_rx_reasons_t)
 */
#define SOC_RX_REASON_CLEAR(_reasons, _reason) \
       _SHR_RX_REASON_CLEAR(_reasons, _reason)

/*
 * Macro to clear a set of reasons (soc_rx_reasons_t).
 */
#define SOC_RX_REASON_CLEAR_ALL(_reasons) \
       _SHR_RX_REASON_CLEAR_ALL(_reasons)

#endif  /* !_SOC_RX_H */
