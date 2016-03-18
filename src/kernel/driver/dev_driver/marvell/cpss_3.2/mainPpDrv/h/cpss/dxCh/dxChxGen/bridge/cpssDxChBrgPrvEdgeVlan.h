/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgPrvEdgeVlan.h
*
* DESCRIPTION:
*       cpss Dx-Ch implementation for Prestera Private Edge VLANs.
*
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/
#ifndef __cpssDxChBrgPrvEdgeVlanh
#define __cpssDxChBrgPrvEdgeVlanh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanEnable
*
* DESCRIPTION:
*       This function enables/disables the Private Edge VLAN on
*       specified device
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE to enable the feature,
*                GT_FALSE to disable the feature
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortEnable
*
* DESCRIPTION:
*       Enable/Disable a specified port to operate in Private Edge VLAN mode.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number to set state.
*       enable    - GT_TRUE for enabled, GT_FALSE for disabled
*       dstPort   - the destination "uplink" physical port to which all traffic
*                   received on srcPort&srcDev is forwarded.  This parameter
*                   is ignored if disabling the mode.
*       dstDev    - the destination "uplink" physical device to which all
*                   traffic received on srcPort&srcDev is forwarded.  This
*                   parameter is ignored if disabling the mode.
*       dstTrunk  - the destination is a trunk member. This parameter
*                   is ignored if disabling the mode.
*                   GT_TRUE - dstPort hold value of trunkId , and dstDev is
*                             ignored
*                   GT_FALSE - dstPort hold value of port in device dstDev
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad portNum or devNum or dstPort or dstDev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanPortEnable
(
    IN GT_U8      devNum,
    IN GT_U8      portNum,
    IN GT_BOOL    enable,
    IN GT_U8      dstPort,
    IN GT_U8      dstDev,
    IN GT_BOOL    dstTrunk
);

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet
*
* DESCRIPTION:
*       Enable/Disable per port forwarding control taffic to Private Edge
*       VLAN Uplink. PVE port can be disabled from trapping or mirroring
*       bridged packets to the CPU. In this case, as long as the packet is not
*       assigned a HARD DROP or SOFT DROP, the packet is unconditionally
*       assigned a FORWARD command with the ingress ports configured
*       PVE destination (PVLAN Uplink).
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum  - device number
*       port    - physical or CPU port to set.
*       enable  - GT_TRUE  - forward control traffic to PVE Uplink
*                 GT_FALSE - not forward control traffic to PVE Uplink,
*                            trapped or mirrored to CPU packets are sent to
*                            CPU port
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet
*
* DESCRIPTION:
*       Get Enable state per Port forwarding control taffic to Private Edge
*       VLAN Uplink. PVE port can be disabled from trapping or mirroring
*       bridged packets to the CPU. In this case, as long as the packet is not
*       assigned a HARD DROP or SOFT DROP, the packet is unconditionally
*       assigned a FORWARD command with the ingress ports configured
*       PVE destination (PVLAN Uplink).
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum  - device number
*       port    - physical or CPU port to set.
*
* OUTPUTS:
*       enablePtr GT_TRUE  - forward control traffic to PVE Uplink
*                 GT_FALSE - not forward control traffic to PVE Uplink,
*                            trapped or mirrored to CPU packets are sent to
*                            CPU port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChBrgPrvEdgeVlanh */
