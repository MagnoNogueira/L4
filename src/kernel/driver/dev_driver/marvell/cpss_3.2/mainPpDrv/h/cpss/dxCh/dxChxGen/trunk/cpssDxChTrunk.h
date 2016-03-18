/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssDxChTrunk.h
*
* DESCRIPTION:
*       API definitions for 802.3ad Link Aggregation (Trunk) facility
*       CPSS - DxCh
*
*       States and Modes :
*       A. Application can work in one of 2 modes:
*           1. use "high level" trunk manipulations set of APIs - Mode "HL"
*           2. use "low level" trunk HW tables/registers set of APIs - MODE "LL"
*
*       B. Using "high level" trunk manipulations set of APIs - "High level"
*           Mode.
*           In this mode the CPSS synchronize all the relevant trunk
*           tables/registers, implement some WA for trunks Errata.
*           1. Create trunk
*           2. Destroy trunk
*           3. Add/remove member to/from trunk
*           4. Enable/disable member in trunk
*           5. Add/Remove port to/from "non-trunk" entry
*           6. set/unset designated member for Multi-destination traffic and
*              traffic sent to Cascade Trunks
*        C. Using "low level" trunk HW tables/registers set of APIs- "Low level"
*           Mode.
*           In this mode the CPSS allow direct access to trunk tables/registers.
*           In this mode the Application required to implement the
*           synchronization between the trunk tables/registers and to implement
*           the WA to some of the trunk Errata.
*
*           1. Set per port the trunkId
*           2. Set per trunk the trunk members , number of members
*           3. Set per trunk the Non-trunk local ports bitmap
*           4. Set per Index the designated local ports bitmap
*
*         D. Application responsibility is not to use a mix of using API from
*            those 2 modes
*            The only time that the Application can shift between the 2 modes,
*            is only when there are no trunks in the device.
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/

#ifndef __cpssDxChTrunkh
#define __cpssDxChTrunkh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>

/*
 * typedef: enum CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT
 *
 *      Description: enumerator that hold values for the type of how the CPSS
 *      SW will fill the HW with trunk members .
 *
 * Enumerations:
 *      CPSS_DXCH_TRUNK_MEMBERS_MODE_NATIVE_E - the trunk members are filled
 *                          according to the order given by application.
 *
 *      CPSS_DXCH_TRUNK_MEMBERS_MODE_SOHO_EMULATION_E - the trunk members are
 *                          filled in order to emulate the load balance that the
 *                          SOHO devices use.
 *                          The DX106/7 needs it to work with the SOHO devices.
 *
 */
typedef enum {
    CPSS_DXCH_TRUNK_MEMBERS_MODE_NATIVE_E = 0,
    CPSS_DXCH_TRUNK_MEMBERS_MODE_SOHO_EMULATION_E
}CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT;



/*
 * typedef: enum CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT
 *
 * Description: Enumeration of general LBH (load balance hash) Mode.
 *              the hashing will be according to use of bits from packet, or
 *              according to the ingress port number.
 *
 * Enumerations:
 *    CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E - Hash is based on the packets data.
 *    CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E - Hash is based on the packet's
 *                                       Source Port# or Source Trunk#.
 *                                       On Cascading ports, the Source Port#
 *                                       or Source Trunk# is assigned from the
 *                                       DSA tag.
 */
typedef enum {
    CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E,
    CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E
}CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT;



/*
 * typedef: enum CPSS_TRUNK_IPV6_HASH_MODE_ENT
 *
 * Description: Enumeration of IPv6 Hash Mode.
 *
 * Enumerations:
 *    CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_FLOW_E - Use LSB of SIP, DIP and Flow Label
 *    CPSS_DXCH_TRUNK_IPV6_HASH_MSB_SIP_DIP_FLOW_E - Use MSB of SIP, DIP and Flow Label
 *    CPSS_DXCH_TRUNK_IPV6_HASH_MSB_LSB_SIP_DIP_FLOW_E - Use MSB and LSB of SIP,
 *                                             DIP and Flow Label
 *    CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_E - Use LSB of SIP, DIP
 *
 */
typedef enum {
    CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_FLOW_E = 0,
    CPSS_DXCH_TRUNK_IPV6_HASH_MSB_SIP_DIP_FLOW_E,
    CPSS_DXCH_TRUNK_IPV6_HASH_MSB_LSB_SIP_DIP_FLOW_E,
    CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_E
}CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT
 *
 * Description: Enumeration of L4 LBH (load balance hash) Mode.
 *              the use of bits from TCP/UDP ports (src,dst ports) to generate
 *              trunk member index selection.
 *
 * Enumerations:
 *    CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E - L4 lbh disabled
 *    CPSS_DXCH_TRUNK_L4_LBH_SHORT_E - use short L4 Hash Mode .
 *                               L4 Trunk hash function is based on bits [5:0]
 *                               of the UDP/TCP destination and source ports.
 *    CPSS_DXCH_TRUNK_L4_LBH_LONG_E - use long L4 Hash Mode.
 *                               L4 Trunk hash function is based on bits [5:0]
 *                               and [13:8] of the UDP/TCP destination and
 *                               source ports.
 */
typedef enum {
    CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E,
    CPSS_DXCH_TRUNK_L4_LBH_LONG_E,
    CPSS_DXCH_TRUNK_L4_LBH_SHORT_E
}CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT;


/*
 * typedef: enum CPSS_DXCH_TRUNK_LBH_MASK_ENT
 *
 * Description: Enumeration of trunk LBH (load balance hash) relate fields to
 *              apply mask .
 *
 * Enumerations:
 *    CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E         - mask for MAC DA
 *    CPSS_DXCH_TRUNK_LBH_MASK_MAC_SA_E         - mask for MAC SA
 *    CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E    - mask for MPLS label 0
 *    CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL1_E    - mask for MPLS label 1
 *    CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL2_E    - mask for MPLS label 2
 *    CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E       - mask for IPv4 DIP
 *    CPSS_DXCH_TRUNK_LBH_MASK_IPV4_SIP_E       - mask for IPv4 SIP
 *    CPSS_DXCH_TRUNK_LBH_MASK_IPV6_DIP_E       - mask for IPv6 DIP
 *    CPSS_DXCH_TRUNK_LBH_MASK_IPV6_SIP_E       - mask for IPv6 SIP
 *    CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E      - mask for IPv6 flow
 *    CPSS_DXCH_TRUNK_LBH_MASK_L4_DST_PORT_E    - mask for L4 DST port
 *    CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E    - mask for L4 SRC port
 */
typedef enum {
    CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E,
    CPSS_DXCH_TRUNK_LBH_MASK_MAC_SA_E,
    CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E,
    CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL1_E,
    CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL2_E,
    CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E,
    CPSS_DXCH_TRUNK_LBH_MASK_IPV4_SIP_E,
    CPSS_DXCH_TRUNK_LBH_MASK_IPV6_DIP_E,
    CPSS_DXCH_TRUNK_LBH_MASK_IPV6_SIP_E,
    CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E,
    CPSS_DXCH_TRUNK_LBH_MASK_L4_DST_PORT_E,
    CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E
} CPSS_DXCH_TRUNK_LBH_MASK_ENT;

/*******************************************************************************
* cpssDxChTrunkPortTrunkIdSet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunkId field in the port's control register in the device
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       portNum  - the port number.
*       memberOfTrunk - is the port associated with the trunk
*                 GT_FALSE - the port is set as "not member" in the trunk
*                 GT_TRUE  - the port is set with the trunkId
*
*       trunkId - the trunk to which the port associate with
*                 This field indicates the trunk group number (ID) to which the
*                 port is a member.
*                 1 through 127 = The port is a member of the trunk
*                 this value relevant only when memberOfTrunk = GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad port number , or
*                      bad trunkId number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkPortTrunkIdSet
(
    IN GT_U8            devNum,
    IN GT_U8            portNum,
    IN GT_BOOL          memberOfTrunk,
    IN GT_TRUNK_ID      trunkId
);

/*******************************************************************************
* cpssDxChTrunkPortTrunkIdGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Set the trunkId field in the port's control register in the device
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       portNum  - the port number.
*
* OUTPUTS:
*       memberOfTrunkPtr - (pointer to) is the port associated with the trunk
*                 GT_FALSE - the port is set as "not member" in the trunk
*                 GT_TRUE  - the port is set with the trunkId
*       trunkIdPtr - (pointer to)the trunk to which the port associate with
*                 This field indicates the trunk group number (ID) to which the
*                 port is a member.
*                 1 through 127 = The port is a member of the trunk
*                 this value relevant only when (*memberOfTrunkPtr) = GT_TRUE
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad port number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_STATE - the trunkId value is not synchronized in the 2 registers
*                      that should hold the same value
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkPortTrunkIdGet
(
    IN GT_U8            devNum,
    IN GT_U8            portNum,
    OUT GT_BOOL         *memberOfTrunkPtr,
    OUT GT_TRUNK_ID     *trunkIdPtr
);

/*******************************************************************************
* cpssDxChTrunkTableEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunk table entry , and set the number of members in it.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id
*       numMembers - num of enabled members in the trunk
*                    Note : value 0 is not recommended.
*       membersArray - array of enabled members of the trunk
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE - numMembers exceed the number of maximum number
*                         of members in trunk (total of 0 - 8 members allowed)
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkTableEntrySet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    IN  GT_U32                  numMembers,
    IN  CPSS_TRUNK_MEMBER_STC   membersArray[]
);

/*******************************************************************************
* cpssDxChTrunkTableEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk table entry , and get the number of members in it.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id
*
* OUTPUTS:
*       numMembersPtr - (pointer to)num of members in the trunk
*       membersArray - array of enabled members of the trunk
*                      array is allocated by the caller , it is assumed that
*                      the array can hold CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS
*                      members
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkTableEntryGet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT GT_U32                  *numMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS]
);


/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunk's non-trunk ports specific bitmap entry.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsPtr - (pointer to) non trunk port bitmap of the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkNonTrunkPortsEntrySet
(
    IN  GT_U8               devNum,
    IN  GT_TRUNK_ID         trunkId,
    IN  CPSS_PORTS_BMP_STC  *nonTrunkPortsPtr
);


/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk's non-trunk ports bitmap specific entry.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id - in this API trunkId can be ZERO !
*
* OUTPUTS:
*       nonTrunkPortsPtr - (pointer to) non trunk port bitmap of the trunk.
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkNonTrunkPortsEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_TRUNK_ID         trunkId,
    OUT  CPSS_PORTS_BMP_STC  *nonTrunkPortsPtr
);

/*******************************************************************************
* cpssDxChTrunkDesignatedPortsEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE          - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDesignatedPortsEntrySet
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    IN  CPSS_PORTS_BMP_STC  *designatedPortsPtr
);


/*******************************************************************************
* cpssDxChTrunkDesignatedPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE          - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDesignatedPortsEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    OUT  CPSS_PORTS_BMP_STC  *designatedPortsPtr
);

/*******************************************************************************
* cpssDxChTrunkHashIpModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the IP SIP/DIP information,
*       when calculation the trunk hashing index for a packet.
*       Relevant when the is IPv4 or IPv6 and <TrunkHash Mode> = 0.
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*       enable - GT_FALSE - IP data is not added to the trunk load balancing
*                           hash.
*               GT_TRUE - The following function is added to the trunk load
*                         balancing hash, if the packet is IPv6.
*                         IPTrunkHash = according to setting of API
*                                       cpssDxChTrunkHashIpv6ModeSet(...)
*                         else packet is IPv4:
*                         IPTrunkHash = SIP[5:0]^DIP[5:0]^SIP[21:16]^DIP[21:16].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpModeSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChTrunkHashIpModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get the enable/disable of device from considering the IP SIP/DIP
*       information, when calculation the trunk hashing index for a packet.
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr -(pointer to)
*               GT_FALSE - IP data is not added to the trunk load balancing
*                           hash.
*               GT_TRUE - The following function is added to the trunk load
*                         balancing hash, if the packet is IPv6.
*                         IPTrunkHash = according to setting of API
*                                       cpssDxChTrunkHashIpv6ModeSet(...)
*                         else packet is IPv4:
*                         IPTrunkHash = SIP[5:0]^DIP[5:0]^SIP[21:16]^DIP[21:16].
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpModeGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChTrunkHashL4ModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the L4 TCP/UDP
*       source/destination port information, when calculation the trunk hashing
*       index for a packet.
*
*       Note:
*       1. Not relevant to NON TCP/UDP packets.
*       2. The Ipv4 hash must also be enabled , otherwise the L4 hash mode
*          setting not considered.
*       3. Not relevant to multi-destination packets (include routed IPM).
*       4. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*       hashMode - L4 hash mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashL4ModeSet
(
    IN GT_U8   devNum,
    IN CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT hashMode
);

/*******************************************************************************
* cpssDxChTrunkHashL4ModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get the Enable/Disable of device from considering the L4 TCP/UDP
*       source/destination port information, when calculation the trunk hashing
*       index for a packet.
*
*       Note:
*       1. Not relevant to NON TCP/UDP packets.
*       2. The Ipv4 hash must also be enabled , otherwise the L4 hash mode
*          setting not considered.
*       3. Not relevant to multi-destination packets (include routed IPM).
*       4. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       hashModePtr - (pointer to)L4 hash mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashL4ModeGet
(
    IN GT_U8   devNum,
    OUT CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT *hashModePtr
);

/*******************************************************************************
* cpssDxChTrunkHashIpv6ModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the hash generation function for Ipv6 packets.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum     - the device number.
*       hashMode   - the Ipv6 hash mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or hash mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpv6ModeSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT   hashMode
);

/*******************************************************************************
* cpssDxChTrunkHashIpv6ModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the hash generation function for Ipv6 packets.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum     - the device number.
*
* OUTPUTS:
*       hashModePtr - (pointer to)the Ipv6 hash mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or hash mode
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpv6ModeGet
(
    IN GT_U8                                devNum,
    OUT CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT   *hashModePtr
);

/*******************************************************************************
* cpssDxChTrunkHashIpAddMacModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the use of mac address bits to trunk hash calculation when packet is
*       IP and the "Ip trunk hash mode enabled".
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*       enable  - enable/disable feature
*                   GT_FALSE - If the packet is an IP packet MAC data is not
*                              added to the Trunk load balancing hash.
*                   GT_TRUE - The following function is added to the trunk load
*                             balancing hash:
*                             MACTrunkHash = MAC_SA[5:0]^MAC_DA[5:0].
*
*                             NOTE: When the packet is not an IP packet and
*                             <TrunkLBH Mode> = 0, the trunk load balancing
*                             hash = MACTrunkHash, regardless of this setting.
*                             If the packet is IPv4/6-over-X tunnel-terminated,
*                             the mode is always GT_FALSE (since there is no
*                              passenger packet MAC header).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpAddMacModeSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChTrunkHashIpAddMacModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the use of mac address bits to trunk hash calculation when packet is
*       IP and the "Ip trunk hash mode enabled".
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)enable/disable feature
*                   GT_FALSE - If the packet is an IP packet MAC data is not
*                              added to the Trunk load balancing hash.
*                   GT_TRUE - The following function is added to the trunk load
*                             balancing hash:
*                             MACTrunkHash = MAC_SA[5:0]^MAC_DA[5:0].
*
*                             NOTE: When the packet is not an IP packet and
*                             <TrunkLBH Mode> = 0, the trunk load balancing
*                             hash = MACTrunkHash, regardless of this setting.
*                             If the packet is IPv4/6-over-X tunnel-terminated,
*                             the mode is always GT_FALSE (since there is no
*                              passenger packet MAC header).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpAddMacModeGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);


/*******************************************************************************
* cpssDxChTrunkHashVidMultiDestinationModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       The VID assigned to the packet may be added as part of the distribution
*       hash for accessing the Designated Trunk Port Entry<n> Table to select a
*       designated trunk member for Multi-Destination packets.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*       enable  - GT_FALSE - Don't use the VID. Distribution hash is based on
*                            the packet header or Ingress port only.
*                 GT_TRUE  - In addition to the packet header, the distribution
*                            hash is based on the VID assigned to the packet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or hash mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashVidMultiDestinationModeSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChTrunkHashVidMultiDestinationModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get The VID assigned to the packet may be added as part of the
*       distribution hash for accessing the Designated Trunk Port Entry<n> Table
*       to select a designated trunk member for Multi-Destination packets.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                 GT_FALSE - Don't use the VID. Distribution hash is based on
*                            the packet header or Ingress port only.
*                 GT_TRUE  - In addition to the packet header, the distribution
*                            hash is based on the VID assigned to the packet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or hash mode
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashVidMultiDestinationModeGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChTrunkHashGlobalModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the general hashing mode of trunk hash generation.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*       hashMode   - hash mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or hash mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashGlobalModeSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  hashMode
);

/*******************************************************************************
* cpssDxChTrunkHashGlobalModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the general hashing mode of trunk hash generation.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       hashModePtr   - (pointer to)hash mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashGlobalModeGet
(
    IN GT_U8    devNum,
    OUT CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  *hashModePtr
);

/*******************************************************************************
* cpssDxChTrunkHashDesignatedTableModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device using the entire designated port trunk table
*       or to always use only the first entry.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*       useEntireTable -
*                       GT_TRUE:
*                           The designated hashing uses the entire table.
*                           (one of 8 entries)
*                       GT_FALSE:
*                           The designated hashing use only the first entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashDesignatedTableModeSet
(
    IN GT_U8   devNum,
    IN GT_BOOL useEntireTable
);

/*******************************************************************************
* cpssDxChTrunkHashDesignatedTableModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get the Enable/Disable the device using the entire designated port trunk
*       table or to always use only the first entry.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       useEntireTablePtr - (pointer to)
*                       GT_TRUE:
*                           The designated hashing uses the entire table.
*                           (one of 8 entries)
*                       GT_FALSE:
*                           The designated hashing use only the first entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashDesignatedTableModeGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *useEntireTablePtr
);

/*******************************************************************************
* cpssDxChTrunkDbEnabledMembersGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       return the enabled members of the trunk
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       numOfEnabledMembersPtr - (pointer to) max num of enabled members to
*                                retrieve - this value refer to the number of
*                                members that the array of enabledMembersArray[]
*                                can retrieve.
*
* OUTPUTS:
*       numOfEnabledMembersPtr - (pointer to) the actual num of enabled members
*                      in the trunk (up to CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
*       enabledMembersArray - (array of) enabled members of the trunk
*                             array was allocated by the caller
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDbEnabledMembersGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                *numOfEnabledMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   enabledMembersArray[]
);

/*******************************************************************************
* cpssDxChTrunkDbDisabledMembersGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       return the disabled members of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       numOfDisabledMembersPtr - (pointer to) max num of disabled members to
*                                retrieve - this value refer to the number of
*                                members that the array of enabledMembersArray[]
*                                can retrieve.
*
* OUTPUTS:
*       numOfDisabledMembersPtr -(pointer to) the actual num of disabled members
*                      in the trunk (up to CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
*       disabledMembersArray - (array of) disabled members of the trunk
*                             array was allocated by the caller
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDbDisabledMembersGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                *numOfDisabledMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   disabledMembersArray[]
);

/*******************************************************************************
* cpssDxChTrunkDbIsMemberOfTrunk
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Checks if a member {device,port} is a trunk member.
*       if it is trunk member the function retrieve the trunkId of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum  - the device number.
*       memberPtr - (pointer to) the member to check if is trunk member
*
* OUTPUTS:
*       trunkIdPtr - (pointer to) trunk id of the port .
*                    this pointer allocated by the caller.
*                    this can be NULL pointer if the caller not require the
*                    trunkId(only wanted to know that the member belongs to a
*                    trunk)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_FOUND             - the pair {devNum,port} not a trunk member
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDbIsMemberOfTrunk
(
    IN  GT_U8                   devNum,
    IN  CPSS_TRUNK_MEMBER_STC   *memberPtr,
    OUT GT_TRUNK_ID             *trunkIdPtr
);

/*******************************************************************************
* cpssDxChTrunkDesignatedMemberSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function Configures per-trunk the designated member -
*       value should be stored (to DB) even designated member is not currently
*       a member of Trunk.
*       Setting value replace previously assigned designated member.
*
*       NOTE that:
*       under normal operation this should not be used with cascade Trunks,
*       due to the fact that in this case it affects all types of traffic -
*       not just Multi-destination as in regular Trunks.
*
*  Diagram 1 : Regular operation - Traffic distribution on all enabled
*              members (when no specific designated member defined)
*
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*  index \ member  %   M1  %   M2   %    M3  %  M4  %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 0       %   1   %   0    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 1       %   0   %   1    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 2       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 3       %   0   %   0    %    0   %  1   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 4       %   1   %   0    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 5       %   0   %   1    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 6       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 7       %   0   %   0    %    0   %  1   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*  Diagram 2: Traffic distribution once specific designated member defined
*             (M3 in this case - which is currently enabled member in trunk)
*
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*  index \ member  %   M1  %   M2   %    M3  %  M4  %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 0       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 1       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 2       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 3       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 4       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 5       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 6       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 7       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number
*       trunkId     - the trunk id.
*       enable      - enable/disable designated trunk member.
*                     GT_TRUE -
*                       1. Clears all current Trunk member's designated bits
*                       2. If input designated member, is currently an
*                          enabled-member at Trunk (in local device) enable its
*                          bits on all indexes
*                       3. Store designated member at the DB (new DB parameter
*                          should be created for that)
*
*                     GT_FALSE -
*                       1. Redistribute current Trunk members bits (on all enabled members)
*                       2. Clear designated member at  the DB
*
*       memberPtr   - (pointer to)the designated member we set to the trunk.
*                     relevant only when enable = GT_TRUE
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST         - this member already exists in another trunk.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDesignatedMemberSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_BOOL                  enable,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
);

/*******************************************************************************
* cpssDxChTrunkDbDesignatedMemberGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function get Configuration per-trunk the designated member -
*       value should be stored (to DB) even designated member is not currently
*       a member of Trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number
*       trunkId     - the trunk id.
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable designated trunk member.
*                     GT_TRUE -
*                       1. Clears all current Trunk member's designated bits
*                       2. If input designated member, is currently an
*                          enabled-member at Trunk (in local device) enable its
*                          bits on all indexes
*                       3. Store designated member at the DB (new DB parameter
*                          should be created for that)
*
*                     GT_FALSE -
*                       1. Redistribute current Trunk members bits (on all enabled members)
*                       2. Clear designated member at  the DB
*
*       memberPtr   - (pointer to) the designated member of the trunk.
*                     relevant only when enable = GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDbDesignatedMemberGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    OUT GT_BOOL                 *enablePtr,
    OUT CPSS_TRUNK_MEMBER_STC   *memberPtr
);

/*******************************************************************************
* cpssDxChTrunkMembersSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function set the trunk with the specified enable and disabled
*       members.
*       this setting override the previous setting of the trunk members.
*
*       the user can "invalidate/unset" trunk entry by setting :
*           numOfEnabledMembers = 0 and  numOfDisabledMembers = 0
*
*       This function support next "set entry" options :
*       1. "reset" the entry
*          function will remove the previous settings
*       2. set entry after the entry was empty
*          function will set new settings
*       3. set entry with the same members that it is already hold
*          function will rewrite the HW entries as it was
*       4. set entry with different setting then previous setting
*          a. function will remove the previous settings
*          b. function will set new settings
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           distribute MC/Cascade trunk traffic among the enabled members
*       else
*           1. Set all member ports bits with 0
*           2. If designated member is one of the enabled members, set its relevant
*           bits on all indexes to 1.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*       enabledMembersArray - (array of) members to set in this trunk as enabled
*                     members .
*                    (this parameter ignored if numOfEnabledMembers = 0)
*       numOfEnabledMembers - number of enabled members in the array.
*       disabledMembersArray - (array of) members to set in this trunk as disabled
*                     members .
*                    (this parameter ignored if numOfDisabledMembers = 0)
*       numOfDisabledMembers - number of disabled members in the array.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE - when the sum of number of enabled members + number of
*                         disabled members exceed the number of maximum number
*                         of members in trunk (total of 0 - 8 members allowed)
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST - one of the members already exists in another trunk
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkMembersSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfEnabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    enabledMembersArray[],
    IN GT_U32                   numOfDisabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    disabledMembersArray[]
);

/*******************************************************************************
* cpssDxChTrunkMemberAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function add member to the trunk in the device.
*       If member is already in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members,
*           now taking into account also the added member
*       else
*           1. If added member is not the designated member - set its relevant bits to 0
*           2. If added member is the designated member & it's enabled,
*              set its relevant bits on all indexes to 1.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number on which to add member to the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to add to the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST         - this member already exists in another trunk.
*       GT_FULL - trunk already contains maximum supported members
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkMemberAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
);

/*******************************************************************************
* cpssDxChTrunkMemberRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function remove member from a trunk in the device.
*       If member not exists in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members -
*           now taking into account also the removed member
*       else
*           1. If removed member is not the designated member - nothing to do
*           2. If removed member is the designated member set its relevant bits
*              on all indexes to 1.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number on which to remove member from the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to remove from the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkMemberRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
);

/*******************************************************************************
* cpssDxChTrunkMemberDisable
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function disable (enabled)existing member of trunk in the device.
*       If member is already disabled in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members -
*           now taking into account also the disabled member
*       else
*           1. If disabled member is not the designated member - set its relevant bits to 0
*           2. If disabled member is the designated member set its relevant bits
*               on all indexes to 0.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number on which to disable member in the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to disable in the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_FOUND - this member not found (member not exist) in the trunk
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkMemberDisable
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
);

/*******************************************************************************
* cpssDxChTrunkMemberEnable
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function enable (disabled)existing member of trunk in the device.
*       If member is already enabled in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members -
*           now taking into account also the enabled member
*       else
*           1. If enabled member is not the designated member - set its relevant bits to 0
*           2. If enabled member is the designated member set its relevant bits
*               on all indexes to 1.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number on which to enable member in the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to enable in the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_FOUND - this member not found (member not exist) in the trunk
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkMemberEnable
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
);

/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsAdd
*
* DESCRIPTION:
*       Function Relevant mode : Low Level mode
*
*       add the ports to the trunk's non-trunk entry .
*       NOTE : the ports are add to the "non trunk" table only and not effect
*              other trunk relate tables/registers.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsBmpPtr - (pointer to)bitmap of ports to add to
*                             "non-trunk members"
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkNonTrunkPortsAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsBmpPtr
);

/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsRemove
*
* DESCRIPTION:
*       Function Relevant mode : Low Level mode
*
*      Removes the ports from the trunk's non-trunk entry .
*      NOTE : the ports are removed from the "non trunk" table only and not
*              effect other trunk relate tables/registers.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsBmpPtr - (pointer to)bitmap of ports to remove from
*                             "non-trunk members"
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkNonTrunkPortsRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsBmpPtr
);

/*******************************************************************************
* cpssDxChTrunkInit
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS DxCh Trunk initialization of PP Tables/registers and
*       SW shadow data structures, all ports are set as non-trunk ports.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum             - device number
*       maxNumberOfTrunks  - maximum number of trunk groups.(0..127)
*                            when this number is 0 , there will be no shadow used
*                            Note:
*                            that means that API's that used shadow will FAIL.
*       trunkMembersMode   - type of how the CPSS SW will fill the HW with
*                            trunk members
*
* OUTPUTS:
*           None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_BAD_PARAM             - wrong devNum or bad trunkLbhMode
*       GT_OUT_OF_RANGE          - the numberOfTrunks > 127
*       GT_BAD_STATE             - if library already initialized with different
*                                  number of trunks than requested
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkInit
(
    IN  GT_U8       devNum,
    IN  GT_U8       maxNumberOfTrunks,
    IN  CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT trunkMembersMode
);

/*******************************************************************************
* cpssDxChTrunkHashMplsModeEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the MPLS information,
*       when calculating the trunk hashing index for a packet.
*
*       Note:
*       1. Not relevant to NON-MPLS packets.
*       2. Relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E)
*
* APPLICABLE DEVICES:   DxChXCat and above devices.
*
* INPUTS:
*       devNum  - the device number.
*       enable - GT_FALSE - MPLS parameter are not used in trunk hash index
*                GT_TRUE  - The following function is added to the trunk load
*                   balancing hash:
*               MPLSTrunkHash = (mpls_label0[5:0] & mpls_trunk_lbl0_mask) ^
*                               (mpls_label1[5:0] & mpls_trunk_lbl1_mask) ^
*                               (mpls_label2[5:0] & mpls_trunk_lbl2_mask)
*               NOTE:
*                   If any of MPLS Labels 0:2 do not exist in the packet,
*                   the default value 0x0 is used for TrunkHash calculation
*                   instead.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashMplsModeEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChTrunkHashMplsModeEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get trunk MPLS hash mode
*
* APPLICABLE DEVICES:   DxChXCat and above devices.
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr - (pointer to)the MPLS hash mode.
*               GT_FALSE - MPLS parameter are not used in trunk hash index
*               GT_TRUE  - The following function is added to the trunk load
*                   balancing hash:
*               MPLSTrunkHash = (mpls_label0[5:0] & mpls_trunk_lbl0_mask) ^
*                               (mpls_label1[5:0] & mpls_trunk_lbl1_mask) ^
*                               (mpls_label2[5:0] & mpls_trunk_lbl2_mask)
*               NOTE:
*                   If any of MPLS Labels 0:2 do not exist in the packet,
*                   the default value 0x0 is used for TrunkHash calculation
*                   instead.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashMplsModeEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChTrunkHashMaskSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the masks for the various packet fields being used at the Trunk
*       hash calculations
*
* APPLICABLE DEVICES:   DxChXCat and above devices.
*
* INPUTS:
*       devNum  - the device number.
*       maskedField - field to apply the mask on
*       maskValue - The mask value to be used (0..0x3F)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or maskedField
*       GT_OUT_OF_RANGE          - maskValue > 0x3F
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashMaskSet
(
    IN GT_U8                          devNum,
    IN CPSS_DXCH_TRUNK_LBH_MASK_ENT   maskedField,
    IN GT_U8                          maskValue
);

/*******************************************************************************
* cpssDxChTrunkHashMaskGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the masks for the various packet fields being used at the Trunk
*       hash calculations
*
* APPLICABLE DEVICES:   DxChXCat and above devices.
*
* INPUTS:
*       devNum  - the device number.
*       maskedField - field to apply the mask on
*
* OUTPUTS:
*       maskValuePtr - (pointer to)The mask value to be used (0..0x3F)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or maskedField
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashMaskGet
(
    IN GT_U8                          devNum,
    IN CPSS_DXCH_TRUNK_LBH_MASK_ENT   maskedField,
    OUT GT_U8                         *maskValuePtr
);

/*******************************************************************************
* cpssDxChTrunkHashIpShiftSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the shift being done to IP addresses prior to hash calculations.
*
* APPLICABLE DEVICES:   DxChXCat and above devices.
*
* INPUTS:
*       devNum  - the device number.
*       protocolStack - Set the shift to either IPv4 or IPv6 IP addresses.
*       isSrcIp       - GT_TRUE  = Set the shift to IPv4/6 source addresses.
*                       GT_FALSE = Set the shift to IPv4/6 destination addresses.
*       shiftValue    - The shift to be done.
*                       IPv4 valid shift: 0-3 bytes (Value = 0: no shift).
*                       IPv6 valid shift: 0-15 bytes (Value = 0: no shift).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or protocolStack
*       GT_OUT_OF_RANGE - shiftValue > 3 for IPv4 , shiftValue > 15 for IPv6
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpShiftSet
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL                      isSrcIp,
    IN GT_U32                       shiftValue
);

/*******************************************************************************
* cpssDxChTrunkHashIpShiftGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the shift being done to IP addresses prior to hash calculations.
*
* APPLICABLE DEVICES:   DxChXCat and above devices.
*
* INPUTS:
*       devNum  - the device number.
*       protocolStack - Set the shift to either IPv4 or IPv6 IP addresses.
*       isSrcIp       - GT_TRUE  = Set the shift to IPv4/6 source addresses.
*                       GT_FALSE = Set the shift to IPv4/6 destination addresses.
*
* OUTPUTS:
*       shiftValuePtr    - (pointer to) The shift to be done.
*                       IPv4 valid shift: 0-3 bytes (Value = 0: no shift).
*                       IPv6 valid shift: 0-15 bytes (Value = 0: no shift).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or protocolStack
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpShiftGet
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL                      isSrcIp,
    OUT GT_U32                      *shiftValuePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTrunkh */

