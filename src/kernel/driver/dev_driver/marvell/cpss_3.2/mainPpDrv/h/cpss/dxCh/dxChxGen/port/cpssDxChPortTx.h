/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortTx.h
*
* DESCRIPTION:
*       CPSS implementation for configuring the Physical Port Tx Traffic Class
*       Queues.
*       This covers:
*       - queuing enable/disable
*       - transmit enable/disable
*       - scheduling algorithms, bandwidth management
*       - shaping on queues and the logical port
*       - drop algorithms for congestion handling
*
* FILE REVISION NUMBER:
*       $Revision: 19 $
*******************************************************************************/

#ifndef __cpssDxChPortTxh
#define __cpssDxChPortTxh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortTx.h>
#include <cpss/generic/cos/cpssCosTypes.h>

/*
 * typedef: enum DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT
 *
 * Description:  Token bucket maximum transmission unit (MTU)
 *
 * Enumerations:
 *      CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E  - 1.5K
 *      CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_2K_E    - 2K
 *      CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_10K_E   - 10K
 */

typedef enum
{
    CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E,
    CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_2K_E,
    CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_10K_E
}CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT;

/*
 * typedef: CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT
 *
 * Description:  Enumeration of shared pool allocation policy
 *
 * Enumerations:
 *     CPSS_DXCH_PORT_TX_SHARED_POLICY_UNCONSTRAINED_E -
 *          Unconstrained. A port may consume the entire shared pool.
 *     CPSS_DXCH_PORT_TX_SHARED_POLICY_CONSTRAINED_E  -
 *          Constrained. The Port Tail Drop Threshold limits the total
 *          number of resources occupied by a port.
 */

typedef enum
{
    CPSS_DXCH_PORT_TX_SHARED_POLICY_UNCONSTRAINED_E,
    CPSS_DXCH_PORT_TX_SHARED_POLICY_CONSTRAINED_E
}CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT;

/*******************************************************************************
* cpssDxChPortTxInit
*
* DESCRIPTION:
*       Init port Tx configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In SALSA family the first profile (profile_0) is used for CPU port and
*       the second one (profile_1) for network ports.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxInit
(
    IN  GT_U8    devNum
);

/*******************************************************************************
* cpssDxChPortTxQueueEnableSet
*
* DESCRIPTION:
*       Enable/Disable enqueuing on all Tx queues on a specific device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum - physical device number
*       enable - GT_TRUE, Global enqueuing enabled
*                GT_FALSE, Global enqueuing disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxQueueingEnableSet
*
* DESCRIPTION:
*       Enable/Disable enqueuing to a Traffic Class queue
*       on the specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this device
*       enable  - GT_TRUE, enable enqueuing to the queue
*                 GT_FALSE, disable enqueuing to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxQueueTxEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmission from a Traffic Class queue
*       on the specified port of specified device.
*
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this Physical Port
*       enable  - GT_TRUE, enable transmission from the queue
*                GT_FALSE, disable transmission from the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueTxEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxFlushQueuesSet
*
* DESCRIPTION:
*       Flush all the traffic class queues on the specified port of
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum - physical device number
*       portNum- physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxFlushQueuesSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum
);

/*******************************************************************************
* cpssDxChPortTxShaperEnableSet
*
* DESCRIPTION:
*       Enable/Disable Token Bucket rate shaping on specified port of
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       enable  - GT_TRUE, enable Shaping
*                 GT_FALSE, disable Shaping
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxShaperEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable Token Bucket rate shaping status on specified port of
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       enablePtr - Pointer to Token Bucket rate shaping status. 
*                   - GT_TRUE, enable Shaping
*                   - GT_FALSE, disable Shaping
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT  GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxShaperProfileSet
*
* DESCRIPTION:
*       Set Token Bucket Shaper Profile on specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       burstSize  - burst size in units of 4K bytes
*                    (max value is 4K which results in 16K burst size)
*       maxRatePtr - Requested Rate in Kbps
*
* OUTPUTS:
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For port number in the range 0..23, whether G or XG type, the global
*       shaper used parameters are those referenced as gig ones -
*       "gigPortsTokensRate" and "gigPortsSlowRateRatio" inputs for
*       "cpssDxChPortTxShaperGlobalParamsSet" API.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperProfileSet
(
    IN    GT_U8     devNum,
    IN    GT_U8     portNum,
    IN    GT_U16    burstSize,
    INOUT GT_U32    *maxRatePtr
);

/*******************************************************************************
* cpssDxChPortTxShaperProfileGet
*
* DESCRIPTION:
*       Get Token Bucket Shaper Profile on specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       burstSizePtr - (pinter to) burst size in units of 4K bytes
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps.
*       enablePtr    - (pointer to) Tocken Bucket rate shaping status.
*                           - GT_TRUE - Tocken Bucket rate shaping is enabled.
*                           - GT_FALSE - Tocken Bucket rate shaping is disabled.
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperProfileGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U16  *burstSizePtr,
    OUT GT_U32  *maxRatePtr,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxQShaperEnableSet
*
* DESCRIPTION:
*       Enable/Disable shaping of transmitted traffic from a specified Traffic
*       Class Queue and specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this port
*       enable  - GT_TRUE, enable shaping on this traffic queue
*                 GT_FALSE, disable shaping on this traffic queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxQShaperEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable shaping status
*       of transmitted traffic from a specified Traffic
*       Class Queue and specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this port
*
* OUTPUTS:
*       enablePtr  - Pointer to  shaping status 
*                    - GT_TRUE, enable shaping on this traffic queue
*                    - GT_FALSE, disable shaping on this traffic queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxQShaperProfileSet
*
* DESCRIPTION:
*       Set Shaper Profile for Traffic Class Queue of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - physical port number
*       tcQueue      - traffic class queue on this Logical Port
*       burstSize    - burst size in units of 4K bytes
*                      (max value is 4K which results in 16K burst size)
*       userRatePtr  - Requested Rate in Kbps
*
* OUTPUTS:
*       userRatePtr  - (pointer to) the actual Rate value in Kbps
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For port number in the range 0..23, whether G or XG type, the global
*       shaper used parameters are those referenced as gig ones -
*       "gigPortsTokensRate" and "gigPortsSlowRateRatio" inputs for
*       "cpssDxChPortTxShaperGlobalParamsSet" API.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperProfileSet
(
    IN    GT_U8     devNum,
    IN    GT_U8     portNum,
    IN    GT_U8     tcQueue,
    IN    GT_U16    burstSize,
    INOUT GT_U32    *userRatePtr
);

/*******************************************************************************
* cpssDxChPortTxQShaperProfileGet
*
* DESCRIPTION:
*       Get Shaper Profile for Traffic Class Queue of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       tcQueue    - traffic class queue on the port
*
* OUTPUTS:
*       burstSizePtr - (pinter to) burst size in units of 4K bytes
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps.
*       enablePtr    - (pointer to) Tocken Bucket rate shaping status.
*                           - GT_TRUE - Tocken Bucket rate shaping is enabled.
*                           - GT_FALSE - Tocken Bucket rate shaping is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperProfileGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_U8    tcQueue,
    OUT GT_U16   *burstSizePtr,
    OUT GT_U32   *maxRatePtr,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxBindPortToSchedulerProfileSet
*
* DESCRIPTION:
*       Bind a port to scheduler profile set.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       profileSet - The Profile Set in which the scheduler's parameters are
*                    associated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxBindPortToSchedulerProfileSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   portNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet
);

/*******************************************************************************
* cpssDxChPortTxBindPortToSchedulerProfileGet
*
* DESCRIPTION:
*       Get scheduler profile set that is binded to the port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       profileSetPtr - The Profile Set in which the scheduler's parameters are
*                        associated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxBindPortToSchedulerProfileGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   portNum,
    OUT CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  *profileSetPtr
);

/*******************************************************************************
* cpssDxChPortTx4TcTailDropProfileSet
*
* DESCRIPTION:
*       Set tail drop profiles limits for particular TC.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters.
*       tailDropProfileParamsPtr -
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcTailDropProfileSet
(
    IN    GT_U8                                   devNum,
    IN    CPSS_PORT_TX_DROP_PROFILE_SET_ENT       profileSet,
    IN    GT_U8                                   trafficClass,
    IN    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *tailDropProfileParamsPtr
);
/*******************************************************************************
* cpssDxChPortTx4TcTailDropProfileGet
*
* DESCRIPTION:
*       Get tail drop profiles limits for particular TC.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters.
*
* OUTPUTS:
*       tailDropProfileParamsPtr - Pointer to
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcTailDropProfileGet
(
    IN    GT_U8                                   devNum,
    IN    CPSS_PORT_TX_DROP_PROFILE_SET_ENT       profileSet,
    IN    GT_U8                                   trafficClass,
    OUT   CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *tailDropProfileParamsPtr
);

/*******************************************************************************
* cpssDxChPortTxTailDropProfileSet
*
* DESCRIPTION:
*       Enables/Disables sharing of buffers and descriptors for all queues of an
*       egress port and set maximal port's limits of buffers and descriptors.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum        - physical device number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*       enableSharing - If GT_TRUE Sharing is enabled. If a packet is to be queued
*                      to an empty queue, it is queued even if the number of buffers
*                       or descriptors allocated to this port exceeds the
*                       < portMaxBuffLimit > and < portMaxDescrLimit >.
*                       If GT_FALSE Sharing is disabled. If the sum of buffers or
*                       descriptors allocated per queue is larger than the number of
*                       buffers or descriptors allocated to this port
*                       < portMaxBuffLimit > and < portMaxDescrLimit >, then some of
*                       the queues may be starved.
*                       DxCh2 and above devices support only GT_FALSE value.
*       portMaxBuffLimit - maximal number of buffers for a port.
*                          For CH & CH2: range 0..4095
*                          For CH3 and above: range 0..16383
*       portMaxDescrLimit - maximal number of descriptors for a port.
*                           For CH & CH2: range 0..4095
*                           For CH3 and above: range 0..16383
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropProfileSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  GT_BOOL                             enableSharing,
    IN  GT_U16                              portMaxBuffLimit,
    IN  GT_U16                              portMaxDescrLimit
);
/*******************************************************************************
* cpssDxChPortTxTailDropProfileGet
*
* DESCRIPTION:
*       Get sharing status of buffers and descriptors for all queues of an
*       egress port and get maximal port's limits of buffers and descriptors.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum        - physical device number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       enableSharingPtr - Pointer to sharing status.
*                      If GT_TRUE Sharing is enabled. If a packet is to be queued
*                      to an empty queue, it is queued even if the number of buffers
*                       or descriptors allocated to this port exceeds the
*                       < portMaxBuffLimit > and < portMaxDescrLimit >.
*                       If GT_FALSE Sharing is disabled. If the sum of buffers or
*                       descriptors allocated per queue is larger than the number of
*                       buffers or descriptors allocated to this port
*                       < portMaxBuffLimit > and < portMaxDescrLimit >, then some of
*                       the queues may be starved.
*                       DxCh2 and above devices support only GT_FALSE value.
*       portMaxBuffLimitPtr - Pointer to maximal number of buffers for a port
*       portMaxDescrLimitPtr - Pointer to maximal number of descriptors for a port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropProfileGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    OUT GT_BOOL                             *enableSharingPtr,
    OUT GT_U16                              *portMaxBuffLimitPtr,
    OUT GT_U16                              *portMaxDescrLimitPtr
);

/*******************************************************************************
* cpssDxChPortTxSniffedPcktDescrLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for mirrored packets.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum            - physical device number
*       rxSniffMaxDescNum - The number of descriptors allocated for packets
*                           forwarded to the ingress analyzer port due to
*                           mirroring.
*                           For DxCh,DxCh2: range 0..0xFFF
*                           For DxCh3 and above: range 0..0x3FFF
*       txSniffMaxDescNum - The number of descriptors allocated for packets
*                           forwarded to the egress analyzer port due to
*                           mirroring.
*                           For DxCh,DxCh2: range 0..0xFFF
*                           For DxCh3 and above: range 0..0x3FFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedPcktDescrLimitSet
(
    IN    GT_U8                  devNum,
    IN    GT_U32                 rxSniffMaxDescNum,
    IN    GT_U32                 txSniffMaxDescNum
);

/*******************************************************************************
* cpssDxChPortTxSniffedPcktDescrLimitGet
*
* DESCRIPTION:
*       Get maximal descriptors limits for mirrored packets.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum            - physical device number
*
* OUTPUTS:
*       rxSniffMaxDescNumPtr - (pointer to) The number of descriptors allocated
*                              for packets forwarded to the ingress analyzer
*                              port due to mirroring.
*       txSniffMaxDescNumPtr - (pointer to) The number of descriptors allocated
*                              for packets forwarded to the egress analyzer
*                              port due to mirroring.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedPcktDescrLimitGet
(
    IN    GT_U8                  devNum,
    OUT   GT_U32                *rxSniffMaxDescNumPtr,
    OUT   GT_U32                *txSniffMaxDescNumPtr
);

/*******************************************************************************
* cpssDxChPortTxMcastPcktDescrLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for multicast packets.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum            - physical device number
*       mcastMaxDescNum   - The number of descriptors allocated for multicast
*                           packets in units of 128 descriptors, the actual number descriptors
*                           allocated will be 128 * mcastMaxDescNum.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None. 
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastPcktDescrLimitSet
(
    IN    GT_U8                  devNum,
    IN    GT_U32                 mcastMaxDescNum
);

/*******************************************************************************
* cpssDxChPortTxMcastPcktDescrLimitGet
*
* DESCRIPTION:
*       Get maximal descriptors limits for multicast packets.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       mcastMaxDescNumPtr  - (pointer to) the number of descriptors allocated for multicast
*                             packets in units of 128 descriptors, the actual number descriptors
*                             allocated will be 128 * mcastMaxDescNumPtr.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastPcktDescrLimitGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *mcastMaxDescNumPtr
);

/*******************************************************************************
* cpssDxChPortTxWrrGlobalParamSet
*
* DESCRIPTION:
*       Set global parameters for WRR scheduler.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       wrrMode - Transmit Scheduler count mode.
*       wrrMtu  - MTU for DSWRR, resolution of WRR weights for byte based
*                 Transmit Scheduler count mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxWrrGlobalParamSet
(
    IN    GT_U8                     devNum,
    IN    CPSS_PORT_TX_WRR_MODE_ENT wrrMode,
    IN    CPSS_PORT_TX_WRR_MTU_ENT  wrrMtu
);


/*******************************************************************************
* cpssDxChPortTxShaperGlobalParamsSet
*
* DESCRIPTION:
*       Set Global parameters for shaper.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum               - physical device number
*       xgPortsTokensRate    - tokens rate for XG ports' shapers
*       gigPortsTokensRate   - tokens rate for Tri Speed ports' shapers
*       gigPortsSlowRateRatio - slow rate ratio for Tri Speed ports. Tokens Update
*                              rate for ports with slow rate divided to the ratio.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For devices the following values are valid.
*       xgPortsTokensRate    - <1-15>
*       gigPortsTokensRate   - <1-15>
*       gigPortsSlowRateRatio - <1-16>
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperGlobalParamsSet
(
    IN    GT_U8                   devNum,
    IN    GT_U32                  xgPortsTokensRate,
    IN    GT_U32                  gigPortsTokensRate,
    IN    GT_U32                  gigPortsSlowRateRatio
);

/*******************************************************************************
* cpssDxChPortTxShaperGlobalParamsGet
*
* DESCRIPTION:
*       Get Global parameters for shaper.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*      devNum                   - device number
*
* OUTPUTS:
*       xgPortsTokensRatePtr     - (pointer to) tokens rate for XG ports' shapers
*       gigPortsTokensRatePtr    - (pointer to) tokens rate for Tri Speed ports' shapers
*       gigPortsSlowRateRatioPtr - (pointer to) slow rate ratio for Tri Speed ports.
*                                  Tokens Update rate for ports with slow rate divided
*                                  to the ratio.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For devices the following values are valid.
*       xgPortsTokensRate    - <1-15>
*       gigPortsTokensRate   - <1-15>
*       gigPortsSlowRateRatio - <1-16>
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperGlobalParamsGet
(
    IN    GT_U8          devNum,
    OUT   GT_U32         *xgPortsTokensRatePtr,
    OUT   GT_U32         *gigPortsTokensRatePtr,
    OUT   GT_U32         *gigPortsSlowRateRatioPtr
);

/*******************************************************************************
* cpssDxChPortTxQWrrProfileSet
*
* DESCRIPTION:
*       Set Weighted Round Robin profile on the specified port's
*       Traffic Class Queue.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue on this Port
*       wrrWeight  - proportion of bandwidth assigned to this queue
*                    relative to the other queues in this
*                    Arbitration Group - resolution is 1/255.
*       profileSet - the Tx Queue scheduler Profile Set in which the wrrWeight
*                    Parameter is associated.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         - if weight will be less then port's MTU (maximum transmit unit) there
*           is possibility for empty WRR loops for given TC queue, but to not
*           tight user and for future ASIC's where this problem will be solved
*           check of wrrWeight * 256 > MTU not implemented
*         - There is errata for Cheetah FEr#29. weigh cannot be = 255 for some
*           modes. The functions limits weight accordingly.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQWrrProfileSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   tcQueue,
    IN  GT_U8                                   wrrWeight,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet
);

/*******************************************************************************
* cpssDxChPortTxQWrrProfileGet
*
* DESCRIPTION:
*       Get Weighted Round Robin profile on the specified port's
*       Traffic Class Queue.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue on this Port
*       profileSet - the Tx Queue scheduler Profile Set in which the wrrWeight
*                    Parameter is associated.
* OUTPUTS:
*       wrrWeightPtr  - Pointer to proportion of bandwidth assigned to this queue
*                       relative to the other queues in this
*                       Arbitration Group - resolution is 1/255.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQWrrProfileGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   tcQueue,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    OUT  GT_U8                                  *wrrWeightPtr
);

/*******************************************************************************
* cpssDxChPortTxQArbGroupSet
*
* DESCRIPTION:
*       Set Traffic Class Queue scheduling arbitration group on
*       specificed profile of specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue
*       arbGroup   - scheduling arbitration group:
*                     1) Strict Priority
*                     2) WRR Group 1
*                     3) WRR Group 0
*       profileSet - the Tx Queue scheduler Profile Set in which the arbGroup
*                    parameter is associated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong profile number, device or arbGroup
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQArbGroupSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   tcQueue,
    IN  CPSS_PORT_TX_Q_ARB_GROUP_ENT            arbGroup,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet
);

/*******************************************************************************
* cpssDxChPortTxQArbGroupGet
*
* DESCRIPTION:
*       Get Traffic Class Queue scheduling arbitration group on
*       specificed profile of specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue 
*       profileSet - the Tx Queue scheduler Profile Set in which the arbGroup
*                    parameter is associated.
*
* OUTPUTS:
*       arbGroupPtr   - Pointer to scheduling arbitration group:
*                       1) Strict Priority
*                       2) WRR Group 1
*                       3) WRR Group 0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong profile number, device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQArbGroupGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   tcQueue,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    OUT CPSS_PORT_TX_Q_ARB_GROUP_ENT            *arbGroupPtr
);

/*******************************************************************************
*  coreTxPortBindPortToDpSet
*
* DESCRIPTION:
*       Bind a Physical Port to a specific Drop Profile Set.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum        - physical device number
*       portNum       - physical port number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChPortTxBindPortToDpSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT  profileSet
);

/*******************************************************************************
*  cpssDxChPortTxBindPortToDpGet
*
* DESCRIPTION:
*       Get Drop Profile Set according to a Physical Port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       profileSetPtr    - Pointer to the Profile Set in which the Traffic
*                          Class Drop Parameters is associated
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChPortTxBindPortToDpGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    OUT CPSS_PORT_TX_DROP_PROFILE_SET_ENT   *profileSetPtr
);


/*******************************************************************************
*  cpssDxChPortTxDescNumberGet
*
* DESCRIPTION:
*       Gets the current number of descriptors allocated per specified port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxDescNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_U16      *numberPtr
);

/*******************************************************************************
*  cpssDxChPortTxBufNumberGet
*
* DESCRIPTION:
*       Gets the current number of buffers allocated per specified port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       numPtr   - number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBufNumberGet
(
    IN   GT_U8       devNum,
    IN   GT_U8       portNum,
    OUT  GT_U16      *numPtr
);

/*******************************************************************************
*  cpssDxChPortTx4TcDescNumberGet
*
* DESCRIPTION:
*       Gets the current number of descriptors allocated on specified port
*       for specified Traffic Class queues.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       trafClass - trafiic class
*
* OUTPUTS:
*       numberPtr  - (pointer to) the number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcDescNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       trafClass,
    OUT GT_U16      *numberPtr
);

/*******************************************************************************
*  cpssDxChPortTx4TcBufNumberGet
*
* DESCRIPTION:
*       Gets the current number of buffers allocated on specified port
*       for specified Traffic Class queues.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       trafClass - trafiic class
*
* OUTPUTS:
*       numPtr    - (pointer to) the number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcBufNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       trafClass,
    OUT GT_U16      *numPtr
);

/*******************************************************************************
* cpssDxChPortTxToCpuShaperModeSet
*
* DESCRIPTION:
*       Set Shaper mode packet or byte based for CPU port shapers
*       Shapers are configured by cpssDxChPortTxShaperProfileSet or
*       cpssDxChPortTxQShaperProfileSet
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       mode    - shaper mode: byte count or packet number based ackets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortTxToCpuShaperModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   mode
);

/*******************************************************************************
*  cpssDxChPortTxSharingGlobalResourceEnableSet
*
* DESCRIPTION:
*       Enable/Disable sharing of resources for enqueuing of packets.
*       The shared resources configuration is set by
*       cpssDxChPortTxSharedGlobalResourceLimitsSet
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE   - enable TX queue resourses sharing
*                 GT_FALSE - disable TX queue resources sharing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharingGlobalResourceEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
*  cpssDxChPortTxSharingGlobalResourceEnableGet
*
* DESCRIPTION:
*       Get enable/disable sharing of resources for enqueuing of packets.
*       The shared resources configuration is set by
*       cpssDxChPortTxSharedGlobalResourceLimitsSet
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) 
*                    GT_TRUE   - enable TX queue resourses sharing
*                    GT_FALSE - disable TX queue resources sharing
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharingGlobalResourceEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL    *enablePtr
);

/*******************************************************************************
*  cpssDxChPortTxSharedGlobalResourceLimitsSet
*
* DESCRIPTION:
*       Configuration of shared resources for enqueuing of packets.
*       The using of shared resources is set
*       by cpssDxChPortTxSharingGlobalResourceEnableSet
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum             - physical device number
*       sharedBufLimit     - The number of buffers in all Transmit queues that
*                            can be shared between eligible packets.
*                            For DxCh2: range 0..4095
*                            For DxCh3 and above: range 0..16380
*       sharedDescLimit    - The number of descriptors that can be shared
*                            between all eligible packets.
*                            For DxCh2: range 0..4095
*                            For DxCh3 and above: range 0..16380
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In DxCh3 and above sharedBufLimit and sharedDescLimit are rounded up
*       to the nearest multiple of 4 since the corresponding fields in the
*       Transmit Queue Resource Sharing register are in 4 buffers\descriptors
*       resolution.
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedGlobalResourceLimitsSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  sharedBufLimit,
    IN  GT_U32  sharedDescLimit
);

/*******************************************************************************
*  cpssDxChPortTxSharedGlobalResourceLimitsGet
*
* DESCRIPTION:
*       Get the configuration of shared resources for enqueuing of packets.
*       The using of shared resources is set
*       by cpssDxChPortTxSharingGlobalResourceEnableSet
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum             - physical device number
*
* OUTPUTS:
*       sharedBufLimitPtr  - (pointer to) The number of buffers in all
*                            Transmit queues that
*                            can be shared between eligible packets.
*       sharedDescLimitPtr - (pointer to) The number of descriptors that 
*                            can be shared between all eligible packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedGlobalResourceLimitsGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *sharedBufLimitPtr,
    OUT GT_U32  *sharedDescLimitPtr
);

/*******************************************************************************
*  cpssDxChPortTxSharedPolicySet
*
* DESCRIPTION:
*       Sets shared pool allocation policy for enqueuing of packets.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number.
*       policy     - shared pool allocation policy.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or wrong policy
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedPolicySet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT  policy
);

/*******************************************************************************
*  cpssDxChPortTxSharedPolicyGet
*
* DESCRIPTION:
*       Gets shared pool allocation policy for enqueuing of packets.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       policyPtr  - (pointer to) shared pool allocation policy.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedPolicyGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT  *policyPtr
);

/*******************************************************************************
* cpssDxChPortTxWatchdogGet
*
* DESCRIPTION:
*       Get Tx Port Watchdog status and timeout
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*
* OUTPUTS:
*       enablePtr     - (pointer to) the enable / disable state
*       timeoutPtr    - (pointer to) the watchdog timeout
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - enablePtr or timeoutPtr is a null pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxWatchdogGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr,
    OUT GT_U16      *timeoutPtr
);

/*******************************************************************************
* cpssDxChPortTxWatchdogEnableSet
*
* DESCRIPTION:
*       Enable/Disable Watchdog on specified port of specified port of
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       enable  - GT_TRUE, enable Watchdog
*                 GT_FALSE, disable Watchdog
*       timeout - Watchdog timeout in milliseconds (1..4K ms)
*                 If disabling the Watchdog, this parameter is ignored
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxWatchdogEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable,
    IN  GT_U16    timeout
);

/*******************************************************************************
* cpssDxChPortTxTailDropUcEnableSet
*
* DESCRIPTION:
*       Enable/Disable tail-dropping for all packets based on the profile limits.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   -  device number.
*       enable   -  GT_TRUE  - Tail Drop is enabled.
*                   GT_FALSE - The Tail Drop limits for all packets are
*                              ignored and packet is dropped only
*                              when the Tx Queue has reached its global
*                              descriptors limit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropUcEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChPortTxTailDropUcEnableGet
*
* DESCRIPTION:
*       Get enable/disable tail-dropping for all packets based on the profile limits.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      -  device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to tail drop status:
*                          GT_TRUE  - Tail Drop is enabled.
*                          GT_FALSE - The Tail Drop limits for all packets are
*                                     ignored and packet is dropped only
*                                     when the Tx Queue has reached its global
*                                     descriptors limit.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropUcEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxBufferTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Tail Drop according to the number of buffers in the
*       queues.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - device number.
*       enable   -  GT_TRUE  -  Enables Tail Drop according to the number of
*                               buffers in the queues. Tail drop use both
*                               decsriptiors and buffers limits.
*                   GT_FALSE - Tail drop use only descriptor limits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBufferTailDropEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChPortTxBufferTailDropEnableGet
*
* DESCRIPTION:
*       Get enable/disable Tail Drop status according to the number of buffers
*       in the queues.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to Tail Drop status:
*                       GT_TRUE  - Enables Tail Drop according to the number of
*                                  buffers in the queues. Tail drop use both
*                                  decsriptiors and buffers limits.
*                       GT_FALSE - Tail drop use only descriptor limits.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBufferTailDropEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
*  cpssDxChPortTxBuffersSharingMaxLimitSet
*
* DESCRIPTION:
*       Sets the maximal number of shared buffers in a Tail Drop system.
*       When the total number of buffers exceeds this threshold, all of the
*       shared buffers are currently used and packets are queued according to
*       their target queue guaranteed limits.
*
* APPLICABLE DEVICES:  DxCh2 and above.
*
* INPUTS:
*       devNum  - physical device number
*       limit   -  maximal number of shared buffers in a Tail Drop system.
*                  - DxCh2 - Range: 0..0xFFF.
*                          To disable Buffers Sharing for Transmit queues
*                          set this field to 0xFFF.
*                  - DxCh3 and above - Range: 0..0x3FFF.
*                          To disable Buffers Sharing for Transmit queues
*                          set this field to 0.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBuffersSharingMaxLimitSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      limit
);

/*******************************************************************************
*  cpssDxChPortTxBuffersSharingMaxLimitGet
*
* DESCRIPTION:
*       Gets the maximal number of shared buffers in a Tail Drop system.
*       When the total number of buffers exceeds this threshold, all of the
*       shared buffers are currently used and packets are queued according to
*       their target queue guaranteed limits.
*
* APPLICABLE DEVICES:   DxCh2 and above.
*
* INPUTS:
*       devNum  - physical device number
*
*
* OUTPUTS:
*       limitPtr   -  pointer to maximal number of shared buffers
*                     in a Tail Drop system
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBuffersSharingMaxLimitGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *limitPtr
);

/*******************************************************************************
*  cpssDxChPortTxDp1SharedEnableSet
*
* DESCRIPTION:
*       Enable/Disable packets with DP1 (Red) to use the shared
*       descriptors / buffers pool.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE   - Allow DP1 (Red) in shared pool.
*                 GT_FALSE  - Disallow DP1 to be shared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxDp1SharedEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
*  cpssDxChPortTxDp1SharedEnableGet
*
* DESCRIPTION:
*       Gets current status of  shared  descriptors / buffer pool usage
*       for packets with DP1 (Yellow).
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr  - pointer to current status of
*                    shared  descroptors / Buffers pool usage:
*                  - GT_TRUE   - Allow DP1 (Yellow) in shared pool.
*                  - GT_FALSE  - Allow DP0 (Green) only in shared pool.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxDp1SharedEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxTcSharedProfileEnableSet
*
* DESCRIPTION:
*       Enable/Disable usage of the shared  descriptors / buffer pool for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum   - physical device number
*       pfSet    - the Profile Set in which the Traffic
*                  Class Drop Parameters is associated
*       tc       - the Traffic Class, range 0..7.
*       enable   - GT_TRUE -  Enable usage of the shared descriptors/buffer pool
*                  GT_FALSE -  Disable usage of the shared descriptors/buffer pool
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTcSharedProfileEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT       pfSet,
    IN  GT_U8                                   tc,
    IN  GT_BOOL                                 enable
);

/*******************************************************************************
* cpssDxChPortTxTcSharedProfileEnableGet
*
* DESCRIPTION:
*       Gets usage of the shared  descriptors / buffer pool status for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum   - physical device number
*       pfSet    - the Profile Set in which the Traffic
*                  Class Drop Parameters is associated
*       tc       - the Traffic Class, range 0..7.
*
* OUTPUTS:
*       enablePtr   -  pointer to usage of
*                      the shared  descriptors / buffer pool status
*                  GT_TRUE  -  Enable usage of the shared descriptors/buffer pool
*                  GT_FALSE -  Disable usage of the shared descriptors/buffer pool
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTcSharedProfileEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT       pfSet,
    IN  GT_U8                                   tc,
    OUT GT_BOOL                                 *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxShaperTokenBucketMtuSet
*
* DESCRIPTION:
*       Set Token bucket maximum transmission unit (MTU).
*       The token bucket MTU defines the minimum number of tokens required to
*       permit a packet to be transmitted (i.e., conforming).
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - physical device number
*       mtu      - MTU for egress rate shaper
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, mtu
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperTokenBucketMtuSet
(
    IN GT_U8                                  devNum,
    IN CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT mtu
);

/*******************************************************************************
* cpssDxChPortTxShaperTokenBucketMtuGet
*
*       Get Token bucket maximum transmission unit (MTU).
*       The token bucket MTU defines the minimum number of tokens required to
*       permit a packet to be transmitted (i.e., conforming).
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       mtuPtr   -  pointer to MTU for egress rate shaper
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperTokenBucketMtuGet
(
    IN  GT_U8                                  devNum,
    OUT CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT *mtuPtr
);

/*******************************************************************************
* cpssDxChPortTxRandomTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Random Tail drop Threshold, to overcome synchronization.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum   - device number.
*       enable   -  GT_TRUE  -  Enable Random Tail drop Threshold.
*                   GT_FALSE -  Disable Random Tail drop Threshold.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRandomTailDropEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChPortTxRandomTailDropEnableGet
*
* DESCRIPTION:
*       Get Random Tail drop Threshold status.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to Random Tail drop Threshold status:
*                       GT_TRUE  - Random Tail drop Threshold enabled.
*                       GT_FALSE - Random Tail drop Threshold disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRandomTailDropEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChTxPortAllShapersDisable
*
* DESCRIPTION:
*       Disables all ports and queues shapers for specified device in minimum delay.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChTxPortAllShapersDisable
(
    IN  GT_U8    devNum
);

/*******************************************************************************
* cpssDxChTxPortShapersDisable
*
* DESCRIPTION:
*       Disable Shaping on Port and all it's queues.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number.
*       port    - physical port number (CPU port supported as well as regular ports)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChTxPortShapersDisable
(
    IN  GT_U8    devNum,
    IN  GT_U8    port
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortTxh */

