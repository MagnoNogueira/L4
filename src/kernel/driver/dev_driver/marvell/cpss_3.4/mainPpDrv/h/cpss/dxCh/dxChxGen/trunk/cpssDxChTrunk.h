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
*           1. Set trunk members (enabled,disabled)
*           2. Add/remove member to/from trunk
*           3. Enable/disable member in trunk
*           4. Add/Remove port to/from "non-trunk" entry
*           5. set/unset designated member for Multi-destination traffic and
*              traffic sent to Cascade Trunks
*           6. Set cascade trunk local ports
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
*       $Revision: 13 $
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
 *    CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E - Hash (XOR) is based on the packets data.
 *    CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E - Hash (XOR) is based on the packet's
 *                                       Source Port# or Source Trunk#.
 *                                       On Cascading ports, the Source Port#
 *                                       or Source Trunk# is assigned from the
 *                                       DSA tag.
 *    CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E - CRC Hash based on the packet's data.
 *                                       Note :
 *                                       1. Applicable for Lion and above.
 *                                       2. Using this value will return GT_BAD_PARAM
 *                                          for devices that not support the feature.
 */
typedef enum {
    CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E,
    CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E,
    CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E
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


/*
 * typedef: enum CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT
 *
 *      Description: enumerator for CRC LBH (load balance hash) mode.
 *
 * Enumerations:
 *      CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E  - CRC use 'CRC 6'  ,
 *                                      polynomial: x^6+ x + 1 .
 *      CPSS_DXCH_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E - CRC use 'CRC 16' and Pearson hash ,
 *                                      polynomial: x^16 + x^15 + x^2 + 1 .
 *
 *
 */
typedef enum {
    CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E,
    CPSS_DXCH_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E
}CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT;

/*
 * typedef: structure CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC
 *
 * Description: structure of entry of 'CRC hash mask' table .
 *      === related to feature 'CRC hash mode' ===
 *
 *  in those fields each bit in the bitmap represents a BYTE to be added to hash.
 *
 *  for fields from the packet : the bitmap represent in bit 0 the MSB and in
 *      last bit the LSB (network order bytes)
 *  for UDBs : the bmp represent the UDBs (bit 14 --> UDB 14 , bit 22 UDB 22)
 *
 *  NOTE: the actual write to the HW will be in 'Little Endian' order (which is
 *         reverse to the 'Network order' for fields from the packet)
 *
 *  For L4 dst port the bmp is of 2 bits , because 2 bytes in L4 port
 *      using bit 0 to represent MSB , bit 1 the LSB (network order bytes)
 *  For mpls label 0 (and mpls label 1,mpls label 2) the bmp of 3 bits (for 3 bytes
 *      from label - actually 2.5 MSB)
 *      using bit 0 to represent MSB , bit 1 the second byte ,bit 2 the third byte (LSB) (network order bytes)
 *  For IP_SIP the bmp is of 16 bits , because 128 bits (16 bytes) in ipv6
 *      using bit 0 to represent IP[0] , bit 1 represent IP[1] ..., bit 15 represent IP[15] (network order bytes)
 *      the bits for ipv4 addresses (4 bits) located in bits 12..15 (MSB..LSB) (network order bytes)
 *  For UDBS the bmp is of 32 bits , because 32 UDBs
 *      using bit 0 to represent UDB 0 , bit 1 represent UDB 1 ..., bit 31 represent UDB 31
 *              Lion supports only UDBs 14..22 (bits 14..22 in the bitmap)
 *  For Local source port the bmp is of single bit (bit 0)
 *
 * fields:
 *    l4DstPortMaskBmp  - L4 DST port mask bmp  ( 2 bits for  2 bytes-'Network order')
 *    l4SrcPortMaskBmp  - L4 SRC port mask bmp  ( 2 bits for  2 bytes-'Network order')
 *    ipv6FlowMaskBmp   - IPv6 flow   mask bmp  ( 3 bits for  3 bytes-'Network order')
 *    ipDipMaskBmp      - IPv6/v4 DIP mask bmp  (16 bits for 16 bytes-'Network order')
 *    ipSipMaskBmp      - IPv6/v4 SIP mask bmp  (16 bits for 16 bytes-'Network order')
 *    macDaMaskBmp      - MAC DA      mask bmp  ( 6 bits for  6 bytes-'Network order')
 *    macSaMaskBmp      - MAC SA      mask bmp  ( 6 bits for  6 bytes-'Network order')
 *    mplsLabel0MaskBmp - MPLS label 0 mask bmp ( 3 bits for  3 bytes-'Network order')
 *    mplsLabel1MaskBmp - MPLS label 1 mask bmp ( 3 bits for  3 bytes-'Network order')
 *    mplsLabel2MaskBmp - MPLS label 2 mask bmp ( 3 bits for  3 bytes-'Network order')
 *    localSrcPortMaskBmp - local source port mask bmp (1 bit for 1 byte)
 *    udbsMaskBmp         - UDBs mask bmp (user defined bytes) (32 bits for 32 UDBs)
 *                          lion supports only UDBs 14..22
 *
 */
typedef struct {
    GT_U32      l4DstPortMaskBmp;
    GT_U32      l4SrcPortMaskBmp;
    GT_U32      ipv6FlowMaskBmp;
    GT_U32      ipDipMaskBmp;
    GT_U32      ipSipMaskBmp;
    GT_U32      macDaMaskBmp;
    GT_U32      macSaMaskBmp;
    GT_U32      mplsLabel0MaskBmp;
    GT_U32      mplsLabel1MaskBmp;
    GT_U32      mplsLabel2MaskBmp;
    GT_U32      localSrcPortMaskBmp;
    GT_U32      udbsMaskBmp;
}CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC ;

/*
 * typedef: enum CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT
 *
 * Description: enumeration for how to access the designated trunk table.
 *
 * Enumerations:
 *    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E - force the use of only
 *                              index 0 in the table.(impact multi-destination
 *                              traffic as well as known UC to cascade trunk).
 *    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E - use the computed
 *                              trunk hash that was calculated in ingress.
 *    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E - use the computed
 *                              trunk hash that was calculated in ingress , and
 *                              XOR it with the vid.
 *    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E - The index is based on
 *                              the original source port/trunk, VID and VIDX.
 *                              If the original source interface is a trunks,
 *                              the 6 LSb of the trunk-id are used.
 */
typedef enum {
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E  = GT_FALSE,
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E = GT_TRUE,
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E,
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E
}CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT
 *
 * Description: Enumeration of the 'Source port' hash mode
 *
 * Enumerations:
 *    CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_MODULO_8_E - hash mode
 *                  calculation using %8 (modulo).
 *    CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_MODULO_64_E - hash mode
 *                  calculation using %64 (modulo).
*/
typedef enum{
    CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_MODULO_8_E,
    CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_MODULO_64_E
}CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT;

/*
 * typedef: structure CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC
 *
 * Description: structure for the relation between source port and a port in trunk ,
 *          to specify trunk designated table for 'source port' hashing.
 *          the port in trunk (trunkPort) will receive the traffic destined to the trunk
 *          when sent from the source port (srcPort).
 *
 * fields:
 *  srcPort - source port , a port which traffic that ingress from it ,and should
 *            egress from the trunk(that trunkPort is member in), will choose
 *            trunkPort as the egress port.
 *  trunkPort - 'port in trunk' that need to represent the trunk when traffic from
 *          srcPort sent to this trunk.
 *
*/
typedef struct{
    GT_U8             srcPort;
    GT_U8             trunkPort;
}CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC;

/*******************************************************************************
* cpssDxChTrunkPortTrunkIdSet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunkId field in the port's control register in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* cpssDxChTrunkHashGlobalModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the general hashing mode of trunk hash generation.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       Set the hash mode of the designated trunk table .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       mode - The designated table hashing mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashDesignatedTableModeSet
(
    IN GT_U8   devNum,
    IN CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT    mode
);

/*******************************************************************************
* cpssDxChTrunkHashDesignatedTableModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the hash mode of the designated trunk table .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       modePtr - (pointer to) The designated table hashing mode.
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
    OUT CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT *modePtr
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* cpssDxChTrunkDbTrunkTypeGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Get the trunk type.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number.
*       trunkId     - the trunk id.
*
* OUTPUTS:
*       typePtr - (pointer to) the trunk type
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDbTrunkTypeGet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT CPSS_TRUNK_TYPE_ENT     *typePtr
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
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
* cpssDxChTrunkCascadeTrunkPortsSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function sets the 'cascade' trunk with the specified 'Local ports'
*       overriding any previous setting.
*       The cascade trunk may be invalidated/unset by portsMembersPtr = NULL.
*       Local ports are ports of only configured device.
*       Cascade trunk is:
*           - members are ports of only configured device pointed by devNum
*           - trunk members table is empty (see cpssDxChTrunkTableEntrySet)
*             Therefore it cannot be used as target by ingress engines like FDB,
*             Router, TTI, Ingress PCL and so on.
*           - members ports trunk ID are set (see cpssDxChTrunkPortTrunkIdSet).
*             Therefore packets ingresses in member ports are associated with trunk
*           - all members are enabled only and cannot be disabled.
*           - may be used for cascade traffic and pointed by the 'Device map table'
*             as the local target to reach to the 'Remote device'.
*             (For 'Device map table' refer to cpssDxChCscdDevMapTableSet(...))
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*       portsMembersPtr - (pointer to) local ports bitmap to be members of the
*                   cascade trunk.
*                   NULL - meaning that the trunk-id is 'invalidated' and
*                          trunk-type will be changed to : CPSS_TRUNK_TYPE_FREE_E
*                   not-NULL - meaning that the trunk-type will be : CPSS_TRUNK_TYPE_CASCADE_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - there are ports in the bitmap that not supported by
*                            the device.
*       GT_BAD_PARAM       - bad device number , or bad trunkId number , or number
*                            of ports (in the bitmap) larger then the number of
*                            entries in the 'Designated trunk table'
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_CASCADE_E
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk ,
*                            or this trunk hold members defined using cpssDxChTrunkMembersSet(...)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*   1. This function does not set ports are 'Cascade ports' (and also not
*       check that ports are 'cascade').
*   2. This function sets only next tables :
*       a. the designated trunk table:
*          distribute MC/Cascade trunk traffic among the members
*       b. the 'Non-trunk' table entry.
*       c. 'per port' trunkId
*   3. because this function not set the 'Trunk members' table entry , the
*       application should not point to this trunk from any ingress unit , like:
*       FDB , PCL action redirect , NH , TTI action redirect , PVE ...
*       (it should be pointed ONLY from the device map table)
*   4. this API supports only trunks with types : CPSS_TRUNK_TYPE_FREE_E or
*       CPSS_TRUNK_TYPE_CASCADE_E.
*   5. next APIs are not supported from trunk with type : CPSS_TRUNK_TYPE_CASCADE_E
*       cpssDxChTrunkMembersSet ,
*       cpssDxChTrunkMemberAdd , cpssDxChTrunkMemberRemove,
*       cpssDxChTrunkMemberEnable , cpssDxChTrunkMemberDisable
*       cpssDxChTrunkDbEnabledMembersGet , cpssDxChTrunkDbDisabledMembersGet
*       cpssDxChTrunkDesignatedMemberSet , cpssDxChTrunkDbDesignatedMemberGet
*
********************************************************************************
*   Comparing the 2 function :
*
*        cpssDxChTrunkCascadeTrunkPortsSet   |   cpssDxChTrunkMembersSet
*   ----------------------------------------------------------------------------
*   1. purpose 'Cascade trunk'               | 1. purpose 'Network trunk' , and
*                                            |    also 'Cascade trunk' with up to
*                                            |    8 members
*   ----------------------------------------------------------------------------
*   2. supported number of members depends   | 2. supports up to 8 members
*      on number of entries in the           |    (also in Lion).
*      'Designated trunk table'              |
*      -- Lion supports 64 entries (so up to |
*         64 ports in the 'Cascade trunk').  |
*      -- all other devices supports 8       |
*         entries (so up to 8 ports in the   |
*         'Cascade trunk').                  |
*   ----------------------------------------------------------------------------
*   3. manipulate next trunk tables :        | 3. manipulate all trunk tables :
*      'Per port' trunk-id ,                 |  'Per port' trunk-id , 'Trunk members',
*      'Non-trunk' , 'Designated trunk'      |  'Non-trunk' , 'Designated trunk' tables.
*   ----------------------------------------------------------------------------
*   4. ingress unit must not point to this   | 4. no restriction on ingress/egress
*      trunk (because 'Trunk members' entry  |    units.
*       hold no ports)                       |
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkCascadeTrunkPortsSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *portsMembersPtr
);

/*******************************************************************************
* cpssDxChTrunkCascadeTrunkPortsGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       Gets the 'Local ports' of the 'cascade' trunk .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*
* OUTPUTS:
*       portsMembersPtr - (pointer to) local ports bitmap of the cascade trunk.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PARAM       - bad device number , or bad trunkId number
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_CASCADE_E
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkCascadeTrunkPortsGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    OUT CPSS_PORTS_BMP_STC       *portsMembersPtr
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
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
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
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
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
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
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
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
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
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
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
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

/*******************************************************************************
* cpssDxChTrunkHashCrcParametersSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the CRC hash parameter , relevant for CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E .
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The device number.
*       crcMode - The CRC mode .
*       crcSeed - The seed used by the CRC computation .
*                 when crcMode is CRC_6 mode : crcSeed is in range of 0..0x3f (6 bits value)
*                 when crcMode is CRC_16 mode : crcSeed is in range of 0..0xffff (16 bits value)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or crcMode
*       GT_OUT_OF_RANGE          - crcSeed out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashCrcParametersSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT     crcMode,
    IN GT_U32                               crcSeed
);

/*******************************************************************************
* cpssDxChTrunkHashCrcParametersGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the CRC hash parameter , relevant for CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E .
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The device number.
*
* OUTPUTS:
*       crcModePtr - (pointer to) The CRC mode .
*       crcSeedPtr - (pointer to) The seed used by the CRC computation .
*                 when crcMode is CRC_6 mode : crcSeed is in range of 0..0x3f (6 bits value)
*                 when crcMode is CRC_16 mode : crcSeed is in range of 0..0xffff (16 bits value)
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
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashCrcParametersGet
(
    IN GT_U8                                devNum,
    OUT CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT     *crcModePtr,
    OUT GT_U32                               *crcSeedPtr
);


/*******************************************************************************
* cpssDxChTrunkPortHashMaskInfoSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set port-based hash mask info.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The device number.
*       portNum - The port number.
*       overrideEnable - enable/disable the override
*       index - the index to use when 'Override enabled'.
*               (values 0..15) , relevant only when overrideEnable = GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or portNum
*       GT_OUT_OF_RANGE          - when overrideEnable is enabled and index out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkPortHashMaskInfoSet
(
    IN GT_U8     devNum,
    IN GT_U8     portNum,
    IN GT_BOOL   overrideEnable,
    IN GT_U32    index
);

/*******************************************************************************
* cpssDxChTrunkPortHashMaskInfoGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get port-based hash mask info.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The device number.
*       portNum - The port number.
*
* OUTPUTS:
*       overrideEnablePtr - (pointer to)enable/disable the override
*       indexPtr - (pointer to)the index to use when 'Override enabled'.
*               (values 0..15) , relevant only when overrideEnable = GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or portNum
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkPortHashMaskInfoGet
(
    IN GT_U8     devNum,
    IN GT_U8     portNum,
    OUT GT_BOOL  *overrideEnablePtr,
    OUT GT_U32   *indexPtr
);


/*******************************************************************************
* cpssDxChTrunkHashMaskCrcEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the entry of masks in the specified index in 'CRC hash mask table'.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - the device number.
*       index   - the table index (0..27)
*       entryPtr - (pointer to) The entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_OUT_OF_RANGE          - one of the fields in entryPtr are out of range
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashMaskCrcEntrySet
(
    IN GT_U8                          devNum,
    IN GT_U32                         index,
    IN CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC *entryPtr
);

/*******************************************************************************
* cpssDxChTrunkHashMaskCrcEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the entry of masks in the specified index in 'CRC hash mask table'.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - the device number.
*       index   - the table index (0..27)
*
* OUTPUTS:
*       entryPtr - (pointer to) The entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashMaskCrcEntryGet
(
    IN GT_U8                          devNum,
    IN GT_U32                         index,
    OUT CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC *entryPtr
);

/*******************************************************************************
* cpssDxChTrunkHashPearsonValueSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the Pearson hash value for the specific index.
*
*       NOTE: the Pearson hash used when CRC-16 mode is used.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - the device number.
*       index   - the table index (0..63)
*       value   - the Pearson hash value (0..63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_OUT_OF_RANGE          - value > 63
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashPearsonValueSet
(
    IN GT_U8                          devNum,
    IN GT_U32                         index,
    IN GT_U32                         value
);

/*******************************************************************************
* cpssDxChTrunkHashPearsonValueGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the Pearson hash value for the specific index.
*
*       NOTE: the Pearson hash used when CRC-16 mode is used.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - the device number.
*       index   - the table index (0..63)
*
* OUTPUTS:
*       valuePtr   - (pointer to) the Pearson hash value (0..63)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashPearsonValueGet
(
    IN GT_U8                          devNum,
    IN GT_U32                         index,
    OUT GT_U32                        *valuePtr
);

/*******************************************************************************
* cpssDxChTrunkMcLocalSwitchingEnableSet
*
*       Function Relevant mode : High Level mode
*
* DESCRIPTION:
*       Enable/Disable sending multi-destination packets back to its source
*       trunk on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       enable      - Boolean value:
*                     GT_TRUE  - multi-destination packets may be sent back to
*                                their source trunk on the local device.
*                     GT_FALSE - multi-destination packets are not sent back to
*                                their source trunk on the local device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or bad trunkId number
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. the behavior of multi-destination traffic ingress from trunk is
*       not-affected by setting of cpssDxChBrgVlanLocalSwitchingEnableSet
*       and not-affected by setting of cpssDxChBrgPortEgressMcastLocalEnable
*       2. the functionality manipulates the 'non-trunk' table entry of the trunkId
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkMcLocalSwitchingEnableSet
(
    IN GT_U8            devNum,
    IN GT_TRUNK_ID      trunkId,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChTrunkDbMcLocalSwitchingEnableGet
*
*       Function Relevant mode : High Level mode
*
* DESCRIPTION:
*       Get the current status of Enable/Disable sending multi-destination packets
*       back to its source trunk on the local device.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*
* OUTPUTS:
*       enablePtr   - (pointer to) Boolean value:
*                     GT_TRUE  - multi-destination packets may be sent back to
*                                their source trunk on the local device.
*                     GT_FALSE - multi-destination packets are not sent back to
*                                their source trunk on the local device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_PARAM             - bad device number , or bad trunkId number
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDbMcLocalSwitchingEnableGet
(
    IN GT_U8            devNum,
    IN GT_TRUNK_ID      trunkId,
    OUT GT_BOOL          *enablePtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTrunkh */

