/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCutThrough.h
*
* DESCRIPTION:
*       CPSS DXCH Cut Through facility API.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __cpssDxChCutThroughh
#define __cpssDxChCutThroughh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>

/*******************************************************************************
* cpssDxChCutThroughPortEnableSet
*
* DESCRIPTION:
*      Enable/Disable Cut Through forwarding for packets received on the port.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number including CPU port.
*       enable   - GT_TRUE:  Enable the port for Cut Through.
*                  GT_FALSE: Disable the port for Cut Through.
*       untaggedEnable -  GT_TRUE:  Enable Cut Through forwarding for
*                                   untagged packets received on the port.      
*                         GT_FALSE: Disable Cut Through forwarding for 
*                                    untagged packets received on the port.
*                         Used only if enable == GT_TRUE.
*                         Untagged packets for Cut Through purposes - packets
*                         that don't have VLAN tag or its ethertype isn't equal 
*                         to one of two configurable VLAN ethertypes.
*                         See cpssDxChCutThroughVlanEthertypeSet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       Cut Through forwarding for tagged packets is enabled
*       per source port and UP. See cpssDxChCutThroughUpEnableSet.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughPortEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable,
    IN GT_BOOL  untaggedEnable
);

/*******************************************************************************
* cpssDxChCutThroughPortEnableGet
*
* DESCRIPTION:
*      Get Cut Through forwarding mode on the specified port.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to Cut Through forwarding mode status:
*                    - GT_TRUE:  Enable the port for Cut Through.
*                      GT_FALSE: Disable the port for Cut Through.
*       untaggedEnablePtr - pointer to Cut Through forwarding mode status
*                            for untagged packets.
*                            Used only if *enablePtr == GT_TRUE.
*                            GT_TRUE:  Enable Cut Through forwarding for
*                                      untagged packets received on the port.      
*                            GT_FALSE: Disable Cut Through forwarding for 
*                                      untagged packets received on the port. 
*                         Untagged packets for Cut Through purposes - packets
*                         that don't have VLAN tag or its ethertype isn't equal 
*                         to one of two configurable VLAN ethertypes.
*                         See cpssDxChCutThroughVlanEthertypeSet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughPortEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr,
    OUT GT_BOOL  *untaggedEnablePtr
);

/*******************************************************************************
* cpssDxChCutThroughUpEnableSet
*
* DESCRIPTION:
*       Enable / Disable tagged packets with the specified UP 
*       to be Cut Through.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum          - device number.
*       up              - user priority of a VLAN or DSA tagged
*                         packet [0..7].
*       enable   - GT_TRUE:  tagged packets, with the specified UP
*                            may be subject to Cut Through forwarding.
*                  GT_FALSE: tagged packets, with the specified UP
*                            aren't subject to Cut Through forwarding. 
*                   
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or up.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The ingress port should be enabled for Cut Through forwarding.
*       Use cpssDxChCutThroughPortEnableSet for it.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughUpEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    up,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChCutThroughUpEnableGet
*
* DESCRIPTION:
*      Get Cut Through forwarding mode for tagged packets
*      with the specified UP.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - device number
*       up              - user priority of a VLAN or DSA tagged
*                         packet [0..7].
*
* OUTPUTS:
*       enablePtr   - pointer to Cut Through forwarding mode status 
*                     for tagged packets, with the specified UP.:
*                     - GT_TRUE:  tagged packets, with the specified UP
*                                 may be subject to Cut Through forwarding.
*                       GT_FALSE: tagged packets, with the specified UP
*                                 aren't subject to Cut Through forwarding. 
*                  
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or up
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughUpEnableGet
(
    IN  GT_U8    devNum,
    IN GT_U8     up,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChCutThroughVlanEthertypeSet
*
* DESCRIPTION:
*       Set VLAN Ethertype in order to identify tagged packets.
*       A packed is identified as VLAN tagged for cut-through purposes.
*       Packet considered as tagged if packet's Ethertype equals to one of two 
*       configurable VLAN Ethertypes.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum           - device number.
*       etherType0       - VLAN EtherType0, values 0..0xFFFF
*       etherType1       - VLAN EtherType1, values 0..0xFFFF
*                   
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong etherType0 or etherType1  
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughVlanEthertypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType0,
    IN GT_U32   etherType1
);

/*******************************************************************************
* cpssDxChCutThroughVlanEthertypeGet
*
* DESCRIPTION:
*       Get VLAN Ethertype in order to identify tagged packets.
*       A packed is identified as VLAN tagged for cut-through purposes.
*       Packet considered as tagged if packet's Ethertype equals to one of two 
*       configurable VLAN Ethertypes.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum           - device number.
*                   
* OUTPUTS:
*       etherType0Ptr    - Pointer to VLAN EtherType0
*       etherType1Ptr    - Pointer to VLAN EtherType1. 
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughVlanEthertypeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *etherType0Ptr,
    OUT GT_U32   *etherType1Ptr
);

/*******************************************************************************
* cpssDxChCutThroughMinimalPacketSizeSet
*
* DESCRIPTION:
*       Set minimal packet size that is enabled for Cut Through.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - device number.
*       size        - minimal packet size in bytes for Cut Through [129..16376].
*                     Granularity - 8 bytes.
*                   
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When using cut-through to 1G port, 
*       the minimal packet size should be 512 bytes.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMinimalPacketSizeSet
(
    IN GT_U8    devNum,
    IN GT_U32   size
);

/*******************************************************************************
* cpssDxChCutThroughMinimalPacketSizeGet
*
* DESCRIPTION:
*       Get minimal packet size that is enabled for Cut Through.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - device number.
*                   
* OUTPUTS:
*       sizePtr     - pointer to minimal packet size in bytes for Cut Through.
*                     Granularity - 8 bytes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum 
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMinimalPacketSizeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *sizePtr
);

/*******************************************************************************
* cpssDxChCutThroughMemoryRateLimitSet
*
* DESCRIPTION:
*       Set rate limiting of read operations from the memory
*       per target port in Cut Through mode according to the port speed.
*       To prevent congestion in egress pipe, buffer memory read operations 
*       are rate limited according to the target port speed. 
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number including CPU port.
*       enable       - GT_TRUE - rate limiting is enabled.
*                    - GT_FALSE - rate limiting is disabled.
*       portSpeed    - port speed.
*                   
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portNum or portSpeed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Rate limit is recommended be enabled 
*       on all egress ports for cut-through traffic.
*       Rate limit to the CPU port should be configured as a 1G port. 
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMemoryRateLimitSet
(
    IN GT_U8   devNum,
    IN GT_U8   portNum,
    IN GT_BOOL enable,
    IN CPSS_PORT_SPEED_ENT  portSpeed    
);

/*******************************************************************************
* cpssDxChCutThroughMemoryRateLimitGet
*
* DESCRIPTION:
*       Get rate limiting of read operations from the memory
*       per target port in Cut Through mode.
*       To prevent congestion in egress pipe, buffer memory read operations 
*       are rate limited according to the target port speed. 
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum        - device number.
*       portNum       - physical port number including CPU port.
*                     
* OUTPUTS:            
*       enablePtr     - pointer to rate limiting mode status.    
*                        - GT_TRUE - rate limiting is enabled.
*                        - GT_FALSE - rate limiting is disabled.
*       portSpeedPtr  - pointer to port speed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or portNum
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMemoryRateLimitGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enablePtr,
    OUT CPSS_PORT_SPEED_ENT  *portSpeedPtr    
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ____cpssDxChCutThroughh */




