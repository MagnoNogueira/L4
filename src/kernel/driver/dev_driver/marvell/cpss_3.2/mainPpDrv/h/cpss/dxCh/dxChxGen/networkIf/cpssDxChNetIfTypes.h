/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNetIfTypes.h
*
* DESCRIPTION:    DxCh networkIf facility types definitions
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#ifndef __cpssDxChGenNetIfTypesh
#define __cpssDxChGenNetIfTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/generic/cos/cpssCosTypes.h>

/*
 * typedef: enum  CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC
 *
 * Description: parameter of the "cpu code table" entry
 *
 * Fields:
 *      tc                  - The Traffic Class assigned to packets with this
 *                            CPU code.
 *      dp                  - The Drop Precedence assigned to packets with this
 *                            CPU code.
 *      truncate            - Enables/disable the truncation of packets assigned
 *                            with this CPU code to up to 128 bytes.
 *                            GT_FALSE - don't truncate packets assigned with
 *                            this CPU code.
 *                            GT_TRUE  - truncate packets assigned with this
 *                            CPU code to up to 128 bytes.
 *      cpuRateLimitMode - Rate Limiting mode
 *                            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E =
 *                            Rate Limiting is done on packets forwarded to the
 *                            CPU by the local device ONLY.
 *                            CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E =
 *                            Rate Limiting is done on all packets forwarded to
 *                            the CPU.
 *      cpuCodeRateLimiterIndex - CPU Code Rate Limiter index
 *                            The index to one of the 31 rate limiters
 *                            0 = This CPU Code is not bonded to any Rate
 *                            Limiter, thus the rate of packets with this CPU
 *                            code is not limited.
 *                            1-31 = CPU Code is bonded to a Rate limiter
 *                            according to this index
 *                            see API :
 *                            cpssDxChNetIfCpuCodeRateLimiterParamsSet(...)
 *
 *      cpuCodeStatRateLimitIndex - Index of one of the 32 statistical rate
 *                            limiters for this CPU Code .
 *                            0-31 = CPU Code is bonded to a statistical Rate
 *                            limiter according to this index
 *                            see API :
 *                            cpssDxChNetIfNetStatisticalRateLimitsTableSet(...)
 *      designatedDevNumIndex - The target device index for packets with this
 *                              CPU code.
 *                              0 = Packets with this CPU code are forwarded to
 *                                  the CPU attached to the local device.
 *                              1-7 = Packets with this CPU code are forwarded
 *                                    to CPU attached to the device indexed by
 *                                    this field
 *                              see API :
 *                              cpssDxChNetIfNetDesignatedDeviceTableSet(...)
 */
typedef struct
{
    GT_U8                       tc;
    CPSS_DP_LEVEL_ENT           dp;
    GT_BOOL                     truncate;
    CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT  cpuRateLimitMode;
    GT_U32                      cpuCodeRateLimiterIndex;
    GT_U32                      cpuCodeStatRateLimitIndex;
    GT_U32                      designatedDevNumIndex;
} CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC;

/*
 * Typedef: struct CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC
 *
 * Description: information about Rx counters -- per Queue (traffic class)
 *
 * Fields:
 *     rxInPkts   - Packets received on this queue since last read
 *     rxInOctets - Octets received on this queue since last read
 *
 */
typedef struct{
    GT_U32    rxInPkts;
    GT_U32    rxInOctets;
}CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC;

/*
 * typedef: enum CPSS_DXCH_NET_DSA_TYPE_ENT
 *
 * Description: Enumeration of DSA tag types
 *
 * Enumerations:
 *    CPSS_DXCH_NET_DSA_TYPE_REGULAR_E  - regular DSA tag(single word - 4 bytes)
 *    CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E - extended DSA tag(two words - 8 bytes)
 *
 */
typedef enum
{
    CPSS_DXCH_NET_DSA_TYPE_REGULAR_E,
    CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E
}CPSS_DXCH_NET_DSA_TYPE_ENT;


/*
 * typedef: enum CPSS_DXCH_NET_DSA_CMD_ENT
 *
 * Description: Enumeration of DSA tag commands
 *
 * Enumerations:
 *    CPSS_DXCH_NET_DSA_CMD_TO_CPU_E  - DSA command is "To CPU"
 *    CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E  - DSA command is "FROM CPU"
 *    CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E - DSA command is "TO Analyzer"
 *    CPSS_DXCH_NET_DSA_CMD_FORWARD_E  - DSA command is "FORWARD"
 *
 */
typedef enum
{
    CPSS_DXCH_NET_DSA_CMD_TO_CPU_E = 0 ,
    CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E    ,
    CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E ,
    CPSS_DXCH_NET_DSA_CMD_FORWARD_E
} CPSS_DXCH_NET_DSA_CMD_ENT;

/*
 * typedef: structure CPSS_DXCH_NET_DSA_COMMON_STC
 *
 * Description: structure of common parameters in all DSA tag formats
 *
 *  dsaTagType - DSA type regular/extended
 *
 *  vpt - the value vlan priority tag (0..7)
 *  cfiBit - CFI bit of the vlan tag  (0..1)
 *  vid - the value of vlan id (0..4095)
 *  dropOnSource - Drop packet on its source device indicator,
 *                 for Fast Stack Fail over Recovery Support.
 *                 GT_TRUE: If packet is looped (packetIsLooped = 1)and
 *                          SrcDev = OwnDev then packet is dropped
 *                 GT_FALSE: If packet is looped and SrcDev = OwnDev, the
 *                           packet is not dropped.
 *                 (relevant only for CH3 devices)
 *  packetIsLooped - Packet is looped indicator, for Fast Stack Fail
 *                   over Recovery Support.
 *                  GT_FALSE: Packet is not looped and is being routed via its
 *                            designated route in the Stack.
 *                  GT_TRUE: Packet is looped and its being routed on an
 *                           alternative route in the Stack.
 *                 (relevant only for CH3 devices).
 *
 */
typedef struct{
    CPSS_DXCH_NET_DSA_TYPE_ENT      dsaTagType;

    GT_U8                           vpt;
    GT_U8                           cfiBit;
    GT_U16                          vid;
    GT_BOOL                         dropOnSource;
    GT_BOOL                         packetIsLooped;
}CPSS_DXCH_NET_DSA_COMMON_STC;

/*
 * typedef: structure CPSS_DXCH_NET_DSA_FROM_CPU_STC
 *
 * Description: structure of "from CPU" DSA tag parameters
 *
 * fields:
 *      dstInterface  - Packet's destination interface:
 *         type == CPSS_INTERFACE_TRUNK_E --> not supported !
 *         when type == CPSS_INTERFACE_PORT_E
 *              devPort.tgtDev - the target device that packet will be send to.
 *                               NOTE : HW device number !!
 *              devPort.tgtPort- the target port on device that packet will be
 *                             send to.
 *         when type == CPSS_INTERFACE_VIDX_E
 *              vidx     - the vidx group that packet will be sent to.
 *         when type == CPSS_INTERFACE_VID_E
 *              vlanId   - must be equal to commonParams.vid !!!
 *
 *      tc - the traffic class for the transmitted packet
 *
 *      dp  - the drop precedence for the transmitted packet
 *            For DxCh3 and above devices the field is not relevant, and the dp is 
 *            globally set using the cpssDxChNetIfFromCpuDpSet API.
 *
 *      egrFilterEn - Egress filtering enable.
 *                  GT_FALSE - Packets from the CPU are not egress filtered.
 *                             Unicast packets are forwarded regardless of the
 *                             Egress port Span State or VLAN Membership.
 *                             Multi-destination packets are forwarded to the
 *                             Multicast group members specified in this tag,
 *                             regardless of the target port's Span state.
 *                  GT_TRUE -  Packets from CPU are Egress filtered.
 *
 *      cascadeControl - This field indicates which TC is assigned to the packet
 *                       when it is forwarded over a cascading/stacking port.
 *                  GT_FALSE - If the packet is queued on a port that is enabled
 *                             for Data QoS mapping (typically a cascade port),
 *                             the packet is queued according to the data
 *                             traffic {TC, DP} mapping table, which maps the
 *                             DSA tag TC and DP to a cascade port TC and DP.
 *                             On a port that is disabled for Data QoS mapping (
 *                             a non-cascade ports), the packet is queued
 *                             according to the DSA tag TC and DP.
 *                   GT_TRUE - If the packet is queued on a port that is enabled
 *                             for Control QoS mapping (typically a cascade
 *                             port), the packet is queued according to the
 *                             configured Control TC and DP. On a port that is
 *                             enabled for Control QoS mapping (non-cascade
 *                             ports), the packet is queued according to the DSA
 *                             tag TC and DP.
 *
 *      egrFilterRegistered -  Indicates that the packet is Egress filtered as a
 *                             registered packet.
 *                  GT_FALSE - Packet is egress filtered as an Unregistered
 *                             packet and is forwarded to an Egress port
 *                             according to its type (Unicast or Multicast) and
 *                             the configuration of the Egress port Port<n>
 *                             UnkFilterEn if packet is Unicast, and according
 *                             to the configuration of Port<n> UnregFilterEn if
 *                             the packet is Multicast.
 *                  GT_TRUE  - Packet is egress filtered as a registered packet
 *                             according to the members of the Multicast group.
 *                             NOTE:
 *                             When this field is 0, the type of the packet
 *                             Multicast or Unicast is set according to the
 *                             packet's MAC DA[40].
 *
 *       srcId              - Packet's Source ID
 *
 *       srcDev             - Packet's Source Device Number.
 *                            NOTE : HW device number !!
 *
 *       extDestInfo - extra destination information:
 *          multiDest - info about multi destination destination , used when:
 *                      type == CPSS_INTERFACE_VIDX_E or
 *                      type == CPSS_INTERFACE_VID_E
 *             excludeInterface - to exclude a "source" interface from the
 *                      destination flooding.
 *                      GT_FALSE - no "source" interface to exclude
 *                      GT_TRUE  - use "source" interface to exclude , see
 *                                 parameter excludedInterface.
 *             excludedInterface - the "source" interface to exclude from the
 *                          Multicast group.
 *                          NOTE: relevant only
 *                                when excludeInterface == GT_TRUE
 *                          when type == CPSS_INTERFACE_PORT_E
 *                              the {devPort.deviceNum,devPort.portNum} will be
 *                              excluded from the vid/vidx
 *                          when type == CPSS_INTERFACE_TRUNK_E
 *                              the trunkId will be excluded from the vid/vidx
 *                          type == CPSS_INTERFACE_VIDX_E not supported
 *                          type == CPSS_INTERFACE_VID_E  not supported
 *
 *             mirrorToAllCPUs - Mirror the packet to all CPUs
 *                          Enable a CPU to send a Multicast/Broadcast packet to
 *                          all CPUs in the system.
 *                          NOTE: relevant only :
 *                               when (excludeInterface == GT_FALSE) or
 *                               when ((excludeInterface == GT_TRUE) and
 *                                (excludedInterface.type == CPSS_INTERFACE_TRUNK_E))
 *                          GT_FALSE - The Multicast packet sent by the CPU is
 *                                     not mirrored to the CPU attached to the
 *                                     receiving device.
 *                          GT_TRUE - The Multicast packet sent by the CPU is
 *                                    mirrored to the CPU attached to the
 *                                    receiving device with a TO_CPU DSA Tag and
 *                                    MIRROR_TO_ALL_CPUS CPU Code.
 *       devPort - {device,port} info (single destination), used when:
 *                          type == CPSS_INTERFACE_PORT_E
 *             dstIsTagged - GT_FALSE - Packet is sent via network port untagged
 *                           GT_TRUE  - Packet is sent via network port tagged
 *             mailBoxToNeighborCPU - Mail box to Neighbor CPU, for CPU to CPU
 *                              mail box communication.
 *                           NOTE: As a Mail message is sent to a CPU with
 *                              unknown Device Number, the TrgDev
 *                              (dstInterface.devPort.tgtDev) must be set to
 *                              the local device number and TrgPort
 *                              (dstInterface.devPort.tgtPort) must be set to
 *                              the Cascading port number in the local device
 *                              through which this packet is to be transmitted.
 *
 */
typedef struct{
    CPSS_INTERFACE_INFO_STC         dstInterface; /* vid/vidx/port */
    GT_U8                           tc;
    CPSS_DP_LEVEL_ENT               dp;
    GT_BOOL                         egrFilterEn;
    GT_BOOL                         cascadeControl;
    GT_BOOL                         egrFilterRegistered;

    GT_U32                          srcId;
    GT_U8                           srcDev;

    union{
        struct{
            GT_BOOL                         excludeInterface;
            CPSS_INTERFACE_INFO_STC         excludedInterface; /* port/trunk */
            GT_BOOL                         mirrorToAllCPUs;
        }multiDest;

        struct{
            GT_BOOL                     dstIsTagged;
            GT_BOOL                     mailBoxToNeighborCPU;
        }devPort;
    }extDestInfo;

}CPSS_DXCH_NET_DSA_FROM_CPU_STC;

/*
 * typedef: structure CPSS_DXCH_NET_DSA_FORWARD_STC
 *
 * Description: structure of "forward" DSA tag parameters
 *
 * fields:
 *
 *  srcIsTagged - source packet received tagged
 *
 *  srcDev - Packet's Source Device Number.
 *           NOTE : HW device number !!
 *
 *  srcIsTrunk - the packet was received from trunk
 *     source.trunkId - - Packet's Source trunk id
 *                  NOTE : relevant when srcIsTrunk = GT_TRUE
 *     source.portNum - - Packet's Source port num
 *                  NOTE : relevant when srcIsTrunk = GT_FALSE
 *
 *  srcId              - Packet's Source ID
 *
 *  srcDev             - Packet's Source Device Number.
 *                       NOTE : HW device number !!
 *
 *  egrFilterRegistered -  Indicates that the packet is Egress filtered as a
 *                         registered packet.
 *              GT_FALSE - Packet is egress filtered as an Unregistered
 *                         packet and is forwarded to an Egress port
 *                         according to its type (Unicast or Multicast) and
 *                         the configuration of the Egress port Port<n>
 *                         UnkFilterEn if packet is Unicast, and according
 *                         to the configuration of Port<n> UnregFilterEn if
 *                         the packet is Multicast.
 *              GT_TRUE  - Packet is egress filtered as a registered packet
 *                         according to the members of the Multicast group.
 *                         NOTE:
 *                         When this field is 0, the type of the packet
 *                         Multicast or Unicast is set according to the
 *                         packet's MAC DA[40].
 *
 *  wasRouted - Indicates whether the packet is routed.
 *              GT_FALSE - Packet has not be Layer 3 routed.
 *              GT_TRUE  - Packet has been Layer 3 routed.
 *
 * qosProfileIndex - Packet's QoS Profile.
 *
 * dstInterface  - Packet's destination interface:
 *         type == CPSS_INTERFACE_TRUNK_E --> not supported !
 *         when type == CPSS_INTERFACE_PORT_E
 *              devPort.tgtDev - the target device that packet will be send to.
 *                               NOTE : HW device number !!
 *              devPort.tgtPort- the target port on device that packet will be
 *                             send to.
 *         when type == CPSS_INTERFACE_VIDX_E
 *              vidx     - the vidx group that packet will be sent to.
 *         when type == CPSS_INTERFACE_VID_E
 *              vlanId   - must be equal to commonParams.vid !!!
 *
 */
typedef struct{
    GT_BOOL                         srcIsTagged;
    GT_U8                           srcDev;
    GT_BOOL                         srcIsTrunk;
    union
    {
        GT_TRUNK_ID                 trunkId;
        GT_U8                       portNum;
    }source;

    GT_U32                          srcId;

    GT_BOOL                         egrFilterRegistered;
    GT_BOOL                         wasRouted;
    GT_U32                          qosProfileIndex;

    CPSS_INTERFACE_INFO_STC         dstInterface; /* vid/vidx/port */

}CPSS_DXCH_NET_DSA_FORWARD_STC;

/*
 * typedef: structure CPSS_DXCH_NET_DSA_TO_ANALYZER_STC
 *
 * Description: structure of "to analyzer" DSA tag parameters
 *
 * fields:
 *
 *      rxSniffer - is this packet destined to the Rx or to the Tx Analyzer port
 *          GT_FALSE - Packet was Tx sniffed and is forwarded to Target Tx
 *                     sniffer (Analyzer).
 *          GT_TRUE  - Packet was Rx sniffed and is forwarded to Target Rx
 *                     sniffer (Analyzer).
 *
 *      isTagged -  When rxSniffer == GT_TRUE:
 *                      This field is srcTagged.
 *                      srcTagged -
 *                          GT_FALSE - Packet was received from a regular
 *                                     network port untagged and is forwarded
 *                                     to the Ingress analyzer untagged.
 *                          GT_TRUE - Packet was received from a regular network
 *                                    port tagged and is forwarded to the Ingress
 *                                    analyzer tagged, with the same VID and UP
 *                                     with which it was received.
 *                      When rxSniffer == GT_FALSE:
 *                      This field is trgTagged.
 *                      trgTagged -
 *                          GT_FALSE - Packet was sent via a regular network
 *                                     port untagged and is forwarded to the Egress
 *                                     analyzer untagged.
 *                          GT_TRUE - Packet was sent via a regular network port
 *                                    tagged and is forwarded to the Egress
 *                                    analyzer tagged.
 *
 *      devPort - src/dst {dev,port} info :
 *              devPort.devNum -
 *                      SrcDev:
 *                      When rx_sniff = GT_TRUE or when tag is not extended:
 *                      SrcDev indicates the packet's original Ingress port.
 *                      When rx_sniff = GT_FALSE and the tag is extended:
 *                      TrgDev:
 *                      TrgDev indicates the packet's Egress port
 *                      NOTE: When the tag is not extended, this field always
 *                      indicates SrcDev.
 *              devPort.portNum -
 *                      When rx_sniff = GT_TRUE or when tag is not extended:
 *                      SrcPort:
 *                      indicates the packet's original Ingress port.
 *                      When rx_sniff = GT_FALSE and the tag is extended:
 *                      TrgPort:
 *                      indicates the packet's Egress port.
 *                      NOTE: When this tag is not extended, port is a 5-bit
 *                      field.
 *                      When this tag is not extended, this field always
 *                      indicates SrcPort.
 *
 *
 */
typedef struct{
    GT_BOOL                         rxSniffer;

    GT_BOOL                         isTagged;
    struct
    {
        GT_U8                       devNum;
        GT_U8                       portNum;
    }devPort;

}CPSS_DXCH_NET_DSA_TO_ANALYZER_STC;

/*
 * typedef: structure CPSS_DXCH_NET_DSA_TO_CPU_STC
 *
 * Description: structure of "To CPU" DSA tag parameters
 *
 * fields:
 *
 *      isEgressPipe - indicates the type of data forwarded to the CPU.
 *                     the data came from egress/ingress pipe (in PP).
 *          GT_FALSE - The packet was forwarded to the CPU by the Ingress pipe
 *                     and this tag contains the packet's source information.
 *          GT_TRUE  - The packet was forwarded to the CPU by the Egress pipe
 *                     and this tag contains the packet's destination
 *                     information.
 *          NOTE: When this tag is not extended, <isEgressPipe> should be
 *                     considered like GT_FALSE.
 *
 *      isTagged -  When isEgressPipe == GT_FALSE:
 *                      This field is srcTagged.
 *                      srcTagged - This tag contains Source Port information
 *                                  and this bit indicates the VLAN Tag format,
 *                                  in which the packet was received on the
 *                                  network port:
 *                          GT_FALSE - Packet was received from a network port
 *                                     untagged.
 *                          GT_TRUE - Packet was received from a network port
 *                                     tagged.
 *                      When isEgressPipe == GT_TRUE:
 *                      This field is trgTagged.
 *                      trgTagged - This tag contains Target Port information
 *                                  and this bit indicates the VLAN Tag format,
 *                                  in which the packet was transmitted via the
 *                                  network port:
 *                          GT_FALSE - Packet was transmitted to a regular
 *                                     network port untagged.
 *                          GT_TRUE - Packet was transmitted to a regular
 *                                     network port tagged.
 *
 *               NOTE: When isEgressPipe == GT_FALSE and the packet
 *                     forwarded to the CPU is received on a customer port on
 *                     which Nested VLAN is implemented, srcTagged is set to
 *                     GT_FALSE, regardless of the packet's VLAN tag format.
 *                     As the packet is considered untagged, when the packet is
 *                     forwarded to the CPU, the customer's VLAN tag (if any)
 *                     resides after the DSA tag.
 *
 *      devNum - When isEgressPipe == GT_FALSE:
 *                  NOTE : HW device number !!
 *                  This field is srcDev.
 *                      This tag contains Source Port information and this field
 *                      indicates the number of the Source Device on which the
 *                      packet was received.
 *                  When isEgressPipe == GT_TRUE:
 *                      This field is trgDev.
 *                      This tag contains Target Port information and this field
 *                      indicates the number of the Destination Device through
 *                      which the packet was transmitted.
 *
 *      srcIsTrunk -  When a packet's DSA Tag is replaced from FORWARD to
 *                      TO_CPU and FORWARD_DSA<SrcIsTrunk> = GT_TRUE and
 *                      isEgressPipe = GT_FALSE, this field is set to
 *                      GT_TRUE and This Tag hold interface.trunkId which is the
 *                      Trunk number parsed from the FORWARD DSA Tag
 *                  When isEgressPipe == GT_FALSE:
 *                      GT_FALSE - the field of <interface.port> indicates the source
 *                                 port number
 *                      GT_TRUE - the field of <interface.trunkId> indicates the source
 *                                 trunk number
 *                  When isEgressPipe == GT_TRUE:
 *                      this field should be GT_FALSE.
 *                      GT_FALSE - the field of <interface.port> indicates the
 *                                 destination Port through which the packet was
 *                                 transmitted.
 *                      GT_TRUE  - not supported !
 *      interface - interface info
 *             trunkId - see comments about srcIsTrunk field
 *             port    - see comments about srcIsTrunk field
 *
 *      cpuCode - CPU codes that indicate the reason for sending a packet to
 *                the CPU.
 *                Note : this is unified CPU code format (and not HW format))
 *
 *      wasTruncated - Was packet sent to CPU truncated?
 *                GT_FALSE - packet was not truncated.
 *                GT_TRUE - Indicates that only the first 128 bytes of the
 *                          packet are sent to the CPU. The packet's original
 *                          byte count is in <originByteCount> field.
 *
 *      originByteCount - The packet's original byte count.
 *
 *      timestamp       - IPFIX timestamping. Relevant only for DxChXcat and 
 *                        above devices, and used only in case that packet was
 *                        mirrored to the CPU due to IPFIX sampling.
 */
typedef struct{
   GT_BOOL                          isEgressPipe;

   GT_BOOL                          isTagged;
   GT_U8                            devNum;
   GT_BOOL                          srcIsTrunk;
   union
   {
       GT_TRUNK_ID                  srcTrunkId;
       GT_U8                        portNum;
   }interface;

   CPSS_NET_RX_CPU_CODE_ENT         cpuCode;
   GT_BOOL                          wasTruncated;
   GT_U32                           originByteCount;
   GT_U32                           timestamp;

}CPSS_DXCH_NET_DSA_TO_CPU_STC;

/*
 * typedef: structure CPSS_DXCH_NET_SDMA_TX_PARAMS_STC
 *
 * Description: structure of Tx parameters , that should be set to PP when using
 *              the SDMA channel
 *
 * fields:
 *      recalcCrc - GT_TRUE  - the PP should add CRC to the transmitted packet,
 *                  GT_FALSE - leave packet unchanged.
 *                  NOTE : The DXCH device always add 4 bytes of CRC when need
 *                         to recalcCrc = GT_TRUE
 *
 *
 *      txQueue - the queue that packet should be sent to CPU port. (0..7)
 *
 *      evReqHndl - The application handle got from cpssEventBind for
 *                  CPSS_PP_TX_BUFFER_QUEUE_E events.
 *
 *     invokeTxBufferQueueEvent - invoke Tx buffer Queue event.
 *              when the SDMA copies the buffers of the packet from the CPU ,
 *              the PP may invoke the event of CPSS_PP_TX_BUFFER_QUEUE_E.
 *              this event notify the CPU that the Tx buffers of the packet can
 *              be reused by CPU for other packets. (this in not event of
 *              CPSS_PP_TX_END_E that notify that packet left the SDMA)
 *              The invokeTxBufferQueueEvent parameter control the invoking of
 *              that event for this Packet.
 *              GT_TRUE - PP will invoke the event when buffers are copied (for
 *                        this packet's buffers).
 *              GT_FALSE - PP will NOT invoke the event when buffers are copied.
 *                        (for this packet's buffers).
 *              NOTE :
 *                  when the sent is done as "Tx synchronic" this parameter
 *                  IGNORED (the behavior will be as GT_FALSE)
 *
 *
 *
 */
typedef struct{
    GT_BOOL                         recalcCrc;
    GT_U8                           txQueue;
    GT_U32                          evReqHndl;
    GT_BOOL                         invokeTxBufferQueueEvent;
}CPSS_DXCH_NET_SDMA_TX_PARAMS_STC;

/*
 * typedef: structure CPSS_DXCH_NET_DSA_PARAMS_STC
 *
 * Description: structure of DSA parameters , that contain full information on
 *              DSA for RX,Tx packets to/from CPU from/to PP.
 *
 * fields:
 *      commonParams - common parameters in all DSA tag formats
 *
 *      dsaType - the type of DSA tag.(FORWARD/TO_CPU/FROM_CPU/TO_ANALYZER)
 *
 *      dsaInfo - the DSA tag information:
 *          toCpu - information needed when sending "TO_CPU" packets.
 *              relevant when dsaInfo = TO_CPU
 *          fromCpu - information needed when sending "FROM_CPU" packets.
 *              relevant when dsaInfo = FROM_CPU
 *          toAnalyzer - information needed when sending "TO_ANALYZER" packets.
 *              relevant when dsaInfo = TO_ANALYZER
 *          forward - information needed when sending "FORWARD" packets.
 *              relevant when dsaInfo = FORWARD
 *
*/
typedef struct{
    CPSS_DXCH_NET_DSA_COMMON_STC  commonParams;

    CPSS_DXCH_NET_DSA_CMD_ENT           dsaType;
    union{
        CPSS_DXCH_NET_DSA_TO_CPU_STC         toCpu;
        CPSS_DXCH_NET_DSA_FROM_CPU_STC       fromCpu;
        CPSS_DXCH_NET_DSA_TO_ANALYZER_STC    toAnalyzer;
        CPSS_DXCH_NET_DSA_FORWARD_STC        forward;
    }dsaInfo;
}CPSS_DXCH_NET_DSA_PARAMS_STC;



/*
 * typedef: structure CPSS_DXCH_NET_TX_PARAMS_STC
 *
 * Description: structure of Tx parameters , that contain full information on
 *              how to send TX packet from CPU to PP.
 *
 * fields:
 *      packetIsTagged - the packet (buffers) contain the vlan TAG inside.
 *                  GT_TRUE  - the packet contain the vlan TAG inside.
 *                  GT_FALSE - the packet NOT contain the vlan TAG inside.
 *
 *      cookie  - The user's data to be returned to the Application when in
 *                the Application's buffers can be re-used .
 *
 *      sdmaInfo - the info needed when send packets using the SDMA.
 *
 *      dsaParam - the DSA parameters
 *
*/
typedef struct{
    GT_BOOL                             packetIsTagged;
    GT_PTR                              cookie;
    CPSS_DXCH_NET_SDMA_TX_PARAMS_STC    sdmaInfo;
    CPSS_DXCH_NET_DSA_PARAMS_STC        dsaParam;
}CPSS_DXCH_NET_TX_PARAMS_STC;

/*
 * typedef: structure CPSS_DXCH_NET_RX_PARAMS_STC
 *
 * Description: structure of Rx parameters , that contain full information on
 *              how packet was received from PP to CPU port.
 *
 * fields:
 *      dsaParam - the DSA parameters
 *
*/
typedef struct{
    CPSS_DXCH_NET_DSA_PARAMS_STC        dsaParam;
}CPSS_DXCH_NET_RX_PARAMS_STC;


/*
 * typedef: enum CPSS_DXCH_NET_PCKT_ENCAP_ENT
 *
 * Description: Defines the different transmitted packet encapsulations.
 *
 * Enumerations:
 *      CPSS_DXCH_NET_REGULAR_PCKT_E  - Non - control packet.
 *      CPSS_DXCH_NET_CONTROL_PCKT_E  - Control packet.
 *
 * Comments:
 *      Non-control packets are subject to egress bridge filtering due
 *      to VLAN egress filtering or Spanning Tree filtering.
 *
 *      Control packts are not subject to bridge egress filtering.
 */
typedef enum
{
    CPSS_DXCH_NET_REGULAR_PCKT_E  = 0,
    CPSS_DXCH_NET_CONTROL_PCKT_E  = 7
}CPSS_DXCH_NET_PCKT_ENCAP_ENT;


/*
 * Typedef: enum CPSS_DXCH_NET_MAC_TYPE_ENT
 *
 * Description: Defines the different Mac-Da types of a transmitted packet.
 *
 * Enumerations:
 *      CPSS_DXCH_NET_UNICAST_MAC_E         - MAC_DA[0] = 1'b0
 *      CPSS_DXCH_NET_MULTICAST_MAC_E       - MAC_DA[0] = 1'b1
 *      CPSS_DXCH_NET_BROADCAST_MAC_E       - MAC_DA = 0xFFFFFFFF
 *
 */
typedef enum
{
    CPSS_DXCH_NET_UNICAST_MAC_E,
    CPSS_DXCH_NET_MULTICAST_MAC_E,
    CPSS_DXCH_NET_BROADCAST_MAC_E
}CPSS_DXCH_NET_MAC_TYPE_ENT;

/*
 * Typedef: enum CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT
 *
 * Description: Defines the behavior in case of Rx SDMA resource error
 *
 * Enumerations:
 *      CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E  - the packet remains scheduled for transmission.
 *      CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E  - the packet is dropped.
 *
 */
typedef enum
{
    CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E,
    CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E
}CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __cpssDxChGenNetIfTypesh */


