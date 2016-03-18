/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortPfc.h
*
* DESCRIPTION:
*       CPSS implementation for Priority Flow Control functionality.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/

#ifndef __cpssDxChPortPfch
#define __cpssDxChPortPfch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>


/*
 * typedef: enum CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT
 *
 * Description: Enumeration of PFC Counting modes.
 *
 * Enumerations:
 *  CPSS_DXCH_PORT_PFC_COUNT_BUFFERS_MODE_E  -
 *    Counting buffers. PFC thresholds are set in buffers. 
 *  CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E - 
 *    Counting packets. PFC thresholds are set in packets.
 *
 * Comments:
 *         None.
 */
typedef enum
{
    CPSS_DXCH_PORT_PFC_COUNT_BUFFERS_MODE_E,
    CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E
}CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT
 *
 * Description: Enumeration of PFC enable options.
 *
 * Enumerations:
 *  CPSS_DXCH_PORT_PFC_DISABLE_ALL_E  - PFC triggering and response are disabled.
 *  CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_ONLY_E - PFC triggering only enabled.
 *  CPSS_DXCH_PORT_PFC_ENABLE_RESPONSE_ONLY_E - PFC response only enabled.
 *  CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E -
 *    Both PFC triggering and response are enabled.
 *
 * Comments:
 *         None.
 */
typedef enum
{
    CPSS_DXCH_PORT_PFC_DISABLE_ALL_E,
    CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_ONLY_E,
    CPSS_DXCH_PORT_PFC_ENABLE_RESPONSE_ONLY_E,
    CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E
}CPSS_DXCH_PORT_PFC_ENABLE_ENT;

/*
 * typedef: struct CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC
 *
 * Description: PFC Profile configurations.
 *
 * Fields:
 *      xonThreshold    - Xon threshold.
 *      xoffThreshold   - Xoff threshold.
 *
 * Comments:
 *      None.
 */
typedef struct {
    GT_U32 xonThreshold;
    GT_U32 xoffThreshold;
} CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC;


/*******************************************************************************
* cpssDxChPortPfcEnableSet
*
* DESCRIPTION:
*       Enable/Disable PFC (Priority Flow Control) triggering or/and response
*       functionality.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       pfcEnable  - PFC enable option.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC enable option
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        If PFC response is enabled, the shaper’s baseline must be
*        at least 0x3FFFC0, see:
*        cpssDxChPortTxShaperBaselineSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcEnableSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_PFC_ENABLE_ENT pfcEnable
);

/*******************************************************************************
* cpssDxChPortPfcEnableGet
*
* DESCRIPTION:
*       Get the status of PFC (Priority Flow Control) triggering or/and response
*       functionality.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       pfcEnablePtr  - (pointer to) PFC enable option.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcEnableGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_PFC_ENABLE_ENT *pfcEnablePtr
);

/*******************************************************************************
* cpssDxChPortPfcProfileIndexSet
*
* DESCRIPTION:
*       Binds a source port to a PFC profile.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       profileIndex - profile index (0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileIndexSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   profileIndex
);

/*******************************************************************************
* cpssDxChPortPfcProfileIndexGet
*
* DESCRIPTION:
*       Gets the port's PFC profile.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U32  *profileIndexPtr
);

/*******************************************************************************
* cpssDxChPortPfcProfileQueueConfigSet
*
* DESCRIPTION:
*       Sets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*       pfcProfileCfgPtr - pointer to PFC Profile configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileQueueConfigSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U8    tcQueue,
    IN CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     *pfcProfileCfgPtr
);

/*******************************************************************************
* cpssDxChPortPfcProfileQueueConfigGet
*
* DESCRIPTION:
*       Gets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       pfcProfileCfgPtr - pointer to PFC Profile configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileQueueConfigGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    IN  GT_U8    tcQueue,
    OUT CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     *pfcProfileCfgPtr
);


/*******************************************************************************
* cpssDxChPortPfcCountingModeSet
*
* DESCRIPTION:
*       Sets PFC counting mode.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       pfcCountMode - PFC counting mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcCountingModeSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  pfcCountMode
);

/*******************************************************************************
* cpssDxChPortPfcCountingModeGet
*
* DESCRIPTION:
*       Gets PFC counting mode.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       pfcCountModePtr - (pointer to) PFC counting mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcCountingModeGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  *pfcCountModePtr
);


/*******************************************************************************
* cpssDxChPortPfcGlobalDropEnableSet
*
* DESCRIPTION:
*       Enable/Disable PFC global drop.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: Enable PFC global drop.
*                    GT_FALSE: Disable PFC global drop.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To configure drop threshold use cpssDxChPortPfcGlobalQueueConfigSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalDropEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortPfcGlobalDropEnableGet
*
* DESCRIPTION:
*       Gets the current status of PFC global drop.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of PFC functionality
*                     GT_TRUE:  PFC global drop enabled.
*                     GT_FALSE: PFC global drop disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalDropEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortPfcGlobalQueueConfigSet
*
* DESCRIPTION:
*       Sets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum        - device number.
*       tcQueue       - traffic class queue (0..7).
*       xoffThreshold - Xoff threshold (0..0x7FF).
*       dropThreshold - Drop threshold. When a global counter with given tcQueue
*               crosses up the dropThreshold the packets are dropped (0..0x7FF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. dropThreshold is used when PFC global drop is enabled.
*       See cpssDxChPortPfcGlobalDropEnableSet.
*       2. All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalQueueConfigSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue,
    IN GT_U32   xoffThreshold,
    IN GT_U32   dropThreshold
);

/*******************************************************************************
* cpssDxChPortPfcGlobalQueueConfigGet
*
* DESCRIPTION:
*       Gets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       xoffThresholdPtr - (pointer to) Xoff threshold.
*       dropThresholdPtr - (pointer to) Drop threshold. When a global counter with given tcQueue
*                       crosses up the dropThreshold the packets are dropped.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalQueueConfigGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    tcQueue,
    OUT GT_U32   *xoffThresholdPtr,
    OUT GT_U32   *dropThresholdPtr
);

/*******************************************************************************
* cpssDxChPortPfcTimerMapEnableSet
*
* DESCRIPTION:
*       Enables mapping of PFC timer to priority queue for given scheduler profile.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       profileSet - the Tx Queue scheduler profile.
*       enable     - Determines whether PFC timer to Priority Queue map
*                    is used.
*                    GT_TRUE: PFC timer to Priority Queue map used.
*                    GT_FALSE: PFC timer to Priority Queue map bypassed.
*                    1:1 mapping between a timer in PFC frame to an egress queue. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to scheduler profile use:
*          cpssDxChPortTxBindPortToSchedulerProfileSet.
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerMapEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_BOOL                                 enable

);

/*******************************************************************************
* cpssDxChPortPfcTimerMapEnableGet
*
* DESCRIPTION:
*       Get the status of PFS timer to priority queue mapping for given
*       scheduler profile.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       profileSet - the Tx Queue scheduler profile Set
*
* OUTPUTS:
*       enablePtr   - (pointer to) status of PFC timer to Priority Queue 
*                      mapping.
*                    GT_TRUE: PFC timer to Priority Queue map used.
*                    GT_FALSE: PFC timer to Priority Queue map bypassed.
*                    1:1 mapping between a timer in PFC frame to an egress queue. 
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerMapEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    OUT GT_BOOL                                *enablePtr

);

/*******************************************************************************
* cpssDxChPortPfcTimerToQueueMapSet
*
* DESCRIPTION:
*       Sets PFC timer to priority queue map.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       pfcTimer     - PFC timer (0..7)
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC timer
*       GT_OUT_OF_RANGE          - on out of traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*      
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerToQueueMapSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    pfcTimer,
    IN  GT_U32    tcQueue

);

/*******************************************************************************
* cpssDxChPortPfcTimerToQueueMapGet
*
* DESCRIPTION:
*       Gets PFC timer to priority queue map.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       pfcTimer        - PFC timer (0..7).
*
* OUTPUTS:
*       tcQueuePtr      - (pointer to) traffic class queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC timer
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*       
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerToQueueMapGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    pfcTimer,
    OUT GT_U32    *tcQueuePtr

);

/*******************************************************************************
* cpssDxChPortPfcShaperToPortRateRatioSet
*
* DESCRIPTION:
*       Sets shaper rate to port speed ratio on given scheduler profile
*       and traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum                - device number.
*       profileSet            - the Tx Queue scheduler profile.
*       tcQueue               - traffic class queue (0..7).
*       shaperToPortRateRatio - shaper rate to port speed ratio 
*                               in percentage (0..100).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number ,profile set
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range shaper rate to port speed ratio
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to scheduler profile use:
*          cpssDxChPortTxBindPortToSchedulerProfileSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcShaperToPortRateRatioSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_U8                                   tcQueue,
    IN  GT_U32                                  shaperToPortRateRatio

);

/*******************************************************************************
* cpssDxChPortPfcShaperToPortRateRatioGet
*
* DESCRIPTION:
*       Gets shaper rate to port speed ratio on given scheduler profile
*       and traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum                - device number.
*       profileSet            - the Tx Queue scheduler profile.
*       tcQueue               - traffic class queue (0..7).
*
* OUTPUTS:
*       shaperToPortRateRatioPtr - (pointer to)shaper rate to port speed ratio 
*                               in percentage.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number ,profile set
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcShaperToPortRateRatioGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_U8                                   tcQueue,
    OUT GT_U32                                 *shaperToPortRateRatioPtr

);

/*******************************************************************************
* cpssDxChPortPfcForwardEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of PFC frames to the ingress 
*       pipeline of a specified port. Processing of unknown MAC control frames 
*       is done like regular data frames if forwarding enabled.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum  - device number.
*       portNum - physical port number (CPU port not supported)
*       enable  - GT_TRUE:  forward PFC frames to the ingress pipe,
*                 GT_FALSE: do not forward PFC frames to the ingress pipe.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   A packet is considered as a PFC frame if all of the following are true:
*   - Packet’s Length/EtherType field is 88-08
*   - Packet’s OpCode field is 01-01
*   - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcForwardEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL enable
);


/*******************************************************************************
* cpssDxChPortPfcForwardEnableGet
*
* DESCRIPTION:
*       Get status of PFC frames forwarding
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (CPU port not supported)
*
* OUTPUTS:
*       enablePtr   - current forward PFC frames status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*   A packet is considered as a PFC frame if all of the following are true:
*   - Packet’s Length/EtherType field is 88-08
*   - Packet’s OpCode field is 01-01
*   - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcForwardEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortPfch */

