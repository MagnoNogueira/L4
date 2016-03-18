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
* cpssDxChCscd.h
*
* DESCRIPTION:
*       CPSS DXCH Cascading API.
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/

#ifndef __cpssDxChCscdh
#define __cpssDxChCscdh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cscd/cpssGenCscd.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>
/*
 * typedef: enum CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT
 *
 * Description: egress QoS remap mode type
 *
 *
 * Enumerations:
 *      CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E   - disable QOS remapping
 *      CPSS_DXCH_CSCD_QOS_REMAP_CNTRL_ONLY_E - enable QOS remap for control traffic
 *      CPSS_DXCH_CSCD_QOS_REMAP_DATA_ONLY_E  - enable QOS remap for data traffic
 *      CPSS_DXCH_CSCD_QOS_REMAP_ALL_E        - enable QOS remap for both control
 *                                              and data traffic
 *
 */
typedef enum
{
    CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E,
    CPSS_DXCH_CSCD_QOS_REMAP_CNTRL_ONLY_E,
    CPSS_DXCH_CSCD_QOS_REMAP_DATA_ONLY_E,
    CPSS_DXCH_CSCD_QOS_REMAP_ALL_E
}CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT;


/*
 * typedef: enum CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT
 *
 * Description: HyperG.Stack Port Speed
 *
 *
 * Enumerations:
 *  CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E    - 10 Gbps - each of the XAUI
 *                                     four SERDES lanes operates at 3.125 Gbps.
 *  CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E - 12 Gbps - each of the XAUI
 *                                     four SERDES lanes operates at 3.75 Gbps.
 *
 */
typedef enum
{
    CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E,
    CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E
} CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT;

/*
 * typedef: CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT
 *
 * Description: type of the load balancing trunk hash for packets forwarded
 *              via trunk cascade link
 *              Enabled to set the load balancing trunk hash for packets
 *              forwarded via an trunk uplink to be based on the packet’s source
 *              port, and not on the packets data.
 *
 * Enumerations:
 *       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
 *              Load balancing trunk hash is based on the ingress pipe
 *              hash mode as configured by function
 *              cpssDxChTrunkHashModeSet(...)
 *       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load balancing trunk
 *              hash for this cascade trunk interface is based on the packet’s
 *              source port, regardless of the ingress pipe hash mode.
 *       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E - Load balancing trunk
 *              hash for this cascade trunk interface is based on the packet’s
 *              destination port, regardless of the ingress pipe hash mode.
 *              Relevant only for Lion and above.   
*/

typedef enum
{
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E,
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E,
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E
} CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT;

/*
 * typedef: CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT
 *
 * Description: TX/RX HyperG.link CRC Mode
 *
 * Enumerations:
 *  CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E   - one byte CRC mode
 *  CPSS_DXCH_CSCD_PORT_CRC_FOUR_BYTES_E - four bytes CRC mode
*/

typedef enum
{
    CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E,
    CPSS_DXCH_CSCD_PORT_CRC_FOUR_BYTES_E
} CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_CSCD_QOS_TC_REMAP_STC
 *
 * Description: Structure for remapping TC to new TCs, for each DSA tag
 *              and source port type.
 *
 * Fields:
 *    forwardLocalTc    - Traffic Class for DSA tag FORWARD and for
 *                        local to stack port
 *    forwardStackTc    - Traffic Class for DSA tag FORWARD and for
 *                        stack to stack port
 *    toAnalyzerLocalTc - Traffic Class for DSA tag TO_ANALYZER and
 *                        for local to stack port
 *    toAnalyzerStackTc - Traffic Class for DSA tag TO_ANALYZER and
 *                        for stack to stack port
 *    toCpuLocalTc      - Traffic Class for DSA tag TO_CPU and for
 *                        local to stack port
 *    toCpuStackTc      - Traffic Class for DSA tag TO_CPU and for
 *                        stack to stack port
 *    fromCpuLocalTc    - Traffic Class for DSA tag FROM_CPU and for
 *                        local to stack port
 *    fromCpuStackTc    - Traffic Class for DSA tag FROM_CPU and for
 *                        stack to stack port
 */
typedef struct
{
    GT_U32      forwardLocalTc;
    GT_U32      forwardStackTc;
    GT_U32      toAnalyzerLocalTc;
    GT_U32      toAnalyzerStackTc;
    GT_U32      toCpuLocalTc;
    GT_U32      toCpuStackTc;
    GT_U32      fromCpuLocalTc;
    GT_U32      fromCpuStackTc;
}CPSS_DXCH_CSCD_QOS_TC_REMAP_STC;

/*
 * typedef: enum CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT
 *
 * Description: Device Map lookup mode.
 *              The mode defines index calculation that is used to 
 *              access Device Map table.
 *
 * Enumerations:
 *      CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E - Target Device 
 *                                       (bits [4:0]) is used as index.
 *      CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E - 
 *            Both Target Device and Target Port are used to get index.
 *            Index bits representation is Target Device[4:0], Target Port[5:0].
 */
typedef enum
{
     CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E,
     CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E
}CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT;

/*******************************************************************************
* cpssDxChCscdPortTypeSet
*
* DESCRIPTION:
*            Configure a PP port to be a cascade port. Application is responsible
*            for setting the default values of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - The port to be configured as cascade
*       portType - cascade  type regular/extended DSA tag port or network port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For Lion:
*       This function also configures insertion of DSA tag for PFC frames.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortTypeSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        portNum,
    IN CPSS_CSCD_PORT_TYPE_ENT      portType
);

/*******************************************************************************
* cpssDxChCscdPortTypeGet
*
* DESCRIPTION:
*            Retreive a PP port cascade port configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - port number or CPU port
*
* OUTPUTS:
*       portTypePtr - (pointer to) cascade type regular/extended DSA tag port
*                     or network port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortTypeGet
(
    IN GT_U8                        devNum,
    IN GT_U8                        portNum,
    OUT CPSS_CSCD_PORT_TYPE_ENT     *portTypePtr
);

/*******************************************************************************
* cpssDxChCscdDevMapTableSet
*
* DESCRIPTION:
*            Set the cascade map table . the map table define the local port or
*            trunk that packets destined to a destination device 
*            should be transmitted to.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       targetDevNum   - the device to be reached via cascade (0..31)
*       targetPortNum  - target port number 
*                        Relevant only for Lion and
*                        CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E mode, 
*                        otherwise ignored. 
*       cascadeLinkPtr - (pointer to)A structure holding the cascade link type
*                      (port (0..63) or trunk(0..127)) and the link number
*                      leading to the target device.
*       srcPortTrunkHashEn - Relevant when (cascadeLinkPtr->linkType ==
*                       CPSS_CSCD_LINK_TYPE_TRUNK_E)
*                       Enabled to set the load balancing trunk hash for packets
*                       forwarded via an trunk uplink to be based on the
*                       packet’s source port, and not on the packets data.
*                       Indicates the type of uplink.
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
*                       Load balancing trunk hash is based on the ingress pipe
*                       hash mode as configured by function
*                       cpssDxChTrunkHashGeneralModeSet(...)
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load
*                       balancing trunk hash for this cascade trunk interface is
*                       based on the packet’s source port, regardless of the
*                       ingress pipe hash mode
*                       NOTE : this parameter is relevant only to DXCH2
*                       and above devices
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on wrong device or target device or target port 
*                          or bad trunk hash mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapTableSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        targetDevNum,
    IN GT_U8                        targetPortNum,   
    IN CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    IN CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT srcPortTrunkHashEn
);

/*******************************************************************************
* cpssDxChCscdDevMapTableGet
*
* DESCRIPTION:
*            Get the cascade map table . the map table define the local port or
*            trunk that packets destined to a destination device 
*            should be transmitted to.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       targetDevNum   - the device to be reached via cascade (0..31)
*       targetPortNum  - target port number 
*                        Used only for Lion and
*                        CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E mode, 
*                        otherwise ignored. 
*
* OUTPUTS:
*       cascadeLinkPtr - (pointer to)A structure holding the cascade link type
*                      (port (0..63) or trunk(0..127)) and the link number
*                      leading to the target device.
*       srcPortTrunkHashEnPtr - Relevant when (cascadeLinkPtr->linkType ==
*                       CPSS_CSCD_LINK_TYPE_TRUNK_E)
*                       (pointer to) Enabled to set the load balancing trunk
*                       hash for packets forwarded via an trunk uplink to be
*                       based on the packet’s source port, and not on the
*                       packets data.
*                       Indicates the type of uplink.
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
*                       Load balancing trunk hash is based on the ingress pipe
*                       hash mode as configured by function
*                       cpssDxChTrunkHashGeneralModeSet(...)
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load
*                       balancing trunk hash for this cascade trunk interface is
*                       based on the packet’s source port, regardless of the
*                       ingress pipe hash mode
*                       NOTE : this parameter is relevant only to DXCH2 and
*                       above devices
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on wrong device or target device or target port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapTableGet
(
    IN GT_U8                         devNum,
    IN GT_U8                         targetDevNum,
    IN GT_U8                         targetPortNum,   
    OUT CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    OUT CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT *srcPortTrunkHashEnPtr
);

/*******************************************************************************
* cpssDxChCscdRemapQosModeSet
*
*
* DESCRIPTION:
*       Enables/disables remapping of Tc and Dp for Data and Control Traffic
*       on a port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - physical port number
*       remapType    - traffic type to remap
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdRemapQosModeSet
(
   IN GT_U8                             devNum,
   IN GT_U8                             portNum,
   IN CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT remapType
);

/*******************************************************************************
* cpssDxChCscdCtrlQosSet
*
* DESCRIPTION:
*       Set control packets TC and DP if Control Remap QoS enabled on a port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*       ctrlTc      - The TC assigned to control packets forwarded to
*                     cascading ports (0..7)
*       ctrlDp      - The DP assigned to CPU-to-network control traffic or
*                     network-to-CPU traffic.
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*       cpuToCpuDp  - The DP assigned to CPU-to-CPU control traffic
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_VALUE     - on wrong ctrlDp or cpuToCpuDp level value or tc value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCtrlQosSet
(
   IN GT_U8             devNum,
   IN GT_U8             ctrlTc,
   IN CPSS_DP_LEVEL_ENT ctrlDp,
   IN CPSS_DP_LEVEL_ENT cpuToCpuDp
);

/*******************************************************************************
* cpssDxChCscdRemapDataQosTblSet
*
* DESCRIPTION:
*       Set table to remap Data packets QoS parameters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*       tc          - original packet TC (0..7)
*       dp          - original packet DP
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*       remapTc     - TC assigned to data packets with DP and TC (0..7)
*       remapDp     - DP assigned to data packets with DP and TC
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_VALUE             - on wrong DP or dp level value or tc value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdRemapDataQosTblSet
(
   IN GT_U8              devNum,
   IN GT_U8              tc,
   IN CPSS_DP_LEVEL_ENT  dp,
   IN GT_U8              remapTc,
   IN CPSS_DP_LEVEL_ENT  remapDp
);

/*******************************************************************************
* cpssDxChCscdDsaSrcDevFilterSet
*
* DESCRIPTION:
*       Enable/Disable filtering the ingress DSA tagged packets in which
*       source id equals to local device number.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - physical device number
*       enableOwnDevFltr - enable/disable ingress DSA loop filter
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssDxChCscdDsaSrcDevFilterSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enableOwnDevFltr
);

/*******************************************************************************
* cpssDxChCscdHyperGInternalPortModeSet
*
* DESCRIPTION:
*       Change HyperG.stack internal port speed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - cascade HyperG port to be configured
*       mode         - HyperG.Stack Port Speed 10Gbps or 12Gbps
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - the request is not supported
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdHyperGInternalPortModeSet
(
    IN  GT_U8                                devNum,
    IN  GT_U8                                portNum,
    IN  CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT mode
);

/*******************************************************************************
* cpssDxChCscdHyperGPortCrcModeSet
*
* DESCRIPTION:
*       Set CRC mode to be standard 4 bytes or proprietary one byte CRC mode.
*
* APPLICABLE DEVICES:
*        DxCh2; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3.
*
* INPUTS:
*       devNum           - physical device number
*       portNum          - port number
*       portDirection    - TX/RX cascade port direction
*       crcMode          - TX/RX HyperG.link CRC Mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdHyperGPortCrcModeSet
(
    IN GT_U8                             devNum,
    IN GT_U8                             portNum,
    IN CPSS_PORT_DIRECTION_ENT           portDirection,
    IN CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT  crcMode
);

/*******************************************************************************
* cpssDxChCscdFastFailoverFastStackRecoveryEnableSet
*
* DESCRIPTION:
*       Enable/Disable fast stack recovery.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       enable           - GT_TRUE: enable fast stack recovery
*                          GT_FALSE: disable fast stack recovery
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverFastStackRecoveryEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChCscdFastFailoverFastStackRecoveryEnableGet
*
* DESCRIPTION:
*       Get the status of fast stack recovery (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       enablePtr        - (pointer to) fast stack recovery status
*                          GT_TRUE: fast stack recovery enabled
*                          GT_FALSE: fast stack recovery disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverFastStackRecoveryEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
);

/*******************************************************************************
* cpssDxChCscdFastFailoverSecondaryTargetPortMapSet
*
* DESCRIPTION:
*       Set secondary target port map.
*       If the device is the device where the ring break occured, the
*       packet is looped on the ingress port and is egressed according to
*       Secondary Target Port Map. Also "packetIsLooped" bit is
*       set in DSA tag.
*       If the device receives a packet with "packetIsLooped" bit is set
*       in DSA tag, the packet is forwarded according to Secondary Target Port
*       Map.
*       Device MAP table (cpssDxChCscdDevMapTableSet) is not used in the case.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - ingress port number
*       secondaryTargetPort - secondary target port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - when secondaryTargetPort is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverSecondaryTargetPortMapSet
(
    IN GT_U8        devNum,
    IN GT_U8        portNum,
    IN GT_U8        secondaryTargetPort
);

/*******************************************************************************
* cpssDxChCscdFastFailoverSecondaryTargetPortMapGet
*
* DESCRIPTION:
*       Get Secondary Target Port Map for given device.
*       If the device is the device where the ring break occured, the
*       packet is looped on the ingress port and is egressed according to
*       Secondary Target Port Map. Also "packetIsLooped" bit is
*       set in DSA tag.
*       If the device receives a packet with "packetIsLooped" bit is set
*       in DSA tag, the packet is forwarded according to Secondary Target Port
*       Map.
*       Device MAP table (cpssDxChCscdDevMapTableSet) is not used in the case.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - ingress port number
*
* OUTPUTS:
*       secondaryTargetPortPtr - (pointer to) secondary target port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverSecondaryTargetPortMapGet
(
    IN  GT_U8        devNum,
    IN  GT_U8        portNum,
    OUT GT_U8        *secondaryTargetPortPtr
);

/*******************************************************************************
* cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet
*
* DESCRIPTION:
*       Enable/Disable fast failover loopback termination
*       for single-destination packets.
*       There are two configurable options for forwarding single-destination
*       packets that are looped-back across the ring:
*       - Termination Disabled.
*         Unconditionally forward the looped-back packet to the configured
*         backup ring port (for the given ingress ring port) on all the ring
*         devices until it reaches the far-end device where it is again
*         internally looped-back on the ring port and then forward it normally.
*       - Termination Enabled.
*         The looped-back packet passes through the
*         ring until it reaches the target device where it is egressed on its
*         target port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       enable           - GT_TRUE: enable terminating local loopback
*                                   for fast stack recovery
*                          GT_FALSE: disable terminating local loopback
*                                   for fast stack recovery
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
);


/*******************************************************************************
* cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet
*
* DESCRIPTION:
*       Get the status of fast failover loopback termination
*       for single-destination packets (Enabled/Disabled).
*       There are two configurable options for forwarding single-destination
*       packets that are looped-back across the ring:
*       - Termination Disabled.
*         Unconditionally forward the looped-back packet to the configured
*         backup ring port (for the given ingress ring port) on all the ring
*         devices until it reaches the far-end device where it is again
*         internally looped-back on the ring port and then forward it normally.
*       - Termination Enabled.
*         The looped-back packet passes through the
*         ring until it reaches the target device where it is egressed on its
*         target port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       enablePtr        - GT_TRUE: terminating local loopback
*                                   for fast stack recovery enabled
*                          GT_FALSE: terminating local loopback
*                                   for fast stack recovery disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
);

/*******************************************************************************
* cpssDxChCscdFastFailoverPortIsLoopedSet
*
* DESCRIPTION:
*       Enable/disable Fast Failover on a failed port.
*       When port is looped and get packet with DSA tag <Packet is Looped> = 0,
*       then device do next:
*        - set DSA tag <Packet is Looped> = 1
*        - bypass ingress and egress processing
*        - send packet through egress port that defined in secondary target
*          port map (cpssDxChCscdFastFailoverSecondaryTargetPortMapSet).
*       When port is looped and get packet with DSA tag <Packet is Looped> = 1,
*       then device do next:
*        - set DSA tag <Packet is Looped> = 0
*        - Apply usual ingress and egress processing
*       When port is not looped and get packet then device do next:
*        - Apply usual ingress and egress processing
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number (including CPU)
*       isLooped         -  GT_FALSE - Port is not looped.
*                           GT_TRUE - Port is looped.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverPortIsLoopedSet
(
  IN  GT_U8      devNum,
  IN  GT_U8      portNum,
  IN GT_BOOL     isLooped
);

/*******************************************************************************
* cpssDxChCscdFastFailoverPortIsLoopedGet
*
* DESCRIPTION:
*       Get status (Enable/Disable) of Fast Failover on the failed port.
*       When port is looped and get packet with DSA tag <Packet is Looped> = 0,
*       then device do next:
*        - set DSA tag <Packet is Looped> = 1
*        - bypass ingress and egress processing
*        - send packet through egress port that defined in secondary target
*          port map (cpssDxChCscdFastFailoverSecondaryTargetPortMapSet).
*       When port is looped and get packet with DSA tag <Packet is Looped> = 1,
*       then device do next:
*        - set DSA tag <Packet is Looped> = 0
*        - Apply usual ingress and egress processing
*       When port is not looped and get packet then device do next:
*        - Apply usual ingress and egress processing
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number (including CPU)
*
* OUTPUTS:
*       isLoopedPtr       - (pointer to) Is Looped
*                           GT_FALSE - Port is not looped.
*                           GT_TRUE - Port is looped.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverPortIsLoopedGet
(
  IN GT_U8      devNum,
  IN GT_U8      portNum,
  OUT GT_BOOL    *isLoopedPtr
);

/*******************************************************************************
* cpssDxChCscdQosPortTcRemapEnableSet
*
* DESCRIPTION:
*       Enable/Disable Traffic Class Remapping on cascading port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - cascading port number (CPU port or 0-27)
*       enable           - GT_TRUE: enable Traffic Class remapping
*                          GT_FALSE: disable Traffic Class remapping
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosPortTcRemapEnableSet
(
    IN GT_U8        devNum,
    IN GT_U8        portNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChCscdQosPortTcRemapEnableGet
*
* DESCRIPTION:
*       Get the status of Traffic Class Remapping on cascading port
*       (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - cascading port number (CPU port or 0-27)
*
* OUTPUTS:
*       enablePtr        - GT_TRUE: Traffic Class remapping enabled
*                          GT_FALSE: Traffic Class remapping disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosPortTcRemapEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChCscdQosTcRemapTableSet
*
* DESCRIPTION:
*       Remap Traffic Class on cascading port to new Traffic Classes,
*       for each DSA tag type and for source port type (local or cascading).
*       If the source port is enabled for Traffic Class remapping, then traffic
*       will egress with remapped Traffic Class.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       tc              - Traffic Class of the packet on the source port (0..7)
*       tcMappingsPtr   - (pointer to )remapped traffic Classes
*                         for ingress Traffic Class
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosTcRemapTableSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  tc,
    IN  CPSS_DXCH_CSCD_QOS_TC_REMAP_STC         *tcMappingsPtr
);

/*******************************************************************************
* cpssDxChCscdQosTcRemapTableGet
*
* DESCRIPTION:
*       Get the remapped Traffic Classes for given Traffic Class.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       tc              - Traffic Class of the packet on the source port (0..7)
*
* OUTPUTS:
*       tcMappingsPtr   - (pointer to )remapped Traffic Classes
*                         for ingress Traffic Class
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosTcRemapTableGet
(
    IN   GT_U8                                  devNum,
    IN   GT_U32                                 tc,
    OUT  CPSS_DXCH_CSCD_QOS_TC_REMAP_STC        *tcMappingsPtr
);

/*******************************************************************************
* cpssDxChCscdPortBridgeBypassEnableSet
*
* DESCRIPTION:
*       The function enables/disables bypass of the bridge engine per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  -  PP's device number.
*       portNum -  physical port number or CPU port
*       enable  -  GT_TRUE  - Enable bypass of the bridge engine.
*                  GT_FALSE - Disable bypass of the bridge engine.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If the DSA tag is not extended Forward, the bridging decision
*       is performed regardless of the setting.
*       When bypass enabled the Bridge engine still learn MAC source addresses,
*       but will not modify the packet command, attributes (or forwarding 
*       decision).
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortBridgeBypassEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChCscdPortBridgeBypassEnableGet
*
* DESCRIPTION:
*       The function gets bypass of the bridge engine per port
*       configuration status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - PP's device number.
*       portNum - physical port number or CPU port
*
* OUTPUTS:
*       enablePtr  -  pointer to bypass of the bridge engine per port
*                     configuration status.
*                     GT_TRUE  - Enable bypass of the bridge engine.
*                     GT_FALSE - Disable bypass of the bridge engine.
*
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortBridgeBypassEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChCscdOrigSrcPortFilterEnableSet
*
* DESCRIPTION:
*       Enable/Disable filtering the multi-destination packet that was received 
*       by the local device, sent to another device, and sent back to this 
*       device, from being sent back to the network port at which it was 
*       initially received.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE - filter and drop the packet 
*                - GT_FALSE - don't filter the packet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/ 
GT_STATUS cpssDxChCscdOrigSrcPortFilterEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChCscdOrigSrcPortFilterEnableGet
*
* DESCRIPTION:
*       Get the status of filtering the multi-destination packet that was 
*       received  by the local device, sent to another device, and sent back to  
*       this device, from being sent back to the network port at which it was 
*       initially received.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - filter and drop the packet 
*                   - GT_FALSE - don't filter the packet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/ 
GT_STATUS cpssDxChCscdOrigSrcPortFilterEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChCscdDevMapLookupModeSet
*
* DESCRIPTION:
*        Set lookup mode for accessing the Device Map table. 
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number
*       mode         - device Map lookup mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device or mode 
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapLookupModeSet
(
    IN GT_U8                               devNum,
    IN CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT   mode
);

/*******************************************************************************
* cpssDxChCscdDevMapLookupModeGet
*
* DESCRIPTION:
*       Get lookup mode for accessing the Device Map table. 
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       modePtr      - pointer to device Map lookup mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device 
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_BAD_STATE     - wrong hardware value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapLookupModeGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT   *modePtr
);

/*******************************************************************************
* cpssDxChCscdPortLocalDevMapLookupEnableSet 
*
* DESCRIPTION:
*       Enable / Disable the local target port for device map lookup 
*       for local device.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number or CPU port
*       portDirection   - port's direction:
*                         CPSS_DIRECTION_INGRESS_E - source port 
*                         CPSS_DIRECTION_EGRESS_E  -  target port
*                         CPSS_DIRECTION_BOTH_E    - both source and target ports
*       enable       - GT_TRUE  - the port is enabled for device map lookup
*                                 for local device. 
*                    - GT_FALSE - No access to Device map table for
*                                 local devices.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device or port number or portDirection
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To enable access to the Device Map Table for the local target devices
*       -  Enable the local source port for device map lookup     
*       -  Enable the local target port for device map lookup
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortLocalDevMapLookupEnableSet
(
    IN GT_U8                devNum,
    IN GT_U8                portNum,
    IN CPSS_DIRECTION_ENT   portDirection,   
    IN GT_BOOL              enable
);

/*******************************************************************************
* cpssDxChCscdPortLocalDevMapLookupEnableGet
*
* DESCRIPTION:
*       Get status of enabling / disabling the local target port 
*       for device map lookup for local device.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number or CPU port
*       portDirection   - port's direction:
*                         CPSS_DIRECTION_INGRESS_E - source port 
*                         CPSS_DIRECTION_EGRESS_E  -  target port
*                         CPSS_DIRECTION_BOTH_E    - both source and target ports
*
* OUTPUTS:
*       enablePtr    - pointer to status of enabling / disabling the local 
*                      target port for device map lookup for local device.
*                    - GT_TRUE  - target port is enabled for device map lookup
*                                 for local device. 
*                    - GT_FALSE - No access to Device map table for
*                                 local devices.
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device or port number or portDirection
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortLocalDevMapLookupEnableGet
(
    IN  GT_U8               devNum,
    IN  GT_U8               portNum,
    IN CPSS_DIRECTION_ENT   portDirection,   
    OUT GT_BOOL             *enablePtr
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChCscdh */

