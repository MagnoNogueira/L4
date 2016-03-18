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
*       $Revision: 2 $
*******************************************************************************/

#ifndef __cpssDxChPortPfch
#define __cpssDxChPortPfch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>


/*
 * typedef: enum CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT
 *
 * Description: Enumeration of PFC Counting modes.
 *
 * Enumerations:
 *  CPSS_DXCH_PORT_PFC_COUNT_BYTES_MODE_E  -
 *    Counting packets length in bytes. PFC thresholds are set in bytes. 
 *  CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E - 
 *    Counting packets. PFC thresholds are set in packets.
 *
 * Comments:
 *         None.
 */
typedef enum
{
    CPSS_DXCH_PORT_PFC_COUNT_BYTES_MODE_E,
    CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E
}CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT;

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
*       Enable/Disable PFC (Priority Flow Control) functionality.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: Enable PFC functionality.
*                    GT_FALSE: Disable PFC functionality.
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
GT_STATUS cpssDxChPortPfcEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortPfcEnableGet
*
* DESCRIPTION:
*       Gets the current status of PFC functionality.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of PFC functionality
*                     GT_TRUE:  PFC functionality enabled.
*                     GT_FALSE: PFC functionality disabled.
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
GT_STATUS cpssDxChPortPfcEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortPfcProfileIndexSet
*
* DESCRIPTION:
*       Binds a source port to a PFC profile.
*
* APPLICABLE DEVICES: Lion and above
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
* APPLICABLE DEVICES: Lion and above
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
* APPLICABLE DEVICES: Lion and above
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
*       All thresholds are set in bytes or packets.
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
* APPLICABLE DEVICES: Lion and above
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
*       All thresholds are set in bytes or packets.
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
* APPLICABLE DEVICES: Lion and above
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
* APPLICABLE DEVICES: Lion and above
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
* APPLICABLE DEVICES: Lion and above
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
* APPLICABLE DEVICES: Lion and above
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
* APPLICABLE DEVICES: Lion and above
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
*       2. All thresholds are set in bytes or packets.
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
* APPLICABLE DEVICES: Lion and above
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
*       All thresholds are set in bytes or packets.
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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortPfch */

