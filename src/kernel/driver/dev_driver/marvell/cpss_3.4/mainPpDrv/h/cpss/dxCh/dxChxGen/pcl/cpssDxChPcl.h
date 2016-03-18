/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPcl.h
*
* DESCRIPTION:
*       CPSS DxCh PCL lib API.
*
* FILE REVISION NUMBER:
*       $Revision: 40 $
*******************************************************************************/
#ifndef __cpssDxChPclh
#define __cpssDxChPclh

#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/generic/pcl/cpssPcl.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Typedef: enum CPSS_DXCH_PCL_RULE_OPTION_ENT
 *
 * Description:
 *      Enumerator for the TCAM rule write option flags.
 *
 * Fields:
 *      CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E   - write all fields
 *                  of rule to TCAM but rule state is invalid
 *                  (no match during lookups).
 *                  The cpssDxChPclRuleValidStatusSet can turn the rule
 *                  to valid state.
 *                  (APPLICABLE DEVICES: DxCh3, xCat, Lion, xCat2)
 *      CPSS_DXCH_PCL_RULE_OPTION_PORT_LIST_ENABLED_E   - if set caused
 *                        write rule to TCAM using port-list format.
 *                  (APPLICABLE DEVICES: xCat2)
 *
 *  Comments:
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E      = (0x1 << 0),
    CPSS_DXCH_PCL_RULE_OPTION_PORT_LIST_ENABLED_E  = (0x1 << 1)
} CPSS_DXCH_PCL_RULE_OPTION_ENT;

/* Action definitions for Policy Rules */

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_MIRROR_STC
 *
 * Description:
 *     This structure defines the mirroring related Actions.
 *
 * Fields:
 *       mirrorToRxAnalyzerPort - Enables mirroring the packet to
 *              the ingress analyzer port.
 *              GT_FALSE = Packet is not mirrored to ingress analyzer port.
 *              GT_TRUE = Packet is mirrored to ingress analyzer port.
 *       cpuCode                - The CPU code assigned to packets
 *              Mirrored to CPU or Trapped to CPU due
 *              to a match in the Policy rule entry
 *       mirrorTcpRstAndFinPacketsToCpu - TCP RST and FIN packets
 *              (i.e. TCP disconnect cases) can be mirrored to CPU
 *              GT_TRUE  - TCP RST and FIN packets mirrored to CPU.
 *              GT_FALSE - TCP RST and FIN packets not mirrored to CPU.
 *              Relevant for xCat and above devices only.
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_NET_RX_CPU_CODE_ENT      cpuCode;
    GT_BOOL                       mirrorToRxAnalyzerPort;
    GT_BOOL                       mirrorTcpRstAndFinPacketsToCpu;
} CPSS_DXCH_PCL_ACTION_MIRROR_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC
 *
 * Description:
 *     This structure defines the using of rule match counter.
 *
 * Fields:
 *       enableMatchCount  - Enables the binding of this
 *                  policy action entry to the Policy Rule Match Counter<n>
 *                  (0<=n<32) indexed by matchCounterIndex
 *                  GT_FALSE  = Match counter binding is disabled.
 *                  GT_TRUE = Match counter binding is enabled.
 *                  NOTE: for DxCh3 and above the index range is 0..(16K-1)
 *
 *       matchCounterIndex - A index one of the 32 Policy Rule Match Counter<n>
 *                 (0<=n<32) The counter is incremented for every packet
 *                 satisfying both of the following conditions:
 *                    - Matching this rule.
 *                    - The previous packet command is not hard drop.
 *                  NOTE: for DxCh3 and above the index range is 0..(16K-1)
 *
 *  Comments:
 *      DxCh3 and above: the CNC mechanism is used for the match counters.
 *
 */
typedef struct
{
    GT_BOOL   enableMatchCount;
    GT_U32    matchCounterIndex;
} CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_QOS_STC
 *
 * Description:
 *     This structure defines the packet's QoS attributes mark Actions.
 *
 * Fields:
 *
 *    modifyDscp             - For Ingress Policy:
 *                               The Modify DSCP QoS attribute of the packet.
 *                             Enables modification of the packet's DSCP field.
 *                             Only relevant if QoS precedence of the previous
 *                             QoS assignment mechanisms (Port, Protocol Based
 *                             QoS, and previous matching rule) is Soft.
 *                             Relevant for IPv4/IPv6 packets, only.
 *                             ModifyDSCP enables the following:
 *                               - Keep previous DSCP modification command.
 *                               - Enable modification of the DSCP field in
 *                                 the packet.
 *                               - Disable modification of the DSCP field in
 *                                 the packet.
 *
 *                             For Egress Policy:
 *                               Enables modification of the IP Header DSCP field
 *                             to egress.dscp of the transmitted packet.
 *                             ModifyDSCP enables the following:
 *                             - Keep previous packet DSCP setting.
 *                             - Modify the Packet's DSCP to <egress.dscp>.
 *
 *    modifyUp               - For Ingress Policy:
 *                            The Modify UP QoS attribute of the packet.
 *                            Enables modification of the packet's
 *                            802.1p User Priority field.
 *                            Only relevant if QoS precedence of the previous
 *                            QoS assignment mechanisms  (Port, Protocol Based
 *                            QoS, and previous matching rule) is Soft.
 *                            ModifyUP enables the following:
 *                            - Keep previous QoS attribute <ModifyUP> setting.
 *                            - Set the QoS attribute <modifyUP> to 1.
 *                            - Set the QoS attribute <modifyUP> to 0.
 *
 *                              For Egress Policy:
 *                            Enables the modification of the 802.1p User
 *                            Priority field to egress.up of packet
 *                            transmitted with a VLAN tagged.
 *                            ModifyUP enables the following:
 *                            - Keep previous QoS attribute <ModifyUP> setting.
 *                            - Modify the Packet's UP to <egress.up>.
 *
 *    egress.dscp            - The DSCP field set to the transmitted packets.
 *                             Relevant for Egress Policy only.
 *    egress.up              - The 802.1p UP field set to the transmitted packets.
 *                             Relevant for Egress Policy only.
 *    ingress members:
 *    profileIndex           - The QoS Profile Attribute of the packet.
 *                             Only relevant if the QoS precedence of the
 *                             previous QoS Assignment Mechanisms (Port,
 *                             Protocol Based QoS, and previous matching rule)
 *                             is Soft and profileAssignIndex is set
 *                             to GT_TRUE.the QoSProfile is used to index the
 *                             QoSProfile to QoS Table Entry<n> (0<=n<72)
 *                             and map the QoS Parameters for the packet,
 *                             which are TC, DP, UP and DSCP
 *                             Valid Range - 0 through 71
 *    profileAssignIndex     - Enable marking of QoS Profile Attribute of
 *                             the packet.
 *                             GT_TRUE - Assign <profileIndex> to the packet.
 *                             GT_FALSE - Preserve previous QoS Profile setting.
 *    profilePrecedence      - Marking of the QoSProfile Precedence.
 *                             Setting this bit locks the QoS parameters setting
 *                             from being modified by subsequent QoS
 *                             assignment engines in the ingress pipe.
 *                             QoSPrecedence enables the following:
 *                             - QoS precedence is soft and the packet's QoS
 *                               parameters may be overridden by subsequent
 *                               QoS assignment engines.
 *                             - QoS precedence is hard and the packet's QoS
 *                               parameters setting is performed until
 *                               this stage is locked. It cannot be overridden
 *                               by subsequent QoS assignment engines.
 *   qos                       - union for ingress or egress qos parameters
 *
 *  Comments:
 *           Egress Policy related actions supported only for DxCh2 and above
 *           devices
 *
 */
typedef struct
{
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT      modifyDscp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT      modifyUp;
    union
    {
        struct
        {
            GT_U32                                      profileIndex;
            GT_BOOL                                     profileAssignIndex;
            CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT profilePrecedence;
        } ingress;
        struct
        {
            GT_U8                              dscp;
            GT_U8                              up;
        } egress;
    } qos;
} CPSS_DXCH_PCL_ACTION_QOS_STC;

/*
 * Typedef: enum CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT
 *
 * Description: enumerator for PCL redirection target
 *
 * Fields:
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E   - no redirection
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E - redirection to output interface
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E - Ingress control pipe not
 *                        bypassed. If packet is triggered for routing,
 *                        Redirect the packet to the Router Lookup Translation
 *                        Table Entry specified in <routerLttIndex> bypassing
 *                        DIP lookup.
 *                        (APPLICABLE DEVICES: DxCh2, DxCh3, xCat, Lion).
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E - redirect to virtual
 *                        router with the specified Id (VRF ID)
 *                        (APPLICABLE DEVICES: xCat, Lion).
 */
typedef enum
{
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E
} CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT
 *
 * Description: enumerator for PCL redirection types of the passenger packet.
 *
 * Fields:
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E - The passenger packet
 *                                                         is Ethernet.
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E - The passenger packet is IP.
 *
 */

typedef enum
{
    CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E

}CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_REDIRECT_STC
 *
 * Description:
 *     This structure defines the redirection related Actions.
 *
 * Fields:
 *       redirectCmd   - redirection command
 *
 *       data.outIf   - out interface redirection data
 *                      relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_IF_E
 *                      packet redirected to output interface
 *       outInterface - output interface (port, trunk, VID, VIDX)
 *       vntL2Echo    - Enables Virtual Network Tester Layer 2 Echo
 *                     (APPLICABLE DEVICES: DxCh2, DxCh3, xCat, Lion, xCat2).
 *                      (relevant only if tunnelStart == GT_FALSE)
 *                      GT_TRUE - swap MAC SA and MAC DA in redirected packet
 *                      GT_FALSE - don't swap MAC SA and MAC DA
 *
 *       tunnelStart  - Indicates this action is a Tunnel Start point
 *                      GT_TRUE - Packet is redirected to an Egress Interface
 *                                and is tunneled as an  Ethernet-over-MPLS.
 *                      GT_FALSE - Packet is redirected to an Egress Interface
 *                                and is not tunneled.
 *                     (APPLICABLE DEVICES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3, xCat, Lion).
 *       tunnelPtr    - the pointer to "Tunnel-start" entry
 *                      (relevant only if tunnelStart == GT_TRUE)
 *                      (APPLICABLE DEVICES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3, xCat, Lion).
 *       tunnelType   -  tunnel type - Ethernet or IP,
 *                      (relevant only if tunnelStart == GT_TRUE)
 *                      (APPLICABLE DEVICES: DxCh3, xCat, Lion).
 *       arpPtr       - index in ARP table
 *                      (relevant only if tunnelStart == GT_FALSE)
 *                      (APPLICABLE DEVICES: Lion, xCat2).
 *       modifyMacDa  -  When enabled, the packet is marked
 *                       for MAC DA modification,
 *                       and the <ARP Index> specifies the new MAC DA.
 *                      (relevant only if tunnelStart == GT_FALSE)
 *                      (APPLICABLE DEVICES: Lion, xCat2).
 *       modifyMacSa  -  When enabled, the packet is marked
 *                       for MAC SA modification - similar to routed packets.
 *                      (relevant only if tunnelStart == GT_FALSE)
 *                      (APPLICABLE DEVICES: Lion, xCat2).
 *       data.routerLttIndex  - index of IP router Lookup Translation Table entry
 *                       relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E
 *                      (APPLICABLE DEVICES: DxCh2, DxCh3, xCat, Lion).
 *       data.vrfId   - virtual router ID
 *                 relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E
 *                      (APPLICABLE DEVICES: DxCh3, xCat, Lion).
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT  redirectCmd;
    union
    {
        struct
        {
            CPSS_INTERFACE_INFO_STC  outInterface;
            GT_BOOL                  vntL2Echo;
            GT_BOOL                  tunnelStart;
            GT_U32                   tunnelPtr;
            CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT tunnelType;
            GT_U32                   arpPtr;
            GT_BOOL                  modifyMacDa;
            GT_BOOL                  modifyMacSa;
        } outIf;
        GT_U32                       routerLttIndex;
        GT_U32                       vrfId;
    } data;

} CPSS_DXCH_PCL_ACTION_REDIRECT_STC;


/*
 * Typedef: enum CPSS_DXCH_PCL_POLICER_ENABLE_ENT
 *
 * Description: enumerator for policer options.
 *
 * Fields:
 *  CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E              - Meter and Counter are disabled.
 *  CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E - Both Meter and Counter enabled.
 *  CPSS_DXCH_PCL_POLICER_ENABLE_METER_ONLY_E        - Meter only enabled.
 *  CPSS_DXCH_PCL_POLICER_ENABLE_COUNTER_ONLY_E      - Counter only enabled.
 *
 */

typedef enum
{
    CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E              = GT_FALSE,
    CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E = GT_TRUE,
    CPSS_DXCH_PCL_POLICER_ENABLE_METER_ONLY_E,
    CPSS_DXCH_PCL_POLICER_ENABLE_COUNTER_ONLY_E
} CPSS_DXCH_PCL_POLICER_ENABLE_ENT;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_POLICER_STC
 *
 * Description:
 *     This structure defines the policer related Actions.
 *
 * Fields:
 *       policerEnable  - policer enable,
 *                DxCh1, DxCh2 and DxCh3 supports only the following commands:
 *                - CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E or GT_FALSE
 *                - CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E or GT_TRUE
 *                xCat and above devices support all values of the enum
 *       policerId      - policers table entry index
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_POLICER_ENABLE_ENT  policerEnable;
    GT_U32   policerId;

} CPSS_DXCH_PCL_ACTION_POLICER_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_VLAN_STC
 *
 * Description:
 *     This structure defines the VLAN modification related Actions.
 *
 * Fields:
 *       modifyVlan  - VLAN id modification command
 *
 *       nestedVlan  -  When this field is set to GT_TRUE, this rule matching
 *                      flow is defined as an access flow. The VID of all
 *                      packets received on this flow is discarded and they
 *                      are assigned with a VID using the device's VID
 *                      assignment algorithms, as if they are untagged. When
 *                      a packet received on an access flow is transmitted via
 *                      a core port or a Cascading port, a VLAN tag is added
 *                      to the packet (on top of the existing VLAN tag, if
 *                      any). The VID field is the VID assigned to the packet
 *                      as a result of all VLAN assignment algorithms. The
 *                      802.1p User Priority field of this added tag may be
 *                      one of the following, depending on the ModifyUP QoS
 *                      parameter set to the packet at the end of the Ingress
 *                      pipe:
 *                      - If ModifyUP is GT_TRUE, it is the UP extracted
 *                      from the QoSProfile to QoS Table Entry<n>
 *                      - If ModifyUP is GT_FALSE, it is the original packet
 *                      802.1p User Priority field if it is tagged and is UP
 *                      if the original packet is untagged.
 *
 *       vlanId      - VLAN id used for VLAN id modification if command
 *                     not CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
 *       precedence  - The VLAN Assignment precedence for the subsequent
 *                     VLAN assignment mechanism, which is the Policy engine
 *                     next policy-pass rule. Only relevant if the
 *                     VID precedence set by the previous VID assignment
 *                     mechanisms (Port, Protocol Based VLANs, and previous
 *                     matching rule) is Soft.
 *                     - Soft precedence The VID assignment can be overridden
 *                       by the subsequent VLAN assignment mechanism,
 *                       which is the Policy engine.
 *                     - Hard precedence The VID assignment is locked to the
 *                       last VLAN assigned to the packet and cannot be overridden.
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT          modifyVlan;
    GT_BOOL                                       nestedVlan;
    GT_U16                                        vlanId;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT   precedence;

} CPSS_DXCH_PCL_ACTION_VLAN_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC
 *
 * Description:
 *     This structure defines the IP unicast route parameters.
 *     Dedicated to override the relevant field of
 *     general action definitions.
 *
 * APPLICABLE DEVICES: DxCh1, DxCh1_Diamond, xCat, Lion, xCat2.
 *
 * Fields:
 *       doIpUcRoute       - Configure IP Unicast Routing Actions
 *                           GT_TRUE - the action used for IP unicast routing
 *                           GT_FALSE - the action is not used for IP unicast
 *                                      routing, all data of the structure
 *                                      ignored.
 *       arpDaIndex        - Route Entry ARP Index to the ARP Table (10 bit)
 *       decrementTTL      - Decrement IPv4 <TTL> or IPv6 <Hop Limit> enable
 *                           GT_TRUE - TTL Decrement for routed packets is enabled
 *                           GT_FALSE - TTL Decrement for routed packets is disabled
 *       bypassTTLCheck    - Bypass Router engine TTL and Options Check
 *                           GT_TRUE - the router engine bypasses the
 *                                     IPv4 TTL/Option check and the IPv6 Hop
 *                                     Limit/Hop-by-Hop check. This is used for
 *                                     IP-TO-ME host entries, where the packet
 *                                     is destined to this device
 *                           GT_FALSE - the check is not bypassed
 *       icmpRedirectCheck - ICMP Redirect Check Enable
 *                           GT_TRUE - the router engine checks if the next hop
 *                                     VLAN is equal to the packet VLAN
 *                                     assignment, and if so, the packet is
 *                                     mirrored to the CPU, in order to send an
 *                                     ICMP Redirect message back to the sender.
 *                          GT_FALSE - the check disabled
 *
 *  Comments:
 *  To configure IP Unicast route entry next elements of the action struct
 *  should be configured
 *    1. doIpUcRoute set to GT_TRUE
 *    2. redirection action set to CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E
 *      Out Interface data should be configured too.
 *    3. Policer should be disabled
 *    4. VLAN command CPSS_DXCH_PCL_ACTION_VLAN_CMD_MODIFY_ALL_E
 *    5. Nested VLAN should be disabled
 *    6. The packet command <pktCmd> should be set according to route entry
 *        purpose:
 *        - SOFT_DROP       - packet is dropped
 *        - HARD_DROP       - packet is dropped
 *        - TRAP            - packet is trapped to CPU with CPU code
 *                            IPV4_UC_ROUTE or IPV6_UC_ROUTE
 *        - FORWARD         - packet is routed
 *        - MIRROR_TO_CPU   - packet is routed and mirrored to the CPU with
 *                            CPU code IPV4_UC_ROUTE or IPV6_UC_ROUTE
 *
 */
typedef struct
{
    GT_BOOL  doIpUcRoute;
    GT_U32   arpDaIndex;
    GT_BOOL  decrementTTL;
    GT_BOOL  bypassTTLCheck;
    GT_BOOL  icmpRedirectCheck;

} CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC
 *
 * Description:
 *     This structure defines packet Source Id assignment.
 *
 * Fields:
 *     assignSourceId  - assign Source Id enable:
 *                     - GT_TRUE  - Assign Source Id.
 *                     - GT_FALSE - Don't assign Source Id.
 *     sourceIdValue   - the Source Id value to be assigned
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_BOOL  assignSourceId;
    GT_U32   sourceIdValue;
} CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC;

/*
 * Typedef: enum CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT
 *
 * Description: enumerator Controls the index used for IPCL lookup.
 *
 * Fields:
 *  CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E   - Retain;
 *                         use SrcPort/VLAN <Lookup PCL Cfg Mode>.
 *  CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E - Override;
 *                         use TTI/PCL Action <ipclConfigIndex>.
 *
 */

typedef enum
{
    CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E,
    CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E
} CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC
 *
 * Description:
 *     This structure defines future lookups configuration.
 *
 * Fields:
 *      ipclConfigIndex - index of Pcl Configuration table for next lookup.
 *                     0 means no value to update.
 *      pcl0_1OverrideConfigIndex - algorithm of selection
 *                     index of PCL Cfg Table for IPCL lookup0_1
 *      pcl1OverrideConfigIndex   - algorithm of selection
 *                     index of PCL Cfg Table for IPCL lookup1
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_U32                                 ipclConfigIndex;
    CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT  pcl0_1OverrideConfigIndex;
    CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT  pcl1OverrideConfigIndex;
} CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_ACTION_STC
 *
 * Description:
 *      Policy Engine Action
 *
 * Fields:
 *      pktCmd       -  packet command (forward, mirror-to-cpu,
 *                      hard-drop, soft-drop, or trap-to-cpu)
 *      actionStop   - Action Stop
 *                     GT_TRUE  - to skip the following PCL lookups
 *                     GT_FALSE - to continue with following PCL lookups
 *                     Supported by DxCh3 and xCat above devices.
 *                     DxCh1 and DxCh2 ignores the field.
 *                     Relevant to Policy Action Entry only.
 *      bypassBridge   - the Bridge engine processed or bypassed
 *                     GT_TRUE  - the Bridge engine is bypassed.
 *                     GT_FALSE - the Bridge engine is processed.
 *      bypassIngressPipe - the ingress pipe bypassed or not.
 *                     GT_TRUE  - the ingress pipe is bypassed.
 *                     GT_FALSE - the ingress pipe is not bypassed.
 *      egressPolicy - GT_TRUE  - Action is used for the Egress Policy
 *                     GT_FALSE - Action is used for the Ingress Policy
 *      lookupConfig - configuration of IPCL lookups.
 *      mirror       -  packet mirroring configuration
 *      matchCounter -  match counter configuration
 *      qos          -  packet QoS attributes modification configuration
 *      redirect     -  packet Policy redirection configuration
 *      policer      -  packet Policing configuration
 *                      Relevant to Policy Action Entry only.
 *      vlan         -  packet VLAN modification configuration
 *      ipUcRoute    -  special DxCh (not relevant for DxCh2 and above) Ip Unicast Route
 *                      action parameters configuration.
 *                      Relevant to Unicast Route Entry only.
 *      sourceId     -  packet source Id assignment
 *                      Relevant to Policy Action Entry only.
 *
 * Comment:
 */
typedef struct
{
    CPSS_PACKET_CMD_ENT                    pktCmd;
    GT_BOOL                                actionStop;
    GT_BOOL                                bypassBridge;
    GT_BOOL                                bypassIngressPipe;
    GT_BOOL                                egressPolicy;
    CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC lookupConfig;
    CPSS_DXCH_PCL_ACTION_MIRROR_STC        mirror;
    CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC matchCounter;
    CPSS_DXCH_PCL_ACTION_QOS_STC           qos;
    CPSS_DXCH_PCL_ACTION_REDIRECT_STC      redirect;
    CPSS_DXCH_PCL_ACTION_POLICER_STC       policer;
    CPSS_DXCH_PCL_ACTION_VLAN_STC          vlan;
    CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC   ipUcRoute;
    CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC     sourceId;
} CPSS_DXCH_PCL_ACTION_STC;


/*
 * Typedef: enum CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT
 *
 * Description:
 *     This enum describes possible formats of Policy rules.
 * Fields:
 *     key formats for Ingress
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E    - Standard (24B) L2
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E - Standard (24B)
 *                          L2+IPv4/v6 QoS
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E   - Standard (24B)
 *                          IPv4+L4
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E  - Standard (24B)
 *                          DxCh specific IPV6 DIP (used for routing)
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E  - Extended (48B)
 *                          L2+IPv4 + L4
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E   - Extended (48B)
 *                          L2+IPv6
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E   - Extended (48B)
 *                          L4+IPv6
 *     key formats for Egress (DxCh2 and above device)
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E     - Standard (24B) L2
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E  - Standard (24B)
 *                          L2+IPv4/v6 QoS
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E    - Standard (24B)
 *                          IPv4+L4
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E   - Extended (48B)
 *                          L2+IPv4 + L4
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E    - Extended (48B)
 *                          L2+IPv6
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E    - Extended (48B)
 *                          L4+IPv6
 *     CPSS_DXCH_PCL_RULE_FORMAT_LAST_E                  - last element to
 *                                                         calculate amount
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E       - standard UDB key
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E       - extended UDB key
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
    CPSS_DXCH_PCL_RULE_FORMAT_LAST_E
} CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_OFFSET_TYPE_ENT
 *
 * Description: Offset types for packet headers parsing used for user defined
 *              bytes configuration
 *
 * Enumerations:
 *    CPSS_DXCH_PCL_OFFSET_L2_E       - offset from start of mac(L2) header
 *    CPSS_DXCH_PCL_OFFSET_L3_E       - offset from start of L3 header.
 *                  Not supported for xCat and above.
 *    CPSS_DXCH_PCL_OFFSET_L4_E       - offset from start of L4 header
 *    CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E - offset from start of
 *                                          IPV6 Extention Header .
 *                  Not supported for xCat and above.
 *    CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E - the user defined byte used
 *                   for TCP or UDP comparator, not for byte from packet
 *    CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E - offset from start of
 *                  L3 header minus 2. Ethertype of IP.
 *                  Supported only for xCat and above.
 *    CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E   - offset from
 *                  Ethertype of MPLS minus 2.
 *                  Supported only for xCat and above.
 *    CPSS_DXCH_PCL_OFFSET_INVALID_E - invalid UDB - contains 0 always
 *                  Supported only for xCat and above.
 *
 * Comment:
 *      all possible valid UDB configurations (DxCh1, DxCh2, DxCh3):
 *      key-format, UDB-index, (valid offset types list)
 *      STD_NOT_IP,    0,   (L2, L3)
 *      STD_NOT_IP,    1,   (L2, L3)
 *      STD_NOT_IP,    2,   (L3)
 *      STD_L2_IP_QOS, 0,   (L2, L3, L4, IPV6_EH, TCP_UDP_COMPR)
 *      STD_L2_IP_QOS, 1,   (L2, L3, L4, IPV6_EH)
 *      STD_IPV4_L4,   0,   (L2, L3, L4, TCP_UDP_COMPR)
 *      STD_IPV4_L4,   1,   (L2, L3, L4)
 *      STD_IPV4_L4,   2,   (L3, L4)
 *      EXT_NOT_IPV6,  0,   (L2, L3, L4)
 *      EXT_NOT_IPV6,  1,   (L2, L3, L4)
 *      EXT_NOT_IPV6,  2,   (L3, L4, TCP_UDP_COMPR)
 *      EXT_NOT_IPV6,  3,   (L3, L4)
 *      EXT_NOT_IPV6,  4,   (L3, L4)
 *      EXT_NOT_IPV6,  5,   (L3, L4)
 *      EXT_IPV6_L2,   0,   (L2, L3, L4, IPV6_EH)
 *      EXT_IPV6_L2,   1,   (L2, L3, L4, IPV6_EH)
 *      EXT_IPV6_L2,   2,   (L3, L4, IPV6_EH, TCP_UDP_COMPR)
 *      EXT_IPV6_L2,   3,   (L3, L4, IPV6_EH)
 *      EXT_IPV6_L2,   4,   (L3, L4, IPV6_EH)
 *      EXT_IPV6_L2,   5,   (L3, L4, IPV6_EH)
 *      EXT_IPV6_L4,   0,   (L2, L3, L4, IPV6_EH)
 *      EXT_IPV6_L4,   1,   (L2, L3, L4, IPV6_EH)
 *      EXT_IPV6_L4,   2,   (L3, L4, TCP_UDP_COMPR)
 *      For xCat and above devices there are 23 UDBs for each of 8 packet type
 *      All 23 UDBs can be L2, L3-2, L4, MPLS-2, TCP_UDP_COMPR
  */
typedef enum
{
    CPSS_DXCH_PCL_OFFSET_L2_E,
    CPSS_DXCH_PCL_OFFSET_L3_E,
    CPSS_DXCH_PCL_OFFSET_L4_E,
    CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E,
    CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E,
    CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
    CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E,
    CPSS_DXCH_PCL_OFFSET_INVALID_E

} CPSS_DXCH_PCL_OFFSET_TYPE_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_PACKET_TYPE_ENT
 *
 * Description: Packet types for xCat and above devices
 *
 * Enumerations:
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E       - IPV4 TCP
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E       - IPV4 UDP
 *    CPSS_DXCH_PCL_PACKET_TYPE_MPLS_E           - MPLS
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_FRAGMENT_E  - IPV4 Fragment
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_OTHER_E     - IPV4 Other
 *    CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E - Ethernet Other
 *    CPSS_DXCH_PCL_PACKET_TYPE_UDE_E            - User Defined Ethertype
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV6_E           - IPV6
 *
 *    types below relevant for Lion and above
 *    CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E           - User Defined Ethertype1
 *    CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E           - User Defined Ethertype2
 *    CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E           - User Defined Ethertype3
 *    CPSS_DXCH_PCL_PACKET_TYPE_UDE4_E           - User Defined Ethertype4
 *
 * Comment:
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E,
    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E,
    CPSS_DXCH_PCL_PACKET_TYPE_MPLS_E,
    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_FRAGMENT_E,
    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_OTHER_E,
    CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E,
    CPSS_DXCH_PCL_PACKET_TYPE_UDE_E,
    CPSS_DXCH_PCL_PACKET_TYPE_IPV6_E,
    CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E,
    CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E,
    CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E,
    CPSS_DXCH_PCL_PACKET_TYPE_UDE4_E

} CPSS_DXCH_PCL_PACKET_TYPE_ENT;


/*
 * typedef: struct CPSS_DXCH_PCL_OVERRIDE_UDB_STC
 *
 * Description:
 *      Defines content of some User Defined Bytes in the keys.
 *      Only for xCat and above devices.
 *
 * Fields:
 *      Each value is BOOL,
 *      GT_TRUE causes overriding some User Defined Bytes in some key.
 *          by VRF-ID's MSB or USB or Qos Profile index as listed below.
 *      GT_FALSE causes the User Defined Bytes contains data from packet
 *          as configured.
 *
 *      vrfIdLsbEnableStdNotIp    - Standard Not Ip Key, VRF-ID's LSB in UDB15
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdMsbEnableStdNotIp    - Standard Not Ip Key, VRF-ID's MSB in UDB16
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdLsbEnableStdIpL2Qos  - Standard Ip L2 Qos Key, VRF-ID's LSB in UDB18
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdMsbEnableStdIpL2Qos  - Standard Ip L2 Qos Key, VRF-ID's MSB in UDB19
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdLsbEnableStdIpv4L4   - Standard Ipv4 L4 Key, VRF-ID's LSB in UDB20
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdMsbEnableStdIpv4L4   - Standard Ipv4 L4 Key, VRF-ID's MSB in UDB21
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdLsbEnableStdUdb      - Standard UDB Key, VRF-ID's LSB in UDB0
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdMsbEnableStdUdb      - Standard UDB Key, VRF-ID's MSB in UDB1
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdLsbEnableExtNotIpv6  - Extended Not Ipv6 Key, VRF-ID's LSB in UDB5
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdMsbEnableExtNotIpv6  - Extended Not Ipv6 Key, VRF-ID's MSB in UDB1
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdLsbEnableExtIpv6L2   - Extended Ipv6 L2 Key, VRF-ID's LSB in UDB11
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdMsbEnableExtIpv6L2   - Extended Ipv6 L2 Key, VRF-ID's MSB in UDB6
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdLsbEnableExtIpv6L4   - Extended Ipv6 L4 Key, VRF-ID's LSB in UDB14
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdMsbEnableExtIpv6L4   - Extended Ipv6 L4 Key, VRF-ID's MSB in UDB12
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdLsbEnableExtUdb      - Extended UDB Key, VRF-ID's LSB in UDB1
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      vrfIdMsbEnableExtUdb      - Extended UDB Key, VRF-ID's MSB in UDB2
 *                      (APPLICABLE DEVICES: xCat, Lion).
 *      qosProfileEnableStdUdb    - Standard UDB key, QoS profile in UDB2
 *      qosProfileEnableExtUdb    - Extended UDB key, QoS profile in UDB5
 *
 * Comment:
 */
typedef struct
{
    GT_BOOL       vrfIdLsbEnableStdNotIp;
    GT_BOOL       vrfIdMsbEnableStdNotIp;
    GT_BOOL       vrfIdLsbEnableStdIpL2Qos;
    GT_BOOL       vrfIdMsbEnableStdIpL2Qos;
    GT_BOOL       vrfIdLsbEnableStdIpv4L4;
    GT_BOOL       vrfIdMsbEnableStdIpv4L4;
    GT_BOOL       vrfIdLsbEnableStdUdb;
    GT_BOOL       vrfIdMsbEnableStdUdb;
    GT_BOOL       vrfIdLsbEnableExtNotIpv6;
    GT_BOOL       vrfIdMsbEnableExtNotIpv6;
    GT_BOOL       vrfIdLsbEnableExtIpv6L2;
    GT_BOOL       vrfIdMsbEnableExtIpv6L2;
    GT_BOOL       vrfIdLsbEnableExtIpv6L4;
    GT_BOOL       vrfIdMsbEnableExtIpv6L4;
    GT_BOOL       vrfIdLsbEnableExtUdb;
    GT_BOOL       vrfIdMsbEnableExtUdb;
    GT_BOOL       qosProfileEnableStdUdb;
    GT_BOOL       qosProfileEnableExtUdb;
} CPSS_DXCH_PCL_OVERRIDE_UDB_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC
 *
 * Description:
 *      Defines content of some User Defined Bytes in the key
 *      to be the packets trunk hash value.
 *
 * Fields:
 *      Each value is BOOL,
 *      GT_TRUE causes overriding some User Defined Bytes in some key
 *          by the packets trunk hash value.
 *      GT_FALSE causes the User Defined Bytes contains data from packet
 *          as configured.
 *
 *      trunkHashEnableStdNotIp   - Standard Not Ip Key,
 *                                  trunk hash in UDB17
 *      trunkHashEnableStdIpv4L4  - Standard Ipv4 L4 Key,
 *                                  trunk hash in UDB22
 *      trunkHashEnableStdUdb     - Standard UDB Key,
 *                                  trunk hash in UDB3
 *      trunkHashEnableExtNotIpv6 - Extended Not Ipv6 Key,
 *                                  trunk hash in UDB3
 *      trunkHashEnableExtIpv6L2  - Extended Ipv6 L2 Key,
 *                                  trunk hash in UDB7
 *      trunkHashEnableExtIpv6L4  - Extended Ipv6 L4 Key,
 *                                  trunk hash in UDB13
 *      trunkHashEnableExtUdb     - Extended UDB Key,
 *                                  trunk hash in UDB3
 *
 * Comment:
 */
typedef struct
{
    GT_BOOL       trunkHashEnableStdNotIp;
    GT_BOOL       trunkHashEnableStdIpv4L4;
    GT_BOOL       trunkHashEnableStdUdb;
    GT_BOOL       trunkHashEnableExtNotIpv6;
    GT_BOOL       trunkHashEnableExtIpv6L2;
    GT_BOOL       trunkHashEnableExtIpv6L4;
    GT_BOOL       trunkHashEnableExtUdb;
} CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC;

/*
 * Typedef: enum CPSS_DXCH_UDB_ERROR_CMD_ENT
 *
 * Description:
 *     This enum defines packet command taken in the UDB error case.
 * Fields:
 *     CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E         - continue Policy Lookup
 *     CPSS_DXCH_UDB_ERROR_CMD_TRAP_TO_CPU_E    - trap packet to CPU
 *     CPSS_DXCH_UDB_ERROR_CMD_DROP_HARD_E      - hard drop packet
 *     CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E      - soft drop packet
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E,
    CPSS_DXCH_UDB_ERROR_CMD_TRAP_TO_CPU_E,
    CPSS_DXCH_UDB_ERROR_CMD_DROP_HARD_E,
    CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E
} CPSS_DXCH_UDB_ERROR_CMD_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT
 *
 * Description:
 *     This enum defines the ip-total-length check algorithm
 *     that used for calculate the key "IP Header OK" bit.
 * Fields:
 *       CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E    - compare
 *             ip-total-lengts with the size of L3 level part of the packet
 *       CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E    - compare
 *             ip-total-lengts with the total size of the packet
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E,
    CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E
} CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT;

/*-------------------------------------------------------------------------*/
/*                           Policy Rules Formats                          */
/*-------------------------------------------------------------------------*/

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC
 *
 * Description:
 *     This structure describes the common segment of all key formats.
 *
 * Fields:
 *     pclId       - The PCL-ID attribute assigned to this lookup.
 *                   Only 2 MSBs of pclId are used when portListBmp is used
 *                   (see notes below about portListBmp).
 *     macToMe     - MAC To Me
 *                    1 - packet matched by MAC-To-Me lookup, 0 - not matched
 *                   The field relevant only for DxCh3 and above devices.
 *                   DxCh1 and DxCh2 ignore the field
 *     sourcePort  - The port number from which the packet ingressed the device.
 *                   Port 63 is the CPU port.
 *     portListBmp   - Port list bitmap. (APPLICABLE DEVICES: xCat2).
 *                   Relevant only when PCL lookup works in port-list mode.
 *                   In this case the pclId contains two MSB bits only and
 *                   sourcePort field not relevant.
 *                   Only one bit of the portListBmp is set in the key.
 *                   Value of the portListBmp in the key is (1 << srcPort).
 *                   The portListBmp bits in rule's mask and pattern
 *                   should be set to 0 (don't care) for matched ports.
 *                   The portListBmp bits in rule's mask should be 1 and
 *                   pattern should be set to 0 for not matched ports.
 *                   Applying a rule for a packet received from the CPU requires
 *                   the portListBmp pattern with all bits equal to zero. Hence 
 *                   rules cannot be shared between the CPU port and other ports
 *                   in port-list mode.
 *     isTagged    - Flag indicating the packet Tag state
 *                   For Ingress PCL Keys:
 *                   For non-tunnel terminated packets:
 *                   - If packet was received on DSA-tagged, this field is set
 *                     to the DSA tag <SrcTagged> field.
 *                   - If packet was received non-DSA-tagged, this field is set
 *                     to 1 if the packet was received  VLAN or Priority-tagged.
 *                   For Ethernet-Over-xxx tunnel-terminated packets: Passenger
 *                   packet VLAN tag format.
 *
 *                   For Egress PCL Keys:
 *                   For non Tunnel Start packets: Transmitted packet's VLAN tag
 *                   format after tag addition/removal/modification
 *                   For Ethernet-Over-xxx Tunnel Start packets:
 *                   Passenger packet's VLAN tag format after tag
 *                   addition/removal/modification.
 *                   0 = Packet is untagged.
 *                   1 = Packet is tagged.
 *
 *     vid         - VLAN ID assigned to the packet.
 *     up          - The packet's 802.1p User Priority field.
 *     qosProfile  - The QoS Profile assigned to the packet until this lookup
 *                   stage, as assigned by the device,
 *                   according to the QoS marking algorithm
 *     isIp        - An indication that the packet is IP
 *                   0 = Non IP packet.
 *                   1 = IPv4/6 packet.
 *     isL2Valid   - Indicates that Layer 2 information in the search key is valid.
 *                   0 = Layer 2 fields in the Ingress PCL (IPCL) or
 *                       Egress PCL (EPCL) are not valid
 *                   1 = Layer 2 field in the Ingress PCL or Egress PCL key is valid.
 *                   For Ingress PCL Keys: Layer 2 fields are not valid in the
 *                   IPCL keys when IP-Over-xxx is tunnel-terminated and its key
 *                   is based on the passenger IP packet which does not include
 *                   a Layer 2 header.
 *                   For Egress PCL Keys: Layer 2 fields are not valid in
 *                   the EPCL keys when an IP-Over-xxx is forwarded to a Tunnel
 *                   Start, and its PCL Key is based on the passenger IP packet
 *                   which does not include Layer 2 data.
 *                     Supported by DxCh2 and above.
 *                   DxCh1 devices ignores the field.
 *
 *     isUdbValid  - User-defined bytes (UDBs) are valid.
 *                   Indicates that all user-defined bytes used in that search
 *                   key were successfully parsed.
 *                   0 = At least 1 user-defined byte couldn't be parsed.
 *                   1 = All user-defined bytes used in this search key
 *                   were successfully parsed.
 *
 *  Comments:
 */
typedef struct
{
    GT_U16     pclId;
    GT_U8      macToMe;
    GT_U8      sourcePort;
    GT_U32     portListBmp;
    GT_U8      isTagged;
    GT_U16     vid;
    GT_U8      up;
    GT_U8      qosProfile;
    GT_U8      isIp;
    GT_U8      isL2Valid;
    GT_U8      isUdbValid;
} CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *
 * Description:
 *     This structure describes the common segment of all extended key formats.
 *
 * Fields:
 *     isIpv6     - An indication that the packet is IPv6.
 *                  0 = Non IPv6 packet.
 *                  1 = IPv6 packet.
 *     ipProtocol - IP protocol/Next Header type.
 *     dscp       - The DSCP field of the IPv4/6 header.
 *     isL4Valid  - Layer 4 information is valid.
 *                  This field indicates that all the Layer 4
 *                  information required for the search key is available
 *                  and the IP header is valid.
 *                  0 = Layer 4 information is not valid.
 *                  1 = Layer 4 information is valid.
 *                  Layer 4 information may not be available for any of the
 *                   following reasons:
 *                  - Layer 4 information is not included in the packet.
 *                     For example, Layer 4 information
 *                     isn't available in non-IP packets, or in IPv4
 *                     non-initial-fragments.
 *                  - Parsing failure: Layer 4 information is beyond
 *                     the first 128B of the packet, or beyond
 *                     IPv6 extension headers parsing capabilities.
 *                  - IP header is invalid.
 *     l4Byte0    - The following Layer 4 information is available for
 *                  UDP and TCP packets - L4 Header Byte0 through Byte3,
 *                  which contain the UDP/TCP destination and source ports.
 *                  For TCP also L4 Header Byte13, which contains the TCP flags.
 *                  For IGMP L4 Header Byte0, which contain the IGMP Type.
 *                  For ICMP L4 Header Byte0 and Byte1,
 *                  which contain the ICMP Type and Code fields
 *                  and L4 Header Byte4<in l4Byte2> and Byte5 <in l4Byte3>,
 *                  which contain the ICMP authentication field.
 *                  The following Layer 4 information is available for packets
 *                  that are not TCP, UDP, IGMP or ICMP:
 *                  L4 Header Byte0 through Byte3 L4 Header Byte13
 *     l4Byte1    - see l4Byte0
 *     l4Byte2    - see l4Byte0
 *     l4Byte3    - see l4Byte0
 *     l4Byte13   - see l4Byte0
 *     ipHeaderOk - Indicates a valid IP header.
 *                  0 = Packet IP header is invalid.
 *                  1 = Packet IP header is valid.
 *
 *  Comments:
 */
typedef struct
{
    GT_U8      isIpv6;
    GT_U8      ipProtocol;
    GT_U8      dscp;
    GT_U8      isL4Valid;
    GT_U8      l4Byte0;
    GT_U8      l4Byte1;
    GT_U8      l4Byte2;
    GT_U8      l4Byte3;
    GT_U8      l4Byte13;
    GT_U8      ipHeaderOk;

} CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC
 *
 * Description:
 *     This structure describes the common segment
 *     of all standard IP key formats.
 *
 * Fields:
 *     isIpv4      - An indication that the packet is IPv4.
 *                  0 = Non IPv4 packet.
 *                  1 = IPv4 packet.
 *     ipProtocol  - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     dscp        - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     isL4Valid   - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte2     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte3     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     ipHeaderOk  - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     ipv4Fragmented - Identifies an IPv4 fragment.
 *                      0 = Not an IPv4 packet or not an IPv4 fragment.
 *                      1 = Packet is an IPv4 fragment (could be
 *                      the first fragment or any subsequent fragment)
 *
 *  Comments:
 */
typedef struct
{
    GT_U8      isIpv4;
    GT_U8      ipProtocol;
    GT_U8      dscp;
    GT_U8      isL4Valid;
    GT_U8      l4Byte2;
    GT_U8      l4Byte3;
    GT_U8      ipHeaderOk;
    GT_U8      ipv4Fragmented;

} CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *
 * Description:
 *     This structure describes the standard not-IP key.
 * Fields:
 *     common     - the common part for all formats (see above)
 *     isIpv4      - An indication that the packet is IPv4.
 *                  0 = Non IPv4 packet.
 *                  1 = IPv4 packet.
 *     etherType  - Valid when <IsL2Valid> =1.
 *                  If <L2 Encap Type> = 0, this field contains the
 *                  <Dsap-Ssap> of the LLC NON-SNAP packet.
 *                  If <L2 Encap Type> = 1, this field contains the <EtherType/>
 *                  of the Ethernet V2 or LLC with SNAP packet.
 *     isArp      - An indication that the packet is an ARP packet
 *                   (identified by EtherType == 0x0806.)
 *                  0 = Non ARP packet.
 *                  1 = ARP packet.
 *     l2Encap    - The Layer 2 encapsulation of the packet.
 *                  For xCat2 and above
 *                     0 = The L2 Encapsulation is LLC NON-SNAP.
 *                     1 = The L2 Encapsulation is Ethernet V2 without LLC.
 *                     2 = Reserved.
 *                     3 = LLC with SNAP.
 *                  For DxCh1, DxCh2, xCat, Lion.
 *                     0 = The L2 Encapsulation is LLC NON-SNAP.
 *                     1 = The L2 Encapsulation is Ethernet V2 or LLC with SNAP.
 *     macDa      - Ethernet Destination MAC address.
 *     macSa      - Ethernet Source MAC address.
 *     udb        - User Defined Bytes with indexes 0,1,2
 *                  For xCat and above devices UDBs 15,16,17
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC common;
    GT_U8                                isIpv4;
    GT_U16                               etherType;
    GT_U8                                isArp;
    GT_U8                                l2Encap;
    GT_ETHERADDR                         macDa;
    GT_ETHERADDR                         macSa;
    GT_U8                                udb[3];

} CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *
 * Description:
 *     This structure describes the standard IPV4/V6 L2_QOS key.
 *
 * Fields:
 *     common            - the common part for all formats (see upper)
 *     commonStdIp       - the common part for all standard IP formats
 *     isArp             - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     isIpv6ExtHdrExist - IPv6 extension header exists.
 *                         Indicates that an IPv6 extension exists.
 *                         0 = Non-Ipv6 packet or IPv6 extension header does
 *                         not exist.
 *                         1 = Packet is IPv6 and extension header exists.
 *     isIpv6HopByHop    - Indicates that the IPv6 Original Extension Header
 *                          is Hop-by-Hop.
 *                         0 = Non-IPv6 packet or IPv6 extension header type is
 *                          not Hop-by-Hop Option Header.
 *                         1 = Packet is IPv6 and extension header type
 *                          is Hop-by-Hop Option Header.
 *     macDa             - Ethernet Destination MAC address.
 *     macSa             - Ethernet Source MAC address.
 *     udb               - User Defined Bytes with indexes 0,1
 *                         For xCat and above devices UDBs 18,19
 *
 *  Comments:
 *  udb[0] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC        common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC commonStdIp;
    GT_U8                                       isArp;
    GT_U8                                       isIpv6ExtHdrExist;
    GT_U8                                       isIpv6HopByHop;
    GT_ETHERADDR                                macDa;
    GT_ETHERADDR                                macSa;
    GT_U8                                       udb[2];

} CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC
 *
 * Description:
 *     This structure describes the standard IPV4_L4 key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonStdIp - the common part for all standard IP formats
 *     isArp       - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     isBc        - Ethernet Broadcast packet.
 *                   Valid when <IsL2Valid> =1.
 *                   Indicates an Ethernet Broadcast packet
 *                    (<MAC_DA> == FF:FF:FF:FF:FF:FF).
 *                   0 = MAC_DA is not Broadcast.
 *                   1 = MAC_DA is Broadcast.
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     l4Byte0     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte1     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte13    - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     udb         - User Defined Bytes with indexes 0,1,2
 *                   For xCat and above devices UDBs 20,21,22
 *
 *  Comments:
 *  udb[0] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC        common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC commonStdIp;
    GT_U8                                       isArp;
    GT_U8                                       isBc;
    GT_IPADDR                                   sip;
    GT_IPADDR                                   dip;
    GT_U8                                       l4Byte0;
    GT_U8                                       l4Byte1;
    GT_U8                                       l4Byte13;
    GT_U8                                       udb[3];

} CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC
 *
 * Description:
 *     This structure describes the standard IPV6 DIP key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonStdIp - the common part for all standard IP formats
 *     isArp       - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     isIpv6ExtHdrExist - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     isIpv6HopByHop    - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     dip         - 16 bytes IPV6 destination address.
 *
 *  Comments:
 *  Not supported by DxCh2 and DxCh3
 *  Used for Unicast IPV6 routing, lookup1 only
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC        common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC commonStdIp;
    GT_U8                                       isArp;
    GT_U8                                       isIpv6ExtHdrExist;
    GT_U8                                       isIpv6HopByHop;
    GT_IPV6ADDR                                 dip;

} CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC
 *
 * Description:
 *     This structure describes the extended not-IPV6 key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 header destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     etherType   - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     l2Encap     - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     ipv4Fragmented - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC
 *     udb         - User Defined Bytes with indexes 0,1,2,3,4,5
 *                   For xCat and above devices UDBs also 0,1,2,3,4,5
 *
 *  Comments:
 *  udb[2] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC     common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC commonExt;
    GT_IPADDR                                sip;
    GT_IPADDR                                dip;
    GT_U16                                   etherType;
    GT_U8                                    l2Encap;
    GT_ETHERADDR                             macDa;
    GT_ETHERADDR                             macSa;
    GT_U8                                    ipv4Fragmented;
    GT_U8                                    udb[6];

} CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the extended IPV6+L2  key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv6 source IP address field.
 *     dipBits127to120   - IPV6 destination address highest 8 bits.
 *     isIpv6ExtHdrExist - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     isIpv6HopByHop    - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     udb         - User Defined Bytes with indexes 0,1,2,3,4,5
 *                   For xCat and above devices UDBs 6,7,8,9,10,11
 *
 *  Comments:
 *  udb[2] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC     common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC commonExt;
    GT_IPV6ADDR                              sip;
    GT_U8                                    dipBits127to120;
    GT_U8                                    isIpv6ExtHdrExist;
    GT_U8                                    isIpv6HopByHop;
    GT_ETHERADDR                             macDa;
    GT_ETHERADDR                             macSa;
    GT_U8                                    udb[6];

} CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the extended IPV6+L4  key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv6 source IP address field.
 *     dip         - IPv6 destination IP address field.
 *     isIpv6ExtHdrExist - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     isIpv6HopByHop    - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     udb         - User Defined Bytes with indexes 0,1,2
 *                   For xCat and above devices UDBs 12,13,14
 *
 *  Comments:
 *  udb[2] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC     common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC commonExt;
    GT_IPV6ADDR                              sip;
    GT_IPV6ADDR                              dip;
    GT_U8                                    isIpv6ExtHdrExist;
    GT_U8                                    isIpv6HopByHop;
    GT_U8                                    udb[3];

} CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC
 *
 * Description: PCL Rule Key fields common to ingress "UDB" key formats.
 *              xCat and above devices only
 *
 * Fields:
 * pclId            - The PCL-ID attribute assigned to this lookup.
 *                    Only 2 MSBs of pclId are used when portListBmp is used
 *                    (see notes below about portListBmp).
 * macToMe          - MAC To Me
 *                     1 - packet matched by MAC-To-Me lookup, 0 - not matched
 *                     The field relevant only for DxCh3 and above devices.
 *                     DxCh1 and DxCh2 ignore the field
 * sourcePort       - The port number from which the packet ingressed the device.
 *                     Port 63 is the CPU port.
 * portListBmp      - Port list bitmap. (APPLICABLE DEVICES: xCat2).
 *                     Relevant only when PCL lookup works in port-list mode.
 *                     In this case the pclId contains two MSB bits only and
 *                     sourcePort field not relevant.
 *                     Only one bit of the portListBmp is set in the key.
 *                     Value of the portListBmp in the key is (1 << srcPort).
 *                     The portListBmp bits in rule's mask and pattern
 *                     should be set to 0 (don't care) for matched ports.
 *                     The portListBmp bits in rule's mask should be 1 and
 *                     pattern should be set to 0 for not matched ports.
 *                     Applying a rule for a packet received from the CPU requires
 *                     the portListBmp pattern with all bits equal to zero. Hence 
 *                     rules cannot be shared between the CPU port and other ports
 *                     in port-list mode.
 * vid              - VLAN ID assigned to the packet.
 * up               - The packet's 802.1p User Priority field.
 * isIp             - An indication that the packet is IP
 *                    0 = Non IP packet.
 *                    1 = IPv4/6 packet.
 * dscpOrExp        -  IP DSCP or MPLS EXP
 * isL2Valid        - Indicates that Layer 2 information in the search key is valid.
 *                    0 = Layer 2 fields in the Ingress PCL (IPCL) or
 *                        Egress PCL (EPCL) are not valid
 *                    1 = Layer 2 field in the Ingress PCL or Egress PCL key is valid.
 *                    For Ingress PCL Keys: Layer 2 fields are not valid in the
 *                    IPCL keys when IP-Over-xxx is tunnel-terminated and its key
 *                    is based on the passenger IP packet which does not include
 *                    a Layer 2 header.
 *                    For Egress PCL Keys: Layer 2 fields are not valid in
 *                    the EPCL keys when an IP-Over-xxx is forwarded to a Tunnel
 *                    Start, and its PCL Key is based on the passenger IP packet
 *                    which does not include Layer 2 data.
 *
 * isUdbValid      - User-defined bytes (UDBs) are valid.
 *                   Indicates that all user-defined bytes used in that search
 *                   key were successfully parsed.
 *                   0 = At least 1 user-defined byte couldn't be parsed.
 *                   1 = All user-defined bytes used in this search key
 *                   were successfully parsed.
 * pktTagging       -  The packet's VLAN Tag format . Valid when <IsL2Valid> = 1.
 *                     0 = Untagged
 *                     1 = Reserved.
 *                     2 = PriorityTagged
 *                     3 = IsTagged
 * l3OffsetInvalid  -  This flag indicates that the Layer 3 offset was
 *                     successfully found. 0=Valid; 1=Invalid
 * l4ProtocolType   -  0=Other/Ivalid;
 *                     1=TCP;
 *                     2=ICMP;
 *                     3=UDP;
 * pktType          -  1- IPv4 TCP
 *                     2- IPv4 UDP
 *                     4- MPLS
 *                     8- IPv4 Fragment
 *                     16 IPv4 Other
 *                     32- Ethernet Other
 *                     64- IPv6
 *                     128- UDE
 * ipHeaderOk       -  0 - invalid, 1 - valid - only for ingress Policy formats
 * macDaType        -  0=Unicast; Known and Unknown Unicast
 *                     1=Multicast; Known and Unknown Multicast
 *                     2=NonARP BC; Not ARP Broadcast packet
 *                     3=ARP BC; ARP Broadcast packet
 * l4OffsetInalid   -  This flag indicates that the Layer 4 offset was
 *                     successfully found. 0=Valid; 1=Invalid
 * l2Encapsulation  -  0=LLC not Snap;
 *                     1=Ethernet V2;
 *                     2=reserved
 *                     3=LLC Snap
 * isIpv6Eh         -  Indicates that an IPv6 extension exists
 *                     0=NotExists; Non-IPv6 packet or IPv6 extension header
 *                     does not exists.
 *                     1=Exists; Packet is IPv6 and extension header exists.
 * isIpv6HopByHop   -  Indicates that the IPv6 Original Extension Header
 *                     is hop-by-hop 0=NonHopByHop; Non-IPv6 packet or
 *                     IPv6 extension header type is not Hop-by-Hop Option Header.
 *                     1=HopByHop; Packet is IPv6 and extension header type is
 *                     Hop-by-Hop Option Header.
 */
typedef struct
{
    GT_U16       pclId;
    GT_U8        macToMe;
    GT_U8        sourcePort;
    GT_U32       portListBmp;
    GT_U16       vid;
    GT_U8        up;
    GT_U8        isIp;
    GT_U8        dscpOrExp;
    GT_U8        isL2Valid;
    GT_U8        isUdbValid;
    GT_U8        pktTagging;
    GT_U8        l3OffsetInvalid;
    GT_U8        l4ProtocolType;
    GT_U8        pktType;
    GT_U8        ipHeaderOk;
    GT_U8        macDaType;
    GT_U8        l4OffsetInalid;
    GT_U8        l2Encapsulation;
    GT_U8        isIpv6Eh;
    GT_U8        isIpv6HopByHop;
} CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC
 *
 * Description: PCL Rule ingress standard "UDB" Key fields.
  *              xCat and above devices only
*
 * Fields:
 * commonIngrUdb    -  fields common for ingress "UDB" styled keys
 * isIpv4           -  An indication that the packet is IPv4.
 *                     0 = Non IPv4 packet.
 *                     1 = IPv4 packet.
 * udb              -  user defined bytes 0-15,
 *                     byte 0 can be used for VRF-ID[7:0],
 *                     byte 1 can be used for VRF-ID[11:8],
 *                     byte 2 can be used for QoS profile index,
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC   commonIngrUdb;
    GT_U8                                              isIpv4;
    GT_U8                                              udb[16];
} CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC
 *
 * Description: PCL Rule ingress extended "UDB" Key fields.
  *              xCat and above devices only
*
 * Fields:
 * commonIngrUdb    -  fields common for ingress "UDB" styled keys
 * isIpv6           -  An indication that the packet is IPv6.
 *                     0 = Non IPv6 packet.
 *                     1 = IPv6 packet.
 * ipProtocol       -  IP protocol/Next Header type.
 * sipBits31_0      -  IPV6 SIP bits 31:0 or IPV4 SIP
 * sipBits79_32orMacSa  -  IPV6 SIP bits 79:32 for IPV6 packets
 *                     MAC_SA for not IPV6 packets
 * sipBits127_80orMacDa -  IPV6 SIP bits 127:80 for IPV6 packets
 *                     MAC_DA for not IPV6 packets
 * dipBits127_112   -  IPV6 DIP bits 127:112
 * dipBits31_0      -  IPV6 DIP bits 31:0 or IPV4 DIP
 * udb              -  user defined bytes 0-15,
 *                     byte 0 can be used for VRF-ID[7:0],
 *                     byte 1 can be used for VRF-ID[11:8],
 *                     byte 2 can be used for QoS profile index,
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC   commonIngrUdb;
    GT_U8                                              isIpv6;
    GT_U8                                              ipProtocol;
    GT_U8                                              sipBits31_0[4];
    GT_U8                                              sipBits79_32orMacSa[6];
    GT_U8                                              sipBits127_80orMacDa[6];
    GT_U8                                              dipBits127_112[2];
    GT_U8                                              dipBits31_0[4];
    GT_U8                                              udb[16];
} CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC;

/* EGRESS KEY FORMATS */

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC
 *
 * Description:
 *     This structure describes the common segment of all egress key formats.
 *
 * Fields:
 *     valid       - the 0-th bit of Egress PCL keys and it' s always 1.
 *                   This field can be used by DxCh3 and above device
 *                   application to determinate the Egress PCL key.
 *                   The field should be masked (don't care state) or
 *                   pattern is set to 1 in order to get match by the Egress PCL lookups.
 *                   The Ingress PCL keys of these devices contain
 *                   0-th bit of PCL ID in the position of the valid bit.
 *                   Ingress PCL rules should comprise 0 in the 0-th bit of PCL ID
 *                   in order to avoid match by Egress PCL lookups.
 *                   (APPLICABLE DEVICES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3, xCat, Lion).
 *                   This bit is not control bit. It's values not related to
 *                   cpssDxChPclRuleValidStatusSet and
 *                   cpssDxChPclRuleStateGet functions.
 *     pclId       - The PCL-ID attribute assigned to this lookup.
 *                   Only 2 MSBs of pclId are used when portListBmp is used
 *                   (see notes below about portListBmp).
 *     sourcePort  - The port number from which the packet ingressed the device.
 *                   Port 63 is the CPU port.
 *     portListBmp   - Port list bitmap. (APPLICABLE DEVICES: xCat2).
 *                   Relevant only when PCL lookup works in port-list mode.
 *                   In this case the pclId contains two MSB bits only and
 *                   the fields dp, tc, qosProfile, cpuCode not relevant.
 *                   Only one bit of the portListBmp is set in the key.
 *                   Value of the portListBmp in the key is (1 << dstPort).
 *                   The portListBmp bits in rule's mask and pattern
 *                   should be set to 0 (don't care) for matched ports.
 *                   The portListBmp bits in rule's mask should be 1 and
 *                   pattern should be set to 0 for not matched ports.
 *     isTagged    - Flag indicating the packet Tag state
 *                   For Ingress PCL Keys:
 *                   For non-tunnel terminated packets:
 *                   - If packet was received on DSA-tagged, this field is set
 *                     to the DSA tag <SrcTagged> field.
 *                   - If packet was received non-DSA-tagged, this field is set
 *                     to 1 if the packet was received  VLAN or Priority-tagged.
 *                   For Ethernet-Over-xxx tunnel-terminated packets: Passenger
 *                   packet VLAN tag format.
 *
 *                   For Egress PCL Keys:
 *                   For non Tunnel Start packets: Transmitted packet's VLAN tag
 *                   format after tag addition/removal/modification
 *                   For Ethernet-Over-xxx Tunnel Start packets:
 *                   Passenger packet's VLAN tag format after tag
 *                   addition/removal/modification.
 *                   0 = Packet is untagged.
 *                   1 = Packet is tagged.
 *
 *     vid         - VLAN ID assigned to the packet.
 *     up          - The packet's 802.1p User Priority field.
 *     isIp        - An indication that the packet is IP
 *                   0 = Non IP packet.
 *                   1 = IPv4/6 packet.
 *     isL2Valid   - Indicates that Layer 2 information in the search key is valid.
 *                   0 = Layer 2 fields in the Ingress PCL (IPCL) or
 *                       Egress PCL (EPCL) are not valid
 *                   1 = Layer 2 field in the Ingress PCL or Egress PCL key is valid.
 *                   For Ingress PCL Keys: Layer 2 fields are not valid in the
 *                   IPCL keys when IP-Over-xxx is tunnel-terminated and its key
 *                   is based on the passenger IP packet which does not include
 *                   a Layer 2 header.
 *                   For Egress PCL Keys: Layer 2 fields are not valid in
 *                   the EPCL keys when an IP-Over-xxx is forwarded to a Tunnel
 *                   Start, and its PCL Key is based on the passenger IP packet
 *                   which does not include Layer 2 data.
 *     egrPacketType - two bits field:
 *                   0 - packet to CPU
 *                   1 - packet from CPU
 *                   2 - packet to ANALYZER
 *                   3 - forward DATA packet
 *     cpuCode     - The CPU Code forwarded to the CPU.
 *                   Relevant only when packet is of type is TO_CPU.
 *     srcTrg      - The <SrcTrg> field extracted from the TO_CPU DSA<SrcTrg>:
 *                   0 = The packet was sent to the CPU by the ingress pipe and
 *                   the DSA tag contain attributes related to the
 *                    packet ingress.
 *                   1 = The packet was sent to the CPU by the egress pipe
 *                   and the DSA tag contains attributes related to the
 *                   packet egress.
 *                   Relevant only when packet is of type is TO_CPU.
 *     tc -          The Traffic Class of the FROM_CPU packet extracted
 *                   from DSA<TC>.
 *     dp -          The Drop Precedence of the FROM_CPU packet extracted
 *                   from DSA<DP>.
 *     egrFilterEnable - The <EgressFilterEn> extracted from
 *                   FROM_CPU DSA<EgressFilterEn>:
 *                   0 = FROM_CPU packet is subject to egress filtering,
 *                   e.g. data traffic from the CPU
 *                   1 = FROM_CPU packet is not subject egress filtering,
 *                   e.g. control traffic from the CPU
 *     isUnknown -   Indicates that the packet’s MAC DA
 *                   was not found in the FDB.
 *                   0 = The packet’s MAC DA was found in the FDB.
 *                   1 = the packet’s MAC DA was not found in the FDB.
 *                   Relevant only when packet is of type is FORWARD.
 *     rxSniff -     Indicates if this packet is Ingress or Egress
 *                   mirrored to the Analyzer.
 *                   0 = This packet is Egress mirrored to the analyzer.
 *                   1 = This packet is Ingress mirrored to the analyzer.
 *                   Relevant only when packet is of type is TO_ANALYZER.
 *     qosProfile  - The QoS Profile assigned to the packet until this lookup
 *                   stage, as assigned by the device,
 *                   according to the QoS marking algorithm
 *                   Relevant only when packet is of type is FORWARD.
 *     srcTrunkId  - Id of source trunk (see srcIsTrunk)
 *                   Relevant only when packet is of type is FORWARD.
 *     srcIsTrunk  - Indicates packets received from a Trunk on a
 *                   remote device or on the local device
 *                   0 = Source is not a Trunk due to one of the following:
 *                   - A non-DSA-tagged packet received from a network port
 *                   which is not a trunk member
 *                   - A DSA-tagged packet with DSA<TagCmd> =FORWARD
 *                   and DSA<SrcIsTrunk> =0
 *                   1 = Source is a Trunk due to one of the following
 *                   - A non-DSA-tagged packet received from a network port
 *                    which is a trunk member
 *                   - A DSA-tagged packet with DSA<TagCmd> =FORWARD and
 *                    DSA<SrcIsTrunk =1
 *                   Relevant only when packet is of type is FORWARD.
 *     isRouted    - Indicates that the packet has been routed either by the
 *                   local device or some previous device in a cascaded system.
 *                   0 = The packet has not been routed.
 *                   1 = The packet has been routed.
 *                   Relevant only when packet is of type is FORWARD.
 *     srcDev      - Together with <SrcPort> and <SrcDev> indicates the
 *                   network port at which the packet was received.
 *                   Relevant for all packet types exclude the
 *                   FWD_DATA packed entered to the PP from Trunk
 *     sourceId    - The Source ID assigned to the packet.
 *     isVidx      - Indicates that a packet is forwarded to a Multicast group.
 *                   0 = The packet is a Unicast packet forwarded to a
 *                   specific target port or trunk.
 *                   1 = The packet is a multi-destination packet forwarded
 *                   to a Multicast group.
 *
 *  Comments:
 *  The cpssDxChPclRuleSet function includes the egrPktType union members
 *  to the rule only if the egrPacketType field in the mask is equal to 3
 *  (i.e. all two bits of egress packet type checked). At this condition the
 *  egrPktType union member that matches the egrPacketType
 *  field in the pattern included to the rule
 *  (0 - toCpu, 1 - fromCpu, 2 - toAnalyzer and 3 - fwdData).
 *  If the egrPacketType field in the mask is no equal to 3 all
 *  egrPktType union members are ignored.
 */
typedef struct
{
    GT_U8      valid;
    GT_U16     pclId;
    GT_U8      sourcePort;
    GT_U32     portListBmp;
    GT_U8      isTagged;
    GT_U16     vid;
    GT_U8      up;
    GT_U8      isIp;
    GT_U8      isL2Valid;
    GT_U8      egrPacketType;
    union
    {
        struct
        {
            GT_U8 cpuCode;
            GT_U8 srcTrg;
        } toCpu;
        struct
        {
            GT_U8 tc;
            GT_U8 dp;
            GT_U8 egrFilterEnable;
        } fromCpu;
        struct
        {
            GT_U8 rxSniff;
        } toAnalyzer;
        struct
        {
            GT_U8 qosProfile;
            GT_U8 srcTrunkId;
            GT_U8 srcIsTrunk;
            GT_U8 isUnknown;
            GT_U8 isRouted;
        } fwdData;
    } egrPktType;
    GT_U8      srcDev;
    GT_U8      sourceId;
    GT_U8      isVidx;
} CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC
 *
 * Description:
 *     This structure describes the common segment of all extended
 *     egress key formats.
 *
 * Fields:
 *     isIpv6     - An indication that the packet is IPv6.
 *                  0 = Non IPv6 packet.
 *                  1 = IPv6 packet.
 *     ipProtocol - IP protocol/Next Header type.
 *     dscp       - The DSCP field of the IPv4/6 header.
 *     isL4Valid  - Layer 4 information is valid.
 *                  This field indicates that all the Layer 4
 *                  information required for the search key is available
 *                  and the IP header is valid.
 *                  0 = Layer 4 information is not valid.
 *                  1 = Layer 4 information is valid.
 *                  Layer 4 information may not be available for any of the
 *                   following reasons:
 *                  - Layer 4 information is not included in the packet.
 *                     For example, Layer 4 information
 *                     isn't available in non-IP packets, or in IPv4
 *                     non-initial-fragments.
 *                  - Parsing failure: Layer 4 information is beyond
 *                     the first 128B of the packet, or beyond
 *                     IPv6 extension headers parsing capabilities.
 *                  - IP header is invalid.
 *     l4Byte0    - The following Layer 4 information is available for
 *                  UDP and TCP packets - L4 Header Byte0 through Byte3,
 *                  which contain the UDP/TCP destination and source ports.
 *                  For TCP also L4 Header Byte13, which contains the TCP flags.
 *                  For IGMP L4 Header Byte0, which contain the IGMP Type.
 *                  For ICMP L4 Header Byte0 and Byte1,
 *                  which contain the ICMP Type and Code fields
 *                  and L4 Header Byte4<in l4Byte2> and Byte5 <in l4Byte3>,
 *                  which contain the ICMP authentication field.
 *                  The following Layer 4 information is available for packets
 *                  that are not TCP, UDP, IGMP or ICMP:
 *                  L4 Header Byte0 through Byte3 L4 Header Byte13
 *     l4Byte1    - see l4Byte0
 *     l4Byte2    - see l4Byte0
 *     l4Byte3    - see l4Byte0
 *     l4Byte13   - see l4Byte0
 *
 *     egrTcpUdpPortComparator - For non Tunnel Start packets:
 *                  The transmitted packet TCP/UDP comparator result
 *                  For Tunnel Start packets:
 *                  The transmitted passenger packet TCP/UDP comparator result
 *
 *  Comments:
 */
typedef struct
{
    GT_U8      isIpv6;
    GT_U8      ipProtocol;
    GT_U8      dscp;
    GT_U8      isL4Valid;
    GT_U8      l4Byte0;
    GT_U8      l4Byte1;
    GT_U8      l4Byte2;
    GT_U8      l4Byte3;
    GT_U8      l4Byte13;
    GT_U8      egrTcpUdpPortComparator;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC
 *
 * Description:
 *     This structure describes the common segment
 *     of all egress standard IP key formats.
 *
 * Fields:
 *     isIpv4      - An indication that the packet is IPv4.
 *                  0 = Non IPv4 packet.
 *                  1 = IPv4 packet.
 *     ipProtocol  - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     dscp        - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     isL4Valid   - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte2     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte3     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     ipv4Fragmented - Identifies an IPv4 fragment.
 *                      0 = Not an IPv4 packet or not an IPv4 fragment.
 *                      1 = Packet is an IPv4 fragment (could be
 *                      the first fragment or any subsequent fragment)
 *
 *     egrTcpUdpPortComparator - For non Tunnel Start packets:
 *                  The transmitted packet TCP/UDP comparator result
 *                  For Tunnel Start packets:
 *                  The transmitted passenger packet TCP/UDP comparator result
 *
 *  Comments:
 */
typedef struct
{
    GT_U8      isIpv4;
    GT_U8      ipProtocol;
    GT_U8      dscp;
    GT_U8      isL4Valid;
    GT_U8      l4Byte2;
    GT_U8      l4Byte3;
    GT_U8      ipv4Fragmented;
    GT_U8      egrTcpUdpPortComparator;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC;


/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC
 *
 * Description:
 *     This structure describes the standard egress not-IP key.
 * Fields:
 *     common     - the common part for all formats (see above)
 *     isIpv4      - An indication that the packet is IPv4.
 *                  0 = Non IPv4 packet.
 *                  1 = IPv4 packet.
 *     etherType  - Valid when <IsL2Valid> =1.
 *                  If <L2 Encap Type> = 0, this field contains the
 *                  <Dsap-Ssap> of the LLC NON-SNAP packet.
 *                  If <L2 Encap Type> = 1, this field contains the <EtherType/>
 *                  of the Ethernet V2 or LLC with SNAP packet.
 *     isArp      - An indication that the packet is an ARP packet
 *                   (identified by EtherType == 0x0806.)
 *                  0 = Non ARP packet.
 *                  1 = ARP packet.
 *     l2Encap    - The Layer 2 encapsulation of the packet.
 *                   0 = The L2 Encapsulation is LLC NON-SNAP.
 *                   1 = The L2 Encapsulation is Ethernet V2 or LLC with SNAP.
 *     macDa      - Ethernet Destination MAC address.
 *     macSa      - Ethernet Source MAC address.
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC common;
    GT_U8                                    isIpv4;
    GT_U16                                   etherType;
    GT_U8                                    isArp;
    GT_U8                                    l2Encap;
    GT_ETHERADDR                             macDa;
    GT_ETHERADDR                             macSa;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC
 *
 * Description:
 *     This structure describes the standard egress IPV4/V6 L2_QOS key.
 *
 * Fields:
 *     common            - the common part for all formats (see upper)
 *     commonStdIp       - the common part for all standard IP formats
 *     isArp             - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     dipBits0to31      - for IPV4 destination Ip address, for IPV6 the
 *                         31 LSBs of destination Ip address (network order)
 *     l4Byte13          - see in CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC
 *     macDa             - Ethernet Destination MAC address.
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC        common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC commonStdIp;
    GT_U8                                           isArp;
    GT_U8                                           dipBits0to31[4];
    GT_U8                                           l4Byte13;
    GT_ETHERADDR                                    macDa;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC
 *
 * Description:
 *     This structure describes the standard egress IPV4_L4 key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonStdIp - the common part for all standard IP formats
 *     isArp       - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     isBc        - Ethernet Broadcast packet.
 *                   Valid when <IsL2Valid> =1.
 *                   Indicates an Ethernet Broadcast packet
 *                    (<MAC_DA> == FF:FF:FF:FF:FF:FF).
 *                   0 = MAC_DA is not Broadcast.
 *                   1 = MAC_DA is Broadcast.
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     l4Byte0     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte1     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte13    - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC        common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC commonStdIp;
    GT_U8                                           isArp;
    GT_U8                                           isBc;
    GT_IPADDR                                       sip;
    GT_IPADDR                                       dip;
    GT_U8                                           l4Byte0;
    GT_U8                                           l4Byte1;
    GT_U8                                           l4Byte13;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC
 *
 * Description:
 *     This structure describes the egress extended not-IPV6 key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 header destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     etherType   - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     l2Encap     - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     ipv4Fragmented - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC     common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC commonExt;
    GT_IPADDR                                    sip;
    GT_IPADDR                                    dip;
    GT_U16                                       etherType;
    GT_U8                                        l2Encap;
    GT_ETHERADDR                                 macDa;
    GT_ETHERADDR                                 macSa;
    GT_U8                                        ipv4Fragmented;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the egress extended IPV6+L2  key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv6 source IP address field.
 *     dipBits127to120   - IPV6 destination address highest 8 bits.
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC     common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC commonExt;
    GT_IPV6ADDR                                  sip;
    GT_U8                                        dipBits127to120;
    GT_ETHERADDR                                 macDa;
    GT_ETHERADDR                                 macSa;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the egress extended IPV6+L4  key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv6 source IP address field.
 *     dip         - IPv6 destination IP address field.
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC     common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC commonExt;
    GT_IPV6ADDR                              sip;
    GT_IPV6ADDR                              dip;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC;

/*
 * Typedef: union CPSS_DXCH_PCL_RULE_FORMAT_UNT
 *
 * Description:
 *     This union describes the PCL key.
 *
 * Fields:
 *     Ingress Policy Key format structures
 *     ruleStdNotIp       - Standard Not IP packet key
 *     ruleStdIpL2Qos     - Standard IPV4 and IPV6 packets
 *                          L2 and QOS styled key
 *     ruleStdIpv4L4      - Standard IPV4 packet L4 styled key
 *     ruleStdIpv6Dip     - Standard IPV6 packet DIP styled key (DxCh only)
 *     ruleIngrStdUdb     - Ingress Standard UDB styled packet key
 *     ruleExtNotIpv6     - Extended Not IP and IPV4 packet key
 *     ruleExtIpv6L2      - Extended IPV6 packet L2 styled key
 *     ruleExtIpv6L4      - Extended IPV6 packet L2 styled key
 *     ruleIngrExtUdb     - Ingress Extended UDB styled packet key
 *     ruleEgrStdNotIp    - Egress Standard Not IP packet key
 *     ruleEgrStdIpL2Qos  - Egress Standard IPV4 and IPV6 packets
 *                          L2 and QOS styled key
 *     ruleEgrStdIpv4L4   - Egress Standard IPV4 packet L4 styled key
 *     ruleEgrExtNotIpv6  - Egress Extended Not IP and IPV4 packet key
 *     ruleEgrExtIpv6L2   - Egress Extended IPV6 packet L2 styled key
 *     ruleEgrExtIpv6L4   - Egress Extended IPV6 packet L2 styled key
 *
 *  Comments:
 */
typedef union
{
    CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC          ruleStdNotIp;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC       ruleStdIpL2Qos;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC         ruleStdIpv4L4;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC        ruleStdIpv6Dip;
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC   ruleIngrStdUdb;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC        ruleExtNotIpv6;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC         ruleExtIpv6L2;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC         ruleExtIpv6L4;
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC   ruleIngrExtUdb;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC      ruleEgrStdNotIp;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC   ruleEgrStdIpL2Qos;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC     ruleEgrStdIpv4L4;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC    ruleEgrExtNotIpv6;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC     ruleEgrExtIpv6L2;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC     ruleEgrExtIpv6L4;
} CPSS_DXCH_PCL_RULE_FORMAT_UNT;

/*
 * typedef: enum CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT
 *
 * Description: packet types that can be enabled/disabled for Egress PCL.
 *              By default on all ports and on all packet types are disabled
 *
 * Enumerators:
 *   CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E - Enable/Disable Egress Policy
 *                   for Control packets FROM CPU.
 *   CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E    - Enable/Disable Egress Policy
 *                   for data packets FROM CPU.
 *   CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E           - Enable/Disable Egress Policy
 *                   on TO-CPU packets
 *   CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E      - Enable/Disable Egress Policy
 *                   for TO ANALYZER packets.
 *   CPSS_DXCH_PCL_EGRESS_PKT_TS_E               - Enable/Disable Egress Policy
 *                   for data packets that are tunneled in this device.
 *   CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E           - Enable/Disable Egress Policy
 *                   for data packets that are not tunneled in this device.
 *
 * Comment:
 *    relevant only for DxCh2 and above devices
 */
typedef enum
{
    CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E,
    CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E,
    CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E,
    CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E,
    CPSS_DXCH_PCL_EGRESS_PKT_TS_E,
    CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E
} CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT;

/*
 * typedef: enum CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT
 *
 * Description: Type of access to Ingress / Egress PCL configuration Table.
 *              Type of access defines how device calculates index of PCL
 *              configuration Table for packet.
 *
 * Enumerators:
 *   CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E - by PORT access mode.
 *                                             ingress port or trunk id is used
 *                                             to calculate index of the PCL
 *                                             configuration table
 *   CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E - by VLAN access mode
 *                                             VLAN ID is used
 *                                             to calculate index of the PCL
 *                                             configuration table
 * Comment:
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E,
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E
} CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT
 *
 * Description: Global Egress PCL or Ingress Policy
 *              Configuration Table Access mode
 *
 * Enumerators:
 *   CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E - access by local port number
 *   CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E - access by remote device Id
 *             and Port number or by trunk Id,
 *             Used for Value Blade systems based on DX switch device.
 *             The remote port and device are taken from DSA tag and
 *             represent real source port and device for a packets.
 *
 * Comment:
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E,
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E
} CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT;


/*
 * typedef: enum CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT
 *
 * Description: size of Ingress Policy / Egress PCL Configuration
 *              table segment accessed by "non-local device and port"
 *
 * Enumerators:
 *   CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E - support 32 port remote devices
 *   CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E - support 64 port remote devices
 *
 * Comment:
 *    relevant only for DxCh2 and above devices
*/
typedef enum
{
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E,
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E
} CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT;

/*
 * typedef: enum CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT
 *
 * Description: base of Ingress Policy / Egress PCL Configuration
 *              table segment accessed
 *              by "non-local device and port"
 *
 * Enumerators:
 *   CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E    - the base is 0
 *   CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E - the base is 1024 (only 32-port)
 *   CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E - the base is 2048
 *   CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E - the base is 3072 (only 32-port)
 *
 * Comment:
 *    relevant only for DxCh2 and above devices
 *    Both egress and ingress PCL Configuration tables contain 4096
 *    entries block "entry per VLAN".
 *    In Non-local mode the packets that mapped by device-and-port served
 *    by entry from MAX-DEVICES(i.e.32)* MAX-PORTS(i.e. 32 * 64) entries block
 *    that overlaps the "entry per VLAN" block, but least than it
 *    (the size is 1024 or 2048 entries). The values upper dedicated to
 *    configure it's position.
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E,
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E,
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E,
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E
} CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT;

/*
 * typedef: struct CPSS_DXCH_PCL_LOOKUP_CFG_STC
 *
 * Description: PCL Configuration Table Entry Per Lookup Attributes.
 *
 * Fields:
 *    enableLookup           - Enable Lookup
 *                             GT_TRUE - enable Lookup
 *                             GT_FALSE - disable Lookup
 *                             The enableLookup enables or disables only 
 *                             Lookup0_0 for xCat and above devices and 
 *                             when the CPSS_PCL_LOOKUP_0_E lookup is used.
 *                             The dualLookup enables or disables Lookup0_1 
 *                             in this case.
 *    pclId                  - PCL-ID bits in the TCAM search key.
 *    dualLookup             - Relevant for CPSS_PCL_LOOKUP_0_E lookup only.
 *                             For DxCh3 devices
 *                                 GT_TRUE -  lookup0 has 2 sublookups.
 *                                 GT_FALSE - lookup0 has only 1 sublookup.
 *                                 lookup0_0 use pclId as is.
 *                                 lookup0_1 uses pclId with inverted LSB.
 *                             For xCat and above devices and above
 *                                 GT_TRUE -  lookup0_1 enabled.
 *                                 lookup0_1 uses pclIdL01.
 *                                 GT_FALSE - lookup0_1 disabled.
 *    pclIdL01               - only for xCat and above devices.
 *                             relevant for CPSS_PCL_LOOKUP_0_E  lookup only.
 *                             independent PCL ID for lookup0_1
 *    groupKeyTypes.nonIpKey - type of TCAM search key for NON-IP packets.
 *    groupKeyTypes.ipv4Key  - type of TCAM search key for IPV4 packets
 *    groupKeyTypes.ipv6Key  - type of TCAM search key for IPV6 packets
 *
 Comments:
 *    1. For DxCh2 and above devices the entry contains
 *    - "lookup enable" bit
 *    - 10 "PCL-ID" bits
 *    - 1 bit "not IP key" type standard or extended
 *    - 2 bits "IPV4 key" type std-L2, std-L4, ext
 *    - 2 bits "IPV6 key" type std-L2, ext-L2, ext-L4
 *    2. For DxCh1 devices the entry contains
 *    - "lookup enable" bit
 *    - 10 "PCL-ID" bits
 *    - 1 bit - key standard or extended (for all types)
 *    - 1 bit - key L2 or L4 styled for standard IPv4 or extended IPv6 packets
 *    - 1 bit - (lookup1 only) IPV6 std-DIP key
 *    2.1 Acceptable key types for DxCh1 devices.
 *    2.1.1  Standard Keys.
 *          nonIpKey - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E
 *          ipv4Key  - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E
 *                       L2 style of key for IPv4 packets
 *                     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E
 *                       L4 style of key for IPv4 packets
 *          ipv6Key  - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E
 *                       L2 style of key for IPv6 packets
 *                     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E
 *                      IPv6 DIP key for lookup#1 only.
 *    2.1.2 Extended Keys.
 *          nonIpKey - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E
 *          ipv4Key  - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E
 *          ipv6Key  - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E
 *                       L2 style of key for IPv6 packets
 *                     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E
 *                       L4 style of key for IPv6 packets
 *    3. The SW structure can represent any possible HW configuration
 *    If an impossible configuration passed an appropriate APIs
 *    GT_BAD_PARAM is returned.
 *
 *    4. For xCat2 if CPSS_PCL_LOOKUP_0_E specified
 *    the groupKeyTypes packet types configured both for
 *    lookup_0_0 and for lookup_0_1 by the same
 *    values. To configure them separately specify CPSS_PCL_LOOKUP_0_0_E
 *    and CPSS_PCL_LOOKUP_0_1_E.
 */
typedef struct
{
    GT_BOOL                                   enableLookup;
    GT_U32                                    pclId;
    GT_BOOL                                   dualLookup;
    GT_U32                                    pclIdL01;
    struct
    {
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT    nonIpKey;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT    ipv4Key;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT    ipv6Key;
    }                                         groupKeyTypes;

} CPSS_DXCH_PCL_LOOKUP_CFG_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC
 *
 * Description: global access mode to Ingress or Egress
 *              PCL configuration tables.
 *
 * Fields:
 *    ipclAccMode     - Ingress Policy local/non-local mode
 *    ipclMaxDevPorts - Ingress Policy support 32/64 port remote devices
 *    ipclDevPortBase - Ingress Policy Configuration table access base
 *    epclAccMode     - Egress PCL local/non-local mode
 *    epclMaxDevPorts - Egress PCL support 32/64 port remote devices
 *    epclDevPortBase - Egress PCL Configuration table access base
 * Comment:
 *   - for DxCh2 and above all fields are relevant
 *   - for DxCh1 relevant only ipclAccMode member
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT   ipclAccMode;
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT ipclMaxDevPorts;
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT ipclDevPortBase;
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT   epclAccMode;
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT epclMaxDevPorts;
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT epclDevPortBase;

} CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC;

/*
 * Typedef: enum CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT
 *
 * Description: enumerator for Egress PCL
 *              VID and UP key fields content mode
 *
 * Fields:
 *  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E   - extract from packet Tag0
 *  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E   - extract from packet Tag1
 */
typedef enum
{
    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E,
    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E
} CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT;

/*******************************************************************************
* cpssDxChPclInit
*
* DESCRIPTION:
*   The function initializes the device for following configuration
*   and using Policy engine
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclInit
(
    IN GT_U8                           devNum
);

/*******************************************************************************
* cpssDxChPclUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter value more then HW bit field
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
);

/*******************************************************************************
* cpssDxChPclUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
);

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes
*   by predefined optional key fields.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
*       udbOverridePtr - (pointer to) UDB override structure
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclOverrideUserDefinedBytesSet
(
    IN  GT_U8                          devNum,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_STC *udbOverridePtr
);

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesGet
*
* DESCRIPTION:
*   The function gets overriding of  User Defined Bytes
*   by predefined optional key fields.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       udbOverridePtr - (pointer to) UDB override structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclOverrideUserDefinedBytesGet
(
    IN  GT_U8                          devNum,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_STC *udbOverridePtr
);

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes
*   by packets Trunk Hash value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum              - device number
*       udbOverTrunkHashPtr - (pointer to) UDB override trunk hash structure
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       The Trunk Hash field cannot be included in the key of the
*       first IPCL lookup (Lookup0_0).
*       Thus, when the configuration for Trunk Hash in the Policy keys
*       is enabled, it applies only to the second and third lookups.
*
*******************************************************************************/
GT_STATUS cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
);

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet
*
* DESCRIPTION:
*   The function gets overriding of  User Defined Bytes
*   by packets Trunk Hash value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum              - device number
* OUTPUTS:
*       udbOverTrunkHashPtr - (pointer to) UDB override trunk hash structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
);

/*******************************************************************************
* cpssDxChPclRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*
*                          For DxCh and DxCh2 devices:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*                          For DxCh3 device:
*                          The TCAM has up to 3.5K (3584) rows.
*                          See datasheet of particular device for TCAM size.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 14335.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 7167.
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n < 3584
*                            is placed in the space of two standard rules with
*                            indexes n and n + 3584.
*                          - Extended rule with index  3584 <= n < 7168
*                            is placed in the space of two standard rules with
*                            indexes n + 3584, n + 7168.
*
*                          For xCat and above devices:
*                          See datasheet of particular device for TCAM size.
*                          For example describe the TCAM that has 1/4K (256) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1K (1024).
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 0.5K (512).
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index 2n (even index)
*                            is placed in the space of two standard rules with
*                            indexes 4n and 4n + 1.
*                          - Extended rule with index 2n+1 (odd index)
*                            is placed in the space of two standard rules with
*                            indexes 4n+2 and 4n + 3.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*                               Write rule to TCAM not using port-list format.
*                               Rule state is valid.
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E -
*                               write all fields of rule to TCAM but rule's
*                               state is invalid (no match during lookups).
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E |
*                                             CPSS_DXCH_PCL_RULE_OPTION_PORT_LIST_ENABLED_E -
*                               write all fields of rule to TCAM using port-list format
*                               but rule's state is invalid
*                              (no match during lookups).
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternPtr       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleSet
(
    IN GT_U8                              devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT      ruleOptionsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

/*******************************************************************************
* cpssDxChPclRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

/*******************************************************************************
* cpssDxChPclRuleActionGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN CPSS_PCL_DIRECTION_ENT              direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

/*******************************************************************************
* cpssDxChPclRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule.
*   For DxCh and DxCh2 devices start indirect rule write operation
*   with "garbage" pattern and mask content and "valid-flag" == 0
*   For DxCh3 devices calculates the TCAM position by ruleSize and ruleIndex
*   parameters and updates the first "valid" bit matching X/Y pair to (1,1)
*   i.e. don't match any value. If origin of valid or invalid rule found at the
*   specified TCAM position it will be invalid and available to back validation.
*   If the specified TCAM position contains the not first 24 byte segment of
*   extended rule, the rule also will be invalidated,
*   but unavailable to back validation.
*   If the garbage found in TCAM the X/Y bits will be updated to be as
*   in invalid rule.
*   The function does not check the TCAM contents.
*   GT_OK will be returned in each of described cases.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Backward compatible styled API.
*       The cpssDxChPclRuleValidStatusSet recommended for using instead.
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleInvalidate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex
);

/*******************************************************************************
* cpssDxChPclRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule.
*        The validation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Valid indication set.
*           The function does not check content of the rule before
*           write it back to TCAM
*        The invalidation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Invalid indication set.
*        If the given the rule found already in needed valid state
*        no write done. If the given the rule found with size
*        different from the given rule-size an error code returned.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM.
*       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - if in TCAM found rule of size different
*                        from the specified
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
);

/*******************************************************************************
* cpssDxChPclRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssDxChPclRuleCopy should be
*  called. And after this cpssDxChPclRuleInvalidate should be used to invalidate
*  Rule in old position
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of Rule.
*       ruleSrcIndex     - index of the rule in the TCAM from which pattern,
*                           mask and action are taken.
*       ruleDstIndex     - index of the rule in the TCAM to which pattern,
*                           mask and action are placed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleCopy
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
);

/*******************************************************************************
* cpssDxChPclRuleMatchCounterGet
*
* DESCRIPTION:
*       Get rule matching counters.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Index (0..31)
*
* OUTPUTS:
*       counterPtr   - pointer to the counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleMatchCounterGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    OUT GT_U32                        *counterPtr
);

/*******************************************************************************
* cpssDxChPclRuleMatchCounterSet
*
* DESCRIPTION:
*       Set rule matching counters.
*       To reset value of counter use counterValue = 0
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Index (0..31)
*       counterValue - counter value.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleMatchCounterSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    IN  GT_U32                        counterValue
);

/*******************************************************************************
* cpssDxChPclPortIngressPolicyEnable
*
* DESCRIPTION:
*    Enables/disables ingress policy per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*    enable          - GT_TRUE - Ingress Policy enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressPolicyEnable
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPclPortIngressPolicyEnableGet
*
* DESCRIPTION:
*    Get the Enable/Disable ingress policy status per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*
* OUTPUTS:
*    enablePtr       - Pointer to ingress policy status.
*                      GT_TRUE - Ingress Policy is enabled.
*                      GT_FALSE - Ingress Policy is disabled.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressPolicyEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPclPortLookupCfgTabAccessModeSet
*
* DESCRIPTION:
*    Configures VLAN/PORT access mode to Ingress or Egress PCL
*    configuration table per lookup.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*    direction       - Policy direction:
*                      Ingress or Egress
*    lookupNum       - Lookup number:
*                      lookup0 or lookup1
*    subLookupNum    - sub lookup number for lookup.
*                      Supported only for  xCat and above devices.
*                      Ignored for other devices.
*                      xCat and above devices supports two sub lookups only for
*                      ingress lookup CPSS_PCL_LOOKUP_0_E , acceptable range 0..1.
*                      All other lookups have not sub lookups,
*                      acceptable value is 0.
*    mode            - PCL Configuration Table access mode
*                      Port or VLAN ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - for DxCh2 and above - full support,
*   - for DxCh1 supports Ingress direction only and set same access type
*     for both lookups regardless the <lookupNum> parameter value
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortLookupCfgTabAccessModeSet
(
    IN GT_U8                                          devNum,
    IN GT_U8                                          port,
    IN CPSS_PCL_DIRECTION_ENT                         direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT                     lookupNum,
    IN GT_U32                                         subLookupNum,
    IN CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT mode
);

/*******************************************************************************
* cpssDxChPclCfgTblAccessModeSet
*
* DESCRIPTION:
*    Configures global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    accModePtr      - global configuration of access mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*   - for DxCh2 and above - full support,
*   - for DxCh1 supports Ingress direction only with 32 ports per device and the
*     non-local-device-entries-segment-base == 0
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblAccessModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
);

/*******************************************************************************
* cpssDxChPclCfgTblAccessModeGet
*
* DESCRIPTION:
*    Get global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*
* OUTPUTS:
*    accModePtr      - global configuration of access mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*    see cpssDxChPclCfgTblAccessModeSet
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblAccessModeGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
);

/*******************************************************************************
* cpssDxChPclCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: port, VLAN, or index for setting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Standard IPV6 DIP key allowed only on lookup1
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblSet
(
    IN GT_U8                           devNum,
    IN CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
);

/*******************************************************************************
* cpssDxChPclCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: port, VLAN, or index for getting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*
* OUTPUTS:
*       lookupCfgPtr      - (pointer to) lookup configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
);

/*******************************************************************************
* cpssDxChPclIngressPolicyEnable
*
* DESCRIPTION:
*    Enables Ingress Policy.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum           - device number
*    enable           - enable ingress policy
*                      GT_TRUE  - enable,
*                      GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIngressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChPclTcamRuleSizeModeSet
*
* DESCRIPTION:
*    Set TCAM Rules size mode.
*    The rules TCAM may be configured to contain short rules (24 bytes), long
*    rules (48 bytes), or a mix of short rules and long rules.
*    The rule size may be set by global configuration or may be defined
*    dynamical according to PCL Configuration table settings.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*    devNum               - device number
*    mixedRuleSizeMode    - TCAM rule size mode
*                           GT_TRUE  - mixed Rule Size Mode, rule size defined
*                                      by PCL configuration table
*                           GT_FALSE - Rule size defined by ruleSize parameter
*    ruleSize             - Rule size for not mixed mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTcamRuleSizeModeSet
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     mixedRuleSizeMode,
    IN CPSS_PCL_RULE_SIZE_ENT      ruleSize
);

/*******************************************************************************
* cpssDxChPclUdeEtherTypeSet
*
* DESCRIPTION:
*       This function sets UDE Ethertype.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index, range 0..4
*       ethType       - Ethertype value (range 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - on out of range parameter value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclUdeEtherTypeSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    IN  GT_U32          ethType
);

/*******************************************************************************
* cpssDxChPclUdeEtherTypeGet
*
* DESCRIPTION:
*       This function gets the UDE Ethertype.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index, range 0..4
*
* OUTPUTS:
*       ethTypePtr    - points to Ethertype value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclUdeEtherTypeGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_U32          *ethTypePtr
);

/*******************************************************************************
* cpssDxChPclTwoLookupsCpuCodeResolution
*
* DESCRIPTION:
*    Resolve the result CPU Code if both lookups has action commands
*    are either both TRAP or both MIRROR To CPU
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum               - device number
*    lookupNum            - lookup number from which the CPU Code is selected
*                           when action commands are either both TRAP or
*                           both MIRROR To CPU
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTwoLookupsCpuCodeResolution
(
    IN GT_U8                       devNum,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum
);

/*******************************************************************************
* cpssDxChPclInvalidUdbCmdSet
*
* DESCRIPTION:
*    Set the command that is applied when the policy key <User-Defined>
*    field cannot be extracted from the packet due to lack of header
*    depth (i.e, the field resides deeper than 128 bytes into the packet).
*    This command is NOT applied when the policy key <User-Defined>
*    field cannot be extracted due to the offset being relative to a layer
*    start point that does not exist in the packet. (e.g. the offset is relative
*    to the IP header but the packet is a non-IP).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*    udbErrorCmd    - command applied to a packet:
*                      continue lookup, trap to CPU, hard drop or soft drop
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclInvalidUdbCmdSet
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_UDB_ERROR_CMD_ENT  udbErrorCmd
);

/*******************************************************************************
* cpssDxChPclIpLengthCheckModeSet
*
* DESCRIPTION:
*    Set the mode of checking IP packet length.
*    To determine if an IP packet is a valid IP packet, one of the checks is a
*    length check to find out if the total IP length field reported in the
*    IP header is less or equal to the packet's length.
*    This function determines the check mode.
*    The results of this check sets the policy key <IP Header OK>.
*    The result is also used by the router engine to determine whether
*    to forward or trap/drop the packet.
*    There are two check modes:
*     1. For IPv4: ip_total_length <= packet's byte count
*        For IPv6: ip_total_length + 40 <= packet's byte count
*     2. For IPv4: ip_total_length + L3 Offset + 4 (CRC) <= packet's byte count,
*        For IPv6: ip_total_length + 40 +L3 Offset + 4 (CRC) <= packet's
*        byte count
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum           - device number
*    mode             - IP packet length checking mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpLengthCheckModeSet
(
    IN GT_U8                                   devNum,
    IN CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxCh2PclEgressPolicyEnable
*
* DESCRIPTION:
*    Enables Egress Policy.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum           - device number
*    enable           - enable Egress Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported enable parameter value
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh2PclEgressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChPclEgressForRemoteTunnelStartEnableSet
*
* DESCRIPTION:
*       Enable/Disable the Egress PCL processing for the packets,
*       which Tunnel Start already done on remote ingress device.
*       These packets are ingessed with DSA Tag contains source port 60.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
*       enable         - enable Remote Tunnel Start Packets Egress Pcl
*                        GT_TRUE  - enable
*                        GT_FALSE - disable
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressForRemoteTunnelStartEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_BOOL                       enable
);

/*******************************************************************************
* cpssDxChPclEgressForRemoteTunnelStartEnableGet
*
* DESCRIPTION:
*       Gets Enable/Disable of the Egress PCL processing for the packets,
*       which Tunnel Start already done on remote ingress device.
*       These packets are ingessed with DSA Tag contains source port 60.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to) enable Remote Tunnel Start Packets Egress Pcl
*                        GT_TRUE  - enable
*                        GT_FALSE - disable
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressForRemoteTunnelStartEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                       *enablePtr
);

/*******************************************************************************
* cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
*
* DESCRIPTION:
*       Sets Egress Policy Configuration Table Access Mode for
*       Tunnel Start packets
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       cfgTabAccMode  - PCL Configuration Table access mode
*                        Port or VLAN ID
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
(
    IN  GT_U8                                            devNum,
    IN  CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   cfgTabAccMode
);

/*******************************************************************************
* cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
*
* DESCRIPTION:
*       Gets Egress Policy Configuration Table Access Mode for
*       Tunnel Start packets
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum             - device number
* OUTPUTS:
*       cfgTabAccModePtr   - (pointer to) PCL Configuration Table access mode
*                            Port or VLAN ID
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
(
    IN  GT_U8                                            devNum,
    OUT CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   *cfgTabAccModePtr
);

/*******************************************************************************
* cpssDxChPclEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum         - device number
*       vidUpMode      - VID and UP key fields content mode
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
);

/*******************************************************************************
* cpssDxChPclEgressKeyFieldsVidUpModeGet
*
* DESCRIPTION:
*       Gets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum             - device number
* OUTPUTS:
*       vidUpModePtr       - (pointer to) VID and UP key fields
*                            calculation mode
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
);

/*******************************************************************************
* cpssDxCh2EgressPclPacketTypesSet
*
* DESCRIPTION:
*   Enables/disables Egress PCL (EPCL) for set of packet types on port
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*   devNum        - device number
*   port          - port number
*   pktType       - packet type to enable/disable EPCL for it
*   enable        - enable EPCL for specific packet type
*                   GT_TRUE - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*   - after reset EPCL disabled for all packet types on all ports
*
*
*******************************************************************************/
GT_STATUS cpssDxCh2EgressPclPacketTypesSet
(
    IN GT_U8                             devNum,
    IN GT_U8                             port,
    IN CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT pktType,
    IN GT_BOOL                           enable
);

/*******************************************************************************
* cpssDxCh3PclTunnelTermForceVlanModeEnableSet
*
* DESCRIPTION:
*   The function enables/disables forcing of the PCL ID configuration
*   for all TT packets according to the VLAN assignment.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       enable         - force TT packets assigned to PCL
*                        configuration table entry
*                        GT_TRUE  - By VLAN
*                        GT_FALSE - according to ingress port
*                                   per lookup settings
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh3PclTunnelTermForceVlanModeEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_BOOL                       enable
);

/*******************************************************************************
* cpssDxCh3PclTunnelTermForceVlanModeEnableGet
*
* DESCRIPTION:
*   The function gets enable/disable of the forcing of the PCL ID configuration
*   for all TT packets according to the VLAN assignment.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to) force TT packets assigned to PCL
*                        configuration table entry
*                        GT_TRUE  - By VLAN
*                        GT_FALSE - according to ingress port
*                                   per lookup settings
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh3PclTunnelTermForceVlanModeEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                       *enablePtr
);

/*******************************************************************************
* cpssDxCh2PclTcpUdpPortComparatorSet
*
* DESCRIPTION:
*        Configure TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (0-7)
*    l4PortType        - TCP/UDP port type, source or destination port
*    compareOperator   - compare operator FALSE, LTE, GTE, NEQ
*    value             - 16 bit value to compare with
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh2PclTcpUdpPortComparatorSet
(
    IN GT_U8                             devNum,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN GT_U8                             entryIndex,
    IN CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType,
    IN CPSS_COMPARE_OPERATOR_ENT         compareOperator,
    IN GT_U16                            value
);

/*******************************************************************************
* cpssDxChPclRuleStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*       Old function for DxCh1 and DxCh2 devices and for
*       standard rules on DxCh3 devices.
*       The general function is cpssDxChPclRuleAnyStateGet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
);

/*******************************************************************************
* cpssDxChPclRuleAnyStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - size of rule
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
);

/*******************************************************************************
* cpssDxChPclRuleGet
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in Hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*
* OUTPUTS:
*       maskArr          - rule mask          - 14  words.
*       patternArr       - rule pattern       - 14  words.
*       actionArr        - Policy rule action - 4 words.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleGet
(
    IN  GT_U8                  devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 maskArr[],
    OUT GT_U32                 patternArr[],
    OUT GT_U32                 actionArr[]
);

/*******************************************************************************
* cpssDxChPclCfgTblEntryGet
*
* DESCRIPTION:
*    Gets the PCL configuration table entry in Hw format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    direction       - Ingress Policy or Egress Policy
*                        (CPSS_PCL_DIRECTION_ENT member),
*    lookupNum       - Lookup number: lookup0 or lookup1
*                      DxCh1-DxCh3 devices ignores the parameter
*                      xCat and above supports the parameter
*                      xCat and above devices has separate HW entries
*                      for both ingress lookups.
*                      Only lookup 0 is supported for egress.
*    entryIndex      - PCL configuration table entry Index
*
* OUTPUTS:
*    pclCfgTblEntryPtr    - pcl Cfg Tbl Entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_STATE             - in case of already bound pclHwId with same
*                            combination of slot/direction
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       DxCh devices support ingress direction only
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
);

/*******************************************************************************
* cpssDxChPclLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*    enable            - GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
);

/*******************************************************************************
* cpssDxChPclLookupCfgPortListEnableGet
*
* DESCRIPTION:
*        The function gets enable/disable state of
*        using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*    enablePtr         - (pointer to)
*                        GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleFormat       - format of the Rule.
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*
*                          For DxCh and DxCh2 devices:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*                          For DxCh3 device:
*                          The TCAM has 3.5K (3584) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 14335.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 7167.
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n < 3584
*                            is placed in the space of two standard rules with
*                            indexes n and n + 3584.
*                          - Extended rule with index  3584 <= n < 7168
*                            is placed in the space of two standard rules with
*                            indexes n + 3584, n + 7168.
*
*                          For xCat and above devices:
*                          See datasheet of particular device for TCAM size.
*                          For example describe the TCAM that has 1/4K (256) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1K (1024).
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 0.5K (512).
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index 2n (even index)
*                            is placed in the space of two standard rules with
*                            indexes 4n and 4n + 1.
*                          - Extended rule with index 2n+1 (odd index)
*                            is placed in the space of two standard rules with
*                            indexes 4n+2 and 4n + 3.
*
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*                               Write rule to TCAM not using port-list format.
*                               Rule state is valid.
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E -
*                               write all fields of rule to TCAM but rule's
*                               state is invalid (no match during lookups).
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E |
*                                             CPSS_DXCH_PCL_RULE_OPTION_PORT_LIST_ENABLED_E -
*                               write all fields of rule to TCAM using port-list format
*                               but rule's state is invalid
*                              (no match during lookups).
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternPtr       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT      ruleOptionsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleActionGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN  CPSS_PCL_DIRECTION_ENT             direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule.
*   For DxCh and DxCh2 devices start indirect rule write operation
*   with "garbage" pattern and mask content and "valid-flag" == 0
*   For DxCh3 devices calculates the TCAM position by ruleSize and ruleIndex
*   parameters and updates the first "valid" bit matching X/Y pair to (1,1)
*   i.e. don't match any value. If origin of valid or invalid rule found at the
*   specified TCAM position it will be invalid and available to back validation.
*   If the specified TCAM position contains the not first 24 byte segment of
*   extended rule, the rule also will be invalidated,
*   but unavailable to back validation.
*   If the garbage found in TCAM the X/Y bits will be updated to be as
*   in invalid rule.
*   The function does not check the TCAM contents.
*   GT_OK will be returned in each of described cases.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Backward compatible styled API.
*       The cpssDxChPclRuleValidStatusSet recommended for using instead.
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleInvalidate
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule.
*        The validation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Valid indication set.
*           The function does not check content of the rule before
*           write it back to TCAM
*        The invalidation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Invalid indication set.
*        If the given the rule found already in needed valid state
*        no write done. If the given the rule found with size
*        different from the given rule-size an error code returned.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM.
*       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - if in TCAM found rule of size different
*                        from the specified
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleAnyStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize       - size of rule
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssDxChPclPortGroupRuleCopy should be
*  called. And after this cpssDxChPclPortGroupRuleInvalidate should be used to invalidate
*  Rule in old position
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize         - size of Rule.
*       ruleSrcIndex     - index of the rule in the TCAM from which pattern,
*                           mask and action are taken.
*       ruleDstIndex     - index of the rule in the TCAM to which pattern,
*                           mask and action are placed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleCopy
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleGet
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in Hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize         - size of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*
* OUTPUTS:
*       maskArr          - rule mask          - 14  words.
*       patternArr       - rule pattern       - 14  words.
*       actionArr        - Policy rule action - 4 words.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleGet
(
    IN  GT_U8                  devNum,
    IN  GT_PORT_GROUPS_BMP     portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 maskArr[],
    OUT GT_U32                 patternArr[],
    OUT GT_U32                 actionArr[]
);

/*******************************************************************************
* cpssDxChPclPortGroupCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       interfaceInfoPtr  - interface data: port, VLAN, or index for setting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Standard IPV6 DIP key allowed only on lookup1
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupCfgTblSet
(
    IN GT_U8                           devNum,
    IN GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN CPSS_INTERFACE_INFO_STC         *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       interfaceInfoPtr  - interface data: port, VLAN, or index for getting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*
* OUTPUTS:
*       lookupCfgPtr      - (pointer to) lookup configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupCfgTblEntryGet
*
* DESCRIPTION:
*    Gets the PCL configuration table entry in Hw format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction       - Ingress Policy or Egress Policy
*                        (CPSS_PCL_DIRECTION_ENT member),
*    lookupNum       - Lookup number: lookup0 or lookup1
*                      DxCh1-DxCh3 devices ignores the parameter
*                      xCat and above supports the parameter
*                      xCat and above devices has separate HW entries
*                      for both ingress lookups.
*                      Only lookup 0 is supported for egress.
*    entryIndex      - PCL configuration table entry Index
*
* OUTPUTS:
*    pclCfgTblEntryPtr    - pcl Cfg Tbl Entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_STATE             - in case of already bound pclHwId with same
*                            combination of slot/direction
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       DxCh devices support ingress direction only
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupTcpUdpPortComparatorSet
*
* DESCRIPTION:
*        Configure TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (0-7)
*    l4PortType        - TCP/UDP port type, source or destination port

*    compareOperator   - compare operator FALSE, LTE, GTE, NEQ
*    value             - 16 bit value to compare with
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupTcpUdpPortComparatorSet
(
    IN GT_U8                             devNum,
    IN GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN GT_U8                             entryIndex,
    IN CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType,
    IN CPSS_COMPARE_OPERATOR_ENT         compareOperator,
    IN GT_U16                            value
);

/*******************************************************************************
* cpssDxChPclPortGroupUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_OUT_OF_RANGE          - parameter value more then HW bit field
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
);

/*******************************************************************************
* cpssDxChPclPortGroupUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupOverrideUserDefinedBytesSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes
*   by predefined optional key fields.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       udbOverridePtr - (pointer to) UDB override structure
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesSet
(
    IN  GT_U8                          devNum,
    IN  GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_STC *udbOverridePtr
);

/*******************************************************************************
* cpssDxChPclPortGroupOverrideUserDefinedBytesGet
*
* DESCRIPTION:
*   The function gets overriding of  User Defined Bytes
*   by predefined optional key fields.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
* OUTPUTS:
*       udbOverridePtr - (pointer to) UDB override structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesGet
(
    IN  GT_U8                          devNum,
    IN  GT_PORT_GROUPS_BMP             portGroupsBmp,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_STC *udbOverridePtr
);

/*******************************************************************************
* cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes
*   by packets Trunk Hash value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum             - device number
*       portGroupsBmp      - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       udbOverTrunkHashPtr - (pointer to) UDB override trunk hash structure
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       The Trunk Hash field cannot be included in the key of the
*       first IPCL lookup (Lookup0_0).
*       Thus, when the configuration for Trunk Hash in the Policy keys
*       is enabled, it applies only to the second and third lookups.
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet
(
    IN  GT_U8                                     devNum,
    IN  GT_PORT_GROUPS_BMP                        portGroupsBmp,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet
*
* DESCRIPTION:
*   The function gets overriding of  User Defined Bytes
*   by packets Trunk Hash value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum             - device number
*       portGroupsBmp      - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
* OUTPUTS:
*       udbOverTrunkHashPtr - (pointer to) UDB override trunk hash structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet
(
    IN  GT_U8                                     devNum,
    IN  GT_PORT_GROUPS_BMP                        portGroupsBmp,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       vidUpMode      - VID and UP key fields content mode
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  GT_PORT_GROUPS_BMP                         portGroupsBmp,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
);

/*******************************************************************************
* cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
*
* DESCRIPTION:
*       Gets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum             - device number
*       portGroupsBmp      - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
* OUTPUTS:
*       vidUpModePtr       - (pointer to) VID and UP key fields
*                            calculation mode
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    IN   GT_PORT_GROUPS_BMP                         portGroupsBmp,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
);

/*******************************************************************************
* cpssDxChPclPortGroupLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*    enable            - GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
);

/*******************************************************************************
* cpssDxChPclPortGroupLookupCfgPortListEnableGet
*
* DESCRIPTION:
*        The function gets enable/disable state of
*        using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*    enablePtr         - (pointer to)
*                        GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
);

/*******************************************************************************
* cpssDxChPclL3L4ParsingOverMplsEnableSet
*
* DESCRIPTION:
*       If enabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as IP packets for key selection and UDB usage.
*       If disabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as MPLS packets for key selection and UDB usage.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable, GT_FALSE - disable
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclL3L4ParsingOverMplsEnableSet
(
    IN  GT_U8                              devNum,
    IN  GT_BOOL                            enable
);

/*******************************************************************************
* cpssDxChPclL3L4ParsingOverMplsEnableGet
*
* DESCRIPTION:
*       If enabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as IP packets for key selection and UDB usage.
*       If disabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as MPLS packets for key selection and UDB usage.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclL3L4ParsingOverMplsEnableGet
(
    IN  GT_U8                              devNum,
    OUT GT_BOOL                            *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPclh */

