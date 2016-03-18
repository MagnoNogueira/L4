/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* cpssDxChTti.h
*
* DESCRIPTION:
*       CPSS tunnel termination declarations.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*
*******************************************************************************/

#ifndef __cpssDxChTtih
#define __cpssDxChTtih

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/tti/cpssDxChTtiTypes.h>
#include <cpss/generic/ip/cpssIpTypes.h>

/*******************************************************************************
* cpssDxChTtiMacToMeSet
*
* DESCRIPTION:
*       This function sets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MACTOME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MACTOME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table (0..7)
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacToMeSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *maskPtr
);

/*******************************************************************************
* cpssDxChTtiMacToMeGet
*
* DESCRIPTION:
*       This function gets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MACTOME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MACTOME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table (0..7).
*
* OUTPUTS:
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacToMeGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *maskPtr
);

/*******************************************************************************
* cpssDxChTtiPortLookupEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for the specified key
*       type at the port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       enable        - GT_TRUE: enable TTI lookup
*                       GT_FALSE: disable TTI lookup
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortLookupEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChTtiPortLookupEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of  the
*       TTI lookup for the specified key type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup
*                       GT_TRUE: TTI lookup is enabled
*                       GT_FALSE: TTI lookup is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id, port or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortLookupEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyTunneledEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only tunneled
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for tunneled packets
*                       GT_FALSE: disable IPv4 TTI lookup only for tunneled packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyTunneledEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyTunneledEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only tunneled packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       tunneled packets
*                       GT_TRUE: IPv4 TTI lookup only for tunneled packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for tunneled packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyTunneledEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for mac to me packets
*                       GT_FALSE: disable IPv4 TTI lookup only for mac to me packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyMacToMeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: IPv4 TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* cpssDxChTtiIpv4McEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for IPv4 multicast
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE: enable TTI lookup for IPv4 MC
*                       GT_FALSE: disable TTI lookup for IPv4 MC
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpv4McEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChTtiIpv4McEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of TTI lookup for
*       IPv4 multicast (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup for IPv4 MC
*                       GT_TRUE: TTI lookup for IPv4 MC enabled
*                       GT_FALSE: TTI lookup for IPv4 MC disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpv4McEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChTtiPortMplsOnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MPLS TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: enable MPLS TTI lookup only for mac to me packets
*                       GT_FALSE: disable MPLS TTI lookup only for mac to me packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMplsOnlyMacToMeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChTtiPortMplsOnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MPLS TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: MPLS TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MPLS TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMplsOnlyMacToMeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* cpssDxChTtiPortMimOnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MIM TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE:  enable MIM TTI lookup only for mac to me packets
*                       GT_FALSE: disable MIM TTI lookup only for mac to me packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMimOnlyMacToMeEnableSet
(
    IN  GT_U8           devNum,
    IN  GT_U8           port,
    IN  GT_BOOL         enable
);

/*******************************************************************************
* cpssDxChTtiPortMimOnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MIM TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable MIM TTI lookup only for
*                       mac to me packets
*                       GT_TRUE:  MIM TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MIM TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMimOnlyMacToMeEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_U8           port,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChTtiRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*       keyType           - TTI key type; valid values:
*                               CPSS_DXCH_TTI_KEY_IPV4_E
*                               CPSS_DXCH_TTI_KEY_MPLS_E
*                               CPSS_DXCH_TTI_KEY_ETH_E
*                               CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 means don't care bit (corresponding
*                           bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionType        - type of the action to use
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
);

/*******************************************************************************
* cpssDxChTtiRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the rule in the TCAM
*       keyType           - TTI key type; valid values:
*                               CPSS_DXCH_TTI_KEY_IPV4_E
*                               CPSS_DXCH_TTI_KEY_MPLS_E
*                               CPSS_DXCH_TTI_KEY_ETH_E
*                               CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       actionType        - type of the action to use
*
* OUTPUTS:
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 means don't care bit (corresponding
*                           bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
);

/*******************************************************************************
* cpssDxChTtiRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       routerTtiTcamRow - Index of the rule in the TCAM
*       actionType        - type of the action to use
*       actionPtr        - points to the TTI rule action that applied on packet
*                          if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
);

/*******************************************************************************
* cpssDxChTtiRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  GT_BOOL                             valid
);

/*******************************************************************************
* cpssDxChTtiRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    OUT GT_BOOL                             *validPtr
);

/*******************************************************************************
* cpssDxChTtiMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       macMode       - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
);

/*******************************************************************************
* cpssDxChTtiMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; Lion; xCat2)
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
);

/*******************************************************************************
* cpssDxChTtiPclIdSet
*
* DESCRIPTION:
*       This function sets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E
*       pclId         - PCL ID value (10 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPclIdSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType,
    IN  GT_U32                          pclId
);

/*******************************************************************************
* cpssDxChTtiPclIdGet
*
* DESCRIPTION:
*       This function gets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E
*
* OUTPUTS:
*       pclIdPtr      - (points to) PCL ID value (10 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPclIdGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType,
    OUT GT_U32                          *pclIdPtr
);

/*******************************************************************************
* cpssDxChTtiIpv4GreEthTypeSet
*
* DESCRIPTION:
*       This function sets the IPv4 GRE protocol Ethernet type. Two Ethernet
*       types are supported, used by TTI for Ethernet over GRE tunnels
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type; valid values:
*                           CPSS_DXCH_TTI_IPV4_GRE0_E
*                           CPSS_DXCH_TTI_IPV4_GRE1_E
*       ethType       - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpv4GreEthTypeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT     greTunnelType,
    IN  GT_U32                              ethType
);

/*******************************************************************************
* cpssDxChTtiIpv4GreEthTypeGet
*
* DESCRIPTION:
*       This function gets the IPv4 GRE protocol Ethernet type. Two Ethernet
*       types are supported, used by TTI for Ethernet over GRE tunnels
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type; valid values:
*                           CPSS_DXCH_TTI_IPV4_GRE0_E
*                           CPSS_DXCH_TTI_IPV4_GRE1_E
*
* OUTPUTS:
*       ethTypePtr    - points to Ethernet type value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or GRE tunnel type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpv4GreEthTypeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT     greTunnelType,
    OUT GT_U32                              *ethTypePtr

);

/*******************************************************************************
* cpssDxChTtiMimEthTypeSet
*
* DESCRIPTION:
*       This function sets the MIM Ethernet type.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       ethType       - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   In xCat and Lion we have 2 registers used for ethertype configuration.
*   One for ethertype identification of incoming tunneled packets in TTI,
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration. These registers are configured to have the same value.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMimEthTypeSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          ethType
);

/*******************************************************************************
* cpssDxChTtiMimEthTypeGet
*
* DESCRIPTION:
*       This function gets the MIM Ethernet type.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       ethTypePtr    - points to Ethernet type value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   In xCat and Lion we have 2 registers used for ethertype configuration.
*   One for ethertype identification of incoming tunneled packets in TTI,
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration. These registers are configured to have the same value.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMimEthTypeGet
(
    IN  GT_U8           devNum,
    OUT GT_U32          *ethTypePtr
);

/*******************************************************************************
* cpssDxChTtiMplsEthTypeSet
*
* DESCRIPTION:
*       This function sets the MPLS Ethernet type.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       ucMcSet       - whether to set it for unicast packets or multicast.
*       ethType       - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   In xCat and Lion we have 2 registers used for ethertype configuration.
*   One for ethertype identification of incoming tunneled packets in TTI,
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration. These registers are configured to have the same value.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMplsEthTypeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet,
    IN  GT_U32                          ethType
);

/*******************************************************************************
* cpssDxChTtiMplsEthTypeGet
*
* DESCRIPTION:
*       This function gets the MPLS Ethernet type.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       ucMcSet       - whether to get it for unicast packets or multicast.
*
* OUTPUTS:
*       ethTypePtr    - points to Ethernet type value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   In xCat and Lion we have 2 registers used for ethertype configuration.
*   One for ethertype identification of incoming tunneled packets in TTI,
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration. These registers are configured to have the same value.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMplsEthTypeGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet,
    OUT GT_U32                          *ethTypePtr
);

/*******************************************************************************
* cpssDxChTtiExceptionCmdSet
*
* DESCRIPTION:
*       Set tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       exceptionType - tunnel termination exception type to set command for
*       command       - command to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E - (APPLICABLE DEVICES: xCat; Lion; xCat2)
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E - (APPLICABLE DEVICES: xCat; Lion; xCat2)
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiExceptionCmdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    IN  CPSS_PACKET_CMD_ENT                 command
);

/*******************************************************************************
* cpssDxChTtiExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       commandPtr    - points to the command for the exception type
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Commands for the different exceptions are:
*
**       CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E - (APPLICABLE DEVICES: xCat; Lion; xCat2)
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E - (APPLICABLE DEVICES: xCat; Lion; xCat2)
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupMacToMeSet
*
* DESCRIPTION:
*       This function sets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MACTOME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MACTOME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       entryIndex        - Index of mac and vlan in MacToMe table (0..7)
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacToMeSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *maskPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupMacToMeGet
*
* DESCRIPTION:
*       This function gets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MACTOME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MACTOME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       entryIndex        - Index of mac and vlan in MacToMe table (0..7).
*
* OUTPUTS:
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacToMeGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *maskPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       macMode       - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacModeSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
);

/*******************************************************************************
* cpssDxChTtiPortGroupMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; Lion; xCat2)
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacModeGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*       keyType           - TTI key type; valid values:
*                               CPSS_DXCH_TTI_KEY_IPV4_E
*                               CPSS_DXCH_TTI_KEY_MPLS_E
*                               CPSS_DXCH_TTI_KEY_ETH_E
*                               CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 means don't care bit (corresponding
*                           bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionType        - type of the action to use
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerTtiTcamRow  - Index of the rule in the TCAM
*       keyType           - TTI key type; valid values:
*                               CPSS_DXCH_TTI_KEY_IPV4_E
*                               CPSS_DXCH_TTI_KEY_MPLS_E
*                               CPSS_DXCH_TTI_KEY_ETH_E
*                               CPSS_DXCH_TTI_KEY_MIM_E   (APPLICABLE DEVICES: xCat; Lion; xCat2)
*       actionType        - type of the action to use
*
* OUTPUTS:
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 means don't care bit (corresponding
*                           bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
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
*       routerTtiTcamRow - Index of the rule in the TCAM
*       actionType       - type of the action to use
*       actionPtr        - points to the TTI rule action that applied on packet
*                          if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
);

/*******************************************************************************
* cpssDxChTtiPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    IN  GT_BOOL                             valid
);

/*******************************************************************************
* cpssDxChTtiPortGroupRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    OUT GT_BOOL                             *validPtr
);

/*******************************************************************************
* cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet
*
* DESCRIPTION:
*       IPv4 Tunnel Termination Header Error exception is detected if ANY of the following criteria are NOT met:
*           - IPv4 header <Checksum> is correct
*           - IPv4 header <Version> = 4
*           - IPv4 header <IHL> (IP Header Length) >= 5 (32-bit words)
*           - IPv4 header <IHL> (IP Header Length) <= IPv4 header <Total Length> / 4
*           - IPv4 header <Total Length> + packet L3 Offset + 4 (CRC length) <= MAC layer packet byte count
*           - IPv4 header <SIP> != IPv4 header <DIP>
*
*       This function globally enables/disables bypassing IPv4 header length criteria checks as part of
*       IPv4 header exception checking.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE:  enable bypass header length check
*                       GT_FALSE: disable bypass header length check
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet
*
* DESCRIPTION:
*       IPv4 Tunnel Termination Header Error exception is detected if ANY of the following criteria are NOT met:
*           - IPv4 header <Checksum> is correct
*           - IPv4 header <Version> = 4
*           - IPv4 header <IHL> (IP Header Length) >= 5 (32-bit words)
*           - IPv4 header <IHL> (IP Header Length) <= IPv4 header <Total Length> / 4
*           - IPv4 header <Total Length> + packet L3 Offset + 4 (CRC length) <= MAC layer packet byte count
*           - IPv4 header <SIP> != IPv4 header <DIP>
*
*       This function gets the globally bypassing IPv4 header length criteria check as part of
*       IPv4 header exception checking.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable bypass header length check
*                       GT_TRUE:  enable bypass header length check
*                       GT_FALSE: disable bypass header length check
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet
(
    IN  GT_U8                               devNum,
    OUT GT_BOOL                             *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTtih */

